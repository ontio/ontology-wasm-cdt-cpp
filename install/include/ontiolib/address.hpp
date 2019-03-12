#pragma once
#include "asset.hpp"

namespace ontio {

#define ADDRLENGTH 20
template<size_t size>
std::array<uint8_t,size> hexstobytes(const std::string& hex) {
    std::array<uint8_t, size> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
	std::string byteString = hex.substr(i, 2);
	uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
	bytes[i/2] = byte;
    }

    return bytes;
}

typedef std::array<uint8_t, ADDRLENGTH> address;

class native_address {
    public:
    address addr;

    native_address() = default;
    native_address(address _addr):addr(_addr) {}

    template<typename DataStream>
    friend DataStream& operator<<( DataStream& ds, const native_address & t ) {
	unsigned_int addr_len = 20;
	ds << addr_len;
	ds << t.addr;
    	return ds;
    }

    template<typename DataStream>
    friend DataStream& operator>>( DataStream& ds, native_address & t ) {
	unsigned_int addr_len;
	ds >> addr_len;
	ds >> t.addr;
	return ds;
    }
};

class ontstate {
    public:
    ontstate() {}
    ontstate(address _from, address _to, asset _v): from(_from), to(_to), value(_v) {}

    native_address from;
    native_address to;
    asset value;

#ifdef WASM_DEBUG_INTERFACE
    void debugout(void) {
	printf("from: ");
	for(auto i : from.addr) { printf("%02x",i); }
	printf("\n");
	printf("to: ");
	for(auto i : to.addr) { printf("%02x",i); }
	printf("\n");
	printf("value: ");
	printf("%lld", value.amount);
	printf("\n");
    }
#endif
    
    ONTLIB_SERIALIZE( ontstate, (from)(to)(value))
};


typedef std::vector<ontstate> states;

class transfer {
	public:
	states states;

#ifdef WASM_DEBUG_INTERFACE
	void debugout(void) {
		for (auto &i: states) {
			i.debugout();
		}
	}
#endif

    	template<typename DataStream> 
    	friend DataStream& operator << ( DataStream& ds, const transfer& t ) { 
	    unsigned_int s, s0;
	    s0 	= pack_size(unsigned_int(t.states.size()));
	    ds << s0;
	    ds << t.states;
	    return ds;
	}

    	template<typename DataStream> 
    	friend DataStream& operator >> ( DataStream& ds, transfer& t ) { 
	    unsigned_int s;
	    ds >> s;
	    ds >> t.states;
	    return ds;
	}
};

class transferfrom {
    public:
    transferfrom() {}
    transferfrom(address _sender, address _from, address _to, asset _v):sender(_sender), from(_from), to(_to), value(_v) {}

    native_address sender;
    native_address from;
    native_address to;
    asset value;

#ifdef WASM_DEBUG_INTERFACE
    void debugout(void) {
	printf("sender: ");
	for(auto i : sender.addr) { printf("%02x",i); }
	printf("\n");
	printf("from: ");
	for(auto i : from.addr) { printf("%02x",i); }
	printf("\n");
	printf("to: ");
	for(auto i : to.addr) { printf("%02x",i); }
	printf("\n");
	printf("value: ");
	printf("%lld", value.amount);
	printf("\n");
    }
#endif

    ONTLIB_SERIALIZE( transferfrom,(sender)(from)(to)(value))
};

class transfer_arg {
    public:
    uint8_t version;
    std::string method;
    transfer tfs;

    transfer_arg() {}
    transfer_arg(std::string _method, address _from, address _to, asset v ): method(_method) {
	version = 0;
	ontstate state_t(_from, _to, v);
	tfs.states.push_back(state_t);
    }

    transfer_arg(std::string _method, states _states): method(_method) {
	version = 0;
	tfs.states = _states;
    }

#ifdef WASM_DEBUG_INTERFACE
    void debugout(void) {
	printf("version: %d\n", version);
	printf("method: %s\n", method.c_str());
	tfs.debugout();
    }
#endif

    template<typename DataStream> 
    friend DataStream& operator << ( DataStream& ds, const transfer_arg& t ) { 
	    unsigned_int s;
	    s 	= pack_size(t.tfs);
	    ds << t.version;
	    ds << t.method;
	    ds << s;
	    /* vector size have a var byte.*/
	    ds << t.tfs;
	    return ds;
    }

    template<typename DataStream> 
    friend DataStream& operator >> ( DataStream& ds, transfer_arg& t ) { 
	    unsigned_int s;
	    ds >> t.version;
	    ds >> t.method;
	    ds >> s;
	    ds >> t.tfs;
	    return ds;
    }
};

class transferfrom_arg {
    public:
    uint8_t version;
    std::string method;
    transferfrom tfsf;

    transferfrom_arg() {}
    transferfrom_arg(std::string _method, address _sender , address _from, address _to, asset _v ): method(_method), tfsf(_sender, _from, _to, _v) {
	version = 0;
    }

#ifdef WASM_DEBUG_INTERFACE
    void debugout(void) {
	printf("version: %d\n", version);
	printf("method: %s\n", method.c_str());
	tfsf.debugout();
    }
#endif

    template<typename DataStream> 
    friend DataStream& operator << ( DataStream& ds, const transferfrom_arg& t ) { 
	    unsigned_int s;
	    s 	= pack_size(t.tfsf);
	    ds << t.version;
	    ds << t.method;
	    ds << s;
	    /* vector size have a var byte.*/
	    ds << t.tfsf;
	    return ds;
    }

    template<typename DataStream> 
    friend DataStream& operator >> ( DataStream& ds, transferfrom_arg& t ) { 
	    unsigned_int s;
	    ds >> t.version;
	    ds >> t.method;
	    ds >> s;
	    ds >> t.tfsf;
	    return ds;
    }
};

class approve_arg {
    public:
    uint8_t version;
    std::string method;
    ontstate ontstate;

#ifdef WASM_DEBUG_INTERFACE
    void debugout(void) {
	printf("version: %d\n", version);
	printf("method: %s\n", method.c_str());
	ontstate.debugout();
    }
#endif

    approve_arg() {}
    approve_arg(std::string _method, address _from, address _to, asset _v ): method(_method), ontstate(_from, _to, _v) {
	version = 0;
    }

    template<typename DataStream> 
    friend DataStream& operator << ( DataStream& ds, const approve_arg& t ) { 
	    unsigned_int s;
	    s 	= pack_size(t.ontstate);
	    ds << t.version;
	    ds << t.method;
	    ds << s;
	    /* vector size have a var byte.*/
	    ds << t.ontstate;
	    return ds;
    }

    template<typename DataStream> 
    friend DataStream& operator >> ( DataStream& ds, approve_arg & t ) { 
	    unsigned_int s;
	    ds >> t.version;
	    ds >> t.method;
	    ds >> s;
	    ds >> t.ontstate;
	    return ds;
    }
};

}
