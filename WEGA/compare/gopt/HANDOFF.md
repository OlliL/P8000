# gopt: state for the next session (read this, then compare/gopt/FINDINGS.md only where needed)

GOAL: /lib/gopt (segmented, code in segments 4 and 5, no source known) rebuilt from C source, byte-identical to the original after the link
(`segcmp.py` exact, then `bincmp`). Increment 160 (host/increments/160-programs.md; new log entries go there). Compiler by measurement: sccvc
(host/OLDCC.md). A function is "masked" when `fnc.py` finds it equal (addresses masked), "exact" only after the link.
NO STOP RULE: work without a context limit and without stopping after a function or a module until gopt is byte-identical; commit often
(no pushes, only branch `toolchain`), keep this file, findings.json and the log current as you go. Ask the owner only for a decision that is
really his (the ladder below answers most questions). Rules of the project: CLAUDE.md (German in conversation, English in git).

## Owner decisions (2026-11-19, before the overnight run)
- STEP 4 OF THE LADDER: when at the end only DIFFs are left that need a new compiler, build it but do NOT commit it: write
  compare/gopt/compiler_changes.diff (against the current compiler; it may break other users, therefore only as a diff) and
  compare/gopt/COMPILER.md (as src/lib/libcisam/COMPILER.md: the switches, what each does, the witnesses).
- SLD / STACK SEGMENT: first analyse src/cmd/sld (ldm.c ...) for a hidden way to get the original header (no -z flag, no 5th table
  entry, _stkseg 63). If there is none, an "old" sld version may be invented (own -D switches, built beside sld) and used to link gopt.
- CPARSE IN CHECKALL: may be fixed by the session (the shipped cparse did not cope with alias2.c: /tmp/wega_check/corpus_cparse.log,
  "DIFF src/cmd/gopt/alias2.c" in the corpus); if there is no fix, alias2.c may be left out of the (s)cparse corpus. Then run checkall.
- SLIBC-V12: gopt is its only user (made for gopt); build it in src/lib/oldslibc as an archive of its own and change it in any way needed.

## Priority ladder for every DIFF (strict order; go down only when the step above is really hopeless)
1. SOURCE FORMS (highest priority, applies before everything else): try other C forms of the same function (statement order, `if` chains vs
   `switch`, `a = a | x` vs `|=`, temporaries, `register`, parameter order and types, declaration order, goto, comma, pointer vs index form,
   the form of the pcc source in mip/ of sys3 and the unix archive). ALSO USE THE PCC SOURCES WHILE WORKING ON THE DIFFs, not only afterwards: gopt shares
   much code with pcc (tmp/sys3/usr/src/cmd/cc/pcc/mip: reader.c, match.c, common.c, manifest, mfile2.h; also cc/vax/mip, cc/pcc/mdp/order.c, lint/mip; the 4.4BSD and
   2.11BSD trees in the local directories of CLAUDE.md). When a function (or its neighbours) has a pcc relative, take its statement order, loop and
   condition forms, declarations and register variables from there as source form candidates: they often lead to the original bytes. The old compiler is right until proven otherwise; the original source
   was written by people for this compiler. Use `host/recon/srctry.py FILE FUNC < variants` (it tries variants against the original). Many
   variants, not six: only when the forms are really exhausted (also the forms of the neighbouring functions) go to 2.
2. An EXISTING SWITCH of the current compiler (sccvc and its passes) that is changed or newly combined, only so that no other user of the
   compiler loses its byte identity (run `host/recon/oldcc.sh matrix check`, sccvc: `MATRIX_VER=vc host/recon/oldcc.sh matrix check`, and
   follow what it says; host/OLDCC_SWITCHES.md is generated).
3. A NEW SWITCH in the existing compiler, again without changing the output of any other user (same check).
4. A NEW COMPILER with its own switches: the worst option (we build ever more C compilers that may never have existed; probably only the
   right source form was not found yet). Only with the owner's decision.
Never change compiler passes or checkall/nightly checks without asking (CLAUDE.md). Record every difference in the quirks table of
host/KNOWLEDGE.md and host/OLDCC.md (D1..D23), notes on a function only in findings.json.

Tools (use these, they print little; never dump disassemblies or diffs into the conversation):
- `RECON_PROG=gopt python3 host/recon/fnc.py FILE.c FUNC ...`  one line per function, the diff is in tmp/tmp/fnc/FUNC.diff
- `fnc.py -a FILE.c`  all functions of the file, only the names that are not OK
- `odisc.py -s FUNC` outline (calls, strings, size); `odisc.py -o FUNC` listing to a file (read it with sed -n a,bp)
- `host/recon/modmap.py`, `own_table_seg2.py`, `rename.py [-m]`, `findings.py gopt` (writes FINDINGS.md), `make -C src/cmd/gopt part`
- findings.json: after every function its name, file, status and note (the scratch helper upd.py is gone with the session: edit the
  json by a small script or `rename.py`).
- `RECON_PROG=gopt python3 host/recon/segcmp.py -m [-v NAME] src/cmd/gopt/gopt`  the linked program against the original per function (the work list: the
  named functions with DIFF)
- `host/recon/datastr.py [-s | -b LO HI [SHIFT]]`  the data segment by strings: differing strings, shifts, bytes
- `host/recon/dataname.py OBJ.o ...`  the data address of every global from the code; `srcorder.py FILE.c` puts the functions in address order
- `host/recon/srctry.py FILE FUNC < variants`  source variants against the original; `regdiff.py`, `odisc.py -s/-o`, `rename.py [-m]` (careful:
  a key that exists is overwritten: check the findings entries afterwards)
- build: `cd src/cmd/gopt; PATH=/home/olivleh1/git/P8000/tmp/tmp/wbin:$PATH make gopt` (wbin holds the wrappers of cas, sld, sccvc ... under z8k-emu; fnc.py takes src/cmd/gopt/FILE.c; findings.json key `note`, not `notes`; a case block has "note": "case block of NAME" (or "tail of NAME")).

## Progress (2026-11-20, overnight run)
fnc.py over all modules: 294 of 321 OK (2026-11-20 late) (was 233 of 322; nullfn removed: it is __fltuse of the libc member fltioc).
Data (step 3): equal but for the almap strings (host/recon/datadiff.py, code addresses masked). Libc (step 2): equal masked
(host/recon/libcmp.py); fseek..getw go to seg 4 by first fit only when the code sizes are exact. Stack segment: gopt links by
sldo (src/cmd/sld make old, -DOLD_SLDS), header structure as the original. bss from 8:0004 (environ at 8:0000).
New vc switches (all +0 on cas, the real cas build byte-identical; host/OLDCC.md vc section, the source forms tried for each in
findings.json/OLDCC_LOG.md): codgen OLD_D2K, OLD_D3JL, OLD_D24L, OLD_D2FA, OLD_D3PL, OLD_D2AV (changes D2A), OLD_D2XP (changes
D2X), OLD_D2PP, OLD_D3JB (] between a jump and its label), OLD_D3S (a static in the body: parameters in the frame, with scparse's #static); OLD_D10 narrowed; spopt OLD_D64; scparse OLD_D12R, OLD_D3S. The slibc-v12 cut members by sccvc (.addr: bit 31).
Source forms found: mkhash, mkdomtree, deltmplpdefs, addblk, addedge, eqopnd, srchdef, freelist, main, notlive, checkops order,
operand's opnd/mkleaf's statics (data), init2 s4d98.
HINTS: the author wrote `continue` in loops (main, srchdef), the loop branch first (freelist), ?: for two-value assignments
(indupdate, notlive), nested if instead of && (eqopnd), (p = f())->m (mkhash). host/recon/formtry.py tries such rules on a
function automatically. After a compiler change: `rm *.o` in src/cmd/gopt before make gopt (make does not know the compiler);
the codgen Makefile has no oldcc.h dependency (`rm build/segvc/*.o`).
Open: popt layout (cause 3: which block codemove takes, mktree 632, vnumber, settmptable, setsymtable, writeil, genquads ...;
cause 8: which copy of a common tail stays, insrtlive, tprint, indupdate; dropped experiments in OLDCC_LOG.md), parameters not
restored at labels (cmwalk, checkops; two OLD_D3JN rules dropped, OLDCC_LOG.md), entry store of a word parameter (tstlbit), word parameter compared
from memory (deledge), pair choices (joinblk rr6/rr4, testbit), the almap data order (needs the popt move).
OWNER 2026-11-20: SOURCE FORMS FIRST, a real srctry batch (10+ forms) before any switch, counts in findings/log.
OWNER 2026-11-20 (later): NO new switch during the source phase; a compiler-looking difference goes as a candidate with its
witnesses to compare/gopt/SWITCH_CANDIDATES.md, then on with source forms. Layout DIFFs were often source forms: the negated
condition with a nested if (mktree 632 -> 20, genquads 50 -> 8, settmptable, setsymtable), gotos of the draft (vnumber),
a local array that hid a global (writeil 108 -> 6). Later: cause 8 is a source form (the common tail written in both branches: indupdate, constfold, dotree); a loop form shows in which copy popt keeps (chkcalls, needustar, isglobal: a guard if before the loop); chain order `y = x = ..` (updatelooplist); `^` instead of `==` (chkfamindvar); NODE type unsigned (TWORD). Forms tried before can match later (writeil comma form): retry them. Tool: $S/vt.py (scratch, variants in parallel on copies) - to be moved to host/recon. The remaining 39 DIFFs are classified in SWITCH_CANDIDATES.md except chklpdefs, fillindvar, chkcalls (layout).

## State (2026-11-19)
`make gopt` LINKS. Layout: gdata.o first (bss in address order: bss exact, 42496 bytes), reader2 .. live2 (-Mt4), ebbo2 lpinv2 lpind2 (-Mt5), beta2
(-Mt4: first fit into the end of segment 4), reg2 alias2b (-Mt5), libc slibc-v12.a. Link flags `-i -e start -Md7 -Mb8 -Ms63`. Functions of every module
are in the address order of the original. Sizes: code seg 4 64768 (orig 65534), seg 5 51200 (orig 50432), data 31232 = orig (all strings of the modules
equal in text; the variables with initialisers are in their modules, the mfile2.h SCCS line comes from gopt.h after the module line), bss exact.
segcmp -m: 203 same, 107 named functions with DIFF (the rest are unnamed case-block pieces). The files of the original are in
tmp/wega31_root/lib/gopt; the old SCCS lines tell the module of every function (modmap.py).
Recurring causes of the function DIFFs ((1)-(9) in the notes of findings.json): (1) parameter copy (`ld r3,r7; mult rr2`) vs in place; (2) `and r6,#ADDR` after
`lda` in struct arguments (fnc.py hides it); (3) popt block order of a switch; (4) displacement form after p = p->next; (5) register cache of a pointer
(the original reloads a global pointer, ours keeps it in rr2: addallist, initallist, canopt, denorm ...); (6) rr4/rr6 temp pair; (7) betatbl index+offset
order (macro BBIT); (8) tail merging (ours keeps the last copy, the original the first); (9) top-test vs bottom-test loops. Known fixes of the form: `a[x] = a[x] | (1<<n)`
instead of `|=`; `printf("\0")` for a two-byte string; `if (c) return 1; return 0;` instead of `return c;`.

## TASK (work through in this order, without stopping)
1. THE 107 NAMED DIFFs of `segcmp.py -m` (and the masked DIFFs of `fnc.py -a`): one-line ones first (addallist, canopt, deltmplpdefs, denorm, initallist,
   insrtlpdef, insrtstmpmove, mkconstopnd, mkdomtree, mkhash, orderbl, pralvars), then the others by size; every one by the priority ladder above (source
   forms first, many variants). Also betainit (DIFF 2: `t->fl = flag` misses `and r0,#0x0800`), eqopnd (DIFF 7: the original spills only b, uses rr4 as temp),
   the open big ones (main/dotree/pr of reader2, genquads, operand, writeil, mktree, settmptable, setsymtable, fillindvar, indupdate, freelist, chkfamindvar, ...).
2. LIBC AND STACK SEGMENT: when the code sizes are exact the libc members must land as in the original (fseek ftell rdwr fopen getw at the end of seg 4 by
   first fit, the rest from 5:9bc0; `-Mt4` before the library spills into segment 0 as long as sizes differ); the order of the members of slibc-v12.a is a guess
   (decided by the link compare); nullfn (5:9bbe, = __fltuse of fltioc) out of alias2b.c; the stack segment: the original has no -z flag (relflg 0x01), no table
   entry for the stack and _stkseg = 63 (instruction bytes 3f); `-Ms63` gives 63 but a 5th table entry, `-z` gives 127 and the flag 0x21 (open: find the sld
   option or the form that gives the header of the original; sld source: src/cmd/sld/ldm.c).
3. DATA OF SEGMENT 7 BYTE BY BYTE (`datastr.py`, then `bincmp`): the string ORDER (checkops: '//    NULL\n' after '// chkops: nonstandard' in the original; '(almap, symbol'
   in alias2.c; 'notlive() = %s' before TRUE/FALSE in lpinv2.c), ebbo2.c (three extra zero words before the value table of the second switch at 0x5930: a static of the original?),
   the jump and value tables of every module, libc data (iob 7:779e, ctype 78b0, errno 79b2) of the members.
4. LINK COMPARE: `segcmp.py` exact (no -m), header and segment table, then `bincmp` of the whole file; `make install`, checkall as for the other programs.
After byte identity: (5) ALIGNMENT TO PCC/MIP (owner idea 2026-11-19): names, variable names, formatting equal to the mip sources of sys3 and the unix archive
where the code is the same (a tool that lists the similarity per function, one module at a time, `fnc.py -a` and the link unchanged after each change); (6) findings.json
cleanup (the case-block entries, at 100 %); (7) the whole-tree check.

LIBC DECISION (owner 2026-11-19): at most a few C attempts per member (source forms of the same slibc source; sccvc -O -DSEG, old stdio.h), then a `.s` cut as in host/tools/oldslibc_cas.py (a gopt variant is to be written: two code segments 4 and 5, data seg 7 at image 0x1c4fe, bss seg 8). Own code ends at 4:fc3a: 4:f5fe..fc3a are gopt functions (sameopnd = operand compare, taballoc ...), NOT libc. Libc of seg 4: fseek fc3a (.s: 3 diffs of register choice), ftell fd9a (.s, 4 diffs), rdwr fe50 (C exact), fopen ff54 (C exact), getw ff78..fffe (C exact, 132 bytes, ends at the 64K border). Libc of seg 5 in order: fltioc 9bbe..a170 (includes the 2-byte ret of ffltioc, = nullfn of alias2b.c, to be moved; C 30 diffs), scanf a170 (C), doscan a264 (C 19 diffs), printf/fprintf/sprintf a94e/a9b0/aa06.. and doprnt ab06.. (.s, boundary to find via call targets: a94e x505 = printf, a9b0 x14, ab06 called from a98c a9e0 aa4e), ungetc b2ba (C), atof b326, ecvt b4ba (found 5:b4cc entry), pow10 b69c, filbuf b712, flsbuf b81e (oldslibc/flsbuf/0_3.c), endopen ba54 (C 9 diffs, .s), findiop bc00, cuexit bc32, cnvt bc3e, decimal bd58, length bdf6, a malloc-like member be20..c1b4 (916 bytes, entries be20 c0a4: .s), isatty c1b4, stty c1dc, strcat c218, strcpy c252, tolower c282, then the syscall stubs (shipped bytes) to c414 and the member at c414 (236 bytes, ends the segment? check), plus unknown small members (a8de..a94e 112 bytes, a1aa/a1ea? are inside scanf/doscan). The call-target table of the libc part (entry -> callers) can be rebuilt with the loop in this session: decode both code segments, collect `call <<seg>>addr`.

LIBC CUT (2026-11-19): host/tools/oldslibc_gopt.py (`names`: symbol table harvested from the known members; `cut src/lib/oldslibc`: writes NAME/0_3.s) cut 14 members as assembler: fseek ftell fltioc doscan printf doprnt ecvt(b49e..b69c, entries _ecvt _fcvt __cvt) endopen malloc(+free, realloc) chmod creat open read write (the stubs are the older ones with a chkseg call); every one assembles by sccvc and equals the original text and data (relocated words masked). Wrong earlier guesses: ecvt is not the shipped one; the printf/doprnt boundary is aa8a (strout belongs to doprnt). Still to do: (1) the member list and ORDER of slibc-v12.a for gopt: seg 4: fseek ftell rdwr fopen getw; seg 5: fltioc scanf doscan printf doprnt ungetc atof ecvt pow10 filbuf flsbuf(0_3.c) endopen findiop __callex cuexit cnvt decimal length malloc isatty stty strcat strcpy tolower, the stubs (chmod access? close creat ioctl lseek mkseg open read sgbrk ssgbrk umask write chkseg cerror) and the data members (iob 7:779e, lastbuf, ctype 78b0, errno 79b2, deverr 79b4); the data of the C and shipped members comes from their objects; unknown: 5:c414 (236 bytes) and the tail after it, whether bit 31 of the address constants needs the shipped cas (as the other .s of oldslibc) or sccvc's assembler (decided by the link compare). (2) Makefile in src/lib/oldslibc: rules for NAME/0_3.o by cas, C members by sccvc -O -DSEG with -I../../head/oldv7. (3) link: gopt Makefile LIBC, then segcmp.py/bincmp. (4) move nullfn (5:9bbe = __fltuse of fltioc) out of alias2b.c: it is the first word of fltioc, not an own function.
