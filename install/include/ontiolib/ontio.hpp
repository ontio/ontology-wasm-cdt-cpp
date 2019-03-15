/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once
#include "action.hpp"
#include "print.hpp"
#include "dispatcher.hpp"
#include "contract.hpp"
#include "ont.hpp"

#ifndef ONTIO_NATIVE
static_assert( sizeof(long) == sizeof(int), "unexpected size difference" );
#endif

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
}
