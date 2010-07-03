/* This file is a part of sfmt-extstate */
/**
 * @file  test.c
 * @brief test program for 32-bit output of sfmt-extstate.
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

#define BLOCK_SIZE 100000
#define BLOCK_SIZE64 50000
#define COUNT 1000

uint32_t gen_rand32(void);
void fill_array32(uint32_t *array, int size);
void check32(void);
void speed32(void);
void paramdump(void);
void function_tests(void);

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

void rshift128(w128_t *out,  w128_t const *in, int shift);
void lshift128(w128_t *out,  w128_t const *in, int shift);
void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
                         w128_t *d);

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
    {
    	init_gen_rand(1234, &sfmt[0]); 
	idx = N32;
	initialized = 1;
    }
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    {
    	init_gen_rand(1234, &sfmt[0]); 
	idx = N32;
	initialized = 1;
    }
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
    {
	init_by_array(ini, 4, &sfmt[0]);
	idx = N32;
	initialized = 1;
    }
    printf("init_by_array__________\n");
    fill_array32(array32, 10000);
    fill_array32(array32_2, 10000);
    {
	init_by_array(ini, 4, &sfmt[0]);
	idx = N32;
	initialized = 1;
    }
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
    {
    	init_gen_rand(1234, &sfmt[0]); 
	idx = N32;
	initialized = 1;
    }
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
    {
    	init_gen_rand(1234, &sfmt[0]); 
	idx = N32;
	initialized = 1;
    }
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
    printf("sizeof(sfmt) = %d\n", sizeof(sfmt));
}

void function_tests(void) {

    w128_t a, b, c, d, r;
    int shift;

    printf("rshift128()\n");
    a.u[0] = 3440181298;
    a.u[1] = 1564997079;
    a.u[2] = 1510669302;
    a.u[3] = 2930277156;
    shift = 1;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 3796268453;
    a.u[1] = 423124208;
    a.u[2] = 2143818589;
    a.u[3] = 3827219408;
    shift = 2;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 2674978610;
    a.u[1] = 1536842514;
    a.u[2] = 2027035537;
    a.u[3] = 2534897563;
    shift = 3;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 545368292;
    a.u[1] = 1489013321;
    a.u[2] = 1370534252;
    a.u[3] = 4231012796;
    shift = 4;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 1764869045;
    a.u[1] = 824597505;
    a.u[2] = 862581900;
    a.u[3] = 2469764249;
    shift = 5;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 359318673;
    a.u[1] = 116957936;
    a.u[2] = 3367389672;
    a.u[3] = 2327178354;
    shift = 6;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 3206507879;
    a.u[1] = 2378925033;
    a.u[2] = 1040214787;
    a.u[3] = 2524778605;
    shift = 7;
    rshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);

    printf("lshift128()\n");
    a.u[0] = 1721379072;
    a.u[1] = 3897926942;
    a.u[2] = 1790395498;
    a.u[3] = 2569178939;
    shift = 1;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 2340259131;
    a.u[1] = 3144212906;
    a.u[2] = 2301169789;
    a.u[3] = 2442885464;
    shift = 2;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 3667880593;
    a.u[1] = 3935928400;
    a.u[2] = 2372805237;
    a.u[3] = 1666397115;
    shift = 3;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 513866770;
    a.u[1] = 3810869743;
    a.u[2] = 2147400037;
    a.u[3] = 2792078025;
    shift = 4;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 3212265810;
    a.u[1] = 984692259;
    a.u[2] = 346590253;
    a.u[3] = 1804179199;
    shift = 5;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 750108141;
    a.u[1] = 2880257022;
    a.u[2] = 243310542;
    a.u[3] = 1869036465;
    shift = 6;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);
    a.u[0] = 2983949551;
    a.u[1] = 1931450364;
    a.u[2] = 4034505847;
    a.u[3] = 2735030199;
    shift = 7;
    lshift128(&b, &a, shift);
    printf("%d %u %u %u %u %u %u %u %u\n",
	   shift, a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3]);

    printf("do_recursion()\n");
    a.u[0] = 3440181298;
    a.u[1] = 1564997079;
    a.u[2] = 1510669302;
    a.u[3] = 2930277156;
    b.u[0] = 3796268453;
    b.u[1] = 423124208;
    b.u[2] = 2143818589;
    b.u[3] = 3827219408;
    c.u[0] = 2674978610;
    c.u[1] = 1536842514;
    c.u[2] = 2027035537;
    c.u[3] = 2534897563;
    d.u[0] = 545368292;
    d.u[1] = 1489013321;
    d.u[2] = 1370534252;
    d.u[3] = 4231012796;
    do_recursion(&r, &a, &b, &c, &d);
    printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
	   a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3],
	   c.u[0], c.u[1], c.u[2], c.u[3],
	   d.u[0], d.u[1], d.u[2], d.u[3],
	   r.u[0], r.u[1], r.u[2], r.u[3]);
    a.u[0] = 1764869045;
    a.u[1] = 824597505;
    a.u[2] = 862581900;
    a.u[3] = 2469764249;
    b.u[0] = 359318673;
    b.u[1] = 116957936;
    b.u[2] = 3367389672;
    b.u[3] = 2327178354;
    c.u[0] = 3206507879;
    c.u[1] = 2378925033;
    c.u[2] = 1040214787;
    c.u[3] = 2524778605;
    d.u[0] = 1417665896;
    d.u[1] = 964324147;
    d.u[2] = 2282797708;
    d.u[3] = 2456269299;
    do_recursion(&r, &a, &b, &c, &d);
    printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
	   a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3],
	   c.u[0], c.u[1], c.u[2], c.u[3],
	   d.u[0], d.u[1], d.u[2], d.u[3],
	   r.u[0], r.u[1], r.u[2], r.u[3]);
    a.u[0] = 2245093271;
    a.u[1] = 1015729427;
    a.u[2] = 2694465011;
    a.u[3] = 3246975184;
    b.u[0] = 463679346;
    b.u[1] = 3721104591;
    b.u[2] = 3475064196;
    b.u[3] = 856141236;
    c.u[0] = 3522818941;
    c.u[1] = 3721533109;
    c.u[2] = 1954826617;
    c.u[3] = 1282044024;
    d.u[0] = 1301863085;
    d.u[1] = 2669145051;
    d.u[2] = 4221477354;
    d.u[3] = 3896016841;
    do_recursion(&r, &a, &b, &c, &d);
    printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
	   a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3],
	   c.u[0], c.u[1], c.u[2], c.u[3],
	   d.u[0], d.u[1], d.u[2], d.u[3],
	   r.u[0], r.u[1], r.u[2], r.u[3]);
    a.u[0] = 462466863;
    a.u[1] = 1037679449;
    a.u[2] = 1228140306;
    a.u[3] = 922298197;
    b.u[0] = 1872938061;
    b.u[1] = 3102547608;
    b.u[2] = 2742766808;
    b.u[3] = 1888626088;
    c.u[0] = 157593879;
    c.u[1] = 1136901695;
    c.u[2] = 4038377686;
    c.u[3] = 3572517236;
    d.u[0] = 2997311961;
    d.u[1] = 1189931652;
    d.u[2] = 3981543765;
    d.u[3] = 2826166703;
    do_recursion(&r, &a, &b, &c, &d);
    printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
	   a.u[0], a.u[1], a.u[2], a.u[3],
	   b.u[0], b.u[1], b.u[2], b.u[3],
	   c.u[0], c.u[1], c.u[2], c.u[3],
	   d.u[0], d.u[1], d.u[2], d.u[3],
	   r.u[0], r.u[1], r.u[2], r.u[3]);
}



int main(int argc, char *argv[]) {
    int i;
    int speed = 0;
    int bit32 = 0;
    int param = 0;
    int functest = 0;

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
	if (strncmp(argv[1],"-f", 2) == 0) {
	    functest = 1;
	}
    }
    if (speed + bit32 + param + functest == 0) {
	printf("usage:\n%s [-s | -b32 | -p | -f]\n", argv[0]);
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
    if (functest) {
	function_tests();
    }
    return 0;
}
