/*
 * WARNING: do not edit!
 * Generated by Makefile from include/openssl/opensslconf.h.in
 *
 * Copyright 2016-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/opensslv.h>

#ifndef HEADER_OPENSSLCONF_H
# define HEADER_OPENSSLCONF_H

# ifdef  __cplusplus
extern "C" {
# endif

# ifdef OPENSSL_ALGORITHM_DEFINES
#  error OPENSSL_ALGORITHM_DEFINES no longer supported
# endif

/*
 * OpenSSL was configured with the following options:
 */

# define OPENSSL_MIN_API -1
# ifndef OPENSSL_RAND_SEED_OS
#  define OPENSSL_RAND_SEED_OS
# endif
# ifndef OPENSSL_NO_AFALGENG
#  define OPENSSL_NO_AFALGENG
# endif
# ifndef OPENSSL_NO_APPS
#  define OPENSSL_NO_APPS
# endif
# ifndef OPENSSL_NO_ARIA
#  define OPENSSL_NO_ARIA
# endif
# ifndef OPENSSL_NO_ASAN
#  define OPENSSL_NO_ASAN
# endif
# ifndef OPENSSL_NO_ASM
#  define OPENSSL_NO_ASM
# endif
# ifndef OPENSSL_NO_ASYNC
#  define OPENSSL_NO_ASYNC
# endif
# ifndef OPENSSL_NO_AUTOALGINIT
#  define OPENSSL_NO_AUTOALGINIT
# endif
# ifndef OPENSSL_NO_AUTOERRINIT
#  define OPENSSL_NO_AUTOERRINIT
# endif
# ifndef OPENSSL_NO_AUTOLOAD_CONFIG
#  define OPENSSL_NO_AUTOLOAD_CONFIG
# endif
# ifndef OPENSSL_NO_CAPIENG
#  define OPENSSL_NO_CAPIENG
# endif
# ifndef OPENSSL_NO_CAST
#  define OPENSSL_NO_CAST
# endif
# ifndef OPENSSL_NO_CHACHA
#  define OPENSSL_NO_CHACHA
# endif
# ifndef OPENSSL_NO_CRYPTO_MDEBUG
#  define OPENSSL_NO_CRYPTO_MDEBUG
# endif
# ifndef OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
#  define OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
# endif
# ifndef OPENSSL_NO_CT
#  define OPENSSL_NO_CT
# endif
# ifndef OPENSSL_NO_DEVCRYPTOENG
#  define OPENSSL_NO_DEVCRYPTOENG
# endif
# ifndef OPENSSL_NO_DGRAM
#  define OPENSSL_NO_DGRAM
# endif
# ifndef OPENSSL_NO_DTLS
#  define OPENSSL_NO_DTLS
# endif
# ifndef OPENSSL_NO_DTLS1
#  define OPENSSL_NO_DTLS1
# endif
# ifndef OPENSSL_NO_DTLS1_METHOD
#  define OPENSSL_NO_DTLS1_METHOD
# endif
# ifndef OPENSSL_NO_DTLS1_2
#  define OPENSSL_NO_DTLS1_2
# endif
# ifndef OPENSSL_NO_DTLS1_2_METHOD
#  define OPENSSL_NO_DTLS1_2_METHOD
# endif
# ifndef OPENSSL_NO_EC_NISTP_64_GCC_128
#  define OPENSSL_NO_EC_NISTP_64_GCC_128
# endif
# ifndef OPENSSL_NO_EGD
#  define OPENSSL_NO_EGD
# endif
# ifndef OPENSSL_NO_ENGINE
#  define OPENSSL_NO_ENGINE
# endif
# ifndef OPENSSL_NO_ERR
#  define OPENSSL_NO_ERR
# endif
# ifndef OPENSSL_NO_FILENAMES
#  define OPENSSL_NO_FILENAMES
# endif
# ifndef OPENSSL_NO_FUZZ_AFL
#  define OPENSSL_NO_FUZZ_AFL
# endif
# ifndef OPENSSL_NO_FUZZ_LIBFUZZER
#  define OPENSSL_NO_FUZZ_LIBFUZZER
# endif
# ifndef OPENSSL_NO_KTLS
#  define OPENSSL_NO_KTLS
# endif
# ifndef OPENSSL_NO_MD2
#  define OPENSSL_NO_MD2
# endif
# ifndef OPENSSL_NO_MSAN
#  define OPENSSL_NO_MSAN
# endif
# ifndef OPENSSL_NO_OCSP
#  define OPENSSL_NO_OCSP
# endif
# ifndef OPENSSL_NO_PADLOCKENG
#  define OPENSSL_NO_PADLOCKENG
# endif
# ifndef OPENSSL_NO_POSIX_IO
#  define OPENSSL_NO_POSIX_IO
# endif
# ifndef OPENSSL_NO_PSK
#  define OPENSSL_NO_PSK
# endif
# ifndef OPENSSL_NO_RC2
#  define OPENSSL_NO_RC2
# endif
# ifndef OPENSSL_NO_RC4
#  define OPENSSL_NO_RC4
# endif
# ifndef OPENSSL_NO_RC5
#  define OPENSSL_NO_RC5
# endif
# ifndef OPENSSL_NO_RDRAND
#  define OPENSSL_NO_RDRAND
# endif
# ifndef OPENSSL_NO_RFC3779
#  define OPENSSL_NO_RFC3779
# endif
# ifndef OPENSSL_NO_SCTP
#  define OPENSSL_NO_SCTP
# endif
# ifndef OPENSSL_NO_SEED
#  define OPENSSL_NO_SEED
# endif
# ifndef OPENSSL_NO_SOCK
#  define OPENSSL_NO_SOCK
# endif
# ifndef OPENSSL_NO_SRP
#  define OPENSSL_NO_SRP
# endif
# ifndef OPENSSL_NO_SRTP
#  define OPENSSL_NO_SRTP
# endif
# ifndef OPENSSL_NO_SSL_TRACE
#  define OPENSSL_NO_SSL_TRACE
# endif
# ifndef OPENSSL_NO_SSL3
#  define OPENSSL_NO_SSL3
# endif
# ifndef OPENSSL_NO_SSL3_METHOD
#  define OPENSSL_NO_SSL3_METHOD
# endif
# ifndef OPENSSL_NO_STDIO
#  define OPENSSL_NO_STDIO
# endif
# ifndef OPENSSL_NO_TLS
#  define OPENSSL_NO_TLS
# endif
# ifndef OPENSSL_NO_TLS1
#  define OPENSSL_NO_TLS1
# endif
# ifndef OPENSSL_NO_TLS1_METHOD
#  define OPENSSL_NO_TLS1_METHOD
# endif
# ifndef OPENSSL_NO_TLS1_1
#  define OPENSSL_NO_TLS1_1
# endif
# ifndef OPENSSL_NO_TLS1_1_METHOD
#  define OPENSSL_NO_TLS1_1_METHOD
# endif
# ifndef OPENSSL_NO_TLS1_2
#  define OPENSSL_NO_TLS1_2
# endif
# ifndef OPENSSL_NO_TLS1_2_METHOD
#  define OPENSSL_NO_TLS1_2_METHOD
# endif
# ifndef OPENSSL_NO_TLS1_3
#  define OPENSSL_NO_TLS1_3
# endif
# ifndef OPENSSL_NO_TRACE
#  define OPENSSL_NO_TRACE
# endif
# ifndef OPENSSL_NO_TS
#  define OPENSSL_NO_TS
# endif
# ifndef OPENSSL_NO_UBSAN
#  define OPENSSL_NO_UBSAN
# endif
# ifndef OPENSSL_NO_UI_CONSOLE
#  define OPENSSL_NO_UI_CONSOLE
# endif
# ifndef OPENSSL_NO_WEAK_SSL_CIPHERS
#  define OPENSSL_NO_WEAK_SSL_CIPHERS
# endif
# ifndef OPENSSL_NO_WHIRLPOOL
#  define OPENSSL_NO_WHIRLPOOL
# endif
# ifndef OPENSSL_NO_DYNAMIC_ENGINE
#  define OPENSSL_NO_DYNAMIC_ENGINE
# endif
# ifndef OPENSSL_NO_AFALGENG
#  define OPENSSL_NO_AFALGENG
# endif


/*
 * Sometimes OPENSSSL_NO_xxx ends up with an empty file and some compilers
 * don't like that.  This will hopefully silence them.
 */
# define NON_EMPTY_TRANSLATION_UNIT static void *dummy = &dummy;

/*
 * Applications should use -DOPENSSL_API_COMPAT=<version> to suppress the
 * declarations of functions deprecated in or before <version>.  If this is
 * undefined, the value of the macro OPENSSL_API_MIN above is the default.
 *
 * For any version number up until version 1.1.x, <version> is expected to be
 * the calculated version number 0xMNNFFPPSL.  For version numbers 3.0.0 and
 * on, <version> is expected to be only the major version number (i.e. 3 for
 * version 3.0.0).
 */
# ifndef DECLARE_DEPRECATED
#  define DECLARE_DEPRECATED(f)   f;
#  ifdef __GNUC__
#   if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)
#    undef DECLARE_DEPRECATED
#    define DECLARE_DEPRECATED(f)    f __attribute__ ((deprecated));
#   endif
#  endif
# endif

/*
 * We convert the OPENSSL_API_COMPAT value to an API level.  The API level
 * is the major version number for 3.0.0 and on.  For earlier versions, it
 * uses this scheme, which is close enough for our purposes:
 *
 *      0.x.y   0       (0.9.8 was the last release in this series)
 *      1.0.x   1       (1.0.2 was the last release in this series)
 *      1.1.x   2       (1.1.1 was the last release in this series)
 */

/* In case someone defined both */
# if defined(OPENSSL_API_COMPAT) && defined(OPENSSL_API_LEVEL)
#  error "Disallowed to defined both OPENSSL_API_COMPAT and OPENSSL_API_LEVEL"
# endif

# ifndef OPENSSL_API_COMPAT
#  define OPENSSL_API_LEVEL OPENSSL_MIN_API
# else
#  if (OPENSSL_API_COMPAT < 0x1000L) /* Major version numbers up to 16777215 */
#   define OPENSSL_API_LEVEL OPENSSL_API_COMPAT
#  elif (OPENSSL_API_COMPAT & 0xF0000000L) == 0x00000000L
#   define OPENSSL_API_LEVEL 0
#  elif (OPENSSL_API_COMPAT & 0xFFF00000L) == 0x10000000L
#   define OPENSSL_API_LEVEL 1
#  elif (OPENSSL_API_COMPAT & 0xFFF00000L) == 0x10100000L
#   define OPENSSL_API_LEVEL 2
#  else
/ * Major number 3 to 15 */
#   define OPENSSL_API_LEVEL ((OPENSSL_API_COMPAT >> 28) & 0xF)
#  endif
# endif

/*
 * Do not deprecate things to be deprecated in version 4.0 before the
 * OpenSSL version number matches.
 */
# if OPENSSL_VERSION_MAJOR < 4
#  define DEPRECATEDIN_4(f)       f;
#  define OPENSSL_API_4 0
# elif OPENSSL_API_LEVEL < 4
#  define DEPRECATEDIN_4(f)       DECLARE_DEPRECATED(f)
#  define OPENSSL_API_4 0
# else
#  define DEPRECATEDIN_4(f)
#  define OPENSSL_API_4 1
# endif

# if OPENSSL_API_LEVEL < 3
#  define DEPRECATEDIN_3(f)       DECLARE_DEPRECATED(f)
#  define OPENSSL_API_3 0
# else
#  define DEPRECATEDIN_3(f)
#  define OPENSSL_API_3 1
# endif

# if OPENSSL_API_LEVEL < 2
#  define DEPRECATEDIN_1_1_0(f)   DECLARE_DEPRECATED(f)
#  define OPENSSL_API_1_1_0 0
# else
#  define DEPRECATEDIN_1_1_0(f)
#  define OPENSSL_API_1_1_0 1
# endif

# if OPENSSL_API_LEVEL < 1
#  define DEPRECATEDIN_1_0_0(f)   DECLARE_DEPRECATED(f)
#  define OPENSSL_API_1_0_0 0
# else
#  define DEPRECATEDIN_1_0_0(f)
#  define OPENSSL_API_1_0_0 1
# endif

# if OPENSSL_API_LEVEL < 0
#  define DEPRECATEDIN_0_9_8(f)   DECLARE_DEPRECATED(f)
#  define OPENSSL_API_0_9_8 0
# else
#  define DEPRECATEDIN_0_9_8(f)
#  define OPENSSL_API_0_9_8 1
# endif

# ifndef OPENSSL_FILE
#  ifdef OPENSSL_NO_FILENAMES
#   define OPENSSL_FILE ""
#   define OPENSSL_LINE 0
#  else
#   define OPENSSL_FILE __FILE__
#   define OPENSSL_LINE __LINE__
#  endif
# endif

/* Generate 80386 code? */
# undef I386_ONLY

# undef OPENSSL_UNISTD
# define OPENSSL_UNISTD <unistd.h>

# undef OPENSSL_EXPORT_VAR_AS_FUNCTION

/*
 * The following are cipher-specific, but are part of the public API.
 */
# if !defined(OPENSSL_SYS_UEFI)
#  undef BN_LLONG
/* Only one for the following should be defined */
#  undef SIXTY_FOUR_BIT_LONG
#  undef SIXTY_FOUR_BIT
#  define THIRTY_TWO_BIT
# endif

# define RC4_INT unsigned int

# ifdef  __cplusplus
}
# endif
#endif                          /* HEADER_OPENSSLCONF_H */
