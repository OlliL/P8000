# The old C compilers of WEGA

This is the edited summary of everything known about the C compiler revisions that built the WEGA binaries: the
differences D1 ... D20 between them and the shipped compiler, which object shows which, and the old compiler
versions we rebuild (v1, ...). It is rewritten when the knowledge changes, never appended to. The running notes,
measurements and attempts go to `OLDCC_LOG.md` (dated entries, append only); the evidence per function is in the
findings.json of each program (`src/cmd/*/tools/findings.json`). Owner decisions are marked with their date.

Contents: 1 Terms and rules, 2 Compiler versions, 3 The differences D1 ... D20, 4 Objects and their differences,
5 Open questions.

## 1 Terms and rules

### Terms

- **shipped**: the compiler of the WEGA 3.1 tree (`/bin/cc`, `/bin/scc` and their passes `/lib/cparse`, `/lib/codgen`,
  `/lib/popt`, `/lib/scparse`, `/lib/scodgen`, `/lib/spopt`). Our normal build uses it.
- **old**: a revision of the compiler that built an original binary and of which no binary exists. Its output is only
  known from the binaries it made.
- **Dn**: one difference between the output of an old compiler and that of the shipped one (section 3). A D class is
  a description of the output first; its mechanism in a pass is known only when a switch reproduces it.
- **switch** `OLD_Dn`: a compile-time switch in the sources of a pass (`src/cmd/cparse`, `codgen`, `popt`) that turns the
  shipped behaviour into the old one. The normal build sets none, so it stays byte-identical with the shipped passes.
- **version** vN: a set of switches that together give one old compiler (section 2), built in `src/cmd/<pass>/vN` and
  run through the driver `ccvN`.
- **witness**: a function whose original code a switch makes equal. **Test bench**: `host/recon/witness.py`, the
  System III sources of WEGA programs of one compiler time, compiled with a version and aligned with the originals.

### Pipeline and where a difference comes from

`cc` runs cpp > cparse > [gopt] > codgen > popt > cas > ld (`scc`: scparse, scodgen, spopt, sld). cparse writes the
intermediate tree (`.I`) and the data as cas text (`.D`); codgen chooses registers, frame and instructions; popt is the
peephole optimizer (descends from the Plexus `c2`, `c20.c 1.1`, Aug 1982). The Plexus one-pass `ccom` is not an ancestor
of the WEGA code generator (it emits Plexus assembler: `jpr`, `csv2`/`cret2`). A D class is assigned to a pass by
experiment: one pass of the chain replaced by its version, the rest shipped.

### Building and running an old version (owner decision 2026-10-31)

- The old versions are native WEGA programs only (built with the shipped cc, run in the emulator); no host versions and
  no cross environment of them. The host versions (`host/codgen`, `host/popt`, ...) get no switches.
- One source per pass; every old version has its own directory with its own Makefile (`src/cmd/cparse/v1/Makefile`,
  `codgen/v1`, `popt/v1`, the driver `cc/v1`). The version Makefile defines the switches, the libc, crt0 and the install
  name; the pass Makefile only descends into it (`make old`). Built programs are not in git.
- Driver: `cc.c` with `OLD_V1` runs `/lib/v1/cparse`, `/lib/v1/codgen`, `/lib/v1/popt`; cpp, gopt, cas, ld, crt0.o and
  libc.a are the shipped ones. `make install PREFIX=dir` in the version directories installs `bin/ccv1` and `lib/v1/*`.
- Emulator: z8k-emu maps every absolute guest path below its ROOT (relative paths go to the host working directory).
  `make -C host oldroot` builds all v1 programs and installs them into a tree of symbolic links to the WEGA tree
  (`host/tools/oldroot.sh`, default `tmp/oldroot`, bin and lib real directories, guest /tmp shared), so the original tree
  stays untouched. Then `z8k-emu -r <oldroot> /bin/ccv1 -c -O foo.c`.

### Evidence rules (owner decisions 2026-10-31)

- The D classes are hard only where a program was checked strictly (`strictcmp.py`: only code addresses masked); a
  class read from a masked or pattern comparison is weak and may hide a wrong source form. ld, strip and size were
  checked strictly before the first switch; every other program is checked the same way before it counts.
- A switch counts only when it explains findings in at least two independent functions, preferably in different
  programs. A function that fits only through a switch no other function needs is a suspect source form and goes back
  to the source check; so do findings that no switch explains. (Lesson of ld: ranlib_scan, dseek and load2td looked like
  compiler differences and were source forms.)
- Regression: every function that is strictly exact stays exact with the switch, in every program built by the same
  compiler version. The version of a program comes from its build date (section 2); a program of another time is no
  counter-witness (owner decision 2026-10-31).
- The test bench is read only by comparison (with and without a switch): WEGA changed many of the System III sources,
  so a difference on the bench is not a compiler difference by itself.
- Every accepted switch: commit (no push), this document and the table of section 4 updated, the normal build stays
  byte-identical (shipped cparse, codgen, popt), the corpora green.

### Time box (owner decision 2026-10-31)

- A round is one hypothesis for a switch (one D class, one assumed mechanism), made and measured under the evidence
  rules. Progress: two findings in independent functions become exact without a regression, or a new measured
  explanation (for example the place of the mechanism in a pass).
- A D class is parked after three rounds in a row without progress, and also when its switch would change the normal
  build or need a large rebuild of a pass (for example a new table structure in codgen). Parked: measurements and attempts
  in `OLDCC_LOG.md`, go on with the next class.
- The whole work stops (state to the handoff) when five classes in a row are parked or no open class is left.
- Review by the owner at the first milestone or after 20 rounds.

## 2 Compiler versions

### Dating

Most programs carry the stamp of their start-up file, `@[$]crt0.s Rev : 3.x <date>`, made at the build
(`src/lib/libc/crt/mkcrt0.sh`, `crt0.list`); the libc members linked into a program date it too. The passes cparse,
codgen, popt and cas have no stamp; their newest source dates are cparse 10/06/83, codgen 08/24/83, popt 01/18/83
(z20.c 1.3 12/21/82, z21.c 1.4 12/21/82, z22.c 1.6, z23.c 1.6 03/25/83), cas 07/83. The date of a source does not date the
compiler that built it.

| Time (build stamp) | Programs (examples) | Compiler |
|---|---|---|
| 01/83 .. 05/83 (crt0 3.1) | size 01/14, strip 01/24, rmdir, write 01/05, ls, sort 02/23, rm 05/19, ... | v1 |
| 07/83 (crt0 3.2) | ld, cp/ln/mv 07/26, login 07/26, yacc 07/21 | v1 (ld, mv checked) |
| 08/83 .. 10/83 | csh 08/11, sh 08/22, troff 09/09, adb 09/28, sld 10/27, cparse (sources 10/06) | open; cparse shows no D1, so not v1 |
| libc 09/12/83 and later | ar 11/18, tar, dump, restor, format, verify, cc, scc, ... | shipped (byte-identical with the shipped cc) |
| no stamp | codgen, popt, cas, ranlib (ranlib.c 07/24/82) | open; cas by an older scc |

### shipped

Builds byte-identically: cc, scc, ar, tar, dump, dumpdir, restor, sync, getud, putud, format, verify, copydisk, pburn,
the doscopy programs, the standalone programs (`make -C host verify-wega`). All of them are linked with libc members of
09/12/83 or later.

### v1 (01/83 .. 07/83)

Status: closed as a milestone (2026-10-31, owner decision). The switches below are the version; the open ld functions
are known remainders with one witness each and are taken up again only when a new witness (another program of the
time, or another version) points to their mechanism.

- Switches: `OLD_D1` and `OLD_D12` (cparse), `OLD_D3` (codgen), `OLD_D20` (popt). The libc of the version Makefiles is
  that of the shipped passes (the libc does not change what the passes write).
- Byte-identical with v1: `/bin/size`, `/usr/bin/strip`.
- ld: 55 of 59 functions strictly exact, data equal, corpus 146 of 146 (open: place_sec, chk_ranlib, load2td,
  read_symbols; section 5).
- Function-equal with v1 (addresses masked, frame offsets compared; not linked yet): mv (`src/cmd/mv.c`, all 7), rm (all
  6), write (all 5), rmdir (rmdir; main differs by D4 and D18).
- Test bench (`witness.py`, System III sources of 26 programs from 01/83 .. 07/83, the stdio.h of the time from
  `src/cmd/cparse/stdio.h`): 89 of 121 functions equal (the shipped cc: 27). The rest are WEGA changes of the sources.
- Without a switch on the bench: without OLD_D1 31 of 119 equal, without OLD_D3 61 (both with 72).
- stdio of the time: `char *_ptr` and the V7 forms of getc/putc (`--_cnt >= 0` first); the stdio.h of the WEGA tree has
  `unsigned char *_ptr` and the other order.

### The compiler of /lib/cparse (later than v1, not built yet)

Seen in cparse (classes read from the strict diffs, `strictlayers.py`, weak): D12 holds (the data under the D12 model),
D1 of v1 does not (its register variables have no frame places: OLD_D1 breaks lxinit, moedef, clearst). Also seen: D18
(never `@r15`), D1 in other forms (the shipped cc reserves a home slot for register parameters and rounds the frame to
4, the cc of cparse does neither; no frame for leaf functions like getlab, noinit), D2, D3 (a spilled parameter kept in
its register across branches), D6, D9, D10, D11 (shared code between the TNEXT arms of talloc), D19.

### The old scc of cas, the old cc of ranlib, codgen, popt

Their differences are in the table of section 4. cas was built by an older scc (D5, D13 are only seen there); the cas
parser and `assemble` without `-O`, the other modules with `-O`. They are not assigned to a version yet.

### The old libc members inside size, strip, ranlib, cas

The Jan 1983 libc that is linked into these binaries (only as machine code there) shows D14 and differs from the shipped
members by its source and by the compiler (D1, D8). Its members are kept as cas sources in `src/lib/oldlibc`.

## 3 The differences D1 ... D20

Each class: the form in the code, the pass, the mechanism as far as measured, the switch, the evidence. "old" = the
compiler that built the original. Not differences of the compiler (checked):

- Position of the prologue: with `-O` both compilers put `dec r15,#n; ldm ...` in front of the body; without `-O` the
  prologue is behind it (`jr Lp` at the entry). `cmpmod.py` converts the behind form before it compares.
- The gopt defect of `cc -Or` (frame offset of register pointer parameters, also on the real machine): no libc function
  was built with `-Or`.
- Older revisions of the libc sources (Jan 1983 `_doprnt`, `readsym` of ranlib): source differences.

### D1 Frame size and frame places

- Form: frames of another size, saved registers at other offsets (`ldm S10(r15),r13` against `S12`).
- Mechanism (v1, switch `OLD_D1` in cparse, `oalloc` in vinit1.c): the old cparse gives every register variable its
  place in the frame too, at its declaration, like an automatic variable of its type; the shipped one only char and
  short register variables under -O. The `spare` locals of the early ld drafts were exactly these places (number of
  spare words = number of register variables).
- Evidence: ld (30 functions lose their spares and stay exact), strip (`copy`; byte-identical), the bench (31 > 72),
  /bin/mv (frames 16, 0x12, 10 of main, pname, dname; shipped 12, 12, 8).
- Other forms, not reproduced: the temporary of a return value (codgen, reader3.c, above the locals) is in every
  function of ld with a return value except read_symbols; one unexplained frame word in main, parse_args, slookup,
  pslookup, index16 of ld (kept as `int spare`); cparse's compiler makes no frame for leaf functions and no home slots
  for register parameters; cas `putins` 12 bytes against 14, libc nlist and length 2 bytes smaller, fseek 14 against 20.

### D2 Scratch registers

- Form: another register for a temporary: rr0 where the shipped cc takes rr4 (cas), rr4 against rr2 (ld), a copy to r2
  in front of a use (codgen `expand`, ld place_sec and chk_ranlib); register variables of leaf functions in scratch
  registers (sld).
- Mechanism: in the register allocator of codgen (rsuse/rsfree lists, rsfreeall at labels); not found.
- Refuted (ld and bench): registers kept at labels with their contents forgotten (bench 69 > 54), only at labels no jump
  has named yet, the parameter registers kept at labels, merged pairs put behind the others, freed words in LIFO
  order, symreg only on idle registers. Two ld findings were source forms (ranlib_scan as a `for` loop, dseek as one
  `||`), strip's expression went with v1 without a D2 switch.
- Open: place_sec (`r8 = (secorg[j] - q->tsize) & 0xff00`: the original takes a new register for the `and`, not in the
  `+ 255` case beside it), chk_ranlib; cas, codgen, ranlib (weak).

### D3 Register contents

- Form: the old compiler knows that a register still holds a value (no reload), or loads where the shipped one uses the
  register; the old popt keeps a reload after a store (cas: `ld S0,r7` followed by `ld r7,S0`).
- Mechanism (v1, switch `OLD_D3` in codgen, cbranch in reader3.c): at the join label of `&&` and `||` the shipped codgen
  gives the idle registers back (`rfreeall(1)`), the old one keeps them with their contents.
- Evidence: ld init_entry and dseek, the bench (61 > 72).
- Risk: a register loaded on one path only keeps its symbol too (not seen; would be a quirk of the original).
- Open: ld chk_ranlib (`q = p` loads p from the frame, `p + n` uses r7 right after), libc malloc (`mult rr2,r6`).

### D4 Spill of arguments

- Form: fewer parameters spilled into their home slots; another order of the copies of register parameters.
- Evidence (weak): ranlib (no spill), rmdir main (a third parameter that is only written is not spilled: `ldl
  %0002(r15),rr6` against v1's `ldm @r15,r5,#3`), codgen (register parameters have no home slots, `ld r14,r7; ld
  r13,r6` first parameter first; the shipped cc spills more: getlr, member, nullsyms), libc swap (an address-taken
  argument spilled in the `0(r15)` form), sld (a long parameter cast to a pointer read from its spill slot).
- Switch: none yet (the spill decision is in efcode3.c, `regargs`).

### D5 Constant index into local arrays

`a[3]` of a local array addressed directly (`ldb S5(r15),rl2`); the shipped scc computes the address (`ldl rr2,#S4;
add r3,r15`). cas `nodes` (`escape`), the parser; popt (weak).

### D6 switch

Up to 3 cases the old compiler compares one after the other and puts the dispatch in front of the bodies; the shipped
one uses a `cpir` table from 1 or 3 cases on (values, then a jump table behind the bodies). With 4 or more cases the old
one uses the table too, values sorted, bodies in source order. The order of the case bodies follows the source; in
codgen `tshape` (FLD before STAR) and `conval` (LT, LE, GT, GE, the unsigned four, then EQ, NE) the source order was
changed to the original's. The d12model of ld and cparse lays the tables out the old way (`D6TABLES`).

### D7 Loop layout

Measured with `scc -O -S`: `while`/`for` loops and goto loops with the test at the head are rotated (the entry jumps to
the test at the bottom); without `-O` the head stays on top. Open: sld 0x7C26 keeps its head on top. codgen `expand`,
`buildstar`. The ld drafts emulated bottom tests with gotos; with v1 the natural loops are right (ranlib_scan, load2td).

### D8 Conversion and loads of bytes

Unsigned char argument: old `ldb rh7,#0`, shipped `subb rh7,rh7` (cas ifile); old libc length `ldb rl3,@r2` + `testb
rl3` against `testb @r2`; char loads as `ldb`/`subb` or `extsb; and #FF`.

### D9 Shift by 8 bits

`w >> 8` as a byte move (`ldb rl7,rh7`) instead of `sra r7,#8` (cas ifile, cparse).

### D10 Shift of a long by a constant

Old `ld r2,#24; sdal rr4,r2` (also `(long)w << 16` through rr0), shipped `slal rr4,#24` (cas, cparse).

### D11 Tail merging (popt)

The old popt merges common tails of branches (`call xtol; jp` shared by the number cases of the cas scanner; codgen
`acon`: three branches into one `printf` call; ld read_symbols: the store behind a dead `ldl rr4,rr2` shared with the
end of the loop). Not reproduced.

### D12 String literals and data layout

- Form: the old compiler writes the initialised data and the switch tables in source order and all string literals of
  a file behind them as one pool, aligned only at its start and end, equal strings not merged; the shipped one writes the
  strings of a function in front of it, each padded to an even address.
- Mechanism (v1, switch `OLD_D12` in cparse, putstrs at the end of the file into the last section of the .D file).
- Evidence: the data of ld, size, strip, mv, rm, write, rmdir equal; the d12model of cparse (`host/recon/d12model.py`)
  gives its data.

### D13 Pointer constants (scc)

The old scc keeps bit 31 of a segmented address constant (`ldl rr2,#%87000030`), the shipped one clears it (and can stop
with `register jam` for `(char *)CONST + n` into a register variable). cas `ssymtab`.

### D14 Varargs prologue and post-increment (old libc members)

The printf family of size, strip and cas spills the EPU registers inline (`.word %4FFC,%8303,off`) and stores r6..r2
besides the `ldm`; ranlib's has the shipped form. In size `*fp->_ptr++` uses the byte before the `inc` (fread, fwrite,
the putc in `_doprnt`). Checked without D14: 35 functions of cas and the members of size, strip, ranlib. `_filbuf` of
size and ranlib: the conditional value goes through a stack temporary (no class yet).

### D15 Block layout of if/else

Measured: the old compiler keeps the source order of then and else and places goto labels where they stand. So a
reordered if/else cannot explain a block-order difference. The D15 entries of codgen (`zzzcode`, `clocal`), sld and ld
load2td are block-order differences whose cause is open (load2td: popt moves the then block of `if (flag)` in front of
the switches; a popt rule that did it is refuted by the bench, see D20).

### D16 Frame operand in arithmetic

The old compiler loads a frame slot into a register before an operation where the shipped one uses it as operand
(`add rS,F`), or the other way round. codgen `sucomp`, `tshape`; ld dseek before it was solved. Pattern verdict only.

### D17 Call form (popt)

The original popt calls a global function of the same file with the absolute `call`, a static one with `calr`; the
shipped build always uses `calr` inside a file. Pattern from popt (166 against 22 sites), not tested with a compiler.

### D18 Frame operand with offset 0

The cc of /lib/cparse never writes `@r15` (506 times `%0000(r15)`); ld and codgen use `@r15`. v1 writes `%0000(r15)` for
the home of a parameter where rmdir has `@r15`. The comparisons of cparse treat both as equal (progcfg `D18`).

### D19 Order of evaluation of the operands of a binary operator (cparse)

The old cc evaluates the left operand of `+`, `==`, `>` first when both are calls; the shipped cc the right one. cparse
`argregs`, `rmconv`, `clocal`: our sources write the operands the other way round (the old cc will need the natural
order).

### D20 Code motion of popt

- Form: blocks behind a jump or return moved to the site of a jump to them (`codemove`, z21.c) in other places.
- Mechanism (v1, switch `OLD_D20` in popt): a `jpr` to the label right behind it does not end the block to be moved
  (the old popt had removed that jump already), so short blocks in front of the function exit stay.
- Evidence: ld ensure_buf, the `return 0` of read_symbols; neutral on the bench. Turning off the whole code motion is
  wrong (the old popt moves the prologue and the switch dispatch too).
- Refuted: going on behind a block moved from behind a jump (ld load2td exact, bench 72 > 69).

### Register allocation of the old scc (measured 2026-10-27, `scc -S`)

The register variables are assigned in declaration order from r13 downwards; an int or char takes one register, a
pointer or long the highest free aligned pair; a `register` parameter comes first. A leaf function puts them into
scratch registers (D2). The declaration order is part of the source form. (Nonsegmented cc: from r14 downwards.)

## 4 Objects and their differences

`x` = the difference was seen in this object; `?` = the construct is in the source but the difference was not separated
from the others yet; empty = not seen (no proof that it is absent). All cas modules with code are listed (table1, table2,
pstab and the three table segments are data only). The old libc members inside size, strip, ranlib, cas are listed with
D14 only: that is all that was checked for them. cparse (`strictlayers.py`): `x` for D1, D2 means the strict difference
shrinks when the frame or the registers are hidden; D6 and D10 hunks of exactly the measured forms; D12 the data model;
D19 the operand swaps; `?` for the other classes: hunks of that form (weak). The codgen rows are the marks of the
function findings (`src/cmd/codgen/tools/findings.json`, `classify.py`), completed by reading; D12, D13, D14 not checked
for codgen, D15 only for zzz3.c and tr3.c. ld, strip, size: the state with the shipped cc; with v1 see section 2.
Column "Ver.": the compiler version (section 2).

| Binary | Object | Ver. | D1 frame | D2 scratch regs | D3 reloads | D4 arg spill | D5 const index | D6 switch | D7 loops | D8 bytes | D9 >>8 | D10 long shift | D11 tail merge | D12 data order | D13 pointer const | D14 varargs, post-incr | D15 if/else layout | D16 frame operand | D17 call form | D18 0(r15) | D19 operand order | D20 code motion |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cas | ifile.c | old scc | x | x | x |  |  | x |  | x | x | x |  | x |  |  |  |  |  |  |  |  |
| cas | instr.c | old scc | x | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | scanner.c | old scc | x | x | x |  |  | ? | ? |  |  |  | x | x |  |  |  |  |  |  |  |  |
| cas | main.c | old scc |  | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | nodes.c | old scc | x | x |  |  | x | x | ? |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | parser.y (parser.c) | old scc | x | x | x |  | x | x | ? |  |  | x |  | x |  |  |  |  |  |  |  |  |
| cas | expr.c | old scc | x | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |  |  |  |  |
| cas | p2loop.c | old scc |  | x | x |  |  | x |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| cas | ofile.c | old scc |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | gen.c | old scc |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | assemble.c | old scc |  | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |  |  |  |  |
| cas | getops.c | old scc |  | x | x |  |  | x |  |  |  |  | x | x |  |  |  |  |  |  |  |  |
| cas | error.c | old scc |  |  | x |  |  | x |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | listing.c | old scc |  |  | x |  |  |  | x | x |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | optim.c | old scc |  |  | x | x |  | x | x |  |  |  | x | x |  |  |  |  |  |  |  |  |
| cas | jpr2.c | old scc |  |  | x |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | jpr.c | old scc |  |  | x | x |  | x | x | x |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | locntr.c | old scc | x | x | x | x |  | x |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| cas | ssymtab.c | old scc |  |  |  |  |  | x |  |  |  |  | x | x | x |  |  |  |  |  |  |  |
| cas | symbols.c | old scc | x | x |  |  |  | x |  | x |  | x | x | x |  |  |  |  |  |  |  |  |
| cas | pst.c | old scc | x | x |  |  |  |  | ? | x |  |  |  |  |  |  |  |  |  |  |  |  |
| cas | subr.c | old scc |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |  |
| ranlib | ranlib.c, readsym.c | open | x | x |  | x |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| strip | strip.c | v1 | x |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| size | size.c | v1 |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| libc.a | malloc.o (gen/malloc.c) | open |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | nlist.o (gen/nlist.c) | open | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | length.o (gen/length.c) | open | x |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | swap.o (gen/swap.c) | open |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| size | libc: printf.o (printf, fprintf, sprintf) | old libc |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |
| size | libc: rdwr.o (fread, fwrite) | old libc |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |
| size | libc: doprnt.o (_doprnt) | old libc |  |  |  |  |  |  |  |  |  |  |  |  |  | ? |  |  |  |  |  |  |
| strip | libc: printf.o (printf, fprintf, sprintf) | old libc |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |
| cas | libc: printf.o (printf, fprintf, sprintf) | old libc |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |
| codgen, scodgen | main3.c | open |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | reader3.c | open |  |  |  | x |  | x | x | ? |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | rdr3.c | open |  |  | x | x |  | x |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | efcode3.c | open | x |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | local3.c | open |  | x |  | x |  | ? |  |  |  | x |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | gen3.c | open |  |  |  | x |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | order3.c | open |  |  |  | x |  | x |  |  |  |  | x |  |  |  |  | x |  |  |  |  |
| codgen, scodgen | match3.c | open |  |  |  | x |  | x | ? |  |  |  |  |  |  |  |  | x |  |  |  |  |
| codgen, scodgen | allo3.c | open |  | x | x | x |  | x | ? |  |  | x |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | regallo3.c | open |  |  |  | x |  | x |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | regfile3.c | open |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tmp3.c | open |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | zzz3.c | open |  |  |  |  |  | ? |  |  |  |  |  |  |  |  | x |  |  |  |  |  |
| codgen, scodgen | comm3.c | open |  |  | x | x |  | x |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | out3.c | open |  | x |  |  |  | x | x | x |  | x | x |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | regs3.c | open |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | lcl3.c | open |  | x |  | x |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tr3.c | open |  |  |  | x |  |  | x | ? |  |  |  |  |  |  | x |  |  |  |  |  |
| codgen, scodgen | optim3.c | open |  |  |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| popt | z21.c (main..backjmp) | open |  |  |  | x |  | x |  | x |  |  |  |  |  |  |  |  | x |  |  |  |
| popt | z22.c (rmove..natural) | open |  | x |  | x | x | x |  |  |  |  |  |  |  |  |  | x | x |  |  |  |
| popt | z23.c (procopt..markregs) | open |  | x |  | x | x |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |
| ld | ld.c (read_symbols, place_sec, init_entry, load2td, dseek, ranlib_scan) | v1 | x | x | x |  |  |  |  |  |  |  | x | x |  |  | x |  |  |  |  | x |
| ld | symbols.c (chk_ranlib, ensure_buf) | v1 |  |  | x |  |  |  |  |  |  |  | x | x |  |  |  |  |  |  |  | x |
| ld | copyname.c | v1 |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| ld | printname.c | v1 |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| mv, rm, rmdir, write | mv.c, rm.c, rmdir.c, write.c | v1 | x |  |  | x |  |  |  |  |  |  |  | x |  |  |  |  |  | x |  |  |
| cparse | cgram1.y | cparse | x | x | ? |  |  | x |  |  |  |  |  | x |  |  |  | ? |  | ? |  |  |
| cparse | xdefs1.c | cparse | x | x |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  | ? |  |  |
| cparse | scan1.c | cparse | x | x | ? |  |  | x |  |  |  | x |  | x |  |  |  |  |  | ? |  |  |
| cparse | pftn1.c | cparse | x | x | ? |  | ? | x |  |  |  | x |  | x |  |  |  |  |  | ? |  |  |
| cparse | trees1.c | cparse | x | x | ? |  |  | x |  |  |  |  |  | x |  |  |  |  |  | ? | x |  |
| cparse | optim1.c, optim | cparse | x | x | ? |  |  | x |  |  |  |  |  | x |  |  |  |  |  | ? | x |  |
| cparse | code1.c | cparse | x | x | ? | ? |  | x |  |  |  | x |  | x |  |  |  |  |  | ? |  |  |
| cparse | local1.c | cparse | x | x | ? |  |  | x |  |  |  |  | ? | x |  |  |  |  |  | ? | x |  |
| cparse | comm1.c, common | cparse | x | x | ? |  |  |  |  |  |  |  |  | x |  |  |  |  |  | ? |  |  |
| cparse | vinit1.c | cparse | x | x | ? |  |  | x |  |  |  | x |  | x |  |  |  |  |  | ? |  |  |
| cparse | sym1.c | cparse | x | x | ? |  | ? |  |  |  |  |  |  | x |  |  |  |  |  | ? |  |  |

Open for codgen: `clocal` (D15), `zzzcode` (D15, partly), `sucomp` (D16), `tshape` (value form of the SZERO return, D16;
the current case order is the best of seven), `expand` and `freereg` (moved blocks, D7; findings `expand-layout-01`,
`freereg-layout-01`); `conval` was fixed by the case order (finding `conval-layout-01`).

## 5 Open questions

- How many compiler versions are there, and when did each change? The table of section 2 is filled from the crt0 stamps
  of the checked programs only; a full table of all originals (stamp, libc state, classes) is the next step.
- Which version built /lib/cparse, /lib/codgen, /lib/popt (final test: our passes compiled with the old compiler are
  byte-identical, `src/TODO.md`)? cparse shows D12 but not D1 of v1.
- The remainders of v1 in ld: place_sec and chk_ranlib (D2/D3), load2td (code motion), read_symbols (no return
  temporary, the dead `ldl rr4,rr2`); one witness each, possibly source forms.
- D4: the rule of the old spill (rmdir main: a parameter that is only written is not spilled).
- The classes of cas, codgen, popt, ranlib were read by masked or pattern comparison; they are weak until those
  programs are checked strictly.
