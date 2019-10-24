using std::vector;
using std::tuple;
using std::array;

namespace ontio {
namespace ontio_internal_namespace  {
enum { 
	ByteArrayType = 0x00,
	StringType = 0x01,
	AddressType = 0x02, 
	BooleanType = 0x03,
	IntType = 0x04,
	H256Type = 0x05,

	ListType  = 0x10
};

template<typename DataStream>
void notify_type(DataStream &ds, const asset &a) {
	static_assert(sizeof(a) == sizeof(int128_t), "[type size error]: asset should be int128_t");
	uint8_t ttype = IntType;

	ds << ttype;
	ds << a;
}

template<typename DataStream>
void get_notify_type(DataStream &ds, asset &a) {
	static_assert(sizeof(a) == sizeof(int128_t), "[type size error]: asset should be int128_t");
	int128_t tv;
	uint8_t ttype;
	ds >> ttype;
	check(ttype == IntType, "[Type error]: asset expect IntType");
	ds >> a;
}

template<typename DataStream>
void notify_type(DataStream &ds, const std::string &s) {
	uint8_t type = StringType;
	uint32_t size = s.size();
	ds << type;
	ds << size;
	ds.write((char *)s.data(), s.size());
}

template<typename DataStream>
void get_notify_type(DataStream &ds, std::string &s) {
	uint8_t type = 0xff; //never right type;
	uint32_t size = 0;
	ds >> type; check(type == ByteArrayType, "[Type error]: string expect ByteArrayType");
	ds >> size;
	if (size != 0) {
		std::vector<char> tmp;
		tmp.resize(size);
		ds.read((char *)tmp.data(), tmp.size());
		s = std::string(tmp.data(),tmp.data()+tmp.size());
	}
	else
		s = std::string();
}

// fixed string like call("transfer");
template<typename DataStream, typename T, std::size_t N, std::enable_if_t<std::is_same<T,char>::value>* = nullptr>
void notify_type( DataStream& ds, const T (&v)[N] ) {
	uint8_t type = StringType;
	uint32_t size = N - 1;
	ds << type;
	ds << size ;
	// con not ds >> v. size has N - 1. delete the '\0' byte.
	ds.write((char *)v, size);
}

template<typename DataStream>
void notify_type(DataStream &ds, const address &t) {
	uint8_t type = AddressType;
	ds << type;
	ds << t;
}

template<typename DataStream>
void get_notify_type(DataStream &ds, address &t) {
	uint8_t type = 0xff; //never right type;
	uint32_t size = 0;
	ds >> type; check(type == ByteArrayType, "[Type error]: address expect ByteArrayType");
	ds >> size; check(size == 20, "[Address len error]: expect 20 Bytes");
	ds >> t;
}

template<typename DataStream>
void notify_type(DataStream &ds, const std::vector<uint8_t> &v) {
	uint8_t type = ByteArrayType;
	uint32_t size = v.size();
	ds << type;
	ds << size;
	ds.write((char *)v.data(), v.size());
}

template<typename DataStream>
void get_notify_type(DataStream &ds, std::vector<uint8_t> &t) {
	uint8_t type = 0xff; //never right type;
	uint32_t size = 0;
	ds >> type; check(type == ByteArrayType, "[Type error]: vector<uint8_t> expect ByteArrayType");
	ds >> size;
	t.resize(size);
	ds.read((char *)t.data(),t.size());
}

template<typename DataStream>
void notify_type(DataStream &ds, const std::vector<char> &v) {
	uint8_t type = ByteArrayType;
	uint32_t size = v.size();
	ds << type;
	ds << size;
	ds.write((char *)v.data(), v.size());
}

template<typename DataStream>
void get_notify_type(DataStream &ds, std::vector<char> &t) {
	uint8_t type = 0xff; //never right type;
	uint32_t size = 0;
	ds >> type; check(type == ByteArrayType, "[Type error]: vector<uint8_t> expect ByteArrayType");
	ds >> size;
	t.resize(size);
	ds.read((char *)t.data(),t.size());
}

template<typename DataStream>
void notify_type(DataStream &ds, const bool &v) {
	uint8_t type = BooleanType;
	ds << type;
	ds << v;
}

template<typename DataStream>
void get_notify_type(DataStream &ds, bool &v) {
	uint8_t type = 0xff;
	ds >> type; check(type == BooleanType, "[Type error]: bool expect BooleanType");
	ds >> v;
}

template<typename DataStream>
void notify_type(DataStream &ds, const H256 &v) {
	uint8_t type = H256Type;
	ds << type;
	ds << v;
}

template<typename DataStream>
void get_notify_type(DataStream &ds, H256 &v) {
	uint8_t type = 0xff;
	uint32_t size = 0;
	// wasm call neovm consider return val address/string/h256/bytearray to ByteArrayType.
	ds >> type; check(type == ByteArrayType, "[Type error]: H256 expect ByteArrayType");
	ds >> size; check(size == 32, "[Size error]: H256 should be 32 Bytes.");
	ds >> v;
}

template<typename DataStream, typename... Args>
void notify_type(DataStream &ds,const std::tuple<Args...>& t) {
	uint8_t type = ListType;
	uint32_t size = sizeof...(Args);
	ds << type;
	ds << size;
	boost::fusion::for_each( t, [&]( const auto& i ) {
       notify_type(ds, i);
	});
}

template<typename DataStream, typename T>
void notify_type(DataStream &ds,const std::vector<T>& v) {
	uint8_t type = ListType;
	uint32_t size = v.size();
	ds << type;
	ds << size;
	for( const auto& i : v )
		notify_type(ds, i);
}

template<typename DataStream, typename T>
void notify_type(DataStream &ds,const std::list<T>& l) {
	uint8_t type = ListType;
	uint32_t size = l.size();
	ds << type;
	ds << size;
	for( const auto& i : l )
		notify_type(ds, i);
}

template<typename DataStream, typename T>
void notify_type(DataStream &ds,const std::deque<T>& q) {
	uint8_t type = ListType;
	uint32_t size = q.size();
	ds << type;
	ds << size;
	for( const auto& i : q )
		notify_type(ds, i);
}

template<typename DataStream, typename T1, typename T2>
void notify_type(DataStream &ds,const std::pair<T1, T2>& t) {
	uint8_t type = ListType;
	uint32_t size = 2;
	ds << type;
	ds << size;
	notify_type(ds, std::get<0>(t));
	notify_type(ds, std::get<1>(t));
}

template<typename DataStream, typename T>
void notify_type(DataStream &ds,const std::set<T>& t) {
	uint8_t type = ListType;
	uint32_t size = t.size();
	ds << type;
	ds << size;
	for( const auto& i : t ) {
		notify_type(ds, i);
	}
}

// if T is char. call("transfer") will treat as T[N]
template<typename DataStream, typename T, std::size_t N,
        std::enable_if_t<! std::is_same<T, char>::value &&
                         !_datastream_detail::is_pointer<T>()>* = nullptr>
void notify_type( DataStream& ds, const T (&v)[N] ) {
	uint8_t type = ListType;
	uint32_t size = N;
	ds << type;
	ds << size;

   for( uint32_t i = 0; i < N; ++i )
	   notify_type(ds, v[i]);
}


template<typename DataStream, typename T, std::size_t N>
void notify_type( DataStream& ds, const std::array<T,N>& v ) {
	uint8_t type = ListType;
	uint32_t size = N;
	ds << type;
	ds << size;

   for( const auto& i : v )
	   notify_type(ds, i);
}

template<typename DataStream, typename T>
void notify_type(DataStream &ds,const T &value ,std::enable_if_t<std::is_integral<T>::value>* = nullptr) {
	uint8_t ttype = IntType;
	int128_t tv = int128_t(value);

	ds << ttype;
	ds << tv;
}

template<typename T>
bool check_integer_overflow(const int128_t &value, T &com, std::enable_if_t<std::is_integral<T>::value>* = nullptr) {
	if (value > std::numeric_limits<T>::max() || value < std::numeric_limits<T>::min()) {
		return false;
	}

	return true;
}

template<typename DataStream, typename T>
void get_notify_type(DataStream &ds, T &value ,std::enable_if_t<std::is_integral<T>::value>* = nullptr) {
	static_assert(sizeof(T) <= sizeof(int128_t), "[use type error]: can only use max typesize int128_t");
	uint8_t ttype = 0;
	int128_t tv;
	ds >> ttype;
	check(ttype == IntType, "NeoInt type error");
	ds >> tv;
	check(check_integer_overflow(tv, value), "[interger overflow error]: should pass the bigger size type, like int128_t.");
	value = (T)tv;
}

template<typename DataStream, typename T, std::enable_if_t<_datastream_detail::is_pointer<T>()>* = nullptr>
void notify_type(DataStream &ds, T) {
	static_assert(!_datastream_detail::is_pointer<T>(), "Pointers should not be notified");
}

template<typename DataStream, typename T>
void notify_event_buff(DataStream &ds, const T &arg0) {
	notify_type(ds,arg0);
}

template<typename DataStream, typename T, typename... Args>
void notify_event_buff(DataStream &ds, const T &arg0, Args&&... args) {
	notify_type(ds,arg0);
	notify_event_buff(ds, args...);
}

template<typename... Args>
size_t notify_size(Args&&... args) {
  datastream<size_t> ds;
  notify_event_buff(ds,args...);
  return ds.tellp();
}

template<typename T>
void unpack_neoargs(const std::vector<char>& v, T &t) {
   uint8_t magic_version;
   datastream<const char*> ds(v.data(),v.size());
   ds >> magic_version;
   check(magic_version == 0, "result version error");
   get_notify_type(ds, t);
}
}

template<typename... Args>
void notify_event(Args&&... args) {
  uint8_t ltype = ontio_internal_namespace::ListType;
  uint32_t lsize = sizeof...(args);
  size_t size = ontio_internal_namespace::notify_size(args...);
  size = size + ontio_internal_namespace::pack_size(uint32_t(0),ltype, lsize); // for magic "evt\x00" and one byte ArgTListType
  std::vector<char> result;
  result.resize(size);
  datastream<char*> ds( result.data(), result.size());

  ds << uint8_t(0x65);
  ds << uint8_t(0x76);
  ds << uint8_t(0x74);
  ds << uint8_t(0x00);
  ds << ltype;
  ds << lsize;

  ontio_internal_namespace::notify_event_buff(ds, args...);
  notify(result);
}

template<typename... Args>
std::vector<char> pack_neoargs(Args&&... args) {
  uint8_t magic_version = 0;
  uint8_t ltype = ontio_internal_namespace::ListType;
  uint32_t lsize = sizeof...(args);
  size_t size = ontio_internal_namespace::notify_size(args...);
  size = size + ontio_internal_namespace::pack_size(magic_version,ltype, lsize); // for magic "evt\x00" and one byte ArgTListType
  std::vector<char> result;
  result.resize(size);
  datastream<char*> ds( result.data(), result.size());

  ds << magic_version;
  ds << ltype;
  ds << lsize;

  ontio_internal_namespace::notify_event_buff(ds, args...);
  return result;
}

/*
struct NeoString {
	std::string s;
	template<typename DataStream>
	friend DataStream& operator<<( DataStream& ds, const NeoString& v ) {
		uint8_t type = StringType;
		uint32_t size = v.s.size();
		ds << type;
		ds << size;
		ds.write((char *)v.s.data(), v.s.size());
		return ds;
	}

	template<typename DataStream>
	friend DataStream& operator>>( DataStream& ds, NeoString& v ) {
		uint8_t type;
		uint32_t size;
		ds >> type;
		ds >> size;
		check(type == StringType, "NeoString type error");
		v.s.resize(size);
		ds.read((char *) v.s.data(), v.s.size() );
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
*/
}


