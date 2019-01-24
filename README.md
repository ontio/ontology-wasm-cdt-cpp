# ontio-dtc
ontology wasm toolchain for smart contract development using c/c++. including libc/libc++/boost/. will add more later.

# Install

clone this project. and follow these command below.

```
cd install
cp -r * your_install_dir/
export PATH=$PAHT:your_install_dir/bin
```

# Feature

- Runtime api for blockchain interaction
- Contract level storage management
- libc/libc++/boost support.

# How to write contract.

code blow is a simple oep4 contract for ontology. only need include<ontoiolib/ontio.hpp>.  class oep4 is your contract class. only need public inherit the contract class. and realize any interface you want. last use the ONTIO_DISPATCH macro to generate the entry code of your contract. developer only need care about the interface.

```
#include<ontiolib/ontio.hpp>

using namespace ontio;

class oep4 : public contract {
	key SUPPLY_KEY = pack(std::string("TotalSupply"));
	address OWNER = {0xe6,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x47,0xa5,0x05,0x61,0xf7,0xf3,0x29,0x40,0xc7,0xc6,0xc2,0x61};
	asset total = 1000000000;
	uint8_t balanceprfix = 0x1;

	public:
	using contract::contract;
	void init(void) {
#ifdef WASM_LOCAL_DEBUG_OEP4
		printf("%s\n",__FUNCTION__);
#endif
		bool success = false;
		asset total_inner = 0;
#ifdef WASM_LOCAL_DEBUG_OEP4
		for(auto i: OWNER) { printf("%02x", i); }; printf("\n");
#endif
		if (check_witness(OWNER)) {
			if(storage_read(SUPPLY_KEY, total_inner)) {
#ifdef WASM_LOCAL_DEBUG_OEP4
				printf("already init. total = %lld\n", total_inner.amount);
#endif
				ret(success);
			} else {
#ifdef WASM_LOCAL_DEBUG_OEP4
				printf("start init. total %lld", total.amount);
#endif
				success = true;
				key key_owner = make_key(balanceprfix, OWNER);
				storage_write(SUPPLY_KEY, total);
				storage_write(key_owner, total);
				ret(success);
			}
		} else {
#ifdef WASM_LOCAL_DEBUG_OEP4
			printf("auth failed\n");
#endif
			success = false;
			ret(success);
		}
	}

	void transfer(address from, address to, asset amount) {
#ifdef WASM_LOCAL_DEBUG_OEP4
		printf("%s\n",__FUNCTION__);
#endif
		bool success = true;
		bool faild = false;
		if (not check_witness(from)) {
#ifdef WASM_LOCAL_DEBUG_OEP4
			printf("transfer auth failed\n");
#endif
			ret(faild);
		}

		if (amount < 0)
			ret(faild);

		key fromkey = make_key(balanceprfix, from);
		asset frombalance = 0; 
		if (not storage_read(fromkey, frombalance))
			ret(faild);

		if (amount > frombalance)
			ret(faild);
		else if (amount == frombalance)
			storage_delete(fromkey);
		else {
			frombalance -= amount;
			storage_write(fromkey, frombalance);
		}

		key tokey = make_key(balanceprfix, to);
		asset tobalance = 0;
		storage_read(tokey, tobalance);

		tobalance += amount;
#ifdef WASM_LOCAL_DEBUG_OEP4
		printf("transfer amount : %lld\n", amount.amount);
#endif
		storage_write(tokey, tobalance);
		ret(success);
	}

	void balance(address addr) {
#ifdef WASM_LOCAL_DEBUG_OEP4
		printf("%s\n",__FUNCTION__);
#endif
		bool success = true;
		bool failed = false;
		asset mybalance = 0;
		key addr_key = make_key(balanceprfix, addr);
		if (not storage_read(addr_key, mybalance)) {
#ifdef WASM_LOCAL_DEBUG_OEP4
			printf("read failed\n");
#endif
			ret(failed);
		}
#ifdef WASM_LOCAL_DEBUG_OEP4
		printf("read success\n");
		printf("balance %lld\n", mybalance.amount);
#endif
		ret(mybalance);
	}
};

ONTIO_DISPATCH(oep4, (init)(transfer)(balance))
```

# Interface

developer can check the [service.hpp](install/include/ontiolib/service.hpp) . provide api to interact with blockchain. like get the context of smartcontract. read or store data to blockchain. etc.

# Tools

ont_c :  wasm tool for compiling  c contract.

ont_cpp : wasm tool for compiling c++ contract

wasm-ld :  linker for wasm object.

wasm-objdump:  dump wasm object file.

wasm2wat:  transfer binary file to wast file.



# Usage

```
ont_c test.c
ont_c test.c -o test.wasm
ont_c oep4.cpp
ont_c oep4.cpp -o oep4.wasm
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
