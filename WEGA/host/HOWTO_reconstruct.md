# HOWTO: reconstructing a WEGA binary

Goal: `make install` gives the original, `cmp` is the only proof (`checkall`). Bugs of the original are rebuilt and listed in `KNOWLEDGE.md`, "Bugs and quirks". Rules of the project: `../CLAUDE.md`. The plan: `increments/README.md` (new programs: increments 140 to 160).

One program per session: a fresh session (opus) with the short start message below does phase A for one program, from
`setup.py` to the commit; a long session that carries other work costs more in every turn than the whole program
(increment 140, measured on cpio). Subagents are not used: the cheaper model made wrong source changes that cost more to
find than it saved (the definitions in `.claude/agents/` stay, used only when the owner names them). Phase B (an open
block that needs the compiler) is a session of its own, for the open blocks of several programs with the same mechanism.

Start message of a session (NAME, ORIG and the increment filled in):

    WEGA, increment 160: rebuild /ORIG (NAME) by WEGA/host/HOWTO_reconstruct.md, phase A, steps 1 to 5.
    Read WEGA/CLAUDE.md and the HOWTO first. No subagents. A block still open after 6 attempts: note it in findings.json,
    put the file in OPEN of host/tools/checkall.sh, do not go deeper. At the end: checkall, the two tables, COVERAGE.md,
    commit; a short report (bincmp, corpus, changed files).

Several programs unattended, one fresh session each: `host/tools/nightly.sh [-b DOLLARS] NAME[:ORIG] ...` (the session commits,
the script runs checkall on each commit and stops at the first failure; at the end COVERAGE.md; logs in `tmp/tmp/nightly`).

## Procedure of a long session (context economy)

A reconstruction without source (gopt) runs for many sessions; what costs context is the output of the tools, not the thinking. Rules:

- One module, or one large function, per session; then `/clear`. The state is in `compare/NAME/HANDOFF.md`, `findings.json` (name, file,
  status, note of every function, kept up to date after every function) and the increment log, not in the conversation.
- Never print a whole disassembly or diff into the conversation. `odisc.py -s FUNC` (calls, strings, size), `odisc.py -o FUNC` (listing to
  a file, then `sed -n a,bp`), `fnc.py FILE.c FUNC ...` (one line per function, the diff in `tmp/tmp/fnc/FUNC.diff`), `fnc.py -a FILE.c`
  (only the names that are not OK). A diff is read in parts (`sed -n`), a listing only for the function in hand.
- Write the draft of a function from the outline and the listing, compile and compare it at once; at most 6 attempts per block, then note
  it in `findings.json` and go on. A recurring cause is written once in HANDOFF.md, not in every note again.
- After each function: `findings.json` (name, file, status, note; `rename.py [-m]` for names), `make part`; a commit per module step.
  A case block or tail of the original that belongs to a function gets `part_of: FUNC` (fnc.py takes it into the range), no note.
  A note says only what is odd in the source (dead code, an unused local, a goto form, a guessed name, a bug of the original),
  not what was tried: attempts go to `OLDCC_LOG.md` when they concern the compiler, else nowhere.
- When fnc.py has every function masked-equal, the program is not done: link it and compare strictly (Phase B, framemap.py);
  the masked compare hides frame layout, constants, globals and members (gopt: about 50 functions).
- Before the context is 70% full: finish the function in hand, update HANDOFF.md, commit, and give the owner the variable part of the next
  start message (below). Do not start a new large function late in a session.

Start message of such a session (only the part between the lines is filled in by the owner; the AI writes it for the next session):

    WEGA, increment 150, program gopt: continue by WEGA/host/HOWTO_reconstruct.md ("Procedure of a long session").
    Read WEGA/CLAUDE.md, WEGA/compare/NAME/HANDOFF.md (the one of the program) and only the file of the increment. No subagents.
    ---- what we do now ----
    TASK: <the module / function / question of this session, e.g. "alias2.c: 4:7c68 checkops with its blocks">
    START: <what to do first, e.g. "struct qa: unsigned w0, then fnc.py -a on all files">
    STOP WHEN: <the end of the piece, e.g. "checkops written; or at 70% of the context">
    ----
    At the end: findings.json, HANDOFF.md, the increment log, commit; then print the block "what we do now" for the next
    session (TASK, START, STOP WHEN), filled in from the real state: what is done, what is open, what comes next.

## Phase A

Never in phase A: change a compiler pass or a switch, change cas, edit a shipped header, write odd forms to force bytes, spend more than 6 attempts on one function. When stuck: note it and go on. A function left open is a normal result.
Phase A looks for changes of the source only (forms, options, headers of the build, WEGA's own edits). Differences that no
source change removes are kept for phase B in `findings.json`: grouped into clusters where several functions show the same
kind of difference (probably one cause), each with its functions, what was tried and the findings or guesses so far. A trace
of a pass (KDBG, poptrace) is phase B (owner 2026-11-23).

1. **Setup: `python3 host/recon/setup.py NAME [ORIG]`** (ORIG below the WEGA root, default `bin/NAME`; nonsegmented programs, E707 or E711; the library part includes a termlib linked before the libc).
   It does the mechanical start and prints a report (RESULT, SCCS, SOURCE, DATE, COMPILER, OWN_LIM, LIBC, FILES); an existing
   source, `progcfg.py` or `findings.json` is kept. What it does, and what the main session decides from it:
   1. **SCCS lines.** The `@[$]` strings of the binary name the own source files, their order and dates; several lines mean
      several files, a date other than System III's means WEGA changed it.
   2. **Source from System III** (`tmp/sys3/usr/src`, the path of `src/COVERAGE.md`, else `cmd/NAME.c` or `cmd/NAME/`), copied
      unchanged to the same place in the V7 layout: a single file to `src/cmd/NAME.c` (its rules in `src/cmd/Makefile`: SIMPLE,
      `ranlib`, `size` are the pattern), a directory to `src/cmd/NAME/` with its own Makefile (`dc/Makefile` is the pattern).
      A source not below `cmd` is only named. What WEGA changed against System III goes later to `compare/NAME/SYSIII.md`.
      For what is specific to the Z8000 the Plexus sources (`tmp/plexus`) are the second base; they may be copied from too (owner 2026-11-22). Without a source: step 3.
   3. **Dating.** The line of `OLDCC_DATES.md`, and the compiler of that time (`OLDCC.md`, section 2): D1 tight = v0 (1982),
      slots = v1 (01/83 .. 07/83, neighbours v1r, v1q), loose with the libc of 09/83 = the shipped cc (scc). The script names the
      candidate; the main session decides, measuring the neighbours too ("Evidence rules").
   4. **compare/NAME/.** `progcfg.py` (NAME, ORIG, OWN_LIM, ORIG_LIM when stripped, CC; for a segmented program SEG and CC by
      hand, `compare/sld` is the pattern), the inventory (`inventory.py`, kept in `tmp/tmp/NAME/inv.json`) and the first
      `findings.json` (`own_table.py`: every function start `f<entry>` with orig, size_orig, status open). Names later with `rename.py`.
   5. **Library and OWN_LIM.** The walk of `host/tools/whichlibc.py` over every `src/lib/libc-vN.a` and the shipped `/lib/libc.a`: the libc
      starts at the first member of at least 32 bytes (with the members right before it); the best library covers the most
      bytes from there. No gap: that library is LIBC. A gap is a module of another version, or a program of another library: the
      main session decides. `ld` takes from an archive what is referenced, in its order. `crt0.list` must have the program.
2. **Build and compare**. The own code with the stdio.h of the time for ccv0 and ccv1 (`src/head/oldv7`), the
   start-up file from `crt0.list`, linked with LIBC (`E="z8k-emu -r $OLDROOT"; $E /bin/ccv1 -O -I../head/oldv7 -c x.c`; the old
   compilers need `make -C host oldroot` once; the emulator sees only paths below its root and the working directory). Then
   `host/tools/bincmp.py ORIG NEW -v` (NEW stripped when ORIG is), in this order; each one moves the rest:
   - Data size, then data bytes: strings stand in the order of the statements of each function; equal strings are not merged.
   - Bss: commons are allocated in the order of first mention in link order. Reorder the declarations; array sizes are address distances (`-v` shows new -> original).
   - Code: the blocks of `bincmp.py`, per block at most 6 attempts with the forms below. Then record it in `findings.json` (per function a `note`, only for an open difference, an odd form, a guessed name, dead code) and go on. `odis.py NAME` shows the original of a function of findings.json, `odis.py LO HI` any range.
   The Makefile rules: an `install` target that strips when the original is stripped; `make install` of `src/cmd` must reach it (one file: SIMPLE rules as `ranlib`, `pr`; a directory: SUBDIRS, `shipped`). A file not yet identical goes to `OPEN` of `host/tools/checkall.sh`.
3. **No source.** One C file per module, K&R, a draft per function from `odis.py`, compared by `fncheck.py DRAFT LO HI` (compiled with progcfg CC under `WEGA_ROOT`; for an old compiler: `CC = '/bin/ccv1'` and `WEGA_ROOT=$OLDROOT`). Names from messages, system calls, strings. The source carries no status; findings go to `findings.json` only.
4. **Corpus**. The bytes do not prove a draft while it is open. The original and ours run under the emulator on the same cases; outputs, messages, exit codes and the files left behind are compared: `compare/NAME/corpus.py` and `compare/NAME/cases/` or the blocks of `compare/NAME/tests.txt` (`host/recon/casefile.py`; dc, rm, mv, tar, sum are patterns: every case with an expectation from the man page or the source) (the case format: head comment of `host/recon/corpusrun.py`; `--show` runs the original alone; the C sources as inputs: `host/recon/corpusc.py`; design: `host/increments/110-corpus-architecture.md`), run by `host/tools/corpus.sh NAME` (`CORPUS_BIN_WEGA=FILE`: a build outside the tree). Run it after each batch; `checkall` runs it on a build by the shipped cc.
5. **Handoff or done.** `IDENTICAL`: the install target lets `checkall` compare it. When `checkall` is ALL OK: the functions of `findings.json` get their names (`f<entry>` -> the name: `nm -n` of the unstripped build, or `rename.py` where the sources must change too; the start-up code has status `crt0`) and status `exact`; `findings.py NAME` writes `FINDINGS.md`, which lists only the open and masked functions and the exact ones with a note (odd forms, guessed names, dead code, a kept bug); `findings.json` keeps every function. Then `src/COVERAGE.md` again: `WEGA=<WEGA_ROOT> CHECK_NAME=cov sh host/tools/checkall.sh -b` (build and install only, the install tree stays in /tmp/wega_check.cov/install), `INSTALL=/tmp/wega_check.cov/install python3 host/tools/coverage.py`. Report. Otherwise a section "Current work" in `HANDOFF_wega_disasm.md`, as short as possible: compiler, the output of `bincmp.py`, per open function what was tried (the functions themselves: `findings.json`). Commit.

### Forms that often decide (in this order)

More, found on sld: `KNOWLEDGE.md`, "what fixed the findings functions".

- The compiler or the stdio.h of the wrong time. Try the neighbour version before anything else.
- Wrong types, wrong parameter order, wrong declaration order: most "register differences" are these. Register variables go from r14 (scc r13) downwards in declaration order; a pointer in scc takes a pair. A parameter not spilled is `register`. `ldm` counts the registers.
- Statement forms: `x = a + b; x /= 8;` against `x = (a + b) / 8`; `if`/`else` against `?:`; `return` against an else chain; `switch` with the real case values, also explicit cases that do what the default does (a jump table shows them); case order is code order.
- The frame (segmented scodgen; `KNOWLEDGE.md`, "codgen fidelity"): the locals lie in the order of their declaration behind the
  saved parameters, then the locals of inner blocks, then the temporaries. A frame of the original that is larger means an
  unused local (or a register variable that is an auto there), a smaller one a local too many or one variable used for two
  purposes; a mapping that only mirrors when two declarations are swapped is a chain or statement order (`q = last = x`
  against `last = q = x`, or two statements).
- Plain forms first. `goto`, unused labels and spare locals only when nothing plain fits; mark them in `findings.json` as odd forms.
- `call` where `calr` reaches: the callee is in another file, or the old cas decided it long (`OLDCC.md`, D17). A backward `call` in reach always means another file.

## Libraries (increment 150)

A library of `/lib` or `/usr/lib` (`libX.a` and its segmented `slibX.a`) the same way, one fresh session per library; the
pattern is `src/lib/libl` (Makefile with build/nonseg and build/seg, `mkar.sh`, `arhdr_*.s`, `Library.SC`).

1. **Setup: `python3 host/recon/libsetup.py libX`.** For every member of `libX.a` and `slibX.a` the source, from System III
   (`tmp/sys3/usr/src/lib/libX`), else 2.11BSD (`usr.lib/libX`), else 4.4BSD-Lite (`usr/src/lib/libX`), in each tree by name,
   else by its object (a file WEGA renamed; liby: `libmai.c` is `main.c`); an odd last member: its padding byte in `arhdr_*.s`. It copies the sources to `src/lib/libX/` under WEGA's names (`Library.SC`
   names them), writes `mkar.sh`, `arhdr_libX.s`, `arhdr_slibX.s` (the member headers of the originals) and `Library.SC`, and
   reports per member the best compiler (every compiler of its kind, with and without -O, with the headers of the guest,
   `src/head/oldv7`, `src/head/oldcptr`): EXACT or the bytes that differ. A library without any source directory is skipped.
2. **Makefile** after `src/lib/libl/Makefile`: the compiler of the time (`Library.SC` dates the sources; `OLDCC.md`, section
   2), the members in the order of the archive; `install` to `lib` or `usr/lib`; `src/lib/Makefile` must reach it.
3. **Members:** each member to EXACT as in phase A, step 2 (source forms, at most 6 attempts per member, a member that stays
   open: `findings.json` of `compare/libX`, the archive in `OPEN` of checkall). A member without a source: note it.
4. **Done:** the archive byte-identical (`cmp`), the two tables, `COVERAGE.md`, commit. A library has no corpus of its own:
   the programs that link it are its test.

Unattended: `host/tools/nightly.sh lib:libX ...` (one session per library, the start message of a library).

## Phase B

For what is left: finding the source form of a hard function, and changing the compiler.

- The order (owner 2026-11-21): source forms, exhausted and written down; then an existing switch changed; a new switch only
  when changing an existing one changes other users; a new compiler only with the owner. A compiler change is never the
  first reaction to a difference: a trace (below) first. Forms tried go to `OLDCC_LOG.md`.
- When no source form fits, the difference is not fixed at once but collected: `compare/NAME/SWITCH_CANDIDATES.md`, one
  row per kind of behaviour (an id K1, K2, ...; the guessed mechanism and pass; the open witnesses with their differing
  lines; the status: candidate, built, refuted). A new difference of the same kind joins its row as a witness; the
  search for source forms of the other functions goes on. Only when the source phase is through are the candidates
  worked one by one, the one with the most witnesses first: trace, then an existing switch changed or a new one built,
  each with all its witnesses at once (gopt: K5, K7/K8 were each one mechanism behind several functions). A witness that
  becomes exact leaves its row; a row without witnesses leaves the file. When the program is done the file is deleted:
  the switches it led to go to `compare/NAME/COMPILER.md` (switch, pass, mechanism, witnesses), the rest to `OLDCC_LOG.md`.
- How popt treats a function: `recon/poptrace.sh PROG FUNC`, or popt built with `-DTRACE`. Which copy of a tail stays depends on the order iterate, comjump, rmove (`OLDCC.md`, D20). When the mechanism is clear but the form is not, try the combinations by script instead of by hand.
- A change of a pass, measured in a copy: `recon/oldvar.sh NAME PATCH.py [PASS]`. Removing a switch: a patch deleting its `#define`.
- Before a compiler change, understand the mechanism by a trace: a temporary `printf("KDBG ...")` in the pass (codgen: rsuse
  in getreg and rsreturn, rbusy, a tree dump in order), the module compiled with `-S` by a compiler in a copy of OLDROOT
  (fnc.py: `FNC_ROOT`), never committed. The trace often shows the source form (a variable used for two purposes, a
  statement order) instead of a switch.
- The masked compare (fnc.py) hides frame offsets, constants, globals and members: when the masked count is complete, link
  and compare strictly (`host/tools/bincmp.py`; segmented: `recon/framemap.py`, the frame offsets new -> orig per function).
- The checks of each compiler change, before its commit (from the gopt reconstruction, 2026-11-21/22): (1) the models:
  `oldcc.sh build codgen v0`, `v0a` and `v0p` (remove the installed file first; v0a: then ex, 339 of 391 functions exact), then `oldcc.sh model P` for cparse scparse codgen
  scodgen popt spopt cpp, all strictly equal, and `oldcc.sh bench`; (2) `recon/oldcc.sh matrix check` and
  `MATRIX_VER=vc ... matrix check`, a changed codgen switch measured with `MATRIX_VER=vc oldcc.sh matrix OLD_X` (the matrix
  knows codgen switches; a popt switch is checked on cas by checkall); (3) the program itself (`fnc.py -a` over its files);
  (4) checkall with umask 002 on the change (`R=$(git stash create); make -C host checkall ... NAME=x PATCH=$R`), ALL OK.
  The shipped codgen, scodgen, popt stay byte-identical: change them only inside `#ifdef OLD_*`; a cosmetic change of the
  passes (names, macros) is compared against a fixed good commit, every variant and `/lib` (a baseline rebuilt on the way
  hid one wrong instruction, 2026-11-22). 2-3 switch changes per checkall, each measured alone first.
- The vc build of a pass: `cd src/cmd/cc/codgen; rm build/segvc/*.o; PATH=<wbin>:$PATH make build/segvc/scodgen` (spopt:
  `src/cmd/cc/popt`, `build/segvc/spopt`), then into OLDROOT/lib/vc; experiments in a copy of OLDROOT (`FNC_ROOT`). The
  text of the vc scodgen is near 64 KiB: measure its size after each change.
- Compiler changes are in `src/cmd/{cparse,codgen,popt,cas}` behind a switch, set in the version (`codgen/oldcc.h`, the `build/nonsegVER` rules of the pass Makefile). The normal build stays identical.
- Function-level check for a program with a model: `RECON_PROG=NAME python3 host/recon/strictcmp.py` (`-c` also compares call forms, `STRICT_CTX=12 ... -v FUNC` shows context). A model (`MODULES`, `model_link` in `compare/NAME/progcfg.py` like `compare/cpp` or `popt/tools`) is needed for a program of v0p; add it to PROGS of `recon/switchmatrix.py`.
- Structs and names when the bytes stand: `rename.py OLD NEW [-n] [-m]` (sources, tables, findings.json, the .md of the program, KNOWLEDGE.md; `-m`: OLD is a placeholder for a function that exists under the name NEW, e.g. `fn1d22` in a module whose neighbour is written later: gopt; the external names must differ in their first 7 characters: delquad and delquads clash); `findings.py PROG` then writes FINDINGS.md; sizes from address distances, names from debug strings, then System III, then Plexus for Z8000 parts (the WEGA changes: `compare/NAME/SYSIII.md`). The exact count must not drop.

## Evidence rules for the old compilers

- The order of the remedies (owner, 2026-11-15/21/23): 1. source forms (written down, each one tried); 2. extend an existing
  switch whose rule the finding refines (grep `codgen/oldcc.h` and `OLDCC.md` for it first: the address form of an element rule goes
  into that rule, a parameter case at a label into the label rule); 3. a new switch name only when no existing switch covers the
  mechanism. Ask the owner before touching a pass.
- Which compiler (owner 2026-11-05): 1. the date of the program (`OLDCC_DATES.md`) names the candidate, the version whose time it lies in (`OLDCC.md`, section 2). 2. Always measure the neighbouring versions too. 3. A new rule goes into the existing version first; `checkall` shows whether its programs stay identical. 4. A new version only when two programs or modules provably contradict each other; it must fit the time line and differ from its neighbour by as few switches as possible. 5. Time and witnesses of each version stand in `OLDCC.md`.
- A switch needs two independent functions, preferably in different programs. A function and its segmented copy are one. A function that fits only through a switch nothing else needs is a suspect source form. Exceptions only by the owner; they are listed in `OLDCC.md`, section 5.
- No regression: every exact function stays exact in every program of the same compiler version. A program of another time is no counter-witness.
- The odder a form, the less likely it was written, even when the bytes fit. An odd form that fits is recorded in `findings.json`; a plainer form or a switch stays possible.
- The test bench (`recon/witness.py`, the System III sources of `tmp/sys3`) is read only by comparison with and without a switch: WEGA changed many System III sources.
- A switch name has at most 8 characters: the cpp of the old compilers reads no more (`OLD_D2POV` was `OLD_D2PO`; the matrix then shows no witness for either).
- After a switch (new or changed), before its commit: the v0/v0p models (`oldcc.sh build codgen v0|v0p`, then `oldcc.sh model P`
  for cparse scparse codgen scodgen popt spopt cpp: strictly equal) and `oldcc.sh bench` (not worse); `oldcc.sh matrix SWITCH`
  (all switches when a program joins) and `oldcc.sh matrix check`; the whole program at work (fnc.py -a, no regression);
  `make -C host checkall` (umask 002: the emulator tests expect it); `OLDCC.md` updated. A vc switch also: the text of the vc
  scodgen stays below 64 KiB (`OLDCC.md`, vc). `OLDCC_SWITCHES.md` is generated. The switches of sccvc (`OLD_VC`, the `segvc` rules; /bin/cas and /lib/gopt are built with it) the same way with `MATRIX_VER=vc oldcc.sh matrix [SWITCH ...]` and `MATRIX_VER=vc oldcc.sh matrix check`: `OLDCC_SWITCHES_VC.md`, measured on the model of cas and on gopt (`recon/goptmodel.py`: the whole program built and compared per function), with the data bytes (a switch that only changes data, as `popt:OLD_D13`) and, for a switch without a lost function, the functions whose lines change.
