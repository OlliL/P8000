#!/bin/sh
# Compare ./ar with the original /bin/ar (run in z8k-emu): the scenarios in
# scenarios.sh run with both; the output of every command, its exit code and the
# archives and extracted files (names, modes, sizes, checksums) must be identical.
#   check.sh   (WEGA=dir with bin/ lib/; E=emulator)
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
unset TZ; export LC_ALL=C
one() {		# $1 = name, $2 = ar command
	mkdir "$T/$1" && cd "$T/$1" || exit 1
	umask 022
	AR="$2" sh "$HERE/scenarios.sh" > log 2>&1
	{ cat log; echo "== files"; ls -la | awk '$1 !~ /^total/ && $1 !~ /^d/ {print $1,$5,$9}' | sort -k3
	  for f in *; do [ -f "$f" ] && echo "$f $(cksum < "$f")"; done; } > "$T/$1.out"
	cd "$T"
}
# the default time zone of WEGA (EST5EDT) and some set by TZ, which tv prints
rc=0
for tz in "" MET-1MEST PST8PDT JST-9 GMT0; do
	rm -rf "$T"/orig "$T"/host
	if [ -n "$tz" ]; then
		one orig "$E -e TZ=$tz -r $R /bin/ar"
		TZ=$tz one host "$HERE/ar"
	else
		one orig "$E -r $R /bin/ar"
		one host "$HERE/ar"
	fi
	if cmp -s "$T/orig.out" "$T/host.out"; then
		echo "ar [TZ=${tz:-default}]: identical ($(grep -c '^\$ ar' "$T/orig.out") commands)"
	else
		echo "ar [TZ=${tz:-default}]: DIFFERENT"; diff "$T/orig.out" "$T/host.out" | head -${LINES:-40}; rc=1
	fi
done
# the libraries of the system: listings and a member read with both
for lib in libc.a slibc.a; do
	"$E" -r "$R" /bin/ar tv /lib/$lib > "$T/o.lst" 2>&1
	"$HERE/ar" tv "$R/lib/$lib" > "$T/h.lst" 2>&1
	if cmp -s "$T/o.lst" "$T/h.lst"; then echo "ar tv $lib: identical ($(wc -l < "$T/h.lst") members)"
	else echo "ar tv $lib: DIFFERENT"; diff "$T/o.lst" "$T/h.lst" | head -10; rc=1; fi
done
exit $rc
