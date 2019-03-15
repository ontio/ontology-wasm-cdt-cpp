//#define WASM_LOCAL_DEBUG
#define WASM_DEBUG_INTERFACE
#include<ontiolib/ontio.hpp>

using namespace ontio;

class hello : public contract {
public:
	using contract::contract;

	void transfer(address from, address to , asset value) {
		printf("%s\n", __FUNCTION__);
		bool success;
		success = ont::transfer(from, to, value);
		printf("success: %u", success);
	}

	void approve(address from, address to , asset value) {
		printf("%s\n", __FUNCTION__);
		bool success = ont::approve(from, to, value);
		printf("success: %u", success);
	}

	void transferfrom(address sender, address from, address to, asset value) {
		printf("%s\n", __FUNCTION__);
		bool success = ont::transferfrom(sender,from, to, value);
		printf("success: %u", success);
	}

	void balanceof(address from) {
		printf("%s\n", __FUNCTION__);
		asset tmp = ont::balanceof(from);
		printf("%lld\n", tmp.amount);
#ifndef WASM_LOCAL_DEBUG
		ret(tmp);
#endif
	}

	void allowance(address from, address to) {
		printf("%s\n", __FUNCTION__);
		asset tmp = ont::allowance(from, to);
		printf("%d\n", sizeof(bool));
		printf("%lld\n", tmp.amount);
#ifndef WASM_LOCAL_DEBUG
		ret(tmp);
#endif
	}

	void tsp(void) {
		printf("%s\n", __FUNCTION__);
		uint64_t t = timestamp();
		printf("%llu\n", t);
		ret(t);
	}

	void blkheight(void) {
		printf("%s\n", __FUNCTION__);
		uint32_t t = block_height();
		printf("%u\n", t);
		ret(t);
	}

	void selfaddr(void) {
		printf("%s\n", __FUNCTION__);
		address addr = self_address();
		for(auto i : addr) { printf("%02x ", i); }; printf("\n");
		ret(addr);
	}

	void calleraddr(void ) {
		printf("%s\n", __FUNCTION__);
		address addr = caller_address();
		for(auto i : addr) { printf("%02x ", i); }; printf("\n");
		ret(addr);
	}

	void entryaddr(void) {
		printf("%s\n", __FUNCTION__);
		address addr = entry_address();
		for(auto i : addr) { printf("%02x ", i); }; printf("\n");
		ret(addr);
	}

	void getinput(void) {
		printf("%s\n", __FUNCTION__);
		std::vector<char> res = get_input();
		for(auto i : res) { printf("%02x ", i); }; printf("\n");
		printf("hhehehehe\n");
	}

	void crtblkhash(void) {
		printf("%s\n", __FUNCTION__);
		H256 t = current_blockhash();
		for(auto i : t) { printf("%02x ", i); }; printf("\n");
		ret(t);
	}

	void crttxhash(void) {
		printf("%s\n", __FUNCTION__);
		H256 t = current_txhash();
		for(auto i : t) { printf("%02x ", i); }; printf("\n");
		ret(t);
	}

	void ntf(std::string s) {
		printf("%s\n", __FUNCTION__);
		printf("%s\n", s.c_str());
		notify(s);
	}

	void callcrt(address &addr) {
		printf("%s\n", __FUNCTION__);
		address t;
		std::string method("calleraddr");
		auto v = pack(method);
		call_contract(addr, v, t);
		printf("get address from call_contract\n");
		for(auto i : t) { printf("%02x ", i); }; printf("\n");
		ret(t);
	}

	void crtmigrate(std::vector<char> &code, uint32_t &vmtype, std::string &name, std::string &version, std::string &author, std::string &email, std::string &desc) {
		printf("%s\n", __FUNCTION__);
		address t = contract_migrate(code, vmtype, name, version, author, email, desc);
		for(auto i : t) { printf("%02x ", i); }; printf("\n");
		ret(t);
	}

	void storageread(key key_t) {
		printf("%s\n", __FUNCTION__);
		address addr;
		bool success = storage_read(key_t, addr);
		if (success) {
			for(auto i : addr) { printf("%02x ", i); }; printf("\n");
		} else {
			printf("storageread error\n");
		}
		ret(addr);
	}

	void storagewrite(key &key_t, address &addr) {
		printf("%s\n", __FUNCTION__);
		storage_write(key_t, addr);
		for(auto i : addr) { printf("%02x ", i); }; printf("\n");
	}

	void stogv(key &key_t, asset &val) {
		printf("%s\n", __FUNCTION__);
		storage_write(key_t, val);
		printf("val: %lld\n", val.amount);
	}

	void storagedel(key key_t) {
		printf("%s\n", __FUNCTION__);
		storage_delete(key_t);
	}
};

ONTIO_DISPATCH( hello, (transfer)(approve)(transferfrom)(balanceof)(allowance)(tsp)(blkheight) (selfaddr)(calleraddr)(entryaddr)(getinput)(crtblkhash)(crttxhash)(ntf)(callcrt)(crtmigrate)(storageread)(storagewrite)(stogv)(storagedel))

extern "C"{
#ifdef WASM_LOCAL_DEBUG
void invoke()
{
	//std::string action("transfer");
	//std::string action("transferfrom");
	//std::string action("approve");
	std::string action("balanceof");
	//std::string action("allowance");

	address sender 	= ontio::hexstobytes<20>("c238da8b5b6ad3d723dc5d4463346c99d53bcf22");
	address from 	= ontio::hexstobytes<20>("a36963b39d3eb14ddc1a9d016a14256ac594f8a4");
	address to 	= ontio::hexstobytes<20>("b0dc4eca16f06404201b3889b6adc2a6ed0246f4");
	asset val 	= 0x1234;

	size_t size = pack_size(sender) + pack_size(from) + pack_size(to) + pack_size(val) + pack_size(action);
	printf("size %lu\n", size);

	std::vector<char> result;
	result.resize(size);
	datastream<char*> ds(result.data(), result.size());
	ds << action;
	//ds << from << to << val;
	//ds << from << to;
	ds << from;
	//ds << sender <<from << to << val;
	
	//print_byte((uint8_t *)result.data(), result.size());

	save_input_arg(result.data(), result.size());
	apply(); //simulation entry call
}
#endif
}
