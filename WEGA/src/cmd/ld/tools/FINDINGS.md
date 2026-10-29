# ld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `ld` compared with `/bin/ld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 64

| Pairing | Count |
|---|---|
| confirmed | 30 |
| open | 34 |

| Difference class | Count |
|---|---|
| - | 64 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| add16_checked | 089a-08cc | 0x089a | confirmed | findings | read by hand | D3 | findings (ld.c): checked long sum of two unsigned parameters, error(1, p1) when the sum is 0x10000 or more, return of the low word. Three attempts: plain sum (int addition, wrong), signed long (addl needs the casts), casts (long). The instructions are equal except two reloads: the original loads p3 and p2 from the frame after the spill (ld r3,%0004(r15)) although they are in r7 and r6 (D3, redundant reloads). Draft: /mnt/share/cvs/P8000/tmp/tmp/d5/f089a_try3.c |
| copy | 1a12-1a74 | 0x1a12 | confirmed | findings | read by hand | D3 | findings (ld.c): copy of a file to the output buffer (lseek to 0, read 256 bytes, each byte put with f226e, close). Six+ attempts. Everything equal except the reload of the spilled parameter p before the fildes read: the original loads it into r7 (ld r7,p; ld r14,@r7), the draft into r2 (ld r2,p; ld r14,@r2). Also the frame needs two int spares before q (dec 12). Source behaviour of the original: the loop body runs while read returns more than 1 (cp r2,#1; jr gt), so a read that returns exactly 1 byte is not copied (quirk to record in KNOWLEDGE.md). Draft: d5/f1a12_try3.c. Accepted into ld.c as a finding. |
| dseek | 1b38-1c5a | 0x1b38 | confirmed | findings | read by hand | D3, D16 | findings (ld.c): sld dseek(sp, v, n) of ld. Six attempts: the frame and the register assignment (obj r14, cur r13) are right; the long compare of bno with hi is in rr4 in the original and in rr2 here (register choice, D3-like); two tests of the page pointer are loaded through r2 in the original (test @r2) and direct here (test @r13). The bno temporary (attempt 4) made it worse. Draft (attempt 3): /mnt/share/cvs/P8000/tmp/tmp/d5/f1b38_v3.c. Accepted into ld.c as a finding. |
| f0000 | - | 0x0000 | open | generated |  |  |  |
| f0010 | - | 0x0010 | open | generated |  |  |  |
| f0018 | - | 0x0018 | open | generated |  |  |  |
| f009c | - | 0x009c | open | open |  |  |  |
| f0370 | - | 0x0370 | open | open |  |  |  |
| f045e | - | 0x045e | open | open |  |  |  |
| f050c | 050c-057c | 0x050c | open | exact | read by hand |  | exact (2026-10-29, second attempt): append entry to the table tab (22-byte records, index ntab): t0 = x, t1 = v44a6, t2 = v0070 (data), t3 = y, t4..t10 = v66d0, v66d2, v66d4, v66c2, v66c6, v66cc, v66ce. One unused int local. No caller in the table (its callers are through the table or unknown). Globals: ntab (bss 0abe), tab (0ac0), the v-names are guesses; v0070 is in the data (step 5b). |
| f057c | - | 0x057c | open | open |  |  |  |
| f0608 | - | 0x0608 | open | open |  |  |  |
| f0714 | - | 0x0714 | open | open |  |  |  |
| f07b2 | - | 0x07b2 | open | open |  |  |  |
| f08cc | - | 0x08cc | open | open |  |  |  |
| f0b6e | - | 0x0b6e | open | open |  |  |  |
| f0d9c | - | 0x0d9c | open | open |  |  |  |
| f108e | - | 0x108e | open | open |  |  |  |
| f120e | - | 0x120e | open | open |  |  |  |
| f12a8 | - | 0x12a8 | open | open |  |  |  |
| f13ca | - | 0x13ca | open | open |  |  |  |
| f1452 | - | 0x1452 | open | open |  |  |  |
| f172e | - | 0x172e | open | open |  |  |  |
| f194a | - | 0x194a | open | open |  |  |  |
| f1ce2 | - | 0x1ce2 | open | open |  |  |  |
| f1f64 | - | 0x1f64 | open | open |  |  |  |
| f221e | 221e-226e | 0x221e | open | exact | read by hand |  | exact (2026-10-29, first attempt): put of an int into the buffer (high byte, then low byte), parameter order (x, p); x is the first declared parameter (last argument register r7), p the register pointer. Uses the shared struct buf. |
| f26e8 | - | 0x26e8 | open | open |  |  |  |
| f282e | - | 0x282e | open | open |  |  |  |
| f294e | - | 0x294e | open | open |  |  |  |
| f29d6 | - | 0x29d6 | open | open |  |  |  |
| f2b04 | - | 0x2b04 | open | open |  |  |  |
| f2b3a | - | 0x2b3a | open | open |  |  |  |
| f2b92 | - | 0x2b92 | open | open |  |  |  |
| f2bf6 | 2bf6-2bfc | 0x2bf6 | confirmed | exact |  |  |  |
| f2bfc | 2bfc-2c02 | 0x2bfc | confirmed | exact |  |  |  |
| f2c20 | - | 0x2c20 | open | open |  |  |  |
| flush_buffer | 229c-22e8 | 0x229c | open | exact | read by hand |  | exact (2026-10-29, fourth attempt): struct buf { int _fd; int _cnt; char *_ptr; char _buf[256]; }; the parameter is a register variable (r14), n a register variable (r13), one unused int local. Note: the struct is a guess for the layout (fd, cnt, ptr, buffer of 256 bytes), the code only needs the offsets. |
| get | 1c6c-1ce2 | 0x1c6c | confirmed | findings | read by hand | D4 | findings (ld.c): getw-like read of a word from a stream (sld struct stream layout, Plexus-like field order). The body is equal except (1) the parameter home slot: the original spills p (ld @r15,r7), the draft does not (D4); (2) the arguments of the refill call f1b38(p, -1, (long)(bno + 1) << 8): the original passes -1 in r3, the draft in r6, because the callee f1b38 (same file, defined before) fixes the argument registers and the draft is compiled alone. Check in the whole-file compile. Draft: /mnt/share/cvs/P8000/tmp/tmp/d5/f1c6c_v3.c. |
| half | 1c5a-1c6c | 0x1c5a | confirmed | exact |  |  |  |
| high_byte | 1a74-1a9a | 0x1a74 | confirmed | exact |  |  |  |
| ldrsym | 2502-25a2 | 0x2502 | confirmed | exact |  |  |  |
| limit_msg | 2c02-2c20 | 0x2c02 | confirmed | exact |  |  |  |
| lookup_name | 25dc-26ac | 0x25dc | confirmed | findings | read by hand | D3 | findings (symbols.c): sld lookup_name with ld names (cursym, nlflag, lnbuf, local, hshtab). Hash modulus 0x0923 in ld (sld 0x0ffa). Six attempts: the original keeps start (r11) and end (r10) in registers and slot, cur, rec in the frame or scratch; the draft has start and end in the frame and the loop test differs (cp r6,frame). Draft: d5/ln1.c. Accepted as finding. |
| mget | 1a9a-1b0a | 0x1a9a | confirmed | exact | read by hand |  | exact (2026-10-29): sld mget structure with the text stream. Global `struct stream text` at bss 1030 (ptr 1030, bno 1032, nibuf 1034, size 1036, pno 1038; ptr first, as in get). Two unused int locals of the original frame are kept as spare1..spare3 (frame 16). Statement order as in the source: i = n; i >>= 1; r = p. Former name f1a9a. Renamed also: f1c6c -> get (sld get(sp)), f1b38 -> dseek (sld dseek(sp, v, n)). |
| mkfsym | 25a2-25dc | 0x25a2 | confirmed | exact |  |  |  |
| mput | 1b0a-1b38 | 0x1b0a | confirmed | exact |  |  |  |
| next_entry | 2aa6-2b04 | 0x2aa6 | confirmed | findings | read by hand | D3 | findings (symbols.c): sld next_entry with ld's 14-byte entries (exact step). Five attempts (d5/ne1..ne5). Register set differs (the original keeps only e in r12; p is in r1 without a spill). The bound test differs: the original compares p with two words of the table (symtab, symtab+nsym*14) in another order than sld's `p >= symtab && p < symtab + nsym`. Draft: d5/ne1.c. Accepted as finding. |
| open_tmp | 120e-12a8 | 0x120e | confirmed | exact |  |  |  |
| pack_name | 2360-23e2 | 0x2360 | confirmed | findings | read by hand | D3 | findings (copyname.c): sld pack_name with 14-byte steps (ld tables). Six attempts (d5/pn1..pn7). Frame, loops and calls equal; the padding loop test (cnt > 0) is inverted in the layout: the original has jr gt back to the loop and a separate jump to the zero test, the draft jr le over it. Draft: d5/pn3.c. Accepted as finding. |
| pslookup | 26ca-26e8 | 0x26ca | confirmed | exact |  |  |  |
| put_all | 282e-2862 | 0x282e | confirmed | exact |  |  |  |
| put_int_buf | 226e-229c | 0x226e | confirmed | exact |  |  |  |
| put_name | 23e2-249a | 0x23e2 | confirmed | exact |  |  |  |
| put_sym | 27be-282e | 0x27be | confirmed | findings | read by hand | D3 | findings (symbols.c): sld put_sym without the struct copy: ld writes the symbol itself (mput(a, p, 14)) and has no nwsym count. Seven attempts (d5/ps1..ps7). Frame size 16 matches with two spares (ps4). Differences: swap_pair gets b from r6 in the original and from the frame here; the position of cnt (frame 7 in the original, 9 here). Draft: d5/ps4.c. Accepted as finding. |
| read_entry_hdr | 249a-2502 | 0x249a | confirmed | exact |  |  | owner decision: read_entry_hdr is in symbols.c (first function, address 0x249a), not printname.c. Exact before the move (34 instructions). symbols.c is now in address order (read_entry_hdr, lookup_name, slookup, pslookup, put_sym, put_all, warn_sym_redef, next_entry, f2bf6, f2bfc). |
| report_error | 1e86-1f22 | 0x1e86 | confirmed | exact |  |  |  |
| sig_handler | 0042-009c | 0x0042 | open | exact | read by hand |  | exact (2026-10-29, first attempt): exit path: f2c02() if v669a, unlink("l.out"), chmod(v007a, ~umask(0) & 0x1ff) if no error, unlink(v007a) if v007c, _exit(v007c). Globals: v007a (char *, data 0x7a), v007c and v0080 (int, data 0x7c, 0x80), v669a and v66a4 (int, bss). The names are guesses from the use; step 5b checks the bss ones. |
| slookup | 26ac-26ca | 0x26ac | confirmed | exact |  |  |  |
| str_compare | 21f2-221e | 0x21f2 | confirmed | exact |  |  |  |
| symreloc | 2766-27be | 0x2766 | confirmed | exact |  |  |  |
| table_lookup | 1f22-1f64 | 0x1f22 | confirmed | exact |  |  |  |
| unpack_run | 22e8-2360 | 0x22e8 | confirmed | exact |  |  |  |
| warn_sym_redef | 2862-294e | 0x2862 | confirmed | exact |  |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
