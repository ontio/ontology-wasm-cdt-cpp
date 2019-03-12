#define WASM_LOCAL_DEBUG
#include<stdio.h>
#include<ontiolib/datastream.hpp>
#include<ontiolib/ontio.hpp>
#include<string>
template<size_t Size>
std::array<uint8_t,Size> HexToBytes(const std::string& hex) {
  std::array<uint8_t, Size> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    //printf("%s\n", byteString.c_str());
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    printf("%02x ", byte);
    bytes[i/2] = byte;
  }

  printf("over\n");

  return bytes;
}

using namespace ontio;
extern "C" {
uint32_t call_contract(void *addr , void *args, uint32_t len);
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

class hello : public contract {
public:
	using contract::contract;
	//[[eosio::action]]
	//[[ chenglin::xxx jdljfalksjflajl jdkjfakj ]]
	void hi(name user) {
		printf("hello world xxxx\n");
	}

	void transfer(address from, address to , asset value) {
		transfer_arg tt0("transfer", from, to, value);
		//tt0.version = 0x0;
		//tt0.method = std::string("transfer");

		//ontstate state_t(from, to, value);
		/*
		   ontstate state_t;
		   state_t.from.addr 	= from;
		   state_t.to.addr 	= to;
		   state_t.value 	= value;
		   */

		//tt0.tfs.states.push_back(state_t);
		//tt0.tfs.states[0] = state_t;

		size_t size = pack_size(tt0);
		//size_t size = 128;
		printf("args size %u\n", size);
		constexpr size_t max_stack_buffer_size = 512;
		void *buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
		memset(buffer, 0, size);
		datastream<const char*> ds_t((char*)buffer, size);
		//ds_t.seekp(0);
		//
		ds_t << tt0;


		size_t s = ds_t.tellp();
		std::array<uint8_t, 20> addr = HexToBytes<20>(std::string("0000000000000000000000000000000000000001"));
		printf("print_byte here\n");
		print_byte(addr.data(), 20);

		printf("args buffer\n");
		print_byte((uint8_t *)buffer, ds_t.tellp());

		//CallContract((void*)addr.data(),(void *)buffer, ds_t.tellp());
		printf("hello world steven in transfer\n");
		printf("version: %u\n", tt0.version);
		printf("method: %s\n", tt0.method.c_str());
		printf("from:  ");
		for(auto i : tt0.tfs.states[0].from.addr){printf("%02x",i);}
		printf("\n");
		printf("to: ");
		for(auto i : tt0.tfs.states[0].to.addr){printf("%02x",i);}
		printf("\n");
		printf("asset: 0x%x\n", tt0.tfs.states[0].value);

		//call_contract((void*)addr.data(),(void *)buffer, ds_t.tellp());

		transfer_arg tt1;
		memset(&tt1, 0, sizeof(transfer_arg));
		ds_t.seekp(0);
		ds_t >> tt1;
		printf("========================================\n");
		printf("version: %u\n", tt1.version);
		printf("method: %s\n", tt1.method.c_str());
		printf("from:  ");
		for(auto i : tt1.tfs.states[0].from.addr){printf("%02x",i);}
		printf("\n");
		printf("to: ");
		for(auto i : tt1.tfs.states[0].to.addr){printf("%02x",i);}
		printf("\n");
		printf("asset: 0x%x\n", tt1.tfs.states[0].value);
	}

	void transferfrom(address sender, address from, address to, asset value)
	{
		transfer_from tt0(sender, from, to, value);

		size_t size = pack_size(tt0);
		//size_t size = 128;
		printf("args size %u\n", size);
		constexpr size_t max_stack_buffer_size = 512;
		void *buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
		memset(buffer, 0, size);
		datastream<const char*> ds_t((char*)buffer, size);
		//ds_t.seekp(0);
		//
		ds_t << tt0;

		std::array<uint8_t, 20> addr = HexToBytes<20>(std::string("0000000000000000000000000000000000000001"));
		printf("print_byte here\n");
		print_byte(addr.data(), 20);

		printf("args buffer\n");
		print_byte((uint8_t *)buffer, ds_t.tellp());
		//call_contract((void*)addr.data(),(void *)buffer, ds_t.tellp());
	}

	void xiiiiiqicheng(unsigned_int &a) {
		uint32_t b = 0x1234567;
		if (a == b)
			printf("hello world chenglin_hi 0x%x\n", a);
		else{
			printf("wrong args passed\n");
		}
	}

};

ONTIO_DISPATCH( hello, (hi)(transfer)(transferfrom)(xiiiiiqicheng) )

//extern "C"{
//void apply(char *str)
//{
//	printf("hello world. %s\n", str);
//}
//}
//


	/*
std::vector<char> HexToBytes(const std::string& hex) {
  std::vector<char> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    //printf("%s\n", byteString.c_str());
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    printf("%02x ", byte);
    bytes.push_back(byte);
  }
  printf("over\n");

  return bytes;
}
*/

//std::array<uint8_t, ADDR_LEN>


extern "C"{


void  ontio_assert( uint32_t test, const char* msg  )
{
	if ( not test )
	{
		printf("%s\n", msg);
		abort();
	}
	
}



void invoke()
{

	address from;
	address to;
	asset val;

	std::string action("transfer");
	//std::string action("transferfrom");
	
	//NativeArg tt;
	//tt.version = 0x1;
	//tt.method = std::string("transfer");
	//tt.from.bytes = HexToBytes(std::string("a36963b39d3eb14ddc1a9d016a14256ac594f8a4")).data();
	//tt.to.bytes = HexToBytes(std::string("a36963b39d3eb14ddc1a9d016a14256ac594f8a4")).data();
	
	//uint8_t *ttt = (uint8_t *)HexToBytes<20>(std::string("a36963b39d3eb14ddc1a9d016a14256ac594f8a4")).data();
	//std::string m_t("transfer");
	from = HexToBytes<20>(std::string("a36963b39d3eb14ddc1a9d016a14256ac594f8a4"));
	to = HexToBytes<20>(std::string("b0dc4eca16f06404201b3889b6adc2a6ed0246f4"));
	val = 0x1234;

	//int i  = 0;
	//while(i < 20) {
	//	printf("%02x\n", tt.to.bytes.data()[i]);
	//	i++;
	//}
	//unsigned_int a = 0x12345678;
	//address to;
	//to.bytes = HexToBytes<20>(std::string("b0dc4eca16f06404201b3889b6adc2a6ed0246f4"));
	
		
	//first save the runtime input buffer.
	//uint64_t action = name("hi").value;
	//uint64_t action = name("xiiiiiqicheng").value;
	//uint64_t action = name("hii").value;
	//unsigned_int a = 0x12345678;
	//uint32_t a = 0x78;
	//unsigned_int a = 0x12345678;
	//uint64_t a = 0x1234;
	//uint64_t a = 20;
	//name user("steven");
	//std::string s("now try string.");


	size_t size = pack_size(from) + pack_size(to) + pack_size(val) + pack_size(action);
	//size_t size = 68;
	printf("size %lu\n", size);
	//printf("%lu\n",name( BOOST_PP_STRINGIZE("hi")  ).value);


	constexpr size_t max_stack_buffer_size = 512;
	void *buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
	datastream<char*> ds((char*)buffer, size);
	
	//printf("%x\n", action);
	//ds << a;
	//WriteVarUint(ds, a);
	//WriteNativeArg<char *>(ds, tt);
	//WriteNativeArg(ds, tt);

	ds << action<< from << to << val;
	printf("ds bytes %lu\n", ds.tellp());
	print_byte((uint8_t *)buffer, ds.tellp());
	//ds << s;
	save_input_arg(buffer, size);

	//ds.seekp(0);

	//printf("ds bytes %lu\n", ds.remaining());
	//NativeArg tt0;
	//ds >> tt0;
	//printf("version: %u\n", tt0.version);
	//printf("method: %s\n", tt0.method.c_str());
	//printf("from:  ");
	//for(auto i : tt0.from.bytes){printf("%02x",i);}
	//printf("\n");
	//printf("to: ");
	//for(auto i : tt0.to.bytes){printf("%02x",i);}
	//printf("\n");

	//printf("asset: 0x%x\n", tt0.value);

	//unsigned_int b;
	//ds >> b;
	//printf("a value 0x%x\n", a.value);
	//printf("b value 0x%x\n", b.value);

	apply(); //simulation entry call
	free(buffer);
}
}
