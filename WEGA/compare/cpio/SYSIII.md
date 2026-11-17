# cpio: changes against System III

Compared tmp/sys3/usr/src/cmd/cpio.c with src/cmd/cpio.c; WEGA SCCS line `cpio.c 2.4  11/18/83 12:56:19 - 87wega3.2` (System III has no SCCS id).

- Added `static char sccsid[]` with the WEGA SCCS line as first data (strings of the original).
- The function `err(a,b,c)` is gone; every call is `fprintf(stderr, ...)` directly (original has no err function, calls go straight to fprintf).
- bintochar: sprintf format `%6o...%11lo%6o%11lo%s` (System III: `%.6o%.6ho...`), `Statb.st_rdev` without `& 00000177777` (original code has no mask); `(short)strlen(...)+1` kept as in the System III (casts make no code difference).
- chartobin: sscanf format `%6o...%11lo%6o%11lo%s` (original string data); the trailing `%s` has no argument (kept as the original has it).
- usage(): `-p[adlmuv]` (no `r`, as in the original string).
- missdir: added an unused `short spare;` (original frame is 2 bytes larger).
- bintochar still differs in one register choice (2 bytes), see findings.json.
