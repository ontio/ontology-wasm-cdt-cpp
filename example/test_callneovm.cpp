#include<ontiolib/ontio.hpp>
using std::string;
namespace ontio {
	struct test_conext {
		address admin;
		std::map<string, address> addrmap;
		ONTLIB_SERIALIZE( test_conext, (admin) (addrmap))
	};
};

using namespace ontio;

class hello: public contract {
	public:
	using contract::contract;
	address callneo_address(test_conext &tc) {
		address res;
		address test_neo = tc.addrmap["neo_contract.avm"];
		call_neo_contract(test_neo, pack_neoargs("bytearray", test_neo), res);

		check(res == test_neo, "get bytearray result wrong");
		notify_event(res);
		return res;
	}

	address callneo_vector(test_conext &tc) {
		vector<char> res;
		address test_neo = tc.addrmap["neo_contract.avm"];
		call_neo_contract(test_neo, pack_neoargs("bytearray", test_neo), res);

		//vector<uint8_t> com(test_neo.begin(), test_neo.end());
		address com;
		check(res.size()== 20, "vector should be 20 Bytes");
		std::copy(res.begin(), res.end(), com.begin());
		check(test_neo == com, "get bytearray result wrong");
		notify_event(com);
		return com;
	}

	string callneo_string(test_conext &tc) {
		string res;
		address test_neo = tc.addrmap["neo_contract.avm"];
		call_neo_contract(test_neo, pack_neoargs("string", "test_callneo"), res);
		check(res == string("test_callneo hello my name is steven"), "return error");
		notify_event(res);
		return res;
	}

	bool callneo_bool(test_conext &tc) {
		bool res;
		address test_neo = tc.addrmap["neo_contract.avm"];
		call_neo_contract(test_neo, pack_neoargs("boolean", true), res);
		check(res == true, "return error");
		notify_event(res);

		call_neo_contract(test_neo, pack_neoargs("boolean", false), res);
		check(res == false, "return error");
		notify_event(res);
		return res;
	}

	H256 callneo_h256(test_conext &tc) {
		H256 res;
		address test_neo = tc.addrmap["neo_contract.avm"];
		H256 cbhash = current_blockhash();
		call_neo_contract(test_neo, pack_neoargs("H256", cbhash), res);
		check(res == cbhash, "return error");
		notify_event(res);
		return cbhash;
	}

	int128_t callneo_int(test_conext &tc) {
		int128_t res = 0;
		address test_neo = tc.addrmap["neo_contract.avm"];

		call_neo_contract(test_neo, pack_neoargs("intype", 9), res);
		check(res = 9 + 0x101, "return error");
		notify_event(res);

		// here test overflow error. should be panic.
		//int32_t res_t = 0;
		//call_neo_contract(test_neo, pack_neoargs("intype", int128_t(std::numeric_limits<int64_t>::min())), res_t);

		return res;
	}
};

ONTIO_DISPATCH( hello,(callneo_h256)(callneo_string)(callneo_address)(callneo_vector)(callneo_bool)(callneo_int))
