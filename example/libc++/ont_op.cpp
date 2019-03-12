//#define WASM_LOCAL_DEBUG
#define WASM_DEBUG_INTERFACE
#include<stdio.h>
#include<string>
#include<ontiolib/ontio.hpp>

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

	void transfer(address from, address to , asset value) {
		transfer_arg tt0("transfer", from, to, value);
		auto data = pack(tt0);

		address addr = hexstobytes<20>("0000000000000000000000000000000000000001");
		printf("print_byte here\n");
		print_byte(addr.data(), 20);

		printf("args size %u\n", data.size());
		print_byte((uint8_t *)data.data(), data.size());

		printf("hello world steven in transfer\n");
		tt0.debugout();
#ifndef WASM_LOCAL_DEBUG
		call_contract((void*)addr.data(), data.data(), data.size());
#endif

		printf("========================================\n");
		transfer_arg tt1;
		memset(&tt1, 0, sizeof(transfer_arg));
		tt1 = unpack<transfer_arg>(data);
		tt1.debugout();
	}

	void approve(address from, address to , asset value) {
		approve_arg tt0("approve", from, to, value);
		auto data = pack<approve_arg>(tt0);
		std::array<uint8_t, 20> addr = hexstobytes<20>(std::string("0000000000000000000000000000000000000001"));
		tt0.debugout();

#ifndef WASM_LOCAL_DEBUG
		call_contract((void*)addr.data(), data.data(), data.size());
#endif

		printf("========================================\n");
		approve_arg tt1 = unpack<approve_arg>(data);
		tt1.debugout();
	}

	void transferfrom(address sender, address from, address to, asset value)
	{
		transferfrom_arg tt0("transferFrom", sender, from, to, value);
		auto data = pack<transferfrom_arg>(tt0);

		std::array<uint8_t, 20> addr = hexstobytes<20>(std::string("0000000000000000000000000000000000000001"));

		printf("args size %u\n", data.size());
		print_byte((uint8_t *)data.data(), data.size());
		tt0.debugout();

#ifndef WASM_LOCAL_DEBUG
		call_contract((void*)addr.data(), data.data(), data.size());
#endif

		printf("========================================\n");
		transferfrom_arg tt1 = unpack<transferfrom_arg>(data);
		tt1.debugout();
	}
};

ONTIO_DISPATCH( hello, (transfer)(approve)(transferfrom))

extern "C"{


void  ontio_assert( uint32_t test, const char* msg  )
{
	if ( not test )
	{
		printf("%s\n", msg);
		printf("abort abort abort abort abort\n");
		abort();
	}
	
}

#ifdef WASM_LOCAL_DEBUG
void invoke()
{
	//std::string action("transfer");
	//std::string action("transferfrom");
	std::string action("approve");

	address sender 	= hexstobytes<20>("c238da8b5b6ad3d723dc5d4463346c99d53bcf22");
	address from 	= hexstobytes<20>("a36963b39d3eb14ddc1a9d016a14256ac594f8a4");
	address to 	= hexstobytes<20>("b0dc4eca16f06404201b3889b6adc2a6ed0246f4");
	asset val 	= 0x1234;

	size_t size = pack_size(sender) + pack_size(from) + pack_size(to) + pack_size(val) + pack_size(action);
	printf("size %lu\n", size);

	std::vector<char> result;
	result.resize(size);
	datastream<char*> ds(result.data(), result.size());
	ds << action;
	ds << from << to << val;
	//ds << from << to << val;

	save_input_arg(result.data(), result.size());
	apply(); //simulation entry call
}
#endif
}
