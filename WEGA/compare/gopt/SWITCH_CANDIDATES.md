# gopt: switch candidates (best guess, not built)

Owner 2026-11-20: during the source phase no new switch is built; a difference that looks like a compiler matter is
classified here with its witnesses, and the search for source forms goes on. A candidate is only taken up after the
source forms of all its witnesses are exhausted (counted in findings.json). Only open witnesses are listed: a witness
that became exact leaves the list (its form is in findings.json and host/increments/160-programs.md), a candidate
without witnesses leaves it too (2026-11-20: K3 = OLD_D3S, K9, K10 went by source forms: dead code, the comma form).
The numbers are the DIFF lines of fnc.py (2026-11-20, 293 of 321 OK).

| Id | Guessed mechanism (pass) | Open witnesses (DIFF) | Status |
|----|--------------------------|-----------------------|--------|
| K1 | a parameter at a label behind a jump: the original reloads it from the frame where ours takes its register back (scodgen D3J) | cmwalk (3), checkops (21) | candidate (two OLD_D3JN rules dropped) |
| K4 | which copy of a common tail stays: the original the first, ours the later one (spopt xjump/comjump); several were source forms (the tail in both branches: indupdate, constfold, dotree; goto to the kept return copy: setsym, insrtlive) | vnumber (38), tprint (11; 8 forms all equal) | candidate |
| K5 | a parameter copied before an operation that changes its register (`ld r1,r3; mult rr0,r5`; `ldl rr0,rr6`, `ld r3,r2`; cause 1) (scodgen); looks like OLD_D3KC of v0p (vc lacks it because of cas copyfile): a narrower form is the guess; 11 type/order/expression forms and 7 dead-code forms: none | mkconstopnd (2), affsize (3), setedges (12), canshare (14), testbit (22) | candidate |
| K6 | a store of an 8 bit field of a frame struct goes directly to the slot (scodgen fixfld) | setsymtable (2; the form `((char *)&tq)[4]` is a guess) | candidate |
| K7 | a pointer/global is reloaded where ours keeps it in a register (register cache; cause 5) | compress (11), eval (9), linkp (34), dominators (17) | candidate |
| K8 | the pair of a temporary: rr4 against rr6 (cause 6); pattern: the original frees the word parameters in r6/r7 (and rr2 of a pointer parameter) for temporaries after their frame store, but keeps a live pointer parameter in rr6; ours the opposite | srchdflist (5), joinblk (9), prcgraph (11), chkfamindvar (9), pr (15), setposition (18), rmlabels (12, with a reload) | candidate |
| K11 | a chain `a = b = x`: the original copies the value into another register first (`ld r2,r0`) | setsymtable (2; 13 forms) | candidate |
| K12 | after a store through a copy (`ldl rr6,rr0; ldl @rr6,rr2`) or an ldir the original reloads the pointer from the frame, ours takes the register | updatelooplist (5), mktree (8), genquads (8) | candidate |
| K13 | the then-block of an if is placed before its test, right behind an unconditional jump, the test jumps back to it (spopt codemove); dotree, aliasm, initaffect, buildgraph, clropt were source forms (the common statement in both branches; the then-block as `if (0) { l: ... }` at the loop top, reached by goto); for the loops: switch, loop forms, branch orders tried | chklpdefs (10: only the step block), checkops (the &t arm) | candidate |
| K14 | a long `&`/`\|` with a frame operand: the original takes r1 for the low word (`ld r1,lo; and r3,r1`), ours overwrites r3 (`ld r3,lo; and r3,r3`: wrong code) (scodgen template) | constfold (2 places) | candidate (a bug of the rebuild) |
| K15 | the EPU store of an int-to-double result is not merged with the store of the first case | constfold (1 instruction) | candidate |
| K16 | `opnd.w4 = p->in.sname - symtab` stops our scodgen with `rbusy: register to be made busy not found on rslist`; the original has that code (7 forms, also with older scodgens) | operand (28) | candidate (a bug of the rebuild) |
| K17 | a word parameter is not taken from its register after the entry store (the original compares/loads it from the frame; may be the common cause of K1 for words and K8 for r6/r7) | deledge (4), checkops (mode at the else if) | candidate |

All differences classified: delbucket, fillindvar and chkcalls, open layout, went by source forms with gotos (goto past
the cerror; the kind 2 code reached by goto from inside the kind 3 branch; chkcalls written with labels).

## The switches built for gopt and their witnesses (measured 2026-11-20)

All are +0 on cas (OLDCC_SWITCHES_VC.md, which measures only cas: gopt is not in the matrix). Measured: scodgen (spopt for
D64) built without the switch, fnc.py over all gopt functions. Witness = exact only with the switch.

| Switch | Witnesses | Other effect without it |
|--------|-----------|-------------------------|
| OLD_D3JB | 1: prquad (18 forms without it: none; the `]` of the block) | - |
| OLD_D3PL | 1: tabfree (without it: `(long)p` loaded from the frame instead of rr4; 8 cast/order forms: none) | - |
| spopt OLD_D64 | 1: domspace (a correction of the rebuild: without it spopt reads `\|_stkseg+~L1+4\|` as offset 0 and loses what a register holds; no source form can stand for it) | operand 28>29, rmlabels 12>14 |
| OLD_D2AV | 2: ceval, orderbl | vnumber 38>39 |
| OLD_D2XP | 2: fndparm, nodeno (without it the parameter pointer is changed in place, the original copies it to rr2 first: as K5) | checkops 21>37 |
| OLD_D2PP | 2: clrlbit, setlbit (without it: dead `if (0) i = 0;` gives the entry store of i but then i is reloaded (5); behind the return: ldm of all (9); 8 forms) | checkops 21>55, chkfamindvar 9>13, deledge 4>7 |
| OLD_D3JL | 3: canopt, denorm, eqopnd | - |
| OLD_D24L | 6: fndparm insrtlpdef insrtopstmp insrtstmpmove savecolors tmpsearch | aliasm, checkops, fillindvar, genquads, initaffect, setsymtable, vnumber worse |
| OLD_D2FA | 9: asgfold clearblocks linkreachdefs liveonexit marklpblocks solvelive solvereach srchdef tmpsearch | updatelooplist 5>9; better without it: genquads 8>6, mktree 8>4, rmlabels 12>11 |
| OLD_D2K | 11: addallist addblk addhdrlist alloclpdef asgfold betainit betanew include initallist labquad mkleaf | mktree, setedges worse |
| scparse OLD_D12R | data only (constfold's static return area) | not measurable by fnc.py |
