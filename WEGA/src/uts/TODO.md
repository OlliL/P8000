# uts: TODO

Goal: `make install` in `src/` writes `boot`, `wega`, `usr/boot` and `usr/sys/...` below PREFIX, and `host/tools/checkall.sh`
finds every one of them byte-identical (`cmp`) with the WEGA root. Everything below is ordered towards that goal.
Rule: finished (sub)items are deleted from this file, so it gets shorter with every step.
`uts` is built by `all` in `src/Makefile` but NOT by `install` (and not added to `checkall.sh`) until `wega`, `LIB1`, `LIB2` and the rest are
identical: before that `checkall.sh` would stop at the first difference. The install targets in `uts/*/Makefile` are written and tested by hand
with `make install PREFIX=dir`; they are connected to `src/Makefile install` at the very end (7.).
State measured with the build in a copy of `src/uts` (tools from `checkall.sh`), objects against `tmp/wega31_kernel`.

## State

- LIB1: 22 of 28 members identical. Different: `sig`, `sys1`, `sys2`, `sys3`, `debug`, `ureg` (7 bytes, one register).
- LIB2: 23 of 25 identical. Different: `tt0` (7 bytes), `tty`. `mt`, `rd`, `udev1..6` are dummies from `*.d.s` (identical).
- conf: `conf.o`, `event.o`, `fpe.o`, `mch.o`, `u.o`, `bstart.o` identical; `boot` identical; `BOOTLIB` same size, 63 bytes differ (archive headers).
- `wega` links, but 96628 of 97282 bytes: the shipped `wega` has the symbols `_ditab _tabeint _ptab1 _ptab2 _itab1 _btab _stab _pctab2 _mstab`
  (data of `siok`), ours has `_sioxout` instead, although our `siok.o` equals the one in LIB2.
- Not built / no source yet: `usr/boot`, `usr/sys/conf/wpar.o` (root copy is from an older `wpar.c`: no `cpu2_flag`, version " 3.0 (4)"),
  `usr/sys/h/*.h` and `usr/sys/dev/*.c` are only copied (identical to the root files), `bstart.c` differs from the root file.

## 1. Build and install (no byte work, unblocks the measuring)

1. Rules for `.s` with `as -u` / `cas -u` in `conf/Makefile` and `dev/Makefile` (the built-in rule has no `-u`): `koppel`, `disk`, `mch`, `event`, `fpe`, `bmch`, `brm`, `bud`, `sa.function`.
2. `conf/Makefile`: `ver.c` from `ver.c+` by the `make` rule `.c+.c` (`chkout`); the date is the time of the checkout, so reproducible only with
   the emulator and `Z8K_TIME=605545021` (10.03.1989 09:57:01 guest time, EST). Decide: set it in the Makefile (as a comment on the build host) or
   in `host/tools/wbin.sh` for `chkout`. `chkout` is no host tool (HANDOFF inventory); the build needs the emulator as the oracle.
3. Build `wega` (`sld -Ns -o wega -e start -X -i -Ms62 wpar.o event.o mch.o u.o ver.o fpe.o conf.o ../sys/LIB1 ../dev/LIB2`) and `boot` (`ld -s -e start -b 0x8000`) in `conf/Makefile`; `all` makes them.
4. Install targets for `boot`, `wega`, `usr/boot` (PREFIX), `usr/sys/conf`, `usr/sys/dev`, `usr/sys/sys`, `usr/sys/h`; plain copies, no chmog (see 6.).
   The Makefiles must work with WEGA's `make` (`=`, `cd dir ; $(MAKE)`), style as `src/cmd/*/Makefile`; the top `uts/Makefile` joins `src/Makefile` by its install target.
5. `checkall.sh`: nothing to add (a file joins by its install target); remove entries from OPEN only when identical. Regenerate `COVERAGE.md` (`host/tools/coverage.py`).
6. Delete from all Makefiles every step that only makes the objects or files look like the original: `chmog`/`chmod`/`touch` before `ar` and in `install`
   (`dev/Makefile`: `chmog 666 26 1 koppel.o fd.o md.o disk.o`, `chmog 644 89 0 tty.o`; `conf/Makefile`: `chmod 644 libb.a`, `chmog ... bstart.o`;
   the `chmog` of `install` in all five). `chmog` does not exist on the host and the build does not run as root. The member headers (owner, group, mode, time)
   are set afterwards by the `fix` of `arhdr.s` as for the libc (6.1); for installed files the owner/mode/time are not compared by `cmp`, the
   install uses `cp`/`install` as `src/cmd/*/Makefile`. Facts to carry into the archive header table: `koppel`, `fd`, `md`, `disk` = mode 666, owner 26, group 1; `tty` = 644, 89, 0; the rest as measured in the shipped archives.

## 2. The links of `wega` and `boot` (what decides 100% of two files)

1. Find the origin of the `siok` data of the shipped `wega` (`_ditab ... _mstab`, `_sioxout` missing): compare the `siok` part of `wega` with `siok.o`
   (`nm -n`, `sdis.py`); is `siok.o` in the root LIB2 a different build than the one linked into `wega`? If yes: reconstruct that `siok.c`
   (variables `static` in ours are global there?) and decide what LIB2 gets.
2. `wpar.o`: the root copy is from `wpar.c` 3.0; the `wega` was linked with another `wpar.o`. Compare the `wpar` part of `wega` with ours (data `_md_flag` ...).
   Then either `wpar.c` is right (check `wpar.o` against the `wega` part) or there is another `wpar.c` version (`git log` on GitHub master).
3. After 1 and 2: the remaining differences of `wega` are the objects of 4.; the link order and the symbol table (`-X`, `-i`, `-Ms62`) are proven by the identical parts.
4. `usr/boot`: not located yet. Find which program/Makefile makes it (`make.boot` makes `boot` from `bstart.o` + `BOOTLIB`; `usr/boot` is likely the same or a sibling, D12 new?). Compare with `boot`.
5. `u.o`: still a binary `u.obj` copied (`$ABS` works only in segment 1; `u` must lie at segment 62, `0xf600`). Target: `u.s`/`u.c` that gives the same bytes (README). Until then `u.obj` stays and is documented.

## 3. Objects that differ (ordered by effort)

1. `ureg.o` (7 bytes): in `segureg`, `caddr += u.u_segmts[i].sg_limit+1` uses r2/r3, the original r3/r4 (r2 stays reserved in the whole branch).
   17 source variants and `sccv1` tried without success; next: other statement order, a different type for `caddr` (`caddr_t`, `unsigned`), a second `register` variable, an older SYSIII `ureg.c`.
2. `tt0.o` (7 bytes): `sysinfo.outch++` in `tt0.c` (FIXME in the source).
3. `sig.o`: FIXME `ipc.ip_addr.l = (caddr_t)((long)uap->addr.l & 0x7F00FFFF)` (size 6824 vs 6820).
4. `sys3.o`, `sys2.o`, `sys1.o`: FIXME comments in the sources; sizes differ by 4 to 8 bytes (nearest-version method: SYSIII `sys1.c` ... `sys4.c`, then `cmpins.py`).
5. `debug.o` (6868 vs 6836, no FIXME) and `tty.o` (10194 vs 10186, no FIXME): find the first differing instruction with `sdis.py` + `diff`.
6. For all: also test which compiler made the original (`scc`, `sccv1`, ... ; the ZEUS-origin files may be older); record the result per object in the table "Objects and their differences" in `host/OLDCC.md` and the quirks in `host/KNOWLEDGE.md`.

## 4. Assembler sources that should be C

Rule (owner): a `.s` whose header names a "Bearbeiter" other than O. Lehmann is an original assembler source and stays assembler.
Checked by the headers:

- Original assembler, stays: `dev/disk.s`, `mt.d.s`, `rd.d.s`, `udev1..6.d.s` (P. Hoge), `dev/koppel.s` (U. Wiesner), `conf/bmch.s`, `brm.s`, `bud.s`,
  `sa.function.s` (P. Hoge), `conf/event.s` (U. Wiesner, P. Hoge), `conf/mch.s` (J. Zabel, P. Hoge).
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
