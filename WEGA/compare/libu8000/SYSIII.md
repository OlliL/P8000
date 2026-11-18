# libu8000 against its source

No source is known (System III, BSD, the web, the local backups). `disasm.c` is written from the member `disasm.o` of
`/usr/lib/libu8000.a` and `slibu8000.a` (Library.SC: disasm.c 1.2, 01/29/83, "87wega3.2"; the two differ only in the time
of the check-out). The interface is `/usr/include/disasm.h` and disasm(3). The same disassembler is in `/bin/adb` and
`/bin/cas` (the opcode and system call names). Measured 2026-11-18: both members byte-identical from the one source with
ccv1 and sccv1 without -O.

What the object fixes:
- The data area in the order of the object: the static variables (no bss: v1 puts them into data), the format strings and
  the mnemonics as static char arrays, the tables of {name, format} (64 main opcodes, the groups 0x8c/0x8d, the shifts,
  the block translates, 64 floating point opcodes), the name tables (flags, the EPU control registers, 64 system calls, the
  16 condition codes); the switch tables and string literals follow as v1 places them.
- 30 static functions in the order of the object, then `disinit` and `disasm`. Forms: explicit `!= 0` where the object
  has `and`/`cp` and a plain test where it has `bit`; assignments inside conditions (`if ((n = word2 & 017) & 1)`,
  `switch (n = word & 07)`); `case 4: if (...) { ...; break; } break;` and a `case 9:` that falls into `default:`.
- `scc -O` of the shipped compiler did not finish disasm.c within 300 s (libsetup.py); not needed, the original has no -O.
