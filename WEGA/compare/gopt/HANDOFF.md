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

Written: comm2, rdr2, reader2, debug2, wrtil2 (complete), quads2 (not 4:1c88), alias2 (4:6c0a..4:7368 and 4:79d0..4:7bb6 ...).
Open in the written functions: see the notes; recurring causes: (1) the original reloads a parameter at the first use after the
prologue / behind a conditional jump (operand, cmwalk, prquad, mkleaf, mktree, main); (2) `and r0,#%7f00` after `lda` in a struct
assignment to a local only in ours (genquads, fndparm, tmpsearch): phase B, fnc.py hides it; (3) popt block order of a switch
(mktree, genquads); (4) displacement form `ld r2,rr2(#4)` after p = p->next (writeil, tmpsearch).

Next (in this order):
1. struct qa: `unsigned w0` (the original shifts the type with srl and compares with ugt: aliasm, mktree); then `fnc.py -a` of every
   file to see nothing got worse.
2. alias2.c: 4:7c68 (checkops, with its blocks to 4:8074), 4:8074 (682), 4:831e, 4:87e6, 4:89c0 .. 4:9108; the placeholders fn5_2bca,
   fn7c68, fn7088 in the sources get their names (rename.py -m).
3. quads2.c 4:1c88 (floating compare, EPU words in the listing); then flow2 (4:927e ..), dom2, init2, live2, ebbo2, lpinv2, lpind2, beta2,
   reg2 (data order, host/recon/modmap.py gave the module of every function in findings.json).
4. the libc starts at 5:9bc0 (members of slibc; 4:f5fe..4:ff78 too); link: src/cmd/gopt/Makefile (sld, -Mt4/-Mt5), LIBC version open.
