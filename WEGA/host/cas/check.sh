#!/bin/sh
# Compare the host cas with the original /bin/cas (run by z8k-emu): the same sources are
# assembled by both; messages, return code and object file must be equal.
# usage: WEGA=<file tree of the original system> sh check.sh [ns ss sl fz fzs] [-- file.s ...]
#   no argument: the relative jumps (rel.py) and all opcodes of the three modes (exh.py)
#   fz fzs:      operand patterns of the mnemonics that z8k.py does not decode (slow)
#   -- file.s:   the options -l -d -r -t -i -m -v -^ on these sources (opts.py)
# The programs are the ones of ../../src/cmd/cas/test (see test/README); NATIVE=1 makes them run
# the program under test directly.
set -e
: "${WEGA:?set WEGA=<file tree of the original system>}"
here=$(cd "$(dirname "$0")" && pwd)
test=$here/../../src/cmd/cas/test
cas=$here/cas
export WEGA NATIVE=1
modes=
files=
while [ $# -gt 0 ]; do
	case $1 in
	--) shift; files="$*"; break;;
	*) modes="$modes $1";;
	esac
	shift
done
if [ -z "$modes$files" ]; then modes="ns ss sl"; run_rel=1; fi
[ -n "$run_rel" ] && python3 "$test/rel.py" "$cas"
for m in $modes; do
	python3 "$test/exh.py" "$m" --new "$cas"
done
[ -n "$files" ] && python3 "$test/opts.py" "$cas" $files
exit 0
