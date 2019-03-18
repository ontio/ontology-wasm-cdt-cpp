#pragma once

#include "name.hpp"
#include "datastream.hpp"

namespace ontio {
class contract {
   public:
      contract( datastream<const char*> ds ): _ds(ds) {}
      inline datastream<const char*> get_datastream()const { return _ds; }
   protected:
      datastream<const char*> _ds = datastream<const char*>(nullptr, 0);
};
}
