# 165 The switch matrix for every compiler version

## Status

planned (owner 2026-11-24); not started.

## Goal

For every old compiler version (v0, v0a, v0p, v1, v1q, v1r, v1c, vk, vc, vl, the segmented ones) a table of its switches and
their witnesses: each switch that is active in the build of that version, taken out alone, measured on every program and
library module built with that version, function by function. So it can always be seen which switch has which witnesses
(and which has none in a version), and `matrix check` notices when a switch, a program or a version changes without the
table. Today only v0p (on the passes, `OLDCC_SWITCHES.md`) and vc (on cas, `OLDCC_SWITCHES_VC.md`) have such a table; the
witnesses of the switches of v0, v0a, v1r ... are only text in `OLDCC.md`, measured by hand, and not checked.

A version inherits switches (v0a: v0 and its own; v1r: v1q's and `OLD_D1R`, `OLD_D67`). All of them are measured in each
version, not only the new ones: a switch can have witnesses in v0p and none in v0a.

## Done when

- One script run per version writes `host/OLDCC_SWITCHES_<VER>.md` (generated, like the two tables of today).
- `oldcc.sh matrix check` compares, for every version, the switches and programs found now with its table.
- `OLDCC.md` points to the tables for the witnesses; `HOWTO_reconstruct.md` ("Evidence rules") says when to rerun.

## Read first

- `../../CLAUDE.md`; `host/OLDCC.md` (versions, switches); `host/HOWTO_reconstruct.md` ("Evidence rules").
- `host/recon/oldcc.sh` (matrix, matrix check), `host/recon/switchmatrix.py` (the v0p matrix: models of the passes),
  `host/tools/coverage.py` (which Makefile rule builds a file with which compiler and libc), the corpus store of
  `host/recon/corpusrun.py` (keys from contents, the pattern for the cache below).

## What the script finds by itself (nothing by hand)

Every list below is computed from the sources at the start of each run; a run on another commit gives other lists.

1. **The versions and their passes.** The driver `src/cmd/cc/cc.c` names, per `OLD_V*`, the pass binaries it runs
   (ccv0a: `/lib/v0a/codgen` and the cparse and popt of v0) and the assembler (the old cas `/lib/old/cas` or `/bin/cas`).
   The build dirs of each pass are in its Makefile (`src/cmd/cc/{cpp,cparse,codgen,popt}/Makefile`, `build/nonsegv1r` ...).
   So a version is a tuple of pass builds; a pass build can belong to several versions (the cparse of v0 serves v0 and v0a).
2. **The switches of a pass build.** Not only `oldcc.h`: the CFLAGS of the rule of the build dir in the pass Makefile define
   switches too (`-DOLD_V1R`, `-DOLD_D3 -DOLD_D14Q -DOLD_D1R -DOLD_D67` for codgen v1r; `-DOLD_D12`, `-DOLD_D8L` ... for
   cparse). The set: the host cpp with `-dM` on the headers of the pass (`codgen/oldcc.h`, the cparse header, popt's) with
   exactly the -D of that rule; every `OLD_*` defined then. (The make variables are expanded as make does: `make -n -p`.)
3. **Where a switch acts.** The sources of each pass are scanned for `#if`/`#ifdef`/`#ifndef`/`defined()` of the switch. The
   shared files of `mip/` count for a pass only in the branch that pass compiles: `common.2.5` has codgen's part under
   `# ifdef PASS3` (defined in codgen's `mfile3`) and cparse's in the `# else`, `manifest` has `ONEPASS`; the scanner keeps
   the stack of these conditions and evaluates them per pass (with the pass's own defines), so a switch inside a `PASS3`
   branch belongs to codgen only. A switch that is defined but that no pass compiled in that version asks for is listed
   as "defined, not used". A switch whose removal does not build (it needs another one: `OLD_D2V` without `OLD_D2R`) is
   listed as "does not build alone", with the error line.
4. **The programs and libraries of a version.** From the Makefiles, as `coverage.py` reads them (the rule of the object,
   `$(CCV1R)` ..., the `.c.o` rule, also `src/lib/oldlibc/*/Makefile` and its libraries `libc-vN.a`, `src/cmd/*/Makefile`
   of the programs with their own directory); the shared function, not a copy. Only what `checkall` says is exact or
   partial.

## How a measurement goes

- **Baseline** per version: every program and module built with all switches; per function the result against the
  original (exact, or the number of differing lines). The functions come from the unstripped baseline build itself (its
  symbols); for an exact program its addresses are those of the original, so no `findings.json` is needed; a variant is
  compared function by function in that order. A library module the same way through its object.
- **Variant** per switch: only the pass build(s) where the switch acts are rebuilt without it (the -D taken out of the
  CFLAGS, or the `#define` of `oldcc.h` taken out by an `#undef` after the include, never by editing the sources in place);
  the other passes are those of the baseline. Then the programs of the version, as below.
- **Result** per switch: lost (exact with all switches, not without it), gained (exact only without it), the change of the
  differing lines; per version and switch the witnesses by name (program function, library module function).

## Cache: what is computed once

Compiling under the emulator is the cost. A pass is a function of its input and its binary, so its output is stored under
a key of contents: hash(pass binary) + hash(input file) + the options -> the output file. Nothing is marked "does not
change" by hand or by a guess; a key either matches bytes or it does not.

- The input of cparse is the output of cpp (the same for every cparse and codgen switch); the input of codgen is the
  output of cparse, and so on. A codgen switch therefore starts from the stored cparse output of the baseline.
- Every stage is looked up with the binary the variant really runs there: the variant's pass build where the switch acts,
  the baseline's elsewhere. A hit only spares that stage. A switch that acts in cparse and in codgen gets a hit in cparse
  and still runs its own codgen (another key). The run of a module may stop early only from the first stage on behind
  which every stage uses the baseline binary.
- When a variant's pass output is byte-equal to one stored before (of the baseline or of any earlier switch: the key of
  the next pass is then the same), every later stage is taken from the store: the function results of that module too.
  Example: switch B acts in cparse only and its cparse output of module m equals that of switch A (A also acts in cparse
  only), so m without B is m without A from there on: no codgen, popt, cas, ld, no compare.
- A changed switch, a changed pass source or a new pass binary gives a new hash: nothing stale can be hit. The store is
  pruned like the corpus store (what a full run did not use goes).
- A program is linked from its modules; when every module object of a variant equals the baseline's, the program result is
  the baseline's without a link.

## Workers

- N worker slots (default: the cores, as `CHECK_CORPORA`; `-j N`). A first stage builds the pass variants (one rebuild per
  switch and pass build, shared by all programs) in parallel; a pass build that fails ends that switch, not the run.
- Then the work is handed out per program: one worker takes all switches of one program (a large program such as ex can be
  split by module: one worker per module and all switches), so its stage outputs stay at hand for the reuse above (in the
  store and in its work directory). The order of the programs: the largest first (from the last run's times), so the run
  does not end with one long program alone.
- Order of the switches inside a worker: grouped by the pass they act in, the later pass first (popt, then codgen, then
  cparse): a popt switch reuses the stored cparse and codgen outputs of the baseline, a codgen switch the cparse output.
  Within a group, the switches whose output most often equals another's (measured in the last run) come first, so their
  outputs are in the store for the rest. To be measured whether this order matters; the store makes the result the same
  in any order.
- Each worker writes one result file per program; the table is written at the end from all of them, so a run that breaks
  can be continued (the finished programs and the store stay).

## Output and check

- Per version the data as JSON (`host/recon/matrix/<VER>.json`), and generated from it `host/OLDCC_SWITCHES_<VER>.md`:
  - table A, one row per switch: switch | pass(es) | lost (count) | gained (count) | lines | measured (commit);
    beside it "defined, not used" and "does not build alone";
  - table B, one row per witness: switch | program or module | function | lost or gained | lines;
  - the programs and modules of the version with their baseline.
  No lists in running text: table B can be filtered and sorted (which switches hang on a function, which functions on a
  switch).
- The tables of today (`OLDCC_SWITCHES.md` of v0p, `OLDCC_SWITCHES_VC.md`) are replaced by the tables of this script and
  their measurement (the programs themselves, not the models of the passes).
- `matrix check`: for every version, the switch set and the program list found now against the table; a switch, program or
  version not in its table, or a row whose switch or pass source changed since its commit, is reported.

## Steps

1. The discovery (versions, pass builds, switch sets with the Makefile -D, pass of each switch with the PASS3/ONEPASS
   branches, programs and modules per version) as its own script with a printed report; checked by hand against
   `OLDCC.md` before anything is measured.
2. The store and the stage runner (cpp, cparse, codgen, popt, cas, ld under the emulator, keys of contents).
3. The workers and the scheduling.
4. v1r end to end; then v0a; then all other versions, the segmented ones the same way (scc, scparse, scodgen, spopt, the
   segmented libraries); `matrix check`; the old tables replaced; the documents.
