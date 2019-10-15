#include<stdio.h>
#include<vector>
#include<array>
using namespace std;

class A {
	public:
		int64_t amount;
		A(int64_t a): amount(a) {}
};

int a = 0x1234567;
A s(0x999999999999);
A b(0x8888888888);
//array owner
array<uint8_t, 20> OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};

extern "C"{
int invoke(void)
{
	std::vector<char> m;
	m.resize(128);
	//char *q =(char *) malloc(8189);
	printf("point 0x%x\n", m.data());
	printf("a 0x%x\n", a);
	printf("class s.a 0x%x\n", s.amount);
	printf("class b.a 0x%x\n", b.amount);
	//printf("class s.a %PRId64\n", s.amount);

	for(auto i: OWNER) { printf("%02x", i); }; printf("\n");
	return 0;
}
}
