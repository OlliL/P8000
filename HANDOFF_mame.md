# Handoff: P8000 in MAME, remaining steps (Claude CLI)

Written 2026-10-23. Continue here with Claude CLI. Read this first, then `mame/TODO.md`
(open items), `mame/REVIEW_BRIEF.md` (what each PR changes and what it rests on) and
`mame/LOG.md` (long background, findings, how to reproduce). The WEGA toolchain work is a
separate topic: `WEGA/toolchain/HANDOFF_wega_disasm.md`.

## What this project is

Emulation of the EAW P8000 in MAME: `p8000` (8 bit board, UDOS) and `p8000_16` (16 bit board,
Z8001 + three Z8010, WEGA boots). Driver `src/mame/ddr/p8k.cpp` (+ `p8k_wdc.*`, `p8k_u130.*`,
and new `p8k_term.*`). Everything was done with Claude Sonnet 5.5; Oliver Lehmann (@OlliL) reviews
it and ran all real-machine tests on his P8000 (16 bit board, 4 MiB).

## Repositories and branches

| What | Where |
|---|---|
| MAME fork (push here) | https://github.com/OlliL/mame, remote `olli`; checkout `/home/claude/mamedev/mame` |
| mamedev (read only, **never push**) | remote `origin` |
| Docs, tests, tools | https://github.com/OlliL/P8000, branch `master`, sparse checkout `/home/claude/olli/P8000`; commit with `git add --sparse -A mame WEGA/toolchain` from the repo root |

Branches in the fork:

- `p8k-clean` (head `03e70d5c`): cleaned series on top of mamedev `master` of 2026-10-13
  (`f26abf2b`). Source for the PRs. Includes the PR drafts commits (to be dropped).
- `p8k-noopt`: reduced series without the optional commits (used to prove the driver does not
  depend on the optional z8000/z80pio/z8010 changes).
- `p8k-firmware-selftest`: old working branch (85 commits), backup only.
- Tag `backup-before-author-rewrite` (local): state before the commit author was changed.

Commit rules: author `Oliver Lehmann <lehmann@ans-netz.de>`, trailers
`Co-Authored-By: Claude Sonnet 5.5 <noreply@anthropic.com>` and `Claude-Session: <url>`; code
commits also `AI disclosure: assisted by Claude Sonnet 5.5`.

## Commit series on `p8k-clean` (oldest first)

```
436f255b cpu/z8000: Fixed DAB after an addition for bytes 0xFA-0xFF
d613b434 cpu/z8000: PC offset wraps at FFFF without carry into the segment
70c2b2c6 machine/z80pio: Acknowledge only enabled ports, latch a mode 3 match
fb7176fb machine/z80pio: Port A has priority over port B, drop a mode 3 request
03dffddf machine/z8010: Fixed the set-all-inhibit commands, SAR wrap, commands
9e5d3d17 machine/z80ctc: Added opt-in set_reload_on_reset()
a54429c4 machine/z80dma: Added a virtual byte_counter() hook and a derivable ...
25528403 hash/p8000_flop.xml: Added the P8000 floppy disk software list
9067870b ddr/p8k.cpp: Emulated the 16 bit board, WEGA boots
d276b808 .. b8757621  PR drafts (PULL_REQUEST_*.md) and updates of them   <- drop before submitting
a2ee8e76 hash/p8000_flop: titles, part numbers, versions and dates          <- squash into 25528403
915b2c70 cpu/z8000: block instructions restart without borrowing ...        <- squash into d613b434
abdc8896 p8k: register the remaining state for save states                  \ driver follow-ups,
72a1544e p8k: support save states                                           / squash into 9067870b
61228372 cpu/z8000: use addr_add/addr_sub for the PC fetch and block restart <- squash into d613b434
7d776fdd drafts: z8000 PR text uses addr_add/addr_sub                       <- drafts
6ee0c0b6 p8k: tidy the driver                                               <- squash into 9067870b
03e70d5c p8k: add the console terminal (p8k_term.*, new, by OlliL)           <- see "Open questions"
```

Check with `git log --oneline f26abf2b..p8k-clean`; the exact list may differ slightly.

## What was done (state 2026-10-23)

- Driver for both boards; WEGA boots (1 and 8 MiB), UDOS boots; floppy software list
  `hash/p8000_flop.xml` (all sets verify, titles from the disk labels).
- Fixes in shared devices with manual references: Z8000 DAB, PC offset wrap at FFFF,
  block-instruction restart (`addr_add`/`addr_sub` after a review comment), Z80 PIO
  acknowledge/priority, Z8010 four corrections, Z80 CTC opt-in reload, Z80 DMA byte-counter hook.
- Real-machine tests 01 to 06 equal the emulator (`mame/tests/`).
- Save states: all members registered, tested in WEGA, `MACHINE_SUPPORTS_SAVE` set.
- Tidy items: private DMA device type, finder-based RS232 callbacks, `memory_share_creator`
  for `m_dram8`, `board16_reset()` without child resets. The daisy device stays public
  (`DEFINE_DEVICE_TYPE_PRIVATE` needs a concrete exposed base).
- Comparison with the Zilog S8000 and SADIE regression run (`mame/S8000_COMPARISON.md`, LOG.md);
  the S8000 HPCPU (SCC) tests cannot run with the existing loader.
- Console terminal with its own firmware (U882/Z8, i8275, ZG0/ZG1) added by the author in
  `03e70d5c`; it is not yet covered by the PR drafts, the review brief or LOG.md.
- Tools: `mame/tools/wegafs.py` (unpack WEGA file systems from raw floppy/HD images),
  `mame/tools/wega_install.py` (installs the files of the w31 floppies by their `CONTENTS`
  with `install`), generators and Lua/shell test helpers (see LOG.md section 14).
- CI of the earlier fork PR #2 was green (Linux, macOS, Windows, include guards, XML/JSON);
  it has to be run again for the final branches.

## Next steps

### 1. Review the terminal commit first (new, not reviewed)

`03e70d5c` adds `p8k_term.cpp/.h` (480 lines) and changes `p8k.cpp`. Before it goes anywhere:

- Read it completely and run the `mame-review` skill on it (see 3).
- Check ROM entries (`p8t_1_5.0`, `p8tezs`, `p8tdzs`, CRC/SHA1) against the real ROMs and the
  ROM naming in the driver; the ROM set must be listed in the ROM definitions of `p8000`/`p8000_16`.
- Decide: part of PR A, or its own commit/PR after the driver? Keep the driver PR small if possible.
- Terminal key changes (RESET = Ctrl+Pause, NMI = Pause instead of F12) must be in the PR text.
- Save state of the terminal, `-rs232 terminal` vs generic terminal, build for the `p8000`
  subtarget without other drivers (the commit added `z8.h`/`i8275.h` includes for that reason).
- Update `PULL_REQUEST_p8k.md`, `REVIEW_BRIEF.md`, `LOG.md` and `TODO.md` for it.

### 2. Read the four PR drafts and verify every statement

`PULL_REQUEST_p8k.md`, `_z8000.md`, `_z80pio.md`, `_z8010.md` in the MAME checkout root
(on `p8k-clean`). Rules for the texts: English, first person "I", short and plain, the word
"Robotron" is not used, no references to other emulators, no links to ROM or software
downloads, reasons rest on official documents or measurements only, AI disclosure with model and
version stays. "I, @OlliL, reviewed it myself" must be true when the text is sent. A review by a
Z8001 specialist is **not** part of the plan (the author's wish).

### 3. Run the MAME review model (`mame-review` skill)

The skill is a MAME code review based on MAME's coding standards (not an official MAMEdev tool;
passing it does not guarantee acceptance). A copy is installed at `/root/.claude/skills/mame-review`
(files `SKILL.md`, `RULES.md`, `STYLE.md`, `SYSTEMS.md`, `DATAFILES.md`, `SUBMISSION.md`,
`README.md`, BSD-3-Clause). For Claude CLI on another machine copy that folder to
`~/.claude/skills/`. Then, in the MAME checkout on `p8k-clean`:

```
/mame-review f26abf2b..HEAD
```

(range = the whole series; also possible per commit range or `/mame-review <path>`). The skill
runs the mechanical checks (include guards `scripts/build/check_include_guards.py`, `xmllint` on
`hash/p8000_flop.xml`, `srcclean` after `make TOOLS=1`) and reports Blocking / Tidy / Nits /
Submission. Do it twice: now on the current branch (finds problems early), and again on the
final PR branches after the split (step 4). Fix findings in fixup commits and squash.

### 4. Make clean branches with clean commits

1. `git fetch origin`; check that `src/devices/cpu/z8000`, `machine/z80pio.*`, `z80ctc.*`,
   `z80dma.*`, `z8010.*`, `hash/p8000_flop.xml`, `src/mame/ddr/p8k*`, `mame.lst` were not changed
   upstream since `f26abf2b` (state 2026-10-16: not changed; mamedev `master` is now 236 commits
   ahead, recheck). A test rebase was clean earlier.
2. Rebase `p8k-clean` onto current `origin/master` (`git rebase -i`, no interactive flags in the
   agent: use `GIT_SEQUENCE_EDITOR` or `git rebase --onto` with `git cherry-pick`/`git commit --fixup`+`--autosquash`).
   Squash as listed above (`915b2c70` and `61228372` into `d613b434`; `a2ee8e76` into
   `25528403`; save-state and tidy commits into the driver commit), drop all drafts commits.
   Keep the author and trailers; the commit message should still be correct after squashing
   (mention addr_add/addr_sub, save states).
3. Make the PR branches from `origin/master`, each independent:
   - `pr-p8k` (A): DAB fix, z80pio required part, z80ctc, z80dma, software list, driver (and the terminal if decided).
   - `pr-z8000-pcwrap` (B), `pr-z80pio-priority` (C), `pr-z8010` (D): optional parts, one commit each.
   - The driver branch must build, validate and boot without B/C/D (prove with `p8k-noopt`).
4. For each branch: full build, `DEBUG=1` build, `-validate`, `srcclean`, clang build if possible,
   boot WEGA and UDOS, test 06 for B (`mame/tools/run_blk06.sh`); push to the fork and wait for CI
   (Linux, macOS, Windows, include guards, XML) before opening a PR. Never push to `origin`.
5. PR texts go in the PR body, not in the repository; the last line of the body is the attribution
   block of the session. Open the PRs (driver first or independent) only when the author says so.

### 5. Smaller open items

- Optional, author only: the two `w31doc` disks in WEGA; Z8010 SWW and Z80 PIO port priority
  tests; someone with ROMs boots m20/m40/c900 (Z8010 users) with the reduced series.
- Optional wiki page "how to install WEGA on the P8000" (`wegafs.py`, `wega_install.py` help).
- Optional: S8000 HPCPU loader (not planned).
- Keep `mame/LOG.md` and `mame/TODO.md` current when something is found or decided.

## How to build and test (this machine)

```
/home/claude/runs/build.sh              # p8k only, optimised ("make SOURCES=src/mame/ddr/p8k.cpp ... OPTIMIZE=1")
/home/claude/runs/build_s8k.sh <name>   # s8k build; add REGENIE=1 when the source list changes
```

The scripts live in `/home/claude/runs` (not in the repo); on another machine use the plain
`make SOURCES=src/mame/ddr/p8k.cpp REGENIE=1 -j$(nproc)` (plus `DEBUG=1` for the debug build),
`./mame -validate p8000_16` (and `p8000`). ROMs and disk images
are not in the repository; paths and details are in LOG.md sections 8 and 14. WEGA hard disk
image used for tests: `/home/claude/sdimg/sd.hd` (use a copy); raw floppy images in
`/home/claude/s8k/raw/`.
Console input to a running WEGA via `mame/tools/console.py` (fifo): prefix `P:` for even parity,
`\r` for Return.

## Pitfalls seen so far

- `pkill -f <script>` can kill the own shell: use `pkill -x` or PIDs. Old MAME processes keep
  running after tests and eat CPU; kill them.
- Source list changes need `REGENIE=1`, otherwise a stale project compiles old files.
- `DEFINE_DEVICE_TYPE_PRIVATE` needs a concrete exposed base class (works for DMA, not for the daisy device).
- Run git for the P8000 doc repo from its root, not from a subdirectory.
- Never reproduce ROM or software download links in PR texts or documents.
