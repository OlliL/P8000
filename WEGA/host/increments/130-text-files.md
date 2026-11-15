# 130 Text and data files (etc, usr/lib, usr/other)

## Status

open (can run beside 110, 120)

## Goal

Every file of the WEGA tree that is text or data (no program to compile) comes from `src/` and is installed by
`make install`, byte-identical.

## Done when

- COVERAGE shows no text or data file as `sys3` or `todo` (or each left one is listed here with the reason).
- checkall ALL OK.

## Read first

- `src/COVERAGE.md` (filter: kind text or data, status sys3 or todo).
- `host/tools/coverage.py` (how a source is found: install rules of the Makefiles).

## Known

- 2026-11-15: 293 files `sys3` (a System III source exists), 67 of them text; 191 `todo`.
- The System III tree (`tmp/sys3`) may be copied from (CLAUDE.md).

## Steps

1. The list (script over COVERAGE).
2. Per directory: take the System III file or the WEGA file, compare, write the install rule; a WEGA-only file is taken as it
   is (a file with no other source is its own source).
3. checkall, COVERAGE new.

## Decisions

- Open (owner): whether WEGA-only text files are copied into `src/` as they are.

## Not part of it

Programs and scripts that need a compiler or are tested by a corpus: 120, 160.

## Log
