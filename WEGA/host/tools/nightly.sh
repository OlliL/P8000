#!/bin/sh
# nightly.sh [-b DOLLARS] NAME[:ORIG] | lib:LIBNAME ...: rebuild programs and libraries one after the other, each in a fresh
# claude session (host/HOWTO_reconstruct.md, phase A, or "Libraries"), then checkall on its commit; at the end
# src/COVERAGE.md again.
#
#   ORIG is the path below the WEGA root (default bin/NAME), e.g. nl  cmp  csplit:usr/bin/csplit
#   lib:LIBNAME a library and its segmented one (libm: lib/libm.a, lib/slibm.a), e.g. lib:libm  lib:libPW
#   -b DOLLARS   the most one session may spend (claude --max-budget-usd; default 5)
#
# Per program: the session does steps 1 to 5 and commits (it does not run checkall: one command may run 10 minutes at
# most); then this script runs checkall on HEAD. It stops at the first program that leaves the working tree changed
# (no commit, or an unfinished one) or whose checkall fails, so that the next program does not build on it; a checkall
# that fails only in host builds of the tools ([host] corpora: the WEGA tree is right) is noted and the run goes on.
# Logs in tmp/tmp/nightly/: NAME.json (the result of the session: report, turns, cost), NAME.checkall, summary.txt.
# Environment: WEGA (the WEGA root, default tmp/wega31_root of the repository). Run it under nohup or tmux.
REPO=`cd \`dirname $0\`/../../.. && pwd`
WEGA=${WEGA:-$REPO/tmp/wega31_root}
L=$REPO/tmp/tmp/nightly
BUDGET=5
[ "$1" = -b ] && { BUDGET=$2; shift 2; }
mkdir -p $L
cd $REPO
S=$L/summary.txt
echo "`date '+%F %T'` start: $*" >> $S
for a in "$@"; do
	case "$a" in
	lib:*) name=${a#lib:}; orig= ;;
	*) name=${a%%:*}; orig=${a#*:}; [ "$orig" = "$a" ] && orig=bin/$name ;;
	esac
	if [ -n "`git status --porcelain`" ]; then
		echo "`date '+%F %T'` $name: the working tree is not clean, stop" >> $S
		exit 1
	fi
	before=`git rev-parse --short HEAD`
	if [ -z "$orig" ]; then
	msg="WEGA, increment 150: rebuild the library $name (and its segmented one) by WEGA/host/HOWTO_reconstruct.md, section
\"Libraries\" (host/recon/libsetup.py $name first; the pattern: src/lib/libl). Read WEGA/CLAUDE.md and the HOWTO first.
No subagents. A member still open after 6 attempts: note it in compare/$name/findings.json, put the archive in OPEN of
host/tools/checkall.sh, do not go deeper. A library without any source: write that down in the log of increment 150,
commit only that. Do not run checkall and do not regenerate COVERAGE.md: the script that started you runs checkall on
your commit. Keep the two tables current. Nothing may stay uncommitted.
At the end a short report: per archive identical or not, members open, sources used (System III or BSD), changed files."
	else
	msg="WEGA, increment 160: rebuild /$orig ($name) by WEGA/host/HOWTO_reconstruct.md, phase A, steps 1 to 5.
Read WEGA/CLAUDE.md and the HOWTO first. No subagents. A block still open after 6 attempts: note it in findings.json,
put the file in OPEN of host/tools/checkall.sh, do not go deeper. Do not run checkall and do not regenerate COVERAGE.md:
the script that started you runs checkall on your commit. Keep the two tables current. Commit when the program is
byte-identical (bincmp IDENTICAL) or its open blocks are noted; nothing may stay uncommitted.
At the end a short report: bincmp, corpus (cases, equal), changed files, open points."
	fi
	claude -p "$msg" --model opus --output-format json --max-budget-usd $BUDGET \
		--permission-mode acceptEdits --allowedTools "Bash Read Edit Write Grep Glob" --disallowedTools "Agent" \
		> $L/$name.json 2> $L/$name.err
	after=`git rev-parse --short HEAD`
	info=`python3 -c "
import json,sys
try:
    d=json.load(open('$L/$name.json'))
    print('turns %s cost %.2f\$ error %s' % (d.get('num_turns'), d.get('total_cost_usd') or 0, d.get('is_error')))
except Exception as e:
    print('no result (%s)' % e)"`
	if [ -n "`git status --porcelain`" ] || [ "$before" = "$after" ]; then
		echo "`date '+%F %T'` $name: $info; no clean commit ($before -> $after), stop" >> $S
		exit 1
	fi
	make -C WEGA/host checkall WEGA=$WEGA NAME=n_$name PATCH=$after > $L/$name.checkall 2>&1
	res=`grep -E 'ALL OK|FAILED' $L/$name.checkall | tail -1`
	echo "`date '+%F %T'` $name: $info; commit $after; checkall: $res" >> $S
	case "$res" in
	*ALL\ OK*) ;;
	*)	# only the host builds of the tools failed (a new source of the tree as a corpus input, e.g. a constant the host's
		# atof rounds otherwise): the WEGA tree is right; noted for the day, the next one goes on
		other=`grep ' FAIL ' $L/$name.checkall | grep -v '^corpus [a-z0-9_]* \[host\]'`
		if [ -z "$other" ]; then
			echo "`date '+%F %T'` $name: only host builds failed, going on: `grep ' FAIL ' $L/$name.checkall | awk '{print $2}' | tr '\n' ' '`" >> $S
		else
			echo "`date '+%F %T'` stop after $name (checkall failed: $L/$name.checkall)" >> $S
			exit 1
		fi ;;
	esac
done
# COVERAGE.md once for all of them
(cd WEGA/host && WEGA=$WEGA CHECK_NAME=cov CHECK_PATCH=`git rev-parse --short HEAD` sh tools/checkall.sh -b) > $L/coverage.log 2>&1 &&
	(cd WEGA && WEGA=$WEGA INSTALL=/tmp/wega_check.cov/install python3 host/tools/coverage.py) &&
	git add WEGA/src/COVERAGE.md && git commit -q -m "COVERAGE.md after the nightly run: $*" &&
	echo "`date '+%F %T'` COVERAGE.md committed" >> $S
echo "`date '+%F %T'` done" >> $S
