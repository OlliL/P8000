#!/bin/sh
# Rebuild the members of WEGA/src/lib/libc with the ORIGINAL cc/cas (under z8k-emu)
# and compare each object byte for byte with the member of the original libc.
#
#   check.sh WEGA_ROOT LIBC_SRC ORIG_OBJ_DIR [member ...]
#
# WEGA_ROOT     original WEGA tree (bin/cc, lib/..., usr/include)
# LIBC_SRC      WEGA/src/lib/libc (uses TODO.done for member -> source mapping)
# ORIG_OBJ_DIR  directory with the original members (*.o extracted from libc.a)
# With LISTDIR set, the cas listings (cc -S .s and cas -l .l) of every matching
# member are written there (see ../tools/listing.sh).
# Result lines: ok <flags> | DIFF | MISSING | FAIL
set -u
LISTDIR=${LISTDIR:-}
TOOLS=$(cd "$(dirname "$0")/../tools" && pwd)
ROOT=$1; SRC=$2; ORIG=$3; shift 3
EMU=$(cd "$(dirname "$0")/.." && pwd)/z8k-emu/z8k-emu
WORK=$(mktemp -d); trap 'rm -rf "$WORK"' EXIT
sed -e 's/[[:space:]]*#.*//' "$SRC/TODO.done" | awk 'NF>=2 {print $1, $2}' | while read obj srcname; do
	m=${obj%.o}
	if [ $# -gt 0 ]; then case " $* " in *" $m "*) ;; *) continue;; esac; fi
	f=$(find "$SRC" -name "$srcname" | head -1)
	if [ -z "$f" ]; then echo "MISSING $m ($srcname)"; continue; fi
	[ -f "$ORIG/$obj" ] || { echo "NOREF   $m"; continue; }
	d="$WORK/$m"; mkdir -p "$d"; cp "$(dirname "$f")"/* "$d"/ 2>/dev/null
	res=DIFF; lfl=
	case $srcname in
	*.s) if ! grep -q "module" "$d/$srcname"; then
		# cas syntax (no PLZ/ASM module header)
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/cas -o "$obj" "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  cas"; lfl=cas; }
	     else
	     for fl in "" "-u" "-f" "-f -u"; do
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/as $fl -o "$obj" "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  as $fl"; break; }
	     done; fi ;;
	*.c) for fl in "-O" "" "-Or" "-Ol"; do
		( cd "$d" && rm -f "$obj" && "$EMU" -r "$ROOT" /bin/cc $fl -c "$srcname" >log 2>&1 )
		cmp -s "$d/$obj" "$ORIG/$obj" && { res="ok  cc $fl"; lfl="cc ${fl:--}"; break; }
	     done ;;
	esac
	echo "$res $m ($srcname)"
	# not identical: still list the source with the usual flags
	[ -z "$lfl" ] && case $srcname in *.c) lfl="cc -O";; esac
	if [ -n "$LISTDIR" ] && [ -n "$lfl" ]; then
		ld="$LISTDIR/$(basename "$(dirname "$f")")"
		case $lfl in
		cas)    "$TOOLS/listing.sh" "$ROOT" "$d/$srcname" "$ld" ;;
		"cc -") "$TOOLS/listing.sh" "$ROOT" "$d/$srcname" "$ld" none ;;
		*)      "$TOOLS/listing.sh" "$ROOT" "$d/$srcname" "$ld" ${lfl#cc } ;;
		esac || echo "LISTFAIL $m"
	fi
done
