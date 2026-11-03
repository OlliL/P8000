# codgen against the PCC of the Plexus tree

What in `src/cmd/codgen` differs from `cmd/cc/pcc.d` (`mip/reader.c`, `match.c`, `allo.c`, `common`, `optim.c`, `trees.c`; `order.c`, `local.c`, `local2.c`, `code.c`) and why. Measured 2026-11-03. `findings.json` holds the notes per function.

## Lines

Lines without comments and blanks that also stand in the PCC file.

| Module | PCC file | Equal | Of |
|---|---|---:|---:|
| reader3.c | mip/reader.c | 555 | 1361 |
| order3.c | order.c | 247 | 993 |
| allo3.c | mip/allo.c | 93 | 625 |
| match3.c | mip/match.c | 116 | 480 |
| comm3.c | mip/common | 91 | 520 |
| local3.c | local2.c | 390 | 1531 |
| optim3.c | mip/optim.c | 137 | 397 |
| tr3.c | mip/trees.c | 43 | 140 |
| lcl3.c | local.c | 57 | 308 |

The module names come from the SCCS stamps of the original; 94 of the 209 function names are those of PCC.

## WEGA's own

The form of the tree nodes: `su` and `fsu` two chars (floating registers counted apart), `sname` a pointer to a symbol in place of `name[]`, `hi` the high word of `lval` under `left`.

Own functions, no function of PCC does the same (read in both texts where the name suggested one):

- Symbol table file and data file (the separate symbol file of the WEGA compiler): rdr3.c `rddata rdsym readsyms readtab readnums readmems sortsyms tagfind mapn mapsym mapnm mkhash nlcheck rdin2`, regs3.c `symuse symreg regforsym fixname oregfix memname`.
- The register lists, in place of `busy[]` of PCC: regallo3.c, all of it, regs3.c `rfreeall rfind rnassign rnnullp rnnullcall rnnullall rnnulll rnnullw rnnull nxwork regreset`.
- Register variables by command (`F`, `R`, `S`, `T` records): regs3.c `fcmd rcmd scmd tcmd findtemp`, wrt3.c `parminit`, local3.c `regargs`.
- Temporaries in registers: regs3.c `regtabinit tabfind tabclear fixtemp tmpcleanup tmpdump`.
- Arguments in registers: order3.c `argscan argsort fixstarg argstrip argplace`, reader3.c `argprep`, allo3.c `freeparm rclass allocreg`.
- Function entry and exit: local3.c `epilog regrestore fpinc prolog fpdec regsave`, wrt3.c `hdrout endrun endline structretcode fbegin`, lcl3.c `gswitch chk`.
- Others: reader3.c `init delstrip`, local3.c `psuffix tsuffix lineout geninc ltypeop shincr prlong oregput icright canon2`, order3.c `orderadd lmuldiv`, comm3.c `syserror bsize prline tbytes plainicon alloc allocinit`, regs3.c `fixfld mapregs`, tr3.c `foldadd member`, optim3.c `isint`.
- Candidates for a PCC name, shortened forms of PCC functions, not renamed: tr3.c `makeblock` (`block`), `buildstar` (`buildtree`), optim3.c `treeeq convelim`.

## Forms that follow the bytes, not the PCC text

Many labels and `goto` in the physical order of the original: `match`, `getreg`, `regjam`, `argscan`, `reclaim`, `setbin`, `rmove`, `allocreg`, `cbranch`, `regargs`, `rsmatch`, `symreg`, `optim` (`order` has its gotos from PCC). They give the bytes with the shipped popt; whether the authors wrote them is not known. Tried 2026-11-03: the PCC text of the search loop of `match` with the goto form of its switch: match 62 lines worse, popt moves the loop (trace in `OLDCC_LOG.md`, how to: `host/recon/poptrace.sh`). A natural form with the same bytes may exist; `mainp2`, `optim`, `xjump` had one.
