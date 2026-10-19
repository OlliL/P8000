# Handoff: WEGA build chain back to C

Status 2026-10-19. Read `../CLAUDE.md` first (how to build, where to look things up),
then `KNOWLEDGE.md` (all facts, long) and `../src/TODO.md` (what is not yet identical).

## Goal

A tool chain on the Linux host (`WEGA/host/`) that is output-identical to the native WEGA
one: `cc`, `scc`, `cas`, `as`, `ld`, `gld`, `ar`, `ranlib`, `strip`, `size`, `libc`, built from
our own C sources, checked byte for byte against the original binaries.

## What exists

- **Oracle:** `host/z8k-emu` runs the original WEGA binaries on Linux (Z8001/Z8002 user mode,
  system calls mapped to Linux, incl. `fcntl`). The whole original chain runs under it:
  `cc scc cas as ld gld ar strip ranlib` and `pcat` (man pages).
- **Native host tools:**
  - `host/cc`: decompiled `cc` driver, 74 command lines identical to the original. Pass paths,
    `mv` and the temp directory are set in `cc.conf` next to the executable (library
    `host/tools/lib/conf.c`, template `cc/cc.conf.example`); `scc` is the same source built with `-DSCC` (`scc.conf`).
  - `host/popt`: peephole optimizer, 522 corpus runs byte identical.
- **Checks:** `make -C host check` (cc, popt, libc 170 identical / 7 different / 3 without C
  source, oldcc). `make -C host verify-wega` builds `src/cmd` with the original tools and
  compares per program.
- **Sources:** `src/cmd` builds under the oracle (see `src/TODO.md` for the per program state;
  `ar`, `dump`, `dumpdir`, `restor`, `sync`, `pb.image`, `pburn` 2.1 identical). `src/lib/libc` has
  C sources for 170 members, `oldcc/*.s` for the rest.
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
| `cas`, `as` | not started |
| `ld`, `sld`, `gld` | not started; `gld` man page read, `pburn` 2.1 link line known |
| `ar`, `ranlib`, `strip`, `size` | C sources exist in `src/cmd` (strip: one expression differs) |
| `cc`, `scc` drivers | done, paths in `cc.conf` / `scc.conf` |
| libc | 170 of 180 members from C; 7 differ (older/newer cc revision), 3 without C |

## Next steps, in this order (fastest way to a usable local tool chain)

1. **Drivers `cc` and `scc` stand-alone: done** (`host/cc`, one source, `cc.conf` / `scc.conf` set the
   passes; `scc` uses `sld`, not `gld`; both checked with `make -C host/cc check`).
2. **Wrapper scripts** `host/bin/{cc,scc,cas,as,ld,gld,ar,...}` that run the original passes under
   `z8k-emu -r ROOT`, so `src/cmd` can be built with plain `make` (CC=host/bin/cc).
   Then `make -C src/cmd` is the daily build and the pieces are replaced one by one.
3. **Small data-format tools first, native:** `ar`/`ranlib` (done in `src/cmd`, build for Linux
   with a host libc shim), then `strip`, `size`, `nm`-like reader. Compare to the original on the
   whole WEGA tree.
4. **Assembler `cas` (and `as`):** closed grammar, good oracle (every `.s` in the tree, every
   `cc -S` output, `oldcc/*.s`). Decode from the binary with `tools/sdis.py`; check with
   `cmpins.py`. Do `cas` before `as` (PLZ/ASM is separate and used less).
5. **Linkers `ld`, then `sld`/`gld`:** object/a.out formats, relocation (`tools/check_reloc.py`),
   man page for `gld`. Oracle: relink every program in the tree and compare.
6. **Emulator: EPU arithmetic** (needed for `cparse` on floating constants and for the doscopy
   commands; formats decoded in `tools/epu.py`). Do it before the parser decompilation, otherwise
   the oracle cannot compile `dosopn.c` (see `src/TODO.md`).
7. **Back end `codgen` (+ `scodgen`), then `gopt`, `spopt`:** table driven, check against the
   oracle's `-S` output on the whole source tree (libc, cmd, kernel). `popt` experience applies.
8. **Front end `cpp`, `cparse` (+ `scparse`):** largest part, leave it to last; with
   the oracle the intermediate file (`cc -:z` temp files) can be compared pass by pass.
9. **libc leftovers:** the 7 differing and 3 missing members (older/newer revisions; see the
   TODO table), decide which revision the target is.
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

- Which revision of libc is the target (the 7 differing members come from older/newer cc revisions)?
  - answer: currently the one on the system. After that all the ones which are needed to get every binary bit-compatible to the delivered version 
- What does `p8000c` (second `sa.diags`) and the `87wega3.2` SCCS suffix stand for?
  - p8000c mean P8000 compact - which is a newer hardware. Not relevant for us.