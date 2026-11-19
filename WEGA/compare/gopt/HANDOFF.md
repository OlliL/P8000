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
Open in the written functions: see the notes; recurring causes ((6) the register cache of a base address (compress, eval, linkp, dominators: the original loads the base of a global array each time, ours keeps it in rr4/r3); tool: python3 regdiff.py (scratchpad) removes register names from tmp/tmp/fnc/FUNC.diff and shows what is structurally different; (5) is seen again in rmlabels, deledge: cause 1 and 5 together make most of the DIFFs of flow2; (5): the register cache of a pointer in the original is kept over a store to a local (pralvars: ours loads again) and dropped over a store through the pointer (initallist, addallist: ours keeps it); DIFF 2 each; parameters: the first is the one in rr6/r7, a pointer after an int skips r6, see checkalias): (1) the original reloads a parameter at the first use after the
prologue / behind a conditional jump (operand, cmwalk, prquad, mkleaf, mktree, main, checkops); (2) `and r0,#%7f00` after `lda` in a struct
assignment to a local only in ours (genquads, fndparm, tmpsearch, checkops): phase B, fnc.py hides it; (3) popt block order of a switch
(mktree, genquads); (4) displacement form `ld r2,rr2(#4)` after p = p->next (writeil, tmpsearch).

Next (in this order):
1. live2.c written up to 4:d868: prqflags, prqall, islive .. tstlbit (the table of the live variables, the marking of the symbols, the bit vector functions); next 4:d868 (390), 4:d9ee (436), 4:dba2 (200), 4:dc6a (458), 4:de34 (724) ... see findings.json (module live2.c, about 30 open entries), then ebbo2 (segment 5?). New recurring causes: (7) the choice of the second pointer register pair (rr4 against rr6) goes the other way in different functions; (8) the address betatbl[j].x: the original adds the constant offset to the index first.
2. (fn5_2bca of alias2 and the segment 5 functions: with the libc part, item 4.)
3. quads2.c 4:1c88 (floating compare, EPU words in the listing); then flow2 (4:927e ..), dom2, init2, live2, ebbo2, lpinv2, lpind2, beta2,
   reg2 (data order, host/recon/modmap.py gave the module of every function in findings.json).
4. the libc starts at 5:9bc0 (members of slibc; 4:f5fe..4:ff78 too); link: src/cmd/gopt/Makefile (sld, -Mt4/-Mt5), LIBC version open.

## What we do now (the block for the next start message; the AI replaces it at the end of every session)

TASK: live2.c: 4:d868 (390 bytes), 4:d9ee (436), 4:dba2 (200), 4:dc6a (458) and the next functions of live2 (findings.json, module live2.c).
START: fnc.py -a live2.c (open: islive 4, settmptable 64, insrtlive 7, setsymtable 69, setsym 20, issym 31, setlbit 9, clrlbit 9, tstlbit 3: layout, register pairs, address form: leave them); odisc.py -o f4_d868; regdiff.py FILE.diff. The names: setlbit, clrlbit, tstlbit are the bit vector functions of live2 (v[i / 16] with i % 16).
STOP WHEN: 12 functions of live2 are written and checked, or at 70% of the context.
