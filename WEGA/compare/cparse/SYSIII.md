# cparse against System III and Plexus

What in `src/cmd/cparse` differs from pass 1 of the portable C compiler of System III (`cmd/cc/pcc/mip`: `cgram.y scan.c trees.c pftn.c optim.c xdefs.c comm1.c manifest mfile1`; `cmd/cc/pcc/mdp`: `local.c code.c macdefs`; `vax/mip` is the same text) and from the Plexus PCC (`cmd/cc/pcc.d`, for the Z8000 parts). Measured 2026-11-07. System III is the model; the node members follow it (`p->in.op`, `p->tn.lval`, `p->fn.cdim`, `p->fpn.dval`), the macros of `manifest`, the table sizes and `ASOP` ... `STROP` in `mfile1`. `findings.json` holds the notes per function.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| cgram1.y | pcc/mip/cgram.y | 757 | pcc.d/mip/cgram.y | 727 | 860 |
| scan1.c | pcc/mip/scan.c | 599 | pcc.d/mip/scan.c | 601 | 678 |
| trees1.c | pcc/mip/trees.c | 830 | pcc.d/mip/trees.c | 634 | 1075 |
| pftn1.c | pcc/mip/pftn.c | 483 | pcc.d/mip/pftn.c | 485 | 786 |
| xdefs1.c | pcc/mip/xdefs.c | 53 | pcc.d/mip/xdefs.c | 54 | 79 |
| optim1.c | pcc/mip/optim.c | 179 | pcc.d/mip/optim.c | 130 | 329 |
| local1.c | pcc/mdp/local.c | 151 | pcc.d/local.c | 141 | 364 |
| code1.c | pcc/mdp/code.c | 108 | pcc.d/code.c | 119 | 423 |
| manifest | pcc/mip/manifest | 186 | pcc.d/mip/manifest | 180 | 230 |
| mfile1 | pcc/mip/mfile1 | 173 | pcc.d/mip/mfile1 | 161 | 307 |
| macdefs | pcc/mdp/macdefs | 34 | pcc.d/macdefs | 40 | 51 |

115 of the 177 function names are those of System III.

## WEGA's own

Own functions, read against the PCC ones whose name or place suggested a match (`copysym` is not `movestab` or `relook`, `lxstart` calls `lxinit`, `bstart` and `bend` are the records of `bccode` and the end of a function):

- The symbol table file and the data file: sym1.c `hash regfree copysym refid sinit sheader putrefsyms putsym putnames puttags putrefs sputc`, vinit1.c `resetmem putstrs`, code1.c `openfiles fnfile fltlab fhashv newsect sectfull sectadd`.
- The intermediate file: code1.c `bstart bend bfend lineid`.
- Arguments in registers: pftn1.c `argalloc setreg initpreg getpreg argdebug psavel`, trees1.c `argregs`.
- Others: scan1.c `lxstart lxclass sfinit dimtinit savestr`, trees1.c `lcons tempnode`, optim1.c `treeeq rmconv notflt`, local1.c `cprint`, comm1.c `syserr bsize twords argsize tbytes ncopy tcopy getmem`, xdefs1.c `scnames`.

PCC functions without a counterpart here: `andable aocode aoend bycode cendarg checkst ejobcode eprint fixdef fldty isitlong movestab outchar p2tree relook tprint`.

## Forms that follow the bytes, not the System III text

- `optim` (optim, also in codgen): the MINUS loop as a goto loop with its test inside the `if`, `if (...) { zapleft: ... } if (...) goto zapleft;`.
- `oalloc`: `if (A) { both: ... } else if (B) goto both;`.
- `defid`: the dimension loop as a goto loop; `mismatch:` and the step of the SMEMB loop in the physical order of the original.
- `stref`: `x = x + PTR` in place of `x += PTR`.
- `cinit`: `inoff += sz; return;` in case NAME.
- `lxer` loop: `switch( lxchar=getchar(), (p=lxcp[lxchar+1])->lxact )` in place of the System III form `switch( (p=lxcp[(lxchar=getchar())+1])->lxact )`; the data of `xdefs1.c` keep the order of the initialized definitions (`reached` before `ftitle`), the order of System III changes the data.
- Warnings are fixed by casts at the call (`(struct symtab *) lookup(...)`, `(NODE *) tcopy(...)`), not by declaring the return type: that would change the code of the segmented build.
- `cgram1.y`: the 7 shift/reduce conflicts (the same as in System III) are resolved as shifts with `%prec CM` and `%nonassoc error RP ELSE`; the yacc tables stay the same.
They give the bytes with the shipped popt (`OLDCC.md`, D20).
