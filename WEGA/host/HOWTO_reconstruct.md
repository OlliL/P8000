# HOWTO: reconstructing a binary of the WEGA system

This is the procedure for every original binary (program or library). It was developed on `codgen`,
`popt` and `sld` (2026-10; `sld` is the most complete: structs, names, corpus test). Facts about a
specific binary go to `KNOWLEDGE.md`, open items and status to `src/TODO.md`, per-function findings to
`findings.json` of that program. Keep this file as the procedure only.

Ground rules: byte identity with the original is the goal. Bugs of the original are rebuilt and
recorded, not fixed. Originals and copyrighted text never go into git. Plexus and V7 sources are read
for ideas and names only. Repo text is English, no umlauts in added text. Makefiles must also work with
the WEGA native `make` (`=` not `?=`, `cd dir ; $(MAKE)`, no pattern rules). The owner commits; the
assistant never commits or pushes. No new status files: the state lives in `own_functions.txt`
(status column) and `findings.json` (`manual_classes` = hand notes, shown in `FINDINGS.md`).

## Two phases, one model change per binary

The work is split so that a cheap model does the mechanical part and an expensive model is needed only
once. Do not change back and forth.

**Phase A (cheap model): everything where a tool answers yes or no.** Steps 0 to 3, the first pass of
step 4 (draft, compile, compare; at most 3 attempts per function, then mark it and go on), the layout of
data and bss, the split into modules, the link and the corpus harness. Phase A ends with a handoff
note (`HANDOFF_<binary>.md` in the tool directory, deleted when the binary is finished) that contains:
the function table with status, the list of functions with the first differing instructions (`firstdiff`
output), what was tried, and the open questions. Phase A is done when:

- function table complete, every function has a C draft and status `done` or `findings` (with the diff);
- data and bss of the rebuilt program have the size and the variable addresses of the original;
- the program is split into the modules of the original, each compiled alone, and the link works;
- the corpus test (step 6) exists and runs; the own-code block (step 3) passes it, which proves the harness.

**Phase B (expensive model): everything that needs understanding.** Hard functions of the second pass,
semantic errors found by the corpus test, structs, names, the quirks table, the documentation. Phase B
uses the rules of step 7 first; most "register" differences are wrong types, parameter order or
declaration order, not compiler differences.

## Step 0: inputs [A]

- The original binary in the WEGA 3.1 root (`/mnt/share/cvs/P8000/tmp/wega31_root`), run by the
  emulator `host/z8k-emu/z8k-emu -r <root>`. The emulator sees only files below the working directory.
- The header and layout: `host/KNOWLEDGE.md` (s.out format, segmented E611 or nonsegmented).
- Analysis tools in `host/tools/`: `sout.py` (s.out parser), `z8k.py` (decoder, use the segmented
  flag for E611 programs), `cmpins.py`, `unlink.py` / `unlink_seg.py` (cut of programs, verified by
  assembling with `cas -u`), `sdis.py`.
- Look for strings first: usage and error messages, SCCS what-strings (`@(#)` / `@[$]`), debug dump
  strings. They give the module list and order, names of functions, variables and struct members
  (sld: `dump.c` strings gave all member names of the segment tables).

## Step 1: function starts [A]

1. Inventory (`inventory.py`): recursive descent from the entry, proven starts, candidates after `ret`
   or jumps. Entry set = call targets in the code range plus proven starts; every entry is an
   instruction boundary; each function ends with `ret`, an unconditional jump or a trap.
2. Do not split at jump targets. Mark large functions as a guess when they may hold jump-only entries.
3. Write the table (`own_functions.txt`: entry, name, end, size, callers, kind, last instruction, status).
   Status: `open`, `done`, `findings:<D classes>`.
4. Decode data references with the segmented flag of the file (the non-segmented decoder gives wrong
   operand kinds).

## Step 2: libc and shipped members [A]

1. Identify the linked library members against `/lib/libc.a`, `/lib/slibc.a` and the old libc
   (`libc_funcs.py`, weak fingerprint, check by hand). Compare code with the relocations masked;
   check the DATA of each member too (sld: the data of printf/doprnt differed and had to be written out).
2. Members that match byte for byte are taken from the library with `ar x` (not cut, not copied).
3. Other library code is cut as assembler (`unlink.py` / `unlink_seg.py`), kept only if verify gives 0
   differing words. Code addresses in jump tables and data references must be symbolic (labels), not
   absolute numbers, or the module breaks when the layout moves.
4. The link order of the members is the order in the original; a data-only member (`data.o`,
   `ctype_.o`) goes where its data lies in the original data (measure it).

## Step 3: own code as one block [A]

Cut the own code range as one module (`unlink_seg.py`), verify 0 differing words. This block is the
reference for the link and the first corpus run (the link of it must reproduce the original).

## Step 4: data and bss before the functions [A]

Do this before the long function work; wrong addresses make every instruction differ.

1. Data order = order of the SCCS strings and the initialized variables per module; strings are in the
   order of the source statements of each function (the old cc does not merge equal strings; a string
   used in a different order than the original means a different statement order or a static array in
   front of the function).
2. Bss: the old linker allocates commons in the order of the FIRST MENTION in the objects in link
   order; the group of the second segment comes first. Put all common definitions into the first
   object in the address order of the original (`order_vars.py`), unused gaps become spare variables,
   array sizes are the address distances. Check every variable address against the original
   (`var_addr.py` finds them by instruction voting).
3. Test: size of data and bss equal, then all variable addresses equal.

## Step 5: functions in C [A first pass, B second pass]

1. One C file per module of the original from the start (the old compiler loses function names in big
   files); each file is compiled alone with the original `scc -O` in the emulator.
2. For each function: write K&R C (no prototypes), compile, compare with `strict_audit.py` (frame
   offsets are compared; addresses are masked). Exact: `accept.py` (status `done`). Otherwise
   `firstdiff.py NAME` shows the first differing places with context, `odis.py NAME` the original.
3. Phase A: at most 3 attempts, then write the diff into `manual_classes` (basis `read`, note) and go on.
4. Names: from messages, system calls, strings; Plexus/V7 names where it is the same function.
   External names are significant for 7 characters (`check_names.py`).

## Step 6: corpus test: original program against the rebuilt program [A, run always]

The strict audit says "exact" for code that is wrong when an immediate and a memory operand, a cast or
a mask differ. Build the program, run it and the original on the same command lines under the
emulator and compare output files, messages and exit codes (sld: `make check`,
`host/ld/check.py -S` with `SLD_NEW`). Run it after every batch of edits. If it fails:

- find the first function that behaves differently with the call trace: `z8k-emu -t`, take the `call`
  / `calr` lines (the next trace line is the callee), map the addresses to names (`nm` of the new
  program, `own_functions.txt` and `libc_names.txt` for the original) and compare the sequences with
  their arguments; then compare the flow inside that function.
- the usual causes: a literal that is a variable, unscaled pointer arithmetic, a wrong width or sign
  of a cast (zero extension), a swapped argument.

## Step 7: rules for the functions that do not match [B]

Found on sld; each one fixed several functions (details in `KNOWLEDGE.md`, "what fixed the findings
functions"):

- Read the original with `odis.py`. The first argument is in rr6, the second in rr4, a word argument
  after them. Wrong parameter order or type shows as different spill offsets. Check the callers.
- Register variables are given from r13 downwards in declaration order; a pointer takes two. A
  parameter that is not spilled is `register`. `ldm` shows how many registers are used. A copy
  `ldl rrN,rr6` of a spilled parameter is `register char *p; p = a;`. A register that is incremented
  is the pointer, not a counter.
- Unused locals reproduce a bigger frame (`spare`); the order of declaration gives the frame offsets.
- Loops: our compiler writes `while` with the test on top; a loop with the test at the bottom is
  written with `goto test; body: ...; test: if (c) goto body;`.
- Stack arrays accessed with constant offsets are structs. A stack record that is built and written is a
  struct. `(char *)&h.f - (char *)&h` gives a field offset.
- Expression forms: `switch (x)` with the real case values, not `switch (x - c)`; `!= 0` gives
  `and; test`; `x = n; x >>= 1;`; pre/post-decrement tests; `?:` as call argument; `(unsigned)` for
  zero extension; assignment inside the test avoids a copy.
- The order of the cases is the order in the code; shared tails are the optimizer (popt tail merge).
- The call for the "not an option" case sits at the end of the loop body in front of the increment.
- Blocks the original moves (error blocks behind an earlier `break`, else arms behind later
  statements) cannot be reproduced when the string pool order contradicts it; record them.
- Types of struct fields and pointers matter more than any trick: after the structs were introduced
  several "register differences" disappeared (`list_move_after`, `start_output`, `readhdr`).

Remaining differences are the old compiler's own (the optimizer moves non-register locals into scratch
registers, `and #0xFFFE` becomes `res`, jump forms). Document them in the D table in `src/TODO.md`.

## Step 8: structs, names, quirks [B]

1. Structs: sizes of arrays (address distances) and offset patterns give the structs; names from the
   debug strings, then Plexus/V7, then own. Members of different structs may share name, offset and
   type. After each struct the exact count must not drop and the corpus test must pass.
2. Names: functions first (`rename.py`), then variables (`rename.py` also updates the address tables).
   A variable must not take the name of an external symbol the compiler generates (`_stkseg`).
3. Bugs and oddities of the original: table "Bugs and quirks of the originals" in `KNOWLEDGE.md`, with
   how the rebuild handles them. Source errors of the original that are rebuilt (for example the
   precedence error `!c & 0x20`) belong there too.

## Step 9: split, link, finish [A prepares, B finishes]

1. Module order from the SCCS strings; code boundaries settled before the split.
2. Makefile (`gen_makefile.py`): one target per source, the link with the original linker of the chain,
   `make check` for the corpus test. The final test is byte identity with the original.
3. Delete scratch files, the work file and the cut block when they are no longer needed; `.gitignore`
   for build output.

## Shared tools (`host/recon/`)

One toolkit for all programs, see `host/recon/README.md`. A program keeps in its `tools/` directory only
`progcfg.py` (name, original, program range, object-name pattern), launchers with the old script names and
what is specific to it. For a new binary: copy `progcfg.py` from `src/cmd/popt/tools` (stripped, nonsegmented)
or `codgen/tools` (with symbols) and the launchers (`classify.py`, `findings_md.py`, `odis.py`, `callcmp.py`,
`funcs.py`, `patterns.py`), then use:

- `odis.py NAME` (step 5), `callcmp.py` (step 6), `classify.py` / `patterns.py` (D classes), `findings_md.py`
  (the report, step 5/8).
- Still sld specific in `src/cmd/ld/tools` (copy and adapt for the next binary, move into `host/recon/` then):
  `fncheck.py` + `strict_audit.py` (step 5), `accept.py` / `accfn.py`, `firstdiff.py`, `rename.py` (step 8),
  `check_names.py`, `sync_findings.py`, `host/ld/check.py` (step 6).

Do not write a new script for something that exists in the toolkit; extend the toolkit.

## Checklist for a new binary

- [ ] Strings: modules, order, names, dump strings
- [ ] Inventory and function table with status column
- [ ] Libc and shipped members identified (code AND data); cut modules verified; Makefile tested
- [ ] Own code cut as one verified block
- [ ] Data and bss: sizes and every variable address equal
- [ ] One C file per module, functions compared, `findings.json` notes
- [ ] Corpus test runs (block passes), then the C build passes
- [ ] Handoff note written, model change (once)
- [ ] Second pass with the rules of step 7; structs; names; quirks table
- [ ] Link byte identical, split done, scratch removed
