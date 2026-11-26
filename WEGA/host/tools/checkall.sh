#!/bin/sh
# checkall.sh [-b]: one check for everything that is byte-identical.
#
# Works on copies of the files known to git (also changed and new ones, not the built ones) below CHECK_TMP, so that it
# proves the repository alone is enough and leaves the working tree alone:
#   1. make in src: the old compilers with the shipped tools, the libraries, the old compilers again, cmd/, uts/;
#   2. make install PREFIX=CHECK_TMP/install: every file there against the file of the same name in WEGA with cmp (the
#      Makefiles strip as the originals are stripped); then cas compiled by vc (sccvc of OLDROOT) against /bin/cas;
#   3. the corpora of compare/CORPORA (host/tools/corpus.sh, side by side; host/recon/corpusrun.py: the prepared inputs and the
#      results are kept in the store WEGA/.corpus of the working tree, by keys of their contents), every candidate: the WEGA program
#      built by make shipped (the shipped cc, scc and libc; the byte-identical builds are proven by 2) and the host build;
# -b: only 1 and 2. Prints one line per check, the logs stay in CHECK_TMP; exit status 0 when all pass.
# A program joins by its install target (src/cmd/Makefile).
#
# Variants (increment 105): CHECK_PATCH=FILE checks the files git knows plus that patch (git diff of the repository, applied
# with patch -p1), CHECK_PATCH=REF the tree of a git commit (e.g. from "git stash create": the working tree with its changes,
# without new files). The tree is copied once at the start (snapshot) and every part uses that copy; the working tree is not
# read again, so it may change while the run goes on. CHECK_NAME=NAME runs in /tmp/wega_check.NAME; runs with different names
# can run at the same time (each has its own copies, OLDROOT, guest /tmp (Z8K_TMPDIR) and logs; the corpus cache is shared,
# its keys are by content). A second run in a directory that a living run uses stops with exit status 2.
#
# Environment: WEGA (the WEGA 3.1 tree), CHECK_TMP (default /tmp/wega_check, or /tmp/wega_check.NAME, a local disk),
# CHECK_NAME, CHECK_PATCH, MAKEJOBS (default -j<cores>), CHECK_CORPORA (how many corpora run at a time, default half the cores), CORPUS_NOCACHE=1 (the corpora ignore the store: make checkall-nocache),
# CHECK_KEEP=1 (keep CHECK_TMP after a run that passed; it is removed then, and kept after a failure or with -b).

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
WEGA=${WEGA:-$REPO/../tmp/wega31_root}
WEGA=`cd $WEGA && pwd`
# the umask of the owner's shell: z8k-emu tests/openflags.out expects a created file with mode 664
umask 002
if [ -n "$CHECK_TMP" ]; then T=$CHECK_TMP
elif [ -n "$CHECK_NAME" ]; then T=/tmp/wega_check.$CHECK_NAME
else T=/tmp/wega_check; fi
PATCH=$CHECK_PATCH
case "$PATCH" in ''|/*) ;; *) [ -f "$PATCH" ] && PATCH=`pwd`/$PATCH ;; esac
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

# not byte-identical yet: reported, not counted (compare/NAME/findings.json)
OPEN='bin/sh bin/rsh bin/adb'

# the snapshot, made once: the files known to git (or the tree of the git commit CHECK_PATCH), the patch CHECK_PATCH applied
snapshot() {
	rm -rf $T/snap
	mkdir -p $T/snap/WEGA
	if [ -n "$PATCH" ] && [ ! -f "$PATCH" ]; then
		(cd $REPO && git archive "$PATCH" -- src host compare) | (cd $T/snap/WEGA && tar xf -) || return 1
	else
		(cd $REPO && git ls-files -co --exclude-standard -- src host compare | tar cf - -T - 2>/dev/null) | (cd $T/snap/WEGA && tar xf -)
		if [ -n "$PATCH" ]; then
			patch -p1 -s -d $T/snap --batch < "$PATCH" > $T/patch.log 2>&1 || return 1
		fi
	fi
	# the list of the files of the snapshot: the corpora take their sources from it (host/recon/corpusc.py, tree_files), not
	# from what the builds add (cpy.c of cpp.y, ...)
	(cd $T/snap/WEGA && find src host compare -type f | sort) > $T/snap/WEGA/.files
}

# a copy of the snapshot in DIR/WEGA; the emulator is not rebuilt here
copytree() {
	rm -rf $1
	mkdir -p $1
	cp -a $T/snap/WEGA $1/WEGA
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
	if [ -s $T/tmplog.log ]; then
		# temporary files of parallel guest processes (host/tools/tmpclash.py): the first clashes and the totals
		python3 $REPO/host/tools/tmpclash.py $T/tmplog.log > $T/tmpclash.txt 2>&1
		echo
		grep -m 5 '^clash\|^same' $T/tmpclash.txt
		tail -1 $T/tmpclash.txt
	fi
	rm -f $LOCK
	# a run that passed has used every entry of the store that is still made: the rest goes (host/tools/corpus_prune.py;
	# not without the store, CORPUS_NOCACHE: it uses none; not while another run's corpusrun.py runs)
	if [ $FAIL = 0 ] && [ $BUILDONLY = 0 ] && [ -z "$CORPUS_NOCACHE" ] && [ -f $T/store.stamp ]; then
		python3 $REPO/host/tools/corpus_prune.py $T/store.stamp --store $REPO/.corpus
	fi
	# a run that passed leaves nothing behind (not with -b: coverage.py reads its install tree; not with CHECK_KEEP=1);
	# a run that failed keeps CHECK_TMP for the logs
	if [ $FAIL = 0 ] && [ $BUILDONLY = 0 ] && [ -z "$CHECK_KEEP" ]; then
		rm -rf $T
	fi
	[ $FAIL = 0 ] && echo ALL OK || echo "FAILED (the logs are in $T)"
	exit $FAIL
}

mkdir -p $T
# one run per directory: the lock holds the pid of the run
LOCK=$T.lock
if [ -f $LOCK ] && kill -0 `cat $LOCK` 2>/dev/null; then
	echo "checkall: $T is used by the run with pid `cat $LOCK`; give another CHECK_NAME" >&2
	exit 2
fi
echo $$ > $LOCK
trap 'rm -f $LOCK' INT TERM
echo "checkall ${CHECK_NAME:-(no name)} in $T${PATCH:+, variant $PATCH}"
# the guest /tmp of every emulator of this run (not the /tmp of the WEGA tree, which parallel runs would share)
rm -rf $T/guesttmp
mkdir -p $T/guesttmp
Z8K_TMPDIR=$T/guesttmp
export Z8K_TMPDIR
# every z8k-emu of this run logs its pids and its files below the guest /tmp (wega.c tmplog), for tmpclash.py in finish
rm -f $T/tmplog.log $T/tmpclash.txt
Z8K_TMPLOG=$T/tmplog.log
export Z8K_TMPLOG
rm -f $T/timing.log $T/timing_corpus.log
echo "machine: `nproc` cores, load `cut -d' ' -f1-3 /proc/loadavg`, MAKEARGS=$J" >> $T/timing.log

# 1. everything from the sources
if snapshot; then :; else
	result 'snapshot' fail "the variant $PATCH does not apply, see $T/patch.log"
	finish
fi
# the emulator, copied once too: a rebuild in the working tree during the run does not reach it
cp $EMU $T/z8k-emu && EMU=$T/z8k-emu
copytree $T/tree
stamp '1 copy of the files of git (tree)'
rm -rf $OLD $T/bin
sh $HERE/oldroot.sh $WEGA $OLD
sh $HERE/wbin.sh $T/bin $WEGA $OLD $EMU
RUN=wrun	# $(RUN) ./generator in the Makefiles (wbin.sh: wrun)
export RUN
stamp '1 oldroot.sh, wbin.sh'
touch $T/tree.stamp	# tree.stamp: nothing of the build and of the corpora may write into the original tree (see the end of the corpora)
# make all = stage1 lib stage2 cmds uts, one after the other so that every part is timed
: > $T/make.log
for g in stage1 lib stage2 cmds uts; do
	if (cd $T/tree/WEGA/src && PATH=$T/bin:$PATH make $g OLDROOT=$OLD MAKEARGS="$J") >> $T/make.log 2>&1; then
		stamp "1 make $g in src"
	else
		stamp "1 make $g in src (failed)"
		result 'make in src' fail "make $g, see $T/make.log"
		finish
	fi
done
result 'make in src' ok 'stage1 lib stage2 cmds uts'

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
# the directories: the same names with the same modes (the guest's /tmp below tmp is not part of it)
(cd $WEGA && find . -mindepth 1 -type d ! -path './tmp/*' -printf '%m %P\n' | sort -k2) > $T/dirs.wega
(cd $I && find . -mindepth 1 -type d -printf '%m %P\n' | sort -k2) > $T/dirs.install
if cmp -s $T/dirs.wega $T/dirs.install; then
	result 'install directories' ok "`wc -l < $T/dirs.wega` directories, names and modes as in WEGA"
else
	result 'install directories' fail "`diff $T/dirs.wega $T/dirs.install | head -6 | tr '\n' ' '`"
fi
echo "open, not counted:$o"
stamp '2 cmp of the install root with WEGA'

# vc, the compiler of /bin/cas (host/OLDCC.md): every source of cas compiled by sccvc of OLDROOT and assembled by its old
# cas, the functions against /bin/cas (the cas model of the vc matrix). Not a corpus; the /bin/cas of make above is built
# by the shipped scc, so without this a vc that no longer compiles cas passes (2026-11-22, OLD_D13)
printf '#!/bin/sh\nexec %s -r %s /lib/old/cas "$@"\n' $EMU $OLD > $T/cas_old
chmod +x $T/cas_old
mkdir -p $T/vcmodel
(cd $T/tree/WEGA && RECON_TMP=$T/vcmodel OLDROOT=$OLD WEGA_ROOT=$WEGA RECON_PROG=cas CCHOST=$T/bin/sccvc CAS=$T/cas_old \
	timeout 1800 python3 host/recon/d12model.py -q && RECON_TMP=$T/vcmodel OLDROOT=$OLD WEGA_ROOT=$WEGA RECON_PROG=cas \
	timeout 600 python3 host/recon/strictcmp.py) > $T/vcmodel.log 2>&1
VM=`tail -1 $T/vcmodel.log`
case "$VM" in
'same '*', differ 0 (strict)') result 'cas by vc (model)' ok "$VM" ;;
*) result 'cas by vc (model)' fail "$VM; see $T/vcmodel.log" ;;
esac
stamp '2 cas by vc (the model)'

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
# the shipped build (make shipped in src/cmd): every command built from the same sources by the shipped cc (scc) and the shipped
# libc, with -DSHIPPED; a byte-identical binary cannot show anything in a corpus, this build tests the sources.
# The host programs (compare/NAME/corpus.py, CANDIDATES) are built beside them by the compiler of the host.
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
build 'cmd (make shipped)' $C shipped
: > $T/hostbuild.log
for p in cpp ar binutils cc popt codgen gopt cparse cas ld; do
	make -C $T/shipped/WEGA/host $p >> $T/hostbuild.log 2>&1
	stamp "3 host build: $p"
done
make -C $T/shipped/WEGA/host/cas casold >> $T/hostbuild.log 2>&1
stamp '3 host build: casold'
# the own tests of the emulator (host/z8k-emu/tests: the signal model)
if TMPDIR=$T sh $T/shipped/WEGA/host/z8k-emu/tests/run.sh $EMU $WEGA > $T/emutest.log 2>&1; then
	result "z8k-emu tests" ok "`grep -c '^ok' $T/emutest.log` hold"
else
	result "z8k-emu tests" fail "see $T/emutest.log"
fi
stamp '3 emulator tests'
# the corpora run side by side, each in a work directory of its own; every candidate of a program is tested against the
# same inputs (the original runs once). At most CHECK_CORPORA corpora run at a time (default: half the cores, each corpus has
# 2 jobs, a corpus with JOBS in its corpus.py that many), in the order of compare/CORPORA (the slowest first): the next one starts when one is done.
CS=$T/shipped/WEGA/host/tools/corpus.sh
CORP=`grep -v '^#' $T/shipped/WEGA/compare/CORPORA`
touch $T/store.stamp	# the store: what the corpora of this run do not use is older than this (corpus_prune.py in finish)
NC=${CHECK_CORPORA:-`expr \( \`nproc\` + 1 \) / 2`}
echo "corpora start: load `cut -d' ' -f1-3 /proc/loadavg`, $NC at a time" >> $T/timing.log
export CS T REPO WEGA OLD CORPUS_OPTS
echo $CORP | tr ' ' '\n' | xargs -P $NC -I{} sh -c 'CORPUS_DB=$REPO/.corpus CORPUS_TMP=$T/corpus_{} CORPUS_OPTS="-k $CORPUS_OPTS" JOBS=2 WEGA_ROOT=$WEGA OLDROOT=$OLD sh $CS {} > $T/corpus_{}.log 2>&1'
stamp '3 all corpora (CHECK_CORPORA at a time)'
echo "corpora end: load `cut -d' ' -f1-3 /proc/loadavg`" >> $T/timing.log
grep -h '^TIME ' $T/corpus_*.log | sed 's/^TIME //' | sort >> $T/timing_corpus.log
for n in $CORP; do
	corpus $n $T/corpus_$n.log
done
# the original tree (WEGA) must not be written by the build or by a corpus (a program that makes a name in a directory that the case's layer over
# the tree does not hold writes it into the tree: etc/utmp by getty, ...); its tmp is the guest's /tmp of older runs
W=`find $WEGA -newer $T/tree.stamp ! -path "$WEGA/tmp" ! -path "$WEGA/tmp/*" 2>/dev/null | head -5 | tr '\n' ' '`
if [ -n "$W" ]; then
	result 'tree not written' fail "files of $WEGA written by the corpora: $W"
else
	result 'tree not written' ok 'no file of the tree changed'
fi

finish
