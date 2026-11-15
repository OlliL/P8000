# 105 Parallel checkall runs with variants

## Status

open

## Goal

Several checkall runs at the same time, each on its own variant of the sources (e.g. a switch changed in v0, a new switch
added, a switch taken out for a source form), without one run seeing or disturbing another, and sharing the cache.

## Done when

- `make checkall NAME=x PATCH=FILE` (PATCH also a git ref, e.g. from `git stash create`) checks the files git knows plus that
  patch, in a copy; the working tree is not read after the copy and not changed.
- Three such runs started together finish with the results each would give alone (shown on a known good and a known bad
  patch); the output names the variant.
- `host/tools/checkall.sh` head comment and `CLAUDE.md` ("Build the emulator and check") describe it.

## Read first

- `host/tools/checkall.sh` (copytree, CHECK_TMP, the shipped copy, the corpus part).
- `host/z8k-emu/wega.c` (how the guest `/tmp` maps to `ROOT/tmp`; tmplog).
- `host/recon/stagecorpus.py` (cache_write: temporary file and rename).

## Known

- Almost everything of a run lies below `CHECK_TMP` already (tree copy, OLDROOT copy, builds, install tree, corpus work
  directories); without a setting it is always `/tmp/wega_check`, so two runs overwrite each other.
- checkall copies the tree twice: at the start and later for the shipped build; a change in between lands in the second copy
  only.
- Every original program writes its temporary files into the guest `/tmp`, that is `wega31_root/tmp`, shared by all runs;
  tmpclash sees only the processes of its own run.
- The cache is keyed by content: a candidate result by the hash of the candidate binary (a variant with other sources builds
  another binary: another key; the same binary: the entry is rightly valid), an original's result by its inputs and the
  original binary (the same in every variant: shared on purpose). Entries are written to a temporary file and renamed.

## Steps

1. checkall: `NAME` and `PATCH`; `CHECK_TMP=/tmp/wega_check.NAME` (unique when not given; old ones removed, the last few
   kept); one copy of the tree at the start, used for every part (also the shipped build).
2. Emulator: an option for the guest `/tmp` (e.g. `Z8K_TMPDIR`); checkall sets it to `$T/guesttmp`.
3. Cache: writing stays atomic (temporary file, rename); a lock file per key (made with O_EXCL) so that a missing entry is
   computed by one run while the others wait for it (efficiency, not correctness).
4. Optional: `-j`/JOBS per run, so that three runs do not overload the machine.
5. Test with three runs; documentation.

## Decisions

- Owner 2026-11-15: only parallel checkall runs of variants of one tree (compiler switches, source forms); not several
  agents on different programs in one tree (too complex for now).

## Not part of it

The new corpus runner (110); several agents working on different programs at the same time.

## Log

- 2026-11-15: written.
