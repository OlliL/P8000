# Test 03: Z8010 segment address register wraps from 63 to 0

Status: **program ready, not yet run on the real machine** (emulator result below).

## What is tested

The descriptor autoincrement commands (0E: read attribute then SAR+1, 0F: read field,
then DSC+1 and SAR+1 after the attribute byte) behind descriptor 63 continue with
descriptor 0 (U8010 description 6.2.13, 6.2.14, 6.2.16; Moore p. 355/358). The old emulation
stayed at 63. The program sets SAR to 63, reads with command 0E, reads SAR back; sets SAR to 63
again, reads with 0A and 0F, reads SAR back.

The program only touches the **stack MMU** (special I/O port low byte `F6`, command number
in the high byte). It runs in system mode, non segmented, from RAM; the MMU is not enabled.
**Reset the machine afterwards** (descriptors of the stack MMU are changed).

## Procedure

Start: reset, Return at the 8 bit monitor, `X`, any key at "Press NMI", then NMI (nothing
else before the NMI, any input stops the boot). At the `*` prompt of MON16: `D <address>`,
then every word on its own line, then `Q`. `G <address>` starts the program, the monitor
answers `BREAK AT ...`. `D <address> <count> W` shows memory (count in words). Hex digits in
upper case only.

Words for `D 9200` (27 words; generator `tools/gen_z8010.py`):

```
C83F 3A87 01F6 3A85 0EF6 6E08 9300 3A85
01F6 6E08 9301 C83F 3A87 01F6 3A85 0AF6
6E08 9302 3A85 0FF6 6E08 9303 3A85 01F6
6E08 9304 7F00
```

Then `Q`, `G 9200`, `D 9300 3 W`.

Opcodes (checked against `z8000tbl.hxx`): `C8vv` = `ldb rl0,#vv`, `3A87 pppp` =
`soutb pppp,rl0` (special I/O), `3A85 pppp` = `sinb rl0,pppp`, `6E08 aaaa` = `ldb %aaaa,rl0`,
`6008 aaaa` = `ldb rl0,%aaaa`, `7F00` = `sc #0` (back to the monitor).

## What comes out

Five bytes at `%9300`: attribute (cmd 0E), **SAR after 0E**, attribute (cmd 0A),
attribute (cmd 0F), **SAR after 0F**. `D 9300 3 W`.

Expected: the 2nd and 5th byte are `00` (emulator: `10 00 10 10 00`, the attribute bytes
are leftovers of test 02: `10`; if test 02 was not run before they differ).
Emulator before the fix: `3F` (stays at 63).

## What it means and which claim it supports

`00` for both SAR values means the register wraps (a 6 bit counter without carry out).
That supports the claim in commit `03dffddf` ("SAR autoincrement wraps from 63 to 0").
Nothing else in the P8000 software depends on it (the kernel always writes SAR first), so a
different real value would be a finding to document, not a boot problem.

## Photos / screen shots

Put the photos of the screen into `photos/` in this directory and list them here:

- (none yet)

## Result on the real machine

| Date | Board / monitor | Bytes read | Same as the emulator? |
|---|---|---|---|
| | | | |
