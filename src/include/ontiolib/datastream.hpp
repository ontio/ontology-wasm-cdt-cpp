/**
 *  @file datastream.hpp
 *  @copyright defined in ont/LICENSE
 */
#pragma once
#include "system.hpp"
#include "fixed_bytes.hpp"
#include "varint.hpp"

#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <list>
#include <queue>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <optional>
#include <variant>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

#include <boost/pfr.hpp>

namespace ontio {

/**
 *  A data stream for reading and writing data in the form of bytes
 *
 *  @tparam T - Type of the datastream buffer
 */
template<typename T>
class datastream {
   public:
      /**
       * @brief Construct a new datastream object
       *
       * @details Construct a new datastream object given the size of the buffer and start position of the buffer
       * @param start - The start position of the buffer
       * @param s - The size of the buffer
       */
      datastream( T start, size_t s )
      :_start(start),_pos(start),_end(start+s){}

     /**
      *  Skips a specified number of bytes from this stream
      *
      *  @param s - The number of bytes to skip
      */
      inline void skip( size_t s ){ _pos += s; }

     /**
      *  Reads a specified number of bytes from the stream into a buffer
      *
      *  @param d - The pointer to the destination buffer
      *  @param s - the number of bytes to read
      *  @return true
      */
      inline bool read( char* d, size_t s ) {
        ontio::check( size_t(_end - _pos) >= (size_t)s, "read" );
        memcpy( d, _pos, s );
        _pos += s;
        return true;
      }

     /**
      *  Writes a specified number of bytes into the stream from a buffer
      *
      *  @param d - The pointer to the source buffer
      *  @param s - The number of bytes to write
      *  @return true
      */
      inline bool write( const char* d, size_t s ) {
        ontio::check( _end - _pos >= (int32_t)s, "write" );
        memcpy( (void*)_pos, d, s );
        _pos += s;
        return true;
      }

     /**
      *  Writes a byte into the stream
      *
      *  @brief Writes a byte into the stream
      *  @param c byte to write
      *  @return true
      */
      inline bool put(char c) {
        ontio::check( _pos < _end, "put" );
        //*_pos = c;
	/*if use *_pos. when T is const char * will put error. *_pos = c will error.*/
	write(&c, 1);
        //++_pos;
        return true;
      }

     /**
      *  Reads a byte from the stream
      *
      *  @brief Reads a byte from the stream
      *  @param c - The reference to destination byte
      *  @return true
      */
      inline bool get( unsigned char& c ) { return get( *(char*)&c ); }

     /**
      *  Reads a byte from the stream
      *
      *  @brief Reads a byte from the stream
      *  @param c - The reference to destination byte
      *  @return true
      */
      inline bool get( char& c )
      {
        ontio::check( _pos < _end, "get" );
        c = *_pos;
        ++_pos;
        return true;
      }

     /**
      *  Retrieves the current position of the stream
      *
      *  @brief Retrieves the current position of the stream
      *  @return T - The current position of the stream
      */
      T pos()const { return _pos; }
      inline bool valid()const { return _pos <= _end && _pos >= _start;  }

     /**
      *  Sets the position within the current stream
      *
      *  @brief Sets the position within the current stream
      *  @param p - The offset relative to the origin
      *  @return true if p is within the range
      *  @return false if p is not within the rawnge
      */
      inline bool seekp(size_t p) { _pos = _start + p; return _pos <= _end; }

     /**
      *  Gets the position within the current stream
      *
      *  @brief Gets the position within the current stream
      *  @return p - The position within the current stream
      */
      inline size_t tellp()const      { return size_t(_pos - _start); }

     /**
      *  Returns the number of remaining bytes that can be read/skipped
      *
      *  @brief Returns the number of remaining bytes that can be read/skipped
      *  @return size_t - The number of remaining bytes
      */
      inline size_t remaining()const  { return _end - _pos; }
    private:
      /**
       * The start position of the buffer
       *
       * @brief The start position of the buffer
       */
      T _start;
      /**
       * The current position of the buffer
       *
       * @brief The current position of the buffer
       */
      T _pos;
      /**
       * The end position of the buffer
       *
       * @brief The end position of the buffer
       */
      T _end;
};

/**
 * @brief Specialization of datastream used to help determine the final size of a serialized value.
 * Specialization of datastream used to help determine the final size of a serialized value
 */
template<>
class datastream<size_t> {
   public:
      /**
       * Construct a new specialized datastream object given the initial size
       *
       * @brief Construct a new specialized datastream object
       * @param init_size - The initial size
       */
     datastream( size_t init_size = 0):_size(init_size){}

     /**
      *  Increment the size by s. This behaves the same as write( const char* ,size_t s ).
      *
      *  @brief Increase the size by s
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     skip( size_t s )                 { _size += s; return true;  }

     /**
      *  Increment the size by s. This behaves the same as skip( size_t s )
      *
      *  @brief Increase the size by s
      *  @param s - The amount of size to increase
      *  @return true
      */
     inline bool     write( const char* ,size_t s )  { _size += s; return true;  }

     /**
      *  Increment the size by one
      *
      *  @brief Increase the size by one
      *  @return true
      */
     inline bool     put(char )                      { ++_size; return  true;    }

     /**
      *  Check validity. It's always valid
      *
      *  @brief Check validity
      *  @return true
      */
     inline bool     valid()const                     { return true;              }

     /**
      * Set new size
      *
      * @brief Set new size
      * @param p - The new size
      * @return true
      */
     inline bool     seekp(size_t p)                  { _size = p;  return true;  }

     /**
      * Get the size
      *
      * @brief Get the size
      * @return size_t - The size
      */
     inline size_t   tellp()const                     { return _size;             }

     /**
      * Always returns 0
      *
      * @brief Always returns 0
      * @return size_t - 0
      */
     inline size_t   remaining()const                 { return 0;                 }
  private:
     /**
      * The size used to determine the final size of a serialized value.
      *
      * @brief The size used to determine the final size of a serialized value.
      */
     size_t _size;
};

/**
 *  Serialize an std::list into a stream
 *
 *  @brief Serialize an std::list 
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::list<T>& l) {
   ds << unsigned_int( l.size() );
   for ( auto elem : l )
      ds << elem;
  return ds;
}

/**
 *  Deserialize an std::list from a stream
 *
 *  @brief Deserialize an std::list
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::list<T>& l) {
   unsigned_int s;
   ds >> s;
   l.resize(s.value);
   for( auto& i : l )
      ds >> i;
   return ds;
}

/**
 *  Serialize an std::deque into a stream
 *
 *  @brief Serialize an std::queue 
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::deque<T>& d) {
   ds << unsigned_int( d.size() );
   for ( auto elem : d )
      ds << elem;
  return ds;
}

/**
 *  Deserialize an std::deque from a stream
 *
 *  @brief Deserialize an std::deque
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::deque<T>& d) {
   unsigned_int s;
   ds >> s;
   d.resize(s.value);
   for( auto& i : d )
      ds >> i;
   return ds;
}

/**
 *  Serialize an std::variant into a stream
 *
 *  @brief Serialize an std::variant
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::variant<Ts...>& var) {
  unsigned_int index = var.index();
  ds << index;
  std::visit([&ds](auto& val){ ds << val; }, var);
  return ds;
}

template<int I, typename Stream, typename... Ts>
void deserialize(datastream<Stream>& ds, std::variant<Ts...>& var, int i) {
   if constexpr (I < std::variant_size_v<std::variant<Ts...>>) {
      if (i == I) {
         std::variant_alternative_t<I, std::variant<Ts...>> tmp;
         ds >> tmp;
         var = std::move(tmp);
      } else {
         deserialize<I+1>(ds,var,i);
      }
   } else {
      ontio::check(false, "invalid variant index");
   }
}

/**
 *  Deserialize an std::variant from a stream
 *
 *  @brief Deserialize an std::variant
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename... Ts>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::variant<Ts...>& var) {
  unsigned_int index;
  ds >> index;
  deserialize<0>(ds,var,index);
  return ds;
}

/**
 *  Serialize an std::pair
 *
 *  @brief Serialize an std::pair
 *  @param ds - The stream to write
 *  @param t - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T1, typename T2>
DataStream& operator<<( DataStream& ds, const std::pair<T1, T2>& t ) {
   ds << std::get<0>(t);
   ds << std::get<1>(t);
   return ds;
}

/**
 *  Deserialize an std::pair
 *
 *  @brief Deserialize an std::pair
 *  @param ds - The stream to read
 *  @param t - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T1, typename T2>
DataStream& operator>>( DataStream& ds, std::pair<T1, T2>& t ) {
   T1 t1;
   T2 t2;
   ds >> t1;
   ds >> t2;
   t = std::pair<T1, T2>{t1, t2};
   return ds;
}

/**
 *  Serialize an optional into a stream
 *
 *  @brief Serialize an optional
 *  @param ds - The stream to write
 *  @param opt - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const std::optional<T>& opt) {
  char valid = opt.has_value();
  ds << valid;
  if (valid)
     ds << *opt;
  return ds;
}

/**
 *  Deserialize an optional from a stream
 *
 *  @brief Deserialize an optional
 *  @param ds - The stream to read
 *  @param opt - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, typename T>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, std::optional<T>& opt) {
  char valid = 0;
  ds >> valid;
  if (valid) {
     T val;
     ds >> val;
     opt = val;
  }
  return ds;
}

/**
 *  Serialize a fixed_bytes into a stream
 *
 *  @brief Serialize a fixed_bytes
 *  @param ds - The stream to write
 *  @param d - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, size_t Size>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const fixed_bytes<Size>& d) {
   auto arr = d.extract_as_byte_array();
   ds.write( (const char*)arr.data(), arr.size() );
   return ds;
}

/**
 *  Deserialize a fixed_bytes from a stream
 *
 *  @brief Deserialize a fixed_bytes
 *  @param ds - The stream to read
 *  @param d - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream, size_t Size>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, fixed_bytes<Size>& d) {
   std::array<uint8_t, Size> arr;
   ds.read( (char*)arr.data(), arr.size() );
   d = fixed_bytes<Size>( arr );
   return ds;
}

/**
 *  Serialize a bool into a stream
 *
 *  @brief  Serialize a bool into a stream
 *  @param ds - The stream to read
 *  @param d - The value to serialize
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
inline datastream<Stream>& operator<<(datastream<Stream>& ds, const bool& d) {
  return ds << uint8_t(d);
}

/**
 *  Deserialize a bool from a stream
 *
 *  @brief Deserialize a bool
 *  @param ds - The stream to read
 *  @param d - The destination for deserialized value
 *  @tparam Stream - Type of datastream buffer
 *  @return datastream<Stream>& - Reference to the datastream
 */
template<typename Stream>
inline datastream<Stream>& operator>>(datastream<Stream>& ds, bool& d) {
  uint8_t t;
  ds >> t;
  d = t;
  return ds;
}



/**
 *  Serialize a string into a stream
 *
 *  @brief Serialize a string
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream>
DataStream& operator << ( DataStream& ds, const std::string& v ) {
   ds << unsigned_int( v.size() );
   if (v.size())
      ds.write(v.data(), v.size());
   return ds;
}

/**
 *  Deserialize a string from a stream
 *
 *  @brief Deserialize a string
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream>
DataStream& operator >> ( DataStream& ds, std::string& v ) {
   std::vector<char> tmp;
   ds >> tmp;
   if( tmp.size() )
      v = std::string(tmp.data(),tmp.data()+tmp.size());
   else
      v = std::string();
   return ds;
}

/**
 *  Serialize a fixed size std::array
 *
 *  @brief Serialize a fixed size std::array
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N>
DataStream& operator << ( DataStream& ds, const std::array<T,N>& v ) {
   for( const auto& i : v )
      ds << i;
   return ds;
}


/**
 *  Deserialize a fixed size std::array
 *
 *  @brief Deserialize a fixed size std::array
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N>
DataStream& operator >> ( DataStream& ds, std::array<T,N>& v ) {
   for( auto& i : v )
      ds >> i;
   return ds;
}

namespace _datastream_detail {
   /**
    * Check if type T is a pointer
    *
    * @brief Check if type T is a pointer
    * @tparam T - The type to be checked
    * @return true if T is a pointer
    * @return false otherwise
    */
   template<typename T>
   constexpr bool is_pointer() {
      return std::is_pointer<T>::value ||
             std::is_null_pointer<T>::value ||
             std::is_member_pointer<T>::value;
   }

   /**
    * Check if type T is a primitive type
    *
    * @brief Check if type T is a primitive type
    * @tparam T - The type to be checked
    * @return true if T is a primitive type
    * @return false otherwise
    */
   template<typename T>
   constexpr bool is_primitive() {
      return std::is_arithmetic<T>::value ||
             std::is_enum<T>::value;
   }
}

/**
 *  Pointer should not be serialized, so this function will always throws an error
 *
 *  @brief Deserialize a a pointer
 *  @param ds - The stream to read
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the pointer
 *  @return DataStream& - Reference to the datastream
 *  @post Throw an exception if it is a pointer
 */
template<typename DataStream, typename T, std::enable_if_t<_datastream_detail::is_pointer<T>()>* = nullptr>
DataStream& operator >> ( DataStream& ds, T ) {
   static_assert(!_datastream_detail::is_pointer<T>(), "Pointers should not be serialized" );
   return ds;
}

/**
 *  Serialize a fixed size C array of non-primitive and non-pointer type
 *
 *  @brief Serialize a fixed size C array of non-primitive and non-pointer type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the pointer
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
DataStream& operator << ( DataStream& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   for( uint32_t i = 0; i < N; ++i )
      ds << v[i];
   return ds;
}

/**
 *  Serialize a fixed size C array of primitive type
 *
 *  @brief Serialize a fixed size C array of primitive type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the pointer
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
DataStream& operator << ( DataStream& ds, const T (&v)[N] ) {
   ds << unsigned_int( N );
   ds.write((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Deserialize a fixed size C array of non-primitive and non-pointer type
 *
 *  @brief Deserialize a fixed size C array of non-primitive and non-pointer type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N,
         std::enable_if_t<!_datastream_detail::is_primitive<T>() &&
                          !_datastream_detail::is_pointer<T>()>* = nullptr>
DataStream& operator >> ( DataStream& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   ontio::check( N == s.value, "T[] size and unpacked size don't match");
   for( uint32_t i = 0; i < N; ++i )
      ds >> v[i];
   return ds;
}

/**
 *  Deserialize a fixed size C array of primitive type
 *
 *  @brief Deserialize a fixed size C array of primitive type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam T - Type of the object contained in the array
 *  @tparam N - Size of the array
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::size_t N,
         std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
DataStream& operator >> ( DataStream& ds, T (&v)[N] ) {
   unsigned_int s;
   ds >> s;
   ontio::check( N == s.value, "T[] size and unpacked size don't match");
   ds.read((char*)&v[0], sizeof(v));
   return ds;
}

/**
 *  Serialize a vector of char
 *
 *  @brief Serialize a vector of char
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream>
DataStream& operator << ( DataStream& ds, const std::vector<char>& v ) {
   ds << unsigned_int( v.size() );
   ds.write( v.data(), v.size() );
   return ds;
}

/**
 *  Serialize a vector
 *
 *  @brief Serialize a vector
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the vector
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
DataStream& operator << ( DataStream& ds, const std::vector<T>& v ) {
   ds << unsigned_int( v.size() );
   for( const auto& i : v )
      ds << i;
   return ds;
}

/**
 *  Deserialize a vector of char
 *
 *  @brief Deserialize a vector of char
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream>
DataStream& operator >> ( DataStream& ds, std::vector<char>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize( s.value );
   ds.read( v.data(), v.size() );
   return ds;
}

/**
 *  Deserialize a vector
 *
 *  @brief Deserialize a vector
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the vector
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
DataStream& operator >> ( DataStream& ds, std::vector<T>& v ) {
   unsigned_int s;
   ds >> s;
   v.resize(s.value);
   for( auto& i : v )
      ds >> i;
   return ds;
}

/**
 *  Serialize a set
 *
 *  @brief Serialize a set
 *  @param ds - The stream to write
 *  @param s - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the set
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
DataStream& operator << ( DataStream& ds, const std::set<T>& s ) {
   ds << unsigned_int( s.size() );
   for( const auto& i : s ) {
      ds << i;
   }
   return ds;
}


/**
 *  Deserialize a set
 *
 *  @brief Deserialize a set
 *  @param ds - The stream to read
 *  @param s - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the object contained in the set
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
DataStream& operator >> ( DataStream& ds, std::set<T>& s ) {
   s.clear();
   unsigned_int sz; ds >> sz;

   for( uint32_t i = 0; i < sz.value; ++i ) {
      T v;
      ds >> v;
      s.emplace( std::move(v) );
   }
   return ds;
}

/**
 *  Serialize a map
 *
 *  @brief Serialize a map
 *  @param ds - The stream to write
 *  @param m - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename K, typename V>
DataStream& operator << ( DataStream& ds, const std::map<K,V>& m ) {
   ds << unsigned_int( m.size() );
   for( const auto& i : m ) {
      ds << i.first << i.second;
   }
   return ds;
}

/**
 *  Deserialize a map
 *
 *  @brief Deserialize a map
 *  @param ds - The stream to read
 *  @param m - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam K - Type of the key contained in the map
 *  @tparam V - Type of the value contained in the map
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename K, typename V>
DataStream& operator >> ( DataStream& ds, std::map<K,V>& m ) {
   m.clear();
   unsigned_int s; ds >> s;

   for (uint32_t i = 0; i < s.value; ++i) {
      K k; V v;
      ds >> k >> v;
      m.emplace( std::move(k), std::move(v) );
   }
   return ds;
}


/**
 *  Serialize a flat_set
 *
 *  @brief Serialize a flat_set
 *  @param ds - The stream to write
 *  @param s - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the value contained in the flat_set
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
[[ deprecated ]]
DataStream& operator << ( DataStream& ds, const boost::container::flat_set<T>& s ) {
   ds << unsigned_int( s.size() );
   for( const auto& i : s ) {
      ds << i;
   }
   return ds;
}

/**
 *  Deserialize a flat_set
 *
 *  @brief Deserialize a flat_set
 *  @param ds - The stream to read
 *  @param s - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the value contained in the flat_set
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T>
[[ deprecated ]]
DataStream& operator >> ( DataStream& ds, boost::container::flat_set<T>& s ) {
   s.clear();
   unsigned_int sz; ds >> sz;

   for( uint32_t i = 0; i < sz.value; ++i ) {
      T v;
      ds >> v;
      s.emplace( std::move(v) );
   }
   return ds;
}


/**
 *  Serialize a flat map
 *
 *  @brief Serialize a flat map
 *  @param ds - The stream to write
 *  @param m - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam K - Type of the key contained in the flat map
 *  @tparam V - Type of the value contained in the flat map
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename K, typename V>
[[ deprecated ]]
DataStream& operator<<( DataStream& ds, const boost::container::flat_map<K,V>& m ) {
   ds << unsigned_int( m.size() );
   for( const auto& i : m )
      ds << i.first << i.second;
   return ds;
}

/**
 *  Deserialize a flat map
 *
 *  @brief Deserialize a flat map
 *  @param ds - The stream to read
 *  @param m - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam K - Type of the key contained in the flat map
 *  @tparam V - Type of the value contained in the flat map
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename K, typename V>
[[ deprecated ]]
DataStream& operator>>( DataStream& ds, boost::container::flat_map<K,V>& m ) {
   m.clear();
   unsigned_int s; ds >> s;

   for( uint32_t i = 0; i < s.value; ++i ) {
      K k; V v;
      ds >> k >> v;
      m.emplace( std::move(k), std::move(v) );
   }
   return ds;
}

/**
 *  Serialize a tuple
 *
 *  @brief Serialize a tuple
 *  @param ds - The stream to write
 *  @param t - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename... Args>
DataStream& operator<<( DataStream& ds, const std::tuple<Args...>& t ) {
   boost::fusion::for_each( t, [&]( const auto& i ) {
       ds << i;
   });
   return ds;
}

/**
 *  Deserialize a tuple
 *
 *  @brief Deserialize a tuple
 *  @param ds - The stream to read
 *  @param t - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam Args - Type of the objects contained in the tuple
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename... Args>
DataStream& operator>>( DataStream& ds, std::tuple<Args...>& t ) {
   boost::fusion::for_each( t, [&]( auto& i ) {
       ds >> i;
   });
   return ds;
}

/**
 *  Serialize a class
 *
 *  @brief Serialize a class
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of class
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value>* = nullptr>
DataStream& operator<<( DataStream& ds, const T& v ) {
   boost::pfr::for_each_field(v, [&](const auto& field) {
      ds << field;
   });
   return ds;
}

/**
 *  Deserialize a class
 *
 *  @brief Deserialize a class
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of class
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<std::is_class<T>::value>* = nullptr>
DataStream& operator>>( DataStream& ds, T& v ) {
   boost::pfr::for_each_field(v, [&](auto& field) {
      ds >> field;
   });
   return ds;
}

/**
 *  Serialize a primitive type
 *
 *  @brief Serialize a primitive type
 *  @param ds - The stream to write
 *  @param v - The value to serialize
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the primitive type
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
DataStream& operator<<( DataStream& ds, const T& v ) {
   ds.write( (const char*)&v, sizeof(T) );
   return ds;
}

/**
 *  Deserialize a primitive type
 *
 *  @brief Deserialize a primitive type
 *  @param ds - The stream to read
 *  @param v - The destination for deserialized value
 *  @tparam DataStream - Type of datastream
 *  @tparam T - Type of the primitive type
 *  @return DataStream& - Reference to the datastream
 */
template<typename DataStream, typename T, std::enable_if_t<_datastream_detail::is_primitive<T>()>* = nullptr>
DataStream& operator>>( DataStream& ds, T& v ) {
   ds.read( (char*)&v, sizeof(T) );
   return ds;
}

/**
 * Defines data stream for reading and writing data in the form of bytes
 *
 * @addtogroup datastream Data Stream
 * @ingroup cpp_api
 * @{
 */

/**
 * Unpack data inside a fixed size buffer as T
 *
 * @brief Unpack data inside a fixed size buffer as T
 * @tparam T - Type of the unpacked data
 * @param buffer - Pointer to the buffer
 * @param len - Length of the buffer
 * @return T - The unpacked data
 */
template<typename T>
T unpack( const char* buffer, size_t len ) {
   T result;
   datastream<const char*> ds(buffer,len);
   ds >> result;
   return result;
}

/**
 * Unpack data inside a variable size buffer as T
 *
 * @brief Unpack data inside a variable size buffer as T
 * @tparam T - Type of the unpacked data
 * @param bytes - Buffer
 * @return T - The unpacked data
 */
template<typename T>
T unpack( const std::vector<char>& bytes ) {
   return unpack<T>( bytes.data(), bytes.size() );
}

/**
 * Get the size of the packed data
 *
 * @brief Get the size of the packed data
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return size_t - Size of the packed data
 */
template<typename T>
size_t pack_size( const T& value ) {
  datastream<size_t> ps;
  ps << value;
  return ps.tellp();
}

template<typename T, typename... Args>
size_t pack_size( const T& first, Args&&... args) {
  datastream<size_t> ps;
  ps << first;
  return (ps.tellp() + pack_size(std::forward<Args>(args)...));
}

/**
 * Get packed data
 *
 * @brief Get packed data
 * @tparam T - Type of the data to be packed
 * @param value - Data to be packed
 * @return bytes - The packed data
 */
template<typename T>
std::vector<char> pack( const T& value ) {
  std::vector<char> result;
  result.resize(pack_size(value));

  datastream<char*> ds( result.data(), result.size() );
  ds << value;
  return result;
}

template<typename... Args>
std::vector<char> pack(Args&&... args) {
  std::vector<char> result;
  result.resize(pack_size(args...));
  datastream<char*> ds( result.data(), result.size());
  std::tuple<std::decay_t<Args>...> tuple_args(std::forward<Args>(args)...);
  ds << tuple_args;
  return result;
}

typedef uint8_t TYPE_MAGIC_NEO_ARGS;
#define CURRENT_MAGIC_VERSION 0
template<typename T>
std::vector<char> serialize_args_forneo( const T& value ) {
  #define ArgListType 0x10
  TYPE_MAGIC_NEO_ARGS magic_version = CURRENT_MAGIC_VERSION;
  uint8_t ltype = ArgListType;
  uint32_t lsize = 1;
  std::vector<char> result;
  result.resize(pack_size(magic_version, ltype, lsize, value));

  datastream<char*> ds( result.data(), result.size() );
  ds << magic_version;
  ds << ltype;
  ds << lsize;
  ds << value;
  return result;
}

template<typename... Args>
std::vector<char> serialize_args_forneo(Args&&... args) {
  TYPE_MAGIC_NEO_ARGS magic_version = CURRENT_MAGIC_VERSION;
  #define ArgTListType 0x10
  uint8_t ltype = ArgListType;
  uint32_t lsize = sizeof...(args);
  std::vector<char> result;
  result.resize(pack_size(magic_version, ltype, lsize, args...));
  datastream<char*> ds( result.data(), result.size());
  std::tuple<std::decay_t<Args>...> tuple_args(std::forward<Args>(args)...);
  ds << magic_version;
  ds << ltype;
  ds << lsize;
  ds << tuple_args;
  return result;
}

template<typename T>
T deserialize_result_forneo( const char* buffer, size_t len ) {
   TYPE_MAGIC_NEO_ARGS magic_version;
   T result;
   datastream<const char*> ds(buffer,len);
   ds >> magic_version;
   check(magic_version == CURRENT_MAGIC_VERSION, "result version error");
   ds >> result;
   return result;
}

template<typename T>
T deserialize_result_forneo( const std::vector<char>& bytes ) {
   return deserialize_result_forneo<T>( bytes.data(), bytes.size() );
}

template<typename T>
std::vector<char> make_notify_buffer( const T& value ) {
  std::vector<char> result;
  result.resize(pack_size(uint32_t(0), value));

  datastream<char*> ds( result.data(), result.size() );
  // "evt\x00"
  ds << uint8_t(0x65);
  ds << uint8_t(0x76);
  ds << uint8_t(0x74);
  ds << uint8_t(0x00);
  ds << value;
  return result;
}

template<typename T, typename... Args>
std::vector<char> make_notify_buffer(const T &arg0, Args&&... args) {
  #define ArgTListType 0x10
  uint8_t ltype = ArgListType;
  uint32_t lsize = sizeof...(args) + 1;
  std::vector<char> result;
  result.resize(pack_size(uint32_t(0), ltype, lsize, arg0, args...));
  datastream<char*> ds( result.data(), result.size());
  std::tuple<T,std::decay_t<Args>...> tuple_args(arg0, std::forward<Args>(args)...);
  // "evt\x00"
  ds << uint8_t(0x65);
  ds << uint8_t(0x76);
  ds << uint8_t(0x74);
  ds << uint8_t(0x00);
  ds << ltype;
  ds << lsize;
  ds << tuple_args;
  return result;
}

template<typename Stream>
inline void WriteVarUint( datastream<Stream>& ds, const uint64_t & v ) {
	if (v < 0xFD){
		uint8_t t = uint8_t(v);
		ds << t;
	} else if (v <= 0xFFFF) {
		uint16_t t = uint16_t(v);
		ds.put(0xFD);
		ds << t;
	} else if (v <= 0xFFFFFFFF) {
		uint32_t t = uint32_t(v);
		ds.put(0xFE);
		ds<<t;
	} else {
		ds.put(0xFF);
		ds<<v;
	}
}

template<typename Stream>
inline uint64_t ReadVarUint( datastream<Stream>& ds) {
	uint8_t t; 
	uint64_t res;
	ds.get(t);

	if (t == 0xFD){
		uint16_t res_t ;
		ds >> res_t;
		res = uint64_t(res_t);
	} else if (t == 0xFE) {
		uint32_t res_t ;
		ds >> res_t;
		res = uint64_t(res_t);
	} else if (t == 0xFF) {
		ds >> res;
	} else {
		res = uint64_t(t);
	}
   return res;
}

}
