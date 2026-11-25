# egrep against System III

`/bin/egrep` of WEGA against `cmd/egrep.y` of System III, built as `:mkcmd` does: `yacc egrep.y`, `y.tab.c` -> `egrep.c`
(the yacc and `/usr/lib/yaccpar` of WEGA), ccvm, `/lib/libc.a`. Measured 2026-11-25.

## WEGA's own

- The SCCS line `@[$]egrep.y	2.2  11/18/83 15:51:47 - 87wega3.2` (the first data of the binary; System III has none).
- The usage test of `main` asks for `!eflag` too: `if (errflg || ((argc <= 0) && !fflag && !eflag))`. `egrep -e exp`
  without a file reads the standard input; System III prints the usage there.
