# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`compare/NAME/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (16 minutes): every file that `make install` in `src/` puts below an empty root is compared with the WEGA tree; a program joins by its install target (stripped by `install` when the original is). Run it after every change of a compiler and before a commit of a source of a program. Step 5 (the open programs in `cmd/`) is done; the owner decides on the next. Everything builds with `make` under `src/`: old compilers with the shipped tools and libc, `lib/`, old compilers again with the libc of their time, `cmd/` (7 minutes; `make -C host src` on Linux, `host/tools/wbin.sh` puts the tools under the emulator in the PATH).

## 2. `lib/libc` and `oldlibc` from C, what is left

`/lib/libc.a`: 175 of 179 members from source. `oldlibc`: 72 of 85 modules from C. The other `.s` wait for a compiler or a source form (`host/tools/libcfit.py`, `libctable.py`; what was tried: `host/OLDCC_LOG.md`):

- `libc/oldcc`, a compiler between v1 and shipped: `malloc` 3_3 (`mult rr2,r6` in calloc), `swap` (`ld 0(r15),r7` where the shipped cc writes `@r15`; longswap has the source form now), `symscan` (r3, r4 where the shipped cc takes r2, r3), `nlist` (frame 2 bytes less). One witness each.
- With v1q (size): `doprnt` 0_1.
- With v0: `fseek` 0_2 (one statement: the registers at the label of an else, `OLD_D3J` fits it and not dc), `doprnt` 0_3.
- Libc of the passes: `malloc` 0_3 (a parameter reloaded at a label, the order of `q+temp-1`), `cnvt` 0_2 (`-val` of a long parameter from memory, one witness), `doprnt` 0_5, 0_7 (`_align` spills r5, `ap = argp` reloads, the order of a double argument), `fltioc` 0_1.
- Others: `swap` 0_1, `malloc` 0_4, `doprnt` 0_8 (0_1 with a smaller frame in `_strout`, libc-v10).
- Assembler in the original, these stay `.s`: `open` 0_1, `ecvt` 0_1.
- `slibc/slibc.a` and `slibc/csu/scrt0.o` are built byte-identical; 8 of the 184 members still from `slibc/oldcc/NAME/0_1.s`
  (nlist, scanf, printf, swap, segmon, malloc, doprnt, symscan): an older scc (D5, D24 with D25, parameter registers; trials with
  existing switches in a scratch tree) or a source form (doprnt 2.8, symscan). `slibc/README`.
- The other files of `/lib` (`crt0.o` is built from `libc/csu/crt0.s` and installed): `mcrt0.o`, `smcrt0.o` (PLZ/ASM
  modules like `crt0.o`), `cnvt.o`, `scnvt.o`, `doprnt.o`, `sdoprnt.o` (not the members of `libc.a`): no source yet.

## 9. New programs

`gld` and everything else under `bin/`, `lib/`, `usr/bin`, ... `host/OLDCC_DATES.md` lists all programs with their time.

## 10. Last, when all other binaries stand

- `/bin/cas`: built by an older scc that we do not have.
- `/bin/as`, `/lib/as*`: PLZ/ASM. All of PLZ: `/usr/bin/plzsys`, `plz`, `/usr/lib/plzcg*`. To be learned from the literature of the owner first.
