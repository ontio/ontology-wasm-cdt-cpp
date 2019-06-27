using std::vector;
using std::tuple;
using std::array;

namespace ontio {
enum { 
	ByteArrayType = 0x00,
	AddressType = 0x01, 
	BooleanType = 0x02,
	IntType = 0x03,
	H256Type = 0x04,

	ListType  = 0x10
};

struct NeoByteArray :public std::vector<uint8_t> {
	using vector::vector;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoByteArray& v ) {
		uint8_t type = ByteArrayType;
		uint32_t size = v.size();
		ds << type;
		ds << size;
		ds.write((char *)v.data(), v.size());
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoByteArray& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		ds >> size;
		assert(type == ByteArrayType);
		v.resize(size);
		ds.read((char *) v.data(), v.size() );
		return ds;
	}
};

struct NeoAddress {
	address value;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoAddress& v ) {
		uint8_t type = AddressType;
		ds << type;
		ds << v.value;
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoAddress& v ) {
		uint8_t type;
		ds >> type;
		ds >> v.value;
		assert(type == AddressType);
		return ds;
	}
};

struct NeoBoolean {
	bool value;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoBoolean& v ) {
		uint8_t type = BooleanType;
		ds << type;
		ds << v.value;
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoBoolean& v ) {
		uint8_t type;
		ds >> type;
		ds >> v.value;
		assert(type == BooleanType);
		return ds;
	}
};


template<typename type>
struct NeoInt {
	type value;
	
	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoInt<type>& v ) {
		uint8_t ttype = IntType;
		uint8_t k = 0;
		ds << ttype;

		if (v.value > 0) {
			uint32_t size = (sizeof(type) + 1);
			ds << size;
			ds << v.value;
			ds << k;
		} else if (v.value == 0) {
			uint32_t size = 0;
			ds << size;
		} else {
			uint32_t size = (sizeof(type));
			ds << size;
			ds << v.value;
		}

		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoInt<type>& v ) {
		uint8_t ttype;
		uint32_t size;
		ds >> ttype;
		ds >> size;
		if (size == 0) {
			v.value = 0;
			assert(size == 0);
		} else {
			ds >> v.value;
			assert((size == sizeof(type)) or (size == sizeof(type) + 1));
		}
		assert(ttype == IntType);
		return ds;
	}
};

struct NeoH256: public H256 {
	string tohexstring(void) const {
		string s;
		s.resize(64);
		int index = 0;
		for(auto it = begin(); it != end(); it++) {
			uint8_t high = *it/16, low = *it%16;
			s[index] = (high<10) ? ('0' + high) : ('a' + high - 10);
			s[index + 1] = (low<10) ? ('0' + low) : ('a' + low - 10);
			index += 2;
		}
		return s;
	}

	void debugout(void) {
		for(auto it = begin(); it != end(); it++) {
			printf("%02x", *it);
		}
		printf("\n");
	}

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoH256& v ) {
		uint8_t type = H256Type;
		ds << type;
		return ds << static_cast<const H256&>(v);
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoH256& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		assert(type == H256Type);
		return ds >> static_cast<H256&>(v);
	}
};

template<typename... Args>
struct NeoList {
	std::tuple<std::decay_t<Args>...> value;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoList& v ) {
		uint8_t type = ListType;
		uint32_t size = pack_size(v.value);
		ds << type;
		ds << size;
		return ds << v.value;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoList& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		ds >> size;
		assert(type == ListType);
		assert(ds.remaining() == size);
		return ds >> v.value;
	}
};

}


