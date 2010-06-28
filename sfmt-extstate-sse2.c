/* This file is a part of sfmt-extstate */
/* Copyright (C) 2010 Kenji Rikitake. All rights reserved. */
/* The new BSD License is applied to this software. */
/* See LICENSE.txt for the further details. */

/** 
 * @file  SFMT.c
 * @brief SIMD oriented Fast Mersenne Twister(SFMT)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */
#include <string.h>
#include <assert.h>
#include "sfmt-extstate.h"

#if defined(HAVE_SSE2)

/* SSE2-specific prototypes */
PRE_ALWAYS __m128i mm_recursion(__m128i *a, __m128i *b, __m128i c,
				   __m128i d, __m128i mask) ALWAYSINLINE;

/* public functions for the state tables */
inline void gen_rand_all(w128_t *intstate);
inline void gen_rand_array(w128_t *array, int size, w128_t *intstate);

/* SSE2 assembly language code */

/**
 * This function represents the recursion formula.
 * @param a a 128-bit part of the interal state array
 * @param b a 128-bit part of the interal state array
 * @param c a 128-bit part of the interal state array
 * @param d a 128-bit part of the interal state array
 * @param mask 128-bit mask
 * @return output
 */
PRE_ALWAYS __m128i mm_recursion(__m128i *a, __m128i *b, 
				   __m128i c, __m128i d, __m128i mask) {
    __m128i v, x, y, z;
    
    x = _mm_load_si128(a);
    y = _mm_srli_epi32(*b, SR1);
    z = _mm_srli_si128(c, SR2);
    v = _mm_slli_epi32(d, SL1);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, v);
    x = _mm_slli_si128(x, SL2);
    y = _mm_and_si128(y, mask);
    z = _mm_xor_si128(z, x);
    z = _mm_xor_si128(z, y);
    return z;
}

/**
 * This function fills the internal state array with pseudorandom
 * integers.
 */
inline void gen_rand_all(w128_t *intstate) {
    int i;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&intstate[N - 2].si);
    r2 = _mm_load_si128(&intstate[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&intstate[i].si, &intstate[i + POS1].si, r1, r2, mask);
	_mm_store_si128(&intstate[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&intstate[i].si, &intstate[i + POS1 - N].si, r1, r2, mask);
	_mm_store_si128(&intstate[i].si, r);
	r1 = r2;
	r2 = r;
    }
}

/**
 * This function fills the user-specified array with pseudorandom
 * integers.
 *
 * @param array an 128-bit array to be filled by pseudorandom numbers.  
 * @param size number of 128-bit pesudorandom numbers to be generated.
 */
inline void gen_rand_array(w128_t *array, int size, w128_t *intstate) {
    int i, j;
    __m128i r, r1, r2, mask;
    mask = _mm_set_epi32(MSK4, MSK3, MSK2, MSK1);

    r1 = _mm_load_si128(&intstate[N - 2].si);
    r2 = _mm_load_si128(&intstate[N - 1].si);
    for (i = 0; i < N - POS1; i++) {
	r = mm_recursion(&intstate[i].si, &intstate[i + POS1].si, r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (; i < N; i++) {
	r = mm_recursion(&intstate[i].si, &array[i + POS1 - N].si, r1, r2, mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    /* main loop */
    for (; i < size - N; i++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r1, r2,
			 mask);
	_mm_store_si128(&array[i].si, r);
	r1 = r2;
	r2 = r;
    }
    for (j = 0; j < 2 * N - size; j++) {
	r = _mm_load_si128(&array[j + size - N].si);
	_mm_store_si128(&intstate[j].si, r);
    }
    for (; i < size; i++) {
	r = mm_recursion(&array[i - N].si, &array[i + POS1 - N].si, r1, r2,
			 mask);
	_mm_store_si128(&array[i].si, r);
	_mm_store_si128(&intstate[j++].si, r);
	r1 = r2;
	r2 = r;
    }
}

#endif /* defined(HAVE_SSE2) */
