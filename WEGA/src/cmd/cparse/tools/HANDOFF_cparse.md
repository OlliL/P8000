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
- Start code: exactly `src/lib/libc/crt/crt0-codgen.s` (text 0x0000..0x002a, data word 0 at 0x0000; `_environ` is the
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
  _iob stdin _ptr/_base -> __sibuf, the initial break -> _end) are made symbolic by host/tools/datafix.py. libc bss:
  __sobuf (512, 0xb4f4), __sibuf (512, 0xb6f4), _sigtab (40, 0xb8f4). Verified: the object has the bytes of the
  original (text 8186, data 1780, 0 differing words outside the relocations).
- Open: split into the members (shipped members by ar x, oldlibc versions, new versions for the gaps) as in ld.

## Step 3: own code as one block (done)

- `make block` / `make checkblock`: crt0-codgen.o + ownraw.o (the own part as raw bytes, made at build time by
  `tools/rawblock.py`, never in git) + cplibc.o give /lib/cparse (code, data, bss equal; `tools/cmpimage.py`).
- A symbolic cut of the own part with unlink.py does not work: with separate I and D space and the own data starting
  at 0x0002, small constants are taken as data addresses (`#D0002`) and bss addresses as text addresses.
- Corpus harness (`stagecorpus.py`, progcfg CORPUS_*): the original cpp makes t.i from each C source of src/cmd and
  src/lib, then `/lib/cparse -E t < t.i` and ours are compared (t.I, t.D, t.S, exit status, messages). The block
  binary gives 15 of 15 identical; /lib/codgen as "ours" gives differences (the harness sees them).

## Next

- Step 4/5: sources from the Plexus PCC base, module by module; compile with the WEGA cc, link with cplibc.o, strict
  comparison (needs the function names in findings.json, then strictcmp/d12model).
