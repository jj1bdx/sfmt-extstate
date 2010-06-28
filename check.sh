#!/bin/sh
# This file is a part of sfmt-extstate
# Copyright (C) 2010 Kenji Rikitake. All rights reserved.
# Copyright (C) 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
# University. All rights reserved.
# The new BSD License is applied to this software.
# See LICENSE.txt for the further details.
#
bit_len=$1
prefix=$2
tmp=tmp.$$
tmp64=tmp64.$$
exps="19937 216091"
for mexp in $exps; do
    if [ $bit_len = "64" ]; then
	./test-std-M${mexp} -b64 > $tmp64
	compare=$tmp64
    else
	compare=SFMT.${mexp}.out.txt
    fi
    command=${prefix}-M${mexp}
    if ./$command -b${bit_len}> $tmp; then
	:;
    else
	echo $command exexute error!
	rm -f $tmp
	if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
	    rm -f $tmp64
	fi
	exit 1
    fi
    if diff -q -w $tmp $compare; then
	echo $command output check OK
	rm -f $tmp
    else
	echo $command output check NG!
	rm -f $tmp
	if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
	    rm -f $tmp64
	fi
	exit 1
    fi
done
if [ -n "$tmp64" ] && [ -e $tmp64 ]; then
    rm -f $tmp64
fi
exit 0
