#!/bin/sh
# bt.sh NAME: popt of o1_NAME on the raw files in bt/, files whose output differs from the committed v1 popt
cd /tmp/SCRATCH/bt; hang=0; d=""; n=0
for f in cmd_*.s lib_*.s; do
  timeout -s KILL 10 /home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu -r /tmp/SCRATCH/o1_$1 /lib/v1/popt $f n_$f 2>/dev/null || hang=$((hang+1)); cmp -s o_$f n_$f || { d="$d $f"; n=$((n+1)); }; done
echo "hangs $hang changed $n:$d" | cut -c1-600
