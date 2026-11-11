#!/bin/sh
# cgb.sh NAME "SW1 SW2 ...": scodgen of vc without the given switches of the OLD_VC block (taken from the working tree),
# installed into a private root $S/o_NAME (copy of OLDROOT). Also +SW adds a define.
S=/tmp/claude-1000/-mnt-share-cvs-P8000/6b8d4c9d-ace2-40e9-b4e5-33a880fd019b/scratchpad
O=/mnt/share/cvs/P8000/tmp/oldroot; W=/mnt/share/cvs/P8000/tmp/wega31_root; E=/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu
name=$1; sw=$2
B=$S/cb_$name
rm -rf $B $S/o_$name; mkdir -p $B/src/cmd $B/src/lib/libc
[ -d $S/wbin ] || sh /mnt/share/cvs/P8000/WEGA/host/tools/wbin.sh $S/wbin $W $O $E
cp -r /mnt/share/cvs/P8000/WEGA/src/cmd/codgen $B/src/cmd/codgen
cp /mnt/share/cvs/P8000/WEGA/src/lib/libc-v5.a $B/src/lib/; cp -r /mnt/share/cvs/P8000/WEGA/src/lib/libc/crt $B/src/lib/libc/crt; cp /mnt/share/cvs/P8000/WEGA/src/lib/libc/* $B/src/lib/libc/ 2>/dev/null
python3 - "$B/src/cmd/codgen/oldcc.h" $sw <<'PY'
import re,sys
p=sys.argv[1]; s=open(p).read()
i=s.index('#ifdef OLD_VC'); j=s.index('#endif',i); blk=s[i:j]
for w in sys.argv[2:]:
    if w.startswith('+'):
        blk+='#define %s\n'%w[1:]
    else:
        n=re.sub(r'^#define %s\b.*\n'%w,'',blk,count=1,flags=re.M)
        assert n!=blk, w
        blk=n
open(p,'w').write(s[:i]+blk+s[j:])
PY
cd $B/src/cmd/codgen && rm -f build/segvc/*.o build/segvc/scodgen && PATH=$S/wbin:$PATH make -s build/segvc/scodgen MAKEARGS=-j8 2>&1 | grep -i "error\|undefined" | head -5
[ -f build/segvc/scodgen ] || { echo "no build"; exit 1; }
cp -a $O $S/o_$name && cp build/segvc/scodgen $S/o_$name/lib/vc/scodgen && echo "built o_$name"
