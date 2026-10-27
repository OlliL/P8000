# popt findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `popt` compared with `/lib/popt`; pending findings are the open items of the analysis.

## Summary

**Functions:** 75

| Pairing | Count |
|---|---|
| confirmed | 68 |
| open | 7 |

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
| absval | 0x1a0a-0x1a22 | 0x23a0 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D6 |  |
| adrlen | 0x3040-0x307c | 0x2352 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| alloc | 0x1906-0x1964 | 0x2476 | confirmed | analysed | D only (pattern-verified) | D1, D13, D16, D2, D3, D6, D8 |  |
| allocnode |  |  | open |  | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| backjmp | 0x17fc-0x18a0 | 0x133c | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8 |  |
| ccinval | 0x24e8-0x24fe | 0x2dda | confirmed | analysed | D only (pattern-verified) | D2, D3 |  |
| cclook | 0x0b94-0x0c68 | 0x0a44 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8 |  |
| cleargroup | 0x22e8-0x232a | 0x2c16 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D8, D8/D2 |  |
| clearmem | 0x18a0-0x18c8 |  | open |  |  |  |  |
| clearreg | 0x1aa0-0x1aca | 0x24d4 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D3, D8, D8/D2 |  |
| codemove | 0x148c-0x1784 | 0x1102 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| comjump | 0x1784-0x17fc | 0x12e6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8/D2 |  |
| compare | 0x29ba-0x2b32 | 0x31ec | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| copy | 0x0a58-0x0ae6 | 0x0964 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| decref | 0x199e-0x19d8 | 0x2416 | confirmed | analysed | D only (pattern-verified) | D2, D3, D8/D2 |  |
| dest | 0x20ba-0x211e | 0x29f4 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8/D2 |  |
| dualop | 0x2350-0x23c8 | 0x2c72 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| dumpnodes | 0x002a-0x004e | 0x0610 | confirmed | analysed | D only (pattern-verified) | D13, D2, D3 |  |
| equop | 0x1a22-0x1aa0 | 0x23b4 | confirmed | analysed | D only (pattern-verified) | D10, D2, D3, D7, D8, D8/D2 |  |
| equstr | 0x19d8-0x1a0a | 0x3340 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| findbyte | 0x1ba8-0x1bde | 0x25b6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findcon | 0x296e-0x29ba | 0x3196 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3 |  |
| findcopy | 0x1cc0-0x1d0e | 0x26c2 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7 |  |
| findlong | 0x1c4a-0x1cc0 | 0x2656 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findreg | 0x1b42-0x1ba8 | 0x255a | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D7, D8, D8/D2 |  |
| findslot | 0x23c8-0x241e | 0x2cd8 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D7, D8, D8/D2 |  |
| findword | 0x1bde-0x1c4a | 0x25f4 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| fixldm | 0x4a96-0x4b1a | 0x37cc | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| getch | 0x0572-0x059c |  | open |  |  |  |  |
| getln | 0x059c-0x064c | 0x04d6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| getnum | 0x064c-0x06a4 | 0x05d2 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D8, D8/D2 |  |
| hashname | 0x0ae6-0x0b40 |  | open |  |  |  |  |
| heapreset |  |  | open |  |  |  |  |
| ilen | 0x2ef4-0x3040 | 0x224c | confirmed | analysed | other differences |  | structure (not a D pattern): order 5 moved |
| input | 0x0310-0x04fe | 0x0358 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| insertl | 0x13d2-0x148c | 0x108c | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7 |  |
| isreg | 0x241e-0x24e8 | 0x2d22 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D8, D8/D2 |  |
| isused | 0x4754-0x47f6 | 0x3838 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D5, D7, D8/D2 |  |
| iterate | 0x106a-0x131e | 0x0e1a | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| itoa_ | 0x40a2-0x40f2 | 0x3c06 | confirmed | analysed | D only (pattern-verified) | D2 |  |
| jswcheck | 0x04fe-0x0572 | 0x02c8 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5 |  |
| jumpsw | 0x2c62-0x2d92 | 0x207e | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| killreg | 0x211e-0x22e8 | 0x2a46 | confirmed | analysed | D only (pattern-verified) | D1, D10, D2, D3, D5, D6, D8 |  |
| main | 0x004e-0x02e4 | 0x002a | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8 |  |
| makereg | 0x2024-0x20ba | 0x2988 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| markregs | 0x47f6-0x4934 | 0x432c | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| memreg | 0x1d0e-0x1f34 | 0x270e | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| movedat | 0x26a8-0x2888 | 0x2fc8 | confirmed | analysed | D only (pattern-verified) | D10, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| natural | 0x2c1e-0x2c62 | 0x3396 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| newcode1 |  |  | open |  |  |  |  |
| nonlab | 0x196e-0x199e | 0x244e | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8/D2 |  |
| oplook | 0x0c68-0x0e2e | 0x0b02 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| opsetup | 0x0b40-0x0b94 | 0x09ec | confirmed | analysed | other differences |  | unexplained instruction forms: push	F,rS |
| output | 0x06a4-0x09a8 | 0x06ba | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| ovlap | 0x4174-0x4468 | 0x3c5a | confirmed | analysed | D only (pattern-verified) | D1, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| prefix | 0x09a8-0x0a58 | 0x0630 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| procopt | 0x4b1a-0x4e7e | 0x33d6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| redunbr | 0x2888-0x296e | 0x30ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| refcount | 0x0e2e-0x106a | 0x0ca0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| regprefix | 0x24fe-0x252e | 0x2df8 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7 |  |
| rename_regs | 0x4934-0x4a96 | 0x36d0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| repladdr | 0x252e-0x26a8 | 0x2e2c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| reverse | 0x4040-0x40a2 | 0x3b9a | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| rmove | 0x3090-0x4040 | 0x13b6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| savereg | 0x1aca-0x1b42 | 0x24f6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| scan | 0x40f2-0x4174 |  | open |  |  |  |  |
| setcc | 0x2be4-0x2c1e | 0x3362 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| setcon | 0x2b32-0x2be4 | 0x32f0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| setreg | 0x1f34-0x2024 | 0x28ae | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| shortjump | 0x2d92-0x2eaa | 0x213c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| singop | 0x232a-0x2350 | 0x2c4a | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| statline | 0x02e4-0x0310 | 0x0322 | confirmed | analysed | D only (pattern-verified) | D2, D3 |  |
| toofar | 0x2eaa-0x2ef4 | 0x21fe | confirmed | analysed | D only (pattern-verified) | D10, D2, D3, D7, D8/D2 |  |
| usesreg | 0x4468-0x4754 | 0x38d0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| xjump | 0x131e-0x13d2 | 0x100e | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
