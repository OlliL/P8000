# HOWTO: reconstructing a WEGA binary

Goal: `make install` gives the original, `cmp` is the only proof (`checkall`). Bugs of the original are rebuilt and listed in `KNOWLEDGE.md`, "Bugs and quirks". Rules of the project: `../CLAUDE.md`. The plan: `../src/TODO.md` (new programs: step 9).

Two models, one handoff per program. Phase A is cheap and mechanical: a tool answers yes or no. Phase B needs understanding.

## Phase A (cheap model)

Never in phase A: change a compiler pass or a switch, change cas, edit a shipped header, write odd forms to force bytes, spend more than 6 attempts on one function. When stuck: note it and go on. A function left open is a normal result.

1. **Source from System III first.** Look in `/mnt/share/cvs/P8000/tmp/sys3/usr/src/cmd` (`NAME.c` or a directory `NAME/`; libraries: `usr/src/lib`). If it is there, copy it to `src/cmd/NAME/` unchanged and start from it; the Makefile in the V7 layout (`size/Makefile` is the pattern). The SCCS strings of the binary (`strings | grep '@\[\$\]'`) name the files, their order and dates; several strings mean several files, a date other than System III's means WEGA changed it. What WEGA changed against System III goes to `compare/NAME/SYSIII.md`. For what is specific to the Z8000 the Plexus sources (`tmp/plexus`) may be read, not copied. Without a source: step 8 first.
2. **Date it.** `OLDCC_DATES.md` gives the time, the time gives the compiler (`OLDCC.md`, section 2): v0 1982, v1r 01/83, v1q and v1 01/83 .. 07/83, shipped cc (scc) from 09/83. Measure the neighbours too ("Evidence rules").
3. **compare/NAME/.** `compare/NAME/progcfg.py` with NAME, ORIG, OWN_LIM (the end of the own code; whichlibc.py shows it), SEG and CC for a segmented program (`compare/sld`, `compare/ld` are patterns). `RECON_PROG=NAME python3 host/recon/inventory.py > inv.json`, then `own_table.py inv.json`: the first `findings.json` (every function start `f<entry>` with orig, size_orig, status open). Names later with `rename.py`.
4. **Build the own code**, with the stdio.h of the time for ccv0 and ccv1 (`-Icparse` for a file in `src/cmd`, `-I../cparse` in a subdirectory):
   `E="z8k-emu -r $OLDROOT"; $E /bin/ccv1 -O -I../cparse -c x.c`. The old compilers need `make -C host oldroot` once.
5. **Library.** `host/tools/whichlibc.py ORIG OWNEND` (OWNEND = crt0 size + own code size, hex) tries every `src/lib/libc-vN.a` and `libc/libc.a` and shows the gaps of the best one. No gap: set `LIBC` in the Makefile of the program (the real compiler at `CC`, the library, few targets, an `install` target that strips when the original is stripped; `make install` of `src/cmd` must reach it). `ld` takes from an archive what is referenced, in its order. A gap is a module of another version, or a program of another library: note it, the handoff decides. `crt0.list` must have the program.
6. **Compare:** `host/tools/bincmp.py ORIG NEW -v` (NEW stripped when ORIG is). Work in this order; each one moves the rest.
   - Data size, then data bytes: strings stand in the order of the statements of each function; equal strings are not merged.
   - Bss: commons are allocated in the order of first mention in link order. Reorder the declarations; array sizes are address distances (`-v` shows new -> original).
   - Code: the blocks of `bincmp.py`.
7. **Functions.** Per block at most 6 attempts with the forms below. Then record it in `findings.json` (per function a `note`, only for an open difference, an odd form, a guessed name, dead code) and go on. `odis.py NAME` shows the original of a function of findings.json, `odis.py LO HI` any range.
8. **No source.** One C file per module, K&R, a draft per function from `odis.py`, compared by `fncheck.py DRAFT LO HI` (compiled with progcfg CC under `WEGA_ROOT`; for an old compiler: `CC = '/bin/ccv1'` and `WEGA_ROOT=$OLDROOT`). Names from messages, system calls, strings. The source carries no status; findings go to `findings.json` only.
9. **Corpus.** The bytes do not prove a draft while it is open. The original and ours run under the emulator on the same inputs; outputs, messages and exit codes are compared: `CORPUS_*` in `compare/NAME/progcfg.py` (`host/recon/stagecorpus.py` says what; `ld/tools`, `cas/tools` are patterns; the C sources as inputs: `host/recon/corpusc.py`), run by `host/tools/corpus.sh NAME`. Run it after each batch; `checkall` runs it on a build by the shipped cc.
10. **Handoff or done.** `IDENTICAL`: the install target lets `checkall` compare it; `findings.py NAME` writes `FINDINGS.md`; report. Otherwise a section "Current work" in `HANDOFF_wega_disasm.md`, as short as possible: compiler, the output of `bincmp.py`, per open function what was tried (the functions themselves: `findings.json`). Commit.

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
- After a switch: `oldcc.sh matrix SWITCH` (all switches when a program joins), `oldcc.sh matrix check` before the commit, `make -C host checkall`, `OLDCC.md` updated. `OLDCC_SWITCHES.md` is generated. The switches of sccvc (`OLD_VC`, the `segvc` rules; only /bin/cas is built with it) the same way with `MATRIX_VER=vc oldcc.sh matrix [SWITCH ...]` and `MATRIX_VER=vc oldcc.sh matrix check`: `OLDCC_SWITCHES_VC.md`, measured on the model of cas, with the data bytes (a switch that only changes data, as `popt:OLD_D13`) and, for a switch without a lost function, the functions whose lines change.
