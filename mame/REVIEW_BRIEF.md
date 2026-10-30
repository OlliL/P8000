# Review brief: P8000 changes for MAME

Purpose: a guide for someone who reviews the changes before they go to MAME. It says, per
planned pull request, what changed, which documents the change rests on, which real-machine
test supports it, and where the author is least sure. The long background is in
[LOG.md](LOG.md), the open work in [TODO.md](TODO.md).

Everything is in the fork https://github.com/OlliL/mame, branch
[`p8k-clean`](https://github.com/OlliL/mame/tree/p8k-clean), on top of mamedev `master` of
2026-10-13 (`f26abf2b`). The commit links below stay valid until the branch is rebased; the
subject lines identify the commits afterwards. The machines are the P8000 8 bit board
(`p8000`) and the 16 bit board (`p8000_16`, Z8001 + three Z8010).

All changes were made with the help of Claude Sonnet 5.5, Claude Haiku 5.5 and Claude Opus 5.5
(Anthropic) and reviewed by Oliver Lehmann (@OlliL), who also ran the tests on his real P8000
(16 bit board, 4 MiB RAM).

## Planned pull requests

| PR | Content | Commits |
|---|---|---|
| A. Driver | `ddr/p8k.cpp` (+ WDC and clock module), software list, and the shared-device changes the driver needs | DAB, z80pio acknowledge/match, z80ctc opt-in, z80dma hook, software list, driver |
| B. cpu/z8000 | PC offset wraps at FFFF (optional, WEGA boots without it) | 1 commit |
| C. machine/z80pio | port priority, mode 3 after RETI (optional) | 1 commit |
| D. machine/z8010 | four corrections (optional) | 1 commit |

B, C and D start from mamedev `master`, not from the driver branch. Which commit of
`p8k-clean` goes into which PR, and which ones are squashed, is in the
[branch plan](#branch-plan) below. PR texts (drafts, not part of the PRs): [A](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_p8k.md),
[B](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z8000.md),
[C](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z80pio.md),
[D](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z8010.md).

Real-machine tests (photos and listings): [tests/](tests/README.md). Programs are entered
word by word in the MON16 monitor.

---

## Branch plan

`p8k-clean` is the working branch. Right before submission each PR gets its own branch from the
current mamedev `master`, built from the commits below. The table lists **every** commit of
`p8k-clean` after `f26abf2b`, oldest first; a new commit on `p8k-clean` gets its row when it is
made.

Rules (MAME has no written rule on the size of commits; these follow S-2 and S-3 of the review
rules and what core developers ask for):

- One PR is about one thing; independent core changes are separate PRs (S-3).
- Inside a PR, one commit per self-contained change, with a message that explains it without the
  diff (S-2). Each commit builds on its own.
- Follow-ups to the same change (review comments, corrections, later measurements) are squashed
  into it; no "fix" or "review comment" commits in a PR. The message of the target commit is then
  rewritten so that it describes the combined change.
- Commits that only touch `PULL_REQUEST_*.md` are dropped. A code commit that also touches a draft
  loses that hunk.

Actions: **keep** = cherry-pick as it is (it may still get a new message), **squash → X** =
cherry-pick right after X (in table order) and fold into it, **drop** = not in any PR.

| Commit | Subject | PR | Action |
|---|---|---|---|
| `436f255b` | cpu/z8000: Fixed DAB after an addition for bytes 0xFA-0xFF | A | keep; new message for addition and subtraction, citing the Zilog manual p. 6-43 instead of "U8000 description p. 68" |
| `d613b434` | cpu/z8000: PC offset wraps at FFFF without carry into the segment | B | keep; message names the block restart too |
| `70c2b2c6` | machine/z80pio: Acknowledge only enabled ports, latch a mode 3 match during service | A | keep |
| `fb7176fb` | machine/z80pio: Port A has priority over port B, drop a mode 3 request after RETI | C | keep |
| `03dffddf` | machine/z8010: Fixed the set-all-inhibit commands, SAR wrap, command %10 and SWW | D | keep |
| `9e5d3d17` | machine/z80ctc: Added opt-in set_reload_on_reset() | A | keep |
| `a54429c4` | machine/z80dma: Added a virtual byte_counter() hook and a derivable ua858d_device | A | keep |
| `25528403` | hash/p8000_flop.xml: Added the P8000 floppy disk software list | A | keep |
| `01843ae3` | ddr/p8k.cpp: Emulated the 16 bit board, WEGA boots | A | keep; message checked against the follow-ups |
| `dc61ba7d` | PR drafts (not part of the pull requests, drop before submitting) | - | drop |
| `72898eda` | PR draft p8k: where the ROMs come from | - | drop |
| `eabc0946` | PR draft p8k: name both monitors of p8000_16 | - | drop |
| `5b8eff36` | PR draft z8010: measured on the real machine | - | drop |
| `a8cd9d29` | PR draft z8010: SAR wrap measured on the real machine | - | drop |
| `ec998467` | PR draft z8010: command %10 measured on the real machine | - | drop |
| `cbecad5a` | PR draft z8000: PC wrap measured on the real machine | - | drop |
| `9b340f14` | cpu/z8000: block instructions restart without borrowing from the segment | B | squash → `d613b434`, without its `PULL_REQUEST_z8000.md` hunk |
| `65456041` | PR draft z8000: add manual references for the PC wrap and the block instruction restart | - | drop |
| `d819729a` | PR draft z8000: block instruction restart confirmed on the real machine | - | drop |
| `21945c9e` | hash/p8000_flop: titles, part numbers, versions and dates from the disk labels and READMEs | A | squash → `25528403` |
| `163a16d3` | p8k: register the remaining state for save states | A | squash → `01843ae3` |
| `8f9fdf0b` | p8k: support save states | A | squash → `01843ae3` |
| `2e507dc1` | cpu/z8000: use addr_add/addr_sub for the PC fetch and block restart | B | squash → `d613b434` |
| `84ec61a1` | drafts: z8000 PR text uses addr_add/addr_sub | - | drop |
| `967f0eca` | p8k: tidy the driver | A | squash → `01843ae3` |
| `431de339` | PR draft p8k: console terminal, its ROMs and keys | - | drop |
| `0c82593f` | p8k: fix company string | A | squash → `01843ae3` |
| `04154ab3` | p8k: pass the address through when no MMU drives the bus | A | squash → `01843ae3` |
| `91eee0c5` | p8k terminal: caret and apostrophe keys follow the German layout | A | squash → `01843ae3` |
| `151b55cc` | p8k: A8-A23 keep their last value when no MMU drives the bus | A | squash → `01843ae3` |
| `dff42e83` | PR draft p8k: all models in the disclosure, bus hold, sa.diags | - | drop |
| `6f93ea28` | cpu/z8000: DAB after a subtraction judges the upper digit by the original byte | A | squash → `436f255b` |

Resulting branches (names are a proposal):

| Branch | Commits in order |
|---|---|
| `p8k-pr-driver` (A) | DAB, z80pio acknowledge/match, z80ctc, z80dma, software list, driver |
| `p8k-pr-z8000` (B) | PC wrap with block restart |
| `p8k-pr-z80pio` (C) | port priority, mode 3 after RETI |
| `p8k-pr-z8010` (D) | Z8010 corrections |

Checked 2026-10-30 on `f26abf2b` in a scratch worktree: B (without the draft hunk), C and D apply
without conflicts (B then changes only `z8000.cpp` and `z8000ops.hxx`). Not yet built on their own.
C applies to `master` without A's z80pio commit; if A is merged first, C is rebased on it.

---

## PR B: cpu/z8000, PC offset wraps at FFFF

Commit [`d613b434`](https://github.com/OlliL/mame/commit/d613b434) in
`src/devices/cpu/z8000/z8000.cpp` (6 places that advanced the PC with `m_pc += 2`).

- **Change:** `m_pc = (m_pc & 0xffff0000) | ((m_pc + 2) & 0xffff)`. A word fetched at
  offset `FFFE` is followed by offset `0000` of the same segment; before, the 23 bit PC
  carried into the segment number.
- **Documents:** U8000 CPU description 3.4.2 (instructions and multi-byte data cannot cross a
  segment boundary), 5.5.5/5.5.7/5.5.8 (indexed, base, relative offsets wrap, already so in
  the core); Moore, Z8000 Handbook ch. 2 p. 19 (PC wraps FFFF -> 0000, no carry).
- **Additional manual references (found 2026-10-16):** Z8001/Z8002 CPU manual (SGS-Thomson edition) p. 65 (indexed address: overflow is ignored, it neither sets the overflow flag nor increments the segment number), p. 66 (relative addresses stay in the segment of the instruction because the segment number is unchanged); U8000 description p. 59 (same statement for indexed addresses). No manual states the sentence "the PC wraps at FFFF" literally; the claim follows from these statements and from Moore p. 19.
- **Evidence:** [test 05](tests/05-z8000-pc-wrap-ffff/README.md) on the real machine: an
  instruction at `<<1>>%FFFE` is followed by the word at `<<1>>%0000` (frame
  `7F21 D000 8100 0002`); the old core continues in segment 2 (`7F22 D000 8200 0002`).
- **Block instructions:** the repeating block instructions restart themselves with
  `m_pc -= 4` (32 places in `z8000ops.hxx`). After a wrapped fetch (instruction ending at
  offset `FFFC`) this borrowed from the segment number. Fixed in commits
  [`9b340f14`](https://github.com/OlliL/mame/commit/9b340f14) and
  [`2e507dc1`](https://github.com/OlliL/mame/commit/2e507dc1) with `addr_sub(m_pc, 4)`
  (to be squashed into `d613b434`; the fetch uses `addr_add(m_pc, 2)`, as suggested in a review comment). Checked on the emulator with an `ldir`
  at `<<1>>%FFFC` (old: leaves segment 1 after one round; new: copies all words). Measured on the
  real machine too: [test 06](tests/06-z8000-block-instruction-restart/README.md) (`7F21 D050 8200 0002`, words `A001 A002 A003`, equal to the emulator).
  Manual support: if an interruptible instruction such as `ldir` is interrupted, the saved PC is the address
  of that instruction (U8000 description table 6.1 note 2, p. 68; Z8001/Z8002 CPU manual p. 71 note 2), so a
  repeat has to start at the same segment and offset.
- **Audit:** the other PC updates (JR, CALR, DJNZ, relative loads, indexed/base addressing, `set_pc*`,
  return addresses, traps) already keep the segment; see `LOG.md`. A `jr` at `<<1>>%FFFE` stays in the segment (emulator).
- **Please check:** whether a real Z8001 behaves the same in non-segmented mode (the segment comes from the PC),
  and the unrelated observation that non-segmented `IRET`/`LDPS` load the PC with `m_pc = POPW(SP)`, which clears
  the segment number.

## PR C: machine/z80pio (optional part)

Commit [`fb7176fb`](https://github.com/OlliL/mame/commit/fb7176fb) in
`src/devices/machine/z80pio.cpp`.

- **Change 1, priority:** a port under service blocked the other port in both directions.
  Now only port A in service blocks port B; port B in service lets a port A request through.
  `z80daisy_irq_state()` and `check_interrupts()` stop at the first port under service.
- **Change 2, mode 3:** `z80daisy_irq_reti()` drops a pending mode 3 request when the logic
  condition is no longer met after RETI.
- **Documents:** Z80 PIO Technical Manual 2.0 p. 4 ("Within a PIO port A interrupts have
  higher priority than those of Port B", "Higher priority devices may interrupt the
  servicing of lower priority devices"); technical description of the U855 D (the Z80 PIO
  compatible circuit) 5.3 p. 16, 7.1 p. 25/26, picture 13.
- **Evidence:** none on the real machine (hard to provoke). WEGA boots with and without it.
- **Less sure:** the U855 D description is for the East German clone, not the Zilog part. The
  mode 3 rule after RETI is taken from it.

## PR D: machine/z8010 (optional part)

Commit [`03dffddf`](https://github.com/OlliL/mame/commit/03dffddf) in
`src/devices/machine/z8010.cpp`.

| Item | Change | Documents | Real-machine test |
|---|---|---|---|
| Commands %15/%16 | always set the CPU/DMA-inhibit flag in all descriptors, data byte ignored | U8010 6.2.21/6.2.22; System 8000 Hardware Reference Manual table 3-2; Moore table 5-3 | [test 02](tests/02-z8010-set-all-inhibit-flags/README.md): `04 04 10 10` |
| SAR | wraps 63 -> 0 on auto increment | U8010 6.2.13, 6.2.14, 6.2.16; Moore p. 355, 358 | [test 03](tests/03-z8010-sar-wrap/README.md): SAR `00` |
| Command %10 | resets MR, VTR, DSCR, SAR untouched | U8010 6.2.17, table 6.2 | [test 04](tests/04-z8010-reset-command-10/README.md): MR, DSC `00`, SAR `07` |
| SWW | only for a write to the system stack (status %9, system mode) while a flag is set; other write warnings in that state are FATL | U8010 5.2 p. 34 | none |

- **Less sure:** the SWW item. It follows U8010 5.2 and the examples on p. 35/37/38; the
  figure in Leventhal p. 13-6 reads differently and the U8010 description was followed. It needs
  translation and a deliberate violation to test on the machine. VTR is not covered by test 04
  (it cannot be set without a violation).

---

## PR A: driver and the shared-device changes it needs

### DAB fix (cpu/z8000)

Commits [`436f255b`](https://github.com/OlliL/mame/commit/436f255b) (addition) and
[`6f93ea28`](https://github.com/OlliL/mame/commit/6f93ea28) (subtraction, to be squashed into
`436f255b`), `src/devices/cpu/z8000/z8000ops.hxx`, function `ZB0_dddd_0000`.

- **Change:** the upper digit is judged from the original byte (`orig > 0x99`), not from the
  byte after the low-nibble correction: `highfix = C || orig > 0x99` for both directions. After
  an addition the `+6` overflows `0xFA`-`0xFF` and hid the need for the `+0x60`; after a
  subtraction the `-6` borrows out of `0x00`-`0x05` (H set), which faked an upper digit F, and
  brings `0x9A`-`0xA5` down to an upper 9, which hid the needed `-0x60`.
- **Why:** WEGA's C library converts longs to decimal with a BCD loop; `expr 13000 + 3000`
  returned 0 and numbers around 16000...16260 and 100000 were printed wrongly. `sysgen`
  wrote a wrong default boot path because of it.
- **Documents:** Zilog Z8000 CPU Technical Manual (January 1983), DAB, p. 6-43; Leventhal,
  Z8000 Assembly Language Programming table 3-12 (p. 3-168); Moore, Z8000 Handbook table 2-8
  (p. 133).
- **Evidence:** all 764 byte/flag combinations the table covers were compared with the code;
  the subtraction change leaves all of them as they were. Combinations outside the table are
  undefined in all three sources. For the subtraction cases outside the table, @tpaxia
  compared the core with test cases captured on a real Z8001 (`0x00`-`0x05` with H=1:
  `0xFA`-`0xFF` C=0; `0x9A`-`0xA5` with H=1 and `0x9A`-`0x9F` with H=0: `0x34`-`0x3F` C=1);
  with `6f93ea28` all of them match, the addition path matched already. Flags: Z and S from
  the result, C set when the upper correction was applied, P/V untouched.
- **Please check:** the carry rule, and the cases with H set and a high digit 9.

### z80pio, required part

Commit [`70c2b2c6`](https://github.com/OlliL/mame/commit/70c2b2c6).

- `z80daisy_irq_ack()` acknowledges only when `m_ie && m_ip` (two lines). The CPU got the
  vector of a port that never asked (mode 3 match or strobe while the enable flip-flop is
  clear).
- A mode 3 match is latched even while the port is in service (`!m_ius` removed from the
  edge detection). The 16 bit board signals an interrupt to the 8 bit board this way; one
  lost edge hung the WEGA kernel.
- **Documents:** Z80 PIO Technical Manual 4.3 and 5.3. The manual says nothing about a new
  condition on a port in service, so the second change rests on the edge-triggered rule
  (5.3, false to true) and on the failure seen.

### z80ctc, opt-in `set_reload_on_reset()`

Commit [`9e5d3d17`](https://github.com/OlliL/mame/commit/9e5d3d17). Default off, so other
drivers are unchanged. After a software reset the counter is reloaded continuously, so a read
returns the time constant. The data sheets are silent; the Zilog Z80-CTC Technical Manual
circuit description explains it (p. 37, p. 44, fig. 4-15). The P8000 monitor self test needs it.

### z80dma, `byte_counter()` hook

Commit [`a54429c4`](https://github.com/OlliL/mame/commit/a54429c4). `byte_counter()` is virtual
and `ua858d_device` has a protected constructor; no change for existing users. The UA858D
reads back one count less than the bytes transferred (U858D description section 4 p. 14,
tables 1 and 2 p. 15); the quirk lives in the driver.

### Software list

Commit [`25528403`](https://github.com/OlliL/mame/commit/25528403), `hash/p8000_flop.xml`
(UDOS, OS/M and WEGA floppies). Only `xmllint --valid` has been run so far.

### Driver

Commit [`01843ae3`](https://github.com/OlliL/mame/commit/01843ae3) (plus the follow-ups listed in the
[branch plan](#branch-plan)): `src/mame/ddr/p8k.cpp`
(about 1800 lines, replaces `skeleton/p8k.cpp`), `p8k_wdc.*` (Winchester controller model),
`p8k_u130.*` (clock module), `mame.lst`. Modelled on
`src/devices/bus/zbi/s8k_cpu.cpp` (System 8000 CPU board). Parts that are specific to the
Z8001/Z8010 side:

- **MMU selection** (`select_code_mmu()`, `select_data_mmu()` in `p8k.cpp`): system mode ->
  code MMU; normal mode, segmented user: segments 0-63 data MMU, 64-127 stack MMU;
  non-segmented user: code fetch code MMU, data/stack by comparing address bits 8-15 with
  the NBR (`seg_offs < nbr` -> data MMU, equal and above -> stack MMU). The equality case
  comes from the original circuit diagram of the 16 bit board (Ms 889255-2, sheet 5: two
  74S85, only `A>B` is used), not from a measurement; see [LOG.md](LOG.md) section 6.
- **MMU commands** (`mmu_cmd_r/w`): special I/O, address bits 1-3 select the MMU (active
  low), bits 8-15 are the command/register.
- **Trap frame:** bit 15 of the saved PC segment word is saved and restored as the PC holds
  it; measured on the real machine ([test 01](tests/01-saved-pc-segment-bit15/README.md)),
  no core change needed.
- **Single step:** CTC0 channel 3 counts stack accesses; the interrupt request reaches the
  CPU only after the next instruction has started.
- **Memory:** 1 MiB default, up to 8 MiB; segment 127 is left out like on the real
  16 MiB card.
- **No MMU driving the bus** (`translate_addr()`): with SCR `ONH` set and the selected MMU in a mode
  where it does not drive A8-A23 (MSEN = 0), the address is A8-A23 of the last memory-bus cycle
  plus A0-A7 from the CPU. Measured on the real machine
  ([test 07](tests/07-p8000-16-fetch-with-mmu-off/README.md), run 8: a fetch at `%9100` read
  `%9000`); sa.diags `CODE_TRAP` needs it. Commits `04154ab` (pass-through, wrong) and `151b55cc`
  (this model) are to be squashed into the driver commit. **Less sure:** that the CPU address is
  held as well while `ONH` is clear, and whether the held value decays over time.

**Known open point in the driver:** the daisy chain device stays outside the anonymous
namespace (`DEFINE_DEVICE_TYPE_PRIVATE` needs a concrete exposed base class).

### Console terminal (part of the driver commit)

`src/mame/ddr/p8k_term.cpp/.h` (new, about 480 lines) and a few lines in `p8k.cpp`. The
terminal of the P8000 runs its own firmware: U882 (Z8, `UB8830D` core) at 7.3728 MHz, i8275
CRTC at 12 MHz / 8 with DMA from the refresh memory, two character generators (ZG0 ASCII,
ZG1 umlauts, selected by a latch), IBM XT keyboard (scancode set 1, shifted into port 2),
bell. It is a private RS-232 port device (`DEFINE_DEVICE_TYPE_PRIVATE`, slot option `adm31`) and
the default of the `rs232` slot of both machines; `-rs232 terminal` still gives the generic one.

- **ROMs** (device romset `p8k_terminal`, verified 2026-10-23 against the files supplied by the
  owner): `p8t_1_5.0` 4096 bytes CRC `1bd93470` SHA1 `436a9441...`, `p8tezs` (ZG0) 2048 bytes
  CRC `f9321251`, `p8tdzs` (ZG1) 2048 bytes CRC `32736503`. The names are the EAW file names
  in lower case (`P8T_1_5.0`, `P8TEZS`, `P8TDZS`); the UDOS floppy carries `P8T_1_4.1` and
  `P8TDZS`. Without these files the machines do not start with the default console
  (`-rs232 terminal` does). Where the owner got `P8T_1_5.0` has to be stated in the PR if asked.
- **Keys:** the terminal takes every key while its keyboard is active (F3 is its VIDEO key).
  The RESET button of the computer is therefore Ctrl+Pause (`schedule_soft_reset()` from
  `nmi_button`, which polls the Ctrl keys with `input_manager::code_pressed`) and the NMI
  button moved from F12 to Pause. The right shift key sends the left shift code (the
  firmware knows only that one), Backspace sends BS, the key left of `1` sends `^` and `#`
  with shift sends `'` (German layout, as the firmware table has them).
- **Timing:** the keyboard delivers nothing for 100 ms after reset, because the firmware
  enables its interrupts later and otherwise ignores the host.
- **Save state:** key queue (ring buffer), refresh memory and all other state are registered.
- **Reviewed 2026-10-23** with the `mame-review` rules: one Blocking (key queue was a
  `std::vector`, not valid in a save state) and several Tidy (in-class initialisers with an
  out-of-line constructor, `XTAL()` instead of literals, `static const` table, a helper at
  global scope) fixed in the driver commit. Left as is: polling `code_pressed` in `nmi_button`
  (a combined key sequence on a second port field would avoid it), `m_conf` read per character
  cell in `display_pixels`, no `PORT_CHAR` on the keys (terminal key matrix, no natural keyboard).
- **Tested:** `p8000` and `p8000_16` show the firmware banner and the monitor prompt, typed
  keys reach the monitor, save and load work. Not tested by me: Ctrl+Pause and Pause on a
  real keyboard, colour setting, bell, VT100 mode, a WEGA session on the new terminal.
- **Please check:** the hardware description in the file header (taken from the firmware
  sources), the DMA/DRQ handling and the screen timing (`set_raw`, 12 MHz, 102 x 8 by 338 lines).

Save states: all driver state is registered and `MACHINE_SUPPORTS_SAVE` is set; save and load were checked in WEGA (at the shell) and on the 8 bit board. Hard disk contents are not part of a state (as in MAME generally).

## What was tested

- WEGA installation on an empty disk and boot to multi-user (1, 4, 8 MiB in the emulator),
  UDOS on the 8 bit board, with and without PRs B, C, D.
- `DEBUG=1` and clang builds, `-validate`, include guards, CI in the fork (Linux, macOS,
  Windows) green.
- Other drivers that use the changed devices (m20, c900, wicat, ...) were only built and
  validated, not started (no ROMs).
