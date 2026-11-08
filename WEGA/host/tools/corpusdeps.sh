#!/bin/sh
# corpusdeps.sh [PROG ...]: runs the first inputs (STAGE_OPTS, default -n 30) of the corpora under strace and checks that the dependencies the
# corpus declares (progcfg CORPUS_ROOT_FILES, CORPUS_DEPENDS) cover every file of the tree the runs read and every program they start.
HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
[ $# = 0 ] && set -- `grep -v '^#' $REPO/compare/CORPORA`
D=${DEPS_TMP:-/tmp/wega_deps}
rm -rf $D
for n in "$@"; do
	STAGE_STRACE=$D CORPUS_NOCACHE=1 STAGE_TMP=$D.stage_$n STAGE_OPTS="${STAGE_OPTS:--n 30}" JOBS=${JOBS:-4} sh $HERE/corpus.sh $n > $D.$n.log 2>&1 &
done
wait
python3 $HERE/corpusdeps.py $D "$@"
