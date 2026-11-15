# 110 Corpus architecture and the emulator as oracle

## Status

active (2026-11-15)

## Goal

A corpus design that scales to every program and script of `src/cmd` (increment 120): the original under z8k-emu and our
build on the same inputs, outputs compared; cheap to add a program, fast enough for checkall, deterministic.

## Done when

- A written design (in this file, then `host/HOWTO_reconstruct.md`, step 9) the owner has agreed to.
- The case format has the fields for the surroundings (terminal, devices, user, time, file systems, signals), the tree
  comparison and expectations; the runner passes them on (a field the emulator cannot serve yet is an error, not ignored).
- The inventory: per program what it needs of its surroundings (for 120).
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
- Emulator limits (models built on demand, see Steps 5): guest signal handlers do not run, a floating point trap stops it, `ioctl` is always ENOTTY.
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

1. One case format, as data, one mode (owner 2026-11-15). A case: its input files and a sequence of steps on that directory
   (each step: argv, stdin, environment); compared: output, exit status, every file afterwards (no list of outputs: a stray
   file shows too). A single run is a case of one step. The runner makes the script of the steps itself; a free shell script
   (run on the host, the programs in it under the emulator) only as an exception for what steps cannot express. A WEGA shell
   script under test is an ordinary step (WEGA's sh under the emulator). The run mode of today (`CORPUS_RUN`,
   `CORPUS_OUTPUTS`, 12 corpora) goes when the corpora move (Steps 3). The runner knows only this format. A program's corpus definition gives
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
6. Candidates (owner 2026-11-15, a ground rule): every corpus compares the original of the shipped root (`wega31_root`)
   with the program we build with the shipped cc/scc and the shipped libc (`make shipped`, candidate `wega`); both run under
   the emulator on the same case. A host candidate exists only for the C compiler chain (cc, cpp, cparse, codgen, popt, cas,
   ld, ...). So everything a program needs of its surroundings comes from the emulator alone, the same for both sides.
7. The surroundings of a case, beside argv, stdin and files: terminal (type, size, the keys as a byte stream), devices
   (name, model, image file), user and group, the time, more file systems, signals at defined points. All of it goes into
   the cache key. The format has room for them from the start; the runner passes them to the emulator.
8. Compared beside output, exit status and files: the whole directory tree with its metadata (also empty directories,
   link count, mode, owner) and the terminal output.
9. Expectations against empty tests: when the emulator lacks something, original and candidate fail the same way and the
   comparison is green although nothing was tested (`ioctl` gives ENOTTY: vi stops on both sides alike). So a corpus also
   checks expectations for chosen cases: that the original really does what the case is for (e.g. the file after `:s/a/b/`
   holds the new text). A corpus is added only when its expectations hold.
10. Run time: a cold run stays expensive (the originals under the emulator) but is rare; a checkall after a small change runs
   only the affected cases, also with 100 and more corpora.

## Steps

1. The owner's questions on the design first, then the open questions below; this section rewritten to the agreed design.
2. Cache per case and candidate, narrow code key, the original run twice (on the existing corpora, without changing their
   definitions).
3. Case format, generators, the shared store of derived inputs; move the existing corpora one by one (checkall after each).
4. Measured ROOT dependencies (emulator).
5. Inventory of src/cmd and of the programs still to come: per program the kind of input and what it needs of its
   surroundings (terminal, devices, user, time, file systems, signals); the list goes to 120. The models themselves are not
   built here (owner 2026-11-15: a model is built when the first program needs it, with that program, in 120 or 160).

## Decisions

- Owner 2026-11-15: write the design down as a draft; the owner's own questions come first, then the open ones below.
- Owner 2026-11-15: one mode of running a case (steps as data, Design 1); no separate run and script modes.
- Owner 2026-11-15, run time: no fixed limit per corpus. `checkall` uses the cache and runs everything whose key changed (after
  a change of the emulator or of the runner core that is everything: a cold run, accepted up to about 2 hours); a checkall
  after a change of one program stays as short as today (goal under 20 minutes). Cases are chosen for coverage, not by
  count; a corpus that takes more than some minutes cold is looked at for cases that test the same twice.
  `checkall-nocache` ignores every cache (today `CORPUS_NOCACHE=1`): now and then, to check the cache itself.
- Open: where the shared store of derived inputs and the caches live (not in git; today beside each corpus in compare/NAME).
- Open: how a hand-written case is written (one file per case, or a table per program) and how a script (sh) is a case.
- Open: the order of moving the existing 22 corpora; whether the reconstruction settings of progcfg.py move now or later.

## Not part of it

The corpora themselves for the programs not yet covered: increment 120.

## Log
