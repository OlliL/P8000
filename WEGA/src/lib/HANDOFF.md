# Handoff: the `oldcc/` members of libc and slibc, compiler vl

State 2026-11-15, evening. Read first: `host/OLDCC.md` ("vl: the compiler of libc.a and slibc.a", D53 to D60),
`host/OLDCC_LOG.md` (entries of 2026-11-14 and 2026-11-15), then this file.

## State

- libc and slibc are built by vl throughout (`CC = ccvl`, `CC = sccvl`): the shipped cparse, gopt, popt (spopt),
  `/lib/vl/scparse` (the shipped one with `OLD_D25`, `src/cmd/cparse` rule `build/segvl/scparse`), `/lib/vl/codgen`,
  `/lib/vl/scodgen` (`OLD_VL` in `src/cmd/codgen/oldcc.h` = `OLD_D53` to `OLD_D60` and `OLD_D24`).
- Committed, each with checkall ALL OK (122 files): D53 to D58 (see `host/OLDCC.md`), then (2026-11-15) 8b78f08d:
  `OLD_D24` + vl scparse with `OLD_D25`, slibc malloc from C (two source forms: `union store *r` in front of `onw`,
  `int spare` in calloc); c63feb89: `OLD_D59` (regforsym: a pointer to a structure in a register is taken from it as a
  pointer to a scalar), `OLD_D60` (match: `++` of a frame slot wanted in a register through it), slibc segmon from C
  (`char *mkseg()`). `libtest.sh`: 203 of 206 (printf of both libraries differ only on the bench: no `-I`).
- libc has no `oldcc` member. slibc: only doprnt is left in `oldcc/`.

## slibc doprnt: open (1324 bytes with vl)

The stdio.h of the time changes nothing. Two places, both a register choice that avoids rr2 (details and measurements:
`host/OLDCC_LOG.md`, entry "vl, slibc doprnt"):
1. `if (_pfile && ferror(_pfile))`: solved in the measurement, not built in. Probe `host/recon/poptveto/s1114/cg/d61.py`
   (PPY for `cg.sh`): when the left operand of an outermost `&&` is a bare name, the registers it takes stay busy through
   the right operand. doprnt 1324 -> 22 bytes, no other member changes. Build in as `OLD_D61` only together with place 2
   (a rule counts when the member gets exact).
2. case 'l' (`if ((c = *fmt++) == 'o')`): the original takes rr4 for the copy of fmt and r3 for c, as if r2 were taken.
   The getreg trace shows r2 as the TEMP (kind 6) of the first switch (`if (prec < 0) switch (c)`), removed from the
   register file when the call `_align(&nreg, &ap, stkap)` in `lng:` wants rr2 by number (regjam). Next: what regjam
   does with a kind-6 node; whether vl keeps it (or the word r2) taken afterwards.
Trace patches used (scratch, rebuild them as needed): a dump of rsuse/rsfree at every getreg (`GR` lines), markers at
`gswitch` and at `L` labels in reader3.c main loop.

## Method (what worked for every member so far)

- Test bench: `host/recon/poptveto/s1114/cg/`. `cg.sh NAME "DEFINES"` (with `PPY=patch.py` for a trial patch of the codgen
  sources) builds codgen and scodgen from the tree with DEFINES (use `-DOLD_VL` plus the trial macro) into a copy of
  OLDROOT as `/lib/codgen`, `/lib/scodgen`; `libtest.sh ROOT` compiles all 206 C members of libc (cc) and slibc (scc) with
  that root and lists the members whose object differs from the member of the original library (extracted to `/tmp/SCRATCH/orig/LIB`; `src/lib/LIB/obj` is the build target and goes with `make clean`). A rule counts only when the wanted member gets
  exact and no other member changes. libc printf and slibc printf show as different there (the bench compiles without
  their `-I`); they are exact in the real build.
- For one member: compile with the root, `host/tools/sdis.py` on both objects, diff. Then print the facts at the place in
  the codgen where the decision falls (a `fprintf(stderr, ...)` patch through `PPY`) for the witness and for the members
  that a broad rule changes, and take the fact that separates them. Every rule of vl was found by narrowing a broad one
  this way (the log has the steps).
- Then: the switch in the tree (`OLD_Dnn` in the `OLD_VL` block), `libtest.sh` again, the Makefile rule from C, the `.s`
  removed, checkall, `host/OLDCC.md` (a section per switch, the vl paragraph), `host/OLDCC_LOG.md`, commit.

- Bench setup after a reboot (`/tmp/SCRATCH` gone): `sh host/tools/wbin.sh /tmp/SCRATCH/wbin <WEGA_ROOT> <OLDROOT> host/z8k-emu/z8k-emu`;
  A vl root needs the vl scparse: `rm -f ROOT/lib/scparse; cp OLDROOT/lib/vk/scparse ROOT/lib/scparse` (same sources and
  switch as `/lib/vl/scparse`). `-I` for the guest: a relative directory below the working directory.

## Slip of this session (repaired; what to watch)

- A copy of OLDROOT (`cp -a`) keeps `lib/scparse`, `lib/codgen` ... as symbolic links to the original files in
  `tmp/wega31_root`. A `cp vk/scparse ROOT/lib/scparse` wrote through the link and overwrote the original
  `tmp/wega31_root/lib/scparse` (2026-11-15 08:44). Found by checkall (`lib/scparse FAIL`). Restored from the independent
  copy `/home/olivleh1/wega_rootfs/lib/scparse` (same bytes as our rebuilt scparse, which earlier checkalls had compared
  with the original; mode, group and time set as the neighbours). No other file of the root was changed (`find -newer`).
  **Always `rm -f` the link before copying a pass into a root copy** (`cg.sh` does).
- Measurements between 08:44 and the repair used the vk scparse (D25) as the shipped one in every root: the D24 runs of
  malloc and one `libtest.sh` run. They are repeated above where it says "cleanly"; the others are marked.
- Do not start a second checkall while one runs (both use `/tmp/wega_check`), and do not `pkill -f` a pattern that is in
  the own command line.
- A trial compile in a source directory with `-S` can overwrite a tracked `NAME.s` (happened with
  `oldlibc/doprnt/0_5.s`, restored from git): compile in a scratch copy.

## Next steps

1. doprnt place 2 (above), then `OLD_D61` + the second rule, libtest, the Makefile rule from C, checkall, `host/OLDCC.md`.
2. `slibc/HANDOFF.md`: bring the member tables to the state of this file (libc has nothing open and no handoff).
3. Open questions of vl that stay: the bound of D53 (between 1170 and 1592 bytes), why D56 holds only segmented, the
   spare word of calloc. Each switch of vl has one source as its witness.
