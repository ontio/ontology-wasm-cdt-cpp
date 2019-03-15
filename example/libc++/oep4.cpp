#define WASM_DEBUG_INTERFACE
//#define WASM_LOCAL_DEBUG_OEP4
#include<ontiolib/ontio.hpp>
using namespace ontio;

class oep4 : public contract {
	key SUPPLY_KEY = pack(std::string("TotalSupply"));
	address OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
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
