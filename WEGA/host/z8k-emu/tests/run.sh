#!/bin/sh
# run.sh EMU WEGA_ROOT: the own tests of z8k-emu (tests/*.c: built with cc and scc under the emulator, the output
# compared with tests/NAME.out); exit status 0 when all hold. Called by tools/checkall.sh.
set -u
EMU=$1; ROOT=$2
HERE=$(cd "$(dirname "$0")" && pwd)
W=$(mktemp -d "${TMPDIR:-/tmp}/emutest.XXXXXX")
trap 'rm -rf "$W"' EXIT
fail=0
for t in "$HERE"/*.c; do
	n=$(basename "$t" .c)
	cp "$t" "$W/$n.c"
	for cc in cc scc; do
		( cd "$W" && rm -f a.out && "$EMU" -r "$ROOT" /bin/$cc -o a.out "$n.c" > cc.log 2>&1 &&
		  timeout 60 "$EMU" -r "$ROOT" ./a.out > "$n.$cc" 2>&1 )
		if cmp -s "$W/$n.$cc" "$HERE/$n.out"; then echo "ok   $n ($cc)"; else echo "FAIL $n ($cc)"; diff "$HERE/$n.out" "$W/$n.$cc"; fail=1; fi
	done
done
exit $fail
