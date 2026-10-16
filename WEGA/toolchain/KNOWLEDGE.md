# WEGA tool chain: what we know

Living document. Goal: rebuild the WEGA tool chain (cc, scc, cas, ld, sld,
ar, ranlib, libc, ...) as C that builds on modern Linux and produces
byte-identical objects and binaries. Long-term goal: the whole WEGA OS as
sources. See `HANDOFF_wega_disasm.md` for the original plan.

Facts here come from documents (man pages, WEGA-Dienstprogramme Band A/B)
or from measurements on the original files. Guesses are marked as such.

## Inputs (not in git)

Copyrighted material and original binaries stay out of the repository.
Local copies used in the work:

- WEGA man pages (man0..man7, manM), German.
- WEGA-Dienstprogramme Band A (calling conventions, Teil 7) and Band B
  (cas assembler, Teil 2; C specifics, Teil 5), transcribed by the owner
  (may contain typos).
- The WEGA file tree from the owner's machine, as an original tar and as a
  big-endian binary cpio (uuencoded). Not a stock install, but all tool
  chain files are present. tar and cpio agree byte for byte on every tool
  chain file; only runtime files (utmp, wtmp, logs, core) differ.
- Kernel sources (KERN 3.2, EAW/ZFT 1985-89) with the owner's hand
  decompilations of 17 files (author "O.Lehmann", based on Plexus/V7):
  clist.s mem.c partab.c sys.c tt0.c tty.c acct.c break.c lock.c nami.c
  sig.c subr.c sys2.c sys3.c sys4.c sysent.c ureg.c. These are test input,
  never a reference for byte equality.

Note: the `*.o.asm` kernel listings from 2009 were made with MAME's
debugger disassembler (not with WEGA's adb) and decode segmented code as
non-segmented. Not used.

## Ground rules from the owner

- Not every WEGA binary was rebuilt with each compiler release. Some code
  may never be reproducible with the compiler we have; then several C
  compiler versions must be maintained side by side.
- One subdirectory per binary for the C sources (`ar/`, `cas/`, `ld/`, ...),
  Makefiles on every level, `make all` builds everything.
- Git content in English.

## Files of the tool chain

All binaries are stripped (`SF_STRIP`: no relocation, no symbols).
Size = s_imsize, I&D = combined (E?07) or separate (E?11).

| file | magic | mode | text | data | bss | modules (SCCS ids in the binary) |
|---|---|---|---|---|---|---|
| /bin/cc | E707 | nonseg | 11228 | 2940 | 2192 | driver |
| /bin/scc | E707 | nonseg | 11240 | 2966 | 2192 | driver |
| /lib/cpp | E707 | nonseg | 13064 | 3418 | 23766 | cpp.c 1.5, cpy.y 1.2, yylex.c 1.1 |
| /lib/cparse | E711 | nonseg | 48896 | 18584 | 28804 | cgram1.y 2.5, code1.c, comm1.c, local1.c, optim1.c, pftn1.c, scan1.c, sym1.c, trees1.c, vinit1.c, xdefs1.c, common, optim |
| /lib/scparse | E711 | nonseg | 49152 | 18634 | 28804 | same module set as cparse |
| /lib/codgen | E711 | nonseg | 53760 | 27084 | 17522 | allo3, comm3, lcl3, local3, match3, optim3, order3, rdr3, reader3, regallo3, regs3, table3, table3.nzcc, tr3, wrt3 |
| /lib/scodgen | E711 | nonseg | 54784 | 28730 | 17512 | as codgen, table3.szcc instead of table3.nzcc |
| /lib/gopt | E611 | seg (4 segments) | 65534+... | | 42496 | alias2, beta2, comm2, debug2, dom2, ebbo2, flow2, init2, live2, lpind2, lpinv2, quads2, rdr2, reader2, reg2, wrtil2 |
| /lib/popt, /lib/spopt | E711 | nonseg | 22272 / 22528 | 3308 / 3312 | 3420 | z20.c 1.3, z21.c 1.4, z22.c 1.6, z23.c 1.6 |
| /bin/cas | E611 | seg (segs 4,5,7,8) | 46848 | 38360+1280+1280 | 12328 | parser.y, main.c, getops.c, ifile.c, ofile.c, pst.c, assemble.c, error.c, expr.c, gen.c, instr.c, itable.c, jpr.c, jpr2.c, listing.c, locntr.c, nodes.c, ntable.c, optim.c, p2loop.c, scanner.c, ssymtab.c, stable.c, subr.c, symbols.c, table1.c, table2.c |
| /bin/ld | E711 | nonseg | 18944 | 2748 | 27082 | ld.c 1.18, copyname.c 1.3, printname.c 1.5, symbols.c 1.8, crt0.s |
| /bin/sld | E611 | seg | 38912 | 7090 | 31054+63488 | ld.c, ldm.c, dump.c, ext.c, swap.c, copyname.c, printname.c, symbols.c, malloc.c, scrt0.s |
| /bin/ar | E707 | nonseg | 10060 | 2296 | 1738 | ar.c 2.2, malloc.c 3.3, crt0.s |
| /usr/bin/ranlib | E707 | nonseg | 7124 | 568 | 9560 | ranlib.c 1.1 |
| /bin/nm | E707 | nonseg | 10870 | 1116 | 9278 | nm.c 2.6, malloc.c, crt0.s |
| /bin/size | E707 | nonseg | 5652 | 518 | 5124 | size.c 2.1, crt0.s |
| /usr/bin/strip | E707 | nonseg | 8146 | 886 | 5198 | strip.c 1.2, crt0.s |
| /usr/bin/objhdr, objdu | E707 | nonseg | 6926 / 8684 | 1280 / 1488 | 1036 / 1046 | objhdr.c 1.5, objdu.c 1.3 |

Runtime and libraries: crt0.o, mcrt0.o, scrt0.o, smcrt0.o, libc.a (180
members), slibc.a (185), libm.a / slibm.a (17), liby.a / sliby.a,
libl.a / slibl.a, doprnt.o / sdoprnt.o, cnvt.o / scnvt.o, the full
/usr/include. Each library contains a member `Library.SC` with the SCCS
ids of the member sources (e.g. `crt0.s Rev 3.2`, `fgets.c 2.1`). libc.a
has no `__.SYMDEF` (not ranlib'ed). Missing on the machine: libu8000.a /
slibu8000.a (native disassembler library, disasm(3)), /usr/lib/yaccpar,
lint1/lint2.

Oracle pairs with original source and object: `/usr/sys/dev/{lp,udev1..6}.c`
vs members of `/usr/sys/dev/LIB2`; `/usr/sys/conf/wpar.c` vs `wpar.o`
(built with `scc -O -c`, see `make.wega`); `bstart.c` vs `bstart.o`.
The kernel sources cover most members of LIB1/LIB2. Each kernel object
carries its revision string (e.g. `_breakwstr`).

The kernel assembler sources (mch.s, event.s, fpe.s, ...) are PLZ/ASM
(`as`), not cas syntax. They are still a ground truth for encodings.

## Compiler pipeline (cc(1), scc(1))

- `cc` (Z8002 code): `/lib/cpp` -> `/lib/cparse` -> [`/lib/gopt`, `-Ol`
  loops, `-Or` registers and loops] -> `/lib/codgen` -> [`/lib/popt`, `-O`]
  -> `cas -u -o x.o` -> `ld -X -e start /lib/crt0.o ... /lib/libc.a`.
  `/lib/clist` for `-Sl`. Profiling `-p` uses mcrt0.o. Temp files /tmp/cc.?.
- `scc` (Z8001 code): `/lib/cpp` -> `/lib/scparse` -> `/lib/scodgen` ->
  [`/lib/spopt`] -> `cas -o x.o` -> `sld -X -e start /lib/scrt0.o ...
  /lib/slibc.a`. No global optimizer.
- PCC derived (System III C): void, enum, struct assignment, unsigned char,
  initialised bit fields. `u8000` predefined as 1.
- Register variables r8..r14 (nonseg, 7) / r8..r13 (seg, 6); `register
  char` ignored. int/short 16, long 32, pointer 16 (nonseg) / 32 (seg),
  float 32, double 64 (80 bit in registers).

### Code generator output (strings in scodgen)

The code generator prints cas source through format strings, e.g.:

```
	.psec data / .data / .even / L%d: / .addr L%d     (switch tables)
	.psec / .code
	~L1 := %u / ~L2 := %u                               (frame sizes)
	}	/* %s */                                         (end of local block)
	ldm	%s,_stkseg+%d(fp),#%d / ldm _stkseg+%d(fp),%s,#%d
	ld	r%d,|_stkseg+~L1+%u|(fp)
	inc	fp,#~L2 / add fp,#~L2 / dec fp,#~L2 / sub fp,#~L2
	tcc	eq,A1 ... tcc ugt,A1, tcc feq,A1 ... (float compares)
	.extern	__fltuse
	.line	%d	%s
	extsl rq4 / subl rr4,rr4 / ldk A1,#0
```

So: each function is a local block `{ ... }`, stack frame offsets are
written as `_stkseg+~L1+n` with `fp` (frame pointer register name), in
segmented mode as short-offset indexed addresses `|_stkseg+...|(fp)`.
Floating point uses EPU mnemonics (`fadd`, `fldd`, ...).

## Calling conventions (Band A Teil 7, Band B Teil 5)

- Scratch r0..r7. Safe r8..r14 (nonseg) / r8..r13 (seg).
- Stack pointer r15 (nonseg) / rr14 (seg); optional frame pointer r14 / rr12.
- Arguments left to right into r7, r6, ... r2: the highest free register of
  matching length whose byte subregisters are all free (long -> rr6, rr4,
  rr2); floating point into fr3..fr0. Once one argument is on the stack, all
  following ones are too. Results from r2 upward (long in rr2), fr0 upward.
- Saved registers in the frame as after `sub r15,#2n; ldm @r15,rx,#n`.
- One register serves as frame and stack pointer; no frame chain.
  setjmp/longjmp are replaced by setret/longret.
- Big endian.

## cas assembler (Band B Teil 2, cas(1))

- Two passes, yacc parser (`parser.y`, error "Yacc stack overflow"),
  intermediate file /tmp/as*. Symbols enter the symbol table in order of
  definition.
- Options: `-d` internal labels into the symbol table, `-l` listing,
  `-o objfile`, `-u` undefined symbols become external.
- Labels: `L:` internal, `L::` global, `~L:` local (only inside `{ }`),
  `.extern`, `.comm size,names`. Assignment `:=`, global `::=`.
  Numbers `%FA2E`, `%(8)7726`, `%(2)1001`. Comments `//` and `/* */`.
  Separators blank, tab, `;`. Strings with escapes incl. `\%nn`.
- Operators: unary `+ - ^B ^C ^F ^FD ^FS ^S ^O`, binary `* / ^< ^> ^$ ^| ^X
  + -`, all unsigned 32 bit. `<<seg>>offset` long segmented address,
  `|addr|` short offset.
- Directives: `.seg .nonseg .even .odd .line .comm .extern .code .data .bss
  .psec [name] .csec name .asec .byte .word .long .quad .extend .addr .blkb
  .blkw .blkl`, debugger `.stable .stabn .stabp .stabr`. Start state: data
  area of the unnamed psec. Up to 256 sections.
- `JPR`/`CALLR` pseudo instructions: JR/CALR when the target is in the same
  section and in range, else JP/CALL. Decided early (long) when `.=` is used
  or a difference of two relocatables spans an optimisable jump.
- cas(1) lists as unsupported in this release: floating constants and
  `^F ^FD ^FS`, `.quad .extend .asec .csec`, `.psec` in nonseg mode,
  `.line`, `\` continuation, `^B ^C`. (To be checked against the binary;
  the code generator does emit `.line`.)

## s.out object format (a.out(5), /usr/include/s.out.h)

- Header `struct s_exec`, 24 bytes, big endian: `int s_magic; long s_imsize;
  long s_bss; unsigned s_segt; unsigned s_syms; long s_entry; unsigned
  s_flag; unsigned s_codesz; unsigned s_unused`.
- Magic: E607 / E611 / E605 segmented (combined / separate I&D / overlay),
  E707 / E711 / E705 nonseg, E507 / E511 / E505 8 bit.
- Flags: SF_STRIP 1, SF_OPREP 2, SF_Z8 4, SF_Z80 8, SF_SEND 0x10,
  SF_7FSTK 0x20.
- Segment table at 0x18, `struct segt` 16 bytes: `char sg_segno, sg_coff,
  sg_doff, sg_boff; unsigned sg_code, sg_data, sg_bss; int sg_atr; long
  sg_unused`. sg_atr: CODE 1, DATA 2, BSS 4, STACK 8, OCODE 0x10, ODATA
  0x20, OBSS 0x40, BOUND 0x80.
- Then text of all segments, data of all segments, relocation (same size
  as the image, one word per word, absent if SF_STRIP), symbols.
  Verified: file size = 24 + s_segt + 2 * s_imsize + s_syms for all kernel
  and library objects.
- Relocation word: bit 3 = 1 external: bits 2-0 kind (0 16 bit value /
  offset, 1 segment part, 2 short segmented address, 3 12 bit relative
  CALR, 4 16 bit relative LDR/LDAR), bits 15-4 symbol index. Bit 3 = 0:
  bits 2-1 area (abs, text, data, bss); bit 0 = 1 segmented: bits 15-8
  segment table index, bits 6-4 kind as above.
- In the code, a long segmented address to an external symbol is stored as
  `8000 0000` (segment bit set, offset = addend).
- `struct s_nlist` 14 bytes: `long sn_value; char sn_type; char sn_segt;
  char sn_name[8]`. Long names: `sn_name[0]` has bit 7 set, low 7 bits =
  length including the length byte; the name continues over the following
  entries, zero padded to whole entries. Types N_UNDF 0, N_ABS 1, N_CODE 2,
  N_DATA 3, N_BSS 4, N_REG 0x14, N_SN 0x1E, N_FN 0x1F, flags N_EXT 0x20,
  N_SEG 0x40. Undefined external with value != 0: common of that size.
- The symbol number in an external relocation word counts 14-byte entries,
  so a long name that spills into following entries also uses up their
  numbers (e.g. section name `udev2_s_p` takes two). Verified: with this
  numbering all 13882 external relocations of the libraries point to
  undefined symbols.
- Objects from PLZ/ASM (`as`) carry section name symbols (N_SN) like
  `udev2_s_p` / `udev2_s_d`.
- Segmented C objects (scc) have two segment table entries (code in segment
  0, data in segment 1; data symbols have values 0x01xxxxxx). A
  hand-written cas object (clist.o) has one entry with code and data.

## ar format (ar(5), /usr/include/ar.h)

`ARMAG` 0177545 (0xFF65), then per member `struct ar_hdr { char
ar_name[14]; long ar_date; char ar_uid; char ar_gid; int ar_mode; long
ar_size; }` (26 bytes), data padded to even length. ranlib adds a first
member `__.SYMDEF` (format still to be decoded from ranlib).

## ld / sld (ld(1), sld(1))

- ld: nonseg and 8 bit. Options -b, -d, -e, -i, -l x (/lib, /usr/lib,
  /z/bin/lib), -N s|l, -o, -r, -s, -t, -u, -w, -x, -X (keep locals except
  section names and names starting with `L`). Reserved _etext, _edata, _end.
  Libraries are searched once in order.
- sld: segmented; -M/-R map sections to segments, segments 0..127;
  `__.SYMDEF` is searched repeatedly. Kernel link: `sld -Ns -o wega -e start
  -X -i -Ms62 ...`.

## Z8000 decoder (tools/z8k.py)

Own table, written from the Z8000 encoding rules, segmented and
nonsegmented (in segmented mode address registers are pairs: `@rr2`,
`rr14(#d)`, `rr2(r4)`; I/O indirect stays `@rN`).

Validation:

- Linear sweep over the text of all 469 objects in libc, slibc, libm,
  slibm, liby, libl, LIB1, LIB2, BOOTLIB: 42601 instructions, none
  undecodable; every one of the 17221 relocation entries lands on an
  operand word of the matching role (segment word, offset word, short
  address, CALR opcode, LDR displacement). This checks instruction lengths
  and address forms, including the EPU instructions.
- Against the original PLZ/ASM kernel sources (mch.s, fpe.s): `di vi,nvi` is
  7C00, `ei vi,nvi` 7C04; `rldb rl1,rh1` is BE19 (first operand in nibble
  3); `tccb c,rh1` AE17; `resflg c` 8D83, `resflg p` 8D13; `ldctlb
  rh0,flags` 8C01, `ldctlb flags,rh0` 8C09.
- EPU (floating point) instructions 0E/0F/4E/4F/8E/8F: template word + one
  word (+ address for 4E/4F). Mnemonics not decoded yet; shown as `.word`.

## Tools (tools/)

- `sout.py` s.out reader: header, segment table, symbols, relocations
  (`-r`), hex (`-x`).
- `war.py` ar reader: `list`, `extract`.
- `cpio_be.py` reader for big-endian binary cpio archives.
- `sstrings.py` NUL-terminated strings from the data sections.
- `z8k.py` Z8000 decoder.
- `check_reloc.py` decoder self-check against relocations.
- `sdis.py` symbolic disassembler for objects: labels from the symbol
  table, relocated operands as symbol+addend, synthetic labels for internal
  branch targets, `-l` adds addresses and code words. Output close to the
  code generator's cas syntax, e.g. `ldm _stkseg+2(r15),r10,#4`.

## Observations from disassembly

- scc function prologue/epilogue (lp.o): `dec r15,#n` / `ldm
  _stkseg+2(r15),r10,#4` ... `ldm r10,_stkseg+2(r15),#4` / `inc r15,#n` /
  `ret`. Calls to functions in the same module use `calr`, others `call`
  with a long segmented address.
- LIB2 member udev2.o (16 bytes, jp _unexint) is built from the PLZ/ASM
  file udev2.d.s, not from udev2.c.

## Reference: lcc (WEGA/contrib/lcc)

A later C compiler for WEGA (sources from the Akademie der Wissenschaften
Berlin, ZKI; the owner fixed bugs in it). Not our target: it generates
different code. Useful as a reference for:

- PCC structure: `cmd/comp/common` (cgram.y, scan.c, trees.c, pftn.c,
  reader.c, match.c, allo.c, cost.c, sty.y) and the Z8000 back end
  `cmd/comp/u8k` (local.c, local2.c, table.c). It is a newer PCC (pcc2 with
  sty table generator). The WEGA code generator uses the same template
  language (`AL`, `AR`, `A1`, `U1`, `Z...` escapes, e.g. `\tZtZi\tUL,AR\n`,
  `\tld\tAL,A1\n`), so zzzcode()/local2.c show how such escapes work.
- cas syntax in real use: `lib/libc/u8k` is written in cas (`.seg`,
  `sp := r15`, `SP := rr14`, `{ ... }` blocks, `|_stkseg+4|(sp)`,
  `ret nc`, `jp c,cerror`). A cas test corpus.
- WEGA system call interface (segmented): arguments in r0.. (e.g. `ldm
  r0,|_stkseg+4|(sp),#4` for read), `sc #n` with V7 numbers (exit 1, fork
  2, read 3, break 17, ...), result in r4 (r4/r5), carry set on error with
  errno in r4 and a device error in r5; fork returns twice (`sc #2; jr L1`).
  lcc's own calling convention passes arguments on the stack, unlike scc.
- `cmd/ar.c` (ar format), `cmd/lld.c` (a BSD ld "string table version"
  adapted to s.out, not WEGA's ld).

## Open questions

- Format of `__.SYMDEF`.
- EPU instruction formats and cas mnemonics (fadd, fldd, ...).
- Which cas condition code and flag names to emit (both `ne`/`nz`,
  `c`/`ult` exist).
