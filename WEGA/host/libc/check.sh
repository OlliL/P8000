#!/bin/sh
# Rebuild the members of WEGA/src/lib/libc with the ORIGINAL cc/cas (under z8k-emu)
# and compare each object byte for byte with the member of the original libc.
#
#   check.sh WEGA_ROOT LIBC_SRC ORIG_OBJ_DIR [member ...]
#
# WEGA_ROOT     original WEGA tree (bin/cc, lib/..., usr/include)
# LIBC_SRC      WEGA/src/lib/libc: the member foo.o is built from foo.c or foo.s
#               (the workarounds are in ../../src/lib/libc/oldcc, see ../../src/TODO.md)
# ORIG_OBJ_DIR  directory with the original members (*.o extracted from libc.a)
# Result lines: ok <flags> | DIFF | MISSING | FAIL
set -u
ROOT=$1; SRC=$2; ORIG=$3; shift 3
EMU=$(cd "$(dirname "$0")/.." && pwd)/z8k-emu/z8k-emu
WORK=$(mktemp -d); trap 'rm -rf "$WORK"' EXIT
for obj in $(cd "$ORIG" && ls *.o); do
	m=${obj%.o}
	if [ $# -gt 0 ]; then case " $* " in *" $m "*) ;; *) continue;; esac; fi
	f=$(cd "$SRC" && find . \( -name "$m.c" -o -name "$m.s" \) -print | head -1)
	if [ -z "$f" ]; then echo "MISSING $m"; continue; fi
	f="$SRC/$f"; srcname=$(basename "$f")
	d="$WORK/$m"; mkdir -p "$d"; cp "$(dirname "$f")"/* "$d"/ 2>/dev/null
	res=DIFF
	case $srcname in
	*.s) if ! grep -q "module" "$d/$srcname"; then
		# cas syntax (no PLZ/ASM module header)
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/cas -o "$obj" "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  cas" ; }
	     else
	     for fl in "" "-u" "-f" "-f -u"; do
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/as $fl -o "$obj" "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  as $fl"; break; }
	     done; fi ;;
	*.c) for fl in "-O" "" "-Or" "-Ol"; do
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/cc $fl -c "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  cc $fl"; break; }
	     done ;;
	esac
	echo "$res $m ($srcname)"
done
