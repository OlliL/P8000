# 110 Corpus architecture and the emulator as oracle

## Status

active (2026-11-15)

## Goal

A corpus design that scales to every program and script of `src/cmd` (increment 120): the original under z8k-emu and our
build on the same inputs, outputs compared; cheap to add a program, fast enough for checkall, deterministic.

## Done when

- A written design (in this file, then `host/HOWTO_reconstruct.md`, step 9) the owner has agreed to.
- The emulator runs what the programs of `src/cmd` need (at least: guest signal handlers, `alarm`, the system calls that the
  programs use; a list of what is still missing).
- The existing corpora (`compare/CORPORA`) run under the new design; checkall ALL OK.

## Read first

- `host/recon/stagecorpus.py` (head comment: modes, cache), `host/recon/corpusc.py`, one `compare/NAME/progcfg.py`
  with a script (`compare/cchain`) and one without (`compare/ld`).
- `host/z8k-emu/README.md` (system calls, limits), `host/KNOWLEDGE.md` "z8k-emu, the guest pid".
- `host/tools/checkall.sh`, part 3 (how the corpora run side by side).

## Known

- 22 corpora exist (`compare/CORPORA`), all for the tool chain; checkall runs them in parallel, about 9 of 16 minutes.
- Inputs are prepared once into `compare/NAME/corpus.tar` (not in git), results of the original are cached by a key of
  binaries, tar, code, emulator. Inputs come only from files git knows (2026-11-15).
- A differing case prints its differing lines and, with `-k` (checkall sets it), keeps its run directories.
- Emulator limits: guest signal handlers do not run, a floating point trap stops it, `ioctl` is always ENOTTY.
- Fixed 2026-11-15: guest pids (a cut host pid of 0 broke fork); kill maps guest pids back.

## Assessment of the current design (2026-11-15)

- Inputs: prepared once into `compare/NAME/corpus.tar` (with the original tools under the emulator), made new only by hand
  (`mkcorpus.sh`) or when missing. They freeze the sources of the tree at that time: a changed or new source of `src/` is not
  tested until someone runs mkcorpus. A tar is one unit: one changed input changes the hash of the whole tar and invalidates
  every result of the corpus. Several corpora prepare the same inputs again (the C sources of src/: cc, cpp, cparse, codgen,
  popt, cchain, ...; cchain and scchain already share one tar).
- Cache keys: right in principle, three faults. (1) The code part hashes every `*.py`, `*.sh` of `host/recon`: an unrelated
  script there invalidates every corpus (seen 2026-11-15). (2) A candidate result is all or nothing per corpus (one ok file).
  (3) A result of the original is cached unchecked: a disturbed run (guest pid 0, 2026-11-15) became the expectation.
  Also: the files of ROOT a run reads (`CORPUS_ROOT_FILES`) are kept by hand (`corpusdeps.sh` checks them only when called).
- Similarity: two modes (`CORPUS_RUN` with argv and stdin, `CORPUS_SCRIPT` with a shell script), own `fixtures.py` in ld, sld,
  gld; `progcfg.py` mixes the settings of the reconstruction (`OWN_LIM`, `MODULES`, `MODEL_OUT`, `STRICT_INPUT`, ...) with those
  of the corpus; 21 to 144 lines.

## Design (draft, to be refined with the owner)

1. One case format, as data. A case: its input files, the command (argv, stdin, environment), what is compared (output,
   exit status, files afterwards, with mode). The runner knows only this format. A program's corpus definition gives
   case generators and, where needed, a normalisation of an output; the settings of the reconstruction move to a file of
   their own (not read by the runner).
2. Two kinds of input. Hand-written cases: small, in git, per program. Derived cases: made from the sources of the tree
   (e.g. every `.c` of src/) by a generator, kept in one shared content-addressed store; a derived case is made again by
   itself when a source it is made from changes (its key: the hash of those sources and of the generator).
3. Cache per case and candidate. Key of the original's result: hash of the case's inputs, the variant, the original binary,
   the files of ROOT the run reads, the emulator. Key of a candidate's result: that key + the candidate binary + its
   dependencies. After a change only the affected cases run: a changed program only in its corpus, a changed source only
   in the cases made from it. The code part of a key: only the code that decides the run (the runner core, the program's
   corpus definition, the case script), not every script of host/recon.
4. An original's result is computed twice when it goes into the cache; both must be equal, else the case is reported as
   unstable and nothing is cached.
5. The files of ROOT a run reads are measured, not kept by hand: the emulator logs the files it opens (tmplog); the runner
   takes that list into the key.
6. Run time: a cold run stays expensive (the originals under the emulator) but is rare; a checkall after a small change runs
   only the affected cases, also with 100 and more corpora.

## Steps

1. The owner's questions on the design first, then the open questions below; this section rewritten to the agreed design.
2. Cache per case and candidate, narrow code key, the original run twice (on the existing corpora, without changing their
   definitions).
3. Case format, generators, the shared store of derived inputs; move the existing corpora one by one (checkall after each).
4. Measured ROOT dependencies (emulator).
5. Inventory of src/cmd (kind of input, what it needs of the emulator) and the emulator gaps (guest signal handlers, alarm, ...).

## Decisions

- Owner 2026-11-15: write the design down as a draft; the owner's own questions come first, then the open ones below.
- Open (owner): the run time budget of checkall for all corpora.
- Open: where the shared store of derived inputs and the caches live (not in git; today beside each corpus in compare/NAME).
- Open: how a hand-written case is written (one file per case, or a table per program) and how a script (sh) is a case.
- Open: the order of moving the existing 22 corpora; whether the reconstruction settings of progcfg.py move now or later.

## Not part of it

The corpora themselves for the programs not yet covered: increment 120.

## Log
