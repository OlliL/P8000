# Test 01: bit 15 of the saved PC segment word (trap frame and IRET)

Status: **done on the real machine** (P8000 16 bit board, MON16 3.1), reproduced in the emulator.

## What is tested

When the Z8001 takes a system call (or any trap), it pushes a frame: identifier word, FCW,
PC segment word, PC offset. The PC segment word has the "SEG" flag in bit 15 in front of
the 7 bit segment number. The U8000 CPU description (6.6.2, figures 6.1 and 6.2) does not
say whether this bit is pushed as the PC holds it and what IRET does with it when it is
changed in the frame.

The test is a small MON16 program that calls `sc` four times. The handler in the program
status area copies the frame to memory, **flips bit 15** of the saved PC segment word and
returns with `IRET`. If IRET loads the bit back unchanged and the next trap pushes the PC
as it is, the saved bit alternates from call to call.

## Procedure

Start: reset, Return at the 8 bit monitor, `X`, any key at "Press NMI", then NMI (nothing
else before the NMI, any input stops the boot). At the `*` prompt of MON16: `D <address>`,
then every word on its own line, then `Q`. `G <address>` starts the program, the monitor
answers `BREAK AT ...`. `D <address> <count> W` shows memory (count in words). Hex digits in
upper case only.

Program main (49 words at `%9000`, runs segmented after `sc #1`, writes the frames to
`%9200`):

```
7F01 2106 8000 7D15 A117 0107 001C 2160
6F00 8000 9110 0D65 8000 A971 2160 6F00
8000 9112 0D65 9080 4D05 8000 9100 8000
4D05 8000 9102 9200 7F11 7F12 7F13 7F14
2106 8000 7D15 A117 0107 001C 6100 8000
9110 2F60 A971 6100 8000 9112 2F60 7F02
7F00
```

Handler (25 words at `%9080`):

```
97E0 97E1 97E2 97E3 5406 8000 9100 2F60
A971 2F61 A971 2F62 A971 2F63 A971 5D06
8000 9100 0902 8000 93E3 93E2 93E1 93E0
7B00
```

Enter both with `D 9000` / `D 9080`, `G 9000`, then `D 9200 10 W` (the generator is
`tools/gen.py`). Lower case hex is rejected by the real MON16.

## What comes out

Four frames (identifier, FCW, PC segment word, PC offset):

```
7F11 D000 8000 903A
7F12 D000 0000 903C
7F13 D000 8000 903E
7F14 D000 0000 9040
```

The emulator prints the same four frames.

## What it means and which claim it supports

- Bit 15 of the PC segment word in the frame is exactly what the PC held (the first `sc` was
  entered with bit 15 set: `8000`), so the bit **is saved**.
- The handler flipped it each time and the following frame shows the flipped value, so
  IRET **loads the bit back unchanged** (nothing masks or forces it) and the next trap pushes
  the PC as is.

This is what the Z8000 core in MAME already did (`m_pc_b15`). The test removes the former
"known limitation" in the PR text, and no change to the core is needed for it.
Source of the questions: U8000 CPU technical description 6.6.2, fig. 6.1/6.2; the answer
comes from the measurement, not from a document.

## Photos / screen shots

Put the photos of the screen into `photos/` in this directory and list them here:

- (none yet)
