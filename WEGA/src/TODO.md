# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`tools/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (14 minutes): the list of programs is in `host/tools/checkall.sh`. Run it after every change of a compiler and before a commit of a source of a program. Everything builds with `make` under `src/`: old compilers with the shipped tools and libc, `lib/`, old compilers again with the libc of their time, `cmd/` (7 minutes; `make -C host src` on Linux, `host/tools/wbin.sh` puts the tools under the emulator in the PATH).

## Where we stopped (2026-11-04)

Done: real libraries; v0 corrected against libc-v1 and dc, new compiler v1q (2026-11-05). `src/lib/libc-v1.a` .. `libc-v9.a` (`lib/oldlibc/Makefile`) from C where a compiler gives the bytes, every program links `LIBC`, no more single objects. `/lib/libc.a` rebuilt from source, all members compared. Next: finish step 2 with the compilers it needs, then step 4 and 5.

To look at: codgen, scodgen and spopt built with the shipped cc and shipped libc plus the old sprintf differ from the originals on the corpora (47, 46, 79 cases; 0 with `libc-v5.a`, 2 for spopt, the `NK core` line), and stage 1 is built that way. The final programs are identical, so it does no harm so far; which module causes it is not known.

Fact to keep in mind: the old compilers of stage 1 use shipped `libc.a` plus only `sprintf` of the old printf, because `popt` (`z23.c`) uses the value of sprintf, the buffer; the shipped one returns a count, and popt then silently loses its register substitution.

## 2. `lib/libc` and `oldlibc` from C, what is left

`/lib/libc.a`: 175 of 179 members from source. `oldlibc`: 71 of 84 modules from C. The other `.s` wait for a compiler or a source form (`host/tools/libcfit.py`, `libctable.py`; what was tried: `host/OLDCC_LOG.md`):

- `libc/oldcc`, a compiler between v1 and shipped: `malloc` 3_3 (`mult rr2,r6` in calloc), `swap` (`ld 0(r15),r7` where the shipped cc writes `@r15`; longswap has the source form now), `symscan` (r3, r4 where the shipped cc takes r2, r3), `nlist` (frame 2 bytes less). One witness each.
- With v1q (size): `doprnt` 0_1, `endopen` 0_1 (the value of an `&&` through the temporary).
- With v0: `fseek` 0_2 (one statement: the registers at the label of an else, `OLD_D3J` fits it and not dc), `doprnt` 0_3.
- Libc of the passes: `malloc` 0_3 (a parameter reloaded at a label, the order of `q+temp-1`), `cnvt` 0_2 (`-val` of a long parameter from memory, one witness), `doprnt` 0_5, 0_7 (`_align` spills r5, `ap = argp` reloads, the order of a double argument), `fltioc` 0_1.
- Others: `swap` 0_1, `malloc` 0_4.
- Assembler in the original, these stay `.s`: `open` 0_1, `ecvt` 0_1.

## 4. Clean up `tools/`

Gone: the cutting of objects, `.link`, `.spec`, `gen_makefile.py` of cpp and ld. Still to look at: `ld/tools` and `sld/tools` (the rest of what worked with cut modules; `sld/tools/gen_makefile.py`, `lib/oldslibc` and `unlink_seg.py` belong to the segmented programs, `slibc.a` was not looked at yet: probably the same way), `castrig/hostcas.patch`, `triggers.txt`, `HANDOFF_ld.md`, `HANDOFF_cparse.md`. `host/libc/check.sh`, `host/oldcc`: now doubled by `checkall`.

## 5. The open programs in `cmd/` byte-identical

In this order; every change of a compiler goes through `checkall`.

- `write`: own code exact with v1, libc part open.
- `rmdir`: main differs (D4, D18), libc part open.
- `dc`: 2 places in the own code with ccv0 (scalint reloads a parameter, one `call`), libc part fits.
- `ranlib`: own code differs (v-version to be found); `readsym` is an old libc module.
- `doscopy` (8 commands): differences not analysed; built with the current cc and libc, the owner believes. The build stops on the `/usr/include` dependencies outside the guest ROOT. The owner began this on the branch `doscopy`: inspect it first for changes to take over.
- `pburn`: same size, built with the shipped cc; differences not analysed.
- `ld`: 55 of 59 with v1 (open: place_sec, chk_ranlib, load2td, read_symbols), library `libc-v9.a`. Decide the source base first.
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
