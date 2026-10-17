# System 8000 and P8000 16 bit board: comparison

Status 2026-10-17. Sources: Zilog System 8000 Hardware Reference Manual 03-3198-01 (Mar 1982) and 03-3237-04
(Dec 1982, scan; only searched, not read completely); S8000 firmware 1.2 source (OlliL/S8000, rebuilt bit
identical from the EPROMs using the P8000 sources); MON16 source (`firmware/MON16`); MAME `s8k_cpu.cpp` and
`p8k.cpp`. Statements below are only those that were read in these files.

## What the two CPU boards share

- The same CPU core (Z8001), three Z8010 MMUs (code, data, stack) and the MMU I/O ports `%F0` (all),
  `%F6` stack, `%FA` data, `%FC` code (firmware equates `ALL_MMU`, `STACK_MMU`, ...).
- The switch/configuration register at I/O `%FFC1` (`S_BNK` / SCR) with the same bit meaning in both firmwares:
  bit 0 on-board PROM+RAM off, bit 1 MMUs on, bit 2 segmented user, bit 3 parity check on/clear parity error.
  Bits 4-7 are DIP switches on the S8000 (baud rate, autoboot device); the MON16 comments mark them "unused".
  The MAME drivers use the same constants (`SCR_BD_MEMON`, `SCR_MMU_ONH`, `SCR_SEG_USER`, `SCR_CLR_PARITY`).
- The RETI port at `%FFE1` (written with ED 4D for the Z80 peripherals, in both firmwares).
- On-board SIO/CTC/PIO set of Z80 peripherals at `%FF81..` in the I/O space with 8 bit registers on odd
  addresses; the SIO0 is at `%FF81` in both, SIO1 at `%FF89`.

## Differences (peripheral map)

| Device | S8000 CPU board | P8000 16 bit board |
| --- | --- | --- |
| SIO | 4 (`%FF81`, `%FF89`, `%FF91`, `%FF99`) | 2 (`%FF81`, `%FF89`) |
| PIO | 1 (`%FFB9`, printer) | 3 (`%FF91`, `%FF99` coupling to the 8 bit board, `%FFA1` Winchester) |
| CTC | 3 (`%FFA1`, `%FFA9`, `%FFB1`) | 2 (`%FFA9`, `%FFB1`) |
| `%FFB9` | PIO0 channel A data | RUN16 lamp off (`LOFF`), lamp on at `%FFD9` |
| System/Normal Break Register | not found at a fixed address in the firmware source | `%FFC9` / `%FFD1` |

The S8000 monitor console is SIO0 channel B.

## MMU select logic

S8000 manual (Mar 1982, section 4.9): the system break register is used for segment 0, the normal break
register for other segments; segments 0, 1, 64 and 65 are "system segments" (always compared against the SBR,
independent of system/normal mode); in normal mode a reference to a system segment selects no MMU and forces a
segment violation until the CPU's segment trap acknowledge. That manual describes a CPU board that was "currently
configured for only non-segmented operating systems" (jumpers E1-E12 choose; segmented mode "will be available").
The MAME `s8k_cpu` has a "Support Segmented OS" setting for the later boards.

The P8000 driver implements the "special logic" of the P8000 hardware manual instead: segmented operating system,
system mode uses the code MMU for everything, normal mode with SEG USR = 1 selects data (segments 0-63) or stack
(64-127), with SEG USR = 0 code fetches use the code MMU and the data/stack split compares address bits 8-15 with
the NBR. These are two board variants; nothing here shows that either description is wrong.

## What follows for testing

- The SADIE tape diagnostics that matter for the shared parts are MMUTST (Z8010 registers, read-only, limit,
  direction, translation violations) and the memory tests (they run with the MMUs on). Both run without errors
  with and without our changes (LOG.md, "S8000 SADIE regression run").
- The S8000 SADIE loader works on the CPU-A board (Z80 SIO). It cannot be used for the HPCPU (SCC, other monitor).
- Open: parity/ECC NMI (SCR bit 3) is not emulated on the P8000 board; the S8000 manual describes the ECC NMI
  (4.7.3) for the S8000 only.
