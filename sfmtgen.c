/* This file is a part of sfmt-extstate */
/**
 * @file  sfmtgen.c
 * @brief random number generator program for 32-bit output of sfmt-extstate.
 *
 * @author Mutsuo Saito (Hiroshima-univ)
 * @author Kenji Rikitake
 *
 * Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * Copyright (C) 2010 Kenji Rikitake. All rights reserved.
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

uint32_t gen_rand32(void);
void fill_array32(uint32_t *array, int size);
void generate(void);
void paramdump(void);

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

void generate(void) {
    uint32_t ini[4] = {0x1234, 0x5678, 0x9abc, 0xdef0};
    union intout {
        unsigned int i;
        unsigned char c[4];
    } p;

    init_by_array(ini, 4, &sfmt[0]);
    initialized = 1;

    while (1) {
	p.i = gen_rand32();
        putchar(p.c[0]);
        putchar(p.c[1]);
        putchar(p.c[2]);
        putchar(p.c[3]);
    }
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
    printf("sizeof(sfmt) = %d\n", sizeof(sfmt));
}

int main(int argc, char *argv[]) {
    int i;
    int gen = 0;
    int param = 0;

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[1],"-g", 2) == 0) {
	    gen = 1;
	}
	if (strncmp(argv[1],"-p", 2) == 0) {
	    param = 1;
	}
    }
    if (gen + param == 0) {
	printf("usage:\n%s [-g | -p]\n", argv[0]);
	return 0;
    }
    if (gen) {
	generate();
    }
    if (param) {
	paramdump();
    }
    return 0;
}
