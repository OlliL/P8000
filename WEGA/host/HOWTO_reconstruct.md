# HOWTO: reconstructing a binary of the WEGA system

This is the procedure for every original binary (program or library). It was developed on `popt`
and `sld` (2026-10). Facts about a specific binary go to `KNOWLEDGE.md`, open items and status to
`src/TODO.md`, per-function findings to `findings.json` of that program. Keep this file as the
procedure only.

Ground rules: byte identity with the original is the goal. Bugs of the original are rebuilt and
recorded, not fixed. Originals and copyrighted text never go into git. Plexus sources are read for
ideas and names only. Repo text is English, no umlauts in added text. Makefiles must work with the
WEGA native `make`.

## Step 0: inputs

- The original binary in the WEGA 3.1 root (`/mnt/share/cvs/P8000/tmp/wega31_root`), run by the
  emulator `host/z8k-emu/z8k-emu -r <root>`. The emulator sees only files below the working directory.
- The header and layout: `host/KNOWLEDGE.md` (s.out format, segmented E611 or nonsegmented).
- Analysis tools in `host/tools/`: `sout.py` (s.out parser), `z8k.py` (decoder, use the segmented
  flag for E611 programs), `cmpins.py` (instruction comparison), `unlink.py` (cut of nonsegmented
  programs), `unlink_seg.py` (cut of segmented programs, verifies by assembling with `cas -u`).

## Step 1: function starts

1. Inventory: `inventory.py` in the tool directory of the program (recursive descent from the entry,
   proven starts, candidates after `ret` or jumps). Facts with a count go to `findings.json`.
2. Entry set = call targets inside the code range plus the proven starts. Check that every entry is an
   instruction boundary and that each function before the next entry ends with `ret`, an
   unconditional jump or a trap. Exceptions are facts to record (for example code that falls into a
   shipped member).
3. Do not split at jump targets; inner jump targets belong to their function. Mark large functions as
   a guess when they may hold jump-only entries.
4. Write the table (entry, end, size, callers, kind, last instruction, status column) to the
   program's tool directory. The status column is the progress record: `open`, `done`, `findings:<ids>`.

Lessons: decode data references with the segmented flag of the file. The non-segmented decoder
gives wrong operand kinds and wrong conclusions (see `KNOWLEDGE.md`, sld section).

## Step 2: libc and shipped members

1. Identify the library members that are linked in: match each function against the members of
   `/lib/libc.a`, `/lib/slibc.a` (segmented) and the old libc (`src/lib/oldlibc`) with `libc_funcs.py`
   (weak fingerprint, check by hand).
2. Members that match byte for byte are taken from the library with `ar x` (not cut, not copied as text).
3. Other library code is cut as modules with `unlink.py` / `unlink_seg.py` and checked by assembling
   and comparing the code bytes (relocated words masked). A module is kept only if `verify` gives 0
   differing words.
4. Cross references: every call into another module is a symbol `_name`. Every entry that is called
   from another module gets the export label `_name::` in the defining module. Names: the module start
   carries the module name, other entries are `f<addr>` until a name is known. Use one naming script
   for all modules and the own code (`link_libc_seg.py` for sld), so both sides agree.
5. Data of a module: its words are spans with labels `_dXXXX`, references `_dXXXX+off`, the words come
   from the original (`libc_data_seg.py`). Bss references are external (`_bssN+off`).
6. Verify the Makefile: it must build the cut modules with `cas -u` and take shipped members with
   `ar x`. Test it in a scratch tree with the emulator (`make CAS="z8k-emu -r <root> /bin/cas" AR=...`).

## Step 3: own code as one block

1. Cut the whole own code range as one module (`unlink_seg.py PROG START END NAME OUTDIR EXTNAMES.json`)
   with names from the same naming script as step 2. Verify: 0 differing words.
2. This block is the reference for step 4 and the link stays possible at any time.

## Step 4: reconstruct the functions in C

1. Work file: one C file per program (`sld.c` for sld), K&R style (the old `scc` rejects ANSI
   prototypes). Functions in the order of the function table.
2. For each function: write the C source, compile with the original `scc` in the emulator
   (`z8k-emu -r <root> /bin/scc -c -O file.c`), compare with the block (`cmpins.py`).
3. Byte identical: status `done`. Not identical: record the difference as a finding (below) and go on.
4. Names: a function gets its name when its behaviour is known (messages, system calls, the program's
   use). Rename it in the table and in the export labels of the other modules at the same time.
5. Low effort first. Do not spend long on one hard case; mark it and go on. Second pass later.

## Step 5: findings and facts

- Compiler differences (old compiler, D1..Dn): table "Compiler differences (old compiler)" in
  `src/TODO.md`. Per function the id goes into `findings.json` (`findings_pending`, status open).
  `FINDINGS.md` is generated by `findings_md.py`; do not edit it.
- Bugs or oddities of the original: table "Bugs and quirks of the originals" in `host/KNOWLEDGE.md`,
  with how the rebuild handles them. Implement the original behaviour.
- Facts about the file format, the program, the library: `host/KNOWLEDGE.md`.
- Status and open items: `src/TODO.md`.

## Step 6: split and link

1. When all functions are `done` or have findings and have names, split the work file into the source
   files of the original modules. The module order comes from the SCCS strings in the data (the link
   order); the code boundaries of the modules are a separate question and must be settled before the
   split (call graph and data references alone were not conclusive for sld).
2. Delete the work file once it is empty.
3. Makefile: one target per source, plus the link with the own tool of the chain. The link is the
   final test: the result must be byte identical to the original.

## Step 7: second pass

Go through the findings with the hard cases. The compiler differences stay as documented differences
if they cannot be removed. Update the status column and `findings.json`.

## Checklist for a new binary

- [ ] Inventory and function table with status column
- [ ] Libc and shipped members identified; cut modules verified; Makefile tested
- [ ] Own code cut as one verified block
- [ ] Work file, functions in table order, each compared
- [ ] Findings in `findings.json`, bugs in KNOWLEDGE, status in TODO
- [ ] Split into modules, work file deleted
- [ ] Link byte identical; second pass done
