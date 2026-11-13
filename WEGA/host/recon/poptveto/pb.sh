#!/bin/sh
# pb.sh NAME "DEFINES": spopt of vc with the segvc defines replaced by DEFINES (from the working tree) into $S/o_NAME
S=/tmp/SCRATCH
O=/home/olivleh1/git/P8000/tmp/oldroot; W=/home/olivleh1/git/P8000/tmp/wega31_root; E=/home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu
name=$1; B=$S/pb_$name
rm -rf $B $S/o_$name; mkdir -p $B/src/cmd $B/src/lib
[ -d $S/wbin ] || sh /home/olivleh1/git/P8000/WEGA/host/tools/wbin.sh $S/wbin $W $O $E
cp -r /home/olivleh1/git/P8000/WEGA/src/cmd/popt $B/src/cmd/popt; cp /home/olivleh1/git/P8000/WEGA/src/lib/libc-v5.a $B/src/lib/
cd $B/src/cmd/popt && { [ -z "$PSED" ] || sed -i "$PSED" z21.c; } && { [ -z "$PPY" ] || python3 $PPY z21.c; } && sed -i "/segvc\/spopt:/,+1s|-DSPOPT [^\"]*\"|-DSPOPT $2\"|" Makefile && rm -f build/segvc/*.o build/segvc/spopt
PATH=$S/wbin:$PATH make -s build/segvc/spopt 2>&1 | grep -i "error\|undefined" | head -5
[ -f build/segvc/spopt ] || { echo "no build"; exit 1; }
cp -a $O $S/o_$name && cp build/segvc/spopt $S/o_$name/lib/vc/spopt && echo "built o_$name"
