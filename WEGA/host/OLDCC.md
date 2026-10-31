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

The full list of the 373 programs of the tree is `OLDCC_DATES.md` (generated by `host/recon/datemap.py`): crt0
stamp, own SCCS ids, libc marks and the D12 measure. D12 can be read from the data of a binary without its source: the
old cparse packs the string literals, the shipped one pads every odd one to an even address. The measure takes the
first 16 strings that decide (the own modules lie in front of the libc). Reading (2026-10-31):

| Time (build stamp) | D12 of the data | Programs (examples) | Compiler |
|---|---|---|---|
| 1981 .. 1982, no crt0 stamp (sources 07/82 ..) | packed | chmod, dc, m4, eqn, cpp (12/82), ranlib, the plz tools | v0 (D1 tight) |
| 10/82 .. 11/82 (crt0 3.1) | packed | ex, vi, at | v0 (D1 tight); shut, dd 11/82 .. 12/82 loose |
| 01/83 .. 05/83 (crt0 3.1) | packed | size, strip, write, ls, sort, passwd, mount, rm, lint1 | v1 |
| 07/83 (crt0 3.2) | packed | ld, cp/ln/mv, login, get, yacc | v1 (ld, mv checked) |
| 08/11/83 | packed | csh | v1 (last packed one) |
| 08/22/83 .. 09/01/83 | padded | sh, rsh, kdb, fsdb | D1 slots with the shipped D12: between v1 and shipped |
| 09/09/83 and later | padded | troffv7, adb 09/83, sld 10/27, ar, make, ps 11/83 | shipped (D1 loose) |
| no stamp, built late | packed | cparse, scparse, codgen, scodgen (sources 10/06/83), popt, spopt, gopt (03/83), cpp, find 10/83, upkeep 11/83 | v0 (D1 tight): the passes were built by the compiler of 1982 |
| no stamp, libc 09/83 | padded | cc, scc, tar, format, verify, copydisk, gld, pburn | shipped |

The second mark is D1, read from the frames of the functions that save register variables (`datemap.py`, column D1):
"slots" (as many never used frame words as saved registers: v1), "tight" (no unused word: v0), "loose" (the shipped
cc). So: v0 up to 11/82, v1 from 01/83 (the change in 12/82 .. 01/83, where shut, dd and some acct programs read loose),
D12 of cparse changed between 08/11/83 (csh) and 08/22/83 (sh), D1 between 09/01/83 (fsdb) and 09/09/83 (troffv7). Exceptions to check: nroff 07/08/83
padded (its first strings may be the binary hyphenation tables), find 10/25/83, upkeep 11/22/83 and stty 12/21/83
packed (built by an old compiler late, or few strings), id 05/83 and the getty programs mixed.

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

### v0 (1982; it also built the compiler passes) — started 2026-10-31

- Time: the programs of 1982 (sources 07/82 .., crt0 3.1 up to 11/82) and, built late, the passes cparse, scparse,
  codgen, scodgen, popt, spopt, gopt, cpp and find, upkeep. Marks: tight frames (D1 "tight": no unused frame word, no
  return temporary, leaf functions without a frame) and packed strings (old D12).
- Switches so far: `OLD_D12` (cparse); `OLD_D2R`, `OLD_D18`, `OLD_D1R`, `OLD_D2W`, `OLD_D10`, `OLD_D3P` (codgen); popt without a switch
  (OLD_D20 is neutral on the bench). Candidates in the sources, not set: `OLD_D2D` (codgen) and `OLD_D1F` (popt), see
  "Leaf functions" below. No OLD_D1 (no frame places of register variables), no OLD_D3 (with it the bench gets worse).
- Directories `src/cmd/{cparse,codgen,popt,cc}/v0`, driver ccv0 (`cc.c` with `OLD_V0`, passes in /lib/v0), installed by
  `make -C host oldroot` together with v1.
- Test bench v0 (`witness.py -t v0`, System III sources of 38 programs of 1982, 153 functions): shipped cc 15, v1 14,
  v0 64 (v1 without OLD_D1: 15; + OLD_D2R: 18; + OLD_D18: 48; + OLD_D1R: 60; without OLD_D3: 61; + OLD_D2W: 62, the
  differing lines 1624 > 1542; + OLD_D10 and OLD_D2R narrowed to loads through a pointer: 64, 1466 lines; OLD_D1R
  without any return temporary: 64, 1463; + OLD_D3P: 65, 1462).
- /lib/cparse as the second test bench (2026-10-31, ccv0, d12model, strictcmp): 44 strictly exact (shipped cc 29);
  cparse built with ccv0 normally passes its corpus like the shipped build (239 of 259 inputs identical, the rest
  differ only in the heap contents of the .S file). The behaviour check is needed: the test benches count equal
  instructions only and do not see wrong code (see OLD_D2R).
- /lib/cparse built with ccv0 before OLD_D10 (d12model, strictcmp): the code is as long as the original's (48896 bytes; shipped cc
  49152), data equal at the same address 17472 of 18584 bytes (shipped 17170), 38 functions strictly exact (shipped
  29; gained mkty, mainp1, lxtitle, bcon, openfiles, bstart, bend, fldal, noinit, tcheck, putstrs; lost savebc, psave). With OLD_D2W: still 38, code 48896, data equal at the same address
  17411 (code addresses in the data moved).
- Leaf functions (2026-10-31): the renaming of register variables into free low registers and the removal of the ldm
  is done by popt (`procopt`, z23.c), in the shipped compiler too; codgen writes r14, r13 with their save. In v0 more
  low registers stay free because its codgen takes r0 and rr0 early (`OLD_D2W`) and puts the dividend of an int
  division into the odd half of rr0 (or of the pair of the wanted register) and the division into that pair (`OLD_D2D`,
  order3.c rallo, allo3.c allocreg); the old popt then also removes the stores into the frame and the frame itself when
  no register is saved and nothing reads the frame (`OLD_D1F`, procopt). With these cal jan1 is 8 lines from the
  original (23 without); the bench: OLD_D1F alone 61/1621 lines, with OLD_D2D 61/1594, both on top of OLD_D2W 62/1561
  (OLD_D2W alone 62/1542): OLD_D2D costs elsewhere (crypt setup, col main, paste main, sdiff main), so both are
  candidates, not set.
- Open (bench): the first register of a statement r2 where ours takes r3 (the getc macro on a global stream); the switch dispatch
  `ld r2,TAB(r3)` against `add r4,r4; neg r4; ld r2,TAB(r4)` (D6); `call` for global functions of the same file (D17:
  probably the old cas, which resolves `callr` to a later global function as `call`).

### Seen in /lib/cparse (classes read from the strict diffs, before v0)

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
- v0 (switch `OLD_D1R` in codgen, delay() in reader3.c): no temporary for the value of a return; with a delayed x++ the
  value stays in r2, which stays taken while the increment runs (`ld r2,_crslab; inc _crslab,#1`: cparse getlab, no
  frame). The bench of v0 goes from 48 to 60 equal functions (first form: a temporary only for a delayed x++).
- Other forms, not reproduced: the temporary of a return value (codgen, reader3.c, above the locals) is in every
  function of ld with a return value except read_symbols; one unexplained frame word in main, parse_args, slookup,
  pslookup, index16 of ld (kept as `int spare`); cparse's compiler makes no frame for leaf functions and no home slots
  for register parameters; cas `putins` 12 bytes against 14, libc nlist and length 2 bytes smaller, fseek 14 against 20.

### D2 Scratch registers

- v0 (switch `OLD_D2W` in codgen, rweight in regallo3.c): r0 has the weight 3 instead of 8, so r0 and the pair rr0 come
  early (the originals of v0: `div rr0` 68 times, `rr2` 135, `rr4` once; v1 and shipped: never rr0); bench 61 > 62,
  1624 > 1542 lines. Weight 0 or 1 makes codgen run endlessly (bench timeouts).
- v0 (switch `OLD_D2R` in codgen, allocreg in allo3.c): a load through a pointer (STAR, OREG) goes into the register of
  its address first (freereg), a free one only after it (`ld r7,@r7` where the shipped codgen writes `ld r6,@r7; ld
  r7,r6`); word registers only. The first form (for every node) gave the bench 15 > 18 but wrong code in cparse
  copysym (`*pp = q` stored q into itself, `ldir @r3,@r2,r3`): an operand register that is still needed; narrowed to
  loads, bench 62 > 64 together with OLD_D10, cparse 39 > 43.
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
- v0 (switch `OLD_D3P` in codgen, regforsym in lcl3.c): a name that lives in a register is taken from it also when
  the node wants another register (the shipped codgen then loads it from the frame): cparse argdebug `return(x)` as
  `ld r2,r7`; bench 64 > 65.
- Open: ld chk_ranlib (`q = p` loads p from the frame, `p + n` uses r7 right after), libc malloc (`mult rr2,r6`).

### D4 Spill of arguments

- Form: fewer parameters spilled into their home slots; another order of the copies of register parameters.
- Evidence (weak): ranlib (no spill), rmdir main (a third parameter that is only written is not spilled: `ldl
  %0002(r15),rr6` against v1's `ldm @r15,r5,#3`), codgen (register parameters have no home slots, `ld r14,r7; ld
  r13,r6` first parameter first; the shipped cc spills more: getlr, member, nullsyms), libc swap (an address-taken
  argument spilled in the `0(r15)` form), sld (a long parameter cast to a pointer read from its spill slot).
- Switch: none yet (the spill decision is in efcode3.c, `regargs`). Refuted for v0: spill only the parameters whose
  home is used (cparse fldal spills its unused parameter; bench 65/1462 > 65/1468). Open in v0: leaf functions have no
  spill and a smaller frame (cparse argdebug: `dec r15,#4` and no store; cal jan1 no frame at all).

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

Old `ld r2,#24; sdal rr4,r2` (also `(long)w << 16` through rr0; right shifts `ld r0,#3; neg r0; sdll`), shipped `slal
rr4,#24` (cas, cparse). v0 (switch `OLD_D10` in codgen, match in match3.c): the table entry 024 (`OIZB AL,AR`, the
immediate shift) is not used for a long, so the entries 025 .. 030 load the count into a register; cparse bccode exact.

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

v0 (switch `OLD_D18` in codgen, the spills and saves of efcode3.c and gen3.c written as `0(fp)`): the bench of v0 goes
from 18 to 48 equal functions. The cc of /lib/cparse never writes `@r15` (506 times `%0000(r15)`); ld and codgen use `@r15`. v1 writes `%0000(r15)` for
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

- The times of D3 and D20 are not known (no mark read from a binary yet).
- v0 is not complete: the open forms of its bench (leaf functions, the first register, D6 dispatch, D17 by cas) and the
  final test (cparse, codgen, popt built with ccv0 byte-identical).
- The compiler of 08/22/83 .. 09/01/83 (sh: slots with the shipped D12) is not built; it may be v1 with the shipped
  cparse.
- Which version built /lib/cparse, /lib/codgen, /lib/popt (final test: our passes compiled with the old compiler are
  byte-identical, `src/TODO.md`)? cparse shows D12 but not D1 of v1.
- The remainders of v1 in ld: place_sec and chk_ranlib (D2/D3), load2td (code motion), read_symbols (no return
  temporary, the dead `ldl rr4,rr2`); one witness each, possibly source forms.
- D4: the rule of the old spill (rmdir main: a parameter that is only written is not spilled).
- The classes of cas, codgen, popt, ranlib were read by masked or pattern comparison; they are weak until those
  programs are checked strictly.
