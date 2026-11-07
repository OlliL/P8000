#!/bin/sh
# checkall.sh [-b]: one check for everything that is byte-identical.
#
# Works on copies of the files known to git (also changed and new ones, not the built ones) below CHECK_TMP, so that it
# proves the repository alone is enough and leaves the working tree alone:
#   1. make in src: the old compilers with the shipped tools, the libraries, the old compilers again, cmd/;
#   2. make install PREFIX=CHECK_TMP/install: every file there against the file of the same name in WEGA with cmp (the
#      Makefiles strip as the originals are stripped); every member of lib/libc/libc.a against the original;
#   3. the corpora (host/tools/corpus.sh, inputs made fresh, side by side) of every program with a tools/progcfg.py, built
#      with the shipped cc (scc) and the libc of its Makefile: the byte-identical builds are proven by 2;
#   4. make -C host check: the host tools.
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

# not byte-identical yet: reported, not counted
OPEN='bin/cas'

# the files known to git, in DIR/WEGA; the emulator is not rebuilt here
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

finish() {
	[ $FAIL = 0 ] && echo ALL OK || echo FAILED
	exit $FAIL
}

mkdir -p $T

# 1. everything from the sources
copytree $T/tree
rm -rf $OLD $T/bin
sh $HERE/oldroot.sh $WEGA $OLD
sh $HERE/wbin.sh $T/bin $WEGA $OLD $EMU
if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make all OLDROOT=$OLD) > $T/make.log 2>&1; then
	result 'make in src' ok 'stage1 lib stage2 cmds'
else
	result 'make in src' fail "see $T/make.log"
	finish
fi

# 2. the bytes: make install into an empty root, every file there against the file of the same name in WEGA
I=$T/install
rm -rf $I
mkdir -p $I/bin $I/etc $I/lib $I/usr/bin
if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make install PREFIX=$I) > $T/install.log 2>&1; then :; else
	result 'make install' fail "see $T/install.log"
	finish
fi
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
# the members of libc.a, each from its source
L=$T/tree/WEGA/src/lib/libc
rm -rf $T/orig
mkdir -p $T/orig
(cd $T/orig && $EMU -r $WEGA /bin/ar x /lib/libc.a)
same=0
for f in `cd $T/orig && ls`; do
	if cmp -s $T/orig/$f $L/obj/$f; then same=`expr $same + 1`; else result "libc $f" fail "differs or missing: $L/obj/$f"; fi
done
result 'libc.a members' ok "$same identical, from source"

[ $BUILDONLY = 1 ] && finish

# 3. the corpora; NAME LOG
corpus() {
	line=`sed -n 's/^.*: \([0-9]*\) of \([0-9]*\) inputs identical.*/\1 \2/p' $2 | tail -1`
	set -- "$1" "$2" $line
	if [ $# = 4 ] && [ "$3" = "$4" ] && [ "$4" -gt 0 ]; then result "corpus $1" ok "$3 of $4 inputs identical"
	else result "corpus $1" fail "see $2"; fi
}
# the shipped build: every program with a corpus built from the same sources by the shipped cc (scc) and the libc of
# its Makefile; a byte-identical binary cannot show anything in a corpus, this build tests the sources
copytree $T/shipped
C=$T/shipped/WEGA/src/cmd
cp $T/tree/WEGA/src/lib/*.a $T/shipped/WEGA/src/lib/
(
	PATH=$T/bin:$PATH; export PATH
	cd $C/cpp && make cpp CC=cc
	cd $C/cparse && make all CC=cc
	cd $C/codgen && make all CC=cc
	cd $C/popt && make all CC=cc
	cd $C/ld && make ld CC=cc
	cd $C/sld && make sld SCC=scc
	cd $C/cas && make all SCC=scc
) > $T/shipped.log 2>&1
# the corpora run side by side, each in a work directory of its own
CS=$T/shipped/WEGA/host/tools/corpus.sh
CORP='cpp:cpp/cpp cparse:cparse/build/nonseg/cparse scparse:cparse/build/seg/scparse codgen:codgen/build/nonseg/codgen
	scodgen:codgen/build/seg/scodgen popt:popt/build/nonseg/popt spopt:popt/build/seg/spopt ld:ld/ld sld:sld/sld
	cas:cas/build/cas'
for p in $CORP; do
	n=${p%%:*}
	CORPUS_BIN=$C/${p#*:} STAGE_TMP=$T/stage_$n JOBS=2 WEGA_ROOT=$WEGA sh $CS $n > $T/corpus_$n.log 2>&1 &
done
wait
for p in $CORP; do
	corpus ${p%%:*} $T/corpus_${p%%:*}.log
done

# 4. the host tools
# the corpus of the host popt, made fresh by the original cc
make -C $T/tree/WEGA/host/popt corpus WEGA=$WEGA CORPUS=$T/popt_corpus > $T/hostcheck.log 2>&1
if CORPUS=$T/popt_corpus make -C $T/tree/WEGA/host check WEGA=$WEGA >> $T/hostcheck.log 2>&1; then
	result 'host tools' ok 'cc ar binutils cas ld popt cpp'
else
	result 'host tools' fail "see $T/hostcheck.log"
fi

finish
