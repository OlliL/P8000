# Test 04: Z8010 command %10 resets MR, VTR and DSCR

Status: **program ready, not yet run on the real machine** (emulator result below).

## What is tested

Command %10 clears the mode register, the violation type register and the descriptor
selector register; SAR and the descriptors stay (U8010 description 6.2.17, table 6.2). The old
emulation did not know the command. The program reads MR, writes MR = 38 (master enable and
translate stay off), SAR = 7, DSCR = 2, reads all three back, issues %10, reads them again,
then writes the saved MR back.

The program only touches the **stack MMU** (special I/O port low byte `F6`, command number
in the high byte). It runs in system mode, non segmented, from RAM; the MMU is not enabled.
**Reset the machine afterwards** (descriptors of the stack MMU are changed).

## Procedure

Start: reset, Return at the 8 bit monitor, `X`, any key at "Press NMI", then NMI (nothing
else before the NMI, any input stops the boot). At the `*` prompt of MON16: `D <address>`,
then every word on its own line, then `Q`. `G <address>` starts the program, the monitor
answers `BREAK AT ...`. `D <address> <count> W` shows memory (count in words). Hex digits in
upper case only.

Words for `D 9400` (45 words; generator `tools/gen_z8010.py`):

```
3A85 00F6 6E08 9500 C838 3A87 00F6 C807
3A87 01F6 C802 3A87 20F6 3A85 00F6 6E08
9501 3A85 20F6 6E08 9502 3A85 01F6 6E08
9503 C800 3A87 10F6 3A85 00F6 6E08 9504
3A85 20F6 6E08 9505 3A85 01F6 6E08 9506
6008 9500 3A87 00F6 7F00
```

Then `Q`, `G 9400`, `D 9500 4 W`.

Opcodes (checked against `z8000tbl.hxx`): `C8vv` = `ldb rl0,#vv`, `3A87 pppp` =
`soutb pppp,rl0` (special I/O), `3A85 pppp` = `sinb rl0,pppp`, `6E08 aaaa` = `ldb %aaaa,rl0`,
`6008 aaaa` = `ldb rl0,%aaaa`, `7F00` = `sc #0` (back to the monitor).

## What comes out

Seven bytes at `%9500`, `D 9500 4 W`: MR at start, MR set, DSC set, SAR set, **MR after
%10**, **DSC after %10**, **SAR after %10**.

Expected (emulator): `00 38 02 07 00 00 07`. The first byte is whatever MR holds at start
(the real MON16 may leave a different value).

## What it means and which claim it supports

MR and DSC `00` after %10 and SAR still `07` means %10 resets exactly MR, VTR and DSCR (VTR
is not tested, it cannot be set without a violation) and not the SAR. That supports the claim
in commit `03dffddf` ("command %10 resets MR, VTR and DSCR").

## Photos / screen shots

Put the photos of the screen into `photos/` in this directory and list them here:

- (none yet)

## Result on the real machine

| Date | Board / monitor | Bytes read | Same as the emulator? |
|---|---|---|---|
| | | | |
