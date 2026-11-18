# libtermlib against its source

System III has no termlib; the source is `usr.lib/libtermlib` of 2.11BSD (5.1 of 1985), built with the options of its
Makefile (`-DCM_N -DCM_GT -DCM_B -DCM_D`). `/usr/lib/libtermlib.a` and `slibtermlib.a` (Library.SC 01/23/83) have the
older code. Measured 2026-11-18.

- All members: no `sccsid` block.
- `termcap.c`, `tgetent`: `char *cp2; tf = 0;`; a TERMCAP that is not a path is taken when `getenv("TERM")` is unset or
  equals the name (`strcmp`), otherwise /etc/termcap is opened; `if (tf==0)` opens /etc/termcap.
- `termcap.c`, `tnchktc`: `while (q && *q != ':')`; no `hopcount = 0` (neither after a failed `tgetent` nor at the end).
- `tgoto.c`: `MAXRETURNSIZE` 16; `which == '\t'` is tested (2.11BSD has it in a comment).
- `tputs.c`: `tmspc10` without the last entry 5.
