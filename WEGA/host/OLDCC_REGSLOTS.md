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
| comm 11/18/83 (sys3, not in the tree) | compare | 2 | ra, rb |
| csplit 11/18/83 (sys3) | fatal | 1 | fls, num |
| csplit | re_arg | 1 | ptr, ch (char) |
| tsort 11/18/83 (sys3) | findloop | 1 | i, j |
| uniq 11/18/83 (sys3) | skip | 2 | nf, nl (s) |

The programs of the witnesses are of one build time: make, diff, nl, comm, csplit, tsort, uniq have the crt0 stamp 3.2
11/18/83, mail 11/16/83. The same build of 11/18/83 has programs without any such word although they have register locals:
ar, cmp, du, grep, pr, putfile, who, cpio (exact by the shipped cc, no unused local). So the word does not belong to a
program or a time but to something of each compilation: this fits the stale sfldc (the heap of cparse) better than a rule of
the source.

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

## The search for witnesses (2026-11-24)

The masked compare of every function of a build by the shipped cc with the original (the frame instructions `dec`/`sub`,
`inc`/`add r15` as one token, `@r15` as `%0000(r15)`), reporting the found functions whose frame differs, finds all 15
words of make and nothing else (calibrated on the build of make without its unused locals). Run over the 66 sys3 programs of
COVERAGE with D1 loose or unknown (System III source by the shipped cc, linked, or the object when the link fails): 17 built
and compared (the rest are fonts or do not compile: a.out.h, missing parts); witnesses comm, csplit, tsort, uniq (above).
wtmpfix, fwtmp (01/83) differ in their own functions and in the libc too: an older compiler, not counted. line, tabs (07/82)
have smaller frames: v0. Not covered: the todo programs without a source (sh 08/83, csh, adb, ps, ...); their D1 verdict
in OLDCC_DATES.md counts unused words but cannot tell these words from the shipped ones.

## The heap experiment (2026-11-24)

A test set of 15 programs by `cc -O -c` per file, the frames of the found functions against the originals (the masked
compare above): the witnesses make (without its unused locals), diff, mail, nl (without theirs), comm, csplit, tsort, uniq
and the counter-programs ar, cmp, du, grep, pr, putfile, who, cpio. The shipped cc: exactly the 27 witness functions differ.
A scratch cparse (the shipped source, not in the tree) behind the shipped `/bin/cc`, without `q->sfldc = 0` in `lookup`:

- No knob: 30 functions differ. Right as a whole: diff (4 witnesses), nl, comm, tsort, uniq, mail isfrom, csplit re_arg,
  make getmflgs, with all their functions without a word. Wrong: misc.c of make (intalloc, subst, colontrans, findfl, get,
  rm_gots get words, straightrans .. eqstr none), make dbreplace, eqstr, yyparse, mail system and csplit fatal one word too
  many, ar (rcmd, notfound, longt, select) and cpio (gmatch, makdir, missdir) words they do not have.
- The mechanism, read from a log of the scratch cparse (write(2), no stdio): `clearst` frees the entries of a function;
  the V7 malloc starts its search at the block freed last, so a new entry takes the freed blocks in front of the rover and
  fresh memory behind them. A parameter leaves sfldc 1 in its block; a register local that gets such a block gets a word,
  and keeps the 1 for the next one. make misc.c: the blocks of concat's parameters a, b, c (sfldc 1) go to subst (vbp),
  colontrans (i, ps1) and intalloc (p) in the experiment; the original gives straightrans its three words (vbp, pchain, pn)
  instead: there these three blocks were taken two functions later.
- Knobs that change nothing for the better (best 29 or 30): the size of an entry (+2 .. +8; -2 breaks cparse), extra bytes
  of the string blocks of savestr, a malloc at the start, the size of the hash table (1 .. 64), the order of the buckets
  in clearst, the malloc of the other libc versions (libc-v12 = the malloc 3.3 of /lib/libc.a, v8, v3, v4: the same reuse),
  an extra block per clearst (freed last: 43, freed first: 30, kept: 40), entries not freed (registers 35, autos 33, locals
  39; parameters: 27, but then no sfldc 1 is left at all, the shipped result).

So the stale sfldc explains whole programs exactly, while the sequence of allocations and frees of the cparse of 11/83 is
not the one of the shipped source: something else allocates or frees between the functions (concat .. straightrans of
make). Not found which.

## The simulation (2026-11-24)

The scratch cparse logs every malloc and free of its own (symbol entries 26 bytes, the string list of savestr: 8 bytes and
the string, freed in vinit1.c at the end of a function), every new entry with the byte at sfldc, every free of an entry and
every decision of oalloc, and the start of each function (bfcode). A Python copy of the V7 malloc of
`src/lib/oldlibc/malloc/malloc.c` (circular first fit, `allocp` = the block freed last, coalescing while searching, sbrk in
1 KB steps) replays the log: all 668 mallocs of make misc.c at the logged addresses (no hidden malloc of stdio), and with a
model of the byte at entry+12 (a parameter leaves its sfldc of getpreg, any other entry what it found, a malloc header or a
string overwrites) the stale sfldc of all 350 new entries right. On the 22 files of the test set it gives exactly the 30
functions of the real experiment. Hypotheses are then changes of the event stream, scored in seconds:

| Hypothesis | Functions wrong (shipped cc: 27) |
|---|---|
| as logged (shipped source without `q->sfldc = 0`) | 30 |
| clearst order: reversed, by address, by creation, inner levels first | 32 .. 59 |
| strings not freed / not on the heap / 1 or 2 bytes longer | 33 / 26 / 30 / 30 |
| an extra block per function or per clearst (26, 8, 2 bytes; kept or freed) | 30 .. 41 |
| every parameter leaves 1 | 30 |
| sfldc cleared at the declaration of autos, statics, registers (and combinations) | 25 .. 30 |
| another size of struct symtab (16 .. 32 bytes) with sfldc at any offset | best 25 |

None gives the 27 witnesses without new false ones. The programs that come out right as a whole (diff, nl, comm, tsort,
uniq, du, grep, pr, putfile, who, cmp) and the wrong ones (make misc.c, ar, cpio, csplit, mail) are the lead: something in
the wrong ones makes the old cparse allocate or free otherwise (not yet seen what).

## Open, next steps

- The heap of the old cparse: what else is malloc'd (the strings of `savestr`, the `fcon` blocks of code1.c, the stdio
  buffers), the size of `struct symtab` (26 bytes now), the malloc of its libc; which of them moves the reuse of the freed
  entries so that misc.c of make comes out right, with main.c and files.c still right.
- The other witnesses as tests of a rule found on make: diff, mail, nl (their `int unused` must come out by the compiler
  then), comm, csplit, tsort, uniq, slibc calloc (segmented); and as counter-tests the programs of 11/18/83 without a word
  (ar, cmp, du, grep, pr, putfile, who, cpio).
- A rule that holds would be a switch of a cparse of its own (a compiler of 11/83), owner's decision; the unused locals
  would then go.
