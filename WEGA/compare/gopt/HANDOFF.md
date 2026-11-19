# gopt: state for the next session (read this, then compare/gopt/FINDINGS.md only where needed)

Rebuild of /lib/gopt (segmented, code in segments 4 and 5, no source known), increment 150 (host/increments/150-libraries.md, the log
at its end). Compiler by measurement: sccvc (host/OLDCC.md); a function is "masked" when `fnc.py` finds it equal (segment numbers and
addresses masked), `exact` only after the link and `segcmp.py` (not tried yet).

Tools (use these, they print little; never dump disassemblies or diffs into the conversation):
- `RECON_PROG=gopt python3 host/recon/fnc.py FILE.c FUNC ...`  one line per function, the diff is in tmp/tmp/fnc/FUNC.diff
- `fnc.py -a FILE.c`  all functions of the file, only the names that are not OK
- `odisc.py -s FUNC` outline (calls, strings, size); `odisc.py -o FUNC` listing to a file (read it with sed -n a,bp)
- `host/recon/modmap.py`, `own_table_seg2.py`, `rename.py [-m]`, `findings.py gopt` (writes FINDINGS.md), `make -C src/cmd/gopt part`
- findings.json: after every function its name, file, status and note (the scratch helper upd.py is gone with the session: edit the
  json by a small script or `rename.py`).

Written: comm2, rdr2, reader2, debug2, wrtil2 (complete), quads2 (not 4:1c88), alias2 (4:6c0a..4:7368, 4:79d0..4:7bb6, checkops 4:7c68..4:8074 DIFF 25, almap 4:8074 masked, addaffect 4:831e..4:87e6 DIFF 3, initaffect DIFF 11, alalloc/alret/chkallist masked, initallist/addallist DIFF 2, pralvars DIFF 2, mkallist/checkalias/aliasmat/praldim/ind masked, affsize DIFF 3 ...; alias2.c is complete except the placeholder fn5_2bca of segment 5).
Open in the written functions: see the notes; recurring causes ((5) is seen again in rmlabels, deledge: cause 1 and 5 together make most of the DIFFs of flow2; (5): the register cache of a pointer in the original is kept over a store to a local (pralvars: ours loads again) and dropped over a store through the pointer (initallist, addallist: ours keeps it); DIFF 2 each; parameters: the first is the one in rr6/r7, a pointer after an int skips r6, see checkalias): (1) the original reloads a parameter at the first use after the
prologue / behind a conditional jump (operand, cmwalk, prquad, mkleaf, mktree, main, checkops); (2) `and r0,#%7f00` after `lda` in a struct
assignment to a local only in ours (genquads, fndparm, tmpsearch, checkops): phase B, fnc.py hides it; (3) popt block order of a switch
(mktree, genquads); (4) displacement form `ld r2,rr2(#4)` after p = p->next (writeil, tmpsearch).

Next (in this order):
1. flow2.c: written up to 4:a66e: flow_graph, optflow, rmlabels (DIFF 13), blkempty, rmjumps, deledge (DIFF 7), rm_deadblks, blkof, marklab, addedge (DIFF 4); open placeholders in flow2.c: fna66e (4:a66e), fna9ac (4:a9ac, the print), jmplabel (4:a93c), then 4:a66e .. the end of flow2 and dom2 (see findings.json for the addresses; flow2 ends before 4:ac.. dom2 starts at its SCCS date).
2. (fn5_2bca of alias2 and the segment 5 functions: with the libc part, item 4.)
3. quads2.c 4:1c88 (floating compare, EPU words in the listing); then flow2 (4:927e ..), dom2, init2, live2, ebbo2, lpinv2, lpind2, beta2,
   reg2 (data order, host/recon/modmap.py gave the module of every function in findings.json).
4. the libc starts at 5:9bc0 (members of slibc; 4:f5fe..4:ff78 too); link: src/cmd/gopt/Makefile (sld, -Mt4/-Mt5), LIBC version open.

## What we do now (the block for the next start message; the AI replaces it at the end of every session)

TASK: flow2.c: 4:a66e (called with three ints: fna66e), 4:a93c (jmplabel), 4:a9ac (the print of the graph), then the rest of flow2 up to the first function of dom2.
START: fnc.py -a flow2.c (open: addedge 4, rmlabels 13, deledge 7; the rest masked); the findings.json keys joinblk, f4_a93c, prgraph and the next ones (see modmap: module flow2.c / dom2.c); rename.py -m for the placeholders fna66e, fna9ac.
STOP WHEN: flow2 is complete (the last function written and checked), or at 70% of the context.
