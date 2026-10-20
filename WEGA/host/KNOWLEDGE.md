# WEGA tool chain: what we know

## Directory layout

- `WEGA/src/...`  WEGA source code, as it would sit in /usr/src of a WEGA
  system, compiled by the WEGA tools (cc, scc, cas, as, ld, gld, ...) and
  meant to build with WEGA's own `make` as well. Layout as in V7
  (/usr/src/cmd): a command with one source file is a file in `src/cmd/`
  (`ar.c`, `size.c`, ...); a command with several files has its own
  subdirectory with its own Makefile (`doscopy`, `standalone`, `BOOT0`). Where
  several shipped versions of a command exist (`pburn` 1.4/2.1/2.3/2.6,
  `copydisk`, `format`, `verify` 1.5/1.6, `standalone/sa.diags` p8000/p8000c),
  every version has its own subdirectory and the Makefile one level up builds
  the version shipped with WEGA 3.1. Start-up variants are in
  `src/lib/libc/crt/`, libc in `src/lib/libc` (with `oldcc/*.s` for members
  made by the older cc), headers in `src/head`, kernel in `src/uts`. Objects
  and programs are built next to the sources. `make` there needs the tool
  variables (`CC SCC CAS AS LD GLD AR STRIP`); `src/make_cmd_linux.sh` is the
  owner's wrapper that runs `make` with all tools under z8k-emu (it holds the
  owner's local paths; the generic recipe is in `CLAUDE.md`).
- `WEGA/host/...` the Linux side. `z8k-emu` (the oracle that runs the original
  binaries), `tools` (analysis scripts), and the Linux-native cross tools
  `cc`, `scc` (the drivers) and `popt`: their source
  is in the same directory, they behave like the WEGA originals and replace
  the emulator calls later. Check scripts and `Makefile` targets:
  `make -C WEGA/host check` (cc, popt, libc, oldcc). `make -C WEGA/host
  verify-wega` is outdated since the version subdirectories, `GLD`/`SCC` and
  the Makefile changes of 2026-10-18; see `HANDOFF_wega_disasm.md`. The status
  of the programs against the originals is kept in `WEGA/src/TODO.md`.
- `WEGA/contrib/...` third-party and own extras, unchanged.

Living document. Goal: rebuild the WEGA tool chain (cc, scc, cas, as, ld, sld,
gld, ar, ranlib, libc, ...) as C that builds on modern Linux and produces
byte-identical objects and binaries. Long-term goal: the whole WEGA OS as
sources. See `HANDOFF_wega_disasm.md` for the state and the next steps, and
`../CLAUDE.md` for how to compile and where to look things up.

Facts here come from documents (man pages, WEGA-Dienstprogramme Band A/B)
or from measurements on the original files. Guesses are marked as such.

## Inputs (not in git)

Copyrighted material and original binaries stay out of the repository.
Local copies used in the work:

- WEGA man pages (man0..man7, manM), German. They are also inside the file
  tree below as packed files (`usr/man/man1/gld.1.z`); the original `pcat`
  reads them under the oracle: `z8k-emu -r ROOT /usr/bin/pcat
  /usr/man/man1/gld.1`.
- WEGA-Dienstprogramme Band A (calling conventions, Teil 7) and Band B
  (cas assembler, Teil 2; C specifics, Teil 5), transcribed by the owner
  (may contain typos).
- The WEGA file tree from the owner's machine, as an original tar and as a
  big-endian binary cpio (uuencoded). Not a stock install, but all tool
  chain files are present. tar and cpio agree byte for byte on every tool
  chain file; only runtime files (utmp, wtmp, logs, core) differ. This tree
  is the reference for every "identical" in `src/TODO.md`: the version
  subdirectories under `src/cmd` build exactly the versions found here (the
  owner calls them the versions shipped with WEGA 3.1; the SCCS ids in the
  binaries end in `87wega3.2`, the meaning of that suffix is not clarified).
- WEGA 3.1 start and floppy disks (images and raw conversions). The owner
  compared `BOOT0` and `standalone/sa.diags` with them; `sa.diags` is not
  part of the file tree.
- Z8000/Z8001/Z8002 CPU manuals (scans with OCR text) and the books named in
  the old handoff (Leventhal, Moore, Mateosian). Not in git.
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
- V7 layout for `src/cmd` (a flat file for a single-source command, a
  subdirectory with its own Makefile for several files or several versions,
  see "Directory layout"); `make all` builds everything.
- Makefiles keep to what WEGA's own `make` understands: plain `=` assignments
  (no `?=`), `cd dir ; $(MAKE)` instead of `$(MAKE) -C`, no GNU extensions.
  The owner moved them that way in the commits "work on supporting WEGA native
  make" (d914fd9, adabdd8, 2b62862). Variables come from the command line.
- Git content in English.
- Original binaries and copyrighted material stay out of git. Plexus sources
  are private reference only; our decompiled cc and popt are not copies of
  Plexus code.

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
| /bin/gld | E607 | seg | 22924 | 4652 | 4680 | gld.c, malloc.c |
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
member `__.SYMDEF` (entries of 12 bytes, see the ranlib section; not yet
checked against a `__.SYMDEF` that the original ranlib produced).

## ld / sld / gld (ld(1), sld(1), gld(1))

- ld: nonseg and 8 bit. Options -b, -d, -e, -i, -l x (/lib, /usr/lib,
  /z/bin/lib), -N s|l, -o, -r, -s, -t, -u, -w, -x, -X (keep locals except
  section names and names starting with `L`). Reserved _etext, _edata, _end.
  Libraries are searched once in order.
- sld: segmented; -M/-R map sections to segments, segments 0..127;
  `__.SYMDEF` is searched repeatedly. Kernel link: `sld -Ns -o wega -e start
  -X -i -Ms62 ...`.
- Which linker (measured): `scc` links with `/bin/sld`, with `-X -e start -z`. The string `gld` does
  not occur in the `/bin/scc` binary, so `gld` is not part of the `scc` pipeline (the driver `host/cc`
  built as `scc` is checked against the original, 74 command lines identical). `gld` is for very
  large programs and is called by hand (owner). Do not confuse the two (`sld`, not `gld`).
- gld (gld(1), `gld.1 1.0`, "segmented U8001 loader for very large programs"):
  segmented like sld; sections can be placed in logical segments, the
  default mapping is text in segments 0-31, data 32-63, bss 64-95, stack
  segment 128; segments can grow to `0xFFFE` by system calls. Options `-e label`, `-l x` (= `/lib/slibx.a`, then
  `/usr/lib`, `/z/bin/lib`), `-L path`, `-o name`, `-s`, `-t`, `-w`, `-M`
  (prints the library modules loaded). Libraries are searched at the position
  of their name; `__.SYMDEF` is searched repeatedly. No relocation information
  stays in the output (cannot be used for another gld run); no output file
  when references stay unresolved; `_stkseg` is reserved. WEGA programs use
  `-e start` with `/lib/scrt0.o`. Verified use: `pburn` 2.1 is compiled with
  `scc -c` and linked with `gld /lib/scrt0.o <objects> /lib/slibc.a -o pburn`;
  the result is byte-identical to `/bin/pburn`.

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
  word (+ address for 4E/4F). `tools/epugen.py` derives the register-form templates by brute force
  from the original `as -f` (needs the emulator; output `tools/epu_data.py`), `tools/epu.py` decodes
  them (`fld r8,f3` = 8F30 8804: bare `fld` = extended, `flds`/`fldd` = single/double; reg-reg
  forms are size independent). `tools/s2as.py obj.o` writes a PLZ/ASM source (`as -f`) from an
  object; ecvt.o rebuilds byte-identical that way. sdis.py still prints EPU as `.word`.

## Tools (tools/)

- `sout.py` s.out reader: header, segment table, symbols, relocations
  (`-r`), hex (`-x`).
- `war.py` ar reader: `list`, `extract`.
- `cpio_be.py` reader for big-endian binary cpio archives.
- `sstrings.py` NUL-terminated strings from the data sections.
- `z8k.py` Z8000 decoder.
- `check_reloc.py` decoder self-check against relocations.
- `cmpmod.py`, `cmpdata.py`, `cmpyacc.py` compare a module/data/yacc tables with a range of a binary (cas work, see "cas: decompilation").
- `swcmp.py` lists the switches of two segmented programs (case value -> body) and compares them; noisy, see its text.
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

## Oracle: z8k-emu (z8k-emu/)

A user-mode Z8001/Z8002 interpreter in C that runs the original WEGA
executables on Linux. It is the reference ("oracle") for everything we
reconstruct: run the original tool, run our rebuilt tool, compare the output
byte for byte.

    z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]

`ROOT` is the extracted WEGA disk image; absolute guest paths are looked up
below it (`/dev/*` goes to the host, `ROOT/tmp` is created). `-t` traces every
instruction, `-v` reports lazily allocated segments.

Results (all byte-identical to the objects shipped with WEGA, see
`z8k-emu/selftest.sh`, `make check-oracle`):

- `scc -O -c` reproduces kernel LIB2 members lp, bio, mem, partab, sys, md, ud,
  fd and `conf/wpar.o`.
- `cc -c` and `cc -O -c` both reproduce `conf/bstart.o`.
- Of the owner's hand decompilations, `mem.c`, `partab.c` and `sys.c` compile to
  objects identical to the shipped ones (so those three are in fact exact).
- `tty.c` differs (our copy is the owner's revised "4.2+" source), `udev*.o` in
  LIB2 are only dummy objects (`dev/Makefile`: "use dummy objects").
- `cc -O -o hello hello.c` links and the result runs (exit status 5 as expected),
  so the whole pipeline cpp, cparse, codgen, popt, cas, ld works unmodified.
- ar, size run correctly; nothing in the tool chain needed more than the
  system calls listed below.
- The whole `src/cmd` builds under the oracle (`CC SCC CAS AS LD GLD AR STRIP`
  all run as `z8k-emu -r ROOT <tool>`); the per-program results are in
  `src/TODO.md`. The segmented chain `scc` -> `gld` (E607/E611, `gld` called by hand) reproduces
  `pburn` 2.1 byte for byte. `make` itself runs on the host, not in the
  emulator.
- Paths: absolute guest paths are looked up below ROOT, relative ones refer
  to the host working directory (so builds happen in place in `src/cmd`);
  guest `/tmp` is `ROOT/tmp`. A file outside ROOT and outside the working
  directory is invisible to the guest.

### Loader

- Magic E707/E607 combined, E711/E611 separate I&D. For non-segmented E711 the
  instruction and data spaces are separate 64K spaces (data at 0).
- File layout: header (24), segment table (16 per entry), text of all segments in
  table order, data of all segments. Per segment: code at `coff*256`, data
  right behind the code (or at `doff*256` if set), bss behind the data.
  Combined binaries place data directly after the text (no padding) - the
  rebuilt `ar` and `size` ran correctly with this, so it is right.
- Initial stack (non-segmented, top 0xFFFE): `argc`, `argv[]`, NULL, `envp[]`,
  NULL, then the strings, argv[0] first (crt0 clears the word below argv[0]).
  Segmented: same with 32-bit pointers `seg<<24|off`, stack segment 0x7F (flag
  7FSTK); SP is rr14 = (0x7F00, offset).
- Entry: `s_entry` (`seg<<24|off` for segmented).

### System call ABI (verified in libc/slibc stubs and in the kernel sources)

- `sc #n` is the word `7F00|n`. Numbers from `sys/sysent.c` (System III based,
  e.g. 1 exit, 2 fork, 3 read, 4 write, 5 open, 6 close, 7 wait, 8 creat, 11 exec,
  17 break, 19 seek, 28 fstat, 38 fcntl, 41 dup, 42 pipe, 45 ssgbrk, 48 signal,
  59 exece). `fcntl` (38: fd, cmd, arg) passes straight to the host `fcntl`
  (added 2026-10-18).
- The kernel reads the arguments from the saved registers r0, r1, ... in order
  (`machdep.c: scfunc`): a 16-bit value takes one register, a 32-bit value two,
  a pointer one (non-segmented) or two (segmented: segment word `0sssssss 00000000`,
  offset). The libc stubs shuffle the compiler's argument registers (r7, r6, r5/rr4,
  r3 ...) into r0.. first, e.g. `lseek(fd, long off, whence)`: r0 fd, rr1(r1,r2) off,
  r3 whence.
- Result in r4 (r5 for a second value: wait = pid, status; pipe = two fds; lseek,
  time = rr4). Error: carry set, error number in r4. `cerror` stores it in `_errno`.
- `fork`: the stub is `sc #2; jr L; <parent code>`; the child continues at the
  `jr` (r4 = parent value, carry clear), the parent skips the `jr` (r4 = child pid).
- `signal` stub installs per-signal trampolines (`calr` table) and passes the
  trampoline address (or 0/1) to the kernel, which returns the old value in r4.
- `ssgbrk(seg, nbytes)` grows a segment in 256 byte clicks and returns
  (seg<<8, old size). `break` in segmented mode takes a segmented address.
- `struct stat` is 26 bytes: dev, ino, mode, nlink, uid, gid, rdev (16 bit each),
  size, atime, mtime, ctime (32 bit each), big endian.

### Not yet emulated

- EPU (Z8070 floating point) arithmetic. Only EPU loads/stores between memory
  and the EPU registers are implemented (as plain data moves), which is all
  varargs prologues (`4FFC 8x03 off`: store fr3..fr0) need. WEGA emulates
  floating point in the kernel (`conf/fpe.s`, full soft float); to run float code
  we have to port or reimplement it. First concrete victim: `/lib/cparse` stops
  with "EPU instruction 8e04 not implemented at 00:b270" on a floating
  constant, e.g. `lseek(fd, fat + (long)(1.5 * first), 0)` in
  `src/cmd/doscopy/dosopn.c` line 175. This blocks all `doscopy` commands
  (every one links `dosopn.o`); `cc -S` already fails, so it is the front end,
  not the assembler or the optimizer.
- Signal handlers installed by the guest (signals keep their host default or
  are ignored for SIG_IGN), terminal ioctls (always ENOTTY), I/O and
  privileged instructions (user mode only).
- Flags that nothing in the tool chain looked at are approximate: V after
  shifts and rotates, C/V after `div`, DA/H after anything but byte add/sub.
  The `cpir`/`cpsir` family sets Z = condition true, P/V = counter reached zero.

## Open questions

- Check the `__.SYMDEF` entry format (12 bytes, from the ranlib binary) against
  a `__.SYMDEF` that the original `ranlib` produces under the oracle.
- EPU arithmetic in the emulator (see "Not yet emulated"). The instruction
  formats are decoded (`tools/epu.py`); what is missing is the semantics and
  the cas mnemonics for the arithmetic.
- Which cas condition code and flag names to emit (both `ne`/`nz`,
  `c`/`ult` exist).
- What `p8000c` (second `sa.diags`) and the `87wega3.2` SCCS suffix mean;
  `gld` options beyond the man page.

## ar (reconstructed, byte-identical)

`src/cmd/ar.c` + `src/lib/libc/crt/crt0-ar.s` rebuild `/bin/ar` byte for byte (`make -C WEGA/host verify-wega`).

- Source is the System III/V7 ar, version string `@[$]ar.c 2.2 11/18/83 - 87wega3.2`;
  WEGA additions: `-l` (local temp files `vXXXXX`), `av0 = trim(argv[0])` in all
  messages, sticky bit `t` in the mode tables, `qcmd`/`cleanup` use `stats()`.
- Link: `ld -s -e start crt0-ar.o ar.o /lib/libc.a`, cc flags `-O`. `crt0-ar.s` is a
  crt0 variant (`jr` over a 22-byte version string, then the usual start-up);
  assemble with `cas -u`.
- Binary layout: crt0 (0x00-0x42), ar.o, then the libc members in archive order.
- Compiler behaviour learned while matching:
  - `register` locals are saved with `ld`/`ldm` below the frame; a plain unused
    `int` local adds one 2-byte frame slot (rcmd, notfound, longt each declare an
    unused `int u;` to reproduce the original frame size).
  - `if((a[i] = *cp) != 0) cp++;` compiles to `testb rl2`; the form
    `a[i] = *cp; if(*cp) cp++;` gives `testb @r14`.
  - `f(g())` keeps g's result in a frame temp.
  - Strings, tables and jump tables are emitted in function order.

## ranlib (work in progress) and emulator findings

- `/usr/bin/ranlib` (E707, text 7124): `ranlib.c 1.1 07/24/82`, a 4.1BSD ranlib
  adapted to s.out (24-byte `s_exec` header, 14-byte `s_nlist`, long names via
  `readsym`, `goodmagic`/`swab`/`longswap` for byte-swapped objects,
  `__.SYMDEF` entries of 12 bytes `{long cloc; char cname[8]}`, max 700 symbols,
  `ar rlb`/`ar rl` via `system`, `fixdate` sets the archive date +5 s).
  Layout: crt0 variant (as in ar, but no data string), `ranlib.o` = 0x42-0x53a
  (trim, main, nextel, stash, fnew, fixdate), then libc members.
- The libc members inside this binary come from an OLDER libc than the shipped
  `/lib/libc.a` (e.g. fseek has a 14-byte frame, shipped fseek.o 20 bytes), so a
  byte-identical link is not possible until that older libc is reconstructed.
  `ranlib.o` itself can only be compared instruction-wise.
- The original ranlib.o was compiled with different optimizer behaviour than
  `cc -O` of the shipped compiler: `register` variables of leaf functions sit in
  scratch registers (r1, r3), parameters are not spilled and the frame keeps the
  slots. Not reproduced yet (tried -O, -Ol, -Or and register/auto variants).
  The "unused local" slots seen in ar (rcmd, notfound, longt) are probably the same effect.
- Oracle: segmented binaries (`/lib/gopt`, `sld`, ...) put the stack in segment
  0x3F unless header flag SF_7FSTK (0x20) is set; z8k-emu used 0x7F always, so
  `gopt` silently produced no output. Fixed; `cc -Ol/-Or` now run. gopt reports
  "coloring failed" for one function of ranlib.c (line 188) and falls back.
- z8k-emu debugging aids: `Z8K_EXEC_TRACE=1` prints every exec,
  `Z8K_SC_TRACE=FILE` logs system calls to a host file (guest may close fd 2).
- `WEGA/src/lib/libc/csu/crt0.s` (PLZ/ASM, Rev 3.2 09/12/83) matches the shipped
  `/lib/crt0.o` (42 bytes text, data `.word 0`). The ar/ranlib binaries use a
  later crt0 variant (version string in text and data), see `src/lib/libc/crt/crt0-ar.s`.

## Plexis System III sources (s3plxsrc.tar.gz, not in git)

- Zilog Z8000 SysIII source tree from Plexis (cmd/, lib/, uts/, ...), 2440 files.
  No ranlib/`__.SYMDEF` anywhere (only a BUGS mention).
- Contains the PCC based Z8000 compiler: `cmd/cc/z8000/pcc` (ccom: order.c,
  local2.c, table.c, code.c), `c2` (peephole optimizer, c20-c23, ancestor of
  popt z20-z23), `cpp`, assembler `cmd/as/z8000`, `cmd/ld`, `lib/libc/z8000`
  (stdio, gen, sys, crt, csu/crt0.s), sources of ar, size, nm, strip.
- crt0 revision strings in WEGA binaries are an era fingerprint: 3.1 (10/82 -
  03/83) vs 3.2 (07/83 - 12/83), e.g. size = 3.1 01/14/83, strip = 3.1 01/24/83,
  ar/nm = 3.2 11/18/83, ld = 3.2 07/26/83, ranlib/cas/cc passes carry none.
- Check: Plexis `lib/libc/z8000/stdio/fseek.c` compiled with the shipped cc -O has
  exactly the instruction sequence of the OLD fseek embedded in ranlib; only the
  register/frame allocation differs (old: frame 14, temps in r1/rr0/rr6; shipped
  compiler: frame 20, temps in r4/r5). So the sources are right, the old
  compiler's code generator/optimizer is the missing piece.

## Compiler lineage and versions (findings from the Plexus sources and size)

- Plexus `ccom` (one pass PCC, runs now in z8k-emu) is NOT the ancestor of the
  WEGA code generator: it emits Plexus assembler (`jpr`, `csv2`/`cret2`
  prologues). Its output cannot be compared with WEGA objects. The WEGA compiler
  (cparse + codgen, 2 passes) is a different lineage.
- Plexus `c2` (peephole optimizer, `@(#)c20.c 1.1`, Aug 1982) IS the ancestor
  of WEGA `popt`. Binaries built with older popt/codgen/cparse revisions show the
  differences listed in "Compiler revisions" (ranlib, cas, some libc members).
- `size` (E707, text 5652): own code is only main() at 0x42-0x224 (s.out aware:
  `goodmagic`, `swab`, segment table of 16 byte entries, prints
  `%U + %U + %U = ` and `%U = 0x%lx`), then the old libc (fread at 0x224, fwrite
  at 0x2a0, ...). Draft in `src/cmd/size.c`; it compiles but differs from the
  original (older compiler revision, see "Compiler revisions").
- z8k-emu now also runs Plexus a.out (0407/0410/0411) with the Plexus system
  call ABI (`sc INDIR; .word blk`, first argument in r0 for some calls). In 0410
  files data addresses start at the text size and wrap around 64K.

## libc check against the original library (status)

`libc/check.sh WEGA_ROOT LIBC_SRC ORIG_MEMBERS [member ...]` rebuilds each
member of `WEGA/src/lib/libc` with the original tools and compares it byte for byte
with the original member: `.s` with `/bin/as` (PLZ/ASM; `-u`, `-f` tried), `.s` without a
`module` header with `cas`, `.c` with `/bin/cc` (-O, "", -Or, -Ol).

- `/bin/as` is the PLZ/ASM assembler (not `cas`). It execs the overlay `/lib/as2`
  (magic E705), so the emulator implements overlay exec (I-space replaced, data,
  stack and registers kept). `-f` allows the EPU (floating point) mnemonics
  (`fldctl`, `flds`, `fmul`, `fldd`, ...); without it they give ERROR 91.
  Module names matter: they appear as `<module>_p`/`<module>_d` symbols
  (abort.s is module `abort_s`).
- Result: 168 of 174 members identical.
- Same code, different compiler revision (cannot match with the shipped cc): `malloc`,
  `nlist`, `length`, `swap` (differences D1, D3, D4, D8 in "Compiler revisions"). The shipped
  libc was built with more than one compiler revision.
- Different source revision (Nov 1983 float versions, files dated Nov 16-17 1983):
  `printf` (printf/fprintf/sprintf with `_pfile`, `_rc`, `_flsbuf`; calls
  `_doprnt(fmt, args, argend, flag, file)`), `scanf`, `doscan` (calls `_inflt`).
  Together with the still missing `doprnt`, `cnvt`, `ecvt`, `fltioc` and `symscan`
  they form the floating-point stdio cluster that remains to be reconstructed.
- Findings while fixing sources: getpass uses `struct termio` (`c_lflag`, ioctl 0x5401/0x5404/0x5403);
  execvp is the 4.3BSD one with `index`, a `/bin/csh` fallback (`whichshell`) and `-` as
  path separator in `execat`; malloc is a variant that extends the top free block when the
  break is contiguous (`maxbytes` 0xFFF8, `maxblocks` 64 as data); nlist/readsym use the
  s.out long-name encoding (`readby`, `(len+13)/14` entries).

### gopt (-Or) output with register pointer parameters: real behaviour, not an emulator bug

`cc -Or -S` on `f(str) register char *str; { register int i; i=0; while(*str++) i++; return(i); }`
emits `ld ~L1+12(fp),r7` / `ld r4,~L1+-1275(fp)` with `~L2 := 12`. Verified on real
WEGA hardware (photos of the terminal, same output, same constant -1275), so this is a
genuine gopt defect and the emulator reproduces it faithfully. `-Ol` on the hardware gives
exactly the emulator's output (`ld r1,r7`, `ldk r3,#0`, `ld @fp,r1` ... `~L2 := 10`).
Consequence: see "Compiler revisions" (not a cause of the length, nlist, swap, malloc differences).

## size and strip (reconstructed, own code identical)

- `src/cmd/size.c` + `src/lib/libc/crt/crt0-size.s` and `src/cmd/strip.c` + `crt0-strip.s` rebuild with the
  ORIGINAL `cc -O`; `make verify` compares the instructions of the own code
  (`tools/cmpins.py`, from 0x42 up to the first libc member, `fseek` resp. `rdwr`).
  size: 0 differences. strip: one expression (`head.s_imsize = code + data` as long uses
  rr4/rr6 in the original, rr2/rr4 in our build; the same kind of expression elsewhere in
  the file compiles identically, so the original source form is just not found yet).
- size (`size.c 2.1 01/14/83`): own code identical to the output of the shipped cc. strip (`strip.c 1.2
  11/19/82`, crt0 3.1 01/24/83): identical except that one expression (D2); the older compiler is clearly
  visible in ranlib and some libc members (table "Compiler differences" in `src/TODO.md`).
- The libc parts inside these binaries are an OLDER libc revision (Jan 1983): `printf`,
  `doprnt`, `fprintf`, `malloc`, `length`, `swap` are not the members of the shipped
  libc.a (those are dated Sep/Nov 1983). The old members can be reconstructed from these
  binaries (printf family in size/strip, malloc in strip/ranlib) - they are the
  predecessors of the float stdio cluster that is still missing from `src/lib/libc`.
- strip: `strip [-h] file ...`. `-h` removes header and segment table. For the 8-bit
  magics (E507/E511/E505) it rewrites image size/codesz and filters the image through the
  relocation bytes (`rcopy`). Signals 1-3 are ignored while copying. `hflag` is
  uninitialised when no option is given (as in the original). Temp file `/tmp/sXXXXXX`,
  opened `w+` and unlinked at once.
- size/strip/ranlib/objhdr/objdu call `goodmagic(magic)` (value, not pointer), `swab`,
  `longswap`, `swap_flg` from the libc.


## Old (Jan 1983) libc inside size/strip/ranlib

- Members found unchanged in size: fopen, decimal, isatty, stty, swab, brk and all syscall stubs.
  Everything else (fread/fwrite, printf family, _doprnt, _flsbuf, _filbuf, fflush, fclose, _cleanup,
  endopen, malloc, strlen, goodmagic ...) differs from the shipped libc.a only through the compiler
  revision, plus the printf family/_doprnt which use an older interface:
  `_doprnt(fmt, regargs, fpargs, stackargs, nreg, file)`; `printf` passes nreg=0, fprintf/sprintf 1.
- `_doprnt` walks the register argument block (r6,r5,r4,r3,r2 stored in that order), switches to the
  stack arguments after 5 words, uses fix() to align longs, supports `- * 0 .` flags, d o u x c s
  D O U X, l-prefix, and e/f/g via an empty stub (no float support in this revision).
- The old cc differs from the shipped one (D14 and others in "Compiler revisions"). No older compiler
  binary exists in any archive, so size/strip as whole binaries cannot be reproduced bit-exact by compiling.
  Their own code (size 0x42..0x224, strip) is identical to the shipped cc output.
- `src/lib/libc/stdio/doprnt.c` is the Nov-1983 `_doprnt` and now rebuilds byte-identical with the shipped cc -O
  (findings: `static char nullstr[]` declared after `fldone` explains the data order, `_pfile` must be
  declared between `_align` and `_doprnt` for the symbol order, `putc((int)*str++,f)` gives
  `extsb r6; ld r7,r6`, `if ((c = *str++) < '0' || c > '9')` keeps c in rl3, `_align` falls into the
  swap after the first branch, float tail jumps to the shared `len = r2` store).
- `src/lib/oldlibc/doprnt.c` holds the structural reconstruction of _doprnt (untested).

## oldcc: assembler sources of objects made by the old cc

Objects that an older cc revision produced cannot be rebuilt from C with the shipped cc. For those
we keep `src/lib/libc/oldcc/<member>.s` (cas syntax) which the original `cas -u` turns into the byte-identical
original member (`make -C oldcc verify`). They are generated from the original objects with
`tools/sdis.py -a` (all symbols are pre-declared in symbol-table order, otherwise the symbol table
of the rebuilt object differs). Sources that the shipped cc reproduces byte for byte get NO `.s`
listing in git.

Covered (10, all verify): cnvt, doscan, fltioc, length, malloc, nlist, printf, scanf, swap,
symscan. `ecvt` is PLZ/ASM (`as -f`), built from `src/lib/libc/gen/ecvt.s`. Not yet covered: the old-cc
libc parts that are only embedded in size/strip/ranlib binaries (no member objects exist for them).

## cas: analysis of the binary (`/bin/cas`)

Tool: `tools/casmap.py /bin/cas` (function entries, text blocks). Results of the first analysis, the base
for the decompilation (`src/cmd/cas/`, then `host/cas`); ranges are text offsets in segment 4.

**Origin.** The SCCS ids and strings show Zilog's U8000 assembler (`"U8000 assembler"`, source tree
`/as/as/*.c`, ids from 03/1983 to 07/1983, `Copr. 1981`). It is not the Plexus `as` (`tmp/plexus/cmd/as`:
two passes with an ifile as well, but hand written scanner/parser, other messages and tables); the module
names `ifile`, `ofile`, `expr`, `symbols`, `table1`, `table2` coincide, so a relation is possible.
Plexus is read only where a place in the binary is unclear.

**File.** E611 (segmented, separate I&D), `-e` entry `04000000`, flags STRIP|7FSTK. Segments: 4 code
(46848 bytes), 5 data (38360) + bss (12328), 7, 8, 9 each 1280 bytes data. Code from `scrt0` (0x0..0x41), the
28 modules of cas, then the libc members (from 0x9694, about 8.3 KB: stdio, `_doprnt`, `sys_errlist`, syscall stubs).
Segment 5 holds the data of all modules in link order (each begins with its SCCS id); segments 7, 8, 9 are
the three symbol tables `stable.c`, `itable.c`, `ntable.c`: each starts with its SCCS id and has the table
area at offset 0x30. `ssymtab.c` allocates from them like a bump allocator and grows the segment by 1024
bytes with `ssgbrk` (syscall 45) when it is full (`0x457a`, `0x4618`, `0x46b6` for segment 7, 8, 9).
Pointers into the tables are 32 bit `seg:off` (`0x87000030` ...).

**Calls.** Inside one module `calr` is used, between modules `call <<4>>addr`; so blocks that no `calr`
crosses are module boundaries. Together with the SCCS order of the data (modules are linked in the order of
the SCCS ids in segment 5: ifile, instr, scanner, main, nodes, parser, pst, subr, symbols, ssymtab, table1,
table2, locntr, expr, p2loop, ofile, gen, assemble, getops, error, jpr, jpr2, optim, listing; the three
table segments last) and the strings each function loads:

| module | text | note |
|---|---|---|
| scrt0 | 0000-0041 | start, `exit` |
| ifile | 0042-04f5 | 20 small functions: put byte/word/long into the intermediate file (via `fwrite`), expression nodes (`Unknown expression type`) |
| instr | 04f6-0789 | 4 functions, `Illegal opcode`, `Invalid operand combination`, use of `table2` data |
| scanner | 078a-1277 | `yylex` (2600 bytes), 2 helpers; `::=`, `^FS`, `^FD`, `Invalid token`, `Unknown keyword` |
| main | 1278-1b01 | `main` (2064 bytes, option switch on `c - '^'`, jump table at data 0x350), 0x1a90 = fatal error exit |
| nodes | 1b02-1ef7 | 8 functions, node allocation (`Out of nodes`) |
| parser | 1ef8-3b05 | `yyparse` 0x1ff6 (6910 bytes, with YYDEBUG strings), stub 0x3af4, helpers; actions print `Expecting ...` |
| pst | 3b06-3d1d | 5 functions: look up a name in the permanent symbol table (mnemonics, registers), case insensitive compare (`3b6a`); called by the scanner, the nodes and the jpr modules |
| subr | 3d1e-3d8f | one function, the name hash (`3d1e`); `3b06` calls it with `call`, not `calr`, so it is another module; also used by ssymtab (`479e`, `4996`, `4a30`) |
| symbols | 3d90-4579 | 10 functions; the last three (`43d4`, `4472`, `44ee`) write records of type `0x2a` to the intermediate file and are called by `yyparse` directly; the border to ssymtab is not proven |
| ssymtab | 457a-4b3d | the three table allocators (`457a`, `4618`, `46b6`) and accessors (`4772`, `4788`, `479e` ...) |
| table1, table2 | data only | 0x5526-0x655e, 0x655e-0x6b82: instruction and operand tables (5.7 KB) |
| locntr | 4b3e-4ecb | location counters per section, 9 functions |
| expr | 4ecc-5f31 | expression evaluation, 18 functions (the operators `+ - * / ^< ^> ^& ^\| ^X ^S ^O` ...) |
| p2loop | 5f32-6393 | pass 2 main loop, 3 functions |
| ofile | 6394-6da5 | object file writer: header (24 bytes `fwrite`), segment table, relocation, the 14 byte symbol entries (`6b66`), seeks into the temporary files (`6b96`, `6cb2` writes the symbols of segment 7) |
| gen | 6da6-7429 | code generation, 8 functions linked by `calr` (6da6, 6e9a, 6f66, 6fba, 70b0, 7192, 71c2, 7286; the last one runs to 7424) |
| assemble | 742a-7fed | instruction assembly (the block `729e` that had no `calr` partner is the second half of `seekout` in gen) |
| getops | 7fee-83ff | operand parsing helpers, 4 functions |
| error | 8400-84fb | one function (`error`, uses `listing`, `ifile`) |
| jpr, jpr2 | 84fc-89c1, 89c2-8af3 | JPR/CALLR pseudo instruction (jr/jp, calr/call) |
| optim | 8af4-92f5 | jump optimisation (`jump optimization`), 8 functions |
| listing | 92f6-9693 | listing (`LOC    OBJ CODE    STMT SOURCE STATEMENT`, `%04x `), the last function copies the rest of the source file with `getc` inline |
| libc | 9694-b6ff | stdio, `fprintf`, `ssgbrk` (b516), `exit` (abc8), `strerror` tables |

**Libc members inside cas** (matched with `slibc.a` by instruction sequence and by the `sc` numbers; it is an
older revision, so some match only partly): `freopen` 9694, `fseek` 96ce, `fread` 982e, `fwrite` 98b2,
`fopen` 9938, `printf` 995c, `fprintf` 99f0, `sprintf` 9a74, `_align` 9b4a, `_doprnt` 9bb0, `_atoip` a58a,
`ungetc` a61a, `_filbuf` a68c, `_flsbuf` a7a0, `fflush` a8b6, `_cleanup` a924, `fclose` a950, `_endopen`
a9dc (+ `ab1e` a creat/close/open/chmod helper of it), `_findiop` ab9e, `exit` abc8/abd0, `perror` abdc,
`_cnvt` aca6, `_deciml` adbe, `_length` ae5c, `calloc` ae8a, `malloc` aef6, `free` b168, `realloc` b198,
`isatty` b276, `mktemp` b29c, `strcmp` b356, `strcpy` b39a, `strncpy` b3cc, `strlen` b422, `tolower` b458,
the stubs `access` b446, `chmod` b47e, `close` b490, `creat` b4a0, `getpid` b4b0, `ioctl` b4b6, `lseek`
b4ce, `mkseg` b4e2, `open` b4f2, `read` b504, `ssgbrk` b516, `signal` b52c, `umask` b5e6, `unlink` b5ee,
`write` b5fe; `sys_errlist` data after the listing strings; the text ends with zero padding from 0xb624.

**Options of `main`** (`c - '^'` indexes a jump table of 25 words at data 0x350): `-o file` (output name,
default `a.out`, pointer at 0x316), `-d` (0x348, read by ofile: internal labels into the symbol table),
`-l` (0x34a, listing; used by main), `-u` (0x342, read by ofile: undefined symbols become external),
`-v` (0x344, main: the messages `Pass 1`, `Pass 2`), `-^` (0x346, scanner), and the register counters
`-i` `-r` `-t` `-m` (r11, r12, r10, r9): keep the intermediate, relocation, symbol and object-symbol
file under the fixed names `i.out`, `r.out`, `sym.out`, `osym.out` instead of a temporary; with `-d`
the name is built from a base name (0x32a) with the last character replaced (`i`, `t`, ...). Any other
letter: `invalid option -%c`. The documented options of cas(1) are `-d -l -o -u`; the others are debug aids.

**Temporary files** (from `main`, read in detail): `/tmp/as{i,r,s,o,t}XXXXXX` are templates (pointers at data
0x31a-0x32a); i = the intermediate file (FILE at 0x9630, name `i.out`), s = the symbol file (FILE 0x962c,
`sym.out`; option -t keeps it), r = relocation (FILE 0xa31e, `r.out`), o = the output symbols (FILE 0xa2ba,
`osym.out`), t = the temporary copy of a source read from stdin when a listing is wanted. The listing is
`list.out` (stdin) or the source name with the last character replaced by `l`; likewise `i`, `t`/s, `r`, `m`.
Each file is created `w+`, and removed at once with `unlink` unless its option asks to keep it. The
errors of pass 1 (`synerr`, `semerr`, data 0x34c, 0x34e) end the run with `Abort because of syntax errors`,
those of pass 2 with `Abort because of semantic errors`; `done(-1)` prints `Assembler Abort`, `done(0)`
`Assembly Complete` with -v.

**Plexus yacc.** `tmp/plexus/cmd/yacc` (y1-y4.c, `yaccpar`, `dextern`, `files`; private reference) builds on the host with
`gcc -std=gnu89 -w` after `PARSER` in `files` points to the local `yaccpar`; the tables it writes (`yyexca`,
`yyact`, `yypact`, `yypgo`, `yyr1`, `yyr2`, `yychk`, `yydef`, tokens from 257, `yychk[0] = -1000`) have the
layout and order of the cas parser, and its `yaccpar` has the debug strings of the binary (`state %d, char 0%o`,
`error recovery pops state %d, uncovers %d`, `reduce %d`, `yacc stack overflow`, `syntax error`). So the grammar
rebuilt from the tables can be checked by running this yacc on it and comparing the tables with the binary.
Only as a local tool: the sources are not copied into git.

**bss** (0x95d8-0xc5ff, 12328 bytes) by direct use (module = who uses it, the owner may be another one):
0x9628-0x9638 file pointers/state of ifile and main, 0xa234-0xa236 parser/instr, 0xa236-0xa2b6 scanner,
0xa2ba-0xa366 main (options, names, the files 0xa2ba/0xa31a/0xa31e), 0xa382 nodes, 0xa9c2 and
0xac20-0xac24 parser, 0xac26-0xac28 jpr/optim, 0xb826 and 0xb83a ssymtab (the allocator state),
0xc00a-0xc0b1 and 0xc138 ofile (header `0xc08e`, section tables `0xc0a6`), 0xc13c-0xc14a assemble,
0xc152-0xc162 optim. The data of the modules before 0x95d8 holds the initialised variables, tables
and strings.

**Raw structures in files.** Intermediate file: single bytes, words and longs (`fwrite` of 1, 2, 4 bytes
from `ifile` and `gen`). Object file (`ofile`): `fwrite(&header, 24, 1, f)` (the `s_exec` header, bss 0xc08e),
a buffer of `n` bytes (`6672`, `66a8`) and arrays of 14 byte `s_nlist` entries (`6b66`): the host
version has to write these big endian and without padding explicitly.

**System calls** in the binary: exit, read, write, open, creat, close, unlink, chmod, lseek, getpid, access,
signal (48), umask (60), ioctl (54), 55 (via a libc stub, not yet named) and `ssgbrk` (45).
Temporary files `/tmp/as{i,r,s,o,t}XXXXXX` (the meaning of the letters is a guess: intermediate, relocation, symbol, object, ...), debug
names `i.out`, `sym.out`, `list.out`, `r.out`, `osym.out`, default output `a.out`.

**yacc (parser.y 1.2).** System III yacc skeleton (`yaccpar`) with YYDEBUG strings (`state %d, char 0%o`,
`reduce %d`, `error recovery ...`), but no name tables. Data at 0x5c0 (SCCS id) then, by the base
addresses used in `yyparse`: scalars 0x5f6-0x604, `yyexca` 0x606-0x6f9 (122 words), `yyact` 0x6fa (1408
words), `yypact` 0x11fa (379 states), `yypgo` 0x14f0 (55), `yyr1` 0x155e (287 productions), `yyr2` 0x179c
(287), `yychk` 0x19da (379), `yydef` 0x1cd0 (379), the action switch table of `yyparse` from 0x1fce
(32 bit pointers into segment 4, about 318 entries), the `Expecting ...` strings in 0x24c6-0x388d (they are
the parser's `yyerror` texts, one per rule). The tables are the original ones; the grammar can be rebuilt
from them: the automaton gives for every production the accessing symbols (`yychk`) of the states a
reduction pops, so the right hand side of each rule follows. Token names are not in the binary (no
`yytoks`), they follow from `yylex` (return values) and from the action texts. The original `yacc` is not
usable (`/usr/lib/yaccpar` is missing on the machine); for a byte identical parser the tables are taken
over from the binary, a rebuilt parser with another yacc is functionally the same only.

**To do for the decompilation:** the border symbols/ssymtab, the bss layout per module (the module bss
variables are in link order; only 56 addresses are used directly), the intermediate file records
(ifile/p2loop), the grammar from the tables (Plexus has a SysIII `yacc` with `yaccpar` in
`tmp/plexus/cmd/yacc` that can be built locally to regenerate and compare the tables; not for git), and the places that depend on the Z8000 types (16 bit `int`, 32 bit
pointers `seg:off` in nodes and tables; the tables in segments 7-9 become heap arrays on the host; the
intermediate files: the small functions of `ifile` (0x42-0x1d0) write single bytes, words and longs with
`fwrite`, which is good for the host; whether other places write raw structures is not checked yet).

## Compiler revisions: how the old compiler differs from the shipped one

Several WEGA programs were built by an older revision of the C compilers: cas by an older scc; ranlib, strip
and some libc members by an older cc. No old compiler binary exists in any archive (the old libc members and
the old objects only show its output), so these differences are the base for rebuilding the old compiler at the
end of the project. Each difference has its own number D1 ... D14; `src/TODO.md` (section "Compiler
differences") lists which objects show which difference. "old" = the compiler that built the original, "shipped"
= the scc/cc in `/bin` of the system. Evidence: `cmpmod.py` listings of the cas modules (`-s` shows frames and
scratch registers), the `oldcc` objects, ranlib, strip.

**Dating.** The shipped popt carries `z20.c 1.3 12/21/82`, `z21.c 1.4 12/21/82`, `z22.c 1.6 01/18/83`,
`z23.c 1.6 03/25/83`; it descends from the Plexus `c2` (`c20.c 1.1`, Aug 1982). Binaries built earlier than
those dates (size: crt0 3.1 / size.c 2.1 01/14/83, strip.c 1.2 11/19/82) were produced with older popt revisions
(z22 <= 1.5, z23 <= 1.5) and codgen/cparse revisions we do not have. The Plexus one-pass `ccom` is not an ancestor
of the WEGA code generator (it emits Plexus assembler: `jpr`, `csv2`/`cret2`). The date of a source does not date
the compiler: the sources of cas are dated up to 07/83 and were compiled by the older scc (`scc -O -c`).

**Not differences of the compiler.**
- Position of the prologue: with `-O` both scc put `dec r15,#n; ldm ...` in front of the body; without `-O` the
  prologue is behind the body (`jr Lp` at the entry, `Lp: dec r15 ... jr Lb`). The cas parser was compiled
  without `-O` (`yyparse` starts with `jr 1ff4`), the other cas modules with `-O`. `cmpmod.py` converts the
  behind form to the front form before it compares.
- The gopt defect of `cc -Or` (`ld ~L1+12(fp),r7` with `~L2 := 12`, verified on real hardware, the emulator
  reproduces it): the shipped libc cannot have been built with `-Or` for such functions; the 2-byte frame and
  `ldb`+`testb` differences of length, nlist, swap and malloc are not caused by options or the emulator but by the
  compiler revision (D1, D3, D4, D8).
- Older revisions of the libc sources (Jan 1983 `_doprnt`, `readsym` of ranlib, ...) are source differences, see
  the libc sections.

### D1 Frame size and saved registers
Frames differ in size (cas `putins`: 12 bytes in the original, 14 in ours; libc `nlist` and `length` 2 bytes
smaller; ranlib smaller frames; libc `fseek` 14 bytes against 20 in the shipped member) and the slots of the saved
register variables are numbered differently (`ldm S10(r15),r13` against `S12`). Seen with `cmpmod.py -s`.

### D2 Scratch registers
The old compiler uses r0/r1 (`rr0`) as scratch where the shipped one uses r4/r5 (`rr4`): cas modules
(`ldl rr0,S4(r15)` against `ldl rr4,S4(r15)`), strip (`head.s_imsize = (long)code + data` is compiled with
rr4/rr6 by the original, rr2/rr4 by ours; the same kind of expression elsewhere in strip.c compiles identically),
ranlib. Register variables of leaf functions are placed in scratch registers. Error blocks are moved ahead
of a loop (ranlib, size).

### D3 Register contents (redundant reloads, memory operands)
The old popt does not remove a reload after a store (`ld S0,r7` followed by `ld r7,S0`): 2 times in ifile,
1 in scanner, 5 in parser, more in the later modules (counted in the binary). The shipped one removes them.
Opposite direction in libc `malloc` (calloc): the old cc knows that a register still holds the value
(`mult rr2,r6`) where the shipped cc uses a memory operand.

### D4 Spill of arguments
ranlib: no spill of arguments into the frame. libc `swap`: the old cc spills an address-taken argument in
the `0(r15)` form.

### D5 Constant index into local arrays
`a[3]` of a local array is addressed directly (`ldb S5(r15),rl2`); the shipped scc computes the address
(`ldl rr2,#S4; add r3,r15`). Seen in cas `nodes` (`escape`) and the parser.

### D6 switch
Up to 3 case labels the old scc compares one after the other and puts the dispatch in front of the case
bodies; the shipped scc uses a table from 1 or 3 cases on (`cpir` over the case values, then a jump table
behind the bodies: `ld r6,#n; cpir ...; sll r6,#2; neg r6; ldl rr2,L(r6); jp @rr2`). With 4 or more labels the
old compiler also uses the `cpir` table (parser, nested switches), the values sorted, the bodies in source order.

### D7 Loop layout
The old compiler puts the loop test at the bottom (`jr` into the test first); the shipped scc tests at the top.

### D8 Conversion and loads of bytes
Unsigned char argument: old `ldb rh7,#0`, shipped `subb rh7,rh7` (cas `ifile`). Old libc `length`:
`ldb rl3,@r2` + `testb rl3` where the shipped cc has `testb @r2`. Old libc: char loads as `ldb`/`subb`.
`extsb` against byte moves in cas `pst`.

### D9 Shift by 8 bits
`w >> 8` is done with a byte move (`ldb rl7,rh7`) by the old compiler, with `sra r7,#8` by the shipped one
(cas `ifile`).

### D10 Shift of a long by a constant
Old: `ld r2,#24; sdal rr4,r2` (also `ldk r0,#0; ld r1,r2; ld r2,#16; sdal rr0,r2` for `(long)w << 16`), shipped: `slal rr4,#24`.

### D11 Tail merging (popt)
The old popt merges common tails of branches (`call xtol; jp` shared by the number cases of the scanner).
The shipped popt does not.

### D12 String literals and data layout
The old compiler writes all string literals and the jump tables of a module behind its other data
(switch tables in front of the strings), does not merge equal strings and aligns them differently; the
shipped one writes the strings of a function in front of the data that follow. The same data, a different
order: in the parser 42 bytes of difference in size, and the tables of `yyexca` ... start at another
offset. `cmpdata.py` shows differences for such modules although the contents are equal.

### D13 Pointer constants
The old compiler keeps bit 31 of a segmented address constant (`ldl rr2,#%87000030`, `add r3,idx`), the shipped
scc clears it (`#%07000030`; `(char *)CONST + n` can even stop with `register jam` when it is assigned to a
register variable or returned). Seen in cas `ssymtab` (the three tables in segments 7, 8, 9).

### D14 Varargs prologue and post-increment (old libc members inside binaries)
The old libc members that are linked into size, strip, ranlib and cas (we only have them as machine code inside
those binaries) were compared function by function with the members of the shipped `libc.a` / `slibc.a` (script
in the scratchpad, matched by instruction sequence; the old members come from the Jan 1983 libc, so differences of
the source are possible; only differences that the compiler explains are counted here).
- Varargs prologue: the printf family (`printf`, `fprintf`, `sprintf`) of size, strip and cas spills the EPU
  registers inline (`.word %4FFC,%8303,off` ... four before and four after the `ldm`) and stores r6..r2 in addition to the
  contiguous `ldm`; the shipped members have neither. The printf family of ranlib has the shipped form (no EPU spills).
- Post-increment: in size the byte of `*fp->_ptr++` is used first and `inc @r13,#1` follows (`fread`, `fwrite`, and the
  `putc` inside `_doprnt`, whose source differs from the shipped one, so that case is a guess); the shipped cc increments
  first (`ld r2,@r13; inc @r13,#1; ldb rl3,@r2`). `fread`/`fwrite` of strip, ranlib and cas have the shipped order.
- Checked and without D14: in cas 35 functions (fseek, fopen, freopen, ungetc, `_filbuf`, `_flsbuf`, fflush, `_cleanup`,
  fclose, `_endopen`, `_findiop`, exit, perror, `_cnvt`, `_deciml`, `_length`, `_align`, `_atoip`, malloc family, isatty,
  mktemp, str*, tolower, signal; their differences are D1, D8 (`extsb; and #FF` against `subb rh,rh`) and those of the
  source), in size/strip/ranlib the same members and `swab`, `goodmagic`, `swap`. Several of them are identical to the
  shipped member (cas: freopen, fopen, ungetc, `_deciml`, `_length`, strcmp, strcpy, strncpy, strlen, tolower).
- `_filbuf` of size and ranlib: the conditional value `(flag & _IONBF) ? 1 : BUFSIZ` goes through a stack temporary
  (`ld @r15,#1` ... `ld r5,@r15`) where the shipped cc uses the register; not counted as D14 (not seen elsewhere, no D number yet).

## cas: decompilation (src/cmd/cas)

Method and tools: `src/cmd/cas/check.sh` (compile a module with the original `scc` under the oracle, compare
with the range of `/bin/cas` by `tools/cmpmod.py`), `MODULES` (ranges and state), `ADDRESSES` (names against
addresses), `README` (the differences between the old and the shipped scc). The work goes in link order; the
aim is the same code and then the same behaviour of the complete program (tested against the original, see "Test of the complete cas"
below: the code comparison alone is not enough); byte identity is not expected.

**The compiler that built cas.** The binary was built by an older scc with `scc -O -c` (the parser without `-O`);
its differences from the shipped scc are described in "Compiler revisions" and listed per module in the table
"Compiler differences" of `src/TODO.md`; see also `src/cmd/cas/README`.

**Done: ifile, nodes, instr, scanner, main, done (code).** Every function was compared with `cmpmod.py` and the remaining differences read by
eye: they are those of the old compiler ("Compiler revisions"; the tool cannot hide them all, so its counts of
"differing" functions are not a measure; `nodes` has 6 of 8 and `ifile` 4 of 19 functions identical, the rest
show only such differences). Data (`cmpdata.py`): the order of strings and jump tables differs (D12).

- Node (16 bytes, `cas.h`): `n_type` (unsigned char, 0), `n_next` (long pointer, 2), then a union at 6: a long;
  a word and a long (type 6); a word at 8 (types 7-9); two longs at 6 and 10; or two bytes at 6, 7 and two
  longs at 8 and 12 (type 25). Node types seen: 1 byte value, 2 word, 3 long, 6 word + node, 7-9 word,
  22 (0x16) string (chain of type 1 nodes, the last one in the long at 6), 23-25 operations (the operator
  byte at 7, the operands at 8 and 12); the others are `Unknown expression type`. The pool is `nodes[100]` in
  the bss (0xa382), `nalloc` and `nfreed` (unsigned, data 0x514, 0x512) count the nodes handed out and
  written to the intermediate file; at the end of a statement they must be equal (`Nodes allocated at end
  of statement`), else both are reset.
- Intermediate files (`ifile.c`): two sets of routines with the same shape for the intermediate file
  (`ifile`, bss 0x9630, name in the variable at data 0x32e) and the symbol file (`sfile`, 0x962c, name at
  0x336): put/get a byte, a word (high byte first) and a long (high word first); a failed `fwrite`/`fread` of
  one byte gives `perror(name); done(-1)`. Expressions are written in prefix order: the operator or type byte,
  then the value (1: byte, 2: word, 3: long, 6: word and the node list, 7-9: word, 23-25: both operands). The
  record codes seen: 10 (and four zero bytes, `075e`), 11 + word (the location counter of the current section:
  `lctab[cursec][cursub]`, 12 bytes per section), 15 + word (line number) or 16, 17, 18 for the next 1, 2, 3
  lines (the variables at data 0x38, 0x3a start with 1).

**Done: pst, subr and the tables table1, table2, pstab.**

- Permanent symbol table: 403 entries of 12 bytes `{int tok; char b2; unsigned char n; struct irow *rows; char *name}`
  at data 0x38c2, then the names; an instruction entry has its forms in `rows` (n of them, `itab1`), `tok` is
  the token the scanner returns (62..65 for the four instruction kinds that write a trailer record). The hash:
  `hash(503, name)` of `subr.c` (the characters, folded to one case, go alternately into the high and the
  low byte of a word and every step adds that word: `*p = c; h += k`; the result is `h % n`), the buckets are
  `pbuckets[503]` of `{next, entry}` (the module `pstab`, no code, no SCCS id, between error and jpr in the
  data), the chains run inside the array. `pstlook` compares the name case-insensitively with the stored
  lower case name (`pstcmp`: -1 = different, anything else = equal; the original returns a stale register
  for "equal", the caller tests only against -1).
- `itab1` (684 rows of 6 bytes, base opcode word, offset of the operand template, index into `instlen`, one
  more byte), `opcls` (operand templates, 522 bytes, each ends with 0x12 or 0x1c), `clstype` (30 bytes, the node
  type an operand class needs), `instlen` (162 records `{len; 5 bytes}`). `tools/casgen.py BINARY DIR`
  writes `pstdata.h`, `table1.c`, `table2.c`, `pstab.c` from the binary; scc compiles them to data that
  equals the binary byte for byte (`tools/cmpdata.py`: table1 4152, table2 1572, pst 6860, pstab 4024 bytes;
  only the relocated pointers are not compared).
- Scanner tokens seen (`yylex`): 2 float constant, 3 `.` (location counter), 4 `+`, 5 `-`, 6 `^FS`, 7 `^FD`,
  8 `^F`, 9 `/`, 10 `*`, 11 `^<`, 12 `^>`, 13 `^&`, 14 `^|`, 15 `^X`, 16 `<<`, 17 `>>`, 18 `|`, 19 `:`,
  20 `::`, 21 `#`, 22 `@`, 23 `^S`, 24 `^O`, 25 `{`, 26 `}`, 27 `(`, 28 `)`, 29 end of line, 30 `,`, 31 `=`,
  32 `::=`, 33 `:=`, 34 invalid token, 35 unknown keyword, 36 symbol index (yylval.i), 85 number (yylval.l),
  86 new name, 87 new local name, 88 string, -1 end of file; the others (37 ... 84) come from the permanent
  symbol table (directives such as `.comm` 37, `.extern` 38, `.even` 39, `.odd` 40, `.byte` 41, `.blkb` 44,
  `.bss` 77, `.code` 75, `.data` 76, `.seg` 78, `.nonseg` 79, `.psec` 82, `.addr` 83, `.line` 84, then the
  registers and mnemonics).
- `scanner.c` details: the name path is shared by `~name` (local names, mode 6) and plain names; a name goes
  first to `pstlook`, then to the user symbols (`stlook(name, mode)`); an alias entry (type 3) is followed
  through the itable and masked with `0x7F00FFFF`; `%1F` hex, `%(2)101` binary, `%(8)17` octal, decimal,
  `1.5` / `.5` floats (token 2; `1e5` without a dot is an invalid token); strings `"..."` and `'...'` end at
  the same quote (no check for end of line); operators are matched against a table of 35 `{string, token}`
  with `ungetc` for the characters read too far; `/* */` and `//` comments are skipped (the line counter
  `curline` is incremented there).

**Done: parser (`parser.y`).** The grammar was rebuilt from the tables and the yydebug traces and written as
`src/cmd/cas/parser.y`; the ORIGINAL `/usr/bin/yacc` (run under z8k-emu, `yacc -d`; `/usr/lib/yaccpar` is the
skeleton of the binary) turns it into tables that are identical to those of `/bin/cas` in all 8 arrays
(`yyexca yyact yypact yypgo yyr1 yyr2 yychk yydef`; `tools/cmpyacc.py`, run by `check.sh parser`). That fixes the
rules and their order, the token numbers (declared in increasing order) and the order of the nonterminals:
39 typed nonterminals come first (`%type`, in the order of `parser.y`), then the 15 untyped ones in order of
first appearance. yacc reports 3 shift/reduce conflicts (as for the original grammar: the tables are equal).
- `%union {long l; int i; char *p; struct pst *pp; NODE *np;}`: the scanner returns `yylval.i` (symbol index, token 36),
  `.l` (number, 85), `.p` (names 86/87, string 88) and `.pp` (the entries of the permanent symbol table, 37..84).
  `scanner.c` includes the generated `y.tab.h` for `YYSTYPE`; `yylval` is defined by the parser (yacc writes
  `YYSTYPE yylval, yyval;`; `yyval` is at bss 0xa9c2, `yylval` at 0xa2b6).
- The value stack element is 4 bytes; `yyparse` is the skeleton of `yaccpar` (`yydebug` data 0x1fc6, `yychar` 0x1fc8,
  `yynerrs` 0x1fca, `yyerrflag` 0x1fcc, `yyv[150]`). `yyerror(s)` (0x1ef8): `"yacc stack overflow"` -> `error(4, msg)`;
  `"syntax error"` -> the three lines `Syntax error(s) found / Syntax checking will continue / No object file will be
  created` once (`synerr`); any other text -> `error(3, msg)` and `synrecover()` (skip tokens up to the end of line
  token 29, `yyerrflag = 0`, `nfreed = nalloc`). The actions of the productions that only say `Expecting ...`
  call `yyerror` with that text (about 140 messages, all in `parser.y`).
- Variables of the parser (data): `plist` 0x5f6 (labels waiting for the next statement), `segmode` 0x5fa (`.seg` /
  `.nonseg`), `fpflag` 0x5fc, `fpmode` 0x5fe, counters of rounding / infinity modes 0x600, 0x602 (the second test of
  the infinity case uses the rounding counter: kept), `nameflag` 0x604 (a name was read: its index goes to the
  symbol file with record 0x15), `errmsg[80]` 0x606; bss: `listp` 0xac20, `symmode` 0xac24 (6 local name, 0 other;
  passed as `(unsigned char)`). Node kinds the grammar builds with `mknode2(type, a, b)`: 0x56 address operand, 0x60
  indirect `@r`, 0x61 `expr(r)`, 0x62 immediate `#expr`, 0x63 `r(#expr)`, 0x64 `r(r)`; register nodes
  `mknode2(tok, mknode1(1, regnumber), 0)`; the kind byte at offset 6 of a name node (0 extern/common, 2 `::=`, 3 `::`, 4 `:=`, 5 `:`)
  is set before `defsym` (0x3d90) or the label list.
- The functions of the modules that are not reconstructed yet are called by address (`fn4CDA` / `fn4DC4` even / odd, `fn4BB0` select the section, `fn4A0E` / `fn4A30` `{` / `}`, `fn84FC` jpr /
  callr, `fn4ECC` the type of an expression, `fn52C6 fn538A fn543C fn5CCC fn59FC fn57C0 fn5708 fn5650 fn5598 fn54E0 fn5876
  fn5928 fn59DC fn5CAC` the operators of `expr.c`, `fn4DF6` the value of `.`); they get their names when their modules are done.
- Code check: `cmpmod.py` on `parser.o` (compiled WITHOUT `-O`, as `yyparse` has the prologue behind the body there):
  the sequence of the 283 calls of the module is identical to the binary (checked by a script, only the names of
  `yyerror`/`synrecover` differ), the remaining differences are those of the old compiler (D2, D3, D5, D6, D10). Data: the tables are equal, the strings are the same set; the layout
  differs (D12), so `cmpdata.py` shows differences for this module.

**Done: symbols (`symbols.c`, 3d90-4579).** Ten functions; names are ours. A name in the grammar is a node of
kind 7-9 whose word at offset 8 is the index of the symbol in the symbol table (`stabp(idx)`, `struct sym`:
`s_0` word, `s_2` long, `s_type` byte 6 (0 unknown, 1 defined, 3 alias of a reserved word), `s_7` flags byte 7
(bit 0, bits 1-2 = scope 0/2/4/6), `s_8` index of the itable entry, `s_12` block level of the definition); the
value is in the itable entry (`itabp(s_8)`: `s_0` = `section << 8 | subsection`, `s_2` the value).
- `defsym(np, val)` (3d90): `Unknown symbol`; an already defined symbol: a kind-0 name (extern) only counts `nfreed += 2`;
  scope 6 (local, `:=`) in the same block level -> `Symbol redefined`, in another level a new generation of the
  symbol (`fn479E(s_2, 6)`); other scopes -> `Symbol redefined`. Records 0x14 + word go to the intermediate file, and to
  the symbol file for a new symbol. Then a switch on the expression type (`fn4ECC`): 3 constant (value into the itable
  entry, kept if not larger when redefined), 1 relative to another symbol (copy of its `s_0`, value `fn4F52(val)`),
  0 and 4-7 `Invalid assignment`. `s_0` is taken from the pair table `lcaux[cursec][cursub]` (bss 0xac26, stride 4).
- `defalias` (4084, `:=` with a reserved word): type 3, the itable entry holds the pointer to the permanent symbol.
- `symval(idx)` (40f4) (a relocatable symbol carries its section in bits 24-30), `isdef(idx)` (4184: type 1, scope
  bits set, bit 0 clear), `symreloc(idx)` (41d6: relocation word of a symbol in an expression: scope 0 gives
  `index << 4 | 8` (`| 9` in the high word with `.seg`), scopes 2/4/6 add 2/4/6 for the area 0/1/2 (`Unknown area`) and with
  `.seg` the section byte; `Unknown scope`; a statement `v != 0;` without effect is in the binary).
- `defcomm(size, list)` (4312, .comm: `Invalid constant` unless the size is constant, then `defsym` for every name),
  `deflabels()` (4368: gives the labels of `plist` the value of the location counter, `plist = 0`),
  `putstabn/putstabp/putstabr` (43d4, 4472, 44ee: records 0x2a, 0x2a, 0x2b with the quoted string and the node
  operands in the symbol file).
- Variables seen: `blklevel` (data 0x54e0, nesting level of `{ }`, max 10), `cursec` 0x6bb2, `cursub` 0x6bb4.
- Check: every function compared with `cmpmod.py`; the remaining differences are those of the old compiler (the
  listing is in "Compiler differences" of `src/TODO.md`); the strings are the same set, the data layout differs (D12).

**Done: ssymtab (`ssymtab.c`, 457a-4b3d).** The three tables of the user symbols in the segments 7 (symbols, `struct sym`),
8 (itable, the values) and 9 (names) grow by `ssgbrk(seg, 0x400)` (`perror("symtab" / "infotab" / "namtab")` and
`done(-1)` when that fails); the first 0x30 bytes of a segment are not used, an index is the offset less 0x30 (0 = none).
- `stalloc(n)`, `italloc(n)`, `namalloc(s)` (457a, 4618, 46b6): bump allocators with the variables `stoff stlim itoff itlim
  ntoff nmlim` (data 0x54e2 ... 0x54ec, start 2 and 0x400); `stabp(idx)`, `itabp(idx)` (4772, 4788): index to address, 0 gives 0.
- `stenter(name, mode)` (479e) makes a symbol: a new hash chain head (10 bytes) or a chained entry (16 bytes: `s_14` =
  next, `s_7 = 8`); mode bits 1-2 = 6 (local, `~name`) put it on the list of the block (`blkstack[level-1]`, `s_10`
  = next of the block, `s_12` = block level, `s_7 |= 6`); an empty name always gets a fresh entry. `hashtab[1000]`
  (bss 0xb83a) holds the index of the first symbol of each `hash(1000, name)`, `blkstack[10]` (0xb826) the first local
  symbol of each level; `blklevel` starts with 1.
- `setsym(idx, type, kind)` (4926): `s_type`, `s_7 |= kind`, and an itable entry of 6 bytes (types 1, 3, 4) or 12
  (type 2) whose index goes to `s_8`. `stlook(name, mode)` (4996: `mode` unused) follows the chain (`s_7 & 8`, `s_14`)
  and compares the names. `blkopen()` (4a0e: `Nesting too deep` at level 10), `blkclose()` (4a30: takes the local
  symbols of the block out of their hash chains again).
- Check: all functions compared with `cmpmod.py` (the remaining differences: D13, the order of code from tail merging
  in `stenter`, scratch registers); the strings are the same set (D12).

**Done: locntr (`locntr.c`, 4b3e-4ecb).** Nine functions. `lctab[cursec][cursub]` (bss 0x9634) is the location counter of
each section (up to 255, `setsec`: `Too many segments`) and subsection (0 code, 1 data, 2 bss); the variables `cursec`
(data 0x6bb2, starts 0), `cursub` (0x6bb4, starts 1), `nsect` (0x6bb6, starts 1).
- `locadv(n)` (4b3e): `Segment overflow` when the counter would pass 0x10000, else adds n. `setsub(n)` (4bb0), `setsec(n)` (4cb4).
- `secname(np)` (4bbc, `.psec name`): a known section name (symbol type 4) selects its section (record 0x29 + number),
  an unknown one (type 0) becomes section `nsect++` (symbol type 4, kind 2; the itable entry holds `nsect<<8|1` and the
  number), anything else is `Invalid section name`; `.psec` without a name is `setsec(0)` and records 0x29, 0.
- `even()` (4cda) and `odd()` (4dc4): `locadv(1); iputloc()` when the counter has the wrong parity.
- `endpass1()` (4d10): for every section the three subsections are made even (records 0x29, section, then 13/12/14 for
  data/code/bss), then the current section and subsection are selected again.
- `dotval()` (4df6, the value of `.`): a fresh unnamed symbol (`stenter("", 0)`, type 1, kind 5) whose value is the
  location counter, returned as a name node. `setdot(np)` (4e6a, `. = expression`): for a type 1 expression the
  counter is advanced by the difference (`locadv`), then `iputloc()` and `nfreed += 3`; other types are ignored.
- Check: every function compared with `cmpmod.py` (the remaining differences: D2, D3, D4 in `setsub`, D6 in `endpass1`); the strings
  are the same set.

**Done: expr (`expr.c`, 4ecc-5f31).** Eighteen functions. An expression is a tree of nodes: constant (type 3, value in the
long at 6), name (type 7, the symbol index at 8) or operation (type 25: `c1` type of the result, `c2` operator, `l1` and
`l2` the operands). Operators: 0x1A +, 0x1B -, 0x1C *, 0x1D /, 0x1E ^<, 0x1F ^>, 0x20 ^&, 0x21 ^|, 0x22 ^X,
0x23 `| |` (short segmented address), 0x24 ^O (offset), 0x25 ^S (segment). The parser calls the `op...` functions; they fold
constants and build a type 25 node for the rest, which `exprval`/`exprreloc` evaluate in pass 2.
- `exprtype(np)` (4ecc) gives the type: 3 constant; a name: 7 undefined, 6 not a plain symbol, 4 external, 1 defined;
  an operation: its `c1`. Types seen: 0 (operand not known yet), 1 relocatable, 3 constant, 4 external, 5 short address, 6
  invalid (`Invalid expression type ...`), 7 undefined. `evalint(np)` (4f52): a name, or name + constant, as an int
  (`Must be simple expression`).
- `opadd`/`opsub` (5ccc, 59fc; `opplus`, `opneg` call them with a zero constant): constant + constant is folded
  (and two nodes are given back: `nfreed += 2`); relocatable/external/undefined plus or minus a constant keeps the
  operation node and folds the constant into its right operand (`(x + a) - b` becomes `x + (a - b)` or `x - (b - a)`); two
  relocatable operands of type 1 are evaluated with `symval` and give a constant (`Invalid addition expression` /
  `Invalid subtraction expression` for the rest). The binary operators `opmul opdiv opshl opshr opand opor opxor`
  need two constants (`Both sides of ^X must be constants`), or an operand of type 0 (then a node is built); each checks
  the type of the right side first (`Invalid expression type for right side of ...`).
- `opshort`, `opseg`, `opoff` (`| |`, ^S, ^O): a constant gives `((v >> 16) & 0xFF00) | (unsigned char)v`, `(v >> 16) & 0xFF00`,
  `(unsigned char)v` (the last one is passed as an int to `mknode1`, whose second argument is a long: the high word of the argument
  is whatever rr4 holds); other types give an operation node (the result type: 1 and 4 become 5, 0 and 7 become 0).
- `exprval(np)` (50b6, pass 2): the value of a type 1 or 4 operation (`symval(idx)` plus or minus the constant), of
  type 5 (`| |`: `Value too large` warning when the offset does not fit in a byte, ^O the offset, ^S the segment);
  `Undefined symbol` for 0 and 7, `Erroneous expression type` for 6. `exprreloc(np)` (4fb2): the relocation word
  (`symreloc`) of type 1 and 4, and for type 5 the flags 0x20/0x02 (`| |`), 0x10/0x01 (^S) depending on bit 3 of the
  relocation of the operand; ^O only tests it (no effect). It counts the short operators in `shortop` (bss 0xc00a, tested and
  cleared in assemble.c).
- Check: all functions compared with `cmpmod.py`; `opneg` and `opplus` are identical, the others show the differences of
  the old compiler (D2, D3, D4, D6, D10); the strings are the same set; the four jump tables (data 0x6c22-0x6c9f) are there (D12).

**Done: p2loop (`p2loop.c`, 5f32-6393).** Three functions, the loop of pass 2. `pass2()` (5f32) calls four initialisations of the
object file and the code generator (0x6cb2, 0x6394, 0x676a, 0x6634), sets `cursec = 0`, `cursub = 1`, `curline = 1`, reads the
tags of the intermediate file with `igetb()` until 0xF0 and calls `p2tag` and `chknodes()` for each; at the end it
writes the last listing line (`-l`) and, without semantic errors, the object file (0x6b96). `p2tag(tag)` (5fc2) is a switch over
the tags: 15 line number (`curline = igetw()`, listing line 0x94d4 with `-l`), 16/17/18 the next 1/2/3 lines, 11 location
counter (`igetw() + lcaux[cursec][cursub].la_0`; in code and data the gap to the counter is generated by 0x6da6, with the
listing switched off meanwhile, in bss the counter is set), 1/2/3 a byte, word or long element (0x6da6, 0x6e9a, 0x6f66), 10 a
segmented address (0x742a), 12/13/14 the subsection 0/1/2, 41 the section (`setsec(igetb())`, subsection 1), 6 a repeat
record (`p2rep(igetw())`), 7/8/9 an operand from a name (`mknode7(igetw())`) and 23/24/25 an operand read by
0x7fee, given to 0x6fba/0x70b0/0x71c2 (7 and 25, 8 and 24, 9 and 23 are pairs), 20 a word that is skipped, 38/39 jpr (0x89c2);
every other tag is `Unknown tag in intermediate file`. `p2rep(n)` (6262) reads the tag of the element that follows
and does the same n times (elements 1/2/3 with the count as the third argument, 7/8/9/23/24/25 in a loop), then `nfreed = nalloc`.
- Variables: `subsw` (data 0x72ec; set on every change of the subsection, tested and cleared in gen.c), `savelflag` (0x72ee).
- Check: all three compared with `cmpmod.py`; `pass2` is identical, the others differ in the old compiler's way (D2, D3, D6, D8); data
  and strings are equal in size (536 bytes) and content.

**Done: ofile (`ofile.c`, 6394-6da5).** Eight functions; the source includes `<s.out.h>`; the sccsid is `@[$]/as/as/ofile.c`.
- `symadjust()` (6cb2, the first call of pass 2): walks the symbol table (entries of 10 bytes, 16 for chained or local ones) and adds
  to the value of every defined relocatable symbol the sizes that precede it: data symbols the size of the code of their section (only
  with `.seg`), bss symbols the size of the data and (with `.seg`) of the code.
- `ohdrinit()` (6394): the header `hdr` (bss 0xc08e; magic 0xE607 with `.seg`, else 0xE711), the segment table `segtab` (16 bytes per
  section, `calloc`, error `segment table`) and a table of two longs per section `seektab` (`seek offset table`) are made from the
  location counters, which are cleared; `s_imsize` is code + data, `s_bss` the bss, `imoff` (0xc00c) = `s_segt + 24`. `ohdrput()` (6634)
  writes the header and the segment table at the start of the output file.
- `osymout()` (676a, reads the symbol file of pass 1): for the records 0x14 (a symbol) and 0x15 (a use; with `-u` an undefined one becomes
  external) it writes an s_nlist entry per symbol (not for local symbols, scope 6, and without `-d` not for scope 4);
  value = value | section << 24 for relocatable ones; `symtype()` (66cc) gives the type byte (1 abs, 2 code, 3 data, 4 bss, + 0x20 external, 0x20
  alone for an undefined symbol, whose value is cleared). The records 0x2a/0x2b (stabs, `stabout`, 6952) hold a string and constant
  expressions (value, section, type) and are written only with `-d` (`Stab field must be constant`). `nlwrite()` (6ad6) writes
  an entry: a name of more than 8 characters is stored with the length byte `(len+1)|0x80` and takes `(len-7)/14 + 2` entries of 14 bytes.
  `hdr.s_syms` = number of entries * 14.
- `ofinish()` (6b96, after the object code was written): appends the relocation file and the osym file, word by word, to the output file.
- Variables (bss): `imoff` 0xc00c, `stabname[128]` 0xc00e, `hdr` 0xc08e, `seektab` 0xc0a6, `nl` (the entry being written, with 128 bytes for the name) 0xc0aa, `segtab`
  0xc138; `shortop` (0xc00a) belongs to expr.c.
- Check: all functions compared with `cmpmod.py` (`ohdrput` and `ofinish` identical, the rest D2, D3, D6, D8); the strings are the same set.

**Done: gen (`gen.c`, 6da6-7429; the end was 729d before: `seekout` runs on to 7424 and `assemble` starts at 742a).** Eight functions that
write the object code; `fwrite` errors are `perror(outname)` (the relocation file: `perror(rfname)`) and `done(-1)`. In bss (`cursub == 2`)
`Bss cannot be initialized`; a changed subsection (`subsw`) first calls `seekout()`.
- `genb(c, a, n)` (6da6): n bytes c to the object file; if the location counter is odd the relocation word a goes to the relocation file; with `-l` the
  listing is told (0x941c); then `locadv(1)`. `genw(w, a, n)` (6e9a): n words and each time a relocation word, listing 0x93ec, `locadv(2)`;
  `genl(l, a, n)` (6f66): n times two words (high, low).
- `genlop(np, flag)` (6fba, a long operand): a number (with `.seg` and while `genaddr` runs the segment bit 0x80000000 is set), a name
  (`symval`, `symreloc`) or an expression (`exprval`, `exprreloc`; with `shortop` set only a word is written). `genwop(np, flag)`
  (70b0, a word operand, types 2 3 0x13, 7 8, 0x18 0x19; bit 0x8000 with `.seg` and flag; range check 0x7fae with the mask 0xFFFF0000), `genbop(np, flag)` (71c2, a byte:
  1 3 0x13, 7 9; mask 0xFFFFFF00). In `genbop` a `break` is missing before `default`: a name always gives `Must be number or relative address`
  (and, if its relocation word is not 0, `Bad relocation bits` before that); the binary has it so. `genaddr(np)` (7192): `addrflag++`, `genlop(np, 1)`
  with `.seg`, else `genwop(np, 1)`.
- `seekout()` (7286): positions the object file at `seektab[cursec].sk_off` (subsection 0) or `.sk_end` (subsection 1) + `imoff` + counter, and the relocation
  file at the same offset without `imoff` and with the counter rounded down to even.
- Variables: `addrflag` (data 0x75ee).
- Check: all compared with `cmpmod.py` (`genw` identical, the others D2, D3, D6, D8); the strings are the same set; the jump tables of the original are fewer
  (the old compiler uses compare chains for up to 3 cases, D6), so the data of this module is larger when rebuilt (D12).

**Done: assemble (`assemble.c`, 742a-7fed; compiled without `-O` like the parser: the prologues are behind the bodies).** Pass 2 assembles an instruction.
A record 10 of the intermediate file points to a row of `itab1`. `asmins(row)` (742a): `opcls + row->ir_cls` is the list of operand classes up to 0x12 or
0x1C; every class reads operands with `getop(n)` (829e, getops.c) into `ops[0..3]` (bss 0xc13e): 6/12/13 two operands; 8 the low 4 bits of the value;
17 the value `>> 4`; 9/10/11 a relative address (`asmrel(n, pc, class)`, pc = counter + length of the instruction); 0x1C one more operand. Then
`asmword(row)` (75f4) assembles the words: `instlen[kind].x[0..4]` are field codes: 1 ... 8 and 0xff take bits of the opcode word of the row (8 bits >> 8,
4 >> 12, 4 (>>4)&F, 4 &F, bit 7, bit 3, 2 bits >> 6, 7 bits >> 9; 0xff all 16), 9 goes on with the next descriptor; the other codes go to `asmfield(code)`
(774e): 14-18 and 21-23 (value less 1), 61-63 (value 0-7), 73/74 (value * 5 - 1) a 4 bit register field (`asmreg`, 7eb8, range check, `putbits(4, ...)`),
26-28 `genbop`, 29-31 `genwop`, 32-34 `genaddr`, 36/37 `genlop`, else `asmimm(code)` (7964): immediate fields of 1 to 16 bits made from the constants of
operands 0 and 1 (and 2, 3): masks, negation, `Operand too large`; then `asmfix` (7c2c, fixed fields 4 bits 0 15 14 2, 8 bits 0 0xff, 1 bit 0 1, 2 bits 0,
16 bits 1) and `asmfix2` (7ce6, 8 bits 0x8e 0x8f 0x0f 0x4f, 4 bits 4 8 0 12; else `Unexpected tag in intermediate file`).
`putbits(n, v)` (7f2c) collects the bits into `word` (bss 0xc13c; `nbits`, data 0x7716): more than 16 bits give `Too many bits assembled for word`, a full word is
written with `genw`; `rangechk(v, mask)` (7fae): `Value too large` (a warning) when neither v nor -v fits the mask. `asmrel` (7d84) turns a symbol or expression
operand into a node of type 0x13 holding the distance to the instruction (class 9 in words, 10 in bytes, 11 backwards in words; `Mixed relocatable and
absolute`, `Must be relative address`). The operand nodes are given back after the instruction (`nfreed++`, `ops[i] = 0`).
- Check: `cmpmod.py` pairs the functions of this module shifted by the prologue fragments (the labels in its list are those of the prologues), so its
  per-function counts are only a guide; the sequence of the 86 calls and the set of strings are identical to the binary.

**Done: getops (`getops.c`, 7fee-83ff).** Four functions that read operands. `igetexpr()` (7fee) and `sgetexpr()` (8146) read an expression in prefix
order from the intermediate file or the symbol file (tag 3 a long, 7 a name: `isdef` gives a constant node of its value, else a name node;
26 ... 34 two operands for `opadd opsub opmul opdiv opshl opshr opand opor opxor`, 35/36/37 one operand for `opshort opoff opseg`).
`getop(n)` (829e) reads operand n of an instruction from the intermediate file into `ops[n]`: tag 1 a byte, 2 a word (unsigned), 3 a long, 7/8/9 a name
(constant if defined), 23/24/25 an expression; else `Unexpected tag in intermediate file`. `sgetnode()` (8358) does the same for a node of the symbol file
(tag 2 gives a signed word; `Unexpected tag in symbol file`). Names: ours; the sccsid is `@[$]/as/as/getops.c`.
- Check: `cmpmod.py` differences are D3, D6 and D11 (the old popt shares the call of `mknode1` and the store into `ops[n]` between the cases, the functions
  are 20 instructions shorter); the strings are the same set.

**Done: error (`error.c`, 8400-84fb).** `error(n, msg)`: 1 a warning (`Warning: line %d: `, then the message as a format; returns 0), 2 an error of the
program (`Fatal error: line %d: `; only the first one of a line is printed, `lasterr` data 0x7d78 holds that line; `semerr++`; `nfreed = nalloc`
drops the nodes of the statement), 3 a syntax error (`Syntax error: line %d: `, `lasterr = curline`), 4 `Immediately fatal error: line %d: ` and `done(-1)`.
The messages go to `stderr` with `fprintf`. Check: one function, the differences are D3 and D6, the strings are the same set.

**Done: jpr (`jpr.c`, 84fc-89c1).** `jpr(tok, list)` (parser: `jpr`/`callr`, tok 0x50 jpr, 0x51 callr) looks at the operand list: nodes of type
0x36 are condition codes (counted in `ncc`; with callr `Invalid operand combination`), type 0x56 an expression, which must be a name or
name+/-constant (otherwise `jpput`, or `Expressions used in jpr must be constant`). With a known label (exprtype 1) `jprange` (8734) decides:
d1 = value of the label - location counter + offset, d2 = d1 less the growth of all jpr entries since the definition (`lcaux[][].la_0` minus
`sym->s_0`, times 2, 4 with `segmode`). jpr: -254 ... 256, callr: -4092 ... 4098. Both in range: 0 = `jrput` (jr/calr by `putins`), none: 1 = `jpput`
(jp/call), one: 2 = undecided. Undecided (or label not known): `jprnew` (8640) makes an itable entry `struct jent` (14 bytes: +0 `cursec<<8 & cursub`
(sic, `&`), +2 location counter, +4 symbol, +6 offset, +10 previous entry, +13 tok), counts `la_0++`, chains it in `la_1`; `jprput` (88ac) writes
tag 38 (jpr) or 39 (callr), the index, 2/1 (with/without condition) and both fields of every operand node to the intermediate file (read by `fn89C2`
in pass 2). Check: 6 functions; differences D3, D4, D6, D7, D8, D12 only; the strings are the same set (SCCS id 1.2 03/24/83).

**Done: jpr2 (`jpr2.c`, 89c2-8af3).** `jpr2(tag)` (pass 2, tags 38/39 of `p2tag`) reads the entry index and the condition flag, then looks at
`j_12` of the entry (set by the second pass logic when the jump became long): long = `jp`/`call` (`asmins` of row 4 of `jp` with a condition, else row 1;
`call` row 1) and `la_0` of the location counter grows by 4 (2 without `segmode`); short = `jr`/`calr` (row 1 with a condition, else row 0).
(Who sets `j_12` is not yet known; probably `optim`.) Check: one function, the differences are the address calculation of `lcaux[][]` (D3 class).

**Done: optim (`optim.c`, 8af4-92f5).** `optim(sub, sec)` (main calls it for every section with sub 1, 0, 2 at the end of pass 1) decides the length of the
undecided jpr/callr entries (`struct jent`, chained newest first through `j_10`, `jn = lcaux[sec][sub].la_0` of them). It gets a new segment
(`mkseg` b4e2, system call 55) of `(jn + jcap) * 6` bytes, `jcap = 2 * jn`: `jtab[jn]` of 6-byte records (`jr_0` distance, `jr_2` list) and behind it the room for list
entries (`newrec` 8c6e takes them, and grows by 0xC00 bytes with `ssgbrk` and `jcap += 0x200` when it is full; error `jump optimization`).
`jdist` (8e72): distance d1 of the entry to its symbol now, d2 with all entries between made long (2 bytes each, 4 with `segmode`);
a symbol that is not defined (or not relocatable) makes the entry long (`j_12 = 1`, distance 0x1004); both in range (jpr -254 ... 256, callr -4092 ... 4098)
gives 0, otherwise d1. `jdeps` (8cfa) puts, for each entry with a distance not 0, its index into the list of every entry in between (those that move the target).
`jiter` (8ffc) repeats until nothing changes: an entry out of range becomes long (`j_12 = 1`) and the distances in its list grow by 2 (4 with `segmode`) away from zero.
`jsum` (916a) makes `jadj[i]` the sum of the growth of the entries up to i (`jadj` overlays `jtab`), `jsymadj` (922e) adds `jadj[s_0 - 1]` to every label of the section and
subsection (symbol table walk, 10 or 16 bytes per symbol), at the end the location counter grows by `jadj[jn - 1]`. `la_0` and `la_1` are cleared.
`jpr2` (pass 2) then reads `j_12`. Check: 8 functions, the differences are D3, D4, D6, D7, D8, D11 and the `lcaux`/`lctab` address calculation; the strings are the same set.

**Done: listing (`listing.c`, 92f6-9693).** The listing (`-l`) is written in pass 2 to `listf`: a line is the location counter (`%04x `, only at the start of a line,
`lcol` is the column, 1 = start), the code (`lstb` a byte `%02x`, `lstw` a word `%04x`, then `lstflag`: `* ` relocatable (flag bit 3), `' ` symbolic (bits 1, 2), else two blanks)
and, behind column 19, the line number (`%5d `) and the source line (`lstline`, read from stdin, which was reopened, with `getc`/`putc`). A line of code that is
longer than 16 columns gets its source line at once (`lstend`, `lnl` counts the lines of code that wait); `lstsync(n)` (called when the line number changes) prints
the source lines up to n, `lstrest` the rest of the file at the end. `listhead` writes the head `LOC    OBJ CODE    STMT SOURCE STATEMENT`; `copyfile(dst, dname, src, sname)`
copies stdin to the temporary file with `fread`/`fwrite` (main). The variable at 0x95d4 that it clears and tests is `errno` (libc).
Check: 10 functions, `copyfile`, `listhead`, `lstw`, `lstend` identical, the rest differs by D3, D7, D8 and the inline `getc`/`putc` layout; the strings are the same set.

**Link of the complete cas.** `make` in `src/cmd/cas` links with `sld -i -s -e start -z -Mt4 -Md5 -Mb5 /lib/scrt0.o <modules in the order of MODULES> -Md7 stable.o
-Md8 itable.o -Md9 ntable.o -Md5 /lib/slibc.a -o cas`. Measured: the text is 46848 bytes like the original; `-z` gives the header flags 0x21 (STRIP, 7FSTK: stack in
segment 127, no stack segment in the table); without `-Mb5` the bss goes to segment 1, without the second `-Md5` the data of `slibc.a` go to segment 9. The three
table modules (`stable.c`, `itable.c`, `ntable.c`, sccsid ids from the binary) consist of the id and `char x[1024] = {0}`: id (48 bytes with padding) + 1024 =
1072 bytes, 1280 with the click, as in the original (`ssymtab.c` starts at 0x30 and grows the segment by 1024 bytes). The bss variables that no module defined
yet: `lctab[256][3]` (long, 9634) and `lcaux[256][3]` (struct of two ints, ac26) in `locntr.c`, `shortaddr` (a234) in `instr.c`; the variable at 0x95d4 is libc's `errno`.
data 38906 / bss 12294 against 38360 / 12328 of the original: the shipped scc lays out strings and alignment differently (D12), no difference in contents.

**Test of the complete cas.** Method: the original `/bin/cas` (z8k-emu) and the rebuilt one assemble the same source, messages (stderr, return code) and object must be equal;
scripts in `src/cmd/cas/test/` (README there). Used: (1) every opcode that `tools/z8k.py` decodes (37308 nonsegmented, 30444 short segmented, 30388 long segmented), written as
source; lines that the original refuses are removed and both runs are compared; (2) about 346 000 operand patterns for the mnemonics that z8k.py does not decode (EPU,
block and flag instructions), nonsegmented and segmented; (3) jr, djnz, dbjnz, calr, ldr/ldar, jpr, callr at 23 distances (0 ... 3000 words) and chains of up to 80
jpr at the limit of the short form (jpr.c, jpr2.c, optim.c); (4) 228 assembler files made by `cc -S` and `scc -S` from src/cmd and src/lib/libc; (5) the options -l -d -r -t -i -m -v -^
on 38 of these files (380 runs; for -i only the length of the intermediate file is compared, it holds addresses of the tables; after a semantic error only the messages, the
listing then shows leftovers of memory in the original too). Result: no difference. The 16 usable files of the kernel and library sources (the others are PLZ/ASM, which the
original does not accept) are equal too.
The Z8000 disassembler of the owner (`/mnt/share/git/Z8000-Disassembler`, GPL, not copied) has lists of all valid opcodes with text (`VERIF/`); as a check of z8k.py: mnemonic and length
agree for all 36253 nonsegmented rows; in the segmented lists 184 rows of res/set/bit with `@rr` are wrong in the list (made before the owner's fix; z8k.py rejects them, the original
cas gives the other encoding).
Faults that the tests found and that `cmpmod.py` had not shown (it compares calls, constants and stack slots, not the meaning of a case label or the type of a result): `putins`
(trailer of the EPU instructions: token 62 -> 0, 63 -> 4, 64 -> 3, 65 -> 1; the jump table of the original, not the order of the cases, decides), `asmword` (the codes 9 = whole word and
0xFF = next row were swapped), `cas.h` (`symreloc`, `italloc`, `stalloc`, `namalloc`, `mkseg`, `ssgbrk` must be declared: an implicit int cuts a long or a pointer), `asmins` (`asmrel` takes
a long: `(long)(unsigned)`), `symtype` (an external symbol gets its number `n`), the file `listing.c` (`errno`). Lesson: a function that returns a long or a pointer needs a declaration, an argument
must have the type of the parameter (K&R), and the case labels of a switch with a jump table are read from the table (`tools/swcmp.py` compares the switches of two programs).
Not equal, because the original has undefined behaviour there: (a) more than one semantic error in one statement with an expression (`.word 1^>Lab,1^>ext`): `error()` returns a long
whose low word is whatever `r3` held (the original) or `curline` (the shipped scc), the parser uses it as a pointer; (b) `"\%4"` or `"\%"` (less than two hex digits): the scanner goes
past the end of the string, the original ends with `Out of nodes`, the memory behind `nodes[]` differs; (c) the original itself does not terminate on a syntax error in the last statement
of a file and on some tokens (`<<`, `^$`, `5|3`); the test `exp.py` therefore leaves out the statements that hang the original.

## ar on the host (`host/ar`)

`host/ar/ar.c` is the WEGA `ar` for Linux, derived from `src/cmd/ar.c`: same commands (`rdxtpmq`),
options (`cuvnbail`), messages, exit codes and archives. `make -C host/ar check` runs 79 commands
(`scenarios.sh`: create, replace, update, delete, move, extract, print, list, quick append, position
options, error cases) with the original `/bin/ar` under z8k-emu and with `host/ar/ar`, in five time
zone settings, and compares output, exit codes, archives and extracted files; also `tv` of the
shipped `libc.a` and `slibc.a`. All identical.

- The 26 byte header (`name[14]`, big endian `date`, `uid`, `gid`, 16 bit `mode`, 32 bit `size`) and
  the magic `FF 65` are packed by hand; widths of the Z8000 are kept where visible (uid/gid are
  signed chars, dates 32 bit).
- `tv` prints with the WEGA `ctime`, not with the host's: `host/tools/lib/wctime.c` is a port of
  `src/lib/libc/gen/ctime.c`. Without `TZ` the time zone is EST5EDT with the US rule of that time
  (last Sunday in April to last Sunday in October, special days in 1974/75); `TZ` has the form
  `ESTn` or `ESTnEDT` (e.g. `MET-1MEST`), the same as on WEGA. Verified with `-e TZ=...` in the oracle.
- Temporary files: `mkstemp` instead of `mktemp`; the `l` option keeps them in the current directory.
- Differences that cannot be avoided: the text after `ar write error:` comes from the host's
  `strerror`.
- Test pitfall: files extracted by `x` get the current time; the scenarios reset it with `touch`,
  otherwise archives built later contain it. Member names are cut to 13 characters.

## ranlib, strip, size on the host (`host/binutils`)

`ranlib`, `strip` and `size` for Linux, derived from `src/cmd/{ranlib,strip,size}.c`. `make -C
host/binutils check` runs 40 cases with the originals under z8k-emu and with the host programs on
copies of a corpus (all members of `libc.a`/`slibc.a`, binaries, 30 libraries of the tree, byte swapped
copies, damaged and non-object files, archives with swapped, stripped, text and too many members) and
compares output, exit code and every file afterwards: all identical.

- Helpers in `host/tools/lib`: `sobj.c` (the file structures are byte arrays in the Z8000 layout; libc
  `goodmagic`, `swab`, `longswap` ported, so byte swapped objects behave as there) and `conf.c`.
- `size`: exit status always 0. `strip`: exit 2 for usage and errors that stop, 1 for a file that was
  skipped. 8-bit objects (E507, E511, E505) with relocation removal are not supported (the original
  result depends on uninitialised variables, no such file exists in the tree): the host strip refuses.
- `strip` closes the output file at once; the original leaves it open, but a second use of the same file
  in one call already sees the data.
- `ranlib` calls `ar` like the original (`ar rlb first archive __.SYMDEF` for a new table, `ar rl
  archive __.SYMDEF` to replace one) and then sets the date of the first member to now + 5 s. The
  host `ar` is `../ar/ar` next to the executable, `ranlib.conf` (`ar = path`) can name another.
- What the original ranlib really does (found with the oracle, and confirmed in the binary): the
  `ar` command line is `ar rl <archive> __.SYMDEF` (the new table `ar rlb first archive __.SYMDEF`);
  `goodmagic` gets the magic value; and the `readsym` linked into the binary is the one of an OLDER
  libc than the shipped `libc.a`: it reads the continuation pieces of a long name over the symbol
  itself (`fread(sym, 14, n)`, not `sym + 1`). So a symbol with a long name (more than 7
  characters after the length byte, e.g. `_getgrgid`) is tested and stashed with the first piece
  of the name text: most long names are missing in the table and some enter it with garbage
  (entries with an empty name). The table of `libc.a` has 238 entries, not the ~290 a correct
  ranlib would write. That is the behaviour of the WEGA binary, not a bug of ours; the host ranlib
  and `src/cmd/ranlib.c` keep it (identical `__.SYMDEF`). The tables are not cleared between two
  archives of one call.
- `src/cmd/ranlib.c` now follows the binary: the earlier version had the `ar rl` arguments swapped
  and passed `&exp` to `goodmagic`; the old readsym is `src/lib/oldlibc/readsym.c`, linked in by
  `src/cmd/Makefile`. Built with the WEGA cc the program behaves exactly like the original (same
  `__.SYMDEF` for libc.a, slibc.a, libcurses.a); the code is not byte identical because the original
  was compiled by an older cc (own code and readsym differ in register use, frame and spills).

## tar (reconstructed, byte-identical)

`src/cmd/tar.c` rebuilds `/bin/tar` (Version 1.8.88, 22494 bytes) byte for byte with the shipped
`cc -O -s` under the oracle (`cmp` against the original, header and data included).

- Compare method: `tools/cmpins.py` on the own code (0x42 up to the first libc member at 0x1db8), then
  `cmp` of the whole file. Differences in the data section show up as shifted string addresses.
- String pool order is the order of the strings in the source, but `popt` moves code blocks
  (`codemove` in `host/popt/z21.c`: a `jr` to a block that is preceded by an unconditional jump or
  `ret` is replaced by the block itself; loop inversion). So the code order of the original can differ
  from the source order. Use the string addresses to recover the lexical order: in `readarchiv` and
  `writearchiv` the "change error-disk" / "format error-disk" blocks stand before the read/write in
  the code, but behind it in the source (retry with `goto readblock` / `goto writeblock`).
- A `goto` or `done(n)` written in several places is not merged by `popt`; the shared `done(2)` of
  `writearchiv` comes from the source form `if (...) { ...; goto writeblock; } done(2);`.
- Initialised `int` globals go to the data section, uninitialised ones to bss: `disks` has no
  initialiser (a `= 1` adds a data word and moves the other bss variables).
- Operand order matters: `BLOCKS * disks` gives `ld r3,BLOCKS; mult rr2,disks`, `disks * BLOCKS` the reverse.
- The frame of a function is locals + 2 bytes (a spilled argument counts as a local): `writearchiv` has
  `char cmd[98]` for `sub r15,#0x66`.
- `dumpblock()` (debug dump of the header block, `dflag >= 2`) prints each byte as `%02x`, then
  `"%c"` for printable characters or three blanks, and at the end `Checksummme: gelesen = %o berechnet = %o`.

## popt (C optimizer, /lib/popt)

`popt/` is a portable C decompilation of the shipped /lib/popt (written from the binary only,
not from Plexus c2). Modules like the original: z20 tables, z21 (input/output, jump optimizations),
z22 (rmove: register contents tracking, redundant loads, push/pop, shifts), z23 (isused liveness
and `stripldm` register renaming). Verified byte-identical (output, `-l`, `-d` incl. statistics)
against the original run in z8k-emu on 174 compiler outputs of the WEGA sources plus ~200 other .s
files: `make -C popt check WEGA=<tree> CORPUS=<dir>` (`popt/corpus.sh` builds the corpus; the corpus
is compiler output and is not kept in git). Details and reproduced quirks: `popt/NOTES.md`.

## cc (compiler driver, /bin/cc)

`host/cc/cc.c` is a decompilation of the WEGA `/bin/cc` driver (14208 bytes, E707
combined I/D; own code 0x2a-0x15f0, libc from 0x15f2).  It is a portable K&R
C program; `make -C WEGA/host/cc` builds it for Linux with the host compiler.

- The driver only runs other programs: `/lib/cpp`, `/lib/cparse`,
  `/lib/gopt` (levels -O0-5, skipped for '0'), `/lib/codgen`, optionally
  `fpf` (-F) and `/lib/popt` (-O), then `/bin/cas` and `/bin/ld`.  Passes
  talk through `/tmp/cc.<pid>.{I,D,s,o,p,z,S}` (names `cc.<pid>.x` in the
  current directory with -:d).
- Command lines are built with sprintf and executed by `callsys`, a small
  tokenizer (`<in`, `>out`, `>>out`) + fork + `execv`; ENOEXEC goes to
  `/bin/sh`.  `-:z` echoes every command line, `-:n` executes nothing; this
  is what `cc/check.sh` uses to compare the original (under z8k-emu) with the
  portable build: 74 option combinations, command lines, messages, exit codes
  and created files identical (pid normalized).
- `scc` is the same program (`host/cc/cc.c` built with `-DSCC`, `make -C host/cc` builds `cc` and
  `scc`). Measured against the original `/bin/scc` (disassembly diff of both drivers, then
  `check.sh PROG=scc`, 74 command lines identical): only the strings differ. Defaults `/lib/scparse`,
  `/lib/scodgen`, `/lib/spopt`, `/bin/sld`, `/lib/scrt0.o`, `/lib/smcrt0.o`, `/lib/slibc.a`,
  `/z/bin/lib/sfpe.o`, `sfpent.o`, `slibcfpf.a`, `/z/bin/sbasic`, `/z/bin/sf77`; cpp is called with
  `-DNULL=0L`; gopt gets the flag `-Z` (initialised `gbuf`); the loader gets `-X -e start -z`.
  Config file `scc.conf` (template `scc.conf.example`). Verified end to end under the emulator:
  `scc -O -o h h.c` with the original passes gives the same executable as the original `/bin/scc`
  (only the process id inside the temporary names differs, as between two runs of the original).
- Configuration: `cc.conf` next to the executable (template `host/cc/cc.conf.example`, the file
  itself is local) sets the pass programs, `crt0`, `mcrt0`, `libc`, `mv`, `shell` and `tmpdir`. A pass
  may be a command with arguments, e.g. `cas = ${_emu} /bin/cas` to run the original under z8k-emu
  (then `access()` checks of `gopt`/`fpf` are skipped and `ld` is exec'ed with its arguments in front).
  Order: built-in default, `cc.conf`, then `-T<x>`. The parser is the library `host/tools/lib/conf.c`
  (`conf.h`, `make -C host/tools/lib check`), reused by later drivers (`scc`). `/bin/mv` and the
  temporary directory are no longer fixed. Under the emulator `tmpdir = .` is needed: a relative path
  means the same for the host driver and the guest.
- Pass paths can be changed with `-Tx<path>`, flags of a pass with `-:x<flags>`,
  all passes can be taken from a directory with `-B<dir>` (+ `-a<ids>`).
- Quirks kept: `-o file.c` prints "would overwrite" and exits 1; an unknown
  option letter is handed to ld one letter at a time (`-x`); `-Ns` adds `-Ns`
  for ld and `-L` for cparse and ends the argument; `-l...` is passed to ld
  with the preceding byte overwritten by '-'; `-o` as last argument leaves
  outfile NULL (the original reads a zero byte at address 0).
- `-Sl` runs the listing program `/lib/clist`; failures of gopt, popt and fpf
  only print a warning and the compile goes on without that pass.
- Not done: building cc.c with the WEGA cc under z8k-emu (the original was
  probably built with the older cc; an exact own-code match is unlikely).
