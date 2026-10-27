# sld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `sld` compared with `/lib/sld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 100

| Pairing | Count |
|---|---|
| confirmed | 73 |
| open | 27 |

| Difference class | Count |
|---|---|
| - | 27 |
| D only (manual reading) | 6 |
| D only (pattern-verified) | 64 |
| other differences | 3 |

**Pending findings:** 24

| Status | Count |
|---|---|
| open | 24 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x0774-0x07a2 | 0x1664 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| copy_leafname | 0x0618-0x06ba | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| ensure_buf | 0x1358-0x13c4 | 0x7112 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f0018 | f0018..0056 | 0x0018 | open | open |  |  |  |
| f0178 | 0x05be-0x0618 | 0x0178 | confirmed | differs | D only (pattern-verified) | D1, D10, D7 |  |
| f0220 | f0220..06c6 | 0x0220 | open | open |  |  |  |
| f0762 | 0x0094-0x00be | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| f078c | f078c..0a5c | 0x078c | open | open |  |  |  |
| f0b04 | f0b04..0f26 | 0x0b04 | open | open |  |  |  |
| f1070 | 0x24ca-0x2628 | 0x1070 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D6, D7, D8, D8/D2 |  |
| f11ea | f11ea..1364 | 0x11ea | open | open |  |  |  |
| f1364 | 0x2296-0x23bc | 0x1364 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| f1560 | 0x218c-0x2296 | 0x1560 | confirmed | differs | D only (pattern-verified) | D1, D10, D6, D7, D8, D8/D2 |  |
| f1696 | f1696..18f2 | 0x1696 | open | open |  |  |  |
| f18f2 | f18f2..1f96 | 0x18f2 | open | open |  |  |  |
| f1f96 | 0x1462-0x152e | 0x1f96 | confirmed | differs | D only (pattern-verified) | D1, D10, D7 |  |
| f2056 | f2056..22a4 | 0x2056 | open | open |  |  |  |
| f23fe | 0x13c4-0x1462 | 0x23fe | confirmed | differs | D only (pattern-verified) | D1, D10, D7, D8 |  |
| f2492 | f2492..2624 | 0x2492 | open | open |  |  |  |
| f2624 | f2624..27d8 | 0x2624 | open | open |  |  |  |
| f27d8 | f27d8..2cbc | 0x27d8 | open | open |  |  |  |
| f2d4c | 0x0d70-0x0df2 | 0x2d4c | confirmed | differs | D only (pattern-verified) | D10, D2, D6 |  |
| f2dde | 0x0550-0x05be | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| f2e76 | 0x0df2-0x0e82 | 0x2e76 | confirmed | differs | D only (manual reading) | D2 | arithmetic shift of the count with another register (r5 here, r9 in the original) |
| f2f04 | 0x051e-0x0550 | 0x2f04 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f3090 | 0x0194-0x01a8 | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| f3144 | f3144..3408 | 0x3144 | open | open |  |  |  |
| f3516 | f3516..3760 | 0x3516 | open | open |  |  |  |
| f3760 | f3760..3a30 | 0x3760 | open | open |  |  |  |
| f3a88 | 0x0262-0x02a0 | 0x3a88 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f3b58 | 0x0c58-0x0ce8 | 0x3b58 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f3c16 | f3c16..410c | 0x3c16 | open | open |  |  |  |
| f410c | f410c..42f8 | 0x410c | open | open |  |  |  |
| f42f8 | 0x1efa-0x204e | 0x42f8 | confirmed | differs | other differences |  | unexplained instruction forms: ld	@rrS,#N; ld	A,#N |
| f44e6 | f44e6..4968 | 0x44e6 | open | open |  |  |  |
| f4968 | 0x1cfc-0x1da6 | 0x4968 | confirmed | differs | D only (pattern-verified) | D1, D3, D6, D7 |  |
| f4a16 | f4a16..5a2c | 0x4a16 | open | open |  |  |  |
| f5a2c | f5a2c..5eec | 0x5a2c | open | open |  |  |  |
| f5eec | f5eec..60f0 | 0x5eec | open | open |  |  |  |
| f60f0 | 0x02a0-0x02d8 | 0x60f0 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| f6134 | 0x07a2-0x0884 | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6, D7 |  |
| f620e | f620e..642e | 0x620e | open | open |  |  |  |
| f64d0 | 0x1ac2-0x1b16 | 0x64d0 | confirmed | differs | D only (pattern-verified) | D2 |  |
| f6800 | 0x0bf2-0x0c58 | 0x6800 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f68a2 | f68a2..6a08 | 0x68a2 | open | open |  |  |  |
| f6a08 | 0x030a-0x035a | 0x6a08 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6a58 | f6a58..6b5a | 0x6a58 | open | open |  |  |  |
| f6b5a | 0x035a-0x0388 | 0x6b5a | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6b84 | 0x0388-0x03b2 | 0x6b84 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6bae | 0x0940-0x09f8 | 0x6bae | confirmed | differs | D only (pattern-verified) | D7 |  |
| f6c6a | 0x204e-0x218c | 0x6c6a | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6da8 | 0x09f8-0x0a6a | 0x6da8 | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8, D8/D2 |  |
| f6e1a | 0x11dc-0x12bc | 0x6e1a | confirmed | differs | D only (manual reading) | D8 | byte b[7] read as the low byte of a word load in the original; D8 by reading, no popt pattern |
| f6efe | 0x03b2-0x03fe | 0x6efe | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6f46 | 0x168e-0x170c | 0x6f46 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f6ffe | 0x042c-0x0458 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| f702a | 0x0458-0x048c | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| f7212 | 0x2402-0x2448 | 0x7212 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D6 |  |
| f724e | 0x23bc-0x2402 | 0x724e | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D6 |  |
| f7286 | 0x2448-0x24ca | 0x7286 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |
| f7300 | f7300..7526 | 0x7300 | open | open |  |  |  |
| f7526 | f7526..7808 | 0x7526 | open | open |  |  |  |
| f79ca | f79ca..7bbc | 0x79ca | open | open |  |  |  |
| f7bbc | 0x0b82-0x0bf2 | 0x7bbc | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f7c26 | f7c26..85f8 | 0x7c26 | open | open |  |  |  |
| f85f8 | 0x0884-0x0916 | 0x85f8 | confirmed | differs | D only (manual reading) | D2 | indirect byte store through another register pair (rr2 here, rr4 in the original) |
| finish_output | 0x0ce8-0x0d70 | 0x2cbc | confirmed | exact | D only (pattern-verified) | D10, D7 |  |
| flush_buffer | 0x06ba-0x071a | 0x3ac6 | confirmed | differs | D only (pattern-verified) | D1, D3, D7 |  |
| get_block | 0x1032-0x10a8 | 0x06c6 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7 |  |
| getw_buf | 0x170c-0x17a4 | 0x30a4 | confirmed | differs | D only (manual reading) | D5 | counter fields addressed through a copy of the pointer in the original (constant index form, D5); by reading, no popt pattern |
| high_byte | 0x0166-0x0194 | 0x2e4c | confirmed | exact | D only (pattern-verified) | D7 |  |
| init_entry | 0x0e82-0x0f36 | 0x2346 | confirmed | exact | D only (pattern-verified) | D7 |  |
| init_signals | 0x011c-0x0166 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x0a6a-0x0b0c | 0x642e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| load_header | 0x0f36-0x0fea | 0x14a4 | confirmed | exact | D only (pattern-verified) | D7 |  |
| make_ccl | 0x0b0c-0x0b82 | 0x7956 | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D8/D2 |  |
| next_entry | 0x1600-0x168e | 0x705e | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D7, D8, D8/D2 |  |
| open_tmp | 0x10a8-0x1148 | 0x22a4 | confirmed | differs | D only (pattern-verified) | D10, D7 |  |
| pack_entry | 0x1560-0x1596 | 0x6866 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| pack_name | 0x1148-0x11dc | 0x00ee | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7, D8 |  |
| put_char | 0x0fea-0x1032 | 0x71ca | confirmed | differs | D only (pattern-verified) | D1, D13, D2 |  |
| put_name | 0x12bc-0x1358 | 0x6522 | confirmed | exact | D only (pattern-verified) | D7 |  |
| read_entry_hdr | 0x1942-0x1ac2 | 0x6684 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| read_words | 0x1b16-0x1c4c | 0x0f26 | confirmed | differs | D only (pattern-verified) | D10, D13, D3, D6, D7, D8, D8/D2 |  |
| refill_wbuf | 0x1da6-0x1efa | 0x2f3a | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| report_error | 0x1c4c-0x1cfc | 0x3408 | confirmed | exact | D only (pattern-verified) |  |  |
| report_limit | 0x15e8-0x1600 | 0x7186 | confirmed | exact | D only (pattern-verified) |  |  |
| scan_field | 0x17a4-0x1942 | 0x7808 | confirmed | differs | D only (manual reading) | D5 | addressing of the count and pointer fields through register pairs and constant index (indirect and indexed forms, D5); by reading, no popt pattern |
| sig_handler | 0x00be-0x011c | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| str_compare | 0x020a-0x0262 | 0x3a30 | confirmed | differs | D only (pattern-verified) | D13 |  |
| swab12 | 0x152e-0x1560 | 0x3b1e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| swap_bytes | 0x0916-0x0940 | 0x65c6 | confirmed | differs | D only (manual reading) | D2 | indirect word load through another register pair (rr4 here, rr2 in the original) |
| swap_pair | 0x02d8-0x030a | 0x65ec | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| swap_quad | 0x04b8-0x051e | 0x6620 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| table16_entry | 0x03fe-0x042c | 0x6fd0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| table16_index | 0x071a-0x0774 | 0x70ce | confirmed | differs | D only (pattern-verified) | D1, D10, D16, D2, D3, D7 |  |
| table32_entry | 0x048c-0x04b8 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x01a8-0x020a | 0x34b8 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7 |  |
| unpack_run | 0x0000-0x0094 | 0x0056 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |
| walk_entries | 0x1596-0x15e8 | 0x01d2 | confirmed | exact | D only (pattern-verified) | D7 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| sld-deferred-0220 | deferred (large function) | f0220 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x0220, 1190 bytes, not started |
| sld-deferred-078c | deferred (large function) | f078c | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x078c, 720 bytes, not started |
| sld-deferred-0b04 | deferred (large function) | f0b04 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x0b04, 1058 bytes, not started |
| sld-deferred-1696 | deferred (large function) | f1696 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x1696, 604 bytes, not started |
| sld-deferred-18f2 | deferred (large function) | f18f2 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x18f2, 1700 bytes, not started |
| sld-deferred-2056 | deferred (large function) | f2056 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x2056, 590 bytes, not started |
| sld-deferred-2492 | deferred (large function) | f2492 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x2492, 402 bytes, not started |
| sld-deferred-2624 | deferred (large function) | f2624 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x2624, 436 bytes, not started |
| sld-deferred-27d8 | deferred (large function) | f27d8 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x27d8, 1252 bytes, not started |
| sld-deferred-3144 | deferred (large function) | f3144 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x3144, 708 bytes, not started |
| sld-deferred-3516 | deferred (large function) | f3516 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x3516, 586 bytes, not started |
| sld-deferred-3760 | deferred (large function) | f3760 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x3760, 720 bytes, not started |
| sld-deferred-3c16 | deferred (large function) | f3c16 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x3c16, 1270 bytes, not started |
| sld-deferred-410c | deferred (large function) | f410c | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x410c, 492 bytes, not started |
| sld-deferred-44e6 | deferred (large function) | f44e6 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x44e6, 1154 bytes, not started |
| sld-deferred-4a16 | deferred (large function) | f4a16 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x4a16, 4118 bytes, not started |
| sld-deferred-5a2c | deferred (large function) | f5a2c | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x5a2c, 1216 bytes, not started |
| sld-deferred-5eec | deferred (large function) | f5eec | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x5eec, 516 bytes, not started |
| sld-deferred-620e | deferred (large function) | f620e | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x620e, 544 bytes, not started |
| sld-deferred-7300 | deferred (large function) | f7300 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x7300, 550 bytes, not started |
| sld-deferred-7526 | deferred (large function) | f7526 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x7526, 738 bytes, not started |
| sld-deferred-79ca | deferred (large function) | f79ca | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x79ca, 498 bytes, not started |
| sld-deferred-7c26 | deferred (large function) | f7c26 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x7c26, 2514 bytes, not started |
| sld-1070-count | open (instruction count) | f1070 | open | - | find the one instruction of the original (112 against 111 in our draft) and the block order of the switch | entry 0x1070: 111 instructions against 112 in the original; not explained by a D pattern (D6 and D15 are the known layout and switch forms) |
