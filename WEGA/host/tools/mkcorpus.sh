#!/bin/sh
# mkcorpus.sh [PROG ...]: make the inputs of the corpora new: prepares every input (with the original under z8k-emu) and writes
# compare/PROG/corpus.tar; the cache of the results beside it (compare/PROG/cache/) is removed. Without PROG: every program of
# compare/CORPORA that owns a tar (casold uses the one of cas). The tar and the cache are not in git; corpus.sh makes a missing tar
# by itself, this script is for making it new on purpose. Environment as of corpus.sh (WEGA_ROOT, JOBS, STAGE_TMP).
HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
[ $# = 0 ] && set -- `grep -v '^#' $REPO/compare/CORPORA`
rc=0
for p in "$@"; do
	RECON_PROG=$p python3 $REPO/host/recon/stagecorpus.py --make || rc=1
done
exit $rc
