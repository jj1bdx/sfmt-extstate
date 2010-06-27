# @file  Makefile
# @brief Makefile
#
# @author Mutsuo Saito (Hiroshima University)
# @author Makoto Matsumoto (Hiroshima University)
#
# Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
# University. All rights reserved.
#
# The new BSD License is applied to this software.
# see LICENSE.txt
#
# @note
# We could comple test-sse2-Mxxx using gcc 3.4.4 of cygwin.
# We could comple test-sse2-Mxxx using gcc 4.0.1 of Linux.
# We coundn't comple test-sse2-Mxxx using gcc 3.3.2 of Linux.
# We could comple test-alti-Mxxx using gcc 3.3 of osx.
# We could comple test-alti-Mxxx using gcc 4.0 of osx.
# 
# AltIvec support removed

WARN = -Wmissing-prototypes -Wall #-Winline 
#WARN = -Wmissing-prototypes -Wall -W
OPTI = -O3 -finline-functions -fomit-frame-pointer -DNDEBUG \
	-fno-strict-aliasing --param max-inline-insns-single=1800 \
	--param inline-unit-growth=500 --param large-function-growth=900 #for gcc 4
#STD =
#STD = -std=c89 -pedantic
#STD = -std=c99 -pedantic
STD = -std=c99
CC = gcc44
CCFLAGS = $(OPTI) $(WARN) $(STD)
SSE2FLAGS = -msse2 -DHAVE_SSE2
STD_TARGET = test-std-M19937
SSE2_TARGET = test-sse2-M19937
ALL_STD_TARGET = test-std-M607 test-std-M19937
ALL_SSE2_TARGET = test-sse2-M607 test-sse2-M19937
# ==========================================================
# comment out or EDIT following lines to get max performance
# ==========================================================
# --------------------
# for gcc 4
# --------------------
#CCFLAGS += --param inline-unit-growth=500 \
#	--param large-function-growth=900
# --------------------
# for icl
# --------------------
#CC = icl /Wcheck /O3 /QxB /Qprefetch
# -----------------
# for PowerPC
# -----------------
#CCFLAGS += -arch ppc
# -----------------
# for Pentium M
# -----------------
#CCFLAGS += -march=prescott
# -----------------
# for Athlon 64
# -----------------
#CCFLAGS += -march=athlon64

.PHONY: std-check sse2-check

# for i386 basic testing
all: std sse2 std-check sse2-check

std: ${STD_TARGET}

sse2:${SSE2_TARGET}

std-check: ${ALL_STD_TARGET}
	./check.sh 32 test-std

sse2-check: ${ALL_SSE2_TARGET}
	./check.sh 32 test-sse2

test-std-M607: test.c SFMT.c SFMT.h SFMT-params607.h
	${CC} ${CCFLAGS} -DMEXP=607 -o $@ test.c

test-sse2-M607: test.c SFMT.c SFMT.h SFMT-sse2.h SFMT-params607.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DMEXP=607 -o $@ test.c

test-std-M19937: test.c SFMT.c SFMT.h SFMT-params19937.h
	${CC} ${CCFLAGS} -DMEXP=19937 -o $@ test.c

test-sse2-M19937: test.c SFMT.c SFMT.h SFMT-sse2.h \
	SFMT-params19937.h
	${CC} ${CCFLAGS} ${SSE2FLAGS} -DMEXP=19937 -o $@ test.c

clean:
	rm -f *.o *~ test-*
