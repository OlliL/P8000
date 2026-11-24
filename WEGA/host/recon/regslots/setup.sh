#!/bin/sh
# setup.sh: the work directory of the register slot experiment (host/OLDCC_REGSLOTS.md), RS_WORK (default
# ../tmp/tmp/regslots beside WEGA):
#   src/      a mirror of the parts of src the cparse build needs; src/cmd/cc/cparse_x = cparse + cparse_x.diff (no
#             q->sfldc = 0 in lookup; knobs XS XR XM XH XV XF XG; with XL=1 a log of every malloc, free, entry, oalloc
#             decision and function start on stderr, by write(2))
#   cparse.x  the scratch cparse; root/ a WEGA root with it as /lib/cparse; ccx: the shipped /bin/cc in that root
#   ts/       the test set: ts/list (program, original, files) and the sources (the witnesses without their unused locals)
#   logs/     the logs of ccx on every file of the test set (XL=1), for replay.py and hyp.py
# Then: python3 hyp.py (score of the logged stream), python3 hyps1.py (the hypotheses), sh score.sh CC TAG (real builds).
set -e
HERE=$(cd $(dirname $0) && pwd)
WEGA=$(cd $HERE/../../.. && pwd)
WORK=${RS_WORK:-$(cd $WEGA/.. && pwd)/tmp/tmp/regslots}
ROOT=${WEGA_ROOT:-$(cd $WEGA/.. && pwd)/tmp/wega31_root}
WBIN=${WBIN:-$(cd $WEGA/.. && pwd)/tmp/wbin}
EMU=$WEGA/host/z8k-emu/z8k-emu
mkdir -p $WORK; cd $WORK

# the mirror of src and the scratch cparse
rm -rf src; mkdir -p src/cmd/cc src/head src/lib
cp -r $WEGA/src/cmd/cc/cparse src/cmd/cc/cparse_x; rm -rf src/cmd/cc/cparse_x/build; mkdir -p src/cmd/cc/cparse_x/build/exp
cp -r $WEGA/src/cmd/cc/mip $WEGA/src/cmd/cc/z8000 src/cmd/cc/
cp -r $WEGA/src/head/oldv7 src/head/; cp $WEGA/src/lib/libc-v7.a src/lib/
(cd src/cmd/cc/cparse_x && patch -s -p1 < $HERE/cparse_x.diff)
[ -f src/cmd/cc/cparse_x/crt0.o ] || cp $WEGA/src/cmd/cc/cparse/crt0.o src/cmd/cc/cparse_x/
(cd src/cmd/cc/cparse_x/build/exp && PATH=$WBIN:$PATH make -f ../../Makefile cparse CFLAGS="-O -I../../../../../head/oldv7" >/dev/null)
cp src/cmd/cc/cparse_x/build/exp/cparse cparse.x

# the root: the WEGA root with the scratch cparse
rm -rf root; mkdir -p root/lib root/tmp
for d in bin usr etc dev; do ln -s $ROOT/$d root/$d; done
for f in $ROOT/lib/*; do b=$(basename $f); [ $b = cparse ] || ln -s $f root/lib/$b; done
ln -s $WORK/cparse.x root/lib/cparse
printf '#!/bin/sh\nexec %s -r %s ${XS:+-e XS=$XS} ${XR:+-e XR=$XR} ${XM:+-e XM=$XM} ${XH:+-e XH=$XH} ${XV:+-e XV=$XV} ${XF:+-e XF=$XF} ${XG:+-e XG=$XG} ${XL:+-e XL=$XL} /bin/cc "$@"\n' $EMU $WORK/root > ccx
chmod +x ccx

# the test set
rm -rf ts; mkdir -p ts/make
for f in Makefile defs main.c doname.c misc.c files.c wegarules.c dosys.c gram.y dyndep.c prtmem.c; do
	git -C $WEGA show 16097d04:./src/cmd/make/$f > ts/make/$f	# make before its unused locals
done
(cd ts/make && PATH=$WBIN:$PATH yacc gram.y >/dev/null && mv y.tab.c gram.c)
echo "make bin/make main.c doname.c misc.c files.c wegarules.c dosys.c gram.c dyndep.c prtmem.c" > ts/list
for p in diff mail nl; do mkdir -p ts/$p; grep -v 'int unused;\|int spare;' $WEGA/src/cmd/$p.c > ts/$p/$p.c; echo "$p bin/$p $p.c" >> ts/list; done
for p in comm csplit tsort uniq; do mkdir -p ts/$p; cp $WEGA/../tmp/sys3/usr/src/cmd/$p.c ts/$p/; echo "$p usr/bin/$p $p.c" >> ts/list; done
for p in ar cmp du grep pr putfile who; do mkdir -p ts/$p; cp $WEGA/src/cmd/$p.c ts/$p/; echo "$p bin/$p $p.c" >> ts/list; done
sed -i '/^\tint u;$/d' ts/ar/ar.c	# the unused locals of ar (rcmd, notfound, longt)
mkdir -p ts/cpio; echo "cpio usr/bin/cpio cpio.c" >> ts/list	# without the unused locals of gmatch and missdir
sed '/^gmatch(/,/^}/{/^\tint i, j, k;$/d}; /^missdir(/,/^}/{/^\tint i;$/d}' $WEGA/src/cmd/cpio.c > ts/cpio/cpio.c

# the logs
rm -rf logs; mkdir -p logs
while read prog orig files; do
	for f in $files; do
		mkdir -p logs/$prog; cp ts/$prog/* logs/$prog/ 2>/dev/null || true
		(cd logs/$prog && XL=1 $WORK/ccx -O -c $f 2> ${f%.c}.log >/dev/null) || true
	done
done < ts/list
echo "work directory: $WORK"
