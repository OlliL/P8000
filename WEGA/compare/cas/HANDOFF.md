# cas: handoff — jpr

`/bin/cas` (src/cmd/cas, compiler `sccvc`): 151 of 152 functions strictly exact in the model; checkall ALL OK. The one open
function is **jpr** (jpr.c, original 0x84fc, 324 bytes, 24 lines). Every attempt is logged in `host/OLDCC_LOG.md`
(search "jpr"); this file holds only what is needed to go on with jpr.

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
1. Trace the 8-line path (`trsa.sh`-style dump with a TRACE veto spopt) and look for a variant where only the prologue
   (not the exit) goes behind plain, or init leaves T by a first inversion.
2. Turn the three vetoes into rules (features of the moves) and check them on the model (`condrun.py`).
3. Then switch(es) in vc, checkall, matrix.
