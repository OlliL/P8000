# Z8010 test programs for the real P8000 (MON16)

Purpose: measure on the real 16 bit board what the U8010/Z8010 documents say (see LOG.md,
section 4.7 and TODO section 4). The programs only touch the **stack MMU** (special I/O
port low byte `F6`, register number in the high byte). They run in system mode, non
segmented, from RAM at `%9000`, `%9200`, `%9400`. Results are written to `%9100`, `%9300`,
`%9500`. They do not enable the MMU; program C sets the mode register to `38` for a moment
and puts the old value back. **Reset the machine afterwards**, the descriptors of the stack
MMU (5, 63) are changed.

Opcodes used (checked against `z8000tbl.hxx`): `C8vv` = `ldb rl0,#vv`, `3A87 pppp` =
`soutb pppp,rl0`, `3A85 pppp` = `sinb rl0,pppp`, `6E08 aaaa` = `ldb %aaaa,rl0`, `6008 aaaa`
= `ldb rl0,%aaaa`, `7F00` = `sc #0` (back to the monitor). Generator: `tools/gen_z8010.py`.

Start: reset, Return at the 8 bit monitor, `X`, any key at "Press NMI", then NMI. At the
`*` prompt enter `D <address>`, then every word on its own line, then `Q`, then
`G <address>`, then show the result with `D <result address> <count> W`. Upper case hex
only. The monitor answers `BREAK AT ...` after `G`.

## Program A: commands %15 and %16

`D 9000`, words:

```
C805 3A87 01F6 C800 3A87 0AF6 C83F 3A87
01F6 C800 3A87 0AF6 C800 3A87 15F6 C805
3A87 01F6 3A85 0AF6 6E08 9100 C83F 3A87
01F6 3A85 0AF6 6E08 9101 C805 3A87 01F6
C800 3A87 0AF6 C83F 3A87 01F6 C800 3A87
0AF6 C800 3A87 16F6 C805 3A87 01F6 3A85
0AF6 6E08 9102 C83F 3A87 01F6 3A85 0AF6
6E08 9103 7F00
```

`Q`, `G 9000`, `D 9100 2 W`. Expected: `0404 1010` (attribute bytes of descriptors 5 and 63
after %15 with data byte 00: bit 04 = CPU inhibit set; after %16 with data 00: bit 10 =
DMA inhibit set). An implementation that lets the data byte decide shows `00`.

## Program B: SAR wraps from 63 to 0

`D 9200`, words:

```
C83F 3A87 01F6 3A85 0EF6 6E08 9300 3A85
01F6 6E08 9301 C83F 3A87 01F6 3A85 0AF6
6E08 9302 3A85 0FF6 6E08 9303 3A85 01F6
6E08 9304 7F00
```

`Q`, `G 9200`, `D 9300 3 W`. Result bytes: attribute (cmd 0E), **SAR after 0E**, attribute
(cmd 0A), attribute (cmd 0F), **SAR after 0F**. Expected for the two SAR values: `00`
(U8010 6.2.13-6.2.16: descriptor 63 is followed by descriptor 0). The attribute bytes are
leftovers (`10` if A ran before). Emulator before the fix: 3F.

## Program C: command %10

`D 9400`, words:

```
3A85 00F6 6E08 9500 C838 3A87 00F6 C807
3A87 01F6 C802 3A87 20F6 3A85 00F6 6E08
9501 3A85 20F6 6E08 9502 3A85 01F6 6E08
9503 C800 3A87 10F6 3A85 00F6 6E08 9504
3A85 20F6 6E08 9505 3A85 01F6 6E08 9506
6008 9500 3A87 00F6 7F00
```

`Q`, `G 9400`, `D 9500 4 W`. Result bytes: MR at start, MR set (writes 38), DSC set (02),
SAR set (07), **MR after %10**, **DSC after %10**, **SAR after %10**. Expected in the
emulator: `00 38 02 07 00 00 07`: %10 clears MR and DSC, SAR stays (U8010 6.2.17, table 6.2).
Fill in the real values below.

## Real machine results

| Date | Program | Bytes read | Matches the emulator? |
|---|---|---|---|
| | A | | |
| | B | | |
| | C | | |

Not tested here (would need a working translation and a deliberate violation): SWW only for
system stack writes (status %9), Z80 PIO priority.
