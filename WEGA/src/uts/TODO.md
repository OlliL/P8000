# uts: TODO

Goal: `make install` in `src/` writes `boot`, `wega`, `usr/boot` and `usr/sys/...` below PREFIX, and `host/tools/checkall.sh`
finds every one of them byte-identical (`cmp`) with the WEGA root. Everything below is ordered towards that goal.
Rule: finished (sub)items are deleted from this file, so it gets shorter with every step.
`uts` is built by `all` in `src/Makefile` but NOT by `install` (and not added to `checkall.sh`) until `wega`, `LIB1`, `LIB2` and the rest are
identical: before that `checkall.sh` would stop at the first difference. The install targets in `uts/*/Makefile` are written and tested by hand
with `make install PREFIX=dir`; they are connected to `src/Makefile install` at the very end (7.).
State measured with the build in a copy of `src/uts` (tools from `checkall.sh`), objects against `tmp/wega31_kernel`.

## State

- LIB1: 22 of 28 members identical. Different: `sig`, `sys1`, `sys2`, `sys3`, `debug` (`ureg` is identical with sccvk).
- LIB2: 23 of 25 identical. Different: `tt0` (7 bytes), `tty`. `mt`, `rd`, `udev1..6` are dummies from `*.d.s` (identical).
- conf: `conf.o`, `event.o`, `fpe.o`, `mch.o`, `u.o`, `bstart.o` identical; `boot` identical; `BOOTLIB` same size, 63 bytes differ (archive headers).
- `wega` links, but 96628 of 97282 bytes: it contains other versions of `lp` and `siok` (2.1).
- Not built / no source yet: `usr/sys/conf/wpar.o` (root copy is from an older `wpar.c`: no `cpu2_flag`, version " 3.0 (4)"),
  `usr/sys/h/*.h` and `usr/sys/dev/*.c` are only copied (identical to the root files), `bstart.c` differs from the root file.

## 1. Build and install

Done: Makefiles in the style of `src/cmd` (`all`, `install PREFIX=`, `clean`), `.s` rules with `as -u`/`cas -u`, `ver.c` by `chkout`
(the wrapper in `host/tools/wbin.sh` fixes the date), `wega` and `boot` built in `conf/`, install into `PREFIX/{boot,wega,usr/sys/...}`
without `chmog`/`chmod` (the archive headers: 6.), `uts` in `all` of `src/Makefile`, not in `install`.

1. `COVERAGE.md`: regenerate (`host/tools/coverage.py`) when files become exact.
2. The `uts` target in `src/Makefile` has not been run through `checkall.sh -b` yet (it needs a full `make`); do it once.

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

The kernel objects are made by the shipped scparse and spopt with an older scodgen: version `vk` (`sccvk`, `src/cmd/cc`, `src/cmd/codgen/build/segvk`,
switch set in `codgen/oldcc.h` under `OLD_VK`; so far `OLD_D3`). `ureg.o` is identical with it (done, `sys/Makefile`). Measured with `vk` (differing
lines of the disassembly): sig 4, sys1 98, sys2 7, sys3 4, debug 19, tt0 1, tty 16 (shipped scc: 4, 118, 16, 4, 19, 4, 14); no other existing OLD_Dn switch
helps (swept alone and on top of D3). The differences are a few code generator behaviours, each to be found as a new switch (witness: the objects):

1. `(caddr_t)(((long)ptr) & 0x7F00FFFF)` (sig, sys2 x2, sys3, debug x3): the original copies the loaded pointer into a new pair and masks it there
   (`ldl rr6,rr4; and r6,#%7F00`, the SCONV template 040a of `table3_scodgen.c`), the shipped one masks in place. Source forms tried: ten, none changes it.
2. switch layout (tty, `sioioctl` in dev/wega/siok.c): the original has the compares of a small switch behind the case bodies (the first one threaded to the
   top by popt); the shipped scodgen puts them in front.
3. `andb rl6,#%0F` against `and r6,#%000F` (debug): byte operation for `& 0xf` of a char.
4. scratch register choice: tt0 (`ld r3,r5` before `ldk r2,#0` in a long conversion; `sysinfo.outch++` through rr6), tty (`ld r4,...` against `ld r2,...`), sys1 (98 lines).
5. After each new switch: `host/recon/oldcc.sh matrix check`, tables in `host/OLDCC.md`, entry in `host/OLDCC_LOG.md`.

## 4. Assembler sources that should be C

Rule (owner): a `.s` whose header names a "Bearbeiter" other than O. Lehmann is an original assembler source and stays assembler.
Checked by the headers:

- Original assembler, stays: `dev/disk.s`, `mt.d.s`, `rd.d.s`, `udev1..6.d.s` (P. Hoge), `dev/koppel.s` (U. Wiesner), `conf/bmch.s`, `brm.s`, `bud.s`
  `conf/event.s` (U. Wiesner, P. Hoge), `conf/mch.s` (J. Zabel, P. Hoge).
- Original assembler (PLZ/ASM `module` syntax, no header): `conf/fpe.s`, `conf/u.s`; stays. `u.s` is wrong in one point (segment 62, see 2.5).
- Reconstructed by O. Lehmann (candidates for C, check each against the object): `sys/lock.s` (no header, written as C-ASM; README: 9 functions
  `lkdata unlk locked deadlock unlock lockalloc lockfree lockadd lkfront` not yet in C, SYSIII `lock.c` as the base) and `dev/clist.s`
  (header O. Lehmann, 02.05.08; C-ASM; check the SYSIII `clist` in `tmp/sys3` and the object for C-compiler traces).

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
