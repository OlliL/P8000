# Test 07: instruction fetch with the code MMU off and MMU ON set (16 bit board)

Status: **done on the real machine** (runs 1-3 on 2026-10-26, runs 4-8 on 2026-10-30, MON16 over
Kermit). Result: with `ONH` set and no MMU driving the bus, address lines A8-A23 keep the value of
the last memory cycle an MMU drove; A0-A7 come from the CPU as always. The emulator does **not**
match yet: commit `04154ab` passes the logical address through, which runs 1-7 cannot tell apart
from the measured behaviour, but run 8 can.

## What is tested

The 16 bit board is in MMU ON mode (SCR bit 1 = `ONH` set) and the code MMU is switched off
with mode `%00`. With MSEN = 0 the Z8010 tri-states its A8-A23 outputs (Fawcett, Z8000 Design
Handbook, ch. 9, mode register; sheet 8 of the 16 bit board). The instructions after the
`soutb` that does this are fetched while no MMU drives the upper address lines; an `outb` to
`%FFC1` then clears `ONH` again.

Question: which address does a fetch read in that state?

| Model | Predicts |
|---|---|
| A: the bus reads `FFFFH` in A8-A23 | no useful fetch: the program does not come back |
| B: the board passes the logical address through (commit `04154ab`) | fetch from `segment * 64K + offset` |
| C: one instruction is prefetched before the I/O write (report from a hobby colleague) | the next opcode is fetched correctly, further fetches fail |
| D: A8-A23 float and keep the value of the last MMU-driven memory cycle | fetches in the same 256 byte page as that cycle look correct; a fetch in another page reads the held page |

Model C does not agree with the documents: Fawcett ch. 5 p. 56/57 says the next instruction's
opcode is fetched during the internal cycles at the end of the current one, but *"instructions
that involve a load to a memory or I/O location cannot be overlapped with the following
instruction; these instructions end with a memory or I/O write ... the bus is not available for
the next instruction fetch until the write operation is completed."* The Z8000 CPU Technical
Manual (Jan 83) 4.3, fig. 4-3 ("Instruction Look-Ahead") says nothing on which instructions are
overlapped. `soutb` ends with an I/O write.

## Runs and results

| Run | Program | Fetches after the critical `soutb` until `ONH` is off | Result on the real machine |
|---|---|---|---|
| 1 | `%9000`, data/stack MMU in reset state | 2 (`3A06 FFC1`) | `BREAK AT 901C` |
| 2 | `%9000`, data/stack MMU `%00` | 2 | `BREAK AT 9028` |
| 3 | `<01>9000`, segment 1 | 2 | `BREAK AT 902C` |
| 4 | as 2, one-word `outb @r1,rh0` | 1 | `BREAK AT 902A` |
| 5 | as 4, one `nop` in front | 2 | `BREAK AT 902C` |
| 6 | as 4, two `nop` in front | 3 | `BREAK AT 902E` |
| 7 | as 2, one `nop` in front of `outb #%FFC1,rh0` | 3 | `BREAK AT 902E` |
| 8 | critical `soutb` at `%90FC`, the next fetch is in page `%91xx` | 2 | `BREAK AT 9104`, R5 = `0000`, R6 = `0001`: the fetch at `%9100` read `%9000` |
| 8K | run 8 with the critical `soutb` replaced by two `nop` (control) | - | `BREAK AT 9104`, R5 = `0001`, R6 = `0000` |

A return to the monitor shows that the instructions up to the `sc #0` were fetched as the
listing says. It does not prove by itself that `ONH` was cleared: MON16 runs from the on-board
EPROM and RAM, which are decoded on the CPU bus in front of the MMUs.

## What it means

- Model A is out: runs 1-7 come back.
- Model C is out: runs 6 and 7 come back with three fetches after the mode write; run 7 is the
  `nop` variant that was reported to fail.
- Runs 1-7 cannot tell B from D: all fetches after the `soutb` lie in the page of the last
  MMU-driven fetch (the second word of the `soutb`), so a held A8-A23 gives the right address.
- Run 8 tells them apart. The last MMU-driven memory cycle is the fetch of `%90FE`; the next
  fetch is at logical `%9100`. It executed `inc r6` from physical `%9000`, not `inc r5` from
  `%9100`. So the board does **not** pass the address through (B is wrong); A8-A23 kept `%0090`
  (D). The I/O cycle of the `soutb` itself did not change the held value.
- The control 8K, with the code MMU left on, executes `inc r5` from `%9100`, so the program is
  right.

Claim supported: **with MMU ON and no MMU driving the bus, A8-A23 keep the value of the last
memory cycle an MMU drove.** Whether the value decays after a longer time (pull-ups on the
bus) is not tested; runs 1-8 cover a few microseconds.

Not tested: data reads and writes in this state; segments 64 and above; a longer time between
the mode write and the fetch.

## Procedure

Terminal: Kermit, as for the other tests. Monitor commands used: `D <address>` (enter words),
`Q` (end of entry), `D <address> <count>` (verify), `G <address>` (run), `R` (registers after
the break).

Enter the words one per line, upper case hex. Verify with `D`, then start with `G`.

MON16 starts a program with the saved user FCW, which is `%C000` (segmented, system mode) after
a reset. None of the programs below has a memory operand with an address, so they run the same
in both modes. (A direct address such as `ld %9200,r5` would not: in segmented mode `9200` is the
first word of a long address.)

## Listings

### Runs 1 to 3

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

### Runs 4 to 7

The first 20 words are run 2 plus `ld r1,#%FFC1` at `%900C`; `outb @r1,rh0` (`3E10`) is an `outb`
of one word.

```
9000  2100 0000   ld    r0,#%0000
9004  3A07 00FA   soutb #%00FA,rh0       ; data MMU  -> %00
9008  3A07 00F6   soutb #%00F6,rh0       ; stack MMU -> %00
900C  2101 FFC1   ld    r1,#%FFC1        ; port of the SCR for outb @r1
9010  2100 8000   ld    r0,#%8000
9014  3A07 00FC   soutb #%00FC,rh0       ; code MMU on (MSEN, no TRNS)
9018  2100 0200   ld    r0,#%0200
901C  3A06 FFC1   outb  #%FFC1,rh0       ; SCR = %02: ONH on
9020  2100 0000   ld    r0,#%0000
9024  3A07 00FC   soutb #%00FC,rh0       ; code MMU off, ONH still set
```

| Run | From `%9028` | `D` count | Words from `%9028` |
|---|---|---|---|
| 4 | `outb @r1,rh0`, `sc #0` | `D 9000 16` | `3E10 7F00` |
| 5 | `nop`, `outb @r1,rh0`, `sc #0` | `D 9000 17` | `8D07 3E10 7F00` |
| 6 | `nop`, `nop`, `outb @r1,rh0`, `sc #0` | `D 9000 18` | `8D07 8D07 3E10 7F00` |
| 7 | `nop`, `outb #%FFC1,rh0`, `sc #0` | `D 9000 18` | `8D07 3A06 FFC1 7F00` |

### Run 8 and control 8K

The critical `soutb` sits at `%90FC`, so its second word at `%90FE` is the last fetch the code MMU
drives and the next fetch is the first one in page `%91xx`. Page `%90xx` holds a second copy of
the end with `inc r6` instead of `inc r5`.

```
90D0  2105 0000   ld    r5,#%0000
90D4  2106 0000   ld    r6,#%0000
90D8  2100 0000   ld    r0,#%0000
90DC  3A07 00FA   soutb #%00FA,rh0       ; data MMU  -> %00
90E0  3A07 00F6   soutb #%00F6,rh0       ; stack MMU -> %00
90E4  2101 FFC1   ld    r1,#%FFC1
90E8  2100 8000   ld    r0,#%8000
90EC  3A07 00FC   soutb #%00FC,rh0       ; code MMU on
90F0  2100 0200   ld    r0,#%0200
90F4  3A06 FFC1   outb  #%FFC1,rh0       ; ONH on
90F8  2100 0000   ld    r0,#%0000
90FC  3A07 00FC   soutb #%00FC,rh0       ; code MMU off (last MMU fetch: 90FE)
9100  A950        inc   r5,#1            ; first fetch in a new page
9102  3E10        outb  @r1,rh0          ; ONH off
9104  7F00        sc    #0

9000  A960        inc   r6,#1            ; read instead of 9100 if A8-A23 hold %0090
9002  3E10        outb  @r1,rh0
9004  7F00        sc    #0
```

Enter `D 9000`: `A960 3E10 7F00`. Then `D 90D0`:

```
2105 0000 2106 0000 2100 0000 3A07 00FA
3A07 00F6 2101 FFC1 2100 8000 3A07 00FC
2100 0200 3A06 FFC1 2100 0000 3A07 00FC
A950 3E10 7F00
```

Verify with `D 9000 3` and `D 90D0 1B`, start with `G 90D0`, then `R`.

| Outcome | Meaning |
|---|---|
| R5 = `0001`, R6 = `0000` | the address passes through (B) |
| R5 = `0000`, R6 = `0001` | A8-A23 keep the last MMU value (D) |
| no return | the fetch in the new page reads nothing useful |

Control 8K: replace the critical `soutb` with two `nop` (`D 90FC`: `8D07 8D07`), `G 90D0`, `R`;
expected R5 = `0001`, R6 = `0000`.

The encodings of runs 4 to 8 were checked with MAME's `unidasm -arch z8000`.

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

Run 3:

```
*D <01>9000 20
<01>9000 7F01 2100 0000 3A07 00FA 3A07 00F6 2100
<01>9010 8000 3A07 00FC 2100 0200 3A06 FFC1 2100
<01>9020 0000 3A07 00FC 3A06 FFC1 7F02 7F00 5A82
<01>9030 5283 5301 5381 5283 5283 5303 5301 5282
*G <01>9000
BREAK AT 902C
```

Run 1: `G 9000` gave `BREAK AT 901C`.

Run 4 (runs 5 to 7 alike, with the words of the table from `%9028`):

```
*D 9000 20
<00>9000 2100 0000 3A07 00FA 3A07 00F6 2101 FFC1
<00>9010 2100 8000 3A07 00FC 2100 0200 3A06 FFC1
<00>9020 2100 0000 3A07 00FC 3E10 7F00 D51D 080A
<00>9030 8002 D520 F536 8022 8022 D524 D524 0000
*G 9000
BREAK AT 902A
```

Run 8 and control 8K (after a reset):

```
*D 9000 3
<00>9000 A960 3E10 7F00
*D 90D0 1B
<00>90D0 2105 0000 2106 0000 2100 0000 3A07 00FA
<00>90E0 3A07 00F6 2101 FFC1 2100 8000 3A07 00FC
<00>90F0 2100 0200 3A06 FFC1 2100 0000 3A07 00FC
<00>9100 A950 3E10 7F00
*G 90D0
BREAK AT 9104
*R
R0   R1   R2   R3   R4   R5   R6   R7      SG     PC     FC     RF
0000 FFC1 0000 0000 0000 0000 0001 0000   8000   9104   C000   134E
R8   R9   R10  R11  R12  R13  R14  R15     N4     N5     PS     PO
0000 0000 0000 0000 0000 0000 8000 4020   8000   0000   8000   4400
*D 90FC
<00>90FC 3A07 :8D07
<00>90FE 00FC :8D07
<00>9100 A950 :Q
*G 90D0
BREAK AT 9104
*R
R0   R1   R2   R3   R4   R5   R6   R7      SG     PC     FC     RF
0000 FFC1 0000 0000 0000 0001 0000 0000   8000   9104   C000   1220
R8   R9   R10  R11  R12  R13  R14  R15     N4     N5     PS     PO
0000 0000 0000 0000 0000 0000 8000 4020   8000   0000   8000   4400
```

## Result on the real machine

| Date | Board / monitor | Run | Result | Same as the emulator? |
|---|---|---|---|---|
| 2026-10-26 | 16 bit board, MON16 | 1 | `BREAK AT 901C` | only with `04154ab` (before it: NOP, no return) |
| 2026-10-26 | 16 bit board, MON16 | 2 | `BREAK AT 9028` | only with `04154ab` |
| 2026-10-26 | 16 bit board, MON16 | 3 | `BREAK AT 902C` | only with `04154ab` |
| 2026-10-30 | 16 bit board, MON16 | 4-7 | return at the `sc #0` each time | only with `04154ab` |
| 2026-10-30 | 16 bit board, MON16 | 8 | R5 = `0000`, R6 = `0001` | no: `04154ab` gives R5 = `0001`, R6 = `0000` |
| 2026-10-30 | 16 bit board, MON16 | 8K | R5 = `0001`, R6 = `0000` | yes |

## Emulator

Before `04154ab`, `translate_addr()` in `src/mame/ddr/p8k.cpp` failed the access when no MMU drove
the bus, so a fetch returned `NOP` and the program never came back (model A). `04154ab` passes the
logical address through (model B), which fits runs 1-7 but not run 8. The measured behaviour (D)
needs the driver to remember A8-A23 of the last memory cycle an MMU drove and to use them, with
A0-A7 from the CPU, while `ONH` is set and no MMU drives the bus.

This also explains sa.diags: its `CODE_TRAP` (in RAM) switches the code MMU off with `soutb` and
clears `ONH` with the next instruction in the same page, so it works on the real machine and
with `04154ab`, but not without it. MON16's `CODE_TRAP` runs from the on-board EPROM, which does
not go through the MMUs, so it worked in the emulator from the start.
