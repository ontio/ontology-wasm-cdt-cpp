#define WASM_LOCAL_DEBUG
#define WASM_DEBUG_INTERFACE
#include<stdio.h>
#include<ontiolib/ontio.hpp>
#include<string>


using namespace ontio;
extern "C" {
bool b58tobin(void *bin, size_t *binszp, const char *b58, size_t b58sz);
void print_byte(uint8_t *buffer, uint64_t len)
{
	uint64_t i = 0;
	for(; i < len; i++)
	{
		printf("%02X ",buffer[i]);
	}
	printf("\n");
}
}
//asset total(1000000000);
//int a = 0x1234;
//int64_t total = 1000000000;

//asset total = 100;

//address OWNER = hexstobytes<20>("e98f4998d837fcdd44a50561f7f32140c7c6c260");
//std::array<uint8_t, 20> OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
//address OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
class hello : public contract {
	//asset total;
	//address OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
public:
	using contract::contract;
	//[[eosio::action]]
	//[[ chenglin::xxx jdljfalksjflajl jdkjfakj ]]
	//hello(datastream<char *> &t) {
	//	total = 1000000000 * 1000000000;
	//	OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
	//}

	void hi(void) {
		//address OWNER = {0xe9,0x8f,0x49,0x98,0xd8,0x37,0xfc,0xdd,0x44,0xa5,0x05,0x61,0xf7,0xf3,0x21,0x40,0xc7,0xc6,0xc2,0x60};
		std::string b58("Ad4pjz2bqep4RhQrUAzMuZJkBC3qJ1tZuT");
#if 0
		std::vector<char> s;
		s.resize(40);
		size_t tmp;
		if (b58tobin(s.data(), &tmp, b58.data(), b58.size())) {
			printf("the data is \n");
			print_byte((uint8_t *)s.data(), tmp);
			printf("size %u\n", tmp);
		} else {
			printf("decode faild\n");
		}
#else
		char *buffer = (char *)malloc(40);
		memset(buffer, 0, 40);
		size_t tmp;
		if (b58tobin(buffer, &tmp, b58.data(), b58.size())) {
			printf("the data is \n");
			print_byte((uint8_t *)buffer, tmp);
			printf("size %u\n", tmp);
		} else {
			printf("decode faild\n");
		}
#endif
		printf("hello world xxxx\n");
		//printf("val: %lld\n", total.amount);
		//for(auto i: OWNER) { printf("%02x", i); }; printf("\n");
		//address sender 	= hexstobytes<20>("c238da8b5b6ad3d723dc5d4463346c99d53bcf22");
		//for(auto i: sender) { printf("%02x", i); }; printf("\n");
	}

	void hii(std::string s) {
		printf("hello world steven %s\n", s.c_str());
	}

	void xiiiiiqicheng(unsigned_int &a) {
		uint32_t b = 0x12345678;
		if (a == b)
			printf("hello world chenglin_hi 0x%x\n", a);
		else{
			printf("wrong args passed\n");
		}
	}

};


ONTIO_DISPATCH( hello, (hi)(hii)(xiiiiiqicheng) )

//extern "C"{
//void apply(char *str)
//{
//	printf("hello world. %s\n", str);
//}
//}
std::vector<char> HexToBytes(const std::string& hex) {
  std::vector<char> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    char byte = (char) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}
extern "C"{


void prints_l( const char* cstr, uint32_t len )
{
	printf("hello world\n");
}

void invoke()
{
	//prints_l("aaaa", 5);
	//first save the runtime input buffer.
	//uint64_t action = name("hi").value;
	//uint64_t action = name("xiiiiiqicheng").value;
	std::string action("hi");
	//uint64_t action = name("hii").value;
	unsigned_int a = 0x12345678;
	//name user("steven");
	//std::string s("now try string.");


	size_t size = pack_size(action) + pack_size(a);
	//size_t size = pack_size(a);
	printf("%lu\n", size);
	//printf("%lu\n",name( BOOST_PP_STRINGIZE("hi")  ).value);


	constexpr size_t max_stack_buffer_size = 512;
	void *buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
	datastream<char*> ds((char*)buffer, size);
	
	//printf("%x\n", action);
	ds << action;

	print_byte((uint8_t *)buffer, ds.tellp());
	//ds << s;
	save_input_arg(buffer, size);



	apply(); //simulation entry call

	free(buffer);
}
}
