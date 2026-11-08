# codgen against System III and Plexus

What in `src/cmd/codgen` differs from pass 2 of the portable C compiler of System III (`cmd/cc/pcc/mip`: `reader.c match.c allo.c comm2.c optim.c trees.c mfile2`; `cmd/cc/pcc/mdp`: `order.c local2.c table.c`) and from the Plexus PCC (`cmd/cc/pcc.d`, for the Z8000 parts). Measured 2026-11-07. The tree nodes follow System III: `p->in.op`, `p->tn.lval`, `p->bn.label`, `p->stn.stsize`. `findings.json` holds the notes per function.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| reader3.c | pcc/mip/reader.c | 550 | pcc.d/mip/reader.c | 479 | 1585 |
| match3.c | pcc/mip/match.c | 99 | pcc.d/mip/match.c | 95 | 514 |
| allo3.c | pcc/mip/allo.c | 80 | pcc.d/mip/allo.c | 74 | 669 |
| regs3.c | pcc/mip/allo.c | 136 | pcc.d/mip/allo.c | 99 | 1039 |
| optim3.c | pcc/mip/optim.c | 132 | pcc.d/mip/optim.c | 113 | 409 |
| order3.c | pcc/mdp/order.c | 235 | pcc.d/order.c | 198 | 1048 |
| local3.c | pcc/mdp/local2.c | 316 | pcc.d/local2.c | 394 | 1694 |
| cg.h | pcc/mip/mfile2 | 38 | pcc.d/mip/mfile2 | 34 | 208 |

78 of the 229 function names are those of System III.

## WEGA's own

The form of the tree nodes: `su` and `fsu` two chars (floating registers counted apart), `sname` a pointer to a symbol in place of `name[]`, `hi` the high word of `lval` under `left`.

Own functions, no function of System III does the same (read in both texts where the name suggested one):

- Symbol table file and data file (the separate symbol file of the WEGA compiler): rdr3.c `rddata rdsym readsyms readtab readnums readmems sortsyms tagfind mapn mapsym mapnm mkhash nlcheck rdin2`, regs3.c `symuse symreg regforsym fixname oregfix memname`.
- The register lists, in place of `busy[]` of System III: regallo3.c, all of it, regs3.c `rfreeall rfind rnassign rnnullp rnnullcall rnnullall rnnulll rnnullw rnnull nxwork regreset`.
- Register variables by command (`F`, `R`, `S`, `T` records): regs3.c `fcmd rcmd scmd tcmd findtemp`, wrt3.c `parminit`, local3.c `regargs`.
- Temporaries in registers: regs3.c `regtabinit tabfind tabclear fixtemp tmpcleanup tmpdump`.
- Arguments in registers: order3.c `argscan argsort fixstarg argstrip argplace`, reader3.c `argprep`, allo3.c `freeparm rclass allocreg`.
- Function entry and exit: local3.c `epilog regrestore fpinc prolog fpdec regsave`, wrt3.c `hdrout endrun endline structretcode fbegin`, lcl3.c `gswitch chk`.
- Others: reader3.c `init delstrip`, local3.c `psuffix tsuffix lineout geninc ltypeop shincr prlong oregput icright canon2`, order3.c `orderadd lmuldiv`, comm3.c `syserror bsize prline tbytes plainicon alloc allocinit`, regs3.c `fixfld mapregs`, tr3.c `foldadd member`, optim3.c `isint`.
- Candidates for a PCC name, shortened forms of System III functions, not renamed: tr3.c `makeblock` (`block`), `buildstar` (`buildtree`), optim3.c `treeeq convelim`.

## Forms that follow the bytes, not the System III text

Many labels and `goto` in the physical order of the original: `match`, `getreg`, `regjam`, `argscan`, `reclaim`, `setbin`, `rmove`, `allocreg`, `cbranch`, `regargs`, `rsmatch`, `symreg`, `optim` (`order` has its gotos from PCC). They give the bytes with the shipped popt; whether the authors wrote them is not known. Tried 2026-11-03: the System III text of the search loop of `match` with the goto form of its switch: match 62 lines worse, popt moves the loop (trace in `OLDCC_LOG.md`, how to: `host/recon/poptrace.sh`). A natural form with the same bytes may exist; `mainp2`, `optim`, `xjump` had one.
- `rddata` (rdr3.c): `if (1) return d12kcopy(n);` in place of `return d12kcopy(n);` with the code behind it; the compiler of `-DOLD_V0P` emits the code that cannot be reached, and `#else` would drop it.
