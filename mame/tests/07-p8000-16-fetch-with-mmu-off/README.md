# Test 07: instruction fetch with the code MMU off and MMU ON set (16 bit board)

Status: **done on the real machine** (2026-10-26, MON16 over Kermit). The emulator does **not**
yet match (see "What it means"): with the current core the same fetch returns `NOP` and the
monitor never gets control back.

## What is tested

The 16 bit board is in MMU ON mode (SCR bit 1 = `ONH` set) and the code MMU is switched off
with mode `%00` (MSEN = 0, the Z8010 then drives no address). The next instruction fetch
(an `outb` that clears SCR again) is then the first access after the code MMU went off.

Question: which address does that fetch read from when no MMU drives the bus?

- Model from the schematic sheet 8 and the k1520emu MMU notes: the bus reads `FFFFH` in
  A8-A23, i.e. the fetch hits nothing useful.
- Measured below: the instruction is read from the place where the monitor wrote it, the
  address `segment * 64K + offset`, which is the MMU-off mapping of the board.

The test program ends with `sc #0` (BREAK). The monitor reads its `sc` vector from memory,
which works only after the critical `outb` has cleared `ONH`. So a return to the monitor
proves that the critical fetch was executed correctly.

Three runs were made:

| Run | Program at | Data MMU / stack MMU before the test | Result |
|---|---|---|---|
| 1 | `%9000`, segment 0 | left in reset state (not programmed) | `BREAK AT 901C` |
| 2 | `%9000`, segment 0 | set explicitly to `%00` | `BREAK AT 9028` |
| 3 | `<01>9000`, segment 1 (starts with `sc #1`) | set explicitly to `%00` | `BREAK AT 902C` |

## Procedure

Terminal: Kermit, as for the other tests. Monitor commands used: `D <address>` (enter words),
`Q` (end of entry), `D <address> <count>` (verify), `G <address>` (run).

Enter the words one per line, upper case hex. Verify with `D`, then start with `G`.

## Assembler listing

Run 2 (segment 0). Run 1 is the same without the two `soutb` to the data and stack MMU and
without the `ld r0,#%0000` in front of them; run 3 is run 2 shifted by `sc #1` and the
addresses of segment 1.

```
9000  2100 0000   ld   r0,#%0000         ; r0 = 0 for the MMU modes
9004  3A07 00FA   soutb #%00FA,rh0       ; data MMU -> %00 (MSEN = 0)
9008  3A07 00F6   soutb #%00F6,rh0       ; stack MMU -> %00 (MSEN = 0)
900C  2100 8000   ld   r0,#%8000         ; code MMU mode %80: MSEN, no TRNS
9010  3A07 00FC   soutb #%00FC,rh0       ; code MMU on
9014  2100 0200   ld   r0,#%0200         ; SCR = %02: MMU ON (ONH)
9018  3A06 FFC1   outb #%FFC1,rh0
901C  2100 0000   ld   r0,#%0000         ; code MMU mode %00 (off)
9020  3A07 00FC   soutb #%00FC,rh0       ; code MMU OFF, ONH still set
9024  3A06 FFC1   outb #%FFC1,rh0        ; critical fetch: ONH=1, code MMU off
9028  7F00        sc   #0               ; BREAK back to the monitor
```

Run 3 (segment 1, entered with `D <01>9000`):

```
<01>9000  7F01        sc   #1             ; segmented mode
<01>9002  2100 0000   ld   r0,#%0000
<01>9006  3A07 00FA   soutb #%00FA,rh0
<01>900A  3A07 00F6   soutb #%00F6,rh0
<01>900E  2100 8000   ld   r0,#%8000
<01>9012  3A07 00FC   soutb #%00FC,rh0
<01>9016  2100 0200   ld   r0,#%0200
<01>901A  3A06 FFC1   outb #%FFC1,rh0
<01>901E  2100 0000   ld   r0,#%0000
<01>9022  3A07 00FC   soutb #%00FC,rh0
<01>9026  3A06 FFC1   outb #%FFC1,rh0    ; critical fetch, segment 1
<01>902A  7F02        sc   #2             ; back to non-segmented mode
<01>902C  7F00        sc   #0             ; BREAK
```

## What comes out

Run 2, entry and verification (`D 9000 15`), then the run:

```
*D 9000 15
<00>9000 2100 0000 3A07 00FA 3A07 00F6 2100 8000
<00>9010 3A07 00FC 2100 0200 3A06 FFC1 2100 0000
<00>9020 3A07 00FC 3A06 FFC1 7F00
*G 9000
BREAK AT 9028
*
```

Run 3, entry and verification (`D <01>9000 20`), then the run:

```
*D <01>9000 20
<01>9000 7F01 2100 0000 3A07 00FA 3A07 00F6 2100
<01>9010 8000 3A07 00FC 2100 0200 3A06 FFC1 2100
<01>9020 0000 3A07 00FC 3A06 FFC1 7F02 7F00 5A82
<01>9030 5283 5301 5381 5283 5283 5303 5301 5282
*G <01>9000
BREAK AT 902C
```

Run 1, entry and run (program at `%9000`, the two data/stack `soutb` lines are omitted):

```
*G 9000
BREAK AT 901C
*
```

## What it means and which claim it supports

- In runs 1 to 3 the fetch after the code MMU was switched off, with `ONH` still set, read the
  instruction that the monitor had written at `segment * 64K + offset`. The `BREAK AT` return
  shows that the instructions after the fetch ran and that `ONH` was cleared by the critical
  `outb`.
- The data and stack MMU being left in their reset state (run 1) or explicitly set to `%00`
  (run 2) makes no difference.
- Segments 0 and 1 behave the same.

Claim supported: **with MMU ON and no MMU driving the bus, an instruction fetch reads
`segment * 64K + offset`** (the MMU-off mapping), not `FFFFH`.

Not tested: data reads and writes with MMU ON and all MMUs off; segments 64 and above; MMUs
in a mode where the N/S condition does not match.

Emulator: the current MAME core (`src/mame/ddr/p8k.cpp`, `translate_addr`) returns `false` in
this state, so the fetch yields `NOP` and the monitor does not return. A change that passes
the address through unchanged in this case is being evaluated.

## Result on the real machine

| Date | Board / monitor | Run | Result | Same as the emulator? |
|---|---|---|---|---|
| 2026-10-26 | 16 bit board, MON16 | 1 (segment 0, data/stack in reset state) | `BREAK AT 901C` | no (emulator: NOP) |
| 2026-10-26 | 16 bit board, MON16 | 2 (segment 0, data/stack `%00`) | `BREAK AT 9028` | no (emulator: NOP) |
| 2026-10-26 | 16 bit board, MON16 | 3 (segment 1) | `BREAK AT 902C` | no (emulator: NOP) |
