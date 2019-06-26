using std::vector;
using std::tuple;

namespace ontio {
enum { 
	ByteArrayType = 0,
	AddressType, 
	BooleanType,
	UsizeType,
	Int64Type,
	Uint64Type,
	Uint256Type,
	ListType 
};

struct NeoUsize {
	uint32_t value;

	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoUsize& v ) {
		uint8_t type = UsizeType;
		ds << type;
		ds << v.value;
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoUsize& v ) {
		uint8_t type;
		ds >> type;
		assert(type == UsizeType);
		ds >> v.value;
		return ds;
	}
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

struct NeoInt64 {
	int64_t value;
	
	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoInt64& v ) {
		uint8_t type = Int64Type;
		ds << type;
		ds << v.value;
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoInt64& v ) {
		uint8_t type;
		ds >> type;
		ds >> v.value;
		assert(type == Int64Type);
		return ds;
	}

};

struct NeoUint64 {
	uint64_t value;
	
	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoUint64& v ) {
		uint8_t type = Uint64Type;
		ds << type;
		ds << v.value;
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoUint64& v ) {
		uint8_t type;
		ds >> type;
		ds >> v.value;
		assert(type == Uint64Type);
		return ds;
	}
};

struct NeoUint256 : public H256 {
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
	friend DataStream& operator<<( DataStream& ds, const NeoUint256& v ) {
		uint8_t type = Uint256Type;
		ds << type;
		return ds << static_cast<const H256&>(v);
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoUint256& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		assert(type == Uint256Type);
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


