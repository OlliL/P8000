# file against System III

`/bin/file` of WEGA (`file.c 1.2 07/24/82`, ccv0, libc-v1) against `cmd/file.c` of System III (Plexus `cmd/file.c` has the
same text). ZEUS 3.21 ships a later version (1.3 05/28/84) without a source. The source was written from the binary on
the base of System III: `main`, `lookup`, `ccom`, `ascom`, `english` and most of `type` keep its text. Measured 2026-11-25.

## WEGA's own

- The SCCS line `@[$]file.c	1.2  07/24/82 00:15:39 - 87wega3.2`; no `utime` at the end of `type` (so `out:` is a return).
- s.out instead of a.out: the magic numbers of `<s.out.h>` in one switch, also byte swapped (0x07e5 for `X_MAGIC1` ...):
  Z8 or Z80 (`SF_Z8`) executable, separate, overlay; the others "executable", "segmented" or " non-segmented" (bit 0x100 of
  the magic), ",not stripped" when `s_syms` is set. No "ldp", "pure", "PDP-11 executable", "cpio archive", "fifo", no version.
- "cannot stat" and "cannot open" (System III: "cannot open" and "cannot open for reading").
- A buffer of 1024 bytes; `refill` reads the next block (at the end of the file the first one again), used in the comment
  loop of `ccom` and in the `#` lines.
- `#` lines: blanks after `#`, an empty directive, the preprocessor words `define include ifndef` (c program text).
- csh command text (`set`, `foreach`); the C words `typedef`, `static`.
- Assembler: no `clr` in the PDP-11 list, no `byte`; "roff, nroff, or eqn input text" (System III "[nt]roff, tbl, or eqn").
- PLZ: comments `! ... !` (`plzcom`), Z8 program text (the Z8 mnemonics), plz/asm program text (and " segmented"),
  plz/sys program text (`module`, `CONSTANT` ...); `skipw` goes to the next white space.
