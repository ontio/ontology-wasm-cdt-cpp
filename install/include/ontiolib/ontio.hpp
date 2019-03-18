/**
 *  @file
 *  @copyright defined in ont/LICENSE
 */
#pragma once
#include <stdint.h>
#include <wchar.h>
#include "dispatcher.hpp"
#include "contract.hpp"
#include "ont.hpp"

static_assert( sizeof(long) == sizeof(int), "unexpected size difference" );

/**
 * Helper macros to reduce the verbosity for common contracts
 */
#define CONTRACT class [[ontio::contract]]
#define ACTION   [[ontio::action]] void
#define TABLE struct [[ontio::table]]

/**
 * @defgroup c_api C API
 * @brief C++ API for writing ESIO Smart Contracts
 */

 /**
  * @defgroup cpp_api C++ API
  * @brief C++ API for writing ESIO Smart Contracts
  */

extern "C" {
void  ontio_assert( uint32_t test, const char* msg ) {
	if (not test) {
		printf("%s\n", msg);
		abort();
	}
}
#ifdef WASM_LOCAL_DEBUG
void save_input_arg( void* msg, size_t len);
#endif
}
