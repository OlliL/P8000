# HOWTO: reconstructing a WEGA binary

Goal: `make install` gives the original, `cmp` is the only proof (`checkall`). Bugs of the original are rebuilt and listed in `KNOWLEDGE.md`, "Bugs and quirks". Rules of the project: `../CLAUDE.md`. The plan: `increments/README.md` (new programs: increments 140 to 160).

Two models, one handoff per program. Phase A is cheap and mechanical: a tool answers yes or no. Phase B needs understanding.

Phase A is a script and two subagents (`.claude/agents/`, increment 140), each with a fixed input, a fixed report and a list of
what it must not decide; the main session reads each report, decides and starts the next step:

| Step | Done by | The main session after it |
|---|---|---|
| 1 Setup | `host/recon/setup.py` (no model) | fixes compiler and library (`progcfg.py` CC, `findings.json` compiler); a library gap |
| 2 Build and compare | agent `recon-match` (sonnet) | checks `bincmp.py`; hooks the program into `src/cmd/Makefile` (one file: SIMPLE rules as `ranlib`; a directory: SUBDIRS, `shipped`) |
| 4 Corpus | agent `recon-corpus` (sonnet) | runs the corpus; the QUIRKS to `KNOWLEDGE.md`, the object to `OLDCC.md`, section 4 |

Step 5, checkall, the two tables, a compiler question and phase B stay with the main session. The prompt of an agent gives
every known value (NAME, ORIG, the source and its System III file, COMPILER, LIBC, stripped, OWN_LIM, the built candidate),
so that the agent looks nothing up. Agents of the same model run one after the other: the second reads the common start
of the context from the cache. New agent definitions are found by Claude Code at the start of a session only.

## Phase A (cheap model)

Never in phase A: change a compiler pass or a switch, change cas, edit a shipped header, write odd forms to force bytes, spend more than 6 attempts on one function. When stuck: note it and go on. A function left open is a normal result.

1. **Setup: `python3 host/recon/setup.py NAME [ORIG]`** (ORIG below the WEGA root, default `bin/NAME`; nonsegmented programs).
   It does the mechanical start and prints a report (RESULT, SCCS, SOURCE, DATE, COMPILER, OWN_LIM, LIBC, FILES); an existing
   source, `progcfg.py` or `findings.json` is kept. What it does, and what the main session decides from it:
   1. **SCCS lines.** The `@[$]` strings of the binary name the own source files, their order and dates; several lines mean
      several files, a date other than System III's means WEGA changed it.
   2. **Source from System III** (`tmp/sys3/usr/src`, the path of `src/COVERAGE.md`, else `cmd/NAME.c` or `cmd/NAME/`), copied
      unchanged to the same place in the V7 layout: a single file to `src/cmd/NAME.c` (its rules in `src/cmd/Makefile`: SIMPLE,
      `ranlib`, `size` are the pattern), a directory to `src/cmd/NAME/` with its own Makefile (`dc/Makefile` is the pattern).
      A source not below `cmd` is only named. What WEGA changed against System III goes later to `compare/NAME/SYSIII.md`.
      For what is specific to the Z8000 the Plexus sources (`tmp/plexus`) may be read, not copied. Without a source: step 3.
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
2. **Build and compare** (`recon-match`). The own code with the stdio.h of the time for ccv0 and ccv1 (`src/head/oldv7`), the
   start-up file from `crt0.list`, linked with LIBC (`E="z8k-emu -r $OLDROOT"; $E /bin/ccv1 -O -I../head/oldv7 -c x.c`; the old
   compilers need `make -C host oldroot` once; the emulator sees only paths below its root and the working directory). Then
   `host/tools/bincmp.py ORIG NEW -v` (NEW stripped when ORIG is), in this order; each one moves the rest:
   - Data size, then data bytes: strings stand in the order of the statements of each function; equal strings are not merged.
   - Bss: commons are allocated in the order of first mention in link order. Reorder the declarations; array sizes are address distances (`-v` shows new -> original).
   - Code: the blocks of `bincmp.py`, per block at most 6 attempts with the forms below. Then record it in `findings.json` (per function a `note`, only for an open difference, an odd form, a guessed name, dead code) and go on. `odis.py NAME` shows the original of a function of findings.json, `odis.py LO HI` any range.
   The Makefile rules (an `install` target that strips when the original is stripped; `make install` of `src/cmd` must reach it) are written by the main session.
3. **No source.** One C file per module, K&R, a draft per function from `odis.py`, compared by `fncheck.py DRAFT LO HI` (compiled with progcfg CC under `WEGA_ROOT`; for an old compiler: `CC = '/bin/ccv1'` and `WEGA_ROOT=$OLDROOT`). Names from messages, system calls, strings. The source carries no status; findings go to `findings.json` only.
4. **Corpus** (`recon-corpus`). The bytes do not prove a draft while it is open. The original and ours run under the emulator on the same cases; outputs, messages, exit codes and the files left behind are compared: `compare/NAME/corpus.py` and `compare/NAME/cases/` or the blocks of `compare/NAME/tests.txt` (`host/recon/casefile.py`; dc, rm, mv, tar, sum are patterns: every case with an expectation from the man page or the source) (the case format: head comment of `host/recon/corpusrun.py`; `--show` runs the original alone; the C sources as inputs: `host/recon/corpusc.py`; design: `host/increments/110-corpus-architecture.md`), run by `host/tools/corpus.sh NAME` (`CORPUS_BIN_WEGA=FILE`: a build outside the tree). Run it after each batch; `checkall` runs it on a build by the shipped cc.
5. **Handoff or done** (main session). `IDENTICAL`: the install target lets `checkall` compare it; `findings.py NAME` writes `FINDINGS.md`; report. Otherwise a section "Current work" in `HANDOFF_wega_disasm.md`, as short as possible: compiler, the output of `bincmp.py`, per open function what was tried (the functions themselves: `findings.json`). Commit.

### Forms that often decide (in this order)

More, found on sld: `KNOWLEDGE.md`, "what fixed the findings functions".

- The compiler or the stdio.h of the wrong time. Try the neighbour version before anything else.
- Wrong types, wrong parameter order, wrong declaration order: most "register differences" are these. Register variables go from r14 (scc r13) downwards in declaration order; a pointer in scc takes a pair. A parameter not spilled is `register`. `ldm` counts the registers.
- Statement forms: `x = a + b; x /= 8;` against `x = (a + b) / 8`; `if`/`else` against `?:`; `return` against an else chain; `switch` with the real case values, also explicit cases that do what the default does (a jump table shows them); case order is code order.
- Plain forms first. `goto`, unused labels and spare locals only when nothing plain fits; mark them in `findings.json` as odd forms.
- `call` where `calr` reaches: the callee is in another file, or the old cas decided it long (`OLDCC.md`, D17). A backward `call` in reach always means another file.

## Phase B (better model)

For what is left: finding the source form of a hard function, and changing the compiler.

- Source form first. A new switch comes only when no form fits; forms tried go to `OLDCC_LOG.md`.
- How popt treats a function: `recon/poptrace.sh PROG FUNC`, or popt built with `-DTRACE`. Which copy of a tail stays depends on the order iterate, comjump, rmove (`OLDCC.md`, D20). When the mechanism is clear but the form is not, try the combinations by script instead of by hand.
- A change of a pass, measured in a copy: `recon/oldvar.sh NAME PATCH.py [PASS]`. Removing a switch: a patch deleting its `#define`.
- Compiler changes are in `src/cmd/{cparse,codgen,popt,cas}` behind a switch, set in the version (`codgen/oldcc.h`, the `build/nonsegVER` rules of the pass Makefile). The normal build stays identical.
- Function-level check for a program with a model: `RECON_PROG=NAME python3 host/recon/strictcmp.py` (`-c` also compares call forms, `STRICT_CTX=12 ... -v FUNC` shows context). A model (`MODULES`, `model_link` in `compare/NAME/progcfg.py` like `compare/cpp` or `popt/tools`) is needed for a program of v0p; add it to PROGS of `recon/switchmatrix.py`.
- Structs and names when the bytes stand: `rename.py`; sizes from address distances, names from debug strings, then System III, then Plexus for Z8000 parts (the WEGA changes: `compare/NAME/SYSIII.md`). The exact count must not drop.

## Evidence rules for the old compilers

- Which compiler (owner 2026-11-05): 1. the date of the program (`OLDCC_DATES.md`) names the candidate, the version whose time it lies in (`OLDCC.md`, section 2). 2. Always measure the neighbouring versions too. 3. A new rule goes into the existing version first; `checkall` shows whether its programs stay identical. 4. A new version only when two programs or modules provably contradict each other; it must fit the time line and differ from its neighbour by as few switches as possible. 5. Time and witnesses of each version stand in `OLDCC.md`.
- A switch needs two independent functions, preferably in different programs. A function and its segmented copy are one. A function that fits only through a switch nothing else needs is a suspect source form. Exceptions only by the owner; they are listed in `OLDCC.md`, section 5.
- No regression: every exact function stays exact in every program of the same compiler version. A program of another time is no counter-witness.
- The odder a form, the less likely it was written, even when the bytes fit. An odd form that fits is recorded in `findings.json`; a plainer form or a switch stays possible.
- The test bench (`recon/witness.py`, the System III sources of `tmp/sys3`) is read only by comparison with and without a switch: WEGA changed many System III sources.
- A switch name has at most 8 characters: the cpp of the old compilers reads no more (`OLD_D2POV` was `OLD_D2PO`; the matrix then shows no witness for either).
- After a switch: `oldcc.sh matrix SWITCH` (all switches when a program joins), `oldcc.sh matrix check` before the commit, `make -C host checkall`, `OLDCC.md` updated. `OLDCC_SWITCHES.md` is generated. The switches of sccvc (`OLD_VC`, the `segvc` rules; only /bin/cas is built with it) the same way with `MATRIX_VER=vc oldcc.sh matrix [SWITCH ...]` and `MATRIX_VER=vc oldcc.sh matrix check`: `OLDCC_SWITCHES_VC.md`, measured on the model of cas, with the data bytes (a switch that only changes data, as `popt:OLD_D13`) and, for a switch without a lost function, the functions whose lines change.
