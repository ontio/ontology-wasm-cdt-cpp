#define WASM_LOCAL_DEBUG

#include<ontiolib/ontio.hpp>
using namespace ontio;
using std::string;
using std::set;

class hello : public contract {
public:
	using contract::contract;

	bool testbytearray(NeoByteArray &t) {
		for(auto i: t) {printf("%02x", i);}
		printf("\n");
		return 1;
	}

	void testneolist(NeoList<NeoByteArray, NeoInt<int64_t>> &t) {
		NeoByteArray t0 = std::get<0>(t.value);
		NeoInt<int64_t> t1 = std::get<1>(t.value);
		for(auto i: t0) {printf("%02x", i);}
		printf("\n");

		printf("NeoInt: %lld\n", t1.value);
	}

	void testh256(NeoH256 &t) {
		t.debugout();
	}

	void testneoint(NeoInt<int64_t> &t) {
		printf("NeoInt: %lld\n", t.value);
	}
};

ONTIO_DISPATCH( hello, (testbytearray)(testneolist)(testh256))

void testbytearray(void) {
	uint32_t testtype = 0;
	string action("testbytearray");
	NeoByteArray t = {0x12,0x34,0xfe, 0xab, 0xff};

	auto v = pack(action, t);
	set_input(v);
	apply();
}

void testneolist(void) {
	NeoInt<int64_t> t0;
	t0.value = 97;
	NeoByteArray t1 = {0x99,0x34,0xfe, 0xab, 0xff};
	string action("testneolist");
	NeoList<NeoByteArray, NeoInt<int64_t>> t;
	std::get<0>(t.value) = t1;
	std::get<1>(t.value) = t0;

	auto v = pack(action, t);
	set_input(v);
	apply();
}

void testneouint256(void) {
	string action("testh256");
	NeoH256 t = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};

	auto v = pack(action, t);
	set_input(v);

	apply();
}


extern "C" void invoke(void) {
	testbytearray();
	testneolist();
	testneouint256();
}
