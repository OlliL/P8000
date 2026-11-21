# gopt: handoff (2026-11-21)

GOAL: /lib/gopt rebuilt from C (src/cmd/gopt), byte-identical after the link; then clean up, port to the host and consolidate
the compiler switches. Compiler: sccvc (vc = v0p segmented + vc switches, host/OLDCC.md, compare/gopt/COMPILER.md). Rules: CLAUDE.md (German in
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
3. (done 2026-11-21: compare/gopt/COMPILER.md, the switches gopt needs with their witnesses; the libcisam one moved to
   compare/libcisam/COMPILER.md)
4. (done 2026-11-21: SWITCH_CANDIDATES.md deleted; its witness table is in COMPILER.md, its observations in OLDCC_LOG.md)
5. (done 2026-11-21: the vc matrix measures cas and gopt (host/recon/goptmodel.py), rebuilt: host/OLDCC_SWITCHES_VC.md)
6. (done 2026-11-21: `make shipped` builds src/cmd/gopt (Makefile: an empty rule for /lib/slibc.a; before, the loop of
   `make shipped` hid the failure). Its output equals /lib/gopt's: 18 gopt sources, cc and scc, -O1..-O5 (180 runs of
   `cc -S -On -TO./gopt`; beta2.c -O4/-O5 stops with "illegal bsize type" in both). `#ifdef SHIPPED`: dominators() (a bug
   of the shipped scodgen), flow_graph() (BBLOCK operand kinds unset in the original), alloclpdef() (the rr2 of the
   original; no effect measured). KNOWLEDGE.md, quirks. Method: the passes by hand (cpp, cparse -E b, gopt -3 FLAGS -E b)
   with the debug flags (d dominators, l live, p loops, r registers, y all), both outputs diffed with the addresses
   masked; a mix of vc and shipped objects does not run (other stack conventions); an instrumented vc build overflows
   segment 4.)
7. (done 2026-11-21: host/gopt, taken over from src/cmd/gopt by the port rules (host/HANDOFF_wega_disasm.md, the new section
   "Global optimizer"), -Wall -Wextra clean; its output and .D file equal /lib/gopt's on the 18 gopt sources, cc and scc,
   -1..-5 (180 runs, cpp and cparse/scparse of WEGA under z8k-emu), also the builds with the sanitizers and -ftrapv and with
   zero and pattern initialised locals. Not yet in host/Makefile nor checkall: with the corpus (step 8), after asking.)
8. (done 2026-11-22: compare/gopt/corpus.py: every C source of src made into x.I, x.S, x.D by the original cc and scc,
   then gopt -1 .. -5 (scc: -Z) one after the other on the same files; output and files compared. Dry run: wega (make
   shipped) and host 942 of 942 (104 sources skipped), cold 1137 s (the slowest corpus: first in compare/CORPORA). Found
   on the way: the shipped build prints the rval of a unary node from its own layout (SHIPPED: -30720 as the original),
   host: %ld of the most negative long prints 0, the 64 KiB of an area (order3.c: out of space), r5 through checkalias,
   segment 0 for the vectors that a preheader record does not have. host/gopt in host/Makefile and the host build of
   checkall.)
9. Centralize the parts of PCC's mip that the passes share: common, rdr and optim are in cparse and codgen (version 2.5)
   and in gopt (2.4). One place in the tree for them, used by cc/cparse/codgen/popt/gopt, every build byte-identical
   (the differences of the versions behind switches or #ifdef). Comes after the corpus (step 8) and before the switch
   work (steps 10, 11), so that those work on the final structure.
10. The vc switches with one or two witnesses (matrix): think hard again whether a source form of the witness does without
    the switch. Also the goto forms that stand for an optimizer layout: deledge (`goto in;` into `while (v != ..)`: the
    original enters the loop body, its test stands above it; do-while and for (;;) with the tests at the end give other
    code; perhaps a do-while the old popt turned round: then a question of spopt).
11. v0 / v0p against vc: unify, merge or simplify switches (switch consolidation); every change through the checks above.
