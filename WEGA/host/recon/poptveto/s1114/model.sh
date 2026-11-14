#!/bin/sh
# model.sh NAME [SRC]: full cas model with root $S/o_NAME; prints the DIFF lines of strictcmp
S=/tmp/SCRATCH; W=/home/olivleh1/git/P8000/WEGA
n=$1
[ -d $S/o_$n/tmp ] && [ ! -L $S/o_$n/tmp ] || { rm -f $S/o_$n/tmp; mkdir -p $S/o_$n/tmp; }
printf '#!/bin/sh\nexec timeout -s KILL 60 /home/olivleh1/git/P8000/WEGA/host/z8k-emu/z8k-emu -r %s/o_%s /bin/sccvc "$@"\n' $S $n > $S/cc_$n; chmod +x $S/cc_$n
export RECON_PROG=cas RECON_TMP=$S/t_$n CCHOST=$S/cc_$n CAS=$W/host/cas/casold
[ -z "$2" ] || export D12_SRC=$2
cd $W; python3 host/recon/d12model.py -q >/dev/null 2>&1
python3 host/recon/strictcmp.py | grep -c . ; python3 host/recon/strictcmp.py | grep "DIFF" | awk '{printf "%s:%s ", $1, $NF}'; echo
