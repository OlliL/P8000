# gopt: handoff (2026-11-21)

GOAL: /lib/gopt rebuilt from C (src/cmd/gopt), byte-identical after the link; then clean up, port to the host and consolidate
the compiler switches. Compiler: sccvc (vc = v0p segmented + vc switches, host/OLDCC.md). Rules: CLAUDE.md (German in
conversation, English in git; commit, never push; branch `toolchain`). Keep this file, findings.json, host/OLDCC_LOG.md and
host/increments/160-programs.md current. Clean-up rule (owner 2026-11-21): what is deleted from a file but still matters (for
the reconstruction of other programs) goes to host/KNOWLEDGE.md or host/OLDCC_LOG.md first.

## State
- fnc.py: 321 of 321 functions masked-equal; the data byte-equal (datadiff 0); stack segment and header as the original (sldo).
- /lib/gopt byte-identical (2026-11-21); built and installed by the tree (src/cmd SUBDIRS).

## Owner rules for compiler changes (2026-11-21)
- Ladder: source forms (exhausted, each form written down) > change an existing switch > a new switch only when changing an
  existing one changes other users > a new compiler (only with the owner). A trace (KDBG) to understand the mechanism comes
  first; it shows which source form could give the original code.
- The shipped (default) codgen/scodgen/popt stay byte-identical: change them only inside `#ifdef OLD_*`. Bugs of the originals
  are kept. Never change checkall or the nightly run without asking.

## The checks (each compiler change, before its commit)
1. Models and bench: `oldcc.sh build codgen v0`, `oldcc.sh build codgen v0p` (rm the installed file first), then `oldcc.sh model P` for
   cparse scparse codgen scodgen popt spopt cpp. Expected: strictly equal, 179/179/209/210/73/73/26. `oldcc.sh bench`: 153/132/212.
2. `host/recon/oldcc.sh matrix check` and `MATRIX_VER=vc ... matrix check`; a changed codgen switch: `MATRIX_VER=vc oldcc.sh
   matrix OLD_X` (cas baseline 0). The matrix knows codgen switches only; a popt (spopt) switch is checked on cas by checkall.
3. gopt total: `fnc.py -a` over all src/cmd/gopt/*.c (321).
4. checkall with umask 002 (the emulator tests expect it): `R=$(git stash create); make -C host checkall WEGA=.../tmp/wega31_root
   NAME=x PATCH=$R`. Expected: ALL OK (about 25 min, in the background).

## Tools
- `RECON_PROG=gopt python3 host/recon/fnc.py FILE.c FUNC` (diff in tmp/tmp/fnc/FUNC.diff, `FNC_CTX=N`; `-a FILE.c`; `FNC_ROOT=DIR`
  another compiler root). `odisc.py FUNC` (original listing), `datadiff.py`, `datastr.py`, `segcmp.py`, `host/tools/bincmp.py`.
- vc builds: `cd src/cmd/codgen; rm build/segvc/*.o; PATH=.../tmp/tmp/wbin:$PATH make build/segvc/scodgen` (spopt:
  `src/cmd/popt`, `make build/segvc/spopt`), then rm and cp to tmp/oldroot/lib/vc/. For experiments a copy of tmp/oldroot (a
  scratch root, `FNC_ROOT`) keeps the installed compiler untouched.
- Traces: temporary `printf("KDBG ...")` (getreg, rsreturn, rbusy, a tree dump in order; never committed), `sccvc -S`.
- The vc scodgen is near 64 KiB of text (host/OLDCC.md, vc): measure its size after each change.

## Next steps (one at a time)
1. (done 2026-11-21: /lib/gopt byte-identical after the link and strip: library start-up code, five library members in
   segment 4, about 50 functions found by the strict compare of the linked file (host/recon/framemap.py; frame layout, chain
   and statement order, constants, globals, members, tails), vc OLD_D14T for initopts; gopt in SUBDIRS of src/cmd)
2. (done 2026-11-21: findings.json: every function exact, the notes say only what is odd in the source; FINDINGS.md has
   only "Exact, with a note"; findings.py leaves empty sections out)
3. compare/gopt/COMPILER.md, after the model of src/lib/libcisam/COMPILER.md: the switches gopt needs, each with its witness
   and mechanism (take the witness table of SWITCH_CANDIDATES.md over). Move src/lib/libcisam/COMPILER.md to
   compare/libcisam/COMPILER.md and fix its references (host/OLDCC.md, host/OLDCC_LOG.md, host/HANDOFF_libcisam.md, comments in
   src/cmd/codgen allo3.c and reader3.c: comments only, the codgen stays byte-identical).
4. Delete compare/gopt/SWITCH_CANDIDATES.md (its content in COMPILER.md, OLDCC_LOG.md).
5. The switch matrix of vc (host/recon/switchmatrix.py) measures gopt too, not only cas; then rebuild the vc matrix
   (host/OLDCC_SWITCHES_VC.md) and `matrix check`.
6. gopt is partly made from the mip of PCC (the machine independent part). Find the best fitting mip: search tmp/sys3,
   tmp/sysv, tmp/plexus, ~/git/unix-history-archive for the version nearest to those parts of src/cmd/gopt.
7. Align the code of src/cmd/gopt that comes from mip with that mip (names, order, forms), gopt byte-identical throughout.
   Watch for places where a wrong source form of gopt made a vc switch necessary: the mip form may do without it.
8. `make shipped` in src/cmd builds src/cmd/gopt with the shipped compilers; where not, `#ifdef SHIPPED` fixes.
9. Port src/cmd/gopt to host/gopt (as the other host ports).
10. A corpus compare/gopt (corpus.py): the original /lib/gopt of WEGA 3.1 against 1. the gopt of `make shipped` and 2. host/gopt.
11. The vc switches with one or two witnesses (matrix): think hard again whether a source form of the witness does without
    the switch. Also the goto forms that stand for an optimizer layout: deledge (`goto in;` into `while (v != ..)`: the
    original enters the loop body, its test stands above it; do-while and for (;;) with the tests at the end give other
    code; perhaps a do-while the old popt turned round: then a question of spopt).
12. v0 / v0p against vc: unify, merge or simplify switches (switch consolidation); every change through the checks above.
