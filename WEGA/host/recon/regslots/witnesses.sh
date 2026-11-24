#!/bin/sh
# witnesses.sh: the search for register slot witnesses among the sys3 programs of src/COVERAGE.md with D1 loose or unknown:
# each System III source by the shipped cc (linked, else the object), fscan.py against the original; the found functions
# whose frame differs. Output in $RS_WORK/witnesses/ (cand.txt, out.txt).
HERE=$(cd $(dirname $0) && pwd)
WEGA=$(cd $HERE/../../.. && pwd)
WORK=${RS_WORK:-$(cd $WEGA/.. && pwd)/tmp/tmp/regslots}/witnesses
ROOT=${WEGA_ROOT:-$(cd $WEGA/.. && pwd)/tmp/wega31_root}
export PATH=${WBIN:-$(cd $WEGA/.. && pwd)/tmp/wbin}:$PATH
mkdir -p $WORK; cd $WORK
grep -E '\| sys3 \|' $WEGA/src/COVERAGE.md | grep -E 'E707|E711' | grep -E 'D1 (loose|-)' |
	awk -F'|' '{gsub(/[` ]/,"",$2); gsub(/[` ]/,"",$4); print $2, $4}' | grep '\.c$' > cand.txt
: > out.txt
while read orig src; do
	n=$(basename $orig); mkdir -p b/$n; cp $WEGA/../$src b/$n/x.c 2>/dev/null || { echo "$n: no source" >> out.txt; continue; }
	(cd b/$n && timeout 120 cc -O -o x x.c >build.log 2>&1); o=b/$n/x; [ -f $o ] || o=b/$n/x.o
	[ -f $o ] || { echo "$n: build failed" >> out.txt; continue; }
	echo "== $n $(python3 $HERE/fscan.py $o $ROOT/$orig | tr '\n' ';')" >> out.txt
done < cand.txt
grep -v 'found 0,\|failed\|no source' out.txt
