#!/bin/sh
# checkall.sh [-b]: one check for everything that is byte-identical.
#
# Works on copies of the files known to git (also changed and new ones, not the built ones) below CHECK_TMP, so that it
# proves the repository alone is enough and leaves the working tree alone:
#   1. the old compilers from the sources, with the shipped tools (make -C host oldroot), into a tree of their own;
#   2. in a fresh copy every program of TABLE with its compiler, stripped, compared with cmp;
#   3. src/cmd with the shipped tools (make -C host verify-wega): SHIPPED must be identical, the rest is listed as open;
#   4. the corpora of the passes against the originals: cpp, cparse, scparse as built in 2 (their output depends on their
#      data layout), codgen, scodgen, popt, spopt in the normal build of 1, whose source also is the old compiler;
#   5. make -C host check: the host tools and the libc members; a member that differs must be in src/lib/libc/oldcc.
# -b: only 1 and 2. Prints one line per check, the logs stay in CHECK_TMP; exit status 0 when all pass.
# A program joins by a line in TABLE or SHIPPED.
#
# Environment: WEGA (the WEGA 3.1 tree), CHECK_TMP (default /tmp/wega_check, a local disk),
# CORPUS_TMP (where the corpora are kept, default the scratch of oldcc.sh).

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
WEGA=${WEGA:-$REPO/../tmp/wega31_root}
WEGA=`cd $WEGA && pwd`
T=${CHECK_TMP:-/tmp/wega_check}
CORPUS_TMP=${CORPUS_TMP:-$REPO/../tmp/tmp}
EMU=$REPO/host/z8k-emu/z8k-emu
W="$EMU -r $WEGA"
OLD=$T/oldroot
FAIL=0
BUILDONLY=0
[ "$1" = -b ] && BUILDONLY=1

# program dir, make target, compiler, original(s): the first is compared, the others must be links to the same bytes
TABLE='
cpp	cpp	ccv0p	lib/cpp
cparse	cparse	ccv0p	lib/cparse
cparse	scparse	ccv0p	lib/scparse
codgen	codgen	ccv0p	lib/codgen
codgen	scodgen	ccv0p	lib/scodgen
popt	popt	ccv0p	lib/popt
popt	spopt	ccv0p	lib/spopt
size	size	ccv1	bin/size
strip	strip	ccv1	usr/bin/strip
mv	mv	ccv1	bin/mv bin/cp bin/ln
rm	rm	ccv1	bin/rm
'

# with the shipped cc; verify-wega compares them
SHIPPED='ar cc/cc cc/scc getud putud copydisk/1.5/copydisk dump dumpdir format/1.5/format restor sync tar verify/1.5/verify
BOOT0/pb.image standalone/sa.cat standalone/sa.format standalone/sa.verify standalone/sa.mkfs standalone/sa.install
standalone/sa.timer standalone/sa.shipdisk'

# the files known to git, in DIR/WEGA; the emulator and the host tools are not rebuilt here
copytree() {
	rm -rf $1
	mkdir -p $1/WEGA
	(cd $REPO && git ls-files -co --exclude-standard -- src host | tar cf - -T - 2>/dev/null) | (cd $1/WEGA && tar xf -)
	mkdir -p $1/WEGA/host/z8k-emu
	cp $EMU $1/WEGA/host/z8k-emu/z8k-emu
}


result() {
	if [ "$2" = ok ]; then
		printf '%-28s %s\n' "$1" "$3"
	else
		printf '%-28s FAIL %s\n' "$1" "$3"
		FAIL=1
	fi
}

mkdir -p $T

# 1. the old compilers
copytree $T/chain
rm -rf $OLD
if make -C $T/chain/WEGA/host oldroot WEGA=$WEGA OLDROOT=$OLD > $T/oldroot.log 2>&1; then
	result 'old compilers' ok 'v1 v0 v0p built'
else
	result 'old compilers' fail "see $T/oldroot.log"
	exit 1
fi

# 2. the programs
copytree $T/prog
S=$T/prog/WEGA/src/cmd
echo "$TABLE" | while read dir target cc origs; do
	[ -n "$dir" ] || continue
	set -- $origs
	first=$1
	log=$T/$target.log
	if ! (cd $S/$dir && make $target CC="$EMU -r $OLD /bin/$cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" \
		LDBIN="$W /bin/ld" YACC="$W /usr/bin/yacc" CRT0=$WEGA/lib/crt0.o) > $log 2>&1; then
		echo "$target	FAILBUILD	$log"
		continue
	fi
	cp $S/$dir/$target $T/$target.stripped
	(cd $T && $W /usr/bin/strip $target.stripped) >> $log 2>&1
	if cmp -s $T/$target.stripped $WEGA/$first; then
		msg="identical, $cc"
		shift
		for o in "$@"; do
			cmp -s $T/$target.stripped $WEGA/$o && msg="$msg, $o" || msg="$msg, DIFFERS FROM $o"
		done
		echo "$target	ok	$msg"
	else
		echo "$target	FAILCMP	$cc: python3 host/tools/bincmp.py $WEGA/$first $T/$target.stripped"
	fi
done > $T/programs.txt
while IFS='	' read name state msg; do
	if [ "$state" = ok ]; then result "$name" ok "$msg"; else result "$name" fail "$msg"; fi
done < $T/programs.txt

[ $BUILDONLY = 1 ] && { [ $FAIL = 0 ] && echo ALL OK || echo FAILED; exit $FAIL; }

# 3. src/cmd with the shipped tools
copytree $T/cmd
make -C $T/cmd/WEGA/host verify-wega WEGA=$WEGA > $T/verify.log 2>&1
for p in $SHIPPED; do
	if grep -q "^$p: identical" $T/verify.log; then result "$p" ok 'identical, shipped cc'; else result "$p" fail "see $T/verify.log"; fi
done
for f in chkdiff uuclear whodo; do
	cmp -s $T/cmd/WEGA/src/cmd/$f $WEGA/bin/$f && result "$f" ok 'identical, a script' || result "$f" fail ''
done
echo 'open, not counted:' `grep -E ': (differs|not built)' $T/verify.log | sed 's/:.*//'`

# 4. the corpora; NAME EXPECTED-DIFFERENCES LOG
corpus() {
	bad=`sed -n 's/.*bad=\([0-9]*\)$/\1/p' $3 | tail -1`
	good=`sed -n 's/^ok=\([0-9]*\) .*/\1/p' $3 | tail -1`
	if [ -z "$bad" ]; then
		line=`sed -n 's/^.*: \([0-9]*\) of \([0-9]*\) inputs identical.*/\1 \2/p' $3 | tail -1`
		set -- "$1" "$2" "$3" $line
		[ $# = 5 ] || { result "corpus $1" fail "see $3"; return; }
		good=$4; bad=`expr $5 - $4`
	fi
	if [ "$bad" = "$2" ]; then result "corpus $1" ok "$good same, $bad different (expected $2)"
	else result "corpus $1" fail "$good same, $bad different (expected $2), see $3"; fi
}
C=$T/chain/WEGA/src/cmd
(cd $C/codgen && make scodgen CC="$W /bin/cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" > $T/scodgen_normal.log 2>&1)
(cd $C/popt && make spopt CC="$W /bin/cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" CRT0=$WEGA/lib/crt0.o > $T/spopt_normal.log 2>&1)
for p in cpp cparse scparse; do
	(cd $S/$p/tools && WEGA_ROOT=$WEGA python3 stagecorpus.py $T/$p.stripped) > $T/corpus_$p.log 2>&1
	corpus $p 0 $T/corpus_$p.log
done
for p in codgen scodgen; do
	l=$T/corpus_$p.log; : > $l
	for sub in cmd lib; do
		(cd $C/codgen && PROG=$p WEGA=$WEGA sh tools/check.sh $CORPUS_TMP/cg_corpus_$p/$sub) >> $l 2>&1
	done
	g=`sed -n 's/^ok=\([0-9]*\) .*/\1/p' $l | paste -sd+ | bc`
	b=`sed -n 's/.*bad=\([0-9]*\)$/\1/p' $l | paste -sd+ | bc`
	echo "ok=$g bad=$b" > $l.sum
	corpus $p 0 $l.sum
done
(cd $C/popt && WEGA_ROOT=$WEGA RECON_TMP=$T CORPUS=$CORPUS_TMP/popt/corpus sh tools/corpus.sh $C/popt/popt) > $T/corpus_popt.log 2>&1
corpus popt 0 $T/corpus_popt.log
# spopt: 2 differ only in the line "NK core" of -d: the place of the heap in the normal build (OLDCC_LOG.md)
(cd $S/spopt/tools && WEGA_ROOT=$WEGA RECON_TMP=$CORPUS_TMP sh corpus.sh $C/popt/spopt) > $T/corpus_spopt.log 2>&1
corpus spopt 2 $T/corpus_spopt.log

# 5. the host tools and the libc members
if CORPUS=$CORPUS_TMP/popt/corpus make -C $T/chain/WEGA/host check WEGA=$WEGA > $T/hostcheck.log 2>&1; then
	result 'host tools' ok 'cc ar binutils cas ld popt'
else
	result 'host tools' fail "see $T/hostcheck.log"
fi
for m in `sed -n 's/^DIFF \([^ ]*\) .*/\1/p; s/^MISSING \([^ ]*\)$/\1/p' $T/hostcheck.log`; do
	grep -q "^ok  *$m (0_1.s\|^ok  *$m (3_3.s" $T/hostcheck.log || result "libc $m" fail 'neither C nor oldcc'
done
result 'libc' ok "`grep -cE '^(DIFF|MISSING) ' $T/hostcheck.log` members only from oldcc, all others from source"

[ $FAIL = 0 ] && echo ALL OK || echo FAILED
exit $FAIL
