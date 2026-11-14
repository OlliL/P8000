#!/bin/sh
# rawcmp.sh NAME: spopt of o_NAME on the raw code of every cas module, assembled; modules whose object differs from o_base's (bytes)
S=/tmp/SCRATCH; E=/home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu; C=/home/olivleh1/git/P8000/WEGA/host/cas/casold
mkdir -p $S/allraw/out_$1
for f in $S/allraw/*.s; do m=$(basename $f .s); (cd $S/allraw; rm -f out_$1/$m.s out_$1/$m.o; timeout -s KILL 20 $E -r $S/o_$1 /lib/vc/spopt $m.s out_$1/$m.s 2>/dev/null; cd out_$1 && $C -u -o $m.o $m.s >/dev/null 2>&1) & done; wait
[ "$1" = base ] && exit 0
for f in $S/allraw/*.s; do m=$(basename $f .s); if [ ! -f $S/allraw/out_$1/$m.o ]; then printf '%s:FAIL ' $m; continue; fi; n=$(cmp -l $S/allraw/out_base/$m.o $S/allraw/out_$1/$m.o 2>&1 | wc -l); [ "$n" = 0 ] || printf '%s:%s ' $m $n; done; echo
