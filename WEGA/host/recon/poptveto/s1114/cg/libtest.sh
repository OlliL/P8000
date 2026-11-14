#!/bin/sh
# libtest.sh ROOT : every C member of libc (cc) and slibc (scc) compiled with ROOT, against obj/NAME.o; prints the members that differ (bytes)
R=$1; T=/tmp/SCRATCH/lt_$(basename $R); rm -rf $T; mkdir -p $T
n=0; same=0; out=""
for lib in libc slibc; do c=cc; [ $lib = slibc ] && c=scc
  for d in gen stdio; do
    rm -rf $T/$lib.$d; cp -r /home/olivleh1/git/P8000/WEGA/src/lib/$lib/$d $T/$lib.$d
    for f in $T/$lib.$d/*.c; do m=$(basename $f .c); o=/home/olivleh1/git/P8000/WEGA/src/lib/$lib/obj/$m.o; [ -f $o ] || continue
      ( cd $T/$lib.$d && timeout -s KILL 60 /home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu -r $R /bin/$c -O -c $m.c >/dev/null 2>&1; if [ -f $m.o ]; then echo "$lib/$m $(cmp -l $m.o $o 2>/dev/null | wc -l)"; else echo "$lib/$m FAIL"; fi > $m.res ) &
      n=$((n+1)); [ $((n%8)) = 0 ] && wait
    done; wait
  done
done
cat $T/*/*.res | awk '$2!="0"{printf "%s:%s ", $1, $2} $2=="0"{s++} END{printf "| same %d of %d\n", s, NR}'
