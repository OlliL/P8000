#!/bin/sh
# corpus.sh [--host BINARY] [PROG ...]: the corpus test of each program: the original and our build on the same inputs, compared
# (host/recon/stagecorpus.py with src/cmd/PROG/tools/progcfg.py). Without PROG: every program whose progcfg.py has a corpus.
# Our binary is the one built in the source directory (CORPUS_BINARY of progcfg.py; CORPUS_BIN=file tests another).
# Environment: WEGA_ROOT (the WEGA 3.1 tree, default as in host/recon/progcfg.py), JOBS, STAGE_TMP.
# --host BINARY: BINARY is a host (Linux) build of PROG, run natively against the original (host/PROG); needs one PROG.
# Exit status 1 when a program differs.
HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
if [ "$1" = --host ]; then
	[ $# = 3 ] || { echo 'usage: corpus.sh --host BINARY PROG' >&2; exit 2; }
	CORPUS_HOST=1 CORPUS_BIN=`cd \`dirname "$2"\` && pwd`/`basename "$2"`; export CORPUS_HOST CORPUS_BIN
	shift 2
fi
if [ $# = 0 ]; then
	set -- `cd $REPO/src/cmd && grep -l '^CORPUS_BINARY' */tools/progcfg.py | sed 's|/tools/progcfg.py||'`
fi
rc=0
for p in "$@"; do
	RECON_PROG=$p python3 $REPO/host/recon/stagecorpus.py || rc=1
done
exit $rc
