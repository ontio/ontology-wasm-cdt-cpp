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

template<typename T, std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value>* = nullptr>
T integer_from_littlesbytes(const uint8_t* bytes, size_t size) {
	T retval;
	check(size > 0 && size <= sizeof(T), "[Size error]: integer size error. pass a larger size integer result. like the int128_t(the max)");
	if (bytes[size - 1] >> 7) {
		retval = -1;
	} else {
		retval = 0;
	}
	memcpy((void *)&retval, (void *)bytes, size);

	return retval;
}

template<typename T, std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>* = nullptr>
T integer_from_littlesbytes(const uint8_t* bytes, size_t size) {
	T retval = 0;
	check(size > 0 && size <= sizeof(T), "[Size error]: integer size error. pass a larger size integer result. like the int128_t(the max)");
	check(bytes[size - 1]>> 7 == 0, "[Signed error]: can not read negtive value read into a unsigned type");
	memcpy((void *)&retval, (void *)bytes, size);

	return retval;
}

template<typename DataStream, typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
void notify_type(DataStream &ds,const T &value) {
	uint8_t ttype = IntType;
	int128_t tv = int128_t(value);

	ds << ttype;
	ds << tv;
}

template<typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
bool check_integer_overflow(const int128_t &value, T &com) {
	if (value > std::numeric_limits<T>::max() || value < std::numeric_limits<T>::min()) {
		return false;
	}

	return true;
}

template<typename DataStream, typename T, std::enable_if_t<std::is_integral<T>::value>* = nullptr>
void get_notify_type(DataStream &ds, T &value) {
	static_assert(sizeof(T) <= sizeof(int128_t), "[use type error]: can only use max typesize int128_t");
	uint8_t ttype = 0;
	int128_t tv;
	ds >> ttype;
	if (ttype == IntType) {
		ds >> tv;
		check(check_integer_overflow(tv, value), "[interger overflow error]: should pass the bigger size type, like int128_t.");
		value = (T)tv;
	}  else if (ttype == ByteArrayType) {
		std::vector<char> tmp;
		uint32_t size = 0;
		ds >> size;
		tmp.resize(size);
		ds.read((char *)tmp.data(), tmp.size());
		value = integer_from_littlesbytes<T>((uint8_t*)tmp.data(), tmp.size());
	} else {
		check(false, "[Type error]: integer expect IntType or ByteArrayType");
	}
}

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
	ds >> type;
	if (type == BooleanType)
		ds >> v;
	else if (type == ByteArrayType) {
		uint8_t tmp = 0xff;
		uint32_t size = 0;
		ds >> size;
		check(size == 1, "[Size error]: bool expect only one byte");
		ds >> tmp;
		if (tmp == 0) {
			v = false;
		} else if (tmp == 1) {
			v = true;
		} else {
			check(false, "[Value error]: bool expect ByteArrayType 1 or 0");
		}
	} else if (type == IntType) {
		int128_t tv;
		ds >> tv;
		if (tv == 0) {
			v = false;	
		} else if (tv == 1) {
			v = true;
		} else {
			check(false, "[Value error]: bool expect IntType 1 or 0");
		}
	} else {
		 check(false, "[Type error]: bool expect BooleanType/ByteArrayType/IntType");
	}
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

template<typename DataStream, typename... Args>
void get_notify_type(DataStream &ds,std::tuple<Args...>& t) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: tuple expect ListType");
	ds >> size; check(size == sizeof...(Args), "[Size error]: tuple not match list size");

	boost::fusion::for_each( t, [&]( auto& i ) {
		get_notify_type(ds, i);
	});
}

template<typename DataStream, typename T, std::enable_if_t<!(std::is_same<T,char>::value || std::is_same<T,uint8_t>::value)>* = nullptr>
void notify_type(DataStream &ds,const std::vector<T>& v) {
	uint8_t type = ListType;
	uint32_t size = v.size();
	ds << type;
	ds << size;
	for( const auto& i : v )
		notify_type(ds, i);
}

template<typename DataStream, typename T, std::enable_if_t<!(std::is_same<T,char>::value || std::is_same<T,uint8_t>::value)>* = nullptr>
void get_notify_type(DataStream &ds, std::vector<T> &v) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: vector expect ListType");
	ds >> size;
	v.resize(size);
	for (auto &elt: v) {
		get_notify_type(ds, elt);
	}
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
void get_notify_type(DataStream &ds,std::list<T>& l) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: list expect ListType");
	ds >> size;
	l.resize(size);

	for( auto& elt : l )
		get_notify_type(ds, elt);
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

template<typename DataStream, typename T>
void get_notify_type(DataStream &ds,std::deque<T>& q) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: deque expect ListType");
	ds >> size;
	q.resize(size);

	for( auto& elt : q )
		get_notify_type(ds, elt);
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

template<typename DataStream, typename T1, typename T2>
void get_notify_type(DataStream &ds,std::pair<T1, T2>& t) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: pair expect ListType");
	ds >> size; check(size == 2, "[Size error]: pair can only have 2 elt");

	T1 t1;
	T2 t2;
	get_notify_type(ds, t1);
	get_notify_type(ds, t2);
	t = std::pair<T1, T2>{t1, t2};
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

template<typename DataStream, typename T>
void get_notify_type(DataStream &ds,std::set<T>& s) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: set expect ListType");
	ds >> size;

	for( uint32_t i = 0; i < size; ++i ) {
		T v;
		get_notify_type(ds, v);
		s.emplace( std::move(v) );
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

template<typename DataStream, typename T, std::size_t N>
void get_notify_type( DataStream& ds, std::array<T,N>& v ) {
	uint8_t type;
	uint32_t size;
	ds >> type; check(type == ListType, "[Type error]: array expect ListType");
	ds >> size; check(size == N, "[Size error]: array[N] size error");


	for( uint32_t i = 0; i < N; ++i ) {
		T x;
		get_notify_type(ds, x);
		v[i] = std::move(x);
	}
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

}


