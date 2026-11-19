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
1. Segment 4 is written up to 4:f5fe (reader2 .. live2; the rest of seg. 4 is libc). Segment 5 modules: ebbo2.c, lpinv2.c, lpind2.c (all written), reg2.c (37, 11400), alias2b.c; the libc members from 5:9bc0 (item 4). Write ebbo2.c first (the SCCS line ebbo2.c 1.8); the Makefile `part` needs ebbo2.o then (OBJS5 = ebbo2.o ...).
2. (fn5_2bca of alias2 and the segment 5 functions: with the libc part, item 4.)
3. quads2.c 4:1c88 (floating compare, EPU words in the listing); then flow2 (4:927e ..), dom2, init2, live2, ebbo2, lpinv2, lpind2, beta2,
   reg2 (data order, host/recon/modmap.py gave the module of every function in findings.json).
4. the libc starts at 5:9bc0 (members of slibc; 4:f5fe..4:ff78 too); link: src/cmd/gopt/Makefile (sld, -Mt4/-Mt5), LIBC version open.

## What we do now (the block for the next start message; the AI replaces it at the end of every session)

TASK: all C modules of gopt are written. Now the libc part (5:9bc0..5:c414, 4:f5fe..4:ff78). Result so far (2026-11-19, host/tools/seglibfit.py, seglibscan.py): it is NOT the shipped slibc nor slibc-v9/-v11, but the slibc sources built by `sccvc -O -DSEG -I<oldv7 stdio.h>` (the shipped scc gives a bigger frame). Exact (instruction level, addresses masked): cnvt 5:bc3e, filbuf b712, findiop bc00, fopen 4:ff54, isatty c1b4, length bdf6, rdwr 4:fe50, scanf a170, strcat c218, strcpy c252, stty c1dc, tolower c282, ungetc b2ba; byte-equal to the shipped members: atof b326, pow10 b69c, ecvt b4ba, decimal bd58, cuexit bc32, the syscall stubs (c2be..c418). Near (an older source revision, small diffs): toupper 1, flsbuf 2, ftell 2, fseek 3, tell 6, strlen 6, endopen 9, doscan 19. Far (older source, ours differs a lot): doprnt, printf, malloc, perror, atoi, mktemp, strncpy, strcmp, fltioc (starts 5:9bc0), sleep, swap, getpass. None of the oldlibc 0_n.c sources fits these. Next: (1) per member exact start/end in gopt (link order), (2) a 0_3.c source (older revision) for the near ones under src/lib/oldslibc/NAME/, (3) .s cut (oldslibc_cas.py as the model) only for what no source form reaches, (4) slibc-v12.a in the oldslibc Makefile (sccvc rules like errlst), (5) link gopt (Makefile -Mt4/-Mt5, segcmp.py). Open masked DIFFs of the C modules and fn4_1c88 stay as listed in findings.json.
START: fnc.py -a ebbo2.c (16 of 20 OK); odisc.py -o clearblocks (RECON_PROG=gopt); case blocks of a function are set with "note": "case block of NAME" in findings.json (key `note`, not `notes`), then rename.py f5_XXXX NAME; fnc.py takes src/cmd/gopt/FILE.c.
STOP WHEN: about 70% of the context is reached (a rough estimate is enough; `/context` shows it, or judge by the number of tool calls and file reads). Until then work through the TASK list without stopping after each function: write, check with fnc.py, commit, then take the next function in the list and the following ones of the same module (ebbo2.c, then lpinv2.c ...). Do not stop early after one block. At about 70%: update findings.json, this block and the increment log, commit, print the next start block, stop (the owner then runs /compact; an auto-compact is fine, the stand is in this file and git).
