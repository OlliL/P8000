# gopt: state for the next session (read this, then compare/gopt/FINDINGS.md only where needed)

Rebuild of /lib/gopt (segmented, code in segments 4 and 5, no source known), increment 160 (host/increments/160-programs.md; the log up to 2026-11-19 is at the end of
150-libraries.md, new entries go to 160-programs.md). Compiler by measurement: sccvc (host/OLDCC.md); a function is "masked" when `fnc.py` finds it equal (segment numbers and
addresses masked), `exact` only after the link and `segcmp.py` (not tried yet).

Tools (use these, they print little; never dump disassemblies or diffs into the conversation):
- `RECON_PROG=gopt python3 host/recon/fnc.py FILE.c FUNC ...`  one line per function, the diff is in tmp/tmp/fnc/FUNC.diff
- `fnc.py -a FILE.c`  all functions of the file, only the names that are not OK
- `odisc.py -s FUNC` outline (calls, strings, size); `odisc.py -o FUNC` listing to a file (read it with sed -n a,bp)
- `host/recon/modmap.py`, `own_table_seg2.py`, `rename.py [-m]`, `findings.py gopt` (writes FINDINGS.md), `make -C src/cmd/gopt part`
- findings.json: after every function its name, file, status and note (the scratch helper upd.py is gone with the session: edit the
  json by a small script or `rename.py`).

Written: comm2, rdr2, reader2, debug2, wrtil2 (complete), quads2 (not 4:1c88), alias2 (4:6c0a..4:7368, 4:79d0..4:7bb6, checkops 4:7c68..4:8074 DIFF 25, almap 4:8074 masked, addaffect 4:831e..4:87e6 DIFF 3, initaffect DIFF 11, alalloc/alret/chkallist masked, initallist/addallist DIFF 2, pralvars DIFF 2, mkallist/checkalias/aliasmat/praldim/ind masked, affsize DIFF 3 ...; alias2.c is complete except the placeholder fn5_2bca of segment 5).
Open in the written functions: see the notes; recurring causes ((6) the register cache of a base address (compress, eval, linkp, dominators: the original loads the base of a global array each time, ours keeps it in rr4/r3); tool: python3 regdiff.py (scratchpad) removes register names from tmp/tmp/fnc/FUNC.diff and shows what is structurally different; (5) is seen again in rmlabels, deledge: cause 1 and 5 together make most of the DIFFs of flow2; (5): the register cache of a pointer in the original is kept over a store to a local (pralvars: ours loads again) and dropped over a store through the pointer (initallist, addallist: ours keeps it); DIFF 2 each; parameters: the first is the one in rr6/r7, a pointer after an int skips r6, see checkalias): (1) the original reloads a parameter at the first use after the
prologue / behind a conditional jump (operand, cmwalk, prquad, mkleaf, mktree, main, checkops); (2) `and r0,#%7f00` after `lda` in a struct
assignment to a local only in ours (genquads, fndparm, tmpsearch, checkops): phase B, fnc.py hides it; (3) popt block order of a switch
(mktree, genquads); (4) displacement form `ld r2,rr2(#4)` after p = p->next (writeil, tmpsearch).

Next (in this order):
1. Segment 4 is written up to 4:f5fe (reader2 .. live2; the rest of seg. 4 is libc). Segment 5 modules: ebbo2.c (5:0000 .., 59 functions, 8300 bytes), lpinv2.c (35, 13300), lpind2.c (23, 6100), reg2.c (37, 11400), alias2b.c; the libc members from 5:9bc0 (item 4). Write ebbo2.c first (the SCCS line ebbo2.c 1.8); the Makefile `part` needs ebbo2.o then (OBJS5 = ebbo2.o ...).
2. (fn5_2bca of alias2 and the segment 5 functions: with the libc part, item 4.)
3. quads2.c 4:1c88 (floating compare, EPU words in the listing); then flow2 (4:927e ..), dom2, init2, live2, ebbo2, lpinv2, lpind2, beta2,
   reg2 (data order, host/recon/modmap.py gave the module of every function in findings.json).
4. the libc starts at 5:9bc0 (members of slibc; 4:f5fe..4:ff78 too); link: src/cmd/gopt/Makefile (sld, -Mt4/-Mt5), LIBC version open.

## What we do now (the block for the next start message; the AI replaces it at the end of every session)

TASK: ebbo2.c (segment 5), after ebbinit (5:00c0) and ebbopt (5:0000), written and masked: 5:0114 (188 bytes + its case blocks 5:01d0, 5:02d4, 5:04ac, 5:05fe, 5:06c6, 5:06e4/5:0718, 5:0778, 5:07b4, 5:07d2: one function of about 1900 bytes up to 5:0894: a switch on q->op with 59 values and 9 targets, jump table at 5:5704; first set the case blocks' notes to "case block of f5_0114" in findings.json, rename.py f5_0114 NAME), then 5:0894 (420), 5:0a38 (348), 5:0b94 (486: the optimizer of one extended basic block, called by ebbopt).
START: fnc.py -a ebbo2.c (ebbinit, ebbopt masked); odisc.py -o f5_0114 (RECON_PROG=gopt); the placeholders fn5_0114, fn5_0b94, fn5_1ffc, fn5_206c in ebbo2.c get their names with rename.py -m.
STOP WHEN: about 70% of the context is reached (a rough estimate is enough; `/context` shows it, or judge by the number of tool calls and file reads). Until then work through the TASK list without stopping after each function: write, check with fnc.py, commit, then take the next function in the list and the following ones of the same module (ebbo2.c, then lpinv2.c ...). Do not stop early after one block. At about 70%: update findings.json, this block and the increment log, commit, print the next start block, stop (the owner then runs /compact; an auto-compact is fine, the stand is in this file and git).
