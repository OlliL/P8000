# P8000 in MAME: final to-do list

Working list for the MAME contribution (driver `src/mame/ddr/p8k.cpp`). Background of every
point: [LOG.md](LOG.md). What a reviewer should look at: [REVIEW_BRIEF.md](REVIEW_BRIEF.md).
Status as of 2026-10-16.

Branches in the fork `OlliL/mame` (never push to `mamedev/mame`):

- `p8k-clean`: the cleaned up series (9 commits plus the drafts commits), based on mamedev
  `master` of 2026-10-13 (`f26abf2b`). Source for the pull requests.
- `p8k-firmware-selftest`: the old working branch (85 commits), kept as a backup.
- Local tag `backup-before-author-rewrite`: state before the commit author was changed.

## Done (condensed)

- CI of the fork PR #2 green (Linux, macOS, Windows, include guards, XML/JSON).
- Local tests: reduced series without the optional commits boots WEGA (1 MiB, 8 MiB) and UDOS;
  `DEBUG=1` build, clang 18, `-validate`, `srcclean`; other drivers that use the changed
  devices build and validate (not booted, no ROMs).
- Real-machine tests 01 to 05 (4 MiB board) equal the emulator: saved PC bit 15, Z8010
  `%15/%16`, SAR wrap, `%10`, PC wrap at FFFF. See [tests/](tests/README.md).

## 1. Code: PC wrap follow-up

- [x] Repeating block instructions and the PC wrap: reproduced 2026-10-16 (`ldir` at
      `<<1>>%FFFC`: old core jumps to segment 0 after one round, R1=2) and fixed with
      `restart_insn()` (32 places, commit `915b2c70`). New core copies all 3 words and
      continues at `<<1>>%0000`. Test: `tools/run_blk.sh` + `tools/blk2.lua` (emulator only).
      **At split time:** squash `915b2c70` into `d613b434`; its change to
      `PULL_REQUEST_z8000.md` belongs to the drafts commit.
- [x] Real-machine test 06 (`tests/06-z8000-block-instruction-restart`): measured 2026-10-16, `7F21 D050 8200 0002`, `A001 A002 A003`, as the emulator.
- [x] Manual references for the PC wrap and the block restart added to the PR draft and the brief (LOG.md).
- [x] Audit of the other PC updates (calls, jumps, `RET`, traps, `IRET`, non-segmented mode): all
      keep the segment, nothing more to change (LOG.md, audit section; emulator check of `jr` at `<<1>>%FFFE`).
- [ ] Ask the Z8001 specialist: non-segmented `IRET`/`LDPS` clear the PC segment number of a
      Z8001 (`m_pc = POPW(SP)`); is that right? Not part of this change.

## 2. Code: tidy items in the driver

- [ ] `p8k_16_daisy_device` and `p8k_dma_device` are outside the anonymous namespace
      (needs `DEFINE_DEVICE_TYPE_PRIVATE` with a base class visible to the driver, or move
      the logic into the driver class).
- [ ] RS232 ports (`tty0`..`tty7`, `rs232`, `u_rs232`) are connected by tag strings; use
      finders.
- [ ] `m_dram8` is a `unique_ptr` with `save_pointer`; prefer `memory_share_creator` or
      `.ram()` in the map.
- [ ] `board16_reset()` resets child devices; redundant when called from `machine_reset()`.
- [ ] Save states: decide. Either complete (coupling members) or leave
      `MACHINE_SUPPORTS_SAVE` unset and say so.

## 3. Data and texts

- [x] `hash/p8000_flop.xml` against the software list rules: mechanical checks pass, all sets
      verify, OS/M tested (LOG.md, last section). Still open, needs the owner:
  - [ ] descriptions: the labels printed on the disks (SL-6); now they are descriptive
  - [ ] years of the WEGA sets: change to `1989?` unless a source exists (SL-7)
  - [ ] origin of the images (original dumps or converted?) and, if possible, run the two
        `w31doc` disks once
- [x] Heading "Systems promoted to working": the skeleton had both systems as not working,
      names match the driver.
- [ ] Read all four PR drafts completely and check each statement. "I, @OlliL, reviewed it
      myself" has to be true when they are sent.
- [ ] Decision kept: no links to ROM or software downloads; only documents, schematics, source.
- [ ] Optional: wiki page "how to install WEGA on the P8000".

## 4. Upstream and review run

- [ ] Check mamedev `master` for changes in `z80pio`, `z80ctc`, `z80dma`, `z8000`, `z8010`
      since `f26abf2b` (conflicts, or someone fixed the same thing).
- [ ] Re-run the `mame-review` skill over the final series.

## 5. Only the author can do (real machine / own material)

- [ ] EPROM labels and board positions of the 8 bit only machine (`mon8_*_3.1_nur8bit`);
      rename the ROMs if the labels exist.
- [ ] Real `MAXSEG` display at WEGA start with 4 MiB (the emulator shows `<3F>`).
- [ ] Optional: Z8010 SWW (needs a translation and a deliberate violation) and Z80 PIO port
      priority (hard to provoke).
- [ ] Optional: someone with ROMs starts m20/m40 and c900 (Z8010 users) with the reduced series.

## 6. Right before submission

- [ ] Sync with mamedev `master`, rebase `p8k-clean`.
- [ ] Make the three PR branches (z8000, z80pio, z8010), each from `master`; the driver
      branch must not depend on them (reduced series `p8k-noopt` as the base for the check).
- [ ] Full build, `DEBUG=1`, `-validate` and CI in the fork for each branch.
- [ ] Drop the drafts commits; `PULL_REQUEST_*.md` must not be part of any PR.
- [ ] Open the PRs, driver first or independent; AI disclosure with model and version stays.

## 7. Notes

- [ ] Keep `LOG.md` current when something is found or decided.
- Test programs and scripts are in [tools/](tools/), generated by `gen.py` and
  `gen_z8010.py`, entered through MON16 `D <addr>` in upper case hex, started with `G <addr>`.
