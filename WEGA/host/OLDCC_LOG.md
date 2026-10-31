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
