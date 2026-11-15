# 02 Corpus architecture and the emulator as oracle

## Status

open

## Goal

A corpus design that scales to every program and script of `src/cmd` (increment 03): the original under z8k-emu and our
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

## Steps

1. Inventory: every program and script of `src/cmd` with its kind of input (files, stdin, arguments, terminal, time,
   other processes) and what it needs of the emulator (script, table).
2. Design questions: inputs as data (one case file format for all programs?), how a script (sh) is tested, normalising
   time, pids and terminal output, the cache key, the run time budget in checkall.
3. The emulator gaps the inventory shows; each with a test.
4. Move the existing corpora to the design.

## Decisions

- Open (owner): the run time budget of checkall for all corpora.

## Not part of it

The corpora themselves for the programs not yet covered: increment 03.

## Log
