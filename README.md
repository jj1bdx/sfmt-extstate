# sfmt-extstate: SFMT C functions with the externalized state table

* Edited and written by Kenji Rikitake
* Email contact: kenji.rikitake AT acm.org
* (change AT to @ for the email address)
* Version 0.1.0 29-JUN-2010

Copyright (C) 2010-2014 by Kenji Rikitake. All rights reserved.

The (modified) BSD License is applied to this software.
If you want to redistribute and/or change source files, see LICENSE.txt.

## This software is based on

SFMT ver. 1.3.3
SIMD oriented Fast Mersenne Twister(SFMT)

Mutsuo Saito (Hiroshima University) and
Makoto Matsumoto (Hiroshima University)

Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
University. All rights reserved.

The (modified) BSD License is applied to this software. See LICENSE.txt.

## Details

* Details of SFMT algorithm is accessible on the Web from:
  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html

* Tested on FreeBSD/amd64 10.0-STABLE with Port lang/gcc47.

## Features removed

* AltIvec support is removed; no test environment.

* SFMT RNG period is fixed to ((2^19937)-1). Parameters of
  the original SFMT-1.3.3 code are applicable with little modification.
  (Code also tested with the ((2^216091)-1) RNG period.)

* 64-bit RNG support is removed to simplify the internal state
  handling.  Mixing up 32- and 64-bit RNG generations can be fatal
  for the whole generation process.

## Why externalized state table needed?

The original SFMT code embedded the internal state table of SFMT as a
static array; this made the whole code thread-unsafe and unable to be
included into other pieces of software such as a part of Erlang
linked-in drivers.

In the functions of `sfmt-extstate-*.c,` the programmers need to
explicitly pass the pointer of the SFMT internal state table.  This will
allow the programmers to separate the memory regions for the state
tables when multiple instances of SFMT generators are concurrently
running.

[End of README]
