#pragma once
#include "ontasset.hpp"

namespace ontio {

#define ADDR_LEN 20
class Address {
    public:
    //uint8_t bytes[ADDR_LEN];	
    std::array<uint8_t, ADDR_LEN> bytes;

    template<typename DataStream>
    friend DataStream& operator<<( DataStream& ds, const Address & v ) {
	//unsigned_int t = 20;
	//ds << t;
	ds << v.bytes;
	return ds;
    }

    template<typename DataStream>
    friend DataStream& operator>>( DataStream& ds, Address & v ) {
	//unsigned_int t;
	//ds >> t;
	//ontio_assert(t.value == 20, "Deserialize error. addr len must be 20 bytes.");
	ds >> v.bytes;
	return ds;
    }
};

class NativeArg {
    public:
    uint8_t version;
    std::string method;
    Address from;
    Address to;
    ontasset value;
    
    // here const is very important. pack_size(const T &value) { ds << value; } so value here is const T. if not const, will can not find a template to match.
    template<typename DataStream> 
    friend DataStream& operator << ( DataStream& ds, const NativeArg& t ) { 
	unsigned_int s = pack_size(t.value);
	unsigned_int addr_len = 20;

    	ds << t.version;
  	ds << t.method;
	ds << addr_len;
        ds << t.from;
	ds << addr_len;
	ds << t.to; 
	ds << s;
	ds << t.value; 
	return ds;
    }

    template<typename DataStream> 
    friend DataStream& operator >> ( DataStream& ds, NativeArg& t ) { 
	unsigned_int s;
	ds >> t.version; 
	ds >> t.method; 
	ds >> s;
	ontio_assert(s.value == 20, "Deserialize error. addr len must be 20 bytes.");
	ds >> t.from;
	ds >> s;
	ontio_assert(s.value == 20, "Deserialize error. addr len must be 20 bytes.");
	ds >> t.to; 
	ds >> s;
	ds >> t.value; 
	return ds;
    }
};

}
