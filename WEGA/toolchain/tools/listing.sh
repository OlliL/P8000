#!/bin/sh
# Produce the cas listings of a source file with the ORIGINAL tools:
#   listing.sh WEGA_ROOT SRC OUTDIR [cc flags]
# cc flags: pass "none" for no flag.
# For file.c:  OUTDIR/file.s  (cc -S output, the assembler source)
#              OUTDIR/file.l  (cas -l listing: addresses, code bytes, source)
# For file.s:  OUTDIR/file.l  (cas -l listing)
# The listings are checked in so that object code can be compared with
# plain diff/text tools until the original (older) cc is available.
set -e
ROOT=$1; SRC=$2; OUT=$3; shift 3
SRC=$(cd "$(dirname "$SRC")" && pwd)/$(basename "$SRC")
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd)
EMU=$(cd "$(dirname "$0")/.." && pwd)/z8k-emu/z8k-emu
W=$(mktemp -d); trap 'rm -rf "$W"' EXIT
b=$(basename "$SRC"); n=${b%.*}
mkdir -p "$OUT"
cp "$(dirname "$SRC")"/* "$W"/ 2>/dev/null || cp "$SRC" "$W/$b"
cd "$W"
case $b in
*.c)
	"$EMU" -r "$ROOT" /bin/cc $(for f; do [ "$f" = none ] || echo "$f"; done) -S "$b" >log 2>&1 || { cat log; exit 1; }
	"$EMU" -r "$ROOT" /bin/cas -u -l -o "$n.o" "$n.s" >log 2>&1 || { cat log; exit 1; }
	cp "$n.s" "$n.l" "$OUT"/ ;;
*.s)
	"$EMU" -r "$ROOT" /bin/cas -u -l -o "$n.o" "$b" >log 2>&1 || { cat log; exit 1; }
	cp "$n.l" "$OUT"/ ;;
esac
