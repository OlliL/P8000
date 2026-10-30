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
| chk_ranlib | 294e-29d6 | 0x294e | confirmed | findings | read by hand | D3 | findings (2026-10-30): an unused register variable (the original saves r14 without using it, quirk); remaining: the original reloads p from the frame for q = p and copies q2 to r2 for the compare (tried four forms) |
| copy | exact | 0x1a12 | confirmed | exact |  |  |  |
| dseek | 1b38-1c5a | 0x1b38 | confirmed | findings | read by hand | D2 | findings (2026-10-30): rewritten: hi = v >> 8 (no mask, arithmetic shift as the original), assignment expressions (cur = &page[...])->..., the read block as if-branch with else report_error, (obj->nibuf = cur->nibuf - lo) <= 0 in long arithmetic; remaining: the second bno load goes through r6 in the original and r3 here |
| end_pass1 | exact | 0xb6e | confirmed | exact | read by hand | D3, D16 | approximation (step 5 of HOWTO, after six attempts not reached): the function is in the file to allow the bss analysis; the body is not reproduced exactly (see the note of the function). |
| ensure_buf | 2b3a-2b92 | 0x2b3a | confirmed | findings | read by hand | D11 | findings (2026-10-30): else return f2b92(n); (the original returns the value of f2b92); remaining: our popt copies the return of rlbuf into the first branch |
| enter | exact | 0x26e8 | confirmed | exact |  |  |  |
| f0000 | - | 0x0000 | open | generated |  |  |  |
| f0010 | - | 0x0010 | open | generated |  |  |  |
| f0018 | - | 0x0018 | open | generated |  |  |  |
| f050c | 050c-057c | 0x050c | open | exact | read by hand |  | exact (2026-10-29, second attempt): append entry to the table tab (22-byte records, index ntab): t0 = x, t1 = v44a6, t2 = v0070 (data), t3 = y, t4..t10 = v66d0, v66d2, v66d4, v66c2, v66c6, v66cc, v66ce. One unused int local. No caller in the table (its callers are through the table or unknown). Globals: ntab (bss 0abe), tab (0ac0), the v-names are guesses; v0070 is in the data (step 5b). |
| f057c | 057c-0608 | 0x057c | confirmed | exact |  |  |  |
| f221e | 221e-226e | 0x221e | open | exact | read by hand |  | exact (2026-10-29, first attempt): put of an int into the buffer (high byte, then low byte), parameter order (x, p); x is the first declared parameter (last argument register r7), p the register pointer. Uses the shared struct buf. |
| f2b92 | 2b92-2bf6 | 0x2b92 | confirmed | exact |  |  |  |
| f2bf6 | 2bf6-2bfc | 0x2bf6 | confirmed | exact |  |  |  |
| f2bfc | 2bfc-2c02 | 0x2bfc | confirmed | exact |  |  |  |
| f2c20 | 2c20-2c4c | 0x2c20 | confirmed | exact |  |  |  |
| finish_output | 194a-1a12 | 0x194a | confirmed | exact |  |  |  |
| flush_buffer | 229c-22e8 | 0x229c | open | exact | read by hand |  | exact (2026-10-29, fourth attempt): struct buf { int _fd; int _cnt; char *_ptr; char _buf[256]; }; the parameter is a register variable (r14), n a register variable (r13), one unused int local. Note: the struct is a guess for the layout (fd, cnt, ptr, buffer of 256 bytes), the code only needs the offsets. |
| get | exact | 0x1c6c | confirmed | exact |  |  |  |
| getfile | exact | 0x1ce2 | confirmed | exact |  |  |  |
| half | 1c5a-1c6c | 0x1c5a | confirmed | exact |  |  |  |
| high_byte | 1a74-1a9a | 0x1a74 | confirmed | exact |  |  |  |
| index16 | 2b04-2b3a | 0x2b04 | confirmed | exact |  |  |  |
| init_entry | 12a8-13ca | 0x12a8 | confirmed | findings | read by hand | D3 | findings (2026-10-30): the original still knows r3 = hdr after the join of x66b8 \|\| x66ba, ours reloads hdr; tried the ?: form (worse) |
| ldrsym | 2502-25a2 | 0x2502 | confirmed | exact |  |  |  |
| limit_msg | 2c02-2c20 | 0x2c02 | confirmed | exact |  |  |  |
| load2 | exact | 0x1452 | confirmed | exact |  |  |  |
| load2arg | 13ca-1452 | 0x13ca | confirmed | exact |  |  |  |
| load2td | 172e-1926 | 0x172e | confirmed | findings | read by hand | D15 | findings (2026-10-30): rewritten from the original: the copy w14 = w13 right after the swap block, the first switch only with x66b8 \|\| x66ba, if (x66ba) w12 = swap(w12) after the symbol branch, the flag output byte-wise (unsigned char), nibuf < 0 branches first (behaviour fixes); remaining: our popt moves the then-block of if (flag) up, the original keeps it in place (same instructions); ch is a two-byte array in the frame (guess, gives the original offset 0x12) |
| load_header | 0714-07b2 | 0x0714 | confirmed | exact |  |  |  |
| load_lib_syms | 0608-0714 | 0x0608 | confirmed | exact |  |  |  |
| lookup_name | exact | 0x25dc | confirmed | exact |  |  |  |
| main | exact | 0x009c | confirmed | exact |  |  |  |
| mget | 1a9a-1b0a | 0x1a9a | confirmed | exact | read by hand |  | exact (2026-10-29): sld mget structure with the text stream. Global `struct stream text` at bss 1030 (ptr 1030, bno 1032, nibuf 1034, size 1036, pno 1038; ptr first, as in get). Two unused int locals of the original frame are kept as spare1..spare3 (frame 16). Statement order as in the source: i = n; i >>= 1; r = p. Former name f1a9a. Renamed also: f1c6c -> get (sld get(sp)), f1b38 -> dseek (sld dseek(sp, v, n)). |
| mkfsym | 25a2-25dc | 0x25a2 | confirmed | exact |  |  |  |
| mput | 1b0a-1b38 | 0x1b0a | confirmed | exact |  |  |  |
| next_entry | exact | 0x2aa6 | confirmed | exact |  |  |  |
| open_tmp | 120e-12a8 | 0x120e | confirmed | exact |  |  |  |
| pack_name | exact | 0x2360 | confirmed | exact |  |  |  |
| parse_args | exact | 0x45e | confirmed | exact |  |  |  |
| place_sec | 0d9c-108e | 0x0d9c | confirmed | findings | read by hand | D2 | findings (2026-10-30): rewritten from the original: areas[0].kind/tsize inside if (tab42b4[0]), q->tsize = q->tend - r8 inside if (tab42b4[j + 1]), ?: assignments, register order q, r8, j (all behaviour or source fixes); remaining: the original copies r3 to r2 before the and of r8 = (secorg[j] - q->tsize) & 0xff00 (tried r8 &= 0xff00 and casts) |
| pslookup | 26ca-26e8 | 0x26ca | confirmed | exact |  |  |  |
| put_all | 282e-2862 | 0x282e | confirmed | exact |  |  |  |
| put_int_buf | 226e-229c | 0x226e | confirmed | exact |  |  |  |
| put_name | 23e2-249a | 0x23e2 | confirmed | exact |  |  |  |
| put_sym | exact | 0x27be | confirmed | exact |  |  |  |
| ranlib_scan | 07b2-089a | 0x07b2 | confirmed | findings | read by hand | D2 | findings (2026-10-30): the address &rltab[i] of the bound test is computed in rr4 by the original and in rr2 by ours; tried: end > rltab + i, &rltab[i] < end, end > &rltab[i] (no change) |
| read_entry_hdr | 249a-2502 | 0x249a | confirmed | exact |  |  | owner decision: read_entry_hdr is in symbols.c (first function, address 0x249a), not printname.c. Exact before the move (34 instructions). symbols.c is now in address order (read_entry_hdr, lookup_name, slookup, pslookup, put_sym, put_all, warn_sym_redef, next_entry, f2bf6, f2bfc). |
| read_symbols | finding | 0x8cc | confirmed | findings | read by hand | D1, D11, D3 | findings (2026-10-30): & ~1 removed (the original only halves); the remaining differences: our cc reserves one more save slot (frame 0x1c against 0x1a; with the spare word all local offsets are equal), our popt copies the short return 0 of goto done into the jump site (the original jumps to the end; the loop exit becomes jp instead of jr), and the original has a dead ldl rr4,rr2 in the common-symbol branch |
| readhdr | exact | 0x1f64 | confirmed | exact |  |  |  |
| reg_sect_addr | exact | 0x0370 | confirmed | exact |  |  |  |
| report_error | 1e86-1f22 | 0x1e86 | confirmed | exact |  |  |  |
| sig_handler | 0042-009c | 0x0042 | open | exact | read by hand |  | exact (2026-10-29, first attempt): exit path: f2c02() if v669a, unlink("l.out"), chmod(v007a, ~umask(0) & 0x1ff) if no error, unlink(v007a) if v007c, _exit(v007c). Globals: v007a (char *, data 0x7a), v007c and v0080 (int, data 0x7c, 0x80), v669a and v66a4 (int, bss). The names are guesses from the use; step 5b checks the bss ones. |
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
