#pragma once
#include "service.h"

namespace ontio {
address self_address(void);
address caller_address(void);
address entry_address(void);
std::vector<char> get_input(void);
bool check_witness(address &v);
H256 current_blockhash(void);
H256 current_txhash(void);
void notify(char *msg);
address contract_migrate(std::vector<char> &code, uint32_t vmtype, std::string name, std::string version, std::string author, std::string email, std::string desc);

template<typename T>
void ret(T &t);

template<typename T>
void call_contract(address &addr, std::vector<char> &v, T &t);

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

bool check_witness(address &v) {
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
void ret(T &t) {
	auto data = pack(t);
	::ret(data.data(), data.size()); 
}

void notify(char *msg) {
	::notify(msg, strlen(msg));
}

template<typename T>
void call_native(address &addr, std::vector<char> &v, T &t) {
	size_t outputlen;
	outputlen = ::call_contract((void*)addr.data(), v.data(), v.size());

	memset((void *)&t, 0, sizeof(T));
	ontio_assert(outputlen <= sizeof(T), "output length too big to fill return type.");
	::get_call_output((void *)&t);
}

template<typename T>
void call_contract(address &addr, std::vector<char> &v, T &t) {
	size_t outputlen = ::call_contract((void*)addr.data(), v.data(), v.size());
	std::vector<char> res;
	res.resize(outputlen);
	::get_call_output(res.data());
	t = unpack<T>(res);
}

address contract_migrate(std::vector<char> &code, uint32_t vmtype, std::string name, std::string version, std::string author, std::string email, std::string desc) {
	address addr;
	size_t len = ::contract_migrate(code.data(), code.size(), vmtype, name.data(), name.size(), version.data(), version.size(), author.data(), author.size(), email.data(), email.size(), desc.data(), desc.size(), addr.data());
	ontio_assert(len == ADDRLENGTH, "contract_migrateerror. address must be 20 bytes.");
	return addr;
}

template<typename T>
void storage_write(key key, T &val) {
	auto data = pack(val);
	::storage_write(key.data(), key.size(), data.data(), data.size());
}

void storage_delete(key key) {
	::storage_delete(key.data(), key.size());
}

}
