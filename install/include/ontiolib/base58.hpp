#pragma once
#include <openssl/sha.h>
namespace ontio {
extern "C" {
bool b58tobin(void *bin, size_t *binszp, const char *b58, size_t b58sz);
bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz);
void  ontio_assert( uint32_t test, const char* msg );
}

void sha256(const std::vector<char> &src , H256 &res) {
    SHA256((const unsigned char *)src.data(), src.size(), res.data());  
}

void hash256(const std::vector<char> &src , H256 &res) {
	H256 t;
	sha256(src, t);
	SHA256((const unsigned char *)t.data(), t.size(), res.data());
}

address Base58toaddress(std::string s) {
	size_t size = 25;
	address addr_t;
	H256 checksum;
	std::vector<char> data;
	data.resize(size);
	b58tobin(data.data(), &size, s.data(), s.size());
	memcpy(addr_t.data(), data.data()+1, addr_t.size());

	std::vector<char> src(data.begin(), data.begin() + 21);
	hash256(src, checksum);

	H256::iterator iter_checsum = checksum.begin();
	std::vector<char>::iterator iter_data = data.begin();

	for (int i = 0; i < 4; i++) {
		uint8_t x = *(iter_checsum + i);
		uint8_t y = *(iter_data + 21 + i);
		ontio_assert(x == y, "address checksum error.");
	}

	ontio_assert(data.data()[0] == 0x17, "address version error.");
	return addr_t;
}
}
