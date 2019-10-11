using std::vector;
using std::tuple;
using std::array;

namespace ontio {
enum { 
	ByteArrayType = 0x00,
	StringType = 0x01,
	AddressType = 0x02, 
	BooleanType = 0x03,
	IntType = 0x04,
	H256Type = 0x05,

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
		check(type == ByteArrayType, "NeoByteArray type error");
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
		check(type == AddressType, "NeoAddress type error");
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
		check(type == BooleanType, "NeoBoolean type error");
		return ds;
	}
};


template<typename type>
struct NeoInt {
	type value;
	
	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoInt<type>& v ) {
		uint8_t ttype = IntType;
		int128_t tv = int128_t(v.value);

		ds << ttype;
		ds << tv;

		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoInt<type>& v ) {
		uint8_t ttype;
		int128_t tv;
		ds >> ttype;
		ds >> tv;
		check(ttype == IntType, "NeoInt type error");
		v.value = tv;
		return ds;
	}
};

struct NeoH256 {
	H256 value;
	string tohexstring(void) const {
		string s;
		s.resize(64);
		int index = 0;
		for(auto it = value.begin(); it != value.end(); it++) {
			uint8_t high = *it/16, low = *it%16;
			s[index] = (high<10) ? ('0' + high) : ('a' + high - 10);
			s[index + 1] = (low<10) ? ('0' + low) : ('a' + low - 10);
			index += 2;
		}
		return s;
	}

	void debugout(void) {
		for(auto it = value.begin(); it != value.end(); it++) {
			printf("%02x", *it);
		}
		printf("\n");
	}

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoH256& v ) {
		uint8_t type = H256Type;
		ds << type;
		return ds << v.value;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoH256& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		check(type == H256Type, "H256Type wrong");
		return ds >> v.value;
	}
};

template<typename... Args>
struct NeoList {
	std::tuple<std::decay_t<Args>...> value;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoList& v ) {
		uint8_t type = ListType;
		uint32_t size = sizeof...(Args);
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
		check(type == ListType, "NeoList type error");
		check(size == sizeof...(Args), "NeoList size error");
		return ds >> v.value;
	}
};


}


