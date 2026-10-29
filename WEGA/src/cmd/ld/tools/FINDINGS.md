# ld findings

> **Generated file.** This report is written by `findings_md.py` from `findings.json`. Do not edit it by hand; change `findings.json` and run the script again.

Source: `findings.json` (single source of truth). Functions are the program part of `ld` compared with `/bin/ld`; pending findings are the open items of the analysis.

## Summary

**Functions:** 62

| Pairing | Count |
|---|---|
| confirmed | 11 |
| open | 51 |

| Difference class | Count |
|---|---|
| - | 62 |

**Pending findings:** 0

| Status | Count |
|---|---|

## Functions

| Function | Ours | Original | Pairing | Level | Difference class | Old-cc classes | Note |
|---|---|---|---|---|---|---|---|
| f0000 | - | 0x0000 | open | generated |  |  |  |
| f0010 | - | 0x0010 | open | generated |  |  |  |
| f0018 | - | 0x0018 | open | generated |  |  |  |
| f0042 | 0042-009c | 0x0042 | open | exact | read by hand |  | exact (2026-10-29, first attempt): exit path: f2c02() if v669a, unlink("l.out"), chmod(v007a, ~umask(0) & 0x1ff) if no error, unlink(v007a) if v007c, _exit(v007c). Globals: v007a (char *, data 0x7a), v007c and v0080 (int, data 0x7c, 0x80), v669a and v66a4 (int, bss). The names are guesses from the use; step 5b checks the bss ones. |
| f009c | - | 0x009c | open | open |  |  |  |
| f0370 | - | 0x0370 | open | open |  |  |  |
| f045e | - | 0x045e | open | open |  |  |  |
| f050c | - | 0x050c | open | open |  |  |  |
| f057c | - | 0x057c | open | open |  |  |  |
| f0608 | - | 0x0608 | open | open |  |  |  |
| f0714 | - | 0x0714 | open | open |  |  |  |
| f07b2 | - | 0x07b2 | open | open |  |  |  |
| f089a | - | 0x089a | confirmed | findings | read by hand | D3 | findings (ld.c): checked long sum of two unsigned parameters, error(1, p1) when the sum is 0x10000 or more, return of the low word. Three attempts: plain sum (int addition, wrong), signed long (addl needs the casts), casts (long). The instructions are equal except two reloads: the original loads p3 and p2 from the frame after the spill (ld r3,%0004(r15)) although they are in r7 and r6 (D3, redundant reloads). Draft: /mnt/share/cvs/P8000/tmp/tmp/d5/f089a_try3.c |
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
| f1a12 | - | 0x1a12 | open | open |  |  |  |
| f1a74 | 1a74-1a9a | 0x1a74 | confirmed | exact |  |  |  |
| f1a9a | 1a9a-1b0a | 0x1a9a | open | exact | read by hand |  | exact (2026-10-29, third attempt): buffer read with refill. Globals v1030 (int *, buffer pointer), v1034 and v1036 (counters, int). The statement order matters: i = n; i >>= 1; r = p; (the form of HOWTO step 7). Three unused int locals (frame 16). |
| f1b0a | 1b0a-1b38 | 0x1b0a | confirmed | exact | read by hand |  | exact (2026-10-29, fifth attempt): parameter list (q, p, n), register pointer r and counter i (declared in this order), and two unused int locals (spare1, spare2). The parameter order is the same as f1f22 and f1b0a stands for the rule; the register order (r14 then r13) as in f1f22. |
| f1b38 | - | 0x1b38 | open | open |  |  |  |
| f1c5a | 1c5a-1c6c | 0x1c5a | confirmed | exact |  |  |  |
| f1c6c | - | 0x1c6c | open | open |  |  |  |
| f1ce2 | - | 0x1ce2 | open | open |  |  |  |
| f1e86 | - | 0x1e86 | open | open |  |  |  |
| f1f22 | 1f22-1f64 | 0x1f22 | confirmed | exact | read by hand |  | exact (2026-10-29, fourth and fifth attempt): parameter list (end, x) and three unused int locals (spare). The parameter order is the one of f1b0a: the first declared parameter takes the last argument register. Frame 18 bytes as the original. |
| f1f64 | - | 0x1f64 | open | open |  |  |  |
| f21f2 | 21f2-221e | 0x21f2 | confirmed | exact | read by hand |  | exact (2026-10-29, fifth attempt): for loop, the equal case is the main path (if (*t == *s++) { if (*t++) continue; return(1); } return(0)); parameter list (t, s). The earlier D1/D15 reading was the pointer copy and the branch sense, both resolved by the form. |
| f221e | 221e-226e | 0x221e | open | exact | read by hand |  | exact (2026-10-29, first attempt): put of an int into the buffer (high byte, then low byte), parameter order (x, p); x is the first declared parameter (last argument register r7), p the register pointer. Uses the shared struct _iobuf. |
| f226e | 226e-229c | 0x226e | confirmed | exact | read by hand |  | exact (2026-10-29, fifth attempt): char c (second argument, r7) and register pointer p (r6 -> r14); struct _iobuf { int _x; int _cnt; char *_ptr; } (the displacements of the original: cnt at 2, ptr at 4). The struct member _x is a name guess; the layout is what matters. |
| f229c | 229c-22e8 | 0x229c | open | exact | read by hand |  | exact (2026-10-29, fourth attempt): struct _iobuf { int _fd; int _cnt; char *_ptr; char _buf[256]; }; the parameter is a register variable (r14), n a register variable (r13), one unused int local. Note: the struct is a guess for the layout (fd, cnt, ptr, buffer of 256 bytes), the code only needs the offsets. |
| f22e8 | - | 0x22e8 | open | open |  |  |  |
| f2360 | - | 0x2360 | open | open |  |  |  |
| f23e2 | - | 0x23e2 | open | open |  |  |  |
| f249a | - | 0x249a | open | open |  |  |  |
| f2502 | - | 0x2502 | open | open |  |  |  |
| f25a2 | - | 0x25a2 | open | open |  |  |  |
| f25dc | - | 0x25dc | open | open |  |  |  |
| f26ac | 26ac-26ca | 0x26ac | confirmed | exact | read by hand |  | exact (2026-10-29): the frame difference was one unused 2-byte local (spare), as in HOWTO step 7. The earlier char-parameter theory was wrong. |
| f26ca | 26ca-26e8 | 0x26ca | confirmed | exact | read by hand |  | exact (2026-10-29): the frame difference was one unused 2-byte local (spare), as in HOWTO step 7. The earlier char-parameter theory was wrong. |
| f26e8 | - | 0x26e8 | open | open |  |  |  |
| f2766 | - | 0x2766 | open | open |  |  |  |
| f27be | - | 0x27be | open | open |  |  |  |
| f282e | - | 0x282e | open | open |  |  |  |
| f2862 | - | 0x2862 | open | open |  |  |  |
| f294e | - | 0x294e | open | open |  |  |  |
| f29d6 | - | 0x29d6 | open | open |  |  |  |
| f2aa6 | - | 0x2aa6 | open | open |  |  |  |
| f2b04 | - | 0x2b04 | open | open |  |  |  |
| f2b3a | - | 0x2b3a | open | open |  |  |  |
| f2b92 | - | 0x2b92 | open | open |  |  |  |
| f2bf6 | 2bf6-2bfc | 0x2bf6 | confirmed | exact |  |  |  |
| f2bfc | 2bfc-2c02 | 0x2bfc | confirmed | exact |  |  |  |
| f2c02 | - | 0x2c02 | open | open |  |  |  |
| f2c20 | - | 0x2c20 | open | open |  |  |  |

## Pending findings

| ID | Kind | Function | Status | Classification | Action | Result |
|---|---|---|---|---|---|---|
