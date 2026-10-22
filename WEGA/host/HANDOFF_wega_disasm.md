# Handoff: WEGA build chain back to C

Status 2026-10-22. Read `../CLAUDE.md` first (how to build, where to look things up),
then `KNOWLEDGE.md` (all facts, long) and `../src/TODO.md` (what is not yet identical).

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA
one: `cc`, `scc`, `cas`, `as`, `ld`, `gld`, `ar`, `ranlib`, `strip`, `size`, `libc`, built from
our own C sources, checked byte for byte against the original binaries.

## What exists

- **Oracle:** `host/z8k-emu` runs the original WEGA binaries on Linux (Z8001/Z8002 user mode,
  system calls mapped to Linux, incl. `fcntl`). The whole original chain runs under it:
  `cc scc cas as ld sld gld ar strip ranlib yacc` and `pcat` (man pages).
- **Native host tools:**
  - `host/cc`: decompiled `cc` driver, 74 command lines identical to the original. Pass paths,
    `mv` and the temp directory are set in `cc.conf` next to the executable (library
    `host/tools/lib/conf.c`, template `cc/cc.conf.example`); `scc` is the same source built with `-DSCC` (`scc.conf`).
  - `host/popt`: peephole optimizer, 522 corpus runs byte identical.
  - `host/ar`: `ar`, archives and output identical with the original.
  - `host/binutils`: `ranlib`, `strip`, `size`, output and files identical with the originals.
  - `host/cas`: the cas assembler, messages, objects and listings identical with the original
    (see `KNOWLEDGE.md`, "cas on the host").
- **Checks:** `make -C host check` (cc, ar, binutils, cas, popt, libc 170 identical / 7 different /
  3 without C source, oldcc). `make -C host verify-wega` builds `src/cmd` with the original tools and
  compares per program (see `../src/TODO.md`).
- **Sources:** `src/cmd` builds under the oracle (see `src/TODO.md` for the per program state;
  `ar`, `tar`, `dump`, `dumpdir`, `restor`, `sync`, `cc`, `scc`, `pb.image`, `standalone/sa.*`,
  `pburn` 2.1 identical; `cas` rebuilt with the same behaviour). `src/lib/libc` has
  C sources for 170 members, `src/lib/libc/oldcc/<member>/<version>.s` for the rest; other libc versions: `src/lib/oldlibc/README`.
- **Analysis scripts:** `host/tools` (`sdis.py` disassembler, `cmpins.py` instruction compare,
  `epu*.py`, `war.py`, ...).

## What is missing for a complete host tool chain

| Part | State |
|---|---|
| `cpp` | not started (original `/lib/cpp`; Plexus-free reconstruction needed) |
| `cparse` / `scparse` | not started; the parser is the biggest part |
| `codgen` / `scodgen` | not started |
| `gopt` | not started (only used with -Ol/-Or) |
| `popt`, `spopt` | `popt` done (non-segmented); `spopt` not started |
| `cas` | done: all modules decompiled to `src/cmd/cas/` (same behaviour as the original, code of the older scc not byte identical, see `src/cmd/cas/MODULES`); host version `host/cas`, checked against the original |
| `as` | not started |
| `ld` | done: `host/ld` (nonsegmented), 146 command lines identical to the original, `make -C host/ld check` |
| `sld` | done: `host/ld/sld.c`, 91 command lines identical (kernel link, cas link included), `make -C host/ld check` |
| `gld` | done: `host/ld/gld.c`, same files and messages as `/bin/gld` on the check lines (pburn link included), `make -C host/ld check` |
| `ar` | done on the host (`host/ar`, identical behaviour and archives) |
| `ranlib`, `strip`, `size` | done on the host (`host/binutils`); the `src/cmd` versions still differ from the originals (see TODO) |
| `cc`, `scc` drivers | done, paths in `cc.conf` / `scc.conf`; WEGA source `src/cmd/cc/cc.c`; cc and scc built there are byte-identical with the originals |
| libc | 170 of 180 members from C; 7 differ (4 made by an older cc, 3 of another source revision), 3 without C |

## Next steps, in this order (fastest way to a usable local tool chain)

1. **Drivers `cc` and `scc` stand-alone: done** (`host/cc`, one source, `cc.conf` / `scc.conf` set the
   passes; `scc` uses `sld`, not `gld`; both checked with `make -C host/cc check`).
2. **Wrapper scripts** `host/bin/{cc,scc,cas,as,ld,gld,ar,...}` that run the original passes under
   `z8k-emu -r ROOT`, so `src/cmd` can be built with plain `make` (CC=host/bin/cc).
   Then `make -C src/cmd` is the daily build and the pieces are replaced one by one.
   Not done yet (`host/bin` does not exist); `src/make_cmd_linux.sh` is the owner's local stand-in.
3. **Small data-format tools first, native:** `ar` (done, `host/ar`, checked against the original),
   `ranlib`, `strip`, `size` (done, `host/binutils`, checked against the originals), then an
   `nm`-like reader.
4. **Assembler `cas` (and `as`):** `cas` done (`src/cmd/cas`, `host/cas`); `as` not started. Closed grammar, good oracle (every `.s` in the tree, every
   `cc -S` output, `src/lib/libc/oldcc/*/*.s`). Decode from the binary with `tools/sdis.py`; check with
   `cmpins.py`. Do `cas` before `as` (PLZ/ASM is separate and used less).
5. **Linkers:** `ld`, `sld` and `gld` done (`host/ld`, see `KNOWLEDGE.md`, "ld/sld/gld on the host"). Oracle: relink every program in the tree and
   compare; the recorded link lines of `src/cmd` (cas, pburn) are the first test input.
6. **Emulator: EPU arithmetic** (needed for `cparse` on floating constants and for the doscopy
   commands): done 2026-10-22 as a C port of the kernel's `src/uts/conf/fpe.s`
   (`z8k-emu/fpe2c.py` -> `fpe_gen.c`, `epu.c`; checked on the host, see `KNOWLEDGE.md`, "EPU
   (floating point)"). Still to do: run `cparse` on `dosopn.c` and build the doscopy commands
   (see `src/TODO.md`).
7. **Back end `codgen` (+ `scodgen`), then `gopt`, `spopt`:** table driven, check against the
   oracle's `-S` output on the whole source tree (libc, cmd, kernel). `popt` experience applies.
8. **Front end `cpp`, `cparse` (+ `scparse`):** largest part, leave it to last; with
   the oracle the intermediate file (`cc -:z` temp files) can be compared pass by pass.
9. **libc leftovers:** the 7 differing and 3 missing members (older cc, other source revision; see
   the TODO table); target: the libc on the system first, then every version the shipped binaries need.
10. **Close out:** `make verify` with only native tools and no emulator; keep the emulator as
    oracle in CI-like checks.

Steps 1 to 3 are a few sessions; 4 to 5 follow; 6 to 8 are the real decompilation work.

## Working rules of the owner (Oliver Lehmann, @OlliL)

- Conversation in German, short messages. Documents, commit messages, PR texts in English.
- The owner handles the repository (commit, push, branches). Claude only edits files.
- Facts only from documents or measurements; mark guesses.
- Keep `host/KNOWLEDGE.md` and `src/TODO.md` current.
- Originals, binaries and copyrighted material stay out of git (explicit exceptions by the owner).
  Plexus sources are private reference only; reconstructed code is not a copy of them.
- V7 layout in `src/cmd`; Makefiles must also work with WEGA's native `make`
  (`=` not `?=`, `cd dir ; $(MAKE)`).

## Open questions for the owner

- Which revision of libc is the target (of the 7 differing members 4 were made by an older cc, 3 are another source revision)?
  - answer: currently the one on the system. After that all the ones which are needed to get every binary bit-compatible to the delivered version 
- What does `p8000c` (second `sa.diags`) and the `87wega3.2` SCCS suffix stand for?
  - p8000c mean P8000 compact - which is a newer hardware. Not relevant for us.