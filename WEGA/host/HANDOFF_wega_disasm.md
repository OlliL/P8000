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

**State (2026-11-01, end of the session).** 173 of 179 functions strictly exact, 96 differing lines; data and bss as
in the original under the D12 model; code 48640 against 48896 bytes. cparse built with ccv0p passes its corpus
(259/259). The bench of v0 (System III programs of 1982 with ccv0): 118 of 153 functions, 275 lines. The
per-function list: `oldcc.sh model; oldcc.sh lines`.

**Open in /lib/cparse, and why** (details and every refuted attempt in `OLDCC_LOG.md`, 2026-11-01; do not repeat
them without new evidence):

1. **popt block placement, 86 lines: optim 51, argregs 27, defid 8.**
   - optim: the block of `else if( LCON(p) && RCON(p) && conval( p->left, o, p->right ) ) goto zapright;` stands in
     the original behind the end of the PLUS case (`break` = `jp 0x569C`, the return) and falls into zapright; ours
     moves the zapright block up to the jump. argregs: the same picture: `L674: ld r2,r13; jpr L676` goes in front of
     the switch dispatch at the end (the original keeps the dispatch there), ours moves the dispatch up. So the old
     popt sometimes moves the jumping block in front of its target instead of the target up to the jump.
   - defid: the loop step `dec 0x16(fp),#2; jp head` of the SMEMB loop stands in the original right behind the
     `jp mismatch` at the end of the class switch (in front of redecl); ours keeps it behind the loop body. The source
     order with the mismatch block in front of redecl does not work (the CJP over the JP is turned round first).
   - Why open: the popt trace with block-move features (283 moves over all modules) shows no feature that separates
     the wrong moves from correct ones in exact functions; every rule tried (no move for multiply named labels, the
     jumping block in front of the target, the switch value at the dispatch) costs hundreds of lines elsewhere.
     cparse alone has too few witnesses.
2. **buildtree, 6 lines.** 4 lines: the double constant 0.0 (`r->dval == 0`) lies in the data behind the first switch
   table (0x2846), ours in front (0x27EA): the order in which cparse writes the .D file (constant at the statement,
   table at the end of the switch). 2 lines: the temporary of `buildtree( COLON, ll, lr )` is at 0x12 in the
   original, ours 0xC; it fits a cparse that does not give back all the frame space of the member block (cp2, cp1, m)
   at its end; moving these variables out of the block makes the frame 2 bytes larger. One witness each: not built.
3. **argdebug, 2 lines.** The original renames the unused register variable `a` (r14) into r1 and drops its save
   (`ld r1,r7`); our popt renames only registers saved with ldm pairs (procopt), and its rmove removes the restore
   first. A popt rule with a single witness: not built.
4. **yylex, 2 lines (one instruction).** `jp 0x21AA` where `jr` reaches: the early jump optimization of an older cas
   (Band B 3.4.5: open forward jumps become long when the optimization runs before their label). Evidence in four
   originals (cparse/scparse yylex, popt rmove, deroff, scodgen): one trigger point per cluster, always at a `call`.
   The shipped cas and the ZEUS cas (12/1986) do not show it; no table model fitted on the cparse input reproduces
   it without false long jumps. Parked as an explained remainder (time box).

**Plan (owner decision 2026-11-01): codgen and popt next.** `/lib/codgen` and `/lib/popt` were built by the same
compiler v0p (OLDCC.md, "v0p"). Make our sources of codgen and popt compiled with ccv0p byte-identical with them,
with the same method as cparse; the hope is that their many more functions give the witnesses that are missing for
the open cparse points (and every switch must keep cparse as it is):
- popt placement (point 1): codgen and popt have many switches and gotos; the placement rules of the old popt can be
  checked on them before cparse is touched.
- argdebug (point 3): leaf functions with one register variable in codgen/popt would show whether the old popt renames
  a single saved register.
- buildtree (point 2): only if codgen/popt have double constants and blocks with own variables (cparse behaviour of
  v0p: the .D order, the frame offsets of blocks); likely few.
- yylex (point 4): popt rmove already shows the cas effect; more clusters in codgen/popt give more trigger points
  for a model of the old cas (each needs an exact .s around it).
Steps: (1) set up the strict comparison for codgen and popt like cparse (progcfg with ORIG_BIN and STRICT_INPUT, a
link of our objects built with ccv0p with the original's data layout where D12 demands it, strictcmp; the
findings.json of `src/cmd/codgen/tools` and `src/cmd/popt/tools` have the function map); (2) a first measurement
with the current v0p; (3) then the same loop as for cparse: source forms first, switches with two witnesses, every
switch checked on cparse (`oldcc.sh model`, corpus) and the v0 bench. The normal builds of codgen and popt stay as
they are (they are the shipped programs).

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

**After codgen and popt:** back to the open cparse points with the new witnesses, then the remaining items of the
plan in `../src/TODO.md` ("Plan: towards the old C compiler").

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
   State 2026-11-01: v1 (strip and size byte-identical, ld 55 of 59); v0 (bench 118 of 153); v0p (/lib/cparse 173 of
   179, the current work above).

The stop line for the search for source forms is in `../src/TODO.md` ("codgen / scodgen: status").
