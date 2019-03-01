#pragma once

#include "name.hpp"
#include "datastream.hpp"


/**
 * @defgroup contract Contract
 * @ingroup types
 * @brief Defines contract type which is %base class for every ONTIO contract
 * @{
 */
namespace ontio {

/**
 * @brief %Base class for ONTIO contract.
 * @details %Base class for ONTIO contract. %A new contract should derive from this class, so it can make use of ONTIO_ABI macro.
 */
class contract {
   public:
      /**
       * Construct a new contract given the contract name
       *
       * @param receiver - The name of this contract
       * @param code - The code name of the action this contract is processing.
       * @param ds - The datastream used
       */
      //contract( datastream<const char*> ds ):_self(receiver),_code(code),_ds(ds) {}
      contract( datastream<const char*> ds ): _ds(ds) {}

      /**
       * Get the datastream for this contract
       *
       * @return datastream<const char*> - The datastream for this contract
       */
      inline datastream<const char*> get_datastream()const { return _ds; }

   protected:
      ///**
      // * The datastream for this contract
      // */
      datastream<const char*> _ds = datastream<const char*>(nullptr, 0);
};
}
