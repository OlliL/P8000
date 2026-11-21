# Handoff: libcisam, the compiler (increment 150)

Done 2026-11-19: libcisam.a and slibcisam.a byte-identical with the compiler v1c (compare/libcisam/COMPILER.md).
What follows is the state at the hand-off.

Goal: `/usr/lib/libcisam.a` and `slibcisam.a` (C-ISAM 1.03, Library.SC 02/11/83) byte-identical. The sources are written
(`src/lib/libcisam`, 14 `.c` and `isint.h`, commit 5a02e79b) and not in the build yet. What is left needs the compiler: v1
is nearest, but some members show code that v1 does not make. Find the switches (new or narrowed) that give it.

## State (2026-11-18)

Per member in `compare/libcisam/findings.json`. Lines of `diff` of the sdis listings:

| Member | ccv1 -O | sccv1 -O | What differs |
|---|---|---|---|
| isdef, isperror, util, key, lock | exact | exact | |
| node | exact | 4 | btremove: scratch register r2/r6 |
| buf | exact | 108 | bfread, bfwrite: scratch registers (the node trailer bytes, `stint`) |
| supp | 4 | exact | freefile, `close(fp->f_audfd)`: the original keeps the parameter fp in r2 (temp r6), v1 takes r3 |
| read | 4 | 6 | isread: the original takes r3 for `openp` (r2 holds `lock`), v1 r2 |
| audit | 4 | 98 | isaudit AUDSTOP, `close(filep->f_audfd)` after `if (... && filep->f_flags & 04)`: the original reloads filep (temp r3), v1 reuses r3 (temp r6) |
| open | 17 | 267 | isopen (`ldint(dictp)`: `ld r5,_dictp` against `ld r3,r5`), isflush (rr2/rr4), isexit (`isstat2 = ... ? '2' : '0'`, r3/r4) |
| btree | 41 | 29 | btsearch: register after the dup test; btdelitem: the original popt leaves the loop test at the head, v1 popt inverts the loop |
| item | 68 | 106 | itcompare FLOATTYPE (below); itpress: r4/r2 for `kp->k_len`, the leading-compression loop not inverted in the original |
| build | 162 | 288 | isaddindex: loop and error block laid out by popt differently; isbuild: registers in the dictionary init |

Three kinds:

1. **Float compare** (item.o itcompare, the clearest): original `fld F0,f1` (4FF4,8001), `fld F1,f2` (4FF4,8101),
   `8E00,4120` (single, register against register), then `jr c` / `jr gt`. v1: `fld F0,f1`, `4FF4,4021,f2` (from memory,
   to double), `8E40,C010`. No source form gives it with any compiler in OLDROOT (tried casts, `register float`, unions,
   arrays, `f2 + 0`, assignments in the compare). Candidates: codgen tables (table3_codgen.c, the compare rules near rule
   052, see D62 in OLDCC.md) or cparse not widening a float/float compare.
2. **Scratch register choice in codgen** (most of the rest, also most of the segmented diffs). Sometimes the original
   reloads a name v1 keeps, sometimes the reverse (supp: a parameter kept). Tried on v1, each alone: OLD_D2N (no effect),
   OLD_D2L, OLD_D2F, OLD_D2W (all much worse), OLD_D2C (build failed alone). See D2 in OLDCC.md (allo3.c allocreg, getreg).
   Look at the unoptimized codgen output (`ccv1` without -O) of the small cases first: audit isaudit, read isread, supp
   freefile (4 lines each).
3. **popt loop inversion**: the original leaves `for`/goto loops with the test at the head (btdelitem, itpress,
   isaddindex). See D20, D49..D52 in OLDCC.md (popt z21.c codemove).

## How to measure

- Compile one member: copy `src/lib/libcisam/X.c` and `isint.h` into a scratch dir, then
  `host/z8k-emu/z8k-emu -r $OLDROOT /bin/ccv1 -O -I. -c X.c` (segmented: `sccv1`). The headers `isam.h`, `sys/stat.h`
  come from the guest root. Original members: `host/tools/war.py extract tmp/wega31_root/usr/lib/libcisam.a DIR`
  (and `slibcisam.a`). Compare `host/tools/sdis.py` listings with `diff`.
- An experimental pass without touching OLDROOT: copy `tmp/oldroot` (2 MB) to the scratchpad, `mkdir -p
  src/cmd/codgen/build/nonsegv1x`, then `OLDROOT=<copy> RECON_TMP=<scratch> sh host/recon/oldcc.sh build codgen v1x
  "-DOLD_D3 -DOLD_D61 -DOLD_D62 -DOLD_...."` and copy `<copy>/lib/v1x/codgen` over `<copy>/lib/v1/codgen`; run ccv1 with
  `-r <copy>`. Remove `build/nonsegv1x` afterwards. v1 switches: OLDCC.md section 2 (codgen v1: OLD_D3, OLD_D61,
  OLD_D62; scodgen also OLD_D24, OLD_D2CW; popt OLD_D20, OLD_D52; cparse OLD_D1, OLD_D12, scparse OLD_D25).
- The sources are fixed by the exact members: change a source only when the nonsegmented member stays exact.

## Rules

- A new or narrowed switch may not break another v1 user: `host/recon/oldcc.sh matrix check`, then checkall
  (`git add`, `git stash create`, `CHECK_NAME=x CHECK_PATCH=<ref> host/tools/checkall.sh`).
- New switch facts to OLDCC.md (rewritten), measurements to OLDCC_LOG.md (dated), the table "Objects and their
  differences" in OLDCC.md.
- Never change checkall, OPEN or the corpora without asking the owner. libcisam goes into `src/lib/Makefile` only when
  both archives are identical (or the owner decides on OPEN).
- When done: Makefile after `src/lib/libscreen/Makefile` (ccv1/sccv1 -O; arhdr and mkar.sh: `host/recon/libsetup.py
  libcisam` takes `src/lib/libcisam` as the source directory), COVERAGE, increment log, commit (no push).

## Also open

- OLDCC.md D1: the v1 fact that the slot words of register variables stand where the `register` declarations stand
  among the automatic variables (now only in the increment log).
