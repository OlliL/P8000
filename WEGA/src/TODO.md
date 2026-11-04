# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`tools/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (13 minutes): the list of programs is in `host/tools/checkall.sh`. Run it after every change of a compiler and before a commit of a source of a program.

## 2. `lib/libc` from C and byte-identical

A table per C source: which compiler (shipped, ccv1, ccv0, ccv0p) gives which object. An `.s` in `oldlibc` or `libc/oldcc` that can be made from C goes. `stdio/getgr*.c` are older copies of `gen/getgr*.c`: decide and remove. No `#ifdef` in the libc; what cannot be made stays `.s`.

## 3. Real libraries and `make` under `WEGA/src`

- `libc-vXXX.a` from the versions in `oldlibc` and the members of the current libc; a list of modules per version. Nothing built goes into git.
- `make` under `WEGA/src` builds everything in the right order and version: the old compilers (shipped cc, shipped libc), then `lib/`, then `cmd/`.
- The Makefiles of the programs become small: the real compiler at `CC =`, their libc, few targets. No `.s` and `ar x` rules any more.
- Watch: from an archive ld only pulls what is referenced; codgen holds perror and errlst though its live code does not call them.

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
