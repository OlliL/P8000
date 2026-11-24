#!/bin/sh
# try.sh BODY [NAME]: getfile.c of the tree with the text from "\t\tack = ACK;\n\tbad:" up to "\tfail:" replaced by the file
# BODY (BODY is the whole getfile.c when it holds "main("); built as the Makefile builds getfile (ccvm -O, crt0-getfile.o, the shipped libc), stripped, against the original.
# Prints "NAME: differing bytes N, first code 0x..." (bincmp). Needs WEGA= (the WEGA root) and the PATH tools of
# "make -C host wbin" (tmp/wbin). The build is in $GF_WORK/NAME (default: tmp/tmp/getfile-forms beside WEGA).
S=$(cd "$(dirname "$0")" && pwd); R=$S/../../..
: "${WEGA:?WEGA= the WEGA root}"
PATH=$R/../tmp/wbin:$PATH; export PATH
W=${GF_WORK:-$R/../tmp/tmp/getfile-forms}; n=${2:-$(basename "$1" | sed 's/\.[^.]*$//')}
mkdir -p "$W/$n" || exit 1
python3 - "$1" "$R/src/cmd/getfile.c" "$W/$n/getfile.c" <<'PY' || exit 1
import sys
src=open(sys.argv[2]).read()
body=open(sys.argv[1]).read()
if 'main(' in body:	# a whole getfile.c (gen3.py)
	open(sys.argv[3],'w').write(body); sys.exit(0)
a=src.index('\t\tack = ACK;\n\tbad:\n'); b=src.index('\tfail:\n')
open(sys.argv[3],'w').write(src[:a]+body+src[b:])
PY
[ -e "$W/crt0-getfile.o" ] || { (cd "$R/src/cmd" && make crt0-getfile.o >/dev/null) && cp "$R/src/cmd/crt0-getfile.o" "$W/"; }
cd "$W/$n" || exit 1
[ -e head ] || { mkdir head; ln -s "$R/src/head/oldv7" head/oldv7; }
rm -f getfile getfile.o getfile.s
timeout 300 ccvm -O -Ihead/oldv7 -c getfile.c > cc.log 2>&1 || { echo "$n: CC ERR"; exit 1; }
timeout 120 ld -e start -o getfile ../crt0-getfile.o getfile.o /lib/libc.a > /dev/null 2>&1 || { echo "$n: LD ERR"; exit 1; }
timeout 60 strip getfile
python3 "$R/host/tools/bincmp.py" "$WEGA/bin/getfile" getfile 2>&1 | sed -n 2p | sed "s/^/$n: /"
