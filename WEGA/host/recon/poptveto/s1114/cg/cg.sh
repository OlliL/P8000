#!/bin/sh
# cg.sh NAME "DEFINES" : codgen and scodgen = shipped sources + DEFINES, into a copy of OLDROOT $S/c_NAME as /lib/codgen and /lib/scodgen
S=/tmp/SCRATCH; O=/home/olivleh1/git/P8000/tmp/oldroot
name=$1; B=$S/cg_$name
rm -rf $B $S/c_$name; mkdir -p $B/src/cmd/cc $B/src/lib/libc
cp -r /home/olivleh1/git/P8000/WEGA/src/cmd/cc/codgen $B/src/cmd/cc/codgen; cp /home/olivleh1/git/P8000/WEGA/src/lib/libc-v5.a $B/src/lib/; mkdir -p $B/src/lib/libc/nonseg; cp -r /home/olivleh1/git/P8000/WEGA/src/lib/libc/nonseg/crt $B/src/lib/libc/nonseg/ 2>/dev/null
cd $B/src/cmd/cc/codgen && { [ -z "$PPY" ] || python3 $PPY . || exit 1; } && sed -i "/^build\/nonsegv1\/codgen:/,+1s|-DOLD_D3\"|$2\"|; /^build\/segv1\/scodgen:/,+1s|-DSEG -DOLD_D3\"|-DSEG $2\"|" Makefile && rm -f build/nonsegv1/*.o build/nonsegv1/codgen build/segv1/*.o build/segv1/scodgen
PATH=$S/wbin:$PATH make -s build/nonsegv1/codgen build/segv1/scodgen 2>&1 | grep -i "error\|undefined" | head -5
[ -f build/nonsegv1/codgen ] && [ -f build/segv1/scodgen ] || { echo "no build"; exit 1; }
cp -a $O $S/c_$name && rm -f $S/c_$name/lib/codgen $S/c_$name/lib/scodgen $S/c_$name/tmp && mkdir $S/c_$name/tmp && cp build/nonsegv1/codgen $S/c_$name/lib/codgen && cp build/segv1/scodgen $S/c_$name/lib/scodgen && echo "built c_$name"
