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
| D only (pattern-verified) | 82 |
| other differences | 7 |

**Pending findings:** 2

| Status | Count |
|---|---|
| open | 2 |

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 0x160e-0x163c | 0x1664 | confirmed | differs | read by hand | D2, D3 | pass 2026-10-28, compiler: D4 reload of the arguments |
| alloc_resv | 0x491c-0x49ca | 0x4968 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| alloc_segment | 0x5ece-0x60d2 | 0x5eec | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| chk_ranlib | 0x6f2c-0x6fae | 0x6f46 | confirmed | differs | read by hand | D10, D3 | pass 2026-10-28, open: q = p comes after the spill in the original; rr2 copy before the byte test |
| copy | 0x2d8a-0x2df8 | 0x2dde | confirmed | exact | D only (pattern-verified) |  |  |
| copy_blocks | 0x61f0-0x6410 | 0x620e | confirmed | exact | D only (pattern-verified) |  |  |
| copy_leafname | 0x43f8-0x449a | 0x4444 | confirmed | exact | D only (pattern-verified) |  |  |
| copy_segs | 0x6116-0x61f0 | 0x6134 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| cvt_fsym | 0x67ec-0x6852 | 0x6800 | confirmed | exact | D only (pattern-verified) |  |  |
| dmp_p1 | 0x02de-0x03a6 | 0x0334 | confirmed | exact | D only (pattern-verified) |  |  |
| dseek | 0x2ee6-0x303a | 0x2f3a | confirmed | differs | read by hand |  | pass 2026-10-28, open: cur copied to rr4 before each field access in the original; struct pg tried |
| dump_p2 | 0x03a6-0x0510 | 0x03fc | confirmed | exact | D only (pattern-verified) |  |  |
| dump_res | 0x0510-0x0670 | 0x0566 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| dump_sym | 0x01ca-0x02de | 0x0220 | confirmed | exact | D only (pattern-verified) |  |  |
| end_pass1 | 0x449a-0x491c | 0x44e6 | confirmed | exact | D only (pattern-verified) |  |  |
| ensure_buf | 0x70f0-0x7164 | 0x7112 | confirmed | exact | D only (pattern-verified) |  |  |
| enter | 0x6b92-0x6c4e | 0x6bae | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: scratch register |
| finish_output | 0x2c68-0x2cf8 | 0x2cbc | confirmed | exact | D only (pattern-verified) |  |  |
| flush_buffer | 0x3a74-0x3ad0 | 0x3ac6 | confirmed | differs | read by hand | D1, D3 | pass 2026-10-28, open: frame 12 vs 10, otherwise D1 |
| get | 0x304e-0x30ee | 0x30a4 | confirmed | exact | D only (pattern-verified) |  |  |
| get_block | 0x0672-0x070e | 0x06c6 | confirmed | exact | D only (pattern-verified) |  |  |
| getfile | 0x30ee-0x33b2 | 0x3144 | confirmed | exact | D only (pattern-verified) |  |  |
| half | 0x303a-0x304e | 0x3090 | confirmed | exact | D only (pattern-verified) |  |  |
| high_byte | 0x2df8-0x2e22 | 0x2e4c | confirmed | exact | D only (pattern-verified) |  |  |
| index16 | 0x70ac-0x70f0 | 0x70ce | confirmed | exact | D only (pattern-verified) |  |  |
| init_entry | 0x22ee-0x23a6 | 0x2346 | confirmed | exact | D only (pattern-verified) |  |  |
| init_signals | 0x0a68-0x0ab2 | 0x0aba | confirmed | exact | D only (pattern-verified) |  |  |
| ins_block | 0x59da-0x5ece | 0x5a2c | confirmed | differs | read by hand |  | pass 2026-10-28, open: bodies of case 3/4 of switch 1 lie behind the dispatch of switch 2, shared tail jr 0x5a96; case permutations tried (94 -> 82) |
| lay_out | 0x49ca-0x59da | 0x4a16 | confirmed | differs | read by hand |  | pass 2026-10-28, open: arms of the section selection (l1 = s2/s3/s4) lie behind the then-arm of the delta statement in the original (shared store, tail merge); frame/regs ok |
| ldrsym | 0x688c-0x69ec | 0x68a2 | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: q lives in rr2 in the original (popt promotes the auto), ours spills it; p is a register copy, c is char |
| limit_msg | 0x7164-0x717c | 0x7186 | confirmed | exact | D only (pattern-verified) |  |  |
| list_move_after | 0x6410-0x64b2 | 0x642e | confirmed | exact | D only (pattern-verified) |  |  |
| load2 | 0x243a-0x25d0 | 0x2492 | confirmed | differs | read by hand | D8 | pass 2026-10-28, compiler: and r2,#0xFFFE is res r2,#0 in the original |
| load2arg | 0x23a6-0x243a | 0x23fe | confirmed | differs | read by hand | D10 | pass 2026-10-28, compiler: argument evaluation order of load2(0L, q) |
| load2seg | 0x25d0-0x2784 | 0x2624 | confirmed | exact | D only (pattern-verified) |  |  |
| load2td | 0x2784-0x2c68 | 0x27d8 | confirmed | exact | D only (pattern-verified) |  |  |
| load_header | 0x144e-0x150a | 0x14a4 | confirmed | exact | D only (pattern-verified) |  |  |
| load_lib_syms | 0x130e-0x144e | 0x1364 | confirmed | differs | D only (pattern-verified) |  |  |
| lookup_name | 0x6a3c-0x6b3e | 0x6a58 | confirmed | exact | D only (pattern-verified) |  |  |
| main | 0x073a-0x0a0a | 0x078c | confirmed | differs | read by hand | D1, D10, D13, D2, D7, D8, D8/D2 | pass 2026-10-28, compiler: 7 instructions differ (scratch registers rr2/rr4); done: one loop with test at the bottom (goto test), the library call as label lib: in front of next:, restore block in front of the test |
| make_blk | 0x370e-0x39de | 0x3760 | confirmed | exact | D only (pattern-verified) | D10, D3, D6 |  |
| map_sym | 0x6c4e-0x6d8e | 0x6c6a | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: scratch register choice (r7/r6 vs r2/r3) |
| mget | 0x2e22-0x2eb0 | 0x2e76 | confirmed | exact | D only (pattern-verified) |  |  |
| mkfsym | 0x69ec-0x6a3c | 0x6a08 | confirmed | exact | D only (pattern-verified) |  |  |
| mlist_add | 0x40c2-0x42ac | 0x410c | confirmed | differs | read by hand | D10, D3, D6 | pass 2026-10-28, compiler: str_compare second argument goes through rr6 -> rr4 |
| mput | 0x2eb0-0x2ee6 | 0x2f04 | confirmed | exact | D only (pattern-verified) |  |  |
| next_entry | 0x703c-0x70ac | 0x705e | confirmed | exact | D only (pattern-verified) |  |  |
| open_tmp | 0x224c-0x22ee | 0x22a4 | confirmed | exact | D only (pattern-verified) |  |  |
| pack_entry | 0x6852-0x688c | 0x6866 | confirmed | differs | read by hand | D10, D3 | pass 2026-10-28, compiler: D4 |
| pack_name | 0x0098-0x0122 | 0x00ee | confirmed | differs | read by hand | D7 | pass 2026-10-28, compiler: jr gt,body; jr test3 merged by our popt |
| parse_args | 0x1194-0x130e | 0x11ea | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: 7 instructions differ (scratch registers rr2/rr4 of the copies); done: flag = 0 first, locals flag/saved_argc/saved_k/saved_av/base/p, library call as label lib: before next:, case bodies in the code order of the original (f, b/t+consume, default, N, l, M, R), restore: saved_av++; av = saved_av |
| patch_hdr | 0x2cf8-0x2d8a | 0x2d4c | confirmed | exact | D only (pattern-verified) |  |  |
| place_sec | 0x189a-0x1f3e | 0x18f2 | confirmed | exact | D only (pattern-verified) | D3, D6, D8 |  |
| pslookup | 0x6b68-0x6b92 | 0x6b84 | confirmed | exact | D only (pattern-verified) |  |  |
| put_all | 0x6ee4-0x6f2c | 0x6efe | confirmed | exact | D only (pattern-verified) |  |  |
| put_int_buf | 0x3a36-0x3a74 | 0x3a88 | confirmed | exact | D only (pattern-verified) |  |  |
| put_name | 0x6506-0x65aa | 0x6522 | confirmed | exact | D only (pattern-verified) |  |  |
| put_pad | 0x64b2-0x6504 | 0x64d0 | confirmed | exact | D only (pattern-verified) |  |  |
| put_sym | 0x6e00-0x6ee4 | 0x6e1a | confirmed | exact | D only (pattern-verified) |  |  |
| ranlib_scan | 0x150a-0x160e | 0x1560 | confirmed | differs | read by hand | D1, D6 | pass 2026-10-28, open: mult/add register choice (rr4/rr6 vs rr2/rr4), inc r3,#4 order |
| read_entry_hdr | 0x6670-0x67ec | 0x6684 | confirmed | exact | D only (pattern-verified) |  |  |
| read_symbols | 0x163c-0x189a | 0x1696 | confirmed | differs | read by hand | D10, D7, D8 | pass 2026-10-28, open: frame 2 bytes bigger than the original, rest scratch registers |
| read_words | 0x0ed4-0x101e | 0x0f26 | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: original code has the word branch first, the strings have the char branch first (D15); init before open done |
| readhdr | 0x34c0-0x370e | 0x3516 | confirmed | differs | read by hand | D8 | pass 2026-10-28, compiler: res vs and (x & ~1), temporaries; header is a struct, rec[8198] |
| reg_sect_addr | 0x101e-0x1194 | 0x1070 | confirmed | differs | read by hand | D10, D2, D3, D7, D8/D2 | pass 2026-10-28, compiler: original keeps all case bodies before the join code, ours moves the join behind the first case; switch order t,d,b,0,default done |
| report_error | 0x33b2-0x3462 | 0x3408 | confirmed | exact | D only (pattern-verified) |  |  |
| resize_symtab | 0x7008-0x703c | 0x702a | confirmed | exact | D only (pattern-verified) |  |  |
| resv_seg | 0x42ac-0x43f8 | 0x42f8 | confirmed | exact | D only (pattern-verified) | D3, D6 |  |
| scan_m_opt | 0x3bc2-0x40c2 | 0x3c16 | confirmed | differs | read by hand | D10, D13, D2, D3, D6, D7, D8 | pass 2026-10-28, open: the original moves the error block "section already specified" behind the break of case a and shares the tail "ldl rr2,s; call report_error" (popt); a goto stub in the source gets the code right (49 -> 7) but changes the string pool order, so it is not used |
| scan_options | 0x0ab2-0x0ed4 | 0x0b04 | confirmed | differs | read by hand |  | pass 2026-10-28, open: the original has the dispatch (cpir, jp @rr2) right after the entry jump, ours puts the body of case f in front of it (compiler block order); 33 instructions differ, rest equal |
| seg_pad | 0x60d2-0x6116 | 0x60f0 | confirmed | exact | D only (pattern-verified) |  |  |
| segtab_alloc | 0x070e-0x0738 | 0x0762 | confirmed | exact | D only (pattern-verified) |  |  |
| sig_handler | 0x0a0a-0x0a68 | 0x0a5c | confirmed | exact | D only (pattern-verified) |  |  |
| slookup | 0x6b3e-0x6b68 | 0x6b5a | confirmed | exact | D only (pattern-verified) |  |  |
| split_seg | 0x1f3e-0x1ffe | 0x1f96 | confirmed | differs | read by hand | D10 | pass 2026-10-28, compiler: e stays in rr2 in the original (auto promoted into a scratch register) |
| start_output | 0x1ffe-0x224c | 0x2056 | confirmed | differs | read by hand | D10 | pass 2026-10-28, compiler: scratch registers (r0 vs r4) |
| startcode | f0018..0056 | 0x0018 | confirmed | exact |  |  |  |
| str_compare | 0x39de-0x3a36 | 0x3a30 | confirmed | differs | read by hand |  | pass 2026-10-28, compiler: temp register rr2 vs rr4 |
| swab12 | 0x3ad0-0x3b02 | 0x3b1e | confirmed | differs | read by hand | D1, D3 | pass 2026-10-28, compiler: original uses scratch r1/r5 for the counter and temp, one register variable (rr10) |
| swap_bytes | 0x65ac-0x65d6 | 0x65c6 | confirmed | differs | read by hand | D2 | pass 2026-10-28, compiler: D4 reload |
| swap_pair | 0x65d6-0x6608 | 0x65ec | confirmed | differs | read by hand | D10, D3 | pass 2026-10-28, compiler: D3 |
| swap_quad | 0x6608-0x666e | 0x6620 | confirmed | differs | read by hand | D10, D3 | pass 2026-10-28, compiler: D3 |
| symreloc | 0x6d8e-0x6e00 | 0x6da8 | confirmed | differs | read by hand | D10, D13, D7, D8, D8/D2 | pass 2026-10-28, compiler: jump to ret threaded in ours; default case copy |
| symtab_alloc | 0x6fdc-0x7008 | 0x6ffe | confirmed | exact | D only (pattern-verified) |  |  |
| table16_entry | 0x6fae-0x6fdc | 0x6fd0 | confirmed | exact | D only (pattern-verified) |  |  |
| table32_entry | 0x717c-0x71a8 | 0x719e | confirmed | exact | D only (pattern-verified) |  |  |
| table_lookup | 0x3462-0x34c0 | 0x34b8 | confirmed | exact | D only (pattern-verified) |  |  |
| trunc_name | 0x0122-0x017a | 0x0178 | confirmed | differs | read by hand | D1, D10 | pass 2026-10-28, open: frame 12 vs 14, extra copy ldl rr2,rr6; s++ forms tried |
| unpack_run | 0x0000-0x0098 | 0x0056 | confirmed | exact | D only (pattern-verified) |  |  |
| walk_entries | 0x017c-0x01ca | 0x01d2 | confirmed | exact | D only (pattern-verified) |  |  |
| warn_sym_redef | 0x3b02-0x3bc0 | 0x3b58 | confirmed | exact | D only (pattern-verified) |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
| sld-1070-count | open (instruction count) | reg_sect_addr | open | - | find the one instruction of the original (112 against 111 in our draft) and the block order of the switch | entry 0x1070: 111 instructions against 112 in the original; not explained by a D pattern (D6 and D15 are the known layout and switch forms) |
| sld-27d8-layout | open (block placement, semantic check by reading) | load2td | open | - | place the blocks of the table path (0x2b02) and of the stream B refill (0x2882) as in the original; then check the remaining 42 instructions (register choices D2 and the sra form) and run the function on the machine or in a test harness | entry 0x27d8, 1252 bytes: 361 instructions, 42 differ (block placement of 0x2b02 and 0x2882, scratch registers of the key, count and byte temporaries); the classifier reports 1 unexplained form (sra rS,#4); semantics checked by reading only |
