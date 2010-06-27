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

#include "SFMT.c"

#define BLOCK_SIZE 100000
#define BLOCK_SIZE64 50000
#define COUNT 1000

void check32(void);
void speed32(void);
void paramdump(void);

#if defined(HAVE_SSE2)
static __m128i array1[BLOCK_SIZE / 4];
static __m128i array2[10000 / 4];
#else
static uint64_t array1[BLOCK_SIZE / 4][2];
static uint64_t array2[10000 / 4][2];
#endif

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
    printf("N64 = %d\n", N64);
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
