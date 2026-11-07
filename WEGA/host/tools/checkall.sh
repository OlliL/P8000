#!/bin/sh
# checkall.sh [-b]: one check for everything that is byte-identical.
#
# Works on copies of the files known to git (also changed and new ones, not the built ones) below CHECK_TMP, so that it
# proves the repository alone is enough and leaves the working tree alone:
#   1. make in src: the old compilers with the shipped tools, the libraries, the old compilers again, cmd/;
#   2. every member of lib/libc/libc.a, and every program of TABLE, stripped, against the original with cmp;
#   3. the corpora (host/tools/corpus.sh, inputs made fresh, side by side) of every program with a tools/progcfg.py, built
#      with the shipped cc (scc) and the libc of its Makefile: the byte-identical builds are proven by 2;
#   4. make -C host check: the host tools.
# -b: only 1 and 2. Prints one line per check, the logs stay in CHECK_TMP; exit status 0 when all pass.
# A program joins by a line in TABLE.
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

# built file below src/cmd, compiler, original(s): the first is compared, the others must have the same bytes
TABLE='
cpp/cpp	ccv0p	lib/cpp
cparse/build/nonseg/cparse	ccv0p	lib/cparse
cparse/build/seg/scparse	ccv0p	lib/scparse
codgen/build/nonseg/codgen	ccv0p	lib/codgen
codgen/build/seg/scodgen	ccv0p	lib/scodgen
popt/build/nonseg/popt	ccv0p	lib/popt
popt/build/seg/spopt	ccv0p	lib/spopt
cc/build/nonseg/cc	cc	bin/cc
cc/build/seg/scc	cc	bin/scc
size/size	ccv1	bin/size
strip/strip	ccv1	usr/bin/strip
mv/mv	ccv1	bin/mv bin/cp bin/ln
rm/rm	ccv1	bin/rm
write/write	ccv1r	bin/write
rmdir/rmdir	ccv1r	bin/rmdir
dc/dc	ccv0	bin/dc
ranlib/ranlib	ccv0	usr/bin/ranlib
ar	cc	bin/ar
ld/ld	ccv1	bin/ld
sld/sld	scc	bin/sld
getud	cc	bin/getud
putud	cc	bin/putud
copydisk/1.5/copydisk	cc	bin/copydisk
dump	cc	etc/dump
dumpdir	cc	etc/dumpdir
format/1.5/format	cc	bin/format
restor	cc	etc/restor
sync	cc	bin/sync
tar	cc	bin/tar
verify/1.5/verify	cc	bin/verify
doscopy/1.0/dosformat	cc	usr/bin/dosformat
doscopy/1.0/dosrm	cc	usr/bin/dosrm
doscopy/1.0/dosls	cc	usr/bin/dosls
doscopy/1.0/dosrmdir	cc	usr/bin/dosrmdir
doscopy/1.0/dosdir	cc	usr/bin/dosdir
doscopy/1.0/dosmkdir	cc	usr/bin/dosmkdir
doscopy/1.0/doscat	cc	usr/bin/doscat
doscopy/1.0/doscp	cc	usr/bin/doscp
pburn/2.1/pburn	cc	bin/pburn
BOOT0/pb.image	cc	pb.image
standalone/sa.cat	cc	sa.cat
standalone/sa.format	cc	sa.format
standalone/sa.verify	cc	sa.verify
standalone/sa.mkfs	cc	sa.mkfs
standalone/sa.install	cc	sa.install
standalone/sa.timer	cc	sa.timer
standalone/sa.shipdisk	cc	sa.shipdisk
chkdiff	script	bin/chkdiff
uuclear	script	bin/uuclear
whodo	script	bin/whodo
'
# not byte-identical yet: reported, not counted
OPEN='cas/build/cas:bin/cas'

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
if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make all PREFIX=$OLD) > $T/make.log 2>&1; then
	result 'make in src' ok 'stage1 lib stage2 cmds'
else
	result 'make in src' fail "see $T/make.log"
	finish
fi
S=$T/tree/WEGA/src/cmd

# 2. the bytes
L=$T/tree/WEGA/src/lib/libc
rm -rf $T/orig
mkdir -p $T/orig
(cd $T/orig && $EMU -r $WEGA /bin/ar x /lib/libc.a)
same=0
for f in `cd $T/orig && ls`; do
	if cmp -s $T/orig/$f $L/obj/$f; then same=`expr $same + 1`; else result "libc $f" fail "differs or missing: $L/obj/$f"; fi
done
result 'libc.a members' ok "$same identical, from source"
if cmp -s $L/libc.a $WEGA/lib/libc.a; then result 'libc.a' ok 'identical, the whole file'; else result 'libc.a' fail "cmp $L/libc.a $WEGA/lib/libc.a"; fi
echo "$TABLE" | while IFS='	' read file cc origs; do
	[ -n "$file" ] || continue
	set -- $origs
	first=$1
	n=`basename $file`
	if [ ! -f $S/$file ]; then
		echo "$file	FAIL	not built"
		continue
	fi
	rm -f $T/$n.stripped
	cp $S/$file $T/$n.stripped
	case $cc in ccv*|scc) (cd $T && $EMU -r $WEGA /usr/bin/strip $n.stripped) > /dev/null 2>&1 ;; esac
	if cmp -s $T/$n.stripped $WEGA/$first; then
		msg="identical, $cc"
		shift
		for o in "$@"; do
			cmp -s $T/$n.stripped $WEGA/$o && msg="$msg, $o" || msg="$msg, DIFFERS FROM $o"
		done
		echo "$file	ok	$msg"
	else
		echo "$file	FAIL	$cc: python3 host/tools/bincmp.py $WEGA/$first $T/$n.stripped"
	fi
done > $T/programs.txt
while IFS='	' read name state msg; do
	if [ "$state" = ok ]; then result "$name" ok "$msg"; else result "$name" fail "$msg"; fi
done < $T/programs.txt
o=
for p in $OPEN; do
	f=${p%%:*}
	cp $S/$f $T/open.stripped 2>/dev/null && (cd $T && $EMU -r $WEGA /usr/bin/strip open.stripped) > /dev/null 2>&1
	cmp -s $T/open.stripped $WEGA/${p#*:} && result "$f" fail 'identical now: enter it in TABLE'
	o="$o $f"
done
echo "open, not counted:$o"

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
) > $T/shipped.log 2>&1
# the corpora run side by side, each in a work directory of its own
CS=$T/shipped/WEGA/host/tools/corpus.sh
CORP='cpp:cpp/cpp cparse:cparse/build/nonseg/cparse scparse:cparse/build/seg/scparse codgen:codgen/build/nonseg/codgen
	scodgen:codgen/build/seg/scodgen popt:popt/build/nonseg/popt spopt:popt/build/seg/spopt ld:ld/ld sld:sld/sld'
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
	result 'host tools' ok 'cc ar binutils cas ld popt'
else
	result 'host tools' fail "see $T/hostcheck.log"
fi

finish
