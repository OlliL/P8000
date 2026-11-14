# cas: handoff

State 2026-11-14: `/bin/cas` (src/cmd/cas, compiler `sccvc`) is byte-identical in the real build (checkall); all 152
functions are exact in the model. The last open function was **jpr** (jpr.c, original 0x84fc); sections 1 to 9 are the
history of the search, section 10 says how jpr got exact, section 11 what the libc part needed, section 12 what is left.
Every attempt is logged in `host/OLDCC_LOG.md` (search "cas jpr").

## 1 Setup

- spopt of vc: the shipped popt with `OLD_D13 OLD_D45 OLD_D20S OLD_D49` (segvc rule of `src/cmd/popt/Makefile`). The other
  passes of vc: `host/OLDCC.md`, "The old scc of cas". All layout decisions of jpr are spopt's: scparse/scodgen only give
  the raw order (`sccvc -S` without -O), cas only chooses `jr`/`jp`.
- Model: `RECON_PROG=cas RECON_TMP=$T CCHOST=<cc wrapper> CAS=<wrapper of /lib/old/cas> python3 host/recon/d12model.py -q`
  (`D12_ONLY=jpr` recompiles only jpr.c, 7 s; `D12_SRC=dir` another source copy), then `host/recon/strictcmp.py [-v jpr]`.
  The original: `RECON_PROG=cas python3 host/recon/odis.py 84fc 8640`.
- Tools in `host/recon/poptveto/` (README there; set the scratch path `S` in each script, written as `/tmp/SCRATCH`):
  `pb.sh` (spopt with other defines, `PSED`/`PPY` patch z21.c), `condrun.py` (rule variants on the whole model, 8 at a
  time), `refuse.py`/`refuse2.py` (codemove refuses a move under a C condition over TRACE features), `vpatchX.py`
  (veto popt: every decision of iterate/comjump numbered and skippable, forward inversion as optional event),
  `pveto.py` (parallel veto search, 8 slots, pattern check "plain behind A", `bfs(..., start, expand, anyk)`), `one.py`
  (one veto run in an extra slot, prints the diff), `par.py`/`fsearch.py` (source forms in parallel). A TRACE spopt
  (`-DTRACE`, `spopt + in.s out.s`) prints each move (`/*CM ...*/`) and the nodes after each pass.
- Pitfalls: the `tmp` of an OLDROOT copy is a symlink into the WEGA root (give each slot its own); a looping spopt keeps
  running after d12model's timeout (run the compiler under `timeout -s KILL`); `pkill -f PATTERN` also kills the own shell
  when PATTERN is in its command line (kill by PID).

## 2 The original, block by block

Names: T loop test, A case 0x36 body, plain `jpput(tok, list); return (0)`, E first dispatch, F case 0x56 (e, type, head of
the second dispatch), G case 7, H case 0x19, C error default of the second switch, D step `p = p->n_next`, I after the loop.

```
init; jr T
A    cp tok,#0x51; jr ne; error; inc ncc; jr D
plain  jpput; sub r2,r2; jr exit
C'   cp r2,#0x19; jr eq H; error            (falls into D)
D    p = p->n_next                           (falls into T)
T    testl rr12; jr eq I                     (falls into E)
E    cp 0x36; jr eq A; cp 0x56; jr ne plain  (falls into F)
F    e = ..; type = exprtype(e); cp 3; jr eq plain; cp 7; jr ne C'   (falls into G)
G    sym = ..; subl rr2,rr2; [store off]; jr D
H    cp type,7 ..; jr ne plain; ...; jr ne D; neg; jr G.store
I    ...
```

Ours (committed form): A, C', E, F, G, H with plain inside H (at the type test), step and test at the bottom (`jne E`).

## 3 What is known

- The original never moves plain: with the form `default: plain: jpput(tok, list); return (0);` in the first switch
  (order 0x36, default, 0x56) the raw code already has A, plain. Our spopt moves plain to the end of E, to the type test
  of H, or to case 3 of the second dispatch.
- The original does not move the step block D to the `continue` of A (our move M105); C falls into D.
- Derived by hand: if after pass 1 the order were `T E(…jpr plain) A plain C' D jpr-T F G H`, our ordinary loop inversion
  at D's back jump (next label F, it finds `cp 0x56; jeq F` in E) gives exactly the original.
- Rule u1 (`refuse2.py`, condition in OLDCC_LOG 2026-11-12): codemove refuses (a) a block that jumps to the exit, is named
  by more than one jump and has more than two instructions, (b) a block ending in the jump back to a loop head that is
  fallen into, at a site of one instruction. With `default: plain:` pass 1 then has `T E A plain F+d2 C' D jpr-T G H`:
  right except F. jpr 50; the model loses iputline, symreloc, stlook, exprreloc, opshort, opseg, opoff, genbop.
- Best so far: u1 with three vetoes (second dispatch not moved whole to F, two comjump pairs): **20 lines**; only C and G
  stand wrong: the original splits the second dispatch after `cp 7` (`cp 7; jr ne C'`, then G), ours after `cp 0x19`.
  Two more vetoes of any event: no better.

- The split after `cp 7` is the mark of a loop inversion that reached `cp 7` (~35 nodes from the loop head; our window
  is 24). With u1 and a window of 40 (comment nodes not counted) pass 1 gives `jr T; C'; D; T; E(..jpr plain); A; plain;
  F(..cp 7; jne C'); ..; H`: the original except that A and plain stand between E and F instead of in front of C'.

## 4 Ruled out (details in OLDCC_LOG)

D41 and partial D41; chain rule before the sweep; no codemove after a chain retarget; S2 (swap targets of `cjp; jp`);
two sweeps in iterate; inversion window 12 to 64, with or without comment nodes; forward inversion; leaving out any one
vc switch (only D45 helps, 24 -> 22, veto search the same); plain inline at each site, as copies, at case 3, at the type
test; `while` loop with the step in case 0x36; second switch nested in case 0x56 (432 forms); pcc `for` layout by hand.

- Nearest so far (2026-11-12): form `jpr_copy_adb.c.txt` (poptveto; plain as a copy in the default behind A, `goto plain`
  from the other two users to a copy at the end), the move of E to the switch head left out and the first forward
  inversion done: 10 lines, only plain stands wrong (at the end of the function instead of behind A). The copy behind A
  is moved to the end of E and xjump joins it to the end copy.

## 5 The state before the last steps (2026-11-12, evening)

Fast tools: `poptveto/jfast.py` (spopt on the raw code of jpr.c and the host cas, jpr against `compare/cas/jpr_orig.txt`,
0.2 s a run), `fveto.py` (veto search on it), `mkstate.py` (a hand-built state: blocks of the raw code in another order,
spec files `jpr_Q*.spec.txt`; base: the raw code of the form `default: plain:`, `poptveto/jpr_adb_plain`).

Worked back from the original:
- The split after `cp 7` is a loop inversion at D's back jump (next label G, found `cp 7; jeq G`). Our ivloop needs an
  instruction falling into the loop head; the jump it leaves behind lands in front of C' and is dead in the original.
  The only block of the original that ends in `jr T` is init, so init fell into T at that time and plain stood right in
  front of init.
- State Q4d (`poptveto/jpr_Q4d.spec.txt`): entry jump, A, plain, init, T E F (second dispatch up to `cp 0x19`), C, D
  with its back jump, G, H, an unnamed label, I. It gives the original exactly (0 lines, all targets) with one change to
  spopt: the entry jump (`jpr L10001`, prologue to the top) is not taken before the first comjump
  (`poptveto/entry2.py`), plus no block over a `ret` (`retstop.py`). The inversion at D's back jump leaves
  `init; jp T` behind plain; the entry move in the second round takes the prologue to the top and init with it.
- The unnamed label in front of I is needed: without it (Q4b) the inversion at H's back jump (next label I, found T's
  `jeq I`) puts the test at the bottom again (12 lines). This is the D49 mechanism (refcount keeps an unnamed label in
  front of another label). Q4 (H's tail already joined to G's store by comjump, no extra label) also gives 0.
- Q6 (`jpr_Q6.spec.txt`, init with `jp T` behind the prologue) gives 0 with the entry rule and rule iv
  (`iv.py`: codemove does the loop inversion when the block of the target ends in the jump itself).
- On the raw forms these rules give 22 to 24; veto searches with them (fixed, or as events E and V, `evopt.py`), depth
  3, six forms: 10.

## 6 Source form `goto done` (2026-11-12, night)

- The unnamed label in front of I comes from the natural form `for (p = list; ; p = p->n_next) { if (p == 0) goto
  done; ... } done:` (`poptveto/jpr_adb_plain_goto_done.c.txt`, generator `loopforms.py`): the break label of the
  loop is named by nobody, stands in front of `done:`, D49 keeps it. Shipped vc spopt: 12 (was 24); with D45 without
  exit (`d45x.py`): 10 on all plain sites, only the place of plain left. This form is the base from now on.
- Veto search (shipped spopt, all events, depth 3) on it: 8 lines with three vetoes: entry move in pass 1, E to the
  switch head, step to A's `continue`. The 8 lines: the exit code stands behind plain (codemove moved exit + prologue
  behind plain's jump because the block runs over `ret`, then init behind the prologue). The original keeps
  `plain: ...; jr exit`. With `retstop.py` that path is gone (best 10).
- So the open question is narrow: what puts init (or prologue + init) behind plain, or init with `jp T` behind the
  prologue (Q6, then rule iv), without moving the exit; and which rule replaces the three vetoes.

## 7 Next

0. State after the first moves of the 8-line path: `entry; A; plain; exit; prologue; init; T(..; jp E); E ...`; the
   original would need the same without the exit between plain and the prologue.
   Veto search depth 4 (141705 runs) with no block over `ret`, entry and iv as events, D45 without exit, form
   `default: plain:` + `goto done`: 10 at best. So without the exit move nothing comes below 10.
1. Trace the 8-line path (`trsa.sh`-style dump with a TRACE veto spopt) and look for a variant where only the prologue
   (not the exit) goes behind plain, or init leaves T by a first inversion.
2. Turn the three vetoes into rules (features of the moves) and check them on the model (`condrun.py`).
3. Then switch(es) in vc, checkall, matrix.

## 8 Night 2026-11-12/13 (mandate: memory project_cas_jpr_night_20261113)

- Raw end variants of the `goto done` form (`poptveto/tailvar.py`): prologue in front of the exit code (T1), prologue
  at the top without entry jump (T2, T3): shipped 12, D45 without exit 10, rules 10 to 12, as the raw layout.
  Veto searches on T1/T2 (depth 3, with and without E/V and retstop): 10. Point 1 gives nothing; point 2
  (scodgen) has no trace pointing to it; going on with point 3 (goto done form + D45 without exit on the model).
- Taken (point 3): `OLD_D45` without the exit as the head's target (z21.c), jpr.c in the `goto done` form; model: only
  jpr open, 10 lines (the place of plain). So the base in git is now the 10-line state; the tools' forms in
  `poptveto/` still use the old loop unless their name says `goto_done`.
- checkall on 5013824b ALL OK; vc matrix measured again (0285fbbd).
- New base (git): the 10 lines are only the order of A and plain: ours `init; jr T; plain; A; C' ...`, original
  `init; jr T; A; plain; C' ...`. Path: E is moved to the switch head in pass 1 and takes plain to its end (via the
  lone `default: goto plain` jump), the inversion at D's back jump then leaves plain in front of A. Wanted: E turned to
  `cp 0x56; jne plain` with plain staying behind A. M1 on this base: 24; M1 + FW: 10. Veto searches running.
- Veto searches on the new base (depth 3): 10 / 10 / 8 (with M1). The 8 is the exit-code path again. Every path below
  10 needs prologue + init right behind plain when the inversion after `cp 7` comes; with our codemove the prologue
  only gets there together with the exit code. Open question for the next session: how the old spopt gets the
  prologue (or init) behind plain without the exit code: e.g. the exit code as one node, a block that stops at `ret`
  but is moved with the prologue, or the entry move taking the block from the exit label. Test each as a variant of
  codemove with `jfast.py` on `gd`-like raw code (raw: `sccvc -S` of src/cmd/cas/jpr.c).
- Natural loop forms instead of `goto done` (do-while in an if, for in an if): 24 to 60, no help.
- Pass 1 as the original needs it: rules turnable (`poptveto/turnable.py`) + LJ + u1(b) give after pass 1
  `T(jp E); A; plain; E(jne plain) F d2 C D(jp T); G; H; I` (form `default: plain:` + `goto done`); pass 2 then moves
  E..D whole to T (62). Veto search on that base running (scratch fvz4.txt).
- The original layout is a fixpoint of our spopt: `poptveto/jpr_OS.spec.txt` (the original's block order built from the
  raw code of the form `default: plain:` + `goto done`, prologue at the top via `tailvar.py` T2) gives 0 lines with the
  committed spopt. Only the path to it is missing.
- 1728 generated natural forms (`jgen3.py`): 10 at best. Now `poptveto/pathsearch.py`: search over the spopt
  primitives in any order from the raw code to the original layout (the order of steps the old spopt must take).
- `pathsearch.py` (unlimited window, blocks over `ret` allowed) finds a 13-step path from the raw code of
  `default: plain:` + `goto done` to the original's loop layout (`replay2.py` prints it): d2 to F's end; E turned in
  place (`cp 0x56; jne plain`, no move to the switch head); C to d2's end; D to C's end; exit + prologue behind
  plain; init + T behind the prologue; inversion at D's back jump (next label G, `cp 7`); T's `jp E` gone; then an
  inversion with an unlimited window at the `jp exit` of the jprange switch and a move bring exit + prologue + init
  to the end again; the entry move takes prologue + init up. The last two steps are not possible with a real window;
  constrained searches (no block over `ret`, window 24/40) running.
- With real limits (no block over `ret`, window 24) the primitive search exhausts the state space without reaching
  the original: our primitives cannot reach it in any order. A primitive of the old spopt is missing or different.
  Testing candidates one by one in the model (forward inversion first).

## 9 Session 2026-11-14 (details: OLDCC_LOG, "cas jpr (2026-11-14)")

- Read all `cas jpr` entries of OLDCC_LOG first; section 4 here is only a summary.
- The ZEUS 3.21 spopt (8/84) decides like the shipped one on all of cas: no old rule to be read out of that binary.
- Refuted on the whole model: backward sweep, turns first, jump-to-next removal first, a codemove-only first sweep,
  "codemove before the turn, turn only when nothing moved" (`poptveto/s1114/`).
- stabout's original has the shape of jpr's (`init; jr T; dispatch tail; step; T; ...; first compare reversed`).
- `poptveto/jpr_Qinv.spec.txt`: a second pre-state that gives 0 with the committed spopt and the normal entry jump; the last
  step is then the ordinary inversion at `cp 0x36` (D's back jump followed by the `cp 0x56` half of the first dispatch).
- Not followed up: the raw code of vc itself (scodgen/scparse) as the cause. All four popt witnesses (stabout, exprreloc,
  yylex, jpr) are switch code; popt of 12/82 and 8/84 agree, and v0p needs no popt switch.
- `tmp/oldroot` must be rebuilt after the checkout moved (`make -C host oldroot`), else `sccvc` cannot load `/lib/cpp`.

## 10 jpr at 0 lines (2026-11-14, later the same day): how to repeat it

Result: with one new rule in the loop inversion (LIV) and three block moves of pass 1 left out, jpr is exact (jfast 0: same
lines, same jump targets). With a second rule in place of the three vetoes (WIN) jpr is exact without any veto; that rule
still loses other functions and is being narrowed.

Source form: `poptveto/jpr_adb_plain_goto_done.c.txt` (first switch `case 0x36`, `default: plain: jpput(tok, list); return (0);`,
`case 0x56: break;`; `goto plain` in case 3 and at the type test; loop `for (p = list; ; p = p->n_next) { if (p == 0) goto done;`).
Raw code: `sccvc -S jpr.c`. The natural loop `for (p = list; p; p = p->n_next)` with the same switches gives 0 as well when
the three moves and every classic inversion at the step label (I105, three times) are left out; with LIV + WIN alone it has 12.

Scripts: `poptveto/s1114/` (set the scratch path, written `/tmp/SCRATCH`). `pb.sh NAME "DEFINES"` with `PPY=` one of:
- `liv.py` (`-DLIV`): rule LIV in the loop inversion of codemove.
- `win.py` (`-DRJ_WIN=24`, optional `-DRJ_INSIDE`, `-DRJ_DBG`): rule WIN in the block move of codemove.
- `all.py` (`-DVETO -DLIV`): `vpatchX.py` (veto popt, all events) and `liv.py`; `lw.py`: `liv.py` and `win.py`.
- `ev.py SPOPT RAW.s [n ...]`: one veto run, prints the lines and the event list; `lab.py SPOPT RAW.s M111 M105 ...`: vetoes
  by event type and label (first occurrence each); `zeros.py SPOPT RAW.s DEPTH`: every veto set that gives 0.

Steps:
1. `PPY=s1114/all.py pb.sh vx "-DOLD_D13 -DOLD_D45 -DOLD_D20S -DOLD_D49 -DVETO -DLIV"`.
2. `ev.py o_vx/lib/vc/spopt gd/jpr.s 3 4 5` prints `lines 0`. The three events (numbers of that form): M111, the first
   dispatch E to the switch head `ldk; ldb; jpr E`; M105, the step block to the `continue` of case 0x36; M110, the block
   of case 0x56 (F) to the lone `jpr F` of `case 0x56: break`. `zeros.py ... 3`: this is the only set up to depth 3
   (1431 runs); LIV alone or any two vetoes: 10.
3. Without vetoes: `PPY=s1114/lw.py pb.sh w24 "-DOLD_D13 -DOLD_D45 -DOLD_D20S -DOLD_D49 -DLIV -DRJ_WIN=24"`,
   `jfast.py o_w24/lib/vc/spopt gd/jpr.s jpr` prints 0 (the same with 22 to 30 as the window).

Rule LIV (loop inversion follows a jump). The scan of the loop inversion (from the loop head, 24 nodes, for the conditional
jump that names the label behind the back jump) goes on at the target of an unconditional jump it meets, when that target
stands behind another jump and in front of the back jump (once per inversion). The stretch that is moved is then the two
pieces (head up to that jump, target up to the conditional jump); the jump and the blocks between them (here `jpr E`, A,
plain) stay behind the jump left at the old loop head, where the `jpr E` is dead code. In jpr: back jump of D, next label
G, scan T, `cpl`, `jeq I`, `ldk`, `ldb`, `jpr E`, E (`cp 0x36`, `jeq A`, `cp 0x56`, `jne plain`), F (8), `cp 3`, `jeq`,
`cp 7`, `jeq G`: node 22 of 24; straight through A and plain it would be about 35. Result: `init; jr T; A; plain; C'; D;
T; E; F(..cp 7; jne C'); G; H; I`, the original. LIV alone leaves the whole cas model as it is (only jpr open, 10).

Rule WIN (first form). codemove takes no block to a jump that stands within 24 nodes behind a loop head (a label that is
fallen into and named by a jump further on). It refuses exactly the three moves in jpr (sites 5, 14 and 22 nodes behind
T; the moves it keeps, the second dispatch to F's end and its default to the dispatch's end, lie outside). On the whole
model it loses iputexpr, sputexpr, putrow, casechk, exprreloc, exprval, p2rep, symtype, osymout, stabout, ofinish: labels
that only cross jumps name count as loop heads, and stabout's move (the `break` block to a site 5 nodes behind its loop
head, D45) is refused though the original makes it. In stabout the target lies behind the loop, in jpr all three targets
lie inside the loop (`-DRJ_INSIDE`); that limit is not yet measured cleanly.

Open: the exact condition of WIN that keeps the 151 others; the natural `for` loop (the classic inversion at the step label
must not be made; `goto done` and D49 do that now); then the switches in `src/cmd/popt/z21.c`, jpr.c in the form above,
checkall and the vc matrix.

## 11 The switches and the libc part (2026-11-14)

- In the tree: `OLD_D50` (rule LIV) and `OLD_D51` (the three refusals) in `src/cmd/popt/z21.c`, both in the segvc rule of
  `src/cmd/popt/Makefile`; described in `host/OLDCC.md` (D50, D51). D51 is narrower than rule WIN of section 10:
  (a) no block that ends in a turnable pair `cjp L; jp X; L:` to a jump within 24 nodes behind a loop head (E to the switch
  head); (b) no block to a jump that stands alone behind its label (F to `case 0x56: break`); (c) no block that ends in
  the jump back to a loop head to a site of one instruction within 24 nodes behind that head (the step to the `continue`
  of case 0x36). Each part is needed; every other module of cas gives the same object with and without both switches.
- jpr.c: the form of section 10 (`default: plain:` as the second case, `goto plain` twice, `goto done`).
- Fast test used for the narrowing: spopt of a variant on the raw code (`sccvc -S`) of all 28 modules, assembled with
  `host/cas/casold`, objects compared with those of the committed spopt (10 s; `rawcmp.sh` in the scratch, the idea is in
  `s1114/model.sh`).
- The libc part of /bin/cas (784 bytes were left after jpr): `src/lib/oldslibc`: the cut members `NAME/0_2.s` are assembled
  by the shipped `cas` (bit 31 of address constants clear); `errlst/0_2.c` (= `slibc/gen/errlst.c`) by `sccvc -S -O` and
  `cas` (both tables in front of the strings); `.comm 2,_rc` in front of `.extern __pfile` in `printf/0_2.s` (order of
  the commons). `OPEN` of `host/tools/checkall.sh` is empty now.

## 12 Left

- The loop of jpr is still `for (p = list; ; p = p->n_next) { if (p == 0) goto done; ... } done:`. The natural
  `for (p = list; p; p = p->n_next)` has 12 lines under D50/D51: the classic inversion at the step label (back jump of
  case 0x19, next label the code behind the loop) must not be made; `goto done` with D49 prevents it now. Two limits of
  the inversion tried, both change other modules (OLDCC_LOG 2026-11-14).
- D45, D20S, D49, D50, D51 have one witness each; the owner's rule for such switches (OLDCC.md, section 5) stands.
