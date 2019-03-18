/**
 *  @file
 *  @copyright defined in ont/LICENSE
 */
#pragma once
#include "system.h"
#include <string>

namespace ontio {

   inline void check(bool pred, const char* msg) {
      if (!pred) {
         ontio_assert(false, msg);
      }
   }

   inline void check(bool pred, const std::string& msg) {
      if (!pred) {
         ontio_assert(false, msg.c_str());
      }
   }

   inline void check(bool pred, std::string&& msg) {
      if (!pred) {
         ontio_assert(false, msg.c_str());
      }
   }
}
