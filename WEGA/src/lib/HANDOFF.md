# Handoff: the `oldcc/` members of libc and slibc, compiler vl

State 2026-11-15, morning. Read first: `host/OLDCC.md` ("vl: the compiler of libc.a and slibc.a", D53 to D57),
`host/OLDCC_LOG.md` (entries of 2026-11-14 and 2026-11-15), then this file.

## State

- libc and slibc are built by vl throughout (`CC = ccvl`, `CC = sccvl`): the shipped cparse, gopt, popt with
  `/lib/vl/codgen`, `/lib/vl/scodgen` (`OLD_VL` in `src/cmd/codgen/oldcc.h`).
- Committed, each with checkall ALL OK (122 files): D53, D54 (nlist, symscan of both), D55 (slibc scanf), D56 (slibc
  printf, with `-I../../../cmd/cparse`), D57 (swap of both). Last commit: 35a63432.
- **Committed without a finished checkall (commit after dcc48ac1):** `OLD_D58` (regs3.c `regforsym`: an unsigned name known in a register is taken
  from it also where it is used as an int; calloc `num *= size`), `oldcc.h`, libc malloc from `gen/malloc.c`
  (`src/lib/libc/oldcc/Makefile`, `oldcc/malloc` removed). `libtest.sh`: 201 of 206, libc malloc exact, nothing else
  changes (`int` used as `unsigned` must not be taken: fputc). A checkall of this state was started at the end of the
  session (`tmp/tmp/checkall_d58.log`); read its result first; if it is not ALL OK, find the cause before going on. The two runs before it are void
  (see "Slip" below).
- Left in `oldcc/`: slibc malloc, segmon, doprnt. libc has none left once D58 is committed.

## slibc malloc: solved in the measurement, not built in

`gen/malloc.c` gives the member exactly (0 bytes) with three things:
1. scodgen with `OLD_D24` (a conversion between a pointer and a non-pointer of the same size stays; the switch of vk) and
   scparse with `OLD_D25` (a cast of a long to a pointer stays a conversion; `/lib/vk/scparse` is the shipped scparse with
   it). D24 alone changes nothing (1657 bytes), both together leave 13.
2. In `realloc` the declaration `union store *r;` in front of `register unsigned onw;` (4 bytes left).
3. In `calloc` an unused `int spare;` (the frame is 2 bytes larger than ours; the cause is not found, as the spare word
   of ld in `host/OLDCC.md` D1).
To build in: vl needs a scparse of its own (`src/cmd/cparse`: a rule `build/segvl/scparse` with `-DSEG -DOLD_D25`,
installed as `/lib/vl/scparse`; `sccvl` in `src/cmd/cc/cc.c` then names it), `OLD_D24` into the `OLD_VL` block of
`oldcc.h`. Then `libtest.sh` with such a root (the root must get the vl scparse as `/lib/scparse`: **remove the symbolic
link first**, see "Slip"). One contaminated run suggests D24 + D25 change no other member (201 of 206); measure it again
cleanly, then checkall. nonsegmented: D24 on libc changed nothing in that run either.

## slibc segmon, doprnt: open

Bytes that differ with vl: segmon 1250 (object 2410 against 2422), doprnt 1324 (6200 against 6204). With D24 segmon gets
worse (1326, 2446 bytes); D24 + D25 not measured cleanly for them. Known from `slibc/HANDOFF.md`: segmon: a parameter
taken from its register (`ldl rr2,rr6`; compare D58 and `OLD_D3P`), `++i` through a register (as `OLD_D16A`), the long
cast to a pointer masked through a copy (D24/D25); doprnt: a register choice as in symscan at two places (compare D54).
Check first whether doprnt needs the stdio.h of the time (`-I../../../cmd/cparse`) as printf did: the putc form changes
the block order.

## Method (what worked for every member so far)

- Test bench: `host/recon/poptveto/s1114/cg/`. `cg.sh NAME "DEFINES"` (with `PPY=patch.py` for a trial patch of the codgen
  sources) builds codgen and scodgen from the tree with DEFINES (use `-DOLD_VL` plus the trial macro) into a copy of
  OLDROOT as `/lib/codgen`, `/lib/scodgen`; `libtest.sh ROOT` compiles all 206 C members of libc (cc) and slibc (scc) with
  that root and lists the members whose object differs from `obj/NAME.o`. A rule counts only when the wanted member gets
  exact and no other member changes. libc printf and slibc printf show as different there (the bench compiles without
  their `-I`); they are exact in the real build.
- For one member: compile with the root, `host/tools/sdis.py` on both objects, diff. Then print the facts at the place in
  the codgen where the decision falls (a `fprintf(stderr, ...)` patch through `PPY`) for the witness and for the members
  that a broad rule changes, and take the fact that separates them. Every rule of vl was found by narrowing a broad one
  this way (the log has the steps).
- Then: the switch in the tree (`OLD_Dnn` in the `OLD_VL` block), `libtest.sh` again, the Makefile rule from C, the `.s`
  removed, checkall, `host/OLDCC.md` (a section per switch, the vl paragraph), `host/OLDCC_LOG.md`, commit.

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

1. Result of the checkall for D58 + libc malloc; commit (`host/OLDCC.md`: add D58 and libc malloc to the vl paragraph,
   D53 to D58; `libc/HANDOFF.md`: no open member left).
2. slibc malloc as above (vl scparse with D25, D24 in vl, the two source forms), libtest, checkall, commit.
3. segmon, doprnt with the method above.
4. `slibc/HANDOFF.md` and `libc/HANDOFF.md`: bring the member tables to the state of this file.
5. Open questions of vl that stay: the bound of D53 (between 1170 and 1592 bytes), why D56 holds only segmented, the
   spare word of calloc. Each switch of vl has one source as its witness.
