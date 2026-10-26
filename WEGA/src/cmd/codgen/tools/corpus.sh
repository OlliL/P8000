#!/bin/sh
# Build the codgen/scodgen test corpus with the original WEGA tools (run in z8k-emu):
#   cc -S -:d <file>.c keeps the files of the passes: <base>.I (intermediate text from cparse), <base>.D (data text), <base>.S
#   (symbol table, binary); the compiler output <file>.s is the reference. Result per source: <id>.I .D .S and <id>.out.s.
# The corpus is compiler output of the WEGA sources, so it is not kept in git.
# Usage: corpus.sh SRCDIR OUTDIR   (absolute paths; WEGA=dir with lib/ bin/; PROG=codgen or scodgen)
SRC=${1:?source dir}; OUT=${2:?output dir}
PROG=${PROG:-codgen}; case $PROG in scodgen) CCP=/bin/scc;; *) CCP=/bin/cc;; esac
HERE=$(cd "$(dirname "$0")" && pwd)
E=${E:-$HERE/../../../../host/z8k-emu/z8k-emu}; R=${WEGA:?set WEGA to the WEGA file tree}
mkdir -p "$OUT"; OUT=$(cd "$OUT" && pwd); W=$OUT/.work
find "$SRC" -name '*.c' | sort | while read f; do
  d=$(dirname "$f"); b=$(basename "$f" .c)
  id=$(echo "$f" | sed "s|^$SRC/||; s|/|_|g; s|\.c\$||")
  rm -rf "$W"; mkdir -p "$W"; cp "$d"/* "$W"/ 2>/dev/null; cd "$W" || exit 1
  timeout 60 "$E" -r "$R" $CCP -S -:d -I"$d" "$b.c" >/dev/null 2>&1 || continue
  [ -s "$b.s" ] || continue
  set -- cc.*.I; base=${1%.I}
  [ -f "$base.D" ] && [ -f "$base.S" ] || continue
  cp "$base.I" "$OUT/$id.I"; cp "$base.D" "$OUT/$id.D"; cp "$base.S" "$OUT/$id.S"; cp "$b.s" "$OUT/$id.out.s"
done
rm -rf "$W"
ls "$OUT"/*.I | wc -l
