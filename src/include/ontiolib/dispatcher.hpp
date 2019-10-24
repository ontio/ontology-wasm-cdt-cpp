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
	  check(ds.remaining() == 0, "not accurate match the args serialize bytes. deserialize error\n");

      T inst;

      auto f2 = [&]( auto... a ) {
         R&& t = ((&inst)->*func)( a... );
		 ontio_return<R>(t);
      };

      boost::mp11::tuple_apply( f2, args );
   }

   template<typename T, typename... Args>
   void execute_action( datastream<const char*>& ds, void (T::*func)(Args...)  ) {
      std::tuple<std::decay_t<Args>...> args;
      ds >> args;
	  check(ds.remaining() == 0, "not accurate match the args serialize bytes. deserialize error\n");

      T inst;

      auto f2 = [&]( auto... a ) {
         ((&inst)->*func)( a... );
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
		  default: \
			check(false, "no metod to call\n");\
			break; \
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
		  default: \
			check(false, "no metod to call\n");\
			break; \
      } \
   } \
}
#endif
}
