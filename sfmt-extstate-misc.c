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

/* public functions for the state tables */
void period_certification(w128_t *intstate);
const char *get_idstring(void);
int get_min_array_size32(void);
void init_gen_rand(uint32_t seed, w128_t *intstate);
void init_by_array(uint32_t *init_key, int key_length, w128_t *intstate);

/* static function prototypes */
inline static uint32_t func1(uint32_t x);
inline static uint32_t func2(uint32_t x);

/** a parity check vector which certificate the period of 2^{MEXP} */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

/**
 * This function certificate the period of 2^{MEXP}
 * @param intstate internal state array
 */
void period_certification(w128_t *intstate) {
    int inner = 0;
    int i, j;
    uint32_t work;
    uint32_t *intstate32;

    intstate32 = &intstate[0].u[0];

    for (i = 0; i < 4; i++)
	inner ^= intstate32[i] & parity[i];
    for (i = 16; i > 0; i >>= 1)
	inner ^= inner >> i;
    inner &= 1;
    /* check OK */
    if (inner == 1) {
	return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
	work = 1;
	for (j = 0; j < 32; j++) {
	    if ((work & parity[i]) != 0) {
		intstate32[i] ^= work;
		return;
	    }
	    work = work << 1;
	}
    }
}

/**
 * This function returns the identification string.
 * The string shows the word size, the Mersenne exponent,
 * and all parameters of this generator.
 */
const char *get_idstring(void) {
    return IDSTR;
}

/**
 * This function returns the minimum size of array used for \b
 * fill_array32() function.
 * @return minimum size of array used for fill_array32() function.
 */
int get_min_array_size32(void) {
    return N32;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
inline static uint32_t func1(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1664525UL;
}

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
inline static uint32_t func2(uint32_t x) {
    return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 * Execution of this function guarantees that the internal state
 * array is correctly initialized.
 *
 * @param seed a 32-bit integer used as the seed.
 * @param intstate internal state array
 */
void init_gen_rand(uint32_t seed, w128_t *intstate) {
    int i;
    uint32_t *intstate32;

    intstate32 = &intstate[0].u[0];

    intstate32[0] = seed;
    for (i = 1; i < N32; i++) {
	intstate32[i] = 1812433253UL * (intstate32[i - 1] 
					    ^ (intstate32[i - 1] >> 30))
	    + i;
    }
    period_certification(&intstate[0]);
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 *
 * Execution of this function guarantees that the internal state
 * array is correctly initialized.
 *
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 * @param intstate internal state array
 */
void init_by_array(uint32_t *init_key, int key_length, w128_t *intstate) {
    int i, j, count;
    uint32_t r;
    int lag;
    int mid;
    int size = N32;
    uint32_t *intstate32;

    intstate32 = &intstate[0].u[0];

    if (size >= 623) {
	lag = 11;
    } else if (size >= 68) {
	lag = 7;
    } else if (size >= 39) {
	lag = 5;
    } else {
	lag = 3;
    }
    mid = (size - lag) / 2;

    memset(&intstate[0], 0x8b, (N32 * 4));

    if (key_length + 1 > N32) {
	count = key_length + 1;
    } else {
	count = N32;
    }
    r = func1(intstate32[0] ^ intstate32[mid] 
	      ^ intstate32[N32 - 1]);
    intstate32[mid] += r;
    r += key_length;
    intstate32[mid + lag] += r;
    intstate32[0] = r;

    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(intstate32[i] ^ intstate32[(i + mid) % N32] 
		  ^ intstate32[(i + N32 - 1) % N32]);
	intstate32[(i + mid) % N32] += r;
	r += init_key[j] + i;
	intstate32[(i + mid + lag) % N32] += r;
	intstate32[i] = r;
	i = (i + 1) % N32;
    }
    for (; j < count; j++) {
	r = func1(intstate32[i] ^ intstate32[(i + mid) % N32] 
		  ^ intstate32[(i + N32 - 1) % N32]);
	intstate32[(i + mid) % N32] += r;
	r += i;
	intstate32[(i + mid + lag) % N32] += r;
	intstate32[i] = r;
	i = (i + 1) % N32;
    }
    for (j = 0; j < N32; j++) {
	r = func2(intstate32[i] + intstate32[(i + mid) % N32] 
		  + intstate32[(i + N32 - 1) % N32]);
	intstate32[(i + mid) % N32] ^= r;
	r -= i;
	intstate32[(i + mid + lag) % N32] ^= r;
	intstate32[i] = r;
	i = (i + 1) % N32;
    }

    period_certification(&intstate[0]);

}
