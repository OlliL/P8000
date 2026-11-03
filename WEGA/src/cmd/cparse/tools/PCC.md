# cparse against the PCC of the Plexus tree

What in `src/cmd/cparse` differs from `cmd/cc/pcc.d` (`mip/pftn.c`, `trees.c`, `cgram.y`, `scan.c`, `optim.c`, `common`, `xdefs.c`, `manifest`, `mfile1`; `local.c`, `code.c`, `macdefs`). Measured 2026-11-03. cparse was written from the PCC text from the start: its macros, `NODE` and its members are those of `mip`. `findings.json` holds the notes per function.

## Lines

Lines without comments and blanks that also stand in the PCC file.

| File | PCC file | Equal | Of |
|---|---|---:|---:|
| cgram1.y | mip/cgram.y | 780 | 869 |
| scan1.c | mip/scan.c | 555 | 628 |
| trees1.c | mip/trees.c | 837 | 1073 |
| pftn1.c | mip/pftn.c | 479 | 759 |
| xdefs1.c | mip/xdefs.c | 53 | 78 |
| optim | mip/optim.c | 171 | 309 |
| local1.c | local.c | 147 | 328 |
| code1.c | code.c | 103 | 375 |
| manifest | mip/manifest | 246 | 289 |
| mfile1 | mip/mfile1 | 180 | 312 |
| macdefs | macdefs | 46 | 51 |
| common | mip/common | 157 | 324 |

126 of the 180 function names are those of PCC.

## WEGA's own

Own functions, read against the PCC ones whose name or place suggested a match (`copysym` is not `movestab` or `relook`, `lxstart` calls `lxinit`, `bstart` and `bend` are the records of `bccode` and the end of a function):

- The symbol table file and the data file: sym1.c `hash regfree copysym refid sinit sheader putrefsyms putsym putnames puttags putrefs sputc`, vinit1.c `resetmem putstrs`, code1.c `openfiles fnfile fltlab fhashv newsect sectfull sectadd`.
- The intermediate file: code1.c `bstart bend bfend lineid`.
- Arguments in registers: pftn1.c `argalloc setreg initpreg getpreg argdebug psavel`, trees1.c `argregs`.
- Others: scan1.c `lxstart lxclass sfinit dimtinit savestr`, trees1.c `lcons tempnode`, optim1.c `treeeq rmconv notflt`, local1.c `cprint`, comm1.c `syserr bsize twords argsize tbytes ncopy tcopy getmem`, xdefs1.c `scnames`.

PCC functions without a counterpart here: `andable aocode aoend bycode cendarg checkst ejobcode eprint fixdef fldty isitlong movestab outchar p2tree relook tprint`.

## Forms that follow the bytes, not the PCC text

- `optim` (optim, also in codgen): the MINUS loop as a goto loop with its test inside the `if`, `if (...) { zapleft: ... } if (...) goto zapleft;`.
- `oalloc`: `if (A) { both: ... } else if (B) goto both;`.
- `defid`: the dimension loop as a goto loop; `mismatch:` and the step of the SMEMB loop in the physical order of the original.
- `stref`: `x = x + PTR` in place of `x += PTR`.
- `cinit`: `inoff += sz; return;` in case NAME.
They give the bytes with the shipped popt (`OLDCC.md`, D20).
