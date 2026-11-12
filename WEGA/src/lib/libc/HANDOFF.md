# Handoff: libc (`/lib/libc.a`, `/lib/crt0.o`) and the libraries of the old programs (`../oldlibc`)

State of 2026-11-12, branch `libs` (a second Claude session works on branch `toolchain` in another checkout on the
compilers: no changes to `src/cmd/{cparse,codgen,popt,cas,cc}` here, no `checkall` without the owner; the rules of
[`../slibc/HANDOFF.md`](../slibc/HANDOFF.md), "Working beside the other session", hold here too).

## Goal

`/lib/libc.a` and `/lib/crt0.o` built from sources, byte-identical with the WEGA 3.1 tree, from C wherever possible; the same
for the libraries the old programs were linked with (`../libc-vN.a`, sources in `../oldlibc`).

Read first: [`../../../CLAUDE.md`](../../../CLAUDE.md), [`../../../host/HOWTO_reconstruct.md`](../../../host/HOWTO_reconstruct.md)
(evidence rules for compilers), [`../../../host/KNOWLEDGE.md`](../../../host/KNOWLEDGE.md) sections "libc check against the
original library", "oldlibc: libc versions ...", "The libraries of the old programs", "libc versions",
"Start-up files (crt0) and the libc part as building blocks"; [`../oldlibc/README`](../oldlibc/README).

## State of libc

- `make` here builds `libc.a` and `csu/crt0.o`, both **byte-identical** (checked 2026-11-12 after the last source change);
  `make install PREFIX=dir`. 179 members + `Library.SC`; `arhdr.s` (the member headers of the original) and `mkar.sh` join
  them in the order of the original.
- 175 members from source with the shipped tools: C with `cc -O` (printf with `-I../../../cmd/cparse`: the stdio.h with
  `char *_ptr`), PLZ/ASM with `as` (`-f` for atof, ecvt, pow10), cas sources with `cas`.
- 4 members from `oldcc/NAME/VER.s` (cas sources cut from the original, `cas -u`): malloc (`3_3`), nlist, swap, symscan.
- Forms found on the segmented members (slibc) are in: execvp, ctime, popen, system, fltioc (see
  [`../slibc/HANDOFF.md`](../slibc/HANDOFF.md), "Differences to libc"). The segmented members show types and declaration
  orders that the nonsegmented ones cannot tell (int and pointer are both 2 bytes); they are the best evidence for libc
  sources too.

### The 4 open members (`oldcc/`)

Each has a C source in `gen/` that the shipped cc compiles to all but the difference below. Each difference has a twin in
the segmented member of slibc, where the switches were measured (`../slibc/HANDOFF.md`, "The 8 open members"):

| Member | Left with the shipped cc | Twin in slibc | Hint (guess where marked) |
|---|---|---|---|
| symscan | one block: `ld r3,..; lda r4,..` where the shipped cc takes r2, r3 (r2 still holds `n`) | the same block (rr4 for rr2) | `OLD_D3` (registers kept at the join of `&&`) makes the slibc member identical but loses others; source `gen/symscan.c` (new, from slibc, 8 diff lines here) |
| nlist | the frame of `nlist()` 2 bytes smaller | the same | `OLD_D1R` (no temporary for the value of a return) gives this frame, but also takes 2 bytes from `readby` |
| swap | `ld 0(r15),r7` where the shipped cc writes `@r15` | a parameter from the spill slot, `a[1]` addressed directly (D5) | guess: `OLD_D18` (frame operand with offset 0 written `0(fp)`, a v0 switch); not measured |
| malloc 3_3 | `mult rr2,r6` (the parameter still in its register) against `mult rr2,0(r15)` in calloc | calloc: the same; setbusy/clearbusy masks | a parameter taken from its register (as `OLD_D3P`); guess, not measured on libc |

Conclusion so far: an older cc than the shipped one (libc and slibc sources of 08/83 .. 09/83, the shipped compilers from
09/83), with rules narrower than the existing switches. [`../../../host/OLDCC.md`](../../../host/OLDCC.md): section 2
(versions), D1 (frame), D2/D3 (registers), D5, D18; the switches in `../../cmd/codgen/oldcc.h`. A new or narrower switch
is a change of codgen: owner's decision.

## State of oldlibc (`../oldlibc`, `../libc-vN.a`)

72 of 85 modules from C. Which program uses which library: `host/KNOWLEDGE.md`, "The libraries of the old programs";
how a module is made: [`../oldlibc/README`](../oldlibc/README). Open (`.s`), waiting for a compiler or a source form
(`host/tools/libcfit.py`, `libctable.py`; what was tried: `host/OLDCC_LOG.md`):

- With v1q (size): `doprnt` 0_1.
- With v0: `fseek` 0_2 (one statement: the registers at the label of an else, `OLD_D3J` fits it and not dc), `doprnt` 0_3.
- Libc of the passes: `malloc` 0_3 (a parameter reloaded at a label, the order of `q+temp-1`), `cnvt` 0_2 (`-val` of a long
  parameter from memory, one witness), `doprnt` 0_5, 0_7 (`_align` spills r5, `ap = argp` reloads, the order of a double
  argument), `fltioc` 0_1.
- Others: `swap` 0_1, `malloc` 0_4, `doprnt` 0_8 (0_1 with a smaller frame in `_strout`, libc-v10).
- Assembler in the original, these stay `.s`: `open` 0_1, `ecvt` 0_1.

## How to measure

- A C member: `cc -O -c NAME.c` in its directory (with this checkout's tool wrappers, `host/tools/wbin.sh`), `cmp` with the
  member of `/lib/libc.a` (`host/tools/war.py extract`), differences with `host/tools/sdis.py` on both and `diff`.
- The whole library: `make clean; make; cmp libc.a <WEGA_ROOT>/lib/libc.a` (about 10 s).
- Old compilers: `ccv1`, `ccv1q`, `ccv1r`, `ccv0`, `ccv0p` from an own OLDROOT (`host/tools/oldroot.sh` on this checkout's WEGA
  root, never the `tmp/oldroot` that points into the other checkout). A switch set without the repository: the trial tree
  of `../slibc/HANDOFF.md`, "How to measure" (there for scc; the same with `codgen` instead of `scodgen` and `ccv1r` or
  another nonsegmented driver).

## Next steps

1. Measure the 4 open members with single switches (nonsegmented: the trial tree with `codgen` and an old driver);
   together with the slibc findings, a proposal for the compiler session (owner).
2. The other files of `/lib`: `mcrt0.o` (PLZ/ASM like `crt0.o`), `cnvt.o`, `doprnt.o` (not the members of `libc.a`); the
   segmented ones (`smcrt0.o`, `scnvt.o`, `sdoprnt.o`) belong to slibc.
3. oldlibc: the open modules above.
4. Later: libc and slibc in one tree (`../slibc/HANDOFF.md`, "Differences to libc").
