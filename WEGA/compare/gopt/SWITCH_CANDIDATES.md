# gopt: switch candidates (best guess, not built)

Owner 2026-11-20: during the source phase no new switch is built; a difference that looks like a compiler matter is
classified here with its witnesses, and the search for source forms goes on. A candidate is only taken up after the
source forms of all its witnesses are exhausted (10+ forms each, counted in findings.json).
Status: `candidate` (not built), `built` (exists, its witnesses get source forms again), `dropped` (tried, OLDCC_LOG.md).

| Id | Guessed mechanism (pass) | Witnesses (DIFF now) | Status |
|----|--------------------------|----------------------|--------|
| K1 | a parameter at a label behind a jump: the original reloads it from the frame where ours takes its register back (scodgen D3J) | cmwalk (3), checkops (21) | candidate (two OLD_D3JN rules dropped) |
| K2 | `]` between a jump and its label counts as a record (scodgen D3J) | prquad (exact) | built OLD_D3JB; source forms of prquad to retry |
| K3 | a static in the body: the parameters live in the frame from the start (scparse/scodgen) | mkleaf, qmkstore (exact), operand (28) | built OLD_D3S; source forms to retry (register/address forms of the parameter) |
| K4 | which copy of a common tail stays: the original the first, ours the later one (spopt xjump/comjump order) | setsym (8), vnumber (38), insrtlive (7), tprint (11; 8 forms all equal), indupdate (exact by a form) | probably source: indupdate exact with the common call in both branches (comjump keeps the first copy); retry the others so (OLD_D67, "no cross jump into a later copy" dropped) |
| K5 | a dead copy of a parameter at the first use (`ldl rr0,rr6`, `ld r3,r2`; cause 1) (scodgen) | mkconstopnd (2), affsize (3), setedges (12: `ld r3,r7` before the mult of the parameter i; 6 forms of GF), canshare (14, the same) | candidate |
| K6 | a store of an 8 bit field of a frame struct goes directly to the slot (scodgen fixfld) | setsymtable (form `((char *)&tq)[4]` found, guessed) | candidate |
| K7 | a pointer/global is reloaded where ours keeps it in a register (register cache; cause 5) | compress (11), eval (9), linkp (34), dominators (17), chklpdefs (51, one reload) | candidate |
| K8 | the pair of a temporary: rr4 against rr6 (cause 6) | srchdflist (5), joinblk (9), testbit (22), deledge (4), prcgraph (11: `ldl rr2,rr6(r3)` against rr0 for the pointer loaded from the GF table; 6 forms) | candidate |
| K9 | the entry store of a word parameter (tstlbit) | tstlbit (2) | candidate |
| K10 | `(q = q->next)->op` with the displacement form (cause 4) | writeil (6) | candidate |
| K11 | a chain `a = b = x` copies the value into another register first | setsymtable (2) | candidate |
| K12 | after a store through a copy (`ldl rr6,rr0; ldl @rr6,rr2`) the original reloads the pointer from the frame, ours takes rr0 | updatelooplist (5) | candidate |
| K13 | the then-block of an if is placed before its test, right behind an unconditional jump, the test jumps back to it (spopt codemove, which block moves) | buildgraph (17), clropt (33), setsym (8; with default last the sharing is right, default is not moved behind the dispatch) | candidate |
| K14 | a long `&`/`|` with a frame operand: the original takes r1 for the low word (`ld r1,lo; and r3,r1`), ours overwrites r3 (`ld r3,lo; and r3,r3`: wrong code) (scodgen template) | constfold (2 places) | candidate |
| K15 | the EPU store of an int-to-double result is not merged with the store of the first case | constfold (1 instruction) | candidate |
