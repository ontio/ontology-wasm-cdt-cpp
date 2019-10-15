package main

import (
	"fmt"
	sdk "github.com/ontio/ontology-go-sdk"

	"encoding/binary"
	"github.com/ontio/ontology-go-sdk/utils"
	"github.com/ontio/ontology/common"
	"io/ioutil"
	"time"
)

func main() {
	fmt.Println("==========================start============================")

	//initialize ontsdk
	ontSdk := sdk.NewOntologySdk()
	//suppose you already start up a local wasm ontology node
	ontSdk.NewRpcClient().SetAddress("http://127.0.0.1:20336")
	wallet, err := ontSdk.OpenWallet("./wallet.dat")
	if err != nil {
		fmt.Printf("wallet open failed\n")
		return
	}

	//we get the first account of the wallet by your password
	signer, err := wallet.GetDefaultAccount([]byte("123456"))
	if err != nil {
		fmt.Printf("password error\n")
		return
	}
	account2, err := wallet.GetAccountByAddress("target address", []byte("123456"))
	if err != nil {
		fmt.Printf("GetContractAddressByaddress error 114\n")
		return
	}

	//get a compiled wasm file from ont_cpp
	wasmfile := "./oep4.wasm"

	//set timeout
	timeoutSec := 30 * time.Second

	// read wasm file and get the Hex fmt string
	code, err := ioutil.ReadFile(wasmfile)
	if err != nil {
		fmt.Printf("readfile %s error\n", wasmfile)
		return
	}

	codeHash := common.ToHexString(code)

	//===========================================
	gasprice := uint64(500)
	gaslimit := uint64(2000000000000)
	// deploy the wasm contract
	txHash, err := ontSdk.WasmVM.DeployWasmVMSmartContract(
		gasprice,
		gaslimit,
		signer,
		codeHash,
		"OEP4 wasm",
		"1.0",
		"author",
		"email",
		"desc",
	)
	if err != nil {
		fmt.Printf("deploy %s error 65\n", wasmfile)
		return
	}
	_, err = ontSdk.WaitForGenerateBlock(timeoutSec)
	if err != nil {
		fmt.Printf("WaitForGenerateBlock error\n")
		return
	}
	fmt.Printf("the deploy contract txhash is %x\n", txHash)

	//calculate the contract address from code
	contractAddr, err := utils.GetContractAddress(codeHash)
	if err != nil {
		fmt.Printf("GetContractAddress error\n")
		return
	}
	fmt.Printf("the contractAddr is %x\n", contractAddr)

	//============================================
	//invoke wasm method
	//we invoke "init" method first
	txhash, err := ontSdk.WasmVM.InvokeWasmVMSmartContract(
		gasprice, gaslimit, signer, contractAddr, "init", []interface{}{})
	if err != nil {
		fmt.Printf("invoke ope4 init error 89\n")
		return
	}

	res0, err := ontSdk.WasmVM.PreExecInvokeWasmVMContract(contractAddr, "balanceOf", []interface{}{signer.Address})
	if err != nil {
		fmt.Printf("invoke balanceOf error 95\n")
		return
	}
	bs0, err := res0.Result.ToByteArray()
	init_balance0 := binary.LittleEndian.Uint64(bs0)
	fmt.Printf("start balance of %s is %d\n", signer.Address.ToBase58(), init_balance0)

	res00, err := ontSdk.WasmVM.PreExecInvokeWasmVMContract(contractAddr, "balanceOf", []interface{}{account2.Address})
	if err != nil {
		fmt.Printf("invoke balanceOf error 95\n")
		return
	}
	bs00, err := res00.Result.ToByteArray()
	init_balance00 := binary.LittleEndian.Uint64(bs00)
	fmt.Printf("start balance of %s is %d\n", account2.Address.ToBase58(), init_balance00)

	_, err = ontSdk.WaitForGenerateBlock(timeoutSec)
	if err != nil {
		fmt.Printf("WaitForGenerateBlock error\n")
		return
	}
	//get smartcontract event by txhash
	events, err := ontSdk.GetSmartContractEvent(txhash.ToHexString())
	if err != nil {
		fmt.Printf("GetSmartContractEvent error\n")
		return
	}
	fmt.Printf("event is %v\n", events)
	//State = 0 means transaction failed
	if events.State == 0 {
		fmt.Printf("transaction failed\n")
		return
	}
	fmt.Printf("events.Notify:%v", events.Notify)
	for _, notify := range events.Notify {
		fmt.Printf("%+v", notify)
	}

	//2. we construct a tx transfer 500 token from signer account to account2
	txhash, err = ontSdk.WasmVM.InvokeWasmVMSmartContract(
		gasprice, gaslimit, signer, contractAddr, "transfer", []interface{}{signer.Address, account2.Address, int64(500)})
	_, err = ontSdk.WaitForGenerateBlock(timeoutSec)
	if err != nil {
		fmt.Printf("WaitForGenerateBlock error 123\n")
		return
	}
	//get smartcontract event by txhash
	events, err = ontSdk.GetSmartContractEvent(txhash.ToHexString())
	if err != nil {
		fmt.Printf("GetSmartContractEvent error 129\n")
		return
	}
	fmt.Printf("event is %v\n", events)
	//State = 0 means transaction failed
	if events.State == 0 {
		fmt.Printf("transfer failed\n")
		return
	}
	fmt.Printf("events.Notify:%v\n", events.Notify)
	for _, notify := range events.Notify {
		//you check the notify here
		fmt.Printf("%+v", notify)
	}

	//we will query the balance using pre-execuse method
	res1, err := ontSdk.WasmVM.PreExecInvokeWasmVMContract(contractAddr, "balanceOf", []interface{}{signer.Address})
	bs1, err := res1.Result.ToByteArray()
	endbalance1 := binary.LittleEndian.Uint64(bs1)
	fmt.Printf("end balance of %s is %d\n", signer.Address.ToBase58(), endbalance1)

	res2, err := ontSdk.WasmVM.PreExecInvokeWasmVMContract(contractAddr, "balanceOf", []interface{}{account2.Address})
	bs2, err := res2.Result.ToByteArray()
	endbalance2 := binary.LittleEndian.Uint64(bs2)
	fmt.Printf("end balance of %s is %d\n", account2.Address.ToBase58(), endbalance2)

	if init_balance0-500 != endbalance1 {
		fmt.Printf("transfer calculate error.\n")
	} else {
		fmt.Printf("signer transfer calculate success.\n")
	}

	if init_balance00+500 != endbalance2 {
		fmt.Printf("transfer calculate error.\n")
	} else {
		fmt.Printf("account2 transfer calculate success.\n")
	}

	fmt.Println("==============================end ==========================")
}
