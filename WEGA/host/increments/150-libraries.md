# 150 The libraries and objects of /lib and /usr/lib

## Status

active (2026-11-18)

## Goal

Every library and object file of `/lib` and `/usr/lib` from source, byte-identical; the remaining assembler members of the
libraries of the old programs from C where a compiler gives them.

## Done when

- COVERAGE shows no library or object of `lib` and `usr/lib` as `sys3` or `todo`.
- `src/lib/oldlibc` and `src/lib/oldslibc` hold `.s` only where the original was assembler or the reason is written down.
- checkall ALL OK.

## Read first

- `src/lib/README.md`, `src/lib/oldlibc/README.md`, `src/lib/oldslibc/README.md`.
- `src/COVERAGE.md`, sections lib and usr/lib.

## Known

- Open in `/lib` (2026-11-15): `mcrt0.o`, `smcrt0.o`, `scnvt.o`, `sdoprnt.o`, `slibm.a` and others (COVERAGE); `/usr/lib`:
  e.g. `slibmp.a`.
- oldlibc: 13 `.s` (90 of 103 modules from C); oldslibc: 18 `.s` (1 of 19 from C), never tried from C.
- vl keeps two switches with one witness (`OLD_D53`, `OLD_D57`, owner's exception, `host/OLDCC.md` section 5).

## Steps

1. The list from COVERAGE; date each (`host/OLDCC_DATES.md`) to find its compiler.
2. One fresh session per library (HOWTO, "Libraries"; `host/tools/nightly.sh lib:libX`): `host/recon/libsetup.py` finds the
   sources (System III, 2.11BSD, 4.4BSD-Lite) and the compiler of each member; the session builds and matches them.
3. oldlibc, oldslibc: the `.s` from C (source forms first, a new switch last).

## Decisions

- 2026-11-18 (owner): sources of libraries also from 2.11BSD and 4.4BSD-Lite when System III has none (copied as System III);
  libl is the pattern; one fresh session per library, as for the programs.

## Not part of it

PLZ libraries and objects made by `as`: 170 when they need the PLZ compiler.

## Log

- 2026-11-17: libm.a, slibm.a from source (System III, v1; fputil.s PLZ/ASM from the original by s2as.py, which now writes
  fsetmode and fldctl with a pair), not identical: fputil.o exact, the 15 C members open for the compiler (floating compare
  without rounding, EPU temporaries; `compare/libm/findings.json`, `OLDCC_LOG.md`); WEGA's exp.c, jn.c, pow.c; archives in OPEN.
- 2026-11-18: libsetup.py (tried on liby, libtermlib, libmp: every member has a source), HOWTO "Libraries", nightly.sh lib:.
- 2026-11-17: liby.a, sliby.a byte-identical (System III sources, v1; WEGA's main.c returns yyparse()). libsetup.py: one
  source tree after the other, by name then by content (it took 4.4BSD's main.c before System III's libmai.c); the padding
  byte of an odd last member goes into arhdr_*.s, with the mkar.sh of libc.
- 2026-11-18: libm.a, slibm.a byte-identical (v1 with OLD_D61, OLD_D62; the host cparse reads floating constants with a
  port of the WEGA atof).
- 2026-11-18: libPW.a, slibPW.a byte-identical. WEGA forms in cat, regcmp, regex
  (register arguments), xalloc (seg: ssgbrk), fatal, setsig, xcreat, xmsg; move.c after pdp11/move.c. Compiler: OLD_D24, OLD_D25 and
  OLD_D2CW (new) into the segmented v1, OLD_D20 (block label named by at most 3 jumps) and OLD_D52 (A more than one instruction)
  narrowed (`OLDCC_LOG.md`).
- 2026-11-18: libmp.a, slibmp.a byte-identical (no System III source: 2.11BSD usr.lib/libmp without sccsid, v1 with the V7
  stdio.h; WEGA forms in gcd.c and mout.c). libsetup.py copies the sources writable.
- 2026-11-18: libtermlib.a, slibtermlib.a byte-identical (no System III source: 2.11BSD usr.lib/libtermlib with its CM_
  options, the older forms of tgetent, tnchktc, tgoto, tputs; v1).
- 2026-11-18: libcurses.a, slibcurses.a byte-identical (no System III source, 2.11BSD's is the later 4.3BSD one: 4.2BSD
  usr.lib/libcurses from TUHS (unix-history-repo, owner), initscr.c and scroll.c of 4.1cBSD; the shipped cc, scc without -O;
  WEGA forms in cr_tty, getstr, newwin and the printw/scanw family). libsetup.py copies the headers of the source tree
  (`.h`, `.ext`) with it.
- 2026-11-18: libscreen.a, slibscreen.a byte-identical: no source anywhere (web, local backups), io.c, mesg.c, mvcursor.c,
  scrn.c written from the members (v1 -O, V7 stdio.h). libsetup.py takes src/lib/NAME as the source directory when no tree
  has one. Web search for C-ISAM 1.03 (libcisam): nothing; libu8000 (disasm.c, the disassembler of adb) from the object.
- 2026-11-18: libu8000.a, slibu8000.a byte-identical: disasm.c (disasm(3)) written from the member, ccv1/sccv1 without -O;
  the two Library.SC differ (Library_libu8000.SC, Library_slibu8000.SC). libsetup.py: a compiler that runs past 300 s
  counts as no object (scc -O on disasm.c).
- 2026-11-18: libcisam (C-ISAM 1.03): the 14 sources and isint.h written from the members (src/lib/libcisam, not built).
  Exact with v1: isdef, isperror, util, key, lock (both), node, buf (nonseg), supp (seg). The rest differs in scratch
  register choices, one float compare (single precision, register against register) and popt loop layout: a compiler
  beside v1 (compare/libcisam/findings.json). v1 frame fact: the slot words of the register variables stand where the
  `register` declarations stand among the automatic ones.
- 2026-11-19: libcisam byte-identical (all 14 members of libcisam.a and slibcisam.a, cmp of the objects) with the
  sources (source forms: OLDCC_LOG.md 2026-11-18/19) and v1 plus `OLD_D63` (in the tree, checkall green) plus five more
  v1 switches kept as diffs outside the tree (host/libcisam_switches: OLD_D3W, OLD_D2PW, OLD_D3Q, OLD_D3H, OLD_D2CT;
  owner 2026-11-18). Without them 34/38 lines left (findings.json); libcisam not in src/lib/Makefile yet (the owner
  decides how the switches go in). Not checked yet: the archives (arhdr, Library.SC) with the diffs applied.
- 2026-11-19: the switches as the compiler v1c (ccv1c, sccv1c; host/OLDCC.md), libcisam in src/lib/Makefile (Makefile,
  arhdr_*.s, Library_*.SC, mkar.sh, COMPILER.md): libcisam.a and slibcisam.a byte-identical, checkall ALL OK. coverage.py:
  compiler and libc also for the libraries (the CCV*/SCCV* variables; libc n/a for an archive or object), the kernel
  parts, standalone and the versioned subdirectories (the rules of the Makefile); no exact binary or archive without both.
- 2026-11-19: the loose objects of /lib exact: cnvt.o, doprnt.o, scnvt.o, sdoprnt.o (not the members of libc.a: _hexchar
  for %X; v1 without -O, doprnt with the stdio.h of src/head/oldv7, the segmented doprnt from another revision under
  `#ifdef SEG`), mcrt0.o, smcrt0.o (PLZ/ASM, as): sources in src/lib, built in src/lib/build/nonseg and build/seg by
  src/lib/Makefile. The lib Makefiles name their compilers CC and SCC again (no CCV1, SCCV1). oldslibc/errlst/0_2.s is
  made from 0_2.c (sccvc -S): out of git. checkall ALL OK.
- 2026-11-19: /lib/clist byte-identical: src/cmd/clist.c written from the binary (no source known), ccv0p with the oldv7
  stdio.h, libc-v2 (now also fputs/0_1, filbuf/0_3, atoi/0_1 and data/0_1 instead of data.o; cpp stays identical);
  rules in src/cmd/Makefile (SIMPLE), crt0.list; corpus compare/clist (17 cases, identical).
- 2026-11-19: /lib/as2, asz2, as2d, asz2d set aside to increment 170 (owner): mostly PLZ/SYS code (170-plz.md, "Known").
- 2026-11-19: /lib/gopt, start (measured, nothing built): E611 segmented, code in segments 4 (65534 bytes) and 5 (50432),
  data segment 7 (31232; strings from data offset 0, the file header is 0x58 bytes), bss segment 8 (42496). 20 SCCS
  lines in link order: reader2.c 1.5, quads2.c 1.8, debug2.c 1.3, rdr2.c 1.3 (with `rdr` 2.4), comm2.c 1.1 (with
  `common` 2.4), wrtil2.c 1.7, alias2.c 1.5, flow2.c 1.6, dom2.c 1.2, init2.c 1.3, live2.c 1.9, ebbo2.c 1.8, lpinv2.c 1.10,
  lpind2.c 1.5, beta2.c 1.5, reg2.c 1.11, alias2.c 1.3 08/30/82 (a second module with the SCCS line of alias2), each
  with mfile2.h 1.7 03/04/83; newest source 03/04/83. No source found (System III, Plexus, System V, by module names and
  messages). `common` is the pcc file of System III (cmd/cc/pcc/mip/common) in a later form: "cannot recover from
  earlier errors\n" without "goodbye!", talloc prints the ops of the nodes before "out of tree space" (NODE 22 bytes,
  op at 0, FREE 100, TREESZ 350; node at <<8>>0x5292, lastfree <<8>>0x528e). tinit (seg 4, 0x4120) is equal with every
  segmented compiler with -O (scc, sccv1, sccvc, sccvk, sccvl): -O; the version not decided yet. Locals of the
  optimizer modules stay in the frame (no register variables seen in common). The start-up code at 0 is not the
  scrt0 of the system (`dec r15,#4; ldl ...,rr8`, `_environ` in <<8>>0). Next: setup as compare/sld (SEG, MODULES),
  the tools for two code segments, the compiler on common/rdr (the messages of codgen's rdr3.c, comm3.c are relatives).
- 2026-11-19: /lib/gopt, session 2 (infrastructure only, no source written): compare/gopt/progcfg.py (CODESEGS: segment 4 at image
  offset 0, 65534 bytes; segment 5 at 65534, 50432 bytes; addresses SEG:OFFSET), host/recon/own_table_seg2.py makes findings.json
  (599 starts: 368 call targets = proven, 231 candidates after an unconditional ret/jump, some of them switch-table targets, so
  a guess), host/recon/odis.py reads SEG:OFFSET (`odis.py f4_0042`, `odis.py 5:0114 0200`). 4:0042 is main (reads <<8>>050A,
  stdin, calls 4:C69E and 4:4120 = tinit). Module of a function: its data references cluster by a per-module word (4d7c,
  4d7e, 4d80 ... 4d90 in segment 7: 4:1d22..4:20b6 4d7c; 4:6c0a..4:8ed4 4d7e; 4:aaac..4:c538 4d82; 4:ca88..5:0 4d84; 5:2138..5:6bf2
  4d86; 5:6eda..5:9adc 4d90; segment 5 from 5:9bc0 reads 75xx..79xx = the late modules), a guess for the module map. Still to do:
  fncheck.py/bincmp.py/strictcmp for two code segments, the module map by function, then the source per module. Size of the
  job: about 116 KB of code in 599 starts, nothing to start from but `common` (pcc, System III); it is several sessions, not one.
- 2026-11-19: gopt tools for several code segments (progcfg CODESEGS): host/recon/segcmp.py (strictcmp for SEG:OFFSET ranges, our
  functions by symbol and segment; tested with the original plus a symbol table made from findings.json: 599 SAME, one changed
  byte shows as DIFF), fncheck.py takes SEG:OFFSET as LO, bincmp.py lists the code of every segment (blocks as SEGNO*0x10000+OFFSET;
  sld unchanged). Next: the module map per function, then the source per module (`common` first).
- 2026-11-19: gopt, comm2.c (src/cmd/gopt/comm2.c, gopt.h, ops_gopt.c; not built, not in src/cmd/Makefile): modmap.py gives every
  function its module (findings.json `module`, from the strings and tables it reads; 17 modules, alias2.c twice, beta2 and the
  borders between neighbours are guesses, 15 functions between live2 and ebbo2); comm2.c = pcc `common` 2.4, the relative is
  codgen/comm3.c; the operator table equals ops_scodgen.c but FLD 8 (ops_gopt.c). 23 functions + 7 tails: 21 equal under fncheck.py
  (masked) with sccvc (not v1: the register variables of leaf functions differ in every other segmented compiler), open:
  tprint (shared fprintf call, popt), ispow2 (`sdal` by -1 against `sral`). Not drafted yet: the area allocator of the module
  (4:4738, 4:4934, 4:4a48, strings "C Optimizer: out of space", "deallocation error in table area %d", "// sgbrk(%X)", the
  flag at 4d7c) and 4:3edc. fncheck.py: FNC_ROOT, FNC_CC, FNC_FLAGS (-Dstatic= for the static functions), FNC_EXTRA, FNC_OLDV7.
- 2026-11-19: gopt, the area allocator (taballoc, tabfree, tabavail in comm2.c: ssgbrk/mkseg/sgbrk of slibc, 11 areas, names guessed;
  tabfree open: one register/frame reload) and rdr2.c (the symbol table and intermediate file readers, relative codgen/rdr3.c, rdr 2.5:
  rdsym, readsyms, readtab, readnums, readmems, sortsyms, prtags, tagfind, mapn, mapsym, mapnm, mkhash, nlcheck, eread, rdin,
  rdin2; with the debug prints; mkhash open: one reload). 49 of the 599 starts (7 of them tails) have a source: 45 masked equal with sccvc, 4 open (tprint,
  ispow2, tabfree, mkhash). The compiler is sccvc: 34 of 40 drafted functions equal when measured (before eread, rdin, rdin2), every other segmented compiler 6 or fewer.
  Not an own module: 5:9bc0 on (slibc: _doscan, printf ..., ssgbrk at 5:c3a2) and probably 4:f5fe..4:ff78 (fread, fseek, getw): the libc
  members are at the end of both code segments; the data of gopt's last SCCS line (alias2.c 1.3 08/30/82) lies in front of the libc data.
- 2026-11-19: gopt, Makefile (src/cmd/gopt/Makefile: sccvc, `sld -i -e start -z -Md7 -Mb8`, -Mt4 for the first modules, -Mt5 from ebbo2;
  measured with synthetic modules: the shipped sld sends an overflowing text to segment 0, not 5, so the second code segment is named;
  a later `-Mt4` module goes by first fit into the free space of segment 4). The start-up code has an entry in crt0.list (gopt: no jump,
  no text, _environ in segment 8; mkcrt0.sh drops the jump for pad `-` also in scrt0 files). The link of the whole program is not tried
  (modules missing): open are the limit between the groups, where the libc members go (fseek, ftell, fread, getw stand at the end of
  segment 4 in the original), the libc revision (LIBC names slibc-v11.a like cas, not measured). reader2.c: main (the cases
  { } ] [ . L B C / are the blocks 4:00fc ... 4:0700), dotree, dotree2, cutree, pr, abortpass, passes, prtbl: dotree2, cutree, abortpass,
  passes, prtbl equal under fncheck (masked), main, dotree, pr open (popt block order and a register, notes in findings.json). quads2.c
  not started: 44 starts from 4:0b5e, the first (genquads, a switch on the operator) runs to about 4:14a2.
- 2026-11-19: gopt, quads2.c started with the small functions from 4:14a2: fold1, fold2, asgop2, asgsimp, tlookup, constlook,
  fconstlook equal under fncheck (masked); cmwalk (one instruction, popt reload of p from the frame) and operand (the first reload,
  `p->in.sname - symtab` makes scodgen stop with an rbusy error when p is in rr6, so a local is used) open. Not yet: genquads
  (4:0b5e..4:14a2, a switch), 1c88 (floating compare, EPU words), the quad list functions 1d22 .. 2bcc.
- 2026-11-19: gopt, quads2.c: the quad list. struct quad (30 bytes: next, op as a bit field of 8, flag bits, the operands c a b of 6 bytes with
  a flag bit behind the byte, the position) and struct qa with that bit (gopt.h; the bit fields explain `res @rr2,#7` and the
  `ldb rl2,rh2` reads). addquad, insertquad, delquad, movequad, delquads, normop equal under fncheck (masked); denorm one dead store
  open; mknode, bcon, mktemp2, newtmpno, labquad, jmpquad, mapall equal. Compiler facts: an undeclared taballoc is int (conversion
  code); `p->in.sname - symtab` makes scodgen stop (rbusy) with p in a register; `*(base + i) += 1` gives the original's code where
  `base[i]++` does not. Not yet: genquads (4:0b5e..4:14a2), map (4:231c..4:2a3a, the strings "// map: ..."), 4:1c88 (floating),
  the pieces 4:2c10 .. 4:2e8e belong to debug2.c.
- 2026-11-19: gopt, quads2.c: map (4:231c..4:2a48: ?: && || and the comma to jumps) masked equal; genquads (4:0b5e..4:14a2) written:
  all but three places equal, open: the `and r0,#%7f00` that every compiler puts behind the lda of a local structure in a structure
  assignment (the original has none: a finding for phase B, hidden for the check by fncheck.py FNC_DROP), `p3->b2 = 0` (rr0 against
  rr2) and the last copy of the tree (one shared ldir in the original). Frame equal. make part builds comm2, rdr2, reader2, quads2.
  Still missing in quads2.c: 4:1c88 (floating compare, EPU words) and debug2.c (4:2c10 ...), the modules after it.
- 2026-11-19: gopt, debug2.c (4:2c10 .. 4:2ec6): mkqnames, prquads, propnd, prstats masked equal, prquad open (the reload of the
  parameter behind the first conditional jump, as cmwalk and operand: one popt finding, phase B). The table qtab (84 quad operators)
  is read from the data; qop[] and qdop[] come from it. propnd prints floating constants with %f from a double of the table: the
  EPU words of the disassembly are `printf("%f", d)`. make part builds debug2.o too.
- 2026-11-19: gopt, wrtil2.c started (4:4a8e .. 4:6c0a, 49 starts): addrof, tlfind, wrline, treeeq, prtcounts, itfind masked equal;
  needustar and asgfold open (one return of 1 shared in the original / one displacement load). Not yet: 4:4a8e (writeil, a switch of
  about 3.4 KB up to 4:53ea), 4:53f0 (1114 bytes), 4:5d0e, the operand to leaf function 4:6050 with its blocks up to 4:657c, 4:65e4,
  4:687e, 4:6c0a is alias2. The strings of the module: "// writeil : quads to be converted to trees", "// mktree: adding quad to
  tree", "cleartrees: emitting tree", "cannot recreate tree (%d, %d)".
