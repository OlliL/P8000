#!/bin/sh
# Make parser.c and parser.h from parser.y with the original yacc of WEGA, run by z8k-emu.
# usage: WEGA=<file tree of the original system> sh mkparser.sh
#
# parser.c is kept in the repository, so that building the host cas needs no yacc: the tables of
# a yacc of this host (bison, byacc) are not those of the original. They use other default
# reductions, which changes where a syntax error is found and so which of the error rules
# (and which message) is taken. The driver of the generated parser is the one of the original
# (/usr/lib/yaccpar).
set -e
: "${WEGA:?set WEGA=<file tree of the original system>}"
here=$(cd "$(dirname "$0")" && pwd)
emu=${EMU:-$here/../z8k-emu/z8k-emu}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
cp "$here/parser.y" "$tmp/"
(cd "$tmp" && "$emu" -r "$WEGA" /usr/bin/yacc -d parser.y)
# the stacks of the generated driver may be used one entry beyond YYMAXDEPTH, yyparse() gets a prototype, and
# the stack pointers start one entry before the stacks without indexing -1 (the sanitizers complain about &yys[-1])
sed -e 's/^\(YYSTYPE yyv\[YYMAXDEPTH\)\]/\1 + 1]/' \
    -e 's/^\(	short yys\[YYMAXDEPTH\)\]/\1 + 1]/' \
    -e 's/^yyparse() {/int yyparse(void) {/' \
    -e 's/yyps= &yys\[-1\];/yyps= yys - 1;/' \
    -e 's/yypv= &yyv\[-1\];/yypv= yyv - 1;/' \
    "$tmp/y.tab.c" > "$here/parser.c"
cp "$tmp/y.tab.h" "$here/parser.h"
grep -c 'YYMAXDEPTH + 1' "$here/parser.c" | grep -qx 2 || { echo "mkparser: the driver was not changed as expected" >&2; exit 1; }
