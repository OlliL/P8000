#!/bin/sh
# libtest.sh ROOT : every C member of libc.a (cc) and slibc.a (scc -DSEG) compiled with ROOT (with -I of src/head/oldv7 where the
# Makefile of src/lib/libc has it), against the member of the original /lib/libc.a, /lib/slibc.a (extracted to /tmp/SCRATCH/orig/LIB,
# not the build directories of src/lib/libc); prints the members that differ (bytes)
R=$1; T=/tmp/SCRATCH/lt_$(basename $R); rm -rf $T; mkdir -p $T
W=/home/olivleh1/git/P8000/tmp/wega31_root; H=/home/olivleh1/git/P8000/WEGA/host; L=/home/olivleh1/git/P8000/WEGA/src/lib/libc
for lib in libc slibc; do [ -d /tmp/SCRATCH/orig/$lib ] || { mkdir -p /tmp/SCRATCH/orig/$lib && python3 $H/tools/war.py extract $W/lib/$lib.a /tmp/SCRATCH/orig/$lib >/dev/null; }; done
n=0
for v in nonseg seg; do
  case $v in nonseg) lib=libc; c=cc; f= ;; seg) lib=slibc; c=scc; f=-DSEG ;; esac
  mkdir -p $T/$v/inc; cp /home/olivleh1/git/P8000/WEGA/src/head/oldv7/stdio.h $T/$v/inc/
  cp $L/common/gen/* $L/common/stdio/* $T/$v/ 2>/dev/null; cp $L/$v/gen/*.c $T/$v/ 2>/dev/null
  for s in $T/$v/*.c; do m=$(basename $s .c); o=/tmp/SCRATCH/orig/$lib/$m.o; [ -f $o ] || continue
    inc=; grep -q "head/oldv7 -c ../../common/stdio/$m.c" $L/Makefile && inc=-Iinc
    ( cd $T/$v && timeout -s KILL 60 /home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu -r $R /bin/$c -O $f $inc -c $m.c >/dev/null 2>&1; if [ -f $m.o ]; then echo "$lib/$m $(cmp -l $m.o $o 2>/dev/null | wc -l)"; else echo "$lib/$m FAIL"; fi > $m.res ) &
    n=$((n+1)); [ $((n%8)) = 0 ] && wait
  done; wait
done
cat $T/*/*.res | awk '$2!="0"{printf "%s:%s ", $1, $2} $2=="0"{s++} END{printf "| same %d of %d\n", s, NR}'
