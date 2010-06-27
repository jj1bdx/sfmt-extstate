/**
 * @file  test.c
 * @brief test program for 32-bit and 64-bit output of SFMT.
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software, see LICENSE.txt
 */

#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sfmt-extstate.h"

#define BLOCK_SIZE 100000
#define BLOCK_SIZE64 50000
#define COUNT 1000

uint32_t gen_rand32(void);
void fill_array32(uint32_t *array, int size);
void init_gen_rand(uint32_t seed);
void init_by_array(uint32_t *init_key, int key_length);
const char *get_idstring(void);
int get_min_array_size32(void);
void check32(void);
void speed32(void);
void paramdump(void);
/* common prototypes */
inline static uint32_t func1(uint32_t x);
inline static uint32_t func2(uint32_t x);

#if defined(HAVE_SSE2)
static __m128i array1[BLOCK_SIZE / 4];
static __m128i array2[10000 / 4];
#else
static uint64_t array1[BLOCK_SIZE / 4][2];
static uint64_t array2[10000 / 4][2];
#endif

/*--------------------------------------
  FILE GLOBAL VARIABLES
  internal state, index counter and flag 
  --------------------------------------*/
/** the 128-bit internal state array */
static w128_t sfmt[N];
/** the 32bit integer pointer to the 128-bit internal state array */
static uint32_t *psfmt32 = &sfmt[0].u[0];
/** index counter to the 32-bit internal state array */
static int idx;
/** a flag: it is 0 if and only if the internal state is not yet
 * initialized. */
static int initialized = 0;

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
 * This function generates and returns 32-bit pseudorandom number.
 * init_gen_rand or init_by_array must be called before this function.
 * @return 32-bit pseudorandom number
 */
inline uint32_t gen_rand32(void) {
    uint32_t r;

    assert(initialized);
    if (idx >= N32) {
	gen_rand_all(&sfmt[0]);
	idx = 0;
    }
    r = psfmt32[idx++];
    return r;
}

/**
 * This function generates pseudorandom 32-bit integers in the
 * specified array[] by one call. The number of pseudorandom integers
 * is specified by the argument size, which must be at least 624 and a
 * multiple of four.  The generation by this function is much faster
 * than the following gen_rand function.
 *
 * For initialization, init_gen_rand or init_by_array must be called
 * before the first call of this function. This function can not be
 * used after calling gen_rand function, without initialization.
 *
 * @param array an array where pseudorandom 32-bit integers are filled
 * by this function.  The pointer to the array must be \b "aligned"
 * (namely, must be a multiple of 16) in the SIMD version, since it
 * refers to the address of a 128-bit integer.  In the standard C
 * version, the pointer is arbitrary.
 *
 * @param size the number of 32-bit pseudorandom integers to be
 * generated.  size must be a multiple of 4, and greater than or equal
 * to (MEXP / 128 + 1) * 4.
 *
 * @note \b memalign or \b posix_memalign is available to get aligned
 * memory. Mac OSX doesn't have these functions, but \b malloc of OSX
 * returns the pointer to the aligned memory block.
 */
void fill_array32(uint32_t *array, int size) {
    assert(initialized);
    assert(idx == N32);
    assert(size % 4 == 0);
    assert(size >= N32);

    gen_rand_array((w128_t *)array, size / 4, &sfmt[0]);
    idx = N32;
}

/**
 * This function initializes the internal state array with a 32-bit
 * integer seed.
 *
 * @param seed a 32-bit integer used as the seed.
 */
void init_gen_rand(uint32_t seed) {
    int i;

    psfmt32[0] = seed;
    for (i = 1; i < N32; i++) {
	psfmt32[i] = 1812433253UL * (psfmt32[i - 1] 
					    ^ (psfmt32[i - 1] >> 30))
	    + i;
    }
    idx = N32;
    period_certification(&sfmt[0]);
    initialized = 1;
}

/**
 * This function initializes the internal state array,
 * with an array of 32-bit integers used as the seeds
 * @param init_key the array of 32-bit integers, used as a seed.
 * @param key_length the length of init_key.
 */
void init_by_array(uint32_t *init_key, int key_length) {
    int i, j, count;
    uint32_t r;
    int lag;
    int mid;
    int size = N * 4;

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

    memset(sfmt, 0x8b, sizeof(sfmt));
    if (key_length + 1 > N32) {
	count = key_length + 1;
    } else {
	count = N32;
    }
    r = func1(psfmt32[0] ^ psfmt32[mid] 
	      ^ psfmt32[N32 - 1]);
    psfmt32[mid] += r;
    r += key_length;
    psfmt32[mid + lag] += r;
    psfmt32[0] = r;

    count--;
    for (i = 1, j = 0; (j < count) && (j < key_length); j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + mid) % N32] 
		  ^ psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + mid) % N32] += r;
	r += init_key[j] + i;
	psfmt32[(i + mid + lag) % N32] += r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }
    for (; j < count; j++) {
	r = func1(psfmt32[i] ^ psfmt32[(i + mid) % N32] 
		  ^ psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + mid) % N32] += r;
	r += i;
	psfmt32[(i + mid + lag) % N32] += r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }
    for (j = 0; j < N32; j++) {
	r = func2(psfmt32[i] + psfmt32[(i + mid) % N32] 
		  + psfmt32[(i + N32 - 1) % N32]);
	psfmt32[(i + mid) % N32] ^= r;
	r -= i;
	psfmt32[(i + mid + lag) % N32] ^= r;
	psfmt32[i] = r;
	i = (i + 1) % N32;
    }

    idx = N32;
    period_certification(&sfmt[0]);
    initialized = 1;
}
void check32(void) {
    int i;
    uint32_t *array32 = (uint32_t *)array1;
    uint32_t *array32_2 = (uint32_t *)array2;
    uint32_t ini[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};
    uint32_t r32;

    if (get_min_array_size32() > 10000) {
	printf("array size too small!\n");
	exit(1);
    }
    printf("%s\n32 bit generated randoms\n", get_idstring());
    printf("init_gen_rand__________\n");
    /* 32 bit generation */
    init_gen_rand(1234);
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    init_gen_rand(1234);
    for (i = 0; i < 10000; i++) {
	if (i < 1000) {
	    printf("%10u ", array32[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
	r32 = gen_rand32();
	if (r32 != array32[i]) {
	    printf("\nmismatch at %d array32:%x gen:%x\n", 
		   i, array32[i], r32);
	    exit(1);
	}
    }
    for (i = 0; i < 700; i++) {
	r32 = gen_rand32();
	if (r32 != array32_2[i]) {
	    printf("\nmismatch at %d array32_2:%x gen:%x\n", 
		   i, array32_2[i], r32);
	    exit(1);
	}
    }
    printf("\n");
    init_by_array(ini, 4);
    printf("init_by_array__________\n");
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    init_by_array(ini, 4);
    for (i = 0; i < 10000; i++) {
	if (i < 1000) {
	    printf("%10u ", array32[i]);
	    if (i % 5 == 4) {
		printf("\n");
	    }
	}
	r32 = gen_rand32();
	if (r32 != array32[i]) {
	    printf("\nmismatch at %d array32:%x gen:%x\n", 
		   i, array32[i], r32);
	    exit(1);
	}
    }
    for (i = 0; i < 700; i++) {
	r32 = gen_rand32();
	if (r32 != array32_2[i]) {
	    printf("\nmismatch at %d array32_2:%x gen:%x\n", 
		   i, array32_2[i], r32);
	    exit(1);
	}
    }
}

void speed32(void) {
    int i, j;
    clock_t clo;
    clock_t min = LONG_MAX;
    uint32_t *array32 = (uint32_t *)array1;

    if (get_min_array_size32() > BLOCK_SIZE) {
	printf("array size too small!\n");
	exit(1);
    }
    /* 32 bit generation */
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < COUNT; j++) {
	    fill_array32(array32, BLOCK_SIZE);
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32 bit BLOCK:%.0f", (double)min * 1000/ CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
    min = LONG_MAX;
    init_gen_rand(1234);
    for (i = 0; i < 10; i++) {
	clo = clock();
	for (j = 0; j < BLOCK_SIZE * COUNT; j++) {
	    gen_rand32();
	}
	clo = clock() - clo;
	if (clo < min) {
	    min = clo;
	}
    }
    printf("32 bit SEQUE:%.0f", (double)min * 1000 / CLOCKS_PER_SEC);
    printf("ms for %u randoms generation\n",
	   BLOCK_SIZE * COUNT);
}

void paramdump(void) {
    printf("MEXP = %d\n", MEXP);
    printf("N = %d\n", N);
    printf("N32 = %d\n", N32);
    printf("POS1 = %d\n", POS1);
    printf("SL1 = %d\n", SL1);
    printf("SL2 = %d\n", SL2);
    printf("SR1 = %d\n", SR1);
    printf("SR2 = %d\n", SR2);
    printf("MSK1 = %#010xU\n", MSK1);
    printf("MSK2 = %#010xU\n", MSK2);
    printf("MSK3 = %#010xU\n", MSK3);
    printf("MSK4 = %#010xU\n", MSK4);
    printf("PARITY1 = %#010xU\n", PARITY1);
    printf("PARITY2 = %#010xU\n", PARITY2);
    printf("PARITY3 = %#010xU\n", PARITY3);
    printf("PARITY4 = %#010xU\n", PARITY4);
    printf("IDSTR = %s\n", IDSTR);
}

int main(int argc, char *argv[]) {
    int i;
    int speed = 0;
    int bit32 = 0;
    int param = 0;

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[1],"-s", 2) == 0) {
	    speed = 1;
	}
	if (strncmp(argv[1],"-b32", 4) == 0) {
	    bit32 = 1;
	}
	if (strncmp(argv[1],"-p", 2) == 0) {
	    param = 1;
	}
    }
    if (speed + bit32 + param == 0) {
	printf("usage:\n%s [-s | -b32 | -p]\n", argv[0]);
	return 0;
    }
    if (speed) {
	speed32();
    }
    if (bit32) {
	check32();
    }
    if (param) {
	paramdump();
    }
    return 0;
}
