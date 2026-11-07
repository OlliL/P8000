# popt against System III and Plexus

What in `src/cmd/popt` differs from the object code improver of System III (`cmd/cc/pdp11/c20.c c21.c c2.h`; the PDP-11 `c2`, the VAX one in `cmd/cc/vax/c2` is less close) and from the Z8000 `c2` of Plexus (`cmd/cc/c2.d`: `c20.c` ... `c23.c`, `c2.h`; `cmd/cc/z8000/c2` is close to it). System III has no counterpart of `z22.c` and `z23.c`. Measured 2026-11-07. popt and c2 share their plan (`iterate`, `xjump`, `codemove`, `comjump`, `rmove`, `movedat`), `struct node` and `struct optab` (`opstring`); the module names z20 ... z23 come from the original.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| z20.c | pdp11/c20.c | 3 | c2.d/c20.c | 4 | 44 |
| z21.c | pdp11/c21.c | 229 | c2.d/c21.c | 180 | 933 |
| z22.c | — | — | c2.d/c22.c | 349 | 1610 |
| z23.c | — | — | c2.d/c23.c | 135 | 621 |
| z2.h | pdp11/c2.h | 41 | c2.d/c2.h | 42 | 163 |

40 of the 75 function names are those of System III.

## WEGA's own

- The opcode numbers and names of `z2.h` come from the original's tables: `CODE`, `WORD`, `LONG`, `COMMENT`, `KCOMMENT`, `PSEC`, `OBRACE`, `CBRACE`, `RL`, `RR`, `JPX`, `ADDR` have no match in c2; c2 has `JR`, `CJR`, `TEXT`, `BSS`, `ASCII`, `DWORD`, `DBYTE`, the float operators, `CRET`.
- The register memory: z22.c `findreg findbyte findword findlong findcopy memreg setreg killreg cleargroup findslot ccinval regprefix`, in place of `findrand`, `isanyreg`, `isreg14`, `dest1`, `typeit`. Read against them, they work on another table.
- The register renaming of a leaf function: z23.c `procopt` and its helpers `isused usesreg markregs rename_regs fixldm regof ovlap ishigh nwords reverse itoa_`. c2 has `OPTCRET` and procedure register code of its own form.
- Diagnostics: z21.c `statline jswcheck dumperr outerr prefix dumpnodes`.
- c2 functions without a counterpart: `ccstrlook check checkreg chgcc clrop2 dest1 equsubstr findrand isanyreg isindex isoffreg isreg14 isstarreg lostcc needcc newreg printanode printnodes shortval source streval trunc typeit usecc`.

## Forms that follow the bytes, not the System III text

- `xjump`, `backjmp`: `while (p = p->back) { if (c) continue; break; }`.
- `rmove`: `do_and` falls into `do_arith`; the arms of the big switch as labels inside the `if` that falls into them (`OLDCC.md`, D20).
