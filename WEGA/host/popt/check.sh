#!/bin/sh
# Compare ./popt (PROG=spopt: ./spopt) with the original /lib/popt (/lib/spopt) (run in z8k-emu) on every
# *.in.s of a corpus directory: output text, and for -d the statistics.
#   check.sh CORPUSDIR   (WEGA=dir with lib/ bin/; E=emulator)
C=${1:?corpus dir}
PROG=${PROG:-popt}
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
T=$(mktemp -d); ok=0; bad=0
for f in "$C"/*.in.s; do
  cp "$f" "$T/in.s"; cd "$T" || exit 1
  for fl in "" -l -d; do
    rm -f a.s b.s
    "$E" -r "$R" /lib/$PROG $fl in.s a.s >ra.txt 2>&1
    "$HERE/$PROG" $fl in.s b.s >rb.txt 2>&1
    if cmp -s a.s b.s && cmp -s ra.txt rb.txt; then ok=$((ok+1))
    else bad=$((bad+1)); echo "DIFF $fl $f"; fi
  done
done
rm -rf "$T"; echo "ok=$ok bad=$bad"; [ $bad = 0 ]
