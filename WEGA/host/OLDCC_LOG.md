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
- dest (`register int sub`, two switches: on p->op and on sub): left the jump layout of the EXTS case and a `sub
  r2,r3` of the original where ours has `sub r2,r2` (popt). makereg (all three parameters register, declared in
  the order n, type, buf: the declaration order fixes the registers; `if (n < 10) ... else { '1', n % 10 }`), ilen
  (switch on p->op without a temporary; the case order from the jump table: the 0 group, shifts, BIT/SET/RES, RET,
  LD, CP, LDM, INC group, CJP/JP; in LD the register branch first), isused (switch on reg[1], case 'r' before 'q',
  `return a || b ...;`: the value form gives the `ldk r2,#0` of the original), markregs (switch on *p with '@'
  falling into 'r'; `char flag`; the `rN(` recursion `markregs(p[3], b, a)` with a character as the pointer, as
  the original: now kept in src/cmd/popt, quirks table): exact. popt model 45 of 70, 4305 lines before markregs.
  With markregs 45 of 70, 4268 lines; popt corpus 816/816; cparse model 173.
- equop (no test of p1, p2: quirk; p1 a register copy; one `return 0` at the end, the loop leaves with break: the
  other `return 0` are cross-jumped to it): left `ld r2,r6` where codgen v0p reloads the home of p2 (the second
  witness after setcon: the parameter register kept across a branch). isreg (one `return -1` at the end, the
  `return n` in front of it; `isalnum(p[2])` without unsigned), compare (a and b themselves as register parameters,
  one accumulator moved into vb, `va *= 10; va += *b++ - '0';`), dualop (the copies inline, not strcpy; a register
  copy of p; declared s, d, then p, then q): exact. setreg (s scanned in place, a register copy s0 for memreg,
  `type` reused for the type of the memory register, `char name[6]`): left the block placement of the findcopy
  loop and of the scan loop (the original jumps from the loop end straight to `mem`, ours tests `*s` again) and the
  argument order of makereg (the value of `type` is still in r2 in ours: codgen). Both loop forms (for/while) give
  the same code.
  popt model 48 of 70, 4014 lines; popt corpus 816/816; cparse model 173.
- xjump (p2, p3 registers): left the loop inversion of the second backward loop (`p2 = p2->back` over labels and
  comments: the original has the LABEL/KCOMMENT tests in front of the step, entered from the end of the first loop;
  do-while and while forms give the same code): a placement witness with backjmp. iterate (no `start`: rp keeps
  the node; the loop that removes the code behind a jump reads `p->forw` every time; `++rp->ref->refc`),
  refcount (tp and n registers, declared p, lp, h, tp, n; `JP || CJP || JSW`; `(lp = nonlab(p))->op == 0`: only the
  assignment inside the expression gives the `testb @r2` of the original, the counterpart of findcon, where the
  original tests the variable after an own statement and codgen v0p tests r2: probably the old codgen does not
  reuse the value of an assignment in the next statement, a codgen candidate), rename_regs (no test of p->code;
  the "preceded by alnum" test on `q[-1]` of the search string, as the original: quirk; registers e, q, r11, p, r9;
  `char changed`): exact.
  popt model 51 of 70, 3793 lines; popt corpus 816/816; cparse model 173.
- D20 switches checked again after rename_regs (the change alters the behaviour): unchanged, all five needed.
- repladdr (registers rs, d, s, op; the two strcpy inline as copy loops with the pointers set before the RT2 test;
  `if (f) rd = ...; else rd = -1;`; `(rs = findslot(...)) < 0`, `(op = p->op) == CP`; `copy(1, buf1)` with two
  arguments: the original does not load r5): 93 > 2 lines. Left: where the two sprintf arms of the rs branch meet,
  the original loads `pfx` again from the frame (`ld r5,8(fp)`), ours takes r2; in the rd branch the original
  itself takes r2. `sprintf(..., pfx = regprefix(rs, t), rs)` is worse (12 lines: codgen v0p puts the value of the
  assignment into a temporary of its own). A witness for the codgen candidate "the value of an assignment kept in
  r2 for the next statement" together with findcon (the original tests the variable), refcount (only the assignment
  inside the expression gives the original's `testb @r2`) and setreg (argument order of makereg); open which
  rule separates the cases (the merge of the two arms in front of the call?).
- OLDCC.md (owner: every difference old against shipped belongs into section 3, with or without a switch): the
  open popt observations added to D2 (itoa_, reverse), D3 (the value of an assignment: findcon, repladdr, refcount,
  setreg; a parameter across a branch: setcon, equop; the old popt's register knowledge: dest) and D20 (xjump,
  backjmp, killreg, movedat, dest, setreg); section 4: the popt rows now v0p with D3, D12, D18, D20.
- input: `register int oper` without an `op` temporary, `char echo`; the echo loop is a switch on `oper & 0xff`
  (LABEL, EOFOP before OBRACE, default); the main switch in the order of the original's code (LABEL, WORD/ADDR
  falling into default, CJP/JP, COMMENT, 0 falling into CBRACE/KCOMMENT); `case COMMENT: oper = getln(); goto sw;`
  and the same for an empty line: popt cross-jumps the `call getln` into the one in front of the switch, which is
  where the original jumps (a `continue` goes to the echo test); `copy(1, s)` with two arguments: exact.
  popt model 52 of 70, 3587 lines; popt corpus 816/816; cparse model 173.
- codemove: p a plain parameter with its home, p1, p2, p3 the registers r14, r13, r12 (declared in this order):
  484 bytes, 1 instruction left: `--n == 0` of a frame slot is `dec n,#1; test n` in the original, codgen v0p
  (OLD_D16A) loads it into r2 first. `!--n` and `!(n -= 1)` give `dec n,#1` without the test (popt drops it),
  `(n -= 1) == 0` and `--n <= 0` are worse: the source stays `--n == 0`, a D16A witness (OLDCC.md D16, open).
- memreg: all variables registers (declared i, len; p; off, roff, n), no `reg` temporary (the index `TL + i` written
  at each use), `len = off = roff = 0`, the step `len++, p++`, `strncmp(s, regs[..], len)`, `n = size == 0 ? 1 :
  size / 8`, `atoi(&regs[TL + i][len] + 1)` (gives the original's `add r2,#regs+1`; `[len + 1]`, `[1 + len]`,
  `+ (len + 1)` do not): exact.
  popt model 53 of 70, 3295 lines; popt corpus 816/816; cparse model 173.
- usesreg: `strcpy(buf, str); str = buf;` (the register parameter str reused for the copy: no `ld r13,r6` at the
  entry, as the original), the cases in the order of the original's code (CJP/JP, PUSH, LD/POP/LDA, CLR, EXTS, RET,
  DIV, MUL, default, CALL): 248 > 177 lines. What is left is placement only: the instructions of both sides are the
  same multiset except jr/jp forms (7 conditional jumps long in ours, short in the original, and one jump more):
  the block after the switch (the ovlap test reached by the breaks) stands behind MUL in the original, ours moves it
  up to the first break (PUSH). A D20 witness.
- procopt: one register r14 for the loop index and the free register (no i), registers r14, p, s, ld1, ld2, r9, r8;
  `switch (op = p->op)` (the original stores the switch value into the first frame word), `struct rent t[9]` (the
  frame is 0x88), the cases in the original's order (the dualop group, the markregs group, LDM, CALL),
  `t[k].len = strlen(sprintf(t[k].s1, "r%d", n))` (the sprintf of the old libc returns the buffer; its value goes
  through a frame temporary), `sprintf(t[k++].s2, "r%d", r14++)`. Behind procopt a function nothing calls (0x36a6:
  `isreg(*s == '@' ? s + 1 : s)`, saves r14 without using it): regof (findings.json; quirks table). 308 > 48
  lines. Left in procopt: the address of `t[k].s1` (a member of an element of a local array of structures) is
  `k*10 + (t + 2)` then `+ fp` in the original, codgen v0p adds the member to fp (`lda r2,2(fp); add r3,r2`): the
  STREF counterpart of OLD_D5S (OLDCC.md D5, open); with it the register of `t[k++]` (r2 against r7). regof: the
  parameter taken from r7 for the test and reloaded for the call in the original, the other way round in ours (D3).
  popt model 53 of 71 (regof added), 2968 lines; popt corpus 816/816; cparse model 173.
- D20 switches checked again after procopt: unchanged, all five needed.
- ovlap: the register search written once as a macro SCAN() (19 uses): `st = text; while (*st) { if (*st++ != 'r')
  continue; sq = b1; while (*sq++ == *st++) { if (*sq) continue; if (*st == 0) return 1; if (isalnum(*st))
  continue; return 1; } }`; the inner loop as `while (*sq++ == *st++)` (the form `for (;;) { if (*sq++ != *st++)
  break; ...}` makes popt move the blocks: 1043 lines against 21); register pointers b1 = b + 1, b2 = b + 2 (r12,
  r11), `char b[8]`, `name` itself advanced (no p), registers st, sq, b1, b2, n, i; isalnum without unsigned.
  Behind ovlap two functions nothing calls (0x42f6: an operand starting with rh; 0x431a: 2 for SLONG, else 1):
  ishigh, nwords (findings.json; quirks table). ovlap, ishigh, nwords exact: popt model 56 of 73, 1621 lines.
  popt corpus 816/816; cparse model 173.
- D20 switches checked again after ovlap: unchanged, all five needed.
- rmove, analysis started: the instruction multisets of both sides differ in about 190 instructions of 645 on each
  side; the rest of the 1296 lines is layout. The content differences come from the variables: the original has
  five register variables (r14 p, r13 p1, r12, r11, r10: `ldm r10,#5`) and a frame of 0x38: word 0 (a second type
  from dest), b1 at 2 and b2 at 0x16 (20 bytes each), word 0x2a (t: dest values, also a findcon pointer), word 0x2c
  (isreg and findcon values: int locals that also hold pointers). No `op` (the switch is on p->op, the tests read
  @r14), probably no `n` (`ld r13,4(r14)`: p1 also holds the node in front). The bss word 0x1846 is a global that
  rmove sets to p at the top of the loop and nothing reads (rmnode; bss.s). Step 1 (r12, r11, r10 registers, b1,
  b2 [20], no s, rmnode): 1296 > 1111 lines.
  popt model 56 of 73, 1436 lines; popt corpus 816/816; cparse model 173.
- rmove: no `op` (the original switches on p->op; the explicit `(unsigned)(op - 1) > 0x33` test of our source is
  the range test codgen makes itself, the default goes to `reset`): 1111 > 1106. The frame is still 0x3c against
  0x38 (n, t, r).
  popt model 56 of 73, 1431 lines; popt corpus 816/816; cparse model 173.
- rmove, step 2: no `n`: p1 (r13) also holds `p->back` and the new nodes of the shift cases, as in the original
  (`ld r13,4(r14)`, `ld r13,r2` behind alloc); where p1 meant `p->forw` behind such an assignment the source now
  writes `p->forw` (shins `p->forw->back = p1`, the `r12 == 0` arm of do_cjp); every path from an assignment of p1
  goes to disp, kill_all, cc_clear or next, which do not use p1. `int ty` in front of b1 (frame word 0): the dest
  value of the LD case (ld_chk) and of do_cjp; in do_cjp the findcon results go into t and r (`r12 = compare(
  p->subop, r, t)`), no block locals; `copy(1, s)` with two arguments (8 times). The content difference of the
  instruction multisets 190 > 76 instructions; the strict lines 1106 > 1208 for now (the layout moved with it).
  Left in the content: our frame 0x3a against 0x38 (one temporary `ld 0x2e(fp),r2` twice), the case order and the
  order of the tests in the main switch (`cpb @r14,#0x17/#0x18`), the string addresses (`ld r6,#0x08..`: the order
  of the string literals of z22.c in the pool), a few reloads of p->back (`ld r2,4(r14)`).
  popt model 56 of 73, 1533 lines (rmove 1208); popt corpus 816/816; cparse model 173.
- rmove, step 3: `t = strlen(b2);` in front of the two strncmp of do_lda (the original stores the length into t
  and passes r2; the call inside the argument gave codgen v0p a temporary: the frame is 0x38 now), do_cjp reads
  `p->back` every time instead of holding it in p1: 61 instructions of content left.
  popt model 56 of 73, ; popt corpus 816/816; cparse model 173.
- rmove, step 4: the sections behind the dispatch in the order of the original's code (the targets of the table at
  0x6fe sorted: ld, clr, dec, inc, and, arith, push, pop, srl, sra, sla, tst, cp, cjp, lda; every section ends with
  a goto, so the order is free in the source): rmove 1190 > 549 lines, content 61 > 48 instructions; popt 874.
- rmove, step 5: the `r12 == 0` arm of do_cjp keeps p1 (= p->forw there): 549 > 519 lines, content 40. Ours is 88
  bytes shorter than the original: tests on MUL (0x17), DIV (0x18), SRA (0x0E) and `cpb @r13,#1/#0x10` exist only
  in the original.
  popt model 56 of 73, 844 lines; popt corpus 816/816; cparse model 173.
- rmove, step 6: disp tests DIV/MUL twice as the original (setreg choice, then ccloc[0] = 0 or setcc): content 36, rmove 528 lines.
- rmove, step 7: sh8 begins with `if (p->op == SRA) dualop(p);`: content 35, rmove 530 lines.
  popt model 56 of 73, 855 lines; popt corpus 816/816; cparse model 173.
- rmove, step 8: push_end tests isreg(RT2 + 1); do_cjp (r12 == 0) goes on with p = p->back->back as the original (host/popt: p->back->forw, a behaviour difference the corpus does not show): content 33, rmove 526.
  popt model 56 of 73, lines 851; popt corpus 816/816; cparse model 173.
- host/popt synchronized with the behaviour of the original found in src/cmd/popt (owner request; only the normal
  popt, no old-cc switches): natural tests the NUL (the `)` branch never applies), push_end tests `isreg(RT2 + 1)`,
  do_cjp goes on with `p = p->back->back`. Not taken over (undefined in the original, host/ may keep its tests):
  the null pointer reads (nullcode, oplook, equop, rename_regs, fixldm), the `rN(` recursion of markregs, the endless
  blank loop of adrlen, the 20-byte conval/ccloc. The other source forms of src/cmd/popt do not change behaviour.
  host/popt against /lib/popt on the popt corpus: 816 of 816.
- rmove, step 9: do_lda tests isreg(RT2) < 0 without r: rmove 523 lines; the rest of the content: jump forms, the jump table addresses, string addresses.
  popt model 56 of 73, lines 848; popt corpus 816/816; cparse model 173.
- rmove, step 10: the string pool of the original ("#0" of do_clr near the end) puts do_clr behind do_cjp in the source; popt moves its code up: rmove 523 > 453 lines.
  popt model 56 of 73, lines 778; popt corpus 816/816; cparse model 173.
- rmove, step 11: clr_cc with the register branch first (if (r12 >= 0) { savereg } setcon): content 13; the z22.c strings are in the original's order except the cpir table of rmove.
  popt model 56 of 73, lines 778; popt corpus 816/816; cparse model 173.
- rmove, step 12: the main switch encloses the sections and disp (the table order of the original): content 11, rmove 451, popt 776.
  popt corpus 816/816; cparse model 173.
- rmove: tried clr_cc as if (r12 < 0) setcon else { savereg }: content 12 against 11, reverted. Left: the two "#0" of clr_cc in the other order, one call (ld r7,r14) more in ours.
- rmove, step 13: JP and CALL as sections do_jp, do_call behind disp (the original's code places): rmove 441, content 10, popt 766.
  popt corpus 816/816; cparse model 173.
- rmove, step 14: reset between do_jp and do_call: rmove 435.
  popt corpus 816/816; cparse model 173.
- rmove: next: behind do_ld in the source gives the same code (435): not the cause, reverted.
- rmove, step 15: the LD case that becomes CLR goes to do_clr (one singop less, as the original): rmove 410, content 8, popt 735.
  popt corpus 816/816; cparse model 173.
- rmove: what is left is layout (jump forms, the two "#0" of clr_cc whose order depends on the branch order); the source work on rmove is done for now.
- cparse: libcbss.s replaced by an old data member (src/lib/oldlibc/data/0_1.c: _sobuf declared before _sibuf): the shipped data.o defines both commons and is their first mention in the link; with the old order the bss of the model is as before (__sobuf 0xb4f4, __sibuf 0xb6f4), cparse model 173.
- popt: bss.s replaced by a list of the globals as definitions at the end of z2.h in the original bss order (every module includes it; z20.o, linked first, gives the order): the bss map of the model is the same symbol by symbol; popt model 56, cparse 173.

## 2026-11-02

- cparse: bss.s replaced by a list of the globals as definitions at the end of mfile1 in the original bss order (cgram1.o,
  linked first, gives the order); `struct instk`, `shdr`, `ref` moved to mfile1. `yytext` is 125 bytes as in scan1.c (bss.s
  said 126, the place up to the next symbol: ld rounds, the addresses are the same). Against HEAD with bss.s: normal, v0,
  v0p, v1 and the model have the same code, data, relocation and bss sizes, every symbol the same value (only the file
  symbol bss.o is gone). Model 173, 96 lines; corpus 260 of 260 (one input more than the 259 of before).
- OLD_D16A not for the left operand of a compare (d16acmp, set in setbin): popt codemove exact, popt model 56 > 57, 735 >
  729; cparse 173, 96 (genswitch stays). Instrumented: the rule fires only in cparse genswitch (op ASG PLUS) and popt
  codemove (op ASG MINUS) in all of cparse and popt, none in our codgen sources, cookie 010036 both. In the originals of
  cparse, scparse, popt, spopt, codgen, scodgen, gopt and cpp the load form `ld r,F; inc/dec r,#n; ld F,r` is only
  genswitch and one `inc r2,#7` of gopt (0x7f52, its source is not known). The many `inc/dec F; cp F` there may be separate
  statements, so they are no evidence. Rule "only ASG PLUS" would fit the two witnesses as well. Normal and v0 objects of
  match3 and order3 unchanged.
- D3, a parameter across a branch (popt setcon, equop, regof): three mechanisms, not one.
  setcon: OLD_D3AW forgets a parameter only in the register the argument is wanted in: 57 > 58, 729 > 727; cparse
  unchanged. equop: its else label has two jumps; the state of the jumps given back there (the attempt of 2026-11-01)
  makes equop exact but buildtree 6 > 14 (its then part has calls); keeping what is known at the jumps and still known in
  front of the label: equop exact, buildtree 6: 59, 725. In v0 bench unchanged (118, 275), so its own switch OLD_D3JM in
  v0p. regof: no forgetting below a `?:` in an argument, the arms do not take the parameter from the wanted register
  (reader3, QUEST): 60, 721. cparse model 173, 96 throughout.
- D2 r1/r3 (popt itoa_, reverse). reverse source: `for (j = 0, i = strlen(s) - 1; ...)` (the original clears j first): 8 > 6
  lines. All moves out of r0 in the originals of popt, cparse and codgen: r1 behind an inc, sla or add of a constant, and
  only itoa_ and reverse with `add r0,rN`, both `ld rN,r0`. OLD_D2RA: both exact. First measure 61 with ovlap 4 worse:
  d2r0reg stayed set after its statement (until now r1, which never matched; now r3); reset in stmt: 62, 709. cparse 173, 96.
- D5 STREF (popt procopt). A tree dump in mapregs: STREF(PLUS(PLUS(index, ICON ~L1+off), REG fp), member); the walk is
  bottom-up, the inner ICON carries the frame symbol. OLD_D5T (the member added to that ICON, no preference for the
  address wanted in a register, as OLD_D5A): procopt 48 > 12 > 0; popt 63, 661; cparse 173, 96; corpus 260. In v0 the bench is
  unchanged (118, 275): v0p.
- D3, the value of an assignment. Without the argument part of OLD_D3N repladdr is exact: rmove replaces the load of a frame slot
  stored last, comjump (before rmove) merges the two arms and the label clears that. prtdcon loses it (`l = lab = getlab()`:
  lab is not the slot stored last): source `fprintf(dfile, ..., l)`, exact. Without d3rsub findcon is exact, lineid and
  psavel lost: `if( *(cp = &ftitle[len-1]) == '"' )` and `*(long *)(ip = &paramstk[paramno]) = l;` exact (the forms `(paramstk +
  paramno)`, casts: `ldl %9524(r2)`, no). Without the pointer store part genswitch lost: `t->rval = swlab = getlab();`
  exact. OLD_D3N removed from codgen (204 lines). popt 65, 657; cparse 173, 96. Not explained by source forms yet: setreg
  (the first argument of makereg through r2): tried nothing further, one witness.
- First full `oldcc.sh matrix` (56 switches, host/OLDCC_SWITCHES.md, commit cef592e). Not separable alone: OLD_D22, OLD_D3A, OLD_D3K,
  OLD_D6T (they need their partners: the cparse half, OLD_D3AW, OLD_D3J). No exact function lost: OLD_D18 and cparse OLD_D12 (+0: the
  model makes the data layout itself, strictcmp treats @r15 and 0(r15) as equal), OLD_D3V, popt OLD_D20L, OLD_D20X (+5, +7, +66 in functions that differ anyway). One exact function: 16 switches.
  OLD_D16A has genswitch alone (codemove was the limit of the rule, not a witness). genswitch alone holds OLD_D16A, codgen and cparse OLD_D21,
  OLD_D2O: a candidate for source forms (guess). Only gain in the table: popt killreg is exact without popt OLD_D20K (cinit lost, +61 lines).
- genswitch, the three switches only it holds. OLD_D21 (both passes): the `div` is signed (`exts rr2`; the entry of 2026-11-01 calling it
  zero-extended was wrong), `size = n*SZLONG/SZCHAR` without the cast is exact: both switches removed. OLD_D16A: `n = n + 1;` and `n = 1 + n;`
  (also inside the index) give `lda r2,1(r6)`; `n += 1;`, `n++;`, `(p+(++n))->sval`, `p[n += 1]` give `inc` in memory: no form found, and `p[++n]` is the
  plain form: stays. OLD_D2O: without it `ldl rr0,@r3`; a register choice, the odd counterpart of OLD_D2Q; no source form tried: stays.
- D20, killreg against OLD_D20K. Without the switch killreg is exact, cinit lost, rmove 410 > 476. Raw code: the case arms end `sla; clrb; jpr L524`, `L524: jpr
  L519`; the original keeps the tail in the then part (comjump), cinit the call in the else part (xjump to the own label). About 90 sites of the rule in
  popt and cparse. Tried: only when xjump to the final target would not merge: no effect (in front of L519 stands the dispatch); only when one instruction would
  merge: killreg exact, rmove 472; only behind a call: killreg, cinit exact, rmove 410: taken. popt 66, 650; cparse 173, 96. Why a call: not known.
- D20, backjmp and xjump. The original reads as `while (p = p->back) { if (op != LABEL ...) break; }`: tests in front of the step. That source alone changes
  nothing (raw `L: step; test; jpr eq,OUT; T; jpr eq,Lc; jpr OUT; Lc: jpr L; OUT:`; iterate follows the jump to Lc first, the back jump dies and no inversion runs).
  OLD_D20B (a conditional jump onto an invertible back jump is not followed): both exact, but ovlap 922, yyparse 11. Only for the shape `jpr OUT; Lc: jpr L; OUT:`: ovlap
  exact. Only when the first test of the loop is eq: yyparse exact. popt 68, 632; cparse 173, 96; corpus 816. The first loop of xjump takes the same form, exact (it stays
  not inverted: its head is behind a jump), so both loops are written alike. Not tried: a source form without a switch beyond do-while, while (a && b), while-break.
- D20: dest, movedat, setreg were source forms, as the rule says to try first. dest: `else if (sub == 0) sub = 0;` behind `if (sub == 0) sub = SLONG;` (the second test of the original, dead code: quirk) 18 > 12; the case order of the second switch, 30 orders and default places measured, 9 exact, among them the plain SBYTE, 0, SLONG (taken); with it `sub r2,r3` too. movedat: the body inside the switch: exact (case order free). setreg: `while (*s) { if (*s == '(' || *s == '@') { if fp break; cleargroup; return; } s++; }`: 22 > 7. Left in setreg: `makereg(r & 0xf, name, type)` (`type` in the argument: 13, worse).
- usesreg: `default: dflt:` with `goto dflt` and MUL falling in: 177; rules refuted: OLD_D20F (no move behind a jump to itself): cparse 471, popt 695; OLD_D20O (no move behind a conditional jump over a jump): usesreg 128, cparse 166. Removed, source back.
- cparse 96 > 27 by source forms: optim `else if( ... ){ zapright: ... return; }` exact (51 lines, open since 2026-11-01); defid the step of the SMEMB loop as `goto mstep;` with `mstep: cp1 -= sizeof(int); goto mtest;` behind `goto mismatch` exact (odd form); argregs `n*2` 27 > 17 (`case 4: break;` with the body behind the switch: worse). popt 594 > 452: usesreg as in OLDCC.md D20, 177 > 35 (`?:` for the POP arm: same, not taken).
- Rule tried for argregs and optim, a jumping block in front of its target, OLD_D20M again in new forms: only `Lx: ld; jpr D` onto a compare (argregs 27 > 25, 17 > 14, nothing else changes: candidate, one witness, not set); any block with a branch inside (cparse 1408, popt 1248), block longer than the target (cparse 1211, popt 1316), no condition: popt does not end. Removed.
- OLD_D20S removed: getpreg `if( pregvar < 2 ) break; goto gotreg;` exact without it (`if( pregvar >= 2 ) return( pregvar-- ); break;`, `if( pregvar < 2 ) break; return( pregvar-- );`: 8 lines). Without it usesreg 35 > 6, rmove 410 > 420. usesreg: EXTS cases in the order 0, SLONG, SBYTE: exact. popt 71, 427; cparse 175, 27. OLD_D3A removed: its block was empty since OLD_D3AW (matrix +0).
- rmove 420 > 19 lines, all source forms, step by step: del inside the first test of do_ld (388); redual inside `if (RT1[0] == '#' && ...)` of do_dec (349); disp behind noop (261); kill_all inside do_push, shins inside do_srl (143); do_clr behind do_sla, also the string order (85); CLR arm first in clr_cc, cjp_chg, lda_mem behind lda_end (19). Refuted: TST arm by `goto cp_tst` between `goto cp_chk` and cp1 (34), with cp_chk nested too (34). Instruction counts: original 949, ours 948, but `call` 167 against 132 and `calr` 3 against 38: strictcmp masks that (the 62 bytes the handoff called missing code are call forms).
- strictcmp -c, -a: cparse 168 of 179 (79 lines), popt 61 of 73 (172 lines); -a the same. In reach of calr, popt: 69 forward and 20 backward calls long, 76 and 52 short; cparse 58 and 23 long, 85 and 128 short. The form is not per callee (dest, dualop, isreg have both), it runs in stretches along the address: rmove long up to 0x1f68, with its long jp.
- The old cas, new round (owner 2026-11-02: the solution has to be found there). From the originals alone (`host/recon/castrig/windows.py`): in every module the long jumps and calls in reach need only one or two points they span, and no short one spans them: popt z21 0x56e..0x964, z22 0x1f5c..0x1fdc and 0x2940..0x2a46, z23 0x3450..0x432c; cparse scan1 0x219e..0x21aa, pftn1 0x2fe6..0x31e0, trees1 0x4362..0x4748, sym1 0x9596..0x99ca and 0x9e56..0x9ece; no backward call or jump in reach is long, at all. Long means: undecided when the optimization ran, the target still unknown (manual Band B 3.4.5). Measured: real table offsets from an instrumented host cas on the _old.s modules (`castrig/offsets.py`).
  Class 1: the symbol table (stoff) crosses 0xC00: z21, pftn1, trees1, sym1 (first point), and in cgram1, scan1, local1, comm1, vinit1 the crossing lies where no short jump spans (invisible, nothing to see there). A scan of thresholds has its minimum at exactly 0xBFC..0xC00. Conflicts: z22 (6 short jumps), code1 (4), z23 late by 0x76 bytes of symbols.
  Class 2: the undecided entries reach 255: scan1 (one site, `callr __filbuf`, entry 255), z22 (249..256), sym1 second point (252..254). z21, pftn1, trees1 have short jumps across entry 255 (they had class 1 before: does it reset?).
  Class 1 refuted the same day: the 0xC00 came from the bss definitions at the end of mfile1, which every module of cparse has only since 2026-11-02 and the old sources surely did not (`.comm` per module 87 > 14 in code1). On the cparse input without them (CAST=dir, the list cut from mfile1 in a scratch copy) stoff at the single free sites is 0x91c (trees1), 0xa1a or 0xa4c (pftn1), and the crossing of 0xC00 has short jumps across it (trees1, pftn1). Class 2 stands: entry 255 in scan1, sym1 (252..254), z22 (249..256). Left to explain, the entries at the other points: trees1 179, z21 187..190, pftn1 198 or 205, sym1 158..193, z23 up to 200, z22 second point 180 behind class 2. No table offset is common to them.
  Open: the second point of z22 (one free site, 0x2a08, stoff 0x12ae, entry 435, 180 behind class 2). Why the old cas does this at these points is not known. Earlier simulations (capacity, itable steps) looked at jumps of cparse only.
- The old cas, the test of the model (owner 2026-11-02): a scratch host cas (`castrig/hostcas.patch`) runs optim at given jpr/callr of a module (`castrig/triggers.txt`, ordinals, one site from each window: z21 0x908, z22 0x1fae and 0x2a08, z23 0x3486, scan1 0x21a6, pftn1 0x3156, trees1 0x463a, sym1 0x997c and 0x9e90), with every symbol's entry count cleared behind it. Result, strictcmp -c = strict: cparse 176 of 179, 25 lines (yylex exact, the long jp comes out; argdebug, buildtree, argregs left); popt 71 of 73, 18 lines (rmove 11, setreg 7). Function addresses equal the original's but for the differences left (popt: jumpsw -2, makereg -6; cparse: buildtree +2, tempnode 0). So one early optimization at these points is the whole explanation of D17 and the long jp; the rule for the points is not known.
- Found on the way: hash, upoff, oalloc belong to sym1.c, not vinit1.c: sym1 reaches hash by calr, and a calr cannot leave its module (exact with the test cas; without it the same as before). Entries at the points on the current input: scan1 255; z22 249..256 and 435; sym1 181..212 and 271..273; z21 187..190; trees1 179; pftn1 198 or 205; z23 up to 199: the 255 of sym1 is gone with the move, so class 2 has scan1 and z22 only.
- Owner's question: several counters with weights against one threshold? `castrig/fit.py`, the first point of each of 7 modules (cparse input without the bss list), non-negative weights: no single counter, no pair (ratios 0.01 to 199) and no triple (a coarser grid) of entries, labels, symbols, name bytes, code location, all jpr and callr fits; the best pair misses by about 25 %. Limits: popt's input is uncertain (z2.h), and an unseen earlier trigger would make a "first" point a second one (no reset tried in the fit); data bytes and per-function counts not tried.
- Weighted counter with reset at each trigger (`castrig/reset.py`, `memsum.py`; 13 modules, 89 long and 2282 short sites in reach, a site wrong when the rule makes it the other form; no trigger at all = 89 wrong). cparse alone: the code location alone at 3128 bytes: 3 wrong, but only at this value (3120: 4, 3140: 7) and popt 36 wrong: chance or part of it. Both programs: code location alone 17; best pairs symbols + 0.71 * all jpr/callr 13, name bytes + 14 * code location 13. Sums in bytes of the tables: stoff + name bytes + 12 per jpr/callr 12 at 7053, + 14 per jpr/callr 13 at 7593, + 16 per jpr/callr 15 at 8149; with 6 per label 13 at 8445. Not exact: left z22, trees1, sym1 mostly. The scores jump between neighbouring thresholds (6917: 14, 6933: 17), so with this many parameters a good score can be chance. Direction, no rule. No independent test yet: codgen would be one.
- The last source lines. setreg: `c = r & 0xf; makereg(c, name, type);` (c is the register variable already there; popt drops the copy) exact. rmove: TST arm inside the block of `if (equstr(RT2, "#0")) { goto cp_chk; cp_tst: repladdr(p, 0); goto cc_clear; }` (an odd form) 19 > 14, cc_clear behind do_cp 14 > 8: what is left are the long jp. argdebug: `argdebug( x ) register x; { register a; return( x ); }` exact (two registers saved by ldm, procopt renames x into r1; no popt rule needed, the witness of 2026-11-01 is gone). argregs: the bodies in front of the dispatch (`goto sw; nofit: ...; two: ...; one: ...; ret0: return( 0 ); sw: switch( n ){ case 1: goto one; case 2: goto two; } if( n == 4 && aregs == 7 ){ ...; goto ret0; } goto nofit;`), `n*2`: exact; with `default:` and `case 4:` in the switch 6 lines (the block moved up ends at the jump of the default, which takes nofit). Candidate D20D (one witness) no longer needed.
- buildtree 6 left: `ld r3,#%27EA` and the 0.0 at 0x2846 (the order of the double constant and the table in the data), the temporary at 0x12. Tried: j, k in their own block (no effect), cp2, cp1, m in the block of the switch (temp 0x12, but the frame and all other temporaries wrong, 26 lines). Not explained.
- buildtree, two attempts. The temporary at 0x12: `case QUEST: { int a1, a2, a3; ... }`: exact for these 2 lines, the temporary starts behind the declarations of the last block, as in clocal (the three words are unused in the source: an odd form; the names are guesses). The 0.0 behind the first table: a trial in a scratch d12model (no compiler change), the block of the constant moved behind the value and target lists of the switch of line 155: the other 4 lines go, buildtree exact, cparse 178 of 179, data equal 17855 > 17890. So the old compiler writes a double constant at its statement, where our cparse writes it into the .D file that codgen reads at the head of the function; only trees1 has a double constant and a cpir table, one witness: not built, owner to decide.
- OLD_D12K (owner decision, one witness): codgen copies the .D data at the head of a function without the blocks `Ln: ` and four words, and writes such a block when eread meets its FCON (d12kout; the rest at the end of the function). buildtree exact, cparse 178, 2 lines; data equal 17855 > 17890; corpus 260; with the switch in v0 bench unchanged (118, 275): v0p. Normal and v0 objects of rdr3, main3 unchanged.
- findings.json of cparse and popt on the base of ccv0p (owner 2026-11-02): status measured, notes only for open differences, odd forms, guessed names, dead code. The notes on source forms and switches that left the files, as they stood (the older fields, audit, candidates, review, D classes against the shipped compiler, are in the git history):
  cparse optim: Exact with v0p. Source: zapright: inside else if( ... ){ }, no goto from there (the node in front of the label is then no jump, so popt leaves the block where the original has it).
  cparse defid: Exact with v0p. Source: the step of the SMEMB loop as goto mstep; with mstep: cp1 -= sizeof(int); goto mtest; behind the goto mismatch. An odd form, recorded under the evidence rules of OLDCC.md: the bytes fit, a plainer form or a rule stays possible. Source fix: one more unused int behind cp2 (a frame word of the original; the name is a guess).
  cparse genswitch: Exact with v0p. Source: r = t = talloc() (store order); size = n+n and the table size assigned inside the sectfull argument. Source: size = n*SZLONG/SZCHAR without a cast. Old-cc: OLD_D16A (p[++n]: load, inc, store), source t->rval = swlab = getlab().
  cparse lxstr: Remaining: the lastcon |= val<<8 arm uses rr0 in the original, rr4 in ours (D2), and it is placed in front of the case bodies in the original (layout); the outer switch is a compare chain (D6).
  cparse argregs: Exact with v0p. Source: n = argregs( p->left ) in front of return( n + argregs( p->right ) ); return( n*2 ); the bodies in front of the dispatch with goto sw, a switch of two cases and the case of 4 as an if behind it. An odd form, recorded under the evidence rules of OLDCC.md.
  cparse rmconv: Exact with v0p. Old-cc: OLD_D3J (c from rl6 at the else label).
  cparse clocal: Source: the l->left chains (SCONV loop and DIV/MOD p->left = l = l->left, char compare l = p->left = l->left); p = l; return(p) at the SCONV end, p = buildtree(...); return(p) for PVCONV/PMCONV. long val declared in the ICON block. The switch on ml in the order CHAR, UCHAR, USHORT/UNSIGNED, SHORT/INT. Exact with v0p.
  cparse argdebug: Exact with v0p. Source: argdebug( x ) register x; { register a; return( x ); } (ldm of two registers, procopt renames x into r1).
  cparse yyparse: Exact with v0p. Source: ncopy( t = talloc(), *(NODE **)swp ) in the switch action (the call result goes to a temporary too).
  cparse hash: Source fix: the locals are declared h, p (the original has h at the lower frame place, the old cc gives the first declared local the lower place).
  cparse hide: Source fix: no unused local h (the original frame has i only; an unused local gets a frame place in the old cc too, initpreg).
  cparse lookup: Source fix: one more unused int between i and n (the original has a frame word there; its name j is a guess).
  cparse nidcl: Source fix: the empty array warning for AUTO and REGISTER (the original compares with 4); the source had EXTDEF.
  cparse getpreg: Source fix: the parameters declared t, p (the original has t in r14 and p in r13; the first declared register variable gets r14). Source: if( pregvar < 2 ) break; goto gotreg; (exact without OLD_D20S, removed).
  cparse fhashv: Source fix: register n (the original saves r14 and gives n no home: a register parameter behind the last other one, OLD_D4R). Remaining: the original takes n from r6 and does not restore r14 (popt renames the register variable), the byte loads use rl3 throughout.
  cparse cisreg: Source fix: t is not a register parameter (the original uses r7 and saves no r14).
  cparse tyreduce: Source fix: the dimension goes through a long local (the original stores (unsigned)temp into a 4 byte frame place and passes it from the register of the assignment); the name off is a guess, Plexus passes (OFFSZ)temp directly.
  cparse vfdzero: Source fix: register n (the original saves r14 and gives n no home).
  cparse tcopy: Source fix: ncopy( q=talloc(), p ) and a register r = p->rval as in Plexus (the original saves r12, r13, r14; r unused afterwards).
  cparse ncopy: Source fix: register NODE *q, *p and an unused register i (the original saves r12; popt renames q and p into r4 and r5).
  cparse beginit: Source fix: defalign( talign(...) ) without the local al (the original stores no al).
  cparse endinit: Source fix: inforce( tsize(...) ) without the local tsz (the original stores no tsz).
  cparse falloc: Source fix: s is a local of the ENUMTY case block (the original reuses its frame place for a codgen temporary).
  cparse tymatch: Source fix: register o, u declared in front of t1, t2, t, tu (o in r13, u in r12 in the original) and register NODE *p1 as in Plexus (an eighth register variable: an unused frame word).
  cparse buildtree: Source fix: one more unused int at the function level, and in the STREF block the locals in the order cp2, cp1, an unused int, j, k (the frame places of the original); the names of the unused ones are guesses. Source: cp2 = &ntab[STP(i)->sname] in the member block. Source: case QUEST: { int a1, a2, a3; ... } gives the temporary at 0x12 (an odd form, the three words unused, names guessed). Old-cc: OLD_D12K (the 0.0 at the statement, behind the first table).
  cparse efcode: Source fix: register i declared in front of p (i in r14, p in r13 in the original) and an unused long behind size (4 frame bytes of the original; the name is a guess).
  cparse ispow2: Source fix: for( i=0; c>1; ++i ) c >>= 1; (the original shifts before it counts).
  cparse getmem: Source fix: n += 1; n &= ~1; (the original rounds n in its register with inc and res).
  cparse oconvert: Exact with v0p. Source: block( SCONV, q = p->left, NIL, INT, 0, INT ) (the assignment in the argument is evaluated first).
  cparse prtdcon: Exact with v0p. Source: sectfull( DATA, size = SZDOUBLE/SZCHAR ) (the assignment inside the argument). Old-cc: OLD_D22 (the double argument in rq4 by ldm) ; source fprintf( dfile, ..., l ) (rmove takes r2 for the slot stored last).
  cparse fltlab: Exact with v0p. Source: d == p->fval (the original loads d). Old-cc: OLD_D22 (the double parameter in rq4, the compare without the rounding fld).
  cparse fincode: Exact with v0p. Old-cc: OLD_D22 (the double parameter in rq4, sz in r3).
  cparse isitfloat: Exact with v0p. Old-cc: OLD_D22 (the result of atof from rq4: fldd f0,r4).
  cparse doinit: Exact with v0p. Old-cc: OLD_D22 (a computed double argument through f0 into rq4).
  cparse treeeq: Exact with v0p. Source: the LTYPE case returns the value of the && expression.
  cparse lineid: Exact with v0p. Source: if( *(cp = &ftitle[len-1]) == '"' ).
  cparse psavel: Exact with v0p. Source: *(long *)(ip = &paramstk[paramno]) = l; (the original saves r14 and leaves it unused: popt removes the dead copy). An odd form (ip is not used again), recorded under the evidence rules of OLDCC.md: the bytes fit, a plainer form or a switch stays possible.
  cparse instk: Exact with v0p. Source: t==STRTY ? (int)dimtab[s+1] : 0.
  cparse getstr: Exact with v0p. Old-cc: OLD_D2QO (the long through its pointer into the pair of the address register, wanted r7).
  cparse talloc: Exact with v0p. Source: an unused third local NODE *r (the frame word at 4 of the original).
  cparse tfree1: Exact with v0p. Old-cc: OLD_D3J (p from r7 at the else label).
  cparse stref: Exact with v0p. Old-cc: OLD_D16B (p->type += PTR rewritten as p->type = p->type + PTR).
  cparse dclstruct: Old-cc: OLD_D2PO, OLD_D2MO (the quotient multiplied in rr2). Exact with v0p.
  cparse refid: Exact with v0p. Old-cc: OLD_D2W6 (the third pair is rr6). OLD_D2PO, OLD_D2QO.
  cparse conval: Exact with v0p. Source: in the PLUS case p->type before p->rval. Old-cc: OLD_D2PT.
  cparse exname: Exact with v0p. Source: *q++ = (...) ? '$' : '_'. Old-cc: OLD_D3AW (&ntab[s] adds s from the frame).
  cparse cinit: Exact with v0p. Source: case ICON before case NAME. Old-cc: popt OLD_D20K.
  cparse defalign: Exact with v0p. Source: register n, n /= SZCHAR (no extra variable; a register parameter gets no home, OLD_D4R).
  cparse argalloc: Exact with v0p. Old-cc: OLD_D2W6 (the third pair is rr6).
  cparse zecode: Exact with v0p. Old-cc: OLD_D3J (n known in rr6 at the else label), OLD_D3AW (n*2 as an argument from rr6).
  cparse newsect: Exact with v0p. Source: for( ; *cp; ++q, ++cp ) *q = *cp. Old-cc: OLD_D5G (&sectname[sec][k] with the address added last).
  popt xjump: v0p model exact with OLD_D20B: both loops while (p = p->back) if (...) break;
  popt codemove: v0p model exact with OLD_D16A not for a compare: --n == 0 stays dec n,#1; test n; p a plain parameter, p1, p2, p3 registers r14, r13, r12
  popt backjmp: v0p model exact with OLD_D20B: while (p1 = p1->back) if (p1->op != LABEL) break;
  popt equop: v0p model exact with OLD_D3JM: p2 stays known in r6 at the else label with two jumps
  popt findcopy: v0p model exact with an unused local word behind r (frame 8; guess) and case TQ: r += 2 falling into case TL
  popt setreg: v0p model exact: c = r & 0xf; makereg(c, name, type); the scan loop with its consequences inside while (*s)
  popt dest: v0p model exact: else if (sub == 0) sub = 0 (dead second test of the original), second switch in the order SBYTE, 0, SLONG
  popt killreg: v0p model exact with OLD_D20K only behind a call: the two if (n & 1) arms merge as the original
  popt repladdr: v0p model exact: rmove takes r2 for the slot pfx only behind no merge of the arms
  popt movedat: v0p model exact: the body of the second loop inside switch (r1->op)
  popt findcon: v0p model exact: p = regs[n]; as a separate statement tests the variable
  popt setcon: v0p model exact with OLD_D3AW only for the wanted register: natural(loc) as ld r7,r6
  popt natural: v0p model exact: while (*s++); s--; the trailing ) test looks at the NUL (quirks table)
  popt toofar: v0p model exact with n = ilen(q); sum += n (the original keeps the call value in a frame word); whether this is the source or a rule of the old compiler is open
  popt adrlen: v0p model exact with an unused local word between the parameter homes and the saved r14 (the frame of the original is 8; the local is a guess) and the blank loop that increments s while testing p (quirks table)
  popt rmove: v0p model 8 lines: section order and if-blocks as in OLDCC.md D20; the remainder is the long jp of the old cas. The TST arm inside the block behind goto cp_chk is an odd form. strictcmp masks call against calr, 35 places here
  popt itoa_: v0p model exact with OLD_D2RA: add r0,r3; ld r3,r0
  popt usesreg: v0p model exact: default as dflt: inside if (reg != r10 && reg != r10 + 1) of MUL, out1 and out0 inside if blocks of the LD and EXTS cases, EXTS cases 0, SLONG, SBYTE. dflt: inside the if is a less usual form
  popt procopt: v0p model exact with OLD_D5T: t[k].s1 as k*10 + (t + 2) + fp
  popt regof: v0p model exact with OLD_D3AW not in the condition of a ?: in an argument
  popt reverse: v0p model exact with OLD_D2RA: add r0,r2; ld r2,r0; source for (j = 0, i = strlen(s) - 1; ...)

## 2026-11-02: /lib/codgen, the first model of v0p

Step 1 of the codgen work: `oldcc.sh model codgen` (launchers, `progcfg.py`, `size_orig` and `file` in the old `findings.json`).
- First number, before data and bss: 17 of 206 strict, 12664 lines; masked (`-m`) 74 of 206. Data 27692 against 27084, bss 18114 against
  17522, code 53760 of both (own part 0x5a short: 0xbb6a against 0xbbc4, the functions `0x4868`, `0x6fca`, `0x854c` are missing).
- The libc part is now the original's as far as the masked comparison goes: order of libc.a (`LIBCOBJS` in the Makefile), with `perror` 0_1
  and `errlst` 0_1 (called only by the dead function 0x854c), `malloc` 0_3 (with `realloc`), `stty` 0_3 (`stty` and `gtty`).
- `ccv0p` stops with `register jam` on `(unsigned long)(s->soffset / 8) < (unsigned long)(unsigned)t` in `regargs`; the shipped compiler does not.
  Reduced: `(unsigned long)(a / 8) < b`, a cast of a long quotient on the left. Without the left cast (the comparison stays unsigned) it compiles and
  the code has the form of the original's (`divl`, `ldk r2,#0`, `ld r3,r11`, `cpl`, `jr nc`). The cast went away from the source. Why v0p jams
  is not looked at: a guess is that the cast keeps the pair of the quotient busy.
- The original's modules by their SCCS stamps in the data: reader3, rdr3, local3, order3, match3, allo3, regallo3, comm3, table3 and table3.nzcc,
  wrt3, regs3, lcl3, tr3, optim3. Our source has 22 files with another split (main3, efcode3, gen3, out3, zzz3, tmp3, regfile3 ... do not
  exist in the stamps) and 206 functions whose code order is not ours.
- Corpus of the normal build (`tools/corpus.sh`, `check.sh`, all of `src/cmd` and `src/lib`, new and larger than the 101 cases of before): codgen 165/165 and
  104/104; scodgen 104/104 and 159/165. The 6 cases give the same result before this step (base built from the previous commit), so they are older than
  the step: `codgen_optim3`, `cparse_optim1` stop with `reclaim dest reg not free` (`p->lval *= val`, a long in memory); `doscopy_1.3_dosdir` and
  `pburn_{2.1,2.3,2.6}_pb.cmdline` give other text.

Step 2 of the codgen work: modules, data and bss of the original.
- Sources in the original's 13 modules with code plus `table3` (`KNOWLEDGE.md`, module map), functions in the original's order. `hop_*.c`, `ops_*.c` are included where the
  tables stand (`local3.c`, `comm3.c`). The common data in the original's order and sizes in `bss3.c`, linked first; declarations per module (a `cg.h` with all of them runs the v0p passes
  out of space: `cparse` on `table3_codgen.c` "malloc: Error 0", under the old root it hangs; `codgen` on `local3.c` "Symbol table out of space"). d12model has a time limit of 120 s per step now.
- A check of the move: with the shipped cc all 233 functions of the old and new files give the same assembler text. The corpus still got worse (3 of 269 codgen files, 11 scodgen) until the
  bss was the original's: the program depends on its layout. Back at 269/269 and 263/269 (the same 6) afterwards.
- A wrong order makes the measuring tool wrong too: `lib/v0p/codgen` built from the intermediate state wrote bad code (`fldd` with a word register, cas stops). Rule for the work: build ccv0p from the
  sources only when the normal build passes the corpus; the model numbers measured with that build are void.
- Size of arrays found: `numtab[1001]`, `tagtab[550]`, `fname[100]`, `saveorig`, `savecls`, `savereg[2]`, `resc[3]`, `parmtab[11]`, `argl[11]`, `dope`, `opst`, `opptr[129]`, `ifname[16]`, `mem[550]`. `buf` of `rddata` and `readnums` are autos.
- Tables placed by the data: `revrel` (common), `rev` (local3), `rwtab` behind `zzzcode`, `fldd` and `flds` named arrays, `rnames` in comm3, `rhnames`, `rlnames`, `rstatus` in local3, `opclass`, `ocnames` in match3.
- Source forms that the data showed: `main` is the forwarder 0x4868 of `mainp2`, `rwprint`, `syserror` added; `clocal` a switch on `p->op` with `case 9` and `case 1` written out, `case 18`; `nxwork` has a `case 0`; `cbranch` the inner switch of 16 relations;
  branches `n <= 4` first in ZS and ZA of `zzzcode` (the block order that was D15 there), `tot <= 0x10` first in `fpinc`, `fpdec`; `printf("r")` in `conput`.
- New switch `OLD_D6D` (v0p, cparse): no jump table at exactly 3 values per case. One witness (`zzzcode`, cases 1, 2, 8, 16); cparse and popt do not change.
- State: code 54016 against 53760 (the rounding of ld to 256), data 27084 and bss 17522 as in the original, 59 of 209 strict, 9803 lines, masked 76 of 209. cparse 178 and popt 72 unchanged with ccv0p built from the new sources; cparse corpus 253/253 (7 source files less
  in the corpus than before); bench 118/153, 275. `oldcc.sh matrix` is due: step 3 takes codgen into the table.

Step 3 of the codgen work: `findings.json` in the new form (the old one is in git before this commit; the old hints go along as notes at the functions), codgen in PROGS of the matrix. `OLD_D6D` stays with one witness (owner decision).
classify.py, patterns.py, funcs.py stay in `host/recon`: sld `classify_sld.py` imports them; strictlayers.py and findings_md.py stay for ld and sld. They can go when those two have their old compiler.

The tools of the old way moved out of `host/recon`: `classify.py`, `patterns.py`, `funcs.py` to `src/cmd/sld/tools`, `findings_md.py` as a script of its own to `src/cmd/ld/tools` and `src/cmd/sld/tools`; `strictlayers.py` gone (no findings.json has its data). Their `FINDINGS.md` come out unchanged. `classify_sld.py` could not start since the launchers of popt went (690b4816) and runs again; against the audit in sld's `findings.json` it differs in 4 functions (`end_pass1`, `lay_out`, `resv_seg`, `ldrsym`), with the configuration of popt and of sld alike; cause not looked at, `findings.json` left as it is.

Step 4 of the codgen work: the sum rule of the old cas against /lib/codgen, values as fixed on cparse and popt (stoff + name bytes + 12 per jpr/callr, threshold 7053, reset at each trigger; `castrig/codgen_test.py`).
- From the original alone (`windows.py codgen`): 12 long forward sites in reach against 2145 short ones, no long backward one, in 3 of 13 modules: reader3 7 (windows 0x90e..0xa0a, 0xfae..0x1884), local3 3 (0x324e..0x3cf2, 0x3e52..0x3f4e), order3 2 (0x5a3e..0x5c7a); no conflict with a short site. The picture of cparse and popt holds: one or two points per module, none backward.
- Rule: 3 long missed, 14 short made long, 17 wrong; no trigger at all is 12 wrong. So the test fails, the rule is not taken. In reader3 both triggers fall into the windows (0x9b0, 0x13fe), 7 of 7 long sites; local3 one of two; order3 none. The neighbouring thresholds give 11 to 22 wrong, the wide range from 16 up: no threshold of this form beats or clearly reaches the zero rule by a margin.
- Limits of the test: 150 functions are still open, the sites of an open function map to the original by their number, so a trigger can be some sites off; the declarations of our modules are ours, and they make part of stoff and of the name bytes. A repeat when the sources are nearer is cheap (same script, same values).
- D17 stays masked in `strictcmp`. A fixed list of points stays the emergency way (owner decision pending).

Step 5 of the codgen work, source forms, first round (comm3, tr3, the small ones; 59 > 114 of 209, 9803 > 8709 lines):
- A word `environ` of crt0 stands first in the bss, the gap in the common data is behind `stfile`; the statics of `alloc` and `revrel` stand behind the other tables of comm3. Most address differences go with that (72 of 209).
- `stdio.h` of popt and cparse (`char *_ptr`, the old `getc`, `putc`) also for codgen.
- What reads as `ld r1,r7`, `ld r3,r6`, `ld r4,r7` and so on in a leaf function is a register variable after `procopt` of popt (r14, r13 ... renamed into free low registers, the saves dropped): `lastchance`, `ltypeop`, `shincr`, `ispow2`, `delstrip`, `ncopy`, `icright`. So the source has `register` there, also when the variable is not used (`ncopy`, `chk`, `init`: `register int` without any use).
- PCC forms taken from the private reference for ideas, not copied: `ncopy(q = talloc(), p)` and the switch in `tcopy`, `down1 = down2 = 0` and `goto again` in `fwalkd`, the switch with fall through in `treeeq`, the cases of `icright`.
  Second round (137 of 209): `procopt` needs two `ldm` lines, so a leaf function with one register keeps r14; with two it renames (`oregfix`). A single `register int` with no use appears in several functions. Left for step 6: `rclass` `cp r6,#%0013` and `convelim` `cpb rl6,#%6C`: a parameter still known in its register behind a jump, where our codgen has forgotten it (OLD_D3J).
  sucomp 1187 > 32 lines: written with `register p, o, ty, sul, sur, nr`, `fl, fr, nf, t` autos, `p->left` without a variable, a `switch` on the type, and one macro: `max(a, b)` is `((a) > (b) ? (a) : (b))`, `min` the same with `<`; the arguments in PCC's order (the harder side first gives the turned compare, so `cp r9,r2; jr ge` is `max(p->left->su, nr)`). What is left are the blocks that popt merges and moves (the store of `fl`, the test of `fl`). conval and tshape exact: `(unsigned)t <= 15 && (unsigned)t >= 12`, `sh = val`, the case order of tshape NAME, ICON, FLD, CCODES, REG, OREG, STAR, `o = p->rval` in the FLD case: the compensating changes of the shipped compiler are gone there.
  Owner decision: the PCC passes follow the PCC of the Plexus tree as closely as the bytes allow (`CLAUDE.md`, plan in `src/TODO.md`). From here the open functions of codgen are written from the PCC text first.
  order, cbranch, match, expand from the PCC text (reader.c, match.c): order and expand strictly exact (`p = argprep(p, cookie); p = canon(p);`: popt removes the store into a register that is overwritten, so no temporary for the nested call; default first in the switch of expand, `C` behind `B`), match 239 > 57 (`register opmtemp`, `l = r = getlr`, `continue` per test), cbranch 297 > 24. Left in both: the place of whole blocks (popt). A block local takes the same word as a temporary outside its block (`int t` of the default of cbranch), a nested call as an argument takes a temporary (`rfind(.., rclass(..))` in expand): both show as frame words that are no variables of the function.
  mainp2 388 > 38, clocal 284 > 12, optim 199 > 77 from the PCC text and the reading of the original (`clocal`: `register o, pf, lf`, `p` an auto, `pf` and `lf` used again for the two types, `val` in a block behind the `while` of SCONV, `l` and `r` not read again before the tail, `return;` in case 3, 6).
  zzzcode 588 > exact: the variables by the original's registers and frame words (`z`, `l`, `q`, `r`, `u` registers; block locals per case at the same words: ZS `r2`, `n2`, `name`, `rn`, `rg`, `tmp`, `rp2`, `y`, `k2`, `cls`, ZA `n3`, `m3`, `k3`, Z0 `m`, `k`, ZP `a`, `b`, ZM `t`), nested calls as arguments, `tmp = 0` in each branch of ZS, `!tmp || (tmp && ...)`. Odd forms: `u` and `n` hold the types in ZT (a register used for another purpose), `if (u == 3 || u == 13) ;` for a `cp` the original has without a jump. The compensating change of the zzzcode if/else (commit cc1df27) is gone; the whole of D15 in zzzcode was source.
  regallo3 (2026-11-03): rsmerge exact, rsinsert 2 lines, getreg 170 and regjam 148, code of the program now smaller than the original's in the rounding (53504 against 53760). The rest of getreg and regjam, like rsmerge before its last source form, is block order, not content. Measured layout of the original, three cases of one shape: a block T that a conditional jump names and that ends with a jump stands in the dead spot behind the last unconditional jump in front of that conditional jump (getreg 0x7c56, 0x7d60, 0x7d74; regjam 0x797e, 0x79c8), the conditional jump points back to it and falls through. `codemove` of /lib/popt (z21.c) moves a block only to a jump that names it, and `cjmove` (OLD_D20C) refuses a block with a label or branch in it. Guess, to be tested in step 6: the old popt had one more rule of this kind; the same shape may explain the open placement in match, cbranch, mainp2, optim, sucomp.
  rsinsert left: the original reads `mode` from the frame behind the first test (`cp 0(r15),#2` at 0x76d2), ours from r6 by OLD_D3J; rclass and convelim show the opposite (original r6, ours the frame). So the rule of OLD_D3J and OLD_D3JM does not separate these yet; step 6.
  rsreturn, mapregs, rcmd exact (2026-11-03); mapnm 10, newnode 12, argscan 43, acon 2 lines. mapregs is a `switch` whose head stands behind the bodies; rcmd has its parameters on the frame (`ldm` of r5 to r7) and `regvar[i].rreg = r = a + 19`; mapnm has no `s` and no register variable at all, the words behind `ofs` are temporaries of nested calls. acon left: a `jr` of exactly 254 bytes, the original keeps it and /bin/cas takes `jp`. Guess: the old cas, the same family as D17; not tested. Nested condition `cls != a->cls || (cls != 1 && cls != 2 && cls != 6)` with the short case first gives the `jeq` chain of the original in rsreturn; the same form was not yet tried for the split block of getreg and regjam.
  getreg and regjam strictly exact (2026-11-03), code of the model now 53760 as the original. Correction of the entry above: the block order of the two needs no new rule of popt. It comes from the source: the split loop stands inside the first loop behind a `return` and is reached by `goto`, the common tail of the cases stands inside the last case, `case 1` and `case 6` jump to it. Mechanism, read from `iterate` and `xjump` of z21.c: with the tail in the last case, the label in front of it has no jump in front of it, so `xjump` merges the cases instead of `codemove` moving the tail. The `ldl` block behind the default (0x79c8, 0x7d74), which looked like a moved block, also came out of this form without any further step. Whether the authors wrote these jumps is not proved; a macro or another form could give the same code. The same form is still to be tried at the open placements of match, cbranch, mainp2, optim, sucomp, argscan, newnode.
  cbranch strictly exact (2026-11-03) with the forms found at getreg and regjam; argscan 43 > 7, match 57 > 40; newnode 9. Read from z21.c: a `jbr L` where the node in front of L is not a jump goes to the loop inversion branch of `codemove`, which moves the code from L to a conditional jump that names the label behind the `jbr`, within 24 nodes. A source with labels and `goto` therefore has to avoid such pairs, for instance `if (a || b) goto x;` directly in front of a label named by the code at x. match, sucomp, optim and mainp2 are not settled: their rest is the placement of single blocks (mainp2: the then part of `getc` behind the entry jump of the main loop), not yet explained by source form. No statement yet whether popt needs a change there.
  match strictly exact (2026-11-03), found with a trace of popt. How to trace: `make popt` in a copy of `src/cmd/popt/v0p` with `SWITCHES="-DOLD_D20C -DOLD_D20X -DOLD_D20L -DOLD_D20K -DOLD_D20B -DTRACE"` (the objects of oldlibc copied with their dates, so make leaves them), `ccv0p -S` without -O for the input of one function, `popt + in out`: `out` holds the nodes after each iteration and a line `/*CM ...*/` per block moved by `codemove`. Cause in match, shown by the first CM lines: the `jbr` of the first test met the candidate code (label behind a `jbr next`, the jump in front not yet turned round by iterate, which walks forward) and moved it. With the code inside `if (... == opmtemp) { ... }` the label stands behind a conditional jump and nothing moves. This corrects the entry above: match needed no change of popt, and its rest was not a loop inversion.
  argscan, newnode, mainp2, optim strictly exact (2026-11-03), all with the trace of popt and no change of popt; 158 of 209, 1400 lines. Source forms that decide what `iterate` and `codemove` do: `for (; c; )` against `while (c)` (mainp2: entry jump, the test behind the body); `while (c) continue;` against `while (c) ;` (two jumps in front of the label, so the conditional jump is not turned round and `codemove` runs first); `m->reg = n->reg = x` against two statements (the cross-jump stops at the store, so its jump has code in front of it and is not moved); a `while` with a `goto` into its body (optim) instead of `if`, which also means the original tests the joined tree again. optim had a wrong source before: `else if` where the original falls through, `a->type = p->type = p->left->type` where we set `p->right->type`; corpus unchanged. So far every block order that looked like a missing rule of popt has come from the source; only sucomp is open (the case fl == 0 in front of its test; a conditional expression gives its shared end, goto its place, neither both). The rule that the old popt needs one more transformation is not supported by any measurement now.
  options, tsuffix, eread, argplace, oreg2, tmpcleanup strictly exact (2026-11-03), scmd, tcmd and sortsyms close; 164 of 209, 883 lines. Mostly variables by the original's registers and frame words; the for form again in argplace and eread; `++cnt[x]` against `cnt[x]++` (the postfix form computes the address twice). Two compiler questions for step 6 with new witnesses. (1) OLD_D3J: at the label behind the first test of a word parameter the original has forgotten the parameter it just tested (scmd and tcmd `ld r7,0(r15)`, rsinsert `cp 0(r15),#2`), while it knows a parameter it did not test (rclass `cp r6,#19`). Guess, not tested: the tested register is busy when the jump is written, and the record of the jump takes only idle ones. convelim (`cpb rl6,#%6C`, a tested char) does not fit this yet. (2) Register choice: mapnm `ld r3,%8FE8(r1)`, sortsyms r2 and r3 again behind an `ld r0`.
  Do the popt switches force the goto forms? (owner's question, 2026-11-03.) Measured in copies, nothing in the tree changed: codgen at 206150ec (before the goto forms) and at d2f44c2e, each with popt v0p, without each switch and without all five. getreg 186, regjam 148, match 57, cbranch 24, argscan 43, newnode 12, mainp2 36 lines at 206150ec do not change in any of the seven variants, and at d2f44c2e these are 0 in all seven. So no switch of popt causes these forms or hides a form for them; a missing rule or another form is not excluded by this. Switches themselves, functions that depend on them (cparse, popt, codgen at d2f44c2e): D20C cparse oalloc 32, optim 27, codgen optim 27 (the same text); D20L optim 7 (cparse and codgen); D20X (with D20B) cparse defid 66; D20B popt xjump 11, backjmp 7; D20K cparse cinit 2, popt rmove 34 > 8, and it costs codgen geninc 14 and adrput 18, which are exact without it and without any change of source. Only seven functions carry all five switches. Since the forms of mainp2 and optim were found, each of these witnesses may be a source form too; not yet tested. With OLD_D20 added: codgen 164 > 162, worse.
  OLD_D20K removed (2026-11-03). Its two witnesses were source forms: cparse cinit `inoff += sz; return;` in case NAME, popt rmove do_and falling into do_arith. codgen geninc and adrput exact without any change of their source; cparse 178, popt 72, codgen 166 of 209, 851 lines; corpus and bench unchanged. `oldcc.sh matrix check` reports the table as stale (the row of the switch, 17 files changed since fc01a77); to be measured again when the other four switches of popt have been looked at, not after each one.
  OLD_D20L removed (2026-11-03). Its one witness is optim, case MINUS, the same text in cparse and codgen; two copies of one text are not two witnesses. Forms tried without the switch: `for (; cond; )` 7 lines as `while`; the goto loop with the test inside the `if` exact, in both programs. This one goes the other way than OLD_D20K: the source reads worse than the `while` with the `goto` into its body, which remains a possible original with a rule of the old popt that we would not have. Not decided by the bytes.
  OLD_D20C removed (2026-11-03). Two independent witnesses, both source forms: cparse oalloc, cparse and codgen optim (forms in OLDCC.md). No loss: cparse 178, popt 72, codgen 166, bench and corpora unchanged. Left of popt: OLD_D20X (defid), OLD_D20B (xjump, backjmp).
  OLD_D20X and OLD_D20B removed (2026-11-03), source forms in OLDCC.md; popt v0p has no switch left: cparse 178, popt 72, codgen 166, corpora and bench unchanged. Result of this round: all three models, built with the original /lib/popt as the pass of ccv0p, are byte-identical (`cmp`) to those built with our popt v0p. So for the passes cparse, popt and codgen nothing is left that asks for an older popt than the one shipped: all the D20 rules of v0p (C, X, L, K, B, S, M) were source forms or are replaced by them. The earlier statement that v0p has an older popt has no support any more. OLD_D20 (v1, ld) is not touched. The same question stands for the switches of codgen and cparse, not measured.
  The original /lib/cparse and /lib/codgen as passes of ccv0p (2026-11-03), the same test that showed popt to be the shipped one. Models, functions strictly equal against 178 of 179 (cparse), 72 of 73 (popt), 166 of 209 (codgen): with /lib/cparse 19, 9, 12; with both 30, 7, 5. With /lib/codgen alone our cparse v0p fails on reader3.c ("Compiler error") and no model comes out; the intermediate form of v0p cparse and /lib/codgen do not fit in every case. So these two passes are older than the shipped ones, unlike popt. The test is biased: our sources are fitted to v0p. A gap of this size is not explained by that alone (guess), and it says nothing about a single switch of cparse or codgen, which can still hide a source form.
  Single-witness switches of codgen (2026-11-03). Removed, source forms: OLD_D3V (defid `idp = hide( p ); q->rval = (int) idp;`), OLD_D16B (stref `p->type = p->type + PTR` instead of `+=`). No form found, switch stays: OLD_D16A (genswitch `p[++n]`; `p[n = n + 1]` gives `lda r2,1(r6)`), OLD_D3JM (equop; separate `if`s change nothing, the label still has two jumps from `&&`), OLD_D6S (falloc; only a `?:` as switch value tried by reading, no form written). Not tried, register choice or local arrays, no idea of a source form: OLD_D2MO (dclstruct), OLD_D2R0 (gotscal), OLD_D2O (genswitch), OLD_D5A (dclargs), OLD_D5T (procopt). OLD_D12K is the owner's exception. Refuted, guess of this day on OLD_D3J: the record of a jump takes only idle parameters: gains fcmd, rsinsert, scmd, tcmd, but loses 11 others (cparse defid, inforce, rmconv, tfree1, zecode; popt findword, findlong; codgen tagfind, tshape, free1, rcmd and more lines in others). The tested parameter alone does not separate the cases. OLDCC_SWITCHES.md is stale again.
  freereg strictly exact, efcode 5 and special 2 lines (2026-11-03). special: `sub r2,#%8001` at the head of a switch with negative values, ours `add r2,#%7FFF`; literal, long and unsigned case constants give the same as `SPECIAL + 1`, so this looks like a question of the compiler, to step 6. efcode: `parmax + maxoff` in rr2 and moved to rr6 for the division, ours directly in rr6; a cast does not change it.
  setbin 44 > 8 lines (not exact), readsyms and readmems 2 lines each (2026-11-03). Both are parameters again: readsyms loads n from the frame behind the test of n, readmems tests n in the frame at once, unlike every other function (it has a static local in front). OLD_D3J question, step 6. setbin: the tail of the two order calls stays in the wrong copy; `goto r1` with a common return label moves more blocks (37 lines).
  rddata, readnums, conput, reclaim, fixtemp, allo strictly exact, regargs 2 lines (2026-11-03); 173 of 209, 304 lines. No compiler question among them: frame words, if/else in place of ?: for constants written to the frame (`ld 8(r15),#1`, `clr`), a chain `a = j = f()`, `x = x >> 3`, no `!= 0` behind an `addl` (the flags are used directly, with the comparison the compiler writes `testl`).
  shtemp, cbgen, usable, rallo, gencall strictly exact, upput 4, allocreg 4, tcopy 11, freeparm 9, notoff 10 lines (2026-11-03). notoff: the original does not store two of its parameters; which rule leaves a parameter unstored is not known (regargs skips one known in its register, how that comes about here is not read).
  rmove, lmuldiv, rsmatch, symreg strictly exact (2026-11-03). rwprint and readmems are the only functions found so far that test their first parameter in the frame at once; both have a static local. Guess for step 6, not tested: the old cparse writes a record for it in front of the first statement that makes codgen forget the parameter; popt has functions with static locals too, so this has to be read against them.
  Step 6, parameters known in a register (2026-11-03). OLD_D3AW widened: the top of an argument goes on through unary minus, multiplication and a cast. scmd, tcmd, fcmd, readsyms, readtab exact, nothing lost in the three models; so "forgets the tested parameter" was wrong for these: the cause is the wanted register of the argument. readmems and rwprint exact with an unreferenced label in front of the first test (a label no jump has named forgets parameters, OLD_D3K); the source form is a guess. A static local is not the cause (measured with small tests). Tried and not adopted: clearing the jump records at a loop head (rsinsert exact, allocreg 4 > 7 through a backward label of my goto form, one witness); restoring at a label with several jumps (rclass exact, cparse buildtree +8; the same with only idle parameters); a free pair counted for a byte (nothing, convelim stays). Why convelim forgets differently is not known.
  Register choice, attempts of 2026-11-03, none adopted, all in copies. sortsyms: after the `ld r0` the free word r1 is taken, the original takes r2 and r3. Giving back the idle mate of the first free word makes sortsyms exact, but costs 17 functions (cparse buildtree, conval, moditype, resetbc and others; codgen rallo, tshape, tagfind and others). A value loaded through an index register not remembered: no change. The comma form of the swap: no change. mapnm `ld r3,%8FE8(r1)`: r1 not reused by OLD_D2R when no register is wanted: no change, so r1 does not come from there; where it comes from is not read. efcode: no preference for the left operand of a long sum under MUSTDO: efcode exact, readmems +15 (`fseek(stfile, stg + 2, 0)` wants it); it would separate only by the register (rr6 against rr4), one witness, not adopted. acon: not looked at again, the old cas is untested.
  Tools (2026-11-03): host/recon/oldvar.sh and poptrace.sh, from the scripts of the work on the popt switches.
  codgen, names of PCC (2026-11-03; older entries of this log use the old names): fwalk > walkf, fwalkd > fwalk, free1 > tfree1, dopeinit > mkdope, rfreenode > recl2, ltypeok > shltype, stmt > p2compile, options > p2init, rallkind > callreg, errexit > dexit, funcend > eobl2; struct indope > struct dopest with dopeop, opst, dopeval. Each pair read in both texts. Not renamed, no PCC function does the same: delstrip (not deltest), init (not p2bbeg), rcount_reset (not allo0), orderadd (not setincr), icright (not hardops), fixfld (not rewfld), prline (not lineid), oregput, prlong. Checks of a rename compare the images (text, data), the symbol tables differ.
  scodgen corpus 269/269 (2026-11-03). The six open cases were two errors of our source under SEG, read from /lib/scodgen. oregput tests `p->lval + maxoff`, ours `frame0` (the bars around _stkseg+~L1+N(fp): doscopy, pburn). setasop gives the pointer of a long multiply or divide 0x1002 at its four stores, ours had the 0x1001 of codgen (getreg asked for register 1 in class 3, reclaim then stopped: codgen_optim3, cparse_optim1). Found by comparing the arguments of all getreg calls of both programs under `z8k-emu -t`. No layout effect in these cases: before the fix, 8, 64 and 1024 words of bss added at the end of bss3.c left the results of three of the cases unchanged in our binary (the original was not tried with another layout). The layout dependence noted for step 2 of the codgen work is another observation; its cause was not looked for. Both fixes are under SEG, so codgen should not change (images not compared); measured afterwards: corpus 269/269, models cparse 178, popt 72, codgen 190.
  Layout dependence of codgen and scodgen, measured (2026-11-03), scratch copies, all 269 cases of each pass. Common data in bss3.c in reverse order: all same. 64 words of zeros behind each definition: all same. Array sizes of before the module step (fname, opst, dope, numtab, savecls, saveorig, savereg, resc, opptr): all same. Reverse order and padding also at the module commit 991abc96: all same as its control (scodgen 263/269, the same 6). A stack filled with A5A5 below the first frame (0x1800 bytes, emulator patched in the scratch copy, checked with a small program that prints an unset local): the originals /lib/codgen, /lib/scodgen and ours give their normal output in all cases. So no dependence on the order of the common data, on a neighbour, or on unset locals in the reach of this corpus. The sentence of step 2, "the program depends on its layout", is not reproduced by these variants: the intermediate state it was seen on is not in git, its cause stays unknown. Not tried: filling the bss or the heap, because the programs may rightly rely on zeros there.
  Function work of codgen, round of 2026-11-03: 19 > 15 open, 146 > 93 lines. rsfreeall, tcopy, upput, notoff exact, sucomp 32 > 12. Forms: sucomp `?:` at two fsu assignments, because rmove runs only after the first comjump and the store of each branch decides what falls together there; rsfreeall goto ret into the last case, no default; upput the label ac behind a break; tcopy a call of rbusy with two arguments (a quirk of the original, KNOWLEDGE.md); notoff the fourth parameter of the PCC. Codgen, OLD_D3J: d3jmark only for a long parameter (notoff 2 > 0, nothing else moves in the three models, bench 118, 275). Corpora 269/269 in both passes, models cparse 178, popt 72, codgen 194.
  allocreg, clocal exact (2026-11-03); 13 open, 77 lines. allocreg is a plain if / else if / else, the goto form of before was not needed. clocal: the cases of pf in the order 2, 12, 14, 4 and p = l; goto ret in both arms. Measuring: the bench runs ccv0, so a change of codgen needs `oldcc.sh build codgen v0` first; the bench figure of the entry above was taken with an older lib/v0/codgen. Measured again with v0 rebuilt: 118, 275, also with d3jmark. A rule with one witness: forgetting the jump records of OLD_D3J at a label no jump has named makes rsinsert exact, nothing else moves in the three models or the bench (adopted in the next entry).
  rsinsert, regargs, setbin exact (2026-11-03); 10 open, 65 lines. d3jmark for long parameters and d3jclear stay on the owner's word, each with one witness. regargs and setbin lost all their goto: regargs is the if / else if / else of its SEG branch, setbin has the test of istnode(l) with its two calls written out twice and popt lays out the blocks. Corpora 269/269 in both passes, models cparse 178, popt 72, codgen 199, bench 118, 275 with v0 rebuilt.
  rclass, convelim, buildstar exact (2026-11-03); 7 open, 60 lines. OLD_D3J: parameters come back also at a label with several jumps (the cost of cparse buildtree that spoke against it is gone since d3jclear), and the record ] does not count as the record in front of a label. Cost nothing in the three models and the bench.
  Strict comparison, what it did not see (2026-11-03). norm_strict masks the targets of jumps and calls, so functions counted as exact could jump elsewhere. strictcmp.py now also requires, for functions with the same lines, that each jump goes to the same instruction or known function. Checked with a deliberate error: regforsym without its third argument counts as different. Found and corrected, all in functions counted as exact: buildstar called clocal only in default (the original calls it after the switch, also for STAR and UAND); regsave returned when tot is 0 (the original skips only the integer registers); allo used 2 words for a float temporary for parameters (the original 4); regforsym lacked the third argument of symreg; rsinsert returned for another mode (the original goes on with a and b unset, quirk table); conval had ULE in front of ULT (only the order); cparse opact had a break behind PLUS (the original falls through into SUBSR, no effect on the result). The first four are differences in behaviour. So earlier figures of exact functions are too high by these. Corpora 269/269 in both passes, cparse corpus 253/253, models cparse 178, popt 72, codgen 202, bench 118, 275.
  Register weights of v0 (2026-11-03): rweight { 3, 1, 2, ... } > { 3, 4, 2, ... }. mapnm exact, sortsyms 26 > 8, nothing lost in the three models, bench 118, 275 > 119, 246. Also measured: r1 = 2 and 3 gain less (58, 44 lines), r1 = 5, 8, 9 cost cparse and codgen functions, { 4, 5 } makes sortsyms exact but costs much more (252 lines), { 2, 3 } leaves cparse and codgen without a model. OLD_D2G: an idle r0 does not win a tie of weight: sortsyms exact, bench 121, 239. 5 open, 30 lines: sucomp 12, freeparm 9, efcode 5, special 2, acon 2. Corpora 269/269 in both passes, cparse corpus 253/253, models cparse 178, popt 72, codgen 204.
  freeparm, special exact (2026-11-03), on the owner's word with one witness each, no cost in the three models, bench 121, 239. OLD_D3L (codgen v0p): a word cast of a global long is not taken from the register that holds the long; with the rule for all names rddata, readtab, regsave, regrestore and conval are lost, so locals, parameters and register variables stay out. cparse OLD_D6D: the base of a direct switch stays a long.
  Status of the three passes without the old cas (2026-11-03, decided with the owner). Strict, with jump targets: cparse 178 of 179, popt 72 of 73, codgen 206 of 209; lines: cparse 2, popt 8, codgen 19. Of these cparse yylex 2, popt rmove 8 and codgen acon 2 are only jr against jp (the old cas, D17). Left without the cas: sucomp 12 and efcode 5, both open in findings.json with what was tried. Corpora 269/269 in both passes, cparse 253/253; the popt corpus was not run again in this round (popt was not changed).
  Matrix of 2026-11-03 (49 switches, parallel, 7.5 minutes): OLD_D2R0, OLD_D2RA and OLD_D3JM show no lost or gained function and no line. Removed with their code, the three together: models cparse 178, popt 72, codgen 206, corpora unchanged, bench 121, 239. Neutral and kept: OLD_D18 and cparse OLD_D12, whose effect the comparison treats as equal or the model makes itself. One witness in the table: OLD_D12K, OLD_D16A, OLD_D2MO, OLD_D3L, OLD_D5A, OLD_D5T, OLD_D6S. Not measurable alone: OLD_D22 (no model of cparse), OLD_D3K (no build), cparse OLD_D6T (no model). popt corpus 816/816.
  spopt (2026-11-03): the sources of popt with -DSPOPT, built with ccv0p, tools in src/cmd/spopt/tools (d12model takes CCFLAGS of progcfg). 24 > 72 of 73 functions strictly exact with jump targets, without a switch. The left function is rmove, three jp against jr: the old cas, the same family as in popt. Source: movedat as in the original (quirk: *prev = 0 through a pointer not set, hd not cleared; the clearmem of before was ours), adrlen and ilen in the form of popt, usesreg CALL reg < 2 first, procopt b[14]. Native build `make spopt`, corpus 805 of 807 runs (spopt/tools/corpus.sh, parallel, 23 s, on the *.out.s of the scodgen corpus); the 2 differ only in the line "NK core" of -d: 32K against 33K. Probably the place of the heap in the normal build, whose data layout is not the original's; not proved.
  scparse (2026-11-03): the sources of cparse with -DSEG, built with ccv0p, tools in src/cmd/scparse/tools. 43 > 178 of 179 functions strictly exact with jump targets, data equal in size. The left function is yylex, jp against jr, as in cparse. /lib/cparse and /lib/scparse differ in 16 functions; all of it under SEG: pointers of 32 bits (SZPOINT, ARGINIT, tbytes, twords, bsize, getpreg, setreg, regfree), the mask 0x7f00ffff of segmented constants (chkpun, conval, clocal), .psec records (newsect, locctr, efcode, up to 127 sections), MAXRVAR 13, register name sp, the section check of a dimension (vinit1), the direct switch of pointers. Corpus with the ccv0p build 253/253, cparse 253/253. The normal build with /bin/cc gives only 232 and 233 of 253: all differences are in t.S, the heap contents of the symbol file (TODO.md), a program whose output depends on its data layout (putsym reads through dimtab[-1], KNOWLEDGE.md). This is the layout dependence the owner remembered; the corpus of cparse and scparse only holds with the ccv0p build. stagecorpus.py runs in parallel on a local disk: 6 s instead of minutes.
  scodgen, first round (2026-11-03): the sources of codgen with -DSEG, built with ccv0p, tools in src/cmd/scodgen/tools. 63 > 194 of 209. The largest step was the common data: scodgen has its own order and regvar[11] (bss3.c under SEG). Further: register name sp, regE 13, shincr, oreg2, setasop, conval, acon, upput, usable, gencall. Corpora and models of the other passes unchanged. Open: zzzcode, gswitch, chk, rallo, clocal, sucomp, offstar, tsuffix, usable and smaller ones.
  scodgen finished without the old cas (2026-11-03): 206 of 210 functions strictly exact with jump targets, 20 lines. Left: sucomp 12 and efcode 5 (the same as in codgen), zzzcode 2 (one jp against jr, the old cas), usable 1 (add r2,#0, form of the zero not found). Also in SEG form: gswitch (own function), chk, mkoreg in front of orderadd, offstar, clocal, case T and u of zzzcode, sucomp (max at STAR). Corpora 269/269 in both passes, models cparse 178, popt 72, codgen 206, spopt 72, scparse 178, bench 121, 239.
  Matrix over six programs (2026-11-03, 46 switches, 7 jobs): no switch is neutral but OLD_D18 and cparse OLD_D12. The switches that had one witness now show two functions (OLD_D12K, OLD_D16A, OLD_D2MO, OLD_D3L, OLD_D5A, OLD_D5T, OLD_D6S), but these are the same source in cparse and scparse, codgen and scodgen, popt and spopt: not independent witnesses.
  cpp, start (2026-11-03): sources of the cpp in cmd/cc of the Plexus tree (src/cmd/cpp), model with ccv0p linked with the shipped libc.a for now, 94 % of about 2900 instructions equal when addresses are masked. Own code ends at 0x2272. Module stamps from 10/82 and 12/82, older than the passes. To do: stamps of cpy.y and yylex.c, the libc part (unlink spec; printf 0_5, endopen 0_3, flsbuf 0_2, cnvt 0_2, malloc 0_3, findiop 0_2, fopen 0_1 and others match), a flag in sayline with a second line form, the line directive in control.
  cpp (2026-11-03): sources of the Plexus cpp, built with ccv0p: 23 of 26 own functions strictly exact, 10 lines (dodef 4, subst 4, slookup 2), all instructions in the same order but these. Own data and bss as in the original (bss in the original's order in one block of cpp.c). The program is combined, so strictcmp shifts the data addresses of the model by the difference in code size (COMBINED in progcfg). That difference is 48 bytes; 40 of them are 20 places where the original has `call` and the shipped cas writes `calr` (D17), the other 8 are not explained yet. ccv0 measured against it: about the same lines but others, so ccv0p stays. WEGA additions found: option -B (LINE form), the line directive as a comment, _sobuf shared with flsbuf. Removed against the Plexus source: the WARN mechanism of escaped newlines (speakup), a flag sayflag instead. Corpus 270 of 270 sources, same with -P, -C, -B, -D, -U and a missing file.
  cpp source style (2026-11-03, owner): the additions of Plexus are gone, the model is the cpp of System III (cmd/cc/vax/cpp), plexus and m68 removed. z8000 stays in the COFF test and as a predefined symbol: the original has the string z8000 and COFF 128 (fastab + 0x80). The block that defines PWB as empty stays out, because `# if PWB` is then a syntax error. No byte moved.
  cpp, status without the old cas (2026-11-03): 25 of 26 functions strictly exact, then 26 of 26 (below). subst and slookup read their parameter from the frame with an unreferenced label in front of the first use (OLD_D3K; a guess at the source). libc part: ffltioc has four stubs, doprnt 0_7 lacks the test of the smallest long of the other versions. The code is 40 bytes shorter than /lib/cpp: exactly the 20 places where the original has `call` and the shipped cas `calr` (D17). Corpus 270/270. A rule tried and dropped: forgetting parameters at a register parameter changed nothing in cpp and cost cparse, popt and codgen functions.
  cpp dodef (2026-11-03): OLD_D16A now spares both operands of a compare, not only the left one (replaced below). With it the form of System III, `--qf>=formal`, gives the bytes, and the helper form `formal<=(--qf, qf)` is gone: 26 of 26 strictly exact, corpus 270/270. cparse, popt, codgen, spopt, scparse, scodgen unchanged (178/179, 72/73, 206/209, 72/73, 178/179, 206/210). Second site for the rule of D16.
  OLD_D16A, correction (2026-11-03): the entry above took dodef as a second site that separates the two readings of D16. It does not, codemove and dodef are both `--` under a compare. Measured: `p->op == ASG PLUS` without any compare state gives the same counts in all seven programs as the compare exception. Taken, d16acmp is gone. cpp again 26 of 26, corpus 270/270.
  cpp, whole file against /lib/cpp (2026-11-03), scratch script over built binary and original: 4401 instructions on both sides, jr 651/651, jp 90/90, call 118/98, calr 99/119. 3387 same bytes, 912 words that map to shifted addresses, 82 jr with a changed distance, 20 call against calr with the same target. 833 jump targets map, none wrong. Data: 1577 words same, 132 addresses shifted by 40, none unexplained. Calls in reach, original: forward `call` in cpp.c up to 0x11ac, `calr` from 0x1448; backward always `calr` in the same module; `control` to `dump` has both forms only by reach (-3910 and -4332).
  Owner's question: can another split into files cause the 20? No. Such a boundary would lie between 0x11ac and 0x1294, and 17 backward `calr` of subst and main cross it (subst to skipbl, subst to unfill, main to stsym, main to ppsym when it lies before the boundary), while no call between known modules is `calr`.
  cpp with the test cas (2026-11-03): `castrig/hostcas.patch` over a scratch copy of host/cas; without a trigger cpp.o equals the normal build; with the trigger at the jpr/callr of ordinal 364 or 371 of cpp.s, ld and strip of WEGA: 16522 bytes, cmp with /lib/cpp finds no difference. The point is read from the original, the rule that makes it is still unknown. A clean build of host/cas fails: parser.c is Bison output and uses yyerrflag, `make` only works with the old parser.o (not touched).

The rule of the old cas (2026-11-03, D17).
- Owner's document: System 8000 ZEUS Languages, Programming Tools Rel 3.2 (03-3249-01, 05/83), AS section 3.4.5, and the same text in Band B: jump optimization runs before the end of pass 1 at a location counter assignment (`.=.+20`) and at a constant expression with the difference of two relocatable values with an optimizable jump between them. Tested with small sources: neither the shipped cas nor the ZEUS cas does this, so the text describes an older cas. `.=` stands in the data section in our inputs and no difference occurs, so these two conditions explain none of the points; they are not modelled.
- Data base `castrig/dataset.py`: 56 modules of the seven programs, 17980 sites with what cas knows there (tables, entries, i and s file offsets, what optim would see, dependency records) and what the source has up to there. `feas.py`: exact interval test of sums of up to 3 counters with whole weights 1..8: no fit on all windows (0 of 21, 20, 15 windows; 4194 fits on 11 windows, too few to mean anything). Single counters, the file offsets and the dependency records do not fit either.
- What gave it: printing the function of each free site. Most windows hold the first site of a function. So the counter is looked at only between functions, and it does not matter whether the count at the point is 179 or 205: it is the first boundary behind the limit. Only `yylex` and `rmove`, too long by themselves, reach a second, hard limit in the middle of a function (255, the class 2 of the earlier round).
- Rule (`castrig/boundary.py`; K entries at a function boundary, H inside): K 176..179, H 255: 4 wrong of 220 long and 9254 short sites (no trigger: 220), the 4 are putnames twice. Fixed on cparse and popt alone: K 174..179; on codgen and cpp then 0 wrong of 32 at K 177..179. On popt alone K 174..181, on codgen alone K 176..181.
- In a real cas (`#ifdef OLD_CAS` in host/cas and src/cmd/cas; `casold`, `src/cmd/cas/old`, installed as /lib/old/cas of ccv0 and ccv0p): first at the closing `}` gave 7 false long calls, all to the next function; so the label of the next function is known at the point, and the test stands in the opening `{` (blkopen). Then, strictcmp -c = strict for all programs but putnames: cparse and scparse 178 of 179 (and strict 179 of 179), popt and spopt 73 of 73, cpp 26 of 26, codgen 206 of 209 and scodgen 207 of 210 with only the known source differences. Real builds at the limits: K 175 and 180 give errors, K 176 and 179 none; H 254 and 256 give errors (yylex 14; yylex, rmove): K 176..179, H exactly 255. OLD_ENTBLK 178 is a choice inside the range, nothing separates the four values. Why 176..179 is not known.
- cpp: `make cpp` with ccv0p (the driver runs /lib/old/cas) and strip give a file identical with /lib/cpp, without a point set by hand. The plain build of src/cmd/cas is unchanged by the #ifdef (compared with a build of the commit before).
- Independent test on a program of another time, not exact in its source: dc of 07/82 from the System III dc.c, ccv0: wrong forms among the aligned jumps and calls 45 without the rule, 5 with it (WEGA splits dc into dc1.c, dc2.c, not yet looked at). m4: no statement, WEGA's m4 aligns with the System III source only 7 to 19 %.
- Whole files after the rule, `make PROG` with ccv0p and strip, cmp with the original: cpp, popt, spopt, cparse, scparse byte-identical. Two wrong libc versions in popt and spopt came out on the way: length 0_2 has `cp r3,#%0200` where they have %4000, stty 0_2 lacks stty (the function; 26 bytes): length 0_3, stty 0_3. In cparse and scparse the two call forms of putnames: sputc and aobeg are a module of their own (sym2.c; the name is a guess), calr cannot leave a module.
- Second difference of the old cas, from acon: `jr` at distance 256 whose target instruction is a long call. In a forward jump jdeps counts a jpr that stands exactly at the label (`v >= q->j_2`), though its growth lies behind the label: 2 bytes too much, the shipped cas makes the jump long. In the old cas it is `>`. acon exact, no other function in the seven programs moves. One function, but only one place in all the programs has this distance, so there is no second witness to be had there.
- scodgen linked its libc part in another order than the model; now the same LIBCOBJS as codgen. Left for whole files: codgen and scodgen efcode (2 bytes, all addresses behind it) and sucomp (12 lines), scodgen usable (1 line). All are source forms, not cas.
- dc as the independent test, closer (System III dc.c unchanged, ccv0, casold, compared with /bin/dc by alignment of the code, ratio 0.879, 1448 aligned jumps and calls). In one file 5 wrong forms (45 without the rule); two of them, dscale to add0 and the backward binop to pushp, ask for a module boundary between pushp and add0, and WEGA has dc1.c and dc2.c. Split in front of add0, 1 wrong form (load to putwd); in front of pop 10, in front of readin 6. The split also moves the triggers, because the counting starts again in the second file, and that took the two calls of copy to nalloc along. dc is of 07/82, had no part in finding the rule, and is not rebuilt exactly.
- ld with casold could not be measured here (its model links with host/ld/ld, not there). Whether the programs of v1 had the old cas stays open; the driver of v1 keeps /bin/cas. The dates known: optim.c of the shipped cas 1.2 05/02/83, ssymtab.c 1.3 05/06/83, ld built 07/26/83.
- Closing the night's work: clean builds with ccv0p and strip: cpp, popt, spopt, cparse, scparse byte-identical. The bench of v0 goes from 121 to 125 of 153 functions and from 239 to 219 lines by the old cas alone (no pass changed). Matrix measured with the old cas in the chain. stty 0_2 removed: it was a wrong cut, stty without the function stty; no program links it now. Single-source switches in the table: OLD_D12K, OLD_D16A, OLD_D2MO, OLD_D3L, OLD_D5A, OLD_D5T, OLD_D6S each rest on one function and its segmented copy.
- Limit of the K scan: in the real build at K 176 the run of scodgen failed (a file was missing, the runs shared a directory), so K 176 rests there on the simulation of `boundary.py` only; the other six programs were built at K 176. K 177 was not built separately either.

Bytes of codgen and scodgen (2026-11-04, owner: they must be identical now).
- efcode is a source form after all: `sz = parmax + maxoff; sz /= 8;` as PCC writes it (`spoff /= SZCHAR`) gives the rr2 and the move to rr6. The switch candidate of 2026-11-03 is not needed.
- usable, `add r2,#0` in `(rv >> 8) - 1 + 1`: cparse runs optim once, the sum becomes x + 0 at the last step and stays; the second optim, in codgen, takes it away. OLD_D6Z (v0p): the optim of codgen does not remove a zero addend. Taken alone it costs what codgen itself builds: a member at offset 0 and an index 0 (mapregs, op 120 and STREF); these go at once (d6zdrop). All seven programs otherwise unchanged. One witness, by order of the owner. Removing the zero in cparse as well is wrong (genswitch, clocal, instk, sputc, memreg, killreg, cleargroup).
- sucomp by search: the trace of popt (`-DTRACE`) shows the mechanism. The first max of the assignments keeps its `ld r2,fl` only when it is merged in the first comjump, before rmove takes the load away; for that its jump needs the same label as the later copy, and that comes from xjump in iterate, which looks one hop along a chain of jumps. So the form depends on how many jumps stand behind each store, and it was found by trying all combinations (`if` or conditional expression per statement, `return` or else chain; 384 builds, a scratch script): `if` / `else` everywhere with an else chain in place of the returns of the BITYPE case: 0 lines. It is also the plainest form.
- The rest of the file: table3 is linked behind comm3 as in the model; szty has `case 10: case 14:` beside `case 4:` (two words of its jump table); codgen and scodgen take length 0_3.
- 2026-11-04: `make PROG` with ccv0p and strip: cpp, popt, spopt, cparse, scparse, codgen, scodgen all byte-identical with the originals, from a clean rebuild of the chain. Corpus of codgen and scodgen in the normal build: no differences.

OLDCC.md cleaned up (2026-11-04, owner: it holds only what is currently fact). Its text before is in git: `git show e1001dd6:WEGA/host/OLDCC.md`. Evidence rules moved to HOWTO_reconstruct.md. Moved here as they stood:

### Time box (owner decision 2026-10-31)

- A round is one hypothesis for a switch (one D class, one assumed mechanism), made and measured under the evidence
  rules. Progress: two findings in independent functions become exact without a regression, or a new measured
  explanation (for example the place of the mechanism in a pass).
- A D class is parked after three rounds in a row without progress, and also when its switch would change the normal
  build or need a large rebuild of a pass (for example a new table structure in codgen). Parked: measurements and attempts
  in `OLDCC_LOG.md`, go on with the next class.
- The whole work stops (state to the handoff) when five classes in a row are parked or no open class is left.
- Review by the owner at the first milestone or after 20 rounds.

Removed and refuted, from section 3:

- Removed 2026-11-03, no effect left in the models or the bench. v0p (switch `OLD_D2R0` in codgen, offstar and freereg): an address computed in r0 is moved to another register
  for the indirection; the value loaded through it goes back into r0 when r0 is free (gotscal `inc r0,#12; ld r1,r0;
  ld r0,@r1`). Bench v0 277 > 278: v0p only.
- Refuted (ld and bench): registers kept at labels with their contents forgotten (bench 69 > 54), only at labels no jump
  has named yet, the parameter registers kept at labels, merged pairs put behind the others, freed words in LIFO
  order, symreg only on idle registers. Two ld findings were source forms (ranlib_scan as a `for` loop, dseek as one
  `||`), strip's expression went with v1 without a D2 switch.
- Removed 2026-11-03, no effect left in the models or the bench. v0p (switch `OLD_D2RA` in codgen, match and offstar): an address `r0 + rN` moved out of r0 goes to rN when that is free
  again (popt itoa_ `add r0,r3; ld r3,r0; ldb @r3,rl2`, reverse `add r0,r2; ld r2,r0`); an address made by a constant
  keeps the lightest register (cparse `add r0,#dimtab; ld r1,r0`, gotscal, codgen). Bench v0 not touched.
- Removed (2026-11-03): OLD_D3V. One witness, a source form: defid `idp = hide( p ); q->rval = (int) idp;`.
- Not a difference of the compiler (OLD_D3N, removed 2026-11-02): "the value of an assignment taken from its register in
  the next statement" was source forms and the frame slot knowledge of popt. `fprintf(..., l)` behind `l = lab = getlab()`
  takes r2 because rmove replaces the load of the slot stored last (prtdcon); where comjump merged two arms first, rmove
  no longer knows the slot and the load stays (repladdr `ld r5,8(fp)` at the merge, `ld r5,r2` in the next branch). The
  value in r2 inside one expression: `t->rval = swlab = getlab()` (genswitch), `*(cp = &ftitle[len-1])` (lineid),
  `*(long *)(ip = &paramstk[paramno]) = l` (psavel), `(lp = nonlab(p))->op` (refcount); a separate statement tests the
  variable (findcon).
- Removed 2026-11-03, no effect left in the models or the bench. v0p (switch `OLD_D3JM` in codgen, d3jlabel): at a label behind a jump named by several jumps the parameters known at
  all the jumps and still known in front of the label stay (popt equop: `ld r2,r6` at the else label of `A || B || C
  && D` behind a short then part; buildtree reloads because its then part has calls). Without the condition on the
  code in front (the state of the jumps comes back): buildtree 6 > 14. Bench v0 unchanged.
Removed (2026-11-03): OLD_D16B. One witness, a source form: stref `p->type = p->type + PTR`, `p->left->type = p->left->type + PTR` instead of `+=`.
- Refuted: going on behind a block moved from behind a jump (ld load2td exact, bench 72 > 69).
- Removed (2026-11-03): OLD_D20C. Both witnesses were source forms. oalloc: not one `if ((A) || (B))`, but `if (A) { both: ... } else if (B) goto both;`, the block stands behind the first test, which is what the switch produced. optim: `if (...) { zapleft: ... } if (...) goto zapleft;` instead of a `while` with a goto into its body. Both read as plainly as the forms they replace; two independent functions, so unlike OLD_D20L the rule of the switch had two witnesses, and both fell.
- Removed (2026-11-03): OLD_D20L. One witness, the same text in cparse and codgen (optim, case MINUS). A goto loop with the test inside the `if` gives the bytes without it (`goto minus1; minus2: if (...) { ...; minus1: if (LO(p) == MINUS) goto minus2; }`): the jump to minus1 stands behind code that falls through, so codemove takes the loop inversion. The `while` with the `goto` into its body read better; which of the two the authors wrote is not known.
- Removed (2026-11-03): OLD_D20K. Both witnesses were source forms: cinit (`inoff += sz; return;` in case NAME instead of `break`) and rmove (do_and falls into do_arith, no `goto arith1`). What decides: the then part of an if/else jumps into the `call` of the else part when the label behind the else part holds no jump (`xjump`); behind a `break` or `goto` the jump is followed first. Without the switch codgen geninc and adrput are exact.
- Removed (2026-11-03): OLD_D20X and OLD_D20B. Source forms: popt xjump and backjmp `while (p = p->back) { if (c) continue; break; }`: the `continue` leaves the back jump alive, and the shipped code motion inverts the loop; cparse defid the dimension loop as `temp = type; goto dimtest; dimloop: ...; temp = DECREF(temp); dimtest: if( temp&TMASK ) goto dimloop;`: the entry's store cross-jumps to the store of the step. So popt v0p needs no switch at all (see the end of OLDCC_LOG.md, 2026-11-03).
- Not placement rules of popt: movedat (the whole body inside `switch (r1->op) { case CODE: case DATA: ... }`), dest (`case SBYTE` in
  front of `case 0`; the `sub r2,r3` of the original follows, the copy knowledge of rmove is lost only behind a label), the scan loop of setreg
  (the test on `(` and `@` with its consequences inside `while (*s)`). All three were source forms.
- Not placement rules either: optim (`zapright:` inside `else if( ... ){ }`, no `goto zapright`), defid (the loop step behind `goto mismatch`; an odd form,
  see findings.json), most of usesreg (`if (a != x && a != y) { dflt: ... continue; } goto out0;`, `if (...) { out1: ...; return 1; }`). What decides is
  whether the node in front of a target is a jump: `if (c) goto L; L:` leaves `jpr L2; L:` and codemove takes the block, `if (!c) { L: ... }` does not.
- Refuted (2026-11-02): a block reached only by conditional jumps moves in front of its target instead of the target up (OLD_D20M again, all lengths, with a branch
  inside, longer than the target): cparse about 1200 to 1400 lines, popt about 1250 to 1300. No move behind a jump to itself (cparse 471), no move behind a conditional jump over a jump (cparse 166).
- Candidate, not set (one witness): the load of a switch value in front of a dispatch behind a jump (`Lx: ld r2,r13; jpr D`, argregs): argregs 17 > 14, nothing else changes.
- Removed (2026-11-02): OLD_D20S. getpreg `if( pregvar < 2 ) break; goto gotreg;` is exact without it, and it turned the POP arm of usesreg the wrong way (usesreg 35 > 6).
### D21 (removed 2026-11-02: not a difference of the compiler)

The counts of the versions on the way:

  v0 64 (v1 without OLD_D1: 15; + OLD_D2R: 18; + OLD_D18: 48; + OLD_D1R: 60; without OLD_D3: 61; + OLD_D2W: 62, the
  differing lines 1624 > 1542; + OLD_D10 and OLD_D2R narrowed to loads through a pointer: 64, 1466 lines; OLD_D1R
  rr2 rr0 rr4 rr6 at the start (OLD_D2W): 74, 1405; OLD_D3P under casts: 74, 1395; + OLD_D2C and OLD_D2F: 75, 1325;
  + OLD_D2L: 84, 1080; + OLD_D2N: 86, 1064; + OLD_D6T: 86, 1004; + OLD_D6C: 86, 788; + OLD_D2T: 95, 557; + OLD_D6R: 98, 539; OLD_D4R on both sides: 99, 531; + OLD_D3K: 107, 495; + OLD_D2V: 108, 429; + OLD_D2M: 108, 418; + OLD_D2P: 109, 407; OLD_D2P with idle halves and idle pairs for words: 109, 400; + OLD_D2U: 109, 396; + OLD_D2G: 112, 368; + OLD_D5S: 114, 318; + OLD_D22: 115, 292; + OLD_D3J: 117, 290; + OLD_D2PO: 117, 282; + OLD_D5A: 118, 277; + OLD_D2W6: 118, 275).
- /lib/cparse as the second test bench (2026-10-31, ccv0, d12model, strictcmp): 44 strictly exact (shipped cc 29);
- /lib/cparse built with ccv0 before OLD_D10 (d12model, strictcmp): the code is as long as the original's (48896 bytes; shipped cc
- State: /lib/cparse 178 strictly exact, 2 differing lines, /lib/popt 72 of 73, 8 lines (strictcmp masks an immediate that is a function address of either binary since 2026-11-01), code and data as long as the original's (48896, 18584
  (2026-10-31: 48 with v0 + OLD_D3A; + OLD_D4R 71; + OLD_D4O 76; + the pair order of OLD_D2W 80; OLD_D3P under casts
  address of the model); nidcl source fix 2717; + OLD_D2N 93, 2602 lines; + OLD_D6T 2492; + OLD_D6C 2343; + OLD_D2T 98, 1732; OLD_D2Q also for a wanted pair 1707; + OLD_D2X 99, 1668; + OLD_D6R 107, 1584; OLD_D4R on both sides 108, 1438; source fixes hash, hide, lookup 111, 1400; + OLD_D3K 113, 1312; source fixes getpreg, fhashv 1282; + OLD_D2V 116, 1194; OLD_D3K at `?:` 117, 1188; source fixes cisreg, tyreduce 119, 1161; source fixes vfdzero, tcopy, ncopy, argregs, beginit, endinit, falloc 122, 1094; + OLD_D2A 125, 1087; source fixes tymatch, argdebug, buildtree, defid, efcode 126, 997; rmconv, ispow2 127, 987; + OLD_D2M 128, 961; + OLD_D2P 129, 906; getmem source fix 130; OLD_D2P with idle halves and idle pairs 132, 780; + OLD_D2U 133, 756; + OLD_D2G 135, 676; + OLD_D5S 136, 652; tymatch register order 137, 528; OLD_D3K: a label just in front of the function end keeps the parameters 139, 525; oconvert source fix 523; popt OLD_D20C 140, 464; popt OLD_D20X 140, 398; popt OLD_D20S 141, 390; popt OLD_D20L 383; OLD_D2A not for an argument pair, + OLD_D2O, + OLD_D21, code1 source fixes 336; + OLD_D16A 333; + OLD_D3N 142, 328; + OLD_D22 145, 302; OLD_D3N for the arguments of the next statement 296; source forms prtdcon, fltlab 147, 286; source forms yyparse, treeeq 148; OLD_D3N for register variables, psavel source form 151, 278; + OLD_D3J 153, 274; instk, talloc source forms, + OLD_D2QO 156, 260; + OLD_D2PO 234; + OLD_D2MO 157, 229; + OLD_D16B 158, 220; conval source order, + OLD_D2PT 159, 210; exname, yylex source forms 215 (yylex as long as the original, new mask artifacts); + OLD_D5A 157, 205; clocal, buildtree, putsym, newsect source forms 159; + OLD_D3V 158, 162; without the mask artifacts 162, 152; cinit case order, popt OLD_D20K 163, 144; clocal case order 164; + OLD_D6S 165, 134; + OLD_D2R0 166, 128; yylex 126; defalign source form 167, 117; + OLD_D2W6 169, 105; + OLD_D3AW 170, 103; OLD_D3AW for an address constant plus a parameter 171, 100; oconvert source form 172, 98; + OLD_D5G 173, 96).
  early (the start order: cparse 76 > 80, bench 69 > 74) (the originals of v0: `div rr0` 68 times, `rr2` 135, `rr4` once; v1 and shipped: never rr0); bench 61 > 62,

mv and rm byte-identical (2026-11-04). Own code exact with ccv1 and the stdio.h of the time (`-I../cparse`); mv.c declares cmd and errbuf in front of s1, s2 (the bss order), errbuf of rm has 50 bytes. libc part from known modules (`host/tools/fitlibc.py`); new versions: perror 0_2, errlst 0_2 (35 messages, without Deadlock and the two segment ones), strrchr 0_2, system 0_2, gets 0_1. The data-only modules (data, errlst) stand where codgen has them.
- OLD_D6Z of the entries above is now OLD_D23, the zero addend has its own class.
- dc connected (2026-11-04): src/cmd/dc, System III dc.c split in front of add0, ccv0 and casold. Libc part complete from known modules (fopen 0_1, printf 0_3, doprnt 0_3 ...), data and bss of equal size, code 6 bytes longer, 28 places in the own code, ratio 0.994; mostly the choice of registers of copies (`ld r2,r3; ld r13,2(r2)` against `ld r13,2(r3)`). ccv0p is not nearer.

- v0 against the libc of 07/82 and dc (2026-11-05). What had been called v0 had only v0p programs as witnesses. In libc-v1.a a parameter wanted in its own register has no copy; that one rule (under `OLD_D2C` and tshape) took isatty, stty, ftell to exact and dc from 71 to 23 blocks. Each switch taken out of v0 alone (dc, modules): only `OLD_D3J` (15) and `OLD_D2P` (24, fseek nearer) help, no other. `OLD_D3J` and `OLD_D2PO` went to v0p. `OLD_D2MO` went to v0 (dc `mult rr2,%5F24`). A name in a scratch register only when no lighter one is free (d2vfirst, not for wanted registers): dc 5 to 2. New `OLD_D3KC` keeps the v0p side of these rules.
- Tried and wrong (2026-11-05): taking the parameter as a scratch register in tshape and istnode altogether: gives `inc r6,#1` for `lda r2,1(r6)`. `rfreeall(1)` at the `?:` for v0: `ld r2,r7` of the arms is right in dc 1d1c, 26ca. Forgetting a parameter in oregfix or in regforsym: too late, the address goes through clocal (case 9).
- fseek 0_2 with v0 (2026-11-05): all but one statement: `offset -= c` in the else takes rr0, then rr6 (r3 and rr4 not free at that label), ours rr2, rr0. With `OLD_D3J` it fits, and dc then has the wrong `ld r2,r7` behind `if (sfeof(p)) {..call..; jr}`. What separates them is not known.
- Source forms found (2026-11-05): perror 0_2 is `if ((errno == 5 || errno == 6) && deverr) { .. } else if ..` (with this the `r6` of its first arm comes by itself, no compiler change). system 0_1 has no spare `int i`. readsym 0_1: `foo2 = ((foo1 = (*name & 127) - 8) + 13) / 14`, no `sym++`. longswap of libc swap: `long *l; p = (int *)l` gives the reload `ld r14,0(r15)`: another type, not a compiler difference. printf 0_3 is the source of printf 0_5.
- The ?: of size's libc (2026-11-05): first through freetemp (behind the autos): filbuf 5 instructions off. In the parameter area: offsets fit. In store only (`stotree`): the temporary is filled in front of the rest, the load stays in the argument order. Delay per arm and the offset not reset per statement: printf, rdwr exact. Not v1: v1 builds filbuf 0_2 without it. endopen 0_1 stays open: the value of an `&&` goes through the same temporary, `tcc`, `extsb` (`(char)`?), source form not found.
- libc/oldcc (2026-11-05): nlist with the shipped cc differs only in the frame, 2 bytes less: nlist has only `return` of constants, readby in the same file has its temporary (guess: no return temporary for a constant in that compiler). malloc 3_3: `mult rr2,r6`. swap: longswap solved, swap `ld 0(r15),r7` against `@r15`. All one witness each, no switch made.
- doprnt 0_5 with v0 (2026-11-05, not kept): a word parameter stored from its register through a pointer is forgotten there: `ld @r2,r5` of `_align` then gets its home, dc unchanged; one function, the module stays open, so no rule. Arguments stable in argsort: right for the double of `_fegcvt`, wrong for dc (4 places) and fltioc. `ap = argp` of `_doprnt` reloads; `int argp[]`, `&argp[0]`, `char *argp` with a cast do not change it. `(q+temp-1)` of malloc 0_3 needs `add r2,r2; dec r2,#2; add r2,r13`: `temp-1+q`, `q+(temp-1)`, `(q-1)+temp`, `&q[temp-1]` do not give it.
- dc byte-identical (2026-11-05): scalint: a parameter the statement assigns to is forgotten at its start. `OLD_ENTBLK` 176, not 178: counted with an instrumented casold on dc2, getwd has 176, putwd 178. The bss order: the declarations of dc.h in the order of the addresses of the original.
- write, rmdir (2026-11-05): `ld` with libc-v3 plus shipped gives the right sizes up to 6 bytes. `OLD_D1S` in v1q: fopen, isatty, stty, ttyname fit; filbuf, rdwr, malloc, printf 0_1 stay exact. The frame of nlist in libc.a may be the same rule (not tried). flsbuf: 140 of 162 instructions, frame 8 against 10.
- write, rmdir byte-identical (2026-11-05): first `OLD_D1S` (no return temporary for constant and call), then the delayed `x++` of filbuf showed there is none at all: `OLD_D1R`, so a new v1r; size keeps its temporaries (v1q). rmdir: `exit(Errors? 2: 0)`, no third parameter. fclose in flsbuf/0_3.c: `r = EOF` for the failed close. rdwr, printf, malloc, cnvt of libc-v10 differ from those of libc-v3 only by that temporary.
- ranlib byte-identical with v0 (2026-11-05), no new version. Source forms: `(x & m) != 0` (not `bit`), `trim(p1) register char *p1`, `(n = n - 8) + 11`, `int fd` and `long time()` in fixdate, order of the globals, no arsize. Compiler: the two forms of an element of an array, idle registers go back behind a conditional jump. Tried: no forgetting at labels no jump names (dc 2 to 28), OLD_D5G, OLD_D16A in v0, rr2 before rr0 by weight: no.
- doscopy 1.1 byte-identical with the shipped cc (2026-11-05, no compiler change, the rule: shipped programs). No register variables; dosout: `for (;;)` with the end test as first statement and a copy in each arm (comjump keeps the earlier copy), found by trying forms; dosin: the read loop written with gotos and a `bad` block in front of it (odd form, kept), `free` in the branch of a full directory is not set (a bug of the original, kept).
- ld (2026-11-05): chk_ranlib: `q` is `char *`, so `q = (char *)p` reloads p (no compiler change). place_sec and chk_ranlib `ld r2,r3` in front of an operation with r2 free: a general rule in tshape (scratch register only when none lighter is free, OLD_D2Y, not kept) stops codgen on ld.c:434 (reclaim dest reg not free). Forms of `(secorg[j] - q->tsize) & 0xff00` tried without effect: casts to int, unsigned, char *, short, long, `~0377`, `/256*256`.
- ld (2026-11-05): read_symbols exact from source forms (`if (a > b) x = y;`, `if (a) continue;` inside the test of the external data item). Tried without success: load2td: `continue`, `goto`, inverted `if (flag)`, unused labels (the block of `if (flag)` still goes in front of the switch); `ld r2,r3` of place_sec and chk_ranlib as a rule for r3 while r2 is free: 48 blocks, wrong.
- ld (2026-11-05): load2td exact: the switch is `case 10: w12 = swap(w12); case 9: case 11: flag++; break;`; the jump table of the original gave it, the three separate arms had made popt move the block. chk_ranlib `ld r2,r3`: the raw code would be a reload into r2 that popt's rmove turns into the copy, so r2 must be first free and the name of q2 not known; tried: `&&`, statements apart, q2 as `char *` and `int` with casts: the reload always goes to r3.
- ld chk_ranlib (2026-11-05): `q2 = r = *pslookup(..); if (r != 0) if (q2->stype != 0x20)` with `register struct symbol *r` gives the shape of the original (`ld @r2; ld q2,..; test; ld r2,..; cpb; ld r6,..`), but r14 where the original has r3; the original saves r14 and does not use it. Plexus ld.z8000.c has neither the function nor the expression of place_sec (`-t`, `-b` are WEGA's), so it does not help for these two.
- sld (2026-11-05, shipped scc, no compiler change): goto loops written as plain `for`/`while` fit (ranlib_scan, mlist_add, pack_name, str_compare): many of the older drafts with gotos are what kept functions apart. Parameter types as for libc swap (`long *a; p = (int *)a`).
- sld (2026-11-05): scan_options: the shape of the original is `goto test; do { ...; next: argc--; argv++; goto test; again: ...; test: ; } while (argc > 0); if (incmd--) goto again;` with the last inner switch `default:` first; open: the block at `next` is still moved behind the first `goto next`. parse_args: `a[j + 1]` and `a[j] = '-'` take r3, r5 where the original takes r2: r2 is free there in the original, unused labels in front of them change nothing.
- sld (2026-11-07, shipped scc, no compiler change): sld is of 10/27/83 and fits the shipped scc (15 functions differ, with sccv1 76). With it main, scan_options, reg_sect_addr, map_sym, dseek, symreloc, load2arg, read_symbols, place_sec, swab12 exact: the goto skeletons go back to `for (;;) { while (c) {...} if (!flag) break; restore }`; a goto label clears the register knowledge of codgen, a loop label does not (main, read_symbols, scan_options); dseek and symreloc in the form of src/cmd/ld. Open: parse_args, start_output, scan_m_opt, lay_out, ins_block (blocks of switch cases moved by codemove of spopt in the original, not here).
- Corpora (2026-11-07): made again from the git files of src (codgen, scodgen 160 + 122, popt 312). popt 2 and spopt 6 runs differ, all only in the line "NK core" of -d (the heap of the normal build); checkall expects these counts.
- host/recon/castrig removed (2026-11-08): the research scripts of the early jump optimization of the old cas (windows, offsets, fit, reset, memsum, dataset, boundary, feas, codgen_test, hostcas.patch, triggers.txt); cas and casold are exact. The entries above name them; they are in the git history before this commit.
- Switches without a witness removed (2026-11-08, TODO section 8): `OLD_D1F`, `OLD_D2D`, `OLD_D19A` and `OLD_D3A`. None was defined in any build of any version (no `-D` in a Makefile, `cc.c` or `oldcc.h`), so their code was never compiled; removing the `#ifdef` blocks changes no byte (proof: `checkall -b`, every installed file `cmp`-identical). What they were, from the entries above (2026-10-31 .. 11-03):
  - `OLD_D2D` (codgen `order3.c` `down1`, `allo3.c`): int DIV/MOD takes the dividend into r1 or into the odd half of the wanted even register, and the pair of a dividend that sits in an odd register. Tried in several forms: losses in crypt, col, paste, sdiff against the 62/1542 without it. Not accepted.
  - `OLD_D1F` (popt `z23.c`, `stripframe`, `fpfirst`): remove the frame (`dec/inc fp`) and the stores into it when nothing reads it after the renaming. Alone 61/1621, with `OLD_D2D` 61/1594. Parked together with `OLD_D2D`.
  - `OLD_D19A` (codgen `order3.c`, `argsort`): a constant argument counts as needing less than any other one, so it is loaded last (`ld r6,r13; ld r7,#%006E` of oconvert). cparse 113 > 77 (yylex, yyparse, genswitch much worse). The argument order of oconvert stays open.
  - `OLD_D3A` (codgen `gencall`): before the arguments of a call forget every register content. cparse 46 (putrefsyms, putnames lost); replaced by `OLD_D3AW` (forget only a parameter that is a whole argument or an address in it), which is in `oldcc.h`. The switch itself was gone from the sources earlier; a comment in `oldcc.h` still named it.
- OLD_D23 without the switch, source forms (2026-11-08; `usable` in allo3.c, the middle term `(rv >> 8) - 1 + 1 == r`; the compiler built without the switch, `oldvar.sh`, the model of scodgen): `(rv >> 8) + 0`, `1 + (rv >> 8) - 1`, `(rv >> 8) + 1 - 1`, `(rv >> 8) - (1 - 1)`, `(rv >> 8) + (1 - 1)`, `(char)(rv >> 8) + 0`, `((rv >> 8) - 1) + 1`, `(rv >> 8) - 2 + 2`, `(rv >> 8) - 1 - -1`, `(rv >> 8) + 0L`, `(rv >> 8) - 0`: `usable` differs in all (the optim takes the zero addend away, `add r2,#0` of the original is missing). The switch stays. oldvar.sh works again (build/nonsegv0p layout, models of the segmented programs).
- OLD_D3KC without the switch, source forms (2026-11-08; `regforsym` in regs3.c, the long parameter `off` behind `s == fpsym1 || s == fpsym2`; compiler without the switch, `oldvar.sh`): `symreg(s, (long)off, type)`, `off + 0L`, `s == fpsym1 ? 1 : s == fpsym2`, `!(s != fpsym1 && s != fpsym2)`, a `goto` into the second `if`, braces, `|` instead of `||` (8 lines differ): `ldl rr4,rr2` instead of the reload `ldl rr4,0(r15)` in all. The switch stays.
- OLD_D16A without the switch, source forms (2026-11-08; `genswitch` in cparse code1.c `range = p[++n].sval-p[2].sval;`; the switch is in codgen `match3.c`): `p[n = n+1]`, `p[n += 1]`, `n++` before the line, `p[n+1]` and `n++` after, `(p+(++n))->sval`, `(p[++n]).sval`, `p[(n++, n)]`, `p[n=1+n]`, the terms swapped: all leave `inc n,#1` (the original `ld r2,n; inc r2,#1; ld n,r2`) or differ more. The switch stays.
- OLD_D6S without the switch, source forms (2026-11-08; `falloc` in cparse sym1.c, `switch( new<0?pty->in.type:p->stype )`; the compiler without the switch): `new>=0?p->stype:pty->in.type`, `!(new<0)?...`, `switch( type )`, brackets round either side, casts to int, `+0`, spaces: the else block (`ld r2,4(r14); jr CODE`) stays in front of the dispatch (4 lines; 6 for the turned and `type` forms). The switch stays. Section 5 of OLDCC.md lists the state of every one-witness switch.
- Version vk (2026-11-08; the kernel objects of /wega, tmp/wega31_kernel lib1 and lib2): the objects ureg, tt0, sig, sys1, sys2, sys3, debug, tty differ from the shipped scc by 4 to 118 lines of code each; 20 source forms of the statements did not change them, sccv1 is far off (thousands of lines). Shipped scparse and spopt with the scodgen of v1 (`OLD_D3` alone) make ureg identical (0 lines), tt0 1, sys2 7, sys1 98, tty 16 (14 with the shipped one). All other OLD_Dn alone and on top of D3 (swept, `kt.sh` in the scratchpad): no object gets better, D2F/D2L/D2P/D2R/D3K/D2W/D2W6/D5G/D5S/D1R make them worse. Open differences: the copy before `& 0x7F00FFFF` of a loaded pointer (SCONV template 040a), the switch layout (compares behind the bodies), `andb` for `& 0xf` of a char, scratch register choices. Driver `sccvk`, passes in /lib/vk (scodgen), build/segvk.
- vk: D24 and D25 (2026-11-08): the pointer mask `(caddr_t)(((long)x) & 0x7F00FFFF)` of sig, sys2 (2), sys3, debug (3): the original has the SCONV long to pointer (template 040a: a copy `ldl rr6,rr4` and `and r6,#%7F00`) and no AND of its own; without the switches the shipped passes drop the conversion. Both switches together make the five objects identical (cmp); OLD_D24 alone or OLD_D25 alone change nothing, OLD_D25 without the AND drop gives two ANDs. debug.c: `zpr` as `phzif[(unsigned char)((int)h >> N) % 16]` (the last digit `(unsigned char)((int)h & 0xffff) % 16`) gives the original `andb`/`subb`.
- vk: D27 (2026-11-08): `exece` (sys1): `if (... && !c.l && uap->envp)`: the original takes rr6 for `uap->envp` after `ldl rr4,c; testl rr4`, the shipped rr4 again. Found by printing the register lists in getreg and `nxwork`: the table entry 57b (`ldl VA1,AR; cpl A1,#0`, the compare of a long with 0) has the register effect 19 (name forgotten); with 14 (the register holds the name of AR) the register stays taken. sys1 98 to 82 lines, the other seven objects unchanged.
- vk: D28 attempt, not adopted (2026-11-08, `getxfile` in sys1; counts are differing lines of the disassembly, one side, labels and frame offsets masked). The value of a switch goes to a TEMP at the head (`cgram1.y`, `lval` -1), its register has kind 6 until the dispatch, which cparse writes behind the bodies; popt moves the dispatch up. The load is the same in both (`ld r2,r9`), the earlier note of a load into r3 first does not hold. Register lists at `gswitch` (shipped rules): rr2 is free, so the `cpir` count takes r6; the hold is lost inside the body (`regjam`, regallo3.c, gives back idle words inside a wanted pair without looking at kind 6). Measured per switch with a mask over the TEMPs (no hold when the bit is set): sys2, sys3, tt0 need the hold, sig, debug do not care, tty no gain; sys1 is best with no hold in its first switch only (82 to 20). First switch: r2 free in the bodies, r2 taken at the dispatch (count r3). Second switch: body takes rr4 (r2 held), count r3. Prototype (cparse: `lval` -2 for a `register` variable; fixtemp: no kind 6; gswitch: takes r2 again): sys1 82 to 15, the 7 other objects unchanged; left: frame 0x1C2, `cpl` block in the second switch (rr2 against rr4). Not adopted: the same `register magic` is treated differently by the two switches of the original, so the condition is not the rule. Refuted as the condition: all labels release the hold (= no hold anywhere: sys2, sys3, tt0 worse), first TEMP of a function (key 0: same), value loaded at the dispatch (sys1 15 but breaks the layout of the jump table in sig), `regjam` skipping kind 6 (no gain).
- vk: sys1 identical by its source, no new switch (2026-11-08; supersedes the D28 attempt above). `getxfile`: (1) the loops behind `case S_MAGIC1:` stand behind the first `switch`, which only selects (`case S_MAGIC3: sepid++; case S_MAGIC1: break; case S_MAGIC4: default: error`): the TEMP of the switch is gone before them, r2 is free there and the only body leaves r2 and r3 as the dispatch needs them. (2) One `return ovly;` at the end, the others `goto out;`, and `stext, sdata, sbss` in the block of the `if`: the `[` record of a block sets `baseoff`, a `return` takes a frame word at `baseoff` (`freetemp`, reader3.c); behind the end of the inner block the base is lower and the word lies inside the autos, so the frame is 0x1C0 and not 0x1C2. (3) `curdsiz = data_size + code_size; data_size = curdsiz;` instead of the double assignment. Still needs OLD_D27 (sys1 16 lines without it). All 27 objects of LIB1 `cmp` identical through `uts/sys/Makefile`. tty: `if (0 == ((long)arg & 0xffffL))` gives the `ldk r2,#0` of `TCSBRK` (16 to 13 lines). Other compilers: the passes of ZEUS 3.21 (`tmp/zeus321_root`, run under the emulator through `/lib/vk`) give r6 and 0x1C2 as well and more differences elsewhere (sys1 118, ureg 7): no use.
- vk, popt and sioioctl (2026-11-08): `sioioctl` (dev/wega/siok.c) has the text of `wega` with `if (copyout(...) == 0) return(0); u.u_error = EFAULT; return;` (scc): the place of the blocks follows from the order of the tails in the source (comjump, then codemove), no switch. tty `TCXONC` is still open: spopt with `OLD_D20` is worse (sig 17, tty 54 lines, two-sided), `while`/`for` instead of `if` give the same code as `if` for sioioctl.
- vk: OLD_D27 removed (2026-11-08): the form of the test decides: `!x` (and a plain `x` as a condition) of a long takes entry 057b (name of the operand forgotten: nx 19), `x == 0` takes 054a and 043 (name kept). `exece` has `c.l == 0`, `getxfile` `code_size != 0`, `ttopen` `!u.u_ttyp`; sys1 stays identical with the shipped table. tt0: `chg = (tp->t_lflag & 0xffffL)^arg` (the 037 template, then the mask) gives `ld r3,r5` before `ldk r2,#0`; tt0.o identical. `dev/Makefile`: tty.o and tt0.o with sccvk (they were built by scc). tty `TCXONC`: the `jr ule,X; jp default` can only come from the loop inversion or the reversal in popt; neither the place of `default` nor the order of the cases gives it (11 lines left).
- vk: `sys/lock.c` written from the disassembly, `lock.o` identical with sccvk (2026-11-09), needs `OLD_D3` (without it 19 lines; corrected 2026-11-09: the first measurement was void, `//` in `oldcc.h` broke the build and the old scodgen ran on). Plain scc differs in `unlk` only: the register of the value of `lp->ll_start = ub` (scc rr4, the original rr0), so popt merges a tail there. `lock.s` removed. Source forms that decided: `lkfront` declared as a pointer function (a cast of a long adds the mask in vk), `case 31` with `goto` and fall-through into `case 15`, unused locals in `deadlock` and `unlock` for the frame.
- vk and `TCXONC` in tty (2026-11-09): each `OLD_D` switch of codgen and cparse added singly to vk, only tty compared. None gives the dispatch of the original; most leave the disassembly unchanged, `OLD_D2P`, `OLD_D5S`, `OLD_D1R`, `OLD_D2L`, `OLD_D2W`, `OLD_D2W6`, `OLD_D14Q` and in cparse `OLD_D6C`, `OLD_D1`, `OLD_D4R` make it worse. Not measured: `OLD_D2C`, `OLD_D3J` (do not build alone), `OLD_D5G` (register jam on tty.c), cparse `OLD_D6T` (format error against the vk scodgen). `sccv1` and the shipped `scc` do not split the dispatch either.
- vk: `OLD_D24` and `OLD_D25` against the source (2026-11-09): vk without both (build checked) differs in sig 7, sys2 12, sys3 7, debug 23 lines, always `and r2,#%7F00; ldl x,rr2` for `ldl rr4,rr2; and r4,#%7F00; ldl x,rr4`. 25 forms of the statement in sig (`ipc.ip_addr.l`: casts, L constants, operand order, `~`, `+ 0`, `| 0`, a long lvalue, parentheses) give the same code; temporaries are worse. No source form.
- vk: `TCXONC` in tty (2026-11-09), `OLD_D29`. Shipped spopt on hand-edited `sccvk -S` output is byte-identical for: `Lt: rl r2,#2; ldl rr4,table(r2); jp @rr4`, bodies 0 to 2 with `jpr L131`, body 3 without a jump, `L131: sub r2,r2; jpr L105` (the code of the label behind the outer switch), `TCSBRK`, `L130: ld r2,arg; cp r2,#3; jpr ule,Lt`, `L137: ldb u_error,#22; jpr L131`. The common tail behind the last body comes from `xjump`: the label the bodies jump to right behind the last body, and no `jp` right behind that label (else the chain is resolved first). `popt` gets this layout through `d29switch`; sccvk now runs `/lib/vk/spopt`. Without effect before: order and place of `TCXONC`, `TCSBRK`, `default`, cases; casts of the value; `return`, `break`, `goto` as case ends; `return(0)` behind the switch; a range check in front of it; statements around it; `TCSBRK` inside the switch by `goto`; all 720 orders of the raw blocks; the popts of v0, v0p, v1, v1 seg, ZEUS 3.21; each `OLD_D` switch alone.
- cas: a model (`MODULES`, `NOOPT`, `model_prepare`, `model_link` in `compare/cas/progcfg.py`; 2026-11-09). 155 functions against `/bin/cas` by `strictcmp.py` (strict / masked, differing lines of the strict form): shipped scc 2 / 16, 9228; sccvk 2 / 16, 9371; sccv1 1 / 13, 10653. None is near, the compiler is still open; the data of the model differs too (38794 against 38360 bytes), so the strict form is pessimistic. Text size: scc and sccv1 46848 as the original, sccvk 47104. `iputw` differs in the statements, not only in registers, so part of the differences may come from the sources.
- cas: compilers and switches (2026-11-09). Segmented v0, v0p, v1q, v1r built from the switch sets of the Makefiles: v0 and v0p, v1q, v1r do not give valid code for segmented sources (`reclaim dest reg not free`, operands the assembler rejects); v1 10653 (10477 with casold). Single switches on the shipped scc: better `OLD_D6C` 8366, `OLD_D2W` 8607, `OLD_D1R` 8862 (equal functions 2 to 6), `OLD_D2R`, `OLD_D2P`, `OLD_D3K`; `OLD_D10` alone is worse and helps in a combination. Greedy rounds: 8366, 7728, 7362, 7154, 6947, 6808, 6727, 6666, 6089 (`OLD_D2T`), 6033, 5987, 5943 (stopped by the limit of 12 rounds; the last steps give about 50 lines). By kind with these switches: 13 equal, 44 only registers, offsets or constants, 85 the same calls and conditional jumps, 7 where these differ, 6 not measured.
- cas: more switches (2026-11-09). `OLD_D14Q` and `OLD_D5S` made valid for segmented code (frame temporary in the stack segment; of `OLD_D5S` only the rule for a local array, its other rules add a 16-bit index to a segmented pointer); `OLD_D5G` is not tried (an address is 32 bits). Further rounds from the best set: `OLD_D3AW` 5903, `OLD_D6S` 5866, `OLD_D3J` with `OLD_D3K` 5827 (16 equal, 47 masked), then no gain; `OLD_D14Q` and `OLD_D5S` build but are not among the gains. Of the 7 suspected sources `mknode2`, `xtol` were the end of the function running into a static function, now `STATICFUNCS` in the model; open with other calls or branches: `iputline`, `main`, `synrecover`, `seekout` (the range of findings.json is doubtful for the last two), `exprreloc`.
- cas: `sccvc` in the tree (2026-11-09): `OLD_VC` in codgen `oldcc.h`, `OLD_D6C` and `OLD_D6D` in scparse, the shipped spopt; `src/cmd/cas/Makefile` uses it for `build/cas`, `old/cas` by scc. `checkall`: ALL OK, corpus of cas 657 of 657, `bin/cas` still open. `compare/cas/check.sh` measures with `sccvc` from OLDROOT: 52 same, 13 close, 90 open (with scc 28, 7, 120).
- cas: `OLD_D13` (2026-11-09): the bit 31 is written by the assembler, not by the compiler: `assemble.c` calls `genlop` for a long immediate with flag 0, the old one (measured by the model) with 1. Model of cas 16 to 24 equal, 5808 to 5420 lines; masked unchanged 47. `/lib/vc/cas` in the tree, `sccvc` runs it.
- cas: source (2026-11-09): `done` and `sigdone` belong to `main.c` (their strings lie in the data of main, `done.c` had no SCCS id); the abort for syntax errors stands behind the rest of `main` (`if (synerr == 0 && semerr == 0) { ... } else ...`, by the order of the strings). 5420 to 5412 lines. `make shipped` builds cas with the system scc again (`SCC=scc` in `src/cmd/Makefile`): between the introduction of sccvc and this commit the corpus of `cas [wega]` tested the build of sccvc.
- cas: the 46 bytes between data and bss (2026-11-09): the model links the shipped slibc; own modules equal in data size. Library part of /bin/cas: of 57 functions of the model behind the own code 28 are found unchanged (instructions, addresses masked), 29 not, among them `printf`, `doprnt`, `malloc`, `filbuf`, `flsbuf`, `fread`, `perror`, `cerror`; the shipped `malloc` has an SCCS string that /bin/cas lacks. As for sld (`src/lib/oldslibc`, `host/tools/oldslibc_cut.py`) the members would be cut as assembler sources; the boundaries of the 29 are only approximate so far.
- cas: the library part against the shipped slibc, by member (2026-11-09; /bin/cas 0x9694..0xb624, words without relocation compared). Equal to the member, to be taken from /lib/slibc.a: freopen 9694, fopen 9938, ungetc a61a, __callex abc8, cuexit abd0, decimal adbe, isatty b276, stty b326, strcmp b356, access .. cerror b446..b624 (access tolower chmod close creat getpid ioctl lseek mkseg open read ssgbrk signal umask unlink write cerror). Not equal, to be cut: fseek 96ce-982e, rdwr 982e-9938, printf from 995c (`_fprintf` 99f0, `_sprintf` 9a74), doprnt up to a61a (their boundary open; 6 bytes more than the printf and doprnt of sld together), filbuf a68c-a7a0 (one word differs), flsbuf a7a0-a9dc, endopen a9dc-ab9a, findiop ab9a-abc8, perror abdc-aca6, cnvt aca6-adbe (frame size), length ae5c-ae8a, malloc ae8a-b276, mktemp b29c-b326, strcpy b39a-b3cc, strncpy b3cc-b422, strlen b422-b446 (these three: only the frame size). `stfl` and `fltioc` of the shipped library are not in /bin/cas. Names from the call sites of the own code: `_fseek` 96ce, `_fread` 982e, `_fwrite` 98b2, `_printf` 995c, `_fprintf` 99f0, `_sprintf` 9a74, `__filbuf` a68c, `__flsbuf` a7a0, `_perror` abdc, `_calloc` ae8a. Still to do for the cut: the other entry names, and the data the cut members use (shared data such as `__iob` must stay one object, so the method of `libc_data_seg.py`, private copies of the words, does not fit without a check).
- cas: `slibc-v11` (2026-11-09): the 16 members that differ from the shipped slibc are cut from /bin/cas as `src/lib/oldslibc/NAME/0_2.s` by `host/tools/oldslibc_cas.py` (text and data verified against /bin/cas, relocated words masked); `slibc-v11.a` takes the other members from /lib/slibc.a, the order is that of /bin/cas. Compared with libc-v1 to v10 by skeleton (calls, jumps, small constants) no version fits throughout, so a new number (segmented against nonsegmented code: a weak measure). Link: `build/cas` and the model use it, `old/cas`, `vc/cas` and `make shipped` the system slibc. Model: data 38360, bss 12328 as the original, 43 equal, 47 masked, 5012 lines.
- cas: `sccvc` runs `/lib/old/cas` (2026-11-09): the assembler of sccvc needs `OLD_CAS` too (text 46848 and the whole file 89048 bytes as the original, differing bytes 75330 to 47867), so only `OLD_D13` was left between it and old/cas. `OLD_D13` acts only with `segmode`, v0 and v0p are nonsegmented: one old/cas with both; `host/cas` (own port of `assemble.c`) got the switch for casold. Remaining switches added to `OLD_VC` one by one: `OLD_D2Q` -33 lines, `OLD_D3KC`, `OLD_D2U` less, the others none or worse. Each of the 14 removed makes it worse. Model: 43 equal, 47 masked, 4900 lines. One `make oldroot` stopped at the link of `build/nonsegv1q/codgen` (error 4); the next run with no change there passed: a failure of the parallel build, cause not found.
- cas: `OLD_D2Q` and `OLD_D2U` added to `OLD_VC` (2026-11-09): 45 equal, 49 masked, 4855 lines. Of the lines 1593 are an artefact (`yyparse` is missing in findings.json, `synrecover` runs into it). Open: in `exprreloc` and others sccvc takes rr0 for an address and computes it (`ldl rr0,rr6; inc r1,#6; ldl rr4,rr0; ldb rl2,@rr4`), the original `ldl rr2,rr6; ldb rl2,rr2(#6)`; rr0 is no base register. Moving rr0 back in the pair order is worse (5036, 5061 lines against 4900). `OLDCC_SWITCHES.md` regenerated by `oldcc.sh matrix`.
- `oldcc.sh matrix` (2026-11-09): `switchmatrix.py` removed a switch at its first `#define` in `oldcc.h`; since `OLD_VC` stands in front of the v0 blocks that was the wrong one, and the switches of `OLD_VC` showed no witnesses. It now removes the define only in the blocks of `OLD_V0P` and `OLD_V0`. After the correction the table is unchanged in all 48 rows but for the measuring commit (e5487b63). The model of cpp needs `cpy.c` and `crt0.o` of `src/cmd/cpp` (make them after `make -C host oldroot`, whose `clean` removes them).
- cas: source forms (2026-11-09): own `stdio.h` (the V7 getc), `iputl`/`sputl` (logical shift), `endpass1` (case order), `isdef` (a second register variable), `ssgbrk` undeclared (int), iputw/sputw. Model 49 equal, 3503 lines, `yyparse` now counted by itself (`EXTRA_FUNCS`). Compiler left: `OLD_D5G` for segmented code (address first, index behind it, 97 lines); the cast of a long constant to a pointer clears bit 31 in sccvc and `(char *)STAB + idx` stops with `register jam`. Most remaining small differences are the choice of the register (rr0 against rr2, the table register of a switch: rr2 in the original, rr6 here).
- cas: bss and bit 31 of `.addr` (2026-11-09). Bss addresses fixed by the sizes of `ops[5]` (assemble.c), `nl.name[136]` (ofile.c) and an `int lcpad` behind `synrecover` (parser.y): strict 53 to 59 equal. `d12model.py` reported the sizes of segment 0 only (data and bss 0 for segmented programs); now the first segment with data. Of 38360 data bytes 4045 differed, 2044 words of them by bit 31 of a segmented address (`%8400xxxx`, `%8500xxxx` in /bin/cas, `%0400`, `%0500` in the model): the jump tables of the switches and the pointer tables. Neither /bin/cas nor old/cas sets the bit in `.addr` or `.long` (a test file: `.addr _x` gives `0100 0000`); old/cas sets it in `ldl #`. First try: `genlop(np, 1)` for the records 7 and 25 of `p2loop` (`.addr` and `.long` alike): 36317 bytes equal, but the cut members doprnt, cnvt, endopen of `slibc-v11` (assembled by `sccvc -c`, so by old/cas) got the bit in their `.long` jump tables, where /bin/cas has none, and their code needs old/cas (D13 immediates). Second: only `.addr` (tags 0xE0/0xE1 around the list in pass 1): the library right, but the jump tables of the -O modules lost the bit, because the shipped spopt prints them as `.long` (z21.c JSW). With `.long L` turned into `.addr L` behind sccvc (a test wrapper): 36359 equal, no bit 31 difference left. So `OLD_D13` in spopt too: `/lib/vc/spopt`, used by sccvc. Model: 59 strict, 59 masked, 3067 lines; findings.py (check.sh) 70 same, 15 close, 70 open. checkall: ALL OK, with the cas part alone and again with spopt (bin/cas open, not counted). Rest of the data: code addresses (follow the code) and 34 words of `yyparse`: the value lists of the cpir switches stand right in front of their target lists in the original (as OLD_D6T), in sccvc all four first; not taken. `oldcc.sh matrix check`: as in the table.
- cas: `OLD_D6T` for segmented code (2026-11-09). The cpir switches of cas (escape, osymout, genwop, four in yyparse) have in the original the value list right in front of the target list and `ld r2,#Lv+2; sub r7,r2; rl r7,#1; ldl rr2,Lt(r7); jp @rr2` with the count in r4; sccvc wrote the value lists with the strings and `sll r3,#2; neg r3; ldl rr6,Lt+n(r3); jp @rr6`. `OLD_D6T` was a switch of v0 only (nonsegmented cparse and the nonsegmented gswitch); now in the segmented gswitch (lcl3.c, inside `#ifdef SEG`, so v0 and v0p unchanged) and for vc in scparse and `OLD_VC`. First the count went to r0, then r1 (two retries of getreg); a loop that rejects the pair of the value and rr0 gives r4. Model: 3067 to 2917 lines, 59 strict; data 36359 to 36374, the 34 words of yyparse gone; the cpir sequences now equal. yyparse is 8 bytes longer than the original (2 per switch; before its size was equal by chance). checkall: one run FAILED in `make shipped` (cparse nonseg: `ld: pftn1.o: cannot open`, the compile of pftn1.c printed and no object, in parallel with the seg build; not touched by this change), the next run with the same tree: ALL OK (bin/cas open, not counted). The cause of the failed run is open: a guess is a clash of the guest temporary files of the parallel builds (the guest pid is the host pid cut to 16 bits).
- cas: `OLD_D6J` (2026-11-09): the dense jump tables of the original load the target into the pair of the value (`rl r2,#2; ldl rr2,tab(r2); jp @rr2`), sccvc into a free pair (rr6); new switch in the segmented gswitch, set in `OLD_VC` only. 23 functions better, none worse, no differing table jump left. Model: 59 to 62 strict and masked, 2917 to 2807 lines. Not yet checked by checkall (batched; changes since the last run: OLD_D6J).
- cas: registers (2026-11-09). `OLD_D2O` of v0p added to `OLD_VC` (a long through an odd index register into the pair below it: `ldl rr2,tab(r3)`): 62 to 67 strict, no function worse. `OLD_D2P` reaches too far in cas: in the op* functions (opxor, opor, opand, opshl, opshr, opadd, opsub) it reuses the idle rr4 (it holds the parameter `r`) where the original takes the free rr6, in evalint, jsum and others it rightly reuses an idle rr2; without OLD_D2P 70 strict but 3038 lines. Sharpened for vc only: `OLD_D2PL`, an idle pair only when it is rr0 or rr2 (67 to 72, no function worse). `OLD_D2M` of v0 added (newrec, asmword, asmins better, putrow worse by its register variable, see below). Source: `register int len` in putrow (39 to 29 lines). Model 73 strict and masked, 2629 lines. Tried and left for the end (no source form found): iputloc (an odd word register wanted, OLD_D2O does not take it; tried 7 casts and temporaries), igetexpr (`return (opseg(igetexpr()))` gets a frame temporary), the op* functions (operand order, temporaries, `^=`), the loop of putrow keeps its head on top in the original (spopt rotates every form tried, D7). Next: OLD_D2R for segmented code (a byte or word through the pair rr2 into r2: freereg frees only the word, the pair stays busy). checkall not run since OLD_D6T (changes since: OLD_D6J, OLD_D2O, OLD_D2PL, OLD_D2M, all only in OLD_VC).
- `OLD_D2PL` folded into `OLD_D2P` (2026-11-09): the restriction to an idle rr0 or rr2 for all users. Both conditions of the loop changed (the one with `OLD_D3KC` is the one of v0p). Measured before and after: the v0p models (oldcc.sh model: cparse 179, popt 73, codgen 209, spopt 73, scparse 179, scodgen 210, cpp 26) all strictly equal both times; the v0 bench 153 functions, 131 same, 221 lines both times; cas 73 strict, 2629 lines unchanged. checkall pending (batched).
- cas: `OLD_D2R` for segmented code (2026-11-09). The original loads a byte or word through a pair into its even half (`ldb rl2,rr2(#4)`, `ld r2,rr2(#2)`) and reloads the pointer afterwards; sccvc took r0. Cause: the base of `rr2(#4)` is coded as `rval = (2+1)<<8`; freereg finds r2 by usable, but `rfree(2, char)` frees no word (the pair is a node of class 3), so getreg falls back to r0. New in allocreg under `SEG`: before freereg, a pair base without index (or a REG pair under STAR) busy once is freed as LONG, given back (`rsreturn(3, pair)`) and its even half taken. Only vc has OLD_D2R among the segmented compilers. Model: 73 to 74 strict, 2629 to 2314 lines; 17 functions better (jdeps 116 to 44, putrow 29 to 6, yyparse 339 to 277, asmins 28 to 16, jdist 35 to 6), asmword 20 to 22. Left: an indexed byte load `tab(r3)` goes to r2 in the original (r3 here), and a copy of the parameter to rr6 in asmword.
- checkall (2026-11-09) after OLD_D6J, OLD_D2O, OLD_D2M, OLD_D2P (rr0, rr2 only), OLD_D2R segmented: the first run FAILED in stage1 (`ld: cgram1.o: cannot open` in cparse build/nonsegv0, the compile of cgram1.c printed and no object, six cparse builds in parallel; cparse is built by the shipped cc there, untouched by these changes), the second run ALL OK. Second such failure (see OLD_D6T). The guest cc names its temporary files `/tmp/cc.<pid>.<x>` (src/cmd/cc/cc.c) and the guest /tmp is one directory under ROOT for all parallel builds; the guest pid is the host pid cut to 16 bits. A clash of two parallel compiles is the guess, not shown.
- cas: OLD_D2R segmented sharpened, source forms (2026-11-09). The pair base of `@rr2` and `rr2(#n)` is also coded as OREG with rval 2 (below 0x100), the first form only knew (pair+1)<<8: now both; the even half also when it is wanted (a return value: exprtype); only rr2 and rr4 (rr6 holds a parameter in evalint, iputnode, where the original loads into rl2). A mark of the copy of an rr0 base (no load into its half) was tried and removed: in optim the original too copies rr0 to rr2 and loads into r2; where it hurts (escape, exprreloc, exprval) the original has no rr0 address at all, the cause is upstream. Source: setsym and jprnew compute `(char *)p - (char *)ITAB` (the original subtracts the offset 0x30 through a register). Model 85 strict, 86 masked, 2113 lines. `oldroot` failed once more with `local1.o: cannot open` (cparse nonsegv0p, without logging); the runs with Z8K_TMPLOG since show no clash (tmpclash.py had to take the forks first: a child may log before its parent writes the fork line).
- cas: `OLD_D2PT` of v0p added to `OLD_VC` (2026-11-09). The rr0 addresses of escape, exprreloc, exprval and others: in `switch (np->n_u.cc.c1)` the TEMP of the switch holds r2, so the copy of the parameter pair went to rr0, which is no base, and the offset was added and copied again (`ldl rr0,rr6; inc r1,#6; ldl rr4,rr0; ldb rl4,@rr4`); the original takes rr2 (`ldl rr2,rr6; ldb rl2,rr2(#6)`). With OLD_D2PT the registers of the TEMP count for OLD_D2P: 85 to 86 strict, 2113 to 1934 lines, 11 functions better, none worse. checkall pending (batched; since b938bd0f: OLD_D2R segmented sharpened, OLD_D2PT, source forms).
- cas: source of iputexpr, sputexpr (2026-11-09): the tag is written by a switch with three cases (compared one after the other, OLD_D6C) and two calls whose tails popt merges, not by `?:`; the second switch has its cases in the order 6, 7-9, 1, 2, 3, 23-25. Both exact; model 88 strict, 1828 lines. OLD_D2X tried for symtype (an int widened to long in the pair of its register): no effect (a conversion, not a binary operator).
- cas: OLD_D2R segmented, indexed loads (2026-11-09): `tab(r3)` loads into r2 in the original (the even half of the pair of the index), sccvc into r3; now the even half when free. 88 to 89 strict, 1828 to 1759 lines, 6 functions better (asmins 16 to 4, jprnew 28 to 7, optim 14 to 2, defsym exact), none worse.
- checkall (2026-11-09) after OLD_D2R segmented (sharpened twice), OLD_D2PT in vc and the source forms of setsym, jprnew, iputexpr, sputexpr, p2tag: ALL OK in the first run; tmpclash: 223467 lines, 15892 trees, no clash. Left for the end besides the list above: the cast of a constant to a pointer clears bit 31 in scparse (`local1.c`, `&= 0x7f00ffffL`; stabp, itabp, stalloc, italloc, namalloc), `ldk r2,#4` in front of the calls of igetl in igetaddr and of iputl in putrow (the only two in /bin/cas; casts tried in vc and vk without it), the long mask of igetaddr through rr0.
- cas: source forms asmimm (`unsigned c0`), putbits (`unsigned n`), rangechk (one `if` with `||`) (2026-11-09): all three exact, 100 strict, 1662 lines. host/cas unchanged (its c0 is only used masked, putbits gets small constants).
- cas (2026-11-09): `OLD_D3P` sharpened for segmented code: the cast case only between pointers (D3PWORD); iputw and sputw `(unsigned)w >> 8` reload w from the frame into r7 in the original. vc without OLD_D3P: iputw, sputw exact but 19 functions worse (pointer casts); the sharpened form: 102 to 104 exact, none worse. A restriction of the register case to pairs (words loaded again when another register is wanted) had no effect. genwop: `v |= 0x80000000L` (dead branch, quirk). error and optim still load a word parameter from the frame where sccvc copies r7 (`switch (n)`, an index): open.
- cas: source forms done for every open function (2026-11-09/10, night). 113 of 151 exact, 1121 differing lines in the model. What no source form removed, as clusters (the note of each function in findings.json names its cluster); switches come next, cluster by cluster:
  - K1 the cast of a long constant to a pointer clears bit 31 (scparse `local1.c`, `&= 0x7f00ffffL`): stabp, itabp, stalloc, italloc, namalloc, jsymadj, symadjust.
  - K2 `ldk r2,#4` in front of the call of igetl (igetaddr) and of iputl (putrow), the only two in /bin/cas; igetaddr masks through rr0.
  - K3 the value of a nested call in `return (f(g()))` goes through a frame temporary in sccvc, not in the original: igetexpr, sgetexpr.
  - K4 OLD_D2R segmented, more forms: a load through base and index (`rr2(r1)`, `rr4(r2)`) into the pair of the base or the index, a byte into the odd half when that is wanted (`ldb rl3,@rr2`): casechk, seekout, pstcmp, main, asmword, jiter, jprnew.
  - K5 an int widened to long stays in the pair of its register: symtype.
  - K6 OLD_D2O when an odd word register is wanted: iputloc.
  - K7 a word parameter wanted in another register (the value of a switch, an index) is loaded from its frame home again: error, optim.
  - K8 a switch with two cases: the compares stand behind the bodies (OLD_D6C compares in front): jpput, jrput, jprange, jpr, yylex.
  - K9 a constant index folded into the address: `lctab[i][1]` as `tab+4(r3)`, `buf[1]` of a local array as a frame operand: ohdrinit, symadjust, escape.
  - K10 OLD_D10 too broad: `>> 1` of a long as `sral rr4,#1` in the original (but `<< 16` through a register): asmrel.
  - K11 the register parameter of a leaf function renamed to a scratch pair (procopt of popt): opmatch.
  - K12 block layout of popt: loop rotation (putrow, jdeps), block placement (lstsync, stabout, stenter, exprreloc, jpr, yylex), the copy kept by tail merging (symreloc, sgetnode).
  - K13 `call` with a short or long segmented address (the assembler, D17): yylex.
  - K14 a value known in its register after the store (yylex `idx`).
  Source forms found in this round: opsub, opadd (`||` of two conditions, `(unsigned char)5`), exprval (`-x`), opoff (a byte argument, quirk), asmfield (`code = code - n`), pstlook, jsymadj, asmrel (register declarations), putins, escape, exprreloc (case order), stlook (`for (;;)`), getop (`ops[n] =`), jpr, stabout (switches), main (D15: `if (!flag || !nfiles)`), jprange (`if (segmode)`), stenter (the case without a name first), yylex (case order, `(unsigned char)0`, switch at done), symadjust, ohdrinit, genwop.
- cas, switches, prio 1 (2026-11-10, night): K4, OLD_D2R segmented also with an index (`rr2(r1)`: the base pair) and into the odd half when that is wanted. 113 to 114 exact, 1121 to 1059 lines (casechk 66 to 24, pstcmp 28 to 15, main 9 to 6, jiter exact), none worse. On the way: the quirk of opoff (a byte for mknode1) had come into /lib/old/cas, which then assembled `#^o 48` of sccvc as 0x4F3C; the quirk is now outside OLD_CAS only (the old cas gives the offset, as the bytes of /bin/cas show).
- cas K4 (2026-11-10): OLD_D2Q segmented: a long through base and index goes into the pair of an even index register, whatever pair is wanted (seekout: the value goes on to rr4 as an argument, the original loads rr2 and copies). seekout 88 to 6, 1059 to 977 lines, none worse. checkall of 75d1718a (the source forms, OLD_D3P segmented): ALL OK, no clash.
- cas K10 (2026-11-10): OLD_D10 segmented: a shift of a long by one stays immediate (asmrel exact; 977 to 930 lines with casechk and pstcmp, whose source forms showed after K4: `upper ? islower : isupper`, `(c = isupper(s[i]) ? tolower(s[i]) : s[i]) != t[i]` with `goto bad`).
- Build failure 2026-11-10 (make oldroot, codgen build/nonsegv1r: ld: match3.o: cannot open, the compile line of stage2 is there, no message): Z8K_TMPLOG showed no clash in /tmp. z8k-emu now logs every creat and unlink and the exit status too.
- cas K9 (2026-11-10): OLD_D5G segmented: a constant second index folded into the address constant. ohdrinit 104 to 0, escape 19 to 0, symadjust 16 to 3; 117 to 119 exact, 930 to 788 lines, none worse.
- cas (2026-11-10): listp (and the unused symmode) defined in pst.c, extern in parser.y: in the original listp lies behind yyv (AC20), so it is no variable of parser.y (section 3 of the yacc file comes in front of yyv). yyparse 166 to 2 (left: yyv 2 bytes later than in the original: the 2 bytes of lcpad lie between yyval and yyv here; where the original has them was not found), 788 to 624 lines.
- cas K1 (2026-11-10): OLD_D13 in scparse of vc: a constant cast to a pointer keeps bit 31 (mask 0xff00ffff). stalloc, italloc, namalloc 2 lines better each; 624 to 618. The original form `(char *)STAB + idx` (stabp, itabp) stops sccvc, sccvk and the shipped scc alike with 'register jam' (all eight word registers taken one by one, op 96, want r2): a bug of the shipped codgen that the old one had not; not fixed.
- cas K6 (2026-11-10): OLD_D2O segmented also when an odd word register is wanted (the even form is OLD_D2QO): iputloc exact; 121 exact, 605 lines. Source: seekout (no even mask for the relocation offset of subsection 0; host/cas too), symtype returns unsigned char.
- cas K1 rest (2026-11-10): the form of the original `(struct sym *)((char *)STAB + i)` (stabp, itabp, stalloc, italloc, namalloc, jsymadj, symadjust: `ldl rr2,#%87000030; add r3,i`) stops sccvc with 'register jam': allocreg is called again and again for a REG node (int, wanted r2) and gets r2, r3, r0, r1, r4 .. r7 until none is left. Not the swap of a constant left operand in optim3.c (tried: no effect). Open.
- cas K1 done (2026-11-10): the jam: cparse gives PLUS(REG int, ICON char *), codgen makes ASG PLUS with the int register as target of a pointer, no template fits, the rewrite takes a new word register again and again. OLD_D13 in scodgen (vc): such an address goes to the left. With the source form of the original (one expression `(char *)STAB + i`): stabp, itabp, stalloc, italloc, namalloc, jsymadj, symadjust exact; 121 to 128 exact, 605 to 572 lines, none worse.
- checkall of 391cb149 FAILED for a real reason: old/cas (and make shipped) are built by the shipped scc, which stops with 'register jam' on `(char *)STAB + i` too. Now sccvc passes -DSCCVC to cpp (cc.c, OLD_VC), and cas.h has TABADDR(t, off): the form of /bin/cas under SCCVC, else the same sum as a long (the shipped scc compiles it, same behaviour). Model unchanged at 128 exact.
- cas K12 (2026-11-10): OLD_D20 in the spopt of vc, again on the state of 128 exact: 121 exact, 572 to 702 lines; symreloc exact, but exprtype, evalint, exprval, opshort, opseg, opoff, genlop, jdist lose their exactness and jpput, jrput get worse (13): the dispatch of a two-case switch is not explained by D20. Removed again.
- checkall (2026-11-10) with TABADDR/SCCVC and the switch work K1, K4, K6, K9, K10: ALL OK, no clash.
- cas, state at the end of the night 2026-11-10: 129 of 151 functions exact in the model, 0 masked, 22 open; text, data and bss of the original size; 1121 lines at the start of the switch phase, now about 540. /bin/cas is not byte-identical: the open functions differ in length (yylex -18, jdeps +20, igetaddr -6, ...), so every address behind them moves. Left, by cluster: K2 (igetaddr, putrow: `ldk r2,#4`), K3 (igetexpr, sgetexpr), K5 (symtype), K7 (error, optim), K8 (two-case switches: jpput, jrput, jprange, jpr, yylex), K11 (opmatch), K12 (popt block layout: yylex 306, stenter 51, jdeps 44, jpr, stabout, exprreloc, lstsync, sgetnode, symreloc, putrow), K13 and K14 (yylex, asmword). K1, K4, K6, K9, K10 are done; OLD_D20 for K12 is refuted. Next: K12 needs a model of the old popt (why it leaves the dispatch of a two-case switch behind the bodies, does not rotate some loops and keeps other copies of merged tails).
- cas K12, model of the old spopt (2026-11-10). Tools in the scratch: the raw code of one function from `sccvc -S` without -O, a TRACE build of spopt with OLD_D13, test switches built into a copy of `/lib/vc/spopt` and measured on the whole model. Measured and dropped: codemove from the last jump backwards (105 exact, 1294 lines); a block whose end jumps to a jump stays (jpput, jrput exact, but jpr worse; sharpened to "the block behind it jumps to the same place": no loss, but jpput and jrput then went by source form); the test of the CJP skip before the shortening of jump chains (jpr2 lost); loop inversion also behind a jump (blkclose lost); a smaller window of loop inversion (n 20, 16, 12: worse); `OLD_D20` again (124 exact). Open candidate for jdeps only (one witness): a conditional jump at the top of a loop to the label right behind its back jump is not shortened through the jump there (jdeps 44 > 13), and the loop inversion then takes the last such jump of the test (jdeps exact); with both 132 of 151 exact at that time, no loss, yylex +9. Most of K12 went by source forms instead: stenter (`if (*name != 0) {...} else {...}`), jpput and jrput (putins in each case, no default), lstsync (`continue` in the listing branch), jprange (no return behind the switch, a quirk; `return (1)` tested first). Now 133 of 151 exact, 486 lines.
- cas yylex by source forms only (2026-11-10, owner: source forms first, switches may later take some back): 306 > 34 lines. Forms: `~` and `.` bodies as goto targets behind the names, their tests behind them (`goto other; tilde: ... dot: ... other: if (c == '~') goto tilde; ...`); in the decimal number the test `c == '.' || c == 'e' || c == 'E'` first and the plain number behind; `(long)(unsigned)(idx = stlook(...)) == 0` (the result of stlook widened from r2 into rr0); alias and found inside the block of an unknown name; the local name looked up when stlook finds it, 0x57 behind, `if (isdef(idx)) goto value;`; the digits of the exponent first. Left, no source form found: the place of `done` (the original has it behind `tok = 3` at the end of the `.` body, our spopt moves it to the first `goto done` of the list, in the names; done with its switch written there takes the case bodies with it, the original has them at the end of the function), and the mask in alias (`ldl rr2,rr0; and r2,#%7F00`: a copy before the and, as D24/D25 of vk; no form of the cast changes it); the rest are jr/jp lengths behind them. Also: jdeps exact by `while (q != 0) { if (v > ...) break; ... }` (the jdeps popt candidate above is not needed); jpr 26 > 22 by one `goto plain`. Popt candidates kept for later: the split dispatch `cp a; jr ne,REST; case a` (stabout, jpr), the order of loop inversion (putrow), the place of `done` (yylex). cas now 134 of 151 exact, 166 lines.
- vc switches with two witnesses (2026-11-10, `MATRIX_VER=vc oldcc.sh matrix`): source forms tried without the switch, none takes it away; both stay. `OLD_D2M` (asmins, newrec: a word times 6 through rr2): `jfree + (jused - 1)` as `&jfree[...]`, through a temporary, `jfree - 1 + jused`, `jfree + jused++` (best: `n = jused - 1; return (jfree + n);`, 1 line left, the store of n in r13); the operands and casts of `lctab[cursec][cursub] + instlen[...].len` in asmins (10 lines at best against 5). `OLD_D2U` (opmul, xtol: the right operand of a long multiplication in rr2): `digval(*s) + n * base`, `base * n`, casts of base; `r->n_u.l * l->n_u.l`: no change.
- vc `OLD_D24` and `OLD_D25` (2026-11-10, candidate B): the masks of yylex (alias) and igetaddr are those of vk. Taken in alone they cost nine exact functions of cas (stlook, defsym, evalint, exprval, opsub, opadd, optim, jpr, blkclose ...): every `(NODE *)np->n_u.cc.l1` got a mask. With pointer members in the union of a node (`p`, `wp.p`, `pp.p1`, `cp.p1`, ...; 39 casts gone), `char *mkseg()` and the name of a symbol as `s_name` (a union with `s_2`, #define as System III does) no function is lost; igetaddr as `struct irow *igetaddr() { return ((struct irow *)igetl()); }` (its caller is `asmins(igetaddr())`). igetaddr 5 > 1 (K2 left), yylex 34 > 29; 134 exact, 157 lines. Matrix: each of the two has igetaddr and yylex as witnesses (both open).
- Candidate C, the split dispatch (2026-11-10): jpr is no witness: our spopt splits its dispatch too (loop inversion at the jump to it), only at the first compare, because the copy of `jpput(tok, list); return (0)` behind it is still there; in the original that copy had gone before (a question of which tail copy stays). Left as witness: stabout. Measured in /lib/vc/spopt: the inversion tried before the move of a block behind a jump (all cases): 107 exact; only when the block is a compare chain that falls into other code (the jump that ends it is not right behind a CJP): 131 exact, 251 lines, stabout 18 > 34. Not taken; one witness.
- Candidate A, the place of `done` in yylex (2026-11-10). Source form found first: the cases of done end with `break` (29 > 26); the block of done (`ld r2,r13`, the compares for 0x22 and 0x23, `ld r2,r13; jp exit`) is now the original's, only its place differs: our spopt moves it to the first `goto done` of the list (tok 0x56 in the names), the original has it behind `tok = 3` (the `.` part), all six jumps of the names go to it (`jp %0C08`). The rest of yylex are jr/jp lengths. Measured: the features of the 19 jumps to done that iteration 1 of our spopt reaches (a TRACE build that does not move done and prints them): the jump of the original is the first whose block is reached only by its label and whose following label no jump names; codemove with that rule (for every block, or only for blocks with more than one reference, each part alone): 21 to 131 exact, yylex 199 to 728; no rule found. Also done written behind the `.` part (with the switch 104 lines, with goto to the case bodies at the end 671, there xjump makes the fall into done a jump and our spopt moves done away; without xjump 646). A has one witness and no mechanism; left open.
- The order of popt, three witnesses (2026-11-10). exprreloc 17 > 11 by source forms first (inner cases ascending, `if (v & 8)` first). What is left there is no move but which of two equal tails stays: the inner and the outer default are both `return (0L)` (`subl rr2,rr2; jpr exit`); our spopt moves the outer one to the dispatch (from inside the dispatch it has just moved, before the jump of the default to the exit, now next to it, is taken away) and then merges the inner one onto it; the original keeps the copy at the end, falling into the exit. Same kind as jpr (which copy of `jpput(); return (0)` stays). Measured in /lib/vc/spopt: codemove goes on behind the moved block, not into it (exprreloc 11 > 9, 126 exact: exprtype, evalint, exprval, symtype, osymout, genlop, lstsync ... lost; in exprtype the original does move the default from inside the moved dispatch); `OLD_D20` on the present sources (127 exact: symreloc 8 > 0, exprreloc 9, sgetnode 1, but exprtype, evalint, exprval, opshort, opseg, opoff, genlop, jdist lost; in opshort the original moves a tail whose jump goes to the next label already in the raw code); comjump once before the first iterate (128 exact, exprreloc 39). No rule; the witnesses split into two kinds: which copy of a tail stays (exprreloc, jpr) and where a block goes (yylex done, stabout).
- cas per module (2026-11-10): the model with /lib/vc/spopt as now, with `OLD_D20`, with "codemove goes on behind the moved block", with comjump before the first iterate, with `OLD_D29` and with D20 and the second together; exact functions / lines per source file. Only symbols.c fits another one: with D20 all ten of its functions exact (now 9 / 8 lines, symreloc), no function of the file worse; getops.c sgetnode 2 > 1. expr.c, gen.c, ofile.c, optim.c, scanner.c, listing.c get worse with D20 and the other variants; D29 changes nothing. symbols.c built by sccv1 or sccvk instead (the other modules by sccvc): its functions differ in codgen (defsym 181 to 195 lines), so it is not an older compiler as a whole; at most an older spopt (D20) with the codgen of vc, one witness (symreloc).
- cas symreloc exact by a source form (2026-11-10): the cases 0 and 2/4/6 end with `return (v)` instead of `break`. The `return (v)` behind the switch had been a block (`ldl rr2,rr8; jpr exit`) that our spopt moved to the end of case 0, which changed the tail copy kept of `v |= ... << 16`. symbols.c needs no other popt (the D20 finding per module is gone). 135 of 151 exact.
- cas K11 and K7 by source forms (2026-11-10): opmatch exact with the list as a plain parameter `q` walked by a register copy declared first (`for (j = 0, p = q; ...)`): the prologue stores all parameters by ldm and procopt renames p into rr6, as in the original. error exact by `switch ((unsigned)n)`, optim by `lcaux[sec][(unsigned)sub]` in the argument of itabp: the vc rule of `OLD_D3P` (a cast of an int parameter loads it from its home). Not found: stenter (the copy of `name` in rr2 taken for `hash(1000, name)`, the original reloads it; the registers of `p2->s_12 = blklevel`). 138 of 151 exact.
- cas symtype, asmword, K3 (2026-11-10), no source form found. symtype (K5): `unsigned n; ip->s_2 = n;` takes n from r3 (with the cast `(long)(unsigned)n` the vc rule of D3P reloads it), but sccvc puts the long into a new pair rr4, the original into the pair of r3 (`ldk r2,#0`); a rule of the code generator for a widening, one witness. asmword (K14): the parameter row copied `ldl rr6,rr2` already in the raw code of sccvc; the order of the two loads, casts, `*(unsigned *)row`: no change. igetexpr/sgetexpr (K3): `opseg(igetexpr())` keeps the inner value in a frame temporary; casts, an assignment inside, a register temporary (copy to rr10), the other cases nested or reordered, a final return: no change. main: only the address of sigdone (lengths in front). checkall after the K11/K7 forms: ALL OK.
- cas K2 (2026-11-10): `ldk r2,#4` in front of `calr igetl` (igetaddr) and of `ldl rr6,row; call iputl` (putrow), the only two in /bin/cas; the other calls of igetl have none. It is template 004 of codgen (`ldk AL,AR`, a small constant into a register variable). Tried: a dead register variable in igetaddr (`register int n; n = sizeof (long);`): sccvc gives `ldk r13,#4`, our spopt deletes the dead load in rmove and keeps the save of r13; the original would have it renamed into r2 with the save gone, which our procopt never does in a function with a call (it stops at CALL). `iputl((long)row)`, `(unsigned long)row`: no change. A guess with two popt changes and an odd source form; not taken.
- vc codgen switches with the owner's exception of one witness (2026-11-10). `OLD_D2S` (allo3.c, a word in r3 widened to long in rr2; symtype with `unsigned n; ip->s_2 = n;`): first built for any odd register (as OLD_D2MO for MUL) it cost locadv (r7 is widened into a new rr2 in the original), opshort, asmins; restricted to r3, no loss. `OLD_D4A`: first tried in argplace (no effect: the temporary comes from store/stoarg in reader3.c), then for every call with one argument (p2tag and yyparse lost: `p2rep(igetw());` is stored in the original), then only under RET (`return (f(g()))`): no loss. `OLD_D2CV`: v0's `OLD_D2C` does not build in vc (its match3.c part needs d2vfirst of OLD_D2V); its allo3.c part alone (the copy of a name holds no name) makes asmword exact and stenter 11 > 9, no loss (two witnesses). Matrix: D2S symtype, D4A igetexpr+sgetexpr, D2CV asmword (+6 lines). checkall ALL OK. cas 142 of 151 exact. Left of stenter: `p2->s_12 = blklevel` takes the address pair rr0 (copy to rr6) where the original takes the free rr2.
- cas K2 solved by source form (2026-11-10): `ldk r2,#4` is the value of a constant switch. `switch (sizeof(struct irow *)) { case 4: return ((struct irow *)igetl()); case 2: return ((struct irow *)igetw()); }` gives `ldk r2,#4; calr igetl` in sccvc: the switch value is forced into r2, the dispatch on a constant is dropped and only the matching case stays (the same with `default:` alone). Not: the comma operator, `if (sizeof ... == 4)`, `?:`, an expression statement, an argument (r7). Both places are the two where a pointer goes through the long routines of the intermediate file (igetaddr reads the row address, putrow writes it), a size test for the segmented and the nonsegmented build. putrow with `switch (sizeof(row)) { case 4: iputl(row); break; case 2: iputw(row); break; }` is exact as a whole: the top-tested first loop (5 lines, taken for a popt order question) follows from the blocks of the switch. Model: 144 of 152 strictly equal; open yylex 26, jpr 22, stabout 18, exprreloc 11, stenter 9, main 6, sgetnode 2, yyparse 2. The dead-register-variable guess is refuted.
- cas sgetnode, exprreloc (2026-11-10). sgetnode exact by source: no `return (np)` behind the switch (the original has no `ldl rr2,rr10` at its exit, the default falls into the exit after error; a quirk as jprange). exprreloc 11 > 3: case 0x24 is `if (v & 8) v |= 0; else v |= 0;` (the compare stays, both stores go; with it the bodies stand as in the original). Left: the outer `default: return (0L)`. Tried on the new form: `return (0L)` behind the switch with and without `break` (3), case 5 with the body inside the first inner switch falling into the default (6), `v` declared in a block of case 5 (no node in the raw code, same). Raw code edited by hand: the original layout comes out exactly when the dispatch ends with a jump to the `subl` block and that block has no `jpr` to the exit behind it (then codemove finds no end and leaves it). spopt with `OLD_D20` on the pre-check form gives exprreloc exactly, but loses exprtype, evalint, exprval, opshort, opseg, opoff, symtype, genlop, jdist (their `default: error(...)` and other blocks in front of the exit are moved in the original): 137 against 145. A D20 limited to a jump over another label (`jpr L122; L123: L122:`): no effect at all. So the block of exprreloc is not moved for a reason other than D20; one witness.
- cas jpr, popt try D44 (2026-11-10): in jpr the loop step (`p = p->n_next` with the test) stays behind the default of the second switch in the original (`error; jpr Lbrk; Lbrk: step`, the jump gone), ours moves the step block to the `break` of case 0x19. Rule tried in codemove: a target whose node in front is a jump to that very label counts as fallen into, no move. Whole model 134 against 145 (iputexpr, sputexpr, iputline, putins, symreloc, exprval, opshort, opseg, opoff, p2rep, genbop lost), jpr unchanged at 22. Dropped.
- cas stabout, popt try D45 (2026-11-10). The original layout of stabout is one decision of popt: at `jpr ne,L183; jpr L181; L183:` (`if (*p == 0) break; p++;`) the original does not turn the conditional jump; codemove takes the block of the `break` (`L181: ldk r2,#0; ldb rl2,tag; jpr L185`, the head of `switch (tag)`) to the jump first, the dispatch follows it, and loop inversion then picks the `cp r2,#0x2a` jump: the second half of the dispatch and the first check behind the switch land behind the entry jump. Confirmed on the raw code by hand (the head of the switch written behind the conditional jump: the original order). Source forms tried without it: nine loop forms (`else p++`, `do ... while (1)`, `while (1)`, `for (;; p++)`, goto forms, `continue`), the switch inside the loop under the end test (34). Rule D45 in iterate, in front of the turn of a conditional jump over a jump: no turn when the block of the jump stands behind a jump, is straight (no label, no conditional jump, at least one instruction), ends in a jump, and the target of that jump stands behind another jump (not its own). Whole model: 146 of 152, stabout exact, nothing lost; jpr changes (size 324 as the original, 24 lines, the `plain` block goes to the type test of case 0x19). Wider forms of the rule: without the three limits 138 (iputexpr, sputexpr, opmatch, pstcmp, casechk, secname, osymout, jdeps lost); without the "not its own jump" limit 142 (iputexpr, sputexpr, pstcmp, secname lost). One witness (stabout), not taken; taken as `OLD_D45` in the next commit.
- cas jpr under D45 (2026-11-10): three plain copies of `jpput(tok, list); return (0);` 26 (the copy of case 3 stays, the original keeps the one of the first default behind case 0x36); `default: plain:` in the first switch with `goto plain` 22 (plain in front of the body of case 0x36); the form in git (plain at the end) 24. A rule "a jump to the return label does not end a block" cannot be written in popt: setref maps every jump to the last label of a run, so `break` at the end of a function and `return` name the same node (measured: no effect).
- cas yylex (2026-11-10), read only: the done block of the original (`0c08`) is the head of `switch (tok)` with its dispatch and `return (tok)`, moved by codemove to the jump behind `tok = 3`; ours moves it to the first `goto done`. The seven jumps in front of it are passed in the original. Unchanged by D45.
- cas popt tries on top of D45 (2026-11-10), whole model, sources of commit 0e4d95ba unless said:
  - D47, codemove leaves a block that more than one jump names (jpr: `plain` refc 4 and the loop step refc 4 are moved by ours, not by the original): 128, 18 exact functions lost. Dropped.
  - D45 + `OLD_D20` with `default: error(...); break;` as the first case in exprtype and evalint: both exact again (the dispatch falls into the default, no move needed), exprreloc and stabout exact, 140. Still lost under D20: genlop (same default form, not rewritten), exprval, symtype, jdist, and opshort, opseg, opoff. The last three speak against D20: their code behind the switch (`return (mknode25(...))`, with the jump to the exit label right behind it in the raw code) is moved to the `break` of the first case in the original, the bodies of the other cases stand behind it at the end; with D20 that block has no end and cannot be moved, and no switch form puts case bodies behind the final return.
  - D48, codemove removes at once the jump that the moved block left in front of its own label (exprreloc: the `subl` block then has no end; jpr: the default of the second switch falls into the loop step): 137; opshort, opseg, opoff, exprval, genlop, iputexpr, sputexpr, putins, p2rep, genbop lost, exprreloc not gained with the form in git, jpr 22. Dropped.
  - So exprreloc wants the jump of a default block to the exit gone when codemove looks at it, opshort wants the same kind of jump present. No rule of codemove separates them; exprreloc stays the only witness of its kind.
  - Tool note: a private copy of OLDROOT (`cp -a`) with its own `/lib/vc/spopt` lets the model run while checkall uses the real OLDROOT; a TRACE build of spopt (`-DTRACE`, run as `spopt + in.s out.s`) prints every block codemove moves (`/*CM L.. refc ..*/`).
- cas yyparse exact (2026-11-10): the two lines were `&yyv[-1]`, yyv stood 2 bytes too far. In /bin/cas the bss runs yyval 0xA9C2, yyv 0xA9C6 (600 bytes), a word 0xAC1E that nothing references (no text, no data, searched as bytes in the whole file), listp 0xAC20, symmode 0xAC24. The word was `int lcpad` in the programs section of parser.y, which yacc copies in front of the tables, so it came before yyv. A common defined in pst.c comes behind symmode (listp and symmode are mentioned in parser.o first); dead references (`if (0)`, a constant switch) leave no mention; a function static goes to the data. Form taken, a guess (only the layout is known): listp is the second member of a structure of 6 bytes defined in pst.c (`struct lwalk { int w_0; NODE *w_p; }`), referenced as `lwalk+2`; the bytes of every reference are the same. Model 147 of 152.
- cas stenter exact (2026-11-10). `p2->s_12 = blklevel`: sccvc took rr0/rr6 for the address because of the statement two lines above, `blkstack[blklevel - 1] = (char *)p2 - (char *)STAB`: its constant 0x30 stays in r3 as an idle word, so the pair rr2 is not free (found by replacing the statements in front one by one: only the subtraction of STAB does it). The existing switch `OLD_D2PO` of v0p (an idle odd word whose even half is free is given back for a pair) gives the original; alone it costs symtype (10 lines: r3 holds the parameter n there, the original leaves it and takes rr0), with or without `OLD_D2S`. Limit `OLD_D2PV`: not for a word that holds a parameter. Model 148 of 152, nothing lost.
- cas, the three popt functions, night 2026-11-10/11 (spopt of vc with D45; a fast loop: `D12_ONLY=module d12model.py` compiles one module again, 7 s; `D12_SRC` another source copy for parallel tries):
  - exprreloc: 40 source forms by script (first check as a switch with `break`/`return`/`goto`, body nested in its cases, default first or last or none, `return (v)` or `goto out`): all 3 or more. Popt forms that give exprreloc exactly: `OLD_D20` for a block of one instruction (loses exprval: its `return (v)` behind the switch, a block of one instruction with the jump to the exit right behind it in the raw code, is moved in the original); `OLD_D20` only for a jump that a block move left in front of its label (loses exprtype, evalint, genlop, which then want `default:` as the first case); the same with the jump removed at once (10 lost). The pair that decides: exprreloc and jpr keep a default block that ends in `return` (`subl rr2,rr2; jpr exit`, and `jpput(...); sub r2,r2; jpr exit`) where it stands, exprtype moves a default block that ends in the `break` jump (`callr _error; jpr exit`) to the end of the dispatch. Both jumps name the same label node in popt (setref maps a jump to the last label of a run).
  - jpr: the layout of the original read as a loop inversion with the far conditional jump `cp r2,#0x36`: in front of it the order was test, `cp 0x36`, body of 0x36, plain, second half of the second dispatch with the error, step, back jump, and behind the back jump the rest of the body from `cp 0x56`. Ours after the first pass: plain moved to the end of the first dispatch (or with D45 to the type test), the step moved to the `continue` of case 0x36. 352 of 4320 generated forms measured (flat or nested second switch, case orders, five places of the plain code, `continue`): best 22. With `default: plain:` as the second case the raw order is that of the original; what is missing is the same as in exprreloc.
  - yylex: the natural form (the done switch behind `tok = 3` with `goto invalid` / `goto unknown`, the two bodies at the end with `return (tok)`) gives the done block of the original byte for byte, but 671 lines: our loop inversion takes the `goto done` at the end of the `%` block as a back jump (done is fallen into, the `cp '%'` jump 17 nodes behind it names the label behind that goto) and moves done and the three character tests away. Stopping the inversion scan at any jump: 136 (the split dispatches of other functions need the scan over jumps); at a jump to the exit label: 140, yylex unchanged. Smaller windows (14, 18, 21): 221, 364, 672.
- vc switch review (2026-11-11), `OLD_D4A` removed: igetexpr and sgetexpr are exact without it when case 37 is written as the cases 35 and 36 (`l = igetexpr(); return (opseg(l));`): the copy `ldl rr10,rr2` does not appear in the last case in front of the exit. The note of 2026-11-10 ("copies to rr10") no longer holds with the present passes. Measured: full model without D4A and the old form: igetexpr 1, sgetexpr 1; without D4A and the new form: both exact; with D4A and the new form: both exact. The code of the switch is deleted (no other version used it).
- vc switch review (2026-11-11), `OLD_D2PV` merged into `OLD_D2PO`: the limit (not a word that holds a parameter) now belongs to D2PO in every version. cas model 148 as before; cparse built by ccv0p: 179 of 179 strictly equal, data equal. One switch less; checkall to confirm for the other passes.
- vc switch review, kept after a look (2026-11-11): `OLD_D2S` (symtype; seven forms of the assignment without it: `(long)n`, `(unsigned long)n`, `n + 0L`, `0L + n`, `(long)(unsigned)n` 7 lines, `n & 0xFFFFL`, `n | 0L` 10; the original widens r3 in place, `ldk r2,#0`, a rule of the register order); `OLD_D2CV` (asmword 4 lines, stenter 2: the original loads the parameter again where ours takes its copy; part of v0's D2C); `OLD_D2U` (opmul, xtol; a switch of v0, the right operand of `multl`); `OLD_D3J` (locadv, lstflag, setsec; a switch of v0p); `OLD_D2M` now has four (asmins, asmword, newrec, putrow); `OLD_D24` with `cparse:OLD_D25` (igetaddr, yylex and four kernel objects of vk).
- vc is v0p (2026-11-11). Of the 27 codgen switches of vc only `OLD_D13`, `OLD_D6J`, `OLD_D2S`, `OLD_D2CV` were its own (and `OLD_D24` vk's); the rest were v0's and v0p's. Each of the 20 switches of v0 and v0p that vc did not have, added alone: 18 neutral on the model of cas, `OLD_D3KC` loses copyfile (6 lines: `p = buf` takes rr0 in the original because rr2 holds the parameter src), `OLD_D2C` alone does not build (needs `OLD_D2V`). All but D3KC added together, with D2C in place of D2CV: 148 as before. So `OLD_VC` now defines `OLD_V0P`, undefines `OLD_D3KC` and adds D13, D6J, D24, D2S; `OLD_D2CV` is gone (its code was the allo3.c part of D2C).
- The real build of cas (2026-11-11): the model rewrites the data into the old layout, the build by `make` did not have it (file 89264 against 89152 bytes, strings in front of the functions and padded). scparse of vc with `OLD_D12` (and `OLD_D4R`, `OLD_D22`, the rest of v0p's parser switches, neutral): the stripped file has the size of the original, 42524 bytes differ, all behind the first open function (strictcmp on the real file: the same four functions; escape and digval are static and have no symbol there).
- cas jpr, decisions of popt (2026-11-11, tools in `host/recon/poptveto`): with a popt that can leave out single block moves, inversions and jump turns by number, searched over pairs. Source `default: plain: jpput(tok, list); return (0);` as the second (or last) case of the first switch, `goto plain` in case 3 and at the type test: without the two moves of the plain block (label 115: at the end of the first dispatch in pass 1, and once more later) 10 lines are left, and those are only the order of two blocks behind the entry jump (ours: plain, then the body of case 0x36; original: the body, then plain). The form in git (plain at the end): 10 with one inversion and one move left out. Three plain copies: 14. Second switch nested in case 0x56: 22 and 26. So in jpr, as in exprreloc, the original does not move a default block that ends in `return (0)` to the end of its dispatch.
- cas popt try D50 (2026-11-11): codemove does not take a block that other jumps name too with the jump behind a conditional jump (the default at the end of a dispatch). Model: casechk 19, jdeps 27, symtype 13 lost; jpr with `default: plain:` 56 (the later moves of the block, which the veto search left out, still happen); exprreloc 11. Dropped.
- cas popt D41 again (2026-11-11), a strong candidate: after a block move codemove returns the last node of the moved block, so the sweep of iterate goes on behind it and does not look at the jumps inside the moved block in the same pass (`return p3` in place of `return p2`; patch `host/recon/poptveto/popt_D41.patch`). With it, and without `OLD_D45`: exprreloc exact and stabout exact (two functions that had no source form), yylex 34, jpr 34 to 36; lost exprtype 7 and evalint 9 (both exact again with `default: error(...); break;` as the first case), genlop 7 (same form, not written yet), exprval 30, iputline 6, lstrest 6, lstsync 10, osymout 17, symreloc 8, symtype 18. The lost ones got their source forms under the shipped popt; they have to be searched again under D41. If that works, D41 replaces D45 and explains the cluster "a block moved out of a block just moved".
- cas, `default:` first (2026-11-11): exprtype, evalint, genlop (`default: error(...); break;` as the first case), iputline (`default: iputb(15); iputw(curline); break;` first) and symreloc (inner switch in the order default, 1, 0, 2) are exact under the committed spopt (D45) and under D41 alike; with the default last they are exact only under the committed one. Written into the sources: they no longer speak against D41. Under D41 (without D45) the model then has exprreloc and stabout exact and exprval 30, lstrest 6, lstsync 10, osymout 17, symtype 18 open besides yylex 34, jpr 36, main. symtype under D41: 30 orders of its inner switch (with and without `default: break;`), best 5 (2, 0, 1).
- cas popt D41, second form (2026-11-11): the sweep goes on behind a moved block, except when the moved block is the entry code of the function (label L1xxxx; then as shipped). Without D45, sources of commit ebb458de: exprreloc, stabout, lstrest, lstsync exact; left exprval 30, osymout 17, symtype 18, yylex 34, jpr 36, main. lstrest (`while ((c = getc(stdin)) != EOF)`, ten loop forms all 6 under the first form of D41) was the witness for the exception. Patch: `host/recon/poptveto/popt_D41.patch`.
- checkall (2026-11-11) on commit 1da06ff8 (vc as v0p, the limit of D2PO in every version, D4A and D2CV removed, scparse of vc with the parser switches of v0p): ALL OK, 116 files of the install root identical, corpus cas 657 of 657 (wega and host).
- cas under D41 with the entry code excepted (2026-11-11), further tries: symtype, 30 orders of its inner switch: best 5 (2, 0, 1; left: the body of case 2 stands at the end of the dispatch, the original has the bodies of 1 and 2 behind the return); jpr, 72 generated forms (flat and nested, all orders of the first switch, three places of the plain code, default of the second switch first or last): best 26; yylex with the done switch behind `tok = 3`: 103 (671 under the committed spopt). No form for exprval (30) and osymout (17) looked for. Balance: the committed spopt with D45 leaves yylex 26, jpr 24, exprreloc 3; D41 leaves yylex 34, jpr 26, exprval 30, osymout 17, symtype 5 and needs no D45. D41 is not taken; it stays the best lead for exprreloc and stabout together (the sweep order after a block move).
- cas yylex and jpr, more of the night (2026-11-11): jpr under the committed spopt, 432 forms with `default:` first in the second switch (all orders of the first switch, five places of the plain code, flat and nested): best 22. With the veto popt every order of the first switch reaches 10 by leaving out the two moves of the plain block; the 10 left are the order of the plain block and the body of case 0x36, which does not follow the source order. yylex with the veto popt (form in git): leaving out the move of the done head (label 145) at the first jump gives 199, at the first seven jumps one after the other 35 at best (the base is 26): the place of the done block in the original is not "the first seven sites passed over" in our order of events.
- cas jpr (2026-11-11), last tries of the night: on top of the two vetoes no pair of further vetoes gets below 10, so the order of the plain block and the body of case 0x36 needs a move our popt does not make at all, not one it makes too much. If chains in place of the two switches (the original compares r2 without a reload): 28 to 48, worse.

- cas popt D41, correction (2026-11-11): the earlier "D41 without D45" was measured with D45 still in (bv.sh passes CFLAGS, the segvc rule adds `-DOLD_D13 -DOLD_D45` behind them). spopt built with the segvc defines replaced (D13 and D41 only): exprreloc exact, stabout 18, exprval 30, osymout 17, symtype 18, yylex 34, jpr 34; with D41 and D45: stabout exact, the rest the same, jpr 36. So D41 alone does not explain stabout; it would be a second switch beside D45, not its replacement. The TRACE of exprval shows why it is not under D41: its inner dispatch (label 164) is moved to `goto` of case 5, and the original then takes the default block (176, at the end of that moved dispatch) and the `nfreed++` block (163) in the same pass, the chain D41 cuts. In exprreloc the original does not take the outer default (147, `subl rr2,rr2; jpr exit`) behind its moved dispatch: the same situation, the opposite decision.
- cas popt `OLD_D20S` (2026-11-11): exprreloc and exprval are one pair. Both have a dispatch moved to a jump and a default block behind the moved dispatch that ends in a jump: exprreloc's outer default (label 147, `subl rr2,rr2; jpr L122`, the exit label right behind it, refc 1) stays in the original; exprval's inner default (176, `error; jpr L163`) and the `return (v)` block (149, `ldl rr2,v; jpr exit`, refc 7) are moved. D20 (a jump to the label right behind it does not end the block) limited to a straight block (no conditional jump in front of that jump) whose label only this jump names: model 149 of 152, exprreloc exact, nothing lost (yylex 26, jpr 24 as before). D20 limited to refc 1 alone: symtype 33, jdist 21 lost (their outer dispatch, refc 1 but with conditional jumps, is then not moved); limited to the straight block alone: exprval 25, opshort, opseg, opoff 11 lost. Without D45: stabout 18 again, so D45 stays. D41 dropped: alone it leaves stabout open, exprval comes out only with `default:` first in the inner switch and then the strings stand in the wrong order (the default's message in front of "Value too large"), symtype best 5 over 90 forms (outer `default:` first or last, all inner orders). osymout under D41: exact with `default: break;` or `default: continue;` first in the switch on tag (`default: continue;` first is exact under the committed spopt too; source left as it is). jpr under D20S: 48 forms (each of the three plain sites inline or `goto plain`, all orders of the first switch): best 22.
- checkall (2026-11-11) on commit 46dc9d17 (popt `OLD_D20S` in vc): ALL OK.
- cas yylex under `OLD_D20S` (2026-11-11): the natural form (the done switch behind `tok = 3`, `goto invalid` / `goto unknown`, the bodies at the end with `return (tok)`) is 671 as before. The loop inversion that spoils it is taken at the `goto done` of the default of `switch (c)` in the `%` block: behind that jump stand two labels, the switch's break label (no jump names it) and the label behind `if (c == '%')`, which the `cp '%'` jump names. Measured: refcount keeping a label no jump names when another label follows it, except in front of an L1xxxx label (exit code), makes the inversion fail there (the label next to the jump is not the one the conditional jump names): the natural form 43, the model as committed unchanged (149, nothing lost). Without the exception for L1xxxx: genb, genw, genl, jinit and 12 more lost (the unnamed return label L107 in front of L10000 stops the inversion of their `for` loops); keeping every unnamed label: 21 functions lost. Not taken: the natural form loses the first `ld r2,r13` of the done head, and the original's bodies jump back to the `ld r2,r13; jp exit` of the head, which is the committed form (switch at the end) with its head moved to `tok = 3`. Under the committed form our codemove takes the head (label 145, `ld r2,r13; jpr dispatch`, refc 24) at the first `goto done` of the sweep (tok 0x56); why the original takes it at `tok = 3` is open.
- cas jpr under `OLD_D20S` (2026-11-11): veto search over the 11 decisions of popt in jpr (committed form), up to three left out: best 10 (moves of labels 116 and 129, the loop inversion at 107), the same plateau as under the committed spopt before. Full vc matrix at 3a97eee0 regenerated; matrix check clean.
- cas yylex exact (2026-11-11), popt `OLD_D49` and a source form. The committed form (the done switch at the end, its head moved by codemove) can only put the head at the first `goto done` of the sweep (tok 0x56); every site is offered in source order. The original's head stands behind `tok = 3` and its bodies at the end of the function jump back to the `ld r2,r13; jp exit` of the head. Forms behind `tok = 3`: the plain switch with the bodies inside (70 under the shipped and under the D49 refcount: the bodies stay behind the head); `goto invalid` / `goto unknown` with the bodies at the end ending in `return (tok)`: 671 (inversion at the `%` block), with D49 43 (the last body falls into the exit and xjump joins the head's `return (tok)` to it); the same with `ret: return (tok);` behind the switch and `goto ret` at the end of both bodies: exact with D49 (658 without). Model 151 of 152, main exact too; only jpr open. D49 is the refcount rule of the entry before (the unnamed label in front of another label stays, not in front of L1xxxx).
- checkall (2026-11-11) on commit e08eefd2 (popt `OLD_D49`, yylex form): ALL OK, corpus cas 657 of 657. jpr under D49: the 48 forms of the plain sites and first-switch orders, best 22 as before.
- cas jpr (2026-11-11, under D20S and D49): the original read against the raw code. Its loop is `init; jr test; A (case 0x36); plain; C' (tail of the second dispatch, `cp 0x19; jr eq`, then the error of its default); D (step); test (`jr eq` out, falls into E); E (first dispatch, `cp 0x56; jr ne plain`); F (case 0x56, head of the second dispatch); G (case 7); H (case 0x19); after the loop`. Ours after pass 1: A, C', E, F, G, H (plain inside H at the type test), step and test at the bottom (`jne E`). Tried: windows 10 to 28 of the loop inversion on the whole model (only 24 keeps everything); the loop written as pcc writes it (test at the bottom) by hand in the raw code (not the original); a forward loop inversion (the conditional jump behind the jump: C' `jpr step` followed by E gives exactly C' D test E) once per function: jpr 22, unlimited: spopt runs out of space; the same as a veto event (off by default) with the parallel veto search, depth 3, committed form and the form with the switch order 0x36, default, 0x56, and the form `default: plain: jpput(); return (0);`: best 10 everywhere, and the 10 lines are always the same: plain stands in front of A, the original has it behind A. Source forms: 48 (plain sites, orders) under D20S, 12 with `default: plain:` (orders, default of the second switch first): 22 to 24. The first parallel veto runs of the day were wrong (shared `/tmp/veto.txt`, see poptveto/README); their numbers are not used here.
- cas jpr, the step behind the plateau (2026-11-11 evening). TRACE of the best veto path (first loop inversion vetoed): after pass 1 the order is test, E (first dispatch, moved to the switch head), plain, A, C', D (step, `jpr test`), F, G, H. The inversion at the back jump of D (its next label is F, the scan finds `cp 0x56; jeq F` in E) then gives exactly the original, except that plain stands in front of A. Worked out by hand: if plain stood behind A (E still ending in `jpr plain`), the same inversion gives the original byte order, the dead `jpr plain` behind the moved jump included. Plain gets in front of A because codemove moves E to the switch head, the sweep goes on into the moved E, the chain rule turns E's `jpr default` into `jpr plain`, and codemove moves plain to the end of E at once. Tried against that: (a) no codemove for a jump the chain rule changed in the same step: model yylex 12, lstline 12 lost, jpr 22 (plain then never moves: its own jump to the exit is gone before a site reaches it); with the veto search (depth 3, forward inversion events): 10, the same rest. (b) codemove refuses a block that ends in a jump back (whole function): 70 functions lost; only when the target is a loop head that is fallen into: 11 lost, jpr 58. (c) a `while` loop with the step written in case 0x36 (`p = p->n_next; continue;`): 40 and worse. The original needs, in pass 1, neither the move of plain to the end of E nor the move of the step block to the end of A (`continue` of case 0x36), and plain moved to the default site right behind A; no single rule found that gives both.
- cas jpr, further rules and the switches (2026-11-11, late). (a) The chain rule (a jump to a lone jump is moved to that jump's target) for all jumps before the sweep of iterate: model 151 less jdeps (44), jpr 24; F (case 0x56) then follows E as in the original (`cp 0x56; jne plain`), but plain still goes to the end of E and the parallel veto search stays at 10. (b) The sweep goes on behind the moved block (D41) only for chosen moves, by the TRACE features of the move (length and conditional jumps of the block and in front of the site, label count, jump in front): no condition keeps the model; D41 at the move of jpr's E alone: jpr 36; going on behind the old place of the block instead: 45. (c) Every vc switch left out alone, jpr measured (copy of switchmatrix): only without `popt:OLD_D45` jpr gets better (24 to 22); D1R, D2L, D2R, D2T, D2W, D6S, cparse D6C/D6D make it worse, the rest is neutral. The veto search without D45 (forward inversion events, depth 3): 10, the same rest. So no switch of today blocks jpr on its own, and no rule found makes jpr exact even at the cost of other functions.
- cas jpr (2026-11-11, late): plain written in place at one of its users and reached by `goto plain` from the others: as `case 3: plain: jpput(tok, list); return (0);` in the second switch, or as `plain:` inside the type test of case 0x19; each with all orders of both switches (288 forms): best 24, and those come out with the same layout as the committed form (plain at the type test, step and test at the bottom). All three sites as copies (no label): 26; popt joins the copies to the one at the type test. Wherever plain is written, our spopt ends with the same layout.
- cas jpr, rules by features of the move (2026-11-12). Searched with the parallel veto search for paths that put plain behind the body of case 0x36 (as the original): with today's spopt none (depth 3). Rules measured on the whole model (`condrun.py`): r5, codemove refuses a block that jumps to the exit, is named by more than one jump, has more than two instructions, and whose site has no jump in front of its run or two conditional jumps in it: only iputline lost (6), jpr 22 (committed form). With the form `default: plain: jpput(); return (0);` (order 0x36, default, 0x56) plain then stays at E's end jump, and one veto pair puts plain behind A. s6, r5 and also no move of a block that ends in the jump back to a loop head that is fallen into, to a site of one instruction: only iputline lost, jpr 42 to 50 by form; one veto (the error default of the second switch not moved to the end of its dispatch) then gives plain behind A, but the veto search stays at 20 lines from there (depth 3; the rest is the place of the step, the test and case 7). Without a rule that makes jpr exact, none of these single-witness rules is taken.
- cas jpr, night 2026-11-12 (owner's mandate: a switch if jpr gets exact without losses). (1) Veto search with every decision of iterate and comjump as an event (`vpatchX.py`: chain rule R, jump to the next label N, xjump X, backjmp B, besides M, I, S, F): committed form, `default: goto plain` in the middle, `default: plain:` inline: no path puts plain behind case 0x36 up to depth 2, best 10 as before. (2) With r5 built in (`default: plain:` form): best 12 at depth 2 with the move of F to the 0x56 break and one comjump pair left out: everything as the original except plain, which stands between E and F; with plain behind A at most 20. The move that spoils it is M105: the step block is taken to the `continue` of case 0x36 in pass 1; left out, the best is 30 (depth 2 from there). (3) Rules tried as whole-model variants: S2 (at `cjp c,L1; jp L2` with L1 not behind, swap the targets when L1 has fewer references, so codemove takes L1's block and the shared block stays behind the conditional jump): yylex 82, exprreloc 8 to 26 lost, jpr 24; with `default: plain:` it splits the first dispatch (56). Two sweeps in iterate (jumps first, codemove after): spopt loops on ifile.c. (4) `OLD_D6C` (vc's scparse) only chooses compares or a cpir table; the dispatch stays behind the bodies in both. (5) Tool fixes of the night (poptveto README): time limit on the slot compilers, stale models, the veto log read to 300 lines; 26 hung emulators of the evening searches stopped.
- cas jpr, loop inversion window (2026-11-12, night): the window of 24 nodes counts the comment nodes of our codgen too. Not counting them (`winc.py`): n=24 keeps the model as it is (jpr 24); n=12/16/20 lose 6 to 13 functions; n=28/32 lose exprreloc, symadjust, jdeps (and opsub); n>=40 also yylex. Not counting labels either: the same picture. With u1 and n=40 (comments not counted) the form `default: plain:` gets 36 lines: C' and the step now stand in front, but A and plain go behind E/G and the test goes to the end again. The pcc layout of `for` (`jp test; step; test; body; jp step`, written into the raw code by hand) gives the same rotation as ours.
- cas jpr, end of the night (2026-11-12, 05:30). With u1 built into the veto popt (all events) and the form `default: plain:` the search reaches 20 lines at depth 3 (vetoes: the second dispatch not moved whole to the end of case 0x56, two comjump pairs); from there two more vetoes of any event (2000 runs) stay at 20. That rest: the error default (C) stands right behind the entry jump and the body of case 7 (G) in front of the step; the original splits the second dispatch after `cp 7` (`cp 7; jr ne C'`, then G) where ours splits it after `cp 0x19`. Nested second switch (432 forms under the committed popt, u1, u1 with window 40): 30 at best. No switch taken; compiler and sources of cas unchanged in the night.
- checkall (2026-11-12, morning) on commit 5e3e1c96: first run FAILED in corpus scchain, one case (src/cmd/cas/nodes.c, messages and files): the cached result of the original was wrong (the case was identical with CORPUS_NOCACHE=1; the whole scchain corpus run again without cache: 294 of 294). Second run ALL OK, corpus cas 657 of 657 (wega and host), scchain 294 of 294. MATRIX_VER=vc matrix check clean. Compiler and cas sources unchanged in the night.
- cas jpr, the split after `cp 7` (2026-11-12). In the original `cp 7; jeq G` of the second dispatch is reversed (`jr ne C'`) and its target C' is the node right behind it in the raw code (`cp 0x19`): the mark of a loop inversion (it reverses the conditional jump it found and puts a new label on the node behind the moved stretch). So the original's inversion reached `cp 7`, about 35 nodes behind the loop head, beyond our window of 24. With u1 and a window of 40 (comment nodes not counted) pass 1 makes exactly this split (`cp 7; jne C'`, C' = `cp 0x19; jeq H; error; jr D`); the order around it is not yet the original's (E, A, plain behind C'; a second inversion puts the test at the bottom again). jpr 36.
- cas jpr, u1 with window 40 traced (2026-11-12, `invtrace.py` prints each loop inversion). Pass 1: E to the switch head; the moves of plain and of the step refused (u1); the second dispatch to the end of F; C with the step to its end; inversion at D's back jump with next label G, found `cp 7; jeq G` (reversed to `jne C'`); a second inversion at a jump to the step with next label I. After pass 1: `jr T; C'(cp 0x19; jeq H; error); D; T; E(..; jeq F; jpr plain); A; plain; F(..; cp 7; jne C'); ..; H`: the original's order except A and plain, which stand between E and F instead of between `jr T` and C' (then E would be turned to `jne plain` with F behind it). The inversion alone cannot give the original's order (it would put its reversed target at A, not at C'), so another move must take A and plain to the front. Veto search on this base (all events, forward inversion, depth 3): 30 at best.
- cas jpr, A and plain to the front (2026-11-12). Traced with every move and inversion dumped (`invdump.py`), form `default: plain:`, plain never moved: pass 1 does M109 (E to the switch head), M105 (the step to the `continue` of A, so A falls into the step), the second dispatch to F, C behind it, then inversion 1 (at a jump to the step, next label G, found `cp 7; jeq G`: the original's split, C' = `cp 0x19; jeq H; error` now stands in front of the step), F to the end of E (E turns to `jne plain`), inversion 2 (at D's back jump, next label plain, found `cp 0x56; jne plain`), inversion 3 (test to the end). Because M105 put the step between A and plain, inversion 1 puts C' and the step there too: `T E A C' D jpr-T plain F ...`; the original needs `T E A plain C' D jpr-T F ...` before its last inversion (then the inversion at D's back jump with next label F finds `cp 0x56; jeq F` and gives the original exactly). So the step must land behind plain, not behind A; the first `jpr step` the sweep meets is A's `continue`, right in front of plain. Veto by event classes (`pclass.py`: plain never moved, first inversion at the test left out, forward inversion allowed): 22 at best.
- cas jpr, the step behind plain (2026-11-12). Our committed result is near the original apart from plain and the place of the step and the test: it has A right behind the entry jump and the split `cp 7; jne C'`. Form with plain as a copy in the default of the first switch (order 0x36, default, 0x56) and `goto plain` from case 3 and the type test to a second copy at the end (`poptveto/jpr_copy_adb.c.txt`): with the move of E to the switch head left out and the first forward inversion done (veto popt, event classes `pclass.py`) jpr has 10 lines, and they are only the place of plain: everything else is the original's, the step and the test stand behind C' as in the original. Our spopt moves the copy behind A (refc 1) to the end of E, and xjump then joins it to the copy at the end (that copy falls into the exit), so plain ends at the end of the function. Not helping on top: leaving out the n-th xjump or the n-th comjump pair, refusing every move of the copy (22), codemove refusing every block that ends in a jump to the exit with more than two instructions (22 on this and the `default: plain:` form). Three copies (no goto): 14, the copy at case 3 stays.
- cas jpr, the step behind plain, continued (2026-11-12). The pair "move of E to the switch head left out in pass 1, first forward inversion done" (event classes, `pclass.py`) gives 10 lines on every form of the plain sites (copy or `goto plain`, 48 forms, `runforms.py`), also on the committed form; the 10 lines are always the place of plain, never anything else. On the committed form plain is first moved to the default site (M116), then to the end of E (M115); in pass 2 E is moved to the switch head with plain behind it, so plain stands in front of A. Leaving out the move to the end of E puts plain at the type test (12); leaving out that one too puts plain behind A, but then the split after `cp 7` and the forward inversion no longer happen (30; two more vetoes of any event from there: 30 with plain behind A, 10 otherwise). Refusing every move of plain by rule (r1, or every block that jumps to the exit) on all 48 forms: 10 or worse.
- cas jpr, plain behind A by rule (2026-11-12). How plain gets in front of A on the 10-line path: codemove moves plain to the site of `default: goto plain` (a jump alone behind its label, jlen 0), and from there with the end of E (M115/M117) to the switch head. Rules tried (whole model, then forms with forward inversion as veto event): (a) D41 only for the move of E to the switch head: 22 at best over 48 forms; (b) codemove refuses a block whose last jump goes to a lone jump (a chain): 22; (c) LJ, codemove takes no block to a jump that stands alone behind its label (`jn==0`): neutral on the model (only jpr open, 24), but with the form "plain as code behind case 0x36, `default: goto plain`" (`poptveto/jpr_pl1.c.txt`) plain is then pulled to the end of E right after E is moved (the chain rule turns E's default into `jpr plain`); (d) LJ with rt (no codemove for a jump the chain rule just changed): plain stays behind A, but the step goes to A's `continue` (M105) again: 24; (e) LJ, rt and u1(b) (no step block to a site of one instruction): pass 1 gives `T E A plain F+dispatch C D jpr-T G H`, the wanted prefix except F (behind the back jump in the original); then plain goes to the type test and no inversion reaches `cp 7`: 45; with window 40: 34 (A and plain then go behind E). Open: what moves F (with the head of the second dispatch) behind the back jump of D.
- cas jpr (2026-11-12, afternoon): veto search (all events, depth 2) on top of LJ + rt + u1(b) with the form "plain behind case 0x36, `default: goto plain`": 20 only with the entry move left out (not physical), else 42 to 45. The default first in the first switch (plain in front of A in the raw code) on the 10-line path: plain at the end of the function, not behind A. The 10-line path stays the nearest: everything right except the place of plain.
- cas jpr, other source structures (2026-11-12, `poptveto/jstruct.py`, `jgoto.py`), all under the committed spopt: loop as `for (p = list; p != 0; p = p->n_next)`, as `while (p != 0)` with `goto next` and `next: p = p->n_next;`, as `for (p = list; ; p = p->n_next)` with `if (p == 0) break;`; plain at the end with `goto`, inline in the default, or as labelled code behind case 0x36; second switch flat or nested in case 0x56; orders of both switches (432 forms): 24 at best (162 forms). Loops written with `goto` (test on top, test at the bottom, step in front of the test with the entry jumping to the test; 216 forms): 24 at best (only the test-on-top kind). The form with the step in front of the test (like the original's layout) keeps A and plain together, but gets 36; veto search on it (depth 2): 26. No other source structure found that comes nearer than the committed one.
- cas jpr, the 10-line path by rule (2026-11-12). Every codemove and every forward-inversion chance of all cas modules logged with features (`featlog.py`, `fwdfeat.py`). Forward inversion: possible in 40 functions; done once per function it is harmless everywhere except jdeps (13) and jiter (29); of all forward inversions done only jpr's has a target label named by three or more jumps (the loop's `continue` label, refc 4; the others 1 or 2). The move of E to the switch head (M109) resembles only stabout's and yylex's dispatch moves; jpr's block is short and its last jump goes to a chain. Two rules together (`combo3.py`): (M1) codemove refuses a block with two or more conditional jumps, a site with one conditional jump in front, a lone label in front of the site, and a last jump to a lone jump (a chain); (FW) a loop inversion may also be done forward (the conditional jump behind the jump), once per function, only when the jump's target label has three or more references. Model: nothing lost, jpr 10 (the committed form and others, e.g. `default: goto plain` in the middle, plain behind case 0x36); the 10 lines are the place of plain. Not yet taken as switches (jpr not exact).
- cas jpr, plain on top of M1 + FW (2026-11-12, evening). TRACE of the form "plain behind case 0x36, `default: goto plain`" under M1 + FW: plain is moved first to the lone `default: goto plain` site (jlen 0), then with the end of E, and in pass 2 E with plain is moved to the switch head, so plain stands in front of A. Added rules: LX (codemove takes no L1xxxx exit block to a jump that stands alone behind its label): jpr 10, plain then pulled to the end of E right after the chain rule turns E's default into `jpr plain`; LX with rt (no codemove for a jump the chain rule just changed): model loses yylex and lstline (12 each), jpr 10, the plain forms 24; R5 (exit block with more than one reference, more than two instructions, no jump in front of the site's run or two conditional jumps in it) beside M1 and LX: iputline lost (6), jpr 10, the plain forms 56. None gives plain behind A with the rest kept.
- cas jpr, D45 and the type test (2026-11-12, evening). Raw code (`sccvc -S` without -O) read against the original: in the form `default: plain:` (order 0x36, default, 0x56) plain already stands behind A, E falls into F after the chain rule and the turn (`cp 0x56; jne plain`), and the type test of case 0x19 (`cp 7; jeq L; cp 1; jeq L; jp plain; L:`) becomes `cp 1; jne plain` by the turn, as in the original; so the original needs no move of plain at all. Ours moves plain into the type test because `OLD_D45` skips that turn: the target of the `jp` (plain) is a straight block not fallen into whose last jump (to the exit) has a jump in front of its target. D45 with the exit (L1xxxx) excluded as that second target: model unchanged (only jpr open), jpr 10 under M1 + FW, the turn now happens. On top of that (committed form, M1, FW): refusing the first move of plain to the end of E: 12, then plain is at the type test no more but moved to the end of E in pass 2 (10, plain in front of the entry jump); refusing that move too: 30 (plain behind A, but the split after `cp 7` and the forward inversion are gone, C stands between A and plain). Veto search (all events, depth 2) on M1 + FW + D45 without exit: 10, no path with plain behind A.
- cas jpr, plain never moved (2026-11-12, evening, form `default: plain:`, order 0x36, default, 0x56; rules M1, FW, D45 without exit). A3, codemove refuses the exit block at a site of 8 instructions with three conditional jumps and no jump in front (jpr's plain only): model unchanged, jpr 56. With u1(b) on top (no step block to a site of one instruction): model unchanged, jpr 54; pass 1 then gives `T E(..jpr plain) A plain F+d2(cp 3, cp 7, cp 0x19, C) D jpr-T G H`. The original needs `T E A plain C' D jpr-T F G H` there: F with the head of the second dispatch (up to `cp 7`) behind D's back jump, C' (the rest of the dispatch and C) in front of D. F is named only by a conditional jump (`cp 0x56; jeq`), so codemove cannot take it, and a loop inversion moves a stretch that starts at the loop head (T), not at F. Windows 28 to 48: no better (44 at 40, other functions lost).
- cas jpr, fast veto search (2026-11-12, `poptveto/jfast.py`, `fveto.py`: spopt on the raw code and the host cas, 0.2 s a run). Veto popt with every event (`vpatchX.py`, forward inversion as event), depth 3 (10000 to 15000 runs a form): committed form, `default: goto plain` in the middle, `default: plain:`, plain as copies (abd, adb, three copies): 10 each, always the place of plain; g3, g3end: 20. Pl1, pl2 and the search on M1 + D45 without exit left out (depth 2 there: 10). No path of left-out decisions up to three makes jpr exact.
- cas jpr, the state before the last steps (2026-11-12, evening). Worked back from the original: the split after `cp 7` is a loop inversion, and an inversion is only done when an instruction falls into the jump's target label (else codemove takes the branch); the jump left behind lands in front of C' and is dead in the original, so the block that fell into the loop head must end in the jump to T after the inversion: the only such block of the original is init (`ldk r9,#0; ldl rr12,rr4; jr T`). Hand-built states (`poptveto/mkstate.py`, blocks of the raw code in another order) run through spopt with `jfast.py`: state Q4 (`poptveto/jpr_Q4.spec.txt`: entry jump, A, plain, init, T E F with the second dispatch up to `cp 0x19`, C, D with its back jump, G, H with its tail already joined to G's store by comjump, the rest) gives the original exactly (0 lines, all jump targets the same) with two changes to spopt: the entry jump (`jpr L10001`, the move of the prologue to the top) is not taken by codemove before the first comjump (`entry2.py`), and codemove takes no block that runs over a `ret` (`retstop.py`; without it the exit and the prologue are moved to the end of plain while the prologue is still at the end). With the shipped vc spopt Q4 gives 50. Path: the inversion at D's back jump (next label G, found `cp 7; jeq G`) puts T..`cp 7` behind D and leaves `init; jp T` behind plain; the entry move in the second round takes the prologue to the top and then init with its `jp T` to the prologue's jump. Open: how the raw code reaches Q4 (plain right in front of init); both rules on the raw forms: 22 to 36 (committed 24). No switch built yet.
- cas jpr, Q6 (2026-11-12, evening). A second rule makes the place of init free: when codemove finds that the block of the target runs up to the jump itself (`p1 == p3`, a loop no other jump leaves), it does the loop inversion instead of giving up (`poptveto/iv.py`; in V7 c2 it gives up there too). State Q6 (`poptveto/jpr_Q6.spec.txt`; entry jump, A, plain, T E F with the second dispatch up to `cp 0x19`, C, D with its back jump, G, H with its tail joined to G's store, the rest; init with `jp T` behind the prologue at the end) gives the original exactly with both rules (entry move after the first comjump and iv; no block over `ret` and D45 without exit beside them); each rule alone: 50. Without the joined tail of H: 12 (the classic inversion at H's back jump puts the test at the bottom), so the inversion after `cp 7` comes after the first comjump. On the raw forms both rules give 22 to 24; a veto search with every event and the two rules fixed (depth 3, six forms): 10. `evopt.py` makes the two rules veto events (E: hold the entry move until the first comjump, one per function; V: the inversion at `p1 == p3`, each time).
- cas jpr, `goto done` (2026-11-12, night). The unnamed label in front of I that Q4d needs comes from the source form `for (p = list; ; p = p->n_next) { if (p == 0) goto done; ... } done:` (`poptveto/loopforms.py`, `jpr_adb_plain_goto_done.c.txt`): the loop's break label is named by no jump and stands in front of `done:`, and D49 keeps it, so the classic inversion at H's back jump does not find its label. With the shipped vc spopt: 12 (`default: plain:`), 10 (plain as copies), committed form 12 (all 24 with the `for` test); with D45 without exit: 10 on all, the rest is the place of plain. `if (p == 0) break;` and a label behind the normal loop: no change. Veto search (all events, depth 3) on the shipped spopt and the form `default: plain:` with `goto done`: 8 lines with three vetoes (the entry move in pass 1, the move of E to the switch head, the move of the step to A's `continue`); the 8 lines are the exit code (`ldm; add; ret`), which then stands behind plain instead of at the end: codemove moves the exit with the prologue behind it to plain's jump (the block runs over the `ret`), then init behind the prologue, and the entry move later takes prologue and init to the top. In the original plain keeps `jr exit`. With `retstop.py` (no block over `ret`) that path is gone (42), best 10.
- cas jpr, depth 4 (2026-11-12, night): veto popt with every event, entry move (E) and rule iv (V) as events, no block over `ret`, D45 without exit, form `default: plain:` with `goto done`: 141705 runs, best 10 (the place of plain). Only the path that moves the exit code with the prologue behind plain reaches 8.
- cas jpr, end of the raw code (2026-11-13, night). Variants of the function frame in the raw code of the `goto done` form (`poptveto/tailvar.py`): prologue in front of the exit code, prologue at the top without the entry jump, with its label: shipped 12, D45 without exit 10; veto searches (all events, with and without the events E/V and no block over `ret`), depth 3: 10 each. No path to the original from another frame layout; scodgen not changed (no trace leads there).
- cas jpr, partial step taken (2026-11-13, night, owner's mandate point 3). `OLD_D45` limited: no skipped turn when the head's jump goes to the exit code (an L1xxxx label); jpr.c written with `for (p = list; ; p = p->n_next) { if (p == 0) goto done; ... } done:`. Full model: 151 of 152 exact as before, jpr 10 (was 24); D45 limited alone with the old jpr.c: jpr 22, nothing else changes. The 10 lines are the place of the jpput/return block.
- checkall (2026-11-13, night) on commit 5013824b (D45 without exit, jpr `goto done`): ALL OK. `MATRIX_VER=vc oldcc.sh matrix check`: switches and programs as in the table; vc matrix measured again.
- cas jpr on the new base (2026-11-13, night; `goto done` in git, D45 without exit). The 10 lines are the order of A and plain only. Veto searches depth 3 (all events; with E/V events and no block over `ret`; with M1): 10, 10, 8. The 8 is again the path that moves the exit code with the prologue behind plain (then init behind the prologue, the inversion leaves `init; jp T` behind it, the entry move takes prologue and init up): right except the exit code, which the original keeps at the end. Every path below 10 found so far needs prologue + init right behind plain at the time of the inversion; our codemove can only bring the prologue there together with the exit code (the block runs over `ret`, and the exit label is not yet fallen into because I's jump to the next label is still there when the sweep passes plain).
- cas jpr, natural loop forms for the unnamed label (2026-11-13): `if (p) do { ... } while ((p = p->n_next) != 0);` (four spellings): 59 to 60; `if (list != 0) for (...)`: 24 to 26. `goto done` stays (owner: no one would write it so; kept for now, marked in findings).
- cas jpr, pass 1 as the original needs it (2026-11-13). On the `goto done` forms the first pass of the original must leave E in place so that iterate turns it (`cp 0x56; jne plain`, falling into F) and plain stays behind A. Our pass 1 instead (a) moves E to the switch head at once, (b) moves F to the lone `jp F` of `case 0x56: break`, (c) moves the step to A's `continue`. Rules: `turnable` (codemove takes no block whose end is `cjp L; jp X; L:` with L reached directly or through a lone jump: the turn first; `poptveto/turnable.py`), LJ (no block to a jump alone behind its label), u1(b): pass 1 then gives `T(jp E); A; plain; E(jne plain) F d2 C D(jp T); G; H; I` with the form `default: plain:` + `goto done`; pass 2 then moves E..D whole to T (62). Veto search on this base running.
- cas jpr (2026-11-13): veto search depth 3 on the pass-1 rules (turnable, LJ, u1(b), E/V events): 22 to 32, worse than the base. Loops written with the test at the bottom by `goto` (four spellings, four plain sites): 34 to 75. Sweep order variants of iterate (stop after the first move of a pass; go on behind the moved block; a pass with turns only before each pass): 10 to 65. A generator of 1728 natural forms (loop with test or `goto done`, the three orders of the first switch, default as `goto plain`/inline/copy, 24 orders of the second switch, two spellings of the type test; `jgen3.py`) runs against the committed spopt and four rule variants.
- cas jpr (2026-11-13): the 1728 generated forms (`poptveto/jgen3.py`): best 10 everywhere (108 forms), nothing below. `pathsearch.py`: a model of the spopt primitives on the raw node list (block move to a jump, loop inversion with any window, the inversion where the block ends in the jump itself, turn, jump to the next label; chain retarget, dead code and label removal eager), best-first search in any order from the raw code to the original layout (`jpr_OS.spec.txt`), to find the order of steps the old spopt takes.
- cas jpr, primitive path search (2026-11-13, `poptveto/pathsearch.py`, `replay2.py`). With an unlimited inversion window and blocks over `ret` allowed the search finds the original's loop layout in 13 steps (E turned in place instead of moved to the switch head; exit + prologue behind plain, init behind the prologue; inversion after `cp 7`; then an inversion spanning almost the whole function takes exit + prologue + init back to the end). With real limits (no block over `ret`, window 24) the search runs out of states (216040 on `default: plain:` + `goto done`, 224567 on the committed form) without reaching the original: with our primitives (block move, back inversion, inversion where the block ends in the jump itself, turn, jump to the next label; chain retarget, dead code, D49 label rule eager) the original is reachable in no order at all. So the old spopt has an operation ours lacks or does one differently. Next: the forward inversion as a primitive.
- cas jpr, path search continued (2026-11-13). Added primitives: forward inversion (`FWD=1`), xjump and comjump/backjmp (`COMJ`). With real limits (no block over `ret`, window 24 or 32): not found within 800000 to 1000000 states (best distance about 10 tokens); the best-first order is drawn to the early entry move. Now searching with the entry move left out and the original with the prologue still at the end (`OS0`) as the goal.
- Stage 1 and libl (2026-11-14). libl `reject.c` with `ccv1` of stage 1 gave `cpb _yyextra(-2),#0`: the v1 codgen of stage 1 was linked with the old `printf` (oldlibc 0_5.c) and the shipped `_doprnt`, whose `%s` reads `base`/`val` left on the stack by an earlier negative `%d` (quirks table, libc `_doprnt`). Stage 1 now links the old `_doprnt` too (`doprnt/0_5.s`, as `make shipped` in src/cmd). The vc scodgen then overflowed the text (`ld: text overflow`): the old `_doprnt` references `__fegcvt`, and the first member of /lib/libc.a that defines it is `fltioc.o` (the real floating point output, member 17), while the shipped `doprnt.o` (member 36) is taken behind it and gets the dummy of `ffltioc.o` (member 38). `ffltioc.o` of /lib/libc.a goes in front of libc.a (`OLDPRINTF`, `s1f.o`): the link has the dummy as before. libl.a and slibl.a byte-identical; libl is built again by `src/lib/Makefile`; `stage1` depends on `clean` again.
- checkall (2026-11-14) with stage 1 on the old `_doprnt` and libl built again: ALL OK (install root 121 files identical, every corpus identical; bin/cas open).
- cas jpr (2026-11-14), not solved; what was measured (scripts: `host/recon/poptveto/s1114/`, `PPY` patches for `pb.sh`, `model.sh` runs the whole cas model with one spopt, 16 s):
  - The ZEUS 3.21 spopt (`tmp/zeus321_root/lib/spopt`, z21.c 1.2 8/27/84) as the spopt of vc: the whole model comes out exactly as with the shipped spopt and `OLD_D13` alone (open: yylex 658, main 6 (shift only), exprreloc 3, stabout 18, jpr 22). So the popt of 12/82 and of 8/84 decide alike on all of cas; the ZEUS binary does not hold the old rule. The ZEUS cas is another version (its jpr is not this one). ZEUS scodgen writes `cpl; jpr eq,Lbrk` for the loop test and a cpir table for three cases: not vc's raw code.
  - Whole-model variants without D45/D20S (each refuted): iterate sweeping backwards (jpr 81, everything scrambled); all turns of `cjp; jp; L:` before the first iterate (jdeps lost, the four open ones unchanged; with D49 jpr 10); all jumps to the next label removed first (exprval, opshort, opseg, opoff, symtype, jdist lost); a first sweep with codemove only, with or without chain rule, jump-to-next removal, xjump (stabout exact, 20 functions lost, jpr 44 to 50); at `cjp; jp L2; L1:` codemove of the `jp` tried first and the turn only when nothing moved (`hs.py`; with D20S and D49: iputexpr, sputexpr, casechk, osymout, jdeps lost, yylex 108, jpr 38; with full D20 also exprval, opshort, opseg, opoff, symtype, jdist).
  - D45 with no limit at all (`d45n.py`: no turn whenever the target of the `jp` stands behind a jump) loses iputexpr, sputexpr, opmatch, pstcmp, casechk, secname, osymout, jdeps; in pstcmp, secname, opmatch the turn is skipped and nothing is moved (`jr eq; jr` left).
  - stabout and jpr have the same shape in the original (`init; jr T; tail of a dispatch; step; T: test; ...; first compare reversed`): in stabout it is the loop inversion that finds the first compare of the dispatch codemove has put into the loop.
  - A second state that the committed spopt turns into the original exactly (0 lines, normal entry jump, no extra rule): `jpr_Qinv.spec.txt` (for `mkstate.py`, raw code of `jpr_adb_plain_goto_done.c.txt`): `init; T: test; load; cp 0x36; jne X; A; plain; C'; D; jp T; X: cp 0x56; jne plain; F; cp 3; jeq plain; cp 7; jne C'; G; H; I`; the inversion at D's back jump (next label X) finds `cp 0x36; jne X`. Not reachable with the primitives from a linear dispatch (both reversed compares would have to exist before); it shows that the last step can be the ordinary inversion at `cp 0x36`, not only the one at `cp 7`.
  - Natural forms (loop with test or `goto done`, plain as copies or `goto`, second switch flat or nested in case 0x56 with `break` or `continue`, 50 forms): committed spopt 10 at best (the known ones), `pathsearch.py` (window 24, no block over `ret`, with and without forward inversion, 500 s each, eight forms): not found.
  - `tmp/oldroot` had dangling links to the old checkout path; `make -C host oldroot` made it new.
- cas jpr exact with a veto popt (2026-11-14, later; steps and scripts in `compare/cas/HANDOFF.md` section 10, `host/recon/poptveto/s1114/`). New rule LIV: the scan of the loop inversion goes on at the target of an unconditional jump (target behind another jump, in front of the back jump) and the two pieces are moved; the jump and the blocks between stay behind the jump left at the loop head. With it `cp 7; jeq G` is node 22 of the window of 24 (about 35 straight through). LIV alone: model unchanged, jpr 10. LIV and three moves of pass 1 left out (E to the switch head, the step to the `continue` of case 0x36, F to the lone jump of `case 0x56: break`) on the form `default: plain:` + `goto done`: jpr 0 lines, the only such set up to depth 3. The natural `for (p = list; p; ...)` form: 0 with the same three and the classic inversion at the step label left out (three times). The three sites lie 5, 14 and 22 nodes behind the loop head, the kept moves outside 24: rule WIN (no block to a site within 24 nodes behind a loop head) with LIV gives jpr 0 without vetoes, window 22 to 30 alike; in its first form it loses 11 functions (labels named only by cross jumps taken as loop heads; stabout's D45 move refused). Also measured and refuted before that: codemove moving the block of the jump to the target instead (all, without the entry, entry only: 27 to 45 functions lost); turnable + LJ + u1(b) with LIV: jpr loop exact, but the dispatch of `switch (res)` is no longer moved (12).
- cas byte-identical in the check tree (2026-11-14). (1) popt: `OLD_D50` and `OLD_D51` in `src/cmd/popt/z21.c` (vc), jpr.c with `default: plain: jpput(tok, list); return (0);` as the second case of the first switch: the model has 152 of 152 functions exact (strict and all forms); every other module of cas gives the same object with and without the two switches (spopt on the raw code of the 28 modules, assembled, compared). Narrowing of D51: the window rule alone (no block to any site within 24 nodes behind a loop head) lost 11 functions; p2loop's loop has the same shape as jpr's head (`for` with a `switch` as its body) and there the original moves the table dispatch to the head and the step block to the first body: so part (a) asks for a block that ends in a turnable pair, part (c) for a site of one instruction within the window of the head the block jumps back to (any head in the window: yyparse changes). Tried for the natural loop `for (p = list; p; p = p->n_next)`: no inversion when the target label has more than 1, 2 or 3 references (jpr 0 with that loop, but assemble, expr, ofile, optim, parser and more change), no inversion over a label that an unconditional jump names (50). Left: `goto done`. (2) checkall with these: ALL OK, bin/cas still different in 784 bytes, all in the libc part behind lstrest: 37 bytes of code and the data of errlst. Causes, each measured by relinking in the check tree: the cut members `0_2.s` went through the assembler of sccvc (bit 31 set in `ldl rr0,#__iob+12` and the like; the original has it clear): assembled by the shipped cas, 14 code bytes left; errlst from /lib/slibc.a has its strings behind each table, the original both tables first: `slibc/gen/errlst.c` by `sccvc -S -O` and the shipped cas, 8 bytes left; these are the addresses of the commons `_rc` (printf) and `__pfile` (doprnt), placed in the order of the symbols: `.comm 2,_rc` in front of `.extern __pfile` in `printf/0_2.s`: `cmp` with /bin/cas clean.
- libl allprint from C (2026-11-14). The members `oldcc/allprint/0_1.s` and `oldcc/allprint.s/0_1.s` are replaced by `allprint.c` (System III, unchanged), `src/lib/libl/stdio.h` (the shipped header with `char *_ptr`) and a new switch of the v1 popt, `OLD_D52`. Measured: ccv1 and sccv1 give every instruction of the original, 126 and 181 bytes differ by the block order only. (1) Source: nine forms of the default case (`if` inverted, braces with `break`/`return`, `?:` in place of the `if`, `== 0`) and the two `putc` forms: no change of the order (the V7 form `>= 0` is further off than `< 0`). (2) Existing compilers: ccv1q/ccv1r 680 bytes (the temporary of D14Q), ccv0/ccv0p 660, cc 668; sccvc, sccvk, scc 798 against 794. (3) popt: our v1 popt made a loop inversion at the jump behind the `fprintf` arguments (found `jne` of the `printable` test as node 20 of 24: a window of 20 or less stops it, 21 does not) and left the store arm of `putc` behind the `_flsbuf` arm; a veto search (all events, depth 2) finds nothing; D45, D49, D50, D51 in the v1 popt, with window 24 and 20: no change. The original has `store; jr exit; test; jr ge,store; flsbuf; exit`. Rule: at `cjp M; A; jp T; M: Y; T:` with T fallen into from Y and the block of the jump behind another jump, the block goes in front of T and the jump stays behind Y: both members 0 bytes (whole object), also with window 24 (the inversion no longer finds its form). In this plain form the rule made popt loop on findiop.c (it undid its own move; found by the owner in checkall) and, with a guard against that, changed 63 of 142 translations by ccv1 (libc stdio and gen, oldlibc, ld, cmd). Limits tried one by one on that set: target is the exit code 44, M a compiler label 41, block behind a jump to the same target 28, Y and block straight 35, all of them 1 (strncmp of the shipped libc, `return(n<0 ? 0 : ...)`, which v1 does not build). Taken with all limits as `OLD_D52` in popt and spopt of v1; checkall ALL OK (122 files, libl.a and slibl.a identical). The same order (`store; jr; test; jr ge; flsbuf`) stands in the old doprnt of size and lex (the members still built from `.s`), there inside loops, where the ordinary inversion gives it. A slip of the session: a test compiled `oldlibc/doprnt/0_5.c` with `-S` in its directory and moved the result away, which removed the tracked `0_5.s`; restored from git before the final checkall.
- libc and slibc `oldcc/` measured against every compiler (2026-11-14; C source of `gen/` or `stdio/`, `-O`, bytes of the object that differ from the original member, object size where it is not the original's). libc: symscan cc 4, ccv1 18; nlist cc 4, ccv1 87; swap ccv0p 85 (cc 202); malloc cc 266. slibc: symscan sccvk 0, scc 6, sccv1 18; nlist scc 4; swap sccvc 228; scanf sccvc 316 (628 against 616 bytes); printf sccvc 445; malloc sccvk 580 (2988 against 2984; scc 1644 with 2752); segmon scc 1250; doprnt scc 1324. slibc symscan is now built by `sccvk` from `gen/symscan.c` (`SCCVK` in `src/lib/slibc/Makefile`), `oldcc/symscan` removed there; checkall ALL OK. 11 members left in the two `oldcc/`.
- libc/slibc nlist and symscan, two narrow rules found, not built in (2026-11-14; owner's decision pending: they need a compiler version of their own, the shipped passes cannot take a switch). Test bench `host/recon/poptveto/s1114/cg/`: `cg.sh NAME "DEFINES"` builds codgen and scodgen = shipped sources + DEFINES (PPY patches reader3.c) into a copy of OLDROOT as /lib/codgen and /lib/scodgen; `libtest.sh ROOT` compiles all 206 C members of libc (cc) and slibc (scc) with it and compares each object with `obj/NAME.o`. Baseline (shipped): 193 same; different are the 12 `oldcc` members and libc printf (203, built otherwise).
  - nlist (both, 4 bytes: the frame of `nlist()` 2 bytes smaller). The word is the temporary that codgen takes for every `return (e)` (`delay`, `freetemp`). Tried: no temporary for a constant value: nlist exact, but ctime, execvp, getpw, isatty, doscan, flsbuf, fltioc, ungetc lose it (they have it with constant values); no temporary in a function with register variables: 158 members change; `OLD_D1R` (never): 170 change. What separates nlist: the size of its automatic variables (`baseoff`): 1592 bytes (libc) and 1602 (slibc); the next largest functions with a return, execvp, have 650 and 1170 and keep the temporary. Rule: no temporary for the value of a return when the automatic variables take more than N bytes, 1170 < N <= 1592 (measured with 1024: slibc execvp lost; 1250: none). Why the old compiler does so is not known.
  - symscan (libc 4 bytes, slibc 6: `s = p->nl_name; t = &sym.sn_name[1];` behind `if ((n = readsym(&sym, stream)) == k && sym.sn_name[0] == c)` takes r3/r4 where the shipped codgen takes r2/r3: r2 still holds `sym.sn_name[0]`). The shipped codgen forgets the idle registers behind every `&&` and `||` (`rfreeall(1)` in cbranch); `OLD_D3` (never forgets) gives symscan but changes getopt, doprnt, doscan, fltioc, fseek, nlist. Narrowed step by step: only `&&` without a join label: symscan exact, nlist, doprnt, fltioc, fseek change; only for word registers: the same; forgetting between the operands instead of behind: worse; only the outermost `&&` (not one inside another `&&`/`||`): nlist and doprnt change; and only when its left operand holds a call: nothing changes. Rule: behind an outermost `&&` in a branch context (no join label) whose left operand holds a function call the idle registers keep their contents.
  - Both rules together (`-DX_D3T -DX_D1RF=1250`, `d3v.py`): 197 of 206 same, nlist and symscan of both libraries exact, no other member changes. Left: malloc, swap (both), segmon, doprnt, printf, scanf of slibc.
- Version vl built in (2026-11-14, owner's decision): `OLD_VL` = `OLD_D53` (no return temporary above 10000 bits of automatic variables) and `OLD_D54` (registers kept behind an outermost `&&` without join label whose left operand holds a call) in codgen and scodgen (`build/nonsegvl`, `build/segvl`, `/lib/vl/`), drivers `ccvl` and `sccvl` (`OLD_VL` in cc.c: the shipped cparse, gopt, popt), wrappers in `host/tools/wbin.sh`. nlist and symscan of libc and slibc are built from `gen/` by them (slibc symscan was sccvk for one commit); `oldcc/nlist`, `oldcc/symscan` removed. checkall ALL OK (122 files); both matrix checks clean (the tables are those of v0p and vc; vl is measured by `libtest.sh`: 197 of 206).
- libc and slibc built by vl throughout (2026-11-14, owner's decision): `CC = ccvl` and `CC = sccvl`. The first checkall failed in stage 2 (`Undefined: L10004` when the drivers were linked with libc-v8.a): the `OLD_D54` code as written into reader3.c had lost the `deflab(lab)` of an `&&` with a join label (the measured patch had it; with only four members on vl nothing used that path). Fixed; `libtest.sh` 197 of 206, checkall ALL OK (122 files).
- vl `OLD_D55`, slibc scanf from C (2026-11-15). Left with vl: `regs[1] = a2` of a local array addressed directly in the original. `OLD_D5G` on vl: scanf exact, 12 members change; only its segmented clause for a constant index: cnvt, execvp, nlist, symscan, popen, doprnt change. The sites of that clause with their trees (`FOLD` print): the original folds in scanf and printf (`regs[k] = aN`, the element under a STAR as the left operand of an ASSIGN, the other operand a parameter) and in swap (`*p = ((char *)&x)[1]`), not for an address (`&tmp[9]`, `&space[SPACE]`), not inside `?:` (popen), not with an address or a constant as the other operand (execvp `newargs[1] = fname`, symscan `sym.sn_name[8] = 0`). Rule: the element is an operand of a plain assignment and the other operand is a name, a register or a value through one: 198 of 206, nothing changes besides scanf; with a constant allowed symscan changes, with any operand execvp and symscan. checkall ALL OK. printf of slibc gets nearer (480 to 342 bytes), swap 269 to 221.
- vl `OLD_D56`, slibc printf from C (2026-11-15). With the stdio.h of the time (`src/cmd/cparse/stdio.h`: the V7 putc; the shipped header gives the `_flsbuf` arm first and a reload of `str`) 9 bytes are left: the frames of printf, fprintf, sprintf 2 bytes smaller, the return temporary again (`return (rc)`, rc global). At the RET statement: scanf (`return (_doscan(...))`) and readby (`return (n)`, automatic) have the temporary, nlist (constants, large frame) and printf (a global name) have not; libc printf (nonsegmented, same source) has it. Rule, segmented only: no temporary when the value is a global variable (class 2), decided behind `mapnm`: printf 0 bytes, 198 of 206 with the plain header in `libtest.sh`, nothing else changes. checkall ALL OK.
- vl `OLD_D57`, swap of libc and slibc from C (2026-11-15). Left with vl: libc `ld 0(r15),r7` for `ld @r15,r7` in the entry code of `swap(x)` and `ld r14,0(r15)` for `ld r14,r7` in longswap (`p = l`); slibc `ldl rr12,|_stkseg|(r15)` for `ldl rr12,rr6` in longswap; swapsegt, the third function, as the shipped codgen. Read as a state left by `swap`, which takes `&x`: the next function's parameter in that register is not known in it. Built as: clocal notes the register and the offset of a register parameter used as memory; rinit of the next function does not enter the parameter of that register into the register file; efcode writes the store of such a parameter with its offset. First by the frame offset of the parameter: both swap exact, but scanf and printf of both libraries change (`&args`, a stack parameter): only register parameters; then doprnt of libc changes (`&nreg` in `_doprnt`, `len` of `_atoip` has the same offset and stays in r6 in the original): by the register (r7 of x; r7 of l, rr6 of l segmented). 200 of 206 in `libtest.sh`, nothing else changes; checkall ALL OK.
- vl, state at the end of the session (2026-11-15 morning; details and next steps: `src/lib/HANDOFF.md`). In the tree, not committed, checkall running: `OLD_D58` (regforsym: an unsigned name in a register is taken from it also as an int; calloc of libc `mult rr2,r6`; with both directions fputc changes, with the word types of `OLD_D3P` lsearch, symscan, fputc and malloc itself) and libc malloc from C: `libtest.sh` 201 of 206. slibc malloc measured exact with scodgen `OLD_D24` + scparse `OLD_D25` (D24 alone: no change), `union store *r` declared in front of `onw` in realloc, an unused `int spare` in calloc; not built in (vl needs its own scparse). Slip: a copy into a root copy wrote through the symbolic link `lib/scparse` and overwrote `tmp/wega31_root/lib/scparse` with the vk scparse (08:44); found by checkall, restored from `/home/olivleh1/wega_rootfs/lib/scparse` (identical with our rebuilt scparse); the measurements in between are repeated or marked in the handoff.
- checkall of the state with `OLD_D58` and libc malloc from C (2026-11-15, after the repair of the original scparse): ALL OK, 122 files.
- vl `OLD_D24` + scparse `OLD_D25`, slibc malloc from C (2026-11-15, afternoon). Clean measurement (`cg.sh vl24 "-DOLD_VL -DOLD_D24"`, `/lib/vk/scparse` as `/lib/scparse` in the root copy, link removed first; the original scparse checked against `/home/olivleh1/wega_rootfs/lib/scparse` before): `libtest.sh` 201 of 206, no other member changes; slibc malloc 1657 -> 13 bytes, segmon 1250 -> 1326, doprnt 1324 unchanged. With `union store *r` in front of `onw` (realloc) and `int spare` (calloc): 0 bytes. The place: `setbusy(&allocs[1])` in the first call of malloc (`ldl rr4,rr2; and r4,#%7f00`). Built in: `OLD_D24` in `OLD_VL`, `build/segvl/scparse` (`-DSEG -DOLD_D25`) installed as `/lib/vl/scparse`, named by sccvl. `libtest.sh` compares with the members of the original libraries, extracted to `/tmp/SCRATCH/orig` (not with `src/lib/LIB/obj`, the build target).
- vl `OLD_D59`, `OLD_D60`, slibc segmon from C (2026-11-15, afternoon). With D24 + D25 the source had `long mkseg()`: the casts `(long *)mkseg(..)`, `(char *)mkseg(..)` were masked (`and r4,#%7F00`), the original has none; `char *mkseg()` (as in malloc) removes them: 252 bytes left. regforsym trace: `sr` (parameter in rr6, stype 24) used with type 18 (`sr->sr_seg`, member at offset 0), refused by the type test. Every pair of pointer types (`(type & 060) == 020` on both): segmon only `++i` left, but libc malloc 482, symscan 262, slibc malloc, symscan 353, printf 41 change (facts: allocp, alloct, b, q 89 from 25; ap 25 from 18; r 18 from 25; symscan p 82 from 24; printf str 28 from 18). Narrowed to stype 24 and a single pointer: only segmon changes. The register part of `OLD_D3P` alone (another wanted register): no effect on segmon, lsearch and scanf change. `++i`: the `inc` template (rshape 0x8002) matched with cookie INAREG; skipping it for cookie INAREG and taking load/inc/store (as `OLD_D16A`): segmon 0 bytes, `libtest.sh` 203 of 206, no other member changes.
- vl, slibc doprnt (2026-11-15, evening; not built in). With vl (D53 to D60, D24, scparse D25): 1324 bytes (6200 against 6204), with the stdio.h of the time (`-Iinc`, a copy of `src/cmd/cparse/stdio.h`; an absolute host path is not seen by the guest) the same. Two places, both a register choice that avoids rr2 in the original. (1) `if (_pfile && ferror(_pfile))`: original `ldl rr2,__pfile; testl rr2; jr eq; ldl rr4,rr2; ldb rl6,rr4(#%000A)`, ours `ldb rl4,rr2(#%000A)`. Raw codgen (no -O) loads `_pfile` into rr2 again; spopt turns an `ldl rr4,__pfile` there into `ldl rr4,rr2` (tried on the raw code), so the vl codgen took rr4: rr2 was still busy behind the left operand. Probe `cg/d61.py` (inside the `OLD_D54` block of cbranch: the registers that getreg hands out while the left operand is compiled are taken again if freed and given back behind the right operand): for every outermost `&&` 17 members change (getgrgid, getpwuid, ftell, setbuf, malloc, symscan, flsbuf, ttyslot FAIL ...; doprnt 22: line 299 `plus && *str != '-' && *str` gets a copy the original has not); left operand not `&&`/`||`: still 14; left operand a bare NAME (`_pfile`): doprnt 22, no other member changes. NE/EQ of a NAME of class 2 with 0: does not fire (the left operand is the NAME itself). (2) case 'l' (`fmtlen--; if ((c = *fmt++) == 'o')`): original `ldl rr4,rr12; inc r13,#1; ldb rl3,@rr4` (r3 free, rr2 not), ours `ldl rr2,rr12 ... ldb rl4,@rr2`. Trace of getreg (register file at each call): the value of the first switch (`if (prec < 0) switch (c)`) is a TEMP in r2 (kind 6, survives labels); in `lng:` the call `_align(&nreg, &ap, stkap)` asks for rr2 by number (cls 3, reg 2, regjam) and the TEMP leaves the register file; the original behaves as if r2 were still taken at case 'l'. Not tried yet: regjam keeping the kind-6 node. The value register of the cpir switch not freed at the end of gswitch (`X_SW`), or marked kind 6 (`X_SW2`): no change (the dispatch of pcc stands behind the cases).
- slibc doprnt from C with vl as it is (2026-11-15, night; owner: look for source forms that fit the existing switches before building new ones). Forms tried with the vl of the tree: for `if (_pfile && ferror(_pfile))` 8 forms; `_pfile != NULL` (also `(FILE *)0`) leaves 22 bytes, the bare name, `if (_pfile) if (...)`, `?:`, parentheses leave 1324, `_pfile->_flag & _IOERR` 1172, `!= 0` on both 1091. For case 'l' 6 forms of the statements (`--fmtlen`, `fmtlen-- == 0`, `!fmtlen`, `<= 0`, `c = *fmt++;` apart): none better. Position of the block of case 'l' among the cases: in front of `lng:` all registers of the original (the value of the switch is a TEMP in r2, kind 6; the call of `_align` in `lng:` asks for rr2 and regjam gives r2 back, so a case behind it finds r2 free); all 360 orders of the blocks `lng:`, `O`, `X`, `U`, `D`, `l` with `l` in front of `lng:`: 0 bytes for `O X U D l lng` and four others. Built so (with `_pfile != NULL`); the stdio.h of the time makes no difference. The probes of this day for doprnt (`d61.py`: the registers of a bare-name left operand of `&&` held; a TEMP that regjam took retaken at a label no jump names, dropped at a call: doprnt exact, cnvt and execvp change) are not needed and removed.
- vl `OLD_D59` and `OLD_D60` removed (2026-11-15): segmon of slibc (their only witness, a reconstructed source) is exact without them by two source forms, `if (sr[0].sr_seg == 0377)` (the NAME of sr keeps the type of a pointer to the structure, so the shipped regforsym takes it from rr6) and `} while ((i = i + 1) <= *nsegs);` (load, add, store as the original). Tried by script against vl without the two switches: for the test 7 forms (`sr->sr_seg`, `*(char *)sr`, `& 0377`, `(unsigned char)`, `(char)0377`, `-1`, `sr[0]`), for the loop 7 (`++i`, `i += 1`, `(long)++i`, `*nsegs >= ++i`, parentheses, `i = i + 1`, `i = i+1`); only `sr[0]` with `i = i + 1` gives 0 bytes. `libtest.sh`: 204 of 206 (printf of both only on the bench), no other member changes.
- vl `OLD_D56` removed (2026-11-15): printf of slibc (its only witness) is exact without it with `int regs[4]` in printf and `int regs[2]` in fprintf and sprintf, the sizes the segmented functions use (4 and 2 register arguments; the sizes 5 and 3 were those of libc). `return (rc)` stays and takes its temporary as with the shipped codgen; the frames are 2 bytes smaller by the arrays. Tried before without the switch: 8 forms of the return (`return rc`, `(int)rc`, `rc+0`, `((rc))`, a block, `return (rc = _doprnt(...))`, `extern`, `static`): no change (9 bytes); without a return statement printf and fprintf are exact, sprintf not (the original loads `r2` from `_rc` at its end). `libtest.sh` 204 of 206, slibc printf with `-I` exact.
- vl `OLD_D58` removed (2026-11-15): calloc of libc (its witness) and of slibc (where the switch was needed too, not noticed when slibc malloc came from C) are exact without it with `unsigned num; int size;`: the use of `size` in `num *= size` then has the type of the name and the shipped regforsym takes it from r6. Forms tried without the switch (7 declarations x 6 forms of the product): only `int size` (with `num *= size`, `(int)size`, `(unsigned)size`) gives 0 bytes; `int num, size` 239, `int num; unsigned size` 247. `libtest.sh` 204 of 206.
- vl `OLD_D55` removed (2026-11-15): its witnesses were scanf, printf (`regs[k] = aN`) and swap of slibc (`((char *)&x)[1]`); the shipped scodgen folds a member offset into the frame offset but computes the address of an element with a constant index other than 0. Exact without the switch: in scanf and printf the register arguments go into a structure (`struct { int *r0, *r1, *r2, *r3, *r4; } regs;`, printf `struct { int r0, r1, r2, r3; }` and `{ int r0, r1; }`, `regs.rN = aN`, `&regs` to `_doscan`, `_doprnt`); separate locals `int *r0, *r1, ...` in this order with `&r0` give the same bytes (taken: the structure, whose layout does not depend on the frame order of the compiler). In swap `struct bytes { char b0, b1; }` and `((struct bytes *)&x)->b1`, `->b0`: slibc and libc swap exact with the one source. Tried before without effect: the size of `regs` (2..5), `*(regs+1)`, the order of the assignments, casts; for swap 7 forms of the index (`*((char *)&x + 1)`, `1[...]`, `p[0]`, `*p++`, ...). `libtest.sh` 204 of 206, printf of slibc with `-I` exact.
- vl `OLD_D57` reduced (2026-11-15): its second part (in the function behind one that takes the address of a register parameter, the parameter in the same register is not known in it: longswap of libc and slibc loads `p = l` from the frame) is replaced by the source form `longswap(l) long *l;` with `p = (int *)l;` (the use has another pointer type than the name, so the shipped regforsym does not take it from its register; longswap swaps the halves of a long). Its first part stays: the entry store of `x` in swap of libc is `ld 0(r15),r7` (shipped `@r15`). Measured: at the entry code `t` and `maxoff` are 0 for the first parameter of swap, longswap, swapsegt, so the shipped path prints `@fp` whatever the body; 10 forms of the body (return, declarations, types of x and tmp, register p) and a local copy `x = n` (with `n` register or not: the frame layout moves) do not give it. Forms of longswap tried: `long *`, `char *`, `unsigned *` with the cast: all three exact; taken `long *`. `libtest.sh` 206 of 206.
- vl `OLD_D54` removed (2026-11-15): symscan of libc and slibc (its witness) is exact without it with two nested `if` for `if ((n = readsym(&sym, stream)) == k && sym.sn_name[0] == c)`: behind a single compare the shipped codgen keeps what the idle registers hold, so r2 still holds `sym.sn_name[0]` and the body takes r3, r4 as the original. Also exact: the inner `if` without braces, `if (... != k) continue;` in front. Not: the operands of the second compare swapped, the assignment of `n` in front of the `if`. `libtest.sh` 206 of 206.
- vl `OLD_D53`, source forms (2026-11-15): without the switch nlist of libc and slibc differ only in the frame size (2 bytes: the temporary of the return, behind the automatic variables, the saved registers move up; no other offset changes). The automatic variables lie in the order of their declaration upwards; the last, `diff` at 0x636, is used, so no unused variable can give back the 2 bytes. The temporary is taken by `freetemp` in the parameter area for every return with a value (types other than float, double, structures); nlist returns -1 and 0 and has no call with arguments on the stack that would have made that area before. No source form found; the switch stays (owner's exception, `OLDCC.md` section 5).

## 2026-11-17: libm.a, slibm.a (increment 150)

- Dated 01/18/83 and 01/19/83 (`Library.SC`): v1. Every C member measured with ccv1, ccv1q, ccv1r, ccvl, ccv0p, cc (-O, none)
  and sccv1, scc, sccvc: v1 (ccv1 -O, sccv1 -O) is nearest; v1q and v1r give the same, ccvl and the shipped cc differ in
  gamma and sinh too, ccv0p and sccvc far off.
- Difference 1 (all 15 C members): before every floating compare with memory v1 rounds the left operand (`%8E.4,%B.00`,
  `fld dbl`); the original has none, as `OLD_D22` of v0 ("a floating compare without rounding") but with the EPU registers
  of v1. Without these instructions asin, fmod, hypot, tanh, pow, jn are equal, nonsegmented and segmented.
- Difference 2 (atan, exp, gamma, j0, j1, log, sin, sinh, tan): the original stores an intermediate EPU value into a frame
  temporary and loads it back at once (`%4FFC,%8x03,off` `%4FF4,%8x03,off`, the frame larger by 8 per temporary), or takes an
  operand from such a temporary where v1 keeps it in an EPU register (log, atan, tan, j1); jp where v1 has jr in sinh and tan
  (guess: the longer code). No source form tried: both are compiler behaviour (phase B).

## 2026-11-18 libm: OLD_D61, OLD_D62 (v1)

- libm.a, slibm.a (System III sources, Library.SC 01/83): every compiler (cc, ccv1, ccv1q, ccv1r, ccvl) gave the same float
  code, none the original; v0, v0p take the parameters otherwise. Two differences: `fld dbl` before a compare with memory
  (rule 052/047a), and store/reload pairs every third operation of a left-deep float expression.
- Tried: `ffregs` 3 and 2 (no); the float need in the last case of sucomp by `fr == fl` or never the special case: the
  store/reload pairs exactly (both the same bytes); with the compare without `fld ZR`: 15 of 15 members exact.

## 2026-11-18 libPW: OLD_D24, OLD_D25 into the segmented v1; OLD_D20 and OLD_D52 narrowed

- libPW.a, slibPW.a (System III sources, Library.SC 07/22/83): with v1 as it was 78 of 86 members exact (after the WEGA
  source forms: compare/libPW/SYSIII.md). Open were xmsg (n, s), regcmp (n, s), regex (n, s), xalloc (s), move (s).
- Measured on a copy of OLDROOT: the shipped popt/spopt (neither D20 nor D52) for the v1 one: xmsg, regcmp n exact. scparse
  of v1 with OLD_D25 alone or scodgen of v1 with OLD_D24 alone (also the vk scodgen): the mask `ldl rr4,rr2; and r4,#%7f00`
  of a long cast to a pointer does not come (xalloc, regcmp s); both together: it comes, and the compare `u.s.off > MAX_PAGE`
  gives `ugt` as the original. With D24, D25 and the shipped popt: 85 of 86. A version of its own (v1 with D24, D25, without
  D20, D52) would contradict ld and csh (D20, 07/83 and 08/83) and libl (D52): owner: check whether the switches are too wide.
- OLD_D20: trace (popt -DTRACE, CM lines) with and without it. The moves it stops: allprint L21 (block label named by 3
  jumps when met, block 1 instruction), ld ensure_buf L232 (2, 2), load1 L337 (2, 2), and in ld main the block of L230 goes
  on behind the jump (1, 9 instructions). The moves libPW needs: xmsg L17 (6 names, 3 instructions: the `fatal(str)` tail),
  regcmp L23 (21, 4). Limits tried: only a straight block (as OLD_D20S of vc): allprint lost, xmsg and regcmp not freed;
  only a block label named once (D20S's other limit): xmsg, regcmp freed, allprint lost (3 names); at most 2 names: allprint
  lost; the block at most 2 instructions: ld main changes; the label behind the jump named once: ld main, ensure_buf, load1
  lost. Taken: at most 3 names; ld.c, symbols.c, copyname.c, printname.c give the same popt output as before.
- OLD_D52: regex `__cclass`, `return(!af)` (`cp; jpr ne,M; ldk r2,#1; jpr T; M: ldk r2,#0; T:`) takes the form; the original
  leaves it. Taken: A (between the conditional jump and the jump) more than one instruction; allprint (A: the `_flsbuf` call,
  4) stays exact; strncmp of the shipped libc (A `sub r2,r2`) falls out too.
- OLD_D25 into build/segv1 of scparse, OLD_D24 into build/segv1 of scodgen. Result: libPW.a identical, slibPW.a 42 of 43
  (move: the byte copy behind the odd-address test in rl0, the original rl2; seven forms of the test, the vk passes: no change).
- move (s): the byte copy behind `if ((int)x&1)` takes rl0 because r2, `ld r2,r13` (the low word of x in rr12), keeps the
  name. v0's `OLD_D2C` copy rule (a register copy holds no name) in the v1 branch of allocreg: move exact, but regex, xalloc,
  xlink, xwrite (s) lost (copies of parameter registers r6/r7, of the int register variable usize); only register variables
  r8..r13: xalloc lost (usize); only a word of a register variable pair (the symbol long or pointer): 86 of 86, slibl,
  sliby, slibm members unchanged. Taken as `OLD_D2CW` (segmented v1). First checkall with D20, D52, D24, D25: 774 files
  identical, only slibPW.a open, corpora equal.

## 2026-11-18 libcisam (increment 150)

- itcompare (item.o), FLOATTYPE: the original loads both floats (`4FF4,8001` into F0, `4FF4,8101` into F1) and compares
  register against register (`8E00,4120`, rule 051); v1 compares F0 against f2 in memory (rule 052). Tried first: rule 052
  only with a double in memory (table3): codgen loops ("expression causes compiler loop"), setbin never loads the right
  operand of a compare whose su and fsu are 0. Then convelim (optim3.c) keeping the conversion float to double: both
  operands get fsu 1, setbin loads left then right, rule 051. item.o nonsegmented 70 > 62 lines, segmented 106 > 98,
  no other member changes. As `OLD_D63` in codgen and scodgen v1; checkall ALL OK.
- item.c `stint(it->it_dupnum, p)`: the original shifts signed (`extsb r2`); `it_dupnum` is compared unsigned in btree.c
  (`jr nc`, `jr ule`), so it stays `unsigned` and item.c casts `(int)`. item 62 > 60.
- Register switches of v1 tried each alone on top of `OLD_D63` (all members, nonsegmented lines; base 292): D2T, D2M, D2Q
  no change; D2P, D2P+D2PO 400; D2W6 542; D2R 2780, D2R+D2V 2705.
- btdelitem (btree.o): the raw code is the same, the shipped popt gives the original's layout (no loop inversion, the
  `bad` block moved behind `iserrno = 111`); the v1 popt (`OLD_D20`) lets the block run on behind `jpr L139` to the next
  label, so it does not move and the loop is inverted. btree with the shipped popt: 41 > 12 lines (btsearch registers).
- v1 popt variants on libcisam (codgen with OLD_D63; lines nonsegmented/segmented, total of all members): v1 292/896;
  shipped popt 558/920; without OLD_D20 550/920 (btree 12/0, open 304/320); without OLD_D52 292/896. D20 fires in
  btdelitem (L142, refc 2, straight block) and isopen (L85, refc 2, the block holds `jpr eq` of the lksemaphore test).
  OLD_D20C (D20 only for a block with a conditional jump in front of the `jpr`): 263/867, btree 12/0, open unchanged.
  Refuted by checkall (bin/ld, lib/libl.a, lib/slibl.a differ: the D20 witnesses have straight blocks); taken out.
- itpress, the leading compression loop: v1 and shipped popt give the same code from the raw code (test at the head); the
  original has `jr T; C: inc; T: tests; jne out; jr C`. A source form gives it exactly with either popt:
  `i = 0; goto t; n: i++; t: if (i < ... && ...) goto n;` (not taken: a compiler cause is not ruled out).
- Owner 2026-11-18: source forms first, then existing switches (also adjusted), new switches last. Taken: itpress as a
  goto loop (item 60/96 > 4/40), btdelitem with the `bad:` label inside the error branch (btree 41/29 > 12/0).
- itpress `bycmpr(..., kp->k_len)`: the original copies the parameter kp (r5) into r4, which held kp from its frame home
  for the `k_flags` test; v1 takes r2. Source forms tried (`!bycmpr`, parentheses, `(*kp)`, `&it_key[0]`): no change.
- itunpack, segmented: the original stores `it_leadcnt` and `it_trailcnt` through rr2 after the `?:` arm has loaded the old
  `p` into rr2 (`ldl rr2,rr8; ... ld @rr2,r4`): the value goes into the buffer, not into the item (a bug of the old scodgen,
  the address pair is taken as free in the arm). v1 takes rr4. No source form.
- btsearch `openp->o_flags |= 020` behind `r == 0 && mode == 2 && bp->bf_level == 0`: the original takes r2 (holding bp) as
  free, as the shipped codgen does at the end of `&&` (v1 OLD_D3 keeps it). codgen v1 without OLD_D3 on libcisam
  (lines nonseg/seg): btree 12/0 > 4/221, open 17/267 > 267/34, item 4/40 > 93/134, key 0/0 > 4/4, lock 0/0 > 8/8;
  total 207/811 > 550/905. Source forms of the condition (nested if, `!bp->bf_level`, statements swapped): no change.
- isopen `if (ldint(dictp) != 0xfe53 ...)`: the original loads dictp again into the register that holds it (nonseg
  `ld r5,_dictp`, r2 openp and r7 filep stay known; seg `ldl rr2,_dictp`), v1 copies it. A label in front of the `if`:
  segmented exact (267 > 0), nonsegmented 17 > 285 (all names forgotten). `dictp + 0`, `&dictp[0]`, casts, constant
  forms: no change.
- D3 per join (a codgen that frees the idle registers at chosen ends of `&&`/`||`): libcisam wants them kept at every
  inner end (item, key, lock, open: the left operand of another `&&`/`||`) and, nonsegmented, the idle word registers given
  back at the end of the whole condition (btsearch); segmented the pair (rr2, bp) stays there. The same at the end of
  `!(openp->o_mode & ISEXCLLOCK)` in isexit (nonseg r2 free, seg rr2 kept). As a rule on top of OLD_D3 ("at the end of a
  whole condition of `&&`, `||`, `!` the idle word registers go back, pairs stay"): libcisam 207/811 > 69/251 lines
  (normalized: label addresses masked; btree exact, open 17 > 13), nothing worse. Under OLD_D3 itself it breaks the kernel
  (vk has OLD_D3: sys1.o, break.o); as a v1-only switch checkall fails on cp, ln, mv, rm, ld, strip, libl.a, slibl.a,
  libPW.a. Taken out. Guess: libcisam was built by a compiler between v1 and the shipped one, not by v1.
- Source forms tried without result: read (`if (lock) ... else if (openp->o_mode & ISAUTOLOCK)`: branches swapped,
  `&&` forms, a goto form), isopen (labels in front of each of the three statements before the `if`, statements swapped),
  isexit (`== 0`; `if (x) ; else {...}` fixes it nonsegmented but not segmented).
- Source forms, round 2 (owner: source first, big members first; lines with label addresses masked): build 40/118 > 0/0
  (isaddindex in the order the `.extern` list shows: the recovery block inside the loop head, `bad:` at the end;
  isbuild `*(unsigned char *)dictp = 0376; ((unsigned char *)dictp)[1] = 0123;`; btrecover two `if`s); buf 0/50 > 0/0
  (bfwrite `if (off != 0)`); node 0/4 > 0/0 (`tp->t_off != 0 ? ip2 : ip1`); open 17/29 > 10/0 (the magic read through
  `(unsigned char *)dictp` cast back to `char`: a cast to another pointer type makes regforsym miss, so dictp is loaded
  again, as in the original; as in ld chk_ranlib). Total 207/811 > 38/50. A script tried `!= 0` on every plain `if`
  condition of read, open, audit, supp, item, btree, build: nothing else gained.
- D3 split (owner's plan, 2026-11-18): `OLD_D3E` ("keep also at the end of the whole condition") for vk, v1q, v1r, v1
  without it. checkall d3a (only `&&`/`||`) and d3b (also `!`): cp, ln, mv, rm, ld, strip, libl.a, slibl.a, libPW.a
  differ in both. Not a narrowing of OLD_D3; taken out. Following the owner, new v1 switches as diffs only:
  host/libcisam_switches (OLD_D3W, OLD_D2PW, OLD_D3Q; descriptions there). Segmented sweep of the existing switches
  (23 runs, each alone on v1 with OLD_D63): none nearer for audit, item, read. Tried and dropped: a D3J-like restore of
  names at a label behind dead code for all names (OLD_D3X: libcisam 193/80, read unchanged: v1 compares `lock` from
  memory, r2 holds no name); a LIFO order of free words (D2LIFO: worse); freeing idle pairs at the end of a plain
  condition (much worse; only the pairs of frame slots: item 10 > 6 but btree, buf, node, open worse).
- itpress `if ((i = kp->k_len - it1->it_trailcnt) < it2->it_leadcnt)` as `i = ...; if (i < ...)`: segmented 6 lines
  less, nonsegmented unchanged (source form, committed).
- Left with all three switches: audit 4/4 (close(filep->f_audfd): nonseg temp r3, seg `ld r7,rr4(#20)` direct), read
  4/6 (isread: r3/rr4 for openp at the else label), supp 4/0 (freefile: the three `close` take r6, r3, r6), item 0/10
  (itpress: rr2 with kp free behind two bit tests). A codgen trace of register choices (allocreg: want, result, free
  and use lists as comments) was the tool; it is not in the tree.

## 2026-11-19 libcisam: byte-identical with five v1 switches as diffs

- Other compiler states (step 1 of the owner's plan): all libcisam members with the sources of today. Whole compilers,
  lines nonseg (label addresses masked): ccv1 38, ccv1q 542, ccv1r 815, ccvl and cc 1216, ccv0 3439, ccv0p 3663;
  segmented sccv1 44, sccvk 1337, sccvl and scc 1379, sccvc 4612. Mixed into the v1 chain: codgen v1q 542, v1r 815,
  vl 71, shipped 71; scodgen vk 56, vl and shipped 98; shipped cparse 1154; shipped or v0p popt 71. audit, read and supp
  have the same 4 lines with every codgen: none of the known states has the original's register choice.
- A trace of allocreg (want, result, free and use lists as comments in the -S output) showed the cases. Found:
  OLD_D3H (frame homes of parameters given out inside a condition go back at its end: supp freefile's three `close`
  r6/r3/r6 that look alike to v1, item itpress segmented), OLD_D2CT (a copy takes over a register given out in the last
  condition: audit AUDSTOP, nonsegmented and segmented). Descriptions in host/libcisam_switches.
- Tried and dropped on the way: freeing the parameter registers too (allocrec keeps `type` in r7), freeing at every
  level of a condition (util longmult keeps the long b in rr4), freeing all named words given out in a condition (audit
  keeps dictp in r2 across the nested `if`), the copy rule without the condition mark (audit write, lock), the copy rule
  for pairs with an even want (open: a long), the address pair of a STAR without the register wish (open), keeping or
  restoring the registers at a label behind dead code (all labels: 168/137 and 370/527 lines), assignment names.
- read isread: a source form. `switch (lock) { case 0: ...; case ISLOCK: break; default: ... goto out; }` instead of
  `if (lock) { if (lock != ISLOCK) ... } else if (...)` gives the original's code in both versions (the case 0 arm
  stands where the else part stood; the register r2 with the switch value stays busy across it).
- Result: the sources of today plus host/libcisam_switches/ALL.diff (OLD_D3W, OLD_D2PW, OLD_D3Q, OLD_D3H, OLD_D2CT):
  all 14 members of libcisam.a and slibcisam.a byte-identical (cmp of the objects). Each switch is needed (one left
  out: OLD_D3W btree 12/0 and open 10/0; OLD_D2PW item 4/0; OLD_D3Q item 0/24; OLD_D2CT audit 4/4).
- Owner 2026-11-19: the five switches go into the tree as the compiler v1c (ccv1c, sccv1c; codgen build/nonsegv1c,
  scodgen build/segv1c), the descriptions to src/lib/libcisam/COMPILER.md, the diffs (host/libcisam_switches) removed.
  libcisam into src/lib/Makefile (ccv1c/sccv1c -O, Library_libcisam.SC and Library_slibcisam.SC differ in the check-out
  times; arhdr and mkar.sh from libsetup.py). Both archives byte-identical (cmp) in a scratch build.

## 2026-11-19 clist (/lib/clist, increment 150)

- Dating: D1 tight, no SCCS line (OLDCC_DATES.md). ccv0p and ccv0 -O with src/head/oldv7 give the same bytes for clist.c
  and for fputs.c of libc; v1, v1r, v1q differ (fputs: 78/82 bytes against 74). Taken: v0p, as cpp (the same libc).
- The libc is libc-v2 of cpp with three more members in its order (fputs/0_1 new: libc's fputs.c by ccv0p; filbuf/0_3,
  atoi/0_1) and data/0_1 instead of the shipped data.o: data/0_1 names __sobuf before __sibuf, which gives the bss order
  of clist (_sobuf 0x19fe, _sibuf 0x1bfe); cpp stays byte-identical with it (cmp after the rebuild).
- The source: no source known (the System III uts/vax/io/clist.c is another thing); src/cmd/clist.c written from the
  binary, exact at the first build but for the bss; the getline loop as plain while or as for(;;) with breaks: the same bytes.

## 2026-11-20 gopt: OLD_D2K (vc)

- addallist, initallist (alias2.c of /lib/gopt): after `alast->x0 = v` (a word store through rr2 at offset 0) the original
  loads the global pointer again, sccvc kept it in rr2. Source forms tried (about 30 in small tests): casts, `*(int *)p`,
  `p[0].m`, `(*p).m`, `(&*p)`, `p + 0`, comma, `NULL`, a register copy, `(t->next = alast)->x0 = v`: none reloads, only
  `(p = f())->m = x` (the value of an assignment, which then fits mkhash and mkdomtree) and an `if` around the next statement.
  A scan of all original functions: no global pointer is used from its register after a store at offset 0 through it
  (24 uses are inside one getc/putc expression). New switch `OLD_D2K` in the OLD_VC block: matrix vc +0 on cas, real cas
  build byte-identical; gopt fnc.py 233 -> 237 OK of 322, nothing lost.
- The vc matrix model of cas compiled with the shipped stdio.h since the headers moved to src/head/oldv7 (baseline showed
  4 differing functions: getch, main, lstline, lstrest): compare/cas/progcfg.py model_prepare copies the old stdio.h;
  switchmatrix.py links src/head into its work tree. The old libc archives src/lib/libc-v*.a were incomplete (a parallel
  make had archived them before the libc objects existed): rebuilt by a plain make in src/lib/oldlibc and oldslibc.
- gopt, vc `OLD_D3JL`: canopt, denorm (DIFF 2 each) differ by the entry store `ldl 0(r15),rr6` that the original leaves out
  (its frame slot is reserved). The store comes from d3jmark (OLD_D3J: a pair parameter restored at a label behind a jump);
  restricted to longs (not pointers) for vc: canopt, denorm, islive exact (masked), nothing lost; matrix vc +0 on cas.
  Source forms tried before: register p, else, nested if, `return !(...)`, unused locals (the frame stays, the store too).
- fnc.py dropped every `and r0,#ADDR` of our object (meant for `and r0,#%7f00` behind lda): the field masks of bitfield
  stores (betainit, addquad, insertquad, exprfind) showed as DIFF. FNC_DROPRAW (unmasked) drops only `#%7f00` now.
- gopt, vc `OLD_D24L`: every `lda rrN,off(fp)` of /lib/gopt (31) has no `and rN,#%7f00` behind it; sccvc (ZL of local3.c)
  masked each. The frame place: OREG of fp with no name, symtab0 or a `~` name (fpsym1, fpsym2). gopt 244 -> 248 OK
  (insrtlpdef, insrtstmpmove, insrtopstmp, savecolors), nothing lost; matrix vc +0 on cas. fnc.py drops nothing by
  default any more (FNC_DROPRAW was `and r0,#%7f00`, the same mask).
- gopt, vc `OLD_D2AP`: OLD_D2A (copy of the value of a long assignment into a new pair) for pointers: all pointers lost 9
  functions (taballoc, clear: value in rr2); pointers in rr0 lost 4 (deletblk, lpinv: auto targets, also as two statements);
  pointers in rr0 with a global target: deltmplpdefs exact, nothing lost, matrix vc +0. orderbl stays open (auto target in
  a compare, copied in the original).
- gopt, vc `OLD_D2FA`: found with a traced scodgen (fprintf in symreg, rnassign; run by `sccvc -T2./dbgcg`): rnassign
  names the pair of an assignment to an auto with (fpsym1, frame offset), regforsym looks the auto up by its symbol.
  Steps: find autos in every class: 245 OK (words lost: findend, the original copies the word); pairs only: 250 (lost
  betainit, rmrestorequad: arguments; killdefs, initlive); not for a wanted register (rall != NOPREF): 253; a statement
  assigning to the auto forgets its frame-named pair first (as OLD_D3K does for parameters): 254 (initlive); no lookup in
  a condition with && or ||, the name stays (clearing it moves the load to rr2): 255, nothing lost; matrix vc +0.
- 2026-11-20 (owner: source forms first): the switches of the night checked again with a scodgen without them (note: the
  codgen Makefile has no dependency on oldcc.h, a change of oldcc.h alone needs `rm build/segvc/*.o`). OLD_D2AP removed:
  deltmplpdefs is exact by the source form `a0e0 = a0e0->next; prev = a0e0;` (its only witness). Kept, with the forms
  tried: OLD_D3JL (canopt 20 forms), OLD_D2FA (clearblocks 10, solvereach 11, pralvars 9), OLD_D24L (no C form gives a
  frame lda without the mask), OLD_D2K (about 30). An experiment OLD_D3JC (a call drops the parameters of r4..r7 from
  earlier jump records: prquad, cmwalk) lost 8 functions and was dropped; prquad: 14 forms tried, open.
- gopt, OLD_D2FA corrected: a register that had loaded an auto kept its frame name after a store of a new value to the
  auto (the template of `p = p->r` names nothing, rnassign is called with the OREG first), and D2FA then found the stale
  register: wrong code (asgfold read the old p). Now a store to a frame place clears that name in all registers before
  allo (the clearing after expand removed the name rnassign had just given: killdefs lost). gopt 255 OK, asgfold 3 -> 2,
  matrix vc +0.
- gopt, vc spopt `OLD_D64`: domspace (9 source forms, none): codgen writes `add r7,|_stkseg+~L1+4|(fp)`, popt turns it into
  `add r7,r3` only when it still knows r3 = that slot; memreg (z22.c) parses the offset at the first `+` (`+~L1+4`: 0), so a
  store to c's slot cleared b's register. Skipping `+~` takes 4: domspace exact, operand 31 -> 30, rmlabels 13 -> 11,
  nothing lost; matrix vc +0, the real cas build byte-identical.
- gopt: vc OLD_D10 narrowed: the immediate shift by one only for a right shift (its witness, cas asmrel, is `>> 1`);
  gopt lpow2 `r = r << 1` (7 source forms, none) goes through a register as every constant long shift of /lib/gopt.
  Matrix vc: OLD_D10 row unchanged, baseline 0. addblk exact by a source form (the ?: with the lp case first).
- gopt, OLD_D2FA for parameters: tmpsearch (8 source forms, none). All parameters found by their frame name: 254
  (delquad, delrun, insertquad, movequad, readsyms, addblk lost: the original reloads a parameter it had loaded); only in
  pairs named by an assignment (flag 0x0400 set by rnassign, cleared where getreg names a register, copied by rmove): 259
  (delrun lost: `q = r`); not when the assignment copies a variable (right NAME, REG or frame OREG): 260, nothing lost.
  Matrix vc +0, the real cas build byte-identical.
- gopt ispow2: `c >>= 1` of the register variable c goes through a register in the original. Only a plain RS immediate
  broke cas asmrel (its `>> 1` reaches match as an ASG RS of the scratch rr2): the rule is now "a right shift by one of a
  scratch register (rval < 8)". OLD_D10 row and baseline unchanged; gopt 261 OK.
- gopt, vc `OLD_D3PL`: tabfree (9 source forms, none): the pointer parameter p under `(long)p` is a NAME of type long, regforsym
  refused the pair rr4 (type differs, D3PWORD of vc: pointers only). A pointer parameter cast to long is taken: tabfree exact,
  nothing lost, matrix vc +0.
- gopt, vc `OLD_D2AV`: ceval (7 forms) and orderbl (5 forms): OLD_D2A (no witness in cas) copies a long in the original never,
  a pointer in rr0 only when the cookie asks for a temporary register (a compare: 010036; the inner assignment of a chain
  has 01312 and no copy). ceval, orderbl exact, nothing lost: 264 OK; matrix vc +0.
- gopt, vc `OLD_D2XP`: nodeno (9 forms, none): template 081 put q + 6 into rr6, the pair of the parameter q, by OLD_D2X
  (no witness in cas). Excluding parameter pairs: nodeno, fndparm exact, checkops 21 -> 15, qmkstore 19 -> 14, nothing lost (266);
  matrix vc +0. (The full OLD_D3KC in vc: 264 -> 202; its tshape part alone changes nothing.)
- gopt, OLD_D2FA refined: linkreachdefs finds q in rr0 inside `p != q && (p->c.b2 == 8 || q->c.b2 == 8)` but compares q of
  `p != q` from memory. The lookup is now only for a name matched alone (the LTYPE case of match), and the condition rule only
  for || at the top (killdefs). linkreachdefs, asgfold exact: 269 OK, nothing lost; matrix vc +0.
- gopt, popt experiments dropped (2026-11-20): no cross jump into a later copy (insrtlive, cause 8): 267 -> 218; no branch
  reversal over a jump back to a loop head (srchdef, cause 9): 269 -> 253. Both causes stay open.
- gopt, vc `OLD_D2PP`: setlbit (9 forms, none): no pair free for the address copy; regjam took the idle pair rr6 of the
  pointer parameter v, the original gives r5 (parameter i) back and takes rr4. Unconditional: 268 (addquad, include,
  insertquad lost: the original takes the idle rr2 there); by weight against regjam's pair: 271, nothing lost; matrix vc +0.
- gopt, OLD_D3JC tried again (a call drops r4..r7 parameters from earlier jump records): prquad, cmwalk exact but tfree1,
  addstore, transfer, prvec, allocvec, tabfree, addblk, mkconstopnd lost (266). tfree1 (`if (p == 0) cerror(..); else p->..`)
  keeps p in the original although its shape is that of prquad (`if (q == 0) { printf(..); return; }`, q reloaded): the
  difference is not the call. Dropped; open.
- gopt data, vc scparse `OLD_D12R`: the 6 zero bytes of ebbo2 between two switch tables are the static return area of
  constfold (struct qa); cparse efcode put it into the bss. In the data: the ebbo2 shift is gone, matrix vc +0 (code), the real
  cas build byte-identical. init2: one more int (s4d98, unused) behind s4d96: the init2 shift is gone. checkops, notlive:
  string order by source forms. Left in the data: almap (zero/neg/done behind the switch in the source; popt).
  Note: `make gopt` does not rebuild the objects after a compiler change (rm *.o first).
- gopt, popt experiments dropped: OLD_D65 (xjump: the copy in front of the target jumps back into the copy in front of the jump,
  when the target lies behind; indupdate exact) 271 -> 227; OLD_D66 (codemove also into a label the code in front falls into,
  with a jump inserted) worse on indupdate. Cause 8 (which copy of a common tail stays) stays open.

- gopt, the parameters at labels (2026-11-20): the records of scparse show the difference of prquad and tfree1: `B25 ] L26`
  against `B27 L24`. vc `OLD_D3JB` (`]` counts as a record for d3jdead): prquad exact, nothing lost (274). Tried and dropped:
  `OLD_D3JN` as "only the newest conditional jump gives its state back" (cmwalk exact, checkops 21 -> 7, addstore and allocvec
  lost, 273) and as "a label named only by unconditional jumps drops the other records" (cmwalk and addstore exact, allocvec
  lost: `] L329 L328 ] B330 L322` keeps q in the original, cmwalk `] L56 ] B57 L51` reloads p; 274, net 0). cmwalk and
  checkops open.
- gopt, the parameters of a function with a static (2026-11-20): mkleaf, operand, qmkstore (the functions with a static in
  the body whose first statement uses a parameter) reload the parameter from the frame at the start, qmkstore even stores it
  there; propnd, identop (switch on a parameter, no static) take it from rr6. vc `OLD_D3S` (scparse writes `#static` in front of
  the first `[`, scodgen forgets the registers): mkleaf, qmkstore exact, operand 30 -> 28; 276, nothing lost.
- gopt, cause 8 (2026-11-20): setsym keeps the `return 1` of default (first in the layout) and the last case jumps to it; ours
  removes the jump of the last case to the label behind it (the dispatch moved to the front) and xjump then takes that copy.
  Experiment OLD_D67 (spopt: one comjump before the first iterate): setsym exact, 15 other functions lost (262). Dropped.
- gopt, OLD_D3S removed (2026-11-20): the entry store and reload of a parameter in mkleaf, qmkstore, operand come from dead
  code that assigns the parameter (`if (0) a = 0;` at the start; owner's idea: a leftover of code popt deleted, as tstlbit
  `i = 0;` behind the last return): codgen forgets the register and gives the parameter its frame home, popt deletes the
  dead code, the store stays. mkleaf and qmkstore exact without the switch, operand 28 as with it. The witnesses of the
  gopt switches were measured (compare/gopt/SWITCH_CANDIDATES.md).
- gopt, K7/K8 (2026-11-20, owner: look for an existing switch that fits or can be adapted; experiment, not built in):
  in every K7/K8 witness ours keeps the address of a global pointer in rr6 (`ldl rr0,rr6` later) and takes rr4 for the
  next pair, the original loads the global again and takes rr6. Existing switches: `OLD_D2N` (allocreg: a NAME loaded into a
  word register is not remembered; v0, so in vc) covers only words (`cls == 2`); `OLD_D2W6` (rr6 before rr4) then takes the
  free rr6; `OLD_D2P` (idle named pair counts as free) is limited to rr0, rr2 and lighter than the first free pair (rr6
  weighs 6, rr4 4), so it cannot reach rr6. Experiment: `OLD_D2N` also for a pair that loads a NAME (`(cls == 2 || cls == 3
  && p->in.op == NAME)`), scratch scodgen, gopt all functions: 299 -> 305 (compress, dominators, eval, joinblk, linkp,
  srchdflist exact, rmlabels 12 -> 2), no function worse. Not measured: cas (matrix check) and the other models.
- gopt, `OLD_D2NP` built into vc (2026-11-21, owner: yes): the experiment above as its own block behind OLD_D2N (allo3.c,
  `cls == 3 && p->in.op == NAME`). gopt 299 -> 305 (compress, dominators, eval, joinblk, linkp, srchdflist exact, rmlabels
  12 -> 2), no function worse; cas +0 (matrix OLD_D2NP), `MATRIX_VER=vc oldcc.sh matrix check`: as in the table.
- gopt, OLD_D2NP also for a frame auto loaded into a pair (`OREG` with rval 15, as OLD_D2N does for words; experiment
  2026-11-21): rmlabels exact, killdefs lost (`inc r1,#8; cpb @rr2` against `ldb rl2,rr2(#8)`): net 0, not taken.
  rmlabels (2: one reload of a local behind a jump) has no lone-label form either.
- `OLD_D2NP` under v0 (2026-11-21, owner's question: could it be part of OLD_D2N?): defined in the v0 block for one
  measurement (so in v0, v0p and every compiler built on them), v0 and v0p codgen rebuilt: the v0p models cparse (164 same,
  15 differ), scparse (164, 15), codgen (191, 18), scodgen (191, 19), popt (71, 2), spopt (71, 2), cpp (23, 3) and the v0
  bench (153 functions, 131 same, 221 lines) all the same as without it. So far no evidence against folding it into
  OLD_D2N; not measured: the full v0p matrix (libc and the other v0/v0p programs). Note: the cparse model is at 164 of 179
  now (code 48640 against 48896; earlier 179 of 179): to be looked at apart from this (the models needed cgram1.c, cpy.c
  and crt0.o of cpp in the source directories, made there by hand).
- The v0p models were void since 2026-11-15 (found 2026-11-21): d12model copies the headers beside the sources, and the
  stdio.h of the old programs moved to src/head/oldv7 (5ddcc5d4), so the models took the guest's stdio.h (getc, putc as
  calls: cparse 164 of 179, codgen 191 of 209, popt 71 of 73, cpp 23 of 26). The shipped builds were not touched (their
  Makefiles have the -I). d12model now copies src/head/oldv7/stdio.h when the Makefile names head/oldv7: cparse 179,
  scparse 179, codgen 209, scodgen 210, popt 73, spopt 73, cpp 26, all strictly equal. The OLD_D2NP-under-v0 measurement
  above was made with the void models and is to be repeated.
- `OLD_D2NP` folded into `OLD_D2N` (2026-11-21, owner: just do it and verify by checkall): OLD_D2N now also forgets a
  global loaded into a pair (not an auto pair: that gives rmlabels but loses killdefs, see above). Measured with the
  repaired models: cparse 179, scparse 179, codgen 209, scodgen 210, popt 73, spopt 73, cpp 26 strictly equal, v0 bench
  153/131/221 as before, gopt 305 of 321 as with OLD_D2NP; `oldcc.sh matrix check` and `MATRIX_VER=vc ... matrix check`:
  as in the table (the OLD_D2NP row of OLDCC_SWITCHES_VC.md removed); checkall (bb22a0c6): ALL OK.
- K8 against the existing switches (2026-11-21, owner): two extensions, gopt 305 > 307 of 321.
  `OLD_D2P`: for a word an idle lighter pair is split, but not a pair that holds a parameter (gopt pr: the original
  takes r0 for `down + 1`, ours split rr2). Unconditional it changed regforsym in the v0p codgen and scodgen models (DIFF 6),
  so only without `OLD_D3KC` (v0, vc), as the pair loop below it already does. `OLD_D2Q` (segmented): a value loaded through
  `rrN(rM)` with an odd index goes into the pair rM-1 when the even half is free, or when rM is the low half of a product
  pair (gopt prcgraph: `ldl rr2,rr6(r3)`, also when an argument pair is wanted). canshare 14 > 16 and setedges 12 > 15: both
  K5 (the parameter in r7 multiplied in place, the original copies it to r3 first; with K5 solved the base goes to rr0
  and no index is used). Tried without effect: vc without `OLD_D3J` (chkfamindvar), a register that holds a parameter not
  renamed when stored (rnassign case 10). Checks: models 179/179/209/210/73/73/26 strictly equal, bench 153/131/221, both
  matrix checks as in the table, checkall ALL OK.
  chkfamindvar (trace): the parameter `r` (rr6) is never on the use list of our scodgen (the frame copy at entry
  stores r5..r7 with ldm); the original keeps it in rr6 behind the && chain and takes rr4 there.
- K5 and K16 (2026-11-21), gopt 307 > 311 of 321.
  K5: `OLD_D2MO` not for a register that holds a parameter (the original copies it first: `ld r3,r7; mult rr2`); the
  test has to come before `d2ofree`, which gives the even half back as a side effect (the first form lost r6 of canshare).
  `OLD_D2P` merge of two idle words: in v0 and vc not the words of a parameter in r4..r7 (testbit; all parameter words
  broke mkconstopnd, where the original merges r2/r3 into rr2: 2 > 65). setedges, canshare, testbit exact; the v0 bench
  gains (131 > 132 same functions, 221 > 212 lines), the models stay strictly equal. Left of K5: mkconstopnd (2: the dead
  `ldl rr0,rr6` of the original), affsize (3: the copy register r3 against r1; `matw = (affdim = nal) / 16 + 1`: 13).
  K16: the reclaim of `opnd.w4 = p->in.sname - symtab` asks rbusy for r1, the low half of a result pair already given
  back, and rbusy stops ("not found on rslist"). vc `OLD_D2TC`: rbusy takes the free word instead (cas +0). With the
  original statement and the NAME case in goto form (the b2 = 4 branch keeps the tail `opnd.w0 = p->in.type; break`)
  operand is exact. Seen on the way: tcopy of a based OREG busies the index as a pair and the base as a word (base and
  index swapped; already in the quirks table as the two-argument rbusy); not the cause, not changed.
  Tried for K8 without effect: chkfamindvar with vc without `OLD_D3J`; vc without `OLD_D2W6` (gopt 311 > 208);
  setposition with the D2P pair loop taking a parameter pair in vc (306).
- The vc scodgen over 64 KiB (2026-11-21): the K5 checkall hung 4 h in the build of oldslibc errlst.c (data only). The tree
  build links the old passes with OLDPRINTF (src/Makefile stage1); the text of the vc scodgen was 0xfe00 (rounded to 256),
  K5 took it over 64 KiB: /bin/ld wrapped `sg_code` to 0 without a message and scodgen ran into zeros. The local build
  (libc-v5.a) still fitted. Owner: reduce code (a segmented build was weighed: sld can spread text over segments, but the
  NODE union of cg.h overlays 16-bit pointers with lval/rval). Reduced, the default build untouched: a helper `d2pair`
  for the six give-back-and-take sequences of allocreg, D2M/D2MO/D2S under one test of the left operand, the even and
  odd index cases of D2Q under one, the naming in getreg (D2L, D2P) by `goto blk`: 0xfd00 with K5 and D2TC. The codgen
  Makefile now stops when sg_code is 0 (owner).
- K14 and K15 (2026-11-21), gopt 311 > 312 (constfold exact). K14: entry 115 of the table (`ld U1,UR; OI AL,AR; OI UL,U1`,
  rewrite RLEFT) needs a scratch pair; OLD_D2X gave it the pair of the left operand (rr2), so U1 was r3 and `and r3,r3`
  destroyed the operand. vc `OLD_D2XP` now also asks that the registers allo takes are the result (RESC1). K15: the
  int-to-double result of a float compare went to f1 because f0 still held d1 (idle, named); the original takes f0, its
  store equals that of the other cases and spopt merges it. New vc `OLD_D4F`: an idle float register that holds a name
  is free when its number is lower than the first free one. Both +0 on cas; the vc scodgen text is 0xfe00 again.
- Room in the vc scodgen (2026-11-21): new vc `OLD_DNR`. The records F R S T of the intermediate file come only from gopt
  (wrtil2.c) with -O3 or -O5 (cc: optlev); no build of the tree calls sccvc so (cas, gopt, oldslibc use -O). Their code is
  left out of the vc build (fcmd, rcmd, scmd, tcmd, memname, findtemp, the rflag part of fixtemp); the reader stops on such
  a record with a message. sg_code 0xfe00 > 0xf800, +0 on cas, gopt 312 unchanged. (A first checkall failed: the edit left regs3.c
  without a newline after the last #endif, and the native cc stops with "system call error: stdin: Error 0".)
- K8 (2026-11-21), gopt 316 > 318 of 321 (chkfamindvar, setposition), by register traces (KDBG in getreg, rsreturn, rbusy,
  tcopy and a tree dump in order; not committed). chkfamindvar: behind `ldb rl3,@rr4` of the first compare r4 stayed busy
  for good: segmented `OLD_D2R` gave the pair rr4 back and took r4, `OLD_D2V` refused r4 ("r stays busy for the operand",
  true only for the freereg case) and nobody freed it; the second compare then took rr6, the idle parameter r. Now the
  refused half goes back (allo3.c, D2R with SEG and D2V). setposition: the second printf argument
  `symtab[p->w4].sclass` is wanted in r3; the original computes the index in r3 (`ldl rr2,p; ld r2,rr2(#4); ld r3,r2;
  mult rr2`), ours took rr0 because the parameter X stayed idle in rr2. `OLD_D3AW` (argplace) now also gives back an idle
  parameter in the registers of the argument that the argument does not use (d3awfree, order3.c). Tried on the way
  (scratch, not kept): the integer operand of a pointer sum gets an odd wish in rallo (setposition 18 > 34, the wish
  reached the index but rr0 stayed), an odd pair wish mapped to its even register (no change). Checks: models
  179/179/209/210/73/73/26 strictly equal, bench 153/132/212, both matrix checks as in the table, vc matrix OLD_D3AW and
  OLD_D2R: cas baseline 0 (rows remeasured: cas jpr is a new witness of D2R, data bytes), gopt 318, vc scodgen text 0xf900
  (OLDPRINTF link).
- gopt affsize (2026-11-21), 318 > 319 of 321. After a trace, 12 more source forms (findings.json): `matw = affdim / 16 + 1`
  fits all but the pair of the division (the original `ld r3,r2; exts rr2; div rr2`, so r2 is gone and `affdim > 225`
  compares memory; ours the free rr0). An existing switch changed (owner: no new switch while an existing one can be
  adapted): `OLD_D2M` (v0) for DIV as for MUL. Unrestricted it changed the v0p models (cparse yyparse, codgen tagfind
  `(lo + hi) / 2`: 178/178/208/209) and the bench (131/237); limited to a name found in its register (the left REG has an
  sname): models 179/179/209/210/73/73/26 strictly equal, bench 153/132/212, both matrix checks as in the table, vc matrix
  OLD_D2M: cas baseline 0 (same witnesses), gopt 319, vc scodgen text 0xf900. setposition: 7 forms without the OLD_D3AW
  extension (a lone label gives the original second argument but changes the first): the extension stays.
- gopt mkconstopnd (2026-11-21), 319 > 320 of 321. Trace: one pair request for the copy of the parameter p (`*p` stored);
  getreg (OLD_D2P) merged the idle parameter words r2/r3 (w0, t) to rr2, which weighs 2 against 3 of the free rr0. The
  original copies to rr0 first (dead: rr0 is no base; the shipped scc shows the same `ldl rr0,..; ldl rr6,rr0` pattern)
  and then to the merged rr2. The earlier guess (OLD_D3KC) was wrong: full D3KC in vc and each of its nine sites alone
  leave mkconstopnd at 2. Changed: OLD_D2P, the merge of two idle words only when no pair is free. Unlimited it changed
  the v0p models (cparse 171, codgen 202, bench 220); segmented for all words gopt fell to 271; segmented for the words of a
  parameter only: gopt 320, models and bench unchanged (nonsegmented code as before), both matrix checks as in the table,
  vc matrix OLD_D2P: cas baseline 0, vc scodgen text 0xf900.
- gopt setsymtable (2026-11-21), 320 > 321 of 321, by a source form, no compiler change. A word copy in OLD_D2AV (an
  assignment to a frame place whose value in r0 is used) made it exact, but a trace showed the same codgen state for delbucket
  `prev = t = bucket[i]`, where the original has no copy: the difference is in the source. `tq.a.w4 = (t = betatbl[..].x1,
  t);` gives the copy `ld r2,r0` with the shipped vc. The old guess (a word OLD_D2A copy, K11) is void.
- gopt almap data (2026-11-21): the original's string order needs the done block behind the switch (7 forms, notes in
  findings.json); then spopt codemove puts the done block behind the first jump to it (zero, a straight block behind the
  first conditional jump), the original behind neg (behind zero's jump). vc spopt OLD_D51 extended, part (e): no move to
  such a jump when a later jump to the same label ends a block behind an unconditional jump. First form (any jump whose label
  stands behind a CJP, more than one jump to the label): checkops 29, constfold 31; with the later jump: checkops 29; with a
  straight block (no CJP between label and jump): gopt 321 of 321, data byte-equal, popt/spopt models 73/73, matrix checks as
  in the table (the matrix measures codgen switches only; cas goes through checkall).
  checkall then failed on /bin/cas (two places of one early function: a block `ldl rr2,..; ldb rl7,@r2; jr back` that the
  original leaves at the first jump); (e) now also needs a block that does not end in a jump back in front of it: gopt
  321, popt/spopt models 73/73.
  Still /bin/cas: iputexpr and sputexpr (cas ifile.c), the merged tail L20002 of `switch (np->n_type)` named by the two
  case blocks; the later block also stands behind an unconditional jump (the switch jump). Limit: at least three jumps name
  the label (almap's done: many). The later block's label must stand behind JP or RET (d51blk). All 26 cas sources that
  build alone: the same spopt output as with the spopt of HEAD; gopt 321; popt/spopt models 73/73.
- gopt strict phase (2026-11-21): the linked /lib/gopt against the original. The masked compare of fnc.py (321 of 321) had
  hidden frame offsets, constants, globals and members: 563 bytes in about 50 functions. All but one by source forms (the
  commits "gopt strict: ..."). initopts: `fopen(strcat(strcpy(buf, argv[i]), ".S"), "r")` three times; the results of
  strcpy and strcat are stored (dead) into temporaries; the original uses one, ours two. No source form (the temporaries of
  an expression are reset only per statement, reader3.c paroff = baseoff; tried: the assignment in the if, a cast, local
  declarations of strcpy and strcat). No existing switch is about these temporaries: new vc `OLD_D14T` (local3.c gencall):
  when the result of a call goes to a temporary (cookie INTEMP), paroff goes back to the lowest temporary among its
  arguments once they are placed. The first form (saving paroff around genargs) had no effect: argprep/argplace of the outer
  call had taken the temporary before gencall; unlimited (every call) it changed main and eread (the frames of the original
  keep the temporaries there). Checks: gopt byte-identical (full rebuild), fnc.py 321, vc matrix OLD_D14T: cas +0, both
  matrix checks as in the table, models and bench unchanged (vc only), vc scodgen text 0xfa00.
- OLDCC.md cleaned of work log (2026-11-21, owner: OLDCC.md explains the switches, no log). Moved here as they stood:
  - vk: 20 forms of the statements tried, none helped.
  - vl, the measurement on the C members of libc.a and slibc.a (host/recon/poptveto/s1114/cg/libtest.sh), as it stood in
    OLDCC.md: Measured on all 206 C members of the two libraries (`host/recon/poptveto/s1114/cg/libtest.sh`): the shipped
    compilers give
    193, vl gives 204 (nlist, symscan and swap of both libraries, malloc of both libraries (calloc: `unsigned num; int
    size;`), scanf, segmon and doprnt of slibc added; the printf of both libraries is exact only with its `-I`, so all 206
    in the build; doprnt needs two source forms: `_pfile != NULL && ferror(_pfile)`, and the block `lng:` behind the cases
    `O X U D l`; segmon needs `char *mkseg()` (with `long mkseg()` D25 masks its casts), `sr[0].sr_seg` and `(i = i + 1) <=
    *nsegs`; slibc malloc also needs two forms of its source: `union store *r` declared in front of `register unsigned onw`
    in realloc, an unused `int spare` in calloc, whose frame is 2 bytes larger without a found cause, as the spare word of
    ld in D1) and changes no other member.
  - D3KC regforsym: 9 source forms tried (a cast, `+ 0L`, `?:`, `!(.. && ..)`, a `goto` label, `|`, ...): none reloads
    (2026-11-08).
  - D16A genswitch: 9 forms tried (`n = n+1`, `n += 1`, `n++` before or after, `(p+(++n))->sval`, ...) leave `inc n,#1`
    (2026-11-08).
  - D45: without the exit limit jpr has 24 lines instead of 10, nothing else changes (2026-11-13).
  - D51: jpr has 42, 42, 54 lines without (a), (b), (c).
  - D53: other conditions tried and refuted: a constant value, register variables, `OLD_D1R` (2026-11-14).
  - D61/D62: measured 2026-11-18 with a copy of the v1 codgen per variant: libm 15 of 15 C members exact.
  - Section 5 of OLDCC.md, the vl switches and those removed after a check of source forms: - Owner's exception for vl
    (2026-11-15, provisional until each is checked against source forms): `OLD_D53` (nlist),
    `OLD_D57` (swap of libc, the entry store only; its second part, the parameter of the next function not known in its
    register, was replaced by `longswap(l) long *l; p = (int *)l;`). Removed after such a check:
    `OLD_D59`, `OLD_D60` (segmon, source forms), `OLD_D56` (printf of slibc: `regs` of the size used), `OLD_D58` (calloc:
    `unsigned num; int size;`), `OLD_D55` (scanf, printf, swap: the register arguments in a structure, the bytes of `x`
    through `struct bytes`), `OLD_D54` (symscan: two nested `if` for the `&&`).
  - OLD_D6S falloc, forms tried: `new>=0?` turned round, `type`, brackets, casts, `+0`; the block of the else part stays in
    front of the dispatch.
  - D23 usable: 11 forms tried (2026-11-08).
- 2026-11-22 (increment 155): the code of OLD_D3K/OLD_D3J (d3kjump and its helpers) moved from comm3.c into reader3.c (branch() no longer calls d3kjump, its two callers do), OLD_D12K (d12k helpers, d12kdata instead of rddata) from rdr3.c into reader3.c; OLD_D24, OLD_D13, OLD_D23, OLD_D63 and the d12kout of eread stay in the shared units mip/optim.2.5 and mip/rdr.2.5 (also under PASS3). The variants built with cc differ in their bytes from their build before; models 179/179/209/210/73/73/26 strictly equal, bench 153/132/212, both matrix checks as in the table. convelim and isint of codgen are rmconv and notflt.
- 2026-11-22 OLD_D3J by the saved lists of regallo3.c (rspush, rsmatch, rsmerge, never called in the shipped codgen; guess: an
  older codgen called them at jumps and labels). Tried in v0p: at every jump a copy of the known parameters saved by
  rspush(lab), at a named label rstab1 = {lab}, rsmatch(), the merged list as the record of d3jlabel; rsstk emptied at '{'
  and the merged lists dropped (rsdepth is never decreased in the original). Models cparse 176, codgen 208, popt 72 (was
  179, 209, 73), cpp 26, bench unchanged. A trace of the parameters at each named label (both variants, the sources of
  the cparse and popt models) shows why: rsmerge is a union in which a register held by two lists is dropped from both,
  D3J (and the original) needs the intersection. cparse ecode L333 (two jumps, both with p in r7): the original keeps p,
  rsmerge drops it; popt findcon L650 (four jumps, only the first with n, t): the original forgets them (regargs stores
  rr6), rsmerge keeps them. With at most 3 saved lists and no pop, a switch fills rsstk (sym1.c 376); a label without a
  saved list must change nothing (as d3jlabel without a record). Refuted as the mechanism of OLD_D3J; the dead functions
  look like a save and restore around a stretch of code (what the code in between took is dropped), not a join at a
  label. The code was not kept (patch only in the session).
- 2026-11-22 OLD_D3K/OLD_D3J code made smaller (reader3.c), each step measured with the seven models and the bench:
  without the limits (64 records, 16 registers) no change; v0p with "named" = "has a jump record" instead of the bit set
  d3kbits no change; without d3jfree (getreg decides whether the register comes back) no change; without d3kpend
  (forget at once) cparse resetbc and codgen rspush lost; without d3jclear at a loop head cparse and codgen one function
  each lost. A first form with one table for v0 too (256 records instead of d3kbits) left the models equal, but with
  a stop at a full table the bench lost 19 functions (v0 keeps the records of backward jumps): d3kbits stays for both.
  Now d3kjump (bit; v0p: the record, the intersection), d3klabel (returns 0 for a label no jump has named; v0p: the
  records go), d3kreset, d3jmark; d3knamed, d3jsave, d3jlabel, d3jclear, d3jfree gone. Models 179/179/209/210/73/73/26
  strictly equal, bench 153/132/212, both matrix checks as in the table.
- 2026-11-22 the switches of codgen out of mip (owner: other logic for the same aim). OLD_D12K: d12kout no longer in eread
  (rdr 2.5), a fwalk behind eread (reader3.c d12kname) puts out the constant of each NAME of a waiting label. OLD_D24
  (both parts), OLD_D63, OLD_D13: no longer in optim and rmconv (optim 2.5); reader3.c optpre (in front of optim: the
  swap of OLD_D13, the conversions the older rmconv keeps become PCONV) and optpost (behind optim: SCONV again, the mask
  of OLD_D24 dropped). Each step: models 179/179/209/210/73/73/26 strictly equal, bench 153/132/212, both matrix
  checks as in the table, checkall ALL OK (the vk, vc and v1 programs). OLD_D23 stays in optim 2.5.
- 2026-11-22 mip text made closer (no change of a binary: codgen, scodgen, cparse, scparse, gopt stripped equal to the
  originals, checkall ALL OK). common 2.5: cunused[NCUNUSED] (cg.h 9, mfile1 8), lastfree, dope and opst for both passes
  (codgen keeps its definitions in bss3.c too: they are commons, and bss3.c sets their place; without lastfree there
  the bss moved, 43 bytes of operands). rdr 2.4/2.5 (src and host): the externs of 2.4 in the wrapper rdr2.c, 2.5
  written as 2.4 where the code is the same (decimal constants, symtab + 3, the locals k, t, x, the table in the lines
  of 2.4, the test of rdin2 without goto, braces, comments); host: the stale OLD_D12K code out of rdr.2.5, its readers
  of the big endian symbol file (rd16, rd32, getw16, symdecode, now with rdsyms22 as gopt) in the wrapper rdr3.c.
  Not the same code, so kept: dst = (int *)((char *)numtab + (base << 2)) (as &numtab[base] all of codgen moves) and no
  int x in eread (the frame). diff rdr.2.4 rdr.2.5: 357 lines before, 259 now; what is left are the changes 2.4 > 2.5.
- 2026-11-22 corpus store: a stored result of the original names the temporary file of the case (cc.18279.p); a
  candidate run on a new preparation names another (cc.9972.p), so every case of codgen [host] differed after a change
  of the host codgen (0 of 427); with CORPUS_NOCACHE=1 427 of 427, then with the store too. Not changed (checkall tool).
- 2026-11-22 codgen: one header mfile3 (owner: as mfile1 of cparse and System III; the name after the files ...3.c,
  gopt's is mfile2.h by the SCCS line in /lib/gopt): cg.h and cg2.h in it, the three anonymous structures of bss3.c
  named (parment, tagent, rssave), and the 183 declarations that more than one module has; the 137 that one module has
  stay there, and the 53 of functions returning int went (as System III). All 320 in mfile3: cparse v0p, which builds
  the shipped codgen, stops in local3.c ("malloc: Not enough core", then "Symbol table out of space"); the five that
  common 2.5 defines as int (branch, ncopy, tfree, uerror, walkf) are not declared. codgen, scodgen stripped equal,
  models 179/179/209/210/73/73/26, bench 153/132/212, both matrix checks as in the table, host corpora 427/427,
  checkall ALL OK (the old variants are built by cc with it). The host port the same (cgproto.h stays, mfile3 includes it).
- 2026-11-22 rdr 2.5: the unused table tnames replaced by the three tables of 2.4 (named tbtype, tmod, tclass in both
  units: codgen has a function ttype); codgen, scodgen stripped equal. gopt: the header is mfile2.h (the name of the
  original: its SCCS line in each module of /lib/gopt), gopt.h went; 119 names that two or more modules declare in the
  same form are in it (one per line), a module keeps what only it declares or what it sees in a form of its own (qhead,
  betatbl, fconsttbl, numtab ...); functions returning int are not declared, nor tprint, ncopy, fwalk, tfree (int in
  common 2.4). gopt stripped equal; host the same (82 names). Models, bench, matrix checks, host corpora as before;
  checkall: all but corpus gopt [host], the stored result of the original with another name of the temporary file (see
  above), 854 of 854 with CORPUS_NOCACHE=1. optim 2.5: the start of the pointer case of PLUS once for both passes (the
  tokens after the preprocessor are the same for every pass and switch set).
- 2026-11-22 optim 2.5, PASS3 blocks made one text for both passes where codgen and scodgen stay stripped equal (cparse:
  the same tokens after the preprocessor): the locals of optim (int i; NODE *sp; TWORD t: codgen uses neither x nor i,
  sp keeps its place; econvert stays under # ifndef PASS3), conval PLUS of SEG (break, not return 1), the division
  and the rest by 0 (if/else of cparse), the case ASG DIV/MOD (the nested if of cparse; zapright and makety of cparse
  stay under # ifndef PASS3), rmconv (the text of cparse; the goto form of codgen went). Not the same code, so kept:
  conval MINUS of SEG (the difference of pointer constants; the form of cparse moves 27 bytes of scodgen). 20 > 17
  lines with PASS3. Host the same, and its stale OLD_D24, OLD_D13 blocks went. Models, bench, matrix checks, host
  corpora 427/427, 452/452, checkall ALL OK.
- 2026-11-22 the headers mfile2.h (gopt) and mfile3 (codgen) side by side: what both had is out of them. The extern
  dope and opst (manifest has them, as in System III); STAR, UMINUS, UAND, UCALL, FCALL, UFCALL (the names of gopt and
  codgen for UNARY MUL ... FORTCALL, the same numbers) into manifest; struct sym and MAXASCII (the symbol table file,
  which only gopt and codgen read, with rdr) into the new mip/rdr.h (name and cut a guess: neither binary has an SCCS
  line of a second header; the place of the SCCS line inside mfile2.h is not measurable, nothing above it makes data;
  in each module of /lib/gopt the module's own SCCS line comes first, then that of mfile2.h). stype is unsigned for
  both now (codgen had int): codgen, scodgen, gopt stripped equal; host codgen reads stype without sign as host gopt.
  Models, bench, matrix checks, host corpora 427/427, 854/854, checkall ALL OK. NODE stays in each header (other layout).
- 2026-11-22 gopt: the 64 globals and locals named by their address have names (src, host, compare/gopt, the documents;
  no source of the optimizer is known: ZEUS of Zilog, whose renamed copy WEGA is, is not in TUHS, System III/V and
  contrib/lcc have only the peephole optimizer c2). From the option letters (pcc style Xdebug): -b bdebug (ebbo), -d
  ddebug (dom), -l ldebug (live), -p pdebug (loops), -f fdebug (flow), -r rdebug (reg), -s sdebug (the statistics),
  -Z zflag, -1..-5 olevel. From the messages: nlabrm, njmprm, ndeadrm (number of labels / jumps / dead blocks
  removed), nqmoved, nstemps, nltemps, nivelim (the loop statistics), stores (stores array overflow), bblockno
  (assignreg), lpdefs, flpdefs (deltmplpdefs), exitlive (liveonexit), startblk. From the use (guessed): maxtmp, nbeta,
  nbeta0, hascall, baseoff, aborted, lvfirst, dffirst, lvmark, nkill, nuse, nstores, vnlist, vnnext, ebbcalls, curblk,
  phquad, ivagain, invset, copyins, invfound, invagain, hdrdtree, invdefs, freend, regused, ncolors, adjmat, nnodes,
  matwords, gnodes, cgraph, nexttmp, and as codgen reads the record { : nlabsave, frame0, maxoff, nparm; the locals of
  srchdflist isstore, isbeta, ismem, isother; unusedflag (no code uses it). gopt stripped equal.
- 2026-11-22 OLD_D13 (vc) back in optim 2.5 (under PASS3 and SEG). Moved in front of optim (b4649980) the vc scodgen
  jams on cas ssymtab.c:48 ("register jam"); behind optim too: the rules of PLUS behind the swap (the constants) work on
  the swapped tree. Not seen for hours: checkall builds /bin/cas with the shipped scc, and `matrix check` measures
  nothing; a single vc matrix run stopped with "baseline model failed". With the swap in optim the cas model of vc is
  same 152, differ 0. OLD_D24 (cas witnesses: the same model) and OLD_D63 (libcisam of v1c, in checkall) stay out of mip.
- 2026-11-22, gopt HANDOFF step 10 (vc switches), `OLD_D3PL` (tabfree) is gone. Without it `s2 = ((long)p >> 24) & 0x7f`
  loads `ldl rr2,|_stkseg+~L1|(fp)` instead of the pair rr4. Source forms (`(long)(char *)p`, `(unsigned long)p`, `(int)(...)`,
  a long local, `register char *p`, `long p`): none. Cause: scparse clocal paints `(long)p` onto the NAME (same size, the
  target no pointer), so regforsym sees type long; keeping the SCONV in scparse (with a test in lcl3.c) stops codgen with
  "expression causes compiler loop" (no template for it), so the original painted it too. The segmented D3PWORD now has
  the types of the size of a pointer (pointer, long, unsigned long) and D3PCAST asks for a pointer on one side: without
  that, cas iputl/sputl (`(unsigned long)l` of a long parameter, from the frame in the original) are lost (149 of 152).
  gopt byte-identical, cas model of vc 152. The same "one side a pointer" in v0: cparse and scparse 178 (sputc, `putc` casts
  the int c to unsigned), so v0 keeps any two of int, unsigned, pointer.
- 2026-11-22, step 10, `OLD_D4F` (constfold): without it the shared tail of the float compares (`dr = d1 == d2`, ...: tcc,
  exts, fldil) takes f1 (f0 holds d1) and is no longer merged by spopt. Forms of case 19: `(d1 == d2)`, `(double)(d1 == d2)`,
  `? 1.0 : 0.0`, `? 1 : 0`, through the int res, `d2 == d1`. None (each other form changes the case itself); the switch stays.
- 2026-11-22, `OLD_D4F` from the compiler side: it is the float form of v0 OLD_D2L (an idle register holding a name is
  free when it is the lower one). Tried instead: rfree gives a float register back to rsfree at once (no float register
  keeps a name): codgen stops on gopt ebbo2.c:571, "rbusy: register to be made busy not found on rslist" (the result
  register is made busy again behind rfree). Refuted.
  Then: vc is v0p and so v0, OLD_D2L and OLD_D2P are on in vc already (an idle register holding a name is free when it
  comes before the first free one: words, pairs). OLD_D4F is that rule for the float registers: its code under OLD_D2L
  and the switch gone: gopt byte-identical, cas model of vc 152, the seven models and the bench unchanged (v0, v0p).
- 2026-11-22, step 10, `OLD_D14T` from the compiler side: the temporaries of a statement grow as a stack (paroff, reset at
  each `.` record). Without the INTEMP condition (the temporaries of the arguments free behind every call): gopt rdr2.c and
  reader2.c (the getc/putc pattern: a call whose result is no temporary has a temporary argument; the original puts the next
  temporary above it) lose 8 lines each; cas 152. The switch is the smallest rule found: the result of a call takes the
  place of its arguments only when it goes to a temporary itself. Stays.
- 2026-11-22, step 10, `OLD_D2AV` gone. Measured in vc: a pointer in any pair with INTAREG adds copies behind
  `(v = checkalias(..)) == 0` (value in rr2; dom2, ebbo2, lpinv2, cas stlook); a pointer in rr0 without INTAREG adds copies
  in chains (`a = b = x->f`: dom2, lpind2, lpinv2). So vc needs pointer, rr0 (no base register segmented) and INTAREG.
  INTAREG in the v0p rule (a long, any pair): the seven models and the bench unchanged. One rule now: the value of an
  assignment wanted in a register for a test; the pair under `#ifdef SEG` (D2APAIR): vc a pointer in rr0, else a long.
- 2026-11-22, step 10, `OLD_D2QO` / `OLD_D2O`: the segmented D2O also takes an odd wanted word (as D2QO for D2Q). The same
  for v0p (the condition under OLD_D2QO instead of SEG): cparse and scparse 178 (yyparse, 3 lines). Refuted, it stays
  segmented.
- 2026-11-22, step 10, `OLD_D3JB` gone: v0p did not count the record `]` behind a jump (d3jdead), vc did. Counting it in
  v0 and v0p too: the seven models and the bench unchanged; the exception was never measured. Now every record counts.
- 2026-11-22, step 10, `OLD_D3JL` gone: "only a long keeps its home at the label, not a pointer" is always true in
  nonsegmented code (a pair never holds a pointer there), so the vc test is the rule of all versions (reader3.c d3jlabel).
- 2026-11-22, step 10, `OLD_D2XP` as part of OLD_D2X in v0 and v0p: both conditions (not a parameter pair, result RESC1):
  cparse, scparse, codgen, scodgen lose 3 each (ispow2, yylex, defalign, zzzcode, tshape); RESC1 alone the same three.
  The RESC1 condition is a difference of vc; the switch stays.
- 2026-11-22, step 10, `OLD_D2PP` gone: its code (no pair free: an idle odd parameter word given back before regjam) under
  OLD_D2PO (v0): the seven models and the bench unchanged (the v0p codgen changes). D2PO now has the parameter words in its
  fail path.
- 2026-11-22, step 10, `OLD_D2K` gone: its code under OLD_D2C (v0: a name found in a scratch register is lost when the tree
  takes the register over; a store through it is such a use). First try: v0p codgen ffld lost (`ld r2,~L1(fp)` for `ld r2,r4`),
  the bench 212 -> 204 lines: d2kforget cleared reg + 1 too, right for a segmented address pair, wrong for a word address (r3
  made r4 forget the parameter). With reg + 1 only under SEG: the seven models and the bench unchanged; vc as before.
- 2026-11-22, step 10, `OLD_D2S` as the D2MO rule for SCONV (vc): any odd register not a parameter into the pair below:
  cas 149 (symtype itself lost: r3 there is a parameter); any odd register: 149 (locadv, opshort, asmins). Reading (a guess):
  the operand of the widening is free before the pair is chosen, and the lightest pair wins (rweight rr2 2), which differs
  from the shipped choice only for an operand in r3. Not part of another rule; stays.
- 2026-11-22, step 10, `OLD_D24L` gone: the unmasked lda of a frame place under OLD_D24 (the address masks of the segmented
  old versions: v1, vk, vl, vc). checkall ALL OK (kernel by vk, the library members by v1 and vl byte-identical); vc unchanged.
- 2026-11-22, step 10, `OLD_D2FA` exception (3) (no lookup in a condition with || at the top): without it gopt killdefs
  (live2.c) and cas yyparse (3 lines) are lost. In killdefs the original takes rr0, which holds q, over for `q->c.b2`
  (`inc r1,#8; ldl rr2,rr0; cpb @rr2,#4`) and reloads q later, as OLD_D2C does for a word. Tried: D2C also for a segmented
  automatic pointer pair (STAREG, not a parameter) instead of exception (3): worse (live2 14, lpinv2 8, wrtil2 6 lines).
  Exception (3) stays.
- 2026-11-22, step 10, `OLD_D2FA` exception (2) (an assigned variable not looked up from the start of its statement):
  without it gopt live2.c 164 lines (initlive `q = q->next`: the original reloads q); the clearing at the store (match3.c)
  comes too late for the right side. Stays.
- 2026-11-22, `OLD_D2S` in a natural form instead of `lr == 3` (vc, cas model): the pair of an odd operand when it weighs no
  more than the first free pair: 150 (opshort: a byte widened, the original takes a new pair; asmins); words only: 151
  (asmins: r1 widened into rr2, the lightest, which stood as two free words); the operand given back first, then any pair
  (d2pair(lr, -1)), words only: 151 (locadv: r7 back, getreg takes rr6, the original rr2). Not yet; the switch stays as it is.
- 2026-11-22, `OLD_D14T` as "paroff remembered before the arguments, restored behind the call": not possible in gencall, the
  temporaries of the arguments (the result of strcpy) are made before it (the store pass of the statement); the minimum
  over the argument temporaries (d14tmin) is the direct form. Stays.
- 2026-11-22, `OLD_D2S` rewritten without the special case `lr == 3`: the lighter of the pair getreg gives and the pair of
  the operand (odd word, even half free or idle; not a byte). The form "operand back, then any pair" had lost locadv: it gave
  the parameter r7 back, the original keeps it and widens into rr2. cas model of vc 152, gopt byte-identical, matrix row as
  before (cas symtype). The same rule in v0p (under OLD_D2M): cparse, scparse lose genswitch and refid, so it stays a vc switch.
- 2026-11-22, step 10, spopt `OLD_D20S` against v1 `OLD_D20`: D20S with the bound of D20 (refc <= 3, straight block): cas
  149 (opshort, opseg, opoff); D20 of v1 with the straight-block condition of D20S: checkall fails on libcisam, slibcisam
  (v1c, the v1 popt). The two do not merge; D20S stays.
- 2026-11-22, step 10, spopt `OLD_D45` as "codemove takes the jp before the conditional jump is turned" (in iterate): spopt
  does not end (iterate runs until nothing changes, codemove moves again in every round). D45 also asks more than codemove
  can (the block straight, ending in a jump whose target stands behind another jump, not the exit code). Stays.
- 2026-11-22, step 10, spopt `OLD_D50` looked at again: its core (the scan of the loop inversion follows an unconditional jump,
  as xjump does elsewhere) reads as written code; its limits come from the one witness (cas jpr). No other mechanism found. Stays.
- 2026-11-22, step 10, the goto forms `if (0) { L: ...; continue; }` at a loop head (gopt aliasm, initaffect, buildgraph,
  chklpdefs): in /lib/gopt the block of `if (c) { ...; continue; }` stands behind the entry jump an inversion leaves at the
  old loop head (codgen puts the for test at the top; spopt turns the loop). A test rule in spopt iterate (vc, beside D45):
  `cjp c,L1; S; jp X; L1:` with S ending in a jump, S moved behind the preceding jump and the cjp turned to it, when that
  jump is the entry jump of an inverted loop (it jumps forward to the test, which ends in the cjp back to the label behind
  it), X is the continue label (named twice or more, straight code up to the test), the code at L1 does not start with an
  unconditional jump nor fall into X (the other arm of an if/else or ?:), and every label inside S is named only from S.
  Measured: cas model 152, the gopt sources of today byte-identical, the natural initaffect exact. Not yet: gopt prcgraph
  (`if (testbit(..)) putc('1'); else putc('0');` at the end of a loop body) and lpinv are moved too: after refcount (every
  jump to the last label of a label run) an if/else at the end of the body looks like `if (c) { A; continue; } B`; without
  that retarget in vc cas loses symreloc and jpr. Only in round 1 of iterate: no help (the retarget is before round 0).
  The patch is kept aside, not in the tree.
- 2026-11-22, the spopt of vc against c2 of System III (pdp11/c20.c: iterate, xjump, codemove and the loop inversion as in
  our popt; differences: window 16 instead of 24, codemove refuses when the block end is the jump or next to it, JBR/JMP):
  vc spopt without D45, D20S, D50, D51: cas 149 (the three witnesses exprreloc, stabout, jpr); the same with the c2 forms:
  141 (yylex, main, symreloc, stlook, ...). The spopt of vc is not the old c2.
  The two c2 forms apart (without D45, D20S, D50, D51): the codemove refusal next to the jump alone: cas 149 as without it
  (neutral); the window 16 alone causes the eight more losses (yylex, main, symreloc, stlook, opadd, jdist, lstline; lstrest
  only by the addresses). stlook is a natural `for (;;)` with an if/else: with 16 the loop is not turned, the original turns
  it (the conditional jump the inversion looks for is more than 16 nodes behind the head) and puts the then block
  (`ldl rr8,rr10; ld r2,rr8(#14); jpr L20014`) behind the entry jump, as in the gopt goto cases. So the window is 24, not a
  question of the source form; and the shipped inversion makes that layout when the conditional jump stands behind the block.
- 2026-11-22, the loop-head goto forms traced (popt with TRACE on the codgen output of the natural initaffect): our spopt
  turns the loop at the head test only; the original also exchanged `[body start .. jpr ne,L523]` with `[S; jpr L519]` and
  turned the cjp (label L523 now at S): an inversion around the continue jump, whose scan finds the cjp naming the label
  behind the jump only after following the conditional jump back of the loop. As a D50 extension (the scan follows that
  jump once, for a forward jump p; S goes in front of the body start): cas 132; only for forward jumps: 144 (yylex, main,
  pstcmp, hash, jiter, ...). hash (`if (p != &k) p = &k; else ...` in a while loop) is a natural if/else whose then arm ends
  in a jump to the join, straight on to the test: in the spopt input the same as `if (c) { S; continue; }`, and the original
  does not move it. So the input of spopt does not tell the two apart; the patch is aside (tmp/tmp/d50follow.patch).

## 2026-11-22 gopt: the handoff closed (what of compare/gopt/HANDOFF.md still matters)

- /lib/gopt byte-identical after the link (2026-11-21): library start-up code, five library members in segment 4, about
  50 functions found by the strict compare of the linked file (host/recon/framemap.py: frame layout, chain and statement
  order, constants, globals, members, tails); vc OLD_D14T for initopts. The switches gopt needs: compare/gopt/COMPILER.md.
- `make shipped` gopt (step 6): its output equals /lib/gopt's on the 18 gopt sources, cc and scc, -O1..-O5 (beta2.c
  -O4/-O5 stops with "illegal bsize type" in both). `#ifdef SHIPPED`: dominators() (a bug of the shipped scodgen),
  flow_graph() (the BBLOCK operand kinds the original leaves unset), alloclpdef() (the rr2 of the original, no effect
  measured); KNOWLEDGE.md, quirks. Method: the passes by hand (cpp, cparse -E b, gopt -3 FLAGS -E b) with the debug flags
  (d dominators, l live, p loops, r registers, y all), both outputs diffed with the addresses masked. A mix of vc and
  shipped objects does not run (other stack conventions); an instrumented vc build overflows segment 4.
- The gopt corpus (step 8, compare/gopt/corpus.py, the slowest corpus) found: the shipped build prints the rval of a
  unary node from its own layout (SHIPPED: -30720 as the original); host: %ld of the most negative long printed 0; the
  64 KiB of an area (order3.c: out of space); r5 through checkalias; segment 0 for the vectors a preheader record lacks.
- Step 11 (names instead of numbers, then the PCC sweep of codgen and mip; SysIII the base, Plexus the second):
  measured on the way: in WEGA FLOFLG is 01000 and LTYFLG 0400 (the float operators + - * / carry 01000 as FLOFLG in
  System III; 0400 only + -), so OPFLOAT is FLOFLG as in System III match.c; SPECIAL|2 and SPECIAL|4 are SSCON and SICON
  (the strings of prcook); the quad ops of gopt are the names of qtab with Q (the strings say QSTORE, QIDR); callreg
  returns a register (F0 or R2), not a cookie; freereg's `cookie & NASL | NBSL` is always true (quirks table). A central
  unsigned DECREF changed one instruction of /lib/codgen and /lib/scodgen (fbegin shifts an int arithmetically): found by
  checkall after the baseline of the variant compare had been rebuilt; now UDECREF where the unsigned form was. Host
  builds of codgen and scodgen differ from the earlier ones by gcc code choice only (checkall corpora equal).
- Step 12 (switch consolidation) closed without a broad change (owner 2026-11-22): merging switches that are always set
  together only removes names and takes away the finer configuration an unknown version may need; rules behind
  several switches are looked for when a program needs them, as in step 10. Done: OLD_D3KC stands in the v0p block
  under `#ifndef OLD_VC` (no `#undef` behind it). The inventory (82 switch entries) for later versions:
  always set together: codgen D61 D62 D63 (v1, v1c, sv1, sv1c); D3W D2PW D3Q D3H D2CT (v1c); popt D45 D49 D50 D51 D64 D20S
  (svc); cparse D4R D6C D6D (v0, v0p, svc); 22 codgen switches in v0+v0p+vc, 16 in v0p+vc, 7 only in vc.
  Without an effect in the vc matrix (cas, gopt), set in vc only because vc is v0p plus switches: D2G, D2MO, D2X, D3L,
  D5A, D5S, D5T, D6R, D12K, D16A, D23; D18 shows 0 in both matrices (progcfg treats D18 as equal).
  Families of a switch and its refinements: D2P/D2PO/D2PT/D2PW, D2Q/D2QO/D2QF, D2L/D2G/D2T/D2TC, D2R/D2V, D2M/D2MO,
  D2X/D2XP, D2C/D2CT/D2CW, D3K/D3KC. In several passes: D6T, D22 (codgen, cparse), D13 (codgen, cparse, popt).
- mkstr (2026-11-22): the compiler of mkstr (crt0 3.1 and mkstr.c 1.1 of 11/01/82) reloads a parameter from its frame home
  where ccv0 and ccv0p take it from r6: main `argc--, progname = *argv++` gives `ld r2,%0000(r15)` (ccv0: `ld r2,r6`), libc
  calloc `num *= size` gives `mult rr2,%0000(r15)` (ccv0: `mult rr2,r6`). Both are the second parameter, stored by `ldl
  0(fp),rr6` at the entry. Tried: 6 forms of the statement in main, 20 forms (5 declarations x 4 products) in calloc;
  none reloads it. Everything else of mkstr is exact with ccv0 (8 functions, all libc members). Not a switch (phase A).
- Source form `(char)(a == b)` (2026-11-23): with ccv0 and ccv1 the original's `tcc; extsb r2` before a word store comes from a
  comparison cast to char; tail `fromend = (char)(*arg=='-')` (ccv0), ctime tzset `sign = (char)(*p == '-')` and `daylight =
  (char)(*p != '\0')` (ccv1, oldlibc ctime 0_1). Without the cast no extsb.
- OLD_D5SA (2026-11-23, phase B of mkdir): mkdir `strcpy(pname+slash, ".")`: the original `lda r2,%0002(r13); add r2,r15; ld r7,r2`,
  ccv0 `lda r7,%0002(r15); add r7,r13`. The tree at mapregs: PLUS(PLUS(REG fp, ICON with the frame symbol), REG r13) (a KDBG trace;
  the ICON carries sname, so no test of !sname). New v0 switch OLD_D5SA in mapregs (case PLUS): PLUS(PLUS(fp, k), x) with x not
  constant becomes PLUS(PLUS(x, k), fp). The r2 and the copy come from the rallo rule of OLD_D5T (no preference); OLD_D5T moved from
  the v0p block to v0 (v0p inherits it; its witness popt procopt stays). Measured in a copy of OLDROOT: mkdir byte-identical; cat,
  echo, chmod, pwd, kill, sleep, tail, env, nohup, gets, clist stay identical; models cparse scparse codgen scodgen popt spopt cpp
  strict (179 179 209 210 73 73 26); bench v0 133 -> 134 functions, 209 -> 207 lines (the gain is mkdir itself). Witness search
  in the originals (lda rX,k(rY); add rX,r15 not followed by a use through rX): mkdir, pwck, tabs, vls, upkeep; tabs filetab is
  exact without the switch, the others have no source yet. One witness: owner's exception.
- 2026-11-23 the owner: extend an existing switch before a new one. OLD_D5SA folded into OLD_D5S (its address form, mapregs).
  For find scomp a char-parameter rule in OLD_D3J (d3kjump keeps no char parameter, so it is forgotten at the label and gets its
  home at the entry: `ld %0000(r15),r5`, the reload `cpb %0001(r15),#%2D`) makes find 39 of 39 but breaks rmconv (mip/optim.2.5:
  `char c`, `if (c == 'r') ... else if (c == 'l')`, the original keeps c in r6) in cparse, scparse, codgen, scodgen: refuted as a
  general rule, taken back. Differences seen: scomp's s is the third parameter (r5), declared `register char`; rmconv's c the
  second (r6), `char`. Open.
- find scomp solved by a source form (2026-11-23): an unused label in front of the second `if(s == '-')` gives the original's
  home store and reload of the char parameter with the v0p as it is (OLD_D3K: at a label no jump names a parameter is forgotten).
  The compiler rules measured before (char parameter, r5, loop order, busy, byte register) are not needed and not built.
- mkstr solved by a source form (2026-11-23): the reload of argv in main and of size in libc calloc (the entry of 2026-11-22
  took them for a compiler older than ccv0) come from an unused label in front of the statement, as in find scomp (OLD_D3K: a
  parameter is forgotten at a label no jump names). ccv0 as it is; oldlibc calloc 0_1 is C again (0_1.c), no .s.
- 2026-11-23 OLD_D2Q from v0p into v0, with OLD_D2QI (v0 only: the odd half of the pair may be idle, a parameter no longer
  used, and is given back as in d2ofree of OLD_D2O). Witnesses: chkout (07/82) readchk `ldl rr2,@r2` and getat `ldl rr4,4(r4)`
  (the same common.c 1.3 is exact in chkin with the shipped cc, so a compiler trait), libc ctime 0_2 (gmtime, localtime).
  The 2026-10 entry measured D2Q in v0 on the bench as 1080 > 1085 lines; now bench v0 134 > 135 same, 207 > 197 lines.
  Models of the passes (v0p) all exact, matrix OLD_D2QI +0. ctime 0_2 stays cas: gmtime/ct_numb reload parameters, asctime
  multiplies in rr2 (ours rr0).

## 2026-11-23 v0a: v0 with OLD_D3AW, the compiler of ex

- Built as its own version (`ccv0a`, `/lib/v0a/codgen`; cparse and popt of v0) after the measurement in
  `OLDCC_PARAMREGS.md` (log 2026-11-23): ex 323 -> 339 of 391 functions exact, none lost; chkout 15 -> 10 of 18, dc,
  ranlib and the v0 modules of libc-v1.a lose with the switch. The libc members inside /bin/ex (isatty, stty, gtty) are
  exact only without it. Owner decision 2026-11-23: a version of its own for ex (evidence rule 4).
- The other variant measured (every parameter register forgotten at the start of each statement): ex 283 exact, 53 lost; refuted.
- Date line of OLD_D3AW: own code compiled by ccv0 and ccv0a, functions against the originals (sources: the tree,
  System III, for xstr Plexus). Against the switch: cron 07/23, join and ptx 07/24, banner and sdiff 07/28/82 (one exact
  function lost each). For it: xstr 11/01/82 (hashit, found exact only with it; xgetc reloads `file` for `_filbuf`).
  No place: echo 08/12, cat 10/29, mkstr, at, expand 11/01/82, env, cut, mesg, pwck, tty, cal, col, cb. grpck 07/28
  (`ld r6,@r6` for argv[1]: v0 copies the parameter first, v0a reloads it; the original does neither) is an open form of v0.
  Not compiled: pcat, unpack, file, exrecover, apropos, whatis, getNAME. Between 07/28/82 and 10/28/82 no program has a place.
- `OLD_D3AC`: the form T1 of `OLDCC_PARAMREGS.md`. ex 339 -> 340
  (tailspec); the ccv0 units of the tree (162 functions) and the bench (135 of 153) unchanged. In the v0 block first: v0p
  lost cparse/scparse sputc and popt/spopt findbyte, findword, findlong (the parameter from its register), so it is set for
  v0 without v0p. The seven v0p models strictly equal again, matrix check as in the table.
  Then moved to v0a (owner 2026-11-23): its only witness is ex, the v0 programs have no such place, and in v0 it needed the
  exception for v0p; v0 and v0p codgen as before.
- The 14 oldlibc modules kept as cas sources, compiled from every C source at hand (NAME/0_n.c, ../libc) by ccv0 and
  ccv0a: none exact; v0 and v0a give the same code except ctime `_ctime` (v0a one instruction off). Closest: doprnt 0_5
  from 0_5.c (_align the entry store of stkap, _doprnt 6), 0_3 and 0_7 the same _align; malloc 0_3 from malloc.c (_malloc 4).
- `OLD_D3K2` (2026-11-23, owner: build it in v0): found by measuring the originals instead of guessing. A scan of the
  binaries (scratch preload.py: the reads of a parameter's frame home while its register still holds it) compared with
  our build: in all of ex only vdellin and vreplace differ, both the third parameter r5 behind a label; `_align` stores
  r5 although it takes it from r5 in an else branch. Over the 81 binaries of 1982 r5 is almost never taken from its
  register behind a label (only the `_align` of the libc printf). Tried first as "only rr6 stays" (X5): ex 342, bench 137,
  doprnt `_align` exact, but libc fseek 0_2 (a long second parameter) 5 -> 8 instructions; "the first two parameters"
  (X6) makes fseek 0_2 exact and is equal elsewhere. Needs the restore of `OLD_D3J` behind a jump (else branch of `_align`),
  so v0 gets `OLD_D3J` too. Earlier forms refuted on the way: forgetting after every statement, after || at once (V1..V4),
  r5 forgotten also behind a conditional jump (chkout apply). v0p: codgen notoff (fourth parameter cp, an unused pointer,
  not stored by the original) differs with it, so v0 and v0a only. fseek 0_2 now built from C by ccv0.
- `OLD_D19P` (2026-11-23, cparse v0 and v0p): the ex cluster "order of additive chains" (vrep, vremote, save, vup, vzop,
  viin, operate, vishft) read as one rule: every place is a chain on a pointer, and the original adds the pointer last
  after combining the integer terms. First built in optim (mip/optim.2.5), then moved to clocal of cparse (local1.c, when
  buildtree makes the node; owner: no switches in mip): the same output on ex, the bench, the tree units and the v0p models. ex 342 -> 348, the other
  two (operate, vishft) only call/calr and jp/jr; bench 137 -> 139 (188 lines); tree units and v0p models unchanged.
  Searched the pcc sources of the unix history archive for it: not there.
- `OLD_D8L` (2026-11-23, cparse v0, v0p, v1): the ex cluster "char compare" (edge, gettmode, advance, vgetline: extsb behind
  the tcc of a byte compare used as a value). Count in the binaries of 1982: cpb + tcc as a value has the extsb 52 times
  (v0) and 22 times (v0a), twice not (apropos, whatis); cp, cpl never. Our tree reached it only with forced casts
  `(char)(...)` (tail, endopen 0_3, 0_5, ctime 0_1). clocal of the shipped cparse sets the char type only for a non-logop
  inside a branch for logops (dead); without that condition: ex 348 -> 352, the casts removed and every module exact with
  v0p (endopen 0_3) and v1 (endopen 0_5, 0_1, ctime 0_1); tree units, bench, v0p models unchanged.
- `OLD_D8L` also in vc (scparse of sccvc, 2026-11-23): the cast `(char)(q->c.b2 == OABTEMP)` of gopt lpinv2.c srchdflist
  removed; `MATRIX_VER=vc oldcc.sh matrix cparse:OLD_D8L`: baseline cas and gopt 0 differing functions, without the
  switch gopt srchdflist lost (+57 data bytes).
- `OLD_D2NS` (2026-11-23, codgen v0a): the ex cluster "register knowledge" (vopen, YANKreg, YANKline: a global just stored
  from r2 is read from memory in the next statement). In rnassign a word stored to a NAME no longer names its register:
  ex 352 -> 355, none lost. With v0: tree units and v0p models unchanged, bench same 139 but 188 -> 191 lines (pack
  packfile: rr0 for rr2 at one more place, the pair choice the function already gets wrong): v0a only. The variant "keeps
  its name for the register choice, not found by symreg" (RSXN): ex 327 (-28), bench 137: refuted.
- 2026-11-23 chkout (07/23/82) exact with two v0 switches, one witness each (owner: commit with one witness, as OLD_D3AC):
  - `OLD_D2LP` (codgen, getreg, under OLD_D2L): wfprintf copies its three stack arguments through r0; ccv0 took the
    parameter's r2 (lighter than r0) and reloaded it. ccv0, ccv0a, ccv0p take r2, ccv1 r0/r1. First in v0 with vc too:
    gopt lost one byte (`ld r0,r3` for `ld r2,r3`), so not in vc. Bench 139/188, the seven models, ex 651 of 684 found
    (masked search of each function), checkall: unchanged. ctime 0_2 (asctime r1/rr0 for r3/rr2) the same with and without.
  - `OLD_D19K` (cparse): setnames `&chkname[n + 1] - p`. The tree of cparse is SUBSR(&chkname, n + 1) under
    a pointer-to-int conversion; the paint to int makes the + an int + in codgen, where optim swaps the address constant
    to the right (`ld r2,n; add r2,#_chkname+1`). Kept a pointer, optim folds the 1 into the constant on the left
    (`ld r2,#_chkname+1; add r2,n`, the original). Tried before: the vc rule of mip optim (OLD_D13, SEG) for
    nonsegmented code: no change (the + is no pointer +); cparse without OLD_D19P: no change. First in clocal for every
    pointer-to-int conversion: dc did not compile (`(int)p->beg & 01`: operands of & incompatible), and checkall stayed
    ALL OK with /bin/dc missing from the install tree (seen only by coverage.py). Then in oconvert, only for the two
    pointers of a difference: dc exact again; bench, ex unchanged.
  - Side findings in crpost (07/26/82, not in the tree; System III crpost.c compiled with ccv0, functions searched in the
    original): put `&obuf[optr] - 1` (obuf a pointer) is `optr - 1` first and obuf added last (the rule of OLD_D19P, here
    over SUBSR); get reloads a parameter `ld r7,0(r15)` before a call (the place of OLD_D3AW, three days after chkout).

## 2026-11-23 ex in phase A (increment 161): what stays for the compiler

- compare/ex rebuilt in the setup format: 394 functions, 361 exact (ccv0a). The open ones: the clusters indirect call
  (listchar, normchar, vgotab), structure copy register (setty, gTTY), local array index (getone, 6 forms tried), and
  dosub; the rest differ only by call/calr or jp/jr (cas by the distance, after the other differences of their module).
- dosub, `loc2 = sp + (linebuf - genbuf)` (the form of every BSD ex and of Plexus): ccv0, ccv0a, ccv1 and the shipped
  /bin/cc fold the difference of the two arrays to 0 (wrong code: `ld _loc2,r13`); the original of ex computes
  `ld r2,#_linebuf; sub r2,#_genbuf; add r2,r13`. ccv0p writes `lda r2,0(r14)`. Folded too: `linebuf - genbuf + sp`,
  `(int)` casts, `&sp[linebuf - genbuf]`. `sp - genbuf + linebuf` is not folded (4 lines left: the order) and removes the
  block order difference of the loops of dosub; not taken (no source has it). A difference for a switch later (not in this run).

## 2026-11-24 v0a OLD_D65: the difference of two address constants (ex dosub)

- cparse passes `&linebuf - &genbuf` unfolded (the .I file: MINUS of two U&NAME); codgen folds it: optim makes each U&NAME
  an ICON and its MINUS case (WEGA: x-x is 0) asks treeeq, which compares an ICON by lval and rval only. First attempt in
  optpre on two ICONs: no effect (optpre runs in front of optim, the operands are still U&NAME). With the U&NAME test:
  dosub exact (also its loop layout), ex 362 of 394 (361), no function lost. matrix check: as in the table.

- 2026-11-24 make (phase A): the shipped cc gives everything but the frames of 13 functions (getmflgs, straightrans, setvar,
  eqsign, appendq, mkqlist, sindex, is_sccs, trychk, addstars, setenv, readenv, eqstr): the original has 2, 4 or 6 bytes more,
  as many words as some (not all) of the register locals; varptr, copstr, is_chk get none. Measured: ccv1 for the whole program loses more functions than it wins,
  ccv1r, ccv1q worse; the v1 cparse (OLD_D1) with the shipped codgen and popt (a scratch root): 177 of 211 functions against 197
  (the slot for every register local is too much). A scratch cparse without `q->sfldc = 0` in lookup (sym1.c; the slot test of
  oalloc reads sfldc of a register local): getmflgs right, the others not (191). Guess: a cparse of 11/83 whose symbol entries
  keep the sfldc of an earlier symbol of the same place; the rule is not found. Nothing changed in the compiler.

- 2026-11-24 register slots (make, phase B, step 1 and 3): the word lies in the part of cparse (sindex: the homes of s1, s2
  stay at 0 and 2, the auto ii moves from 4 to 6), at the declaration of a register local (oalloc). Witnesses of the same
  kind, written so far as `int unused` / `int spare`: diff prepare, equiv, main, stone; mail isfrom, system; nl main; slibc
  calloc (vl). No feature of the source separates the 19 functions with a word from the 58 without (name, earlier use as a
  parameter, type, number of register locals, for-init, initializer, ++/--, leaf or not). The ld spares (slookup, pslookup,
  index16) have no register local: another cause. Experiment (scratch copy of cparse, not in the tree): without
  `q->sfldc = 0` in lookup, sfldc is what the freed block held before (clearst frees by the hash chains, the strings of
  savestr are malloc'd and never freed); behind the shipped cc it gives the right words in main.c (getmflgs) but in misc.c
  words in the functions just before the right ones (intalloc, subst, colontrans, findfl, get, rm_gots instead of
  straightrans .. eqstr): 47 of 65 functions right. The heap of the cparse of 11/83 runs otherwise (a guess: other mallocs or
  another size of struct symtab); not found.

## 2026-11-24 v0a OLD_D66: the base register of a structure copy (ex setty, gTTY)

- `nttyc = ottyc` (6 bytes, ldir): the destination is loaded into r0 and, r0 being no base (notoff), copied by
  `order(u, STAREG)` into a word register: getreg(RCWORD, -1) once. Allocator state there (instrumented codgen): setty free
  r1, r6, rr4 (r7 holds the parameter f), gTTY free r1, rr4, rr6. v0 takes r1; the original r4 and r6. Each of D2W, D2F,
  D2L, D2LP, D2G, D2P, D2W6, D2N, D2C, D2T, D2R undefined alone: no change; D2F with `>=` and "no word whose pair mate is
  busy" move the source register too. Rule that gives both: the even half of the pair a pair request takes (first free pair,
  OLD_D2W6 rr6 before rr4), only for this copy. ex 361 -> 363 of 394 (exmap), none lost. Two witnesses, one file.

## 2026-11-24 OLD_D5S: an array that is a member of a local structure (ex getone)

- `G.argv[G.argc0 - 1]`, G a local `struct glob`: the original `add r2,#%0004; add r2,r15`, v0 `lda r3,4(r15); add r3,r2`.
  Tree in fixfld (instrumented codgen): SUBSR(PLUS(REG fp, PLUS(ICON ~L1, ICON 4)), index); the first OLD_D5S rule wants
  an ICON on the right of the PLUS and misses the unfolded frame + member. Extended (no new switch) to PLUS of two ICONs:
  `add r2,#~L1+4; add r2,fp`. ex 363 -> 364 of 394, none lost; bench same 139, 188 lines (unchanged); matrix check as in
  the table; checkall ALL OK (cas by vc model same 152). codgen v0, v0p, v0a rebuilt into OLDROOT.

## 2026-11-24 ex shrink.sh: the temporary of an indirect call (listchar, normchar, vgotab)

- Not a compiler difference: the codgen text is `ld ~L1(fp),r2 ... ld r1,0(fp); call @r1` in all six places (the only
  ones in ex; the other indirect calls load r1 from a register). In /bin/ex the store keeps its offset (0(r15)) in all
  six, the load becomes @r15 only at the last indirect call of each function (listchar 2nd of 2, normchar 3rd of 3,
  vgotab 1 of 1). Rule taken into shrink.sh as read off (owner 2026-11-24), written ~L1+0(fp); no mechanism known.
  ex 364 -> 368 of 394 (listchar, normchar, vgotab; _atoip was cut by the limit of the measurement, exact).
- The call/calr functions do not follow: the own code is still 152 bytes shorter than the original, the gap grows only
  in them (pragged: `call _undo`, a forward call within reach, calr in the build): the length choice of the old cas.

## 2026-11-24 ex byte-identical: the old cas, the commons, libc-v11

- The 24 call/calr functions: ex assembled with the old cas (ccv0a -c x.s, /lib/old/cas): all 394 functions exact, the own
  code ends at 0xe96e as in /bin/ex. D17 holds for ex too.
- Then the file: data 4 bytes longer: EXSTRINGS /usr/lib/ex2.13strings, /bin/ex has /lib/ex2.13strings. bss in another
  order: /bin/ex has neither the first-mention order of the headers (shipped ld) nor an order of a simple hash of the
  names (h*k+c mod m, k 1..39, m 50..2099, with and without '_', 8 or all chars: no fit); exbss.c (as bss3.c of codgen)
  gives it; 13 unreferenced commons fill the 182 bytes of holes exactly. Two jp stubs of ex_vput swapped: the host sort
  in the locale (_column before _Copy); LC_ALL=C in shrink.sh. Last byte: _length 0_3 (libc-v1 has 0_2; 0_3 in libc-v1
  costs 17 programs). Of all libc-vN only libc-v11 has no member of ex in another version; its 19 shared members stand
  in the order of /bin/ex, the 23 others were inserted in their places (owner 2026-11-24). checkall -b: ALL OK, 847 files.

## 2026-11-24 v0a OLD_D18A: the parameter stores at offset 0 (ex shrink.sh)

- shrink.sh (WEGA's step between compiler and cas, rebuilt from /bin/ex) looked made up in its step 2 (owner): its rules
  for 0(fp) beside r0..r7 are the compiler's. A v0a variant without OLD_D18 in regargs (only there) writes the parameter
  stores at offset 0 as @fp; ex stays byte-identical without the three store rules of shrink.sh. The only loads
  `rN,0(fp)` the compiler still writes are the six `ld r1,0(fp)` of the indirect calls (gencall, OLD_D18), the case
  "only the last load of a function is @fp", still in shrink.sh with the ~L1(fp) rule. Step 1 (the jp tables) rewritten
  as a short script, same output.
- The rest of step 2 of shrink.sh (the same day): where "only the last indirect-call load of a function is @fp" comes
  from. Not codgen: an instrumented v0a codgen has the same state at all six loads (maxoff 0, tmpoff 0, lval 0). Not
  popt: raw codgen against -O output: popt merges the two '^' calls of listchar (that load short in /bin/ex), but also
  the tails of the two long ones of normchar, and leaves vgotab (short) untouched. Not the next or previous instruction
  (listchar 2nd and normchar 1st have the same neighbours, short and long). Not the old cas shared with the other
  programs: cparse and the rest have ~L1(fp) at ~L1 := 0 long (D18), a change there would break them. Left: WEGA's own
  step for ex, form unknown; the rule stays in shrink.sh as read off (6 places).

## 2026-11-24 v1r OLD_D67: an idle parameter word goes back (od cput)

- od cput `c &= 0377`: the original r2, ccv1r r6. Instrumented v1r codgen: the word request sees free r6 (word), rr2, rr4,
  rr0 and in use an idle r7 without a name (the register of c, stored to its home); the first free word is r6. Variants:
  every idle nameless word back (X1): cput right, od main wrong (ldb rl3 there); only when the mate is free (X2): the same
  (r2 idle beside a free r3 in main, line, putx, putn: the original takes r3); only r4 .. r7 with a free mate (X3): cput
  exact, od only putn left (popt), write and rmdir byte-identical, dd unchanged, the 17 libc modules of ccv1r unchanged.
  Taken as OLD_D67 (v1r). One witness.

## 2026-11-24 v1r: its own popt, without OLD_D52 (od putn; od byte-identical)

- od putn: the original puts the else arm of the inline putc (`if (d > 9) putchar(..); else putchar(..);`) in front of the
  shared `_ptr++` and its `--_cnt` test at the end (`jr ge` back); ccv1r kept the source order. Seven source forms (d <= 9,
  ?: as a statement, return in either arm, >= 10, !(d > 9)): none. The raw codgen output through every popt: v0, v0p and
  the shipped /lib/popt make the layout of the original, only the v1 popt (-DOLD_D20 -DOLD_D52) does not; without
  OLD_D52 it does, without OLD_D20 not. v1r (01/83) gets the popt of v1 without OLD_D52 (build/nonsegv1r/popt,
  /lib/v1r/popt, cc.c): od byte-identical, write and rmdir byte-identical, dd unchanged, the 17 libc modules of ccv1r
  unchanged. v1q keeps the v1 popt (not measured).

## 2026-11-24 df byte-identical: the cast in alloc, calloc 0_2 with int size, v1 OLD_D53 (nlist)

- alloc: `b >= sblock.s_fsize` signed in the original, daddr_t unsigned long in WEGA's sys/param.h: `(long)` casts, not
  under -DSHIPPED (owner). `long b` alone does not do it (my first count was wrong).
- calloc 0_2 (`num *= size`, size from r6 in the original): cparse types the operand int, regforsym of v1 wants the very
  type (unsigned) and takes the frame. As a v1 switch (int, unsigned, pointers one type, OLD_D68 trial) 23 files of
  ccv1 lose (cc, scc, ld, mv, rm, sort, passwd, su, mail ...): taken out. 29 source forms (casts, operand orders, *1, +0,
  blocks, declarations): exact with size declared int, size undeclared, or short size; taken `unsigned num; int size;`
  (owner). calloc 0_2 is only in libc-v9, ld does not call calloc: no new version.
- nlist 0_1 (ccv1): frame 1602 against 1600; ccv1r gives 1600 for nlist but 4 for readby (6 in the original): the return
  temporary of v1, which the original has in readby and not in nlist (autos 1592 bytes): vl OLD_D53 exactly. OLD_D53 added
  to the v1 codgen: checkall -b, df byte-identical, no other file lost.

## 2026-11-24 time and date byte-identical by the place of a register declaration (OLD_D1)

- time main: the longs at 0x1a/0x1e with a free word at 0x18 in the original, ccv1 0x18/0x1c with the free word at 0x20.
  date main: tb at 0xc in the original, 0xa with ccv1. The free word is the frame place of a register variable (OLD_D1 of
  v1 gives every register variable one at its declaration): time `register p;` declared in front of `long before, after;`,
  date `register char *tzn;` in front of `struct timeb tb;`: both byte-identical. Earlier 11 orders of time did not move p.
  To be tried on the other "spare" and "unused" frame words of v1 programs (ld main, endload, slookup, pslookup, index16).

## 2026-11-24 dd byte-identical: v1r popt without OLD_D20, libc-v13 (open 0_1), two source forms

- match: the loop of the original keeps the test on top; the raw ccv1r output through the popt of v0, v0p, the shipped
  one or v1 without OLD_D20 gives it, with OLD_D20 (v1, v1r) not. The v1r popt now has no v1 switch: od, write, rmdir
  byte-identical, the 17 libc modules of ccv1r unchanged.
- open: dd has the open of two arguments (open 0_1), write and rmdir (libc-v10) that of three; of all libc-vN only v10
  fits dd but for open: libc-v13 = libc-v10 with open 0_1 (owner).
- ibc = ibs in main: r2 in the original, r3 with ccv1r (r2 still names ibc after `ld r2,_ibc; cp r2,_ibs`). 21 forms:
  none; an unused label in front of the assignment gives it (taken, owner). OLD_D2N (v0) in v1r: no effect.
- The string "dd" (perror in chkres): ccv1r writes data and switch tables where they stand and all string literals at the
  end; in the original "dd" lies behind the switch table of number and in front of the strings: a global
  `char ddname[] = "dd";` between number and cnull, perror(ddname). dd byte-identical.

## 2026-11-24: register slots of 1983, the scratch cparse on the test set

- make: the 63 extra entries in front of the first function are `<pwd.h>`, `<ar.h>`, `<s.out.h>`, `<nlist.h>` in defs
  (160 real builds of the header placements, hplace.py); then only files.c eqstr wrong (2 entries fewer needed, which
  not known). Refuted on the way: sys/types.h + sys/stat.h in defs with 0 .. 110 dummies; types.h without saddr_t,
  uaddr_t; s.out.h without s_head or v_lines for all files.
- ar and cpio, taken as counter-programs, have unused locals under other names (`int u`; `int i, j, k`, `int i`): without
  them all their frames come out by the scratch cparse. Left: mail system one word too many.

## 2026-11-24: vm, the cparse of 11/18/83 (owner)

- `OLD_D1S` in cparse sym1.c (`lookup` keeps the stale `sfldc`), `/lib/vm/cparse` (shipped cparse built as shipped, ccv0p,
  libc-v7.a, plus the switch), driver `ccvm` (`OLD_VM`). The test set by ccvm: the same as the scratch cparse.
- Built by ccvm without their unused locals, byte-identical: ar, cmp, cpio, diff, du, grep, mail, make, nl, pr, putfile,
  who; getfile as before (partial, the same bytes as by cc). make: defs with pwd.h, ar.h, s.out.h, nlist.h, the yyparse
  sed gone, eqstr keeps `int unused`. mail: system needed 3 entries more in front of the first function; of 175
  combinations of natural declarations only `optind, strcat, strcpy`; of headers sys/dir.h and sys/prim.h; taken
  `<sys/dir.h>`.
- checkall: 853 installed files identical with WEGA (the 13 by ccvm among them, /lib/cparse unchanged), all corpora but
  casold equal; casold failed once on "Text file busy" (a host race at the start of cas), its rerun 924 of 924 equal.

## 2026-11-25 v1l: the compiler of lex, and the cause of regcmp

- lex (01/14/83, System III source) with ccv1: 25 of 61 functions differ, the frames larger, the tcc form of regcmp
  everywhere. ccv1q closer (QUEST temporary; sizes of error, warning, cpyact, layout right), ccv1r worse (D1R).
- Built v1q + a store rule, measured per function (a pf diff of each function, mapped by order, against the original):
  `&&`/`||` as a value through a temporary: 25 > 18; the stdio.h of the time (an absolute -I is invisible to the guest:
  copied) 18 > 12; the WEGA edits of lex (option -f, myalloc, `={`): 12 > 10, data and code sizes equal.
- In a condition: first "the left of `&&`/`||` is `&&`/`||`": layout, cpyact wrong (`a && b && c` stays jumps); "the left
  of `||` or the operand of `!`" (siconv `!((a && b) || c)`): right. An operand that is no comparison: `x != 0` (cclinter).
  10 > 5, yylex exact.
- The rest register choice: v1 switches D61 D62 D63 D53 neutral; D1R 22 differ; D67 usescape right, space wrong. The popt of
  v1r: munput (the loop not rotated) right, 4 left.
- mn2, yyparse, packtrans: behind `||` the registers go back (r3/r4), behind `&&` not (r5/r6). Variants: free behind `||`
  always; only behind its own join label; also `&&` behind its own label: all three 0 differ on lex and on regcmp. Built the
  first (OLD_D3O; first named OLD_D3L, which v0 already has: checkall showed v0 and v0p changed, renamed).
- usescape/space (D67): a dump of rsuse/rsfree at the dispatch (popt cuts `//` lines: read without -O): in space the idle
  TEMP r6 of the switch value is in use and D67 gives it back with the free r7; in usescape the bodies gave it back. D67
  excluding TEMPs: the bodies of usescape wrong. D67 + D2T: the dispatch of usescape takes r6 again (getreg). OLD_D67S
  (busy only when still in use, no getreg): 0 differ.
- libc: atoi and calloc missing in libc-v3 (the two gaps of setup.py); atoi of ccv1q and the System III calloc.c (0_2.c) of
  ccv1q (mult rr2,r6) give the whole binary: IDENTICAL.
- regcmp: main by ccv1 as before, src/lib/libPW/regcmp.c by the new compiler: IDENTICAL at the first try. Earlier rounds
  (2026-11-24: 40 source forms, no compiler of the tree) are explained: no tree compiler had the rule.
- Probe, units getflt (System III form `do c = get(); while(c == ' ' || c == '\t');`, alone, ccv1l -S): both compares into
  @fp, `test r2; jpr ne` after the first, `test @fp; jpr ne` back into the loop, as the original (increment 161 note):
  stored by OLD_D14L (guess: cparse hands the do-while condition to codgen under a `!`). Only the form, units not built.
- The calloc of lex: ccv1 and ccv1q give the same bytes for the System III calloc.c, so libc-v3 takes calloc 0_2 (the note
  "not exact" of 0_2 was old); atoi differs between ccv1 and ccv1q: atoi 0_3 (ccv1q).

## 2026-11-25 vz: local, a Zilog binary of 02/84

- local built by the shipped cc: own code exact, the rest off (statics in the data, printf, doprnt, malloc). The ZEUS 3.21
  root (`tmp/zeus321_root`) has a libc.a whose printf (0x112), doprnt (0x9ec), malloc (0x33a) have the sizes of the original;
  its `/bin/cc` builds local.c into the original but the PID of the scratch file and the order of `_rc` in the symbol table
  (rc declared in front of acknak: then all equal). The ZEUS `/bin/local` is another version (1.3, 05/29/84, stripped).
- The owner: the WEGA developers patched the Zilog marks out of the ZEUS binaries with a hex editor; the SCCS line of local
  (`aaaaaaaaaaaabbbb/local.c ... - ............`) fits that.
- Pass by pass: the shipped chain with the ZEUS cparse alone gives the object of the ZEUS chain. Probes (t.c, u.c, v.c: 20
  kinds of statics) on the ZEUS cparse: an uninitialised static in the .bss, `.even` on both sides for char, char arrays and a
  scalar float (not for float arrays, pointers, structs, short, long, double). `OLD_D12B` writes that: the objects of local.c
  and of the probes equal to those of the ZEUS cparse.
- The libc members of local that differ from the shipped libc.a: data, filbuf (`_NFILE` 40: the stdio.h of ZEUS is the
  shipped one with 40), doscan (no unused locals in _instr, _getccl), printf (no _pfile, no unused long in sprintf), doprnt
  (seven changes, findings in `oldlibc/doprnt/0_9.c`), malloc (no sccsid, calloc size unsigned). Each from C by cc and by ccvl,
  equal to the ZEUS member.

## 2026-11-28 sh service.c: the frames by vm

- service.c (08/22/83) by the shipped cc: all frames right but await (+2 words), mactrim (+1), scan (+2), gsort (+2); by
  ccv1 nearly every function has too many words; by ccvm (OLD_D1S) execscr, await, mactrim, scan wrong.
- ccvm with K dummy `extern int` after defs.h (real builds, K = 0 .. 110): best 13 of 14 at K = 19, 56, 92 (period 1 KB),
  scan short of its 2 words. The replay of the scratch cparse log (`host/recon/regslots`, the sh test file added in the
  work directory) gives the same; no single change of the stream (leak, alloc/free, dropped free) and no second block of
  entries in front of a later function (await, trim, mactrim, scan; 0 .. 40) makes all right but 39 in front of trim.
- Not it: the declaration of gsort inside scan (three places), scan without `INT argn;`, single headers (sys/types.h,
  sys/stat.h, sys/param.h, errno.h, signal.h, sgtty.h, termio.h, fcntl.h, time.h, ...) with K = 0 .. 40.
- Found by the replay: the implicit declarations made inside a function are entries of their own; two of those first
  made in await (blank, prn, prp, wait) declared in front of the first function, with 17 further entries there, give all
  functions right (real builds: 14 of 14 masked, strictly equal in the linked sh). `<sgtty.h>` is 17 entries; taken:
  `#include <sgtty.h>` and `PROC VOID prp(); PROC VOID prn();` after defs.h (which pair is not in the bytes).

## 2026-11-28 adb: frsize, exform, stfind, getreg

- frsize: `a.a_offset = ap->a_offset + (inst&0xff)*2 + 2` gives the jr target with ap loaded first (taken); the frame
  stays 18 bytes (16 in the original): not by the (POS) casts, the place of `ADDR b`, a cast on the return of get.
- exform, the long case: `wx=itol(w,get(...)); wx &= 0x7fffffff;` (the Plexus line and the mask as a statement; taken)
  stores wx and masks as the original. Left: the original keeps the result of get in a frame temporary (0x28, below
  the temporary of inkdot's result) and stores itolws.I[0] first; the shipped cc stores I[1] at once. A local for the
  value lands at other offsets; itol with its arguments in the other order leaves only that store different.
- stfind: `off>=base` in rr4 with the copy for `off-base` (original) against rr2: five forms of the condition (order,
  `base<=off`, `!(off<base)`, `off-base>=0`), none.
- getreg: the original compares regnam in rl7 four times, the shipped cc the last two from the home of the parameter;
  four forms of the condition, and print.c by ccvm, ccvl, ccvz, v0p, v1, v1c, v1l, v1q, v1r: none. A popt question.

## 2026-11-28 sh cmd (inout) and getch: popt block order (v1)

- inout (in the range of cmd): the order of the cases (6 permutations of DOCSYM, '>'+'<', default) and a nested IF for
  the ELIF chain do not help; a `break;` behind `iof |= IOMOV` (or behind `iof |= IORDW`, or each branch its own break)
  gives the layout of the original (popt: the '>' and '<' blocks behind the return). Taken: the break behind IOMOV.
- getch (macro.c, in the range of copy): the original places the retry head (readc, subchar, the test of '$') behind
  the call of comsubst and the DQUOTE block behind the return; the v1 popt (traced: popt v1 with -DTRACE) inverts the
  goto loop and puts the head behind the DQUOTE block. Not it: `IF !subchar(d=readc())`, ELSE return(d) for the last
  return, `;` for the endch return, peekc behind the FI, the tail with a common `goto retry` (four forms).
- getch solved: `ELIF d==DQUOTE THEN goto dq; FI return(d); dq: quoted++; quote^=QUOTE; goto retry;` gives the layout
  of the original (no loop inversion at the DQUOTE jump, which no longer stands in front of the label of the return).
- assign (name.c): the original reloads n for `failed(n->namid, wtfailed)` (`ld r2,r7; ld r7,4(r2)`), ours keeps n
  from the test in r2 (`ld r7,r2; ld r3,4(r7); ld r7,r3` before popt). Not by `(..)!=0`, the branches swapped, `(*n)`,
  a return in the then part, `&(n->namval)`, a cast, an unused label, any old compiler (12).
- e3 (test.c): `REG` on the longs, p2 or p1 changes the code; the extra word of the original is not found.

## 2026-11-28 sh e3 (test.c): the extra frame word, traced

- Trace of oalloc (v1 cparse, scratch copy, KDBG to stderr): e3 gets p1 0, a 16 (the v1 word of the register), p2 32,
  int1 48, int2 80 (bits); `L_INT atol();` gets no place. The frame (ccv1 -S, ~L1 = 0): the autos end at 14, the
  codgen temporaries follow at 14 (a word) and 16 (a long), r14 is saved at 20. The original has the same autos and
  the temporaries at 16 and 18: a gap of one word between the end of the autos and the temporaries.
- Rules tried in scratch copies (none in the tree):
  - cparse bccode `SETOFF(autooff, SZLONG)` always: e3 exact, nxtarg and e1 of test.c get a word they lack (no temps).
  - the same only in functions with a long auto: test.c exact, time main (01/83) and ld.c change.
  - codgen freetemp: the first temporary of a statement at a 32-bit offset, in a function whose referenced symbols
    hold a long auto: test.c, time, date exact; sh main.c (exfile: mailtime long, statb), ld.c, sccs chkin.c and
    common.c change (exfile against the original: wrong).
  So the gap is not an alignment of the temporaries by longs; the cause stays open. test.c keeps `INT unused;` (the
  last declaration of e3) and `INT spare;` (behind av, the unused word in front of the signal table of the libc).
- ld load2: `long u1, u2` are used (dseek offsets), not unused words; the frame of load2 (a 0, sp/lp/symno the v1
  words 4..10, c 11, d 13, n 14, u1 16, u2 20) shows no gap in front of the temporaries. No second witness.

## 2026-11-28 sh assign: n kept in r2

- Before popt the shipped and the v1 codgen give for `failed(n->namid, wtfailed)` behind the test `bit 10(r2),#15`
  the sequence `ld r7,r2; ld r3,4(r7); ld r7,r3` (n taken from r2, where the test left it); the original has
  `ld r2,r7; ld r7,4(r2)` (n taken from its parameter register r7 again). Not by the order or the types of the
  parameter declarations (STRING v first, STRUCT namnod *n, CHAR *v), nor by a v1 codgen without OLD_D3 (scratch copy),
  nor by the forms tried before. Every codgen of the tree keeps n in r2 here; a trace of the register state in codgen
  (rsuse, regforsym) is the next step.

## 2026-11-28 popt D20 too wide: getch, inout, Dword, putline (owner: taken into the tree)

- getch (sh macro.c) in the System III form: with the v1 popt without `OLD_D20` the range copy..getch is equal to the
  original (0 lines); `goto dq` was a way around D20. Trace: the return block `ldb; extsb; jpr L184` (L240/L236) stands
  in front of the exit; D20 lets the block go on behind the jump to the next label, runs into `ret` and nothing moves;
  the original moves it to the `jpr L236` of the peekc block, and the DQUOTE jump is no longer followed by its label (no
  loop inversion there).
- D20 users in the tree (raw code of all 293 modules built by ccv1/ccv1q/sccv1 through popt and spopt v1, with and
  without D20, compared with the labels renumbered): ld load1, ld ensure_buf, libl allprint (nonseg and seg). Their
  blocks in front of the exit hold one instruction or none in front of the jump (`ldk r2,#0`, `ld r2,_rlbuf`, empty).
  getch (2: `ldb; extsb`) and Dword of csh in the Berkeley form (`ret:` behind the loop, 4) move in the original.
- Rule (scratch popt, an extension of `OLD_D20`): the block that goes on behind a jump to the next label and runs into
  `ret` stays only with at most one instruction in front of that jump; a longer one moves, ending at that jump. All 293
  modules unchanged (popt and spopt). Natural forms exact with it: getch (System III), inout of sh cmd.c without the
  extra `break` (System III), csh Dword with `ret:` behind the loop (Berkeley), ed putline as the while loop of System
  III. Not D20: csh donice (the natural `||` forms give 104 bytes, the original 108, with and without the rule).

## 2026-11-28 sh assign: a codgen rule, OLD_D3B (owner: taken into the tree, v1 codgen)

- Scratch v1 codgen (reader3.c, cbranch default case): behind the condition of a statement `if` (the CBRANCH node) whose
  test is `&` of an operand without a conversion (`bit 10(r2),#15`, the operand tested in memory), the registers that hold
  copies of parameters outside their own register go back. assign exact (name.c 21 of 21). First form (every AND):
  csh backeval lost (`int quoted = (literal || (cp[0] & QUOTE)) ? QUOTE : 0;`, the AND inside `||` and `?:`, Berkeley
  source as it is); with the statement `if` only: oldlibc nlist readby changed (`if(sym->sn_name[0] & 128)`, a char:
  `ldb; extsb; bit r2,#7`; df is byte-identical with the old code). With both limits the codgen output of all 211 ccv1
  modules is unchanged except sh name.c. One witness; the two limits come from the two counter-witnesses (guess, no
  mechanism known).

## 2026-11-28 sh: the sources against System III once more

- Against System III only sccsid lines, setjmp/longjmp as setret/longret and WEGA's own functions (loginsh, unset,
  ESC B, scripts, copyargs global behind useargs) are left, besides two spares in test.c. defs.h `extern address end[];`
  went back to the System III `address end[];` (ccv1: `.comm 0,_end`): sh byte-identical with both. Without `INT spare;`
  in test.c sh differs in 8 bytes (kept). Left as hints of a cause in the compiler: the frame word of e3 (`INT unused;`,
  of the kind of the v1 spares of diff, mail, nl, ld; open) and the global spare (a bss word in front of the signal table
  of the libc; compiler, libc or a lost WEGA variable: not known).

- 2026-11-28, D20 rule narrowed after checkall: libcisam and slibcisam failed (isopen: the error path L85, 11
  instructions, entered by the gotos to `exit:` and `bad:`, moved; the original keeps it; the 28 modules of ccv1c/sccv1c
  were not in the sweep). Basic blocks only (no conditional jump, no label inside): isopen right, but sh inout lost (its
  block of 19 has an `if` of its own, label L261 named only inside). Taken: the block stays when a label behind its first
  instruction is named by a jump from outside the block (`d20outer`). 293 + 28 modules unchanged, getch, inout, Dword,
  putline, assign exact.

## 2026-11-28 the spare/unused words of v1 programs, systematically

- ld slookup, pslookup (v1): the Plexus source has `return(lookup());`; with `return lookup_name();` the frame word is
  the return temporary of the shipped/v1 codgen and the spares go: ld byte-identical. sld slookup, pslookup (shipped scc):
  the same, `return(lookup_name());` for `long dummy` (the 4 bytes of a segmented pointer): sld byte-identical.
- ld index16: 6 bytes in the original, 4 with every natural form tried (two returns, else, ?:, pointer differences,
  sizeof, a local i (6, other code), a register parameter (6, other code)); sld index16 (shipped scc) has the same extra
  word: two compilers agree, so the word is most likely in the common source (an unused local; guess).
- sh e3: the original's temporaries start at 0x10 also on the outer level (`int2 = atol(nxtarg(0))`), the autos end at
  0x0e. Refuted (scratch passes): baseoff aligned to a long in inner blocks (nxtarg, e1 lose); the same only with a long
  auto (e3 exact, but 12 modules of 211 change: libmp mdiv, mout, oldlibc a64l, cnvt, ctime, doscan, passwd, time, strip,
  more, csh sh.func.c, sh main.c); in every block with a long auto (16 change, among them libPW patol, xalloc, su, ld.c);
  the first temporary of a statement at a 32-bit offset in codgen (nxtarg and e1 have their return temporary at 2 in the
  original). test.c carries the sccsid 2.3 where the unchanged files of sh have 2.1 and the changed ones 2.2 (msg.c,
  xec.c, name.c): WEGA changed test.c twice without a visible change of code; the two unused words (`INT unused;` in e3,
  `INT spare;` behind av) fit such deltas (guess).
- Other spares of v1 programs: passwd main (`long unused` between u and insist: a place among the declared autos, so a
  declaration of WEGA's source, which changed passwd), more and ld.c (bss words of unknown commons). ld main, endload: no
  spare left (the findings notes were old).
- sld (shipped scc, 10/27/83): its spares (`int spare[N]`) match in several functions the number of register locals of
  the Plexus source (load2arg 2: cp, lp; load2td 3: r, t, sp; flush 1: n): the picture of the register slots of the
  compilers of 11/83 (OLD_D1S, OLDCC_REGSLOTS.md), here segmented. Not measured; a lead.
