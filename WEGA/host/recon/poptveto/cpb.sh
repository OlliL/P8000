#!/bin/sh
# cpb.sh NAME "FLAGS" [BASEROOT]: scparse of vc built with -DSEG FLAGS into private root $S/o_NAME (copy of BASEROOT or OLDROOT)
S=/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad
O=${3:-/mnt/share/cvs/P8000/tmp/oldroot}
name=$1; B=$S/pp_$name
rm -rf $B; mkdir -p $B/src/cmd $B/src/lib/libc
cp -r /mnt/share/cvs/P8000/WEGA/src/cmd/cparse $B/src/cmd/cparse
cp /mnt/share/cvs/P8000/WEGA/src/lib/*.a $B/src/lib/ 2>/dev/null; cp -r /mnt/share/cvs/P8000/WEGA/src/lib/libc/crt $B/src/lib/libc/crt
cd $B/src/cmd/cparse && rm -f build/segvc/*.o build/segvc/scparse
sed -i "s|^\(\tcd build/segvc .* CFLAGS=\"\$(CFLAGS) -DSEG\).*\"|\1 $2\"|" Makefile
grep -n "build/segvc ;" Makefile | cut -c60-240
PATH=$S/wbin:$PATH make -s build/segvc/scparse MAKEARGS=-j8 2>&1 | grep -i "error\|undefined" | head -5
[ -f build/segvc/scparse ] || { echo "no build"; exit 1; }
[ "$O" = "$S/o_$name" ] || { rm -rf $S/o_$name; cp -a $O $S/o_$name; }
cp build/segvc/scparse $S/o_$name/lib/vc/scparse && echo "built o_$name"
