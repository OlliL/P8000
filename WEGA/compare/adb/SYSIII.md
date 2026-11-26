# adb against System III and Plexus

`/bin/adb` of WEGA (SCCS lines 08/25/83 and 09/28/83, crt0 09/28/83, the version line "ADB: P8000 1.6") is the adb of
Zilog's ZEUS 3.2 (`/bin/adb` of ZEUS 3.21 has the same SCCS versions, 05/26/84); no source of it is known. It is written
from the binary on the base of Plexus' `cmd/adb/z8000` (the System III adb for the Z8000, 01/07/83). Measured 2026-11-26.

## Lines

Lines without comments and blanks that also stand in the file (System III: `usr/src/cmd/adb/pdp11`).

| File | Plexus file | Equal | System III file | Equal | Of |
|---|---|---:|---|---:|---:|
| access.c | access.c | 72 | access.c | 72 | 137 |
| command.c | command.c | 145 | command.c | 142 | 198 |
| expr.c | expr.c | 170 | expr.c | 166 | 269 |
| findfn.c | findfn.c | 28 | findfn.c | 28 | 183 |
| format.c | format.c | 157 | format.c | 154 | 222 |
| init.c | - | 0 | - | 0 | 75 |
| input.c | input.c | 57 | input.c | 57 | 58 |
| main.c | main.c | 75 | main.c | 77 | 102 |
| message.c | message.c | 32 | message.c | 32 | 40 |
| opset.c | opset.c | 5 | opset.c | 11 | 52 |
| output.c | output.c | 144 | output.c | 143 | 427 |
| pcs.c | pcs.c | 74 | pcs.c | 68 | 78 |
| print.c | print.c | 138 | print.c | 136 | 223 |
| runpcs.c | runpcs.c | 161 | runpcs.c | 161 | 264 |
| setup.c | setup.c | 84 | setup.c | 77 | 233 |
| sym.c | sym.c | 60 | sym.c | 61 | 149 |
| version.c | - | 0 | - | 0 | 1 |
| fpcvt.s | - | 0 | - | 0 | 6 |
| defs.h | defs.h | 78 | defs.h | 66 | 103 |
| glob.h | - | 0 | - | 0 | 44 |
| mac.h | mac.h | 37 | mac.h | 37 | 37 |
| machine.h | machine.h | 11 | machine.h | 9 | 15 |
| mode.h | mode.h | 63 | mode.h | 58 | 65 |
| suser.h | - | 0 | - | 0 | 140 |

## ZEUS's own

- Addresses are `ADDR` (disasm.h: segment and offset); a segmented program (s.out E6xx) or not (E7xx); `<<seg>>offset` in
  expressions and output; the maps carry a segment and change with a message when another segment is addressed
  (`chgtmap`, `chgdmap`, the segment table of the s.out).
- The disassembler is `disasm(3)` of `/usr/lib/libu8000.a` (opset.c only prints its result, `symfn` gives the symbols).
- Symbols: s.out symbol table with long names; a segment symbol table built by setsym (`struct segsym`) and searched
  twice (findfn.c, sym.c); no local symbols.
- The core file: the user block of a segmented kernel (`suser.h`: struct user with the pointers as longs), the
  registers from the state behind `u_state`; `-k` for the kernel (`/wega` and `/dev/kmem`, `/dev/mem` above 64K, or a
  crash dump: kva.c and kpa.c of ps, linked behind libu8000).
- `$c` prints the stack frames from the code (frsize), `$e` the external values, `$f` the EPU registers; `$x`, `$d` the
  input base; hex numbers with `%`; output in hex with `%`.
- The output functions are a `_doprnt` (the oldlibc's, as in ls and csh) with sprintf and the floats.
- All globals that no earlier module names are defined in init.c (with the initialised ones): their order is the
  original's bss.

## Build

The shipped cc -O, separate I and D; the objects in alphabetical order, fpcvt.s last; libu8000 (`disasm.o`, by v1
without -O), kva.o and kpa.o (`src/cmd/ps`, by cc), `/lib/libc.a`.
