# Handoff: WEGA build chain back to C

Status 2026-11-05. Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file: the inventory and the decisions of the whole project. The state of a program is in its `tools/FINDINGS.md`, that of the
old compilers in `OLDCC.md`, `OLDCC_SWITCHES.md` and the end of `OLDCC_LOG.md`. A section on the current work is written here only when a
handoff is due.

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA one: `cc`, `scc`,
`cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `gopt`, `popt`, `cas`, `as`, `ld`, `sld`, `gld`, `ar`, `ranlib`,
`strip`, `size`, `libc` and `crt0`, built from our own C sources and checked byte for byte against the originals.
The original binaries run under the emulator (`host/z8k-emu`, the oracle); they are not part of the tool chain.

Byte identity is the only guarantee of identical behaviour, so it stays the goal (owner decision). Where the
original was built by an older compiler revision (`OLDCC.md`), byte identity needs that compiler, which we rebuild.

## Inventory: what belongs to the tool chain

The state: the check commands in the last column; the plan: `../src/TODO.md`.
"Source from binary" = our source was written from the original binary, not from a source tree.

| Tool | Original (WEGA 3.1 tree) | Original filesize | WEGA source (`src/`) | Host version (`host/`) | Source from binary | State | Check |
|---|---|---|---|---|---|---|---|
| `cc`, `scc` (driver) | `/bin/cc`, `/bin/scc` | 14208, 14246 | `src/cmd/cc/cc.c` | `host/cc` | no (WEGA source) | done: 74 command lines identical; `cc` and `scc` byte-identical | `make -C host check` (cc) |
| `cpp` | `/lib/cpp` | 16522 | `src/cmd/cpp` (System III source, owner decision) | - | no | byte-identical with ccv0p | `tools/FINDINGS.md` |
| `cparse`, `scparse` | `/lib/cparse`, `/lib/scparse` | 67520, 67826 | `src/cmd/cparse` (K&R, from the Plexus PCC base, owner decision) | - | yes | byte-identical with ccv0p | `host/tools/corpus.sh cparse scparse`, `host/recon/oldcc.sh model cparse` |
| `codgen`, `scodgen` | `/lib/codgen`, `/lib/scodgen` | 80884, 83554 | `src/cmd/codgen` (K&R) | `host/codgen` (ANSI, gcc `-Wall`; `-DSEG` for scodgen; functional equality) | yes | byte-identical with ccv0p | `host/tools/corpus.sh codgen scodgen` |
| `gopt` | `/lib/gopt` (only with `-Ol`/`-Or`) | 147286 | - | - | - | not started | - |
| `popt`, `spopt` | `/lib/popt`, `/lib/spopt` | 25620, 25880 | `src/cmd/popt` (K&R, `z20.c` to `z23.c`, `stdio.h`; the bss order from the definitions at the end of `z2.h`) | `host/popt` | yes | byte-identical with ccv0p | `make -C host check` (popt); `host/tools/corpus.sh popt spopt` |
| `cas` | `/bin/cas` | 89152 | `src/cmd/cas` (`MODULES`) | `host/cas` | yes (decompiled, see `src/cmd/cas/README`) | done in behaviour; code not byte-identical (older scc) | `make -C host check` (cas) |
| `as` | `/bin/as` (PLZ/ASM) | 74536 | - | - | - | not started | - |
| `ld` | `/bin/ld` | 21732 | - (source in `host/ld`) | `host/ld/ld` | yes (`ld.c` from the binary) | done: 146 command lines identical | `make -C host check` (ld) |
| `sld` | `/bin/sld` | 46074 | `src/cmd/sld` (shipped scc, `lib/slibc-v9.a`) | `host/ld/sld` | yes | `src/cmd/sld` byte-identical (2026-11-07); host: 91 command lines identical | `make -C host checkall`, `make -C host check` (ld) |
| `gld` | `/bin/gld` | 27616 | - (source in `host/ld`) | `host/ld/gld` | yes | done on the check lines | `make -C host check` (ld) |
| `ar` | `/bin/ar` | 12396 | `src/cmd/ar.c` | `host/ar/ar` | to be checked | done on the host (archives identical) | `make -C host check` (ar) |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | 7732, 9072, 7732, | `src/cmd/ranlib`, `strip`, `size` | `host/binutils` | to be checked | done on the host; the `src/cmd` versions byte-identical (`checkall`) | `make -C host check` (binutils) |
| `libc`, `slibc` | `/lib/libc.a`, `/lib/slibc.a` | 90484, 110938 | `src/lib/libc` (`libc.a` byte-identical as a whole file; 175 of 179 objects from source), `src/lib/libc/oldcc` (4 objects as `.s`), `src/lib/oldlibc` (versions) | `host/libc` | partly | 175 of 179 objects from C; 4 by a compiler between v1 and shipped (`.s`) | `make -C host check` (libc), `make -C host/oldcc verify` |
| `crt0` (start-up files) | `/lib/crt0.o` and others | 226 |  `src/lib/libc/crt` (`crt0.list`, `mkcrt0.sh`) | - | to be checked | generated; start code checked against the original | `src/lib/libc/crt` |
| `yacc` | `/usr/bin/yacc` (used by `verify-wega`) | 37224 |  - | - | - | not inventoried | open |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

Overall checks: `make -C host checkall` (everything byte-identical, the corpora and the following), `make -C host check` (all tools except `codgen`), `make -C host verify-wega` (builds `src/cmd` with the
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

- **`host/oldcc`** stays: it checks the four `.s` objects that the old cc produced. Its paths are corrected.
- **`yacc`:** should it be added to the inventory and to the tool chain?

## Next steps

The plan is `../src/TODO.md`; its step 5 (the open programs in `cmd/`) is done (2026-11-07), the owner decides on the next step. Pause after each finished program.

Identical since 2026-11-05: dc and ranlib (v0), write and rmdir (v1r); ld (v1) and sld (shipped scc) since 2026-11-07. All 8 commands of doscopy 1.0 (`1.3` is ignored) and pburn 2.1 identical (shipped scc). Step 5 of `../src/TODO.md` is done with them. Open in step 2: 13 `.s` in `oldlibc`, 2 of them real assembler, and 4 objects of `libc/oldcc`.

Which compiler to take: the rule in `HOWTO_reconstruct.md`, "Evidence rules": the date first, measure the neighbours, sharpen an existing version before a new one. The time line is in `OLDCC.md`. Do not invent versions.

`make -C host checkall` (14 minutes): last full run `ALL OK` at f4d1be0d (2026-11-05), with all corpora. Seen once: the build stopped with `cannot unlink /tmp/cc.-5.s` and passed unchanged on the next run; guess: the emulator gives the host pid to the guest, negative as 16 bits. Not fixed. `oldcc.sh matrix check` reports the table of switches as older than the passes: measure again (step 8 at the latest).
