#!/bin/bash
# usage: run_blk06.sh <mame binary> <log copy>
cd /home/claude/runs/sc
export MAMEBIN=$1
bash start.sh
(printf '\\r\n' > mon.in &)
until grep -a -q "Press RETURN" mon.log 2>/dev/null; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 4; printf 'X\\r\n' > mon.in
until grep -a -q "Press NMI" mon.log; do sleep 2; done
sleep 1; printf '\\r\n' > mon.in; sleep 3; echo nmi > /home/claude/runs/ctl.txt
until grep -a -q "^NMI" mon.log; do sleep 2; done; sleep 3
cat enter_blk06.txt > mon.in; sleep 80
cp mon.log $2
pkill -9 -f "mame p8000"
