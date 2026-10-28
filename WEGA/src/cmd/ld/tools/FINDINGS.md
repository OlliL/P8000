# sld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `sld` compared with `/lib/sld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 105

| Pairing | Count |
|---|---|
| confirmed | 101 |
| open | 4 |

| Difference class | Count |
|---|---|
| - | 11 |
| D only (manual reading) | 6 |
| D only (pattern-verified) | 74 |
| other differences | 14 |

**Pending findings:** 6

| Status | Count |
|---|---|
| open | 6 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x0770-0x079e | 0x1664 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| alloc_segment | 0x4308-0x450c | 0x5eec | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| copy_blocks | 0x450c-0x4730 | 0x620e | confirmed | exact | D only (pattern-verified) | D7 |  |
| copy_leafname | 0x0614-0x06b6 | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| dump_p1 |  | 0x0334 | confirmed | exact |  |  |  |
| dump_p2 |  | 0x03fc | confirmed | exact |  |  |  |
| dump_res |  | 0x0566 | confirmed | exact |  |  |  |
| dump_sym | 0x5dd2-0x5ee6 | 0x0220 | confirmed | exact | D only (pattern-verified) |  |  |
| end_pass1 | 0x6278-0x66fe | 0x44e6 | confirmed | differs | other differences |  | unexplained instruction forms: clr	_; ld	_,rS; ld	rS,_ |
| ensure_buf | 0x1334-0x13a0 | 0x7112 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f0018 | f0018..0056 | 0x0018 | open | open |  |  |  |
| f0178 | 0x05ba-0x0614 | 0x0178 | confirmed | differs | D only (pattern-verified) | D1, D10, D7 |  |
| f0762 | 0x0094-0x00be | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| f1070 | 0x24a2-0x2600 | 0x1070 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D6, D7, D8, D8/D2 |  |
| f1364 | 0x226e-0x2394 | 0x1364 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| f1560 | 0x2164-0x226e | 0x1560 | confirmed | differs | D only (pattern-verified) | D1, D10, D6, D7, D8, D8/D2 |  |
| f18f2 | f18f2..1f96 | 0x18f2 | open | open |  |  |  |
| f1f96 | 0x143e-0x1506 | 0x1f96 | confirmed | differs | D only (pattern-verified) | D1, D10, D7 |  |
| f23fe | 0x13a0-0x143e | 0x23fe | confirmed | differs | D only (pattern-verified) | D1, D10, D7, D8 |  |
| f2492 | 0x28c6-0x2a5c | 0x2492 | confirmed | differs | D only (pattern-verified) | D10, D3, D7, D8 |  |
| f2624 | 0x2a5c-0x2c70 | 0x2624 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| f27d8 | 0x2c70-0x317c | 0x27d8 | confirmed | differs | other differences |  | unexplained instruction forms: sra	rS,#4 |
| f2d4c | 0x0d50-0x0dd2 | 0x2d4c | confirmed | differs | D only (pattern-verified) | D10, D2, D6 |  |
| f2dde | 0x054c-0x05ba | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| f2e76 | 0x0dd2-0x0e62 | 0x2e76 | confirmed | differs | D only (manual reading) | D2 | arithmetic shift of the count with another register (r5 here, r9 in the original) |
| f2f04 | 0x051a-0x054c | 0x2f04 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f3090 | 0x0194-0x01a8 | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| f3516 | 0x317c-0x3412 | 0x3516 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N); rr	rS,#1; sra	rS,#1 |
| f3a88 | 0x0262-0x02a0 | 0x3a88 | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f3b58 | 0x0c3c-0x0ccc | 0x3b58 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f410c | 0x3412-0x35fc | 0x410c | confirmed | differs | D only (pattern-verified) | D10, D3, D6 |  |
| f42f8 | 0x1ed2-0x2026 | 0x42f8 | confirmed | differs | other differences |  | unexplained instruction forms: ld	@rrS,#N; ld	A,#N |
| f4968 | 0x1cd4-0x1d7e | 0x4968 | confirmed | differs | D only (pattern-verified) | D1, D3, D6, D7 |  |
| f4a16 | f4a16..5a2c | 0x4a16 | open | open |  |  |  |
| f5a2c | f5a2c..5eec | 0x5a2c | open | open |  |  |  |
| f60f0 | 0x02a0-0x02d8 | 0x60f0 | confirmed | differs | D only (pattern-verified) | D1, D13, D2, D3, D7, D8/D2 |  |
| f6134 | 0x079e-0x0868 | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6, D7 |  |
| f64d0 | 0x1a9a-0x1aee | 0x64d0 | confirmed | differs | D only (pattern-verified) | D2 |  |
| f6800 | 0x0bd6-0x0c3c | 0x6800 | confirmed | differs | D only (pattern-verified) | D1 |  |
| f68a2 | 0x276a-0x28c6 | 0x68a2 | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6a08 | 0x030a-0x035a | 0x6a08 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6b5a | 0x035a-0x0384 | 0x6b5a | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6b84 | 0x0384-0x03ae | 0x6b84 | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6bae | 0x0924-0x09dc | 0x6bae | confirmed | differs | D only (pattern-verified) | D7 |  |
| f6c6a | 0x2026-0x2164 | 0x6c6a | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6da8 | 0x09dc-0x0a4e | 0x6da8 | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8, D8/D2 |  |
| f6e1a | 0x11bc-0x1298 | 0x6e1a | confirmed | differs | D only (manual reading) | D8 | byte b[7] read as the low byte of a word load in the original; D8 by reading, no popt pattern |
| f6efe | 0x03ae-0x03fa | 0x6efe | confirmed | exact | D only (pattern-verified) | D7 |  |
| f6f46 | 0x1666-0x16e4 | 0x6f46 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7, D8, D8/D2 |  |
| f6ffe | 0x0428-0x0454 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| f702a | 0x0454-0x0488 | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| f7bbc | 0x0b66-0x0bd6 | 0x7bbc | confirmed | differs | D only (pattern-verified) | D1, D10, D3 |  |
| f85f8 | 0x0868-0x08fa | 0x85f8 | confirmed | differs | D only (manual reading) | D2 | indirect byte store through another register pair (rr2 here, rr4 in the original) |
| finish_output | 0x0ccc-0x0d50 | 0x2cbc | confirmed | exact | D only (pattern-verified) | D7 |  |
| flush_buffer | 0x06b6-0x0716 | 0x3ac6 | confirmed | differs | D only (pattern-verified) | D1, D3, D7 |  |
| get_block | 0x1012-0x1088 | 0x06c6 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7 |  |
| getw_buf | 0x16e4-0x177c | 0x30a4 | confirmed | differs | D only (manual reading) | D5 | counter fields addressed through a copy of the pointer in the original (constant index form, D5); by reading, no popt pattern |
| high_byte | 0x0166-0x0194 | 0x2e4c | confirmed | exact | D only (pattern-verified) | D7 |  |
| index16 | 0x0716-0x0770 | 0x70ce | confirmed | differs | D only (pattern-verified) | D1, D10, D16, D2, D3, D7 |  |
| init_entry | 0x0e62-0x0f16 | 0x2346 | confirmed | exact | D only (pattern-verified) | D7 |  |
| init_signals | 0x011c-0x0166 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| ld_doprnt | 0x368c-0x40b0 | 0x7c26 | confirmed | differs | read by hand | D2, D7 | putc macro form (--cnt < 0 ? _flsbuf : (int)(*ptr++ = c)) reproduces the stream writes; 714 instructions equal in count, 60 differ in content: register choice of temporaries (rr4/r3 where ours uses rr2/r4, D2) and block placement of the output block and jump lengths (jr/jp, D7); frame 144 and stack offsets equal; semantics checked by reading only |
| ld_doscan | 0x4730-0x495a | 0x7300 | confirmed | differs | D only (pattern-verified) | D7 |  |
| ld_fprintf | 0x40b0-0x4130 | 0x7a5e | confirmed | exact | D only (pattern-verified) | D7 |  |
| ld_fscanf | 0x2394-0x23da | 0x724e | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D6 |  |
| ld_inum | f7526..7808 | 0x7526 | confirmed | exact |  |  |  |
| ld_printf | 0x35fc-0x368c | 0x79ca | confirmed | exact | D only (pattern-verified) | D7 |  |
| ld_scanf | 0x23da-0x2420 | 0x7212 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D6 |  |
| ld_sprintf | 0x4130-0x4206 | 0x7ae2 | confirmed | exact | D only (pattern-verified) | D7 |  |
| ld_sscanf | 0x2420-0x24a2 | 0x7286 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |
| limit_msg | 0x15bc-0x15d4 | 0x7186 | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x0a4e-0x0af0 | 0x642e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| load_header | 0x0f16-0x0fca | 0x14a4 | confirmed | exact | D only (pattern-verified) | D7 |  |
| lookup_name | 0x4206-0x4308 | 0x6a58 | confirmed | exact | D only (pattern-verified) |  |  |
| main | f078c..0a5c | 0x078c | confirmed | differs |  |  |  |
| make_blocks | 0x517e-0x545a | 0x3760 | confirmed | exact | other differences |  | unexplained instruction forms: ld	rS,_ |
| make_ccl | 0x0af0-0x0b66 | 0x7956 | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D8/D2 |  |
| next_entry | 0x15d4-0x1666 | 0x705e | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3, D7, D8, D8/D2 |  |
| open_library | 0x4e9e-0x517e | 0x3144 | confirmed | exact | other differences |  | unexplained instruction forms: clr	_; ld	A,#N; ld	_,#N |
| open_tmp | 0x1088-0x1128 | 0x22a4 | confirmed | differs | D only (pattern-verified) | D10, D7 |  |
| pack_entry | 0x1538-0x156e | 0x6866 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| pack_name | 0x1128-0x11bc | 0x00ee | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D3, D7, D8 |  |
| parse_args | 0x2600-0x276a | 0x11ea | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N; popl	rrS,@rrS |
| put_char | 0x0fca-0x1012 | 0x71ca | confirmed | differs | D only (pattern-verified) | D1, D13, D2 |  |
| put_name | 0x1298-0x1334 | 0x6522 | confirmed | exact | D only (pattern-verified) | D7 |  |
| read_entry_hdr | 0x191a-0x1a9a | 0x6684 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| read_symbols | 0x4c40-0x4e9e | 0x1696 | confirmed | differs | other differences |  | unexplained instruction forms: ld	_,rS; ld	rS,_ |
| read_words | 0x1aee-0x1c24 | 0x0f26 | confirmed | differs | D only (pattern-verified) | D10, D13, D3, D6, D7, D8, D8/D2 |  |
| refill_wbuf | 0x1d7e-0x1ed2 | 0x2f3a | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| report_error | 0x1c24-0x1cd4 | 0x3408 | confirmed | exact | D only (pattern-verified) |  |  |
| scan_field | 0x177c-0x191a | 0x7808 | confirmed | differs | D only (manual reading) | D5 | addressing of the count and pointer fields through register pairs and constant index (indirect and indexed forms, D5); by reading, no popt pattern |
| scan_m_opt | f3c16..410c | 0x3c16 | confirmed | differs |  |  |  |
| scan_options | f0b04..0f26 | 0x0b04 | confirmed | differs |  |  |  |
| sig_handler | 0x00be-0x011c | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| start_output | 0x545a-0x56d0 | 0x2056 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,_ |
| str_compare | 0x020a-0x0262 | 0x3a30 | confirmed | differs | D only (pattern-verified) | D13 |  |
| swab12 | 0x1506-0x1538 | 0x3b1e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| swap_bytes | 0x08fa-0x0924 | 0x65c6 | confirmed | differs | D only (manual reading) | D2 | indirect word load through another register pair (rr4 here, rr2 in the original) |
| swap_pair | 0x02d8-0x030a | 0x65ec | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| swap_quad | 0x04b4-0x051a | 0x6620 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| table16_entry | 0x03fa-0x0428 | 0x6fd0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| table32_entry | 0x0488-0x04b4 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x01a8-0x020a | 0x34b8 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D7 |  |
| unpack_run | 0x0000-0x0094 | 0x0056 | confirmed | differs | D only (pattern-verified) | D1, D10, D3, D8 |  |
| walk_entries | 0x156e-0x15bc | 0x01d2 | confirmed | exact | D only (pattern-verified) | D10, D7 |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| sld-deferred-18f2 | deferred (large function) | f18f2 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x18f2, 1700 bytes, not started |
| sld-deferred-4a16 | deferred (large function) | f4a16 | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x4a16, 4118 bytes, not started |
| sld-deferred-5a2c | deferred (large function) | f5a2c | open | - | second pass: rebuild after the small and medium functions (large, complex) | entry 0x5a2c, 1216 bytes, not started |
| sld-1070-count | open (instruction count) | f1070 | open | - | find the one instruction of the original (112 against 111 in our draft) and the block order of the switch | entry 0x1070: 111 instructions against 112 in the original; not explained by a D pattern (D6 and D15 are the known layout and switch forms) |
| sld-27d8-layout | open (block placement, semantic check by reading) | f27d8 | open | - | place the blocks of the table path (0x2b02) and of the stream B refill (0x2882) as in the original; then check the remaining 42 instructions (register choices D2 and the sra form) and run the function on the machine or in a test harness | entry 0x27d8, 1252 bytes: 361 instructions, 42 differ (block placement of 0x2b02 and 0x2882, scratch registers of the key, count and byte temporaries); the classifier reports 1 unexplained form (sra rS,#4); semantics checked by reading only |
| sld-7c26-semantics | open (semantic check not done) | ld_doprnt | open | - | run the function against the original (test harness or the whole sld) and compare the output; then close the remaining content differences (register choice of temporaries, block placement of the output block) and re-check the stream, wptr/scnt and float argument logic | entry 0x7c26, 2514 bytes, 714 instructions equal in count, 60 differ in content (order-independent comparison), frame 144 and stack offsets equal; the putc macro form and pointer-typed wptr/ap/dp/sptr are measured against the original; semantics checked by reading only, the external helpers f8684, f8ad0, f943c, f8c86 are not checked against their own definitions |
