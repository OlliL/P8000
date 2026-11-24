#!/bin/sh
# makecand.sh DIR: build the make sources in DIR with ccx, print the functions whose frame differs from bin/make
H=$(cd $(dirname $0) && pwd); WEGA=$(cd $H/../../.. && pwd); W=${RS_WORK:-$(cd $WEGA/.. && pwd)/tmp/tmp/regslots}; ROOT=${WEGA_ROOT:-$(cd $WEGA/.. && pwd)/tmp/wega31_root}
cd $1; tot=0
for f in main.c doname.c misc.c files.c dosys.c gram.c dyndep.c; do
  rm -f ${f%.c}.o; $W/ccx -O -c $f >/dev/null 2>&1
  [ -f ${f%.c}.o ] || { echo "$f: no object"; tot=$((tot+99)); continue; }
  r=$(python3 $H/fscan.py ${f%.c}.o $ROOT/bin/make | grep -v '^#' | awk '{print $1}' | tr '\n' ' ')
  [ -n "$r" ] && echo "$f: $r"; tot=$((tot+$(echo $r | wc -w)))
done
echo "WRONG $tot"
