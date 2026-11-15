#!/bin/sh
# corpus.sh [--wega | --host] [PROG ...]: the corpus test of each program (compare/PROG/corpus.py, host/recon/corpusrun.py): the
# original (under z8k-emu) and every candidate of the program on the same cases, compared: the WEGA program of the shipped build
# ("wega", runs under the emulator) and the host build ("host", compiler chain only, runs natively). Without a switch every
# candidate is tested; --wega or --host restricts the test to one of them. Without PROG: every program listed in compare/CORPORA
# (with a switch: those that have that candidate).
# The binaries are the ones built by make (corpus.py CANDIDATES); CORPUS_BIN_WEGA / CORPUS_BIN_HOST name another file.
# Environment: WEGA_ROOT (the WEGA 3.1 tree), OLDROOT (casold), JOBS, CORPUS_DB (the store, default WEGA/.corpus), CORPUS_TMP
# (work directories), CORPUS_NOCACHE=1 (ignore the store), CORPUS_OPTS (-k -f FILTER for corpusrun.py).
# PROG may also be a directory with a corpus.py. Exit status 1 when a candidate differs.
HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
C=
case "$1" in
--wega) C=wega; shift;;
--host) C=host; shift;;
esac
ALL=0
if [ $# = 0 ]; then
	set -- `grep -v '^#' $REPO/compare/CORPORA`
	ALL=1
fi
rc=0
for p in "$@"; do
	d=$p; [ -f $d/corpus.py ] || d=$REPO/compare/$p
	if [ -n "$C" ]; then
		if grep -q "'$C':" $d/corpus.py 2>/dev/null; then :
		elif [ $ALL = 1 ]; then continue
		fi
		python3 $REPO/host/recon/corpusrun.py -c $C $CORPUS_OPTS $p || rc=1
	else
		python3 $REPO/host/recon/corpusrun.py $CORPUS_OPTS $p || rc=1
	fi
done
exit $rc
