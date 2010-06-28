/* This file is a part of sfmt-extstate */
/* Copyright (C) 2010 Kenji Rikitake. All rights reserved. */
/* Copyright (C) 2006,2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima */
/* University. All rights reserved. */
/* The new BSD License is applied to this software. */
/* See LICENSE.txt for the further details. */

#ifndef SFMT_PARAMS_M216091_H
#define SFMT_PARAMS_M216091_H

/** Mersenne Exponent. The period of the sequence 
 *  is a multiple of 2^MEXP-1. */
/* #define MEXP 216091 */
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define N ((MEXP / 128) + 1)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define N32 (N * 4)
/** the pick up position of the array. */
#define POS1 627
/** the parameter of shift left as four 32-bit registers. */
#define SL1 11
/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. */
#define SL2 3
/** the parameter of shift right as four 32-bit registers. */
#define SR1 10
/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. */
#define SR2 1 
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD. */
#define MSK1 0xbff7bff7U
#define MSK2 0xbfffffffU
#define MSK3 0xbffffa7fU
#define MSK4 0xffddfbfbU
/** These definitions are part of a 128-bit period certification vector. */
#define PARITY1	0xf8000001U
#define PARITY2	0x89e80709U
#define PARITY3	0x3bd2b64bU
#define PARITY4	0x0c64b1e4U
/* identification string for the algorithm */
#define IDSTR	"SFMT-216091:627-11-3-10-1:bff7bff7-bfffffff-bffffa7f-ffddfbfb"

#endif /* SFMT_PARAMS_M216091_H */
