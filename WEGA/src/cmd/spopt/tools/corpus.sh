#!/bin/sh
# corpus.sh [SPOPT]: our native spopt (default ../../popt/spopt, the normal build) against the original /lib/spopt, both
# run in z8k-emu, on the output of scodgen of its corpus (*.out.s, codgen/tools/corpus.sh with PROG=scodgen), without
# option, with -l and with -d (output and statistics). JOBS inputs at a time (default: all cores but one), each in a
# directory of its own below TMPDIR (default /tmp).
# Environment: WEGA_ROOT (the WEGA 3.1 tree), RECON_TMP (the corpus is RECON_TMP/cg_corpus_scodgen), JOBS, TMPDIR.
HERE=`cd \`dirname $0\` && pwd`
R=${WEGA_ROOT:-/mnt/share/cvs/P8000/tmp/wega31_root}
E=$HERE/../../../../host/z8k-emu/z8k-emu
if [ "$1" = -one ]; then		# -one MYSPOPT INPUT, one directory, a line per run
	d=`mktemp -d -p ${TMPDIR:-/tmp}`; cd $d; cp $3 in.s; cp $2 myspopt
	for fl in "" -l -d; do
		rm -f a.s b.s
		$E -r $R /lib/spopt $fl in.s a.s > ra.txt 2>&1; ra=$?
		$E -r $R ./myspopt $fl in.s b.s > rb.txt 2>&1; rb=$?
		if cmp -s a.s b.s && cmp -s ra.txt rb.txt && [ $ra = $rb ]; then echo ok; else echo "DIFF $fl $3 $ra $rb"; fi
	done
	cd /; rm -rf $d
	exit 0
fi
P=${1:-$HERE/../../popt/spopt}
C=${CORPUS:-${RECON_TMP:-/mnt/share/cvs/P8000/tmp/tmp}/cg_corpus_scodgen}
T=`mktemp -p ${TMPDIR:-/tmp}`; M=`mktemp -p ${TMPDIR:-/tmp}`; cp $P $M
find $C -name '*.out.s' | sort | xargs -P ${JOBS:-`expr \`nproc\` - 1`} -n 1 sh $HERE/corpus.sh -one $M > $T
grep DIFF $T
echo "ok=`grep -c '^ok' $T` bad=`grep -c DIFF $T`"
rm -f $T $M
