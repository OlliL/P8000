# WEGA source tree: TODO

Living list of what in `src/` is not yet byte-identical with the original
WEGA, why, and what we use instead. Everything in `src/` should build with
`make`; a file listed here is built from a workaround until the cause is gone.
Git content is in English; the German notes of the original WEGA sources stay as they are.

Last checked: 2026-11-04 for the seven programs of the compiler chain (`make PROG CC=ccv0p`, strip, cmp); 2026-10-21 for the rest: `make check` and
`make verify-wega` in `host/` (see `host/KNOWLEDGE.md`).

## Differences and workarounds

Workaround `oldcc/...s` means: the file in `src/lib/libc/oldcc/<member>/` is the cas
assembler source of the original object; `cas -u` on it gives the
byte-identical member. Such a `.s` is only used where the C source cannot
produce the same bytes with the shipped cc.

| Source | Object | Status | Cause | Workaround |
|---|---|---|---|---|
| `lib/libc/gen/malloc.c` | malloc.o | differs | Older cc (register contents, D3) | `lib/libc/oldcc/malloc/3_3.s` |
| `lib/libc/gen/nlist.c` | nlist.o | differs | Older cc (frame size, D1) | `lib/libc/oldcc/nlist/0_1.s` |
| `lib/libc/gen/length.c` | length.o | differs |  Older cc (frame size D1, byte loads D8) | `lib/libc/oldcc/length/0_1.s` |
| `lib/libc/gen/swap.c` | swap.o | differs |  Older cc (argument spill, D4) | `lib/libc/oldcc/swap/0_1.s` |
| `lib/libc/stdio/doscan.c` | doscan.o | differs | Different revision (Nov 1983), floating point, calls `_inflt`; the source is the older version | `lib/libc/oldcc/doscan/0_1.s` |
| `lib/libc/stdio/printf.c` | printf.o | differs | Different revision (Nov 1983): printf/fprintf/sprintf via `_pfile`, `_flsbuf`, new `_doprnt` | `lib/libc/oldcc/printf/0_1.s` |
| `lib/libc/stdio/scanf.c` | scanf.o | differs | Different revision (Nov 1983), floating point | `lib/libc/oldcc/scanf/0_1.s` |
| (none in src/) | cnvt.o | no C source | C source not reconstructed | `lib/libc/oldcc/cnvt/0_1.s` |
| (none in src/) | fltioc.o | no C source |  C source not reconstructed | `lib/libc/oldcc/fltioc/0_1.s` |
| (none in src/) | symscan.o | no C source | C source not reconstructed | `lib/libc/oldcc/symscan/0_1.s` |
| `cmd/ar.c` + `lib/libc/crt/crt0.s.in` | ar | identical (built with the WEGA cc, linked with the shipped libc.a) | - | `crt0-ar.s` is made from the template (`crt0.list`) |
| `cmd/size/size.c` + `crt0-size.s` (made from `lib/libc/crt/crt0.s.in`) | size | own code identical (0 differing blocks up to the first libc member), file differs by 2 bytes of data; built with the old compiler v1 (`cc/v1` ccv1) byte-identical | size.o of the original was made by an older cc that does not pad string literals to even addresses (D12); with that object the file is identical. The program built from `size.c` gives the same output as the original on the corpus (objects, swapped objects, damaged files) | libc part linked from 13 old modules (`lib/oldlibc`) + 19 shipped members, identical |
| `cmd/strip/strip.c` + `crt0-strip.s` (made from `lib/libc/crt/crt0.s.in`) | strip | behaves like the original; built with the old compiler v1 (ccv1) byte-identical (2026-10-31: OLD_D1 and the source fix `int c` in rcopy); with the shipped cc the own code differs in the frames of main and copy (D1) and the data in the string alignment (D12) | Before v1 (shipped cc): the blocks were read as D2, one expression, `head.s_imsize = (long)segt[0].sg_code + segt[0].sg_data`. Source forms tried without success: both operands cast to long, operands swapped, cast of the sum, one cast moved. String alignment of strip.o (D12). The rest of the code is identical; run on the whole corpus (objects, swapped objects, damaged files, `-h`) the program built from `strip.c` gives the same output and files as the original | libc part linked from 16 old modules (`lib/oldlibc`) + 24 shipped members, identical |
| `cmd/mv/` | mv (= cp, ln: the same file) | **identical** with the old compiler v1 (`CC=ccv1`) | - | - |
| `cmd/rm/` | rm | **identical** with the old compiler v1 (`CC=ccv1`) | - | - |
| `cmd/rmdir.c` (System III rmdir, rmdir.c 2.1) | rmdir | rmdir and the data equal with v1; main: the old compiler does not spill the third parameter (D4, one witness) and writes `@r15` (D18) | main has a third parameter `status` (the frame word below argv, guess) | libc part open |
| `cmd/write.c` (System III write, write.c 2.1) | write | all 5 functions and the data equal with v1 | WEGA messages (with a full stop, `"%s" -- no such tty.`), `/tty???` when no tty is found, printf in ex, signum in front of me | libc part open |
| `cmd/ld/` (ld.c, copyname.c, printname.c, symbols.c, crt0 from `lib/libc/crt/mkcrt0.sh ld`) | ld | built with the old compiler v1 (ccv1, 2026-10-31): 54 of 59 functions strictly exact, the spare locals replaced by the register declarations (OLD_D1), data equal (OLD_D12), corpus 146 of 146; the state with the shipped cc (before the spares went): code size equal (0x4a00); 51 of the 59 own functions are exact under the strict check (`tools/strictcmp.py`: only jump/call targets and function addresses masked; data, bss addresses and constants compared), 8 are findings (`cmd/ld/tools/FINDINGS.md`), together 8 bytes short (the libc part starts at 0x2c44, original 0x2c4c); data: equal in size and content under the layout model of the old cc (`tools/d12model.py`: 2748 bytes, the remaining 54 differing bytes are code addresses in jump tables and libc pointers that move with the 8 bytes); the real build has 2784 bytes of data because the shipped cc pads odd strings and puts the string literals in front of each function (D12); bss equal (27082) | D12 (string alignment and placement); findings D1, D2, D3, D11, D15 (assigned by reading the strict diffs, 2026-10-30) | libc part linked from `cmd/ld` (44 old and shipped members, `ld.link`), identical in text and data (`libc_verify.py`); corpus test (`tools/corpus_ld.py`): 146 of 146 command lines identical |
| `cmd/ranlib/ranlib.c` (`readsym` is an old libc module) | ranlib | behaves like the original (same `__.SYMDEF` for libc.a, slibc.a, libcurses.a), code differs | Older cc (frame size D1, scratch registers D2, argument spill D4, string alignment D12); the old `readsym` of the original is `lib/oldlibc/readsym/0_1.s` (C form `cmd/ranlib/readsym.c`, kept as documentation, not built) | libc part linked from 24 old modules (`lib/oldlibc`) + 19 shipped members, identical |
| `cmd/cpp/`, `cmd/cparse/`, `cmd/codgen/`, `cmd/popt/` | cpp, cparse, scparse, codgen, scodgen, popt, spopt | **identical** with the old compiler v0p (`CC=ccv0p`) | - | - |
| `cmd/dc/` (System III dc, split as WEGA has it) | dc | differs: 28 places in the own code with ccv0, 6 bytes of code; the libc part fits | not worked through | - |
| `cmd/cc/cc.c` | cc, scc | **identical** (linked from `cmd/cc`: cc.o + 9 old libc modules + 33 shipped members, no libc.a) | - | - |
| `cmd/getud.c`, `cmd/putud.c` | getud, putud | own code identical, file differs | unstripped binary, temporary cc filename differs cause PID changes every run | none yet |
| `cmd/doscopy/*.c` (8 commands) | doscat ... dosformat | builds under z8k-emu, but all files differ from the originals | `dosopn.c` line 175 (`1.5*first`) needed EPU instructions; z8k-emu now runs them (2026-10-22, `host/z8k-emu/README.md`) and the commands compile. Locally the build fails only on the `/usr/include` make dependencies (outside the guest ROOT). The owner believes they were built with the current cc and libc (no old-libc version needed; belief, not measured). The differences are not analysed yet | analyse **after the host tool chain is complete** (nearest-version method, `cmpins.py`) |

## Plan: the PCC passes in the form of PCC (owner decision 2026-11-02)

Goal: `src/cmd/cparse` and `src/cmd/codgen` differ from the PCC of the Plexus tree (`cmd/cc/pcc.d`, `pcc.d/mip`, `z8000/pcc`) only where WEGA differs. Byte identity comes first: a change that moves bytes goes only together with what restores them; a rename must leave the model unchanged.
Measured 2026-11-02, lines without comments and blanks that also stand in the PCC file: cparse pftn1 410 of 757, trees1 740 of 1064; codgen reader3 338 of 1453, order3 63 of 1058, allo3 63 of 695, match3 39 of 470. Function names: cparse 131 of 180, codgen 84 of 209 occur in PCC.
1. Open functions take their PCC form when they are worked on (codgen step 5).
2. After the bytes, one pass per module without byte change: names (functions, `NODE` and its members as in `pcc.d/mip`: `p->op`, `p->rval`; `in.op`, `tn.rval` is the form of `z8000/pcc` only, cparse follows mip too), macros (`optype`, `asgop`, `logop`, `FORCC`, `MUSTDO`, `NOPREF`, `INTEMP`) instead of numbers, PCC's file names where the stamps allow (`common`, `optim`, `rdr`), comments. Check: the model and the corpus stay as they are. `scparse`, `scodgen` go with it.
   Done for codgen (2026-11-03): the macros (`optype` ... `MUSTDO`, cookies), `NODE` as `union ndu`, the names of functions that do the same; the list of what stays WEGA's is `codgen/tools/PCC.md`. cparse was in this form already, its list is `cparse/tools/PCC.md`. popt: only `getline`, `abs` renamed, list `popt/tools/PCC.md`. To do: scparse, spopt, cpp when they are worked on.
3. What is left different from PCC is WEGA's: a list per pass (`tools/FINDINGS.md` or a section of its own).
The same for `popt`, `spopt` (model `cmd/cc/c2.d`, `z8000/c2`). `cpp` follows the System III source (owner decision 2026-11-03).

## State and next steps (2026-11-04)

The old compiler v0p is rebuilt (`host/OLDCC.md`). `make PROG CC=ccv0p` and strip give the originals, byte for byte: `cpp`, `cparse`, `scparse`,
`codgen`, `scodgen`, `popt`, `spopt`. Their state per function is in `tools/FINDINGS.md`.

Next, in this order (owner 2026-11-04):

1. Connect the programs that are not compared as whole files yet: `rmdir`, `write`; each with its Makefile, its libc part and the check against the original. `dc` (`cmd/dc`, the System III source as `dc1.c` and `dc2.c`, built with ccv0) is connected: libc part complete, data and bss of equal size, code 6 bytes too long, 28 places in its own code; to be worked through. Tools: `host/tools/fitlibc.py` (the libc part from known modules), `mkspec.py` and `unlink.py` (new modules), `linkmk.py` (the Makefile). Done: `mv` (cp, ln), `rm`.
2. `ld` and `sld` with the old compilers; decide for each whether its source base is Plexus or System III.
3. The host versions (`host/cparse`, ANSI C) from the proven sources, of the shipped revision. Deferred: `/lib/gopt` (only with -Ol/-Or), `/bin/gld`, `/usr/bin/yacc` (the original is used as a tool).

Procedure and evidence rules: `host/HOWTO_reconstruct.md`. Shared toolkit: `host/recon/` (README there). Lesson of ld: the masked comparison and the pattern check accepted wrong code; strict comparison (`strictcmp.py`) is the authority.

## sld (segmented loader): preparation (started 2026-10-27)

- Structure: `src/cmd/sld/tools/` with `Makefile`, `inventory.py`, `findings_md.py`, `findings.json` (empty function list, binary facts),
  `FINDINGS.md` (generated). Our sources of sld are not in `src/cmd/sld` yet: `host/ld/sld.c` is a Linux program written from the
  binary (not compiled by the WEGA cc), so it is a guide for names and structure, not the comparison basis.
- The original `/bin/sld`: segmented (E611), code 38912 bytes in segment 0, data 7090, bss 31054 + 63488. Stripped (no symbols).
- Function starts: `src/cmd/sld/tools/inventory.py`. Recursive descent from the start (0x18) found 2 starts only: the
  control flow of the start code leaves through a return and the rest is reached by calls. The linear sweep over the code
  (12279 instructions, no undecodable words in the paths that matter) collects all `call`/`calr` targets: 63 starts are
  proven (targets of calls). 311 more candidates start right after a `ret` and are not called from anywhere; they are
  unproven and need a check by hand. `findings.json` lists them as `f_xxxx` (proven) and `c_xxxx` (candidate). The
  decoder (`host/tools/z8k.py`) works for these paths; the earlier blocker (undecodable words) was a wrong branch
  target (the `cc` operand of conditional jumps was taken as target), fixed in inventory.py.

- sld modules (from the SCCS strings in the data of `/bin/sld`): scrt0.s, copyname.c 3.1, dump.c 3.1, ext.c 3.3, ld.c 3.8,
  ldm.c 3.10, printname.c, swap.c, symbols.c, malloc.c (the data of each module lies in the order of the link). Module hints for
  the proven functions: `src/cmd/sld/tools/modules.py`; 20 of 63 proven functions refer to strings of dump.c, 43 refer to no
  message string (not decided). Plexus `ld.z8000.c` is a reference for names only (private, never copied).

- sld, libc part (`host/tools/libcver.py`): the libc of `/bin/sld` starts at 0x8680 and runs to the end of the code (0x9800).
  Shipped members found unchanged: ffltioc, getpid, umask. The other parts (0x8686 .. 0x97fc, 10 pieces) are an older libc,
  not matched yet (nearest names by fingerprint are guesses only). The own code of sld is 0x0018 .. 0x8680: 56 of the 63
  proven starts and 250 of the 311 candidates are there; 7 and 61 are libc.
  Plan: first libc (shipped members by ar, oldlibc members by CATALOG, the rest as new versions), then the own code.

- sld libc, first result (`src/cmd/sld/tools/libc.spec.draft`): against all 67 oldlibc modules (assembled with cas, archived with
  ar) only `ffltioc` matches; 8 pieces have no counterpart (best ratios 0.03 to 0.16). They need new oldlibc versions, which
  need names, data ranges and symbols. The libc work is not finished: `src/cmd/sld/Makefile` does not exist yet.
- sld libc, status (2026-10-27): 14 modules are cut from `/bin/sld` (`host/tools/link_libc_seg.py`, `unlink_seg.py`) and verified
  byte-identical (`src/lib/oldslibc/<module>/0_1.s`, README there); cross references are symbols (`_name`, `_fXXXX` for inner
  entries); data spans added (`host/tools/libc_data_seg.py`, not yet compared with the original); the bss of segment 1 (0x1bb2)
  is the external `_bss1`. Shipped members of `/lib/slibc.a`, identical: ffltioc (0x8680), getpid (0x95aa), umask (0x96f2).
  `src/cmd/sld/Makefile` builds the libc objects of sld (cut modules with `cas -u`, shipped members with `ar x`); tested with the
  emulator. Open: names of the `_fXXXX` entries, the own modules of sld and the link, the scrt0 start file, data verification.

- sld libc, second result (against `slibc.a`, the segmented library that sld links, not `libc.a`): `ffltioc`, `getpid`, `umask`
  identical. `src/cmd/sld/tools/libc_rank.txt`: p07 = `swap` (0.59), p08 = `signal` (0.49), p01 = `flsbuf` (0.34, probable), p06 =
  `malloc` (0.21, not matched). p02, p03, p04, p05, p09, p10 are below 0.2: not matched at module level, a function-level
  comparison is needed. Names are candidates only until the behaviour is checked.

- sld libc, function level (`src/cmd/sld/tools/libc_funcs.py`, result `libc_funcs.txt`, against the functions of the members of
  `slibc.a`): `_fflush` 0.77 (flsbuf.o), `_fclose` 0.86 (flsbuf.o), `__cnvt` 0.56 (cnvt.o), `decimal` 0.88 (decimal.o),
  `_malloc` 0.57 (malloc.o), `signal` 0.51 (signal.o). Not matched: 0x8686 (best 0.14), 0x8f7c, 0x903a, 0x96be, 0x96fa.
  `swap` 0.59 on module level (0x946e), not on function level: the function boundaries there need a check. The libc of sld is
  therefore not complete; the next steps are the open pieces, the check of 0x946e, then the new oldlibc versions and the Makefile.

- sld libc, `swap` at 0x946e (checked): the three functions of swap.o of slibc.a (`_swap`, `_longswap`, `_swapsegt`) lie in
  sld in this order (ratios 0.72, 0.81, 0.93, sliding window). The earlier piece 0x946e..0x95a4 contains swap and code behind it.

- sld libc, boundaries (`src/cmd/sld/tools/libc_bounds.txt`): 12 functions with whole-range ratios 0.55 to 0.88 (ungetc, filbuf,
  flsbuf, _fflush, _fclose, __cnvt, decimal, _malloc, _calloc, _cfree, _isatty, _mktemp). Open: the gap 0x92xx..0x946e (`_stty`
  and more), the gap 0x94f0..0x95b0, 0x96be..0x97fc (write, read, unlink candidates), and the tail of malloc at 0x8f7c..0x9038.

- sld libc, gaps resolved to candidates (`src/cmd/sld/tools/libc_bounds.txt`): the string functions strrchr, swab, strcat, strncmp,
  strcmp, strcpy at 0x92e6..0x946e; the system call stubs fstat, getpid (identical), getuid, getgid, ioctl, lseek, open, read,
  ssgbrk, umask (identical), unlink, write, alarm at 0x94f0..0x97fc; signal at 0x9638. Still open: 0x8f7c..0x9038 and 12 bytes at
  0x96be. The libc of sld is not finished: the cut of the modules (unlink.py spec with data sizes and symbols), the new oldlibc
  versions and src/cmd/sld/Makefile remain.

- sld libc data (`src/cmd/sld/tools/libc_data.txt`): the libc ranges reference data that lies inside the SCCS areas of sld's own
  modules (symbols.c at 0x159a..0x1a60, malloc.c at 0x1a60..0x1bb2). The own modules malloc.c and swap.c are in the same program,
  so the libc data is not a separate block. Open: which data belongs to which libc module; whether the libc malloc at 0x8cb4
  is a second copy of sld's own malloc.c. This blocks the cut of the libc modules (unlink.py needs the data per module).

- sld libc data, result of the ownership test (`libc_data.txt`): the malloc area (SCCS malloc.c, 0x1a60..0x1bb2) is used only by
  the libc code at 0x8cb4 and 0x96f0 ff. So the libc malloc is a libc module with its SCCS string; sld's own modules are copyname,
  dump, ext, ld, ldm, printname, swap. 23 data words are used only by libc code, 6 by both. Still open: the symbols area
  (0x159a..0x1a60), which holds own data and libc globals.

- sld symbols area split (`libc_data.txt`): 20 referenced words. Own blocks 0x15c8..0x1638, 0x16f2, 0x16fa, 0x194e..0x1952;
  libc words scattered between them (0x15aa, 0x15b0, 0x15ee, 0x1622, 0x171c, 0x1959, 0x1a32); mixed 0x1610, 0x170a. The libc data
  is not one block here; the cut needs a word list per module, and the mixed words are assigned by hand.

- sld data, CORRECTED: the earlier ownership result counted call targets as data (see `libc_data.txt`). Without branch and call
  operands 40 data words are referenced: 10 only by libc code (0x1959, 0x1a32, 0x1a90..0x1aa8, 0x1bae, 0x1bb0), 30 only by own
  code, none mixed. The malloc data (0x1a90..0x1aa8) stays libc. The libc stdio functions reference no data word.

- sld libc, remaining code gaps: 0x8f7c..0x9038 lies inside `_malloc` (part of the malloc module); 0x96be..0x96f0 is a function
  with `srl`/`call` and no match at 0.3 (open); 0x96fa..0x9714 is the syscall stub group. Still open for the libc: the cut of the
  modules with unlink.py (spec with word lists for the data), the new oldlibc versions where the members are not identical, and
  src/cmd/sld/Makefile.

- sld libc, cutting the modules: NOT possible with the present tool. `host/tools/unlink.py` handles nonsegmented programs only
  (its docstring: PROGRAM nonsegmented, I&D combined; it reads segs[0] and absolute addresses). `/bin/sld` is segmented (E611):
  the calls and data references carry segment numbers (call 0, 0x8000 | off; ldl with segment 1 offsets), and the modules of the
  libc part must be assembled as segmented objects. Next step: extend unlink.py to the segmented form (E611, the segment bit of
  the addresses, the segmented call and data symbols), test it on a member that is identical (ffltioc, getpid, umask) before any
  cut of an unknown module.

- sld libc modules CUT AND VERIFIED (`host/tools/unlink_seg.py`, the segmented version of unlink.py; `src/lib/oldslibc/`):
  14 modules, each assembled with cas -u and its code bytes equal to sld (0 differing words, relocated words masked). Cross
  references are symbols now (`_name`, `_fXXXX`), export labels also for calls from the own code. Data not yet compared.
- sld own code CUT AND VERIFIED as one block (`src/cmd/sld/sld_own.s`, 0x0018..0x8680, 34408 bytes, 0 differing words).
  Since 2026-10-27 the start (0x18..0x55) is its own object `scrt0.o` (`make` in src/cmd/sld, from mkcrt0.sh + scrt0.s.in);
  sld_own.s now is 0x56..0x8680 (34346 bytes, 0 differing words, `_main` exported at 0x78c). Linked (2026-10-27): `make link` (src/cmd/sld) gives sld.new with the original sld as linker: code segment
  38834 of 38912 bytes equal; the 78 other bytes are data references of the libc modules (data of sld not cut yet, the libc
  data spans are at other offsets), data and bss of sld.new are 256 bytes (original 7090 + 31054 + 63488). Symbol fixes:
  `__callex` exported by flsbuf (0x8a48; the old label _f8a48 is removed), `__inflt` (ffltioc at 0x8684) in sld_own.s, `_bss1` absolute in signal.
  Not split into modules yet. The own code has 348 direct and 58 indexed references into segment 1 (data) and 442 direct
  references into segment 2 (bss). Their owners by data range vote (`src/cmd/sld/tools/own_functions.txt` lists functions;
  the votes are not final): the vote shows the owner of the data, not of the code (global variables of one module are
  used by another), so the code boundaries of the SCCS modules (scrt0, copyname, dump, ext, ld, ldm, printname, swap,
  symbols, malloc) are still open.
  Function boundaries: 94 functions (`src/cmd/sld/tools/own_functions.txt`): entries are the call targets and the proven
  starts of the own code; no fall-through between them except 0x18 (ends with `sc`) and 0x85f8 (ends with `add`; its
  `ret` is the first instruction of the shipped ffltioc at 0x8680). Entry 0x8000 is no function start (inner jump target).
  Guess: entries reached only by jumps are not separate; the largest entries (up to 1700 bytes) may hold such entries.
  Reconstruction of the functions in `src/cmd/sld/sld.c` (work file, K&R): status column of `tools/own_functions.txt`.
  2026-10-27: 23 done (see src/cmd/sld/tools/own_functions.txt; names in sld.c),
  54 with findings (see the audit in findings.json),
  23 open: 21 large functions deferred (kind 'deferred (large function)' in findings_pending, 0x2492 and 0x2624 are done
  now), 0x0018 (start code, now scrt0.o; table row still open), 0x6a58 (not accepted). 100 entries.
  2026-10-28 (end of the session): all 105 entries are processed: 37 done, 68 with findings, 0 open. The last three large
  functions are accepted: `place_sec` 0x18f2 (1700 bytes, identical in strict mode), `ins_block` 0x5a2c (310 instructions, D11:
  two shared blocks placed the other way round) and `lay_out` 0x4a16 (1072 of 1073 instructions, D2 and D15: else arms of the
  section selection and of the delta calculation are placed after the next unconditional jump in the original, in source
  order by our compiler). 0x0018 is the start code `startcode`, built from `scrt0.s.in` (status done). Tools: `fncheck.py`
  is strict (frame offsets), `rename.py` renames table + sld.c + findings.json, `strict_audit.py` compares every accepted
  function, `check_names.py` checks the 8-character rule, `tryname.py` finds a name the compiler does not lose. Renamed
  because the compiler lost the function name: `make_blocks` -> `make_blk`, `dump_p1` -> `dmp_p1`.
  The libc call targets of sld.c have their names (tools/libc_names.txt, 34 functions: exit, atoi, read, write, strcpy ...).
  Module cut (2026-10-28): module_map.py assigns the functions to copyname, dump, ext, ld, ldm, printname, swap, symbols (code
  0x56..0x71ca, see KNOWLEDGE "C files of the program"). From 0x71ca the objects are libc: putchar, scanf, doscan are the shipped
  members; the C functions that were reconstructed for them (14 functions) are taken out of sld.c (tools/libc_archive.json).
  printf and doprnt (0x79ca..0x8682) are another revision; they stay as cas sources in src/lib/oldslibc (no C by decision).
  91 own functions remain in sld.c (34 done, 57 with findings). `make link` / `make linkslibc` (src/cmd/sld/Makefile, written by
  tools/gen_makefile.py) link; code differs only in data addresses behind 0x71ca.
  Split done (2026-10-28): src/cmd/sld/{copyname,dump,ext,ld,ldm,printname,swap,symbols}.c + ld.h replace sld.c (removed); every file
  compiles alone with the old compiler (no lost names), `make cobjs`. Tools follow the files (tools/ldsrc.py: module_ranges.txt, sources(),
  compile_all()): accept.py (new function or replacement of a function in its C file, new declarations into ld.h/ext.c), rename.py,
  strict_audit.py, classify_sld.py, check_names.py, tryname.py, sync_findings.py. Names of functions and variables can still be changed
  with rename.py at the end.
  Strings and initialized variables (2026-10-28): the original texts are in the C files (tools/fill_strings.py), SCCS strings as `static char
  sccsid[]`; the initialized variables with their values are defined in their files; the data of 7 of the 8 files equals the original
  (tools/check_data.py), ld.c to 0x0e38. Several functions came closer by following the string order (KNOWLEDGE, "initialized
  variables"). Open: getfile and the rest of the ld.c data, the owner file of each uninitialized variable (bss bands; all in ext.c now),
  the bss cut, the full link and the byte comparison; use the string order for the other findings functions (D15 block layout).
  Link (2026-10-28): `make link` in src/cmd/sld links sld.new from scrt0.o, the 8 C files, the members of /lib/slibc.a and the two old
  objects (oldslibc printf, doprnt); Makefile written by tools/gen_makefile.py; (`make linkown` and `sld_own.s` were removed 2026-10-28.) The
  uninitialized arrays have sizes now; names that lie inside a larger variable are macros in ld.h. Layout (2026-10-28): the data
  (7090 bytes) and the bss (94542 bytes: segment 1 31054, segment 2 63488) of sld.new are equal in size to /bin/sld and EVERY
  variable has the address of the original (checked for the 139 variables with an address in var_addr.txt). The data of the C files is exact
  (check_data.py: 0 differing bytes in all 8 files), the code bytes still differ because the own functions with findings differ in
  length and shift the code. What made the layout equal: (1) the commons are allocated in the order of the first mention in the
  objects in link order, so all of them are defined in copyname.c in the order of the original (segment 2 group first, each by
  address; tools/order_vars.py); (2) cmdargv has 2000 pointers, unused gaps are spare variables (bE000, bE412, bE44A, bE4B8,
  bE4C4, bE4CC, bF314), bE466/bE467 are the two bytes of `struct bytes2 bE466s` (macros); (3) getfile: the prefixes "", "/usr",
  "/z/bin" are static arrays in front of the function (shared "" and the pool order of the original); (4) printf/doprnt of
  oldslibc have their complete data and symbolic references (host/tools/oldslibc_doprnt_data.py); (5) data.o is linked behind
  filbuf.o and ctype_.o behind malloc.o (place of their data). Findings functions (2026-10-28): 58 of 90 functions equal in strict_audit.py (corpus test `make check` in src/cmd/sld: 51 of 51 command lines identical with the original sld) (state per function: findings.json `manual_classes` = hand notes, shown in the Note column of FINDINGS.md: basis read = reviewed with what was tried, basis open = not yet reviewed; the entry is dropped by sync_findings.py when the function becomes done), (load2td done; lay_out, ins_block: layout of the arms, see KNOWLEDGE),
  32 differ (rules in KNOWLEDGE, "what fixed the findings functions"; tools/odis.py shows the original). Next: byte comparison after the findings functions are exact;
  the other directories of src/lib/oldslibc are not used any more and can go.
  Next: improve the findings-class functions found by `strict_audit.py` (parse_args, map_sym, make_ccl, scan_field, ...),
  re-check the D15 tags, cut the data (7090 bytes, bss), map the libc `fXXXX` addresses to libc names, `make link` and
  compare the bytes, split sld.c into modules.
  Second pass (2026-10-27): 0x2492 accepted (D2, D7), 0x2624 accepted (D7, D16), 0x27d8 accepted (D2, D7, D15) with the open
  finding sld-27d8-layout (42 of 361 instructions differ: block placement of 0x2b02 and 0x2882; semantics checked by reading only).
  0x3516 accepted (D2, D16; same 156 instructions; the classifier reports 5 forms without pattern: indexed through a
  register pair, rr #1, sra #1; the record fields are frame operands in the original).
  0x410c accepted (D2, D5; same 120 instructions; quirk: the attributes of a name already in the list go to the step
  counter, not the entry index). Next deferred: 0x27d8 (1252 bytes, bit
  stream decoder with two jump tables at 0x0cf6 and 0x0da4): goto skeleton complete (361 instructions, equal), register
  allocation of the old compiler is now known (declaration order from r13 down, pointer = highest free pair, see OLDCC.md
  'Register allocation of the old scc'); the registers of 0x27d8 match with the declaration order w13, w12, w11, e, w10. The block order
  was searched (scratch f27d8_m.c: 42 differing of 361 instructions, not identical, not accepted. The classifier gives 59 pattern
  differences and 1 unexplained: sra rS,#4, because the if/else form folds the bit-3 ternary of the key). Open: the table path
  placement, the byte move of cursym.sseg, the switch count register. Notes in the scratch folder.
  0x68a2 accepted (D2, D16; same 118 instructions).
  0x11ea accepted as parse_args (D2, D5, D6, D7, D15, D16; same 112 instructions; layout by reading).
  0x1070 accepted (D6, D15) with one open point: 111 instructions against 112 in the original (pending finding sld-1070-count).
  Latest: 0x1560 (D2, D5, D8: table loop over 12-byte entries), 0x1364 (D7, D15: dispatcher, file reading),
  0x724e and 0x7212 (dead wrappers, D5, D16), 0x7286 (wrapper of 0x7300, D4, D16). Open non-deferred: 0x0018 (asm); 0x6a58 (draft in the scratch folder: the probe pointer must sit in rr6 as in the original; not found).
  Last rounds: 0x42f8 accepted (D6, D15, D16: switch on the type byte of the long parameter); 0x6c6a accepted (D2, D6, D15:
  state machine on cursym.stype). 0x68a2 (358 bytes, branches on the type bytes 0x21..0x24, globals 0x3c0a, 0x3c06, 0x3c0e,
  0x0474) analysed, not drafted.
  Done in the last rounds: 0x64d0 (D2), 0x0f26 read_words (D15), 0x3408 report_error (done), 0x4968 (D5, one dead increment
  open by reading), 0x2f3a dseek (D5, D9, D15). Open small ones: 0x1070 (378, parser with a jump table, not started),
  0x7286 (wrapper of 0x7300, which is deferred), 0x6a58 (hash lookup with linear probing; draft in the scratch folder: same
  82 instructions, but the register of the probe pointer (rr6 in the original) and the loop shape differ; not accepted),
  0x1560 (table loop, 12-byte entries, not drafted fully).
  Dead code (no caller, no address reference): rebuilt: 0x724e and 0x7212 (wrappers of the deferred 0x7300, D5, D16), 0x6866 pack_entry, 0x01d2 walk_entries,
  0x3b1e swab12, 0x7186 report_limit. 0x0018 is the start code: the template src/lib/libc/crt/scrt0.s.in reproduces its 62 bytes (see KNOWLEDGE, sld section); open: take the start out of sld_own.s and link scrt0.o first (needs the link test).
  0x7808 (334 bytes, open): a scanf-like reader: stream at rr10 (count at +4, pointer at +0), conversion characters c, s, [ (character
  class table 0x17BA, filled by make_ccl 0x7956), calls ungetc 0x86fa and store 0x8688. Not started; needs the stream layout first.
  0x6684 done as read_entry_hdr (D2).
  Large open functions: 0x79ca (498 bytes, EPU float formatting, calls 0x7c26) and 0x7c26 (2514 bytes). The first 12
  bytes of 0x7c26 occur in no file of the WEGA 3.1 root except sld itself, so there is no library member to take (sdoprnt.o
  starts differently). Both need the EPU instructions (see host/z8k-emu/README.md) in the drafts.
  6b5a and 6b84 need an unused 4-byte local to reproduce the 8-byte frame (role unknown, noted in the source).
  Open with notes: 00ee (string packing: a run longer than 8 gets a header byte cnt|0x80; the padding loop counts a char
  down; guess: it runs to char wrap-around when cnt is negative, to be checked), 1664 (parameter form unclear). 65c6 (byte swap, frame 10 in the original) open: the form of the local buffer is not found.
  (95 entries: 6866 added, no caller and no address reference found, dead code candidate).
  Open hard cases: 0056 (register parameters, bit-7 coding), 70ce (parameter order and frame), 64d0 (loop temporary in
  r2 against r12). Check tools: `tools/fncheck.py` (one draft against the original range), `tools/accept.py` (accept into
  sld.c and record).
  Open: the split into the module files (module order of the data SCCS strings is the link order, not checked against
  the code); the start code 0x18 is not `/lib/scrt0.o` (same start, but `scrt0.o` has the word pair `5df8 0000` after the
  first word that sld lacks, and its stack operands are 0x0004/0x0006 where sld has 0x7f04/0x7f06; see `host/KNOWLEDGE.md`). `src/cmd/sld/Makefile` builds the libc objects and `sld_own.o` (tested with the emulator).
  The link is not done yet.
- 0x79ca accepted (done, 2026-10-27): the message printer (EPU spills, argument block form in KNOWLEDGE). Dead entries
  0x7a5e and 0x7ae2 (not called, no references) stay open.

## Programs and libc versions

Result of `make verify-wega` (2026-10-21) and the libc version of the original program (measured with
`host/tools/libcver.py`, details in `lib/oldlibc/README` and `lib/oldlibc/CATALOG.md`). "libc version" = the libc
state the original was linked with; `shipped` = the members of the shipped `/lib/libc.a`.

| Program | Result | libc version | Remaining difference |
|---|---|---|---|
| ar, tar, dump, dumpdir, restor, sync | identical | shipped | - |
| getud, putud | own code identical, file differs | shipped | unstripped, the PID of the cc temporary name is in the symbol table |
| cc, scc | identical | cc | - |
| size | own code identical; file differs by 2 bytes of data when linked from the old libc modules (`cmd/size`: own objects + 13 old modules + 19 shipped members, no libc.a; identical with the size.o of the older cc) | size | libc part of an older libc and older compiler; `size.o` needs the older cc's string alignment |
| strip | own code differs in 2 blocks (D2), file differs by 6 bytes (D12); the libc part linked from `cmd/strip` (16 old modules + 24 shipped members) is identical (`cmpins` 0 blocks) | strip | the 2 blocks, the string alignment of strip.o |
| ranlib | code differs (D1, D2, D4), file differs; the libc part linked from `cmd/ranlib` (24 old modules + 19 shipped members) is identical (`cmpins` 0 blocks) | ranlib | the own code of ranlib.o (older cc), string alignment (D12) |
| copydisk, format, verify | not measured by `verify-wega` yet (paths corrected 2026-10-22) | shipped (measured on the originals) | - |
| BOOT0/pb.image, standalone/sa.* | identical | - | - |
| pburn | identical (2.1, built with `SCC`/`GLD`, see `host/KNOWLEDGE.md`); `verify-wega` builds it since 2026-10-22 (not run yet) | - | - |
| cas | rebuilt, see "In progress" | not measured (segmented, slibc.a) | older scc |

## In progress

- `cmd/cas/` (the cas assembler): all modules are rebuilt from `/bin/cas` (`cmd/cas/MODULES`), `make` links the complete program
  (`sld -i -s -e start -z -Mt4 -Md5 -Mb5`, tables in the segments 7, 8, 9; text 46848 bytes like the original). The original was built by an older
  scc, so the code differs (`host/OLDCC.md`, `cmd/cas/README`); byte identity is not expected. Proof of behaviour: `cmd/cas/test/`
  (all opcodes in the three address modes, about 346 000 operand patterns, jr/jpr/callr at 23 distances, 228 compiler outputs, 10 option sets): same
  messages and objects as the original. Not the same (undefined behaviour of the original, see KNOWLEDGE.md "Test of the complete cas"): more than one
  semantic error in an expression in one statement, `"\%4"` (one hex digit). The host fork `host/cas` (modern C) exists and is tested against the original with the same scripts (KNOWLEDGE.md "cas on the host").

- Host tool chain (compile without z8k-emu): native on the host are the `cc`/`scc` drivers, `popt`, `cas`, `ar`, `ranlib`, `strip`, `size` and
  since 2026-10-22 `ld` (`host/ld`, nonsegmented, same files as `/bin/ld` on 146 command lines; no 8 bit objects, no `-T`/`-v`) and `sld`
  (`host/ld/sld.c`, same files as `/bin/sld` on 91 command lines; not implemented: `-f`, `-A`, `-b`, `-t`, `-M` with `:t`/`:b`/`+files+`, `-M` changing
  between large files, several overflowing start segments in combined mode, see KNOWLEDGE.md). `gld` (`host/ld/gld.c`, same files and messages as `/bin/gld`; not reproduced: the original's undefined output when no object is loaded, its memory limit ("out of memory(load2td)" with sections of about 50000 bytes) and hangs with sections near 64K). Still only the
  original binaries under the emulator: `cpp`, `cparse`/`scparse`, `gopt`, `as`. `spopt` is done (`host/popt`, `make spopt`). `codgen`/`scodgen` exist as source (`src/cmd/codgen`; the same assembler text as the originals on the test corpus, 101 of 101 cases each; 206 of 206 functions paired with the original, 202 of them differ only by old-cc patterns, 4 have open differences; codgen and scodgen are linked without /lib/libc.a: their libc members are oldlibc versions (cut from the original) and identical members of libc.a (ar x), corpus 101/101 for both; see KNOWLEDGE.md "codgen / scodgen"; built by `src/cmd/Makefile`). Next: `gopt`, the front end (order in `host/HANDOFF_wega_disasm.md`). `src/cmd/sld` (a source that builds the byte-identical
  `/bin/ld`) does not exist yet; `host/ld/ld.c` is written from the binary and measurements, not transcribed function by function.

## Scripts without an object check

`cmd/chkdiff`, `cmd/uuclear` and `cmd/whodo` are text scripts; they were
compared using `cmp` and are identical.

## Layout still open

- `popt` and the Linux versions of the tools (`host/cc` with `cc.conf`, `host/ar`, `host/binutils`, `host/cas`, `host/ld`) are Linux-side tools and live in `host/`; the WEGA source of `cc` and `scc` is `cmd/cc/cc.c`.

- The single-file commands are flat in `cmd/` (V7 layout): `ar`, `dump`,
  `dumpdir`, `getud`, `putud`, `restor`, `sync`, `tar`. `size`, `strip`,
  `ranlib` and `cc` have their own subdirectory, because they are linked from
  their own libc part (`lib/oldlibc`); `cas` has several files.
- For some commands several versions exist. For those, subdirectories were
  created, one per version (even if it is a single C source). The `Makefile`
  in the subdirectory builds the version shipped with WEGA 3.1; the others
  are not built. This applies to `pburn` (2.1; others 1.4/2.3/2.6),
  `copydisk`, `format` and `verify` (1.5; 1.6 alternative).
- `BOOT0`, `doscopy`, `standalone` have their own template Makefiles and are
  built by `make` in `cmd/` (`SUBDIRS`; `doscopy` by a separate line before
  them); `cas` too (it needs `SCC`, `SLD` and `YACC`).
  `standalone/sa.diags` is separate and built as well, but only the p8000
  version shipped with WEGA 3.1 (`p8000c` is for the P8000 compact, a newer
  hardware, not relevant here); its
  Makefile needs `ASFLAGS=-u` (the original `as` needs `-u` for external
  symbols like `_printf`).
- `cmd/doscopy/mk` is a one-line helper (`make doscat ...`) that duplicates
  the `all` target; not used.
- `lib/libc` has no Makefile of its own yet.

## Original programs without source in src/

From the earlier `cmd/TODO`; the old percentages are dropped, only the list is kept. `cas`, `scc`,
`ranlib` and `strip` were taken out: their sources are now in `cmd/`.

**bin/** (91): adb, as, bc, bcmp, bd, cat, chgrp, chkin, chkout, chkwhat, chmod, chown, cmp, cp, csh, date, dc, dd, df, diff, dog, du, echo, echo2, ed, edit, egrep, env, ex, file, find, getfile, gets, gpasswd, grep, hd, kill, ld, ln, local, login, ls, mail, make, mkdir, more, mv, newgrp, nice, nl, nm, nohup, od, page, passwd, pr, printenv, ps, pstat, putfile, pwd, regcmp, remote, rm, rmail, rmdir, rsh, sh, sld, sleep, sort, stty, su, sum, tail, tee, test, time, touch, tr, uname, uucp, uulog, uuname, uusend, uux, vi, view, whereis, who, write

**etc/** (52): GETTY, INIT, UGETT1, UGETTY, adduser, chmog, chog, chroot, clri, cron, datem, dcheck, devnm, down, dqueuer, fsck, fsdb, getty, halt, icheck, init, install, kdb, killall, labelit, link, login, makelost+found, makewhatis, man.dbmgr, mfs, mkfs, mknod, mlf, mount, mvdir, ncheck, new.install, rc, rc_csh, rmuser, setlp, setmnt, shut, sysgen, ttyconfig, umfs, umount, unlink, update, upkeep, wall

**usr/** (135): bin/LOAD, bin/SEND, bin/acctcom, bin/admin, bin/apropos, bin/at, bin/awk, bin/banner, bin/basename, bin/bcheck, bin/bdiff, bin/cal, bin/calendar, bin/cb, bin/cdc, bin/checkcw, bin/checkeq, bin/code, bin/col, bin/comb, bin/comm, bin/cpio, bin/cref, bin/crypt, bin/csplit, bin/ct, bin/ctags, bin/cu, bin/cut, bin/cw, bin/cxref, bin/daytime, bin/delta, bin/deroff, bin/dfilter, bin/diff3, bin/diffmk, bin/dircmp, bin/dirname, bin/eqn, bin/error, bin/expand, bin/expr, bin/false, bin/fgrep, bin/flow, bin/get, bin/getNAME, bin/getopt, bin/grpck, bin/head, bin/help, bin/hyphen, bin/id, bin/join, bin/lex, bin/line, bin/lint, bin/logname, bin/look, bin/lorder, bin/lpr, bin/m4, bin/man, bin/mesg, bin/mkmenu, bin/mkstr, bin/mm, bin/mmchek, bin/mmt, bin/mvt, bin/neqn, bin/news, bin/nq, bin/nroff, bin/nroffv7, bin/objdu, bin/objhdr, bin/objsu, bin/pack, bin/paste, bin/pcat, bin/plz, bin/plzcg, bin/plzsys, bin/prof, bin/prs, bin/ptx, bin/pwck, bin/quot, bin/reset, bin/rmchg, bin/rmdel, bin/sact, bin/sccsdiff, bin/script, bin/sdiff, bin/sed, bin/spell, bin/spellin, bin/spellout, bin/split, bin/sprof, bin/str, bin/strings, bin/strprint, bin/tabs, bin/talk, bin/tbl, bin/timex, bin/troff, bin/troffv7, bin/true, bin/tsort, bin/tty, bin/uimage, bin/unget, bin/uniq, bin/units, bin/unpack, bin/users, bin/val, bin/vls, bin/vnews, bin/wc, bin/what, bin/whatis, bin/whoami, bin/whois, bin/xargs, bin/xq, bin/xstr, bin/yacc, bin/zmenu, bin/zmprint

## libc check (status, moved from host/KNOWLEDGE.md)


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
- Result: 170 of the 180 members identical, 7 differ, 3 have no C source (the 10 are
  kept as cas sources in `src/lib/libc/oldcc`, see "oldlibc").
- Same code, different compiler revision (cannot match with the shipped cc): `malloc`,
  `nlist`, `length`, `swap` (differences D1, D3, D4, D8). The shipped
  libc was built with more than one compiler revision.
- Different source revision (Nov 1983 float versions, files dated Nov 16-17 1983):
  `printf` (printf/fprintf/sprintf with `_pfile`, `_rc`, `_flsbuf`; calls
  `_doprnt(fmt, args, argend, flag, file)`), `scanf`, `doscan` (calls `_inflt`).
  Together with `cnvt`, `fltioc` and `symscan` (no C source yet) they form the
  floating-point stdio cluster that remains to be reconstructed as C (`doprnt` and
  `ecvt` rebuild byte-identical now, see below).
- Findings while fixing sources: getpass uses `struct termio` (`c_lflag`, ioctl 0x5401/0x5404/0x5403);
  execvp is the 4.3BSD one with `index`, a `/bin/csh` fallback (`whichshell`) and `-` as
  path separator in `execat`; malloc is a variant that extends the top free block when the
  break is contiguous (`maxbytes` 0xFFF8, `maxblocks` 64 as data); nlist/readsym use the
  s.out long-name encoding (`readby`, `(len+13)/14` entries).

## codgen host version (`host/codgen`, ANSI C, separate source)

Working copy of `src/cmd/codgen` for the host, made with `host/tools/kr_code_transform.py` (run on the copy only; the script overwrites its input).
Rule: no sharing with `src/cmd/codgen`; each file is checked with `gcc -Wall`.

- `main3.c`: done at syntax level (`gcc -Wall -std=gnu99 -fsyntax-only` clean). ANSI heads for `options`, `init`, `main`; unused variables removed; `malloc`, `strcpy`, `strcat` from the standard headers.
- `rdr3.c`: done at syntax level, also clean with `-Wold-style-definition`. The binary symbol table file is no longer copied into C structs (the host `struct sym` has another size):
  `get_sym` decodes the 22-byte records field by field; `rd16be` and `rd32be` read the big-endian words and longs (`rd32be` moved from `main3.c`);
  the number and member tables are decoded the same way; all byte offsets (`+ 0x16`, `* 22`, `+ 0x42`) became element indices (`symtab + 1`, `symtab + dest`, `symtab + 3`).
  `eread` keeps the 16-bit truncation of the original (macro `S16` in `cg.h`). A pointer that was passed through an `int` is kept as a pointer (`mapnm`).
  The debug print of a pointer uses `%p`.
- `cg.h`: the stdio macros of the original (`getc` on `_cnt`/`_ptr`) are replaced by the standard `getc`; declarations of `rd16be`, `rd32be`, `S16`.
- `regallo3.c`: done at syntax level (also clean with `-Wold-style-definition`). Findings:
  - `rsmerge(hp)` took `struct rs *hp` but dereferences it as a pointer to the list head (`*hp`); the original loads the word at `hp` (`ld r3,@r2`),
    so the parameter is `struct rs **hp` (the only caller passes `&rsstk[j].list`). The WEGA `cc` accepted the mistake silently.
  - `rweight[8]` is indexed with `reg` (a `char` 0..15 in the struct) for the classes 2 and 3 only; the index range for those classes is not checked yet.
  - The unused third register pointer in `rsbuild` and the unused `last` in `rsrestart` are removed (no effect on the behaviour; only the register use of the original is not reproduced).
  - Register numbers used as array indices are cast to `unsigned char` (the `char` fields hold 0..15).
- `comm3.c`: done at syntax level. Findings:
  - `alloc` and `allocinit` no longer use `sbrk`/`brk`: on the host `malloc` (used in `main3.c`) keeps its heap on the same break, so the
    original scheme would corrupt it. One static arena of 4 MB replaces it; `alloc` keeps the order of the reservations (symbol tables must be contiguous),
    `allocinit(base)` resets the pointer. Behaviour change: out of space is reported at 4 MB instead of at the system limit.
  - `cerror`, `uerror` and `werror` are variadic (`...`, `vfprintf`); their prototypes are in `cg.h`; the K&R `extern` declarations in all files were removed.
    `uerror` returns `int` (as the other declarations say); the original returned nothing.
  - The wasted-space message prints the node address as `%lx` of an `unsigned long` (the address itself is different on the host).
- `lcl3.c`: done at syntax level (also clean with `-Wold-style-definition`). The copy loop of `fbegin` tests the copied byte explicitly (same meaning);
  `findtemp` gets a forward declaration (it is used before its definition); two unused register variables of `fbegin` and one of `regtabinit`-era code removed.
- `reader3.c`: done at syntax level (also clean with `-Wold-style-definition`). Findings:
  - `label` was a macro for `sname` (the same word of the node). On the host `sname` is a pointer, the label an `int`; `cg.h` now puts both in one union
    (`nsn.sy` as `sname`, `nsn.lb` as `label`), so the overlap of the original is kept.
  - The cookie names are printed with `printf("%s", ...)` (no format from data); the hex width of the SPECIAL and cookie messages stays `%02x`.
  - `oreg2`: unused register variable removed; `ffld` converted to ANSI.
- `allo3.c`: done at syntax level (also clean with `-Wold-style-definition`). The `calloc` declaration (not used) is removed; `fwalk` and `tfree` are declared;
  the mask tests `cookie & 04 | 0100` get explicit parentheses (the same meaning, `&` binds tighter than `|`); unused variables removed (`prev`, `spare`, `q`);
  `getlr` is still called in `freereg` for its effect (it can stop the run on a bad cookie).
- `tr3.c`: done at syntax level (also clean with `-Wold-style-definition`). Unused variables removed (`ob`, `t`, `spare`, `spare1` to `spare4`, `sz`, `cl`).
  The assignments of `sz` (`(int)m->ssize`) and `cl` (`m->sclass`) in `member` had no further use; `makeblock` and `buildstar` converted to ANSI.
- The remaining files of the host copy (`order3`, `out3`, `zzz3`, `optim3`): done at syntax level and the K&R heads converted (all files are clean with `gcc -Wall` and `-DSEG`, with and without `-Wold-style-definition`). Next: the four small ones (`order3`, `out3`, `zzz3`, `optim3`), then the 16-bit width audit (`int` fields and comparisons, `printf` formats with negative values).
- Acceptance: the host `codgen` must produce the same text as `/lib/codgen` (emulator) on the corpus. Not yet possible: the program does not link yet.

### codgen host version: first run against the original (corpus, not finished)

Status: the host `codgen` (`host/codgen`, non-SEG) links and runs. Test: `host_corp.sh` (scratch), 100 cases of the codgen corpus,
output compared with `/lib/codgen` under the emulator: 69 identical, 25 crash (SIGSEGV), 6 stop in `cerror` (exit 64), 0 other differences.

Bugs found by the run and fixed (host copy only):
- `setbuf(dfile, malloc(0x200))`: glibc `setbuf` writes a `BUFSIZ` buffer (8192 bytes); now `setvbuf(..., _IOFBF, 0x200)`.
- `opclass[]` had 24 entries, the table uses `op = 0x1000 + k` up to k = 24; the original array has k = 24 -> `0x4000` and k = 25 -> `0xe` (read from the binary at 0x3530).
- `dope[]` and `opptr[]` had 128 entries; `setrew` runs to `op 128`; both arrays now have 129 entries (the entry is 0).

Node overlay (decided: option A, separate fields): `lval`/`left`, `sname`/`label` and `rval`/`right` are separate fields in the host copy (`cg.h`).
The corpus result did not change (70/100); the crashes are not caused by the overlay (see below).

### codgen host version: corpus status (current)

`codgen` (non-SEG): 100 of 100 corpus cases identical to `/lib/codgen` (`host_corp.sh`, scratch). Last step: in `lcl3.c` the rewrite
of `STREF` and `UAND` nodes (`mapregs`) copied only some fields of the replacement node; the original's overlaid `lval`/`rval` carry the
operands `left`/`right` along by accident. The host copy now copies the whole node (`ncopy`). That removed all 20 failures.

Corrections of earlier notes in this file:
- `INCR`/`DECR` (`78`/`79`) are binary nodes (dope flags `0x9`): the step is the right subtree, not `rval`. The earlier "INCR is the cause"
  was wrong; the cause was the incomplete copy above. The INCR rewrite in `reader3.c` is the original's and is not changed.

`scodgen` (`-DSEG`): 100 of 100 corpus cases identical to `/lib/scodgen` (`host_scorp.sh`, scratch). An earlier run showed
46 of 100 (15 crashes); that was an error of the test script, which copied the codgen corpus data (`.D`, `.S`) for the scodgen cases.
With the corrected script both host versions are complete for the corpus.

### Large corpus: source files of `src/cmd` and `src/lib` (scratch, `big_corp.sh`, `big_scorp.sh`)

Each file: `cpp` and `cparse` (or `scparse`) of the original make the intermediate files; the original and the host code generator run on them.
- `codgen`: 213 of 213 comparable files identical to `/lib/codgen`, after two host corrections found by this run:
  `zzzcode` tested `sname->sclass` before `op == NAME` (an `OREG` has no symbol); `fixname` returns for a `NAME` without symbol (a label;
  the original reads memory there; the five `doscopy` cases match the original with this rule). 3 files are not comparable: `cas/scanner.c`
  needs the yacc output `y.tab.h` (not in the tree); `doscopy/dosdir.c` and `dosfat.c` fail in the original `cparse` itself.
- `scodgen` (`-DSEG`): 209 of 213 identical; 4 text differences (`doscopy/1.3/dosdir.c`, `pburn/2.1|2.3|2.6/pb.cmdline.c`): the host prints
  a `|` around `_stkseg+~L1` operands (`oregput`, the SEG branch with `bar`), the original does not. Cause not found. The union of `sname` and
  `label` was tried (no change); the cause is not the field split.

### scodgen SEG: the `|` difference (resolved)

Cause: the bar test in `oregput` (SEG). The original's code (`scodgen` at 0x4076) tests the low words only: for `sname == symtab0` the
low word of `lval`; for `sname == fpsym1` the low word of `lval + X`, where X is the global at 0x963A. The dump of that global at the
calls in question shows `maxoff` (288 = 36 * 8), not `frame0` (36). The host used `frame0` and the full value.
Fix (host copy): `(lval & 0xFFFF) <= 0xFF` for `symtab0`, `((lval + maxoff) & 0xFFFF) <= 0xFF` for `fpsym1`.
Result: `scodgen` 100 of 100 corpus cases identical; large corpus 213 of 213 comparable files identical (3 not comparable, see above).
The method: the scratch emulator was patched to dump memory at the n-th hit of an address (`Z8K_MEM_NTH`); the host debug copy printed the
values at the same calls.
