# Handoff: WEGA build chain back to C

Status 2026-11-01. Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file. The section "Current work" below says what is being done now and what to read for it; the rest of this file is
the inventory and the decisions of the whole project.

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA one: `cc`, `scc`,
`cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `gopt`, `popt`, `cas`, `as`, `ld`, `sld`, `gld`, `ar`, `ranlib`,
`strip`, `size`, `libc` and `crt0`, built from our own C sources and checked byte for byte against the originals.
The original binaries run under the emulator (`host/z8k-emu`, the oracle); they are not part of the tool chain.

Byte identity is the only guarantee of identical behaviour, so it stays the goal (owner decision). Where the
original was built by an older compiler revision (`OLDCC.md`), byte identity needs that compiler, which does not
exist any more; this is the open task of step 3 below.

## Inventory: what belongs to the tool chain

Source of truth for the state: `../src/TODO.md` (per program) and the check commands in the last column.
"Source from binary" = our source was written from the original binary, not from a source tree.

| Tool | Original (WEGA 3.1 tree) | Original filesize | WEGA source (`src/`) | Host version (`host/`) | Source from binary | State | Check |
|---|---|---|---|---|---|---|---|
| `cc`, `scc` (driver) | `/bin/cc`, `/bin/scc` | 14208, 14246 | `src/cmd/cc/cc.c` | `host/cc` | no (WEGA source) | done: 74 command lines identical; `cc` and `scc` byte-identical | `make -C host check` (cc) |
| `cpp` | `/lib/cpp` | 16522 | - | - | - | not started | - |
| `cparse`, `scparse` | `/lib/cparse`, `/lib/scparse` | 67520, 67826 | `src/cmd/cparse` (K&R, from the Plexus PCC base, owner decision) | - | yes | all functions written; corpus 259/259 (behaviour, built with the shipped cc and with ccv0p); byte identity with the old compiler v0p: 166 of 179 functions strictly exact (2026-11-01); scparse not started | `src/cmd/cparse/tools/oldcc.sh model`, `corpus` |
| `codgen`, `scodgen` | `/lib/codgen`, `/lib/scodgen` | 80884, 83554 | `src/cmd/codgen` (K&R, WEGA `cc`; byte identity is the goal, D1 to D16 open) | `host/codgen` (ANSI, gcc `-Wall`; `-DSEG` for scodgen; functional equality) | yes | host: corpus 100/100 identical for codgen and scodgen; large corpus (213 source files of `src/cmd` and `src/lib`) 213/213 for both, 3 files not comparable. WEGA side: corpus 101/101 (behaviour), byte identity open | `host/codgen` (`host_corp.sh`-style test, see `src/TODO.md`); `src/cmd/codgen/tools/check.sh`, `corpus.sh` |
| `gopt` | `/lib/gopt` (only with `-Ol`/`-Or`) | 147286 | - | - | - | not started | - |
| `popt`, `spopt` | `/lib/popt`, `/lib/spopt` | 25620, 25880 |- (source in `host/popt`) | `host/popt` | yes (`z20.c` to `z23.c`) | done: 522 corpus runs byte-identical | `make -C host check` (popt) |
| `cas` | `/bin/cas` | 89152 | `src/cmd/cas` (`MODULES`) | `host/cas` | yes (decompiled, see `src/cmd/cas/README`) | done in behaviour; code not byte-identical (older scc) | `make -C host check` (cas) |
| `as` | `/bin/as` (PLZ/ASM) | 74536 | - | - | - | not started | - |
| `ld` | `/bin/ld` | 21732 | - (source in `host/ld`) | `host/ld/ld` | yes (`ld.c` from the binary) | done: 146 command lines identical | `make -C host check` (ld) |
| `sld` | `/bin/sld` | 46074 | - (source in `host/ld`) | `host/ld/sld` | yes | done: 91 command lines identical | `make -C host check` (ld) |
| `gld` | `/bin/gld` | 27616 | - (source in `host/ld`) | `host/ld/gld` | yes | done on the check lines | `make -C host check` (ld) |
| `ar` | `/bin/ar` | 12396 | `src/cmd/ar.c` | `host/ar/ar` | to be checked | done on the host (archives identical) | `make -C host check` (ar) |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | 7732, 9072, 7732, | `src/cmd/ranlib`, `strip`, `size` | `host/binutils` | to be checked | done on the host; the `src/cmd` versions still differ (`src/TODO.md`) | `make -C host check` (binutils) |
| `libc`, `slibc` | `/lib/libc.a`, `/lib/slibc.a` | 90484, 110938 | `src/lib/libc` (170 modules in C), `src/lib/libc/oldcc` (10 modules as `.s`), `src/lib/oldlibc` (versions) | `host/libc` | partly | 170 of 180 from C; 7 differ (4 older cc, 3 other revision); 3 without C | `make -C host check` (libc), `make -C host/oldcc verify` |
| `crt0` (start-up files) | `/lib/crt0.o` and others | 226 |  `src/lib/libc/crt` (`crt0.list`, `mkcrt0.sh`) | - | to be checked | generated; start code checked against the original | `src/lib/libc/crt` |
| `yacc` | `/usr/bin/yacc` (used by `verify-wega`) | 37224 |  - | - | - | not inventoried | open |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

Overall checks: `make -C host check` (all tools except `codgen`), `make -C host verify-wega` (builds `src/cmd` with the
original tools and compares each program).

## Decided: equality criteria per code base

- `src/cmd` (K&R C, WEGA `cc`): byte identity with the original is the top priority. Original defects are rebuilt first and recorded in the quirks table in `KNOWLEDGE.md`; a fix needs the owner's decision.
- `host/` (ANSI C, gcc): functional equality only. Fixing defects of the original is allowed where the original's result is undefined or wrong; the fix is documented in the quirks table and in `src/TODO.md`.
- Tests for `host/`: textual comparison with the original where the original is well defined; for cases with undefined behaviour in the original, a functional test (a test program run under the emulator).

## Decided: two independent code bases

- `host/` holds ANSI C that compiles with a current gcc and `-Wall`. It is checked by behaviour (check lines, corpora).
- `src/cmd/` holds K&R C that compiles with the WEGA `cc`. It is checked by behaviour and by `cmp` of the built binaries.
- The two are functionally equal but have no relation to each other: no shared sources, no `#ifdef`, no build switch.
  A tool that exists in both worlds is written twice; a copy from `host/` to `src/cmd/` is a new K&R source, not a transfer.

## Decisions open for the owner

- **Compensating source changes.** Some changes adapt the source to the old compiler instead of correcting it:
  `conval` (case order), `sucomp` (storage classes), `zzzcode` (if/else swap, commit `cc1df27`), `tshape` (case order),
  `clocal` (casts). They are behaviour-neutral but not justified for the source. Keep or revert? This is decided in step 2 below.
- **`host/oldcc`** stays: it checks the ten `.s` modules that the old cc produced. Its paths are corrected.
- **`yacc`:** should it be added to the inventory and to the tool chain?

## Current work (2026-11-01): /lib/cparse byte-identical through the old compiler v0p

**What.** `/lib/cparse` was built by an old revision of the C compiler of which no binary exists. We rebuild that
compiler as switches `OLD_Dn` in our sources of the passes (cparse, codgen, popt) and compile our cparse sources with
it; the goal is a byte-identical `/lib/cparse` (owner: "oberste Direktive"). The version that built the passes is
**v0p** (v0, the compiler of the 1982 programs, plus switches only the passes show). Driver `ccv0p`, passes in
`src/cmd/{cparse,codgen,popt}/v0p`, installed into the oldroot tree (`make -C host oldroot`).

**State.** 166 of 179 functions strictly exact, 126 differing lines; data and bss as in the original under the D12
model; code 48640 against 48896 bytes. cparse built with ccv0p passes its corpus (259/259). The bench of v0 (System
III programs of 1982 with ccv0): 118 of 153 functions, 277 lines. The per-function list: `oldcc.sh model; oldcc.sh
lines`. Open, in three groups:

1. **codgen remnants, about 30 lines:** defalign 9 (the original does not store the parameter n in the frame),
   refid 6 and argalloc 6 (the right operand of a long compare in rr6, ours rr4), buildtree 6 (the data order of a
   switch table and the constant 0.0; the temporary of `buildtree( COLON, ll, lr )` at 0x12), exname 3 (`&ntab[s]`
   adds s from the frame), zecode 2 (a long parameter from rr6 inside an argument), oconvert 2 (argument order),
   newsect 2 (`&sectname[sec][k]` adds k before the array). Plan: collect every occurrence of a pattern in the
   original listings (cparse and the bench programs) before writing a rule; the failed attempts so far were rules made
   from one case (see the log).
2. **popt block placement, about 86 lines:** optim 51 (the block of `else if( LCON(p) && RCON(p) && conval(...) )`
   stands behind the MINUS case), argregs 27 (the switch dispatch with `ld r2,r13` stays at the end), defid 8 (the
   loop step `dec 0x16(fp),#2` stands in front of redecl). Lesson of cinit, clocal, falloc: such placements were source
   orders (case order, statement order) or a detail of the codgen output (`ld r2,r2`), not popt rules. Plan: first a
   script that tries the case and block orders of the source, then the popt trace (below) against the original order;
   a new popt rule only after that, checked on all functions and the bench.
3. **Probably outside the compiler, about 4 lines:** yylex `jp` where the same getchar elsewhere has `jr` (the old
   cas?), argdebug (popt renames a single saved register variable into r1), the data order in buildtree. To be
   discussed with the owner (an older cas, or record as an explained remainder).

**Tools.**

- `src/cmd/cparse/tools/oldcc.sh`: `build PASS VER [SWITCHES]` (rebuild and install one pass), `model` (the strict
  comparison of /lib/cparse built with ccv0p), `lines`, `corpus`, `bench`. Acceptance: model better, corpus 259/259,
  bench not worse for a v0 switch; neutral on the bench means v0p. Then the normal-object check: the changed source
  compiled with the shipped cc must give the same object as before (the normal build stays the shipped one).
- `src/cmd/cparse/tools/olist.py NAME`: the original instructions of a function. `python3 tools/strictcmp.py -v NAME`:
  the strict diff of one function (run in src/cmd/cparse after `oldcc.sh model`).
- popt trace: build `src/cmd/popt/v0p` with `-DTRACE` in addition to its switches (`oldcc.sh build popt v0p "<the
  switches of the Makefile> -DTRACE"`, then rebuild without), run it under the emulator as `popt + in.s out.s` on the
  codgen output of one file (`ccv0p -S` without -O, cut to one function): out.s holds the node list after every
  iterate and comjump round.
- Switches of codgen are set in `src/cmd/codgen/oldcc.h` (the native cc takes about 160 characters of -D options);
  those of cparse and popt in the version Makefiles.

**Read, in this order:** `host/OLDCC.md` sections 1 (rules, evidence rules, time box) and 2 "v0" and "v0p" (state,
switches, history), section 3 for the class you work on (D2, D3, D6, D16, D20, D22 are the busy ones);
`host/OLDCC_LOG.md` section "2026-11-01" (every attempt of the last rounds, also the refuted ones: do not repeat
them); `src/cmd/cparse/tools/HANDOFF_cparse.md` (owner decisions for cparse: commits allowed, no push, Plexus base);
`src/cmd/cparse/tools/findings.json` key `manual_classes` (the note per function: source fixes, switches, remainder);
`host/KNOWLEDGE.md` sections "Bugs and quirks of the originals" and "codgen / scodgen" (register machinery) as
reference. Working rules of the owner for this task (besides CLAUDE.md): notes on findings only in findings.json (no
finding status or attempt history in source comments); knowledge about the D classes only in OLDCC.md (rewritten) and
OLDCC_LOG.md (dated, appended), other documents do not repeat it; commits allowed in the directories of the passes and
the docs after each accepted step (English, never push); originals and build products never in git; conversation with
the owner in German.

**After cparse:** the same for the other passes that v0p built (codgen, popt: their originals are the shipped
`/lib/codgen`, `/lib/popt`), then the remaining items of the plan in `../src/TODO.md` ("Plan: towards the old C
compiler").

## Next steps, in this order

1. **Complete the tool chain.** `codgen`/`scodgen` (stop line reached) and `popt` (K&R, analysed) are done as far as the
   old compiler allows; `sld` and `ld` are reconstructed in `src/cmd/sld` and `src/cmd/ld` (K&R; ld 51 of 59 functions
   strictly exact, corpus 146/146). The procedure for every further binary is `HOWTO_reconstruct.md`, the shared scripts
   are in `recon/` (`recon/README.md`). `cparse` is written (see "Current work"); gopt, cpp, gld and yacc are deferred (owner decision 2026-10-30).
   The order and the criteria are in `../src/TODO.md`, section "Plan: towards the old C compiler"; step 3 below is item 4
   of that plan.
2. **Bring the source to its natural form.** Revert the compensating changes or justify them. New baseline:
   bytes, functions, D classes.
3. **Reconstruct the old compiler.** Everything about the old compilers is in `OLDCC.md` (the edited summary: the
   classes D1 ... D22, the versions v1, v0, v0p, rules, building and running) and `OLDCC_LOG.md` (the running notes).
   State 2026-11-01: v1 (strip and size byte-identical, ld 55 of 59); v0 (bench 118 of 153); v0p (/lib/cparse 166 of
   179, the current work above).

The stop line for the search for source forms is in `../src/TODO.md` ("codgen / scodgen: status").
