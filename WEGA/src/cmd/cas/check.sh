#!/bin/sh
# Compare the modules of this directory with the code of the original /bin/cas.
#   check.sh [-v] WEGA=<file tree of the original system> [module ...]
# Each module is compiled with the original scc (z8k-emu) and compared by
# host/tools/cmpmod.py with its text range from MODULES. Static functions are
# made global for the comparison (-Dstatic=), so that each function is listed.
HERE=$(cd "$(dirname "$0")" && pwd)
TOOLS=$HERE/../../../host
V=
[ "$1" = "-v" ] && { V=-v; shift; }
case "$1" in WEGA=*) R=${1#WEGA=}; shift;; esac
: "${R:?set WEGA=<file tree>}"
EMU=${EMU:-$TOOLS/z8k-emu/z8k-emu}
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
cp "$HERE"/*.c "$HERE"/*.h "$T"/
cd "$T" || exit 1
mods=${*:-$(awk '$1 !~ /^#/ && $4 == "done:" {print $1}' "$HERE/MODULES")}
rc=0
for m in $mods; do
	set -- $(awk -v m="$m" '$1 == m {print $2, $3}' "$HERE/MODULES")
	"$EMU" -r "$R" /bin/scc -Dstatic= -c $m.c || { rc=1; continue; }
	echo "== $m"
	python3 "$TOOLS/tools/cmpmod.py" $m.o "$R/bin/cas" $1 $2 $V | tail -n +2
done
exit $rc
