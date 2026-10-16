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

All changes were made with the help of Claude Sonnet 5.5 (Anthropic) and reviewed by
Oliver Lehmann (@OlliL), who also ran the tests on his real P8000 (16 bit board, 4 MiB RAM).

## Planned pull requests

| PR | Content | Commits |
|---|---|---|
| A. Driver | `ddr/p8k.cpp` (+ WDC and clock module), software list, and the shared-device changes the driver needs | DAB, z80pio acknowledge/match, z80ctc opt-in, z80dma hook, software list, driver |
| B. cpu/z8000 | PC offset wraps at FFFF (optional, WEGA boots without it) | 1 commit |
| C. machine/z80pio | port priority, mode 3 after RETI (optional) | 1 commit |
| D. machine/z8010 | four corrections (optional) | 1 commit |

B, C and D start from mamedev `master`, not from the driver branch. PR texts (drafts, not
part of the PRs): [A](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_p8k.md),
[B](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z8000.md),
[C](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z80pio.md),
[D](https://github.com/OlliL/mame/blob/p8k-clean/PULL_REQUEST_z8010.md).

Real-machine tests (photos and listings): [tests/](tests/README.md). Programs are entered
word by word in the MON16 monitor.

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
- **Evidence:** [test 05](tests/05-z8000-pc-wrap-ffff/README.md) on the real machine: an
  instruction at `<<1>>%FFFE` is followed by the word at `<<1>>%0000` (frame
  `7F21 D000 8100 0002`); the old core continues in segment 2 (`7F22 D000 8200 0002`).
- **Known open issue (found while writing this brief, from reading the code, not yet
  reproduced):** the repeating block instructions restart themselves with `m_pc -= 4`
  (about 25 places in `z8000ops.hxx`). An instruction of two words that ends exactly at the
  segment end (offset `FFFC`) now leaves the PC at offset `0000` of the same segment after
  the fetch, and `m_pc -= 4` then borrows from the segment number. Before the change the
  carry and the borrow cancelled. These places need an offset-wrapping subtraction too.
  Please look at this first.
- **Please check:** other places that change the PC offset (calls, jumps, `RET`, traps,
  `IRET`, the non-segmented mode that takes the segment from the PC), and whether a real Z8001
  behaves the same in non-segmented mode.

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

Commit [`436f255b`](https://github.com/OlliL/mame/commit/436f255b),
`src/devices/cpu/z8000/z8000ops.hxx`, function `ZB0_dddd_0000`.

- **Change:** after an addition the upper digit is judged from the original byte
  (`orig > 0x99`), not from the byte after the low-nibble correction. For `0xFA`-`0xFF` that
  `+6` overflows the byte and hid the need for the `+0x60` correction. The subtraction path
  is unchanged.
- **Why:** WEGA's C library converts longs to decimal with a BCD loop; `expr 13000 + 3000`
  returned 0 and numbers around 16000...16260 and 100000 were printed wrongly. `sysgen`
  wrote a wrong default boot path because of it.
- **Documents:** U8000 CPU description, arithmetic instructions, DAB p. 68; Leventhal,
  Z8000 Assembly Language Programming table 3-12; Moore, Z8000 Handbook table 2-8.
- **Evidence:** all 764 byte/flag combinations the table covers were compared with the code.
  Combinations outside the table are undefined in all three sources. Flags: Z and S from
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

Commit [`9067870b`](https://github.com/OlliL/mame/commit/9067870b): `src/mame/ddr/p8k.cpp`
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

**Known open points in the driver** (see TODO): helper device classes outside the anonymous
namespace, RS232 slots still named by tag strings, `m_dram8` as `unique_ptr`, resets of child
devices in `board16_reset()`, save states not claimed.

## What was tested

- WEGA installation on an empty disk and boot to multi-user (1, 4, 8 MiB in the emulator),
  UDOS on the 8 bit board, with and without PRs B, C, D.
- `DEBUG=1` and clang builds, `-validate`, include guards, CI in the fork (Linux, macOS,
  Windows) green.
- Other drivers that use the changed devices (m20, c900, wicat, ...) were only built and
  validated, not started (no ROMs).
