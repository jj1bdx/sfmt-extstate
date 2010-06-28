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

/** a parity check vector which certificate the period of 2^{MEXP} */
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

/**
 * This function certificate the period of 2^{MEXP}
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

