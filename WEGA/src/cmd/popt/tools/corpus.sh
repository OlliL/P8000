#!/bin/sh
# corpus.sh [POPT]: our native popt (default ../popt, the normal build) against the original /lib/popt, both run in
# z8k-emu, on every *.in.s of the popt corpus, without option, with -l and with -d (output and statistics).
# Environment: WEGA_ROOT (the WEGA 3.1 tree), RECON_TMP (scratch; the corpus is RECON_TMP/popt/corpus).
HERE=`cd \`dirname $0\` && pwd`
P=${1:-$HERE/../popt}
R=${WEGA_ROOT:-/mnt/share/cvs/P8000/tmp/wega31_root}
RECON_TMP=${RECON_TMP:-/mnt/share/cvs/P8000/tmp/tmp}
C=${CORPUS:-$RECON_TMP/popt/corpus}
E=$HERE/../../../../host/z8k-emu/z8k-emu
T=`mktemp -d -p $RECON_TMP`; cp $P $T/mypopt; ok=0; bad=0
cd $T
for f in $C/*.in.s; do
	cp $f in.s
	for fl in "" -l -d; do
		rm -f a.s b.s
		$E -r $R /lib/popt $fl in.s a.s > ra.txt 2>&1
		$E -r $R ./mypopt $fl in.s b.s > rb.txt 2>&1
		if cmp -s a.s b.s && cmp -s ra.txt rb.txt; then ok=`expr $ok + 1`; else bad=`expr $bad + 1`; echo "DIFF $fl $f"; fi
	done
done
cd /; rm -rf $T; echo "ok=$ok bad=$bad"
