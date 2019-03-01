#include<stdio.h>
#include <ontiolib/ontio.hpp>
#include<string>

using namespace ontio;
class hello : public contract {
public:
	using contract::contract;
	//[[eosio::action]]
	//[[ chenglin::xxx jdljfalksjflajl jdkjfakj ]]
	void hi(name user) {
		printf("hello world xxxx\n");
	}

	void hii(std::string s) {
		printf("hello world steven %s\n", s.c_str());
	}

	void xiiiiiqicheng(int64_t a) {
		printf("hello world chenglin_hi 0x%x\n", a);
	}

};

ONTIO_DISPATCH( hello, (hi)(hii)(xiiiiiqicheng) )

//extern "C"{
//void apply(char *str)
//{
//	printf("hello world. %s\n", str);
//}
//}

extern "C"{
void  ontio_assert( uint32_t test, const char* msg  )
{
	if ( test )
	{
		printf("%s\n", msg);
		abort();
	}
	
}
void invoke()
{
	//first save the runtime input buffer.
	//uint64_t action = name("hi").value;
	uint64_t action = name("xiiiiiqicheng").value;
	//uint64_t action = name("hii").value;
	int64_t a = 0x1234567;
	//name user("steven");
	std::string s("now try string.");


	size_t size = pack_size(action) + pack_size(s);
	printf("%lu\n", size);
	//printf("%lu\n",name( BOOST_PP_STRINGIZE("hi")  ).value);


	constexpr size_t max_stack_buffer_size = 512;
	void *buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
	datastream<const char*> ds((char*)buffer, size);
	
	printf("%x\n", action);
	ds << action;
	//ds << s;
	ds << a;
	save_input_arg(buffer, size);

	apply(); //simulation entry call
	free(buffer);
}
}
