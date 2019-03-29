#pragma once
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
	::self_address(t.data());
	return t;
}

address caller_address(void) {
	address t;
	::caller_address(t.data());
	return t;
}

address entry_address(void) {
	address t;
	::entry_address(t.data());
	return t;
}

std::vector<char>  get_input(void) {
	std::vector<char> result;
	size_t len = input_length();
	result.resize(len);
	::get_input(result.data());
	return result;
}

bool check_witness(const address &v) {
	uint32_t auth = ::check_witness(v.data());
	return bool(auth);
}

H256 current_blockhash(void) {
	H256 t;
	::current_blockhash(t.data());
	return t;
}

H256 current_txhash(void) {
	H256 t;
	::current_txhash(t.data());
	return t;
}

template<typename T>
void ret(const T &t) {
	auto data = pack(t);
	::ret(data.data(), data.size()); 
}

void notify(const std::string &s) {
	::notify(s.data(), s.size());
}

template<typename T>
void call_native(const address &addr, const std::vector<char> &v, T &t) {
	size_t outputlen;
	outputlen = ::call_contract((void*)addr.data(), v.data(), v.size());

	memset((void *)&t, 0, sizeof(T));
	ontio_assert(outputlen <= sizeof(T), "output length too big to fill return type.");
	::get_call_output((void *)&t);
}

/* if type T is a vector or map. the length only can get from outputlen. */
template<typename T>
void call_contract(const address &addr, const std::vector<char> &v, T &t) {
	size_t outputlen = ::call_contract((void*)addr.data(), v.data(), v.size());
	std::vector<char> res;
	res.resize(outputlen);
	::get_call_output(res.data());
	t = unpack<T>(res);
}

template<typename T>
void get_call_output(T &t) {
	size_t outputlen = call_output_length();
	std::vector<char> res;
	res.resize(outputlen);
	::get_call_output(res.data());
	t = unpack<T>(res);
}

address contract_migrate(const std::vector<char> &code, const uint32_t &vmtype, const std::string &name, const std::string &version, const std::string &author, const std::string &email, const std::string &desc) {
	address addr;
	size_t len = ::contract_migrate(code.data(), code.size(), vmtype, name.data(), name.size(), version.data(), version.size(), author.data(), author.size(), email.data(), email.size(), desc.data(), desc.size(), addr.data());
	ontio_assert(len == ADDRLENGTH, "contract_migrateerror. address must be 20 bytes.");
	return addr;
}

/* similar with call_contract, then get the output. the val length also can not assure by smartcontract. like std::vector, std::map. etc. this will get any num element. */
template<typename T>
bool storage_get(const key &key, T &val) {
	std::vector<char> s;
	size_t initlength = 32;
	s.resize(initlength);
	size_t length = ::storage_read(key.data(), key.size(), s.data(), s.size(), 0);
	if (length == UINT32_MAX) {
		return false;
	}

	/* if length < initlength. then only ready length to vector s*/
	if (length > initlength) {
		/* bigger then size of serialize T is ok. but can not smaller.*/
		s.resize(length);
		/* not resize change both the size and capacity. so the memory will realloc. should return from offset 0 */
		::storage_read(key.data(), key.size(), s.data(), s.size(), 0);
	}

	val = unpack<T>(s);
	return true;
}

template<typename T>
void storage_put(const key &key, const T &val) {
	auto data = pack(val);
	::storage_write(key.data(), key.size(), data.data(), data.size());
}

void storage_delete(const key& key) {
	::storage_delete(key.data(), key.size());
}

#define make_key pack
}
