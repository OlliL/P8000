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
| add16_checked | 089a-08cc | 0x089a | confirmed | findings | read by hand | D3 | findings (ld.c): checked long sum of two unsigned parameters, error(1, p1) when the sum is 0x10000 or more, return of the low word. Three attempts: plain sum (int addition, wrong), signed long (addl needs the casts), casts (long). The instructions are equal except two reloads: the original loads p3 and p2 from the frame after the spill (ld r3,%0004(r15)) although they are in r7 and r6 (D3, redundant reloads). Draft: /mnt/share/cvs/P8000/tmp/tmp/d5/f089a_try3.c |
| chk_ranlib | 294e-29d6 | 0x294e | confirmed | findings | read by hand | D3 | findings (symbols.c): sld chk_ranlib (ranlib entries of 12 bytes). Five attempts (d5/cr2..cr6, draft cr5). Loop, calls (pslookup, warn_sym_redef) and the frame size are equal with one spare int; the original stores q = p from a reload of the parameter (frame) and keeps a register variable (r14, not identified). Accepted as finding. |
| copy | 1a12-1a74 | 0x1a12 | confirmed | findings | read by hand | D3 | findings (ld.c): copy of a file to the output buffer (lseek to 0, read 256 bytes, each byte put with f226e, close). Six+ attempts. Everything equal except the reload of the spilled parameter p before the fildes read: the original loads it into r7 (ld r7,p; ld r14,@r7), the draft into r2 (ld r2,p; ld r14,@r2). Also the frame needs two int spares before q (dec 12). Source behaviour of the original: the loop body runs while read returns more than 1 (cp r2,#1; jr gt), so a read that returns exactly 1 byte is not copied (quirk to record in KNOWLEDGE.md). Draft: d5/f1a12_try3.c. Accepted into ld.c as a finding. |
| dseek | 1b38-1c5a | 0x1b38 | confirmed | findings | read by hand | D3, D16 | findings (ld.c): sld dseek(sp, v, n) of ld. Six attempts: the frame and the register assignment (obj r14, cur r13) are right; the long compare of bno with hi is in rr4 in the original and in rr2 here (register choice, D3-like); two tests of the page pointer are loaded through r2 in the original (test @r2) and direct here (test @r13). The bno temporary (attempt 4) made it worse. Draft (attempt 3): /mnt/share/cvs/P8000/tmp/tmp/d5/f1b38_v3.c. Accepted into ld.c as a finding. |
| end_pass1 | approx | 0xb6e | confirmed | findings | read by hand | D3, D16 | approximation (step 5 of HOWTO, after six attempts not reached): the function is in the file to allow the bss analysis; the body is not reproduced exactly (see the note of the function). |
| ensure_buf | 2b3a-2b92 | 0x2b3a | confirmed | findings | read by hand | D3 | findings (symbols.c): sld ensure_buf in its sld form (if (rlsize < n) with the zero test first, grow by f2b92 otherwise). 30 instructions, 29 equal: the original keeps a jump to the end after the grow call (jr), the draft falls through. The first form (rlsize >= n first) differed in the comparison copy and the branch sense. Draft: d5/eb9.c. Accepted as finding; the attempts in this round went beyond the limit of six (d5/eb7..eb10). |
| enter | 26e8-2766 | 0x26e8 | confirmed | findings | read by hand | D3 | findings (symbols.c): sld enter without the sseg store (ld symbols have no sseg; table size 0x0923). Five attempts (d5/en1..en5). Frame and calls equal with one spare int; the store *a = r is loaded through r2 in the original (ld r3,a; ld @r3,r2), the draft reloads r from r14. Draft: d5/en2.c. |
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
| get | 1c6c-1ce2 | 0x1c6c | confirmed | findings | read by hand | D4 | findings (ld.c): getw-like read of a word from a stream (sld struct stream layout, Plexus-like field order). The body is equal except (1) the parameter home slot: the original spills p (ld @r15,r7), the draft does not (D4); (2) the arguments of the refill call f1b38(p, -1, (long)(bno + 1) << 8): the original passes -1 in r3, the draft in r6, because the callee f1b38 (same file, defined before) fixes the argument registers and the draft is compiled alone. Check in the whole-file compile. Draft: /mnt/share/cvs/P8000/tmp/tmp/d5/f1c6c_v3.c. |
| getfile | 1ce2-1e86 | 0x1ce2 | confirmed | findings | read by hand | D3 | findings (ld.c): sld getfile for ld (nonsegmented): library search with libname ("/lib/libxxxxxxxxxxx"), prefixes /usr and /z/bin, the strncpy count 0x168 - p, v66b4 selects .a or .8. The instruction count is equal (120). Differences: the original keeps three register variables (r12..r14), the draft two; the count of strncpy is computed as constant minus p (sub) and the last branch is inverted. The third prefix is not opened in the original (the final open of filname does it). reloc is a second stream whose address is not settled (pno at 0x1042, step 5b). Five attempts (d5/gf1..gf5, draft d5/gf4.c). |
| half | 1c5a-1c6c | 0x1c5a | confirmed | exact |  |  |  |
| high_byte | 1a74-1a9a | 0x1a74 | confirmed | exact |  |  |  |
| index16 | 2b04-2b3a | 0x2b04 | confirmed | exact |  |  |  |
| init_entry | 12a8-13ca | 0x12a8 | confirmed | findings | read by hand | D3 | findings (ld.c): sld init_entry with the segment record of ld (struct fseg at p, flags x702f, x7030, x7031 as char, x66b8/x66ba for hy). Six attempts (d5/ie1..ie5, draft ie5.c). Equal except the store of hy: the original stores through r7 (hdr in a register), the draft reloads hdr from the frame. Accepted as finding. |
| ldrsym | 2502-25a2 | 0x2502 | confirmed | exact |  |  |  |
| limit_msg | 2c02-2c20 | 0x2c02 | confirmed | exact |  |  |  |
| load2 | 1452-172e | 0x1452 | confirmed | findings | read by hand | D3 | findings (ld.c): sld load2 for ld (one long argument). Nine attempts (d5/l2_1..l2_9, draft l2_9, in ld.c). The loop over the symbols follows the original; the tail (dseek on text and reloc, half() offsets, the base increments of v6bf8, v6bfa, v6bfc) is now equal in the order of the calls; the remaining differences are the frame (2 bytes) and register choices in the loop (rr2 for the sums). Not yet exact. |
| load2arg | 13ca-1452 | 0x13ca | confirmed | exact |  |  |  |
| load2td | 172e-1926 | 0x172e | confirmed | findings | read by hand | D3 | findings (ld.c): sld load2td for ld (nonsegmented words). Nine attempts (d5/ltd1..ltd9, draft ltd9). Frame, the text and reloc reads, the swaps, the switch targets and the exit path are equal in the code; the remaining difference is the form of the first test and the text read of the loop (164 against 168 instructions). Accepted as finding.; word_put is f221e, swap is the libc swap (ld.h), v6bf6 is the open index base. |
| load_header | 0714-07b2 | 0x0714 | confirmed | exact |  |  |  |
| load_lib_syms | 0608-0714 | 0x0608 | confirmed | exact |  |  |  |
| lookup_name | 25dc-26ac | 0x25dc | confirmed | findings | read by hand | D3 | findings (symbols.c): sld lookup_name with ld names (cursym, nlflag, lnbuf, local, hshtab). Hash modulus 0x0923 in ld (sld 0x0ffa). Six attempts: the original keeps start (r11) and end (r10) in registers and slot, cur, rec in the frame or scratch; the draft has start and end in the frame and the loop test differs (cp r6,frame). Draft: d5/ln1.c. Accepted as finding. |
| main | approx | 0x9c | confirmed | findings | read by hand | D3, D16 | approximation (step 5 of HOWTO, after six attempts not reached): the function is in the file to allow the bss analysis; the body is not reproduced exactly (see the note of the function). |
| mget | 1a9a-1b0a | 0x1a9a | confirmed | exact | read by hand |  | exact (2026-10-29): sld mget structure with the text stream. Global `struct stream text` at bss 1030 (ptr 1030, bno 1032, nibuf 1034, size 1036, pno 1038; ptr first, as in get). Two unused int locals of the original frame are kept as spare1..spare3 (frame 16). Statement order as in the source: i = n; i >>= 1; r = p. Former name f1a9a. Renamed also: f1c6c -> get (sld get(sp)), f1b38 -> dseek (sld dseek(sp, v, n)). |
| mkfsym | 25a2-25dc | 0x25a2 | confirmed | exact |  |  |  |
| mput | 1b0a-1b38 | 0x1b0a | confirmed | exact |  |  |  |
| next_entry | 2aa6-2b04 | 0x2aa6 | confirmed | findings | read by hand | D3 | findings (symbols.c): sld next_entry with ld's 14-byte entries (exact step). Five attempts (d5/ne1..ne5). Register set differs (the original keeps only e in r12; p is in r1 without a spill). The bound test differs: the original compares p with two words of the table (symtab, symtab+nsym*14) in another order than sld's `p >= symtab && p < symtab + nsym`. Draft: d5/ne1.c. Accepted as finding. |
| open_tmp | 120e-12a8 | 0x120e | confirmed | exact |  |  |  |
| pack_name | 2360-23e2 | 0x2360 | confirmed | findings | read by hand | D3 | findings (copyname.c): sld pack_name with 14-byte steps (ld tables). Six attempts (d5/pn1..pn7). Frame, loops and calls equal; the padding loop test (cnt > 0) is inverted in the layout: the original has jr gt back to the loop and a separate jump to the zero test, the draft jr le over it. Draft: d5/pn3.c. Accepted as finding. |
| parse_args | approx | 0x45e | confirmed | findings | read by hand | D3, D16 | approximation (step 5 of HOWTO, after six attempts not reached): the function is in the file to allow the bss analysis; the body is not reproduced exactly (see the note of the function). |
| place_sec | 0d9c-108e | 0x0d9c | confirmed | findings | read by hand | D3 | findings (ld.c): ld place_sec (no parameters; areas[4] at 0x7004 with kind, tstart, tsize, tend; secorg, tab42b4). Four attempts (d5/ps_1..ps_4, draft ps_4): all branches, messages and stores are in, 233 against 242 instructions. The differences are the code of the kind and the zero-or-total ternaries (order of the immediate loads) and the overlap loop start. Accepted as finding. |
| pslookup | 26ca-26e8 | 0x26ca | confirmed | exact |  |  |  |
| put_all | 282e-2862 | 0x282e | confirmed | exact |  |  |  |
| put_int_buf | 226e-229c | 0x226e | confirmed | exact |  |  |  |
| put_name | 23e2-249a | 0x23e2 | confirmed | exact |  |  |  |
| put_sym | 27be-282e | 0x27be | confirmed | findings | read by hand | D3 | findings (symbols.c): sld put_sym without the struct copy: ld writes the symbol itself (mput(a, p, 14)) and has no nwsym count. Seven attempts (d5/ps1..ps7). Frame size 16 matches with two spares (ps4). Differences: swap_pair gets b from r6 in the original and from the frame here; the position of cnt (frame 7 in the original, 9 here). Draft: d5/ps4.c. Accepted as finding. |
| ranlib_scan | 07b2-089a | 0x07b2 | confirmed | findings | read by hand | D3 | findings (ld.c): sld ranlib_scan with rltab (word at 0x6c00, a pointer) and rlcnt (0x6c02). Six attempts (d5/rs1..rs7, draft rs3). All calls and the frame (sv, t, end, i) are equal; the bound test rltab + i < end is computed through r3 in the draft and through r5 in the original (no form tried gave r5). Accepted as finding. |
| read_entry_hdr | 249a-2502 | 0x249a | confirmed | exact |  |  | owner decision: read_entry_hdr is in symbols.c (first function, address 0x249a), not printname.c. Exact before the move (34 instructions). symbols.c is now in address order (read_entry_hdr, lookup_name, slookup, pslookup, put_sym, put_all, warn_sym_redef, next_entry, f2bf6, f2bfc). |
| read_symbols | approx | 0x8cc | confirmed | findings | read by hand | D3, D16 | approximation (step 5 of HOWTO, after six attempts not reached): the function is in the file to allow the bss analysis; the body is not reproduced exactly (see the note of the function). |
| readhdr | 1f64-21f2 | 0x1f64 | confirmed | findings | read by hand | D3 | findings (ld.c): sld readhdr in its nonsegmented form (one argument: b is not used in ld). Six attempts (d5/rh_1..rh_6, draft rh_5). Equal in most of the code: header, segment records (16 bytes), counters c0..c2 from the atr bits of record 1, rounding for the nonsegmented case, flags x66b8/x66ba/x66b6. Open: the two error blocks (Bad format, Invalid segment attributes) are placed inline; the draft stores hdr fields in another order at the end. Accepted as finding; the draft has no separate store to fcdrel/fcbrel beyond the globals fcdrel (0x66d2) and fcbrel (0x66d4). |
| reg_sect_addr | 0370-045e | 0x0370 | confirmed | findings | read by hand | D3 | findings (ld.c): sld reg_sect_addr for -t, -d, -b with the selector switch (t, d, b, end), tab42b4 (char[4], 0x702e), secorg (int[4], 0x7034), secspec (char, 0x702c). Six attempts (d5/rsa1..rsa6, draft rsa4). Equal code except the frame offset of fmt (6 in the original, 8 here) and the reduced frame. Accepted as finding. |
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
