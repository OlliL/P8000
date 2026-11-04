# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`tools/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (13 minutes): the list of programs is in `host/tools/checkall.sh`. Run it after every change of a compiler and before a commit of a source of a program.

## 2. `lib/libc` from C, what is left

`/lib/libc.a`: 174 of 179 members from source; open `fltioc` (no C yet), `malloc`, `nlist`, `swap`, `symscan` (`KNOWLEDGE.md`, libc check). `oldlibc`: 54 of 84 modules from C (`oldlibc/SOURCES`); to do `fltioc`, `malloc` (5 versions), `ecvt`, `endopen`, `rdwr`, `filbuf`, `fseek`, `perror`, `errlst`. `stdio/getgr*.c` are older copies of `gen/getgr*.c`: decide and remove.

Waits for a compiler change (step 5): the modules of ranlib and dc (`isatty`, `stty`, `readsym`, `system`, `ftell`, `printf` 0_3, `swap` 0_2, `doprnt` 0_3): the original has no `ld r2,r7` or reload where v0 has one, as in dc itself (guess: one cause, not measured). `doprnt` 0_5, 0_7: `_align` spills r5, `ap = argp` reloads, a switch of 4 cases over a range of 14 as `cpir`, the order of a double argument.

## 3. Real libraries and `make` under `WEGA/src`

- `libc-vXXX.a` from the versions in `oldlibc` and the members of the current libc; a list of modules per version. Nothing built goes into git. The .s of a module in `oldlibc/SOURCES` goes when no Makefile names it (`mkoldlibc.py`, called by `checkall`).
- `make` under `WEGA/src` builds everything in the right order and version (owner 2026-11-04): the old compilers including cas with the shipped tools and the shipped libc; then the libraries with them; then the old compilers once more in the right order, now with the right libc; then `cmd/`.
- The Makefiles of the programs become small: the real compiler at `CC =`, their libc, few targets. No `.s` and `ar x` rules any more.
- From an archive ld only pulls what is referenced; codgen holds perror and errlst though its live code does not call them: find the reference (no ld links an unreferenced object). A real `libc-vXXX.a` is the goal. Only if a program was never linked from a library: `libc-vXXX/` as a directory with C sources, `.s` where needed and objects, the link takes the objects from there.

## 4. Clean up `tools/`

The programs that are identical keep the corpus and the comparison. `gen_makefile.py`, the cutting of libc objects and the like go; also `castrig/hostcas.patch`, `triggers.txt` and the unused directories of `lib/oldslibc`.

## 5. The open programs in `cmd/` byte-identical

In this order; every change of a compiler goes through `checkall`.

- `write`: own code exact with v1, libc part open.
- `rmdir`: main differs (D4, D18), libc part open.
- `dc`: 28 places in the own code with ccv0, libc part fits.
- `ranlib`: own code differs (v-version to be found); `readsym` is an old libc module.
- `doscopy` (8 commands): differences not analysed; built with the current cc and libc, the owner believes. The build stops on the `/usr/include` dependencies outside the guest ROOT. The owner began this on the branch `doscopy`: inspect it first for changes to take over.
- `pburn`: same size, built with the shipped cc; differences not analysed.
- `ld`: 55 of 59 with v1 (open: place_sec, chk_ranlib, load2td, read_symbols). Decide the source base first.
- `sld`: 58 of 90 functions, corpus 51 of 51. Decide the source base first.

## 6. System III form for all existing programs

The source follows the System III sources as far as the bytes allow: names, macros, comments, order. System III is primary; for what is specific to the Z8000 the Plexus sources are the model (owner 2026-11-04). Bytes stay equal, checked by `checkall`. First find out what System III holds for cparse, codgen and popt. Lists of what is WEGA's: `tools/PCC.md` of codgen, cparse, popt, to be redone against System III. Also the odd forms in findings.json (the `top:` labels of cpp).

## 7. Host tool chain

Take over the changes of the sources where they change behaviour. Make the missing programs from `cmd`: cpp, cparse, scparse. Checked by the corpora against the originals.

## 8. Clean up

Switches not in any version (`OLD_D1F`, `OLD_D2D`, `OLD_D3A`, `OLD_D19A`); switches with one witness (`OLDCC.md`, section 5); whether v0 and v0p are two versions; whether v1 had the old cas. `KNOWLEDGE.md`, `HANDOFF_*`, the table in section 4 of `OLDCC.md`.

## 9. New programs

`gld` and everything else under `bin/`, `lib/`, `usr/bin`, ... `host/OLDCC_DATES.md` lists all programs with their time.

## 10. Last, when all other binaries stand

- `/bin/cas`: built by an older scc that we do not have.
- `/bin/as`, `/lib/as*`: PLZ/ASM. All of PLZ: `/usr/bin/plzsys`, `plz`, `/usr/lib/plzcg*`. To be learned from the literature of the owner first.
