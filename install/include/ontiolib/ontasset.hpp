#pragma once
//#include "system.hpp"

#include <tuple>
#include <limits>

namespace ontio {
  /**
   *  Defines %CPP API for managing assets
   *  @addtogroup ontasset ontasset CPP API
   *  @ingroup cpp_api
   *  @
   */

   /**
    * @struct Stores information for owner of ontasset
    */

   struct ontasset {
      /**
       * The amount of the ontasset
       */
      int64_t      amount = 0;

      /**
       * Maximum amount possible for this ontasset. It's capped to 2^62 - 1
       */
      static constexpr int64_t max_amount    = (1LL << 62) - 1;

      ontasset() {}

      /**
       * Construct a new ontasset given the amount
       *
       * @param a - The amount of the ontasset
       */
      ontasset( int64_t a)
      :amount(a)
      {
         ontio::check( is_amount_within_range(), "magnitude of ontasset amount must be less than 2^62" );
      }

      /**
       * Check if the amount doesn't exceed the max amount
       *
       * @return true - if the amount doesn't exceed the max amount
       * @return false - otherwise
       */
      bool is_amount_within_range()const { return -max_amount <= amount && amount <= max_amount; }

      /**
       * Check if the ontasset is valid. %A valid ontasset has its amount <= max_amount.
       *
       * @return true - if the ontasset is valid
       * @return false - otherwise
       */
      bool is_valid()const               { return is_amount_within_range(); }

      /**
       * Set the amount of the ontasset
       *
       * @param a - New amount for the ontasset
       */
      void set_amount( int64_t a ) {
         amount = a;
         ontio::check( is_amount_within_range(), "magnitude of ontasset amount must be less than 2^62" );
      }

      /**
       * Unary minus operator
       *
       * @return ontasset - New ontasset with its amount is the negative amount of this ontasset
       */
      ontasset operator-()const {
         ontasset r = *this;
         r.amount = -r.amount;
         return r;
      }

      /**
       * Subtraction assignment operator
       *
       * @param a - Another ontasset to subtract this ontasset with
       * @return ontasset& - Reference to this ontasset
       * @post The amount of this ontasset is subtracted by the amount of ontasset a
       */
      ontasset& operator-=( const ontasset& a ) {
         amount -= a.amount;
         ontio::check( -max_amount <= amount, "subtraction underflow" );
         ontio::check( amount <= max_amount,  "subtraction overflow" );
         return *this;
      }

      /**
       * Addition Assignment  operator
       *
       * @param a - Another ontasset to subtract this ontasset with
       * @return ontasset& - Reference to this ontasset
       * @post The amount of this ontasset is added with the amount of ontasset a
       */
      ontasset& operator+=( const ontasset& a ) {
         amount += a.amount;
         ontio::check( -max_amount <= amount, "addition underflow" );
         ontio::check( amount <= max_amount,  "addition overflow" );
         return *this;
      }

      /**
       * Addition operator
       *
       * @param a - The first ontasset to be added
       * @param b - The second ontasset to be added
       * @return ontasset - New ontasset as the result of addition
       */
      inline friend ontasset operator+( const ontasset& a, const ontasset& b ) {
         ontasset result = a;
         result += b;
         return result;
      }

      /**
       * Subtraction operator
       *
       * @param a - The ontasset to be subtracted
       * @param b - The ontasset used to subtract
       * @return ontasset - New ontasset as the result of subtraction of a with b
       */
      inline friend ontasset operator-( const ontasset& a, const ontasset& b ) {
         ontasset result = a;
         result -= b;
         return result;
      }

      /**
       * @brief Multiplication assignment operator, with a number
       *
       * @details Multiplication assignment operator. Multiply the amount of this ontasset with a number and then assign the value to itself.
       * @param a - The multiplier for the ontasset's amount
       * @return ontasset - Reference to this ontasset
       * @post The amount of this ontasset is multiplied by a
       */
      ontasset& operator*=( int64_t a ) {
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
       * @param a - The ontasset to be multiplied
       * @param b - The multiplier for the ontasset's amount
       * @return ontasset - New ontasset as the result of multiplication
       */
      friend ontasset operator*( const ontasset& a, int64_t b ) {
         ontasset result = a;
         result *= b;
         return result;
      }


      /**
       * Multiplication operator, with a number preceeding
       *
       * @param a - The multiplier for the ontasset's amount
       * @param b - The ontasset to be multiplied
       * @return ontasset - New ontasset as the result of multiplication
       */
      friend ontasset operator*( int64_t b, const ontasset& a ) {
         ontasset result = a;
         result *= b;
         return result;
      }

      /**
       * @brief Division assignment operator, with a number
       *
       * @details Division assignment operator. Divide the amount of this ontasset with a number and then assign the value to itself.
       * @param a - The divisor for the ontasset's amount
       * @return ontasset - Reference to this ontasset
       * @post The amount of this ontasset is divided by a
       */
      ontasset& operator/=( int64_t a ) {
         ontio::check( a != 0, "divide by zero" );
         ontio::check( !(amount == std::numeric_limits<int64_t>::min() && a == -1), "signed division overflow" );
         amount /= a;
         return *this;
      }

      /**
       * Division operator, with a number proceeding
       *
       * @param a - The ontasset to be divided
       * @param b - The divisor for the ontasset's amount
       * @return ontasset - New ontasset as the result of division
       */
      friend ontasset operator/( const ontasset& a, int64_t b ) {
         ontasset result = a;
         result /= b;
         return result;
      }

      /**
       * Division operator, with another ontasset
       *
       * @param a - The ontasset which amount acts as the dividend
       * @param b - The ontasset which amount acts as the divisor
       * @return int64_t - the resulted amount after the division
       */
      friend int64_t operator/( const ontasset& a, const ontasset& b ) {
         ontio::check( b.amount != 0, "divide by zero" );
         return a.amount / b.amount;
      }

      /**
       * Equality operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if both ontasset has the same amount
       * @return false - otherwise
       */
      friend bool operator==( const ontasset& a, const ontasset& b ) {
         return a.amount == b.amount;
      }

      /**
       * Inequality operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if both ontasset doesn't have the same amount
       * @return false - otherwise
       */
      friend bool operator!=( const ontasset& a, const ontasset& b ) {
         return !( a == b);
      }

      /**
       * Less than operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if the first ontasset's amount is less than the second ontasset amount
       * @return false - otherwise
       */
      friend bool operator<( const ontasset& a, const ontasset& b ) {
         return a.amount < b.amount;
      }

      /**
       * Less or equal to operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if the first ontasset's amount is less or equal to the second ontasset amount
       * @return false - otherwise
       */
      friend bool operator<=( const ontasset& a, const ontasset& b ) {
         return a.amount <= b.amount;
      }

      /**
       * Greater than operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if the first ontasset's amount is greater than the second ontasset amount
       * @return false - otherwise
       */
      friend bool operator>( const ontasset& a, const ontasset& b ) {
         return a.amount > b.amount;
      }

      /**
       * Greater or equal to operator
       *
       * @param a - The first ontasset to be compared
       * @param b - The second ontasset to be compared
       * @return true - if the first ontasset's amount is greater or equal to the second ontasset amount
       * @return false - otherwise
       */
      friend bool operator>=( const ontasset& a, const ontasset& b ) {
         return a.amount >= b.amount;
      }

      ONTLIB_SERIALIZE( ontasset, (amount) )
   };
}
