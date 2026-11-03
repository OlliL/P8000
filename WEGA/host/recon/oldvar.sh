#!/bin/sh
# oldvar.sh NAME PATCH.py [PASS]: measure a change of a pass of ccv0p without touching the tree.
#
# Copies src/cmd/PASS (default codgen) to RECON_TMP/oldvar/NAME, runs PATCH.py there (cwd: the copy of the pass; "-"
# for no patch), builds the pass of v0p with the shipped tools, installs it into a copy of OLDROOT and runs the models
# of cparse, popt and codgen with that compiler. Prints, per program, the functions whose difference changed against
# RECON_TMP/oldcc_model_PROG.txt: run `oldcc.sh model PROG` first, so that these lists fit the current sources.
# Environment as in oldcc.sh. The results stay in RECON_TMP/oldvar/NAME/PROG/.
# A switch of codgen is taken out with a patch that deletes its #define from oldcc.h (host/OLDCC.md, Evidence rules).

HERE=`cd \`dirname $0\` && pwd`
REPO=`cd $HERE/../.. && pwd`
WEGA_ROOT=${WEGA_ROOT:-${WEGA:-/mnt/share/cvs/P8000/tmp/wega31_root}}
OLDROOT=${OLDROOT:-/mnt/share/cvs/P8000/tmp/oldroot}
RECON_TMP=${RECON_TMP:-/mnt/share/cvs/P8000/tmp/tmp}
EMU=$REPO/host/z8k-emu/z8k-emu
Z="$EMU -r $WEGA_ROOT"
name=$1 patch=$2 pass=${3:-codgen}
[ -n "$name" ] && [ -n "$patch" ] || { sed -n '2,10p' $0; exit 1; }
case "$patch" in -|/*) ;; *) patch=`pwd`/$patch ;; esac

W=$RECON_TMP/oldvar/$name
rm -rf $W
mkdir -p $W/WEGA/src/cmd
cp -a $REPO/src/cmd/$pass $W/WEGA/src/cmd/
ln -s $REPO/src/lib $W/WEGA/src/lib
ln -s $REPO/host $W/WEGA/host
cd $W/WEGA/src/cmd/$pass || exit 1
if [ "$patch" != - ]; then
	python3 $patch || exit 1
fi
cd v0p || exit 1
rm -f *.o $pass
[ "$pass" = codgen ] && sw=SWITCHES=-DOLD_V0P || sw=
make CC="$Z /bin/cc" CAS="$Z /bin/cas" AR="$Z /bin/ar" LD="$Z /bin/ld" LDBIN="$Z /bin/ld" $sw > $W/build.log 2>&1
[ -f $pass ] || { echo "$name: build failed"; tail -3 $W/build.log; exit 1; }

cp -a $OLDROOT $W/root
cp $pass $W/root/lib/v0p/$pass
printf '#!/bin/sh\nexec %s -r %s /bin/ccv0p "$@"\n' $EMU $W/root > $W/cc
chmod +x $W/cc

for prog in cparse popt codgen; do
	(
		cd $REPO/src/cmd/$prog
		mkdir -p $W/$prog
		RECON_TMP=$W/$prog CCHOST=$W/cc python3 tools/d12model.py -q > $W/$prog/model.txt 2>&1
		RECON_TMP=$W/$prog python3 tools/strictcmp.py > $W/$prog/strict.txt 2>&1
	) &
done
wait

python3 - $W $RECON_TMP <<'P'
import re, sys
W, T = sys.argv[1:3]
def load(f):
    d = {}
    for l in open(f):
        q = l.split()
        if len(q) >= 4 and re.match(r'^[0-9a-f]{3,4}$', q[1]):
            d[q[0]] = 0 if 'SAME' in l else (int(q[-1]) if q[-1].isdigit() else -1)
    return d
for p in ('cparse', 'popt', 'codgen'):
    a, b = load('%s/oldcc_model_%s.txt' % (T, p)), load('%s/%s/strict.txt' % (W, p))
    if not b:
        print(p, 'no model, see', '%s/%s/model.txt' % (W, p))
        continue
    print(p, 'same', sum(x == 0 for x in a.values()), '>', sum(x == 0 for x in b.values()),
          'lines', sum(a.values()), '>', sum(x for x in b.values() if x > 0),
          {k: (a.get(k), b[k]) for k in b if a.get(k) != b[k]})
P
