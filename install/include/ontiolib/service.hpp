#pragma once

#ifndef WASM_LOCAL_DEBUG
#include "service.h"

namespace ontio {

address self_address(void);
address caller_address(void);
address entry_address(void);
std::vector<char> get_input(void);
bool check_witness(const address &v);
H256 current_blockhash(void);
H256 current_txhash(void);
void notify(const std::string &s);
address contract_migrate(const std::vector<char> &code, const uint32_t &vmtype, const std::string &name, const std::string &version, const std::string &author, const std::string &email, const std::string &desc);

address self_address(void) {
	address t;
	::ontio_self_address(t.data());
	return t;
}

address caller_address(void) {
	address t;
	::ontio_caller_address(t.data());
	return t;
}

address entry_address(void) {
	address t;
	::ontio_entry_address(t.data());
	return t;
}

std::vector<char>  get_input(void) {
	std::vector<char> result;
	size_t len = ontio_input_length();
	result.resize(len);
	::ontio_get_input(result.data());
	return result;
}

bool check_witness(const address &v) {
	uint32_t auth = ::ontio_check_witness(v.data());
	return bool(auth);
}

H256 current_blockhash(void) {
	H256 t;
	::ontio_current_blockhash(t.data());
	return t;
}

H256 current_txhash(void) {
	H256 t;
	::ontio_current_txhash(t.data());
	return t;
}

template<typename T>
void ontio_return(const T &t) {
	auto data = pack(t);
	::ontio_return(data.data(), data.size()); 
}

void notify(const std::string &s) {
	::ontio_notify(s.data(), s.size());
}

template<typename T>
void call_native(const address &addr, const std::vector<char> &v, T &t) {
	size_t outputlen;
	outputlen = ::ontio_call_contract((void*)addr.data(), v.data(), v.size());

	memset((void *)&t, 0, sizeof(T));
	ontio_assert(outputlen <= sizeof(T), "output length too big to fill return type.");
	::ontio_get_call_output((void *)&t);
}

/* if type T is a vector or map. the length only can get from outputlen. */
template<typename T>
void call_contract(const address &addr, const std::vector<char> &v, T &t) {
	size_t outputlen = ::ontio_call_contract((void*)addr.data(), v.data(), v.size());
	std::vector<char> res;
	res.resize(outputlen);
	::ontio_get_call_output(res.data());
	t = unpack<T>(res);
}

template<typename T>
void get_call_output(T &t) {
	size_t outputlen = ::ontio_call_output_length();
	std::vector<char> res;
	res.resize(outputlen);
	::ontio_get_call_output(res.data());
	t = unpack<T>(res);
}

address contract_migrate(const std::vector<char> &code, const uint32_t &vmtype, const std::string &name, const std::string &version, const std::string &author, const std::string &email, const std::string &desc) {
	address addr;
	size_t len = ::ontio_contract_migrate(code.data(), code.size(), vmtype, name.data(), name.size(), version.data(), version.size(), author.data(), author.size(), email.data(), email.size(), desc.data(), desc.size(), addr.data());
	ontio_assert(len == ADDRLENGTH, "contract_migrateerror. address must be 20 bytes.");
	return addr;
}

/* similar with call_contract, then get the output. the val length also can not assure by smartcontract. like std::vector, std::map. etc. this will get any num element. */
template<typename T>
bool storage_get(const key &key, T &val) {
	std::vector<char> s;
	size_t initlength = pack_size(val);
	s.resize(initlength);
	size_t length = ::ontio_storage_read(key.data(), key.size(), s.data(), s.size(), 0);
	if (length == UINT32_MAX) {
		return false;
	}

	/* if length < initlength. then only ready length to vector s*/
	if (length > initlength) {
		/* bigger then size of serialize T is ok. but can not smaller.*/
		s.resize(length);
		/* not resize change both the size and capacity. so the memory will realloc. should return from offset 0 */
		::ontio_storage_read(key.data(), key.size(), s.data(), s.size(), 0);
	}

	val = unpack<T>(s);
	return true;
}

template<typename T>
void storage_put(const key &key, const T &val) {
	auto data = pack(val);
	::ontio_storage_write(key.data(), key.size(), data.data(), data.size());
}

void storage_delete(const key& key) {
	::ontio_storage_delete(key.data(), key.size());
}

#define make_key pack
}
#else
extern "C" void ontio_debug( const char* cstr, const uint32_t len);

namespace ontio {

using std::vector;
using std::map;
static vector<char> debug_prefix_ontio_args_input_wasm_local_debug;
static vector<char> debug_prefix_ontio_contract_return_wasm_local_debug;
static map<vector<char>, vector<char>> debug_prefix_ontio_storage_debug_map;


address self_address(void) {
	address t;
	return t;
}

address caller_address(void) {
	address t;
	return t;
}

address entry_address(void) {
	address t;
	return t;
}

void set_input(vector<char> input) {
	debug_prefix_ontio_args_input_wasm_local_debug.clear();
	debug_prefix_ontio_args_input_wasm_local_debug = input;
}

vector<char> get_input(void) {
	return debug_prefix_ontio_args_input_wasm_local_debug;
}

bool check_witness(const address &v) {
	return true;
}

H256 current_blockhash(void) {
	H256 t;
	return t;
}

H256 current_txhash(void) {
	H256 t;
	return t;
}

template<typename T>
void ontio_return(const T &t) {
	auto data = pack(t);
	debug_prefix_ontio_contract_return_wasm_local_debug.clear(); 
	debug_prefix_ontio_contract_return_wasm_local_debug = data;
}

void notify(const std::string &s) {
	printf("%s\n", s.c_str());
}

template<typename T>
void get_call_output(T &t) {
	t = unpack<T>(debug_prefix_ontio_contract_return_wasm_local_debug);
}

template<typename T>
void call_native(const address &addr, const std::vector<char> &v, T &t) {
	assert("local test not support multicontract call\n");
}

template<typename T>
void call_contract(const address &addr, const std::vector<char> &v, T &t) {
	assert("local test not support\n");
}

address contract_migrate(const std::vector<char> &code, const uint32_t &vmtype, const std::string &name, const std::string &version, const std::string &author, const std::string &email, const std::string &desc) {
	address t;
	return t;
}

template<typename T>
void storage_put(const key &key, const T &val) {
	auto mval = pack(val);
	//debug_prefix_ontio_storage_debug_map.insert(map<vector<char>,vector<char>>::value_type(key, mval));
	debug_prefix_ontio_storage_debug_map[key] = mval;
}

template<typename T>
bool storage_get(const key &key, T &val) {
	auto iter = debug_prefix_ontio_storage_debug_map.find(key);
	if (iter != debug_prefix_ontio_storage_debug_map.end()) {
		vector<char> s = iter->second;
		val = unpack<T>(s);
		return true;
	}
	else
		return false;
}

void storage_delete(const key& key) {
	debug_prefix_ontio_storage_debug_map.erase(key);
}

#define make_key pack

}
#endif
