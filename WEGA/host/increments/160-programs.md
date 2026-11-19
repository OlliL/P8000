# 160 The programs of bin, usr/bin, etc, usr/lib

## Status

active (2026-11-19; gopt is the current work)

## Goal

Every program of the WEGA tree from source, byte-identical, each with a corpus.

## Done when

- COVERAGE shows no program as `sys3` or `todo` (except PLZ, increment 170), each with a corpus (120 pattern).
- checkall ALL OK.

## Read first

- `host/HOWTO_reconstruct.md`, the agents of 140.
- `src/COVERAGE.md`, `host/OLDCC_DATES.md`.

## Known

- 2026-11-15: bin 25 of 111 exact, usr/bin 10 of 145, etc 33 of 67, usr/lib 1 of 197 (counts include text files, 130).
- The tool chain is done except `gopt`, `chkout`, `yacc` and the PLZ tools (`host/HANDOFF_wega_disasm.md`, inventory);
  `/bin/cas` is byte-identical, left there: the natural `for` loop of jpr (`compare/cas/HANDOFF.md`).

## Steps

1. Order by date and compiler (programs of one compiler version together); the tool chain gaps first (`gld`, `yacc`,
   `chkout`, `gopt`), since later programs need them.
2. Per program with the agents of 140: source, library, own code, corpus; checkall per batch.

## Decisions

## Not part of it

PLZ programs and `as`: 170.

## Log

- 2026-11-19: /lib/gopt, ebbo2.c complete (20 functions, 5:0000..5:2138, segment 5): masked: ebbinit, ebbopt, vnfind, updateopnd, ebbfix, exprenter, identop, nostore, clear, killed, ebbcall, checkuse, inuse, addstore, resetbb, ebbclean; open: vnumber (DIFF 133, popt block order: codemove), exprfind (2), canopt (2), constfold (28: tail merging, long `&` temp register). Findings: `constfold` returns struct qa by value; `!x->a.b2 == 7` in the original source (KNOWLEDGE.md quirks table); the debug output strings give the names. Values at the data addresses: s71e4 (kill table count), s71e6 (stores), s71e8 (value counter), s71ea (use table), a0cc (value table), htab at 8:7164.

- 2026-11-19: /lib/gopt, lpinv2.c complete (35 functions, 5:2138..5:5534): masked 26; open: clearblocks, marklpblocks, liveonexit (displacement form, cause 4), qmkstore (parameter reload), srchdflist (rr4/rr6 temp), clropt, updatelooplist, isglobal (block order / tail merging), orderbl (2). Findings: fnc masks the word immediates of cp (thresholds and op numbers are not checked: read them from the listing, not from the diff); `(!f() != 0)` gives the materialized boolean; `a = b = x` and `if ((x = x->next) == 0)` give the copy patterns; function-local statics live in the data of the module (lblcnt = 15000 at 7:5CB6); the quirks table has alloclpdef (no return) and samebetalpdef (`printf(q)`).
