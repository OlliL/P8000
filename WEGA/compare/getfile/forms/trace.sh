#!/bin/sh
# trace.sh NAME: after try.sh, the popt of the tree built with -DTRACE (the same output as /lib/popt) on the codgen output
# of NAME; prints the retry block (from the second "inc _retry" to the next "cp _retry,#10") after each iterate and comjump,
# and the /*CM ...*/ lines of codemove (label, refc of the label, length of the moved block ...). popt [+] in out: "+" is the
# debug switch of TRACE. The TRACE popt is built once into $GF_WORK/poptT.
S=$(cd "$(dirname "$0")" && pwd); R=$S/../../..
: "${WEGA:?WEGA= the WEGA root}"
PATH=$R/../tmp/wbin:$PATH; export PATH
W=${GF_WORK:-$R/../tmp/tmp/getfile-forms}; P=$W/poptT
if [ ! -x "$P/popt" ]; then
	mkdir -p "$P" && cp "$R"/src/cmd/cc/popt/*.c "$R"/src/cmd/cc/popt/*.h "$P/" && cd "$P" || exit 1
	[ -e head ] || { mkdir head; ln -s "$R/src/head/oldv7" head/oldv7; }
	for f in z20 z21 z22 z23; do timeout 300 ccv0p -O -Ihead/oldv7 -DTRACE -c $f.c || exit 1; done
	cp "$R/src/lib/libc-v5.a" lib.a
	timeout 120 ld -X -e start -i -o popt /lib/crt0.o z20.o z21.o z22.o z23.o lib.a || exit 1
fi
cd "$W/$1" || exit 1
timeout 300 ccvm -Ihead/oldv7 -S getfile.c || exit 1
cp "$P/popt" ./poptT
timeout 120 "$R/host/z8k-emu/z8k-emu" -r "$WEGA" ./poptT + getfile.s trace.s > /dev/null 2>&1
grep '/\*CM' trace.s
python3 - <<'PY'
import re
sec=None; buf={}; order=[]
for l in open('trace.s').read().split('\n'):
    m=re.match(r'/\*TRACE (.*)\*/',l)
    if m: sec='%s#%d'%(m.group(1),len(order)); order.append(sec); buf[sec]=[]; continue
    if sec and not l.startswith('/*'): buf[sec].append(l)
for s in order[:8]:
    b=buf[s]; idx=[i for i,l in enumerate(b) if 'inc\t_retry' in l]
    if len(idx)<2: continue
    out=[]
    for l in b[idx[1]:]:
        out.append(l.strip().replace('\t',' '))
        if 'cp _retry,#10' in out[-1] and len(out)>5: break
    print('==',s,' | '.join(out))
PY
