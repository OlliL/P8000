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
| absval | 0x2414-0x2432 | 0x23a0 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3 |  |
| adrlen | 0x23d6-0x2414 | 0x2352 | confirmed | analysed | D only (pattern-verified) | D13, D3, D7, D8 |  |
| alloc | 0x2510-0x256e | 0x2476 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| allocnode |  |  | open |  | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| backjmp | 0x1364-0x13d8 | 0x133c | confirmed | analysed | D only (pattern-verified) | D3, D7, D8 |  |
| ccinval | 0x2f54-0x2f6c | 0x2dda | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| cclook | 0x0a1c-0x0af2 | 0x0a44 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8 |  |
| cleargroup | 0x2d68-0x2d9c | 0x2c16 | confirmed | analysed | D only (pattern-verified) | D1, D8 |  |
| clearmem | 0x3526-0x354e |  | open |  |  |  |  |
| clearreg | 0x256e-0x2590 | 0x24d4 | confirmed | analysed | D only (pattern-verified) |  |  |
| codemove | 0x1130-0x1312 | 0x1102 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D8 |  |
| comjump | 0x1312-0x1364 | 0x12e6 | confirmed | analysed | D only (pattern-verified) |  |  |
| compare | 0x3342-0x3446 | 0x31ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D6, D8 |  |
| copy | 0x091c-0x09aa | 0x0964 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| decref | 0x24b0-0x24e8 | 0x2416 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| dest | 0x2b38-0x2b9e | 0x29f4 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8/D2 |  |
| dualop | 0x2dc2-0x2e3a | 0x2c72 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| dumpnodes | 0x061c-0x0638 | 0x0610 | confirmed | analysed | D only (pattern-verified) |  |  |
| equop | 0x2432-0x24b0 | 0x23b4 | confirmed | analysed | D only (pattern-verified) | D10, D2, D3, D7, D8, D8/D2 |  |
| equstr | 0x3478-0x34aa | 0x3340 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| findbyte | 0x265e-0x2694 | 0x25b6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findcon | 0x32f2-0x3342 | 0x3196 | confirmed | analysed | D only (pattern-verified) | D1, D2 |  |
| findcopy | 0x2778-0x27c6 | 0x26c2 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7 |  |
| findlong | 0x2702-0x2778 | 0x2656 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findreg | 0x2608-0x265e | 0x255a | confirmed | analysed | D only (pattern-verified) | D1, D2, D7 |  |
| findslot | 0x2e3a-0x2e84 | 0x2cd8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| findword | 0x2694-0x2702 | 0x25f4 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D5, D7, D8, D8/D2 |  |
| fixldm | 0x39a8-0x3a1e | 0x37cc | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| getch |  |  | open |  |  |  |  |
| getln | 0x04e8-0x05dc | 0x04d6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7, D8 |  |
| getnum | 0x05dc-0x061c | 0x05d2 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| hashname | 0x14c0-0x151a |  | open |  |  |  |  |
| heapreset |  |  | open |  |  |  |  |
| ilen | 0x22e0-0x23d6 | 0x224c | confirmed | analysed | other differences |  | structure (not a D pattern): order 5 moved |
| input | 0x0350-0x04e8 | 0x0358 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| insertl | 0x10ba-0x1130 | 0x108c | confirmed | analysed | D only (pattern-verified) | D1 |  |
| isreg | 0x2e84-0x2f54 | 0x2d22 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D8, D8/D2 |  |
| isused | 0x3a1e-0x3ac0 | 0x3838 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D5, D7, D8/D2 |  |
| iterate | 0x0e46-0x1024 | 0x0e1a | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| itoa_ | 0x3e0e-0x3e5e | 0x3c06 | confirmed | analysed | D only (pattern-verified) | D2 |  |
| jswcheck | 0x02c0-0x031a | 0x02c8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| jumpsw | 0x20f2-0x21cc | 0x207e | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D8 |  |
| killreg | 0x2b9e-0x2d68 | 0x2a46 | confirmed | analysed | D only (pattern-verified) | D1, D10, D2, D3, D5, D6, D8 |  |
| main | 0x002a-0x02c0 | 0x002a | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8 |  |
| makereg | 0x2ac0-0x2b38 | 0x2988 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| markregs | 0x45f2-0x46e4 | 0x432c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| memreg | 0x27c6-0x29d2 | 0x270e | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| movedat | 0x3116-0x323a | 0x2fc8 | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8, D8/D2 |  |
| natural | 0x34e4-0x3526 | 0x3396 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8 |  |
| newcode1 |  |  | open |  |  |  |  |
| nonlab | 0x24e8-0x2510 | 0x244e | confirmed | analysed | D only (pattern-verified) | D1 |  |
| oplook | 0x0af2-0x0cc2 | 0x0b02 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| opsetup | 0x09aa-0x0a1c | 0x09ec | confirmed | analysed | other differences |  | unexplained instruction forms: push	F,rS |
| output | 0x06c8-0x091c | 0x06ba | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| ovlap | 0x3e5e-0x45f2 | 0x3c5a | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| prefix | 0x0638-0x06c8 | 0x0630 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| procopt | 0x354e-0x38b4 | 0x33d6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| redunbr | 0x323a-0x32f2 | 0x30ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8/D2 |  |
| refcount | 0x0cc2-0x0e46 | 0x0ca0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8 |  |
| regprefix | 0x2f6c-0x2f9c | 0x2df8 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7 |  |
| rename_regs | 0x38b4-0x39a8 | 0x36d0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| repladdr | 0x2f9c-0x3116 | 0x2e2c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| reverse | 0x3dac-0x3e0e | 0x3b9a | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| rmove | 0x1416-0x20f2 | 0x13b6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| savereg | 0x2590-0x2608 | 0x24f6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| scan |  |  | partly fixed |  |  |  |  |
| setcc | 0x34aa-0x34e4 | 0x3362 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| setcon | 0x3446-0x3478 | 0x32f0 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8 |  |
| setreg | 0x29d2-0x2ac0 | 0x28ae | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| shortjump | 0x21cc-0x229c | 0x213c | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D7 |  |
| singop | 0x2d9c-0x2dc2 | 0x2c4a | confirmed | analysed | D only (pattern-verified) |  |  |
| statline | 0x031a-0x0350 | 0x0322 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| toofar | 0x229c-0x22e0 | 0x21fe | confirmed | analysed | D only (pattern-verified) | D10, D3, D7 |  |
| usesreg | 0x3ac0-0x3dac | 0x38d0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| xjump | 0x1024-0x10ba | 0x100e | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
