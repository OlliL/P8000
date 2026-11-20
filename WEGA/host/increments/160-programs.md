# 160 The programs of bin, usr/bin, etc, usr/lib

## Status

active (2026-11-19; gopt is the current work)

## Goal

Every program of the WEGA tree from source, byte-identical, each with a corpus.

## Done when

- COVERAGE shows no program as `sys3` or `todo` (except PLZ, increment 170), each with a corpus (120 pattern).
- checkall ALL OK.

## Read first

- `host/HOWTO_reconstruct.md`, the agents of 140.
- `src/COVERAGE.md`, `host/OLDCC_DATES.md`.

## Known

- 2026-11-15: bin 25 of 111 exact, usr/bin 10 of 145, etc 33 of 67, usr/lib 1 of 197 (counts include text files, 130).
- The tool chain is done except `gopt`, `chkout`, `yacc` and the PLZ tools (`host/HANDOFF_wega_disasm.md`, inventory);
  `/bin/cas` is byte-identical, left there: the natural `for` loop of jpr (`compare/cas/HANDOFF.md`).

## Steps

1. Order by date and compiler (programs of one compiler version together); the tool chain gaps first (`gld`, `yacc`,
   `chkout`, `gopt`), since later programs need them.
2. Per program with the agents of 140: source, library, own code, corpus; checkall per batch.

## Decisions

## Not part of it

PLZ programs and `as`: 170.

## Log

- 2026-11-19: /lib/gopt, ebbo2.c complete (20 functions, 5:0000..5:2138, segment 5): masked: ebbinit, ebbopt, vnfind, updateopnd, ebbfix, exprenter, identop, nostore, clear, killed, ebbcall, checkuse, inuse, addstore, resetbb, ebbclean; open: vnumber (DIFF 133, popt block order: codemove), exprfind (2), canopt (2), constfold (28: tail merging, long `&` temp register). Findings: `constfold` returns struct qa by value; `!x->a.b2 == 7` in the original source (KNOWLEDGE.md quirks table); the debug output strings give the names. Values at the data addresses: s71e4 (kill table count), s71e6 (stores), s71e8 (value counter), s71ea (use table), a0cc (value table), htab at 8:7164.

- 2026-11-19: /lib/gopt, lpinv2.c complete (35 functions, 5:2138..5:5534): masked 26; open: clearblocks, marklpblocks, liveonexit (displacement form, cause 4), qmkstore (parameter reload), srchdflist (rr4/rr6 temp), clropt, updatelooplist, isglobal (block order / tail merging), orderbl (2). Findings: fnc masks the word immediates of cp (thresholds and op numbers are not checked: read them from the listing, not from the diff); `(!f() != 0)` gives the materialized boolean; `a = b = x` and `if ((x = x->next) == 0)` give the copy patterns; function-local statics live in the data of the module (lblcnt = 15000 at 7:5CB6); the quirks table has alloclpdef (no return) and samebetalpdef (`printf(q)`).

- 2026-11-19: /lib/gopt, lpind2.c started (16 of 23 functions, 5:5534..5:6eda incl. delindvar, chklpdefs, chkfamindvar with its case block 5:6d40; masked: qmove, moveindquad, copyopndstmp; the others differ in cause 2 `and r6` after lda, popt block order, tail merging). Findings: struct parameters of ceval go on the stack (6 bytes, struct qa); a function without return keeps rr2 as its result (moveindquad, alloclpdef); chained assignments `prev = a0e0 = x` give the order of the stores; the original keeps the first copy of a merged tail, sccvc the last; `fnc.py` masks the word immediates of cp (check thresholds against the listing).
- 2026-11-19: /lib/gopt, reg2.c complete (5:6eda..5:9b3c; 37 functions, masked 29; the first 21: the optimizer driver `optimize`, the loop call flags, the register allocation: `assignreg`, `include`, `buildgraph` (interference graph as bit matrix a110, node list a11e), `colorgraph` (stack coloring), `pickcolor`, `rmnode`, `addreg`/`addrestore`/`addsave`; masked 16 of 21). Findings: struct qa arguments (6 bytes) are copied to the stack like in ceval; the first pointer goes to rr4 when an int took r7 (insertquad(op, a, b, c, where): a rr4, b rr2, c and where on the stack); the original reads `a[x] = a[x] | (1 << n)` (not `|=`); a11a (10-byte nodes) is indexed as `(char *)a11a + (i*10+off)`; testbit/clrbit argument order differs between callers (row, col).
- 2026-11-19: /lib/gopt, alias2b.c (isebbend, nullfn, masked OK): all C modules of gopt are written; left are the libc members (5:9bc0.., 4:f5fe..) and the link.
- 2026-11-19: /lib/gopt libc part: built by `sccvc -O -DSEG` from the slibc sources with the old stdio.h (`char *_ptr`, src/head/oldv7), not by the shipped scc; 13 members exact at instruction level, 8 near (older source revision), the rest (doprnt, printf, malloc, perror, atoi, mktemp, strncpy, strcmp, fltioc ...) far. Tools: host/tools/seglibfit.py (one source against an address), seglibscan.py (all sources against the libc part). Next: member boundaries, 0_3.c sources, slibc-v12.a, link.
- 2026-11-19: /lib/gopt libc: host/tools/oldslibc_gopt.py cuts 14 members as NAME/0_3.s (all equal to the original text and data, relocated words masked); `make ../slibc-v12.a` in src/lib/oldslibc builds the library (45 members). Found: 4:f5fe..4:fc3a is not libc but beta2.c (the betatbl module), still to be written; the libc of seg 4 starts at 4:fc3a (fseek).

- 2026-11-19 gopt: beta2.c written (betainit DIFF 2, eqopnd DIFF 7, the other four OK); findings lost by an earlier rename (live2 sameopnd, flow2 prgraph) restored, the new functions named eqopnd and prcgraph. `make gopt` compiles every module with slibc-v12.a; the link needs the data definitions of the 228 globals (next). Owner idea: align the code with the pcc mip sources after the byte identity (listed in HANDOFF).
- 2026-11-19 gopt LINKS: dataname.py (data address of every global from the code), gdata.c/gdata7.c (generated definitions), aliases of globals unified, srcorder.py (functions in the order of the original, forward declarations), dconstlook written (4:1c88 OK), fnc.py accepts notes 'tail of NAME (...)'. bss exact (42496), code 64768+51200 against 65534+50432, data 30464 against 31232. Open: stack segment flags (-Ms63 vs original), libc placement, seg 7 data, the 107 named DIFFs (segcmp.py -m).
- 2026-11-20 gopt: new vc switch `OLD_D2K` (a store through a scratch register forgets its name; neutral on cas, cas build byte-identical): addallist, initallist exact (masked); mkhash, mkdomtree by the source form `(p = taballoc(..))->m = 0`; fnc.py 237 of 322 OK. Fixed on the way: the vc matrix model of cas (old stdio.h), the incomplete libc-v*.a.
- 2026-11-20 gopt: vc switch `OLD_D3JL` (a restored pointer parameter gets no frame home; neutral on cas): canopt, denorm, islive; fnc.py FNC_DROPRAW (field masks no longer dropped): betainit, addquad, insertquad, exprfind; fnc.py 244 of 322 OK.
- 2026-11-20 gopt: vc switch `OLD_D24L` (lda of a frame place without the mask; neutral on cas): insrtlpdef, insrtstmpmove, insrtopstmp, savecolors; fnc.py strict (drops nothing); 248 of 322 OK.
- 2026-11-20 gopt: vc switch `OLD_D2AP` (OLD_D2A for a pointer in rr0 stored to a global): deltmplpdefs; 249 of 322 OK.
- 2026-11-20 gopt: vc switch `OLD_D2FA` (an auto pointer is found in the pair named by its frame place; not for arguments, not in &&/|| conditions, not when the statement assigns to it): clearblocks, marklpblocks, pralvars, solvelive, solvereach, liveonexit; genquads 84 -> 50; 255 of 322 OK.
- 2026-11-20 gopt: OLD_D2AP removed again (deltmplpdefs by a source form); the other switches re-checked with 9..20 source forms each; 255 of 322 OK.
- 2026-11-20 gopt: vc spopt switch `OLD_D64` (memreg reads segmented frame offsets): domspace; OLD_D2FA corrected (stale frame names, wrong code); 256 of 322 OK.
- 2026-11-20 gopt: addblk (source form), lpow2 (vc OLD_D10: immediate only for a right shift by one); 258 of 322 OK.
- 2026-11-20 gopt: addedge by a source form (if/else with two stores); 259 of 322 OK.
- 2026-11-20 gopt: OLD_D2FA extended to parameters named by an assignment of a computed value: tmpsearch; 260 of 322 OK.
- 2026-11-20 gopt: ispow2 (OLD_D10: the shift of a register variable goes through a register); 261 of 322 OK.
- 2026-11-20 gopt: vc `OLD_D3PL` (a pointer parameter cast to long from its pair): tabfree; 262 of 322 OK.
- 2026-11-20 gopt: vc `OLD_D2AV` (OLD_D2A copy only for a pointer in rr0 wanted in a temporary register): ceval, orderbl; 264 of 322 OK.
- 2026-11-20 gopt: vc `OLD_D2XP` (OLD_D2X not into a parameter's pair): nodeno, fndparm; 266 of 322 OK.
- 2026-11-20 gopt: eqopnd by a source form (nested if instead of &&); 267 of 322 OK.
- 2026-11-20 gopt: OLD_D2FA refined (only names matched alone; || at the top): linkreachdefs, asgfold; 269 of 322 OK.
- 2026-11-20 gopt: vc `OLD_D2PP` (a parameter word given back for a lighter pair): setlbit, clrlbit; 271 of 322 OK.
- 2026-11-20 gopt stack segment: the shipped sld cannot give the header of /lib/gopt (no stack table entry, _stkseg 63, relflg 1): `_stkseg = maxseg` without an entry needs stksegn -3, i.e. at most one -M option (middle() in src/cmd/sld/ldm.c); -Ms63 adds an entry (alloc_resv of res 3000), -z gives 127 and 0x21. Owner's decision: an old sld. sldo = sld with ldm.c -DOLD_SLDS (an empty -Ms stack segment gets no table entry), `make old`/`oldinstall` in src/cmd/sld, OLDROOT/bin/sldo by stage2, wrapper by host/tools/wbin.sh; sld itself stays byte-identical. gopt links by sldo: 4 table entries (4 5 7 8), flag 1, _stkseg 63 as the original.
- 2026-11-20 gopt data (step 3): init2 s4d98, vc scparse `OLD_D12R` (struct return area in the data: ebbo2 constfold), checkops and notlive string order by source forms; datastr: only the almap strings differ in order.
- 2026-11-20 gopt data: slibc-v12 members assembled by sccvc (.addr for the pointers of the cut .s, v12/data.o by sccvc: bit 31 as the original); OLD_D12R writes the area behind the function; source forms: operand's opnd and mkleaf's ftot/fflag are function statics, pend first in wrtil2. host/recon/datadiff.py: the data differs only in the almap strings and one bss address in _iob (code addresses masked).
- 2026-11-20 gopt bss: the start-up code stores environ at <<8>>%0000; the original reserves 8:0000..0003 for it, ours had its first bss variables there (all of bss 4 bytes low, _iob's buffer pointers off by 4): gdata.c `char environ[4]` first. Data: only the almap strings differ now (datadiff.py).
- 2026-11-20 gopt: host/recon/formtry.py (automatic source forms: continue, nested if, ?:, swapped if/else, for) over all 48 DIFF functions: two improve, eval (DIFF 15 -> 9) and delbucket (22 -> 20), both by swapped if/else branches; the others only worse.
- 2026-11-20 gopt: vc OLD_D3JB (a `]` between a jump and its label is a record: prquad) and OLD_D3S (scparse `#static`: a function with a static in its body keeps the parameters in the frame: mkleaf, qmkstore, operand 30 -> 28); 276 of 321 OK; matrix vc +0, the real cas build byte-identical. Two OLD_D3JN rules for cmwalk dropped (net 0).
- 2026-11-20 gopt mktree: the "popt layout" (cause 3) was a source form: the talloc block under the negated condition with return, the asg code behind it without a label: DIFF 632 -> 20.
- 2026-11-20 gopt vnumber: the gotos of the draft replaced by structured code (inner switch with default, tails copied): DIFF 131 -> 38; the rest is cause 8 (popt keeps the later copy of a common tail).
- 2026-11-20 gopt genquads: the end chain negated with a nested if: DIFF 50 -> 8. HINT for the layout DIFFs (cause 3): the negated condition with the nested if (mktree, genquads).
- 2026-11-20 gopt writeil: the local array of the register symbols hid the global betatbl (draft error; regsym): DIFF 108 -> 6.
- 2026-11-20 gopt settmptable exact: `if (!islive(..) && nlv > 480) return 0;` and the bit set in the BBIT address form.
- 2026-11-20 gopt setsymtable: the body under the negated test (68 -> 17), the op stores of the local tq as a byte of the frame (17 -> 2, form guessed).
- 2026-11-20 gopt updatelooplist: `y = x = p->bls` (the store order) and the branches swapped: DIFF 32 -> 5. clropt: 14 more forms, none.
- 2026-11-20 gopt chkcalls: the loops by the cross jump direction (init copy kept: top test; step copy kept: for): DIFF 31 -> 11.
- 2026-11-20 gopt updatevector exact: `*++p != -1` (the draft took the head entry of the srchdef list).
- 2026-11-20 gopt indupdate exact: cause 8 is a source form: the common tail written in both branches, both end with a jump to the same label, comjump keeps the first copy.
- 2026-11-20 gopt constfold: the return tail in both branches (cause 8 as a source form): DIFF 28 -> 10. K14: our sccvc emits wrong code for a long & / | with frame operands (`ld r3,lo; and r3,r3`), the original takes r1.
- 2026-11-20 gopt needustar exact (early return 1, one condition, return 0 last).
- 2026-11-20 gopt isglobal exact (a guard if before the loop).
- 2026-11-20 gopt dotree exact: the common statement copied into both branches (K13 is a source form too).
- 2026-11-20 gopt: NODE type unsigned (TWORD as in mip): mktree 10 -> 8, nothing lost; 282 of 321 OK.
