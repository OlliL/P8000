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

## 0. New: console terminal (commit `03e70d5c`, by the author)

- [x] Reviewed with the `mame-review` rules, findings fixed (`2c89a5a0`), ROM CRC/SHA1 checked against the
      owner's files, PR draft A, review brief and LOG updated (2026-10-23).
- [x] Decided 2026-10-23: the terminal is part of the driver commit (squashed into it; branch history rewritten).
- [ ] Optional: test Ctrl+Pause / Pause on a real keyboard, colour setting, bell, a WEGA session on the new terminal.

## 1. Code: PC wrap follow-up

- [x] Repeating block instructions and the PC wrap: reproduced 2026-10-16 (`ldir` at
      `<<1>>%FFFC`: old core jumps to segment 0 after one round, R1=2) and fixed with
      `addr_sub(m_pc, 4)` (32 places, commits `9b340f14` and `2e507dc1`; first as `restart_insn()`,
      changed after a review comment). New core copies all 3 words and
      continues at `<<1>>%0000`. Test: `tools/run_blk.sh` + `tools/blk2.lua` (emulator only).
      Split: see the branch plan in [REVIEW_BRIEF.md](REVIEW_BRIEF.md#branch-plan).
- [x] Real-machine test 06 (`tests/06-z8000-block-instruction-restart`): measured 2026-10-16, `7F21 D050 8200 0002`, `A001 A002 A003`, as the emulator.
- [x] Manual references for the PC wrap and the block restart added to the PR draft and the brief (LOG.md).
- [x] Audit of the other PC updates (calls, jumps, `RET`, traps, `IRET`, non-segmented mode): all
      keep the segment, nothing more to change (LOG.md, audit section; emulator check of `jr` at `<<1>>%FFFE`).
- [ ] Ask the Z8001 specialist: non-segmented `IRET`/`LDPS` clear the PC segment number of a
      Z8001 (`m_pc = POPW(SP)`); is that right? Not part of this change.

- [x] DAB after a subtraction (reviewer, test cases captured on a real Z8001): commit `02f5fb03`,
      `highfix = C || orig > 0x99` for both directions; the 764 table cases unchanged, all reported
      silicon cases match (LOG.md 4.1).

## 2. Code: tidy items in the driver

- [x] `p8k_dma_device` is private now (`DEFINE_DEVICE_TYPE_PRIVATE`, exposed as `ua858d_device`).
- [ ] `p8k_16_daisy_device` stays outside the anonymous namespace: `DEFINE_DEVICE_TYPE_PRIVATE`
      needs a concrete exposed base class (with `device_t` it fails to build), and the driver needs
      its methods. Only possible by moving the daisy logic into the driver class; left as is.
- [x] RS232 ports: the callbacks use the port references instead of tag strings (slot names unchanged).
- [x] `m_dram8` is a `memory_share_creator<uint8_t>` now.
- [x] `board16_reset()` no longer resets child devices; `soft_reset_w()` (the only place that needs it) does.
- [x] Save states: members registered, tested in WEGA, `MACHINE_SUPPORTS_SAVE` set (LOG.md).
- [x] Address bus with MMU ON and no MMU driving it: bus-hold model (commit `151b55cc`, replaces
      `04154ab`), test 07 runs 1-8 on the real machine, emulator equal, sa.diags passes (LOG.md 5.5).
      Split: see the branch plan in [REVIEW_BRIEF.md](REVIEW_BRIEF.md#branch-plan).
- [ ] After the bus-hold change: MON16 hardware test and a WEGA boot to multi-user once more.
- [ ] Optional: test 07 run 9, a delay loop before the page change, to see whether the held value decays.

## 3. Data and texts

- [x] `hash/p8000_flop.xml` against the software list rules: mechanical checks pass, all sets verify,
      OS/M tested, titles/part numbers/versions/dates now from the disk labels and the READMEs the
      owner supplied (commit `caaf82fb` on `p8k-clean`, to be squashed into the software list commit).
  - [ ] Optional: run the two `w31doc` disks once (mount in WEGA, copy a packed manual).
  - [ ] Optional: the checksums printed on the labels ("Pruefsumme", COPY.DISK / COPY.TAR under UDOS)
        could prove that the images equal the originals; algorithm not known, not needed for the list.
- [x] Heading "Systems promoted to working": the skeleton had both systems as not working,
      names match the driver.
- [ ] Read all four PR drafts completely and check each statement. "I, @OlliL, reviewed it
      myself" has to be true when they are sent.
- [ ] Decision kept: no links to ROM or software downloads; only documents, schematics, source.
- [ ] Optional: wiki page "how to install WEGA on the P8000".

## 4. Upstream and review run

- [x] Check mamedev `master` for changes in `z80pio`, `z80ctc`, `z80dma`, `z8000`, `z8010`
      since `f26abf2b`: none (state 2026-10-16, `a9d405aa`); test rebase clean, build/validate/test 06 ok
      (LOG.md, last section). Repeat right before submission.
- [ ] Re-run the `mame-review` skill over the final series.

## 5. Only the author can do (real machine / own material)

- [x] EPROM labels of the 8 bit only machine: only pencil labels (`mon16...` and a CRC), no component
      identifiers on the boards; nothing to rename, the file names stay.
- [x] Real `MAXSEG` display: the original machine shows `3F` with 4 MiB and `0F` with 1 MiB, as the emulator.
- [ ] Optional: Z8010 SWW (needs a translation and a deliberate violation) and Z80 PIO port
      priority (hard to provoke).
- [ ] Optional: someone with ROMs starts m20/m40 and c900 (Z8010 users) with the reduced series.

## 6. Right before submission

- [ ] Sync with mamedev `master`, rebase `p8k-clean`.
- [ ] Make the four PR branches from `master` as the branch plan in
      [REVIEW_BRIEF.md](REVIEW_BRIEF.md#branch-plan) says (every commit of `p8k-clean` has a row
      there: keep, squash or drop); the driver branch must not depend on B, C or D. Rewrite the
      messages of the commits that get squashed into (DAB, PC wrap, driver).
- [ ] Full build, `DEBUG=1`, `-validate` and CI in the fork for each branch.
- [ ] Drop the drafts commits; `PULL_REQUEST_*.md` must not be part of any PR.
- [ ] Open the PRs, driver first or independent; AI disclosure with model and version stays.

## 7. Notes

- [ ] Keep `LOG.md` current when something is found or decided.
- [ ] Every new commit on `p8k-clean` gets its row in the branch plan of `REVIEW_BRIEF.md`.
- Test programs and scripts are in [tools/](tools/), generated by `gen.py` and
  `gen_z8010.py`, entered through MON16 `D <addr>` in upper case hex, started with `G <addr>`.
