# gopt: handoff (2026-11-21)

GOAL: /lib/gopt rebuilt from C (src/cmd/gopt), byte-identical after the link. Compiler: sccvc (vc = v0p segmented + vc switches,
host/OLDCC.md). Rules: CLAUDE.md (German in conversation, English in git; commit, never push; branch `toolchain`). Keep this file,
findings.json (key `note`), SWITCH_CANDIDATES.md, host/OLDCC_LOG.md and host/increments/160-programs.md current.

## State
- fnc.py: 315 of 321 functions masked-equal. Data equal but the almap strings; stack segment and header as the original (sldo).
- Open (DIFF lines, candidate of SWITCH_CANDIDATES.md):
  setposition 18 (K8), chkfamindvar 9 (K8), affsize 3 (K5),
  rmlabels 2 (spopt with the D2AV copy), setsymtable 2 (K11), mkconstopnd 2 (dead `ldl rr0,rr6`, as v0p OLD_D3KC).
- What was tried for each: its `note` in findings.json and the dated entries of host/OLDCC_LOG.md.

## Owner authorisation (2026-11-21)
- Ladder: source forms > change an existing switch > new vc switch > new compiler (only with the owner).
- Existing switches (also v0/v0p) may be changed. New vc switches are allowed. Both need four checks to pass (below).
- The shipped (default) codgen/scodgen must stay byte-identical: change codgen only inside `#ifdef OLD_*`. Bugs of the originals
  are kept. The vc scodgen may be fixed as long as cas stays identical.
- Never change checkall or the nightly run without asking.

## The four checks (each compiler change, before its commit)
1. Models and bench: `oldcc.sh build codgen v0`, `oldcc.sh build codgen v0p` (rm the installed file first), then `oldcc.sh model P` for
   cparse scparse codgen scodgen popt spopt cpp. Expected: strictly equal, 179/179/209/210/73/73/26.
   `oldcc.sh bench`: not worse than 153/132/212.
2. `host/recon/oldcc.sh matrix check` and `MATRIX_VER=vc ... matrix check`. A new or changed vc switch first gets
   `MATRIX_VER=vc oldcc.sh matrix OLD_X` (must be +0).
3. gopt total: `fnc.py -a` over all src/cmd/gopt/*.c (no regressions).
4. checkall on the working tree: `R=$(git stash create); make -C host checkall WEGA=.../tmp/wega31_root PATCH=$R`. Expected: ALL OK
   (about 25 min, in the background).

## Tools
- `RECON_PROG=gopt python3 host/recon/fnc.py FILE.c FUNC` (diff in tmp/tmp/fnc/FUNC.diff, `FNC_CTX=N` for context; `-a FILE.c` for all).
- `odisc.py FUNC` (original listing), `vt.py` (variants in parallel), `host/tools/sdis.py -l X.o`.
- vc build for fnc: `cd src/cmd/codgen; rm build/segvc/*.o; PATH=.../tmp/tmp/wbin:$PATH make build/segvc/scodgen`, then rm and cp
  to tmp/oldroot/lib/vc/scodgen.
- Register traces: a temporary `printf("KDBG ...")` of rsuse/rsfree in getreg, then `sccvc -S` of the module (never commit it).

## Watch out: the vc scodgen size
The tree build (src/Makefile stage1) links the vc scodgen with OLDPRINTF. Its text is 0xf800 of 0x10000 (after OLD_DNR).
/bin/ld wraps sg_code to 0 silently. The codgen Makefile now stops then; before, checkall hung.
- Measure the size like checkall does: link with `LIBC="../../../../lib/oldlibc/printf/s1p.o ../../../../lib/oldlibc/doprnt/s1d.o
  ../../../../lib/oldlibc/doprnt/s1f.o /lib/libc.a"`. The three objects are copied from a checkall tree; they are gitignored.
- Read it with `od -An -tx1 -j28 -N2 build/segvc/scodgen`.
- Before adding vc code, free room in the OLD_* blocks without changing behaviour (as done: d2pair helper, merged tests,
  `goto blk`). A segmented port was rejected (the NODE union of cg.h assumes 16-bit pointers).

## Next steps (one at a time)
1. (done 2026-11-21: vc OLD_DNR leaves out the gopt register records, sg_code 0xf800)
2. (done 2026-11-21: vnumber by gotos to the copies the original keeps and `if (0) { l: then-block }` before the test)
3. (done 2026-11-21: tprint by narrowing vc spopt OLD_D50; method: run the shipped /lib/spopt on our codgen output (sccvc -S)
   and compare the block order, then build vc spopt without each switch)
4. (done 2026-11-21: chklpdefs by the goto form of the original block order and vc spopt OLD_D51 (d))
5. setposition (18) and chkfamindvar (9) (K8, pair choice). Trace rsuse at the pair getreg. In setposition the original takes the
   idle parameter pair rr2 at the second printf argument but not at the first. Find the state difference; allowing parameter pairs
   in the D2P loop is worse (306).
6. Small ones: affsize (3: copy register r3 against r1), setsymtable (2: dead `ld r2,r0` in a chain; a word OLD_D2AV had no effect),
   mkconstopnd (2: dead copy of a parameter), rmlabels (2: spopt).
7. After 321 of 321: the almap string order of the data, `segcmp.py` exact, `bincmp` of the linked file, `make install`, checkall.
