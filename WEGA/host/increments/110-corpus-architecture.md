# 110 Corpus architecture and the emulator as oracle

## Status

done (2026-11-16); the decisions marked "decided by Claude" wait for the owner's review

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

## Detailed design (decided by Claude 2026-11-15, for review)

- Files: `compare/NAME/corpus.py` (the new definition; a corpus with it runs on the new runner `host/recon/corpusrun.py`,
  one without on the old one), `compare/NAME/cases/CASE/{case,in/}` (hand-written cases). `corpus.py`: `NAME`, `ORIG`
  (path in the WEGA tree), `PROG` (the name of the program in the steps), `CANDIDATES` ({'wega': path of the shipped build,
  'host': ... only for the compiler chain}), `ROOT_FILES` (until they are measured), optional `generate(tree)` (derived cases)
  and `normalize(case, what, data)`.
- The case file: one field per line, `#` starts a comment line. `step: WORDS` runs a WEGA program under the emulator: the
  first word `PROG` is the program under test (the original or a candidate), another first word a program of the WEGA tree
  (`/bin/NAME`, the original for both sides); words are split as by the shell (`shlex`), no shell operators. `stdin: FILE`
  and `env: NAME=VALUE` belong to the step before them; a line `env: ...` before the first step holds for all steps.
  `host: SHELL-LINE` runs a line of the host shell between steps (preparation that has no WEGA program, e.g. `touch -d` for
  fixed times; `$WEGA_ROOT` is set). `variant: NAME=VALUE ...` (several lines): the case runs once per variant with that
  environment. `expect: exit N` (the last step), `expect: output contains TEXT`, `expect: file PATH contains TEXT`,
  `expect: file PATH same-as in/FILE`, `expect: missing PATH`; checked on the original's run only.
- argv[0]: the program under test runs with the same argv[0] on both sides (its name), so that messages that name it are
  equal; the emulator gets an option for argv[0].
- Compared per case: for every step its output (stdout and stderr) and exit status; afterwards the directory tree: per
  entry type, mode, size, checksum (files), link count; not owner and times (the emulator runs as the host user on both
  sides; times are those of the host).
- Store `.corpus/`: `inputs/HASH/` (derived cases, the same form as a hand-written case), `results/orig/KEY` (the original's
  result, computed twice, cached only when both are equal), `results/ok/KEY` (a candidate was identical), `locks/`.
  Keys: case = sha1 of the case file and the files of `in/` (names and contents); original = case + variant + sha1 of the
  original binary + of the ROOT_FILES + of the emulator + of the runner core (`corpusrun.py` and the case module); candidate
  = original key + sha1 of the candidate binary + of its dependencies. Written to a temporary file and renamed; a key is
  computed by one run at a time (flock).
- checkall: every corpus runs on corpusrun.py (stagecorpus.py, the tars and mkcorpus.sh are gone); the result line keeps its
  form (`NAME [cand]: N of M inputs identical`).

Decided by Claude 2026-11-15 while moving (points 3 to 6, steps 4, 5), for review:

- stdout and stderr of a step are kept apart (stdout, then `-- stderr` and stderr): in one pipe their order depends on the
  buffering of the C library, which the host build does not share.
- `after_step(argv, rc, out, err, dir)` of a corpus cuts what the original leaves undefined (cas: after an error the warnings
  and the files it leaves; `normalize()` for the intermediate file of `-i`, size only), as the old case.sh did.
- `$V` in a step: the words of the variant (popt, spopt: `-l`, `-d`); a word with `* ? [` is expanded in the case directory.
- `ROOT_ENV` of a corpus: the original runs in the tree of that variable (casold: `OLDROOT`, its original is `/lib/old/cas`
  of our build); the preparations stay in `WEGA_ROOT`.
- The key of a prepared input holds the name of its source file (the files of one directory had shared one input), the
  function of the preparation with its arguments (no addresses), the source files, the emulator.
- The sources of the derived cases: the files git knows (tracked and untracked, not ignored), in a checkall copy the list
  `.files` its snapshot writes; before, the builds of the copy added cpy.c and three standalone files (cpp: 299 in checkall
  against 295 in the working tree).
- checkall copies the emulator once per run (a rebuild in the working tree during a run had reached its later parts).
- Measured dependencies (step 4): the emulator writes every guest path it looks up below its ROOT (not /tmp, /dev) to
  `Z8K_ROOTLOG`; the runner sets it to a file of the starting directory, collects it after a run or a preparation and keeps
  the state of these files (contents of a file, names of a directory, missing) with the result, the "identical" mark of a
  candidate and the prepared input; on use they are checked, a change makes it again; the candidate key holds the original's
  dependencies (a host candidate reads the same headers natively). Measured at once: the preparation of codgen runs
  `/bin/mv`, which the hand list did not have. `ROOT_FILES` and `prep_files` stay for what is read outside the emulator
  (host lines such as the copy of /lib/libc.a in ar, native host candidates); the existing lists stay until checked.
- The fields of the surroundings are reserved in the case format (`user:`, `terminal:`, `device:`, `time:`, `fs:`,
  `signal:`): each is an error naming what it will mean, until the emulator serves it (120). argv[0] stays the path until a
  corpus needs the emulator option.
- `make checkall-nocache` (`CORPUS_NOCACHE=1`); `corpus.sh` takes `CORPUS_TMP`, `CORPUS_OPTS` (were `STAGE_TMP`, `STAGE_OPTS`).
- Expectations: one or two chosen cases per corpus (e.g. abs.c: exit 0 and the expected name in the output file), strip:
  case 3 reports `unable to open`; not every case (some originals fail on purpose, e.g. strip of a stripped file).

## Steps

Done: 1 to 5 (2026-11-15, see Log).

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
- Owner 2026-11-15, the store: `WEGA/.corpus/` (in .gitignore): `inputs/` (derived inputs) and `results/` (results of the
  originals and the candidates), each by the hash of its key; its place can be set by an environment variable (as
  `CORPUS_STORE` today) so that checkall and the runs in the working tree share one store. `compare/NAME/` keeps only what
  belongs to git (corpus definition, hand-written cases, findings). Entries no key has used for long are removed by date of
  last use.
- Owner 2026-11-15, a hand-written case: a directory `compare/NAME/cases/CASE/` with a text file `case` and the files the
  case starts with in `in/` (none needed for pure command lines). `case`: `step:` (a run, words as in the shell; several run
  in order), `stdin:`, `env:`, later `user:`, `terminal:`, `device:` when their model exists, `variants:` (a list of options,
  each run), `expect:` (expectations on the original, Design 9); an unknown field is an error. Derived cases are made by a
  generator in the same form in `.corpus/inputs`; the runner knows only this one form.
- Owner 2026-11-15, how cases are made: (1) derived cases by a generator in the corpus definition (e.g. cc: every `.c`
  known to git with its headers, step `cc -c -O X.c`); key = hash of its source files + the generator code (+ the binaries
  of preparing steps that run original tools, e.g. cpp and cparse for codgen); made only when the key is missing from the
  store: a changed source makes exactly its case again (this replaces mkcorpus and the tars). (2) Hand-written cases from
  the man page (every option, every documented error) and the System III tests where there are some; `newcase.py NAME CASE`
  makes the frame; `variants:` for option combinations; a task for a subagent (140): the expectations come from the man
  page, never from a run of the original (that would be circular). (3) Coverage: the emulator logs the code addresses of the
  original a corpus runs; with the function bounds of `findings.json` it shows per function run or never run.
- Owner 2026-11-15, the move: (1) the new runner beside the old one (store, cache per case, the original run twice); checkall
  calls the one each corpus uses. (2) A pilot in script form (size or ar), hand-written cases only. (3) The group with the C
  sources as input: cc, scc, cpp, cparse, scparse, codgen, scodgen, popt, spopt, cchain, scchain (the first generator, with
  preparing steps; the shared store). (4) ld, sld, gld (their fixtures.py become generators). (5) cas, casold. (6) The old
  runner, the tars and mkcorpus.sh removed. checkall after each of these six points; inside a point after each corpus only
  that corpus (`corpus.sh NAME`). A moved corpus passes the same cases as before (the number of cases is compared).
  `progcfg.py` is split while a corpus moves: `compare/NAME/corpus.py` (generators, normalisation, candidates); `progcfg.py`
  keeps only the settings of the reconstruction tools and goes where no tool reads it.

- Owner 2026-11-15, for working on 105 and 110 alone: (1) details not discussed (case file syntax, generator interface, store
  layout, what the tree comparison takes as owner and time, how checkall chooses the old or new runner while corpora move)
  are decided by Claude and recorded here as "decided by Claude (date), for review"; the owner reviews them later.
  (2) The original is always right, also in its bugs: a difference found by a corpus is fixed on our side until our build
  behaves as the original. A source has to give two builds: the byte-identical one with the compiler of its time (unchanged,
  checkall) and the shipped build with cc/scc and /lib/libc.a, /lib/slibc.a (behaves as the original); a change only the
  shipped build needs goes behind `#ifdef` (`-DSHIPPED` of `make shipped` in src/cmd), so the byte-identical build does not
  see it. Host variants (compiler chain only) need functional equality only. A difference whose cause is not found is
  written down here and the other corpora go on. (3) "The same cases as before": derived inputs come from the sources of
  today, so their number changes; the old corpus is made new on the same tree state and the numbers are compared and
  recorded. (4) The existing corpora get simple expectations when they move (e.g. a compiler: exit status 0 and an object
  for chosen cases), not complete ones. (5) Coverage measurement waits; after 110 the work stops (no 120 without the owner).

## Not part of it

The corpora themselves for the programs not yet covered: increment 120.

## Log

- 2026-11-15: move points 1 and 2 done. `host/recon/corpusrun.py` (the new runner: case format, store `.corpus/`, keys, the
  original twice, flock per key, tree comparison, expectations) beside stagecorpus.py; `corpus.sh` takes a corpus with
  `corpus.py` to it, `mkcorpus.sh` and `corpusdeps.sh` leave it out, checkall passes `CORPUS_DB`. Pilot ar: `compare/ar/cases/`
  (scenario from scenarios.sh, libc.a, slibc.a), five variants of TZ each (the old corpus: 3 inputs with 5 variants inside, now
  15 cases); the expectations found a fault of my conversion at once (a loop line without `;`: dates.a was not made on either
  side, so the two sides were equal although nothing was tested). Decided by Claude: every step and host line with umask 022;
  argv[0] stays the path until a corpus needs the emulator option (ar does not). checkall ALL OK, ar 15 of 15 on both
  candidates; the old progcfg.py, scenarios.sh, lib.sh of ar removed.
- 2026-11-15: points 3, 4, 5 done (commit 31350742): the C-source group, ld, sld, gld, size, strip, ranlib (compare/binutils/tree.py
  as a shared prepared tree), cas, casold on corpusrun.py; checkall ALL OK after point 3 (covered 4) and after 5. Case counts
  against the old corpora: ld 146, sld 91, gld 106, ar 15, size 12, strip 62, ranlib 37, cc/scc 1 as before; cas 766 -> 664 (+6
  skipped) and the C group: the old tars were made before libc and slibc were merged (397 C sources then, 295 now: the 102
  duplicates); cpp 295, cparse/scparse 297, codgen/scodgen 289, popt/spopt 921, cchain/scchain 602, casold 666.
- 2026-11-16: point 6 and steps 4, 5 done: stagecorpus.py, mkcorpus.sh, corpusdeps.sh/.py, the tars and caches removed;
  corpus.sh only corpusrun.py; make checkall-nocache; measured ROOT dependencies (Z8K_ROOTLOG); reserved fields of the
  surroundings; the inventory of src/cmd in 120; HANDOFF, HOWTO step 9, CLAUDE.md, host/recon/README.md,
  host/z8k-emu/README.md. checkall ALL OK, cold (new emulator and runner): corpora 15 minutes in parallel (cas 15, casold 13,
  cchain/scchain 9 minutes), the whole run 19 minutes; warm: cpp 0.1 s, cas 0.3 s.
