#!/bin/sh
# checkall.sh [-b]: one check for everything that is byte-identical.
#
# Works on copies of the files known to git (also changed and new ones, not the built ones) below CHECK_TMP, so that it
# proves the repository alone is enough and leaves the working tree alone:
#   1. make in src: the old compilers with the shipped tools, the libraries, the old compilers again, cmd/;
#   2. make install PREFIX=CHECK_TMP/install: every file there against the file of the same name in WEGA with cmp (the
#      Makefiles strip as the originals are stripped); every member of lib/libc/libc.a against the original;
#   3. the corpora (host/tools/corpus.sh, side by side; the prepared inputs are compare/NAME/corpus.tar of the working tree (made when missing,
#      host/tools/mkcorpus.sh makes them new) and the results are cached beside them (compare/NAME/cache, key: see stagecorpus.py)) of every program with a compare/NAME/progcfg.py, every candidate: the WEGA program built
#      with the shipped cc (scc) and the libc of its Makefile (the byte-identical builds are proven by 2) and the host build;
# -b: only 1 and 2. Prints one line per check, the logs stay in CHECK_TMP; exit status 0 when all pass.
# A program joins by its install target (src/cmd/Makefile).
#
# Environment: WEGA (the WEGA 3.1 tree), CHECK_TMP (default /tmp/wega_check, a local disk).

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
WEGA=${WEGA:-$REPO/../tmp/wega31_root}
WEGA=`cd $WEGA && pwd`
T=${CHECK_TMP:-/tmp/wega_check}
EMU=$REPO/host/z8k-emu/z8k-emu
OLD=$T/oldroot
FAIL=0
BUILDONLY=0
[ "$1" = -b ] && BUILDONLY=1
# the parallel make: MAKEARGS reaches the directories that can build in parallel (src/Makefile); MAKEJOBS=-j1 for a serial run
J=${MAKEJOBS:--j`nproc`}

# the time of every step, in $T/timing.log: seconds of the step, seconds since the start, the step (a table at the end)
now() { date +%s.%N; }
START=`now`
LAST=$START
stamp() {
	n=`now`
	awk -v a="$LAST" -v b="$n" -v s="$START" -v l="$1" 'BEGIN { printf "%8.1f %8.1f  %s\n", b - a, b - s, l }' >> $T/timing.log
	LAST=$n
}

# not byte-identical yet: reported, not counted
OPEN='bin/cas'

# the files known to git, in DIR/WEGA; the emulator is not rebuilt here
copytree() {
	rm -rf $1
	mkdir -p $1/WEGA
	(cd $REPO && git ls-files -co --exclude-standard -- src host compare | tar cf - -T - 2>/dev/null) | (cd $1/WEGA && tar xf -)
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

finish() {
	stamp 'end'
	echo
	echo "time of the steps ($T/timing.log; seconds of the step, seconds since the start):"
	cat $T/timing.log
	if [ -s $T/timing_corpus.log ]; then
		echo
		echo "time of the corpora (wall; CPU seconds of the phases over all cases: prepare, original, wega, host, rest; cases):"
		cat $T/timing_corpus.log
	fi
	[ $FAIL = 0 ] && echo ALL OK || echo FAILED
	exit $FAIL
}

mkdir -p $T
rm -f $T/timing.log $T/timing_corpus.log
echo "machine: `nproc` cores, load `cut -d' ' -f1-3 /proc/loadavg`, MAKEARGS=$J" >> $T/timing.log

# 1. everything from the sources
copytree $T/tree
stamp '1 copy of the files of git (tree)'
rm -rf $OLD $T/bin
sh $HERE/oldroot.sh $WEGA $OLD
sh $HERE/wbin.sh $T/bin $WEGA $OLD $EMU
stamp '1 oldroot.sh, wbin.sh'
# make all = stage1 lib stage2 cmds, one after the other so that every part is timed
: > $T/make.log
for g in stage1 lib stage2 cmds; do
	if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make $g OLDROOT=$OLD MAKEARGS="$J") >> $T/make.log 2>&1; then
		stamp "1 make $g in src"
	else
		stamp "1 make $g in src (failed)"
		result 'make in src' fail "make $g, see $T/make.log"
		finish
	fi
done
result 'make in src' ok 'stage1 lib stage2 cmds'

# 2. the bytes: make install into an empty root, every file there against the file of the same name in WEGA
I=$T/install
rm -rf $I
mkdir -p $I/bin $I/etc $I/lib $I/usr/bin
if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make install PREFIX=$I MAKEARGS="$J") > $T/install.log 2>&1; then :; else
	result 'make install' fail "see $T/install.log"
	finish
fi
stamp '2 make install'
same=0
o=
for f in `cd $I && find . -type f | sed 's|^\./||' | sort`; do
	case " $OPEN " in *" $f "*)
		cmp -s $I/$f $WEGA/$f && result "$f" fail 'identical now: remove it from OPEN' || o="$o $f"
		continue ;;
	esac
	if cmp -s $I/$f $WEGA/$f; then same=`expr $same + 1`; else result "$f" fail "python3 host/tools/bincmp.py $WEGA/$f $I/$f"; fi
done
result 'install root' ok "$same files identical with WEGA ($I)"
echo "open, not counted:$o"
stamp '2 cmp of the install root with WEGA'
# the members of libc.a, each from its source
L=$T/tree/WEGA/src/lib/libc
rm -rf $T/orig
mkdir -p $T/orig
(cd $T/orig && $EMU -r $WEGA /bin/ar x /lib/libc.a)
stamp '2 ar x of the original libc.a'
same=0
for f in `cd $T/orig && ls`; do
	if cmp -s $T/orig/$f $L/obj/$f; then same=`expr $same + 1`; else result "libc $f" fail "differs or missing: $L/obj/$f"; fi
done
result 'libc.a members' ok "$same identical, from source"
stamp '2 cmp of the members of libc.a'

[ $BUILDONLY = 1 ] && finish

# 3. the corpora; NAME LOG: one line per candidate ("NAME [wega]: 288 of 288 inputs identical", [host] likewise)
corpus() {
	sed -n 's/^\(.*\) \[\([a-z]*\)\]: \([0-9]*\) of \([0-9]*\) inputs identical.*/\1 \2 \3 \4/p' $2 > $T/corpus_lines
	if [ ! -s $T/corpus_lines ]; then result "corpus $1" fail "see $2"; return; fi
	while read nm cand a b; do
		if [ "$a" = "$b" ] && [ "$b" -gt 0 ]; then result "corpus $nm [$cand]" ok "$a of $b inputs identical"
		else result "corpus $nm [$cand]" fail "see $2"; fi
	done < $T/corpus_lines
}
# the shipped build: every program with a corpus built from the same sources by the shipped cc (scc) and the libc of
# its Makefile; a byte-identical binary cannot show anything in a corpus, this build tests the sources.
# The host programs (compare/NAME/progcfg.py, CANDIDATES) are built beside them by the compiler of the host.
copytree $T/shipped
cp $T/tree/WEGA/src/lib/*.a $T/shipped/WEGA/src/lib/
stamp '3 copy of the files of git (shipped)'
C=$T/shipped/WEGA/src/cmd
: > $T/shipped.log
build() {	# build LABEL DIR make-arguments: the program of the shipped build, with its time
	l=$1; d=$2; shift 2
	(PATH=$T/bin:$PATH; export PATH; cd $d && make "$@" MAKEARGS="$J") >> $T/shipped.log 2>&1
	stamp "3 shipped build: $l"
}
build 'ar ranlib strip size' $C ar ranlib strip size CC=cc
build cpp $C/cpp cpp CC=cc
build 'cparse scparse' $C/cparse all CC=cc
build 'codgen scodgen' $C/codgen all CC=cc
build 'popt spopt' $C/popt all CC=cc
build ld $C/ld ld CC=cc
build sld $C/sld sld SCC=scc
build cas $C/cas all SCC=scc
build 'cc scc' $C/cc all CC=cc
: > $T/hostbuild.log
for p in cpp ar binutils cc popt codgen cparse cas ld; do
	make -C $T/shipped/WEGA/host $p >> $T/hostbuild.log 2>&1
	stamp "3 host build: $p"
done
make -C $T/shipped/WEGA/host/cas casold >> $T/hostbuild.log 2>&1
stamp '3 host build: casold'
# the corpora run side by side, each in a work directory of its own; every candidate of a program is tested against the
# same inputs (the original runs once)
CS=$T/shipped/WEGA/host/tools/corpus.sh
CORP=`grep -v '^#' $T/shipped/WEGA/compare/CORPORA`
echo "corpora start: load `cut -d' ' -f1-3 /proc/loadavg`" >> $T/timing.log
for n in $CORP; do
	CORPUS_STORE=$REPO/compare STAGE_TMP=$T/stage_$n JOBS=2 WEGA_ROOT=$WEGA OLDROOT=$OLD sh $CS $n > $T/corpus_$n.log 2>&1 &
done
wait
stamp '3 all corpora (in parallel)'
echo "corpora end: load `cut -d' ' -f1-3 /proc/loadavg`" >> $T/timing.log
grep -h '^TIME ' $T/corpus_*.log | sed 's/^TIME //' | sort >> $T/timing_corpus.log
for n in $CORP; do
	corpus $n $T/corpus_$n.log
done

finish
