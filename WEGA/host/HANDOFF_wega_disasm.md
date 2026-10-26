# Handoff: WEGA build chain back to C

Status 2026-10-26. Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up),
then this file (inventory, decisions, next steps), then `KNOWLEDGE.md` (facts) and `../src/TODO.md` (status per program).

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA one: `cc`, `scc`,
`cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `gopt`, `popt`, `cas`, `as`, `ld`, `sld`, `gld`, `ar`, `ranlib`,
`strip`, `size`, `libc` and `crt0`, built from our own C sources and checked byte for byte against the originals.
The original binaries run under the emulator (`host/z8k-emu`, the oracle); they are not part of the tool chain.

Byte identity is the only guarantee of identical behaviour, so it stays the goal (owner decision). Where the
original was built by an older compiler revision (D1 to D16 in `KNOWLEDGE.md`), byte identity needs that
compiler, which does not exist any more; this is the open task of step 3 below.

## Inventory: what belongs to the tool chain

Source of truth for the state: `../src/TODO.md` (per program) and the check commands in the last column.
"Source from binary" = our source was written from the original binary, not from a source tree.

| Tool | Original (WEGA 3.1 tree) | WEGA source (`src/`) | Host version (`host/`) | Source from binary | State | Check |
|---|---|---|---|---|---|---|
| `cc`, `scc` (driver) | `/bin/cc`, `/bin/scc` | `src/cmd/cc/cc.c` | `host/cc` | no (WEGA source) | done: 74 command lines identical; `cc` and `scc` byte-identical | `make -C host check` (cc) |
| `cpp` | `/lib/cpp` | - | - | - | not started | - |
| `cparse`, `scparse` | `/lib/cparse`, `/lib/scparse` | - | - | - | not started (largest part) | - |
| `codgen`, `scodgen` | `/lib/codgen`, `/lib/scodgen` | `src/cmd/codgen` | only built via `z8k-emu` (with the shipped `cc`) | yes | behaviour: corpus 101/101 (codgen and scodgen); byte identity open (D1 to D16) | `src/cmd/codgen/tools/check.sh`, `corpus.sh` |
| `gopt` | `/lib/gopt` (only with `-Ol`/`-Or`) | - | - | - | not started | - |
| `popt`, `spopt` | `/lib/popt`, `/lib/spopt` | - (source in `host/popt`) | `host/popt` | yes (`z20.c` to `z23.c`) | done: 522 corpus runs byte-identical | `make -C host check` (popt) |
| `cas` | `/bin/cas` | `src/cmd/cas` (`MODULES`) | `host/cas` | yes (decompiled, see `src/cmd/cas/README`) | done in behaviour; code not byte-identical (older scc) | `make -C host check` (cas) |
| `as` | `/bin/as` (PLZ/ASM) | - | - | - | not started | - |
| `ld` | `/bin/ld` | - (source in `host/ld`) | `host/ld/ld` | yes (`ld.c` from the binary) | done: 146 command lines identical | `make -C host check` (ld) |
| `sld` | `/bin/sld` | - (source in `host/ld`) | `host/ld/sld` | yes | done: 91 command lines identical | `make -C host check` (ld) |
| `gld` | `/bin/gld` | - (source in `host/ld`) | `host/ld/gld` | yes | done on the check lines | `make -C host check` (ld) |
| `ar` | `/bin/ar` | `src/cmd/ar.c` | `host/ar/ar` | to be checked | done on the host (archives identical) | `make -C host check` (ar) |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | `src/cmd/ranlib`, `strip`, `size` | `host/binutils` | to be checked | done on the host; the `src/cmd` versions still differ (`src/TODO.md`) | `make -C host check` (binutils) |
| `libc` | `/lib/libc.a` | `src/lib/libc` (170 modules in C), `src/lib/libc/oldcc` (10 modules as `.s`), `src/lib/oldlibc` (versions) | `host/libc` | partly | 170 of 180 from C; 7 differ (4 older cc, 3 other revision); 3 without C | `make -C host check` (libc), `make -C host/oldcc verify` |
| `crt0` (start-up files) | `/lib/crt0.o` and others | `src/lib/libc/crt` (`crt0.list`, `mkcrt0.sh`) | - | to be checked | generated; start code checked against the original | `src/lib/libc/crt` |
| `yacc` | `/usr/bin/yacc` (used by `verify-wega`) | - | - | - | not inventoried | open |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

Overall checks: `make -C host check` (all tools except `codgen`), `make -C host verify-wega` (builds `src/cmd` with the
original tools and compares each program).

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

## Next steps, in this order

1. **Complete the tool chain.** Order: front end (`cpp`, `cparse`/`scparse`, `gopt`), then `as`, then `codgen`/`scodgen`
   native on the host, then the check of the libraries (`libc`, `crt0`). Each tool passes the check in the table.
   Step 3 starts only after that.
2. **Bring the source to its natural form.** Revert the compensating changes or justify them. New baseline:
   bytes, functions, D classes.
3. **Reconstruct the old compiler.** Derive a model of the differences D1 to D16 from the fit functions, check it on a
   separate validation set. Goal: the original binaries reproduced byte-identical from our sources. Fix a time box
   and an abort criterion before starting.

The stop line for the search for source forms is in `../src/TODO.md` ("codgen / scodgen: status").
