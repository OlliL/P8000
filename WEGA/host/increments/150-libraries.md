# 150 The libraries and objects of /lib and /usr/lib

## Status

active (2026-11-18)

## Goal

Every library and object file of `/lib` and `/usr/lib` from source, byte-identical; the remaining assembler members of the
libraries of the old programs from C where a compiler gives them.

## Done when

- COVERAGE shows no library or object of `lib` and `usr/lib` as `sys3` or `todo`.
- `src/lib/oldlibc` and `src/lib/oldslibc` hold `.s` only where the original was assembler or the reason is written down.
- checkall ALL OK.

## Read first

- `src/lib/README.md`, `src/lib/oldlibc/README.md`, `src/lib/oldslibc/README.md`.
- `src/COVERAGE.md`, sections lib and usr/lib.

## Known

- Open in `/lib` (2026-11-15): `mcrt0.o`, `smcrt0.o`, `scnvt.o`, `sdoprnt.o`, `slibm.a` and others (COVERAGE); `/usr/lib`:
  e.g. `slibmp.a`.
- oldlibc: 13 `.s` (90 of 103 modules from C); oldslibc: 18 `.s` (1 of 19 from C), never tried from C.
- vl keeps two switches with one witness (`OLD_D53`, `OLD_D57`, owner's exception, `host/OLDCC.md` section 5).

## Steps

1. The list from COVERAGE; date each (`host/OLDCC_DATES.md`) to find its compiler.
2. One fresh session per library (HOWTO, "Libraries"; `host/tools/nightly.sh lib:libX`): `host/recon/libsetup.py` finds the
   sources (System III, 2.11BSD, 4.4BSD-Lite) and the compiler of each member; the session builds and matches them.
3. oldlibc, oldslibc: the `.s` from C (source forms first, a new switch last).

## Decisions

- 2026-11-18 (owner): sources of libraries also from 2.11BSD and 4.4BSD-Lite when System III has none (copied as System III);
  libl is the pattern; one fresh session per library, as for the programs.

## Not part of it

PLZ libraries and objects made by `as`: 170 when they need the PLZ compiler.

## Log

- 2026-11-17: libm.a, slibm.a from source (System III, v1; fputil.s PLZ/ASM from the original by s2as.py, which now writes
  fsetmode and fldctl with a pair), not identical: fputil.o exact, the 15 C members open for the compiler (floating compare
  without rounding, EPU temporaries; `compare/libm/findings.json`, `OLDCC_LOG.md`); WEGA's exp.c, jn.c, pow.c; archives in OPEN.
- 2026-11-18: libsetup.py (tried on liby, libtermlib, libmp: every member has a source), HOWTO "Libraries", nightly.sh lib:.
- 2026-11-17: liby.a, sliby.a byte-identical (System III sources, v1; WEGA's main.c returns yyparse()). libsetup.py: one
  source tree after the other, by name then by content (it took 4.4BSD's main.c before System III's libmai.c); the padding
  byte of an odd last member goes into arhdr_*.s, with the mkar.sh of libc.
- 2026-11-18: libm.a, slibm.a byte-identical (v1 with OLD_D61, OLD_D62; the host cparse reads floating constants with a
  port of the WEGA atof).
- 2026-11-18: libPW.a, slibPW.a byte-identical. WEGA forms in cat, regcmp, regex
  (register arguments), xalloc (seg: ssgbrk), fatal, setsig, xcreat, xmsg; move.c after pdp11/move.c. Compiler: OLD_D24, OLD_D25 and
  OLD_D2CW (new) into the segmented v1, OLD_D20 (block label named by at most 3 jumps) and OLD_D52 (A more than one instruction)
  narrowed (`OLDCC_LOG.md`).
- 2026-11-18: libmp.a, slibmp.a byte-identical (no System III source: 2.11BSD usr.lib/libmp without sccsid, v1 with the V7
  stdio.h; WEGA forms in gcd.c and mout.c). libsetup.py copies the sources writable.
- 2026-11-18: libtermlib.a, slibtermlib.a byte-identical (no System III source: 2.11BSD usr.lib/libtermlib with its CM_
  options, the older forms of tgetent, tnchktc, tgoto, tputs; v1).
- 2026-11-18: libcurses.a, slibcurses.a byte-identical (no System III source, 2.11BSD's is the later 4.3BSD one: 4.2BSD
  usr.lib/libcurses from TUHS (unix-history-repo, owner), initscr.c and scroll.c of 4.1cBSD; the shipped cc, scc without -O;
  WEGA forms in cr_tty, getstr, newwin and the printw/scanw family). libsetup.py copies the headers of the source tree
  (`.h`, `.ext`) with it.
