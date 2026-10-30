# ld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `ld` compared with `/bin/ld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 62

| Pairing | Count |
|---|---|
| confirmed | 55 |
| open | 7 |

| Difference class | Count |
|---|---|
| - | 62 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | exact | 0x089a | confirmed | exact |  |  |  |
| chk_ranlib | 294e-29d6 | 0x294e | confirmed | findings | read by hand | D3 | Remaining (12 lines): the original reloads p from the frame for q = p (so the p + n product goes to rr4) and copies q2 to r2 for the stype compare. Tried: 5 drafts on 2026-10-29 (d5/cr2..cr6), on 2026-10-30 q = &p[0], q2 assigned in the if, && instead of the nested if: no change. Kept: an unused register variable (the original saves r14 and never uses it, quirk table). |
| copy | exact | 0x1a12 | confirmed | exact |  |  |  |
| dseek | 1b38-1c5a | 0x1b38 | confirmed | findings | read by hand | D2 | Remaining (4 lines): the second bno load goes through r6 in the original, through r3 in ours. Tried: 6 drafts on 2026-10-29 (d5/f1b38_v1..v4). Fixed on the way (behaviour): hi = v >> 8 without a mask (arithmetic shift as the original); source: the assignment expressions (cur = &page[...])->..., the read block as if-branch with else report_error, (obj->nibuf = cur->nibuf - lo) <= 0 in long arithmetic, the order hi, lo, nw. |
| end_pass1 | exact | 0xb6e | confirmed | exact |  |  |  |
| ensure_buf | 2b3a-2b92 | 0x2b3a | confirmed | findings | read by hand | D11 | Remaining (3 lines): our popt copies the return of rlbuf into the end of the first branch, the original jumps to the shared return. Tried: 10 drafts on 2026-10-29 (d5/eb7..eb10, both test orders). Fixed on the way: else return f2b92(n); (the original returns the value of f2b92). |
| enter | exact | 0x26e8 | confirmed | exact |  |  |  |
| f0000 | - | 0x0000 | open | generated |  |  |  |
| f0010 | - | 0x0010 | open | generated |  |  |  |
| f0018 | - | 0x0018 | open | generated |  |  |  |
| f050c | 050c-057c | 0x050c | open | exact |  |  |  |
| f057c | 057c-0608 | 0x057c | confirmed | exact |  |  |  |
| f221e | 221e-226e | 0x221e | open | exact |  |  |  |
| f2b92 | 2b92-2bf6 | 0x2b92 | confirmed | exact |  |  |  |
| f2bf6 | 2bf6-2bfc | 0x2bf6 | confirmed | exact |  |  |  |
| f2bfc | 2bfc-2c02 | 0x2bfc | confirmed | exact |  |  |  |
| f2c20 | 2c20-2c4c | 0x2c20 | confirmed | exact |  |  |  |
| finish_output | 194a-1a12 | 0x194a | confirmed | exact |  |  |  |
| flush_buffer | 229c-22e8 | 0x229c | open | exact |  |  |  |
| get | exact | 0x1c6c | confirmed | exact |  |  |  |
| getfile | exact | 0x1ce2 | confirmed | exact |  |  |  |
| half | 1c5a-1c6c | 0x1c5a | confirmed | exact |  |  |  |
| high_byte | 1a74-1a9a | 0x1a74 | confirmed | exact |  |  |  |
| index16 | 2b04-2b3a | 0x2b04 | confirmed | exact |  |  |  |
| init_entry | 12a8-13ca | 0x12a8 | confirmed | findings | read by hand | D3 | Remaining (6 lines): the original still knows r3 = hdr after the join of x66b8 \|\| x66ba and stores hy through it, ours reloads hdr from the frame. Tried: 6 drafts on 2026-10-29 (d5/ie1..ie5), on 2026-10-30 hy = cond ? x1066 : 0 (worse). |
| ldrsym | 2502-25a2 | 0x2502 | confirmed | exact |  |  |  |
| limit_msg | 2c02-2c20 | 0x2c02 | confirmed | exact |  |  |  |
| load2 | exact | 0x1452 | confirmed | exact |  |  |  |
| load2arg | 13ca-1452 | 0x13ca | confirmed | exact |  |  |  |
| load2td | 172e-1926 | 0x172e | confirmed | findings | read by hand | D15 | Remaining (28 lines, same instructions): our popt moves the then-block of if (flag) up in front of the switches, the original keeps it in place. Guess: ch is a two-byte array in the frame (gives the offset 0x12 of the original). Tried: 9 drafts on 2026-10-29 (d5/ltd1..ltd9), on 2026-10-30 four declaration orders of flag/ch. Fixed on the way (behaviour): the copy w14 = w13 right after the swap block (the two switches test the unchanged word), the first switch only with x66b8 \|\| x66ba, if (x66ba) w12 = swap(w12) after the symbol branch, the flag output byte-wise; source: nibuf < 0 branches first. |
| load_header | 0714-07b2 | 0x0714 | confirmed | exact |  |  |  |
| load_lib_syms | 0608-0714 | 0x0608 | confirmed | exact |  |  |  |
| lookup_name | exact | 0x25dc | confirmed | exact |  |  |  |
| main | exact | 0x009c | confirmed | exact |  |  |  |
| mget | 1a9a-1b0a | 0x1a9a | confirmed | exact |  |  |  |
| mkfsym | 25a2-25dc | 0x25a2 | confirmed | exact |  |  |  |
| mput | 1b0a-1b38 | 0x1b0a | confirmed | exact |  |  |  |
| next_entry | exact | 0x2aa6 | confirmed | exact |  |  |  |
| open_tmp | 120e-12a8 | 0x120e | confirmed | exact |  |  |  |
| pack_name | exact | 0x2360 | confirmed | exact |  |  |  |
| parse_args | exact | 0x45e | confirmed | exact |  |  |  |
| place_sec | 0d9c-108e | 0x0d9c | confirmed | findings | read by hand | D2 | Remaining (5 lines): in case t the original copies r3 to r2 before the and of r8 = (secorg[j] - q->tsize) & 0xff00. Tried: r8 &= 0xff00 as its own statement, casts (unsigned), (int): no change or worse. Fixed on the way (behaviour): areas[0].kind/tsize only inside if (tab42b4[0]), q->tsize = q->tend - r8 only inside if (tab42b4[j + 1]); source: ?: assignments, register order q, r8, j, the b case as two statements. |
| pslookup | 26ca-26e8 | 0x26ca | confirmed | exact |  |  |  |
| put_all | 282e-2862 | 0x282e | confirmed | exact |  |  |  |
| put_int_buf | 226e-229c | 0x226e | confirmed | exact |  |  |  |
| put_name | 23e2-249a | 0x23e2 | confirmed | exact |  |  |  |
| put_sym | exact | 0x27be | confirmed | exact |  |  |  |
| ranlib_scan | 07b2-089a | 0x07b2 | confirmed | findings | read by hand | D2 | Remaining (8 lines): the address &rltab[i] of the bound test is computed in rr4 by the original, in rr2 by ours (scratch register choice). Tried: 6 forms on 2026-10-29 (d5/rs1..rs7), on 2026-10-30 end > rltab + i, &rltab[i] < end, end > &rltab[i]: no change. |
| read_entry_hdr | 249a-2502 | 0x249a | confirmed | exact |  |  |  |
| read_symbols | finding | 0x8cc | confirmed | findings | read by hand | D1, D11, D3 | Remaining (15 lines): (1) frame 0x1c against 0x1a: our cc reserves one save slot more; with one spare word in front all local offsets are equal (D1); (2) our popt copies the short return 0 of goto done to the jump site, the original jumps to the end, so the loop exit is jp instead of jr (D11); (3) a dead ldl rr4,rr2 of the original in the common-symbol branch (D3). Tried: 7 drafts on 2026-10-29 (d8/rs_1..rs_3); without the spare (all offsets -2), the putc expressions as fputc (no change of the frame). Fixed on the way: & ~1 removed from the halving of hdrsz + segtsz. |
| readhdr | exact | 0x1f64 | confirmed | exact |  |  |  |
| reg_sect_addr | exact | 0x0370 | confirmed | exact |  |  |  |
| report_error | 1e86-1f22 | 0x1e86 | confirmed | exact |  |  |  |
| sig_handler | 0042-009c | 0x0042 | open | exact |  |  |  |
| slookup | 26ac-26ca | 0x26ac | confirmed | exact |  |  |  |
| start_output | 108e-120e | 0x108e | confirmed | exact |  |  |  |
| str_compare | 21f2-221e | 0x21f2 | confirmed | exact |  |  |  |
| symreloc | 2766-27be | 0x2766 | confirmed | exact |  |  |  |
| table16_entry | 29d6-2aa6 | 0x29d6 | confirmed | exact |  |  |  |
| table_lookup | 1f22-1f64 | 0x1f22 | confirmed | exact |  |  |  |
| unpack_run | 22e8-2360 | 0x22e8 | confirmed | exact |  |  |  |
| warn_sym_redef | 2862-294e | 0x2862 | confirmed | exact |  |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
