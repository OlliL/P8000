# P8000 in MAME: work log and findings

This is the long version of everything that was done and found while bringing the
P8000 (8 bit board and 16 bit board) into current MAME, with the documents each
decision rests on. The short texts for the actual pull requests are the
`PULL_REQUEST_*.md` files on the branch `p8k-clean` of the fork. The open work is in
[TODO.md](TODO.md). Both files are meant to be extended.

Status: 2026-10-16. Driver `src/mame/ddr/p8k.cpp` with `p8k_wdc.*` and `p8k_u130.*`,
machines `p8000` and `p8000_16`, software list `hash/p8000_flop.xml`, fork
`https://github.com/OlliL/mame`, branch `p8k-clean`.

Contents:

1. Where the code is
2. The system and the emulated board revisions
3. Driver structure
4. Changes in shared devices (with sources)
5. Facts measured on the real machine
6. MMU selection and the break register (NBR)
7. WEGA installation and test results
8. ROMs, floppies and the UDOS file names
9. Comparison with another P8000 emulation
10. MAME contribution rules and the review
11. Pull request layout and commit history
12. Fork, CI and GitHub notes
13. Known limitations and decisions
14. Tools and how to reproduce
15. Sources and links

---

## 1. Where the code is

| What | Where |
|---|---|
| Fork (push target) | `https://github.com/OlliL/mame`, remote `olli`. It is a fork of `MattisLind/mame`, not directly of mamedev. |
| mamedev | remote `origin` in the sandbox, read only, never pushed to |
| Clean series | branch `p8k-clean`, 10 commits on top of mamedev `master` `f26abf2b` (2026-10-13), plus the drafts commit |
| Old working branch | `p8k-firmware-selftest`, 85 commits, kept as backup |
| Tag | `backup-before-author-rewrite` (local), state before the author of all commits was set to Oliver Lehmann |
| Firmware, WEGA, UDOS sources and schematics | `https://github.com/OlliL/P8000` and `http://www.pofo.de/P8000/` |
| This documentation | `mame/` in `https://github.com/OlliL/P8000` |

The fork's `master` was 158 commits behind mamedev (it was last synchronised
earlier). On 2026-10-16 it was fast-forwarded to mamedev `master` (`f26abf2b`), so a pull
request inside the fork shows only our commits.

## 2. The system and the emulated board revisions

The P8000 was built by VEB Elektro-Apparate-Werke Berlin-Treptow (EAW), GDR.

- 8 bit board: U880 (Z80) at 4 MHz, floppy controller (i8272), DMA (UA858D), 64 KiB
  DRAM, 8 KiB monitor EPROM, SIOs, CTCs, PIOs, two 5.25" drives. OS/M (CP/M like)
  and UDOS (Z80-RIO like) run on it.
- 16 bit board (optional): U8001 (Z8001) with three U8010 (Z8010) MMUs for code, data
  and stack, 16 KiB EPROM (MON16), 2 KiB static RAM, two SIOs, two CTCs, three PIOs,
  memory cards. WEGA (a ZEUS clone, System III UNIX) runs on it. A separate
  Z80 based Winchester controller (WDC) hangs on PIO2 of the 16 bit board.
- The boards talk through PIOs. The 8 bit monitor starts the 16 bit CPU with `X`.

Revisions: several exist. The emulation follows the hardware manual: 8 bit board
Index 3 (`V: x3xx`, monitor MON8 3.1, chapter 4; the memory control is the same on
Index 1) and 16 bit board Index 4 (`V: 4xxx`, monitor MON16 3.1, chapter 7). An
8 bit board Index 3 or higher only works with a 16 bit board Index 4 or higher.

The original circuit diagrams of the 16 bit board (Ms 889255-2) exist as Index 1 and
Index 4 (plus the development board "GLE"); they are in the user's collection and were
read for the NBR logic (section 6).

## 3. Driver structure

`p8k.cpp` was moved from `skeleton/` to `ddr/` and `MACHINE_NOT_WORKING` removed.

### 3.1 8 bit board

- Memory control as in manual 3.3/4.3: 16 slices of 4K, each selects nothing, the
  EPROM (8 KiB, repeating), the 2 KiB static RAM (A11 not decoded: shows twice in its
  4K window, the same 2 KiB in every slice) or the 64 KiB DRAM, through the
  write-only address port at I/O 00H (the slice is the high address byte of
  `OUT (C),r`). After a reset only the EPROM is visible until a dummy access to I/O 04H
  releases the address port (reset flip-flop). Implemented with 16 `memory_view`s.
- Checked against the schematics of the 8 bit board: two 74138 decoders give the I/O
  map (00-1F, 20-3F); chips are 2 x 2732, 4 x 2114, 8 x 2164; baud rate CTC channels
  clock the SIO channels through divide by two flip-flops (`baud_w()`); the baud quartz
  is 9.832 MHz (parts list "MQ 02-9832,0k"; the earlier assumption was 9.8304 MHz)
  divided by eight = 1.229 MHz CTC trigger; interrupt chain: DMA, PIO2, CTC0, SIO0,
  SIO1, PIO0, PIO1, CTC1.
- Floppy wiring: PIO2 port A bits 0-3 motor of drive 0-3 (drives 0 and 1 modelled),
  bit 4 clock/data rate (1 = 4 MHz / 250 kbit/s 5.25", 0 = 8 MHz / 500 kbit/s 8"),
  bit 5 head load (not modelled), bit 6 TC (ORed with the DMA interrupt), bit 7 RESI
  (high after power-on reset, low after operator reset; a DIP switch "Hardware Test"
  stands in); port B bits 0-3 /DS0-3, bit 4 FDC INT, bits 5/6 write precompensation (not
  modelled), bit 7 RFDC. Three connections were missing and made every disk access
  fail: motors, FDC interrupt, and the DMA access to the FDC data register
  (`i8272a_device::dma_r/dma_w`).
- Drives: 5.25" double sided, 96 tpi (`FLOPPY_525_QD`). MAME cannot save CopyQM images,
  writes only last until the image is unloaded (use `.mfi` to keep them).
- `p8000` uses its own monitor EPROMs (`mon8_*_3.1_nur8bit`), because the normal ones try
  to start a 16 bit board. `p8000_16` uses `mon8_*_3.1`.
- The firmware's hardware test (`T` at the monitor prompt) passes.

### 3.2 16 bit board

- Written to look like `src/devices/bus/zbi/s8k_cpu.cpp` (the Zilog System 8000 CPU-A
  board): same Z8001 + 3 x Z8010 arrangement, same registers (FFC1 SCR, FFC9 SBR, FFD1
  NBR, FFE1 RETI port, FFE9 soft reset, FFF1/FFF9 segment trap, SIO/CTC/PIO from FF81),
  same function and member names. The VI interrupt handling follows `zbi.cpp`: the VI
  line forwards the state of the whole daisy chain and is re-evaluated after an
  acknowledge and after RETI.
- Differences from the s8k board: no ZBI bus (memory on the board; two SIOs, two CTCs,
  three PIOs); the OS is always segmented (only that branch of the MMU selection);
  no parity/ECC NMI, no DMA mode, no segment violation register (FFD9 is the RUN lamp);
  `soft_reset_w` resets only the 16 bit board; `nmiack_r` returns 0001 (NMI key); on-board
  RAM 2 KiB mirrored; stack accesses pulse CTC0 CLK/TRG3 for single stepping.
- On-board memory: 16 KiB EPROM 0000-3FFF, 2 KiB static RAM at 4000-47FF (four copies up
  to 5FFF), 6000-7FFF empty, main memory from 8000 while the on-board memory is active
  (SCR bit 0 switches it off).
- Main memory 1 MiB by default, `-ram 2m|4m|8m`. 8 MiB is the maximum (128 segments of
  64 KiB; 16 MiB was tried and the WEGA kernel hangs in its memory sizing loop in `main()`).
  Segment 127 (7F0000-7FFFFF) is not populated, as on the real 16 MiB memory card
  ("8 MB minus 64 KB can be used"); without the hole the MON16 memory test (step 70
  of `p.test.s`) never ends, because the segment number wraps around.
- SBR (FFC9): the manual lists it only for the development board (schematic marks it
  "GLE"), but MON16's hardware test writes it and reads it back (error 84 otherwise)
  and WEGA writes it. It is a plain register that the MMU selection ignores.
- Real-time clock: CTC1 channels 2 and 3 cascaded on 1.229 MHz (WEGA programs /256 and
  /80 = 60 Hz).
- Single step: CTC0 channel 3 counts stack memory requests (each pulses CLK/TRG3). WEGA
  arms it with time constant 17 for one instruction, MON16's `N` command counts stack
  operations the same way. The CTC expires during the `IRET` that enters the program,
  so the interrupt request must only reach the CPU after the next instruction has started:
  the VI request is delayed until the next instruction fetch (timer as fallback for a
  CPU in HALT).
- Serial ports: eight channels (WEGA `sio.h`: tty0-3 on the 8 bit board, tty4-7 on the
  16 bit board), slots `tty0`, `rs232` (= tty1, console), `tty2`, `tty3`, `tty4`,
  `u_rs232` (= tty5), `tty6`, `tty7`. Baud rates come from the CTCs (x32 mode, time
  constant 2 = 9600 baud).

### 3.3 Coupling

The 16 bit PIO0 sends to the 8 bit board (8 bit PIO0 port A in mode 1; bit 0 of port B =
INT8, bits 1-4 status). The 8 bit board sends through two output latches (P16A data,
P16B status: bit 7 strobe, bit 0 INT16) to the 16 bit PIO1. Ready lines are visible as
bits 5 and 6 of PIO0 B. 8 bit PIO0 B bit 7 holds the 16 bit board in reset. This is how
`X` starts the U8001 and how the 16 bit console output reaches the 8 bit terminal.

### 3.4 Winchester controller (`p8k_wdc`)

Software model of the protocol on PIO2 of the 16 bit board (port A data, port B bits
0-2 /ST0-/ST2, bit 5 RESET, bit 6 TE, bit 7 TR). Commands, parameter table (PAR), bad
track table (BTT) and error codes follow signal captures of a real WDC 4.2. The disk is a
`harddisk_image_device` (`-hard`). Status codes: 0 busy/idle, 1 ready for command
(9 bytes), 2 wants data, 3 offers data, 7 error. The BTT sits in the parameter sector (byte
count, up to 40 entries cylinder/head in ascending order, end mark `ff ff ff`). Tracks
found bad during formatting are entered in sorted order, bad tracks are skipped by block
and sector commands. `sa.format` on an empty 48 MB image (NEC D5126: 615 cylinders, 4
heads, 18 sectors; 2460 tracks) works, two hand-entered bad tracks give two tracks less
(44172 useful blocks) and a second `sa.format` finds the parameters valid.

### 3.5 Clock module U130 (`p8k_u130`)

Quartz clock with six digit LCD and three keys, I/O FEF1-FEFF (`timer.c` of the kernel,
`sa.timer`). FEF1 read = BF (module present), write bit 0 VT, bit 1 HT, bit 2 ST (not
used, not emulated); FEF5-FEFF the digits. The module starts with the host's UTC time;
setting time or date only changes an offset kept in the device's NVRAM file (like the
battery). Drawing: "EAW P8000 Echtzeituhr V2" (drawn by the author, may contain errors).
Not emulated: the stop watch and weekday symbols.

## 4. Changes in shared devices (with sources)

Each of these is one commit on `p8k-clean`. The reasons are in the commit messages too.
"Needed" means the P8000 does not work without it, "optional" means WEGA boots without it
(tested with 1 MiB only so far, see TODO).

### 4.1 `cpu/z8000`: DAB (needed) – commit `436f255b`

`DAB` computed the high-nibble correction from the byte after the +6 low-nibble correction.
For 0xFA-0xFF that addition overflows and hides the need for the 0x60 correction. After an
addition the high digit is now judged from the original byte. WEGA's C library converts
longs to decimal with a BCD loop: before this, `expr 13000 + 3000` returned 0, which made
`sysgen` write `md(0,0)wega` as default boot path.
Sources: U8000 CPU description, arithmetic instructions, DAB p. 68 (table of corrections
00, 06, 60, 66, FA, A0, 9A and resulting carry); Leventhal, Z8000 Assembly Language
Programming, table 3-12 (p. 3-168); Moore, Z8000 Handbook, table 2-8 (p. 133). All 764
byte/flag combinations the table covers were compared with the code: no difference. Z and S
are set from the result, P/V is not touched ("CZS---"). Combinations outside the table are
undefined in all sources.

### 4.2 `machine/z80pio`: acknowledge only enabled ports, latch mode 3 match during service (needed) – commit `70c2b2c6`

- `z80daisy_irq_ack()` acknowledges only when `m_ie && m_ip`. `m_ip` can be set while the
  enable flip-flop is cleared (mode 3 match, strobe); the CPU then got the vector of a port
  that never asked. Source: Z80 PIO Technical Manual: a pending request is only enabled
  onto the interrupt line once the enable flag is set (4.3); an interrupt is generated if
  interrupts are enabled and the mask condition is met (5.3). That the enable flag gates the
  vector is derived.
- A mode 3 match is latched even while the port is in service (`!m_ius` removed). The mode 3
  condition is edge triggered (5.3, "false to true"); if it is dropped while `m_ius` is set it
  never comes back. The 16 bit board signals INT8 to the 8 bit board this way; one lost edge
  hung the WEGA kernel in its coupling loop. Whether INT is driven is decided in
  `check_interrupts()`. The spec does not talk about a new condition on a port in service;
  this rests on the quotes and on the failure that was seen.

### 4.3 `machine/z80pio`: port A before port B, mode 3 after RETI (optional) – commit `fb7176fb`

- A port under service blocked the other port in both directions. Z80 PIO Technical
  Manual 2.0, p. 4: "Within a PIO port A interrupts have higher priority than those of Port
  B" and "Higher priority devices may interrupt the servicing of lower priority devices". Now
  `z80daisy_irq_state()` and `check_interrupts()` stop at the first port under service and
  let the ports in front through.
- A request that comes in while the channel is in service is stored, except in mode 3,
  where it is passed on only if the logic condition is still met after RETI.
  `z80daisy_irq_reti()` drops such a request. Source: Technische Beschreibung U855 D
  (Z80 PIO compatible), 5.3 p. 16 and 7.1 p. 25/26, picture 13. The same description confirms:
  a pending request stays pending while the enable flip-flop is cleared (already so), IEO
  stays low from the acknowledge until RETI.

### 4.4 `machine/z80ctc`: `set_reload_on_reset()` (needed, opt-in) – commit `9e5d3d17`

After a software reset (STOP = 1) the down counter is continuously reloaded from the time
constant, so a read returns the time constant. Default off, so other drivers are unchanged.
The data sheets are silent (Zilog spec: channel stops counting; U857D: registers unchanged,
p. 18). The Zilog Z80-CTC Technical Manual circuit description explains it: a reset sets
flip-flop FF8 which generates STOP (p. 37); the down counter is loaded when it reaches zero or
when the channel stops counting; transistors 86 through 90 generate LOAD DC whenever STOP or
TO1 = 1 (p. 44, fig. 4-15). The P8000 monitor self test (`U880EIG.S`) starts each channel,
resets it with control word 3 and reads it back, expecting the time constant.

### 4.5 `machine/z80dma`: `byte_counter()` hook (needed, no behaviour change) – commit `a54429c4`

`z80dma_device::byte_counter()` is new and virtual; `ua858d_device` got a protected
constructor with a device type. The UA858D of the P8000 reads back one count less than the
bytes transferred (override in `p8k_dma_device` in the driver). Source: U858D description:
byte counter initialised only after the first byte (section 4, p. 14); one byte more than the
block length is always transferred; in every row of tables 1 and 2 (p. 15) the counter reads
the transferred bytes minus one. The Zilog Z80 DMA spec says nothing about the counter value,
so the quirk is not put into the shared Z80 DMA. The monitor's DMA self test needs it.

### 4.6 `cpu/z8000`: PC offset wraps at FFFF (optional) – commit `d613b434`

`m_pc += 2` carried from FFFE into the segment number. U8000 CPU description 3.4.2:
instructions and multi-byte data cannot cross segment boundaries; 5.5.5, 5.5.7, 5.5.8: the
offset of indexed, base, relative addresses wraps without touching the segment (the core
already did that). Moore, Z8000 Handbook chapter 2, p. 19: the PC offset wraps from FFFF to
0000 without carry into the segment register. This also keeps the segment that a Z8001 in
non-segmented mode takes from the PC.

### 4.7 `machine/z8010` (optional) – commit `03dffddf`

1. Commands %15 and %16 (set all CPU-inhibit / DMA-inhibit attribute flags) cleared the
   flags when the data byte was 0. The data byte is not part of the command. Sources:
   ZiLOG System 8000 Hardware Reference Manual, table 3-2; Z8010 data sheet; U8010
   description 6.2.21/6.2.22 ("Der MMU müssen dazu keine Daten übergeben werden", the
   source register is chosen arbitrarily, content ignored); Moore chapter 5, table 5-3.
   WEGA's `_invsdrs` writes %15 with whatever is left in a register (seen: 00, 3E, D8).
2. SAR autoincrement wraps from 63 to 0 (U8010 6.2.13, 6.2.14, 6.2.16: "Dem Deskriptor 63
   folgt dabei der Deskriptor 0"; Moore p. 355, 358). `INC_SAR()` and `INC_DSC_SAR()`
   stayed at 63 before. P8000 software always writes the SAR first, so nothing depends on it.
3. Command %10 resets MR, VTR and DSCR to 0 (SAR not mentioned): U8010 6.2.17, table 6.2.
   The device did not know it.
4. The secondary write warning SWW only for a write to the system stack (status %9, system
   mode) while a flag is already set; any other write warning in that state is FATL (U8010
   5.2, p. 34). The device checked system mode only. (Code: `viol |= VTYPE_SWW` only for
   `sys && st == 0x9`; the `ST_REQ_STACK` status is 9.)
   Also in the descriptor logic: valid offsets of a normal segment 0 to 256*N+255, of a stack
   segment (DIRW set) 256*N to 65535, N = limit field, N = 256 - n for n blocks; the lowest
   block gives the write warning (U8010 5.4 p. 35 and examples p. 37/38; the figure in
   Leventhal p. 13-6 reads differently, the U8010 description was followed).

## 5. Facts measured on the real machine

### 5.1 Bit 15 of the saved PC segment word

The U8000 description (6.6.2, fig. 6.1, 6.2) does not say whether the "SEG" bit in front of
the segment number is pushed on a trap and what IRET does with it. A small MON16 program on a
real P8000 16 bit board: a system call handler in the program status area reads the frame
(identifier, FCW, PC segment, PC offset), flips bit 15 of the PC segment word and returns with
IRET, four times in a row. Frames (`D 9200 10 W`):

```
7F11 D000 8000 903A
7F12 D000 0000 903C
7F13 D000 8000 903E
7F14 D000 0000 9040
```

So the bit is saved as the PC holds it (the first call entered with bit 15 set) and IRET loads
it back unchanged. The emulation gives the same four frames; the core's `m_pc_b15` was
already right. This removed the earlier "known limitation".

MON16 facts used (version 3.1): start with `X` at the 8 bit monitor, then a key press, then
NMI (NMI without prior input is AUTOBOOT into WEGA). `D <addr>` shows and enters memory
interactively (word wide, upper case hex digits only, `Q` leaves; the `W` suffix is a display
option); `G <addr>` starts; the SC entry in the program status area is at PSAP+0x18 (FCW
+0x1A, PC segment +0x1C, PC offset +0x1E); `sc #1` / `sc #2` switch to segmented / non
segmented mode; `sc #0` is BREAK. The test program is in `tools/gen.py` (main 49 words at
%9000, handler 25 words at %9080).

Entered words (as typed):

```
main  (%9000): 7F01 2106 8000 7D15 A117 0107 001C 2160
               6F00 8000 9110 0D65 8000 A971 2160 6F00
               8000 9112 0D65 9080 4D05 8000 9100 8000
               4D05 8000 9102 9200 7F11 7F12 7F13 7F14
               2106 8000 7D15 A117 0107 001C 6100 8000
               9110 2F60 A971 6100 8000 9112 2F60 7F02
               7F00
handler (%9080): 97E0 97E1 97E2 97E3 5406 8000 9100 2F60
                 A971 2F61 A971 2F62 A971 2F63 A971 5D06
                 8000 9100 0902 8000 93E3 93E2 93E1 93E0
                 7B00
```

Mistakes made on the way (emulator and hardware): wrong offset (#4 instead of #$1C), wrong
opcode for `LDL DA,RR` (5F06 is LDA, the right one is 5D06), program run in non segmented mode
(fixed with `sc #1` at the start and `sc #2` before the final `sc #0`), and lower case hex
digits typed at the real machine (MON16 needs upper case).

### 5.2 Other measurements (earlier)

Run on the real machine and reproduced in the emulator: the WEGA installation on an empty
disk (all counts of the final `df -t` match block for block), the `sa.format`, `sa.verify`,
`sa.mkfs`, `sa.install`, `sysgen` sequence, the MON16 hardware test steps up to step 70.

### 5.3 Z8010 commands %15 / %16 and SAR wrap (measured 2026-10-16)

Test `tests/02-z8010-set-all-inhibit-flags`: with the attribute bytes of descriptors 5 and 63
set to 00, command %15 and later %16 with data byte 00 gave `04 04 10 10` on the real 16 bit
board, the same as the emulator after commit `03dffddf`. So the data byte is not part of
the command, as the U8010 description (6.2.21/6.2.22) says. Photo in the test directory.

Test `tests/03-z8010-sar-wrap`: with SAR = 63, reading with command 0E (attribute, then SAR+1)
and with 0A/0F (read passes the attribute byte, then SAR+1) both left SAR = `00` on the real
board (dump `1000 1010 0020`: bytes `10 00 10 10 00`, the last byte is not a result). Same as the
emulator after the fix; the old emulation stayed at 3F. Supports the SAR wrap item of commit
`03dffddf` (U8010 6.2.13, 6.2.14, 6.2.16). 
Test `tests/04-z8010-reset-command-10`: MR := 38, SAR := 7, DSCR := 2 read back as `38 07 02`; after
command %10 the real board gave MR = `00`, DSC = `00`, SAR = `07` (dump `0038 0207 0000 0702`,
the last byte is not a result). The emulator gives the same. So %10 resets MR and DSCR and leaves SAR
alone (U8010 6.2.17, table 6.2). VTR cannot be set without a violation, so it is not covered.

### 5.4 PC offset wrap at FFFF (measured 2026-10-16)

`tests/05-z8000-pc-wrap-ffff` runs a one word instruction at `<<1>>%FFFE` and lets two
different system calls sit at `<<1>>%0000` (wrap) and `<<2>>%0000` (carry). The real board
(4 MiB RAM) gave the frame `7F21 D000 8100 0002`: the next word was fetched from segment 1,
offset 0, so the offset wraps without a carry into the segment. Emulator with commit `d613b434`:
the same frame; with the commit taken out again (old core, rebuilt on 2026-10-16):
`7F22 D000 8200 0002`. So the test tells the two behaviours apart and the real machine
agrees with the change. Supports commit `d613b434` (U8000 description 3.4.2; Moore p. 19).

### 5.5 Address bus with MMU ON and no MMU driving it (measured 2026-10-26 and 2026-10-30)

`tests/07-p8000-16-fetch-with-mmu-off`. Found through sa.diags: its `CODE_TRAP` (in RAM) switches
the code MMU off with `soutb CODE_MMU,rh0` (mode %00, MSEN = 0) while SCR `ONH` is still set and
clears `ONH` with the next instruction. On the real machine this works. In the emulator the
fetch failed (`translate_addr()` returned false, `ram_r()` gave `NOP`) and the test hung. MON16
has the same `CODE_TRAP`, but in the on-board EPROM, which is decoded on the CPU bus in front of
the MMUs, so the monitor self test was never affected.

- Runs 1-3 (2026-10-26): the fetch after the mode write works. First read as "the board passes the
  address through", commit `04154ab` in the fork.
- A hobby colleague reported that the variant with a `nop` between `soutb` and `outb` fails and
  explained the working case with instruction prefetch. Fawcett, Z8000 Design Handbook ch. 5
  p. 56/57: only the next opcode is prefetched, and not after an instruction that ends with a
  memory or I/O write; Z8000 CPU Technical Manual 4.3, fig. 4-3 gives no details.
- Runs 4-7 (2026-10-30): one to three fetches between the mode write and clearing `ONH`, including
  the reported `nop` variant: all work on the real machine. Prefetch is not the reason.
- Run 8: the first fetch after the mode write is in the next 256 byte page (`%9100`, last
  MMU-driven fetch `%90FE`). The CPU executed the word at physical `%9000`. So A8-A23 keep the value
  of the last MMU-driven memory cycle (MSEN = 0 tri-states the Z8010 outputs, Fawcett ch. 9;
  sheet 8); A0-A7 come from the CPU. Runs 1-7 could not show this because all their fetches stayed
  in the held page.

Fork: commit `151b55cc` ("p8k: A8-A23 keep their last value when no MMU drives the bus") replaces
the pass-through of `04154ab`; both are to be squashed into the driver commit. The driver keeps
A8-A23 of every memory-bus cycle in `m_bus_a8_a23` (saved). That the CPU address drives the lines
while `ONH` is clear is an assumption; only the MMU-driven case is measured. Emulator results
(`tools/run07.lua`): run 7 comes through, run 8 R5 = `0000` R6 = `0001`, control 8K R5 = `0001`
R6 = `0000`, as the real machine; sa.diags passes (owner, 2026-10-30). Not tested: whether the held
value decays after a longer time.

Lesson for MON16 test programs: `G` starts with the saved user FCW, `%C000` (segmented) after a
reset, so a direct address such as `ld %9200,r5` is read as the first word of a long address.
`BREAK AT` alone does not prove that the program left the board in a sane state, because the
monitor runs from the on-board memory in front of the MMUs.

## 6. MMU selection and the break register (NBR)

For a non-segmented user in normal mode the board decides between data MMU and stack MMU by
comparing the high address byte (bits 8-15) with the Normal Break Register (port FFD1). The
emulation (`select_data_mmu()`): `seg_offs < nbr` selects the data MMU, equality and above the
stack MMU.

Evidence: original circuit diagram of the 16 bit board, Ms 889255-2, sheet 5, Index 4 (P 8000
/ 16.4, change 1.7.88) and Index 1 (P 8000 / 16.1, 4./9.11.87), same circuit on both:

- Two K531SP1P (74S85) comparators, 2D32 (upper half) and 1D32 (lower half). B = LAD15..12
  and LAD11..8, A = the NBR latch outputs (DS8282, 3D25 on Index 4, 4D25 on Index 1), which
  also drive ADIN through buffer 8D22 for reading %FFD1. Cascade inputs of 1D32: A>B = L,
  A=B = H, A<B = L. Only output Q(A>B) of 2D32 is used ("NBR > address high").
- Q(A>B) goes to D29 (74S51, AND-OR-INVERT), D28 (74S04) gives the inverse:
  `MDATA- = NOT(NOT ISNAD6 * X + Q(A>B) * Y)`, `MSTACK- = NOT(ISNAD6 * X + NOT Q(A>B) * Y)`
  with X enabling the segmented user branch (selects by SN6) and Y the non segmented user
  branch (selects by the NBR compare).
- Result for a non segmented user: address high < NBR: Q = 1, data MMU; address high =
  NBR: Q = 0, stack MMU; address high > NBR: stack MMU.

WEGA (`nsseg`, `getmem`, `putmem` in `mch.s`) compares with `jr ugt` and treats equality as
data: off by one in software, harmless, because `estabur()` (`ureg.c`) sets `u_break` to the
data limit plus one (the first block behind the data segment) and rejects `nd + ns > 255`, so
that block lies in the gap and a correct program never touches it. `sureg()`/`estabur()`:
`sg_limit = nd - 1` for data, `CPAS - ns` for the stack (CPAS = 256), `outb(0xffd1, u_break)`.
MON16 `p.test.s` steps 85-91 test NBR = %90, accesses to %8000 (data) and %A000 (stack),
NBR = %FF with %9000 (data) and NBR = %00 for the stack, but no access with address high =
NBR.

Read from the drawing, not measured on the device.

## 7. WEGA installation and test results

Procedure that was run on the real machine and repeated with the emulation (also at the top
of `p8k.cpp`). Console input needs 7E1: every character typed at the WEGA login and in the
shell must carry even parity (WEGA's getty sets up an ADM-31).

1. Create an empty image of at least 47185920 bytes (1024 cyl x 5 heads x 18 sectors x 512),
   e.g. `chdman createhd -o wega.chd -chs 1024,5,18 -ss 512` (or `truncate -s 48M wega.hd`).
   The geometry is not read from the image, `sa.format` stores it on the disk. Start
   `mame p8000_16 -flop1 w31start.cqm -hard wega.chd -rs232 null_modem -bitb socket.127.0.0.1:5620`.
   The WEGA start floppy always stays in the first drive, the installation floppies go into
   the second.
2. Return at the 8 bit monitor, then the 16 bit monitor asks "Press NMI". Do not press NMI
   (the hardware test stops with error 52, the first command to the unformatted disk fails).
   Type " O U" (any character stops the autoboot, `O U` boots the UDOS floppy). At the
   loader prompt `>` enter `boot`, at `Boot :` e.g. `ud(0,0)sa.format`.
3. `sa.format`: disk type 4 (ROB K5504.50: 1024 cylinders, 5 heads, 18 sectors), answers
   4, y, n, a, y, y, y.
4. `sa.verify`: begin cylinder 0, end cylinder 1023.
5. `sa.mkfs`: `13000` for md(0,0) (/usr) and `7000` for md(0,16000) (root).
6. `sa.install`, twice, always from the start floppy in drive 0. Date 05/30/89. Insert the
   installation floppies in drive 1 only after `sa.install` runs (a foreign disk in drive 1
   at boot time makes UDOS stop with error C4). Input `fd(1,0)`. First run: output
   md(0,16000), disks root1-root5; second run: output md(0,0), disks usr1-usr9. Answer `A`
   at the directory prompts, change the disk, `y` to "next input disk" for every further disk,
   `n` after the last.
7. Boot the kernel by hand once (`O U`, `boot`, `md(0,16000)wega`), single-user mode. Run
   `/etc/new.install`: `j`, 4000 for /dev/tmp, 60732 for /dev/z (runs for a long time). Then
   `init 2`; `datem` asks for date (05/30/89) and time ("Last known date" shows the U130
   module's time).
8. Log in as `wega` (password `root`), `cd /usr/sys/conf`, `sysgen`: n, y, 13000 /usr, 3000
   /swap, 7000 root, 4000 /tmp, 65016 /z, Return for md5, n, n, n. Then
   `mv /usr/sys/conf/wega /wega`, `mv /usr/sys/conf/boot /usr/boot`,
   `chmod 400 /wega /usr/boot`, `halt`, `sync;sync` in the single-user shell.
   The new boot program has `md(0,16000)wega` as its default (the DAB bug used to give
   `md(0,0)wega`).
9. Reset, Return at the monitor, then only NMI; after the hardware test the boot program
   starts the kernel. After the file system checks and a login: `umount /dev/z`,
   `mkfs /dev/rz 65016`, `/etc/labelit /dev/rz /z 1`, `mount /dev/z /z`,
   `/etc/makelost+found /z`, `rm -f /etc/new.install`, `df -t`.

Results:

- `p8000`: hardware test passes. UDOS system floppy boots to the `%` prompt; `STATUS`, `DATE`,
  `ECHO` and `CAT F=L P=&` work.
- `p8000_16`: `X` starts the U8001, NMI starts the 16 bit hardware test, the monitor boots
  WEGA (`md(0,16000)wega`) to multi-user, the date comes from the clock module, `ls`, `who`,
  `date` work.
- `-ram 4m`: `MAXSEG=<3F>`, 4022784 bytes user memory; `-ram 8m`: `MAXSEG=<7E>`, 8217088 bytes.
- Full login on tty0, tty1, tty2, tty5, tty6, tty7 (with getty); output on all eight ports;
  `ttyconfig -m 3` / `-t 3` switches tty3 between modem and terminal.
- Floppies: `tar` in both directions, mounting a WEGA file system floppy, `format` of an empty
  image. `sa.timer` finds the module, shows the host time and accepts a new one.
- 2026-10-16, after the review changes (include sorting, finders, constexpr, `machine_reset`):
  `p8000` and `p8000_16` pass `mame -validate`; WEGA boots to "Going multi-user" (1 MiB).
  `-ram 8m` and the reduced series (without the optional commits) were not re-run.
### 7.1 Local tests that CI does not do (2026-10-16)

- Reduced series (`p8k-noopt` = mamedev `f26abf2b` + DAB, z80pio acknowledge/match, z80ctc
  opt-in, z80dma hook, software list, driver; without z8000 PC wrap, z80pio priority/mode 3
  after RETI and z8010 changes): `p8000_16` hardware test and WEGA boot to "Going
  multi-user" with `-ram 1m` and `-ram 8m` (`MAXSEG=<7E>`, user memory 8151552 bytes);
  `p8000` boots the UDOS floppy to `%`, `STATUS` and `DATE` work. So the three optional
  commits are not needed for these paths.
- `DEBUG=1` (gcc 13.3): `mamed -validate p8000` and `p8000_16` return 0; hardware test and
  WEGA boot to multi-user with 1 MiB, no assertion. The first debug build was killed by the
  out of memory killer in `luaengine.cpp` with `-j2` and left a broken object; use `-j1` and
  delete the object.
- clang 18.1.3 build of `p8k.cpp` and everything it links: no warning in the touched files.
- Other drivers built together with the changed devices and validated (`-validate` returns
  0): `m20`, `m40`, `m44`, `c900` (Z8000 + Z8010), `c5000`, `c8002` (Z8000), `aussieby`,
  `psi98`, `wicat`, `kc85_2..5`, `z1013*` (Z80 PIO/CTC/DMA). They were not run, because the
  ROMs are not available here. `specnext_dma` derives from `z80dma_device`: its
  `reset_byte_counter()` override does not clash with the new `byte_counter()` hook.
- `srcclean` (hand built, see TODO 2): the 15 touched files are unchanged.
- ROMs for the other drivers were not available, so "other systems are not affected" is
  only checked by building and validating; the PR texts must not claim more.
- The harness `ramtest.sh` sends two Returns at fixed times. In the (slower) debug build the
  second one arrives after "Press NMI", which stops the autoboot. Any input before NMI
  stops the boot, so `ramtest_dbg.sh` waits for the prompts instead.

- Two earlier wrong assumptions that were fixed: baud quartz 9.8304 MHz (right: 9.832 MHz),
  NBR described as a Z8010 property (it is board logic).

## 8. ROMs, floppies and the UDOS file names

Driver ROM names (no checksums in the names):

| Machine | Files |
|---|---|
| `p8000` | `mon8_1_3.1_nur8bit`, `mon8_2_3.1_nur8bit` (4096 bytes each) |
| `p8000_16` | `mon8_1_3.1`, `mon8_2_3.1`; `mon16_1h_3.1_udos`, `mon16_1l_3.1_udos`, `mon16_2h_3.1_udos`, `mon16_2l_3.1_udos` |

- `mon8_*_3.1_nur8bit` are dumps from the EPROMs of a real 8 bit only P8000. There is no
  release of them on the EAW floppies.
- `mon8_*_3.1` (EPROMs for the 8 bit board together with a 16 bit board) are named like
  the files on the EAW floppies; `mon16_*_3.1_udos` like `MON16_*_3.1_UDOS` on the WEGA start
  floppy. Their names are lower case versions of the original UDOS names.
- Not in the driver: the WDC ROMs (`wdc4.2_1-2c43.bin`, `wdc4.2_2-5d66.bin`, the controller
  is a software model), `p8t_zs`, `p8tdzs.2`.

Listing of the floppies with the UDOS command `CAT D=<drive> F=L P=&` (2026-10-16; syntax from
the UDOS Systemhandbuch 4.5: `CAT [String] [T=Type] [P=Properties] [D=Drive] [F=Format]
[L=Listingdisposition] [DATE rel Datum] [CDATE rel Datum]`; `P=&` shows all files including
secret ones, a plain `CAT` only shows non-secret files and every file on these disks is
secret). In the emulation: UDOS floppy `udossys.cqm` in drive 0, the second floppy loaded
after boot (a disk in drive 1 at boot makes UDOS stop with C4); `STATUS` then shows
`DRIVE 0 P8000_UDOS-SYSTEM` and `DRIVE 1 WEGA-STARTDISK`.

UDOS system disk (drive 0), relevant files: `MON8_1_3.1`, `MON8_2_3.1` (16 sectors of 256
bytes, record length 0100, dates 890125/890705); `P8TCU`, `P8TCUB`, `P8TCVB` (16 sectors),
`P8TDZS` (8 sectors = 2048 bytes), `P8T_1_4.1` (32 sectors); `EIGENTEST`, `ASM`, `U8000ASM`,
`ZLINK`, `EDIT` and others; 75 files.

WEGA start disk (drive 1), 57 files, relevant: `MON16_1H/1L/2H/2L_3.1` (9 records of 512
bytes, dates 880126/890510), `MON16_1H/1L/2H/2L_3.1_UDOS` (16 sectors of 256, 890302/890829),
`MON8_1_3.1` and `MON8_2_3.1` (9 x 512, 890509/890706), `MON8_1_3.1_UDOS` and
`MON8_2_3.1_UDOS` (16 x 256, 890125/890829), `WDC_1_4.2`, `WDC_2_4.2` (9 x 512), `WDC_1_4.2_UDOS`,
`WDC_2_4.2_UDOS` (16 x 256, 890706), `sa.install`, `sa.format`, `sa.timer`, `sa.shipdisk`,
`rc_csh`, `checklist`.

Open: which of `MON8_*` (UDOS disk, start disk, `_UDOS` variants) has exactly the content of
our `mon8_*_3.1`. The user confirmed that the `mon8_*_3.1` EPROMs are the ones for operation
with a 16 bit board and correct.

Software list `hash/p8000_flop.xml`: UDOS 3.1 system disk, OS/M 3.0 system disk, WEGA 3.1
start disk, WEGA 3.1 installation disks (root 1-5, usr 1-9, doc 1-2). Multi disk sets have one
part per disk, e.g. `mame p8000_16 w31start -flop2 w31root:flop1`. Local images are in
`/home/claude/sw/p8000_flop/` (sandbox only). The images are not part of the pull request.

UDOS Systemhandbuch (PDF) was read from `UDOS_Systemhandbuch.pdf` (86 pages): drive prefix
`0/CAT`, `:1/CAT`, `$ABDOS/CAT` select where a program is loaded from; sections 4.2
ACTIVATE, 4.5 CAT, 4.12 DEACTIVATE.

## 9. Comparison with another P8000 emulation

Another developer has an independent P8000 emulation (project k1520emu, `core/cards/p8000`).
It was compared with ours; where the two differed it was clarified who is right. This
comparison is not mentioned in the MAME pull request texts.

Differences found and resolved:

1. Bit 15 of the saved PC segment word: measured on the real machine (section 5.1); the
   other emulation's behaviour and ours now agree.
2. SWW only for system stack accesses: U8010 description 5.2 p. 34.
3. NBR equality goes to the stack MMU: circuit diagram (section 6). Both emulations did the
   same, derived from the circuit. A confirmation report ("[L1] NBR-Gleichheit: Originalplan
   bestätigt") was written for him (German) so he can close the point on his side.

Remaining difference: only the stop watch and weekday symbols of the clock module (not
emulated here, documented). Before the real hardware test the other emulation's code was only
read, nothing was copied.

## 10. MAME contribution rules and the review

Rules the PR has to follow (docs.mamedev.org contributing, and the `mame-review` skill from
`rb6502/mame-ai-tips`, v1.1, rule files RULES.md, STYLE.md, SYSTEMS.md, DATAFILES.md,
SUBMISSION.md):

- S-1 (blocking): AI help is declared in the first description, with model and version
  ("AI disclosure: assisted by Claude Sonnet 5.5"); the text and every answer to reviewers
  are the author's own, concise words (no templated headings, walls of bullets, tables).
  Hiding AI use or submitting work the author does not understand is the problem.
- S-2: title with path prefix (`ddr/p8k.cpp, machine/z80pio, ...: ...`), descriptive commit
  messages; new or promoted systems listed under the release notes headings (e.g. "Systems
  promoted to working").
- S-3: one thing per PR (independent core changes separately); no unrelated files
  (`PULL_REQUEST*.md` must not be in the PR); no reformatting of code that is not changed.
- S-4: full build, a `DEBUG=1` build, `mame -validate`, CI green in the contributor's own
  fork before the PR; a software list and usage notes (wiki) are nice to have.
- Do not link to ROM or software downloads.

Review results (all non-blocking), working tree status:

- Checked: include guards (`check_include_guards.py`), `xmllint --noout --valid
  hash/p8000_flop.xml`; `./srcclean` could not be built (manual check of tabs, trailing
  whitespace, line endings, BOM, final newline, non-ASCII: only two non-ASCII characters in
  comments of `z8010.cpp`, lines 362 and 448).
- Fixed: include order; tag lookups in the 8 bit board maps and configuration replaced by
  finders (`m_ctc8`, `m_pio1`, `m_dma`, `m_sio8_*`); `#define` replaced by `constexpr`; legacy
  `MCFG_MACHINE_RESET_OVERRIDE` replaced by `machine_reset()`; C-style casts; `memshare()`
  replaced by the finder; `#pragma once` removed; `static const` tables made `constexpr`;
  history comments removed; the statement about another emulator removed from `p8k_wdc.h`.
- Not fixed (see TODO section 5): helper device classes outside the anonymous namespace,
  RS232 tags, `m_dram8` as `unique_ptr`, child resets in `board16_reset()`, incomplete save
  states.

## 11. Pull request layout and commit history

Decision (2026-10-16): changes the P8000 needs stay in the driver PR; only the optional
corrections go into separate PRs: one for `z8000` PC wrap, one for the two `z80pio` priority
and mode 3 items, one for `z8010` (not one per line, MAME wants one topic per PR but not
micro PRs). Each branch starts at the current mamedev `master`, not at the driver branch.

Series on `p8k-clean` (author Oliver Lehmann, `Co-Authored-By` Claude Sonnet 5.5, each message
has an "AI disclosure" paragraph):

```
436f255b cpu/z8000: Fixed DAB after an addition for bytes 0xFA-0xFF
d613b434 cpu/z8000: PC offset wraps at FFFF without carry into the segment
70c2b2c6 machine/z80pio: Acknowledge only enabled ports, latch a mode 3 match during service
fb7176fb machine/z80pio: Port A has priority over port B, drop a mode 3 request after RETI
03dffddf machine/z8010: Fixed the set-all-inhibit commands, SAR wrap, command %10 and SWW
9e5d3d17 machine/z80ctc: Added opt-in set_reload_on_reset()
a54429c4 machine/z80dma: Added a virtual byte_counter() hook and a derivable ua858d_device
25528403 hash/p8000_flop.xml: Added the P8000 floppy disk software list
...      ddr/p8k.cpp: Emulated the 16 bit board, WEGA boots   (hash changes after later fixups)
...      PR drafts (not part of the pull requests, drop before submitting)
```

The old branch had 85 commits with messages like "PR text"; the clean series replaces them.
Authorship: the 82 commits of the old branch were written as "Claude <noreply@anthropic.com>";
on 2026-10-16 the author and committer of all of them were set to Oliver Lehmann
`<lehmann@ans-netz.de>`, the AI help stays visible in the `Co-Authored-By` trailer and in the
disclosure in the PR text. Reason: the author takes responsibility for the code (MAME S-1).
The statement "I, @OlliL, reviewed it myself, checked it against the documents named below and
ran it on the emulator" in the PR drafts has to be true when the PR is sent.

The statement "No driver needs this" for the z8000 PC wrap was removed from the draft: it is
not known whether another driver could use it; the draft now says that other drivers were not
tested.

## 12. Fork, CI and GitHub notes

- Workflows: `.github/workflows/ci-linux.yml` (gcc tiny and clang mame), `ci-macos.yml`,
  `ci-windows.yml` (gcc-x64, clang-x64, clang-arm64, then `-validate`), `includeguards`,
  `hash` (XML validation), `docs`, `language`. Triggers: `push` and `pull_request` with paths
  `src/**`, `hash/**`, `scripts/**`, `3rdparty/**`, `makefile`, `COPYING`,
  `.github/workflows/**`. No `workflow_dispatch`, so there is no "Run workflow" button.
- In a fork, Actions are disabled until enabled in the repository's *Actions* tab (browser;
  the mobile app has no such button). A pull request inside the fork (from `p8k-clean` to its
  `master`) starts the runs; "Re-run all jobs" repeats them.
- The CI result cannot be read from the sandbox (`gh` is not logged in); errors have to be
  copied from the web page.
- Stop hook in the sandbox ("N unpushed commits and no remote branch") is a false alarm: it
  looks at `origin` (mamedev); all branches are pushed to `olli`.
- 2026-10-16: CI of the pull request inside the fork (PR #2) is green on all jobs: Linux 2h32m,
  macOS 1h36m, Windows 3h18m, include guards 3m45s, XML/JSON validation 7m26s.
- A pull request to mamedev from a fork of a fork is possible (same fork network); this was
  not tried yet.

## 13. Known limitations and decisions

- Not emulated: stop watch key (ST) and weekday symbols of the clock module (the software
  does not use them); head load, write precompensation, separate drive select, drives 2/3,
  8" drives; parity/ECC NMI of the 16 bit board.
- Save states: see the section at the end (flag set since 2026-10-17).
- ROMs: the `nur8bit` ones are dumps of real EPROMs; others carry the EAW file names.
- Console terminal (2026-10-23): see the last section.
- Decision: no links to the ROM or floppy collection in the driver or in the PR (MAME does not
  point to downloads). Hosting a `mame/` collection of ROMs/images in the user's repository
  was discussed: rights for WEGA, UDOS and firmware images are for the owner to clear.
- Decision: keep the shared device changes that the P8000 needs inside the driver PR.
- The `tiffe.de` chip descriptions (U857D, U858D) are cited by title in commit messages, not by
  link.

## 14. Tools and how to reproduce

Build (sandbox, Linux, SDL2 in `/home/claude/sdl2`):

```
make SOURCES=src/mame/ddr/p8k.cpp SDL_INSTALL_ROOT=/home/claude/sdl2 NO_OPENGL=1 NO_X11=1 \
  NO_USE_XINPUT=1 NO_USE_XINPUT_WII_LIGHTGUN_HACK=1 NO_USE_MIDI=1 NO_USE_PORTAUDIO=1 \
  NO_USE_PULSEAUDIO=1 USE_QTDEBUG=0 TOOLS=0 NOWERROR=1 OPTIMIZE=1 SYMBOLS=0 -j8
# with PATH=/home/claude/sdl2/bin:$PATH LIBRARY_PATH=/home/claude/sdl2/lib
```

`mame -rompath <dir> -validate p8000_16` for validation.

Scripts in `tools/` (copied from the sandbox, paths inside are those of the sandbox and must
be adapted):

- `console.py <port> <logfile> <fifo>`: socket server for `-bitb socket.127.0.0.1:<port>`;
  logs what the machine prints and sends lines from the fifo (a `P:` prefix gives 7E1 parity,
  `\r` is turned into CR).
- `ctl2.lua`: autoboot script. It polls `ctl.txt` for `nmi`, `reset`, `swap <file>` (floppy
  into drive 1) and `swap0 <file>` (drive 0) and traces FDC I/O when `trace_on` exists.
- `ramtest.sh <1m|8m>`: boots `p8000_16` with the WEGA disk copy to the hardware test and
  on to multi-user; log in `inst.log`, wait for "multi-user".
- `udos8.sh`: boots `p8000` with the UDOS floppy and the console on port 5630 (insert the
  second floppy afterwards with `echo "swap <file>" > ctl.txt`, then `0/CAT F=L P=&`,
  `CAT D=1 F=L P=&`, `STATUS`).
- `gen.py`, `full.sh`, `enter.py`, `dbg.lua`: generate and enter the MON16 test program of
  section 5.1 into the emulator (`D 9000` ... `G 9000`) and read the frames back.

Cautions learnt: never use `pkill -f` (it kills the shell); kill stale `mame` and `console.py`
processes with a `ps | grep "[m]ame p8000"` loop inside a script file (an inline command whose
text contains the pattern kills itself); long foreground `sleep` is blocked, use `until` loops.

## 15. Sources and links

Documents (official or period):

- P8000 Hardwarehandbuch (hardware manual): chapters 3.3/4.3 (memory control), 4 (8 bit
  board), 5.2 (clocks, parts list), 6 (registers, SBR only on Index 0), 7 (16 bit board),
  table 3.7-4 (NMI identifier). Text version in `firmware/P8000_Hardwarehandbuch.txt`.
  `http://www.pofo.de/P8000/notes/books/`
- UDOS Systemhandbuch (4.5 CAT etc.).
- Original circuit diagrams of the 16 bit board, Ms 889255-2, Index 1 and Index 4 (also GLE).
- Schematics, parts list, layout of the 8 bit board (P8000/8.1, Ms 889 254-2):
  `http://www.pofo.de/P8000/notes/plaene/8-Bit-Rechner/`
- Schematics of the 16 bit board and U130 module drawn from the real boards (may contain
  errors): `http://www.pofo.de/P8000/notes/plaene/eigene/`
- Zilog Z80 PIO Technical Manual (priority 2.0, enable 4.3, mode 3 5.3, IEO until RETI 6.0);
  Z80 PIO Product Specification, `https://z80.mirrors.tinycomputers.io/zip/z80piomn.pdf`;
  Z80 Family Interrupt Structure, `https://z80.mirrors.tinycomputers.io/1653.htm`
- Technische Beschreibung PIO U855 D (VEB Mikroelektronik Karl Marx Erfurt), 5.3 p. 16/17,
  7.1 p. 25-27, picture 13.
- Zilog Z80 CTC Product Specification,
  `https://bitsavers.mirrorservice.org/components/zilog/z80/Z80_CTC_Product_Specification_Feb80.pdf`;
  Z80-CTC Technical Manual (circuit description pp. 37, 38, 44, fig. 4-15),
  `https://www.ecworld.ru/media/museum/Z80_ctc.pdf`
- Zilog Z80 DMA Product Specification,
  `https://bitsavers.mirrorservice.org/components/zilog/z80/Z80_DMA_Product_Specification_Feb80.pdf`
- U857D (CTC) and U858D (DMA) technical descriptions, VEB Funkwerk Erfurt
  (`tiffe.de` Bausteinübersicht, files `U857D-CTC.pdf` and `U858D-DMA.pdf`).
- U8000 CPU technical description: 3.4.2, 5.5.5-5.5.8, 6.6.2, fig. 6.1/6.2 (segment
  boundaries, offsets, saved program status, program status area), DAB p. 68.
- Technical description of the U8010 MMU: 3.2.1, 4.1, 5.2 p. 34, 5.4 p. 35 and 37/38,
  6.2.13-6.2.17, 6.2.21, 6.2.22, E.3, table 6.2.
- Zilog Z8010 MMU data sheet; ZiLOG System 8000 Hardware Reference Manual (table 3-2).
- Leventhal, Z8000 Assembly Language Programming: DAB table 3-12 (p. 3-167/3-168), chapter 12
  (p. 12-10 stack frame, 12-17 priorities, 12-18 IRET and reset, 12-23 PIO), chapter 13
  (p. 13-5/13-6, 13-8..13-11 table 13-1, 13-16/13-17).
- Mateosian, Programming the Z8000, chapter II p. 41-45 and appendix E p. 294.
- Moore, Z8000 Handbook (Prentice-Hall): chapter 2 p. 19 (PC wrap), table 2-8 (DAB, p. 133),
  chapter 5 p. 355 and 358 (SAR, table 5-3).
- Description of the 16 MiB memory card (segment 7FH left out).

Source code and software:

- Firmware (MON8, MON16, WDC), UDOS and WEGA sources and standalone programs:
  `https://github.com/OlliL/P8000`
- WDC hardware emulator with firmware sources and signal captures of a real WDC:
  `https://github.com/OlliL/P8000_WDC_Emulator`
- `src/devices/bus/zbi/s8k_cpu.cpp` and `zbi.cpp` in MAME (System 8000 CPU-A board).
- MAME C++ coding guidelines, `https://docs.mamedev.org/contributing/cxx.html`; Contributing,
  `https://docs.mamedev.org/contributing/`.
- `mame-review` skill, `https://github.com/rb6502/mame-ai-tips` (v1.1).
- mamedev pull request seen for reference: `https://github.com/mamedev/mame/pull/16447`.

## 16. Block instructions and the PC wrap (2026-10-16)

Reading `z8000ops.hxx` after the PC wrap fix showed that the repeating block instructions
restart with `m_pc -= 4`. An instruction of two words at `seg:FFFC` leaves the PC at
`seg:0000` after the fetch, and `-= 4` then borrows from the segment. Test with `ldir
@rr2,@rr4,r1` (`BB41 0120`) at `<<1>>%FFFC`, count 3, followed by `ld r0,#%1234` at
`<<1>>%0000`: old core (with the wrap fix only) copies one word, R1=2, then runs away
(PC=7FE0); with `restart_insn()` all three words are copied, R1=0, R0=1234, PC at
`<<1>>%0004`. Run with `tools/run_blk.sh <mame binary> <out>` (MON16 reached through `X` and
NMI, then `blk2.lua` takes over the CPU). Fix: commit `57986a34` on `p8k-clean`.

Test 06 (`tests/06-z8000-block-instruction-restart`) is the same case as a real-machine program like test 05:
`ldir` at `<<2>>%FFFC`, `sc #%21` at `<<2>>%0000`, marker `sc #%22` at `<<1>>%FFFC`. Emulator: old core
`7F22 D000 8100 FFFE` and `A001 0000 0000`; new core `7F21 D050 8200 0002` and `A001 A002 A003`.

### Manual references for the PC wrap and the block instruction restart (2026-10-16)

Read in the scanned documents supplied by the author (OCR):

- U8000 CPU description (German): 3.4.2 p. 36 (instructions and multi-byte data cannot cross
  segment boundaries), 5.5.5 p. 59 (indexed address: an overflow is ignored, it neither sets the
  overflow flag nor increments the segment number), 6.6.2 p. 68 table 6.1 note 2 (for an
  interruptible instruction such as LDIR the saved PC is the address of the current instruction).
- Z8001/Z8002 CPU manual, SGS-Thomson edition (82 pages): p. 5 (segment number is not altered by
  the effective address calculation), p. 65 (indexed: overflow ignored, segment number not
  incremented), p. 66 (relative addresses are in the same segment as the instruction, because the
  segment number is unchanged), p. 71 note 2 (same LDIR statement as the U8000 description).
- Zilog Z8001/2 CPU product specification, April 1985 (36 pages): segmented PC is two words, the
  first holds the 7 bit segment number, the second the 16 bit offset (p. 3). Nothing about wrap.
- No document says literally "the PC wraps from FFFF to 0000"; it follows from the statements
  above and from Moore p. 19. The Mateosian pages 267 to 269 (program status area, initial PC and
  FCW at reset) are not about this change.

Test 06 on the real machine (2026-10-16, 4 MiB): frame `7F21 D050 8200 0002`, words `A001 A002 A003`, equal to the emulator with the fix, FCW included.

### Audit of the other PC updates in the z8000 core (2026-10-16)

All places that change the PC after the fetch keep the upper 16 bits (segment) of `m_pc`:
`RDOP`, `get_operand`, `get_addr_operand`, `get_raw_addr_operand` (wrap), `addr_add`/`addr_sub`
(used by JR, CALR, DJNZ/DBJNZ, LDR/LDAR via `GET_DSP16`, base and index addressing),
`set_pc` (segmented: the new PC as is; non-segmented: low 16 bits only), `set_pc_long`,
`restart_insn()` (block instructions). Return addresses (`CALL`, `CALR`, traps, interrupts) are
pushed with `make_segmented_pc(m_pc)`, so after a wrapped fetch they hold `seg:0000`.
With the old carry, a JR/CALR/DJNZ whose displacement base was `seg:0000` after a fetch at
`seg:FFFE` would have jumped in the next segment (by reading the code, not run).
Emulator check (`tools/run_blk3.sh`, `tools/blk3.lua`): `jr +2 words` at `<<1>>%FFFE` ends in
segment 1 (r0=1111, PC `<<1>>%0008`).
Observation, not part of the change: in non-segmented mode `IRET` and `LDPS` load the PC with
`m_pc = POPW(SP)` / `RDMEM_W`, which clears the segment number of a Z8001 running non-segmented
code in a segment other than 0 (the manuals say the segment comes from the PC segment field).
Not looked at further; to ask the Z8001 specialist.

### Check of `hash/p8000_flop.xml` against the software list rules (2026-10-16)

Mechanical: `xmllint --noout --valid` clean; `srcclean` leaves the file unchanged; header, license
comment and tab indentation as required (SL-12, SL-13); names are lowercase and at most 8
characters (SL-1); `mame -verifysoftware p8000_16`: all 6 sets good (sizes, CRC, SHA1 equal to the
local images); `mame p8000_16 w31start` and `w31root` start (interface `floppy_5_25` is the one of
the `525qd` drives).
Content: `supported` is absent (= yes). Tested running: UDOS 3.1 (earlier), WEGA start, root 1-5 and
usr 1-9 (WEGA install), and now OS/M 3.0 (`mame p8000 osm30`: banner "P8000 OS/M 3.0 (C)ZFT/KEAW",
`DIR` lists the files). Not run: the two documentation disks `w31doc` (only the format is checked).
Publisher "ZFT/KEAW" is the spelling of the copyright lines in the UDOS and WEGA sources
(`(C) ZFT/KEAW Abt. Basissoftware - 1989`), so SL-8 is fine. Years: UDOS 3.1 1989 follows the
source copyright; the WEGA sets use 1989 without a source (kernel sources 1985-1988, hardware
manual 15.03.1989): `1989?` would be the honest form (SL-7).
Open for the owner: descriptions are descriptive, not the labels printed on the disks (SL-6);
where the images come from (original dumps of the disks, or converted?).
"Systems promoted to working" in the driver commit: the old skeleton had both systems as
`MACHINE_NOT_WORKING`; the heading names equal the full names in `p8k.cpp` ("P8000 (8bit Board)",
"P8000 (8bit and 16bit Board)").

Software list updated with the disk labels and READMEs of the owner (2026-10-16): the images are dumps
of the original disks delivered with the system. Titles as printed ("UDOS - Systemdisk", "OS/M -
Systemdisk", "WEGA - Startdisk", "WEGA - root n", "WEGA - usr n", "WEGA - doc n"), drawing numbers
889 336 (UDOS 3.1), 889 338 (OS/M 3.0), 889 347 to 889 363 (WEGA 3.1), versions 3.1/3.0/3.1,
dates from the READMEs: UDOS 29.05.1989, OS/M 15.01.1988, WEGA July 6, 1989 (so the year of the
WEGA sets is sourced and OS/M is 1988). The label list calls the last usr disk "usr 8" twice; the
WEGA README names it "usr 9" (889 361), the list follows the README. The doc disks are not to be
installed (README): WEGA file system disks with the manuals as packed print files.

### Upstream check (2026-10-16)

`origin/master` is at `a9d405aa` (85 commits after `f26abf2b`). No change in `src/devices/cpu/z8000`,
`cpu/z80`, `machine/z80pio`, `z80ctc`, `z80dma`, `z8010`, `z80daisy`, `bus/zbi`, `imagedev/floppy`, `i8272a` or
`src/mame/ddr`, and nobody touched the P8000 files. Other changes (`ioport.cpp` analog fields, `xtal.cpp`
one more crystal, cassette image code, z80scc) do not touch the driver. Test rebase of the 20 commits of
`p8k-clean` onto `a9d405aa` (local branch `p8k-rebase-test`, not pushed): no conflicts, build of `p8k.cpp`
without new warnings, `-validate` clean for `p8000` and `p8000_16`, `-verifysoftware` 6 of 6 good, test 06 program on the
emulator gives the same frame and words. The real rebase is still done right before the PRs.

## Save states (2026-10-17)

Registered with `save_item`: `m_dma_int`, `m_fdc_tc`, `m_is_seg_user`, `m_p16a`, `m_p16b`, `m_ardy0/1`,
`m_u_pio0_writing`, `m_u_pio0_a_bus`, `m_u8000_held`; `m_normal_mode` and `m_dma_on` are `bool` now
(`m_dma_on` is never set, so it is not saved). Test: WEGA from the hard disk to the shell, state saved,
one command, state loaded: the shell prompt counter and the output continued from the saved point;
`cp`/`cmp` on the disk worked afterwards. `p8000` (8 bit board): save and load in the monitor without
errors. `-validate` ok. `MACHINE_SUPPORTS_SAVE` set on both systems.

## Review comment on the PC wrap (2026-10-17)

A reviewer found the block-instruction borrow independently (test rig z8000_test, same result as test 06)
and suggested `addr_add(m_pc, 2)` for the six fetch places and `addr_sub(m_pc, 4)` for the 32 restarts.
`addr_add/addr_sub` already exist in the core with exactly my expression. Done: no `restart_insn()` any
more, the six comments are removed.

## Tidy items in the driver (2026-10-17)

Done: `board16_reset()` without child resets (the machine resets them; `soft_reset_w()` resets them
explicitly), `m_dram8` as `memory_share_creator`, RS232 callbacks use the port references,
`p8k_dma_device` private. The daisy device cannot be private (see TODO). Tested: build, `-validate`
(both systems), WEGA cold boot to login, machine reset and boot again to login, save/load of `p8000`.

## S8000 SADIE regression run (2026-10-17)

Source: https://github.com/tpaxia/Zilog_S8000 (`sadie_serial/`, injected diagnostics from a saved COMMAND LEVEL
state). Two `s8000` builds of the same MAME base (`f26abf2b`): one with the z8000/z8010/z80pio/z80ctc/z80dma files
reverted to the base, one with the `p8k-clean` series. Machine `s8000` (CPU-A), 150 s per diagnostic.

| Diagnostic | Base | p8k-clean | Remark |
| --- | --- | --- | --- |
| MMUTST | PASS, ERRORS=0 | PASS, ERRORS=0 | transcripts identical (the repo README reports 252 errors with another MAME build) |
| CACHETST, CIOTST | aborts: not an HPCPU | same | need `s8000s2` (HPCPU) |
| ICPTST2-4 | "no ICP's present" | same | ICP board not emulated |
| MEMTEST | still in "SEG 0 self parity bit test" after 150 s | same after 150 s; with 1800 s both: PASS, ERRORS=0 | needs several minutes |
| SIOTEST | endless laps | same | lap count depends on timing |
| SCCTST, ICPTST1, TEX, ECCTEST | wait for operator input / special hardware | same | not automatable |

No difference between base and the series where the output is deterministic. Not yet run: the other 24 diagnostics
(disk, tape, FPP, printer).

### Setup and limits (state 2026-10-17)

- Binaries: `s8000` from `SOURCES=src/mame/zilog/s8k.cpp` (`REGENIE=1` is needed when the source list changes);
  the SADIE scripts of the repo were run unchanged with `S8000_MAME` and `S8000_ROMS` set (ROM directory
  `s8k_cpu/`, device name, not `s8000/`). The harness leaves old MAME processes running; they have to be killed.
- The 24 diagnostics that were not run need disk, tape, FPP or printer hardware, or an operator; the harness
  cannot answer their questions (150 s timeout, "waiting for input").
- **`s8000s2` (HPCPU) cannot run the SADIE tests with this loader.** The ROMs are available
  (https://github.com/OlliL/S8000-roms, SHA1 equal to the driver, directory `s8k_hpcpu/`) and the machine starts
  ("System 8000 HPCPU Monitor 10.1 - Press START to Load System"), but the SADIE loader of the repo is written for
  the CPU-A board: it enters through the CPU-A monitor's `L` command and drives the Z80 SIO (`%FF81`), while the
  HPCPU has an SCC at other addresses and another monitor. It would need a new loader. CACHETST and CIOTST abort on
  CPU-A with "Not a HPCPU board" / "Host processor is not an HPCPU". Not needed for the P8000 changes.
- MEMTEST needs more than 150 s: after 150 s it is still in "SEG 0 self parity bit test" (both binaries), with a
  1800 s limit both binaries finish with **PASS, ERRORS=0** (16 segments = 1 MiB found, parity memory).
- Comparison of the manuals with the P8000 16 bit board: [S8000_COMPARISON.md](S8000_COMPARISON.md).

## Console terminal (2026-10-23)

Commit `03e70d5c` (by the owner, since squashed into the driver commit `64462a93`) adds `p8k_term.cpp/.h`: the terminal of the P8000 as a private
RS-232 device that runs the terminal firmware (U882/Z8, i8275, refresh memory, ZG0/ZG1, XT
keyboard, bell). It is the default of the `rs232` slot (option `adm31`) of both machines.

- Review with the `mame-review` rules: Blocking: the key queue was a `std::vector` (cannot be
  saved). Tidy: in-class initialisers with out-of-line constructor, `XTAL(...)` literals,
  `static const` colour table, helper function at global scope. All fixed (later squashed into the driver commit).
  Not changed: `code_pressed` polling in `nmi_button`, `m_conf` read per character cell,
  no `PORT_CHAR`.
- ROMs (device romset `p8k_terminal`): `p8t_1_5.0` (4096, CRC `1bd93470`, SHA1
  `436a9441971c2c88fe10791807742b7e727bb4ee`), `p8tezs` (2048, CRC `f9321251`, SHA1
  `a6a796b58d50ec4a416f2accc34bd76bc83f18ea`), `p8tdzs` (2048, CRC `32736503`, SHA1
  `6a1d7c55dddc64a7d601dfdbf917ce1afaefbb0a`). Checked against the three files the owner
  supplied. Without them the machines need `-rs232 terminal`; this goes into the PR text.
  Files for a local run: `/home/claude/roms/p8k_terminal/`.
- Keys: RESET = Ctrl+Pause, NMI = Pause (was F12), because the terminal keyboard takes all
  keys including F3.
- Tests: `-validate` for both machines, `-verifyroms p8k_terminal` good, both machines show the
  firmware banner and the monitor prompt and typed keys arrive; save and load work.
- Build note: `make` needs a tag in the history for `git describe`; the local annotated tag
  `mame0289-base` (on `f26abf2b`) was added for that and is never pushed.
- 2026-10-23: the terminal commits were squashed into the driver commit (`64462a93`); `p8k-clean`
  was force-pushed (head `431de339`), local backup branch `backup-p8k-clean-before-squash`.
  Hashes of older docs/commit links to the driver commit changed; the other commits keep theirs.
