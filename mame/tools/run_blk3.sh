#!/bin/bash
# usage: run_blk.sh <mame binary> <out file>
cd /home/claude/runs/sc
export MAMEBIN=$1 LUA=/home/claude/runs/sc/blk3.lua
rm -f go.txt
bash start.sh
(printf '\\r\n' > mon.in &)
until grep -a -q "Press RETURN" mon.log 2>/dev/null; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 4; printf 'X\\r\n' > mon.in
until grep -a -q "Press NMI" mon.log; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 3; echo nmi > /home/claude/runs/ctl.txt
until grep -a -q "^NMI" mon.log; do sleep 2; done; sleep 3
touch go.txt
sleep 6
grep -a RESULT mame.out > $2
pkill -9 -f "mame p8000" 
