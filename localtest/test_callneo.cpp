#define WASM_TEST
#define WASM_LOCAL_DEBUG

#include<ontiolib/ontio.hpp>
using namespace ontio;
using std::string;
using std::set;

class hello : public contract {
public:
	using contract::contract;
	bool testsize(NeoUsize &t) {
		printf("size is : %u\n", t);
		return 1;
	}

	bool testbytearray(NeoByteArray &t) {
		for(auto i: t) {printf("%02x", i);}
		printf("\n");
		return 1;
	}

	void testneolist(NeoList<NeoByteArray, NeoInt64> &t) {
		NeoByteArray t0 = std::get<0>(t.value);
		NeoInt64 t1 = std::get<1>(t.value);
		for(auto i: t0) {printf("%02x", i);}
		printf("\n");

		printf("NeoInt64: %lld\n", t1.value);
	}

	void testuint256(NeoUint256 &t) {
		t.debugout();
	}
};

ONTIO_DISPATCH( hello, (testsize)(testbytearray)(testneolist)(testuint256))

void testbytearray(void) {
	uint32_t testtype = 0;
	string action("testbytearray");
	NeoByteArray t = {0x12,0x34,0xfe, 0xab, 0xff};

	auto v = pack(action, t);
	save_input_arg(v.data(), v.size());
	apply();
}

void testusize(void) {
	string action("testsize");
	NeoUsize t;
	t.value = 9;

	auto v = pack(action, t);
	save_input_arg(v.data(), v.size());
	apply();
}

void testneolist(void) {
	NeoInt64 t0;
	t0.value = 97;
	NeoByteArray t1 = {0x99,0x34,0xfe, 0xab, 0xff};
	string action("testneolist");
	NeoList<NeoByteArray, NeoInt64> t;
	std::get<0>(t.value) = t1;
	std::get<1>(t.value) = t0;

	auto v = pack(action, t);
	save_input_arg(v.data(), v.size());
	apply();
}

void testneouint256(void) {
	string action("testuint256");
	NeoUint256 t = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

	auto v = pack(action, t);
	save_input_arg(v.data(), v.size());

	apply();
}


extern "C" void invoke(void) {
	testbytearray();
	testusize();
	testneolist();
	testneouint256();
}
