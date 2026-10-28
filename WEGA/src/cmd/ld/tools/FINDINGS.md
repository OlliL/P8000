# sld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `sld` compared with `/lib/sld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 91

| Pairing | Count |
|---|---|
| confirmed | 91 |

| Difference class | Count |
|---|---|
| - | 1 |
| D only (manual reading) | 4 |
| D only (pattern-verified) | 73 |
| other differences | 13 |

**Pending findings:** 2

| Status | Count |
|---|---|
| open | 2 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x15c0-0x15ee | 0x1664 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| alloc_segment | 0x5edc-0x60e0 | 0x5eec | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| copy_blocks | 0x61ee-0x640e | 0x620e | confirmed | exact | D only (pattern-verified) |  |  |
| copy_leafname | 0x4422-0x44c4 | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| dmp_p1 | 0x02e6-0x03ae | 0x0334 | confirmed | exact | D only (pattern-verified) |  |  |
| dump_p2 | 0x03ae-0x0518 | 0x03fc | confirmed | exact | D only (pattern-verified) |  |  |
| dump_res | 0x0518-0x0678 | 0x0566 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| dump_sym | 0x01d2-0x02e6 | 0x0220 | confirmed | exact | D only (pattern-verified) |  |  |
| end_pass1 | 0x44c4-0x4946 | 0x44e6 | confirmed | differs | D only (pattern-verified) | D1 |  |
| ensure_buf | 0x713e-0x71b2 | 0x7112 | confirmed | exact | D only (pattern-verified) |  |  |
| f0178 | 0x0124-0x0182 | 0x0178 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| f0762 | 0x06f4-0x071e | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| f1070 | 0x1004-0x1172 | 0x1070 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D6, D7, D8, D8/D2 |  |
| f1364 | 0x12d8-0x13fe | 0x1364 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| f1560 | 0x14ba-0x15c0 | 0x1560 | confirmed | differs | D only (pattern-verified) | D1, D10, D6, D8, D8/D2 |  |
| f1f96 | 0x1ef4-0x1fc4 | 0x1f96 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| f23fe | 0x236e-0x2400 | 0x23fe | confirmed | differs | D only (pattern-verified) | D1, D10, D8 |  |
| f2492 | 0x2400-0x259e | 0x2492 | confirmed | differs | D only (pattern-verified) | D10, D3, D8 |  |
| f2624 | 0x259e-0x27a2 | 0x2624 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| f27d8 | 0x27a2-0x2c82 | 0x27d8 | confirmed | differs | other differences |  | unexplained instruction forms: sra	rS,#4 |
| f2d4c | 0x2d12-0x2d94 | 0x2d4c | confirmed | differs | D only (pattern-verified) | D10, D2, D6 |  |
| f2dde | 0x2d94-0x2e02 | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| f2e76 | 0x2e2c-0x2ebc | 0x2e76 | confirmed | differs | D only (manual reading) | D2 | arithmetic shift of the count with another register (r5 here, r9 in the original) |
| f2f04 | 0x2ebc-0x2eee | 0x2f04 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f3090 | 0x303e-0x3052 | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| f3516 | 0x34bc-0x3732 | 0x3516 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N); rr	rS,#1; sra	rS,#1 |
| f3a88 | 0x3a5a-0x3a98 | 0x3a88 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f3b58 | 0x3b26-0x3bc2 | 0x3b58 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f410c | 0x40d0-0x42ba | 0x410c | confirmed | differs | D only (pattern-verified) | D10, D3, D6 |  |
| f42f8 | 0x42ba-0x4422 | 0x42f8 | confirmed | differs | other differences |  | unexplained instruction forms: ld	@rrS,#N; ld	A,#N |
| f4968 | 0x4946-0x49f4 | 0x4968 | confirmed | differs | D only (pattern-verified) | D1, D3, D6 |  |
| f60f0 | 0x60e0-0x6114 | 0x60f0 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| f6134 | 0x6114-0x61ee | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| f64d0 | 0x64b0-0x6504 | 0x64d0 | confirmed | differs | D only (pattern-verified) | D2 |  |
| f6800 | 0x67fc-0x6862 | 0x6800 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f68a2 | 0x689c-0x69fc | 0x68a2 | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6a08 | 0x69fc-0x6a4c | 0x6a08 | confirmed | exact | D only (pattern-verified) |  |  |
| f6b5a | 0x6b4e-0x6b78 | 0x6b5a | confirmed | exact | D only (pattern-verified) |  |  |
| f6b84 | 0x6b78-0x6ba2 | 0x6b84 | confirmed | exact | D only (pattern-verified) |  |  |
| f6bae | 0x6ba2-0x6c5e | 0x6bae | confirmed | differs | D only (pattern-verified) |  |  |
| f6c6a | 0x6c5e-0x6d9c | 0x6c6a | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6da8 | 0x6d9c-0x6e0e | 0x6da8 | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8, D8/D2 |  |
| f6e1a | 0x6e0e-0x6ef6 | 0x6e1a | confirmed | differs | D only (manual reading) | D8 | byte b[7] read as the low byte of a word load in the original; D8 by reading, no popt pattern |
| f6efe | 0x6ef6-0x6f3e | 0x6efe | confirmed | exact | D only (pattern-verified) |  |  |
| f6f46 | 0x6f3e-0x6fbc | 0x6f46 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f6ffe | 0x6fea-0x7016 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| f702a | 0x7016-0x704a | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| finish_output | 0x2c82-0x2d12 | 0x2cbc | confirmed | exact | D only (pattern-verified) |  |  |
| flush_buffer | 0x3a98-0x3af4 | 0x3ac6 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| get_block | 0x067a-0x06f4 | 0x06c6 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7 |  |
| getw_buf | 0x3052-0x30ea | 0x30a4 | confirmed | differs | D only (manual reading) | D5 | counter fields addressed through a copy of the pointer in the original (constant index form, D5); by reading, no popt pattern |
| high_byte | 0x2e02-0x2e2c | 0x2e4c | confirmed | exact | D only (pattern-verified) |  |  |
| index16 | 0x70e4-0x713e | 0x70ce | confirmed | differs | D only (pattern-verified) | D1, D10, D16, D2, D3, D7 |  |
| init_entry | 0x22b6-0x236e | 0x2346 | confirmed | exact | D only (pattern-verified) |  |  |
| init_signals | 0x0a4e-0x0a98 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| ins_block | 0x5a04-0x5edc | 0x5a2c | confirmed | differs | other differences |  | unexplained instruction forms: ld	@rrS,#N; ld	rS,rrS(#N) |
| lay_out | 0x49f4-0x5a04 | 0x4a16 | confirmed | differs | other differences |  | unexplained instruction forms: cp	A,#N |
| limit_msg | 0x71b2-0x71ca | 0x7186 | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x640e-0x64b0 | 0x642e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| load_header | 0x13fe-0x14ba | 0x14a4 | confirmed | exact | D only (pattern-verified) |  |  |
| lookup_name | 0x6a4c-0x6b4e | 0x6a58 | confirmed | exact | D only (pattern-verified) |  |  |
| main | 0x0720-0x09f0 | 0x078c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D7, D8, D8/D2 |  |
| make_blk | 0x3732-0x3a02 | 0x3760 | confirmed | exact | D only (pattern-verified) | D10, D3, D6 |  |
| next_entry | 0x704a-0x70e4 | 0x705e | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D7, D8, D8/D2 |  |
| open_library | 0x30ea-0x33ae | 0x3144 | confirmed | exact | D only (pattern-verified) |  |  |
| open_tmp | 0x2212-0x22b6 | 0x22a4 | confirmed | differs | D only (pattern-verified) | D10 |  |
| pack_entry | 0x6862-0x689c | 0x6866 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| pack_name | 0x0094-0x0124 | 0x00ee | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7, D8 |  |
| parse_args | 0x1172-0x12d8 | 0x11ea | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N; popl	rrS,@rrS |
| place_sec | 0x184c-0x1ef4 | 0x18f2 | confirmed | exact | D only (pattern-verified) | D3, D6, D7, D8 |  |
| put_name | 0x6506-0x65aa | 0x6522 | confirmed | exact | D only (pattern-verified) |  |  |
| read_entry_hdr | 0x6670-0x67fc | 0x6684 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| read_symbols | 0x15ee-0x184c | 0x1696 | confirmed | differs | D only (pattern-verified) | D10, D7, D8 |  |
| read_words | 0x0eba-0x1004 | 0x0f26 | confirmed | differs | other differences |  | structure (not a D pattern): order 27 moved |
| refill_wbuf | 0x2eee-0x303e | 0x2f3a | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| report_error | 0x33ae-0x345e | 0x3408 | confirmed | exact | D only (pattern-verified) |  |  |
| scan_m_opt | 0x3bc4-0x40d0 | 0x3c16 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N |
| scan_options | 0x0a98-0x0eba | 0x0b04 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N |
| sig_handler | 0x09f0-0x0a4e | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| start_output | 0x1fc4-0x2212 | 0x2056 | confirmed | differs | D only (pattern-verified) | D10 |  |
| startcode | f0018..0056 | 0x0018 | confirmed | exact |  |  |  |
| str_compare | 0x3a02-0x3a5a | 0x3a30 | confirmed | differs | D only (pattern-verified) | D13 |  |
| swab12 | 0x3af4-0x3b26 | 0x3b1e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| swap_bytes | 0x65ac-0x65d6 | 0x65c6 | confirmed | differs | D only (manual reading) | D2 | indirect word load through another register pair (rr4 here, rr2 in the original) |
| swap_pair | 0x65d6-0x6608 | 0x65ec | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| swap_quad | 0x6608-0x666e | 0x6620 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| table16_entry | 0x6fbc-0x6fea | 0x6fd0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| table32_entry | 0x71ca-0x71f6 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x345e-0x34bc | 0x34b8 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| unpack_run | 0x0000-0x0094 | 0x0056 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |
| walk_entries | 0x0184-0x01d2 | 0x01d2 | confirmed | exact | D only (pattern-verified) |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| sld-1070-count | open (instruction count) | f1070 | open | - | find the one instruction of the original (112 against 111 in our draft) and the block order of the switch | entry 0x1070: 111 instructions against 112 in the original; not explained by a D pattern (D6 and D15 are the known layout and switch forms) |
| sld-27d8-layout | open (block placement, semantic check by reading) | f27d8 | open | - | place the blocks of the table path (0x2b02) and of the stream B refill (0x2882) as in the original; then check the remaining 42 instructions (register choices D2 and the sra form) and run the function on the machine or in a test harness | entry 0x27d8, 1252 bytes: 361 instructions, 42 differ (block placement of 0x2b02 and 0x2882, scratch registers of the key, count and byte temporaries); the classifier reports 1 unexplained form (sra rS,#4); semantics checked by reading only |
