# Register slots: unused frame words of the compiler of 11/83

An open question of the old compilers (`OLDCC.md`, D1). Programs built in 1983 by a cc that is otherwise the shipped one
have, in some functions, frames that are 1 to 3 words larger than the shipped cc makes them; the code is the same. The
words are never used. The tree rebuilds them as unused locals (`int unused;`, `int unused[N];`, `int spare;`); the rule
that gives them is not found. This file collects what is known, to be taken up on its own. Started 2026-11-24 (make).

## What is known

- The word is allocated by cparse, at the declaration of a register local (`oalloc` in `cparse/sym1.c`), not by codgen as a
  temporary. make `sindex(s1, s2)`: the homes of the parameters stay at 0 and 2, the auto `ii` declared after the register
  locals moves from 4 to 6. make `setenv`: the autos `es`, `vp`, `length` behind three register locals all move up by 4.
  mail `system`: the word lies where the register locals `istat`, `qstat` are declared.
- So the original gives some register locals a frame word, as v1 (`OLD_D1`) gives one to every register local and the shipped
  cparse to none (only char and short register variables, see `OLDCC.md` D1). It is not the v1 rule: most functions with
  register locals have no word, and a function with several register locals has fewer words than register locals.
- The shipped `oalloc` gives a register local a place when `sfldc` is 1 or 2; only `getpreg` (parameters) sets these values,
  and `lookup` sets `sfldc = 0` in a new entry (`sym1.c`). A cparse without that line would read what the freed block held
  before (the entries are malloc'd and freed by `clearst` along the hash chains). This is the working guess (below).

## Witnesses (a word, rebuilt as an unused local)

| Program (compiler) | Function | Words | Register locals (parameters in register) |
|---|---|---|---|
| make 11/83 (shipped cc) | getmflgs | 1 | vpr, pe, p |
| make | colontrans | 1 | i, ps1 (b) |
| make | straightrans | 3 | vbp, pchain, pn (b) |
| make | setvar | 1 | p (v, s) |
| make | eqsign | 1 | p (a) |
| make | appendq | 1 | p (head, tail) |
| make | mkqlist | 2 | qbufp, s (p) |
| make | sindex | 1 | p1, p2, flag |
| make | is_sccs | 1 | p (filename) |
| make | trychk | 1 | p (str) |
| make | addstars | 2 | p1, p2 (pfx) |
| make | setenv | 2 | ea, nenv, p |
| make | readenv | 1 | ea, p |
| make | eqstr | 1 | i (a, b, n) |
| make | yyparse (`/usr/lib/yaccpar`) | 1 | yypvt, yystate, yyps, yyn, yypv, yyxi (sed in the Makefile) |
| diff (shipped cc) | prepare | 1 | p, j, h |
| diff | equiv | 1 | i, j |
| diff | main | 1 | k |
| diff | stone | 1 | i, k, y |
| mail (shipped cc) | isfrom | 1 | p (lp) |
| mail | system | 1 | istat, qstat |
| nl (shipped cc) | main | 1 | j, i, p, header, body, footer, ptr |
| slibc (vl, segmented) | calloc | 1 | q, m, mp (`int spare`, `#ifdef SEG`) |

Without a word (counter-examples, same programs): make main, isprecious, rdmakecomm, prname, setflags, setmflgs, doname,
docom1, expand, dbreplace, ballbat, hashloc, makename, copys, concat, intalloc, subst, varptr, srchvar, any, findfl,
execat, trysccs, sccstrip, strshift, execvp, get, rm_gots, exists, amatch, lookarch, dosys, doshell, doclose, doexec,
dynamicdep, runtime, prtmem; diff ralloc, sort, unsort, filename, prune, newcand, search, unravel, check, skipline, output,
fetch, readhash; mail main, copyback, sendrmt; gpasswd notify; nonsegmented libc calloc.

Unused words of another kind (no register local in the function; not counted here): ld slookup, pslookup, index16 (v1,
`int spare`), gpasswd catlist (`int unused`); popt findcopy, adrlen (v0p, "an unused local word", guesses).

## Measured

- No feature of the source separates the functions with words from those without: not the name of the register local, not
  its earlier use as a parameter in the file (setmflgs has `p` after getmflgs and gets no word), not the type, the number
  of register locals, `for (x = ...)`, an initializer, `++`/`--`, a leaf function or a register parameter (a table of the 77
  functions with register locals of make, diff, mail, nl, gpasswd).
- Other compilers for the whole of make: ccv1, ccv1r, ccv1q lose more functions than they win; the v1 cparse (`OLD_D1`) with
  the shipped codgen and popt (a scratch root): 177 of 211 functions against 197 of the shipped cc (a word for every
  register local is too many).
- Stale sfldc: a scratch copy of the shipped cparse without `q->sfldc = 0` in `lookup`, behind the shipped `/bin/cc`
  (logging by `write(2)`, no stdio, so the heap stays as it is): main.c right (getmflgs); misc.c gets its words in the
  functions just before the right ones (intalloc, subst, colontrans, findfl, get, rm_gots instead of straightrans .. eqstr):
  47 of 65 functions of main.c, misc.c, files.c right. The heap of the cparse of 11/83 runs otherwise.
- A variant whose string constants (`savestr`, malloc'd, never freed) do not use the heap hung (the output of the string
  list); no result.

## Open, next steps

- The heap of the old cparse: what else is malloc'd (the strings of `savestr`, the `fcon` blocks of code1.c, the stdio
  buffers), the size of `struct symtab` (26 bytes now), the malloc of its libc; which of them moves the reuse of the freed
  entries so that misc.c of make comes out right, with main.c and files.c still right.
- The other witnesses as tests of a rule found on make: diff, mail, nl (their `int unused` must come out by the compiler
  then), slibc calloc (segmented).
- A rule that holds would be a switch of a cparse of its own (a compiler of 11/83), owner's decision; the unused locals
  would then go.
