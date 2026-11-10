# cas: handoff

State of the reconstruction of `/bin/cas` (src/cmd/cas) on 2026-11-10, branch toolchain, up to commit ffaf62d9
and the commit of this file. Extract of `host/OLDCC_LOG.md` (entries 2026-11-09 and 2026-11-10), `compare/cas/findings.json`
and the working knowledge of the sessions. What is fact is measured; guesses are marked as such.

## 1 State

- Compiler: sccvc (`OLD_VC` in `src/cmd/codgen/oldcc.h`, the `segvc` rules of cparse and popt, `/lib/vc/*` of OLDROOT),
  linked with `src/lib/slibc-v11.a`, assembled by `/lib/old/cas`. `old/cas` is built by the shipped scc with `OLD_CAS`.
- Model (d12model + strictcmp): 145 of 152 functions strictly exact, 7 open (2026-11-10, after the vc switches D2S, D4A, D2CV and the source forms of K2 and sgetnode). Text, data and bss
  have the original sizes; `/bin/cas` is not byte-identical because the open functions have other lengths and every
  address behind them moves.
- Open functions (strictcmp: original size, model size, lines):

| Function | Orig | Model | Lines | Kind |
|---|---|---|---|---|
| yylex | 2600 | 2588 | 26 | popt: place of the `done` block |
| jpr | 324 | 328 | 22 | popt: which tail copy stays, place of the loop step |
| stabout | 388 | 388 | 18 | popt: split dispatch |
| exprreloc | 260 | 258 | 3 | popt: the `return (0L)` of the outer default is moved to the dispatch |
| stenter | 392 | 392 | 9 | codgen: registers of `p2->s_12 = blklevel` (the reload of `name`: OLD_D2CV) |
| main | 2064 | 2064 | 6 | only the address of `sigdone` (lengths in front) |
| yyparse | 6928 | 6928 | 2 | data / D6T switch table (`cparse:OLD_D6T` witness) |

- Commands (from WEGA, `O=/mnt/share/cvs/P8000/tmp/oldroot`, `T` a scratch directory, wrappers `cc_vc` = `z8k-emu -r $O
  /bin/sccvc`, `cas_old` = `z8k-emu -r $O /lib/old/cas`):

      RECON_PROG=cas RECON_TMP=$T/t CCHOST=$T/cc_vc CAS=$T/cas_old python3 host/recon/d12model.py -q
      RECON_PROG=cas RECON_TMP=$T/t python3 host/recon/strictcmp.py [-v FUNC]      # STRICT_CTX=n for context
      MATRIX_VER=vc sh host/recon/oldcc.sh matrix [-j N] [SWITCH ...]               # host/OLDCC_SWITCHES_VC.md
      MATRIX_VER=vc sh host/recon/oldcc.sh matrix check
      make -C host oldroot ; make -C host checkall WEGA=<WEGA_ROOT>

- Scratch tools used in the sessions (not in git; worth putting into host/recon when cas is taken up again):
  - `vmodel.py FILE FUNC OLD NEW...`: each NEW in place of OLD in src/cmd/cas/FILE, model, strictcmp line of FUNC,
    file restored. The workhorse of the source form search (about 1.5 minutes per variant).
  - `pt.sh FUNC [spopt]`: the raw code of FUNC from `sccvc -S` without -O, run through a TRACE build of spopt (built
    from a copy of src/cmd/popt with `-DSPOPT -DOLD_D13 -DTRACE` by ccv0p); prints `/*CM ...*/` for every block codemove
    moves and the nodes after each iteration. Needs y.tab.h of the model for scanner.c.
  - `bv.sh "CFLAGS"`: spopt of vc with test switches into OLDROOT/lib/vc/spopt, whole model, better/worse list.
    Always copy the original spopt back afterwards.
  - `cgb.sh`, `cpb.sh`: the same for scodgen (switches out of / into `OLD_VC`) and scparse.

## 2 What we learned in general

### Method

- Source forms first, for the whole program; switches only for what no form explains; an existing switch (also one of
  another version) before a new one. The two-witness rule holds; a switch with one witness is not taken.
- The open clusters of the night (K1 .. K14) turned out to be source forms in most cases, also those that looked like
  popt differences. Before a layout difference is called a compiler difference, try:
  - `if (c) A else B` against early `return`/`goto` and the inverted condition (`if (v & 8) X else Y`);
  - how a case or a block ends: `break`, `return (v)`, `continue`, a fall into the code behind the switch;
  - the order of the cases (codgen emits the bodies in source order, the dispatch sorted by value);
  - `while (a && b)` against `while (a) { if (!b) break; ...}`;
  - a missing `return` at the end (the value left in r2: jprange, a quirk);
  - one shared tail by `goto` (jpr `plain`);
  - for registers: a plain parameter walked by a register copy (`opmatch`), a cast of an int parameter (the vc rule of
    D3P reloads it from its home: error, optim), the declaration order of register variables.
- Measure every variant on the whole model (vmodel.py); the effect is often in other functions of the same file.
- Check for "hidden" witnesses: a source form found under the present popt can hide a switch, and a switch can make a
  form unnecessary (symreloc first looked like a D20 witness, then `return (v)` in the cases made it exact without D20).
- `MATRIX_VER=vc oldcc.sh matrix` measures every switch of vc on cas (lost / gained functions, changed lines, data
  bytes). On 2026-11-10 no vc switch was without effect; D2M, D2U have two witnesses, D3J three; `popt:OLD_D13` changes
  only data (the `.addr` jump tables, 382 bytes); `cparse:OLD_D6T` only yyparse and data; D3K, D6T (codgen) and D13
  (codgen) cannot be taken out alone (no build / intermediate file / register jam).

### Compiler facts found for cas (now in host/OLDCC.md)

- vc has `OLD_D24` and `OLD_D25` of vk: a long cast to a pointer is masked through a copy (`ldl rr2,rr0; and r2,#%7F00`).
  Witnesses yylex (`alias`) and igetaddr. This needs the node union of cas with pointer members (`n_u.p`, `cp.p1`, ...)
  and the symbol name as `s_name` (a union with `s_2`), else every node operand gets a mask.
- `OLD_D13` in scparse, scodgen, spopt and cas: bit 31 of segmented address constants and `.addr` lists stays.
- vc rule of `OLD_D3P`: a cast of an int parameter loads it from its frame home (iputw, sputw, error, optim); for a
  widening to long the original takes the register (symtype), see K5.

### popt (spopt of vc = the shipped one with OLD_D13)

- The remaining layout differences are all of one kind: our codemove moves a block out of a block it has just moved,
  before the jump of that block to the label behind it (now adjacent) is taken away; and xjump/comjump then keep
  another copy of a common tail. The original does this in some functions (exprtype: default moved out of the moved
  dispatch) and not in others (exprreloc, jpr, yylex, stabout). No rule separates them.
- Measured and dropped (whole model, `/lib/vc/spopt`; details in OLDCC_LOG 2026-11-10):

| Variant | Exact | Effect |
|---|---|---|
| codemove from the last jump backwards | 105 | - |
| a block ending in a jump to a jump stays (R1, R1b) | 131 | only jpput/jrput, which then went by source form |
| CJP skip before jump chain shortening | 128 | jpr2 lost |
| loop inversion also behind a jump (D32) | 128 | blkclose lost |
| smaller window of loop inversion (n 20/16/12) | 123-128 | - |
| inversion tried first for a block behind a jump (D38/D39, split dispatch) | 107 / 131 | stabout worse |
| codemove goes on behind the moved block (D41) | 126 | exprreloc 11 > 9, exprtype etc. lost |
| comjump once before the first iterate (D43) | 128 | - |
| `OLD_D20` (v1) on the present sources | 127 | symreloc (since solved by source), sgetnode 1 |
| D36/D37 (CJP to a loop exit not shortened; inversion takes the last CJP) | 132 | jdeps only, later solved by source |
| features of the 19 jump sites of yylex `done` as a rule (D40) | 21-131 | - |
| `OLD_D29` (vk) | 134 | no change |
| `OLD_D20` on the sources of 2026-11-10 (exprreloc pre-check form) | 137 of 152 | exprreloc exact; exprtype, evalint, exprval, opshort, opseg, opoff, symtype, genlop, jdist lost |
| D20 only for a jump over another label | 145 | no effect |
| a target behind a jump to itself counts as fallen into (D44) | 134 | jpr unchanged, 11 lost |

- Per module (spopt variants against each source file): no module fits another popt any more (symbols.c did, until
  symreloc's source form). symbols.c built by sccv1 or sccvk differs in codgen: cas is one compiler.
- Without `-O` is excluded for the open functions: they all show popt work (moved dispatch, rotated loops, merged
  tails, `testl`); only `assemble.c` is built without -O (as in the original).

### The old codgen, what the raw code shows

- `ldk Rn,#k` comes only from templates 004 (`x = k` into a register variable) and 053 (a constant as the value of an
  expression). Before a call r7/rr6, r5/rr4, r3, r2 are the argument registers (kernel `physio(..., dev, 1)`:
  `ld r3,dev; ldk r2,#1`).

## 3 Per open function

### yylex (scanner.c), 26 lines

- Source forms found (306 > 26): `~` and `.` bodies as goto targets behind the names with their tests behind them
  (`goto other; tilde: ... dot: ... other: if (c == '~') goto tilde; ...`); decimal number: test `c == '.' || c == 'e'
  || c == 'E'` first, the plain number behind; `(long)(unsigned)(idx = stlook(...)) == 0`; `alias:`/`found:` inside the
  block of an unknown name; local name looked up when stlook finds it, 0x57 behind; `if (isdef(idx)) goto value;`;
  exponent digits first; the cases of `done` end with `break`.
- Left: the place of the `done` block (`ld r2,r13; cp r13,#0x22; jp eq; cp r13,#0x23; jp eq; ld r2,r13; jp exit`, its
  content is now the original's). Original: behind `tok = 3` at the end of the `.` body (all six jumps of the names go
  to `%0C08`); ours: at the first `goto done` of the list (tok 0x56). About 15 of the 26 lines are jr/jp lengths behind
  it. 5 lines were the mask of `alias` (now D24/D25).
- Tried without success: `done` written behind the `.` body (with its switch 104 lines: the case bodies come along,
  the original has them at the end of the function; with `goto invalid/unknown` 671: xjump turns the fall into done into
  a jump and our spopt moves done away; without xjump 646); bodies of `~`/`.` at the end before done (364/378); the
  ends of done (`default: break/return`, `return (tok)` in cases: 26/29); the features of the 19 jump sites (the
  original's site is the first whose block is reached only by its label and whose following label no jump names) as a
  popt rule: all worse.
- Assessment: a popt order question (where codemove puts a block reached by 24 jumps); one witness, no mechanism.

### jpr (jpr.c), 22 lines

- Forms found: switches on `p->n_type`, `e->n_type` and `res`; the three `jpput(tok, list); return (0)` as `goto plain`
  with `plain:` at the end (26 > 22).
- Left: (a) the `plain` block: original behind the body of case 0x36, ours right behind the entry; (b) the loop step
  `p = p->n_next` with the test: in the original the default (error) falls into it, ours has it behind case 0x19;
  (c) which copy of `ldl off,rr2` stays (original: in case 7, case 0x19 jumps to it).
- Tried: all 24 orders of the cases 3, 7, 0x19, default (before and with `goto plain`), `continue` instead of `break`,
  0x36 as `if ... continue`, the e-switch nested in case 0x56, `default: plain:` inside the first switch, `res` switch
  variants; none below 22.
- Assessment: popt order (which copy of a tail stays, block moved out of a moved dispatch); same kind as exprreloc.
  jpr is no witness of a split dispatch: our spopt splits its dispatch too.

### stabout (ofile.c), 18 lines

- Original: behind the entry jump `jr X` stand the second half of the tag dispatch (`cp r2,#2b; jr eq,C2b`) and the
  test with the error branch of the first check behind the switch (sect); the name loop (rotated) follows, at its exit
  only `ld tag; cp r2,#2a; jr ne,REST` and case 0x2a. Ours: the whole dispatch at the loop exit.
- Tried: loop forms (`for (;;)`, `while ((*p = sgetb()) != 0)`, `for (p = ...; ...; p++)`), case order, `default`,
  no break, if/else-if instead of switch (worse: the original compares r2, a switch), both checks behind the switch
  inverted, 16 combinations; split-dispatch rules in popt (D38/D39): worse.
- Assessment: popt order; one witness.

### exprreloc (expr.c), 3 lines

- Forms found: inner cases ascending (0x23, 0x24, 0x25), each `if (v & 8) X else Y`; case 0x24 with `v |= 0` on both
  sides (11 > 3: the compare without a jump of the original is an if/else whose two stores vanish).
- Left: the outer `default: return (0L)` (`subl rr2,rr2`) stands at the end in front of the exit in the original and
  both dispatches jump to it; ours moves it to the outer dispatch.
- Measured on the raw code: the original comes out exactly when, at the time codemove looks at the jump of the
  dispatch, the `subl` block has no `jpr` to the exit behind it. `OLD_D20` does that and gives exprreloc exactly, but
  loses 9 exact functions (exprtype, evalint, exprval, opshort, opseg, opoff, symtype, genlop, jdist: their blocks in
  front of the exit are moved in the original). A D20 only for a jump over another label: no effect.
- Tried on the new form: `return (0L)` behind the switch, with `break` (3); case 5 falling into the default (6); `v`
  declared in a block (same).

### stenter (ssymtab.c), 9 lines

- Form found: `if (*name != 0) { ... } else { ... }` (51 > 11). `OLD_D2CV` (vc) gives the reload of `name` for
  `hash(1000, name)` (11 > 9).
- Left: `p2->s_12 = blklevel`: original `ldl rr2,rr8; inc r3,#12; ld r0,blklevel; ld @rr2,r0` (the address in the free
  pair rr2, the value in r0); sccvc `ldl rr0,rr8; inc r1,#12; ldl rr6,rr0; ld r2,blklevel; ld @rr6,r2`, already in the
  raw code. The statement before (`p2->s_7 |= 6`, value in r2) takes rr0/rr6 in both.
- Tried: `name[0]`, `'\0'`, casts of the argument, `(unsigned char)*name`, `|=`, the order of the two assignments.
- Assessment: codgen, the pair for the address of a member store when r2 is free; one witness, not built yet.

### main (main.c), 6 lines

- Only the address of `sigdone` in the `signal` calls; follows from the lengths of the functions in front. Expected
  to vanish when the others are exact.

### sgetnode (getops.c): solved

- No `return (np)` behind the switch (the value is still in rr2). Lesson: a missing load of the return value at the
  exit is a missing `return`, not a popt question (jprange was the first).

### yyparse (parser.y), 2 lines

- Witness of `cparse:OLD_D6T` (with it exact up to 2 lines); the rest is data / the switch table, not looked at
  further.

### igetaddr (ifile.c), putrow (instr.c): solved (K2)

- `ldk r2,#4` is the value of a constant switch: `switch (sizeof(struct irow *)) { case 4: ...igetl()...; case 2:
  ...igetw()...; }`. sccvc forces the value into r2, drops the dispatch and keeps the matching case. Both places pass
  a pointer through the long routines of the intermediate file. putrow is exact as a whole with it (the loop order
  too). The bodies of `case 2` are dead and guessed.
- Lesson: an unexplained constant load into r2 is a switch value; a layout difference near it can follow from the
  blocks of that switch.

## 4 Quirks of the original found in cas (host/KNOWLEDGE.md, quirks table)

- `opoff`: `mknode1(3, (unsigned char)value)` passes a word where a long is wanted.
- `genwop`: dead branch `if (segmode != 0 && flag != 0) v |= 0x80000000L`.
- `jprange`: no `return` behind the switch, returns tok by chance (r2).
- `exprreloc` case 0x24: `if (v & 8) v |= 0; else v |= 0;` (a compare whose result nothing uses).
- `sgetnode`: no `return (np)`.
- `igetaddr`, `putrow`: `switch (sizeof(pointer))`, a dead case for a pointer of 2 bytes.

## 5 Solved by vc switches with the owner's exception (2026-11-10)

All three measured on the whole model (no exact function lost), the matrix confirms the witnesses, checkall ALL OK.
Descriptions in host/OLDCC.md (D2, D4); the attempts in OLDCC_LOG.md 2026-11-10.

- symtype (K5), `OLD_D2S` (allo3.c): a word in r3 widened to long takes the pair rr2 (`ldk r2,#0`). Source form with it:
  `unsigned n; ip->s_2 = n;` (with `(long)(unsigned)n` the D3P rule reloads n). The first version for any odd
  register cost locadv (r7 is widened into a new rr2 in the original), opshort, asmins; restricted to r3.
- igetexpr, sgetexpr (K3), `OLD_D4A` (reader3.c store): in `return (f(g()))` the only argument is not stored in a frame
  temporary. As a statement (`p2rep(igetw());`: p2tag, yyparse) and with two arguments (`mknode1(3, igetl())`) the
  original does store, so the rule is limited to RET. The first try in argplace had no effect (the temporary comes
  from stoarg).
- asmword (K14) and the reload of `name` in stenter, `OLD_D2CV` (allo3.c): the copy of a name in a register holds no
  name, the allo3.c part of v0's `OLD_D2C` (the whole of D2C does not build in vc: its match3.c part needs OLD_D2V).
  Two witnesses.

## 6 Open decisions

- codgen: stenter (`p2->s_12 = blklevel`, one witness), next to be built under the owner's exception.
- popt order: yylex, jpr, stabout, exprreloc; no mechanism found (section 2).
- main follows from the lengths in front; yyparse is the D6T/data witness.
