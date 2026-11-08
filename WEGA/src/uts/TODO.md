# uts: TODO

Goal: `make install` in `src/` writes `boot`, `wega`, `usr/boot` and `usr/sys/...` below PREFIX, and `host/tools/checkall.sh`
finds every one of them byte-identical (`cmp`) with the WEGA root. Everything below is ordered towards that goal.
Rule: finished (sub)items are deleted from this file, so it gets shorter with every step.
`uts` is built by `all` in `src/Makefile` but NOT by `install` (and not added to `checkall.sh`) until `wega`, `LIB1`, `LIB2` and the rest are
identical: before that `checkall.sh` would stop at the first difference. The install targets in `uts/*/Makefile` are written and tested by hand
with `make install PREFIX=dir`; they are connected to `src/Makefile install` at the very end (7.).
State measured with the build in a copy of `src/uts` (tools from `checkall.sh`), objects against `tmp/wega31_kernel`.

## State

- Objects not identical yet: `sys1.o` (LIB1), `tt0.o` and `tty.o` (LIB2), `dev/wega/siok.o` (`sioioctl`). Everything else in LIB1 and LIB2 is identical.
- `wega`: 97282 bytes like the original, content differs because of the objects above and the data of `siok`/`wpar`.
- `BOOTLIB`: 63 bytes differ (archive headers). `wpar.o` in the root is from an older `wpar.c`.

## 1. Build and install

1. `COVERAGE.md`: regenerate (`host/tools/coverage.py`) when files become exact.

## 2. The links of `wega` and `boot` (what decides 100% of two files)

1. `dev/wega/lp.c` (identical object) and `dev/wega/siok.c` (text size equal, 3 diff hunks left) are reconstructed from the shipped `wega`; `LIB2W`
   (LIB2 with these two members replaced) is what `wega` links. The shipped LIB2 has other versions of both members. Left in `siok.c`:
   `sioioctl` has the code blocks of the cases in another order (the orig places the TIOCMCS body before the second comparison: a
   dispatch at the end of a switch, threaded by popt; the shipped scc does not generate this for the forms tried: if/else chains, switch with
   break/return/default, while/for, goto); the data of `siok.o` (28 bytes more than LIB2's) is not compared yet. Material from WEGA 3.0
   (`/home/olivleh1/Downloads/tmp`: LIB1, LIB2, lp.c with `$D$`, wpar.o, ...) shows that 3.0's `lp.o` and `siok.o` are older again (not the ones in `wega`).
   The link places `tty tt0 md lp cpu2` in segment 2 (first fit); proven when the objects are right.
2. `wpar.o`: the root copy is from `wpar.c` 3.0; check the `wpar` data part of `wega` against ours (data `_md_flag` ...); the objects of `wpar` have no text.
3. After 1 and 2: the remaining differences of `wega` are the objects of section 3; the link order and the symbol table (`-X`, `-i`, `-Ms62`) are proven by the identical parts.
4. `u.o`: still a binary `u.obj` copied (`$ABS` works only in segment 1; `u` must lie at segment 62, `0xf600`). Target: `u.s`/`u.c` that gives the same bytes (README). Until then `u.obj` stays and is documented.

## 3. Objects that differ

Compiler `vk` (`sccvk`, `host/OLDCC.md` section 2). Differing lines of the disassembly with `vk` (shipped scc): sys1 82 (118), tt0 1 (4), tty 16 (14).
Each remaining difference is a code generator behaviour to be found as a new switch (witnesses: these objects):

1. switch layout (tty, `sioioctl` in dev/wega/siok.c): the original has the compares of a small switch behind the case bodies (the first one threaded to the
   top by popt); the shipped scodgen puts them in front.
2. registers (all three objects): tt0 (`ld r3,r5` before `ldk r2,#0` in a long conversion; swapping the template 037 breaks sig, sys2, sys3), tty (`ld r4,...`
   against `ld r2,...`), sys1 (82 lines; frame 0x1C0 against 0x1C2 and the count register of `cpir` r3 against r6 in `getxfile`: the switch value is loaded into r3
   first, which keeps the name of the frame slot, the original takes the value straight into the TEMP r2). Source forms tried without effect (about 40): casts, operand
   order, `+ 0`, `& 0xffff`, other types for `chg`/`nseg`, `switch (magic)` in six forms, `if (nseg > NUSEGS)` in six forms, `sysinfo.outch` in four forms. Existing switches
   on top of vk: only D2L+D2T change sys1 (82 to 37) and worsen tt0 (1 to 24), D2P/D2F/D2R/D3K/D1R/D2N: no use. Method for a new switch: the debug output of
   `getreg`, `rsreturn`, `nxwork` and the table entry in a scratch copy (see OLDCC_LOG.md, vk D27). New switches only as the last resort (owner).
3. After each new switch: `host/recon/oldcc.sh matrix check` (it reports `cas`, `cc` without decision and changed pass files since 3ea37915 and a1e60f58 already
   before this work), tables in `host/OLDCC.md`, entry in `host/OLDCC_LOG.md`.

## 4. Assembler sources that should be C

Rule (owner): a `.s` whose header names a "Bearbeiter" other than O. Lehmann is an original assembler source and stays assembler. Candidates for C (check each
against the object): `sys/lock.s` (README: 9 functions `lkdata unlk locked deadlock unlock lockalloc lockfree lockadd lkfront`, SYSIII `lock.c` as the base) and
`dev/clist.s` (O. Lehmann, C-ASM; SYSIII `clist` in `tmp/sys3`, traces of the C compiler in the object).

## 5. Text files of the root

1. `usr/sys/dev/*.c`, `usr/sys/h/*.h`, `usr/sys/conf/{make.boot,make.wega,ver.c+,wpar.c}`: identical now; install them, status "todo" in `COVERAGE.md` disappears with the install target.
2. `bstart.c` (differs from the root file, the header comment): fix the file (it is installed, 666 wega).
3. `usr/sys/conf/wpar.o`: see 2.2; if it is only an old leftover, build it from the `wpar.c` of that time (find the source) or install the shipped bytes (as a documented binary `.obj` like `u.obj`).

## 6. The archives (last, after the objects are identical)

1. `LIB1`, `LIB2`, `BOOTLIB`: contents are the objects; the headers (owner, group, mode, time) are set afterwards with `fix` as for the libc (`src/lib/libc/arhdr.s`).
   Needs the member list with the shipped headers per archive; `chmog` calls in `dev/Makefile` are then not needed.
2. Compare the archives member by member first (`ar x` of the original, `cmp`), then as a whole.

## 7. At the end

1. Connect `uts` to `install` in `src/Makefile`; `checkall.sh -b`: `boot`, `wega`, `usr/boot`, `usr/sys/...` all identical, nothing in OPEN for uts.
2. Update `README.md` (what is still object/assembler: should be only `u.obj`, if any), `COVERAGE.md`, the tables in `host/OLDCC.md` and `host/KNOWLEDGE.md`, `HANDOFF_wega_disasm.md`.
