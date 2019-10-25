/**
 *  @file
 *  @copyright defined in ont/LICENSE
 */
#pragma once
#include "system.h"
#include <string>

namespace ontio {

   extern "C" void ontio_panic( const char* cstr, const uint32_t len);
   inline void check(bool pred, const char* msg) {
      if (!pred) {
		 ontio_panic(msg, strlen(msg));
      }
   }

   inline void check(bool pred, const std::string& msg) {
      if (!pred) {
		 ontio_panic((char *)msg.c_str(), msg.size());
      }
   }

   inline void check(bool pred, std::string&& msg) {
      if (!pred) {
		 ontio_panic((char *)msg.c_str(), msg.size());
      }
   }
}
