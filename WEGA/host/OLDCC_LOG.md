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
