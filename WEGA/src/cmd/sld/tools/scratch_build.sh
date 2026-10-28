#!/bin/sh
# scratch_build.sh SCRATCHDIR - copy the sources into a scratch tree (a copy of src/cmd/sld below SCRATCHDIR/src/cmd/sld with ../../lib/oldslibc and
# ../../lib/libc/crt), build sld.new with the original tools under the emulator and print the sizes. Use a scratch tree below
# /mnt/share/cvs/P8000/tmp/tmp, the emulator sees only files below its root and the working directory.
D=$1
R=$(cd $(dirname $0)/.. && pwd)
W=${WEGA_ROOT:-/mnt/share/cvs/P8000/tmp/wega31_root}
E="$R/../../../host/z8k-emu/z8k-emu -r $W"
cp $R/*.c $R/ld.h $R/Makefile $D/src/cmd/sld/
for o in printf doprnt; do mkdir -p $D/src/lib/oldslibc/$o; cp $R/../../lib/oldslibc/$o/0_1.s $D/src/lib/oldslibc/$o/; done
mkdir -p $D/src/cmd/sld $D/src/lib/libc; cp -r $R/../../lib/libc/crt $D/src/lib/libc/ 2>/dev/null
cd $D/src/cmd/sld
make SCC="$E /bin/scc" CAS="$E /bin/cas" AR="$E /bin/ar" SLDBIN="$E /bin/sld" link >/dev/null 2>&1
$E /bin/size sld.new | tail -1
