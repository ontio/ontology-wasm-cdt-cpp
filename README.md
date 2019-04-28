# 				*ontology wasm cdt*

​	ontology wasm toolchain for smart contract development using c/c++. including libc/libc++/boost/. will add more later.

- Free software: MIT license

[TOC]

# Install

##### a. install directly( support ubuntu18.04 amd64 )

clone this project. and follow these command below.

```
cd install
cp -r * ${your_install_dir}/
export PATH=$PATH:${your_install_dir}/bin
```

##### b. install by deb

```
wget https://github.com/ontio/ontology-wasm-cdt-cpp/blob/master/pkg/ontowasm-ubuntu18.04-amd64.deb?raw=true
dpkg -i ontowasm-ubuntu18.04-amd64.deb?raw=true
export PATH=$PATH:/ontowasm
```

##### c. install by docker

​	1. you need install docker first.  

```
mkdir workkdir; cd workdir
wget https://github.com/ontio/ontology-wasm-cdt-cpp/blob/master/docker/Dockerfile
docker built -t ontowasm .
docker run -it --rm -v /absolute/path/input_dir:/root/contracts ontowasm "/bin/bash"
```

​	

# Feature

- Runtime api for blockchain interaction
- libc/libc++/boost/crypto support.

# How to write contract.

code blow is a simple oep4 contract for ontology. only need include<ontoiolib/ontio.hpp>.  class oep4 is your contract class. only need public inherit the contract class. and realize any interface you want. last use the ONTIO_DISPATCH macro to generate the entry code of your contract. developer only need care about the interface.

```
#include<ontiolib/ontio.hpp>
#include<ontiolib/base58.hpp>

using namespace ontio;

class oep4 : public contract {
    key SUPPLY_KEY = make_key("totalsupply");
    address OWNER = base58toaddress("Ab1z3Sxy7ovn4AuScdmMh4PRMvcwCMzSNV");
    asset total = 1000000000;
    std::string balancePrefix = "balance_";
    std::string approvePrefix = "approve_";
    std::string NAME = "WTK";
    std::string SYMBOL = "SYM";

    int DECIMALS = 9;

    struct transferM {
        address from;
        address to;
        asset amount;
        ONTLIB_SERIALIZE( transferM,  (from)(to)(amount) )
    };
	using mul_trans_args = std::vector<struct transferM>;

    public:
    using contract::contract;
    bool init(){
        check(check_witness(OWNER),"checkwitness failed");
        asset totalsupply = 0;
        if (storage_get(SUPPLY_KEY,totalsupply)){
            return false;
        }

        storage_put(SUPPLY_KEY,total);
        storage_put(make_key(balancePrefix,OWNER),total);
        char buffer [100];
        sprintf(buffer, "%s %s %d","init",OWNER.tohexstring().c_str(),total);
        notify(buffer);
        return true;
    }

    std::string name(){
        return NAME;
    }

    std::string symbol(){
        return SYMBOL;
    }

    asset totalSupply(){
        return total;
    }

    int decimals(){
        return DECIMALS;
    }

    bool transfer(address from, address to , asset amount){
        check(check_witness(from),"checkwitness failed");
        check(amount > 0,"amount should greater than 0");
        if (from == to){
            return true;
        }
        asset frombalance = balanceOf(from);
        asset tobalance = balanceOf(to);

        key fromkey = make_key(balancePrefix,from);
        key tokey = make_key(balancePrefix,to);

        check(frombalance >= amount,"amount error");
        if (frombalance == amount){
            storage_delete(fromkey);
        }else{
            asset tmp = frombalance - amount;
            storage_put(fromkey,tmp);
        }
        asset tmp = tobalance+amount;
        storage_put(tokey,tmp);
        char buffer [128];
        sprintf(buffer, "transfer from %s to %s . amount %d",from.tohexstring().c_str(),to.tohexstring().c_str(),amount);
        notify(buffer);
        notify("transfer accomplised");

        return true;
    }

    bool approve(address from, address to , asset amount){
        check(check_witness(from),"checkwitness failed");
        check(amount > 0,"amount error");
        check(balanceOf(from) >= amount,"amount error");
        storage_put(make_key(approvePrefix,from,"_",to),amount);
        return true;
    }

    asset allowance(address from, address to){
        asset b = 0;
        storage_get(make_key(approvePrefix,from,"_",to),b);
        return b;
    }

    bool transferFrom(address owner, address from,address to, asset amount){
        check(check_witness(owner),"checkwitness failed");
        asset aw = allowance(from,owner);
        check(aw >= amount,"amount error");
        asset frombalance = balanceOf(from);
        check(frombalance >= amount,"amount error");

        key allownancekey = make_key(approvePrefix,from,"_",owner);
        key frombalancekey = make_key(balancePrefix,from);

        if (aw == amount){
            storage_delete(allownancekey);
        }else{
            storage_put(allownancekey, aw - amount);
        }

        if (balanceOf(from) == amount){
            storage_delete(frombalancekey);
        }else{
            storage_put(frombalancekey,frombalance - amount);
        }

        asset tmp = balanceOf(to) + amount;
        storage_put(make_key(balancePrefix,to),tmp);
        return true;
    }

    bool transferMulti(mul_trans_args params){
        for (struct transferM p :params){
            check(transfer(p.from, p.to, p.amount),"tranfer failed");
        }
        return true;
    }

    asset balanceOf(address acct){
        asset b = 0;
        storage_get(make_key(balancePrefix,acct),b);
        return b;
    }
};
ONTIO_DISPATCH(oep4, (init)(transfer)(balanceOf)(allowance)(transferFrom)(approve)(name)(symbol)(totalSupply)(decimals)(transferMulti));
```

# Interface

developer can check the [How_To_Use_cdt](How_To_Use_cdt.md) . provide api to interact with blockchain. like get the context of smartcontract. read or store data to blockchain.  and [How_To_Run_ontologywasm_node](How_To_Run_ontologywasm_node.md) provide a simple guide that run a ontology wasm node. and how to deploy or invoke a contract with cli. 

# Tools

ont_c :  wasm tool for compiling  c contract.

ont_cpp : wasm tool for compiling c++ contract

wasm-ld :  linker for wasm object.

wasm-objdump:  dump wasm object file.

wasm2wat:  transfer binary file to wast file.

ontio-abigen: generate abi file.

wasm-opt: can opt the code of wasm.

# Usage

```
ont_cpp oep4.cpp
ont_cpp oep4.cpp -o oep4.wasm
ontio-abigen oep4.cpp --contract="oep4" --output="oep4.abj.json" --binary="oep4.wasm"
```

# License

This project is licensed under the [MIT license](LICENSE).

# Third Party License

To quickly explore the feasibility of wasm contract development, initial development is based on the work make by third parties:

- eos like coding style. 
- libc
- libc++
- boost

See the source code files for more details.

Copies of third party licenses can be found in [LICENSE-THIRD-PARTY](Third_Party_Licenses.md).
