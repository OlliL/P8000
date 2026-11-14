#!/bin/sh
# pv1.sh NAME "EXTRA DEFINES": popt and spopt of v1 with extra defines, into a copy of OLDROOT $S/o1_NAME
S=/tmp/SCRATCH; O=/home/olivleh1/git/P8000/tmp/oldroot; W=/home/olivleh1/git/P8000/tmp/wega31_root; E=/home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu
name=$1; B=$S/pv_$name
rm -rf $B $S/o1_$name; mkdir -p $B/src/cmd $B/src/lib
cp -r /home/olivleh1/git/P8000/WEGA/src/cmd/popt $B/src/cmd/popt; cp /home/olivleh1/git/P8000/WEGA/src/lib/libc-v5.a $B/src/lib/
cd $B/src/cmd/popt && { [ -z "$PSED" ] || sed -i "$PSED" z21.c; } && { [ -z "$PPY" ] || python3 $PPY z21.c || exit 1; } && sed -i "s|-DOLD_D52\"|-DOLD_D52 $2\"|" Makefile && rm -f build/nonsegv1/*.o build/nonsegv1/popt build/segv1/*.o build/segv1/spopt
PATH=$S/wbin:$PATH make -s build/nonsegv1/popt build/segv1/spopt 2>&1 | grep -i "error\|undefined" | head -5
[ -f build/nonsegv1/popt ] && [ -f build/segv1/spopt ] || { echo "no build"; exit 1; }
cp -a $O $S/o1_$name && cp build/nonsegv1/popt $S/o1_$name/lib/v1/popt && cp build/segv1/spopt $S/o1_$name/lib/v1/spopt && rm -f $S/o1_$name/tmp && mkdir -p $S/o1_$name/tmp && echo "built o1_$name"
