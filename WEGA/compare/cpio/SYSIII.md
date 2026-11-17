# cpio against System III

`/usr/bin/cpio` of WEGA against `cmd/cpio.c` of System III (no SCCS line). WEGA's SCCS line: `cpio.c 2.4 11/18/83`.
Measured 2026-11-17; built with the shipped cc and `/lib/libc.a`, byte-identical.

## WEGA's own

- The SCCS line `static char sccsid[] = "@[$]cpio.c\t2.4  11/18/83 12:56:19 - 87wega3.2";` stands first in the data.
- Every `err(...)` of System III is `fprintf(stderr, ...)`; the function `err()` is gone (the original calls `fprintf`
  directly, and has no function between `missdir` and `pwd`).
- The usage line of `-p` reads `cpio -p[adlmuv]` (System III: `-p[adlmruv]`).
- The ASCII header (`-c`): `bintochar` writes `"%6o%6o%6o%6o%6o%6o%6o%6o%11lo%6o%11lo%s"` (System III:
  `"%.6o%.6ho..."`, zero-padded), and `chartobin` reads with the same text, `%s` at the end included (System III:
  `"%6o%6ho...%11lo"`, no `%s`). The header is padded with blanks: ` 70707` instead of `070707`.

## Forms that follow the bytes, not the System III text

- `gmatch`: an unused `int i, j, k;` (the frame of the original is 6 bytes larger); `missdir`: an unused `int i;` (2 bytes).
  Odd forms, the real declarations are not known (`findings.json`).
