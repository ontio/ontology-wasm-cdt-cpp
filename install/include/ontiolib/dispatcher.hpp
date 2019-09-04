#pragma once
#include "datastream.hpp"
#include "serialize.hpp"
#include "memory.hpp"

#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_tuple.hpp>
#include <boost/preprocessor/tuple/enum.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

#include <boost/mp11/tuple.hpp>
#include "stringhash.hpp"

namespace ontio {

template<typename T>
void ontio_return(const T &t);

#ifdef WASM_USE_SYS_KEYTABLE
using std::set;
using std::string;
using std::pair;
using H256 = std::array<uint8_t, 32>;
using H160 = std::array<uint8_t, 20>;
using key = std::vector<char>;
using std::array;
using std::string;
template<typename T> bool storage_get(const key &key, T &val);
template<typename T> void storage_put(const key &key, const T &val);

class keytable :private set<string> {
	private:
	public:
		using set::set;
		using set::begin;
		using set::end;

		static bool load(const string &tablename, keytable &t) {
			return storage_get(pack(tablename), t);
		}

		static void commit(const string &tablename, const keytable &t) {
			storage_put(pack(tablename), t);
		}

		template<typename T>
		bool get(const string &t, T &v) {
			auto it = find(t);
			if (it != end()) {
				string s(*it);
				if(storage_get(pack(s), v))
					return true;
			}
			return false;
		}

		template<typename T>
		void put(const string &t, const T &v) {
			insert(t);
			storage_put(pack(t),v);
		}

		void erase(const string &t) {
			set::erase(t);
			void storage_delete(const key& key);
			storage_delete(pack(t));
		}

		auto prefixwith(const string &t) {
			auto itlow = lower_bound(t);
			uint16_t a = 0xff;
			string s;
			s.resize(1);
			memcpy(s.data(),&a, s.size());
			auto itupper = lower_bound(t + s);

			pair<set<string>::iterator, set<string>::iterator> p(itlow, --itupper);
			return p;
		}

		ONTLIB_SERIALIZE_DERIVED_NOMEMBER(keytable, set<string> )
};
#endif


     /**
    * Unpack the received action and execute the correponding action handler
    *
    * @tparam T - The contract class that has the correponding action handler, this contract should be derived from ontio::contract
    * @tparam Q - The namespace of the action handler function
    * @tparam Args - The arguments that the action handler accepts, i.e. members of the action
    * @param obj - The contract object that has the correponding action handler
    * @param func - The action handler
    * @return true
    */
   template<typename T, typename R, typename... Args>
   void execute_action( datastream<const char*>& ds, R (T::*func)(Args...)  ) {
      std::tuple<std::decay_t<Args>...> args;
      ds >> args;

#ifdef WASM_USE_SYS_KEYTABLE
	  class keytable syskt;
	  keytable::load("###syskt###", syskt);
      T inst(syskt);
#else
      T inst;
#endif

      auto f2 = [&]( auto... a ) {
         R&& t = ((&inst)->*func)( a... );
#ifdef WASM_USE_SYS_KEYTABLE
		 keytable::commit("###syskt###", syskt);
#endif
		 ontio_return<R>(t);
      };

      boost::mp11::tuple_apply( f2, args );
   }

   template<typename T, typename... Args>
   void execute_action( datastream<const char*>& ds, void (T::*func)(Args...)  ) {
      std::tuple<std::decay_t<Args>...> args;
      ds >> args;

#ifdef WASM_USE_SYS_KEYTABLE
	  class keytable syskt;
	  keytable::load("###syskt###", syskt);
      T inst(syskt);
#else
      T inst;
#endif

      auto f2 = [&]( auto... a ) {
         ((&inst)->*func)( a... );
#ifdef WASM_USE_SYS_KEYTABLE
		 keytable::commit("###syskt###", syskt);
#endif
      };

      boost::mp11::tuple_apply( f2, args );
   }

 // Helper macro for ONTIO_DISPATCH_INTERNAL
 #define ONTIO_DISPATCH_INTERNAL( r, OP, elem ) \
	case __hash_string__::hash_at_compile_time( BOOST_PP_STRINGIZE(elem) ) : \
       ontio::execute_action( ds, &OP::elem ); \
       break;

 // Helper macro for ONTIO_DISPATCH
 #define ONTIO_DISPATCH_HELPER( TYPE,  MEMBERS ) \
    BOOST_PP_SEQ_FOR_EACH( ONTIO_DISPATCH_INTERNAL, TYPE, MEMBERS )



/**
 * @addtogroup dispatcher
 * Convenient macro to create contract apply handler
 *
 * @note To be able to use this macro, the contract needs to be derived from ontio::contract
 * @param TYPE - The class name of the contract
 * @param MEMBERS - The sequence of available actions supported by this contract
 *
 * Example:
 * @code
 * ONTIO_DISPATCH( ontio::bios, (setpriv)(setalimits)(setglimits)(setprods)(reqauth) )
 * @endcode
 */
#if defined(WASM_LOCAL_DEBUG )
#define ONTIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" { \
   void apply(void) {  \
      std::string method; \
      auto input = get_input(); \
      datastream<const char*> ds(input.data(), input.size()); \
      ds >> method; \
      switch(__hash_string__::string_hash(method) ) { \
          ONTIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
      } \
   } \
}
#else
#define ONTIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" { \
   void invoke(void) {  \
      std::string method; \
      auto input = get_input(); \
      datastream<const char*> ds(input.data(), input.size()); \
      ds >> method; \
      switch(__hash_string__::string_hash(method) ) { \
          ONTIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
      } \
   } \
}
#endif
}
