# Handoff: slibc (`/lib/slibc.a`, `/lib/scrt0.o`)

State of 2026-11-12, branch `libs` (a second Claude session works on branch `toolchain` in another checkout on the
compilers; see "Working beside the other session").

## Goal

`/lib/slibc.a` (the libc of the segmented compiler `scc`) and `/lib/scrt0.o` built from sources, byte-identical with the
WEGA 3.1 tree, from C wherever possible. The sources started as a copy of `../libc` (a later merge into one tree with
`#ifdef` is wanted, therefore the libc form is kept where the bytes allow it). There is no cpp symbol that tells scc from cc
except `NULL` (`0L` in scc), so the segmented sources stay a directory of their own for now.

Read first: [`../../../CLAUDE.md`](../../../CLAUDE.md) (rules), [`../../../host/HOWTO_reconstruct.md`](../../../host/HOWTO_reconstruct.md)
(method, evidence rules for compilers), [`../../TODO.md`](../../TODO.md) (step 2).

## State

- `make` here builds `slibc.a` and `csu/scrt0.o`, both **byte-identical** (about 6 s); `make install PREFIX=dir`;
  `../Makefile` builds and installs it with libc. `checkall` has not run with slibc yet (owner: only when slibc is done,
  and never at the same time as the other session).
- 184 members + `Library.SC` (the SCCS list of the original, copied), in the order of the original. `arhdr.s` holds the
  member headers of the original (date, uid, gid, mode, size; generated from `/lib/slibc.a`), `mkar.sh` (from `../libc`)
  joins them; `Library.SC` is the only member of odd size, its padding byte is `0x34`.
- `Makefile` was generated (rules in archive order): `.c` with `$(CC) -O` (`scc`; printf with `-I../../../cmd/cparse` as in
  libc), PLZ/ASM with `$(AS)` (`-f` for atof, ecvt, pow10), cas sources with `$(CAS)`, `oldcc/NAME/0_1.s` with `$(CAS) -u`.

| Kind | Count | How |
|---|---|---|
| C with the shipped scc | 95 | identical |
| PLZ/ASM and cas | 81 | identical |
| `oldcc/NAME/0_1.s` (cut from the original) | 8 | nlist, scanf, printf, swap, segmon, malloc, doprnt, symscan |
| `csu/scrt0.s` | 1 | PLZ/ASM, identical |

### Differences to libc (for a later merge into one tree)

The forms that the segmented members showed and that keep libc identical (execvp, ctime, popen, system, fltioc) are in
`../libc` too (2026-11-12, `libc.a` built and compared: identical); those files are now equal in both trees. What differs on
purpose:

- C, the segmented calling convention (pointer arguments in rr6, rr4, rr2, then the stack): `stdio/doscan.c`
  (`++nreg == 2`, no spare local `unused` in `_instr`), `stdio/scanf.c` and `stdio/printf.c` (2 or 4 register arguments,
  `regs[5]`/`regs[3]`, the flag of `_doprnt` 1 and 3), `stdio/doprnt.c` (revision 2.8 against 2.9 of libc, `%s` a 4 byte
  pointer, see the open members).
- C, own for slibc: `gen/malloc.c` (the arena is a segment: `mkseg`/`ssgbrk`, BLOCK 2048, maxblocks 32, `INT long`),
  `gen/segmon.c` (instead of `mon.c`), `gen/symscan.c` (libc has no C source of symscan, only `../libc/oldcc/symscan`;
  whether this one fits libc is not measured).
- Assembler: every PLZ/ASM source has `$segmented` and segmented instructions; only in slibc: `sys/` chkseg, mkseg, sgbrk,
  sgstat, sprofil, ssgbrk, `csu/scrt0.s`; `crt/mcount.s` is another program; `sys/ulimit.s` another system call number;
  `gen/stfl.s` (cas) segmented; only in libc: `brk.s`, `mon.c`, `csu/crt0.s`.
- The headers of the sources are those of libc, except the files rewritten for slibc (their SCCS line is that of
  `Library.SC` of slibc).

Open question (a guess): libc's `int unused` in `_instr` (doscan) may be a `register` that gets a frame slot only in
nonsegmented code; `host/KNOWLEDGE.md`, "libc check against the original library".

### Assembler members

The libc sources with `$segmented` and the instructions of the original. New in `sys/`: chkseg, mkseg, sgbrk, sgstat,
sprofil, ssgbrk; `crt/mcount.s` is another program than libc's (`_ctbase`, `_cntno`); `gen/stfl.s` is cas;
`gen/atof.s`: the jump table is `LONG := label` and the state table bytes are doubled (offsets into 4-byte entries).
PLZ/ASM notes: `out` is a mnemonic (not a label), a segmented address constant is `LONG := label`, a reference to the
stack segment is `|_stkseg+n|(r15)` with `_stkseg` external.

Tools: `host/tools/s2as.py -s` (draft of a segmented member, `-n` module name), `host/tools/epu.py` (`@rrN` in segmented
mode). The scripts that merged the drafts into the libc sources are not kept (scratch); the result is in the sources.

### The 8 open members (`oldcc/`)

The C source next to each (`gen/`, `stdio/`) is the best known form; with the shipped scc these differences are left
(diff lines of the disassembly against the original):

| Member | Lines | Left | Existing switch that gives it | Too broad: loses |
|---|---|---|---|---|
| scanf | 44 | `a[k]` of a local array addressed directly (D5) | `OLD_D5G`: identical | popen (`p[1]` in `?:`), cnvt (`&buf[9]`), crypt, execvp, where the original computes the address |
| printf | 100 | D5, register choice | `OLD_D5G`: 41 | as above |
| swap | 32 | D5 (`a[1]` load), a parameter from the spill slot | `OLD_D5G`: 28 | as above |
| symscan | 12 | one block takes rr4 where the shipped scc takes rr2 (r2 still holds `n`) | `OLD_D3`: identical | getopt, doscan, fltioc, fseek (= what vk loses) |
| doprnt | 93 | the same register choice as symscan, two places | `OLD_D3`: 656 (worse) | |
| segmon | 61 | parameter taken from its register (`ldl rr2,rr6`), `++i` through a register (as `OLD_D16A`), long cast to pointer masked through a copy | `OLD_D3P` partly (no loss); `OLD_D24` + `OLD_D25` for the mask | |
| malloc | ~190 | setbusy/clearbusy masked through a copy, parameter registers | `OLD_D24` (codgen) + `OLD_D25` (scparse) | none alone |
| nlist | 8 | frame of `nlist()` 2 bytes smaller (also in libc, `../libc/oldcc/nlist`) | `OLD_D1R`: right frame, but `readby` 2 bytes too small | 78 members |

Conclusion (measured): no set of existing switches gives all members; slibc (sources 08/17/83, the shipped scc is from
09/83) was made by an older scc whose rules are narrower than `OLD_D5G`, `OLD_D3`, `OLD_D1R`. A new or narrower switch is a
change of codgen: owner's decision, the other session works on the compilers. Until then the members come from `oldcc/`.
Compiler terms: [`../../../host/OLDCC.md`](../../../host/OLDCC.md), section 2 (versions vk, vc), D1 (line ~173), D2, D5
(~282), D16 (~369), D24/D25 (~431); the switches and their meaning: `../../cmd/codgen/oldcc.h`.

Source details still open (guesses marked):
- doprnt (`doprnt.c 2.8 08/17/83`): the cases `O X U D` stand behind `lng:` (an odd order, found by trying all orders; popt keeps other copies otherwise).
- segmon: names are guesses (`struct segrange`, `struct seghdr`, `sp`); the use of `sp` (a pointer to `sh_scale`, set
  before `sh_seg` is stored) follows the code, the form may be another.
- symscan: names are guesses; quirk below.

## Quirks of the originals (in the table of `host/KNOWLEDGE.md`)

- `ulimit`: system call 65 in slibc (`nosys` in `src/uts/sys/sysent.c`), 54 in libc (`ioctl`). Kept.
- `symscan`: a long name is found by its length only (`while (*s++ && *t++ && *t)` compares no characters). Kept.

## How to measure

Directories (not in git): `tmp/tmp/slibc` (`orig/` the members of `/lib/slibc.a`, `lorig/` of `/lib/libc.a`, `dis/` the
disassembly), `tmp/tmp/wbin` (wrappers of the WEGA tools for the PATH, made by `host/tools/wbin.sh` with this checkout's
roots).

- One C member: compile in its directory with `scc -O -c NAME.c`, compare with `orig/NAME.o` (`cmp`; differences with
  `host/tools/sdis.py` on both and `diff`). The helper `tmp/tmp/slibc/c.sh NAME [flags]` does this and also compiles the
  same source with `cc` against the libc member (shows whether a form keeps libc identical).
- A cut source for `oldcc/`: `sdis.py -a`, the declarations in the section scc has open at their first mention (a `.comm` in
  data, the others in code), `.psec data` opened before the code, a segmented address pair in the data as `.long`
  (the cutter was `tmp/tmp/slibc/oldcut.py`).
- Trial compilers without touching the repository (`tmp/tmp/sccvl`): a copy of `src/`, the passes built there with any set
  of switches (`make -f ../../Makefile scodgen CC=cc LIBC=../../../../lib/libc-v5.a CFLAGS="-O -DSEG ..."` in
  `src/cmd/codgen/build/NAME`, scparse with `libc-v7.a`, spopt with `-DSPOPT`), copied into `lib/vk/` of an own root
  (`host/tools/oldroot.sh` on `tmp/wega31_root`, with `bin/sccvk`), then `z8k-emu -r ROOT /bin/sccvk -O -c` on the slibc
  sources. The cc of the emulator truncates long command lines: more than a few `-D` go into a header (the scratch
  `oldcc.h` includes `vlsw.h` under `-DOLD_VL`). Script: `tmp/tmp/sccvl/vl.sh NAME "codgen" "cparse" "popt" [files]`
  (results of the single-switch sweeps: `sweep.txt`, `dsweep.txt` there).

## Working beside the other session

- The old `tmp/oldroot` and `tmp/wbin` of this checkout point into the other checkout (`/tmp` of its WEGA root too). Use
  only own roots: `tmp/tmp/oldroot`, `tmp/tmp/vlroot`, `tmp/tmp/vlroot2` (made with `host/tools/oldroot.sh` on
  `tmp/wega31_root`) and `tmp/tmp/wbin`.
- Before running a script, check the fixed `/tmp` paths it uses; no changes to the compiler chain (`src/cmd/{cparse,codgen,popt,cas,cc}`)
  on this branch; no `checkall` without the owner.

## Next steps

1. Owner's decision on a narrower D5 / D3 / D1R (or a new version for slibc) in the compiler session; then measure the 8
   members again with it (`vl.sh`) and drop `oldcc/` for those that are identical.
2. `checkall` once with slibc (owner).
3. Later the merge of slibc into libc (section "Differences to libc").
4. The other start-up files and objects of `/lib` (`mcrt0.o`, `smcrt0.o`, `cnvt.o`, `scnvt.o`, `doprnt.o`, `sdoprnt.o`), `slibm.a` and the other `s*` libraries.
