#pragma once
namespace ontio {
extern "C" {
bool b58tobin(void *bin, size_t *binszp, const char *b58, size_t b58sz);
void  ontio_assert( uint32_t test, const char* msg );
}

address Base58toaddress(std::string s) {
	address addr_t;
	memset(addr_t.data(), 0, addr_t.size());
	std::vector<char> data;
	size_t size = 25;
	data.resize(size);
	b58tobin(data.data(), &size, s.data(), s.size());
	memcpy(addr_t.data(), data.data()+1, addr_t.size());
	ontio_assert(data.data()[0] == 0x17, "address version error.");
	return addr_t;
}
}
