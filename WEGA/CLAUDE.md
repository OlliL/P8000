# WEGA tool chain: instructions for Claude

Project: rebuild the WEGA (Z8000/P8000) tool chain and system sources, output-identical to the
originals. Branch `toolchain` of github.com/OlliL/P8000. Project dir `WEGA/` (`src/`, `host/`, `compare/`, `contrib/`; `compare/NAME/` holds what belongs to a program besides its source: corpus, `findings.json`, `SYSIII.md`).
Read `host/HANDOFF_wega_disasm.md` (state, next steps), then `host/KNOWLEDGE.md` (facts) and
`src/TODO.md` (the plan).

## Rules

- Conversation in German, short messages. Git content (files, commits) in English.
- Commits are allowed, but pushes are not. Only allowed branch to work on is `toolchain`.
- `src/TODO.md` holds only the plan; keep `host/KNOWLEDGE.md` current and say which files changed.
- Facts only from documents or measurements; mark guesses as guesses.
- Always keep two tables current while working: "Objects and their differences" in `host/OLDCC.md`
  (which object shows which difference D1 ... D23) and "Bugs and quirks of the originals" in `host/KNOWLEDGE.md`
  (every bug, oddity or dead code found in an original program, with how the rebuild handles it).
- When working on the compiler: New or changed switch of an old compiler, or a new program with a model: run `host/recon/oldcc.sh matrix check`
  and follow what it says; `host/OLDCC_SWITCHES.md` is generated, see `host/HOWTO_reconstruct.md`, "Evidence rules".
- The old compilers: `host/OLDCC.md` holds only what is currently fact (rewritten, not appended to; no removed switches, refuted ideas or
  counts on the way); running notes, measurements, attempts and what went away go as dated entries to `host/OLDCC_LOG.md`. Other documents do not repeat this knowledge.
- Notes on findings only in the program's `findings.json`.
- Byte identity with the original has priority over fixing a bug found in the original: rebuild the bug first, record it in the quirks table, and fix it only when the owner says so.
- SYSIII sources (tmp/sys3) can and should be the base for reconstructing the WEGA sources. They are allowed to be copied from. The Plexus sources can be consulted for Z8000 specific stuff.
- V7 layout in `src/cmd`. Makefiles must also work with WEGA's native `make`:
  use `=` not `?=`, `cd dir ; $(MAKE)`.

## Working economically (tokens)

- Answers short: result, changed files, open points; no recap of what the owner already knows, no repeated tables or file
  contents, no lists of options that will not be taken.
- Read only what is needed: `grep`, `head`, `sed -n`, line ranges; never whole large files or logs (`tail`, `grep` on them).
  Do not read a file again after editing it.
- Tool output small: filter with `grep`/`tail`, `| head`, `-q`; long runs (checkall, corpora) in the background with the
  result line only, no polling.
- Independent calls in one step; no subagents unless the owner asks.
- Ask only when a decision is really the owner's; otherwise decide, say it in one line and go on.

## Local directories

| Path                                             | Content                                              | Use                                                 |
| ------------------------------------------------ | ---------------------------------------------------- | --------------------------------------------------- |
| `/mnt/share/cvs/P8000/tmp/wega31_root`           | extracted WEGA 3.1 file tree (`bin/ etc/ lib/ usr/`) | ROOT for `z8k-emu -r`; originals to compare against |
| `/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu` | z8k Emulator (oracle)                                | run original WEGA binaries                          |
| `/mnt/share/cvs/P8000/tmp/wega31_root/usr/man`   | man pages                                            | read with `pcat` under the emulator                 |
| `/mnt/share/cvs/P8000/tmp/wega31_books`          | WEGA manuals and handbooks (scans, OCR text)         | lookup                                              |
| `/mnt/share/cvs/P8000/tmp/cpu_docs`              | Z8000/Z8001/Z8002 CPU manuals (PDF + OCR text)       | instruction sets, formats                           |
| `/mnt/share/cvs/P8000/tmp/wega31_kernel`         | original kernel objects / libraries                  | compare, disassemble                                |
| `/mnt/share/cvs/P8000/tmp/plexus`                | Plexus sources, private reference only               | read for ideas, never copy                          |
| `/mnt/share/cvs/P8000/tmp/sys3`                  | SYS III sources                                      | use as a starting point for source reconstruction   |
| `/mnt/share/cvs/P8000/tmp/wega31_disks`          | WEGA 3.1 floppy images and raw conversions           | extract originals                                   |
| `/mnt/share/git/mame/`                           | MAME fork with P8000 machines (`p8000`, `p8000_16`)  | real-system tests, boot WEGA                        |
| `/mnt/share/cvs/P8000/tmp/tmp`                   | temp directory                                       | intermediate files, build logs                      |

Paths on the owner's machine are in his scripts (`src/make_cmd_linux.sh`); do not copy them into documents.

## Build the emulator and check

    make -C WEGA/host/z8k-emu            # builds z8k-emu
    make -C WEGA/host check WEGA=<WEGA_ROOT>      # the host tools without a corpus: cas, ld
    make -C WEGA/host checkall WEGA=<WEGA_ROOT>   # on a copy of the git files: make in src, make install into an empty root,
                                                  # every file there against WEGA; the corpora; check (16 min)
    host/tools/corpus.sh [--wega|--host] [PROG ...]   # the corpus of a program (compare/PROG/progcfg.py): the original against every candidate
    host/tools/corpusdeps.sh [PROG ...]               # strace of the corpus runs: is every file/program they use in the cache key (CORPUS_ROOT_FILES, CORPUS_DEPENDS)?
    host/tools/mkcorpus.sh [PROG ...]                 # make the prepared inputs (compare/PROG/corpus.tar) and the result cache (compare/PROG/cache) new; both not in git

## Compile with the original tools (z8k-emu)

    z8k-emu [-r ROOT] [-t|-T] [-v] [-e NAME=VALUE] PROGRAM [ARG...]

- `ROOT` = `<WEGA_ROOT>`. Absolute guest paths are looked up below ROOT (guest `/tmp` = `ROOT/tmp`);
  relative paths refer to the host working directory. A file outside ROOT and the working
  directory is invisible to the guest: copy test input below the working directory.
- Debug: `Z8K_EXEC_TRACE=1`, `Z8K_SC_TRACE=FILE`, `-t` (`-T`: also the floating point emulator); `host/z8k-emu/README.md`.

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

Whole tree: `make -C WEGA/host src`: `make` in `src/` with the tools under the emulator in the PATH (`host/tools/wbin.sh`); the Makefiles
use the plain names. Make itself runs on the host, not in the emulator. The old compilers go to `OLDROOT` (`make -C WEGA/host oldroot`,
then `z8k-emu -r <OLDROOT> /bin/ccv1 ...`); `make install PREFIX=DIR` in `src/` installs the shipped programs and libraries, stripped as the originals.

Limits: EPU (floating point) instructions run through a C port of the kernel's `fpe.s`
(`host/z8k-emu/README.md`); a floating point trap (SIGFPE) stops the emulator. Guest signal
handlers do not run.

## Compare

- `host/tools/cmpins.py` compares code instruction by instruction, `host/tools/sdis.py` disassembles,
  `host/tools/war.py` reads archives, `host/tools/epu.py` EPU formats.
- Nearest-version method: find the nearest git version of a source (history on GitHub master:
  `gh api 'repos/OlliL/P8000/commits?sha=master&path=...'`; the local clone is shallow, do not unshallow),
  build with the oracle, compare.

## Where to look things up

- Tool facts, formats, syscall ABI, results: `host/KNOWLEDGE.md`; the old compilers: `host/OLDCC.md`; how a program is reconstructed: `host/HOWTO_reconstruct.md`.
- CPU instruction tables, segmented addressing: `tmp/cpu_docs`.
- Man pages of the tools: `tmp/wega31_root/usr/man` (via `pcat`).
- System behaviour, kernel interfaces: `tmp/wega31_kernel`, `src/uts`, `src/head/sys`.
- Sources to start from: `tmp/sys3` (System III); for what is specific to the Z8000: `tmp/plexus` (see Rules). Original binaries: `tmp/wega31_root`;
  floppies: `tmp/wega31_disks`.
- Real machine behaviour: `/mnt/share/git/mame/`, `mame/LOG.md` in the repository (beside `WEGA/`).
