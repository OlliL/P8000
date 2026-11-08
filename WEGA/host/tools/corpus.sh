#!/bin/sh
# corpus.sh [--wega | --host] [PROG ...]: the corpus test of each program: the original (under z8k-emu) and every candidate of the program
# (compare/PROG/progcfg.py, CANDIDATES) on the same inputs, compared (host/recon/stagecorpus.py): the WEGA program built from src/cmd
# ("wega", runs under the emulator) and the host build ("host", runs natively). Without a switch every candidate is tested; --wega or --host
# restricts the test to one of them. Without PROG: every program whose progcfg.py has a corpus (with a switch: those that have that candidate).
# The binaries are the ones built by make (progcfg CANDIDATES); CORPUS_BIN_WEGA / CORPUS_BIN_HOST name another file.
# Environment: WEGA_ROOT (the WEGA 3.1 tree, default as in host/recon/progcfg.py), JOBS, STAGE_TMP, STAGE_OPTS (-k -n N -f FILTER for stagecorpus.py).
# PROG may also be a directory with a progcfg.py. Exit status 1 when a candidate differs.
HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
C=
case "$1" in
--wega) C="-c wega"; shift;;
--host) C="-c host"; shift;;
esac
SKIP=
if [ $# = 0 ]; then
	set -- `cd $REPO/compare && grep -l '^CORPUS_BINARY\|^CANDIDATES' */progcfg.py | sed 's|/progcfg.py||'`
	[ -n "$C" ] && SKIP=-s
fi
rc=0
for p in "$@"; do
	RECON_PROG=$p python3 $REPO/host/recon/stagecorpus.py $C $SKIP $STAGE_OPTS || rc=1
done
exit $rc
