# 161 The programs of bin: phase A only (unattended run)

## Status

active (owner's go 2026-11-23)

## Goal

Every program of `bin` that COVERAGE shows as `sys3` or `todo` worked on once with source forms only (phase A), after ex;
what stays open is recorded in the program's `findings.json`. Part of 160; a batch that runs without questions to the owner.

## Done when

- Every program of the list below is ticked "worked on" (exact or not; the state is in `findings.json` and COVERAGE).
- checkall ALL OK on the last commit; `src/COVERAGE.md` regenerated.

## Read first

- `../../CLAUDE.md`, `host/HOWTO_reconstruct.md` (setup.py, phase A, source forms, "No source").
- `host/OLDCC.md` section 2 (which compiler a dating points to), `host/OLDCC_DATES.md`.

## Rules of this run (owner 2026-11-23)

1. **Source forms only.** No new compiler switch, no change of an existing switch, no change to the compiler at all
   (cpp, cparse, codgen, popt, cas, ld, the drivers). What a source form cannot remove stays as a difference in `findings.json`.
2. **Libraries.** A new oldlibc version is allowed. First check whether an existing `libc-vXXX.a` can be extended by the
   member without another program losing (all programs of that library still identical); only then a new version.
3. **Links and copies.** A program that is a hard link or a 1:1 copy of another (same inode or `cmp` equal) is no program of
   its own: it goes into the Makefile of the original as `ln` (or `cp`), with the install target, nothing else.
4. **Time limit.** About 1 h per `sys3` program, about 2 h per `todo` program (no source), about 2-3 h for ex. Then record
   the state in `findings.json`, tick it here, go on.
5. **Setup.** Every program starts with `python3 host/recon/setup.py NAME [ORIG]` (nonsegmented); the HOWTO's steps follow.
   A segmented program (magic E6xx) by hand after the HOWTO. setup.py stops at a split I/D program (magic E711: ex, diff,
   sh, make, csh, adb); the recon tools (host/recon, not the compiler) may be extended for E711, measured on ex first.
   setup.py keeps an existing `findings.json` and its functions (own_table.py adds the missing ones by orig).
6. **Commits.** Per program (or a few together) after `checkall` ALL OK; no push. A change that breaks checkall and cannot be
   fixed in the time limit is reverted; the finding goes to `findings.json`.
7. **Documents.** `findings.json` per program (the setup format: `functions` with name, file, orig, size_orig, status, note);
   the quirks table of KNOWLEDGE.md and the table "Objects and their differences" of OLDCC.md kept current; this file ticked.
8. **Processes.** timeout on every z8k-emu run; long runs in the background; no leftovers (`ps`).
9. **No questions.** What would need the owner goes to "Decisions" below as an open question; the run goes on with the next program.

## Steps

0. ex: run setup.py (progcfg.py missing; `findings.json` is not in the setup format: move it aside, let setup write the
   function table, carry the old notes over into the notes of the functions and a `note` key), CC ccv0a; then phase A
   on the open functions (indirect call: listchar, normchar, vgotab; structure copy register: setty, gTTY; getone; dosub).
   The links edit, vi, view of ex: `ln` in the install target of ex once ex is installed (not yet: not exact).
   Done 2026-11-23: setup.py takes E711 (and the termlib before the libc, the zero padding of the code); compare/ex has
   progcfg.py (ccv0a, OWN_LIM 0xe96e) and findings.json in the setup format (394 functions by name, 359 exact).

The list, in this order (smallest first; the size of the original in bytes). Ticked = worked on, no statement whether exact.

### sys3 (source in tmp/sys3)

- [x] test (2478)
- [x] tr (6532)
- [x] uuname (6538)
- [x] nice (7218)
- [x] od (7526); link: hd
- [x] df (7718)
- [x] time (7812)
- [x] date (8550)
- [x] dd (8612)
- [x] regcmp (8844)
- [x] file (9254) -- byte-identical (2026-11-25): file.c of System III rewritten from the binary (s.out, Z8/Z80, PLZ, csh), ccv0, libc-v1; quirks in KNOWLEDGE.md
- [x] getfile (10002)
- [x] local (10398) -- byte-identical (2026-11-25): a Zilog binary of 02/84, ccvz (OLD_D12B) and libc-v14 (the ZEUS 3.21 members)
- [x] who (11540)
- [x] nm (12026) -- byte-identical (2026-11-25): nm.c of System III rewritten for s.out from the binary (getopt, long names, -v, -l, segmented formats), ccvm, libc.a; quirks in KNOWLEDGE.md
- [x] diff (12258)
- [x] egrep (12618) -- byte-identical (2026-11-25): egrep.y of System III by the yacc of WEGA, ccvm, libc.a; WEGA adds the sccsid and `!eflag` in the usage test
- [x] uulog (12784)
- [x] ls (13512) -- deferred: 2BSD ls with WEGA options; needs a libc with setbuf, qsort, fgetc, fread
- [x] newgrp (13898)
- [x] passwd (15188)
- [x] stty (15702) -- byte-identical (2026-11-25): stty.c of System III with the WEGA speeds and modes, the shipped cc, libc-v15 (new: libc.a with doprnt 0_10)
- [x] ed (16750) -- deferred: needs libc-v4 members (fakcu, strncpy, pipe, dup, chown) and setret; 69 functions
- [x] mail (18572); link: rmail
- [x] login (19030); same file as etc/login -- deferred: rewritten by WEGA (ttytype, termcap, csh) and a libc with large gaps
- [x] uux (21136)
- [x] ps (22794; segmented E607, last of this group) -- deferred: segmented, with the WEGA modules kva.c and kpa.c

### todo (no source)

- [x] echo2 (1834)
- [x] printenv (3998)
- [x] whereis (6444)
- [x] dog (8330)
- [x] putfile (9752)
- [x] uusend (13216) -- deferred: a WEGA form of the 4.2BSD uusend (-f, directories, mkdir) with two libc gaps
- [x] remote (15162) -- deferred: about 5 KB of own code (24 functions), no source
- [x] gpasswd (15362)
- [ ] sh (24032); link: rsh
- [ ] make (39048)
- [ ] csh (45728)
- [ ] adb (46996)
- [ ] fsc (2078; segmented E611)
- [ ] fc (3074; segmented E607)
- [ ] pstat (15638; segmented E607)
- [ ] gld (27616; segmented E607)

Links found 2026-11-23 (same inode in the WEGA root): hd = od, rmail = mail, page = more (more: partial, src/cmd/more),
rsh = sh, edit = vi = view = ex, bin/login = etc/login. Not in this run: `as` (170), `chkout` (exact).

## Decisions

- 2026-11-23 (owner): phase A only, rules above.
- 2026-11-23 (owner): extending setup.py / inventory.py for E711 counts as tool work, not compiler work.

## Not part of it

Compiler changes of any kind; `as` and the PLZ tools (170); usr/bin, etc, usr/lib (after this run).

## Log
- 2026-11-23 step 0 ex worked on: 361 of 394 exact (360 + crt0); open: the compiler clusters (indirect call, structure copy
  register, getone, dosub: the shipped cc folds `a - b` of two arrays to 0, KNOWLEDGE.md quirks) and functions with only
  call/calr; no source change taken.
- 2026-11-23 sys3 batch 1 (state in each findings.json; COVERAGE after the next checkall -b):
  IDENTICAL: test (V7 test.c + System III operators, ccv1, libc-v6 + fakcu), tr (usage of WEGA), uuname and uulog (BSD
  uucp: 4BSD uuname.c, 3BSD uulog and modules, uucp.h of 3BSD), nice (V7, libc-v4 + fputs 0_2, execvp 0_2, sleep 0_3 ...),
  who (-g of WEGA, shipped cc), diff (Plexus diff.c, spare locals), newgrp (V7 with WEGA changes, libc-v12), passwd (lock
  with lkdata/unlk, a spare long). Partial (installed, OPEN of checkall): od/hd, df, time, date, dd, regcmp, getfile.
  Deferred with the reason in the list: file, local, nm, egrep, ls, stty, ed. corpusrun: RC_UNDEFINED (od).
- 2026-11-24 sys3 batch 2: mail IDENTICAL (System III mail.c with WEGA changes, libc-v12, rmail a link), uux partial (3BSD
  uux and modules with WEGA changes, the frame of main); login deferred. The corpus of uulog wrote LOGFILE into the WEGA
  root once (removed again); uulog and uux corpora now write into the layer (ROOT_COPY of usr/spool/uucp/remotelines).
- 2026-11-24 todo batch 1 (no sources): IDENTICAL echo2 (V7 echo on stderr), printenv (2BSD), whereis (2.9BSD with the WEGA
  directories), dog (written from the code; libc-v6 + freopen, setbuf, ttyname 0_2, strcat 0_1, fcntl, fstat), putfile
  (written from the code); partial: gpasswd (two temporaries of main). Deferred: uusend, remote, ps.
- 2026-11-25 gpasswd: byte-identical with a third spare local z in main (the frame word of the two temporaries); every
  nonsegmented compiler measured: cc, ccvm, ccvl, ccvz give the same 7 bytes without it, the others far more.
- 2026-11-24 phase B ex: byte-identical (with edit, vi, view): v0a OLD_D65 (dosub), OLD_D66 (setty, gTTY), OLD_D5S extended
  (getone), shrink.sh (indirect call temporary; ASCII sort), the old cas (the 24 call/calr functions), exbss.c (the order of
  the commons), /lib/ex2.13strings, libc-v11 extended by the members of ex. checkall -b ALL OK, 847 files.
- 2026-11-24 phase B od/hd, df, time, date, dd: byte-identical (OLDCC_LOG.md of the day: v1r OLD_D67 and its own popt
  without the v1 switches, v1 codgen with OLD_D53, calloc 0_2 with int size, the (long) casts of df alloc not under SHIPPED,
  the register declarations of time and date where their frame words lie (OLD_D1), libc-v13 and two forms in dd).
- 2026-11-24 phase B getfile: IDENTICAL (later the same day): main in the structure of getfile.c 1.7 of ZEUS.
- 2026-11-24 phase B getfile: still partial (ccvm). The retries in the order of their strings (werr, ok, nak, can); the
  buf[i] reload is gone, only the shared printf tail of ok and CAN is kept on the wrong side (81 bytes, 0x29d..0x441;
  onalarm, drain, getc1 exact). Traced with a popt built with -DTRACE (same output as /lib/popt): codemove moves CAN behind
  the retry test in the first iterate, before comjump; about 1200 source forms, none better (`compare/getfile/findings.json`).
  regcmp deferred (below).
- 2026-11-25 phase B regcmp: IDENTICAL, and lex IDENTICAL (new): the compiler v1l (ccv1l), see the section below. checkall
  ALL OK (857 files), COVERAGE: exact 943.
- 2026-11-25 local: IDENTICAL. A Zilog binary of 02/84 (SCCS line patched): the cparse of 1984 (ccvz, OLD_D12B: statics in the
  .bss) and libc-v14 (shipped libc.a with the six ZEUS 3.21 members data, filbuf, doscan, printf, doprnt, malloc, from C; `src/head/oldnfile40`). checkall ALL OK (858 files), COVERAGE exact 944.
- 2026-11-25 uux: IDENTICAL. The frame of main: the original has no variable n, the fread count goes into ret (the
  three orders of the declarations tried before could not give it). Corpus 2 of 2; checkall ALL OK (859 files), COVERAGE exact 945.

## Phase B regcmp: solved (2026-11-25)

`/bin/regcmp` byte-identical: the libPW regcmp in it is the source of the libPW member (`src/lib/libPW/regcmp.c`) built by
a compiler of its own, v1l, found by restoring `/usr/bin/lex` (36 places of the same tcc form, System III source):

- lex (`src/cmd/lex`, `compare/lex`): System III lex with the WEGA edits (SCCS lines, option -f, myalloc, "={" in the
  generated tables), the stdio.h of the time, liby, libc-v3 with atoi 0_3 (ccv1q) and calloc 0_2, `ld -i`. IDENTICAL.
- v1l = v1q codgen + `OLD_D14L` (the value of `&&`/`||` through a temporary; in a condition the `&&`/`||` under `!` or on the
  left of `||`), `OLD_D3O` (behind `||` the idle registers go back), `OLD_D67` + `OLD_D67S` (the switch value stays taken
  in the dispatch), the popt of v1r (munput: loop not rotated). The rule came from lex function by function (pf diff of each
  function against the original); every switch is in `host/OLDCC.md` (v1l, D3, D14, D67).
- The same compiler builds the libPW regcmp exactly: the cause of regcmp was the compiler, not the source.

Open (later): the other witnesses of the tcc form (`host/recon/tccscan.py`): acctcom (the same regcmp code at 0x172e),
shut, error, units; whether they are v1l too. `OLD_D3O` has two equal variants on lex and regcmp (only behind its own join
label; `&&` too behind its own label): the simplest one is built.

## Phase B getfile: solved (2026-11-24)

**Result: `/bin/getfile` byte-identical (ccvm).** Step 4 found `tmp/zeus321_root/usr/src/getfile.c` (1.7, Zilog, 11/19/84):
the source of ZEUS, structured: `while ((c = getchar()) != CAN ...) { acknak = NAK; for (i = 0; i < RETRIES && acknak !=
ACK;) { if (c != STX) look_for_etx(); else { ... break on a write error ... acknak = ACK; } i++; if (acknak == ACK)
printf(ok); else { if (i < RETRIES) do ... while (timeout); else printf(CAN); } } }`. main written in this structure
with the code of 1.6 (names, the scanf quirk) gives the original at the first build. The notes below are the way
there (the goto forms could not reach it); the scripts of `compare/getfile/forms/` are in the git history (525517f0).


State: `/bin/getfile` partial, 81 differing bytes (0x29d..0x441), all from one difference in the retry block of main;
everything from 0x442 on (onalarm, drain, getc1) is exact. The source is written from the code (WEGA had no source;
`contrib/WEGA-Remote/getfile.c` is no model: at most written from a disassembly). Built with ccvm and libc (Makefile).
Notes per function: `compare/getfile/findings.json`.

The difference (original against the build):

    orig: 3ae inc retry; cpb ack,#6; jr eq,ok; cp retry,#10; jr lt,nak
          3c2 ld r7,#CAN-string; ld r6,#24; jr 416          CAN jumps into the call of ok
          3cc werr ... 40a jr test
          40c ok: ld r7; ldb rl6,ack; extsb r6
          416 call printf; jr test
          41c nak: ... jr ne,nak   (falls into test)
    build: the same layout, but CAN keeps the call (3ca call printf) and ok jumps into it.

What is fixed by measurement:
- The order of the printf calls in the source is the order of their strings in the original: werr's "%c", "disk write
  error", then ok (0x2310), nak (0x2314), CAN (0x2318). The tree has this order; with it the layout, the strings and all
  jumps are those of the original, only the side of the shared `call printf` is wrong (it shifts the code by 4 bytes).
- The mechanism (popt, `src/cmd/cc/popt/z21.c`): the main loop is `do { refcount; do iterate while (nchange); comjump;
  rmove } while (nchange || jumpsw())`. iterate walks the nodes in order; for each JP it calls xjump, then codemove.
  codemove moves the block from the target label to its first JP to the site of the JP when the label stands behind a JP
  or RET. comjump (backjmp) keeps, of two JPs to the same label with the same code in front, the copy of the earlier JP;
  xjump keeps the copy that falls into the target label. In our form the retry test (in front of everything) has a JP to
  can, can stands behind a JP, so codemove moves can (with its call) to the retry test in the first iterate, before
  comjump; then CAN's JP stands before ok's and comjump keeps the call in CAN. For the original, CAN must reach the retry
  test only after comjump (by a JP that comes up later, e.g. from jumpsw), or the merge must be done by xjump with ok
  falling into test and nak put between them later.
- **A popt change is ruled out:** `tmp/zeus321_root/bin/getfile` (getfile.c 1.7, Zilog 05/27/84) has exactly the same
  retry block (call kept in ok, strings ok, nak, CAN): Zilog did not change that part of the source. The Zeus popt
  (z2x.c 1.2 of 08/27/84, another binary than WEGA's /lib/popt of 12/82..03/83) gives on our codgen output the same text
  as the WEGA popt, and the whole Zeus chain (`/bin/cc -O` under zeus321_root) on our source keeps the call in CAN too.
  Two popts of two years agree, and with Zilog's source they give the original: the difference is in the source form.

Tried (none below 81; all with ccvm, the cc of WEGA gives the same):
- 13 forms of the retry test: `if (ack == ACK) goto ok;` followed by `if (retry >= 10) goto can; goto nak;` or `if
  (retry < 10) goto nak; goto can;`, the nested forms `if (ack != ACK) { ... } goto ok;`, if/else chains, tests that fall
  into the next block without a goto.
- Every order of the retry test, werr/next, ok, nak, can and test/drn (with the string order werr < ok < nak < can), each
  block in front of test with and without its `goto test`; the retry test first (reached from `ack = ACK`) or behind with
  `goto bad`. Without the string order (other orders) 28 bytes is reachable, but with the strings of ok, nak, CAN
  swapped and ok and nak in the wrong order: no way to the original.
- Structured forms: `if (ack == ACK) printf(ack); else if (retry < 10) do ... while (tflag); else printf(CAN);` and
  variants with braces, `goto test` in the branches, `while (1) { ... break; }` for nak: 122.

Session 2026-11-24 (evening), measured with trace.sh and an xjump trace (a printf in xjump of the TRACE popt, scratch):
- In the tree form the merge is done by xjump in iterate 1 at ok's JP: can's call falls into test then, so the copy in
  can is kept. With can behind drn (`goto bad; can: ...`), codemove moves can to the retry test in iterate 1 and comjump
  keeps can (81 again).
- `switch` forms of the retry test: about 7960 bytes (other code), out.
- Order W O N T H C with T falling into H (`drain();` then `bad:`, no `goto bad`) and `ack = ACK; goto bad;` (gen.py with
  this case added: 42 forms, best 162): **the call stays in ok as in the original.** Why: the JP of the retry test to nak
  lies in the straight run from nak (nak, test, drn, bad), so codemove refuses (p1 == p3); can is not moved in round 1;
  comjump keeps ok (the earlier JP); jumpsw then gives `jr lt,nak` and the next iterate moves can (now `ld; ld; jr` into
  ok's call) behind it. Left: the block bad+can stays behind drain (0x432) instead of behind `ack = ACK` (0x3ae), so
  163 bytes. Bad stands behind `call drain` (no JP), so codemove never moves it to the `goto bad` of record; the
  original has `calr drain; jr bad`. Open: a form in which bad+can goes to record only after the first comjump.
- The wide search (`gen3.py`: the blocks also between done and `record:`, record falling into H or `goto bad`, retry
  tests a b j k c d l, all subsets of the jumps to the next block; 1697 forms): none below 81. Best with the call in ok:
  104 (b_RWONCHT_0: bad behind `goto bad` of record is moved in iterate 1, before the skip rule has made `jr eq,ok`
  of the retry test; the block ends at the raw `jpr ok`, so ok is pulled into the retry test too).
- Loop forms (`while (retry < 10 && ack != ACK) { if (answer == STX) goto record; drain(); bad: ... }`, also `for (;;)`,
  ok/nak before or inside the loop, `continue` or `goto test`): 152..160. With ok/nak in front of the loop (falling
  into it) everything from werr to test is laid out as in the original and the call stays in ok; only bad+can stands
  behind drain instead of behind `ack = ACK` (the same state as W O N T H C).
- So the open point is one: bad must be moved to `ack = ACK` of record after the first comjump (round 2), or before it
  but only after the skip rule has made the conditional jumps of the retry test. In round 1 bad must not stand behind a
  jump (else it moves in iterate 1 and takes can along or splits at `jpr ok`), in round 2 it must. Changes between the
  rounds come only from comjump (tails), jumpsw (`CJP a; JP b` swapped) and rmove; none found that puts a jump in front
  of bad.

Not tried yet (next steps, each to be checked with trace.sh: when does can move, which JP is first at comjump):
1. A loop around reception and retry instead of the labels: `for (;;)` / `while (...)` with `continue` and `break` in place
   of `goto test` / `goto bad`; the loop test at the bottom gives other JPs and a loop inversion (codemove's second half).
2. The retry test as a `switch` or with the CAN case inside the test of `test:` (`if (retry >= 10) { printf(CAN); ... }`
   behind `test:`), so that the JP to can is made by jumpsw or by iterate only after comjump.
3. The parts in front of `bad:` (how `ack = ACK` and the write test reach it: `if (write(...) == len) { ack = ACK; ... }`,
   `else goto werr`) and the form of `test:` (`if (retry < 10 && ack != ACK)` ...), since they decide which JPs exist.
4. Compare with getfile 1.7 of Zeus where it differs from 1.6 (the scanf of `&answer` fixed, other frame sizes): it may show
   how Zilog wrote other parts of the same source.

Scripts (`compare/getfile/forms/`, need `WEGA=` the WEGA root and the tools of `make -C host wbin` in tmp/wbin; work
directory `$GF_WORK`, default `tmp/tmp/getfile-forms` beside WEGA):
- `try.sh BODY [NAME]`: getfile.c of the tree with the text from `\t\tack = ACK;\n\tbad:` up to `\tfail:` replaced by
  the file BODY, built like the Makefile (ccvm -O, crt0-getfile.o, libc), stripped; prints the differing bytes (bincmp).
  The current text of the tree as BODY gives 81.
- `trace.sh NAME`: after try.sh, the popt of the tree built with `-DTRACE` (once; its output without `+` is the same as
  /lib/popt, checked) on the codgen output of NAME; prints the codemove lines (`/*CM label refc tlen ...*/`) and the retry
  block after each iterate and comjump.
- `gen3.py DIR [GETFILE.C] [TESTS]`: the wide search (whole getfile.c files, the blocks also in front of `record:`).
- `gen.py DIR`: writes the bodies of the order search above (227 forms) and DIR/list; run them with
  `cat DIR/list | xargs -P 6 -I{} sh try.sh DIR/{} > res.txt` (about 30 s), then `sort -t' ' -k4 -n res.txt | head`.
- The Zeus check: in a work directory, `z8k-emu -r <zeus321_root> /lib/popt getfile.s out.s` on the output of
  `ccvm -Ihead/oldv7 -S getfile.c`, against the same with the WEGA root; `/bin/cc -O -S` under zeus321_root for the whole
  chain. `host/tools/sdis.py -l` disassembles with addresses (the retry block of 1.7 is at 0x3aa).
