# 120 Corpora for every program and script of src/cmd

## Status

open (after 110)

## Goal

Every program and script that `src/cmd` builds or installs is tested against the original on a corpus, so that every later
change (a port to the host, a fix the owner allows) is checked against an expectation.

## Done when

- Every installed file of `src/cmd` that is a program or a script has a corpus in `compare/NAME` (or is listed here with the
  reason why not, e.g. needs real hardware).
- checkall runs them; ALL OK within the budget decided in 110.

## Read first

- This increment's design from 110 (`host/increments/110-corpus-architecture.md`, then `host/HOWTO_reconstruct.md` step 9).
- `src/COVERAGE.md` (what `src/cmd` installs, status exact).

## Known

- Ground rule (owner 2026-11-15): each corpus compares the original of `wega31_root` with our build by the shipped cc/scc and
  libc (`make shipped`); host candidates only for the C compiler chain.
- A model of the emulator (terminal, a device such as `/dev/ud`, user, a second file system, guest signal handlers) is built
  when the first program needs it, together with that program's corpus (owner 2026-11-15), with tests of its own; the
  inventory of 110 says which program needs which.
- A corpus is added only when its expectations show that the original really does what its cases are for (110, Design 9).

- Today corpora exist for the tool chain only (cc, cpp, cparse, codgen, popt and their segmented twins, cas, ld, sld, gld,
  ar, ranlib, strip, size, cchain, scchain).

## Steps

1. The list of programs and scripts (from COVERAGE: source in `src/cmd`, status exact), grouped by the kind of input.
2. Per group a corpus pattern; then per program its inputs (System III tests, man pages, own cases for every option).
3. Add them in batches, each batch with checkall; a batch that needs a new model of the emulator builds it first, with
   tests of its own, and records it in `host/z8k-emu/README.md`.

## Decisions

## Not part of it

Programs not yet rebuilt (increment 160 adds their corpus when it rebuilds them).

## Log
