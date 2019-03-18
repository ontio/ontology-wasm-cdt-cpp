#pragma once

#define ADDRLENGTH 20
typedef std::array<uint8_t, 32> H256;
typedef std::array<uint8_t, 20> H160;
typedef H160 address;
typedef std::vector<char> key;

#include "asset.hpp"
#include "service.hpp"

namespace ontio {

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

class transferinner {
    private:
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

	class native_asset {
		public:
			asset amount;
			native_asset() = default;
			native_asset(asset _v): amount(_v) {}

			template<typename DataStream>
			friend DataStream& operator<<( DataStream& ds, const native_asset& t ) {
				unsigned_int s = pack_size(t.amount);
				ds << s;
				ds << t.amount;
				return ds;
			}

			template<typename DataStream>
			friend DataStream& operator>>( DataStream& ds, native_asset& t ) {
				unsigned_int s;
				ds >> s;
				ds >> t.amount;
				return ds;
			}
	};
	
	class ontstate {
		public:
			ontstate() {}
			ontstate(address _from, address _to, asset _v): from(_from), to(_to), value(_v) {}

			native_address from;
			native_address to;
			native_asset value;

			#ifdef WASM_DEBUG_INTERFACE
	    	void debugout(void) {
				printf("from: ");
				for(auto i : from.addr) { printf("%02x",i); }
				printf("\n");
				printf("to: ");
				for(auto i : to.addr) { printf("%02x",i); }
				printf("\n");
				printf("value: ");
				printf("%lld", value.amount.amount);
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
	    	native_asset value;
	
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
				printf("%lld", value.amount.amount);
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

			transfer_arg() = default;
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

	class balanceof_arg {
		public:
			uint8_t version;
			std::string method;
			native_address addr;

			balanceof_arg() {}
			balanceof_arg(std::string _method, address _from): method(_method), addr(_from) {
				version = 0;
			}

			#ifdef WASM_DEBUG_INTERFACE
			void debugout(void) {
				printf("version: %d\n", version);
				printf("method: %s\n", method.c_str());
				printf("from: ");
				for(auto i : addr.addr) { printf("%02x",i); }
				printf("\n");
			}
			#endif

			template<typename DataStream> 
			friend DataStream& operator << ( DataStream& ds, const balanceof_arg& t ) {
				unsigned_int s;
				s 	= pack_size(t.addr);
				ds << t.version;
				ds << t.method;
				ds << s;
				/* vector size have a var byte.*/
				ds << t.addr;
				return ds;
			}

			template<typename DataStream> 
			friend DataStream& operator >> ( DataStream& ds, balanceof_arg& t ) {
				unsigned_int s;
				ds >> t.version;
				ds >> t.method;
				ds >> s;
				ds >> t.addr;
				return ds;
			}
	};

	class allowance_arg {
		uint8_t version;
		std::string method;
		native_address from;
		native_address to;
		public:
			allowance_arg() {}
			allowance_arg(std::string _method, address _from, address _to): method(_method), from(_from), to(_to) {
				version = 0;
			}

			#ifdef WASM_DEBUG_INTERFACE
			void debugout(void) {
				printf("version: %d\n", version);
				printf("method: %s\n", method.c_str());
				printf("from: ");
				for(auto i : from.addr) { printf("%02x",i); }
				printf("\n");
				printf("to: ");
				for(auto i : to.addr) { printf("%02x",i); }
				printf("\n");
			}
			#endif

			template<typename DataStream> 
			friend DataStream& operator << ( DataStream& ds, const allowance_arg& t ) {
				unsigned_int s;
				s 	= pack_size(t.from) + pack_size(t.to);
				ds << t.version;
				ds << t.method;
				ds << s;
				/* vector size have a var byte.*/
				ds << t.from;
				ds << t.to;
				return ds;
			}

			template<typename DataStream> 
			friend DataStream& operator >> ( DataStream& ds, allowance_arg& t ) {
				unsigned_int s;
				ds >> t.version;
				ds >> t.method;
				ds >> s;
				ds >> t.from;
				ds >> t.to;
				return ds;
			}
	}; 

    public:
		static bool transfer_inner(address addr, address &from, address &to, asset &amount) {
			bool success;
			transfer_arg arg("transfer", from, to, amount);
			auto data = pack(arg);
			call_native(addr, data, success);
			return success;
		}

		static bool transferfrom_inner(address addr, address sender, address from, address to, asset amount) {
			bool success;
			transferfrom_arg arg("transferfrom", sender, from, to, amount);
			auto data = pack(arg);
			call_native(addr, data, success);
			return success;
		}

		static bool approve_inner(address addr, address from, address to, asset amount) {
			bool success;
			approve_arg arg("approve", from, to, amount);
			auto data = pack(arg);
			call_native(addr, data, success);
			return success;
		}

		static asset balanceof_inner(address addr, address from) {
			int128_t balance;

			balanceof_arg arg("balanceOf", from);
			auto data = pack(arg);
			call_native(addr , data,  balance);
			ontio::check( balance <= asset::max_amount, "balanceof_inner. balance overflow");
			ontio::check( balance >= -asset::max_amount, "balanceof_inner. balance underflow");
			asset res((int64_t) balance);
			return res;
		}

		static asset allowance_inner(address addr, address from, address to) {
			int128_t balance;
			allowance_arg arg("allowance", from, to);
			auto data = pack(arg);
			call_native(addr, data, balance);

			ontio::check( balance <= asset::max_amount, "balanceof_inner. balance overflow");
			ontio::check( balance >= -asset::max_amount, "balanceof_inner. balance underflow");
			asset res((int64_t) balance);

			return res;
		}
};


class ont: transferinner {
	private:
		static address ontaddr;
	public:
		static bool transfer(address &from, address &to, asset &amount) {
			return transfer_inner(ontaddr, from, to, amount);
		}
		static bool transferfrom(address &sender, address &from, address &to, asset &amount) {
			return transferfrom_inner(ontaddr, sender, from, to, amount);
		}
		static bool approve(address &from, address &to, asset &amount) {
			return approve_inner(ontaddr, from, to, amount);
		}
		static asset balanceof(address from) {
			return balanceof_inner(ontaddr, from);
		}
		static asset allowance(address &from, address &to) {
			return allowance_inner(ontaddr, from, to);
		}
};

class ong: public transferinner {
	private:
		static address ongaddr;
	public:
		static bool transfer(address &from, address &to, asset &amount) {
			return transfer_inner(ongaddr, from, to, amount);
		}
		static bool transferfrom(address &sender, address &from, address &to, asset &amount) {
			return transferfrom_inner(ongaddr, sender, from, to, amount);
		}
		static bool approve(address &from, address &to, asset &amount) {
			return approve_inner(ongaddr, from, to, amount);
		}
		static asset balanceof(address from) {
			return balanceof_inner(ongaddr, from);
		}
		static asset allowance(address &from, address &to) {
			return allowance_inner(ongaddr, from, to);
		}
};

address ont::ontaddr = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
address ong::ongaddr = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};

}
