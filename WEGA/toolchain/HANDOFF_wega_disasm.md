# Handoff: WEGA build chain back to C (new session)

Written 2026-10-16 at the end of the MAME session. Read this first, then `mame/LOG.md` (long
background, English) and `mame/tests/` (what was measured on the real machine).

## Goal of the new work

Disassemble the WEGA build chain (C compilers `cc` and `scc`, assembler `cas`, linker `ld`,
`libc`, ...) and turn it back into C that builds on a modern Linux, so that it behaves like the
original and produces the same objects and binaries, byte for byte, as a build under WEGA. In
other words: a cross tool chain that is output-identical to the native one.

Suggested way (not decided with the owner yet, discuss first):

1. Get the original binaries out of the WEGA disk image and the floppy images (see below).
   Find the executable format (segmented Z8000, probably a.out like) and write a small
   reader.
2. Disassemble with a Z8000 disassembler (MAME's `z8000` core has one: debugger `dasm`, or the
   `unidasm` tool; a script for it is in `mame/tools/dbg.lua`). Segmented and non-segmented code can
   be mixed, take care of the format flags.
3. **Oracle first:** to know what "identical" means, create reference outputs by running the
   original tools under the emulator (WEGA boots in `p8000_16`, 1 to 8 MiB, hard disk image
   below, `sd.hd`; use a copy) on a set of test inputs (the sources in `WEGA/src` are good material). Save inputs,
   outputs and sizes. Every decompiled stage is checked against them.
4. Option: a small user mode Z8000 interpreter on Linux that maps the WEGA system calls to
   Linux ones would run the original binaries natively and give a fast oracle and a fallback.
5. Decompile bottom up: `libc` and the assembler and linker (small, format heavy) before the
   compilers. Compare at object level after every step.

## Where things are

- Git: `https://github.com/OlliL/P8000` (branch master, sparse checkout here: `UDOS/WEGA`,
  `WEGA/src/{boot,cmd/standalone,head/sys,stand,uts}`, `firmware/MON8`, `firmware/MON16`, `mame`).
  Commit with `git add --sparse -A mame WEGA/toolchain`. Other `WEGA/src` directories exist in the full repo
  (`git sparse-checkout add <dir>`): there are about 1100 files in the repo, among them `cmd`
  and the contributed K5JB sources.
- MAME fork: `https://github.com/OlliL/mame` (never push to `mamedev/mame`), checkout
  `/home/claude/mamedev/mame`, branch `p8k-clean`. Binaries: `mame` (optimised), `mamed`
  (`DEBUG=1`). Build scripts `/home/claude/runs/build.sh` and friends (see `mame/LOG.md`).
  Machines: `p8000` (8 bit board, UDOS) and `p8000_16` (16 bit board, WEGA).
- ROMs and disk images: `/home/claude/roms`, `/home/claude/sdimg` (`sd.hd` -> `sd.dd`, about 85 MB, the
  WEGA hard disk used for the boot tests; `sd_*.hd`/`tr*.hd` are variants from experiments, some with other
  kernels; `wega.stock`, `wega.sd_original` are small files, probably the kernel only, check), `/home/claude/flop` (floppy images and their
  raw conversions: `wgaend22.raw`, `w31start.raw`, `vsh.raw`, `udossys.raw`), `/home/claude/sw`.
  No ROM or software download links in any document or PR.
- Test harness `/home/claude/runs/sc` and copies in `mame/tools`: `console.py` (console on a
  socket, log in `mon.log`, input through the fifo `mon.in`), `mame/tools/dbg.lua`/`blk2.lua` (Lua scripts
  that press NMI, swap floppies, read memory and registers), `start.sh`, `run_*.sh`. Entering
  programs by hand into MON16 is slow (about 0.5 s per word).
- Manuals as scans with OCR text, in `/home/claude/docs` (not in git, copyright):
  `U8000-CPU.pdf` (German, 103 pages), `Z8001-Z8002-CPU-SGS-Thomson.pdf` (82 pages),
  `Z8001-2-CPU-Zilog-1985.pdf` (36 pages), each with `.ocr.txt` (English OCR, German umlauts
  garbled). Handy: instruction set tables in the SGS-Thomson manual, saved program status
  (Table 6.1), segmented addressing (3.4.2, 5.5).
- Other references named in `mame/LOG.md` section 15: Leventhal, Z8000 Assembly Language Programming;
  Moore, Z8000 Handbook; Mateosian, Programming the Z8000 (owner has the books and photographs
  pages on request).

## Z8000 facts that were verified on the real machine or in the emulator

- `m_pc` of the MAME core: segment in bits 16..22, offset in the low 16 bits. The offset wraps at
  `FFFF`, a two word block instruction at `seg:FFFC` restarts in the same segment (tests 05, 06).
- Trap frame on the system stack (segmented): identifier, FCW, PC segment word, PC offset;
  bit 15 of the PC segment word is kept (test 01). `sc #n` pushes `7Fnn`.
- Opcode examples used in the tests: `3A d5`/`3A s7` SINB/SOUTB with an immediate port, `C8vv`
  `ldb rl0,#vv`, `4D05 DA imm` `ld DA,#imm`, `5E08 DA` `jp DA`, `BB s1 0cd0` `ldir`, `97E0..97E3` pop,
  `93E3..93E0` push, `7B00` iret, `7F01`/`7F02` MON16 switch to segmented/non segmented mode, `7F00` break.
- MMU: three Z8010 (stack `F6`, data `FA`, code `FC`) selected by the special I/O port number.
  See `mame/LOG.md` section 5 and `mame/tests/02..04`.
- Fixed in the MAME core (branch `p8k-clean`): `DAB` after addition with a high digit
  (WEGA's decimal conversion in libc broke on it), PC wrap, the block instruction restart.

## Working rules of the owner (Oliver Lehmann, @OlliL)

- Conversation in German, short messages. Documents, commit messages, PR texts in English.
- Commit and push often as a safety net. Author `Oliver Lehmann <lehmann@ans-netz.de>`
  (`git -c user.name="Oliver Lehmann" -c user.email="lehmann@ans-netz.de" commit`), trailers
  `Co-Authored-By: Claude Sonnet 5.5 <noreply@anthropic.com>` and the `Claude-Session:` URL of the
  session that made the commit. The stop hook only checks `origin`; its "unpushed commits" alarms for
  other remotes are false alarms.
- Facts only from documents or measurements; real machine tests are possible on his P8000
  (16 bit board, 4 MiB, MON16), he types the programs by hand: keep them short and give him the
  words.
- The owner already tried to translate the WEGA kernel back to C once (`WEGA/src/uts`), and wrote
  an own RTC driver. Ask him what exists for the tool chain before starting.

## Open questions for the owner

- Which tools exactly, which WEGA version (the disk image is 3.x?), are the sources of any part
  of the chain already available (the repo has `cmd` sources and K5JB)?
- What counts as "identical": byte equal objects, including timestamps and symbol table order?
- May the original binaries be published in the repo (probably not): keep them out of git.
