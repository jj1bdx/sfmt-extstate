/* This file is a part of sfmt-extstate */
/* Copyright (C) 2010 Kenji Rikitake. All rights reserved. */
/* The new BSD License is applied to this software. */
/* See LICENSE.txt for the further details. */

/** 
 * @file SFMT.h
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 *
 * @note We assume that your system has inttypes.h.  If your system
 * doesn't have inttypes.h, you have to typedef uint32_t and uint64_t,
 * and you have to define PRIu64 and PRIx64 in this file as follows:
 * @verbatim
 typedef unsigned int uint32_t
 typedef unsigned long long uint64_t  
 #define PRIu64 "llu"
 #define PRIx64 "llx"
@endverbatim
 * uint32_t must be exactly 32-bit unsigned integer type (no more, no
 * less), and uint64_t must be exactly 64-bit unsigned integer type.
 * PRIu64 and PRIx64 are used for printf function to print 64-bit
 * unsigned int and 64-bit unsigned int in hexadecimal format.
 */

#ifndef SFMT_EXTSTATE_H
#define SFMT_EXTSTATE_H

#include <stdio.h>
#if !defined(MEXP)
#ifdef __GNUC__
#warning "MEXP is not defined. I assume MEXP is 19937."
#endif /* __GNUC-- */
#define MEXP 19937
#endif /* !defined(MEXP) */

#if MEXP == 19937
  #include "sfmt-params-M19937.h"
#elif MEXP == 216091
  #include "sfmt-params-M216091.h"
#else
#ifdef __GNUC__
  #error "MEXP is not valid."
  #undef MEXP
#else
  #undef MEXP
#endif /* __GNUC__ */
#endif /* MEXP */

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
  #include <inttypes.h>
#elif defined(_MSC_VER) || defined(__BORLANDC__)
  typedef unsigned int uint32_t;
  typedef unsigned __int64 uint64_t;
  #define inline __inline
#else
  #include <inttypes.h>
  #if defined(__GNUC__)
    #define inline __inline__
  #endif
#endif

#ifndef PRIu64
  #if defined(_MSC_VER) || defined(__BORLANDC__)
    #define PRIu64 "I64u"
    #define PRIx64 "I64x"
  #else
    #define PRIu64 "llu"
    #define PRIx64 "llx"
  #endif
#endif

#if defined(__GNUC__)
#define ALWAYSINLINE __attribute__((always_inline))
#else
#define ALWAYSINLINE
#endif

#if defined(_MSC_VER)
  #if _MSC_VER >= 1200
    #define PRE_ALWAYS __forceinline
  #else
    #define PRE_ALWAYS inline
  #endif
#else
  #define PRE_ALWAYS inline
#endif

/*------------------------------------------------------
  128-bit SIMD data type for SSE2 or standard C
  ------------------------------------------------------*/
#if defined(HAVE_SSE2)
  #include <emmintrin.h>

/** 128-bit data structure */
union W128_T {
    __m128i si;
    uint32_t u[4];
};
/** 128-bit data type */
typedef union W128_T w128_t;

#else /* HAVE_SSE2 */

/** 128-bit data structure */
struct W128_T {
    uint32_t u[4];
};
/** 128-bit data type */
typedef struct W128_T w128_t;

#endif /* HAVE_SSE2 */

/* public functions for the state tables */
void gen_rand_all(w128_t *intstate);
void gen_rand_array(w128_t *array, int size, w128_t *intstate);
void period_certification(w128_t *intstate);
const char *get_idstring(void);
int get_min_array_size32(void);
void init_gen_rand(uint32_t seed, w128_t *intstate);
void init_by_array(uint32_t *init_key, int key_length, w128_t *intstate);

#endif /* SFMT_EXTSTATE_H */
