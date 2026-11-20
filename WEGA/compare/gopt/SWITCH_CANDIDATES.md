# gopt: switch candidates (best guess, not built)

Owner 2026-11-20: during the source phase no new switch is built; a difference that looks like a compiler matter is
classified here with its witnesses, and the search for source forms goes on. A candidate is only taken up after the
source forms of all its witnesses are exhausted (10+ forms each, counted in findings.json).
Status: `candidate` (not built), `built` (exists, its witnesses get source forms again), `dropped` (tried, OLDCC_LOG.md).

| Id | Guessed mechanism (pass) | Witnesses (DIFF now) | Status |
|----|--------------------------|----------------------|--------|
| K1 | a parameter at a label behind a jump: the original reloads it from the frame where ours takes its register back (scodgen D3J) | cmwalk (3), checkops (21) | candidate (two OLD_D3JN rules dropped) |
| K2 | `]` between a jump and its label counts as a record (scodgen D3J) | prquad (exact) | built OLD_D3JB; 2026-11-20 without it 18 forms (else, ne, braces ...) none: the ] comes from the block, our codgen had the ] exception only for v0p |
| K3 | a static in the body: the parameters live in the frame from the start (scparse/scodgen) | mkleaf, qmkstore (exact), operand (28) | built OLD_D3S; source forms to retry (register/address forms of the parameter) |
| K4 | which copy of a common tail stays: the original the first, ours the later one (spopt xjump/comjump order) | setsym (8), vnumber (38), insrtlive (7), tprint (11; 8 forms all equal), indupdate (exact by a form) | probably source: indupdate exact with the common call in both branches (comjump keeps the first copy); retry the others so (OLD_D67, "no cross jump into a later copy" dropped) |
| K5 | a dead copy of a parameter at the first use (`ldl rr0,rr6`, `ld r3,r2`; cause 1) (scodgen); looks like OLD_D3KC of v0p (vc lacks it because of cas copyfile): a narrower form (copy when the operation changes the register, mult) is the guess; 11 type/order/expression forms on setedges and testbit: none | mkconstopnd (2), affsize (3), setedges (12: `ld r3,r7` before the mult of the parameter i; 6 forms of GF), canshare (14, the same), testbit (22: `ld r1,r3; mult rr0,r5` with the copy of dim, the rest follows from r1 as index) | candidate |
| K6 | a store of an 8 bit field of a frame struct goes directly to the slot (scodgen fixfld) | setsymtable (form `((char *)&tq)[4]` found, guessed) | candidate |
| K7 | a pointer/global is reloaded where ours keeps it in a register (register cache; cause 5) | compress (11), eval (9), linkp (34), dominators (17), chklpdefs (51, one reload) | candidate |
| K8 | the pair of a temporary: rr4 against rr6 (cause 6); pattern: the original frees the word parameters in r6/r7 (and rr2 of a pointer parameter) for temporaries after their frame store, but keeps a live pointer parameter in rr6; ours the opposite | srchdflist (5), joinblk (9), prcgraph (11: `ldl rr2,rr6(r3)` against rr0 for the pointer loaded from the GF table; 6 forms), chkfamindvar (9: a temporary in rr6 kills the parameter r, the original takes rr4), pr (15: the original uses rr2 of the parameter down1 for a temporary, ours avoids it), setposition (18: address of symtab[i].sclass under register pressure) | candidate |
| K9 | the entry store of a word parameter (tstlbit) | tstlbit: exact by dead code that changes i (owner's idea: a leftover of deleted code) | source |
| K10 | `(q = q->next)->op` with the displacement form (cause 4) | writeil (exact by the comma form `while (q = q->next, q->op != 0x35)`) | source |
| K11 | a chain `a = b = x` copies the value into another register first | setsymtable (2; 13 forms: two statements per case, reversed chain: the chain is right, the copy `ld r2,r0` of the original stays) | candidate |
| K12 | after a store through a copy (`ldl rr6,rr0; ldl @rr6,rr2`) the original reloads the pointer from the frame, ours takes rr0 | updatelooplist (5), mktree (8: p after `p->in.op = qdop[..]`), genquads (8: p3 after the store of b2 and after an ldir) | candidate |
| K13 | (aliasm/initaffect: switch on q->op 39, loop forms, branch orders: none; dotree exact by copying the common tail into both branches: probably source) the then-block of an if is placed before its test, right behind an unconditional jump, the test jumps back to it (spopt codemove, which block moves) | buildgraph (17), aliasm (11), chklpdefs (51: the ternary block before the q->op tests that jump back to it), initaffect (11; both: the first then-block of a for body behind the jump of the init), clropt (33), setsym (8; with default last the sharing is right, default is not moved behind the dispatch) | candidate |
| K14 | a long `&`/`|` with a frame operand: the original takes r1 for the low word (`ld r1,lo; and r3,r1`), ours overwrites r3 (`ld r3,lo; and r3,r3`: wrong code) (scodgen template) | constfold (2 places) | candidate |
| K15 | the EPU store of an int-to-double result is not merged with the store of the first case | constfold (1 instruction) | candidate |
| K16 | compiler error of our scodgen: `opnd.w4 = p->in.sname - symtab` (pointer difference of a struct member through a parameter pointer) stops with `rbusy: register to be made busy not found on rslist`; the original compiled it (operand has that code) | operand (28; 7 forms of the difference all stop with rbusy, also with the scodgen of before D3S) | candidate (a bug of the rebuild, not a switch) |
| K17 | a word parameter is not taken from its register after the entry store (the original compares/loads it from the frame; may be the common cause of K1 for words, K8 for r6/r7, K9) | deledge (4: `cp r2,0(r15)` against `cp r5,@rr2`), checkops (21: mode and q at the else if; the rest: the &t arm of opnd70c8 behind an unconditional jump, K13) | candidate (to check against the exact functions that use a word parameter in its register) |

## The switches built for gopt and their witnesses (measured 2026-11-20)

All are +0 on cas (OLDCC_SWITCHES_VC.md, which measures only cas: gopt is not in the matrix). Measured: scodgen (spopt for
D64) built without the switch, fnc.py over all gopt functions. Witness = exact only with the switch.

| Switch | Witnesses | Other effect without it |
|--------|-----------|-------------------------|
| OLD_D3JB | 1: prquad | - |
| OLD_D3PL | 1: tabfree | - |
| spopt OLD_D64 | 1: domspace | operand 28>29, rmlabels 12>14 |
| OLD_D2AV | 2: ceval, orderbl | vnumber 38>39 |
| OLD_D2XP | 2: fndparm, nodeno | checkops 21>37 |
| OLD_D2PP | 2: clrlbit, setlbit | checkops 21>55, chkfamindvar 9>13, deledge 4>7 |
| OLD_D3S (with scparse) | 2: mkleaf, qmkstore | operand 28>30; both exact without it by dead code at the start (`if (0) a = 0;`): to be replaced |
| OLD_D3JL | 3: canopt, denorm, eqopnd | - |
| OLD_D24L | 6: fndparm insrtlpdef insrtopstmp insrtstmpmove savecolors tmpsearch | aliasm, checkops, fillindvar, genquads, initaffect, setsymtable, vnumber worse |
| OLD_D2FA | 9: asgfold clearblocks linkreachdefs liveonexit marklpblocks solvelive solvereach srchdef tmpsearch | updatelooplist 5>9; better without it: genquads 8>6, mktree 8>4, rmlabels 12>11 |
| OLD_D2K | 11: addallist addblk addhdrlist alloclpdef asgfold betainit betanew include initallist labquad mkleaf | mktree, setedges worse |
| scparse OLD_D12R | data only (constfold's static return area) | not measurable by fnc.py |
