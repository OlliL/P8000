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

- 2026-11-18: libsetup.py (tried on liby, libtermlib, libmp: every member has a source), HOWTO "Libraries", nightly.sh lib:.
