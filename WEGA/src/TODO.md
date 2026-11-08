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
- The other files of `/lib` (`crt0.o` is built from `libc/csu/crt0.s` and installed): `scrt0.o`, `mcrt0.o`, `smcrt0.o` (PLZ/ASM
  modules like `crt0.o`), `cnvt.o`, `scnvt.o`, `doprnt.o`, `sdoprnt.o` (not the members of `libc.a`), `slibc.a`: no source yet.

## 7. Host tool chain

Every program of the inventory (`host/HANDOFF_wega_disasm.md`) with WEGA status "exact" gets a host version: ANSI C, compiles with a recent gcc without warnings (`-Wall -Wextra`, no `-w`), behaves like the WEGA original. Excluded: `libc`, `cas`, all `*crt0*`. Programs: ar, ranlib, strip, size, cpp, cparse and scparse, codgen and scodgen, popt and spopt, ld, sld, cc and scc.

Method: the sources are taken over anew from `src/cmd/X` (the exact, byte-identical state), not synchronised by diff; `findings.json` is no source for what changed.

1. Collect what exists only in the host version before it is replaced: fixes of defects of the original (quirks table in `host/KNOWLEDGE.md`, "kept"/"fixed"), settings of the host (`cc.conf`, `ld.conf`), tests and checks. None of it may get lost.
2. Take the sources of `src/cmd/X` into `host/X`, as close as possible: same file, function and variable names and order, so that `diff src/cmd/X host/X` shows exactly the port. Allowed changes: ANSI prototypes, headers, and what Z8000 to 64 bit needs (3.). Programs of `src/cmd` that are one file (`ar.c`, `ranlib.c`, `strip.c`, `size.c`) the same way. `scparse`, `scodgen`, `spopt`, `sld`, `scc` from the same source with `-DSEG`/`-DSCC` as in `src`.
3. Z8000 to 64 bit (host int is 32 bit, long 64 bit, char may be unsigned, little endian):
   - Always: fixed width types (`int16_t`, `uint16_t`, `int32_t`, ...) for everything that is a data format (object files, archive headers, symbol tables, relocations) and for `long`; binary files are read and written field by field, big endian, never a struct by `fread`/`fwrite`; pointers kept in an `int` become `intptr_t` or an index; `char` signedness explicit (`-fsigned-char` or a type).
   - Plain computation with `int` stays `int` at first. Built with `-fsanitize=undefined,address` and `-ftrapv`, run with the corpora: where a 16 bit overflow, a sign extension of a `short`/`char` or a shift beyond 16 bit changes the result, that place gets `int16_t`/`uint16_t`. Each such place is recorded (evidence) in `host/KNOWLEDGE.md`.
   - `printf` formats with `long`, `sizeof` and struct layout assumptions, `strcpy` overlaps and similar are checked on every program.
4. cparse needs a host yacc (bison `-y` or byacc); the tables differ from the WEGA yacc, so inputs with syntax errors belong to the corpus.
5. Order: first a small pilot (`ar`) to settle the recipe, shown to the owner; then ranlib, strip, size, cpp, ld, sld, cc, popt, codgen, cparse. One program per commit.
6. Check per program, against the original under `z8k-emu`, by the corpus only (`host/tools/corpus.sh`, candidates `wega` and `host` of `compare/NAME/progcfg.py`, see `host/HANDOFF_wega_disasm.md`, "Check of a program"); the `check.sh` and `check.py` of the host directories and their `check` targets are converted into corpus cases and removed (done: `ar`, `cpp`, binutils, `cc`; open: cas, popt, ld, libc), proved by deliberately broken builds; for the compiler passes also error cases. Codgen, cpp, cparse get a `Makefile` and an entry in `host/Makefile` (`all`, `check`) and in `host/tools/checkall.sh`.
7. The driver: `host/cc/cc.conf.example` points to the host cpp, cparse, codgen instead of the emulator; end-to-end: the same programs compiled with the host chain and with the originals under `z8k-emu`, the objects compared.
8. Finish: inventory table (column "Host source") and `host/KNOWLEDGE.md` current, `make -C host checkall` including the new programs.

## 8. Clean up

To look at: codgen, scodgen and spopt built with the shipped cc and shipped libc plus the old sprintf (stage 1) differ from the originals on the corpora (47, 46, 79 cases; with `libc-v5.a` 0, spopt 2, the `NK core` line). The final programs are identical; which module causes it is not known.

Switches not in any version (`OLD_D1F`, `OLD_D2D`, `OLD_D3A`, `OLD_D19A`); switches with one witness (`OLDCC.md`, section 5); whether v0 and v0p are two versions; whether v1 had the old cas. `KNOWLEDGE.md`, `HANDOFF_*`, the table in section 4 of `OLDCC.md`.

Host side: `host/recon/castrig` (`hostcas.patch`, `triggers.txt`), `host/libc/check.sh` and `host/oldcc` (now doubled by
`checkall`), the reconstruction scripts of `host/recon` that only served the cutting of objects.

## 8a. Sources without the old libc

For building everything with the shipped cc and the shipped libc later: the sources still need the libc of their time
(measured 2026-11-07 with the corpus, shipped cc and shipped libc): popt uses the value of `sprintf` (the buffer; the
shipped one returns a count), codgen meets the `%s` defect of the shipped `printf` (KNOWLEDGE.md, quirks), ld calls
`swap_quad`, which only the old `swap.o` (`oldlibc/swap/0_1.s`) has. Changing them changes the bytes of the identical
builds: only when the development line leaves them.

## 9. New programs

`gld` and everything else under `bin/`, `lib/`, `usr/bin`, ... `host/OLDCC_DATES.md` lists all programs with their time.

## 10. Last, when all other binaries stand

- `/bin/cas`: built by an older scc that we do not have.
- `/bin/as`, `/lib/as*`: PLZ/ASM. All of PLZ: `/usr/bin/plzsys`, `plz`, `/usr/lib/plzcg*`. To be learned from the literature of the owner first.
