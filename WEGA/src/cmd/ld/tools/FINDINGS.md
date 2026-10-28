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
| D only (manual reading) | 1 |
| D only (pattern-verified) | 78 |
| other differences | 11 |

**Pending findings:** 2

| Status | Count |
|---|---|
| open | 2 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x15dc-0x160a | 0x1664 | confirmed | differs | D only (pattern-verified) | D2, D3 |  |
| alloc_segment | 0x5f0c-0x6110 | 0x5eec | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| copy_blocks | 0x622e-0x644e | 0x620e | confirmed | exact | D only (pattern-verified) |  |  |
| copy_leafname | 0x4452-0x44f4 | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| dmp_p1 | 0x02de-0x03a6 | 0x0334 | confirmed | exact | D only (pattern-verified) |  |  |
| dseek | 0x2f12-0x306c | 0x2f3a | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| dump_p2 | 0x03a6-0x0510 | 0x03fc | confirmed | exact | D only (pattern-verified) |  |  |
| dump_res | 0x0510-0x0670 | 0x0566 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| dump_sym | 0x01ca-0x02de | 0x0220 | confirmed | exact | D only (pattern-verified) |  |  |
| end_pass1 | 0x44f4-0x4976 | 0x44e6 | confirmed | exact | D only (pattern-verified) |  |  |
| ensure_buf | 0x713a-0x71ae | 0x7112 | confirmed | exact | D only (pattern-verified) |  |  |
| f0178 | 0x0122-0x017a | 0x0178 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| f1560 | 0x14d6-0x15dc | 0x1560 | confirmed | differs | D only (pattern-verified) | D1, D10, D6, D8, D8/D2 |  |
| f1f96 | 0x1f10-0x1fe0 | 0x1f96 | confirmed | differs | D only (pattern-verified) | D1, D10 |  |
| f2624 | 0x25b0-0x27b4 | 0x2624 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N) |
| f27d8 | 0x27b4-0x2c94 | 0x27d8 | confirmed | differs | other differences |  | unexplained instruction forms: sra	rS,#4 |
| f2d4c | 0x2d24-0x2db6 | 0x2d4c | confirmed | exact | D only (pattern-verified) |  |  |
| f2dde | 0x2db6-0x2e24 | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| f2f04 | 0x2edc-0x2f12 | 0x2f04 | confirmed | exact | D only (pattern-verified) |  |  |
| f3516 | 0x34f2-0x3768 | 0x3516 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,rrS(#N); rr	rS,#1; sra	rS,#1 |
| f410c | 0x411c-0x4306 | 0x410c | confirmed | differs | D only (pattern-verified) | D10, D3, D6 |  |
| f42f8 | 0x4306-0x4452 | 0x42f8 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| f4968 | 0x4976-0x4a24 | 0x4968 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| f60f0 | 0x6110-0x6154 | 0x60f0 | confirmed | exact | D only (pattern-verified) |  |  |
| f6134 | 0x6154-0x622e | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| f64d0 | 0x64f0-0x6542 | 0x64d0 | confirmed | exact | D only (pattern-verified) |  |  |
| f6800 | 0x683a-0x68a0 | 0x6800 | confirmed | exact | D only (pattern-verified) |  |  |
| f68a2 | 0x68da-0x6a3a | 0x68a2 | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6a08 | 0x6a3a-0x6a8a | 0x6a08 | confirmed | exact | D only (pattern-verified) |  |  |
| f6b5a | 0x6b8c-0x6bb6 | 0x6b5a | confirmed | exact | D only (pattern-verified) |  |  |
| f6b84 | 0x6bb6-0x6be0 | 0x6b84 | confirmed | exact | D only (pattern-verified) |  |  |
| f6bae | 0x6be0-0x6c9c | 0x6bae | confirmed | differs | D only (pattern-verified) |  |  |
| f6c6a | 0x6c9c-0x6dda | 0x6c6a | confirmed | differs | other differences |  | unexplained instruction forms: add	rS,@rrS; ld	@rrS,rS; ld	rS,rrS(#N) |
| f6da8 | 0x6dda-0x6e4c | 0x6da8 | confirmed | differs | D only (pattern-verified) | D10, D13, D7, D8, D8/D2 |  |
| f6e1a | 0x6e4c-0x6f30 | 0x6e1a | confirmed | exact | D only (pattern-verified) |  |  |
| f6efe | 0x6f30-0x6f78 | 0x6efe | confirmed | exact | D only (pattern-verified) |  |  |
| f6f46 | 0x6f78-0x6ffa | 0x6f46 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| f702a | 0x7054-0x7088 | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| finish_output | 0x2c94-0x2d24 | 0x2cbc | confirmed | exact | D only (pattern-verified) |  |  |
| flush_buffer | 0x3ace-0x3b2a | 0x3ac6 | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| get | 0x3080-0x3120 | 0x30a4 | confirmed | exact | D only (pattern-verified) |  |  |
| get_block | 0x0672-0x070e | 0x06c6 | confirmed | exact | D only (pattern-verified) |  |  |
| getfile | 0x3120-0x33e4 | 0x3144 | confirmed | exact | D only (pattern-verified) |  |  |
| half | 0x306c-0x3080 | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| high_byte | 0x2e24-0x2e4e | 0x2e4c | confirmed | exact | D only (pattern-verified) |  |  |
| index16 | 0x70f8-0x713a | 0x70ce | confirmed | differs | D only (pattern-verified) | D1, D10, D2, D3 |  |
| init_entry | 0x22d0-0x2388 | 0x2346 | confirmed | exact | D only (pattern-verified) |  |  |
| init_signals | 0x0a68-0x0ab2 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| ins_block | 0x5a34-0x5f0c | 0x5a2c | confirmed | differs | other differences |  | unexplained instruction forms: ld	@rrS,#N; ld	rS,rrS(#N) |
| lay_out | 0x4a24-0x5a34 | 0x4a16 | confirmed | differs | other differences |  | unexplained instruction forms: cp	A,#N |
| limit_msg | 0x71ae-0x71c6 | 0x7186 | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x644e-0x64f0 | 0x642e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| load2 | 0x241a-0x25b0 | 0x2492 | confirmed | differs | D only (pattern-verified) | D8 |  |
| load2arg | 0x2388-0x241a | 0x23fe | confirmed | differs | D only (pattern-verified) | D1, D10, D8 |  |
| load_header | 0x141a-0x14d6 | 0x14a4 | confirmed | exact | D only (pattern-verified) |  |  |
| load_lib_syms | 0x12f4-0x141a | 0x1364 | confirmed | differs | D only (pattern-verified) | D10, D3, D7 |  |
| lookup_name | 0x6a8a-0x6b8c | 0x6a58 | confirmed | exact | D only (pattern-verified) |  |  |
| main | 0x073a-0x0a0a | 0x078c | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D2, D7, D8, D8/D2 |  |
| make_blk | 0x3768-0x3a38 | 0x3760 | confirmed | exact | D only (pattern-verified) | D10, D3, D6 |  |
| mget | 0x2e4e-0x2edc | 0x2e76 | confirmed | exact | D only (pattern-verified) |  |  |
| next_entry | 0x7088-0x70f8 | 0x705e | confirmed | exact | D only (pattern-verified) |  |  |
| open_tmp | 0x222e-0x22d0 | 0x22a4 | confirmed | exact | D only (pattern-verified) |  |  |
| pack_entry | 0x68a0-0x68da | 0x6866 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| pack_name | 0x0098-0x0122 | 0x00ee | confirmed | differs | D only (pattern-verified) | D7 |  |
| parse_args | 0x118e-0x12f4 | 0x11ea | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N; popl	rrS,@rrS |
| place_sec | 0x1868-0x1f10 | 0x18f2 | confirmed | exact | D only (pattern-verified) | D3, D6, D7, D8 |  |
| put_int_buf | 0x3a90-0x3ace | 0x3a88 | confirmed | exact | D only (pattern-verified) |  |  |
| put_name | 0x6544-0x65e8 | 0x6522 | confirmed | exact | D only (pattern-verified) |  |  |
| read_entry_hdr | 0x66ae-0x683a | 0x6684 | confirmed | differs | D only (pattern-verified) | D1, D10, D13, D16, D2, D3, D6, D7, D8, D8/D2 |  |
| read_symbols | 0x160a-0x1868 | 0x1696 | confirmed | differs | D only (pattern-verified) | D10, D7, D8 |  |
| read_words | 0x0ed6-0x1020 | 0x0f26 | confirmed | differs | other differences |  | structure (not a D pattern): order 27 moved |
| reg_sect_addr | 0x1020-0x118e | 0x1070 | confirmed | differs | D only (pattern-verified) | D10, D2, D3, D6, D7, D8, D8/D2 |  |
| report_error | 0x33e4-0x3494 | 0x3408 | confirmed | exact | D only (pattern-verified) |  |  |
| scan_m_opt | 0x3c1c-0x411c | 0x3c16 | confirmed | differs | D only (pattern-verified) | D10, D13, D2, D3, D6, D7, D8 |  |
| scan_options | 0x0ab2-0x0ed6 | 0x0b04 | confirmed | differs | other differences |  | unexplained instruction forms: ld	rS,#N |
| segtab_alloc | 0x070e-0x0738 | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| sig_handler | 0x0a0a-0x0a68 | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| start_output | 0x1fe0-0x222e | 0x2056 | confirmed | differs | D only (pattern-verified) | D10 |  |
| startcode | f0018..0056 | 0x0018 | confirmed | exact |  |  |  |
| str_compare | 0x3a38-0x3a90 | 0x3a30 | confirmed | differs | D only (pattern-verified) |  |  |
| swab12 | 0x3b2a-0x3b5c | 0x3b1e | confirmed | differs | D only (pattern-verified) | D1, D3 |  |
| swap_bytes | 0x65ea-0x6614 | 0x65c6 | confirmed | differs | D only (manual reading) | D2 | indirect word load through another register pair (rr4 here, rr2 in the original) |
| swap_pair | 0x6614-0x6646 | 0x65ec | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| swap_quad | 0x6646-0x66ac | 0x6620 | confirmed | differs | D only (pattern-verified) | D10, D3 |  |
| symtab_alloc | 0x7028-0x7054 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| table16_entry | 0x6ffa-0x7028 | 0x6fd0 | confirmed | differs | D only (pattern-verified) | D1 |  |
| table32_entry | 0x71c6-0x71f2 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x3494-0x34f2 | 0x34b8 | confirmed | exact | D only (pattern-verified) |  |  |
| unpack_run | 0x0000-0x0098 | 0x0056 | confirmed | exact | D only (pattern-verified) |  |  |
| walk_entries | 0x017c-0x01ca | 0x01d2 | confirmed | exact | D only (pattern-verified) |  |  |
| warn_sym_redef | 0x3b5c-0x3c1a | 0x3b58 | confirmed | exact | D only (pattern-verified) |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| sld-1070-count | open (instruction count) | reg_sect_addr | open | - | find the one instruction of the original (112 against 111 in our draft) and the block order of the switch | entry 0x1070: 111 instructions against 112 in the original; not explained by a D pattern (D6 and D15 are the known layout and switch forms) |
| sld-27d8-layout | open (block placement, semantic check by reading) | f27d8 | open | - | place the blocks of the table path (0x2b02) and of the stream B refill (0x2882) as in the original; then check the remaining 42 instructions (register choices D2 and the sra form) and run the function on the machine or in a test harness | entry 0x27d8, 1252 bytes: 361 instructions, 42 differ (block placement of 0x2b02 and 0x2882, scratch registers of the key, count and byte temporaries); the classifier reports 1 unexplained form (sra rS,#4); semantics checked by reading only |
