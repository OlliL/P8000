#!/bin/sh
# treescan.sh TREE: the programs of src/COVERAGE.md that are exact by the shipped cc, built in TREE (a copy of src
# built with cc = ccx, the scratch cparse), against the originals by fscan.py; per program the date of its SCCS
# line and the functions whose frame differs (name ours orig)
H=$(cd $(dirname $0) && pwd); WEGA=$(cd $H/../../.. && pwd)
ROOT=${WEGA_ROOT:-$(cd $WEGA/.. && pwd)/tmp/wega31_root}; T=$1
awk -F'|' '$5~/exact/ && $6~/^ *cc *$/{gsub(/[` ]/,"",$2); gsub(/[` ]/,"",$4); print $2, $4}' $WEGA/src/COVERAGE.md |
while read f s; do
	case $s in src/cmd/*.c) p=$T/${s%.c} ;; src/cmd/*) p=$T/$s/$(basename $f) ;; *) continue ;; esac
	[ -f $p ] || { echo "$f: no product ($p)"; continue; }
	d=$(strings -a $ROOT/$f | grep '@\[\$\]' | grep -v 'crt0\|malloc' | head -1 | grep -o '[0-9][0-9]/[0-9][0-9]/[0-9][0-9]')
	r=$(python3 $H/fscan.py $p $ROOT/$f 2>/dev/null | grep -v '^#' | awk '{print $1"("$2"/"$3")"}' | tr '\n' ' ')
	echo "$f ${d:-nodate}: ${r:-ok}"
done
