//  Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
/*
   xxHash - Extremely Fast Hash algorithm
   Header File
   Copyright (C) 2012-2016, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/

// This is a fork of a preview version of xxHash, as RocksDB depends on
// this preview version of XXH3. To allow this to coexist with the
// standard xxHash, including in the "unity" build where all source files
// and headers go into a single translation unit, here "XXH" has been
// replaced with "XXPH" for XX Preview Hash.

#ifndef XXPHASH_H_5627135585666179
#define XXPHASH_H_5627135585666179 1

/* BEGIN RocksDB customizations */
#ifndef XXPH_STATIC_LINKING_ONLY
// Access experimental APIs
#define XXPH_STATIC_LINKING_ONLY 1
#endif
#define XXPH_NAMESPACE ROCKSDB_
#define XXPH_INLINE_ALL
#include <cstring>
/* END RocksDB customizations */

#include "rocksdb-rs/src/xxph3.rs.h"

// clang-format off
#if defined (__cplusplus)
extern "C" {
#endif


/* ****************************
*  Definitions
******************************/
#include <stddef.h>   /* size_t */
typedef enum { XXPH_OK=0, XXPH_ERROR } XXPH_errorcode;


/* ****************************
 *  API modifier
 ******************************/
/** XXPH_INLINE_ALL (and XXPH_PRIVATE_API)
 *  This build macro includes xxhash functions in `static` mode
 *  in order to inline them, and remove their symbol from the public list.
 *  Inlining offers great performance improvement on small keys,
 *  and dramatic ones when length is expressed as a compile-time constant.
 *  See https://fastcompression.blogspot.com/2018/03/xxhash-for-small-keys-impressive-power.html .
 *  Methodology :
 *     #define XXPH_INLINE_ALL
 *     #include "xxhash.h"
 * `xxhash.c` is automatically included.
 *  It's not useful to compile and link it as a separate object.
 */
#if defined(XXPH_INLINE_ALL) || defined(XXPH_PRIVATE_API)
#  ifndef XXPH_STATIC_LINKING_ONLY
#    define XXPH_STATIC_LINKING_ONLY
#  endif
#  if defined(__GNUC__)
#    define XXPH_PUBLIC_API static __inline __attribute__((unused))
#  elif defined (__cplusplus) || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */)
#    define XXPH_PUBLIC_API static inline
#  elif defined(_MSC_VER)
#    define XXPH_PUBLIC_API static __inline
#  else
     /* this version may generate warnings for unused static functions */
#    define XXPH_PUBLIC_API static
#  endif
#else
#  if defined(WIN32) && defined(_MSC_VER) && (defined(XXPH_IMPORT) || defined(XXPH_EXPORT))
#    ifdef XXPH_EXPORT
#      define XXPH_PUBLIC_API __declspec(dllexport)
#    elif XXPH_IMPORT
#      define XXPH_PUBLIC_API __declspec(dllimport)
#    endif
#  else
#    define XXPH_PUBLIC_API   /* do nothing */
#  endif
#endif /* XXPH_INLINE_ALL || XXPH_PRIVATE_API */

/*! XXPH_NAMESPACE, aka Namespace Emulation :
 *
 * If you want to include _and expose_ xxHash functions from within your own library,
 * but also want to avoid symbol collisions with other libraries which may also include xxHash,
 *
 * you can use XXPH_NAMESPACE, to automatically prefix any public symbol from xxhash library
 * with the value of XXPH_NAMESPACE (therefore, avoid NULL and numeric values).
 *
 * Note that no change is required within the calling program as long as it includes `xxhash.h` :
 * regular symbol name will be automatically translated by this header.
 */
#ifdef XXPH_NAMESPACE
#  define XXPH_CAT(A,B) A##B
#  define XXPH_NAME2(A,B) XXPH_CAT(A,B)
#  define XXPH_versionNumber XXPH_NAME2(XXPH_NAMESPACE, XXPH_versionNumber)
#endif


/* *************************************
*  Version
***************************************/
#define XXPH_VERSION_MAJOR    0
#define XXPH_VERSION_MINOR    7
#define XXPH_VERSION_RELEASE  2
#define XXPH_VERSION_NUMBER  (XXPH_VERSION_MAJOR *100*100 + XXPH_VERSION_MINOR *100 + XXPH_VERSION_RELEASE)
XXPH_PUBLIC_API unsigned XXPH_versionNumber (void);


/*-**********************************************************************
*  32-bit hash
************************************************************************/
#if !defined (__VMS) \
  && (defined (__cplusplus) \
  || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */) )
#   include <stdint.h>
    typedef uint32_t XXPH32_hash_t;
#else
#   include <limits.h>
#   if UINT_MAX == 0xFFFFFFFFUL
      typedef unsigned int XXPH32_hash_t;
#   else
#     if ULONG_MAX == 0xFFFFFFFFUL
        typedef unsigned long XXPH32_hash_t;
#     else
#       error "unsupported platform : need a 32-bit type"
#     endif
#   endif
#endif

#ifndef XXPH_NO_LONG_LONG
/*-**********************************************************************
*  64-bit hash
************************************************************************/
#if !defined (__VMS) \
  && (defined (__cplusplus) \
  || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */) )
#   include <stdint.h>
    typedef uint64_t XXPH64_hash_t;
#else
    /* the following type must have a width of 64-bit */
    typedef unsigned long long XXPH64_hash_t;
#endif

#endif  /* XXPH_NO_LONG_LONG */



#ifdef XXPH_STATIC_LINKING_ONLY

/* ================================================================================================
   This section contains declarations which are not guaranteed to remain stable.
   They may change in future versions, becoming incompatible with a different version of the library.
   These declarations should only be used with static linking.
   Never use them in association with dynamic linking !
=================================================================================================== */


/*-**********************************************************************
*  XXPH3
*  New experimental hash
************************************************************************/
#ifndef XXPH_NO_LONG_LONG


/* ============================================
 * XXPH3 is a new hash algorithm,
 * featuring improved speed performance for both small and large inputs.
 * See full speed analysis at : http://fastcompression.blogspot.com/2019/03/presenting-xxh3.html
 * In general, expect XXPH3 to run about ~2x faster on large inputs,
 * and >3x faster on small ones, though exact differences depend on platform.
 *
 * The algorithm is portable, will generate the same hash on all platforms.
 * It benefits greatly from vectorization units, but does not require it.
 *
 * XXPH3 offers 2 variants, _64bits and _128bits.
 * When only 64 bits are needed, prefer calling the _64bits variant :
 * it reduces the amount of mixing, resulting in faster speed on small inputs.
 * It's also generally simpler to manipulate a scalar return type than a struct.
 *
 * The XXPH3 algorithm is still considered experimental.
 * Produced results can still change between versions.
 * Results produced by v0.7.x are not comparable with results from v0.7.y .
 * It's nonetheless possible to use XXPH3 for ephemeral data (local sessions),
 * but avoid storing values in long-term storage for later reads.
 *
 * The API supports one-shot hashing, streaming mode, and custom secrets.
 *
 * There are still a number of opened questions that community can influence during the experimental period.
 * I'm trying to list a few of them below, though don't consider this list as complete.
 *
 * - 128-bits output type : currently defined as a structure of two 64-bits fields.
 *                          That's because 128-bit values do not exist in C standard.
 *                          Note that it means that, at byte level, result is not identical depending on endianess.
 *                          However, at field level, they are identical on all platforms.
 *                          The canonical representation solves the issue of identical byte-level representation across platforms,
 *                          which is necessary for serialization.
 *                          Q1 : Would there be a better representation for a 128-bit hash result ?
 *                          Q2 : Are the names of the inner 64-bit fields important ? Should they be changed ?
 *
 * - Prototype XXPH128() :   XXPH128() uses the same arguments as XXPH64(), for consistency.
 *                          It means it maps to XXPH3_128bits_withSeed().
 *                          This variant is slightly slower than XXPH3_128bits(),
 *                          because the seed is now part of the algorithm, and can't be simplified.
 *                          Is that a good idea ?
 *
 * - Seed type for XXPH128() : currently, it's a single 64-bit value, like the 64-bit variant.
 *                          It could be argued that it's more logical to offer a 128-bit seed input parameter for a 128-bit hash.
 *                          But 128-bit seed is more difficult to use, since it requires to pass a structure instead of a scalar value.
 *                          Such a variant could either replace current one, or become an additional one.
 *                          Farmhash, for example, offers both variants (the 128-bits seed variant is called `doubleSeed`).
 *                          Follow up question : if both 64-bit and 128-bit seeds are allowed, which variant should be called XXPH128 ?
 *
 * - Result for len==0 :    Currently, the result of hashing a zero-length input is always `0`.
 *                          It seems okay as a return value when using "default" secret and seed.
 *                          But is it still fine to return `0` when secret or seed are non-default ?
 *                          Are there use cases which could depend on generating a different hash result for zero-length input when the secret is different ?
 *
 * - Consistency (1) :      Streaming XXPH128 uses an XXPH3 state, which is the same state as XXPH3_64bits().
 *                          It means a 128bit streaming loop must invoke the following symbols :
 *                          XXPH3_createState(), XXPH3_128bits_reset(), XXPH3_128bits_update() (loop), XXPH3_128bits_digest(), XXPH3_freeState().
 *                          Is that consistent enough ?
 *
 * - Consistency (2) :      The canonical representation of `XXPH3_64bits` is provided by existing functions
 *                          XXPH64_canonicalFromHash(), and reverse operation XXPH64_hashFromCanonical().
 *                          As a mirror, canonical functions for XXPH128_hash_t results generated by `XXPH3_128bits`
 *                          are XXPH128_canonicalFromHash() and XXPH128_hashFromCanonical().
 *                          Which means, `XXPH3` doesn't appear in the names, because canonical functions operate on a type,
 *                          independently of which algorithm was used to generate that type.
 *                          Is that consistent enough ?
 */

#ifdef XXPH_NAMESPACE
#  define XXPH3_64bits XXPH_NAME2(XXPH_NAMESPACE, XXPH3_64bits)
#  define XXPH3_64bits_withSeed XXPH_NAME2(XXPH_NAMESPACE, XXPH3_64bits_withSeed)
#endif

/* XXPH3_64bits() :
 * default 64-bit variant, using default secret and default seed of 0.
 * It's the fastest variant. */
XXPH_PUBLIC_API XXPH64_hash_t XXPH3_64bits(rust::Slice<const uint8_t> data);

/* XXPH3_64bits_withSeed() :
 * This variant generates on the fly a custom secret,
 * based on the default secret, altered using the `seed` value.
 * While this operation is decently fast, note that it's not completely free.
 * note : seed==0 produces same results as XXPH3_64bits() */
XXPH_PUBLIC_API XXPH64_hash_t XXPH3_64bits_withSeed(rust::Slice<const uint8_t> data, XXPH64_hash_t seed);

#if defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)   /* C11+ */
#  include <stdalign.h>
#  define XXPH_ALIGN(n)      alignas(n)
#elif defined(__GNUC__)
#  define XXPH_ALIGN(n)      __attribute__ ((aligned(n)))
#elif defined(_MSC_VER)
#  define XXPH_ALIGN(n)      __declspec(align(n))
#else
#  define XXPH_ALIGN(n)   /* disabled */
#endif

#define XXPH3_SECRET_DEFAULT_SIZE 192   /* minimum XXPH3_SECRET_SIZE_MIN */

#endif  /* XXPH_NO_LONG_LONG */


/*-**********************************************************************
*  XXPH_INLINE_ALL
************************************************************************/
#if defined(XXPH_INLINE_ALL) || defined(XXPH_PRIVATE_API)

/* === RocksDB modification: was #include here but permanently inlining === */

typedef struct {
    XXPH64_hash_t low64;
    XXPH64_hash_t high64;
} XXPH128_hash_t;

/* *************************************
*  Tuning parameters
***************************************/
/*!XXPH_FORCE_MEMORY_ACCESS :
 * By default, access to unaligned memory is controlled by `memcpy()`, which is safe and portable.
 * Unfortunately, on some target/compiler combinations, the generated assembly is sub-optimal.
 * The below switch allow to select different access method for improved performance.
 * Method 0 (default) : use `memcpy()`. Safe and portable.
 * Method 1 : `__packed` statement. It depends on compiler extension (ie, not portable).
 *            This method is safe if your compiler supports it, and *generally* as fast or faster than `memcpy`.
 * Method 2 : direct access. This method doesn't depend on compiler but violate C standard.
 *            It can generate buggy code on targets which do not support unaligned memory accesses.
 *            But in some circumstances, it's the only known way to get the most performance (ie GCC + ARMv6)
 * See http://stackoverflow.com/a/32095106/646947 for details.
 * Prefer these methods in priority order (0 > 1 > 2)
 */
#ifndef XXPH_FORCE_MEMORY_ACCESS   /* can be defined externally, on command line for example */
#  if !defined(__clang__) && defined(__GNUC__) && defined(__ARM_FEATURE_UNALIGNED) && defined(__ARM_ARCH) && (__ARM_ARCH == 6)
#    define XXPH_FORCE_MEMORY_ACCESS 2
#  elif !defined(__clang__) && ((defined(__INTEL_COMPILER) && !defined(_WIN32)) || \
  (defined(__GNUC__) && (defined(__ARM_ARCH) && __ARM_ARCH >= 7)))
#    define XXPH_FORCE_MEMORY_ACCESS 1
#  endif
#endif

/*!XXPH_ACCEPT_NULL_INPUT_POINTER :
 * If input pointer is NULL, xxHash default behavior is to dereference it, triggering a segfault.
 * When this macro is enabled, xxHash actively checks input for null pointer.
 * It it is, result for null input pointers is the same as a null-length input.
 */
#ifndef XXPH_ACCEPT_NULL_INPUT_POINTER   /* can be defined externally */
#  define XXPH_ACCEPT_NULL_INPUT_POINTER 0
#endif

/*!XXPH_FORCE_ALIGN_CHECK :
 * This is a minor performance trick, only useful with lots of very small keys.
 * It means : check for aligned/unaligned input.
 * The check costs one initial branch per hash;
 * set it to 0 when the input is guaranteed to be aligned,
 * or when alignment doesn't matter for performance.
 */
#ifndef XXPH_FORCE_ALIGN_CHECK /* can be defined externally */
#  if defined(__i386) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_X64)
#    define XXPH_FORCE_ALIGN_CHECK 0
#  else
#    define XXPH_FORCE_ALIGN_CHECK 1
#  endif
#endif

/*!XXPH_REROLL:
 * Whether to reroll XXPH32_finalize, and XXPH64_finalize,
 * instead of using an unrolled jump table/if statement loop.
 *
 * This is automatically defined on -Os/-Oz on GCC and Clang. */
#ifndef XXPH_REROLL
#  if defined(__OPTIMIZE_SIZE__)
#    define XXPH_REROLL 1
#  else
#    define XXPH_REROLL 0
#  endif
#endif

#include <limits.h>   /* ULLONG_MAX */

#ifndef XXPH_STATIC_LINKING_ONLY
#define XXPH_STATIC_LINKING_ONLY
#endif

/* BEGIN RocksDB customizations */
#include "port/lang.h" /* for FALLTHROUGH_INTENDED, inserted as appropriate */
/* END RocksDB customizations */

/* *************************************
*  Compiler Specific Options
***************************************/
#ifdef _MSC_VER    /* Visual Studio */
#  pragma warning(disable : 4127)      /* disable: C4127: conditional expression is constant */
#  define XXPH_FORCE_INLINE static __forceinline
#  define XXPH_NO_INLINE static __declspec(noinline)
#else
#  if defined (__cplusplus) || defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* C99 */
#    ifdef __GNUC__
#      define XXPH_FORCE_INLINE static inline __attribute__((always_inline))
#      define XXPH_NO_INLINE static __attribute__((noinline))
#    else
#      define XXPH_FORCE_INLINE static inline
#      define XXPH_NO_INLINE static
#    endif
#  else
#    define XXPH_FORCE_INLINE static
#    define XXPH_NO_INLINE static
#  endif /* __STDC_VERSION__ */
#endif



/* *************************************
*  Debug
***************************************/
/* DEBUGLEVEL is expected to be defined externally,
 * typically through compiler command line.
 * Value must be a number. */
#ifndef DEBUGLEVEL
#  define DEBUGLEVEL 0
#endif

#if (DEBUGLEVEL>=1)
#  include <assert.h>   /* note : can still be disabled with NDEBUG */
#  define XXPH_ASSERT(c)   assert(c)
#else
#  define XXPH_ASSERT(c)   ((void)0)
#endif

/* note : use after variable declarations */
#define XXPH_STATIC_ASSERT(c)  { enum { XXPH_sa = 1/(int)(!!(c)) }; }


/* *************************************
*  Basic Types
***************************************/
#if !defined (__VMS) \
 && (defined (__cplusplus) \
 || (defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) /* C99 */) )
# include <stdint.h>
  typedef uint8_t  xxh_u8;
#else
  typedef unsigned char      xxh_u8;
#endif
typedef XXPH32_hash_t xxh_u32;


/* ===   Endianess   === */

/* XXPH_CPU_LITTLE_ENDIAN can be defined externally, for example on the compiler command line */
#ifndef XXPH_CPU_LITTLE_ENDIAN
#  if defined(_WIN32) /* Windows is always little endian */ \
     || defined(__LITTLE_ENDIAN__) \
     || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#    define XXPH_CPU_LITTLE_ENDIAN 1
#  elif defined(__BIG_ENDIAN__) \
     || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#    define XXPH_CPU_LITTLE_ENDIAN 0
#  else
static int XXPH_isLittleEndian(void)
{
    const union { xxh_u32 u; xxh_u8 c[4]; } one = { 1 };   /* don't use static : performance detrimental  */
    return one.c[0];
}
#   define XXPH_CPU_LITTLE_ENDIAN   XXPH_isLittleEndian()
#  endif
#endif




/* ****************************************
*  Compiler-specific Functions and Macros
******************************************/
#define XXPH_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#ifndef __has_builtin
#  define __has_builtin(x) 0
#endif

#if !defined(NO_CLANG_BUILTIN) && __has_builtin(__builtin_rotateleft32) && __has_builtin(__builtin_rotateleft64)
#  define XXPH_rotl32 __builtin_rotateleft32
#  define XXPH_rotl64 __builtin_rotateleft64
/* Note : although _rotl exists for minGW (GCC under windows), performance seems poor */
#elif defined(_MSC_VER)
#  define XXPH_rotl32(x,r) _rotl(x,r)
#  define XXPH_rotl64(x,r) _rotl64(x,r)
#else
#  define XXPH_rotl32(x,r) (((x) << (r)) | ((x) >> (32 - (r))))
#  define XXPH_rotl64(x,r) (((x) << (r)) | ((x) >> (64 - (r))))
#endif

#if defined(_MSC_VER)     /* Visual Studio */
#  define XXPH_swap32 _byteswap_ulong
#elif XXPH_GCC_VERSION >= 403
#  define XXPH_swap32 __builtin_bswap32
#else
static xxh_u32 XXPH_swap32 (xxh_u32 x)
{
    return  ((x << 24) & 0xff000000 ) |
            ((x <<  8) & 0x00ff0000 ) |
            ((x >>  8) & 0x0000ff00 ) |
            ((x >> 24) & 0x000000ff );
}
#endif


/* ***************************
*  Memory reads
*****************************/
typedef enum { XXPH_aligned, XXPH_unaligned } XXPH_alignment;

XXPH_FORCE_INLINE xxh_u32
XXPH_readLE32_align(const void* ptr, XXPH_alignment align)
{
    if (align==XXPH_unaligned) {
        return xxph::xxph_read_le32(rust::Slice(static_cast<const uint8_t*>(ptr), 4));
    } else {
        return XXPH_CPU_LITTLE_ENDIAN ? *static_cast<const xxh_u32 *>(ptr) : XXPH_swap32(*static_cast<const xxh_u32 *>(ptr));
    }
}


/* *************************************
*  Misc
***************************************/
XXPH_PUBLIC_API unsigned XXPH_versionNumber (void) { return XXPH_VERSION_NUMBER; }


static const xxh_u32 PRIME32_1 = 0x9E3779B1U;   /* 0b10011110001101110111100110110001 */
static const xxh_u32 PRIME32_2 = 0x85EBCA77U;   /* 0b10000101111010111100101001110111 */
static const xxh_u32 PRIME32_3 = 0xC2B2AE3DU;   /* 0b11000010101100101010111000111101 */
static const xxh_u32 PRIME32_4 = 0x27D4EB2FU;   /* 0b00100111110101001110101100101111 */
static const xxh_u32 PRIME32_5 = 0x165667B1U;   /* 0b00010110010101100110011110110001 */

#ifndef XXPH_NO_LONG_LONG

/* *******************************************************************
*  64-bit hash functions
*********************************************************************/

/*======   Memory access   ======*/

typedef XXPH64_hash_t xxh_u64;

#if defined(_MSC_VER)     /* Visual Studio */
#  define XXPH_swap64 _byteswap_uint64
#elif XXPH_GCC_VERSION >= 403
#  define XXPH_swap64 __builtin_bswap64
#else
static xxh_u64 XXPH_swap64 (xxh_u64 x)
{
    return  ((x << 56) & 0xff00000000000000ULL) |
            ((x << 40) & 0x00ff000000000000ULL) |
            ((x << 24) & 0x0000ff0000000000ULL) |
            ((x << 8)  & 0x000000ff00000000ULL) |
            ((x >> 8)  & 0x00000000ff000000ULL) |
            ((x >> 24) & 0x0000000000ff0000ULL) |
            ((x >> 40) & 0x000000000000ff00ULL) |
            ((x >> 56) & 0x00000000000000ffULL);
}
#endif

XXPH_FORCE_INLINE xxh_u64
XXPH_readLE64_align(const void* ptr, XXPH_alignment align)
{
    if (align==XXPH_unaligned)
        return xxph::xxph_read_le64(rust::Slice(static_cast<const uint8_t*>(ptr), 8));
    else
        return XXPH_CPU_LITTLE_ENDIAN ? *static_cast<const xxh_u64 *>(ptr) : XXPH_swap64(*static_cast<const xxh_u64 *>(ptr));
}


/*======   xxh64   ======*/

static const xxh_u64 PRIME64_1 = 0x9E3779B185EBCA87ULL;   /* 0b1001111000110111011110011011000110000101111010111100101010000111 */
static const xxh_u64 PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;   /* 0b1100001010110010101011100011110100100111110101001110101101001111 */
static const xxh_u64 PRIME64_3 = 0x165667B19E3779F9ULL;   /* 0b0001011001010110011001111011000110011110001101110111100111111001 */
static const xxh_u64 PRIME64_4 = 0x85EBCA77C2B2AE63ULL;   /* 0b1000010111101011110010100111011111000010101100101010111001100011 */
static const xxh_u64 PRIME64_5 = 0x27D4EB2F165667C5ULL;   /* 0b0010011111010100111010110010111100010110010101100110011111000101 */


/* *********************************************************************
*  XXPH3
*  New generation hash designed for speed on small keys and vectorization
************************************************************************ */

/*======== Was #include "xxh3.h", now inlined below ==========*/

/*
   xxHash - Extremely Fast Hash algorithm
   Development source file for `xxh3`
   Copyright (C) 2019-present, Yann Collet.

   BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

       * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above
   copyright notice, this list of conditions and the following disclaimer
   in the documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   You can contact the author at :
   - xxHash source repository : https://github.com/Cyan4973/xxHash
*/

/* RocksDB Note: This file contains a preview release (xxhash repository
   version 0.7.2) of XXPH3 that is unlikely to be compatible with the final
   version of XXPH3. We have therefore renamed this XXPH3 ("preview"), for
   clarity so that we can continue to use this version even after
   integrating a newer incompatible version.
*/

/* ===   Dependencies   === */

#undef XXPH_INLINE_ALL   /* in case it's already defined */
#define XXPH_INLINE_ALL


/* ===   Compiler specifics   === */

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* >= C99 */
#  define XXPH_RESTRICT   restrict
#else
/* note : it might be useful to define __restrict or __restrict__ for some C++ compilers */
#  define XXPH_RESTRICT   /* disable */
#endif

#if defined(__GNUC__)
#  if defined(__AVX2__)
#    include <immintrin.h>
#  elif defined(__SSE2__)
#    include <emmintrin.h>
#  elif defined(__ARM_NEON__) || defined(__ARM_NEON)
#    define inline __inline__  /* clang bug */
#    include <arm_neon.h>
#    undef inline
#  endif
#elif defined(_MSC_VER)
#  include <intrin.h>
#endif

/*
 * Sanity check.
 *
 * XXPH3 only requires these features to be efficient:
 *
 *  - Usable unaligned access
 *  - A 32-bit or 64-bit ALU
 *      - If 32-bit, a decent ADC instruction
 *  - A 32 or 64-bit multiply with a 64-bit result
 *
 * Almost all 32-bit and 64-bit targets meet this, except for Thumb-1, the
 * classic 16-bit only subset of ARM's instruction set.
 *
 * First of all, Thumb-1 lacks support for the UMULL instruction which
 * performs the important long multiply. This means numerous __aeabi_lmul
 * calls.
 *
 * Second of all, the 8 functional registers are just not enough.
 * Setup for __aeabi_lmul, byteshift loads, pointers, and all arithmetic need
 * Lo registers, and this shuffling results in thousands more MOVs than A32.
 *
 * A32 and T32 don't have this limitation. They can access all 14 registers,
 * do a 32->64 multiply with UMULL, and the flexible operand is helpful too.
 *
 * If compiling Thumb-1 for a target which supports ARM instructions, we
 * will give a warning.
 *
 * Usually, if this happens, it is because of an accident and you probably
 * need to specify -march, as you probably meant to compileh for a newer
 * architecture.
 */
#if defined(__thumb__) && !defined(__thumb2__) && defined(__ARM_ARCH_ISA_ARM)
#   warning "XXPH3 is highly inefficient without ARM or Thumb-2."
#endif

/* ==========================================
 * Vectorization detection
 * ========================================== */
#define XXPH_SCALAR 0
#define XXPH_SSE2   1
#define XXPH_AVX2   2
#define XXPH_NEON   3
#define XXPH_VSX    4

#ifndef XXPH_VECTOR    /* can be defined on command line */
#  if defined(__AVX2__)
#    define XXPH_VECTOR XXPH_AVX2
#  elif defined(__SSE2__) || defined(_M_AMD64) || defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP == 2))
#    define XXPH_VECTOR XXPH_SSE2
#  elif defined(__GNUC__) /* msvc support maybe later */ \
  && (defined(__ARM_NEON__) || defined(__ARM_NEON)) \
  && (defined(__LITTLE_ENDIAN__) /* We only support little endian NEON */ \
    || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
#    define XXPH_VECTOR XXPH_NEON
#  elif defined(__PPC64__) && defined(__POWER8_VECTOR__) && defined(__GNUC__)
#    define XXPH_VECTOR XXPH_VSX
#  else
#    define XXPH_VECTOR XXPH_SCALAR
#  endif
#endif

/* control alignment of accumulator,
 * for compatibility with fast vector loads */
#ifndef XXPH_ACC_ALIGN
#  if XXPH_VECTOR == 0   /* scalar */
#     define XXPH_ACC_ALIGN 8
#  elif XXPH_VECTOR == 1  /* sse2 */
#     define XXPH_ACC_ALIGN 16
#  elif XXPH_VECTOR == 2  /* avx2 */
#     define XXPH_ACC_ALIGN 32
#  elif XXPH_VECTOR == 3  /* neon */
#     define XXPH_ACC_ALIGN 16
#  elif XXPH_VECTOR == 4  /* vsx */
#     define XXPH_ACC_ALIGN 16
#  endif
#endif

/* xxh_u64 XXPH_mult32to64(xxh_u32 a, xxh_u64 b) { return (xxh_u64)a * (xxh_u64)b; } */
#if defined(_MSC_VER) && defined(_M_IX86)
#    include <intrin.h>
#    define XXPH_mult32to64(x, y) __emulu(x, y)
#else
#    define XXPH_mult32to64(x, y) ((xxh_u64)((x) & 0xFFFFFFFF) * (xxh_u64)((y) & 0xFFFFFFFF))
#endif

/* VSX stuff. It's a lot because VSX support is mediocre across compilers and
 * there is a lot of mischief with endianness. */
#if XXPH_VECTOR == XXPH_VSX
#  include <altivec.h>
#  undef vector
typedef __vector unsigned long long U64x2;
typedef __vector unsigned char U8x16;
typedef __vector unsigned U32x4;

#ifndef XXPH_VSX_BE
#  if defined(__BIG_ENDIAN__) \
  || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#    define XXPH_VSX_BE 1
#  elif defined(__VEC_ELEMENT_REG_ORDER__) && __VEC_ELEMENT_REG_ORDER__ == __ORDER_BIG_ENDIAN__
#    warning "-maltivec=be is not recommended. Please use native endianness."
#    define XXPH_VSX_BE 1
#  else
#    define XXPH_VSX_BE 0
#  endif
#endif

/* We need some helpers for big endian mode. */
#if XXPH_VSX_BE
/* A wrapper for POWER9's vec_revb. */
#  ifdef __POWER9_VECTOR__
#    define XXPH_vec_revb vec_revb
#  else
XXPH_FORCE_INLINE U64x2 XXPH_vec_revb(U64x2 val)
{
    U8x16 const vByteSwap = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
                              0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08 };
    return vec_perm(val, val, vByteSwap);
}
#  endif

/* Power8 Crypto gives us vpermxor which is very handy for
 * PPC64EB.
 *
 * U8x16 vpermxor(U8x16 a, U8x16 b, U8x16 mask)
 * {
 *     U8x16 ret;
 *     for (int i = 0; i < 16; i++) {
 *         ret[i] = a[mask[i] & 0xF] ^ b[mask[i] >> 4];
 *     }
 *     return ret;
 * }
 *
 * Because both of the main loops load the key, swap, and xor it with input,
 * we can combine the key swap into this instruction.
 */
#  ifdef vec_permxor
#    define XXPH_vec_permxor vec_permxor
#  else
#    define XXPH_vec_permxor __builtin_crypto_vpermxor
#  endif
#endif  /* XXPH_VSX_BE */
/*
 * Because we reinterpret the multiply, there are endian memes: vec_mulo actually becomes
 * vec_mule.
 *
 * Additionally, the intrinsic wasn't added until GCC 8, despite existing for a while.
 * Clang has an easy way to control this, we can just use the builtin which doesn't swap.
 * GCC needs inline assembly. */
#if __has_builtin(__builtin_altivec_vmuleuw)
#  define XXPH_vec_mulo __builtin_altivec_vmulouw
#  define XXPH_vec_mule __builtin_altivec_vmuleuw
#else
/* Adapted from https://github.com/google/highwayhash/blob/master/highwayhash/hh_vsx.h. */
XXPH_FORCE_INLINE U64x2 XXPH_vec_mulo(U32x4 a, U32x4 b) {
    U64x2 result;
    __asm__("vmulouw %0, %1, %2" : "=v" (result) : "v" (a), "v" (b));
    return result;
}
XXPH_FORCE_INLINE U64x2 XXPH_vec_mule(U32x4 a, U32x4 b) {
    U64x2 result;
    __asm__("vmuleuw %0, %1, %2" : "=v" (result) : "v" (a), "v" (b));
    return result;
}
#endif /* __has_builtin(__builtin_altivec_vmuleuw) */
#endif /* XXPH_VECTOR == XXPH_VSX */

/* prefetch
 * can be disabled, by declaring XXPH_NO_PREFETCH build macro */
#if defined(XXPH_NO_PREFETCH)
#  define XXPH_PREFETCH(ptr)  (void)(ptr)  /* disabled */
#else
#if defined(_MSC_VER) && \
    (defined(_M_X64) ||  \
     defined(_M_IX86)) /* _mm_prefetch() is not defined outside of x86/x64 */
#    include <mmintrin.h>   /* https://msdn.microsoft.com/fr-fr/library/84szxsww(v=vs.90).aspx */
#    define XXPH_PREFETCH(ptr)  _mm_prefetch((const char*)(ptr), _MM_HINT_T0)
#  elif defined(__GNUC__) && ( (__GNUC__ >= 4) || ( (__GNUC__ == 3) && (__GNUC_MINOR__ >= 1) ) )
#    define XXPH_PREFETCH(ptr)  __builtin_prefetch((ptr), 0 /* rw==read */, 3 /* locality */)
#  else
#    define XXPH_PREFETCH(ptr) (void)(ptr)  /* disabled */
#  endif
#endif  /* XXPH_NO_PREFETCH */


/* ==========================================
 * XXPH3 default settings
 * ========================================== */

#define XXPH_SECRET_DEFAULT_SIZE 192   /* minimum XXPH3_SECRET_SIZE_MIN */

#if (XXPH_SECRET_DEFAULT_SIZE < XXPH3_SECRET_SIZE_MIN)
#  error "default keyset is not large enough"
#endif

XXPH_ALIGN(64) static const xxh_u8 kSecret[XXPH_SECRET_DEFAULT_SIZE] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,

    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};

/* ==========================================
 * Short keys
 * ========================================== */

/* ===    Long Keys    === */

#define STRIPE_LEN 64
#define XXPH_SECRET_CONSUME_RATE 8   /* nb of secret bytes consumed at each accumulation */
#define ACC_NB (STRIPE_LEN / sizeof(xxh_u64))

typedef enum { XXPH3_acc_64bits, XXPH3_acc_128bits } XXPH3_accWidth_e;

#define XXPH_PREFETCH_DIST 384

static XXPH64_hash_t
XXPH3_mergeAccs(rust::Slice<const uint64_t> XXPH_RESTRICT acc, rust::Slice<const uint8_t> XXPH_RESTRICT secret, xxh_u64 start)
{
    xxh_u64 result64 = start;

    result64 += xxph::xxph3_mix2_accs(acc, secret);
    result64 += xxph::xxph3_mix2_accs(
        rust::Slice(acc.data()+2, acc.length() - 2),
        rust::Slice(secret.data() + 16, secret.length() - 16));
    result64 += xxph::xxph3_mix2_accs(
        rust::Slice(acc.data()+4, acc.length() - 4),
        rust::Slice(secret.data() + 32, secret.length() - 32));
    result64 += xxph::xxph3_mix2_accs(
        rust::Slice(acc.data()+6, acc.length() - 6),
        rust::Slice(secret.data() + 48, secret.length() - 48));

    return xxph::xxph3_avalanche(result64);
}

#define XXPH3_INIT_ACC { PRIME32_3, PRIME64_1, PRIME64_2, PRIME64_3, \
                        PRIME64_4, PRIME32_2, PRIME64_5, PRIME32_1 };

XXPH_FORCE_INLINE XXPH64_hash_t
XXPH3_hashLong_internal(rust::Slice<const uint8_t> XXPH_RESTRICT input,
                       rust::Slice<const uint8_t> XXPH_RESTRICT secret)
{
    XXPH_ALIGN(XXPH_ACC_ALIGN) xxh_u64 acc[ACC_NB] = XXPH3_INIT_ACC;

    xxph::xxph3_hash_long_internal_loop(rust::Slice(acc, sizeof(acc)), input, secret);

    /* converge into final hash */
    XXPH_STATIC_ASSERT(sizeof(acc) == 64);
#define XXPH_SECRET_MERGEACCS_START 11  /* do not align on 8, so that secret is different from accumulator */
    XXPH_ASSERT(secretSize >= sizeof(acc) + XXPH_SECRET_MERGEACCS_START);
    return XXPH3_mergeAccs(
        rust::Slice(static_cast<const uint64_t*>(acc), sizeof(acc)),
        rust::Slice(secret.data() + XXPH_SECRET_MERGEACCS_START, secret.length() - XXPH_SECRET_MERGEACCS_START),
        static_cast<xxh_u64>(input.length()) * PRIME64_1);
}


XXPH_NO_INLINE XXPH64_hash_t    /* It's important for performance that XXPH3_hashLong is not inlined. Not sure why (uop cache maybe ?), but difference is large and easily measurable */
XXPH3_hashLong_64b_defaultSecret(rust::Slice<const uint8_t> XXPH_RESTRICT input)
{
    return XXPH3_hashLong_internal(input, rust::Slice(kSecret, sizeof(kSecret)));
}

XXPH_FORCE_INLINE void XXPH_writeLE64(rust::Slice<uint8_t> dst, xxh_u64 v64)
{
    if (!XXPH_CPU_LITTLE_ENDIAN) v64 = XXPH_swap64(v64);
    memcpy(dst.data(), &v64, sizeof(v64));
}

/* XXPH3_initCustomSecret() :
 * destination `customSecret` is presumed allocated and same size as `kSecret`.
 */
XXPH_FORCE_INLINE void XXPH3_initCustomSecret(rust::Slice<uint8_t> customSecret, xxh_u64 seed64)
{
    int const nbRounds = XXPH_SECRET_DEFAULT_SIZE / 16;
    int i;

    XXPH_STATIC_ASSERT((XXPH_SECRET_DEFAULT_SIZE & 15) == 0);

    for (i=0; i < nbRounds; i++) {
        XXPH_writeLE64(rust::Slice(customSecret.data() + 16*i, customSecret.length() - 16*i),     xxph::xxph_read_le64(rust::Slice(kSecret + 16*i, 8))     + seed64);
        XXPH_writeLE64(rust::Slice(customSecret.data() + 16*i + 8, customSecret.length() - 16*i - 8), xxph::xxph_read_le64(rust::Slice(kSecret + 16*i + 8, 8)) - seed64);
    }
}


/* XXPH3_hashLong_64b_withSeed() :
 * Generate a custom key,
 * based on alteration of default kSecret with the seed,
 * and then use this key for long mode hashing.
 * This operation is decently fast but nonetheless costs a little bit of time.
 * Try to avoid it whenever possible (typically when seed==0).
 */
XXPH_NO_INLINE XXPH64_hash_t    /* It's important for performance that XXPH3_hashLong is not inlined. Not sure why (uop cache maybe ?), but difference is large and easily measurable */
XXPH3_hashLong_64b_withSeed(rust::Slice<const uint8_t> input, XXPH64_hash_t seed)
{
    XXPH_ALIGN(8) xxh_u8 secret[XXPH_SECRET_DEFAULT_SIZE];
    if (seed==0) return XXPH3_hashLong_64b_defaultSecret(input);
    XXPH3_initCustomSecret(rust::Slice(secret, sizeof(secret)), seed);
    return XXPH3_hashLong_internal(input, rust::Slice(static_cast<const xxh_u8*>(secret), sizeof(secret)));
}

#define XXPH3_MIDSIZE_MAX 240

/* ===   Public entry point   === */

XXPH_PUBLIC_API XXPH64_hash_t XXPH3_64bits(rust::Slice<const uint8_t> input)
{
    if (input.length() <= 16) return xxph::xxph3_len_0to16(input, 0);
    if (input.length() <= 128) return xxph::xxph3_len_17to128(input, 0);
    if (input.length() <= XXPH3_MIDSIZE_MAX) return xxph::xxph3_len_129to240(input, 0);
    return XXPH3_hashLong_64b_defaultSecret(input);
}

XXPH_PUBLIC_API XXPH64_hash_t
XXPH3_64bits_withSeed(rust::Slice<const uint8_t> input, XXPH64_hash_t seed)
{
    if (input.length() <= 16) return xxph::xxph3_len_0to16(input, seed);
    if (input.length() <= 128) return xxph::xxph3_len_17to128(input, seed);
    if (input.length() <= XXPH3_MIDSIZE_MAX) return xxph::xxph3_len_129to240(input, seed);
    return XXPH3_hashLong_64b_withSeed(input, seed);
}

/* ===   XXPH3 streaming   === */

/* RocksDB Note: unused & removed due to bug in preview version */

/*======== END #include "xxh3.h", now inlined above ==========*/

#endif  /* XXPH_NO_LONG_LONG */

/* === END RocksDB modification of permanently inlining === */

#endif /*  defined(XXPH_INLINE_ALL) || defined(XXPH_PRIVATE_API) */

#endif /* XXPH_STATIC_LINKING_ONLY */

#if defined (__cplusplus)
}
#endif

#endif /* XXPHASH_H_5627135585666179 */
