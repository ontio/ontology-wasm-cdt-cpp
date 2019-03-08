#pragma once
#include <ontiolib/print.hpp>
#include <ontiolib/action.hpp>

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

#include <boost/mp11/tuple.hpp>

namespace ontio {

   template<typename Contract, typename FirstAction>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return false;
   }


   /**
    * This method will dynamically dispatch an incoming set of actions to
    *
    * ```
    * static Contract::on( ActionType )
    * ```
    *
    * For this to work the Actions must be derived from ontio::contract
    *
    */
   template<typename Contract, typename FirstAction, typename SecondAction, typename... Actions>
   bool dispatch( uint64_t code, uint64_t act ) {
      if( code == FirstAction::get_account() && FirstAction::get_name() == act ) {
         Contract().on( unpack_action_data<FirstAction>() );
         return true;
      }
      return ontio::dispatch<Contract,SecondAction,Actions...>( code, act );
   }

   /**
    * @addtogroup dispatcher Dispatcher C++ API
    * @ingroup cpp_api
    * @brief Defines C++ functions to dispatch action to proper action handler inside a contract
    * @{
    */

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
   template<typename T, typename... Args>
   bool execute_action( datastream<const char*>& ds, void (T::*func)(Args...)  ) {
	   /*
      size_t size = input_length();

      //using malloc/free here potentially is not exception-safe, although WASM doesn't support exceptions
      constexpr size_t max_stack_buffer_size = 512;
      void* buffer = nullptr;
      if( size > 0 ) {
         buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size);
         get_input( buffer, size );
      }

      datastream<const char*> ds((char*)buffer, size);
      */
      printf("execute_action\n");
      std::tuple<std::decay_t<Args>...> args;
      ds >> args;

      T inst(ds);

      auto f2 = [&]( auto... a ){
         ((&inst)->*func)( a... );
      };

      boost::mp11::tuple_apply( f2, args );
      /*
      if ( max_stack_buffer_size < size ) {
         free(buffer);
      }
      */
      return true;
   }

/// @}



 // Helper macro for ONTIO_DISPATCH_INTERNAL
 #define ONTIO_DISPATCH_INTERNAL( r, OP, elem ) \
   case ontio::name( BOOST_PP_STRINGIZE(elem)  ).value: \
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
      size_t size = input_length(); \
      constexpr size_t max_stack_buffer_size = 512; \
      void* buffer = nullptr; \
      if( size > 0 ) { \
         buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size); \
         get_input( buffer ); \
      } \
      datastream<const char*> ds((char*)buffer, size); \
      ds >> method; \
      printf("method %s\n", method.c_str());	   	\
      std::string method_t; 	\
      uint64_t action = ontio::name(method).value; 	\
      switch( action ) { \
          ONTIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
      } \
      if ( max_stack_buffer_size < size ) { \
         free(buffer); \
      } \
      /* does not allow destructor of thiscontract to run: ontio_exit(0); */ \
   } \
} 
#else
#define ONTIO_DISPATCH( TYPE, MEMBERS ) \
extern "C" { \
   void invoke(void) {  \
      std::string method; \
      size_t size = input_length(); \
      constexpr size_t max_stack_buffer_size = 512; \
      void* buffer = nullptr; \
      if( size > 0 ) { \
         buffer = max_stack_buffer_size < size ? malloc(size) : alloca(size); \
         get_input( buffer ); \
      } \
      datastream<const char*> ds((char*)buffer, size); \
      ds >> method; \
      printf("method %s\n", method.c_str());	   	\
      std::string method_t; 	\
      uint64_t action = ontio::name(method).value; 	\
      switch( action ) { \
          ONTIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
      } \
      if ( max_stack_buffer_size < size ) { \
         free(buffer); \
      } \
      /* does not allow destructor of thiscontract to run: ontio_exit(0); */ \
   } \
}
#endif

}
