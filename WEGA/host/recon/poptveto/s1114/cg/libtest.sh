#!/bin/sh
# libtest.sh ROOT : every C member of libc (cc) and slibc (scc) compiled with ROOT (with -I of src/head/oldv7 where its Makefile has it), against the member of the original
# /lib/libc.a, /lib/slibc.a (extracted to /tmp/SCRATCH/orig/LIB, not src/lib/LIB/obj: that is the build target); prints the members that differ (bytes)
R=$1; T=/tmp/SCRATCH/lt_$(basename $R); rm -rf $T; mkdir -p $T
W=/home/olivleh1/git/P8000/tmp/wega31_root; H=/home/olivleh1/git/P8000/WEGA/host
for lib in libc slibc; do [ -d /tmp/SCRATCH/orig/$lib ] || { mkdir -p /tmp/SCRATCH/orig/$lib && python3 $H/tools/war.py extract $W/lib/$lib.a /tmp/SCRATCH/orig/$lib >/dev/null; }; done
n=0; same=0; out=""
for lib in libc slibc; do c=cc; [ $lib = slibc ] && c=scc
  for d in gen stdio; do
    rm -rf $T/$lib.$d; cp -r /home/olivleh1/git/P8000/WEGA/src/lib/$lib/$d $T/$lib.$d; mkdir -p $T/$lib.$d/inc; cp /home/olivleh1/git/P8000/WEGA/src/head/oldv7/stdio.h $T/$lib.$d/inc/
    for f in $T/$lib.$d/*.c; do m=$(basename $f .c); o=/tmp/SCRATCH/orig/$lib/$m.o; [ -f $o ] || continue
      inc=; grep -q "head/oldv7 -c $m.c" /home/olivleh1/git/P8000/WEGA/src/lib/$lib/Makefile /home/olivleh1/git/P8000/WEGA/src/lib/$lib/$d/Makefile 2>/dev/null && inc=-Iinc	# the stdio.h of the time, as the Makefile
      ( cd $T/$lib.$d && timeout -s KILL 60 /home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu -r $R /bin/$c -O $inc -c $m.c >/dev/null 2>&1; if [ -f $m.o ]; then echo "$lib/$m $(cmp -l $m.o $o 2>/dev/null | wc -l)"; else echo "$lib/$m FAIL"; fi > $m.res ) &
      n=$((n+1)); [ $((n%8)) = 0 ] && wait
    done; wait
  done
done
cat $T/*/*.res | awk '$2!="0"{printf "%s:%s ", $1, $2} $2=="0"{s++} END{printf "| same %d of %d\n", s, NR}'
