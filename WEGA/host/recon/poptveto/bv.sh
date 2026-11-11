#!/bin/sh
# bv.sh "EXTRA CFLAGS": spopt of vc with extra switches into OLDROOT/lib/vc/spopt (restore: cp spopt.vc.orig back)
S=/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad
O=/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad/o2; W=/mnt/share/cvs/P8000/tmp/wega31_root; E=/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu
rm -rf $S/pb; mkdir -p $S/pb $S/wbin; sh /mnt/share/cvs/P8000/WEGA/host/tools/wbin.sh $S/wbin $W $O $E
mkdir -p $S/pb/src/cmd $S/pb/src/lib; cp -r /mnt/share/cvs/P8000/WEGA/src/cmd/popt $S/pb/src/cmd/popt; cp /mnt/share/cvs/P8000/WEGA/src/lib/libc-v5.a $S/pb/src/lib/
cd $S/pb/src/cmd/popt && rm -f build/segvc/*.o build/segvc/spopt
PATH=$S/wbin:$PATH make -s build/segvc/spopt CFLAGS="-O $1" 2>&1 | tail -3
ls -l build/segvc/spopt && cp build/segvc/spopt $O/lib/vc/spopt
