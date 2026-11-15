# 01 slibc into the tree of libc

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
- Many C files are already equal in both trees; the differing C files and the assembler differences are listed in KNOWLEDGE.md.
- PLZ/ASM (`as`) and cas sources have no conditional assembly that we know of: separate files for the segmented version.

## Steps

1. List the files: equal in both trees, C differing, assembler differing, only in one tree (script, `cmp`).
2. Decide the layout (open question below), then move the equal files and the differing C files with `#ifdef`.
3. One build for both archives; checkall.
4. Documentation; `slibc` directory removed or reduced.

## Decisions

- Open (owner): the name of the cpp symbol (e.g. `-DSEG` as in the compiler sources) and where the segmented assembler sources
  live (e.g. `NAME.s` beside `sNAME.s`, or a subdirectory `seg/`).

## Not part of it

The other segmented files of `/lib` (`smcrt0.o`, `scnvt.o`, `sdoprnt.o`, `slibm.a`): increment 06.

## Log

- 2026-11-15: plan written.
