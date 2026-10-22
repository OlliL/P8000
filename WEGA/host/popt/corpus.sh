#!/bin/sh
# Build the popt test corpus with the original WEGA tools (run in z8k-emu):
#   cc -S <file>.c  ->  <id>.in.s ; /lib/popt <id>.in.s -> <id>.out.s
# (PROG=spopt: scc -S and /lib/spopt, the segmented variant)
# The corpus contains compiler output of the WEGA sources, so it is not kept
# in git.  Usage: corpus.sh SRCDIR OUTDIR   (WEGA=dir with lib/ bin/)
SRC=${1:?source dir}; OUT=${2:?output dir}
PROG=${PROG:-popt}; case $PROG in spopt) CCP=/bin/scc;; *) CCP=/bin/cc;; esac
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd); W=$OUT/.work
find "$SRC" -name '*.c' | sort | while read f; do
  d=$(dirname "$f"); b=$(basename "$f" .c)
  id=$(echo "$f" | sed "s|^$SRC/||; s|/|_|g; s|\.c\$||")
  rm -rf "$W"; mkdir -p "$W"; cp "$d"/* "$W"/ 2>/dev/null; cd "$W" || exit 1
  timeout 60 "$E" -r "$R" $CCP -S -I"$d" "$b.c" >/dev/null 2>&1 || continue
  [ -s "$b.s" ] || continue
  cp "$b.s" "$OUT/$id.in.s"
  if timeout 60 "$E" -r "$R" /lib/$PROG "$b.s" popt_out.s >/dev/null 2>&1; then
    cp popt_out.s "$OUT/$id.out.s"
  else rm -f "$OUT/$id.in.s"; fi
done
rm -rf "$W"
ls "$OUT"/*.in.s | wc -l
