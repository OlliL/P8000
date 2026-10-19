#!/bin/sh
# Compare the modules of this directory with the original /bin/cas.
#   check.sh [-v] WEGA=<file tree of the original system> [module ...]
# Each module is compiled with the original scc -O (z8k-emu); its code is compared by
# host/tools/cmpmod.py with the text range from MODULES, its data by host/tools/cmpdata.py
# with the range from DATA. Static functions are made global for the comparison
# (-Dstatic=), so that each function is listed.
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
mods=${*:-$(awk '$1 !~ /^#/ && $4 == "done" || $4 == "done:" {print $1}' "$HERE/MODULES")}
rc=0
for m in $mods; do
	"$EMU" -r "$R" /bin/scc -O -Dstatic= -c $m.c || { rc=1; continue; }
	echo "== $m"
	set -- $(awk -v m="$m" '$1 == m {print $2, $3}' "$HERE/MODULES")
	case "$1" in
	-) ;;
	*) python3 "$TOOLS/tools/cmpmod.py" $m.o "$R/bin/cas" $1 $2 $V | tail -n +2;;
	esac
	set -- $(awk -v m="$m" '$1 == m {print $2, $3}' "$HERE/DATA")
	python3 "$TOOLS/tools/cmpdata.py" $m.o "$R/bin/cas" $1 $2
done
exit $rc
