#pragma once
#include <tuple>
#include <limits>

namespace ontio {
   struct asset {
      int64_t      amount = 0;
      static constexpr int64_t max_amount    = (1LL << 62) - 1;

      asset() {}
      asset( int64_t a)
      :amount(a)
      {
         ontio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      bool is_amount_within_range()const { return -max_amount <= amount && amount <= max_amount; }
      bool is_valid()const               { return is_amount_within_range(); }

      void set_amount( int64_t a ) {
         amount = a;
         ontio::check( is_amount_within_range(), "magnitude of asset amount must be less than 2^62" );
      }

      asset operator-()const {
         asset r = *this;
         r.amount = -r.amount;
         return r;
      }

      asset& operator-=( const asset& a ) {
         amount -= a.amount;
         ontio::check( -max_amount <= amount, "subtraction underflow" );
         ontio::check( amount <= max_amount,  "subtraction overflow" );
         return *this;
      }

      asset& operator+=( const asset& a ) {
         amount += a.amount;
         ontio::check( -max_amount <= amount, "addition underflow" );
         ontio::check( amount <= max_amount,  "addition overflow" );
         return *this;
      }

      inline friend asset operator+( const asset& a, const asset& b ) {
         asset result = a;
         result += b;
         return result;
      }

      inline friend asset operator-( const asset& a, const asset& b ) {
         asset result = a;
         result -= b;
         return result;
      }

      asset& operator*=( int64_t a ) {
         int128_t tmp = (int128_t)amount * (int128_t)a;
         ontio::check( tmp <= max_amount, "multiplication overflow" );
         ontio::check( tmp >= -max_amount, "multiplication underflow" );
         amount = (int64_t)tmp;
         return *this;
      }

      friend asset operator*( const asset& a, int64_t b ) {
         asset result = a;
         result *= b;
         return result;
      }

      friend asset operator*( int64_t b, const asset& a ) {
         asset result = a;
         result *= b;
         return result;
      }

      asset& operator/=( int64_t a ) {
         ontio::check( a != 0, "divide by zero" );
         ontio::check( !(amount == std::numeric_limits<int64_t>::min() && a == -1), "signed division overflow" );
         amount /= a;
         return *this;
      }

      friend asset operator/( const asset& a, int64_t b ) {
         asset result = a;
         result /= b;
         return result;
      }

      friend int64_t operator/( const asset& a, const asset& b ) {
         ontio::check( b.amount != 0, "divide by zero" );
         return a.amount / b.amount;
      }

      friend bool operator==( const asset& a, const asset& b ) {
         return a.amount == b.amount;
      }

      friend bool operator!=( const asset& a, const asset& b ) {
         return !( a == b);
      }

      friend bool operator<( const asset& a, const asset& b ) {
         return a.amount < b.amount;
      }

      friend bool operator<=( const asset& a, const asset& b ) {
         return a.amount <= b.amount;
      }

      friend bool operator>( const asset& a, const asset& b ) {
         return a.amount > b.amount;
      }

      friend bool operator>=( const asset& a, const asset& b ) {
         return a.amount >= b.amount;
      }

      ONTLIB_SERIALIZE( asset, (amount))
   };
}
