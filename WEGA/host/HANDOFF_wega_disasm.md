# Handoff: WEGA build chain back to C

Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file: the inventory and the decisions of the whole project. The state of a program is in its `tools/findings.json`. Its readable (generated) form is `tools/FINDINGS.md`. The state of the old compilers is in `OLDCC.md`, `OLDCC_SWITCHES.md` and everything we learn in form of a logbook in `OLDCC_LOG.md`. A section on the current work is written here only when a handoff is due.

## Goal

1. restore all sources of the whole WEGA installation

2. create a tool chain which runs on Linux for crosscompiling

## Inventory: what belongs to the tool chain

| Tool                      | Original (WEGA 3.1 tree)                         | WEGA source                                     | WEGA status                           | Host source                                      | Host status               |
| ------------------------- | ------------------------------------------------ | ----------------------------------------------- | ------------------------------------- | ------------------------------------------------ | ------------------------- |
| `cc`, `scc`               | `/bin/cc`, `/bin/scc`                            | `src/cmd/cc`                                    | exact                                 | `host/cc`                                        | needs rechecking          |
| `cpp`                     | `/lib/cpp`                                       | `src/cmd/cpp`                                   | exact                                 | -                                                | missing                   |
| `cparse`, `scparse`       | `/lib/cparse`, `/lib/scparse`                    | `src/cmd/cparse`                                | exact                                 | -                                                | missing                   |
| `codgen`, `scodgen`       | `/lib/codgen`, `/lib/scodgen`                    | `src/cmd/codgen`                                | exact                                 | `host/codgen` (gcc `-Wall`; `-DSEG` for scodgen) | needs rechecking          |
| `gopt`                    | `/lib/gopt` (only with `-Ol`/`-Or`)              | -                                               | **missing**                           | -                                                | **missing**               |
| `popt`, `spopt`           | `/lib/popt`, `/lib/spopt`                        | `src/cmd/popt`                                  | exact                                 | `host/popt`                                      | needs rechecking          |
| `cas`                     | `/bin/cas`                                       | `src/cmd/cas`                                   | **see its FINDINGS.md**               | `host/cas`                                       | needs rechecking          |
| `as`                      | `/bin/as` (PLZ/ASM)                              | -                                               | **missing**                           | -                                                | **missing**               |
| `ld`                      | `/bin/ld`                                        | `src/cmd/ld`                                    | exact                                 | `host/ld/ld`                                     | needs rechecking          |
| `sld`                     | `/bin/sld`                                       | `src/cmd/sld`                                   | exact                                 | `host/ld/sld`                                    | needs rechecking          |
| `gld`                     | `/bin/gld`                                       | -                                               | **missing**                           | `host/ld/gld`                                    | needs rechecking          |
| `ar`                      | `/bin/ar`                                        | `src/cmd/ar.c`                                  | exact                                 | `host/ar/ar`                                     | needs rechecking          |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | `src/cmd/ranlib` `src/cmd/strip` `src/cmd/size` | exact                                 | `host/binutils`                                  | needs rechecking          |
| `libc`, `slibc`           | `/lib/libc.a`, `/lib/slibc.a`                    | `src/lib/libc`                                  | **libc: see TODO.md; slibc: missing** | `host/libc`                                      | `make -C host check` (libc); to be dropped with step 8 of TODO.md |
| `crt0` (start-up files)   | `/lib/crt0.o`, `scrt0.o`, `mcrt0.o`, `smcrt0.o` | `src/lib/libc/csu` (`crt0.o`), `src/lib/libc/crt` (the start-up files of the programs) | **crt0.o and those of the programs: exact; scrt0, mcrt0, smcrt0: missing** | - | not needed |
| `yacc`                    | `/usr/bin/yacc` (used by the build of cas)      | -                                               | **missing**                           | -                                                | **missing**               |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

## Decided: equality criteria per code base

- `src/cmd` (K&R C, WEGA `cc`): byte identity with the original is the top priority. Original defects are rebuilt first and recorded in the quirks table in `KNOWLEDGE.md`; a fix needs the owner's decision. Second priority to keep the sources as equal to its SYSIII counterparts as possible (Copy&Paste is allowed).
- `host/` (ANSI C, gcc): functional equality only. Fixing defects of the original is allowed where the original's result is undefined or wrong; the fix is documented in the quirks table and in `src/TODO.md`. Functional equality with the original is the top goal here and has to be assured by corpus runs.

## Next steps

A more detailed plan (steps) is in `../src/TODO.md`
