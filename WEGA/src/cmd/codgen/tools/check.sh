#!/bin/sh
# Compare ./codgen (PROG=scodgen: ./scodgen) with the original /lib/codgen (/lib/scodgen) on every <id>.I of a corpus
# directory (built by corpus.sh): the assembler text and the exit status must be the same.
#   check.sh CORPUSDIR   (WEGA=dir with lib/ bin/; E=emulator)
C=${1:?corpus dir}
PROG=${PROG:-codgen}
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../../../../host/z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
T=$(mktemp -d); ok=0; bad=0
for f in "$C"/*.I; do
  id=$(basename "$f" .I)
  cp "$C/$id.I" "$T/x.I"; cp "$C/$id.D" "$T/x.D"; cp "$C/$id.S" "$T/x.S"; cd "$T" || exit 1
  "$E" -r "$R" /lib/$PROG -E x <x.I >a.s 2>ra.txt; ra=$?
  cp "$HERE/../$PROG" "$T/my$PROG"; "$E" -r "$R" ./my$PROG -E x <x.I >b.s 2>rb.txt; rb=$?
  if cmp -s a.s b.s && [ $ra = $rb ]; then ok=$((ok+1)); else bad=$((bad+1)); echo "DIFF $f"; fi
done
rm -rf "$T"; echo "ok=$ok bad=$bad"; [ $bad = 0 ]
