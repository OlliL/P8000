# Log of the old compiler work

Running notes on the old C compilers: one dated entry per round or measurement (hypothesis, measurement, result,
decision), appended at the end, never rewritten. The edited summary is `OLDCC.md`; the evidence per function is in the
findings.json of the programs. Entries before 2026-10-31 were collected from the earlier notes in KNOWLEDGE.md and
TODO.md when this log was started.

## Before 2026-10-30 (collected)

- 2026-10-27, scc -S on small test functions: the register allocation of the old scc (declaration order from r13
  downwards, pairs for pointers and longs, leaf functions in scratch registers).
- 2026-10-28, scc -O -S (scratch `tmp/layout`): if/else keeps the source order (the D15 note "the original puts the
  `n <= 4` branch first" was wrong); while/for and goto loops with the test at the head are rotated, a far exit does not
  prevent it, without -O the head stays on top (D7).
- cas, ranlib, strip, size, the oldcc libc members: classes D1 ... D14 read with `cmpmod.py -s` and the masked
  comparison; the old libc members of size, strip, ranlib, cas compared function by function with the shipped members
  (D14; `_filbuf` through a stack temporary, no class).
- codgen: classes from `classify.py` and reading; tshape and conval case orders changed to the original's (D6), conval
  differing blocks 17 > 14.
- popt: call forms of 68 functions against the storage class of our sources (D17, pattern only).

## 2026-10-30

- ld strict check (`strictcmp.py`): 51 of 59 exact, 8 findings with classes read from the strict diffs (ranlib_scan D2,
  read_symbols D1/D11/D3, place_sec D2, init_entry D3, load2td D15, dseek D2, chk_ranlib D3, ensure_buf D11). Lesson: the
  masked comparison and the pattern check had accepted wrong code and wrong labels (12 of 20 findings were source forms).
- ld read_symbols: frame 0x1c against 0x1a; with one spare word in front of the locals all offsets of the original.
- cparse (strictlayers): D18 new (never `@r15`); D19 measured in argregs, rmconv, clocal.
- Experiment (tmp/tmp/stage): cparse writes the initialisers and string literals as cas text into base.D with `.even`
  behind each string; D12 is a cparse difference.

## 2026-10-31

- cparse getlab, noinit (`cc -O -S`): the shipped cc reserves at least 2 bytes of frame in every function and stores a
  returned `crslab++` there; the old cc makes no frame (D1, from codgen).
- v1 started in `src/cmd/{cparse,codgen,popt}/v1`. OLD_D12 (cparse, string pool at the end of the file): the data of ld
  equal. Accepted.
- OLD_D20 (popt codemove): a jump to the next label does not end a block: ensure_buf, the return part of read_symbols
  exact. Second rule (go on behind a block moved from behind a jump): load2td exact, ld 53 of 59. Both accepted then
  (the second with one witness). Turning off the whole code motion: wrong (prologue and switch dispatch move too); "no
  other label in the block": wrong (reg_sect_addr).
- D1 round 1 (codgen): no return temporary unless a delayed x++ needs it: read_symbols right, 21 functions lose 2
  bytes. Not accepted.
- OLD_D3 (codgen, no rfreeall at the join of && and ||): init_entry exact, ld 54 of 59. Accepted (one witness then).
- D2 rounds 1 .. 7 (codgen): no merging in rsreturn: register jam; freed words in LIFO order: worse; the parameter
  count in the .I head: dseek exact, 18 worse; the parameter registers kept at labels: dseek exact, 14 worse; a label
  kind for loop heads (cparse) with registers kept at the other labels: 18 worse; parameter registers kept only at labels
  that are not loop heads, contents forgotten: 53; a merged pair put behind the other pairs: rr0 first, cas stops ("Cannot
  perform an indirection on r0"); in front of rr0: 45 worse. Measured mechanism of dseek: the shipped codgen frees the
  parameter registers n (r3) and sp (r7) at the label of an if (`rsfreeall(1)` at 'L', main3.c). D2 parked.
- Witnesses then: size with v1 byte-identical; strip data identical, 9 code bytes left; ld v1 corpus 146 of 146.
  About 20 rounds: owner review.
- Owner review: go on with D2, D3, D1 in codgen; the spare variables go when a D1 switch replaces them; ccv1 to be built.
- ccv1 (`src/cmd/cc/v1`, OLD_V1) and `make -C host oldroot`; ld built with ccv1: 54 of 59 (the same as the passes run
  one by one).
- OLD_D1 (cparse oalloc: register variables get frame places): the number of spare words equals the number of register
  variables in nearly every function of ld; the register declarations put at the place of the spares: ld 54 of 59 with
  30 functions without spares; `n` of warn_sym_redef and `u` of strip main were spares under another name. strip: the
  9 bytes were D1 (copy, 6 bytes) and a source error (`int c` in rcopy, 3 bytes); strip byte-identical with v1. Left: one
  frame word in main, parse_args, slookup, pslookup, index16 (register parameters instead of the spare: worse). cparse
  loses lxinit, moedef, clearst with OLD_D1: a later build (dates of the crt0 stamps). Accepted.
- Owner decision: the regression rule holds per compiler version.
- Dates: all programs the shipped cc rebuilds byte-identically are linked with libc members of 09/12/83 or later.
- D2 round 8 (codgen): at labels no jump has named yet forget the contents but keep the registers (switch tables marked
  as references): ranlib_scan exact, symreloc right after marking the switch targets, table16_entry and chk_ranlib worse;
  54. Not accepted.
- Test bench `witness.py` (System III sources of 26 programs 01/83 .. 07/83, 119 functions): shipped cc 27, v1 72;
  without OLD_D1 31, without OLD_D3 61, without OLD_D20 72 (the second rule costs 3: od offset, regcmp main, sort merge,
  touch main, uname main, units getflt, xargs main against ls main, tr main). Second rule of OLD_D20 removed; first rule
  neutral. Bench with the D2 label rule: 54 (refuted).
- /bin/mv (mv.c 2.2, built 07/26/83): main, pname, dname equal with v1 from the unchanged System III source, the shipped
  cc differs in all four.
- ranlib_scan as a `for` loop: exact with v1 (source form). load2td as the Plexus `for (;;)`: 28 lines; 7 forms of the
  flag block (if/else swapped, flag = 0 at the end, the put_int_buf call shared: 10 lines, ?:, continue): not exact.
- Bench with the stdio.h of the time (src/cmd/cparse/stdio.h): 79 of 119.
- place_sec: allocreg traced in a debug codgen: one register for `secorg[j] - q->tsize & 0xff00` in ours, the original
  takes a new one for the `and` (not in the `+ 255` case); 9 source forms (operands swapped, `~0xff`, casts unsigned, int,
  long): no change.
- chk_ranlib: the && form under OLD_D3, symreg only on idle registers: no change in ld, none on the bench; 5 more forms
  of the loop and the conditions: 12 lines.
- mv.c brought to the WEGA version (all 7 functions equal with v1): bench 82 of 120; the rest of the bench is WEGA
  changes of the sources.
- dseek: the two page tests as one `||`: exact with v1 (source form; second witness of OLD_D3). ld 55 of 59.
- rm.c, rmdir.c, write.c brought to the WEGA versions: rm and write all functions and data equal with v1; rmdir main:
  a third parameter that is only written is not spilled by the old cc (D4, one witness), `@r15` against `%0000(r15)`
  (D18). Bench 89 of 121.
- OLDCC.md and this log started (owner decision): the D knowledge moved out of KNOWLEDGE.md and TODO.md.
- v1 closed as a milestone (owner decision): OLD_D1, OLD_D3, OLD_D12, OLD_D20; strip and size byte-identical, ld 55
  of 59 (place_sec, chk_ranlib, load2td, read_symbols open), bench 89 of 121. Next: the date table of all originals.
- Date table (`host/recon/datemap.py`, `OLDCC_DATES.md`, 373 programs): crt0 stamp, own SCCS ids, libc 09/83 mark,
  D14 mark (older printf; cc and scc of 11/83 have it too, so it dates the printf, not the compiler) and D12 read from
  the data (packed/padded strings among the first 16 that decide). Packed up to csh 08/11/83, padded from sh 08/22/83;
  the compiler passes (cparse, codgen with sources of 10/06/83, popt, gopt, cpp, cas) packed; exceptions nroff 07/83
  padded, find, upkeep, stty late packed.
- Second mark (D1 read from a binary): the frames of the functions that save register variables: "slots" (v1:
  ld 70 %, strip, cp, ls, sort, ed, csh 64 %, sh 58 %), "loose" (shipped: ar, tar, make, cc 12 .. 39 %), "tight" (no
  unused word: cparse, codgen, popt, ranlib 07/82, vi 10/82). In datemap.py; timeline: tight up to 11/82, slots from
  01/83 to 09/01/83 (sh, rsh, kdb, fsdb already with the shipped D12), loose from 09/09/83. The passes are tight: built
  by the compiler of 1982, named v0.
- Test bench v0 (`witness.py -t v0`, 38 programs of 1982, 153 functions): shipped 15, v1 14, v1 without OLD_D1 15.
  Hunks: `ld r7,@r7` against `ld r6,@r7; ld r7,r6` (reuse of the operand register), `%0000(r15)` against `@r15`,
  `call` against `calr`, the switch dispatch, the return temporary (frames 2 bytes larger).
- OLD_D2R (allocreg: freereg first): 18, 3203 > 2096 lines; first for all classes: sdiff breaks (`exts rr7`: an odd
  pair), so word registers only. OLD_D18 (efcode, gen: `0(fp)`): 48. OLD_D1R (return temporary only for a delayed x++,
  the round 1 of D1 in v1): 60. Without OLD_D3: 61. OLD_D20 on/off: 61 both (1643 against 1624 lines): not in v0.
- callr is resolved by cas: the `call` of v0 programs for global functions (D17) is probably the old cas.
- Leaf functions (cal jan1): v0 keeps both register variables in scratch registers (r4, r5) without a frame and
  without the parameter spill; shipped and v1 put the first one into r1 and the second into r13. Not done.
- v0 set up: cmd/{cparse,codgen,popt,cc}/v0, ccv0, `make oldroot` builds v1 and v0. Checks: cc, scc identical; the
  normal objects of codgen unchanged; ld with ccv1 55, bench v1 89, bench v0 with ccv0 61.
- /lib/cparse with ccv0 (d12model, strictcmp): code 48896 bytes = original (shipped cc 49152), data equal at the same
  address 17472 (17170), strictly exact 38 (29): gained mkty, mainp1, lxtitle, bcon, openfiles, bstart, bend, fldal,
  noinit, tcheck, putstrs; lost savebc, psave.
- Leaf functions: the renaming of register variables to low registers is popt's procopt (the shipped one too); the
  register numbers in the .S file are r13, r14. v0's originals divide in rr0 and rr2 (counted: div rr0 68, rr2 135,
  rr4 1; v1 and shipped never rr0). Pair order of rsbuild changed: no effect (rsinsert sorts by rweight). rweight of r0:
  0 and 1 hang codgen, 2: 19/502, 3: 19/457 on 5 programs (base 18/470); full bench with 3: 62/1542. OLD_D2W accepted.
- OLD_D2D (int DIV/MOD: the dividend into r1 or the odd half of the wanted even register; the pair of a dividend in an
  odd register): jan1 23 > 8 lines; first form (any odd wish overwritten, pair reuse for every long op) 61/1569; the
  odd wish kept (sdiff put2 again equal); pair reuse only for DIV/MOD 62/1561 against 62/1542 without it: losses in
  crypt setup (+29, register shifts and the local address form D5), col main, paste main, sdiff main. Not accepted.
- OLD_D1F (popt: frame and its stores removed when nothing reads it after the renaming): alone 61/1621, with OLD_D2D
  61/1594. Parked with OLD_D2D.
- v0 with OLD_D2W rebuilt (make oldroot): bench v0 62/1542; /lib/cparse with ccv0: 38 strictly exact, code 48896
  (= original), data equal at the same address 17411; ld with ccv1 55; normal objects of codgen and popt unchanged.
- /lib/cparse as test bench (strictlayers on the ccv0 model): the most frequent forms D3 reload (29 in 18 functions),
  D2 register copies, D10 (6 functions).
- OLD_D10 (codgen match: entry 024 not for a long shift): the form of the original (`ld r2,#4; sdal`, right shifts
  with `neg`); cparse bccode exact; uerror, cerror, werror, syserr, where looked lost: a mask artifact (the address of
  stderr equals a function address of the model).
- Wrong code by OLD_D2R found in cparse copysym (`ld @r3,r3` for `*pp = q`, `ldir @r3,@r2,r3`): freereg returns the
  register of an operand that is still needed. Narrowed to STAR: correct but `ld r3,@r2` (the node is an OREG); STAR
  and OREG: correct, cparse 43 strictly exact, code 49152; bench v0 64/1466.
- cparse built with ccv0 (make, not the model): corpus 239 of 259 identical (as the shipped build): the behaviour of
  v0 codgen holds on cparse.
- OLD_D1R without any return temporary (no save of r2 around the delayed x++): cparse getlab exact (44), bench 64/1463.
- OLD_D3P (regforsym: the register of a name also when another one is wanted): cparse argdebug returns x from r7;
  bench 65/1462; cparse 44.
- D4 in v0, tried: spill only the parameters whose home is used (marked in oregfix and in clocal for &x; the first
  form without clocal made wrong code for g(&x)): cparse 42 (fldal spills its unused parameter in the original),
  bench 65/1468. Removed. The leaf functions of v0 (argdebug: frame 4, no spill; jan1: no frame) are open.
- Behaviour: cparse built with ccv0 passes its corpus (239 of 259) after each change.
- Register copies in cparse (ccv0): three causes. chkstr: the address is moved out of r2 before the load because
  the result is wanted in r2 (`ld r3,r2; ld r2,4(r3)` against `ld r2,4(r2)`; in order3, before allocreg). fincode:
  v0 passes a double parameter in r4..r7 (spilled with `ldm %0000(r15),r4,#4`), the shipped one in a floating register
  (an ABI difference of cparse getpreg and codgen; new, no D number yet). putbyte: the argument v of bcon(v) is
  loaded from its home into r7 by v0 (`ld %0000(r15),r7; ld r7,%0000(r15)`), our codgen copies it (`ld r6,r7; ld
  r7,r6`, genargs).
- OLD_D3S tried (popt rmove: keep a load whose slot is in a register, 1: always, 2: only right behind the store of the
  same register): cparse 43 both (putnames lost, putbyte not fixed: its cause is the argument above); removed.
- putbyte: OLD_D3N (parminit: a parameter not known in its register at all): cparse 48, bench 65 > 51 (kill main:
  `cp r7,#2` in the original). OLD_D3A (gencall: before the arguments forget every register content): cparse 46
  (putrefsyms, putnames lost); only the parameters forgotten: cparse 48 without a loss, corpus 239 of 259, bench 63
  (cron ex, cal cal lost: the 1982 programs take a parameter from its register for an argument). The passes and the
  1982 programs differ here.
- Owner decision: two versions. v0p set up (cmd/*/v0p, ccv0p, OLD_V0P in cc.c; codgen v0p = v0 + OLD_D3A); make
  oldroot builds v1, v0, v0p; cc and scc identical.
- OLD_D4R first form (oalloc: no home for any register parameter in a register): cparse 72 strictly, but the cparse
  built with it fails on every input ("syntax error" on `int x; main(){ x = 1; }`): block needs the homes of its
  register parameters between the others. Second form (argalloc: only those behind the last other parameter): cparse
  71, corpus 239 of 259, bench v0 68.
- OLD_D4O (parameter list walked from its end): cparse 76 (lxenter, lxmore, makety, block, unhide), bench 69.
- The pairs rr2 rr0 rr4 rr6 at the start of a function (rsbuild, under OLD_D2W): cparse 80 (dstash, addcase, chkpun,
  incode), corpus 239, bench 74. ld with ccv1 55; the normal objects of codgen and cparse unchanged.
- OLD_D3P under casts between word types (regforsym compares the type): cparse rstruct exact (81; scnames looked lost:
  a mask artifact), bench v0 74/1395, corpus 239.
- OLD_D3L tried (codgen: the known register contents at a conditional jump restored at its label when the label has
  that jump only and no fall-through; for cparse tfree1, which takes p from r7 behind the else label): cparse 77
  (walkf, irbrace, fixtype lost: the restored register is then taken as a temporary), tfree1 not fixed (its jump is
  not printed by cbgen). Removed.
- Register copies in cparse (ccv0p), the most frequent form `ld R,R` (gotscal `ld r3,r2; inc 4(r3),#1`): orderadd
  orders the NAME pstk with cookie 6, fixname inside match makes it the REG of the cached register and entry 060 (`ld
  A1,AL`) copies it; the shipped codgen does the same. OLD_D2C (match: such a REG in a scratch register is the result,
  its name forgotten): cparse 80 (3222 lines; sputc lost: its parameter in r6 is copied by the original); without
  parameters: 82 (3284), clocal still off: the copy takes r6 beside the parameter in r7 and holds the name, so the
  next use copies back into r7 and popt deletes that; the copy without a name (allocreg) gives `ld r6,r7` before each
  use, the original `ld r2,r7`.
- OLD_D2F first form (getreg: the rest of a split pair stays where the pair was, not sorted in): codgen fails on
  most cparse sources ("reclaim dest reg not free": rsreturn merges partners only in the sorted run). Second form
  (only the choice: a free word only when it weighs no more than the first free pair): cparse 89, 3132 lines; corpus
  239 of 259; bench v0 75, 1325 lines (D2C and D2F together).
- OLD_D2L (getreg: an idle scratch word holding only a name is taken when it weighs less than the word getreg would
  take; newsect `lda r2,9(r15)` right after `ld 12(r15),r2`): cparse 89, 2836 lines (yyparse 86 > 27, copysym 78 > 34,
  putsym 74 > 34; defid 140 > 180 is a moved block in the diff); corpus 239 of 259; bench v0 84, 1080 lines. The
  normal objects of codgen unchanged (match3, lcl3, allo3, regallo3 compiled with the shipped cc).
- OLD_D2Q (allocreg: a long through a pointer into the pair of its even address register; the word is given back
  with rsreturn first so that it merges with its free partner, rfree alone leaves it on rsuse): cparse 2719 lines
  (optim 164 > 114, gotscal, instk, doinit; econvert exact; addcase, ftnend look lost: the mask artifact %6554);
  corpus 239 of 259; bench v0 84, 1080 > 1085 lines: set in v0p only.
- cparse nidcl: the original compares the class with 4 (REGISTER), our source had EXTDEF (5): a reconstruction error
  in sym1.c, fixed (`int a[];` outside a function warned in ours only); nidcl 7 > 1 lines.
- OLD_D2N (allocreg: a loaded NAME gets its register without the name): cparse 92, 2669 lines, no function worse;
  also for autos loaded from the frame (OREG off r15): 93, 2602 (fhashv +1, optim +6); corpus 239 of 259; bench v0
  86, 1064 lines. Set in v0 and v0p.
- Still open in cparse (small): talloc has a frame word that nothing uses (6 bytes against our 4; two `?:` of TNEXT);
  lineid reads the register variable cp through the register of its assignment (`cpb @r2` against `cpb @r14`);
  psavel stores through a computed address (`add r2,#X; ldl @r2,rr0`); fixclass, tfree1 take a parameter from its
  register behind a label (`ld r2,r6`), we load it from the frame.
- OLD_D6T (cpir switch: values in front of the targets, the target from the cpir pointer; cparse record V, codgen
  main3.c and gswitch): first without a record the d12model data broke (+108 bytes: d6tables finds the target list
  by `ld rX,Lnnn+d(rY)`, which is gone); d12model now also takes the label behind the value list as the target list
  of a switch up to 3 cases. cparse 2492 lines, data equal at the same address 17275 > 17561; corpus 239 of 259;
  bench v0 86, 1004 lines. The normal objects of codgen main3, lcl3 and cparse code1 unchanged.
- OLD_D6C (cparse genswitch: the cpir table from 4 cases on): cparse 93, 2343 lines, code 48896 as the original
  (argregs, efcode a little worse: the block order behind the compare chain); corpus 259 of 259 for the first time
  (until now 20 inputs differed in the heap contents of the .S file); bench v0 86, 788 lines.
- OLD_D2T (the kind 6 register of a switch TEMP is a candidate of OLD_D2L): setreg used r3 in every body because r2
  held the switch value (`ld r2,r6`) until the compares; the original takes r2. cparse 98, 1732 lines (setreg,
  chkstr, opact, fwalk, lxcom, lxres exact; openfiles only a mask artifact); corpus 259 of 259; bench v0 95, 557.
- Long register copies in cparse (`ldl rr2,X; ldl rr4,rr2` against `ldl rr4,X`, 14 hunks): OLD_D2Q also when the
  wanted pair is that of the address register: cparse 1707 (cinit, cprint, doinit, endinit, getstr, talign, optim).
- OLD_D2X (allocreg: freereg first for a long binary result): first for every binary operator wrong code in bccode
  (the shift count of OLD_D10 got rr6: `ld r6,#4; sdal rr6,r6`); only `& | ^ + -`: no effect; every binary operator
  except the shifts: cparse 99, 1668 (defalign, ispow2, yylex); code 48640 against 48896; corpus 259 of 259; bench
  v0 557 unchanged: v0p only.
- Groups of equal effects in the remaining cparse functions (strictlayers): 20 functions differ only in the frame
  or the registers. 8 of them only in the cpir count register (r4 against r1): OLD_D6R. The first try had no effect:
  under OLD_D2T the switch register r2 is free again at gswitch (a body gave it back) and getreg offers it first,
  which costs one of the three tries; gswitch now takes it (getreg(2, reg)) before ra and rb. cparse 107, 1584;
  corpus 259; bench v0 98, 539.
- The native cc takes about 160 characters of -D options (17 switches of 9 characters: "Can't create
  /tmp/cc.-1824/lib/cpp"); codgen v0/v0p name only OLD_V0 / OLD_V0P, codgen/oldcc.h sets the switches. The codgen of
  v0 and v0p built that way is identical with the one built with the explicit list.
- offcon (frame 4 bytes larger in ours): argalloc gets the parameters in the order of declaration; OLD_D4R skipped
  only the register parameters in front of the first other one. Only behind the last one: 1754 (offcon, buildtree,
  conval better, convert, fwalk, walkf, cast, makety, genswitch worse); both sides: cparse 108, 1438, no loss;
  corpus 259; bench v0 99, 531.
- Frame-only differences that look like source errors (not yet changed): hash (h and p in the other order in the
  frame), hide (one frame word less in the original: the unused `h`), lookup (one word more in the original between
  i and n). An unused local gets its frame place in the old cc too (initpreg with `int i, j;` is exact). talloc has
  one word more in the original although Plexus declares the same two locals: a compiler difference.
- Source fixes hash, hide, lookup (frame layout): cparse 111, 1400; corpus 259.
- The largest group (reload instead of copy, 12 hunks in 9 functions, ecode 4): the original copies a parameter
  from its register (`ld r2,r7`) behind labels and conditional jumps. cparse writes nparm 0 into the function head,
  so codgen gives back every register at a label. OLD_D3K tries: kept at every label and behind && / ||: 1488 (105;
  twords, fwalk, instk worse: the default label of a switch); only at labels a jump has named already: 111, 1383
  (fixclass, vfdzero worse: `and r6,#%0030` in the parameter's register, an ASG AND of order3 because istnode took
  r6 for a scratch register); the register of a parameter not scratch in tshape: 113, 1344; nor in istnode: 113,
  1312 (fixclass, ctype, sputc, ecode exact; ilbrace, irbrace only mask artifacts). Corpus 259 of 259; bench v0 107,
  495.
- OLD_D19A tried (argsort: a constant argument sorted behind the others, for oconvert `ld r6,r13; ld r7,#%006E`):
  cparse 113 > 77 (yylex, yyparse, genswitch much worse). Removed; the argument order of oconvert is open.
- Parameters copied into register variables (strictlayers, the first copies of all functions): only getpreg differs
  (r13 < r7, r14 < r6 in the original): a source fix (the parameters declared t, p); getpreg 24 > 8.
- getpreg's rest: `cp; jpr lt,L386; jpr L375; L386: jpr L363` from codgen; the original popt makes `jr ge,L375`, ours
  follows the jump to a jump first (L386 > L363), so the turn-round no longer applies and codemove moves the block of
  L375. OLD_D20S tried (popt iterate: the turn-round in front of the jump to a jump, instead of behind): cparse 1296 >
  1311 (yyparse: a turn-round now missing); in front and behind: 1298 (getpreg exact; gotscal, yyparse worse: the
  loop test copied to the top). Parked (neutral); OLD_D20 of v1 in v0p: 1296 > 2049.
- fhashv: the original saves r14 and gives n no home: source `register n;` (frame equal, 18 > 13).
- main `ld r2,@r3` where OLD_D2R took the address register r3: OLD_D2V (the address register only when it weighs no
  more than the word getreg would take, free or idle as under OLD_D2L): cparse 116, 1194 (argalloc 76 > 6, main
  exact); corpus 259; bench v0 108, 429.
- sectfull spills only rr4 (n), not r7 (sec): codgen's `?:` (reader3 QUEST) gave back every register. OLD_D3K there
  too: no spill at all; word registers only at `?:`, `&&`, `||` and at labels too: 1193 (zecode worse: it keeps the
  long parameter rr6 at its labels); word registers only at `?:`, `&&`, `||`, every class at labels: cparse 117,
  1188; corpus 259; bench v0 108, 429 (unchanged).
- Source fixes cisreg (t not register: the original saves no r14) and tyreduce (a long local for the dimension:
  `ldk r2,#0; ld r3,r12; ldl 0(r15),rr2; ldl rr6,rr2`): cparse 119, 1161.
- Prologue differences of all remaining functions (frame size, saved registers): source fixes vfdzero (register n),
  tcopy (register r as in Plexus), ncopy (register q, p and an unused i), argregs (n = argregs(p->left) first),
  beginit (no al), endinit (no tsz), falloc (s local to the ENUMTY block: the original reuses its place for a
  temporary): cparse 122, 1094; corpus 259.
- The original copies the value of `(sz = X/8) > 0xfffe` into a new pair (nidcl, beginit, endinit; 3 times, never
  for words): OLD_D2A (match: behind a long ASSIGN in a value context getreg + rmove, the copy keeps the name):
  cparse 125, 1087; corpus 259; bench v0 108, 429 unchanged: v0p only.
- Frame maps (the frame offsets each function uses, original against ours): source fixes tymatch (register p1 as in
  Plexus: an eighth register variable lands in the frame), argdebug (one register variable), buildtree (one more
  word at the function level; the STREF block in the order cp2, cp1, a word, j, k), defid (one more word), efcode
  (an unused long; i in front of p): cparse 126, 997; corpus 259. A search over all permutations of the register
  variables found no further swapped declarations.
- Source fix ispow2: `for( i=0; c>1; ++i ) c >>= 1;` (the original shifts before it counts; Plexus has the other
  form): ispow2 exact.
- Order of delayed increments (`*q++ = *cp++`): newsect (autos) increments q first in the original, exname (register
  variables) p first like ours. OLD_D4L tried (delay2: the left operand first): newsect better, exname worse.
  Removed; open.
- D11 in cparse (popt tails), parked after three rounds: OLD_D11B (comjump keeps the later tail): cparse 989 >
  1812; OLD_D11X (xjump on a jump before the jump to a jump is followed; cinit: the NAME case jumps into the call of
  the L225 case that falls into the label, as in the original): 993 (cprint, newsect worse); OLD_D11X only when no
  other label stands between the instruction and the target: unchanged 993. The rest of cinit and cprint is the
  order in which codemove and comjump place the blocks. Removed.
- Group `ld r3,rX; exts rr2` / `mult rr2` (adddef, icons, upoff, falloc: the original takes rr2, ours rr0 or rr4):
  OLD_D2E for SCONV (the pair of the even operand register): 1118, worse; for MUL and SCONV: 1092; for MUL only
  (OLD_D2M): cparse 128, 961 (adddef exact, swend, genswitch better); corpus 259; bench v0 108, 418. icons and upoff
  are open (icons: the SCONV operand comes from an assignment; upoff: the operand is r1).
- OLD_D2P (OLD_D2L for pairs): cparse 129, 906 (inforce exact; defid, putsym, dclstruct better; buildtree +2);
  corpus 259; bench v0 109, 407. upoff still takes rr4 (its rr2 is not an idle pair: open).
- Source fix getmem: `n += 1; n &= ~1;` (the original `inc r14,#1; res r14,#0`; `n = (n+1) & ~1` goes through r2):
  cparse 130.
- yyparse, instk take rr2 for a long where we took rr0: r2 and r3 were idle words holding names. OLD_D2P extended:
  two idle words of a pair lighter than the first free pair are given back (they merge): cparse 131, 864 (swend,
  icons exact; putsym 14 > 46: the merged pair then stays idle and words go to r0, r1); for a word also an idle
  lighter pair is given back and split: cparse 132, 780, no loss; corpus 259; bench v0 109, 400.
- upoff `multl rq4,rr2` where we took rr4: rallo wants MUSTDO|4 for the right operand of a long MUL. OLD_D2U
  (no wish): cparse 133, 756 (upoff exact, falloc 16 > 4, dclstruct); corpus 259; bench v0 109, 396.
- yyparse: the original stores the result of talloc() twice (`ld 0x130(r15),r2; ld 0x132(r15),r2`, the second
  never read) in the switch action; open.
- mkdope computes an index in r0 (cannot index: `add r0,#X; ld r1,r0; ld @r1,r2`), the original in r3: r3 idle with a
  name had the weight 3 of the free pair rr0. OLD_D2G (equal weight is enough for an idle word): cparse 135, 676;
  corpus 259; bench v0 112, 368.
- psavel and lineid: the original reads a register variable through the scratch register of its assignment
  (`*(long *)ip = l` as `ldl @r2,rr0`, no `ld r14,r2`). psavel with `ip = &paramstk[paramno]` in the source: 3 > 5
  (`ld r14,r2; ldl @r14,rr2`). OLD_D3R tried (the scratch register of an assignment to a register variable keeps
  its name, regforsym takes it): no effect on psavel (the ASSIGN target is no REG at match), argdebug and falloc
  worse, and copies of a register variable would carry its name too (stale values). Removed with the source change.
- D5 in cparse (types, dclargs): `t[i]` of a local array is SUBSR(PLUS(REG fp, ICON off), index) in codgen (mapnm has
  resolved U& already, so fixfld returns early). OLD_D5S (SUBSR: PLUS(PLUS(index, ICON off), REG fp)): first for
  STREF too, a codgen error in fhashv (`u.l % n`: register jam); SUBSR only and no constant index: cparse 136, 652;
  corpus 259; bench v0 114, 318. Open: newsect `&sectname[sec][k]` (a global array: the original adds the name last
  too), dclargs (the address in r2, the load into the wanted r6).
- OLD_D5S for global arrays tried (the outer SUBSR of `sectname[sec][k]` rotated so that the constant comes last):
  with a named constant no effect (mapnm clears the sname of an ICON); with every constant not a frame symbol cparse
  652 > 682 (yylex, gotscal, lxres worse) and newsect unchanged. Removed; newsect open.
- dclargs computes an address in r2 and loads into the wanted r6, ours computes in r6. OLD_D2S tried (rallo: STAR
  passes no wish to its address): cparse 652 > 998. Removed.
- Source fix tymatch: register o, u in front of t1, t2, t, tu (o r13, u r12 in the original): exact; cparse 137, 528.
  The permutation search over the register variables (now up to 7) finds no other function.
- resetbc and defalign spill a parameter the original does not: the last label of the function (the return label of
  cparse, named by no jump) makes OLD_D3K forget the parameter, so regargs spills it. twords shows a label of the
  same kind behind which the original reloads from the frame. Open (a spill rule that knows whether the frame copy
  is read).
- OLD_D3K: the rsfreeall of a label no jump has named waits for the next record and is left out when that is the
  end of the function: cparse 139, 525 (resetbc exact); corpus 259; bench v0 unchanged (114, 318). defalign still
  spills (its last label has code behind it).
- popt (owner: work on popt): OLD_D11N tried (no comjump at all): cparse 525 > 1202, code 50176 bytes: the old popt
  has comjump. oconvert as a pure layout case: its return copies were merged because our source ends with
  `return(p);` behind the cerror; the original has no code there. Source fix: oconvert 4 > 2.
- OLD_D20S again on the current state: 523 > 541 (getpreg exact; gotscal: the loop is not rotated any more, the test
  stays on top; yyparse: `jr eq,A; jr B` stays where the original has `jr ne,B`). With the loop inversion accepting
  a label of the same place (L269/L267/L10037): unchanged. Parked again. The block rules of popt need a tool that
  maps the blocks of the original onto the raw codgen output (the order the old popt chose) before more guesses.
- Owner question: are the differences gopt (cc -Ol, -Or)? Measured with the shipped cc on the cparse model: -O 29
  strictly exact, code 49152; -Ol 20, code 56320; -Or 16, code 58112 (ours 139, 48640; the original 48896). gopt makes
  the code much larger: /lib/cparse was not made with gopt.
- popt OLD_D20C (from oalloc, raw `[A ... jpr eq,T] L1: [B ... jpr ne,Y] T: [then ... jpr L300] Y:`, original `[A ...
  jr ne,L1] [then ... jr L300] L1: [B ... jr eq,T] Y:`): first form looped endlessly (the move undid itself);
  only a block behind p: cparse 477 (oalloc exact, optim 85 > 58; conval +8: a diamond `ldk r2,#1; jr E; Y: ldk
  r2,#0; E:`, argregs +5); no diamond: 469; no unconditional jump to T (argregs `goto full`): 464, no loss; corpus
  259; bench v0 318 > 330 (join main): v0p only. blockmap.py helped to see the moves.
- popt, the dimension loop of defid (79 lines): traced with stderr prints in codemove/xjump. Ours (shipped order of
  iterate: xjump, then codemove): the back jump `jpr L153` is cross-jumped onto the store `ld temp,r2` of the
  initialisation (L20000), the loop is inverted, the test block then moved up to the entry, and the second
  inversion fails (its test is 28 nodes from L20000, the search goes 24). The original inverts the loop first and then
  cross-jumps the entry onto the store at the bottom (`ld r2,6(r15); jp 27A6`). lxmore has the same pattern, but there
  the shipped order (xjump, inversion, move up, second inversion) gives the original exactly.
  OLD_D20X tried (codemove before xjump for a jump whose target is not behind a jump): defid 79 > 13, lxmore 0 > 14,
  lxtitle 0 > 22, fixclass, talloc worse: 464 > 444 (137 exact); only for a jump with no label in front: unchanged.
  OLD_D20N tried (a longer reach for the test: 29 nodes: 552; counting instructions only, 26: 574, 24: 584).
  Removed; the rule that tells defid from lxmore is open.
- popt xjump turns the instruction in front of the jump into the new jump and leaves the old jump dead behind it;
  iterate then calls codemove with the dead jump. OLD_D20Y tried (codemove with the new jump): cparse 464 > 494,
  defid unchanged. Removed. Under OLD_D20X the losers are tails of the `?:` of TNEXT (talloc: the original stores in
  each arm) and the `call; ld r2,2(r15); jr` tail of fixclass placed at the other occurrence: the cross-jump and
  comjump decisions depend on the order too.
- Owner question (2026-11-01): were defid or lxmore changed in the source to explain the order? defid in the PCC form
  (goto mismatch in the loop, the WEGA block behind the switch): 79 > 122, so our placement stays. But the test
  distance gave the rule: defid has its test 4 nodes from the head, lxmore 19. OLD_D20X with a short reach for the
  early inversion only (12 nodes): 430 (lxmore exact again; lxtitle, fixclass, talloc worse); 6 or 8: 426 (lxtitle
  +22, talloc +6). lxtitle traced: its inner for loop has getchar in init and step, the original cross-jumps that
  5-instruction tail first and inverts then. Early inversion only when the cross-jump takes one instruction at most:
  cparse 140, 398, only defid changed (79 > 13); reach 5..8 the same, 12: fixclass +4, 24: lxmore +14, fixclass +4.
  Corpus 259; bench v0 with OLD_D20X alone 318 > 323 (110): v0p only.
- OLD_D20S again on top of OLD_D20X: 398 > 416 (getpreg exact; gotscal +14, yyparse +12). yyparse: `cp; jpr ne,L467;
  jpr L462; L467: jpr L463` with L462, L463 behind (backwards), the original keeps `jr ne; jr`; getpreg has a forward
  jump at La. Only with a forward jump at La or no jump: 404 (gotscal +14: with no jump at La the early turn-round lets
  the same round follow the jump to the loop end, so the inversion misses its test); only with a forward jump at La:
  cparse 141, 390, no loss; corpus 259; bench v0 with OLD_D20S alone 318 > 333: v0p only.
- argregs (27): traced. comjump merges the one-instruction tails `add r2,r13; jpr L668` of the CM case and of
  `full:`, then codemove takes the jump into the middle of `full:` for a loop and inverts it, so `full:` moves; the
  original keeps both tails. OLD_D11M tried (comjump only for two common instructions): 390 > 660 (bsize, addcase need
  one-instruction merges: a whole block `ld r2,#16; jr`, a partial `call; jr`); two instructions or a whole block on
  one side: 536. Removed; argregs open.
- optim: the original inverts the goto loop of L141 (its first test behind the back jump); our codemove takes the
  move branch (the head is behind `jpr L140`), the block reaches the back jump, it gives up. OLD_D20L (then the
  inversion): first cparse 390 > 394 (optim 58 > 52, fortarg 0 > 10: a while loop whose head follows a return jump);
  only when the jump in front of the head leads into the loop: 384, but the corpus run hung (wrong code: p2 was one
  node behind at the goto into the inversion); fixed: cparse 383, corpus 259; bench v0 with OLD_D20L alone 318 > 337:
  v0p only.
- optim, the block of L154 (5 references, all `jpr cc,X; jpr L154`): our codemove moves it to the first of them, the
  original behind `ldk r13,#8; jp`. OLD_D20J tried (no move to a jump right behind a conditional jump): optim 51 > 30
  but 383 > 516 (rstruct, yylex, endinit, lxstr, cinit worse); only when another jump to the block is not behind a
  conditional one: 402 (lxcom worse, optim unchanged). Removed; open.

## 2026-11-01

- genswitch: the size of the search table `(unsigned)(n*SZLONG/SZCHAR)`: the original divides (`div` into a
  zero-extended pair). OLD_D21 (cparse optim: no shift or and for an unsigned division; codgen optim3.c: no shift for
  an unsigned DIV/MOD by a power of two): genswitch closer. Bench v0: cparse part neutral, codgen part 318 > 340: v0p
  only. Source fixes code1.c: `r = t = talloc()` in the order of the stores, `size = n+n` and the table size assigned
  inside the sectfull argument.
- OLD_D2A only when the pair is not wanted for an argument (`rall & 0x1000`); OLD_D2O (a long through an odd address
  register into the pair below it, the even half free or idle given back). Bench v0 with OLD_D2O unchanged: v0p.
- State: /lib/cparse 141 strict, 336 lines; corpus 259; bench v0 114, 318.
- genswitch left: `range = p[++n].sval - ...` with n a parameter: the original `ld r2,0(r15); inc r2,#1; ld 0(r15),r2;
  ld r3,r2` (load, increment, store), ours `inc 0(r15),#1; ld r2,0(r15)`; only this site in cparse, the other frame
  increments are `inc F(r15)`. After `swlab = getlab()` the original takes r2 for `t->rval = swlab`, ours reloads.
- genswitch `range = p[++n].sval`: the cookie of the ASG PLUS under the MUL is 010036 (INTEMP), gotscal `ix =
  ++p->in_x` 01312, newsect `*--cp` 6. OLD_D16A (memory increment not for INTEMP, frame slot only, the value through
  A1 with Zi): 336 > 333; without the frame condition 336 > ~1000 (resetbc, lxtitle, yylex ...); with STAREG instead of
  INTEMP newsect worse.
- genswitch `t->rval = swlab` right behind `swlab = getlab()`: codgen keeps nothing after the store (rsuse empty).
  Value first (setasg: order the right NAME before the address): 336 > 424, the original stores address first
  elsewhere (the search branch of genswitch, psave). Naming the stored register in general: ftnarg, lookup, copysym,
  defid worse (384). OLD_D3N (only for the store through a pointer of the next statement, no register allocated in
  between): genswitch exact, 142, 328.
- Found on the way: `opclass[]` had 24 entries, the original data has 26 (`0x4000, 0xe`); the 0x1018 table entries read
  behind the array (a global d3nreg placed there broke scnames and trees1). Fixed; the bench v0 stays 318, cparse
  with v0p unchanged.
- Corpus 259; bench v0 318 (OLD_D16A and OLD_D3N together 348: v0p only).
- Doubles: the original passes a double argument in rq4 (doinit `fldd r4,f0` behind the EPU load, prtdcon `ldm
  r4,8(r14),#4`, then the int argument in r3), fincode and fltlab store their double parameter with `ldm 0(fp),r4,#4`,
  isitfloat takes the result of atof with `fldd f0,r4`; ours used f3 for all. OLD_D22 (cparse getpreg, argregs;
  codgen argscan, argplace, regargs, lcl3, gencall): 328 > 316; the double compare template 052 without its `fld dbl`
  rounding (fltlab, buildtree): 302; doinit: a STAR operand through the EPU, only a NAME or OREG by ldm. Corpus 259
  (the shipped atof of the cparse link returns in rq4 too). Bench v0 318 > 292: OLD_D22 in v0.
- OLD_D3N widened: all word variables stored from the same register in one statement (`l = lab = getlab()`), used as a
  whole argument too (argplace), only in the next statement (counter in stmt; buildtree has two statements between
  the store and the argument and reloads), getreg forgets it only when it takes that register: prtdcon `ld r5,r2`,
  296.
- Source forms: prtdcon `if( sectfull( DATA, size = SZDOUBLE/SZCHAR ) )` (as in genswitch; the argument with the
  assignment is evaluated first, `ldl rr4,#8; ldl size,rr4; ldl rr4,#8; ldk r7,#1`); fltlab `d == p->fval` (the
  original loads d and compares with the member). cparse 147 strict, 286 lines; corpus 259; bench v0 292.
- Source forms: yyparse `ncopy( t = talloc(), *(NODE **)swp );` (the original also stores the call result into its
  temporary 0x132); treeeq `return( p->lval == q->lval && p->rval == q->rval );` (the `ldk r2,#0` of the && value).
- OLD_D3N for register variables (d3rsub in delay, after mapnm): every assignment from a scratch register first:
  336 lines > 400+ (bcon, addcase: after a call result the original uses the variable; lxenter, lxstr: after a load or
  a getchar `?:`); only the value of an operator without a call or `?:`: lineid exact; with psavel in the form `ip =
  &paramstk[paramno]; *(long *)ip = l;` (the saved, otherwise unused r14 of the original) psavel exact. cparse 151,
  277; corpus 259.
- Seen, not done: tfree1, rmconv, zecode take a parameter from its register at an else label whose jump came before a
  then part with calls (the state of the jump, not of the fall-through).
- OLD_D3J: the state of a conditional jump given back at its else label behind a jump: rmconv, tfree1 exact, but
  dclstruct 17 > 27: the join label behind the else part kept r7 from the fall-through while the then part had
  called (a wrong-code risk of OLD_D3K too); with the intersection of the states of all jumps at a named label
  dclstruct is back; buildtree 17 > 25 (an `&&` label with two jumps, the original reloads): the state comes back only
  for one jump. A restored parameter keeps its frame home (regargs; the flag 0x4000 on the node, cleared in rsbuild).
  zecode stays open (the long parameter inside an argument, OLD_D3A forgets it). cparse 153, 274; corpus 259; bench
  v0 290 (117): in v0.
- instk: `t==STRTY ? (int)dimtab[s+1] : 0` (the original clears only r3 in the else arm: the conversion is in the
  then arm). getstr: the long through the pointer wanted for r7 (want 7 for a pair): OLD_D2QO takes the pair of the
  address register (rr2) anyway: getstr exact, refid 24 > 20. talloc: an unused third local (`NODE *p, *q, *r;`, the
  frame word at 4); a return temporary for `return(x = y)` refuted (hide, argsize have none). cparse 156, 260; corpus
  259.
- refid `reftab[nrefs].r_id = nrefsym * 22`: the original multiplies in rr2, ours in rr0: r3 idle with the parameter p
  of the statement before, r2 free. OLD_D2PO (the odd idle half given back so that the pair merges): refid 20 > 6,
  dclstruct 17 > 5; cparse 234; corpus 259; bench v0 290 > 282: in v0.
- dclstruct `(sz / al + 1) * al`: the quotient in r3 multiplied in rr2 (ours copied it to r1, rr0). OLD_D2MO (with
  d2ofree): dclstruct exact, cparse 229, corpus 259; bench v0 282 > 291: v0p only.
- stref `p->type += PTR` (TWORD unsigned): the original loads, increments and stores, through a pointer with the
  address loaded again for the store: the a = a op b rewrite of order before offstar. OLD_D16B (no entry for an
  unsigned word in memory, order skips setasop): stref exact, cparse 220, corpus 259, bench v0 unchanged (v0p).
- conval: source order `p->type = q->type; p->rval = q->rval;` (the original stores offset 2 first); the long add
  behind the switch dispatch in rr2: OLD_D2PT (the TEMP registers count for OLD_D2P): conval exact, cparse 212,
  corpus 259, bench v0 unchanged (v0p).
- defalign: the original does not store the parameter n (used once at the start, then calls). Tried OLD_D3JF (a named
  label takes the state of its jumps and ignores the fall-through, which would also explain defalign's end label):
  212 > 300+ (fixclass, tymerge, refid ...). Removed; defalign open.
- exname: source `*q++ = ( (c = *p) == ... ) ? '$' : '_';` (the original stores the ?: value through rl2): 8 > 3.
  Left: `p = &ntab[s]` adds the parameter from the frame (`ld r2,#ntab; add r2,0(fp)`), ours `lda r2,ntab(r7)`.
- yylex: the comma forms `switch( lxchar=getchar(), (p=lxcp[lxchar+1])->lxact )` and `if( lxchar=getchar(),
  lxmask[lxchar+1] & ... )` (the original adds 1 to r14, the register variable, as lxstr does after its comma):
  yylex now as long as the original (2026 bytes), 4 lines left (jp/jr spans); tinit, talloc, tcheck, defid show the
  data address 0x7C08 (node) masked because one of our functions now starts there: mask artifacts. Corpus 259.
- dclargs: the address of a local array element for an argument computed in r2, ours in r6 (the wanted register).
  OLD_D5A (no preference for that address): dclargs exact, cparse 205; for every STAR with a wanted register much
  worse; bench v0 282 > 277 (118): in v0.
- clocal source forms: of the three `l->left` chains, the SCONV loop and the DIV/MOD case `p->left = l = l->left`,
  the char compare `l = p->left = l->left` (all 8 combinations measured: 27 > 17); the SCONV end `p = l; return( p );`
  and the PVCONV, PMCONV ends `p = buildtree( ... ); return( p );` (the original stores p and cross-jumps to the
  common `return(p)`): 10. Left: the frame place of a call result (4 against 8) and one block placed by popt.
  cparse 160 strict; corpus 259.
- clocal: `long val` declared in the ICON block of the SCONV case (the temporaries of the statements outside it start
  at 4, the '[' offset of cparse): 10 > 6, only the popt block placement left.
- buildtree, the WEGA member block of STREF: `cp2 = &ntab[STP(i)->sname];` (the original reads i from the frame,
  not sp from r8; then OLD_D3N passes cp2 from r3): 17 > 6. Left: the switch table and the constant 0.0 in the
  other order in the data, and the temporary of `buildtree( COLON, ll, lr )` at 0x12 (ours 0xC; the block offsets
  of the member block do not explain it: moving cp2, cp1, m out of it makes the frame 2 bytes larger).
- Delayed increments: codgen does the right one of a statement first (delay2). putsym `while( dimtab[i] )
  taglist[ntaglist++] = ((struct symtab *) dimtab[i++])->symno;` (i first): exact; newsect `for( ; *cp; ++q, ++cp )
  *q = *cp;` (q first): 4 > 2; left in newsect: `&sectname[sec][k]` adds k before the array address (variants
  `sectname[sec] + k`, `k + sectname[sec]`, `q += k` do not give it).
- defid `q->rval = (int) (idp = hide( p ));`: the original reloads idp for the store (`ld r3,4(fp)`); the ASSIGN
  template gives only RRIGHT. OLD_D3V (RLEFT too when a cast painted the assignment): defid 13 > 8 (the rest is the
  popt block of `dec 0x16(fp),#2`); for every word assignment much worse. Corpus 259, bench v0 unchanged (v0p).
- strictcmp (host/recon): an immediate that is a function address of either binary is now masked on both sides (a
  data address of the original equal to one of our function addresses counted as a difference, and the other way
  round): cparse 158 > 162 strict, 162 > 152 lines.
- cinit: `case ICON` in front of `case NAME` in the source (the original has the ICON block behind the default part
  that the dispatch falls into; the compare chain is sorted by value): 8 > 2. popt trace (z21.c `-DTRACE`, `+`
  flag: the node list after each iterate and comjump): the then part's `callr fprintf; jpr L227` with L227: `jpr
  L222`: our iterate follows the jump to the jump first, so xjump compares with the tail of L222; the original
  cross-jumps to L227 first (the then part jumps into the else part's call). OLD_D20K: cinit exact; for every label
  cprint +6, only for a label with one reference: no loss. cparse 163, 144; corpus 259; bench v0 unchanged (v0p).
- clocal: the switch on ml in the source order CHAR, UCHAR, USHORT/UNSIGNED, SHORT/INT: exact.
- falloc `switch( new<0?pty->type:p->stype )`: ours printed `L10006: ld r2,r2; jpr L130` (the ?: value assigned to
  the switch register r2 that holds it), so in the first round the then part's jump does not lead to a jump and popt
  moves the dispatch behind the else part; the original moves it to the then part. OLD_D6S (no self assignment):
  falloc exact; cparse 165, 134; corpus 259; bench v0 unchanged (v0p).
- defid: the loop step `dec 0x16(fp),#2; jp head` of the SMEMB loop stands in the original right behind the `goto
  mismatch` at the end of the class switch (in front of redecl), ours behind the loop (popt trace: our iterate
  moves the L159 block behind uerror in round 1, the step stays). The mismatch block in front of redecl in the
  source: 8 > 175. Open.
- gotscal `DECREF(pstk->in_t)`: the address pstk+12 computed in r0, moved to r1; the original loads into r0, ours
  into r1. OLD_D2R0: gotscal exact, cparse 166, 128; corpus 259; bench v0 277 > 278: v0p.
- argregs: the original keeps the `switch( n )` dispatch with its `ld r2,r13` at the end (reached by `jr ne`), ours
  moves it up to `L674: ld r2,r13; jpr L676`. Tried: popt OLD_D20M (a short block only jumps reach moves in front of
  its target instead of the target up): 128 > 900+; codgen OLD_D6V (the TEMP assignment of the switch value
  deferred to the SWTCH, the value computed at the dispatch): 128 > 900+. Both removed; open.
- yylex: the second getchar (`lxmask[ (lxchar=getchar())+1]`) is the assignment form after all (the original
  `inc r2,#1`), only the switch head is the comma form: 4 > 2. Left: `jp 21AA` over `ld r7,#_iob; call _filbuf`
  (12 bytes) where the same getchar at the top of yylex has `jr`: probably the old cas.
- refid, argalloc: the right operand of a long compare (`exts`, a negation) in rr6, ours rr4. A preference rr6 for
  it (rallo): 126 > 170. Removed; open.
- yylex `jp 0x21AA` (the only short `jp` in the whole original cparse, 12 bytes forward). WEGA Dienstprogramme Band
  B, 3.4.5 "Pseudobefehle": cas makes a JPR long when its jump optimization ran before the target label was seen; it
  runs early at a direct assignment of the location counter (`.=.+20`) or at a constant expression with the
  difference of two relocatable values with an optimizable jump between them. Probed with the shipped /bin/cas
  (tmp/tmp/castest): `.=.+N` in .code or .data, `.even`, `.word L6-L5` and `ld r2,#L6-L5` (L5, L6 around a jump, the
  JPR before them still open) between a JPR and its label: always `jr`. The shipped cas does not show the documented
  early optimization; the old cas that assembled /lib/cparse probably did. Candidate: a v0p cas with the rule of the
  manual; to be checked: it must make exactly this one jump long in the cparse output (and which construct of the old
  compiler output triggers it).
- Counter-check with the ZEUS cas (S8000, file date 12/1986, runs under the emulator): the same result as the
  shipped cas for every probe (`.=.+N` in .code/.data, `.even`, label differences in .word/ld/index, `jpr L+k`,
  1200 open jumps): always `jr`. Neither assembler shows the early optimization of the manual.
- But the originals show it: short `jp` in the own code of popt rmove (0x1F16 .. 0x1F5C: every open forward jump
  `jp ne,0x1FDC`, `jp lt,0x1FF6` long, from 0x1F6E on the forward jumps short again: an early optimization at one
  point of the text, as the manual describes), of scodgen (0x3666) and of cparse/scparse yylex (0x219E). The libc
  members have more (hand-written assembler). So the assembler of the old tool chain had the documented behaviour;
  the trigger in the old compiler output is still to be found (rmove gives the best evidence: the source between the
  last long and the first short jump).
- The trigger of the long short jumps, from the originals (constraints: every long short-range jump spans the trigger,
  no short one does): one trigger point per cluster, always at a `call`: popt rmove 0x1F68 (`call equstr`), cparse
  yylex 0x21A6 (`call _filbuf`), deroff 0x184E (`call skeqn`, a backward call; it explains both clusters of deroff,
  0x17C6..0x17D8 and 0x1834..0x184A), scodgen between 0x3666 and 0x3676 (two calls). The shipped cas (jpr.c, optim.c)
  keeps every undecided jpr/callr as a 14-byte itable entry and optimizes at the end of pass 1; an old cas with a
  smaller table would optimize when it is full (the manual's "optimization before the target was found").
  Undecided entries counted from the binary at the triggers: popt 249, cparse 255, deroff 508 (about 2 x 254). A
  simulation over all modules of popt and cparse with a capacity of 80 .. 520 and five ways of counting (calls
  all/forward/unknown/external/none) predicts no fit without false long jumps: the entry count of the old .s cannot
  be read from the binary exactly (or the rule is another one). Open; the call at every trigger and the "all pending
  jumps at once" pattern speak against a compiler rule (jumps to the same target are long before the point and short
  behind it, deroff 0x1834 `jp 0x1858` and 0x1852 `jr 0x1858`).
- The long short jumps, simulated on the real cas input (the `_old.s` modules of the d12model, the instructions
  aligned with the assembled objects for exact addresses; the decision rule of jprange in the shipped jpr.c):
  1. a table of undecided entries that is optimized when full: capacity 255 hits the yylex jump but also predicts two
     long jumps in trees1 (short in the original); with the gray zone of jprange (a known target whose distance can
     still flip) no capacity hits it;
  2. the itable segment (14 bytes per undecided jpr, 6 or 12 per symbol) optimized instead of grown at each 0x400,
     0x800, 0x1000 or 0x2000 step, cumulative or reset: at best 39 false long jumps.
  Refuted as far as the cparse input can tell. The trigger lies at a `call` in all four originals (cparse, popt,
  deroff, scodgen), always with only one `call` in the deroff window. Parked under the time box (rounds: manual
  triggers, ZEUS cas, binary counts, capacity on the real input, itable steps): yylex 0x219E stays as an explained
  remainder (the early jump optimization of an older cas, described in the manual, evidence in four programs, trigger
  unknown).
- defalign: `defalign(n) register n; { n /= SZCHAR; ...` (the original has no frame place for n: a register parameter
  gets no home under OLD_D4R, popt drops the dead copy `ld r14,r7`); the form with `register al` gives 9, without
  register 7. Exact: cparse 167, 117.
- The third pair: in the whole original cparse a long result needs a new pair while rr0 and rr2 are taken only in
  refid (`exts rr6`) and argalloc (`subl rr6,rr6`), both rr6; ours rr4. A preference rr6 for the right operand of a
  long compare (rallo): passed on to the operand, 117 > 157. The free list in the order rr2 rr0 rr6 rr4: refid and
  argalloc exact, but words split from it change (the cpir count r6 instead of r4: bdty, talign ...), 189. OLD_D2W6:
  only a pair request takes rr6 before rr4: 169 strict, 105 lines; corpus 259; bench v0 277 > 275 (v0).
- Note: make's built-in rule `.l -> .c` regenerates scan1.c with flex when a file scan1.l lies in src/cmd/cparse
  (it happened with a scratch listing); keep no .l files there.
- zecode `fprintf( dfile, "...", n*2 )`: the original takes the long parameter n from rr6 (known at the else label,
  OLD_D3J); OLD_D3A forgot it before the arguments. OLD_D3AW (forget only a parameter that is the whole argument):
  zecode exact, refid +2 (`refid( (struct symtab *) dimtab[p->sizoff+3] )` reloads p, used as an address); forget
  also below an indirection: 170 strict, 103 lines; corpus 259.
- exname `p = &ntab[s]`: the original adds the parameter s from the frame to the address constant; OLD_D3AW in match
  (a parameter right of PLUS with a named ICON is not taken from its register): exact, no other change; 171, 100;
  corpus 259.
- oconvert: `p->left = block( SCONV, q = p->left, NIL, INT, 0, INT );` (the original loads `ld r6,r13` before the
  constant for r7; the only such order in cparse, 25 the other way: the assignment in the argument goes first):
  exact. newsect `&sectname[sec][k]`: the original adds k before the array address; cparse optim with the plain PCC
  swap condition: no effect; codgen OLD_D5G (fixfld: SUBSR of PLUS(UAND name, index) with a variable index becomes
  (index + index) + address): exact, 173, 96; corpus 259; bench v0 275 > 303 (v0p).
- popt placement, round 1 (2026-11-01):
  defid: the original falls from the dims test into the mismatch block (0x2670) and has a real `jp mismatch` at the
  end of the class switch (0x2990) with the loop step behind it. The mismatch block in front of redecl (with and
  without `goto mismatch;` in front of it): 8 > 175, the block stays there: in the raw code the dims test is `jpr
  eq,L156; jpr L150` and the CJP over the JP is turned round before the JP could move the block. Kept in the loop.
  optim: the block of `else if( LCON(p) && RCON(p) && conval(...) ) goto zapright` stands in the original behind the
  end of the PLUS case (`break` = `jp 0x569C`, the return), falling into zapright: the jumping block moved in front of
  its target; ours moves zapright up to the jump. Same picture as argregs (`L674: ld r2,r13; jpr L676` in front of
  the dispatch). Trial: no block move for a label with more than one reference: 98 > 1752. Removed.
- popt placement, round 2: the TRACE build of popt (z21.c) now also prints every block move of codemove with its
  features (`/*CM Lx refc tlen tcjp tend jlen jcjp jlabrefc jpre*/`). Over all cparse modules: 283 block moves; the
  wrong ones (argregs L676, optim L154/L162, defid) have the same features as correct ones in exact functions
  (chkstr L325, clocal L128: a one- or two-instruction block reached only by jumps, a target with CJPs that ends with
  a direct jump). No separating feature in the node list. OLD_D6V again, only for a switch on a plain variable:
  96 > 427. Removed.
- Proposal: the passes codgen and popt were built by the same compiler (v0p); compiled from our sources with ccv0p
  and compared with /lib/codgen and /lib/popt they would give many more placement witnesses for popt rules.
- popt model (step 1 of the plan): src/cmd/popt/tools gets progcfg (MODULES z20..z23, model_link like `make popt`,
  D18, D6TABLES, STATICFUNCS: the static functions get global labels in the model so that strictcmp finds them),
  the d12model and strictcmp launchers, `size_orig` for the 68 paired functions (OWN_LIM corrected to 0x4422: z23.c
  ends with markregs, the libc follows); `oldcc.sh model popt`. First measurement with the current v0p: 1 of 68
  functions strictly exact, 6305 lines; code 23040 against 22272, data 4076 against 3308, bss 3214 against 3420.
  Kinds seen first: the data and bss addresses (decref, insertl differ only there: a layout model is needed first),
  register declarations (equstr: the original has its parameters in registers, absval not), and seven functions of
  our source without a counterpart in the original (getch, hashname, clearmem, heapreset, allocnode, newcode1, scan:
  their code is probably inline in the callers).
- popt model, the data (3308 against 4076 bytes), aligned byte by byte: at 0x39 the model has a 256-byte table more
  (the z20.c tables in another form or order), at 0x3c9 739 zero bytes more (an array initialized to zero in our
  data that is bss in the original, or a table of another size), the switch jump tables (.addr lists, D6) and some
  strings at other places. Next: the data and bss layout first (as for cparse: the tables of z20.c against the
  original data, the bss order), then the functions.

## 2026-11-01 popt model: data and bss layout, first functions

- The bss of /lib/popt mapped (bsspair.py, now also for entries without a range): the uninitialized arrays of our
  source are not all bss in the original. In the data (all three are function statics in the order of their
  functions): `tmpcc` (cclook) 0x2de, `tmpop` (oplook) 0x2fe, `labhash` (refcount) 0x31e; the buffers of repladdr
  `buf1`, `buf2` (50 bytes each) 0x808. In the bss as commons: `ophash` 0x159e, `conval`, `ccloc`, `conloc` (20 bytes
  each), and the stdout buffer of main is `_sobuf` of the libc (no own array). popt/bss.s (only .comm lines, linked
  first like cparse/bss.s) gives the bss the original order; `heapcur` 0x163c, `heaplim` 0x163e (alloc), main sets
  them `heapcur = heapbase = heaplim = sbrk(2)`; `debug` is a char (incb); `nrmove` has no counterpart (removed);
  0x1846 is a word of rmove not in our source (`_bssgap1` in bss.s).
- `nullcode` (the bytes at address 0 in our data) is not in the original: dualop, ovlap and markregs read through
  the null pointer. The static patterns of findword and findlong are initialized ("rxx", "rrxx"); regprefix has a
  static table of four strings and a switch on the type. d12model.py: an initialized char array (one `.byte` line per
  element) is no string literal and stays in place (cparse model unchanged, 173).
- output(): the cpir table and the string order give the case order CBRACE, JP, CALL, LABEL, DLABEL, CODE/DATA
  falling into default, JSW, SOB, EOFOP, KCOMMENT, 0; `sub` is a register; prefix() calls printf with the `?:`
  string (no `%s`). Behind output two functions nothing calls (0x08f8, 0x092a: stdout on the fd of stderr,
  dumpnodes or output, back): dumperr, outerr (findings.json; quirks table).
- stdio.h of the time (as cparse, popt/stdio.h): the old putc (store path first, extsb).
- The libc part in the order of the original (the libc.a order: freopen printf doprnt ffltioc filbuf setbuf flsbuf
  endopen data __callex cuexit atoi cnvt decimal length malloc isatty stty ctype_ strrchr strcat strncmp strcpy
  strlen, the system calls brk .. write, cerror; Makefiles: LIBCOBJS); strchr and strcmp are not in the original
  (no caller). zalloc (unused) removed, clearmem only for SPOPT.
- Small functions: the loop step `p += 32, i++` (findreg, findslot); register parameters (getnum `register char
  *p`, equstr), absval without register.
- Measurement: data 3308 = 3308, bss 3420 = 3420, code 22784 against 22272; 23 of 70 functions strictly exact
  (was 1 of 68), 5090 lines (was 6305); with equstr 24. Normal build (shipped cc) against /lib/popt on the popt corpus (272 inputs,
  without option, -l, -d): 816 of 816. cparse model 173 (unchanged).
- equstr: `if (!*b++)` gives the `testb @r2` of the original (`*b++ == 0` gives `ldb rl2,@r2; testb rl2`): 24.
- Seen, open: findcon `cpb @r14` where
  ours has `cpb @r2` after `ld r14,r2`; itoa_ r3 where ours takes r1.
- Further source forms (popt model 24 > 35 of 70, 4665 lines): natural `while (*s++); s--;` (the trailing `)` test
  reads the NUL: quirk), adrlen (a register copy p; the blank loop increments s: quirk; an unused frame word),
  findcopy (case TQ `r += 2` falling into TL; an unused frame word), setcc and savereg (the parameter itself as the
  register, `(*d++ = *s) == 0`), toofar (`n = ilen(q); sum += n;`, `if (sum < 256) return 0;`: whether the frame word
  of the call value is the source or a rule of the old compiler is open), shortjump (`register char *s`), copy,
  backjmp (non-register parameters with register copies: the homes and r12..r14 of the original), opsetup, cclook
  and oplook (pointer walk over ophash, `h++` as its own statement; oplook reads `o->name` without testing o: quirk;
  `h = ...; start = h;`).
- backjmp, open: the `&& p1->op == LABEL` test of the inner while stands in the original in front of the loop
  body (a popt placement, as the open cparse points).
- Are switches obsolete after the popt source fixes? (Owner's question: some D switch may hide an error of our popt
  source.) Normal popt against /lib/popt on the corpus 816 of 816. cparse model with v0p popt without one switch:
  all 173 / 96 lines; without OLD_D20C 172 / 155, D20X 173 / 162, D20S 172 / 104, D20L 173 / 103, D20K 172 / 98.
  popt model (35, 4665): without D20S 4688, without D20K 4718, without D20C, D20X, D20L unchanged (neutral there,
  not contradicted). All five stay. The codgen and cparse switches do not depend on the popt source.
- findbyte, findword, findlong: `(char)n + '0'` (the original narrows the int before the addition): exact.
  killreg: `makereg(n % 1, ...)` (the known quirk, now in the source), `char name[6]`. The two `if (n & 1) ... else
  ...` of the cases TB and TW are cross-jumped into each other in the original; the TW one in the inverted form
  gives the instructions of the original but popt places the block elsewhere (4676 > 4639 lines): kept the plain
  form, a placement witness. findcon (`cpb @r2` after `ld r14,r2` where the original tests `@r14`) is already in
  the codgen output (no popt): codgen v0p; itoa_, reverse (r1 for r3): codgen register choice. Left for codgen.
  popt model 38 of 70, 4639 lines; popt corpus 816/816.
- jumpsw (n reused for the label swap, t a register, no d2/subop temporaries), redunbr (`if ((t = p->ref) == 0)`,
  compare tested directly, `++p->ref->refc`: the statement `p->ref->refc++` gives with codgen v0p a dead
  `ld r2,6(r14); inc r2,#12` in front), fixldm (a switch: case 0 without the test of p->forw, quirk; case 1
  `*strrchr(...) = 0`; default `sprintf(++q, ...)`): exact. movedat: `cur` a register declared between the node
  pointers (the leaf renaming of popt follows the declaration order), the second loop dispatches with a switch on
  r1->op; left: the `cp r2,#0x1c` block stands in front of the loop head in the original (placement witness).
  setcon (parameters with register copies): left one `ld r7,r6` where codgen v0p reloads the home (the parameter
  register kept across `cpb; jr ne`: codgen D3 family). popt model 41 of 70, 4498 lines; popt corpus 816/816;
  cparse model 173.
