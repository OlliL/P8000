# 100 slibc into the tree of libc

## Status

active (2026-11-15)

## Goal

One source tree for `/lib/libc.a` and `/lib/slibc.a`: `src/lib/slibc` goes into `src/lib/libc`; C sources once, with `#ifdef`
where the segmented member differs; the PLZ/ASM and cas sources stay separate files where they differ (no preprocessor there).

## Done when

- `src/lib/slibc` holds no source any more (at most what only slibc has, if a separate place is decided); one Makefile (or two
  that use the same sources) builds both archives and `crt0.o`, `scrt0.o`.
- checkall ALL OK (both archives byte-identical); `libtest.sh` 206 of 206 (or adapted to the new paths).
- `src/lib/README.md`, `host/KNOWLEDGE.md` ("slibc: its sources against those of libc") rewritten to the new layout.

## Read first

- `src/lib/README.md` (layout, how to measure a member).
- `host/KNOWLEDGE.md`, section "slibc: its sources against those of libc" (every difference, also the assembler ones).
- `src/lib/libc/Makefile`, `src/lib/slibc/Makefile`; `host/recon/poptveto/s1114/cg/libtest.sh` (uses both trees).

## Known

- Both libraries are built by vl (`ccvl`, `sccvl`), every C member from source; no member from `oldcc/`.
- scc and cc share no cpp symbol except `NULL` (`0L` in scc). A symbol for `#ifdef` must come from the Makefile (`-D...`), and
  the line numbers and the bytes must not change: test every changed file with both compilers (libtest).
- The files (2026-11-15): csu: crt0.s only libc, scrt0.s only slibc. crt: cerror.s, mcount.s differ. gen: 61 C and 1 .h
  equal; malloc.c, symscan.c differ (symscan only in its SCCS comment line, malloc also in the `sccsid` string); mon.c only
  libc, segmon.c only slibc; all 9 .s differ. stdio: 35 C equal, doprnt.c, doscan.c, printf.c, scanf.c differ. sys: all 64
  common .s differ, brk.s only libc, chkseg, mkseg, sgbrk, sgstat, sprofil, ssgbrk only slibc. Top: Library.SC, arhdr.s differ.
- The member order of the two archives differs: two member lists, two object directories.
- PLZ/ASM (`as`) and cas sources have no conditional assembly that we know of: separate files for the segmented version.

## Steps

1. List the files: equal in both trees, C differing, assembler differing, only in one tree (script, `cmp`).
2. Decide the layout (open question below), then move the equal files and the differing C files with `#ifdef`.
3. One build for both archives; checkall.
4. Documentation; `slibc` directory removed or reduced.

## Decisions

- Owner 2026-11-15: the cpp symbol is `-DSEG`. File names stay as they are. The segmented assembler sources go to separate
  directories where they overlap with the nonsegmented ones (crt, gen, sys); csu need not be split (crt0.s, scrt0.s).
- Open (owner): the layout. Proposed: one tree `libc/seg/` that mirrors the layout and holds every file that is only
  segmented or differs there (`seg/Library.SC`, `seg/arhdr.s`, `seg/crt/`, `seg/gen/`, `seg/sys/`); the C files stay in `gen`
  and `stdio` with `#ifdef SEG` (also around the `sccsid` of malloc.c); mon.c and segmon.c stay in `gen`; scrt0.s in `csu`.
  Alternatives: `seg/` inside each directory (`gen/seg/`, ...), or siblings `sgen`, `scrt`, `ssys`.

## Not part of it

The other segmented files of `/lib` (`smcrt0.o`, `scnvt.o`, `sdoprnt.o`, `slibm.a`): increment 150.

## Log

- 2026-11-15: plan written.
