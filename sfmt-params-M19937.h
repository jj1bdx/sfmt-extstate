/* This file is a part of sfmt-extstate */

/** 
 * @file sfmt-params-M19937.h
 *
 * @brief SFMT configuration file for a parameter set of
 * period ((2^19937)-1)
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 * @author Kenji Rikitake
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * Copyright (C) 2010 Kenji Rikitake. All rights reserved.
 *
 * The new BSD License is applied to this software.
 * see LICENSE.txt
 *
 * @note Other sets of parameters are available from SFMT-1.3.3
 * source code.
 */

#ifndef SFMT_PARAMS_M19937_H
#define SFMT_PARAMS_M19937_H

/** Mersenne Exponent. The period of the sequence 
 *  is a multiple of 2^MEXP-1. */
#define MEXP 19937
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define N ((MEXP / 128) + 1)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define N32 (N * 4)
/** the pick up position of the array. */
#define POS1 122 
/** the parameter of shift left as four 32-bit registers. */
#define SL1 18
/** the parameter of shift left as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. */
#define SL2 1 
/** the parameter of shift right as four 32-bit registers. */
#define SR1 11
/** the parameter of shift right as one 128-bit register. 
 * The 128-bit integer is shifted by (SL2 * 8) bits. */
#define SR2 1 
/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD. */
#define MSK1 0xdfffffefU
#define MSK2 0xddfecb7fU
#define MSK3 0xbffaffffU
#define MSK4 0xbffffff6U 
/** These definitions are part of a 128-bit period certification vector. */
#define PARITY1	0x00000001U
#define PARITY2	0x00000000U
#define PARITY3	0x00000000U
#define PARITY4	0x13c9e684U
/* identification string for the algorithm */
#define IDSTR	"SFMT-19937:122-18-1-11-1:dfffffef-ddfecb7f-bffaffff-bffffff6"

#endif /* SFMT_PARAMS_M19937_H */
