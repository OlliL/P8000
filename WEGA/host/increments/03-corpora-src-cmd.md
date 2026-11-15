# 03 Corpora for every program and script of src/cmd

## Status

open (after 02)

## Goal

Every program and script that `src/cmd` builds or installs is tested against the original on a corpus, so that every later
change (a port to the host, a fix the owner allows) is checked against an expectation.

## Done when

- Every installed file of `src/cmd` that is a program or a script has a corpus in `compare/NAME` (or is listed here with the
  reason why not, e.g. needs real hardware).
- checkall runs them; ALL OK within the budget decided in 02.

## Read first

- This increment's design from 02 (`host/increments/02-corpus-architecture.md`, then `host/HOWTO_reconstruct.md` step 9).
- `src/COVERAGE.md` (what `src/cmd` installs, status exact).

## Known

- Today corpora exist for the tool chain only (cc, cpp, cparse, codgen, popt and their segmented twins, cas, ld, sld, gld,
  ar, ranlib, strip, size, cchain, scchain).

## Steps

1. The list of programs and scripts (from COVERAGE: source in `src/cmd`, status exact), grouped by the kind of input.
2. Per group a corpus pattern; then per program its inputs (System III tests, man pages, own cases for every option).
3. Add them in batches, each batch with checkall.

## Decisions

## Not part of it

Programs not yet rebuilt (increment 07 adds their corpus when it rebuilds them).

## Log
