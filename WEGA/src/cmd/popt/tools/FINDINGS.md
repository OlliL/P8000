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
| absval | 0x1684-0x16a2 | 0x23a0 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3 |  |
| adrlen | 0x2aa0-0x2ade | 0x2352 | confirmed | analysed | D only (pattern-verified) | D13, D3, D7, D8 |  |
| alloc | 0x1594-0x15f2 | 0x2476 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| allocnode |  |  | open |  | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| backjmp | 0x14ba-0x152e | 0x133c | confirmed | analysed | D only (pattern-verified) | D3, D7, D8 |  |
| ccinval | 0x2124-0x213c | 0x2dda | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| cclook | 0x0a76-0x0b4a | 0x0a44 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8 |  |
| cleargroup | 0x1f38-0x1f6c | 0x2c16 | confirmed | analysed | D only (pattern-verified) | D1, D8 |  |
| clearmem | 0x152e-0x1556 |  | open |  |  |  |  |
| clearreg | 0x1720-0x1742 | 0x24d4 | confirmed | analysed | D only (pattern-verified) |  |  |
| codemove | 0x121c-0x1468 | 0x1102 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| comjump | 0x1468-0x14ba | 0x12e6 | confirmed | analysed | D only (pattern-verified) |  |  |
| compare | 0x2512-0x268a | 0x31ec | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| copy | 0x093a-0x09c8 | 0x0964 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3 |  |
| decref | 0x161a-0x1652 | 0x2416 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| dest | 0x1d08-0x1d6e | 0x29f4 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8/D2 |  |
| dualop | 0x1f92-0x200a | 0x2c72 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| dumpnodes | 0x002a-0x0046 | 0x0610 | confirmed | analysed | D only (pattern-verified) |  |  |
| equop | 0x16a2-0x1720 | 0x23b4 | confirmed | analysed | D only (pattern-verified) | D10, D2, D3, D7, D8, D8/D2 |  |
| equstr | 0x1652-0x1684 | 0x3340 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| findbyte | 0x1810-0x1846 | 0x25b6 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findcon | 0x24c2-0x2512 | 0x3196 | confirmed | analysed | D only (pattern-verified) | D1, D2 |  |
| findcopy | 0x192a-0x1978 | 0x26c2 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7 |  |
| findlong | 0x18b4-0x192a | 0x2656 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| findreg | 0x17ba-0x1810 | 0x255a | confirmed | analysed | D only (pattern-verified) | D1, D2, D7 |  |
| findslot | 0x200a-0x2054 | 0x2cd8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| findword | 0x1846-0x18b4 | 0x25f4 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D5, D7, D8, D8/D2 |  |
| fixldm | 0x4624-0x469a | 0x37cc | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| getch |  |  | open |  |  |  |  |
| getln | 0x0504-0x05f8 | 0x04d6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7, D8 |  |
| getnum | 0x05f8-0x0638 | 0x05d2 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| hashname | 0x09c8-0x0a22 |  | open |  |  |  |  |
| heapreset |  |  | open |  |  |  |  |
| ilen | 0x29aa-0x2aa0 | 0x224c | confirmed | analysed | other differences |  | structure (not a D pattern): order 5 moved |
| input | 0x0312-0x04aa | 0x0358 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| insertl | 0x11a6-0x121c | 0x108c | confirmed | analysed | D only (pattern-verified) | D1 |  |
| isreg | 0x2054-0x2124 | 0x2d22 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D8, D8/D2 |  |
| isused | 0x42ec-0x438e | 0x3838 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D5, D7, D8/D2 |  |
| iterate | 0x0f32-0x1110 | 0x0e1a | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D8 |  |
| itoa_ | 0x381c-0x386c | 0x3c06 | confirmed | analysed | D only (pattern-verified) | D2 |  |
| jswcheck | 0x04aa-0x0504 | 0x02c8 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| jumpsw | 0x27b8-0x2896 | 0x207e | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D7, D8 |  |
| killreg | 0x1d6e-0x1f38 | 0x2a46 | confirmed | analysed | D only (pattern-verified) | D1, D10, D2, D3, D5, D6, D8 |  |
| main | 0x0046-0x02dc | 0x002a | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8 |  |
| makereg | 0x1c90-0x1d08 | 0x2988 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8, D8/D2 |  |
| markregs | 0x438e-0x44cc | 0x432c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| memreg | 0x1978-0x1ba2 | 0x270e | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| movedat | 0x22e6-0x240a | 0x2fc8 | confirmed | analysed | D only (pattern-verified) | D13, D2, D3, D7, D8, D8/D2 |  |
| natural | 0x2776-0x27b8 | 0x3396 | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8 |  |
| newcode1 |  |  | open |  |  |  |  |
| nonlab | 0x15f2-0x161a | 0x244e | confirmed | analysed | D only (pattern-verified) | D1 |  |
| oplook | 0x0b4a-0x0cf8 | 0x0b02 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| opsetup | 0x0a22-0x0a76 | 0x09ec | confirmed | analysed | other differences |  | unexplained instruction forms: push	F,rS |
| output | 0x0638-0x088c | 0x06ba | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| ovlap | 0x386c-0x4000 | 0x3c5a | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| prefix | 0x088c-0x093a | 0x0630 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D7, D8, D8/D2 |  |
| procopt | 0x469a-0x49fe | 0x33d6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| redunbr | 0x240a-0x24c2 | 0x30ec | confirmed | analysed | D only (pattern-verified) | D1, D2, D3, D7, D8/D2 |  |
| refcount | 0x0cf8-0x0f32 | 0x0ca0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| regprefix | 0x213c-0x216c | 0x2df8 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D7 |  |
| rename_regs | 0x44cc-0x4624 | 0x36d0 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D5, D7, D8, D8/D2 |  |
| repladdr | 0x216c-0x22e6 | 0x2e2c | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| reverse | 0x37ba-0x381c | 0x3b9a | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D6, D7, D8, D8/D2 |  |
| rmove | 0x2ade-0x37ba | 0x13b6 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| savereg | 0x1742-0x17ba | 0x24f6 | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8 |  |
| scan |  |  | partly fixed |  |  |  |  |
| setcc | 0x273c-0x2776 | 0x3362 | confirmed | analysed | D only (pattern-verified) | D1, D3 |  |
| setcon | 0x268a-0x273c | 0x32f0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| setreg | 0x1ba2-0x1c90 | 0x28ae | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |
| shortjump | 0x2896-0x2966 | 0x213c | confirmed | analysed | D only (pattern-verified) | D1, D13, D3, D7 |  |
| singop | 0x1f6c-0x1f92 | 0x2c4a | confirmed | analysed | D only (pattern-verified) |  |  |
| statline | 0x02dc-0x0312 | 0x0322 | confirmed | analysed | D only (pattern-verified) | D1 |  |
| toofar | 0x2966-0x29aa | 0x21fe | confirmed | analysed | D only (pattern-verified) | D10, D3, D7 |  |
| usesreg | 0x4000-0x42ec | 0x38d0 | confirmed | analysed | D only (pattern-verified) | D1, D10, D11, D13, D2, D3, D5, D6, D7, D8, D8/D2 |  |
| xjump | 0x1110-0x11a6 | 0x100e | confirmed | analysed | D only (pattern-verified) | D1, D13, D2, D3, D7, D8, D8/D2 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
