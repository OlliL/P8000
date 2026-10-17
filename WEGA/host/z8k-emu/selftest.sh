#!/bin/sh
# Oracle self test: rebuild original WEGA objects with the ORIGINAL compilers
# (cc/scc and all their passes, run under z8k-emu) and compare byte for byte
# with the objects shipped on the disk image.
#
#   selftest.sh WEGA_ROOT UTS_DIR KERNEL_OBJ_DIR
#
# WEGA_ROOT       extracted disk image (bin, lib, usr, ...), see KNOWLEDGE.md
# UTS_DIR         extracted uts.tar.gz (contains dev/, conf/, h/, sys/)
# KERNEL_OBJ_DIR directory with the lib2/*.o members of the kernel LIB2
set -u
ROOT=$1; UTS=$2; OBJ=$3
EMU=$(cd "$(dirname "$0")" && pwd)/z8k-emu
WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cp -r "$UTS" "$WORK/uts"
cp -n "$ROOT"/usr/include/sys/*.h "$WORK/uts/h/" 2>/dev/null
fail=0
cmpobj() {	# name builddir compiler flags reference
	( cd "$2" && rm -f "$1.o" && "$EMU" -r "$ROOT" "/bin/$3" $4 -c "$1.c" >"$1.log" 2>&1 )
	if cmp -s "$2/$1.o" "$5"; then echo "ok   $3 $4 $1.c"; else echo "FAIL $3 $4 $1.c"; fail=1; fi
}
for m in lp bio mem partab sys md ud fd; do
	cmpobj $m "$WORK/uts/dev" scc -O "$OBJ/$m.o"
done
cmpobj wpar "$WORK/uts/conf" scc -O "$ROOT/usr/sys/conf/wpar.o"
cmpobj bstart "$WORK/uts/conf" cc -O "$ROOT/usr/sys/conf/bstart.o"
cmpobj bstart "$WORK/uts/conf" cc "" "$ROOT/usr/sys/conf/bstart.o"
exit $fail
