# popt against c2 of the Plexus tree

What in `src/cmd/popt` differs from `cmd/cc/z8000/c2` (`c20.c` ... `c23.c`, `c2.h`). `cmd/cc/c2.d` is close to it: only `z22.c` against `c22.c` differs by a few lines. Measured 2026-11-03. popt and c2 share their plan (`iterate`, `xjump`, `codemove`, `comjump`, `rmove`, `movedat`) and `struct node`; the module names z20 ... z23 come from the original.

## Lines

Lines without comments and blanks that also stand in the c2 file.

| File | c2 file | Equal | Of |
|---|---|---:|---:|
| z20.c | c20.c | 3 | 43 |
| z21.c | c21.c | 173 | 900 |
| z22.c | c22.c | 341 | 1573 |
| z23.c | c23.c | 132 | 571 |
| z2.h | c2.h | 43 | 101 |

44 of the 73 function names are those of c2.

## WEGA's own

- The opcode numbers and names of `z2.h` come from the original's tables: `CODE`, `WORD`, `LONG`, `COMMENT`, `KCOMMENT`, `PSEC`, `OBRACE`, `CBRACE`, `RL`, `RR`, `JPX`, `ADDR` have no match in c2; c2 has `JR`, `CJR`, `TEXT`, `BSS`, `ASCII`, `DWORD`, `DBYTE`, the float operators, `CRET`.
- The register memory: z22.c `findreg findbyte findword findlong findcopy memreg setreg killreg cleargroup findslot ccinval regprefix`, in place of `findrand`, `isanyreg`, `isreg14`, `dest1`, `typeit`. Read against them, they work on another table.
- The register renaming of a leaf function: z23.c `procopt` and its helpers `isused usesreg markregs rename_regs fixldm regof ovlap ishigh nwords reverse itoa_`. c2 has `OPTCRET` and procedure register code of its own form.
- Diagnostics: z21.c `statline jswcheck dumperr outerr prefix dumpnodes`.
- c2 functions without a counterpart: `ccstrlook check checkreg chgcc clrop2 dest1 equsubstr findrand isanyreg isindex isoffreg isreg14 isstarreg lostcc needcc newreg printanode printnodes shortval source streval trunc typeit usecc`.

## Forms that follow the bytes, not the c2 text

- `xjump`, `backjmp`: `while (p = p->back) { if (c) continue; break; }`.
- `rmove`: `do_and` falls into `do_arith`; the arms of the big switch as labels inside the `if` that falls into them (`OLDCC.md`, D20).
