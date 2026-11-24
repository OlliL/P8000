#!/bin/sh
# score.sh CC TAG: every file of the test set (setup.sh) by CC -O -c (the knobs XS XR XM XH XV XF XG from the
# environment), the frames of its functions against the originals (fscan.py); prints per file the functions whose frame
# differs (name and bytes, + = the original is larger) and the total. CC: the shipped cc (cc of the PATH) or $RS_WORK/ccx.
HERE=$(cd $(dirname $0) && pwd)
WEGA=$(cd $HERE/../../.. && pwd)
WORK=${RS_WORK:-$(cd $WEGA/.. && pwd)/tmp/tmp/regslots}
ROOT=${WEGA_ROOT:-$(cd $WEGA/.. && pwd)/tmp/wega31_root}
cc=$1; tag=$2; D=$WORK/run/$tag; mkdir -p $D
tot=0
while read prog orig files; do
	for f in $files; do
		mkdir -p $D/$prog; cp $WORK/ts/$prog/* $D/$prog/ 2>/dev/null
		(cd $D/$prog && $cc -O -c $f >/dev/null 2>&1)
		o=$D/$prog/${f%.c}.o
		[ -f $o ] || { echo "$prog/$f: no object"; continue; }
		r=$(python3 $HERE/fscan.py $o $ROOT/$orig | grep -v '^#' | awk '{print $1 $4}' | tr '\n' ' ')
		[ -n "$r" ] && echo "$prog/$f: $r"
		n=$(echo $r | wc -w); tot=$((tot+n))
	done
done < $WORK/ts/list
echo "TOTAL $tag $tot"
