# Handoff: /lib/cparse

Started 2026-10-30. Program: `/lib/cparse` (WEGA 3.1, pass 1 of cc, PCC front end; E711, nonsegmented, stripped).
Procedure: `host/HOWTO_reconstruct.md`; plan and criteria: `src/TODO.md`, "Plan: towards the old C compiler".
Tools: `host/recon/` through the launchers of this directory (configuration `progcfg.py`).

## Owner decisions for cparse (2026-10-30)

- Commits are allowed for the cparse work (no push). Commit after each finished step or module when the build works and
  no exact function got worse.
- The Plexus PCC sources (`<PLEXUS_SRC>/cmd/cc/pcc.d/mip`: cgram.y, scan.c, pftn.c, trees.c, optim.c, xdefs.c, common,
  mfile1, manifest, and pcc.d/code.c, local.c, macdefs) may be taken as the base of our sources and changed until the
  code matches; the owner takes the copyright risk. This is an exception to the CLAUDE.md rule "Plexus private reference
  only", for cparse.
- If the yacc tables cannot be made byte-identical, the grammar state is saved and the parser is linked as a cut
  assembler module for the time being.
- Attempts per function: no fixed limit. Go on while each attempt reduces the strict diff, removes a kind of difference
  or tests a new founded hypothesis (step 7 rules, or an observation from a related exact function). A function becomes
  a finding when its remaining difference is assigned to a D class with evidence (the same pattern documented elsewhere
  as a compiler difference, or measured in a small test program), or after three attempts in a row without progress
  with all step 7 rules checked (note "not explained", the forms tried, second round after structs and names).
- Stop criterion: not before every function has a draft and the corpus test runs; a part that does not progress is
  documented and saved, and the work goes on with the rest.

## Facts (step 0)

- Header: code 48896 (0xbf00), data 18584 (0x4898), bss 28804 (0x7084); no symbols.
- Start code: exactly the start-up file of codgen (`mkcrt0.sh cparse`, entry `cparse` in crt0.list; same text as `crt0-codgen.s`) (text 0x0000..0x002a, data word 0 at 0x0000; `_environ` is the
  first bss word, 0x4898). The own code starts at 0x002a.
- SCCS stamps in the data (module order; each stamp is one file): cgram1.y 2.5 (0x0002), xdefs1.c 2.3 (0x1a64), scan1.c 2.4
  (0x1bee), pftn1.c 2.7 (0x2422), trees1.c 2.8 (0x27b0), optim1.c 2.1 (0x2eae) and "optim" 2.5 (0x2ee8), code1.c 2.7
  (0x3000), local1.c 2.7 (0x32c6), comm1.c 2.1 (0x350a) and "common" 2.5 (0x3544), vinit1.c 2.3 (0x3bb0), sym1.c 2.8
  (0x3d7e). "optim" and "common" are files included by optim1.c and comm1.c (PCC: comm1.c includes common). The data of
  sym1.c ends at 0x41a4; the libc data follows (INFINITY, NAN, the error texts, _iob at 0x4360, _ctype_ at 0x4790).
- cgram1.y is a yacc grammar ("yacc stack overflow", "syntax error"); the original /usr/bin/yacc is the tool for it.

## Step 1: function starts (done)

- `inventory.py` (host/recon): 230 proven starts, 29 candidates, 0 decode failures; `own_table.py` wrote
  `own_functions.txt`: 198 functions below 0x9f06 (186 proven, 12 candidates). findings.json has all of them (status
  open; f0000 is crt0).
- Module of each function from the strings it references (scratch `tmp/tmp/cparse_datamap.txt`): the code is in the
  order of the stamps: cgram1 0x002a.. (yyparse at 0x0312, 4194 bytes), xdefs1 ~0x1374, scan1 ~0x13b6..0x2448 (yylex
  0x1b2a), pftn1 0x2448.., trees1 ~0x397a.., optim1 ~0x5cb8.., code1 ~0x61d0.., local1 ~0x7574.., comm1 ~0x7b4c..,
  vinit1 ~0x821a.., sym1 ~0x8c8e..0x9f06. main is at 0x60d8 (crt0 calls it), in code1. The exact boundaries are open.

## Step 2: libc (provisional: one block)

- Own code ends at 0x9f06 (9e04..9f04 are own: they use ftell, fwrite, putc). libcver.py said "libc from af18" because
  it only counts shipped members; oldlibc members start at 0xa40c.
- Identified: shipped atof, pow10, cuexit, decimal, brk, chmod, close, creat, ioctl, lseek, open, read, signal, umask,
  write, cerror; oldlibc fseek 0_2, ftell 0_2, rdwr 0_3, fopen 0_1, printf 0_5, doprnt 0_5, ungetc 0_1, filbuf 0_3,
  setbuf 0_1, flsbuf 0_2, endopen 0_4, findiop 0_2, cnvt 0_2, length 0_2, malloc 0_3 (not 0_5), isatty 0_2, stty 0_2,
  strcat 0_2, strcpy 0_2, strlen 0_2 (host/recon/oldlibc_match.py, libc_spec.py). Gaps without a version: 9f06..a40c
  (float output, INFINITY/NAN), a706..a790 (fdopen), b088..b26a (ecvt-like), b78a (__callex), b79a..b832 (perror),
  bc36..bc50, bc78..bca6 (strcmp), be2c..bf00. The data of several members differs from every known version (doprnt,
  data/_iob, errlst), so this libc is an older revision.
- Provisional solution: the whole libc part 0x9f06..0xbf00 (data 0x41a4..0x4898) is one cut module `../cplibc.s`
  (`cparse_libc.spec`, host/tools/unlink.py) with the entry names from the oldlibc and libc.a objects. unlink.py writes
  the jump tables as labels; the other pointer words of the data (sys_errlist 38 entries, the device error list 8,
  _iob stdin _ptr/_base -> __sibuf, the initial break -> _end, and `_lastbuf` at 0x4400 -> __iob+160, found when the data of our link moved) are made symbolic by host/tools/datafix.py. libc bss:
  __sobuf (512, 0xb4f4), __sibuf (512, 0xb6f4), _sigtab (40, 0xb8f4). Verified: the object has the bytes of the
  original (text 8186, data 1780, 0 differing words outside the relocations).
- Open: split into the members (shipped members by ar x, oldlibc versions, new versions for the gaps) as in ld.

## Step 3: own code as one block (done)

- `make block` / `make checkblock`: crt0.o (mkcrt0.sh cparse) + ownraw.o (the own part as raw bytes, made at build time by
  `tools/rawblock.py`, never in git) + cplibc.o give /lib/cparse (code, data, bss equal; `tools/cmpimage.py`).
- A symbolic cut of the own part with unlink.py does not work: with separate I and D space and the own data starting
  at 0x0002, small constants are taken as data addresses (`#D0002`) and bss addresses as text addresses.
- Corpus harness (`stagecorpus.py`, progcfg CORPUS_*): the original cpp makes t.i from each C source of src/cmd and
  src/lib, then `/lib/cparse -E t < t.i` and ours are compared (t.I, t.D, t.S, exit status, messages). The block
  binary gives 15 of 15 identical; /lib/codgen as "ours" gives differences (the harness sees them).

## Next

- Step 4/5: sources from the Plexus PCC base, module by module; compile with the WEGA cc, link with cplibc.o, strict
  comparison (needs the function names in findings.json, then strictcmp/d12model).

## Step 4/5: sources on the PCC base (2026-10-30, in progress)

- Base: `tools/gen/gen.py` composed every module from the Plexus PCC functions in the order of the original (match by
  instruction similarity, scratch tmp/tmp/cpmap/match.py); placeholders for the functions without a match. Commits
  6017ea2 (base), 078e9c7 (comm1).
- WEGA is a strongly reworked PCC pass 1, measured so far:
  - operator numbers and dope flags differ (manifest from the indope table of the original; FEQ..FGT, RET, PROC,
    UPROC 119, SUBSR, TEMP, SWTCH, DESC, SEARCH, TMPS, LAB; BITFLG 0100000; DSIZE 130; NONAME 0);
  - the symbol table is not PCC's stab[]: entries of 26 bytes from malloc, hash chains linked by XOR of the
    neighbours (heads in a bss array at 0x9f3e), names in a name table ntab (0x4966) that grows from both ends
    (ntabindx 0x1b14, ntab_globals 0x1b16, "name table overflow"); a NAME node holds a pointer to its entry in rval;
  - the init functions of pftn.c are in vinit1.c, the type and symbol functions in sym1.c, local.c functions in code1.c;
    common has System V additions (where, dexit, ncopy, tcopy) and WEGA ones (bsize, twords, tbytes, argsize, branch,
    getmem);
  - the output: .I through the FILE* at 0x91ec (branch writes "B%d"), .D through the FILE* at 0x7c06 (incode, cinit).
- Compiler differences seen in cparse (classification by reading): D18 new (never @r15, always 0(r15)), D1 (our cc
  reserves a home slot for register parameters and rounds the frame to 4; the cc of cparse does neither), D2, D3
  (the old cc keeps a spilled parameter in its register across branches), D9 (w>>8 as byte move), D10 (long shift by
  a constant as sdal with a negative count), D11 (shared code between the TNEXT arms in talloc, probably popt).

## State 2026-10-30 (end of the first AFK session)

- Every module has a draft from the WEGA disassembly (xdefs1, scan1, pftn1, trees1, optim1/optim, code1, local1,
  comm1/common, vinit1, sym1); the parser cgram1 is cut from the original as an assembler module (`tools/cutgram.py`,
  owner fallback) until the grammar is rebuilt. `make` links crt0.o (`mkcrt0.sh cparse`), the eleven modules and
  cplibc.o. The parser keeps the original text and data addresses (it is the first module), so its raw table bytes stay
  valid (unlink.py `notables`).
- The cut parser declares the whole bss as commons in the order of the original (`BSS` in cutgram.py, found by
  `tools/bsspair.py`), so the bss of our link has the original layout and size. Data: every module begins with its SCCS
  stamp; the data of our link is 102 bytes longer than the original (switch tables of the shipped cc, D6, and string
  layout, D12); `d12model.py` (now usable for cparse) gives 18572 against 18584 bytes.
- Corpus (`stagecorpus.py ../cparse`): 235 of 255 inputs identical. The 20 others differ only in the .S file, by the
  heap contents that `putsym` writes for the never set `sfldb` of extern symbols (and, for structures that are never
  defined, the word behind `dimtab[-1]`); both depend on `_end` (the start of the heap), so they become identical only
  with the original data size (KNOWLEDGE quirks table).
- Facts found on the way: YYSTYPE has 4 bytes (yyval 0x62ce, yyv 0x62d4..0x652c, YYMAXDEPTH 150); member names are
  global in this C (a new union member `lval` broke `NODE.lval`); block clears rval; ARGINIT 16; DSIZE 129;
  D19 (operand order) in KNOWLEDGE.
- Function state: modcheck -f -r per module (frame and registers hidden): code1 9/23, scan1 8/14, pftn1 7/21,
  trees1 14/27, optim1 2/6, local1 3/16 (clocal rebuilt), sym1 16/24, comm1 9, vinit1 1/14 SAME; the other differences
  read as D1, D2, D3, D6, D7, D10, D14, D15, D19. findings.json has the names but no per-function notes yet.

## Next

1. findings.json per function (strictcmp on the d12model link once the data model matches better; notes and D classes).
2. Open source questions: defid (member block placed inside the dimension loop in the original), getpreg (the shared
   `return pregvar--`), genswitch size of the search table (`(unsigned)(n*SZLONG/SZCHAR)` compiles to srl, the original
   divides), lineid/locctr naming, `f3970` (used by the parser, purpose open), `bssgap1/2` (unreferenced words).
3. The grammar: derive cgram1.y from the yacc tables of the parser (yyr1, yyr2, yychk, yyact, ...; ~175 productions)
   and the actions from yyparse, then replace cgram1.s.
4. Split cplibc.s into the members.

## State 2026-10-31

- The grammar is rebuilt: `cgram1.y` = the V7/System III/Plexus grammar plus `lcon_e` (a long constant expression,
  `lcons`) for the three array dimensions; the original yacc makes every table equal to the original (yyexca at 0x00ce,
  yyact 0x0122, yypact 0x0a24, yypgo 0x0c8a, yyr1 0x0d02, yyr2 0x0e6c, yychk 0x0fd6, yydef 0x123c). The actions follow
  yyparse of the original (`tools/actcmp.py`: 135 of 178 actions equal with frame and registers hidden). WEGA changes:
  no precedence warnings, -F writes the names of called functions, `elist` passes every argument through `f3970`,
  `term LB e RB` builds SUBSR, switch values go to a TEMP in swtab[swx] (switchpart, adddef, swend use the offsets +1
  and +2), lregvar and the long autooff are saved per block, `bfend` at the end of a block, "void function %s" prints
  the name offset (quirk). `cgram1.s` and the cut are gone (tools/cutgram.py stays as the record of the cut).
- `bss.s` (only `.comm` lines, linked first) fixes the bss in the order of the original.
- Data model (`d12model.py`, `D6TABLES`): the size and layout of the original; strict comparison on the model: 36 exact,
  the others classified (findings.json). The `_lastbuf` word of cplibc.s was a number and is now `__iob+160`.
- Corpus 234 of 255; the rest: the heap-dependent .S fields (quirk table).

- Step 2 done: the libc part is split (`tools/cparse_libc_split.spec`, unlink.py; `make checkblock` links the raw own
  part with the modules and gives the image of /lib/cparse exactly). 19 shipped members (ar x), 19 known oldlibc
  versions and 8 new ones: fltioc 0_1, fdopen 0_1, ecvt 0_1, perror 0_1, errlst 0_1 (data only, pointers made
  symbolic with datafix.py), stty 0_3 (stty at 0xbc1c and gtty at 0xbc36), strcmp 0_1, length 0_3 (limit 0x4000);
  `libcbss.s` orders __sobuf before __sibuf. unlink.py: modules with data only. cplibc.s is gone.
- endopen/0_4.s (used by codgen, scodgen, popt) had the errno address of /lib/codgen written as a number
  (`cp %69C8,#2`); it is `_errno` now. The originals: codgen 0x69c8, scodgen 0x7036, popt 0x0ce8, cparse 0x4894, so
  the number was right only for the original codgen (our codgen link has errno at 0x69a4).
- cgram1.y has the SCCS stamp and the WEGA message texts ("fcn level error", "field outside struct", ...); the data
  model has the size of the original again (18584). Strict on the model: 29 exact (the parser functions are no longer
  "exact by cut"); classify.py: 154 D only (pattern-verified), 25 other differences, mostly instruction forms that
  patterns.py does not know (multl with memory operands, srl for unsigned DECREF, set for flags, xor in lookup);
  corpus 235 of 255.

## Next (2026-10-31)

1. The 25 "other differences": read each against the patterns (new D classes or source forms); the D19 swaps.
2. f3970 (every call argument passes through it; returns it): name and purpose; bssgap1/2.
3. The two files bss.s and libcbss.s are layout aids; with the old compiler the order should come from the first
   mentions in cgram1.o (check when the old cc exists).
