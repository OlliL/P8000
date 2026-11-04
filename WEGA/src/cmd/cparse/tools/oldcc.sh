#!/bin/sh
# oldcc.sh: the measuring loop of the old compiler work on /lib/cparse (host/OLDCC.md, "v0p").
#
#   oldcc.sh build PASS VER [SWITCHES]   rebuild pass PASS (cparse, codgen, popt) of version VER (v0, v0p) with
#                                        SWITCHES (default: those of its Makefile; codgen: -DOLD_V0 or -DOLD_V0P, the
#                                        switches themselves are in codgen/oldcc.h) and install it into OLDROOT/lib/VER
#   oldcc.sh model [PROG]                PROG (cparse, default, or popt) built with ccv0p (d12model) and compared
#                                        strictly with the original (strictcmp); prints the totals, keeps the
#                                        per-function list in RECON_TMP/oldcc_model_PROG.txt
#   oldcc.sh corpus                      cparse built with ccv0p from our sources, its corpus against /lib/cparse
#                                        (must stay 259 of 259: catches wrong code); restores the normal build
#   oldcc.sh bench                       the bench of v0 (System III programs of 1982, ccv0)
#   oldcc.sh lines [PROG]                the differing functions of the last model run and the sum of their lines
#   oldcc.sh findings [PROG]             findings.json (status from the last model) and FINDINGS.md of PROG
#   oldcc.sh matrix [-j N] [SWITCH ...]  each switch of v0p taken out alone, N at a time, all programs measured: host/OLDCC_SWITCHES.md
#   oldcc.sh matrix check                no measurement: switches and programs of the sources against that table
#
# Environment (defaults as in host/recon/progcfg.py): WEGA_ROOT (the WEGA 3.1 tree), OLDROOT (the tree with
# bin/ccv0, ccv0p and lib/v0, lib/v0p; made by `make -C host oldroot`), RECON_TMP (scratch), SYS3SRC (usr/src/cmd of
# a System III tree, for bench).
# A change of a pass is accepted when model gets better, corpus stays 259/259 and bench (for a switch in v0) does not
# get worse; a switch that is neutral on the bench goes to v0p (OLDCC.md, evidence rules).

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../../../.. && pwd`
WEGA_ROOT=${WEGA_ROOT:-${WEGA:-/mnt/share/cvs/P8000/tmp/wega31_root}}
OLDROOT=${OLDROOT:-/mnt/share/cvs/P8000/tmp/oldroot}
RECON_TMP=${RECON_TMP:-/mnt/share/cvs/P8000/tmp/tmp}
SYS3SRC=${SYS3SRC:-/mnt/users/olivleh1/BACKUPs/Operating_Systems/SYSIII/sys3/usr/src/cmd}
EMU=$REPO/host/z8k-emu/z8k-emu
W="$EMU -r $WEGA_ROOT"
export RECON_TMP WEGA_ROOT
mkdir -p $RECON_TMP

case "$1" in
build)
	pass=$2 ver=$3 sw=$4
	cd $REPO/src/cmd/$pass/$ver || exit 1
	rm -f *.o $pass
	if [ "$pass" = codgen ] && [ -z "$sw" ]; then
		[ "$ver" = v0p ] && sw=-DOLD_V0P || sw=-DOLD_V0
	fi
	if [ -n "$sw" ]; then
		make CC="$W /bin/cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" LD="$W /bin/ld" SWITCHES="$sw" \
			> $RECON_TMP/oldcc_build.log 2>&1
	else
		make CC="$W /bin/cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" LD="$W /bin/ld" \
			> $RECON_TMP/oldcc_build.log 2>&1
	fi || { tail -5 $RECON_TMP/oldcc_build.log; exit 1; }
	mkdir -p $OLDROOT/lib/$ver && cp $pass $OLDROOT/lib/$ver/$pass
	;;
model)
	prog=${2:-cparse}
	printf '#!/bin/sh\nexec %s -r %s /bin/ccv0p "$@"\n' $EMU $OLDROOT > $RECON_TMP/ccv0pw
	chmod +x $RECON_TMP/ccv0pw
	cd $REPO/src/cmd/$prog || exit 1
	make -s -C $REPO/host/cas casold
	CAS=${CAS:-$REPO/host/cas/casold} CCHOST=$RECON_TMP/ccv0pw python3 tools/d12model.py -q 2>&1 | tail -2
	python3 tools/strictcmp.py > $RECON_TMP/oldcc_model_$prog.txt 2>&1
	[ "$prog" = cparse ] && cp $RECON_TMP/oldcc_model_$prog.txt $RECON_TMP/oldcc_model.txt
	tail -1 $RECON_TMP/oldcc_model_$prog.txt
	;;
lines)
	prog=${2:-cparse}
	awk '$(NF-1)=="DIFF" {s += $NF; printf "%s %s; ", $1, $NF} END {print ""; print "lines", s}' $RECON_TMP/oldcc_model_$prog.txt
	;;
corpus)
	cd $REPO/src/cmd/cparse || exit 1
	O="cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o cparse"
	E="timeout 300 $EMU -r $OLDROOT"
	rm -f $O; make cparse CC="$E /bin/ccv0p" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" \
		YACC="$W /usr/bin/yacc" > $RECON_TMP/oldcc_corpus_build.log 2>&1
	cp cparse $RECON_TMP/cparse_v0p
	timeout 1500 python3 tools/stagecorpus.py $RECON_TMP/cparse_v0p 2>&1 | tail -1
	rm -f $O; make cparse CC="$W /bin/cc" CAS="$W /bin/cas" AR="$W /bin/ar" LD="$W /bin/ld" \
		YACC="$W /usr/bin/yacc" > /dev/null 2>&1
	;;
findings)
	exec python3 $REPO/host/recon/findings.py ${2:-cparse}
	;;
matrix)
	shift
	if [ "$1" = check ]; then
		exec python3 $REPO/host/recon/switchmatrix.py check
	fi
	exec python3 $REPO/host/recon/switchmatrix.py run "$@"
	;;
bench)
	cd $REPO && SYS3SRC=$SYS3SRC WEGA=$WEGA_ROOT OLDROOT=$OLDROOT timeout 1500 \
		python3 host/recon/witness.py -t v0 -c "-r $OLDROOT /bin/ccv0" 2>&1 | tail -1
	;;
*)
	sed -n '2,22p' $0
	exit 1
	;;
esac
