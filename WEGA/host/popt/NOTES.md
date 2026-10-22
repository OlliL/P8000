# popt (WEGA /lib/popt) - decompilation notes

Goal: portable C that turns `cc -S` output into the same text the shipped
/lib/popt produces (oracle: z8k-emu running the original; corpus in a scratch directory (CORPUS=...),
built by corpus.sh).  Source is written from the WEGA binary (not from Plexus c2).

## Binary layout (stripped, 22272 text, 3308 data, 3420 bss)
- text 0x42.. : z21 (main, input, getline, oplook, output, refcount, iterate, xjump, ...),
  z22 (rmove 0x13b6 ... register tracking helpers), z23 (procopt 0x33d6 ...).
  z20.c = tables (optab/ccodes/revbr) in data.  libc (old cc) from ~0x3838 on.
- node (14 bytes): op(0) subop(1) forw(2) back(4) ref(6) labno(8) code(10) refc(12)
  `first` node at data 0x158c.

## Op numbers (optab at data 0x4e: {char *name; int opcode})
1 CJP (jpr cc,L)   2 JP (jpr L / jp x)   3 LABEL (Lnnn:)   4 DLABEL (named label)
5 '{'   6 JSW (.word Lnnn)   7 LD   8 CLR  9 COM  10 INC  11 DEC  12 NEG  13 TST(test)
14 SRA 15 SLA 16 EXTS 17 CP 18 ADD 19 SUB 20 BIT 21 AND 22 OR 23 MUL 24 DIV 25 SDA 26 XOR
27 .code 28 .data 29 .word 30 .even 31 SOB(djnz) 32 CALL(call/callr) 33 PUSH 34 POP 35 RET
36 LDM 37 LDA 39 SET 40 RES 41 rl 43 rr 44 jp 45 SRL 46 SLL 47 SDL 48 .long 50 '}' 51 .psec
52 comment /*~~ (kept) 49 other comment (dropped) 53 .addr
`ldk` is LD with subop 0x12.  subop 0x10 = byte (b suffix), 0x11 = long (l suffix).
cc names live in the same hash table: eq ne le ge lt gt ult ugt ule uge feq fneu flt fle fgt fge
(opcode word 0x0001|cc<<8).

## Status
`popt` (z20.c tables, z21.c flow, z22.c register tracking + rmove, z23.c liveness
and register renaming) reproduces the original byte for byte on all 174 corpus
pairs, for the plain run, `-l` and `-d` (the statistics on stderr including the
"NK core" line).  `make check WEGA=... CORPUS=...` (corpus.sh builds the corpus
with the original cc/popt in z8k-emu).  Also identical on ~200 further .s files.

## Quirks that are reproduced on purpose
- `getnum` is a 16 bit parse; hash `((c0<<3)+2*c1+c2)&0x7fff % 73`, name[1]/[2]
  read as 0 beyond the NUL; probing continues past empty slots only for names
  ending in b/l.
- killreg() looks for the long register with `makereg(n % 1 = 0, TL)`, i.e. always rr0.
- Code of nodes without operand text is read from address 0 of the original (see
  `nullcode`); dualop/singop/ovlap/markregs use the same bytes.
- procopt (z23): markregs() records r0..r7 in a[], and in b[] only the long (rr/rq)
  uses of r8..r14; renames the top saved registers to unused low ones.  Pairs only
  when n odd, b[n-1] set, n<=13, free register even with an odd partner free;
  otherwise the free register is skipped.  The "preceded by alnum" test of the
  match looks at the low byte of the entry length (never alnum) - no test in effect.
  Shrinking the ldm to one register uses the LAST comma (libc 0x548c is rindex).
  A name shorter than the old one is padded with 0xff and compacted afterwards.
  The `rN(` recursion of the original passes a character as a pointer (garbage);
  not reproduced, never occurs with cc output.
- isused(p, reg) = usesreg() for each register of the (rr/rq) name; calls, labels,
  CJP/JP count as "used" (CALL: r2..r7 used, r0/r1 killed), RET uses r2 r3 r14.
  usesreg() restores RT1/RT2 from the node before returning.
- Memory: bump allocator over sbrk, break grows by 2000, reset after each function;
  `heaplim` model gives the "NK core" figure (initial break 0x1a48 = data+bss).

## spopt (/lib/spopt, the segmented variant)

`/lib/spopt` (E711, 22528 text, 3312 data, 3420 bss, modules z20.c 1.3, z21.c 1.4, z22.c 1.6, z23.c 1.6 like popt) is the same program with
a few changes; `make spopt` builds the same sources with `-DSPOPT`. Found by comparing the disassembly of both binaries (52 hunks,
about 10 real changes), all in the code marked `SPOPT`:
- input: `.long Lnnn` (not `.word`) becomes a JSW node; output prints `\t.long\tL%d`. Output has a PSEC case: the subop
  that movedat() left in the node is cleared before the mnemonic lookup.
- movedat(): sections are `.psec [name]` + `.code`/`.data`; the PSEC node's subop gets the kind of the next directive; data
  sections move in front; a header that repeats the previous kind and name is dropped, an empty section in front of another
  header too. The original starts with `clrb @r9` (r9 = whatever main's caller left): the name of the "previous" section is
  an empty string; not reproduced as a write.
- rmove: `ld` to a register nobody uses is kept when it is r8 and follows a call; the push/pop conversions compare
  `isreg(@rrN part) == r - 1` (rr14 is r14/r15); an `inc`/`dec` that is not a push/pop clears the register contents (`reset`,
  popt only goes on with `disp`).
- dest(): `lda` gives a long; ilen(): a jump without operand text is 6 bytes; adrlen(): `#` is 6 (long) or 4, an operand with `|`
  is 4, `x(reg)` is 4 when the base is a register, anything else 6.
- usesreg(): ret uses r2 r3 r12 r13; a call through `@reg` counts for reg and (always, `reg ^ 1` is not 0) any register >= 2.
- procopt: the free registers are paired from r13 downward (`13 - k`).
- Heap start 0x1a4c (data is 4 bytes longer): "NK core" in `-d`.
- Both popt and spopt: `conloc` (the memory location known to hold a constant) has 20 bytes in the original, directly followed by
  the statistic counters `ndirect` and `nbrbr`: an operand of 21 or more characters (`|_stkseg+~L1+142|(fp)`) overwrites them, and
  `-d` prints "10496 operations made more direct" (0x2900 = ")" and NUL). setcon() does the same to the two counters.
