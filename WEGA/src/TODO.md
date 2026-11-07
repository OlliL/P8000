# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`tools/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (14 minutes): the list of programs is in `host/tools/checkall.sh`. Run it after every change of a compiler and before a commit of a source of a program. Step 5 (the open programs in `cmd/`) is done; the owner decides on the next. Everything builds with `make` under `src/`: old compilers with the shipped tools and libc, `lib/`, old compilers again with the libc of their time, `cmd/` (7 minutes; `make -C host src` on Linux, `host/tools/wbin.sh` puts the tools under the emulator in the PATH).

## 2. `lib/libc` and `oldlibc` from C, what is left

`/lib/libc.a`: 175 of 179 members from source. `oldlibc`: 72 of 85 modules from C. The other `.s` wait for a compiler or a source form (`host/tools/libcfit.py`, `libctable.py`; what was tried: `host/OLDCC_LOG.md`):

- `libc/oldcc`, a compiler between v1 and shipped: `malloc` 3_3 (`mult rr2,r6` in calloc), `swap` (`ld 0(r15),r7` where the shipped cc writes `@r15`; longswap has the source form now), `symscan` (r3, r4 where the shipped cc takes r2, r3), `nlist` (frame 2 bytes less). One witness each.
- With v1q (size): `doprnt` 0_1.
- With v0: `fseek` 0_2 (one statement: the registers at the label of an else, `OLD_D3J` fits it and not dc), `doprnt` 0_3.
- Libc of the passes: `malloc` 0_3 (a parameter reloaded at a label, the order of `q+temp-1`), `cnvt` 0_2 (`-val` of a long parameter from memory, one witness), `doprnt` 0_5, 0_7 (`_align` spills r5, `ap = argp` reloads, the order of a double argument), `fltioc` 0_1.
- Others: `swap` 0_1, `malloc` 0_4, `doprnt` 0_8 (0_1 with a smaller frame in `_strout`, libc-v10).
- Assembler in the original, these stay `.s`: `open` 0_1, `ecvt` 0_1.

## 4. Clean up the host side

The `tools/` directories of the programs have their final layout (`host/recon/README.md`). Still to look at outside them:
`host/recon/castrig` (`hostcas.patch`, `triggers.txt`), `host/libc/check.sh` and `host/oldcc` (now doubled by `checkall`),
the reconstruction scripts of `host/recon` that only served the cutting of objects.

## 6. System III form for all existing programs

The source follows the System III sources as far as the bytes allow: names, macros, comments, order. System III is primary; for what is specific to the Z8000 the Plexus sources are the model (owner 2026-11-04). Bytes stay equal, checked by `checkall`. First find out what System III holds for cparse, codgen and popt. Lists of what is WEGA's: `tools/SYSIII.md` of codgen, cparse, popt (still against the PCC of Plexus), to be redone against System III; every program gets one. Also the odd forms in findings.json (the `top:` labels of cpp).

## 7. Host tool chain

Take over the changes of the sources where they change behaviour. Make the missing programs from `cmd`: cpp, cparse, scparse. Checked by the corpora against the originals.

## 8. Clean up

To look at: codgen, scodgen and spopt built with the shipped cc and shipped libc plus the old sprintf (stage 1) differ from the originals on the corpora (47, 46, 79 cases; with `libc-v5.a` 0, spopt 2, the `NK core` line). The final programs are identical; which module causes it is not known.

Switches not in any version (`OLD_D1F`, `OLD_D2D`, `OLD_D3A`, `OLD_D19A`); switches with one witness (`OLDCC.md`, section 5); whether v0 and v0p are two versions; whether v1 had the old cas. `KNOWLEDGE.md`, `HANDOFF_*`, the table in section 4 of `OLDCC.md`.

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
