# Handoff: WEGA build chain back to C

Status 2026-11-01. Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file. The section "Current work" below says what is being done now and what to read for it; the rest of this file is
the inventory and the decisions of the whole project.

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA one: `cc`, `scc`,
`cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `gopt`, `popt`, `cas`, `as`, `ld`, `sld`, `gld`, `ar`, `ranlib`,
`strip`, `size`, `libc` and `crt0`, built from our own C sources and checked byte for byte against the originals.
The original binaries run under the emulator (`host/z8k-emu`, the oracle); they are not part of the tool chain.

Byte identity is the only guarantee of identical behaviour, so it stays the goal (owner decision). Where the
original was built by an older compiler revision (`OLDCC.md`), byte identity needs that compiler, which does not
exist any more; this is the open task of step 3 below.

## Inventory: what belongs to the tool chain

Source of truth for the state: `../src/TODO.md` (per program) and the check commands in the last column.
"Source from binary" = our source was written from the original binary, not from a source tree.

| Tool | Original (WEGA 3.1 tree) | Original filesize | WEGA source (`src/`) | Host version (`host/`) | Source from binary | State | Check |
|---|---|---|---|---|---|---|---|
| `cc`, `scc` (driver) | `/bin/cc`, `/bin/scc` | 14208, 14246 | `src/cmd/cc/cc.c` | `host/cc` | no (WEGA source) | done: 74 command lines identical; `cc` and `scc` byte-identical | `make -C host check` (cc) |
| `cpp` | `/lib/cpp` | 16522 | - | - | - | not started | - |
| `cparse`, `scparse` | `/lib/cparse`, `/lib/scparse` | 67520, 67826 | `src/cmd/cparse` (K&R, from the Plexus PCC base, owner decision) | - | yes | all functions written; corpus 259/259 (behaviour, built with the shipped cc and with ccv0p); byte identity with the old compiler v0p: 173 of 179 functions strictly exact (2026-11-01); scparse not started | `src/cmd/cparse/tools/oldcc.sh model`, `corpus` |
| `codgen`, `scodgen` | `/lib/codgen`, `/lib/scodgen` | 80884, 83554 | `src/cmd/codgen` (K&R, WEGA `cc`; byte identity is the goal, D1 to D16 open) | `host/codgen` (ANSI, gcc `-Wall`; `-DSEG` for scodgen; functional equality) | yes | host: corpus 100/100 identical for codgen and scodgen; large corpus (213 source files of `src/cmd` and `src/lib`) 213/213 for both, 3 files not comparable. WEGA side: corpus 101/101 (behaviour), byte identity open | `host/codgen` (`host_corp.sh`-style test, see `src/TODO.md`); `src/cmd/codgen/tools/check.sh`, `corpus.sh` |
| `gopt` | `/lib/gopt` (only with `-Ol`/`-Or`) | 147286 | - | - | - | not started | - |
| `popt`, `spopt` | `/lib/popt`, `/lib/spopt` | 25620, 25880 | `src/cmd/popt` (K&R, `z20.c` to `z23.c`, `stdio.h`; the bss order from the definitions at the end of `z2.h`) | `host/popt` | yes | host: 522 corpus runs byte-identical; src/cmd: the normal build gives the output of /lib/popt on the popt corpus (816 of 816 runs); byte identity with the old compiler v0p: 56 of 73 functions strictly exact, code, data and bss of the original's size (2026-11-01) | `make -C host check` (popt); `src/cmd/cparse/tools/oldcc.sh model popt` |
| `cas` | `/bin/cas` | 89152 | `src/cmd/cas` (`MODULES`) | `host/cas` | yes (decompiled, see `src/cmd/cas/README`) | done in behaviour; code not byte-identical (older scc) | `make -C host check` (cas) |
| `as` | `/bin/as` (PLZ/ASM) | 74536 | - | - | - | not started | - |
| `ld` | `/bin/ld` | 21732 | - (source in `host/ld`) | `host/ld/ld` | yes (`ld.c` from the binary) | done: 146 command lines identical | `make -C host check` (ld) |
| `sld` | `/bin/sld` | 46074 | - (source in `host/ld`) | `host/ld/sld` | yes | done: 91 command lines identical | `make -C host check` (ld) |
| `gld` | `/bin/gld` | 27616 | - (source in `host/ld`) | `host/ld/gld` | yes | done on the check lines | `make -C host check` (ld) |
| `ar` | `/bin/ar` | 12396 | `src/cmd/ar.c` | `host/ar/ar` | to be checked | done on the host (archives identical) | `make -C host check` (ar) |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | 7732, 9072, 7732, | `src/cmd/ranlib`, `strip`, `size` | `host/binutils` | to be checked | done on the host; the `src/cmd` versions still differ (`src/TODO.md`) | `make -C host check` (binutils) |
| `libc`, `slibc` | `/lib/libc.a`, `/lib/slibc.a` | 90484, 110938 | `src/lib/libc` (170 modules in C), `src/lib/libc/oldcc` (10 modules as `.s`), `src/lib/oldlibc` (versions) | `host/libc` | partly | 170 of 180 from C; 7 differ (4 older cc, 3 other revision); 3 without C | `make -C host check` (libc), `make -C host/oldcc verify` |
| `crt0` (start-up files) | `/lib/crt0.o` and others | 226 |  `src/lib/libc/crt` (`crt0.list`, `mkcrt0.sh`) | - | to be checked | generated; start code checked against the original | `src/lib/libc/crt` |
| `yacc` | `/usr/bin/yacc` (used by `verify-wega`) | 37224 |  - | - | - | not inventoried | open |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

Overall checks: `make -C host check` (all tools except `codgen`), `make -C host verify-wega` (builds `src/cmd` with the
original tools and compares each program).

## Decided: equality criteria per code base

- `src/cmd` (K&R C, WEGA `cc`): byte identity with the original is the top priority. Original defects are rebuilt first and recorded in the quirks table in `KNOWLEDGE.md`; a fix needs the owner's decision.
- `host/` (ANSI C, gcc): functional equality only. Fixing defects of the original is allowed where the original's result is undefined or wrong; the fix is documented in the quirks table and in `src/TODO.md`.
- Tests for `host/`: textual comparison with the original where the original is well defined; for cases with undefined behaviour in the original, a functional test (a test program run under the emulator).

## Decided: two independent code bases

- `host/` holds ANSI C that compiles with a current gcc and `-Wall`. It is checked by behaviour (check lines, corpora).
- `src/cmd/` holds K&R C that compiles with the WEGA `cc`. It is checked by behaviour and by `cmp` of the built binaries.
- The two are functionally equal but have no relation to each other: no shared sources, no `#ifdef`, no build switch.
  A tool that exists in both worlds is written twice; a copy from `host/` to `src/cmd/` is a new K&R source, not a transfer.

## Decisions open for the owner

- **Compensating source changes.** Some changes adapt the source to the old compiler instead of correcting it:
  `conval` (case order), `sucomp` (storage classes), `zzzcode` (if/else swap, commit `cc1df27`), `tshape` (case order),
  `clocal` (casts). They are behaviour-neutral but not justified for the source. Keep or revert? This is decided in step 2 below.
- **`host/oldcc`** stays: it checks the ten `.s` modules that the old cc produced. Its paths are corrected.
- **`yacc`:** should it be added to the inventory and to the tool chain?

## Current work (2026-11-01): /lib/cparse and /lib/popt byte-identical through the old compiler v0p

**What.** `/lib/cparse` was built by an old revision of the C compiler of which no binary exists. We rebuild that
compiler as switches `OLD_Dn` in our sources of the passes (cparse, codgen, popt) and compile our cparse sources with
it; the goal is a byte-identical `/lib/cparse` (owner: "oberste Direktive"). The version that built the passes is
**v0p** (v0, the compiler of the 1982 programs, plus switches only the passes show). Driver `ccv0p`, passes in
`src/cmd/{cparse,codgen,popt}/v0p`, installed into the oldroot tree (`make -C host oldroot`).

**State (2026-11-01, end of the session).** 173 of 179 functions strictly exact, 96 differing lines; data and bss as
in the original under the D12 model; code 48640 against 48896 bytes. cparse built with ccv0p passes its corpus
(259/259). The bench of v0 (System III programs of 1982 with ccv0): 118 of 153 functions, 275 lines. The
per-function list: `oldcc.sh model; oldcc.sh lines`.

**Open in /lib/cparse, and why** (details and every refuted attempt in `OLDCC_LOG.md`, 2026-11-01; do not repeat
them without new evidence):

1. **popt block placement, 86 lines: optim 51, argregs 27, defid 8.**
   - optim: the block of `else if( LCON(p) && RCON(p) && conval( p->left, o, p->right ) ) goto zapright;` stands in
     the original behind the end of the PLUS case (`break` = `jp 0x569C`, the return) and falls into zapright; ours
     moves the zapright block up to the jump. argregs: the same picture: `L674: ld r2,r13; jpr L676` goes in front of
     the switch dispatch at the end (the original keeps the dispatch there), ours moves the dispatch up. So the old
     popt sometimes moves the jumping block in front of its target instead of the target up to the jump.
   - defid: the loop step `dec 0x16(fp),#2; jp head` of the SMEMB loop stands in the original right behind the
     `jp mismatch` at the end of the class switch (in front of redecl); ours keeps it behind the loop body. The source
     order with the mismatch block in front of redecl does not work (the CJP over the JP is turned round first).
   - Why open: the popt trace with block-move features (283 moves over all modules) shows no feature that separates
     the wrong moves from correct ones in exact functions; every rule tried (no move for multiply named labels, the
     jumping block in front of the target, the switch value at the dispatch) costs hundreds of lines elsewhere.
     cparse alone has too few witnesses.
2. **buildtree, 6 lines.** 4 lines: the double constant 0.0 (`r->dval == 0`) lies in the data behind the first switch
   table (0x2846), ours in front (0x27EA): the order in which cparse writes the .D file (constant at the statement,
   table at the end of the switch). 2 lines: the temporary of `buildtree( COLON, ll, lr )` is at 0x12 in the
   original, ours 0xC; it fits a cparse that does not give back all the frame space of the member block (cp2, cp1, m)
   at its end; moving these variables out of the block makes the frame 2 bytes larger. One witness each: not built.
3. **argdebug, 2 lines.** The original renames the unused register variable `a` (r14) into r1 and drops its save
   (`ld r1,r7`); our popt renames only registers saved with ldm pairs (procopt), and its rmove removes the restore
   first. A popt rule with a single witness: not built.
4. **yylex, 2 lines (one instruction).** `jp 0x21AA` where `jr` reaches: the early jump optimization of an older cas
   (Band B 3.4.5: open forward jumps become long when the optimization runs before their label). Evidence in four
   originals (cparse/scparse yylex, popt rmove, deroff, scodgen): one trigger point per cluster, always at a `call`.
   The shipped cas and the ZEUS cas (12/1986) do not show it; no table model fitted on the cparse input reproduces
   it without false long jumps. Parked as an explained remainder (time box).

**Plan (owner decision 2026-11-01): codgen and popt next.** `/lib/codgen` and `/lib/popt` were built by the same
compiler v0p (OLDCC.md, "v0p"). Make our sources of codgen and popt compiled with ccv0p byte-identical with them,
with the same method as cparse; the hope is that their many more functions give the witnesses that are missing for
the open cparse points (and every switch must keep cparse as it is):
- popt placement (point 1): codgen and popt have many switches and gotos; the placement rules of the old popt can be
  checked on them before cparse is touched.
- argdebug (point 3): leaf functions with one register variable in codgen/popt would show whether the old popt renames
  a single saved register.
- buildtree (point 2): only if codgen/popt have double constants and blocks with own variables (cparse behaviour of
  v0p: the .D order, the frame offsets of blocks); likely few.
- yylex (point 4): popt rmove already shows the cas effect; more clusters in codgen/popt give more trigger points
  for a model of the old cas (each needs an exact .s around it).
Order (owner decision 2026-11-01): **popt first, then codgen.** popt is small (z20.c .. z23.c, 25 KB against 80 KB
and 206 functions of codgen), so the strict comparison for a second program is built there first and then reused;
it is nearly all switches, loops and gotos (the placement question of point 1), its rmove has the clearest cas
cluster (point 4), its small leaf functions may answer point 3.
1. (done 2026-11-01) Set up the popt model: progcfg with ORIG_BIN (/lib/popt) and STRICT_INPUT, a link of our objects built with
   ccv0p with the original's data layout where D12 demands it, strictcmp (the function map is in
   `src/cmd/popt/tools/findings.json`); a first measurement with the current v0p.
2. (under way, see "State of popt") The same loop as for cparse: source forms first, switches with two witnesses; every switch also checked on cparse
   (`oldcc.sh model`, corpus) and on the v0 bench. The normal builds of popt and codgen stay as they are (they are
   the shipped programs).
3. Then codgen with the same frame, and back to the open cparse points with the new witnesses.

**State of popt (step 1 done, step 2 under way, 2026-11-01).** Our popt sources compiled with ccv0p against
/lib/popt (`oldcc.sh model popt; oldcc.sh lines popt`, run in src/cmd/popt): 56 of 73 functions strictly exact,
735 differing lines (rmove 410 of them, see "Now: rmove") (start of the day: 1 of 68, 6305 lines). Code 22272 = 22272, data 3308 = 3308 and bss 3420 =
3420 bytes (the definitions at the end of z2.h give the original bss order; the libc is linked in the order of the original, Makefile
LIBCOBJS). After every source change: the normal build (shipped cc) against /lib/popt on the popt corpus
(`src/cmd/popt/tools/corpus.sh` after `make popt` with the WEGA tools: 272 inputs times no option, -l, -d;
816 of 816; the corpus is RECON_TMP/popt/corpus, which can be made
with `make -C host/popt corpus CORPUS=$RECON_TMP/popt/corpus`), then `oldcc.sh build popt v0p` and the cparse
model (must stay 173). When a change alters the behaviour of popt, the five popt switches OLD_D20C/X/S/L/K are
checked again by dropping each one (owner request: a switch must not hide an error of our popt source; last check
after ovlap: all five still needed). The method that worked: read the original function (`olist.py`-style
listing, the jump tables in the data give the case order), rewrite the source form (register declaration order
fixes the registers, also after the leaf renaming of popt; `if (!*p++)` against `*p++ == 0`; a single `return -1`
at the end; parameters with register copies give the homes of the original; `++p->x` where a statement
`p->x++` leaves dead code with codgen v0p; function statics in the order of their functions); when the rest looks
like layout, compare the instruction multisets of both sides (usesreg: equal but for jr/jp). Original bugs found
on the way are in the quirks table of KNOWLEDGE.md (natural, adrlen, oplook, equop, fixldm, markregs,
rename_regs, five dead functions: dumperr, outerr, regof, ishigh, nwords; the write-only global rmnode). Every difference of the compiler found on the way goes
at once into OLDCC.md section 3 as "Open" under its class (owner rule), with or without a switch. Details:
OLDCC_LOG.md 2026-11-01 "popt model".

**Open popt functions** (strict diff lines in brackets; the owner may clean up this list as functions get done):

- equop (2)
- findcon (2)
- repladdr (2)
- setcon (2)
- itoa_ (4)
- regof (4)
- movedat (5)
- codemove (6)
- backjmp (7)
- killreg (7)
- reverse (8)
- xjump (11)
- dest (18)
- setreg (22)
- procopt (48)
- usesreg (177)
- rmove (410 lines; content equal but for jump forms and two string addresses: layout, D20)

Known causes (OLDCC.md section 3, "Open (popt, v0p model)"); none of them is a source error any more, they wait
for the codgen work and the placement question:

- codgen, D3 (the value of an assignment in the next statement): findcon, repladdr, setreg (also refcount, exact
  with the assignment inside the expression).
- codgen, D3 (a parameter kept in its register across a branch): setcon, equop, regof.
- codgen, D2 (scratch register r1 against r3): itoa_, reverse.
- codgen, D16 (`--n == 0` of a frame slot stays in memory in the original, OLD_D16A loads it): codemove.
- codgen, D5 (the member of an element of a local array of structures, the STREF form of OLD_D5S): procopt.
- popt, D20 (block placement, loop inversion): backjmp, killreg, movedat, xjump, dest, setreg, usesreg (dest also
  the old popt's register knowledge, `sub r2,r3`).
- Under analysis: rmove (see "Now: rmove" below; it also holds the cas cluster of the open cparse point 4).

**Now: rmove, under analysis** (z22.c, original 0x13b6, 3272 bytes; the only function with a large remainder,
1111 lines on 2026-11-01). Listing: `python3 tools/olist.py rmove`, diff: `python3 tools/strictcmp.py -v rmove` (in
src/cmd/popt). The jump tables of rmove are in the data at 0x6e6 (the values 10, 11, 14, 15, 45, 46 of a cpir switch
with their targets) and 0x6fe (the main switch on p->op, 52 entries, index op-1). Found so far (OLDCC_LOG.md
2026-11-01, "rmove, analysis started"): only about 190 of 645 instructions differ in content, the rest is layout;
the content comes from the variables. Done: r12, r11, r10 register variables, `char b1[20], b2[20]`, the unused
`s` removed, the write-only global `rmnode` (0x1846) set at the top of the loop, no `op` (the switch is on p->op,
its default goes to `reset`; the explicit range test of our source is the one codgen makes): 1106 lines. Step 2 done: no `n` (p1 also holds p->back and the new
nodes; `p->forw` written out where p1 meant it), `int ty` in front of b1 (frame word 0), the findcon values of
do_cjp in t and r, `copy(1, s)` with two arguments: the instruction multisets now differ in 76 instructions (from
190); the strict lines rose to 1208 because the layout moved. Measure rmove by the multiset (`python3 tools/msdiff.py rmove
jumpsw` in src/cmd/popt), not by the line count, until the content is equal.
Step 3 done: `t = strlen(b2);` as its own statement in front of the two strncmp of do_lda (the
frame is 0x38 now, as the original), do_cjp reads `p->back` every time (no p1 there): 61 instructions of content
left (`python3 tools/msdiff.py rmove jumpsw`). Step 4 done: the sections behind the dispatch (each ends
with a goto) in the order of the original's code (from the table at 0x6fe): do_ld, do_clr, do_dec, do_inc, do_and,
do_arith, do_push, do_pop, do_srl, do_sra, do_sla, do_tst, do_cp, do_cjp, do_lda: rmove 549 lines, 48 instructions
of content. Step 5 done: the `r12 == 0` arm of do_cjp uses p1 (= p->forw, not reassigned there) as the
original: rmove 519 lines, 40 instructions of content; our rmove is 88 bytes shorter than the original (3184
against 3272): some code of the original is still missing in ours. Step 6 done: disp tests DIV/MUL twice as the original
(once for the setreg choice, once for `ccloc[0] = 0` against `setcc(RT2)`): 36 instructions of content, rmove 528
lines (3204 bytes). Step 7 done: sh8 starts with `if (p->op == SRA) dualop(p);` (the dualop of do_sra
moved behind the label, as the original): 35 instructions of content, rmove 530 lines (3210 bytes, 62 short).
Step 8 done: push_end tests `isreg(RT2 + 1)` (not RT1), and the `r12 == 0` arm of do_cjp goes on with
`p = p->back->back` (host/popt has `p->back->forw`, which differs from the original; the corpus does not show
it): 33 instructions of content, rmove 526 lines. Step 9 done: do_lda tests `isreg(RT2) < 0` directly (no r):
rmove 523 lines; what is left of the content is nearly all jump forms (jr/jp, layout), the two jump table
addresses (data order) and about 20 string addresses. Step 10 done: the string pool order of the original
shows that do_clr stands behind do_cjp in the source (popt moves its code up to where the original has it):
section order do_ld, do_dec, do_inc, do_and, do_arith, do_push, do_pop, do_srl, do_sra, do_sla, do_tst, do_cp,
do_cjp, do_clr, do_lda: rmove 453 lines, 14 instructions of content left (4 of them string addresses, the two
jump table addresses, the rest jump forms). Step 11 done: in clr_cc the register branch first (`if (r12 >= 0)
{ savereg } setcon`): 13 instructions of content. The strings of z22.c are now in the original's order
except the cpir table of rmove (data layout). Step 12 done: the main switch encloses all sections and disp
(its closing brace in front of cc_clear), so its jump table comes behind the cpir table of disp as in the original:
11 instructions of content, rmove 451 lines, popt 776. Left in the content: the two "#0" of clr_cc are
passed the other way round (the original gives setcon the earlier literal, ours savereg; `if (r12 < 0) setcon
else { savereg }` is worse: 12), and our rmove has one call more (`ld r7,r14; call`: probably one dualop or
singop of p too many); the rest are jump forms (layout). Step 13 done: the JP and CALL cases as own sections
(do_jp, do_call) behind disp inside the main switch, where the original has their code (0x2054, 0x2060): rmove
441 lines, 10 instructions of content, popt 766. Step 14 done: `reset:` (clearreg; goto next) between do_jp
and do_call, as the original's default target 0x205a: rmove 435 lines. Open: `next` (`p = p->forw`) stands near the
top in the original (0x143e, behind the LD section); writing `next:` behind do_ld in the source gives the same
code (popt places it), so the source form is not the cause. Step 15 done: the LD case that becomes a CLR goes
`goto do_clr` (the original falls into do_clr, whose first statement is the singop; ours had its own singop
and `goto clr_tail`): the call counts per callee are now equal (a check: count call/calr per target on both
sides); rmove 410 lines, 8 instructions of content, popt 735. What is left in rmove is layout only: jump forms
(jr/jp) and the two "#0" of clr_cc (with the setcon branch first in the text, before step 11, the strings were in
the original's order but the jumps worse: one source form cannot give both under our popt, so it is a placement
question, D20). The source work on rmove is done for now. Next, in this order:
1. The missing 62 bytes: the `cpb @r13,#1/#0x10` tests are
   only in the original (msdiff); then the string literals of z22.c in the order of the original's pool (the
   `ld r6,#0x08..` differences, about 20); a `ld r14,4(r2)` and `ld r3,4(r2)` of the original.

2. The long `jp` of the old cas (OLDCC.md, open cparse point 4) is in rmove: one `jp` where `jr` reaches, trigger
   at a `call`; record it, do not try to build it here.
Lessons of the other functions that will apply: the case order from the jump table, register declaration order,
`++p->x`, `return a || b`, a single return at the end, macros for repeated code (ovlap SCAN: the loop form decides
whether popt moves the blocks), `while (*a++ == *b++)` against `for (;;) { if (...) break; }`.

**Suggested first compiler refinement (owner request 2026-11-01, not started):** D16A, the simplest one.
OLD_D16A (codgen, match) loads `++n`/`--n` of a frame slot into a register when its value may go to a temporary
(INTEMP); popt codemove `if (... || --n == 0)` shows that the old codgen keeps it in memory for a compare with 0
(`dec n,#1; test n`). Try: no OLD_D16A when the parent is a compare (EQ/NE) with the constant 0; check popt
codemove, cparse genswitch (`p[++n]`, must stay), the cparse model (173), corpus and the v0 bench.

**Done: the bss order without .s helpers (owner request 2026-11-01, finished 2026-11-02).** popt: the globals as
definitions at the end of z2.h. cparse: the globals as definitions at the end of mfile1 (cgram1, linked first, gives
the order) and an old data member (src/lib/oldlibc/data/0_1.c, _sobuf before _sibuf; run `make` in src/cmd/cparse
first, the model link needs its object). bss.s and libcbss.s are gone in both.

**Next: D16A, then the other codgen cases** (owner 2026-11-02: start with D16A; proposed order D16A, D3 parameter
across a branch, D2 r1/r3, D5S as STREF, D3 value of an assignment). Baseline: cparse 173/96, popt 56/735. The model
reports code 48640 against 48896 (cparse) and 22016 against 22272 (popt), not equal sizes; the cause is not checked.

**Tools.**

- `src/cmd/cparse/tools/oldcc.sh`: `build PASS VER [SWITCHES]` (rebuild and install one pass), `model` (the strict
  comparison of /lib/cparse built with ccv0p), `lines`, `corpus`, `bench`. Acceptance: model better, corpus 259/259,
  bench not worse for a v0 switch; neutral on the bench means v0p. Then the normal-object check: the changed source
  compiled with the shipped cc must give the same object as before (the normal build stays the shipped one).
- `src/cmd/cparse/tools/olist.py NAME`: the original instructions of a function. `python3 tools/strictcmp.py -v NAME`:
  the strict diff of one function (run in src/cmd/cparse after `oldcc.sh model`).
- popt trace: build `src/cmd/popt/v0p` with `-DTRACE` in addition to its switches (`oldcc.sh build popt v0p "<the
  switches of the Makefile> -DTRACE"`, then rebuild without), run it under the emulator as `popt + in.s out.s` on the
  codgen output of one file (`ccv0p -S` without -O, cut to one function): out.s holds the node list after every
  iterate and comjump round.
- Switches of codgen are set in `src/cmd/codgen/oldcc.h` (the native cc takes about 160 characters of -D options);
  those of cparse and popt in the version Makefiles.

**Read, in this order:** `host/OLDCC.md` sections 1 (rules, evidence rules, time box) and 2 "v0" and "v0p" (state,
switches, history), section 3 for the class you work on (D2, D3, D6, D16, D20, D22 are the busy ones);
`host/OLDCC_LOG.md` section "2026-11-01" (every attempt of the last rounds, also the refuted ones: do not repeat
them); `src/cmd/cparse/tools/HANDOFF_cparse.md` (owner decisions for cparse: commits allowed, no push, Plexus base);
`src/cmd/cparse/tools/findings.json` key `manual_classes` (the note per function: source fixes, switches, remainder);
`src/cmd/popt/tools/findings.json` (the popt function map, `v0p_note` per function);
`host/KNOWLEDGE.md` sections "Bugs and quirks of the originals" and "codgen / scodgen" (register machinery) as
reference. Working rules of the owner for this task (besides CLAUDE.md): notes on findings only in findings.json (no
finding status or attempt history in source comments); knowledge about the D classes only in OLDCC.md (rewritten) and
OLDCC_LOG.md (dated, appended), other documents do not repeat it; commits allowed in the directories of the passes and
the docs after each accepted step (English, never push); originals and build products never in git; conversation with
the owner in German.

**After codgen and popt:** back to the open cparse points with the new witnesses, then the remaining items of the
plan in `../src/TODO.md` ("Plan: towards the old C compiler").

## Next steps, in this order

1. **Complete the tool chain.** `codgen`/`scodgen` (stop line reached) and `popt` (K&R, analysed) are done as far as the
   old compiler allows; `sld` and `ld` are reconstructed in `src/cmd/sld` and `src/cmd/ld` (K&R; ld 51 of 59 functions
   strictly exact, corpus 146/146). The procedure for every further binary is `HOWTO_reconstruct.md`, the shared scripts
   are in `recon/` (`recon/README.md`). `cparse` is written (see "Current work"); gopt, cpp, gld and yacc are deferred (owner decision 2026-10-30).
   The order and the criteria are in `../src/TODO.md`, section "Plan: towards the old C compiler"; step 3 below is item 4
   of that plan.
2. **Bring the source to its natural form.** Revert the compensating changes or justify them. New baseline:
   bytes, functions, D classes.
3. **Reconstruct the old compiler.** Everything about the old compilers is in `OLDCC.md` (the edited summary: the
   classes D1 ... D22, the versions v1, v0, v0p, rules, building and running) and `OLDCC_LOG.md` (the running notes).
   State 2026-11-01: v1 (strip and size byte-identical, ld 55 of 59); v0 (bench 118 of 153); v0p (/lib/cparse 173 of
   179, /lib/popt 56 of 73, the current work above).

The stop line for the search for source forms is in `../src/TODO.md` ("codgen / scodgen: status").
