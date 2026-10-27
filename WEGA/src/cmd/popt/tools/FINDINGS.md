# popt findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `popt` compared with `/lib/popt`; pending findings are the open items of the analysis.

## Summary

**Functions:** 75

| Pairing | Count |
|---|---|
| confirmed | 68 |
| open | 6 |
| partly fixed | 1 |

| Difference class | Count |
|---|---|
| - | 6 |
| D only (pattern-verified) | 67 |
| other differences | 2 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| absval | 0x2414-0x2432 | 0x23a0 | confirmed | analysed | read by hand | D4 | argument spilled to the frame by ours (D4) |
| adrlen | 0x23d6-0x2414 | 0x2352 | confirmed | analysed | D only (pattern-verified) | D13, D3, D7, D8 |  |
| alloc | 0x2510-0x256e | 0x2476 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| allocnode |  |  | open |  | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| backjmp | 0x1364-0x13d8 | 0x133c | confirmed | analysed | D only (pattern-verified) | D3, D7, D8 |  |
| ccinval | 0x2f3a-0x2f52 | 0x2dda | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| cclook | 0x0a1c-0x0af2 | 0x0a44 | confirmed | analysed | read by hand | D2 | scratch registers of the copy loop |
| cleargroup | 0x2d4e-0x2d82 | 0x2c16 | confirmed | analysed | D only (pattern-verified) | D1, D8 |  |
| clearmem | 0x352a-0x3552 |  | open |  |  |  |  |
| clearreg | 0x256e-0x2590 | 0x24d4 | confirmed | analysed | identical |  | byte-identical (measure.py); no class needed |
| codemove | 0x1130-0x1312 | 0x1102 | confirmed | analysed | open (not classified) |  | parameter in another register than in the original; not classified (popt-30) |
| comjump | 0x1312-0x1364 | 0x12e6 | confirmed | analysed | identical |  | byte-identical (measure.py); no class needed |
| compare | 0x3328-0x342c | 0x31ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D6, D8 |  |
| copy | 0x091c-0x09aa | 0x0964 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| decref | 0x24b0-0x24e8 | 0x2416 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| dest | 0x2b1e-0x2b84 | 0x29f4 | confirmed | analysed | read by hand | D2, D6 | scratch registers (D2), order of the value cases (D6) |
| dualop | 0x2da8-0x2e20 | 0x2c72 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| dumpnodes | 0x061c-0x0638 | 0x0610 | confirmed | analysed | identical |  | byte-identical with the frame operand form masked (@r15 against 0(r15), D16) |
| equop | 0x2432-0x24b0 | 0x23b4 | confirmed | analysed | D only (pattern-verified) | D10, D2, D3, D7, D8, D8/D2 |  |
| equstr | 0x347c-0x34ae | 0x3340 | confirmed | analysed | read by hand | D2 | scratch registers r1/r3 against r4/r6 |
| findbyte | 0x2644-0x267a | 0x25b6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findcon | 0x32d8-0x3328 | 0x3196 | confirmed | analysed | D only (pattern-verified) | D1, D2 |  |
| findcopy | 0x275e-0x27ac | 0x26c2 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7 |  |
| findlong | 0x26e8-0x275e | 0x2656 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findreg | 0x25ee-0x2644 | 0x255a | confirmed | analysed | D only (pattern-verified) | D1, D2, D7 |  |
| findslot | 0x2e20-0x2e6a | 0x2cd8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| findword | 0x267a-0x26e8 | 0x25f4 | confirmed | analysed | read by hand |  | open: 18 equal with register n, length 33 against 31 |
| fixldm | 0x39ac-0x3a22 | 0x37cc | confirmed | analysed | read by hand | D1, D4 | frame; no register variant helped |
| getch |  |  | open |  |  |  |  |
| getln | 0x04e8-0x05dc | 0x04d6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7, D8 |  |
| getnum | 0x05dc-0x061c | 0x05d2 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| hashname | 0x14c0-0x151a |  | open |  |  |  |  |
| heapreset |  |  | open |  |  |  |  |
| ilen | 0x22e0-0x23d6 | 0x224c | confirmed | analysed | open (not classified) |  | call forms (D17), register and frame differences; not classified as a whole |
| input | 0x0350-0x04e8 | 0x0358 | confirmed | analysed | read by hand | D4, D6, D8 | comparison chain of the switch (D6), byte flag echo (D8), frame (D4) |
| insertl | 0x10ba-0x1130 | 0x108c | confirmed | analysed | D only (pattern-verified) | D1 |  |
| isreg | 0x2e6a-0x2f3a | 0x2d22 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D8, D8/D2 |  |
| isused | 0x3a22-0x3ac4 | 0x3838 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D5, D7, D8/D2 |  |
| iterate | 0x0e46-0x1024 | 0x0e1a | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| itoa_ | 0x3e12-0x3e62 | 0x3c06 | confirmed | analysed | read by hand | D2 | scratch registers r0/r2/r3 against r4/r5 |
| jswcheck | 0x02c0-0x031a | 0x02c8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| jumpsw | 0x20f2-0x21cc | 0x207e | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D8 |  |
| killreg | 0x2b84-0x2d4e | 0x2a46 | confirmed | analysed | read by hand | D5, D6 | array index form (D5), jump table of the switch (D6) |
| main | 0x002a-0x02c0 | 0x002a | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8 |  |
| makereg | 0x2aa6-0x2b1e | 0x2988 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| markregs | 0x45f6-0x46e8 | 0x432c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| memreg | 0x27ac-0x29b8 | 0x270e | confirmed | analysed | read by hand | D2, D4, D5 | scratch registers, home slots, address form of regs[reg] (D5) |
| movedat | 0x30fc-0x3220 | 0x2fc8 | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8, D8/D2 |  |
| natural | 0x34e8-0x352a | 0x3396 | confirmed | analysed | read by hand | D4, D7 | frame and loop layout |
| newcode1 |  |  | open |  |  |  |  |
| nonlab | 0x24e8-0x2510 | 0x244e | confirmed | analysed | D only (pattern-verified) | D1 |  |
| oplook | 0x0af2-0x0cc2 | 0x0b02 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| opsetup | 0x09aa-0x0a1c | 0x09ec | confirmed | analysed | read by hand | D1, D2 | hash scratch registers r0/r4 (D2), frame size (D1) |
| output | 0x06c8-0x091c | 0x06ba | confirmed | analysed | read by hand | D6 | jump table indexed directly by the original (D6) |
| ovlap | 0x3e62-0x45f6 | 0x3c5a | confirmed | analysed | open (not classified) |  | copies of the scan loop: scratch registers (D2), frame (D1, D4), pattern address (D3 likely, not proven) (popt-32) |
| prefix | 0x0638-0x06c8 | 0x0630 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| procopt | 0x3552-0x38b8 | 0x33d6 | confirmed | analysed | read by hand | D5 | address form of local arrays with a register index (D5) |
| redunbr | 0x3220-0x32d8 | 0x30ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8/D2 |  |
| refcount | 0x0cc2-0x0e46 | 0x0ca0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8 |  |
| regprefix | 0x2f52-0x2f82 | 0x2df8 | confirmed | analysed | open (not classified) |  | string pointers loaded from data; D12 or D13 not confirmed (popt-31) |
| rename_regs | 0x38b8-0x39ac | 0x36d0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| repladdr | 0x2f82-0x30fc | 0x2e2c | confirmed | analysed | read by hand | D2, D4 | frame and scratch registers |
| reverse | 0x3db0-0x3e12 | 0x3b9a | confirmed | analysed | read by hand | D2 | temporaries in other scratch registers |
| rmove | 0x1416-0x20f2 | 0x13b6 | confirmed | analysed | read by hand | D17, D1, D2 | call forms (D17), register saving and frame (D1, D2) |
| savereg | 0x2590-0x25ee | 0x24f6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D7, D8 |  |
| scan |  |  | partly fixed |  |  |  |  |
| setcc | 0x34ae-0x34e8 | 0x3362 | confirmed | analysed | read by hand | D2, D4 | frame 6 against 4 bytes (D4), scratch byte rl4 against rl0 (D2) |
| setcon | 0x342c-0x347c | 0x32f0 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3 |  |
| setreg | 0x29b8-0x2aa6 | 0x28ae | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| shortjump | 0x21cc-0x229c | 0x213c | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D7 |  |
| singop | 0x2d82-0x2da8 | 0x2c4a | confirmed | analysed | read by hand | D2 | scratch byte rl0 against rl4, register of the source pointer (D2) |
| statline | 0x031a-0x0350 | 0x0322 | confirmed | analysed | read by hand | D4, D16 | register parameter home slot (D4), frame operand form (D16) |
| toofar | 0x229c-0x22e0 | 0x21fe | confirmed | analysed | D only (pattern-verified) | D10, D3, D7 |  |
| usesreg | 0x3ac4-0x3db0 | 0x38d0 | confirmed | analysed | read by hand | D2, D4 | home slots of the register parameters (D4), scratch registers (D2) |
| xjump | 0x1024-0x10ba | 0x100e | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
