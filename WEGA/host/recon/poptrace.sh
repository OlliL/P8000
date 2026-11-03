#!/bin/sh
# poptrace.sh PROG FUNC: what popt does with one function of src/cmd/PROG.
#
# Builds popt v0p with -DTRACE once (RECON_TMP/poptrace/popt; delete it after a change of popt), compiles the module of
# FUNC with ccv0p -S and cuts the function out as the input. Writes RECON_TMP/poptrace/FUNC.in and FUNC.out: the out
# file holds the nodes after each iteration (/*TRACE iterate n*/, /*TRACE comjump*/) and a line /*CM ...*/ for each
# block moved by codemove. Prints those lines. Run it with the sources of the function as they are now.

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
WEGA_ROOT=${WEGA_ROOT:-${WEGA:-/mnt/share/cvs/P8000/tmp/wega31_root}}
OLDROOT=${OLDROOT:-/mnt/share/cvs/P8000/tmp/oldroot}
RECON_TMP=${RECON_TMP:-/mnt/share/cvs/P8000/tmp/tmp}
EMU=$REPO/host/z8k-emu/z8k-emu
Z="$EMU -r $WEGA_ROOT"
prog=$1 func=$2
[ -n "$prog" ] && [ -n "$func" ] || { sed -n '2,7p' $0; exit 1; }
W=$RECON_TMP/poptrace
mkdir -p $W

if [ ! -f $W/popt ]; then
	rm -rf $W/b
	mkdir -p $W/b/WEGA/src/cmd
	cp -a $REPO/src/cmd/popt $W/b/WEGA/src/cmd/
	ln -s $REPO/src/lib $W/b/WEGA/src/lib
	ln -s $REPO/host $W/b/WEGA/host
	cd $W/b/WEGA/src/cmd/popt/v0p || exit 1
	rm -f z2*.o popt
	# OLD lies outside the root of the emulator: a relative copy inside the working directory
	cp -a $REPO/src/lib/oldlibc ../oldlibc
	make -o ../crt0.o popt OLD=../oldlibc CC="$Z /bin/cc" LD="$Z /bin/ld" SWITCHES=-DTRACE > $W/build.log 2>&1
	[ -f popt ] || { tail -3 $W/build.log; exit 1; }
	cp popt $W/popt
fi

cd $REPO/src/cmd/$prog || exit 1
m=`grep -l "^$func *(" *.c | head -1`
[ -n "$m" ] || { echo "$func: not found in src/cmd/$prog"; exit 1; }
m=${m%.c}
rm -rf $W/w; mkdir -p $W/w
cp *.c *.h $W/w/ 2>/dev/null
for f in *; do [ -f "$f" ] && case "$f" in *.*) ;; *) head -c 1 "$f" | grep -q '[[:print:]]' && cp "$f" $W/w/ ;; esac; done 2>/dev/null
cd $W/w
$EMU -r $OLDROOT /bin/ccv0p -S $m.c > /dev/null 2>&1 < /dev/null
[ -f $m.s ] || { echo "$m.c: no assembly"; exit 1; }
awk -v f="$func" '$0 ~ "^\t_" f ":" {p=1} p {print} index($0, "/* _" f " */") {p=0}' $m.s > ../$func.in
cp ../popt tpopt
$EMU -r $OLDROOT ./tpopt + ../$func.in ../$func.out > /dev/null 2>&1 < /dev/null
grep -n '/\*CM\|TRACE' ../$func.out | cut -c1-90
