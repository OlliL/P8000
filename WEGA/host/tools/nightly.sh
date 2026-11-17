#!/bin/sh
# nightly.sh [-b DOLLARS] NAME[:ORIG] ...: rebuild programs one after the other, each in a fresh claude session
# (host/HOWTO_reconstruct.md, phase A), then checkall on its commit; at the end src/COVERAGE.md again.
#
#   ORIG is the path below the WEGA root (default bin/NAME), e.g. nl  cmp  csplit:usr/bin/csplit
#   -b DOLLARS   the most one session may spend (claude --max-budget-usd; default 5)
#
# Per program: the session does steps 1 to 5 and commits (it does not run checkall: one command may run 10 minutes at
# most); then this script runs checkall on HEAD. It stops at the first program that leaves the working tree changed
# (no commit, or an unfinished one) or whose checkall fails, so that the next program does not build on it.
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
	name=${a%%:*}
	orig=${a#*:}
	[ "$orig" = "$a" ] && orig=bin/$name
	if [ -n "`git status --porcelain`" ]; then
		echo "`date '+%F %T'` $name: the working tree is not clean, stop" >> $S
		exit 1
	fi
	before=`git rev-parse --short HEAD`
	msg="WEGA, increment 160: rebuild /$orig ($name) by WEGA/host/HOWTO_reconstruct.md, phase A, steps 1 to 5.
Read WEGA/CLAUDE.md and the HOWTO first. No subagents. A block still open after 6 attempts: note it in findings.json,
put the file in OPEN of host/tools/checkall.sh, do not go deeper. Do not run checkall and do not regenerate COVERAGE.md:
the script that started you runs checkall on your commit. Keep the two tables current. Commit when the program is
byte-identical (bincmp IDENTICAL) or its open blocks are noted; nothing may stay uncommitted.
At the end a short report: bincmp, corpus (cases, equal), changed files, open points."
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
	*) echo "`date '+%F %T'` stop after $name (checkall failed: $L/$name.checkall)" >> $S; exit 1 ;;
	esac
done
# COVERAGE.md once for all of them
(cd WEGA/host && WEGA=$WEGA CHECK_NAME=cov CHECK_PATCH=`git rev-parse --short HEAD` sh tools/checkall.sh -b) > $L/coverage.log 2>&1 &&
	(cd WEGA && WEGA=$WEGA INSTALL=/tmp/wega_check.cov/install python3 host/tools/coverage.py) &&
	git add WEGA/src/COVERAGE.md && git commit -q -m "COVERAGE.md after the nightly run: $*" &&
	echo "`date '+%F %T'` COVERAGE.md committed" >> $S
echo "`date '+%F %T'` done" >> $S
