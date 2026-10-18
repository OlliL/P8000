# WEGA tool chain: instructions for Claude

Project: rebuild the WEGA (Z8000/P8000) tool chain and system sources, output-identical to the
originals. Branch `toolchain` of github.com/OlliL/P8000. Project dir `WEGA/` (`src/`, `host/`, `contrib/`).
Read `host/HANDOFF_wega_disasm.md` (state, next steps), then `host/KNOWLEDGE.md` (facts) and
`src/TODO.md` (open differences).

## Rules

- Conversation in German, short messages. Git content (files, commits) in English.
- Push often. Keep `host/KNOWLEDGE.md` and `src/TODO.md` current.
- Facts only from documents or measurements; mark guesses as guesses.
- Originals, binaries and copyrighted material never go into git (unless the owner says so).
  Plexus sources are private reference only; our code is reconstructed from the WEGA binaries
  and is not a copy of them.
- V7 layout in `src/cmd`. Makefiles must also work with WEGA's native `make`:
  use `=` not `?=`, `cd dir ; $(MAKE)`.
- Commit trailers: `Co-Authored-By: Claude Sonnet 5.5 <noreply@anthropic.com>` and the `Claude-Session:` URL.

## Local directories (placeholders, the owner fills in the real paths)

| Placeholder | Content | Use |
|---|---|---|
| `<WEGA_ROOT>` | extracted WEGA 3.1 file tree (`bin/ etc/ lib/ usr/`) | ROOT for `z8k-emu -r`; originals to compare against |
| `<Z8K_EMU>` | built `host/z8k-emu/z8k-emu` (or its path) | run original WEGA binaries |
| `<WEGA_MAN>` | man pages (inside `<WEGA_ROOT>/usr/man`) | read with `pcat` under the emulator |
| `<WEGA_BOOKS>` | WEGA manuals and handbooks (scans, OCR text) | lookup |
| `<CPU_DOCS>` | Z8000/Z8001/Z8002 CPU manuals (PDF + OCR text) | instruction sets, formats |
| `<KERNEL_OBJECTS>` | original kernel objects / libraries | compare, disassemble |
| `<PLEXUS_SRC>` | Plexus sources, private reference only | read for ideas, never copy |
| `<WEGA31_DISKS>` | WEGA 3.1 floppy images and raw conversions | extract originals |
| `<MAME_REPO>` | MAME fork with P8000 machines (`p8000`, `p8000_16`) | real-system tests, boot WEGA |
| `<SCRATCH>` | temp directory | intermediate files, build logs |

Paths on the owner's machine are in his scripts (`src/make_cmd_linux.sh`); do not copy them into documents.

## Build the emulator and check

    make -C WEGA/host/z8k-emu            # builds z8k-emu
    make -C WEGA/host check WEGA=<WEGA_ROOT>   # cc, popt, libc, oldcc checks
    make -C WEGA/host verify-wega WEGA=<WEGA_ROOT>   # build src/cmd with the original tools, compare

## Compile with the original tools (z8k-emu)

    z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]

- `ROOT` = `<WEGA_ROOT>`. Absolute guest paths are looked up below ROOT (guest `/tmp` = `ROOT/tmp`);
  relative paths refer to the host working directory. A file outside ROOT and the working
  directory is invisible to the guest: copy test input below the working directory.
- Debug: `Z8K_EXEC_TRACE=1`, `Z8K_SC_TRACE=FILE`, `-t`.

Examples (run in the directory of the source, `E="z8k-emu -r <WEGA_ROOT>"`):

    $E /bin/cc -c -O foo.c                  # non-segmented: cpp > cparse > codgen > popt > cas
    $E /bin/cc -o foo foo.o                 # link with /lib/crt0.o, /lib/libc.a
    $E /bin/scc -c foo.c                    # segmented chain: scparse, scodgen, spopt
    $E /bin/gld /lib/scrt0.o foo.o /lib/slibc.a -o foo   # segmented link
    $E /bin/cas -u foo.s                    # assemble (cas)
    $E /bin/as -u foo.s                     # PLZ/ASM, needs -u for external symbols (e.g. _printf)
    $E /bin/ar rv lib.a a.o b.o             # archives
    $E /usr/bin/strip foo
    $E /usr/bin/pcat /usr/man/man1/gld.1    # read a man page

Whole tree: `make -C WEGA/src/cmd` with `CC="z8k-emu -r <WEGA_ROOT> /bin/cc"` and the same for
`CAS AS LD GLD AR STRIP` (see the top of `host/Makefile`, variable `WTOOLS`).
Make itself runs on the host, not in the emulator.

Limits: EPU (floating point) arithmetic is not emulated, so `/lib/cparse` fails on floating
constants (blocks `src/cmd/doscopy`). Guest signal handlers do not run.

## Compare

- `host/tools/cmpins.py` compares code instruction by instruction, `host/tools/sdis.py` disassembles,
  `host/tools/war.py` reads archives, `host/tools/epu.py` EPU formats.
- Nearest-version method: find the nearest git version of a source (history on GitHub master:
  `gh api 'repos/OlliL/P8000/commits?sha=master&path=...'`; the local clone is shallow, do not unshallow),
  build with the oracle, compare.

## Where to look things up

- Tool facts, formats, syscall ABI, results: `host/KNOWLEDGE.md`.
- CPU instruction tables, segmented addressing: `<CPU_DOCS>`.
- Man pages of the tools: `<WEGA_MAN>` (via `pcat`).
- System behaviour, kernel interfaces: `<KERNEL_OBJECTS>`, `src/uts`, `src/head/sys`.
- Ideas only: `<PLEXUS_SRC>`. Original binaries: `<WEGA_ROOT>`; floppies: `<WEGA31_DISKS>`.
- Real machine behaviour: `<MAME_REPO>`, `mame/LOG.md` in the repo.
