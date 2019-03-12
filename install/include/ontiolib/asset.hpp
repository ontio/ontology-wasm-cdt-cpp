#pragma once
//#include "system.hpp"

#include <tuple>
#include <limits>

namespace ontio {
  /**
   *  Defines %CPP API for managing assets
   *  @addtogroup asset asset CPP API
   *  @ingroup cpp_api
   *  @
   */

   /**
    * @struct Stores information for owner of asset
    */

   struct asset {
      /**
       * The amount of the asset
       */
      int64_t      amount = 0;

      /**
       * Maximum amount possible for this asset. It's capped to 2^62 - 1
       */
      static constexpr int64_t max_amount    = (1LL << 62) - 1;

      asset() {}

      /**
       * Construct a new asset given the amount
       *
       * @param a - The amount of the asset
       */
      asset( int64_t a)
      :amount(a)
      {
         ontio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      /**
       * Check if the amount doesn't exceed the max amount
       *
       * @return true - if the amount doesn't exceed the max amount
       * @return false - otherwise
       */
      bool is_amount_within_range()const { return -max_amount <= amount && amount <= max_amount; }

      /**
       * Check if the asset is valid. %A valid asset has its amount <= max_amount.
       *
       * @return true - if the asset is valid
       * @return false - otherwise
       */
      bool is_valid()const               { return is_amount_within_range(); }

      /**
       * Set the amount of the asset
       *
       * @param a - New amount for the asset
       */
      void set_amount( int64_t a ) {
         amount = a;
         ontio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      /**
       * Unary minus operator
       *
       * @return asset - New asset with its amount is the negative amount of this asset
       */
      asset operator-()const {
         asset r = *this;
         r.amount = -r.amount;
         return r;
      }

      /**
       * Subtraction assignment operator
       *
       * @param a - Another asset to subtract this asset with
       * @return asset& - Reference to this asset
       * @post The amount of this asset is subtracted by the amount of asset a
       */
      asset& operator-=( const asset& a ) {
         amount -= a.amount;
         ontio::check( -max_amount <= amount, "subtraction underflow" );
         ontio::check( amount <= max_amount,  "subtraction overflow" );
         return *this;
      }

      /**
       * Addition Assignment  operator
       *
       * @param a - Another asset to subtract this asset with
       * @return asset& - Reference to this asset
       * @post The amount of this asset is added with the amount of asset a
       */
      asset& operator+=( const asset& a ) {
         amount += a.amount;
         ontio::check( -max_amount <= amount, "addition underflow" );
         ontio::check( amount <= max_amount,  "addition overflow" );
         return *this;
      }

      /**
       * Addition operator
       *
       * @param a - The first asset to be added
       * @param b - The second asset to be added
       * @return asset - New asset as the result of addition
       */
      inline friend asset operator+( const asset& a, const asset& b ) {
         asset result = a;
         result += b;
         return result;
      }

      /**
       * Subtraction operator
       *
       * @param a - The asset to be subtracted
       * @param b - The asset used to subtract
       * @return asset - New asset as the result of subtraction of a with b
       */
      inline friend asset operator-( const asset& a, const asset& b ) {
         asset result = a;
         result -= b;
         return result;
      }

      /**
       * @brief Multiplication assignment operator, with a number
       *
       * @details Multiplication assignment operator. Multiply the amount of this asset with a number and then assign the value to itself.
       * @param a - The multiplier for the asset's amount
       * @return asset - Reference to this asset
       * @post The amount of this asset is multiplied by a
       */
      asset& operator*=( int64_t a ) {
         int128_t tmp = (int128_t)amount * (int128_t)a;
         ontio::check( tmp <= max_amount, "multiplication overflow" );
         ontio::check( tmp >= -max_amount, "multiplication underflow" );
         amount = (int64_t)tmp;
         return *this;
      }

      /**
       * Multiplication operator, with a number proceeding
       *
       * @brief Multiplication operator, with a number proceeding
       * @param a - The asset to be multiplied
       * @param b - The multiplier for the asset's amount
       * @return asset - New asset as the result of multiplication
       */
      friend asset operator*( const asset& a, int64_t b ) {
         asset result = a;
         result *= b;
         return result;
      }


      /**
       * Multiplication operator, with a number preceeding
       *
       * @param a - The multiplier for the asset's amount
       * @param b - The asset to be multiplied
       * @return asset - New asset as the result of multiplication
       */
      friend asset operator*( int64_t b, const asset& a ) {
         asset result = a;
         result *= b;
         return result;
      }

      /**
       * @brief Division assignment operator, with a number
       *
       * @details Division assignment operator. Divide the amount of this asset with a number and then assign the value to itself.
       * @param a - The divisor for the asset's amount
       * @return asset - Reference to this asset
       * @post The amount of this asset is divided by a
       */
      asset& operator/=( int64_t a ) {
         ontio::check( a != 0, "divide by zero" );
         ontio::check( !(amount == std::numeric_limits<int64_t>::min() && a == -1), "signed division overflow" );
         amount /= a;
         return *this;
      }

      /**
       * Division operator, with a number proceeding
       *
       * @param a - The asset to be divided
       * @param b - The divisor for the asset's amount
       * @return asset - New asset as the result of division
       */
      friend asset operator/( const asset& a, int64_t b ) {
         asset result = a;
         result /= b;
         return result;
      }

      /**
       * Division operator, with another asset
       *
       * @param a - The asset which amount acts as the dividend
       * @param b - The asset which amount acts as the divisor
       * @return int64_t - the resulted amount after the division
       */
      friend int64_t operator/( const asset& a, const asset& b ) {
         ontio::check( b.amount != 0, "divide by zero" );
         return a.amount / b.amount;
      }

      /**
       * Equality operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if both asset has the same amount
       * @return false - otherwise
       */
      friend bool operator==( const asset& a, const asset& b ) {
         return a.amount == b.amount;
      }

      /**
       * Inequality operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if both asset doesn't have the same amount
       * @return false - otherwise
       */
      friend bool operator!=( const asset& a, const asset& b ) {
         return !( a == b);
      }

      /**
       * Less than operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is less than the second asset amount
       * @return false - otherwise
       */
      friend bool operator<( const asset& a, const asset& b ) {
         return a.amount < b.amount;
      }

      /**
       * Less or equal to operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is less or equal to the second asset amount
       * @return false - otherwise
       */
      friend bool operator<=( const asset& a, const asset& b ) {
         return a.amount <= b.amount;
      }

      /**
       * Greater than operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is greater than the second asset amount
       * @return false - otherwise
       */
      friend bool operator>( const asset& a, const asset& b ) {
         return a.amount > b.amount;
      }

      /**
       * Greater or equal to operator
       *
       * @param a - The first asset to be compared
       * @param b - The second asset to be compared
       * @return true - if the first asset's amount is greater or equal to the second asset amount
       * @return false - otherwise
       */
      friend bool operator>=( const asset& a, const asset& b ) {
         return a.amount >= b.amount;
      }

      template<typename DataStream> 
      friend DataStream& operator<<(DataStream& ds, const asset& t ) {
	  unsigned_int s = pack_size(t.amount);
	  ds << s;
	  ds << t.amount;
	  return ds;
      }

      template<typename DataStream> 
      friend DataStream& operator>>(DataStream& ds, asset& t ) {
	  unsigned_int s;
	  ds >> s;
	  ds >> t.amount;
	  return ds;
      }
   };

}
