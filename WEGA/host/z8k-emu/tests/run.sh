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
	args=; [ -f "$HERE/$n.args" ] && args=$(cat "$HERE/$n.args")	# options of the emulator for the run (NAME.args)
	envs=; [ -f "$HERE/$n.env" ] && envs=$(sed "s#@W#$W#g" "$HERE/$n.env")	# variables of the host for the run (NAME.env; @W the work directory)
	for cc in cc scc; do
		( cd "$W" && rm -rf pids && mkdir pids && rm -f a.out && "$EMU" -r "$ROOT" /bin/$cc -o a.out "$n.c" > cc.log 2>&1 &&
		  timeout 60 env $envs "$EMU" -r "$ROOT" $args ./a.out > "$n.$cc" 2>&1 )
		if cmp -s "$W/$n.$cc" "$HERE/$n.out"; then echo "ok   $n ($cc)"; else echo "FAIL $n ($cc)"; diff "$HERE/$n.out" "$W/$n.$cc"; fail=1; fi
	done
done
exit $fail
