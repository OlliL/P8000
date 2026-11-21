# gopt: switch candidates (best guess, not built)

Owner 2026-11-20: during the source phase no new switch is built; a difference that looks like a compiler matter is
classified here with its witnesses, and the search for source forms goes on. A candidate is only taken up after the
source forms of all its witnesses are exhausted (counted in findings.json). Only open witnesses are listed: a witness
that became exact leaves the list (its form is in findings.json and host/increments/160-programs.md), a candidate
without witnesses leaves it too (2026-11-20: K3 = OLD_D3S, K9, K10 went by source forms: dead code, the comma form; K7 by OLD_D2N extended to pairs (2026-11-21); K16 by vc OLD_D2TC, K14 by vc OLD_D2XP extended, K15 by vc OLD_D4F (2026-11-21); K8 by OLD_D2R (the half OLD_D2V refuses goes back) and OLD_D3AW (an idle parameter in the register of an argument goes back) (2026-11-21); K5 by OLD_D2M (a division) and OLD_D2P (segmented parameter words merge only without a free pair) (2026-11-21); K1, K17, K12: a lone label, a dead goto to a label or a dead store to the parameter makes codgen reload it (checkops, cmwalk, deledge)).
The numbers are the DIFF lines of fnc.py (2026-11-21, 320 of 321 OK).

| Id | Guessed mechanism (pass) | Open witnesses (DIFF) | Status |
|----|--------------------------|-----------------------|--------|
| K6 | a store of an 8 bit field of a frame struct goes directly to the slot (scodgen fixfld) | setsymtable (2; the form `((char *)&tq)[4]` is a guess) | candidate |
| K11 | a chain `a = b = x`: the original copies the value into another register first (`ld r2,r0`) | setsymtable (2; 13 forms) | candidate |

Observation 2026-11-20 (K7 and K8 one mechanism, a guess): in every witness of K7 and K8 (linkp, dominators, eval, compress,
joinblk, rmlabels, prcgraph, srchdflist, setposition, pr) ours keeps the address of a global pointer cached in rr6
(`ldl rr0,rr6` later) and so takes rr4 (or r3, rr0) for the next temporary; the original loads the global again and
reuses rr6 (or rr2). Source forms tried without effect: a lone label in front of every statement (sweep), subscript and member syntax (a[b], b[a], *(a+b), (*p).m: one
tree), operand order, dead stores to every parameter (and pairs), dead gotos. Best guess: the register cache of our
scodgen keeps a global's address across the next expression where the original drops it.
Built 2026-11-21 (owner) as vc `OLD_D2NP`, the same day folded into v0 `OLD_D2N` (a global loaded into a pair); left of K7/K8: prcgraph, chkfamindvar,
setposition, pr, rmlabels (2). Later the same day pr by OLD_D2P and prcgraph by OLD_D2Q (both extended, OLDCC_LOG).
The last two (2026-11-21, register traces) were other mechanisms: in chkfamindvar our scodgen left r4 busy (OLD_D2R gave
the pair back, took r4, OLD_D2V refused it), so the next pair was rr6 with the parameter r; in setposition the parameter X
stayed in rr2 while the argument was wanted in r3.

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
| OLD_D2P (no parameter pair split, v0/vc, 2026-11-21) | 1: pr | - |
| OLD_D2Q (odd index, vc, 2026-11-21) | 1: prcgraph | - |
| OLD_D2MO (not a parameter, v0/vc, 2026-11-21) | 3: canshare setedges testbit (K5) | - |
| OLD_D2P (no merge of parameter words in r4..r7, v0/vc, 2026-11-21) | 1: testbit | all parameter words: mkconstopnd 2>65 |
| OLD_D2TC (vc, 2026-11-21) | 1: operand (K16) | the original statement does not compile without it |
| OLD_D2XP (RESC1 only, vc, 2026-11-21) | 1: constfold (K14, with OLD_D4F) | - |
| OLD_D4F (vc, 2026-11-21) | 1: constfold (K15) | - |
| OLD_D2R (OLD_D2V gives the refused half back, 2026-11-21) | 1: chkfamindvar (K8) | - |
| OLD_D3AW (an idle parameter in the argument register goes back, 2026-11-21) | 1: setposition (K8) | - |
| OLD_D2M (a division of a name in its register, 2026-11-21) | 1: affsize (K5) | - |
| OLD_D2P (segmented parameter words merge only without a free pair, 2026-11-21) | 1: mkconstopnd (K5) | - |
| OLD_D2N (pair part, 2026-11-21) | 6: compress dominators eval joinblk linkp srchdflist (K7/K8) | rmlabels 2 instead of 12 |
| scparse OLD_D12R | data only (constfold's static return area) | not measurable by fnc.py |
