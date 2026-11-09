# uts: TODO

Goal: `make install` in `src/` writes `boot`, `wega`, `usr/boot` and `usr/sys/...` below PREFIX, and `host/tools/checkall.sh`
finds every one of them byte-identical (`cmp`) with the WEGA root. Everything below is ordered towards that goal.
Rule: finished (sub)items are deleted from this file, so it gets shorter with every step.
`uts` is built by `all` in `src/Makefile` but NOT by `install` (and not added to `checkall.sh`) until `wega`, `LIB1`, `LIB2` and the rest are
identical: before that `checkall.sh` would stop at the first difference. The install targets in `uts/*/Makefile` are written and tested by hand
with `make install PREFIX=dir`; they are connected to `src/Makefile install` at the very end (6.).
State measured with the build in a copy of `src/uts` (tools from `checkall.sh`), objects against `tmp/wega31_kernel`.

## State

- Only one object differs: `tty.o` (LIB2), and there only `TCXONC` in `ttiocom` (section 3).
- `wega`: 97282 bytes like the original, about 9300 bytes differ, all because `tty.o` is 4 bytes short and moves what follows. Linked with the original `tty.o` as a test (copy it over `dev/tty.o`, rebuild `LIB2`, `LIB2W`, `wega`): `cmp` identical, so the link order, the symbol table (`-X`, `-i`, `-Ms62`) and all other members are proven. `boot` is identical.
- `BOOTLIB`: 63 bytes differ (archive headers). `wpar.o` in the root is from an older `wpar.c`.

## 1. Build and install

1. `COVERAGE.md`: regenerate (`host/tools/coverage.py`) when files become exact.

## 2. `u.o`

1. `u.o`: still a binary `u.obj` copied (`$ABS` works only in segment 1; `u` must lie at segment 62, `0xf600`). Target: `u.s`/`u.c` that gives the same bytes (README). Until then `u.obj` stays and is documented.

## 3. The object that differs: `tty.o`, `TCXONC`

Compiler `vk` (`sccvk`, `host/OLDCC.md` section 2). 11 differing lines of the disassembly (one is the size line). Look for the source form first: `sys1`, `tt0`, `sioioctl` and the removal of `OLD_D27` all came from the source, not from the compiler: the place of statements in or behind a `switch`, the number of `return`s, the block of a declaration, `!x` against `x == 0` of a long (entry 057b forgets the name of the operand, 054a with 043 keeps it), the order of the tails of a block, a mask `& 0xffffL`. Method: `sccvk -S` without `-O` shows the table entry of each line; `host/recon/poptrace.sh` shows popt.

1. Original: `L344: rl r2,#2; ldl rr4,table(r2); jp @rr4`, the four case bodies (common tail behind the last), `TCSBRK`, then `L3b0: ld r2,arg; cp r2,#3; jr ule,L344; jp default`. Ours: value, compare and table jump in one piece in front of the bodies, and the common tail behind the first body.
2. How popt works here (`z21.c`): `codemove` moves the dispatch to the `jpr` of the switch head when a jump stands in front of the dispatch (ours). Only the loop inversion puts a new label inside such a block: it needs `jp T` behind `T`, no jump in front of `T`, and within 24 nodes behind `T` a conditional jump to the label right behind the `jp`. So the raw code of the original should have `cp; cjp ugt,default` before a `jp` which `default` follows, behind `TCSBRK` (deduced from the code of popt, the source that gives it is not found).
3. Without effect: order of `TCXONC` and `TCSBRK`, place of `default` (first, middle, last), order of the cases, casts of the switch value, `return` for `break`, the form of the end of `TCGETA`, the form of `TCSBRK`; `spopt` with `OLD_D20` is worse. `case 3` without `break` moves the common tail to the last body (2 lines fewer), but it falls into `default`: not usable. With `default` first, or in front of `case 3`, there is no fall into it, but the tail stays behind the first body (46 lines).
4. `vk` still needs `OLD_D3` (without it sys1, sys2, ureg, tt0 differ) and `OLD_D24` with `OLD_D25` (sig, sys2, sys3, debug); seven forms of the mask `(caddr_t)((long)x & 0x7F00FFFF)` in sys3 gave nothing.
5. After each changed switch: `host/recon/oldcc.sh matrix check` (it reports `cas`, `cc` without decision and changed pass files since 3ea37915 and a1e60f58 already before this work), tables in `host/OLDCC.md`, entry in `host/OLDCC_LOG.md`.

## 4. Text files of the root

1. `usr/sys/dev/*.c`, `usr/sys/h/*.h`, `usr/sys/conf/{make.boot,make.wega,ver.c+,wpar.c}`: identical now; install them, status "todo" in `COVERAGE.md` disappears with the install target.
2. `bstart.c` (differs from the root file, the header comment): fix the file (it is installed, 666 wega).
3. `usr/sys/conf/wpar.o`: not linked into `wega` in this form (the `wpar` part of `wega` has no difference); build it from the `wpar.c` of that time (find the source) or install the shipped bytes (as a documented binary `.obj` like `u.obj`).

## 5. The archives (last, after the objects are identical)

1. `LIB1`, `LIB2`, `BOOTLIB`: contents are the objects; the headers (owner, group, mode, time) are set afterwards with `fix` as for the libc (`src/lib/libc/arhdr.s`).
   Needs the member list with the shipped headers per archive; `chmog` calls in `dev/Makefile` are then not needed.
2. Compare the archives member by member first (`ar x` of the original, `cmp`), then as a whole.

## 6. At the end

1. Connect `uts` to `install` in `src/Makefile`; `checkall.sh -b`: `boot`, `wega`, `usr/boot`, `usr/sys/...` all identical, nothing in OPEN for uts.
2. Update `README.md` (what is still object/assembler: should be only `u.obj`, if any), `COVERAGE.md`, the tables in `host/OLDCC.md` and `host/KNOWLEDGE.md`, `HANDOFF_wega_disasm.md`.
