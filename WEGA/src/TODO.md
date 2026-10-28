# WEGA source tree: TODO

Living list of what in `src/` is not yet byte-identical with the original
WEGA, why, and what we use instead. Everything in `src/` should build with
`make`; a file listed here is built from a workaround until the cause is gone.
Git content is in English; the German notes of the original WEGA sources stay as they are.

Last checked: 2026-10-21. Verification (from `WEGA/host`):
`make check` in host/ (cc, ar, binutils, cas, popt, libc, oldcc; the run of
2026-10-17, before ar, binutils and cas were added: cc 74 command lines identical;
popt 522 corpus runs identical; libc 170 identical, 7 different, 3 without C
source; oldcc all ok; the results of ar, binutils and cas are in `host/KNOWLEDGE.md`)
and the build of `src/cmd` with the original cc, scc, cas, as, ld, sld, gld, ar
and strip under z8k-emu, followed by the compare per program (see
`host/KNOWLEDGE.md`). Programs that are identical or whose own code is
identical are marked in the table; the rest differs or is not built.

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
| `cmd/size/doprnt.c` | - | not built | Structural reconstruction of the Jan 1983 `_doprnt`, untested | - |
| `cmd/ar.c` + `lib/libc/crt/crt0.s.in` | ar | identical (built with the WEGA cc, linked with the shipped libc.a) | - | `crt0-ar.s` is made from the template (`crt0.list`) |
| `cmd/size/size.c` + `crt0-size.s` (made from `lib/libc/crt/crt0.s.in`) | size | own code identical (0 differing blocks up to the first libc member), file differs by 2 bytes of data | size.o of the original was made by an older cc that does not pad string literals to even addresses (D12); with that object the file is identical. The program built from `size.c` gives the same output as the original on the corpus (objects, swapped objects, damaged files) | libc part linked from 13 old modules (`lib/oldlibc`) + 19 shipped members, identical |
| `cmd/strip/strip.c` + `crt0-strip.s` (made from `lib/libc/crt/crt0.s.in`) | strip | behaves like the original; own code differs in 2 blocks (0x2b0 and 0x2bc, `cmpins` range 0x68c), file differs by 6 bytes of data | Older cc (scratch registers, D2): one expression, `head.s_imsize = (long)segt[0].sg_code + segt[0].sg_data`. Source forms tried without success: both operands cast to long, operands swapped, cast of the sum, one cast moved. String alignment of strip.o (D12). The rest of the code is identical; run on the whole corpus (objects, swapped objects, damaged files, `-h`) the program built from `strip.c` gives the same output and files as the original | libc part linked from 16 old modules (`lib/oldlibc`) + 24 shipped members, identical |
| `cmd/ranlib/ranlib.c` (`readsym` is an old libc module) | ranlib | behaves like the original (same `__.SYMDEF` for libc.a, slibc.a, libcurses.a), code differs | Older cc (frame size D1, scratch registers D2, argument spill D4, string alignment D12); the old `readsym` of the original is `lib/oldlibc/readsym/0_1.s` (C form `cmd/ranlib/readsym.c`, kept as documentation, not built) | libc part linked from 24 old modules (`lib/oldlibc`) + 19 shipped members, identical |
| `cmd/cc/cc.c` | cc, scc | **identical** (linked from `cmd/cc`: cc.o + 9 old libc modules + 33 shipped members, no libc.a) | - | - |
| `cmd/getud.c`, `cmd/putud.c` | getud, putud | own code identical, file differs | unstripped binary, temporary cc filename differs cause PID changes every run | none yet |
| `cmd/doscopy/*.c` (8 commands) | doscat ... dosformat | builds under z8k-emu, but all files differ from the originals | `dosopn.c` line 175 (`1.5*first`) needed EPU instructions; z8k-emu now runs them (2026-10-22, `host/z8k-emu/README.md`) and the commands compile. Locally the build fails only on the `/usr/include` make dependencies (outside the guest ROOT). The owner believes they were built with the current cc and libc (no old-libc version needed; belief, not measured). The differences are not analysed yet | analyse **after the host tool chain is complete** (nearest-version method, `cmpins.py`) |

## Plan after the sld naming (2026-10-28; order of work)

State: sld is rebuilt in C (codgen, popt, sld are the three programs done in this style). sld: 58 of 90 functions exact, data/bss equal,
corpus test `make check` 51 of 51 identical, structs and all names done. Procedure for a new binary: `host/HOWTO_reconstruct.md`
(rewritten 2026-10-28 with the lessons of sld, split into Phase A for a cheap model and Phase B for an expensive one; one model change
per binary).

Order (the documentation first, because the knowledge must not get lost):

1. DONE: `host/HOWTO_reconstruct.md` rewritten (lessons, two phases, corpus test, rules of step 7).
2. PARTLY DONE (2026-10-28): shared toolkit `host/recon/` (README there): `progcfg.py`, `funcs.py`, `patterns.py`, `classify.py`, `findings_md.py`, `odis.py`, `callcmp.py`; codgen, popt and sld use it through `progcfg.py` plus launchers in their `tools/` (the duplicate `popt/tools/sout.py` is removed; `findings_md.py` reproduces the three FINDINGS.md unchanged; sld strict_audit still 58 exact). Still to move when the next binary needs them: `fncheck.py`, `strict_audit.py`, `accept.py`, `accfn.py`, `firstdiff.py`, `rename.py`, `check_names.py`, `sync_findings.py`, `host/ld/check.py` (they read sld files). Note: `classify.py` regenerated two `layout_note` texts in codgen `findings.json` and `INVENTORY.md` changed (generated files were out of date, not caused by the move). Original text of the item, kept for reference: (the sld scripts that lived in the scratchpad are saved in `src/cmd/ld/tools`: `accfn.py` (accept exact functions by name), `callcmp.py` (call-trace comparison of two runs), `uses.py` (uses of a variable per function), `scratch_build.sh`) (find duplicates first, no new scripts for old problems). Measured duplicates: `findings_md.py` in codgen, popt, ld
   (three copies, 111-120 lines), `funcs.py` / `patterns.py` / `classify.py` identical in size for codgen and popt, `sout.py` in popt/tools equals
   `host/tools/sout.py` (delete). The sld tools that are general: `fncheck.py` + `strict_audit.py` (compile, normalise, compare), `accept.py`,
   `rename.py`, `firstdiff.py`, `odis.py`, `check_names.py`, `tryname.py`, `sync_findings.py`, `classify_sld.py` (= the pattern check of
   `patterns.py`), the call-trace comparison (scratchpad scripts `calls.py`, `cmpcalls.py`: move them), the corpus runner
   (`host/ld/check.py` with `SLD_NEW`). Program specific: `ldsrc.py`, `order_vars.py`, `var_addr.py`, `gen_makefile.py`, `split_sld.py`,
   `libc_*`. Steps: (a) move the identical files, (b) one config per program (binary, segmented flag, module ranges, table, flags),
   (c) make strict_audit/accept/rename read the config, (d) run codgen and popt tools through it once to prove it.
3. sld, rest of e): table "Bugs and quirks of the originals" (precedence error `!c & 0x20` in warn_sym_redef, missing return value of
   put_pad, the unnamed spare variables), function head comments, label names of the goto code (`l08a2`, `c28a0` ...), remove `sld_own.s` and
   `make linkown`, `.gitignore`, the 32 findings (see `src/cmd/ld/tools/FINDINGS.md`: `manual_classes` says what was tried; layout cases
   lay_out, ins_block, reg_sect_addr, read_words are probably the old compiler's optimizer), spare variables (spare0 ... unused*).
4. Next binary: `/lib/cpp` (16522 bytes, 40248 total, nonsegmented) before `/lib/cparse` (67520 bytes, 96284 total). Phase A with the cheap model
   following the HOWTO, handoff note, then Phase B once. Candidate after that: `scparse`, `c2`/`as`.

## sld (segmented loader): preparation (started 2026-10-27)

- Structure: `src/cmd/ld/tools/` with `Makefile`, `inventory.py`, `findings_md.py`, `findings.json` (empty function list, binary facts),
  `FINDINGS.md` (generated). Our sources of sld are not in `src/cmd/ld` yet: `host/ld/sld.c` is a Linux program written from the
  binary (not compiled by the WEGA cc), so it is a guide for names and structure, not the comparison basis.
- The original `/bin/sld`: segmented (E611), code 38912 bytes in segment 0, data 7090, bss 31054 + 63488. Stripped (no symbols).
- Function starts: `src/cmd/ld/tools/inventory.py`. Recursive descent from the start (0x18) found 2 starts only: the
  control flow of the start code leaves through a return and the rest is reached by calls. The linear sweep over the code
  (12279 instructions, no undecodable words in the paths that matter) collects all `call`/`calr` targets: 63 starts are
  proven (targets of calls). 311 more candidates start right after a `ret` and are not called from anywhere; they are
  unproven and need a check by hand. `findings.json` lists them as `f_xxxx` (proven) and `c_xxxx` (candidate). The
  decoder (`host/tools/z8k.py`) works for these paths; the earlier blocker (undecodable words) was a wrong branch
  target (the `cc` operand of conditional jumps was taken as target), fixed in inventory.py.

- sld modules (from the SCCS strings in the data of `/bin/sld`): scrt0.s, copyname.c 3.1, dump.c 3.1, ext.c 3.3, ld.c 3.8,
  ldm.c 3.10, printname.c, swap.c, symbols.c, malloc.c (the data of each module lies in the order of the link). Module hints for
  the proven functions: `src/cmd/ld/tools/modules.py`; 20 of 63 proven functions refer to strings of dump.c, 43 refer to no
  message string (not decided). Plexus `ld.z8000.c` is a reference for names only (private, never copied).

- sld, libc part (`host/tools/libcver.py`): the libc of `/bin/sld` starts at 0x8680 and runs to the end of the code (0x9800).
  Shipped members found unchanged: ffltioc, getpid, umask. The other parts (0x8686 .. 0x97fc, 10 pieces) are an older libc,
  not matched yet (nearest names by fingerprint are guesses only). The own code of sld is 0x0018 .. 0x8680: 56 of the 63
  proven starts and 250 of the 311 candidates are there; 7 and 61 are libc.
  Plan: first libc (shipped members by ar, oldlibc members by CATALOG, the rest as new versions), then the own code.

- sld libc, first result (`src/cmd/ld/tools/libc.spec.draft`): against all 67 oldlibc modules (assembled with cas, archived with
  ar) only `ffltioc` matches; 8 pieces have no counterpart (best ratios 0.03 to 0.16). They need new oldlibc versions, which
  need names, data ranges and symbols. The libc work is not finished: `src/cmd/ld/Makefile` does not exist yet.
- sld libc, status (2026-10-27): 14 modules are cut from `/bin/sld` (`host/tools/link_libc_seg.py`, `unlink_seg.py`) and verified
  byte-identical (`src/lib/oldslibc/<module>/0_1.s`, README there); cross references are symbols (`_name`, `_fXXXX` for inner
  entries); data spans added (`host/tools/libc_data_seg.py`, not yet compared with the original); the bss of segment 1 (0x1bb2)
  is the external `_bss1`. Shipped members of `/lib/slibc.a`, identical: ffltioc (0x8680), getpid (0x95aa), umask (0x96f2).
  `src/cmd/ld/Makefile` builds the libc objects of sld (cut modules with `cas -u`, shipped members with `ar x`); tested with the
  emulator. Open: names of the `_fXXXX` entries, the own modules of sld and the link, the scrt0 start file, data verification.

- sld libc, second result (against `slibc.a`, the segmented library that sld links, not `libc.a`): `ffltioc`, `getpid`, `umask`
  identical. `src/cmd/ld/tools/libc_rank.txt`: p07 = `swap` (0.59), p08 = `signal` (0.49), p01 = `flsbuf` (0.34, probable), p06 =
  `malloc` (0.21, not matched). p02, p03, p04, p05, p09, p10 are below 0.2: not matched at module level, a function-level
  comparison is needed. Names are candidates only until the behaviour is checked.

- sld libc, function level (`src/cmd/ld/tools/libc_funcs.py`, result `libc_funcs.txt`, against the functions of the members of
  `slibc.a`): `_fflush` 0.77 (flsbuf.o), `_fclose` 0.86 (flsbuf.o), `__cnvt` 0.56 (cnvt.o), `decimal` 0.88 (decimal.o),
  `_malloc` 0.57 (malloc.o), `signal` 0.51 (signal.o). Not matched: 0x8686 (best 0.14), 0x8f7c, 0x903a, 0x96be, 0x96fa.
  `swap` 0.59 on module level (0x946e), not on function level: the function boundaries there need a check. The libc of sld is
  therefore not complete; the next steps are the open pieces, the check of 0x946e, then the new oldlibc versions and the Makefile.

- sld libc, `swap` at 0x946e (checked): the three functions of swap.o of slibc.a (`_swap`, `_longswap`, `_swapsegt`) lie in
  sld in this order (ratios 0.72, 0.81, 0.93, sliding window). The earlier piece 0x946e..0x95a4 contains swap and code behind it.

- sld libc, boundaries (`src/cmd/ld/tools/libc_bounds.txt`): 12 functions with whole-range ratios 0.55 to 0.88 (ungetc, filbuf,
  flsbuf, _fflush, _fclose, __cnvt, decimal, _malloc, _calloc, _cfree, _isatty, _mktemp). Open: the gap 0x92xx..0x946e (`_stty`
  and more), the gap 0x94f0..0x95b0, 0x96be..0x97fc (write, read, unlink candidates), and the tail of malloc at 0x8f7c..0x9038.

- sld libc, gaps resolved to candidates (`src/cmd/ld/tools/libc_bounds.txt`): the string functions strrchr, swab, strcat, strncmp,
  strcmp, strcpy at 0x92e6..0x946e; the system call stubs fstat, getpid (identical), getuid, getgid, ioctl, lseek, open, read,
  ssgbrk, umask (identical), unlink, write, alarm at 0x94f0..0x97fc; signal at 0x9638. Still open: 0x8f7c..0x9038 and 12 bytes at
  0x96be. The libc of sld is not finished: the cut of the modules (unlink.py spec with data sizes and symbols), the new oldlibc
  versions and src/cmd/ld/Makefile remain.

- sld libc data (`src/cmd/ld/tools/libc_data.txt`): the libc ranges reference data that lies inside the SCCS areas of sld's own
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
  src/cmd/ld/Makefile.

- sld libc, cutting the modules: NOT possible with the present tool. `host/tools/unlink.py` handles nonsegmented programs only
  (its docstring: PROGRAM nonsegmented, I&D combined; it reads segs[0] and absolute addresses). `/bin/sld` is segmented (E611):
  the calls and data references carry segment numbers (call 0, 0x8000 | off; ldl with segment 1 offsets), and the modules of the
  libc part must be assembled as segmented objects. Next step: extend unlink.py to the segmented form (E611, the segment bit of
  the addresses, the segmented call and data symbols), test it on a member that is identical (ffltioc, getpid, umask) before any
  cut of an unknown module.

- sld libc modules CUT AND VERIFIED (`host/tools/unlink_seg.py`, the segmented version of unlink.py; `src/lib/oldslibc/`):
  14 modules, each assembled with cas -u and its code bytes equal to sld (0 differing words, relocated words masked). Cross
  references are symbols now (`_name`, `_fXXXX`), export labels also for calls from the own code. Data not yet compared.
- sld own code CUT AND VERIFIED as one block (`src/cmd/ld/sld_own.s`, 0x0018..0x8680, 34408 bytes, 0 differing words).
  Since 2026-10-27 the start (0x18..0x55) is its own object `scrt0.o` (`make` in src/cmd/ld, from mkcrt0.sh + scrt0.s.in);
  sld_own.s now is 0x56..0x8680 (34346 bytes, 0 differing words, `_main` exported at 0x78c). Linked (2026-10-27): `make link` (src/cmd/ld) gives sld.new with the original sld as linker: code segment
  38834 of 38912 bytes equal; the 78 other bytes are data references of the libc modules (data of sld not cut yet, the libc
  data spans are at other offsets), data and bss of sld.new are 256 bytes (original 7090 + 31054 + 63488). Symbol fixes:
  `__callex` exported by flsbuf (0x8a48; the old label _f8a48 is removed), `__inflt` (ffltioc at 0x8684) in sld_own.s, `_bss1` absolute in signal.
  Not split into modules yet. The own code has 348 direct and 58 indexed references into segment 1 (data) and 442 direct
  references into segment 2 (bss). Their owners by data range vote (`src/cmd/ld/tools/own_functions.txt` lists functions;
  the votes are not final): the vote shows the owner of the data, not of the code (global variables of one module are
  used by another), so the code boundaries of the SCCS modules (scrt0, copyname, dump, ext, ld, ldm, printname, swap,
  symbols, malloc) are still open.
  Function boundaries: 94 functions (`src/cmd/ld/tools/own_functions.txt`): entries are the call targets and the proven
  starts of the own code; no fall-through between them except 0x18 (ends with `sc`) and 0x85f8 (ends with `add`; its
  `ret` is the first instruction of the shipped ffltioc at 0x8680). Entry 0x8000 is no function start (inner jump target).
  Guess: entries reached only by jumps are not separate; the largest entries (up to 1700 bytes) may hold such entries.
  Reconstruction of the functions in `src/cmd/ld/sld.c` (work file, K&R): status column of `tools/own_functions.txt`.
  2026-10-27: 23 done (see src/cmd/ld/tools/own_functions.txt; names in sld.c),
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
  91 own functions remain in sld.c (34 done, 57 with findings). `make link` / `make linkslibc` (src/cmd/ld/Makefile, written by
  tools/gen_makefile.py) link; code differs only in data addresses behind 0x71ca.
  Split done (2026-10-28): src/cmd/ld/{copyname,dump,ext,ld,ldm,printname,swap,symbols}.c + ld.h replace sld.c (removed); every file
  compiles alone with the old compiler (no lost names), `make cobjs`. Tools follow the files (tools/ldsrc.py: module_ranges.txt, sources(),
  compile_all()): accept.py (new function or replacement of a function in its C file, new declarations into ld.h/ext.c), rename.py,
  strict_audit.py, classify_sld.py, check_names.py, tryname.py, sync_findings.py. Names of functions and variables can still be changed
  with rename.py at the end.
  Strings and initialized variables (2026-10-28): the original texts are in the C files (tools/fill_strings.py), SCCS strings as `static char
  sccsid[]`; the initialized variables with their values are defined in their files; the data of 7 of the 8 files equals the original
  (tools/check_data.py), ld.c to 0x0e38. Several functions came closer by following the string order (KNOWLEDGE, "initialized
  variables"). Open: getfile and the rest of the ld.c data, the owner file of each uninitialized variable (bss bands; all in ext.c now),
  the bss cut, the full link and the byte comparison; use the string order for the other findings functions (D15 block layout).
  Link (2026-10-28): `make link` in src/cmd/ld links sld.new from scrt0.o, the 8 C files, the members of /lib/slibc.a and the two old
  objects (oldslibc printf, doprnt); Makefile written by tools/gen_makefile.py; `make linkown` is the old link with sld_own.s. The
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
  filbuf.o and ctype_.o behind malloc.o (place of their data). Findings functions (2026-10-28): 58 of 90 functions equal in strict_audit.py (corpus test `make check` in src/cmd/ld: 51 of 51 command lines identical with the original sld) (state per function: findings.json `manual_classes` = hand notes, shown in the Note column of FINDINGS.md: basis read = reviewed with what was tried, basis open = not yet reviewed; the entry is dropped by sync_findings.py when the function becomes done), (load2td done; lay_out, ins_block: layout of the arms, see KNOWLEDGE),
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
  allocation of the old compiler is now known (declaration order from r13 down, pointer = highest free pair, see KNOWLEDGE
  'Register allocation'); the registers of 0x27d8 match with the declaration order w13, w12, w11, e, w10. The block order
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
  first word that sld lacks, and its stack operands are 0x0004/0x0006 where sld has 0x7f04/0x7f06; see `host/KNOWLEDGE.md`). `src/cmd/ld/Makefile` builds the libc objects and `sld_own.o` (tested with the emulator).
  The link is not done yet.

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

## Compiler differences (old compiler)

The numbers D1 ... D16 are the sections of "Compiler revisions" in `host/KNOWLEDGE.md`. `x` = the difference
was seen in this object; `?` = the construct is in the source but the difference was not separated from the
others yet; empty = not seen (no proof that it is absent). All cas modules with code are listed (table1,
table2, pstab and the three table segments are data only). The old libc members that are only inside the binaries (size, strip, ranlib, cas) are listed with
the marks of D14 only: that is all that was checked for them.

| Binary | Object | D1 frame | D2 scratch regs | D3 reloads | D4 arg spill | D5 const index | D6 switch | D7 loops | D8 bytes | D9 >>8 | D10 long shift | D11 tail merge | D12 data order | D13 pointer const | D14 varargs, post-incr | D15 if/else layout | D16 frame operand | D17 call form |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cas | ifile.c | x | x | x |  |  | x |  | x | x | x |  | x |  |  |  |  |  |
| cas | instr.c | x | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |  |
| cas | scanner.c | x | x | x |  |  | ? | ? |  |  |  | x | x |  |  |  |  |  |
| cas | main.c |  | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |  |
| cas | nodes.c | x | x |  |  | x | x | ? |  |  |  |  | x |  |  |  |  |  |
| cas | parser.y (parser.c) | x | x | x |  | x | x | ? |  |  | x |  | x |  |  |  |  |  |
| cas | expr.c | x | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |  |
| cas | p2loop.c |  | x | x |  |  | x |  | x |  |  |  |  |  |  |  |  |  |
| cas | ofile.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |  |
| cas | gen.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |  |
| cas | assemble.c |  | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |  |
| cas | getops.c |  | x | x |  |  | x |  |  |  |  | x | x |  |  |  |  |  |
| cas | error.c |  |  | x |  |  | x |  |  |  |  |  | x |  |  |  |  |  |
| cas | listing.c |  |  | x |  |  |  | x | x |  |  |  | x |  |  |  |  |  |
| cas | optim.c |  |  | x | x |  | x | x |  |  |  | x | x |  |  |  |  |  |
| cas | jpr2.c |  |  | x |  |  |  |  |  |  |  |  | x |  |  |  |  |  |
| cas | jpr.c |  |  | x | x |  | x | x | x |  |  |  | x |  |  |  |  |  |
| cas | locntr.c | x | x | x | x |  | x |  |  |  |  |  | x |  |  |  |  |  |
| cas | ssymtab.c |  |  |  |  |  | x |  |  |  |  | x | x | x |  |  |  |  |
| cas | symbols.c | x | x |  |  |  | x |  | x |  | x | x | x |  |  |  |  |  |
| cas | pst.c | x | x |  |  |  |  | ? | x |  |  |  |  |  |  |  |  |  |
| cas | subr.c |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |
| ranlib | ranlib.c, readsym.c | x | x |  | x |  |  |  |  |  |  |  | x |  |  |  |  |  |
| strip | strip.c |  | x |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |
| size | size.c |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |  |
| libc.a | malloc.o (gen/malloc.c) |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | nlist.o (gen/nlist.c) | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | length.o (gen/length.c) | x |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |  |
| libc.a | swap.o (gen/swap.c) |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| size | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |
| size | libc: rdwr.o (fread, fwrite) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |
| size | libc: doprnt.o (_doprnt) |  |  |  |  |  |  |  |  |  |  |  |  |  | ? |  |  |  |
| strip | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |
| cas | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |
| codgen, scodgen | main3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | reader3.c |  |  |  | x |  | x | x | ? |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | rdr3.c |  |  | x | x |  | x |  | ? |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | efcode3.c | x |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | local3.c |  | x |  | x |  | ? |  |  |  | x |  |  |  |  |  |  |  |
| codgen, scodgen | gen3.c |  |  |  | x |  |  |  |  |  |  | ? |  |  |  |  |  |  |
| codgen, scodgen | order3.c |  |  |  | x |  | x |  |  |  |  | x |  |  |  |  | x |  |
| codgen, scodgen | match3.c |  |  |  | x |  | x | ? |  |  |  |  |  |  |  |  | x |  |
| codgen, scodgen | allo3.c |  | x | x | x |  | x | ? |  |  | x |  |  |  |  |  |  |  |
| codgen, scodgen | regallo3.c |  |  |  | x |  | x |  |  |  | ? |  |  |  |  |  |  |  |
| codgen, scodgen | regfile3.c |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tmp3.c |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | zzz3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  | x |  |  |
| codgen, scodgen | comm3.c |  |  | x | x |  | x |  | x |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | out3.c |  | x |  |  |  | x | x | x |  | x | x |  |  |  |  |  |  |
| codgen, scodgen | regs3.c |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | lcl3.c |  | x |  | x |  | x |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tr3.c |  |  |  | x |  |  | x | ? |  |  |  |  |  |  | x |  |  |
| codgen, scodgen | optim3.c |  |  |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |
| popt | z21.c (main..backjmp) |  |  |  | x |  | x |  | x |  |  |  |  |  |  |  |  | x |
| popt | z22.c (rmove..natural) |  | x |  | x | x | x |  |  |  |  |  |  |  |  |  | x | x |
| popt | z23.c (procopt..markregs) |  | x |  | x | x |  |  |  |  |  |  |  |  |  |  |  | x |

The codgen / scodgen rows are the per-object marks of the D classes of the function findings (`src/cmd/codgen/tools/findings.json`, written by `classify.py`), completed by reading the disassembly where noted. `x` = seen in a function of the object; `?` = construct present, not separated. D12 (data order), D13 and D14 are not checked for codgen. D15 (block layout of if/else) is marked for `zzz3.c` and `tr3.c` only (the two functions `zzzcode` and `clocal`); the other objects are not checked for it. Open for codgen: `clocal` (D15), `zzzcode` (D15, partly), `sucomp` (`add` with a frame operand, D16), `tshape` (value form of the SZERO return; `and` with a frame operand, D16; the case order was checked: the current order is the best of seven; two `and rS,F` forms, see KNOWLEDGE.md); and 2 functions with a moved block that the source does not change (`expand` D7, `freereg` D7 goto placement; findings `expand-layout-01`, `freereg-layout-01`); `conval` was fixed by the case order of the comparisons (`optim3.c`, finding `conval-layout-01`); the 17 other length checks were explained by D patterns, see KNOWLEDGE.md.

## In progress

- `cmd/cas/` (the cas assembler): all modules are rebuilt from `/bin/cas` (`cmd/cas/MODULES`), `make` links the complete program
  (`sld -i -s -e start -z -Mt4 -Md5 -Mb5`, tables in the segments 7, 8, 9; text 46848 bytes like the original). The original was built by an older
  scc, so the code differs (see "Compiler differences" above and `cmd/cas/README`); byte identity is not expected. Proof of behaviour: `cmd/cas/test/`
  (all opcodes in the three address modes, about 346 000 operand patterns, jr/jpr/callr at 23 distances, 228 compiler outputs, 10 option sets): same
  messages and objects as the original. Not the same (undefined behaviour of the original, see KNOWLEDGE.md "Test of the complete cas"): more than one
  semantic error in an expression in one statement, `"\%4"` (one hex digit). The host fork `host/cas` (modern C) exists and is tested against the original with the same scripts (KNOWLEDGE.md "cas on the host").

- Host tool chain (compile without z8k-emu): native on the host are the `cc`/`scc` drivers, `popt`, `cas`, `ar`, `ranlib`, `strip`, `size` and
  since 2026-10-22 `ld` (`host/ld`, nonsegmented, same files as `/bin/ld` on 146 command lines; no 8 bit objects, no `-T`/`-v`) and `sld`
  (`host/ld/sld.c`, same files as `/bin/sld` on 91 command lines; not implemented: `-f`, `-A`, `-b`, `-t`, `-M` with `:t`/`:b`/`+files+`, `-M` changing
  between large files, several overflowing start segments in combined mode, see KNOWLEDGE.md). `gld` (`host/ld/gld.c`, same files and messages as `/bin/gld`; not reproduced: the original's undefined output when no object is loaded, its memory limit ("out of memory(load2td)" with sections of about 50000 bytes) and hangs with sections near 64K). Still only the
  original binaries under the emulator: `cpp`, `cparse`/`scparse`, `gopt`, `as`. `spopt` is done (`host/popt`, `make spopt`). `codgen`/`scodgen` exist as source (`src/cmd/codgen`; the same assembler text as the originals on the test corpus, 101 of 101 cases each; 206 of 206 functions paired with the original, 202 of them differ only by old-cc patterns, 4 have open differences; codgen and scodgen are linked without /lib/libc.a: their libc members are oldlibc versions (cut from the original) and identical members of libc.a (ar x), corpus 101/101 for both; see KNOWLEDGE.md "codgen / scodgen"; built by `src/cmd/Makefile`). Next: `gopt`, the front end (order in `host/HANDOFF_wega_disasm.md`). `src/cmd/ld` (a source that builds the byte-identical
  `/bin/ld`) does not exist yet; `host/ld/ld.c` is written from the binary and measurements, not transcribed function by function.

## Scripts without an object check

`cmd/chkdiff`, `cmd/uuclear` and `cmd/whodo` are text scripts; they were
compared using `cmp` and are identical.

**Stop line (2026-10-26):** the source search for codgen/scodgen ends; the open items above go to the old-compiler track (no oracle for that compiler; see the bullet "Stop line" in the section "codgen / scodgen: status").

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

## codgen / scodgen: status (moved from host/KNOWLEDGE.md)

The facts of the code generator (module versions, pipeline, function map, register machinery) stay in `host/KNOWLEDGE.md`; the status is here.


Sources: `src/cmd/codgen` (K&R C for the WEGA cc, the same files build `codgen` and `scodgen` with `-DSEG`; the tables
`table3_codgen.c` / `table3_scodgen.c` and `ops_*.c` / `hop_*.c` differ). They are built by `src/cmd/Makefile` (subdirectory `codgen`).

- **Assembler text.** `codgen` and `scodgen` give the same assembler text and exit status as `/lib/codgen` and `/lib/scodgen` on the
  test corpus: 101 of 101 cases each (`corp4test` with 100 cases plus `corpsr`, and the scodgen counterparts), checked on the final sources.
  The corpus is small and does not contain structure returns (see the `fbegin` note below).
- **scodgen.** `locate.py --seg` finds all objects of `scodgen` (18 objects, 211 functions, no problems). Its functions are the
  209 names of `codgen` plus `newlreg` (`lcl3.c`) and `mkoreg` (`order3.c`), which exist only under `SEG`. The corpus check
  `tools/check.sh` with `PROG=scodgen` gives 100 of 100 and 1 of 1.
- **Functions.** The program part of `/lib/codgen` (0x0 .. 0xbbc4) and of our build (the codgen objects, up to 0xbc2a) contain 206
  functions (`tools/findings.json`). Original boundaries come from the call targets and prologues (the original is stripped).
  All 206 are paired with a function of the original (`tools/confirm.py`: the same instructions, the same strings, or a review).
  The entries `start` (the crt0 entry, not codgen code), `rcookie` (a duplicate of `rallkind`, removed) and the `printf` wrapper
  `cgwipe`/`cgprintf` (removed, see below) are no longer in the list.
- **Pairings** (`tools/findings.json`, `pairing_evidence`): all 206 confirmed. The open review entries of `clocal`, `sucomp`, `canon2`, `rsinsert`, `rsmerge`, `psuffix`, `getlab`, `rdsym`, `markcall`, `acon`, `tcmd`, `sortsyms`, `lmuldiv`, `genscall`, `argscan` are closed with the callee and body evidence (the local `L0t_` call targets of the original resolve to paired functions). `rdsym` continues after an internal `ret` (`jr gt` into `0x1aae`), so its original end is set by `orig_end` (0x1b64); `classify.py` honours `orig_end`. Still open in the review: `parminit` and `regjam` (classes, not pairings).
- **Stop line for the source search (decided 2026-10-26).** The search for source forms that reproduce the original's code in `codgen` / `scodgen` ends here. Rule used: a round counts only if it gives a new explanation or a measured gain with the corpus green. Two rounds in a row without a new explanation stop the search. The rounds since the last rule: `tshape` (case order, no gain), `sucomp` (storage classes, a measured gain, reason unknown), `clocal` and `zzzcode` (storage classes, no gain), and the last source round on the shift forms (`clocal`: the unsigned casts removed, 4 -> 2 unexplained forms, corpus green; `zzzcode`: no change). What stays open: the layout classes D15 (`clocal`, `zzzcode`, `freereg`, `expand`), the operand forms D16 (`sucomp`, `tshape`), the SZERO value form (`tshape`), the `sra`/`srl rS,#n` forms (not in the pattern set), and the `zzzcode` shift `srl rS,#2` (source of it not found). These go to the compiler track (reconstruction of the old C compiler); there is no oracle for that compiler (decided by the owner).
- **Differences of the 206 paired functions** (`tools/classify.py`, pattern check of the old-cc differences D1-D16):
  21 have no differing instruction after normalisation (addresses, frame offsets and scratch registers masked); 200 are
  "D only (pattern-verified)": every differing instruction matches an old-cc pattern (D1 to D16) and no block is moved by more
  than 5 % of the original (`patterns.structure_flags`). 6 have other differences: 4 known (`clocal`, `sucomp`, `tshape`, `zzzcode`,
  see "Open differences" above; `clocal` and `zzzcode` carry the layout class D15 as a manual annotation, `layout_classes` in findings.json) and
  2 with a moved block that the source does not change: `expand` (D7 loop layout) and `freereg` (D7, the goto target `found` of the
  first loop is placed after its body; findings `expand-layout-01`, `freereg-layout-01`). `conval` was fixed by the case order (below).

- **Tools.** `src/cmd/codgen/tools` (see its `README.md`): `locate.py` (our functions by name), `confirm.py` (pairing check),
  `patterns.py` (pattern check of the D classes), `classify.py` (writes the class into `findings.json`), `inventory.py`
  (`INVENTORY.md`), `blocks.py` (differing blocks of one function), `check.sh` and `corpus.sh` (corpus comparison). The single
  source of truth for the per-function state is `src/cmd/codgen/tools/findings.json`; the pipeline is `make -C tools pipeline`.
  Host tools: `host/tools/cgread.py` (reads `.I` files: 100 of 100 corpus files without remainder), `cgsym.py` (symbol files `.S`:
  100 of 100), `cgcmp.py` (instruction comparison of functions), `regdecl.py` (toggles `register` on the declarations of a function),
  `codgentab.py` (writes the operator and template tables as C).

### Source changes that made functions equal to the original

These changes are in the sources; the git history has the exact diffs. Each keeps the corpus result (101 of 101).

- `allo3.c`: duplicate static `rcookie` removed (it had the logic of `rallkind`); `allocreg` calls `rallkind`; `tcopy` uses `ncopy(q, p)`.
- `efcode3.c`: `lineout` takes `int line` (the original's `long` was wrong).
- `main3.c`: `options` declares `register int i, err`.
- Unsigned shifts (the original shifts the field values with `srl`): `DECREF`/`INCREF` operands unsigned in `reader3.c`, `tr3.c`,
  `zzz3.c`, `lcl3.c` (`fixfld`: `INCREF(DECREF((unsigned)m->type))`); `ttype` in `match3.c`; `shincr` in `local3.c`; `buildstar`
  in `tr3.c` (per site); `rsdump`, `tmpcleanup`, `fixtemp`: `struct rs` and `struct tmpent` flags are `int`. `struct sym` `sflags`
  stays `int`: making it unsigned broke six other functions. `fbegin` (`lcl3.c`) keeps the signed `DECREF`.
- `fbegin`: `structsz` is the structure size in words (`s->ssize / 16`), not `/ 2`.
- `tshape` (`match3.c`): the `case FLD:` block precedes `case STAR:` (the original's body order; the call order now matches).
- `sucomp` (`order3.c`): the locals and `l`, `r` are register variables (`register int o, arity, nr, ...`). This is an approximation,
  see "Open differences" above.
- `argscan` (`order3.c`): `switch (argsize(q))` written as `if` statements with the result in `int sz` (the original is a compare
  chain, D6). `argsort` uses pointers `ai`, `aj` over `struct arg` (`argl` as a named type).
- `rnnullall` (`regfile3.c`): `register int i`.
- `delstrip` (`reader3.c`): `*p = *p->left` (the original copies the node inline).
- `conput` (`out3.c`, REG case): the `r` prefix of a register in rmode is `printf("r")`, not `putchar('r')`. Checked with
  the printf call sequence of `scodgen` (`execvp`): with this change the 512 calls equal the original's; the original
  calls `printf` from a single site (`if (rmode) printf("r")`, `test` of a global, then the register name).
- `expand` (`out3.c`): the `%ld` helper `prlong` (the original calls a one-line function for `M` and `N`).
- `gswitch` (`lcl3.c`): the `newreg` register blocks written out inline in the non-SEG path (the original calls `getreg` directly).
- `mycanon` (`gen3.c`): the `switch` over the ten relation operators (the jump table of the original).
- `stmt` (`reader3.c`), `markcall`, `mapregs` (first switch): `switch` statements written as `if` statements, so the shipped cc makes
  the compare chain of the original.
- `alloc` (`comm3.c`), `reclaim` (`allo3.c`), `nextcook` (`gen3.c`), `eread` (`rdr3.c`), `zzzcode` (`zzz3.c`, `c` is `char`,
  register variables): see `findings.json` for the per-function evidence.
- Tried and not kept: `makeblock` (`l` and `r` as register variables made it worse) and `cbranch` (`true` and `false` as stack
  variables changed the saved register sets).

### Open differences

Listed by function; the current evidence is in `findings.json` (`findings_pending`).

- `clocal` (`tr3.c`, `clocal-size-01`): 230 against 264 instructions; the body is in another block order (the `NAME` block is
  out of line in the original, D7 or source). Swapping the `NAME` and `SCONV` branches was tried: block count 26 to 10, but the
  alignment fell from 47 % to 17 %, so it was reverted.
  Measured again in this round (scratch copy, `blocks.py` and `patterns.check`): the swap gives 10 differing blocks (26 before),
  but the pattern check gets worse: 406 differing instructions (246 before), 6 unexplained (4 before), no moved chunk. The
  original is not a plain `NAME` then `SCONV` order either: its `SCONV` switch (`dec`/`cp`/`jp ugt`/table) sits in the middle of
  the function, after the `NAME` class tests. Rejected again; the layout class of `clocal` is D15 (annotation in findings.json),
  the source form that gives the original order is not found.
  Round "clocal and zzzcode check": storage classes tested (each pointer local automatic in turn; the int locals): no gain, so the difference is the
  layout (D15), not the declarations. The 4 unexplained forms are the immediate shifts `sra`/`srl rS,#n`, which the pattern set does not cover.
  Note on D15 (2026-10-28): D15 names a difference in block order whose cause is not measured. Measured with the old cc
  (`scc -O -S`): an if/else keeps the source order of its branches, so an if/else reorder is not the cause (`host/KNOWLEDGE.md`, D15).
  The layout annotations of `clocal` and `zzzcode` and the D15 tags of the sld functions are kept as recorded; they are not re-evaluated.
  Pairing: confirmed with the original at 0xab48 (the only strong start candidate; the same calls to `makeblock`, `member`, `tbytes`, `szty`, `cerror`; the counts differ). The open review entry is closed.
- `zzzcode` (`zzz3.c`, `zzzcode-order-01`), round "clocal and zzzcode check": storage classes of the locals tested one at a time (q, r, l, tmp, rg, m, n, k, t to automatic; rp, rn2, cls, rn to register): no gain, the source is unchanged. The 4 unexplained forms are immediate shifts (`sra rS,#1`, `srl rS,#2`).
- `zzzcode` (`zzz3.c`, `zzzcode-order-01`): the block `if (notoff(...)) order(r, STAREG)` sits at another place (808 in ours, 920 in
  the original, before the swap below). The two `sra rS,#1` forms are the same in context; shift counts are equal (`shift-type-02` is closed for them).
  Done (`zzz3.c`): the `if (n <= 4)` / `else` branches of the structure move (`ZS`) were swapped to `if (n > 4)` first;
  the original lays out the big branch after the small one, the swap is a layout change. Differing blocks 95 -> 80, 1045 -> 1044
  instructions; corpus codgen 101/101 and scodgen 101/101. Still open: the `if (!tshape(r, STAREG))` block of the big branch
  (orig 920, ours 809 before the swap) and about 20 other blocks in the same function.
  The original lays out the small branch first (`cp n,#4; jp gt,BIG`, small code, `jp` to the end, then BIG). Ours keeps the big branch
  first with `jr le,SMALL`, in both source forms (`if (n <= 4)` and the swapped one). A variant with `return` at the end of the big
  branch (duplicating the tail of the `S` case) gave 78 differing blocks; not kept (duplicated code for 2 blocks).
- `sucomp` (`order3.c`, `sucomp-size-01`; pairing confirmed with the original at 0x49f0, the only strong start candidate, three calls to `szty` in both), round "sucomp check": the locals `nf`, `fl` and `t` are no longer `register` (same meaning; corpus 101/101 for codgen and scodgen). Measured: 715 instructions against 684 (was 731), differing 607 (was 739), moved 19 (was 33), alignment 55 % (was 46 %). The saved registers stay at 7 against the original's 6 (D1). The other locals, one at a time and in pairs, gave no further gain. Before the change: 731 instructions against 684 (after the register declarations; 756 before). 55 `add rS,F`
  forms remain: the original adds a frame slot directly in 49 places, we load it into a register first. Operand order in the sums
  and temporaries for `l->su` were tried without gain; the register declarations were the only change with an effect. The cause is
  not found.
- `tshape` (`match3.c`, `tshape-order-01`), round "tshape check": seven case orders of the switch were tested in a scratch copy. The source order (NAME, ICON, FLD, STAR, REG, OREG, CCODES, FLD before STAR) is the best: 60 differing instructions, 2 unexplained (`and rS,F`), 0 moved, alignment 74 %. The other orders give 64 to 96 differing instructions. No source change. The remaining difference is the SZERO value form (see below) and the operand form `and rS,F`, the same kind as `sucomp` (`add rS,F`): a frame operand used directly in arithmetic, where the original loads the value first. Class D16 (operand form of a frame slot), added in the next round.
- `tshape` (`match3.c`, `tshape-order-01`): the last operand of the `SZERO` return is a value in the original (`cpl rr2,#0; ldk r2,#0;
  tcc eq,r2`), a branch in ours. The `if` form gave no gain. Also tried (scratch copy, tsmeas2): `!p->lval` and a parenthesised
  `&&` chain (both 117 instructions, ratio 0.776, same as now), `? 1 : 0` (113, 0.754), a local `r` before `return r` (117, 0.759).
  None produces `tcc`/`cpl`; the form that does is not found.
- Left from earlier rounds, not re-checked in this round: `cbranch` (the table dispatch after the `cpir` search, D6, and the
  compare chain order 0x56-0x59), `gswitch` (the original is a structurally different case code with about 20 `printf` calls against
  our 10; not reconstructed), `rdsym` (frame of two bytes, D1), `freeparm` (scratch register choice, D2), `ffld` (parameter spilled in
  ours, D4), `ascii` (one reload, D3), `newnode` (the original jumps back into the case 3/7 body, layout), `cerror` (`ldm` with a
  displacement in the original, old cc), `argplace` (local array `cnt` addressed directly in the original, D5), `ltypeop` (the
  original loads `p->op` once into a scratch register, D3).

### codgen runs (facts, not all re-checked in this round)

- `struct node` is laid out as in the original (0x12 bytes: `sname` at 8 doubles as the CBRANCH label, the left subtree overlays the
  low word of `lval`, `rval`/`right` share 0x10). The members are reached through macros (`lval left rval right label`), because the
  K&R `cc` has one member name space (`struct rv` must not use `a`/`b`).
- **Link without /lib/libc.a (codgen, scodgen).** The programs are linked from our objects, `crt0-codgen.o` (made by
  `mkcrt0.sh codgen`, pad field `-`: no jump and no text in front of the code), the oldlibc versions listed in the
  Makefile (cut from the original, see `VERSIONS`) and the members of `libc.a` that are identical to the original
  (`ar x`: `__callex data brk cuexit ffltioc write close read lseek decimal cerror open creat ioctl umask chmod`).
  Corpus 101/101 for both programs. Byte comparison of the libc functions in the linked `codgen` against the original
  (`sdis -l`, words per instruction): 1328 of 1434 words identical, the other 106 differ only in addresses of calls, jumps
  and data. Versions that were first taken as identical and were not (frame sizes of `strlen` and `freopen`, `isatty`,
  `endopen`) are replaced by versions cut from the original: `strlen/0_2`, `freopen/0_1`, `isatty/0_2`, `endopen/0_4`.
  Stubs and data checked against the original (`sdis -l`, words): `write read lseek open creat close chmod umask cerror
  ioctl decimal` identical except the addresses of the calls (to `cerror`, `_errno`); `data` identical except the pointers
  the linker sets. `gtty` (`stty/0_2`, the shipped `stty.o` has a frame of 6 instead of 4) and `isatty` (`isatty/0_2`)
  replaced by versions cut from the original. `__callex` identical.
- **The stray `-` in `printf` (solved).** The `printf` of the shipped `libc.a` (Nov 1983, frame 0x1e) writes a stray `-` in front
  of a `%s` argument (and drops its first character) when the stack at its work area holds an old `%d` radix of -10 and a negative
  number. The original `codgen` and `scodgen` link an older `printf` and `doprnt` that are not in the shipped library. They are now
  cut out of `/lib/codgen` as `src/lib/oldlibc/printf/0_5.s` and `doprnt/0_5.s` (listed in `VERSIONS`; checked instruction by
  instruction against the original, the data block of `doprnt` at program address 0x6532). With these modules the stray character
  does not occur, the workaround `cgprintf` is gone, and the corpus is 101/101 for both programs.
  Lesson: the first attempt used `doprnt/0_3`, which is a different revision (it calls `__cnvt` where the original tests the radix
  inline); it only worked by luck of the stack layout. A `.s` cut from the binary has to keep absolute jump targets as labels.
- `allo` clears `resc[k].sname` (not `stsize`) for the A and B registers; a stale symbol made the result register look as if it held a
  variable.
- `ispow2` returns an `int`; the table `indope` must end with op -1; `ftell` and `ispow2` must be declared (a long return read as int
  loses the high word).
- The statement dispatcher is `stmt()` (0x075a): op 118 `PROC` (function head with the parameters, `fwalkd(parminit)`), 119 (head
  without parameters), 122 `SWTCH` (`gswitch`), else `delay`.
- `fbegin`: a structure return is only covered by the test file `t2.c` (kept in a scratch directory, not in the corpus): a function
  returning a 40 byte structure gives `ld r1,#20` in both builds.

### scodgen (facts from earlier rounds)

`make scodgen` compiles the same sources with `cc -DSEG` (objects in `seg/`). Facts that were not obvious from the function list:

1. A pointer is 32 bit in scodgen: `szty` 2 words, `tbytes` 4, `bsize` 32. The template test `0x8000` ("same size") depends on it.
   With 16 the template `addl AR,AL` matched an int register and gave a bad register pair. `rclass`/`allocreg` put types 8 and 9 into
   the long class; `rsbuild` leaves r14 out of the register sets, `rfree` ignores r14.
2. The frame is `_stkseg+~L1+n(fp)` (fp = r15, sp = rr14). The short form `|_stkseg+~L1+n|(fp)` is used when the offset fits a byte
   (`oregput`). Indexed addresses are `rrB(rI)` with the base in the high byte of `rval` (`adrput`), direct ones `name(rI)`.
3. The sites of `offstar` are not alike: `offstar(p)` takes the STAR itself at the left operand of `setbin`, the right site of `setasg`,
   the right site of `setasop` and the STAR case of `order`; the other sites pass `p->left`. The two versions differ only in the jump
   targets of the shared tail, so a comparison with the addresses removed does not show it.
4. `sucomp` gives a STAR at least `su` 2.
5. In `zzzcode`, `ZT` (long to short): an operand that is neither a REG nor frame based goes to a register by `order(p,6)` first.
6. `gencall` of an indirect call: if no argument register pair is in the way, the address is loaded into a free pair (`rall` 0x1002);
   otherwise rr8 is saved in a temporary (`freetemp(2)/8`) and restored after the call.
7. `conval` for PLUS of pointer constants masks the offset (`& 0x7F00FFFF`, segment byte kept); MINUS keeps only the low byte of the
   difference, the high word becomes 0 (`(lval - val) & 0xFF`). The MINUS mask looks like a slip in the original; kept (no test file
   reaches it, see the quirks table).


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
  `nlist`, `length`, `swap` (differences D1, D3, D4, D8 in "Compiler revisions"). The shipped
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

### popt: feasibility test for byte identity in `src/cmd` (scratch)

Two small functions written as K&R (`absval`, `equstr`, from `host/popt`) compiled with the shipped WEGA `cc -S -O` and compared with
`/lib/popt`. Neither matches:
- `absval`: original frame 2 bytes, no spill of the parameter; the WEGA `cc` gives frame 4 and spills the parameter (D1, D4).
- `equstr`: original keeps the two pointers in `r1` and `r3`; the WEGA `cc` keeps them in the frame (D1, D2).
The shipped `popt` was therefore built by an older compiler revision, as `codgen` was. A K&R source alone cannot give byte identity with
the shipped `cc` for these functions. Result: not feasible for byte identity without the old compiler; behaviour (the corpus of `host/popt`)
is the only check the K&R source can meet until the compiler question is settled. Sample of two functions only.

### popt in `src/cmd/popt` (K&R, first build)

Files: `z2.h`, `z20.c`, `z21.c`, `z22.c`, `z23.c` (K&R versions of `host/popt`; all four compile with the WEGA `cc -c -O`).
Changes against the host copy, needed for the WEGA `cc`: no prototypes with parameter types, no `signed` (`char` is signed on the WEGA),
no `stdlib.h`/`string.h` (missing in the tree), no `calloc`/`memset`/`snprintf` (the WEGA libc has `malloc`, `sprintf`, `strchr`, `strrchr`):
`zalloc` (malloc and clear) and `clearmem` are own helpers. Open questions: the original's heap (the `alloc`/`heaplim` model) and calloc.
Link: `ld -X -e start` with `/lib/crt0.o`, the four objects and `/lib/libc.a` (the byte identity needs the old-cc libc parts of the original,
not done yet).

Behaviour (corpus `host/popt/corpus.sh`, 272 cases from the WEGA sources): 243 identical, 29 fail with "C Optimizer: out of space".
Size: text 26158 against 22272 bytes, data 3582 against 3308. The original fits its heap in the 64 KB limit, this build does not for the
large inputs. Next: the original's allocation (a bump heap, no per-block overhead) and the size difference per function (D classes).
Byte identity: not measured yet.

### popt: analysis setup (`src/cmd/popt/tools`, like codgen)

The original `/lib/popt` is stripped (no symbol table). The setup follows `src/cmd/codgen/tools`:
- `mksym.py`: copy of the sources with `static` removed from the function heads and prototypes (the code is the same; the
  disassembly differs only in the names of the call targets). Build with the WEGA `cc` under the emulator: `popt_sym`, 73 functions.
- `locate.py`: finds each function of `popt_sym` in the symbol table; `--update` writes our range into `findings.json` (73 of 73 found).
- `pair.py`: ranks the candidate starts of `/lib/popt` (`funcs.orig_candidates`) per function by the similarity of the strict
  instruction form (`funcs.canon`, difflib ratio). Output: key `candidates` in `findings.json`. A pairing is not set by the script
  (`pairing` stays `open`); the owner confirms it by hand, as for codgen.
- `classify.py`, `patterns.py`, `findings_md.py`: the same classification as for codgen; they need confirmed pairings.
- `Makefile`: `make pipeline WEGA=<WEGA_ROOT> EMU=...` runs symbuild, locate, pair, classify, findings_md. Work files in `tmp/tmp/popt`.

Ranking of the best candidate per function (`pair.py`, first run; similarity of the whole function in per cent):
`findcopy` 0x26c2 (73), `ccinval` 0x2dda (70), `findlong` 0x2656 (66), `findword` 0x25f4 (52), `findbyte` 0x25b6 (51),
`absval` 0x23a0 (53), `setreg` 0x28ae (47), `killreg` 0x2a46 (56), `isreg` 0x2d22 (39), `compare` 0x31ec (43),
`main` has no usable candidate: 0x002a (43 %) is the start of crt0 in the original (`host/popt/NOTES.md`), not of `main`.
Most functions stay below 40 %; `rmove` (13 %), `usesreg` (9 %), `ovlap` (11 %), `iterate` (10 %) have no usable candidate.

Superseded: the earlier "first function mapping" (approximate, another metric) gave `cleargroup` 0x2a46 (86 %) and `ovlap`,
`movedat` 0x2e2c as plausible. The new ranking does not confirm these (`cleargroup` best candidate 0x2c16, 45 %). Those numbers
are not used any more.

Confirmed pairings (`findings.json`, `pairing` = confirmed, `level` = analysed): `findcopy` 0x26c2, `ccinval` 0x2dda, `findlong` 0x2656.
The callee addresses agree with the other candidates: `isreg` 0x2d22, `findword` 0x25f4, `findlong` 0x2656, `findreg` 0x255a, `equstr` 0x3340.
- `findcopy`: same calls and cases (TB/TW findword, TL findlong, TQ findlong(r+2), return -1). The switch is lowered differently
  (the original uses a table of value/target pairs behind `cpir`, ours an index table): D6. Not byte identical.
- `ccinval`: the original reloads `s` from the frame and, after `clrb`, sets `r2` to 0 (`sub r2,r2`) before the return; ours has
  neither. The reload is D3; the `sub r2,r2` is not clearly one of D1 to D16 (open, reviewed by reading, the classifier says
  "D only").
- `findlong`: our `pat[0]`, `pat[1]` are stored at run time (`ldb`), in the original they are not stored in the code (likely a static
  initialiser, not verified); the char arithmetic is done in another register (D8, D2). Not byte identical.
The classifier verdict "D only (pattern-verified)" is a pattern verdict, not a proof of the reason (see the open point for `ccinval`).
- `ovlap` / `scan` (open, measured 2026-10-27): the original has no `scan` function; `ovlap` is 0x3c5a..0x432c (682 instructions with the
  19 inline copies of the scan loop). A copy of the scan loop at each call site (return 1 on match) passes the corpus (813 of 816,
  only the known `-d` lines), but our `ovlap` is 862 instructions with 80 equal in the comparison, against 219 instructions and
  48 equal without the change. Not adopted. Differences: frame size (0x1a in the original, 0x36 ours), buffer at frame offset 4,
  and the loop jumps back into a shared part (0x3cea, 0x3cd8). The `isalnum` table lookup and `and #7` match the macro.
- popt state (measured 2026-10-27, symbol build vs `/lib/popt`, call forms and the `@r15`/`0(r15)` form masked): 3 functions
  byte-identical (`dumpnodes`, `clearreg`, `comjump`); the rest differs mostly in the frame size (D4: a register parameter gets no
  home slot in the original, D1), scratch registers (D2), loop layout (D7), frame operand form (D16) and the switch (D6).
  Total gap 3347 instructions over 68 functions (3836 at the start of this round, 5778 before the later register rewrites).
  Changes in this round: register declarations and pointer loops in `ilen`, `ccinval` (`return 0`), `dumpnodes`, `statline`,
  `clearreg` (pointer loop), `cleargroup`, `nonlab`, `decref`, `singop` (inline copy, reads address 0 for a missing code text as
  the original does), `natural`, `alloc`, `findslot`, `findreg`, `getnum`, `toofar`, `jswcheck`, `adrlen`, `jumpsw`, `refcount`
  (labhash clear as pointer loop), `codemove`, `iterate`, `shortjump`, `movedat`, `input`, `oplook`, `rmove` (184 -> 405 equal),
  and `ovlap` with `scan` inline (`scan` removed from `z23.c`). A greedy search (`register` on single declarations, measured per
  function) changed 18 functions. Corpus after each step: 813 of 816 (only the known `-d` lines).
- popt-22 (hash inline) is fixed: `HASHNAME` reads the three characters without a test, as the original does. The earlier
  failures were the one-character optab names `{` and `}`: the original stores them with a NUL pad, ours did not, so the
  unconditional read gave another hash at insertion than at lookup. `z20.c` now has `"{\0"` and `"}\0"`.
- Text, step 2 (started): `setcon` no longer emulates the overwrite of the counters. The original has `conloc` with 20 bytes
  and the counters `ndirect`, `nbrbr` directly behind it (in `z22.c` now, as globals). `strcpy` then does what the original does.
  setcon 56 -> 17 instructions (original 32). Corpus: 0 differences without `-d`. Text is still about 1386 bytes larger
  over the 68 functions (ovlap +194, refcount +192, memreg +138, compare +116, procopt +110, codemove +104, rename_regs +92).
- Text, step 2, round 2 (corpus 0 differences without `-d`): register declarations and pointer loops in `refcount` (labhash
  clear as pointer loop, now 107 equal, length 130 vs 128), `compare` (length 109 = 109, 77 equal), `codemove` (length 143 = 143),
  `memreg`, `rename_regs`, `markregs`, `prefix`, `opsetup`; `setcon` without emulation of the counter overwrite. The text of the
  68 functions is now 604 bytes larger than the original (1386 at the start of the step). Left: `ovlap` (+194, inline `scan`
  with other frame and order), `procopt` (+108, address form of local arrays: D5), `memreg` (+108), `oplook` (+50), `usesreg`
  (+34, home slots, D4), `equop`/`jumpsw` (D2). Most of what is left is the old compiler (D2, D4, D5, D16).
- Compiler difference D17 (new, see `host/KNOWLEDGE.md`, "Compiler revisions"): the old cc calls global functions with the
  absolute `call` even inside one object file, the shipped cc uses `calr` there. Popt: 51 of 68 functions agree between the
  storage class in our source and the call forms of the original; 17 do not. Not closable by the file split (the original has
  four object files and our three function groups already match its blocks). Counted as a compiler difference, not as a source fault.
- Text, round 3 (classification of the remaining size gap): `memreg` (+108 before this round, now 162 instructions against 146,
  58 equal): remaining differences are scratch registers (D2), home slots of register parameters (D4), and the index form of
  `regs[i][j]` (`sla` then `add` with the base, D5/D16, the same as in `procopt`). `procopt`: D5 (address form of local arrays).
  `oplook`, `usesreg`, `equop`, `jumpsw`: D2, D4, D16. `ovlap`: not classified yet (the goto experiment did not decide it).
- `oplook` (146 instructions against 138, 65 equal): classified. The hash is computed with `r1`/`rr0` as scratch in the
  original and with `r5`/`rr4` in ours (D2, as defined in `KNOWLEDGE.md`); the frame is 14 bytes against 18 (D1); the placement
  of the two pointers in registers follows the same scratch choice (D2). Three declaration orders were tried, no gain.
- Small functions, round 1: `setcon` uses inline copy loops now (as the original does; was a strcpy call): 33 against 32,
  10 equal. `savereg` (register variant Y): 11 equal, length 35 against 36. `fixldm`: no variant helps (15 equal), the rest
  is the frame (D1/D4). `absval`: argument spill (D4). `regprefix`: the original loads the string pointers from data
  (`ld r2,%0800`), ours loads the address as an immediate (`ld r2,#...`): not yet assigned to D12 or D13.
- Small functions, round 2: `reverse`: the temporaries are in other scratch registers (`rl0`/`rl2` against `rl4`/`rl6`, D2); register
  variants gave no gain (16 equal, kept). `findword`: keeping `n` as register gives 18 equal (length 33 against 31), without it
  16 equal (length 31); the version with more equal instructions is kept. Corpus for the `setcon` and `savereg` steps: 0
  differences without `-d`.
- Text, round 4 (largest gaps after the small-function round, instructions): `ovlap` (743 against 682, see above), `rmove` (957
  against 949, 397 equal), `procopt` (257 against 248, D5), `output` (179 against 210, the switch: D6), `memreg` (162 against 146,
  D2/D4/D5), `usesreg` (241 against 239, D4/D2), `oplook` (D1/D2), `codemove` (143 = 143, 75 equal), `repladdr` (109 against
  130, register variants tried, no gain: frame/scratch, D2/D4), `input` (130 against 125), `killreg` (135 against 136, 80 equal).
  Text size of the 68 functions: 608 bytes over the original.
- `output` (179 against 210 instructions, 77 equal): classified as D6. The original indexes its jump table directly from the
  operation value (`ld r2,%0016(r3)`), the shipped scc computes the index with `add`/`neg` and loads from the table (the form
  described in `host/KNOWLEDGE.md`, D6). The bodies are in source order in both. No source change: the switch form is fixed by
  the compiler, not by the source.
- `memreg` (162 against 146, 58 equal): the register loops are the same; the difference is the address form of `regs[reg]` (the
  original computes index times 32, then adds the base from its register; the shipped cc adds the constant base first, D5/D16).
  Writing the access as `*(regs + reg)` gives the same code (58 equal). Classified, no source change.
- Open items, classified in this round: `usesreg` (241 against 239, 159 equal): frame 6 bytes more (home slots of the three
  register parameters, D4), scratch registers (D2). `killreg` (135 against 136, 80 equal): the array index form `sla`/`clrb`
  and the jump table of the switch (D5, D6). `input` (130 against 125): the comparison chain of the switch (D6), the byte flag
  `echo` (D8). `codemove` (143 = 143, 75 equal): three declaration orders tried, the parameter goes into another register
  than in the original, the cause is not found yet (open, not classified).
- Next group, classified (no source change): `setcc` (22 against 20): frame 6 against 4 (D4) and the scratch byte `rl4` against
  `rl0` (D2). `dest` (32 against 33): scratch registers (`r1` against `r4`, D2) and the switch order of the value cases (D6).
  `natural` (27 against 26): frame and loop layout (D4, D7). `itoa_` (26 against 27): scratch registers `r0`/`r2`/`r3` against
  `r4`/`r5` (D2). `singop` (16 = 16, 8 equal): scratch byte `rl0` against `rl4` (D2). `statline` (17 = 17, 11 equal): frame
  and the register parameter (D4, D16).
- Batch of the remaining functions, classified (no source change): `cclook` (71 against 67, 17 equal): scratch registers for
  the copy loop (`r1`/`r6` against `r4`/`r5`, D2). `rmove` (957 against 949, 397 equal): call forms (D17), saving of registers
  (D1, D2). `opsetup` (40 against 35): scratch registers for the hash (D2), frame (D1). `absval` (12 against 9): argument
  spill (D4). `procopt` (257 against 248): address form of local arrays (D5). `repladdr` (109 against 130): frame and scratch
  (D2, D4). `regprefix` (17 against 18): string pointers loaded from data (`ld r2,%0800`), not yet assigned to D12/D13.
- `codemove` (143 = 143, 75 equal), open: the parameter as register or not (73 or 75 equal), and the declaration orders tried so far
  give no gain. Not classified yet.
- `ovlap` (743 against 682, 93 equal), classified in part: the 19 copies have the same shape as in the original (outer loop over
  `r`, inner compare, the table test at the end). Differences per copy: the scratch registers of the character loads (`rl3`
  against `rl2`, D2) and the address of the pattern `b+1`, which the original keeps in a register (`ld r13,r12`) and we compute
  with `lda` in each copy: the original keeps a value across statements (`lda` once, then `ld r13,r12`), which is closest to D3
  (register contents kept), not proven; a test with a register variable `pat` changes the allocation of the whole function. A register `pat = b + 1` gives 726 instructions but only 84 equal:
  not kept. Frame 26 against 54 bytes (D1, D4). Status: mostly D, the address form not yet proven.
  Function order: the 68 confirmed functions are in the same order as in the original (`findings.json`, key
  `order_original`). The three source files keep their link order (z21, z22, z23) and the definitions inside each file
  follow the original; global declarations stay at the top of their file in their order, prototypes were added for
  functions used before their definition (no change of code). Corpus after the reorder: 813 of 816.
  Open and not D-classified: `dest` (the original branches on the value in another order), the call form `call`/`calr`
  (open: the original uses `calr` for some calls across what we build as separate objects; the file split of the original
  is not known), and the orders of the remaining functions.
Band 40 % to 60 % (initial rating, rule: similarity >= 50 %, start not taken, internal calls agree in both directions,
length ratio 0.8 to 1.25; `findings.json` keys `pairing`, `rating`):
- Confirmed: `absval` 0x23a0, `findbyte` 0x25b6, `findword` 0x25f4, `killreg` 0x2a46.
- Correction: the first call comparison of this band missed the internal calls, because the symbol build shows them as
  `calr _name` and the check only read `call %addr`. `setreg`, `findreg` were rejected on that wrong reading. Redone with the
  call graph (below): `setreg` 0x28ae, `findreg` 0x255a, `findslot` 0x2cd8 are confirmed.
- Call graph method (`findings.json` `rating`): our internal calls from `calr _name` in the symbol build, the original's calls from
  `calr L0t_hhhh` and `call %hhhh`. A pairing is confirmed when the confirmed neighbours agree in both directions. Functions with
  no confirmed neighbour cannot be decided this way (e.g. `adrlen`, `cleargroup`, `compare`).
- Rejected (kept `open`, reason in `rating`): `singop`, `setcc`
  (0x2dda is `ccinval`), `getch` (0x23a0 is `absval`), `main` (0x2a was taken as crt0; wrong, see below), `newcode1` (length), `statline` (similarity 45 %),
  and the others below 50 %.
- `classify.py` puts all confirmed functions (now 10: plus `setreg`, `findreg`, `findslot`) in "D only (pattern-verified)". Spot checks show this verdict is too lenient:
  `findcopy`, `ccinval`, `findlong` have differences that are not D1 to D16 (see above). Treat the class as a pattern verdict only.
Call graph, second step: `makereg` 0x2988 (callers `setreg` and `killreg` both call it), `memreg` 0x270e (called by `setreg`),
`isreg` 0x2d22 (called by `findcopy` and `setreg`) are confirmed. Libc calls agree too (ours 0x5EB8 and 0x6494 against the
original 0x5016 and 0x54D6). 13 functions are confirmed now; all 13 are "D only (pattern-verified)" by the classifier,
which is a pattern verdict only.
Entry and main (call order): crt0 is 0x0 to 0x29 (argv setup, `call` main at 0x001e, `exit` call at 0x0024), so 0x002a is `main`.
An earlier note put crt0 at 0x2a; that was wrong, and `host/popt/NOTES.md` said `z21` starts at 0x42 (wrong too); both are corrected. Ours and the original
call the same functions in the same order in `main`: the libc groups (3 + 3 calls), `opsetup`, `input`, `movedat`, `refcount`,
`iterate`, `clearreg`, `comjump`, `rmove`, `jumpsw`, `shortjump`, `procopt`, `output`, `heapreset`, and 23 calls of `statline`
(ours `calr _statline` 23 times, original 23 calls to 0x0322). Confirmed by the call order and by the two-way check of the
callees: `main` 0x002a, `statline` 0x0322, `input` 0x0358, `movedat` 0x2fc8, `refcount` 0x0ca0, `comjump` 0x12e6,
`rmove` 0x13b6, `shortjump` 0x213c, `procopt` 0x33d6. Not yet decided: `opsetup`, `clearreg`, `jumpsw`, `iterate`,
`output`, `heapreset` (main's call order gives one original call fewer than ours for `output` and `heapreset` in that stretch;
open).
Classifier after this step: 22 confirmed functions; 21 "D only (pattern-verified)"; `main` is "other differences": the forms
`sra rS,#10` and `srl rS,#10` are not in the pattern set (the same open point as in codgen, see above).
Note on the call form in the symbol build: internal calls appear both as `calr _name` and as `call %hhhh` (absolute), so
both must be read; a reading that takes only one form gives wrong call sets.
Next step in `main`: `clearreg` 0x24d4 (same length 12, no calls either side), `jumpsw` 0x207e (two calls to `absval` on both
sides, length 86 against 57: larger difference), `iterate` 0x0e1a (the four callees agree as a set, length 202 against 150: larger
difference). 25 functions were confirmed at that point (`opsetup` was open then, see the later entry). Open in this stretch: `opsetup` (the original 0x09ec makes no calls, ours `opsetup` calls
`hashname`), `output` and `heapreset` (one call at 0x0610 in the original, a loop over the node list that calls 0x06ba; ours calls
`output` and `heapreset`; `heapreset` is a store of HEAP0 and `ret` and does not appear in the original `main`).
Classifier: 24 "D only (pattern-verified)", 1 "other differences" (`main`, see above). The length differences of `jumpsw` and
`iterate` are not explained yet.
`opsetup` (now confirmed, see below), `output`, `heapreset` (main, after `procopt`): output and heapreset still open.
- `output`: probable 0x06ba. The original's call counts match ours (3 calls to the prefix function, 6 to one libc function, 3 to
  another); order and length differ (178 against 235 instructions). Its caller in the original is the loop function 0x0610 (a separate
  function that loops over the node list and calls 0x06ba); ours has the loop inside `main`. That is a difference of the source
  structure of `main`, not yet classified.
- `heapreset`: the constant of `HEAP0` (0x1a48) is not stored anywhere in the original code; main in the original calls no reset
  function. Probably no reset in the original (to be decided; it changes memory use per iteration).
`opsetup` = 0x09ec (confirmed): the original loops over `optab` (start 0x004e, entries of 4 bytes) and probes the table of 73
two-byte entries (`cp` with 0x1630, restart at 0x159e). The hash is computed inline: `div` by 73 at 0x0a16. Ours calls `hashname`,
a separate function in `z21.c`. The original has no call there; the same inline hash appears at 0x0aa2 (in the function at 0x0a44) and
0x0baa (in the function at 0x0b02, probably `oplook`). Ours reads the name conditionally (`c0 ? s[1] : 0`); the original reads
`s[1]` and `s[2]` without a test. So `hashname` as a function is a source difference (inline hash as a macro would match the code
shape). Classifier: "other differences" (`push @r5,r4` form is not in the pattern set, and the call of `hashname`).
26 functions confirmed; 24 "D only (pattern-verified)"; 2 "other differences" (`main`, `opsetup`).
`jumpsw` and `iterate` (length differences): both are confirmed by the call sequence in `main` and `iterate`, and the length is not
a sign of a different function. `jumpsw` (0x207e): two calls to `absval` on both sides; the original keeps its pointers in `r13`/`r14`,
ours reloads them from the frame (D2, D3); 86 against 57 instructions. `iterate` (0x0e1a): the calls come in the same order (`nonlab`,
`decref` four times, `xjump`, `codemove`); the first lines are the same (`nchange = 0`, `first.forw`, the JP/CJP tests, `nbrbr++`);
the code differs in register use and reloads; 202 against 150 instructions, 29 % instruction alignment. The classifier says "D only",
but that alignment does not support the verdict; the function needs a closer look (open).
Confirmed in this step: `nonlab` 0x244e, `decref` 0x2416, `codemove` 0x1102, `xjump` 0x100e (the callees `insertl` and `equop` are
confirmed in the next step). 30 functions confirmed.
`iterate` (block comparison, 0x0e1a, complete): the six sections of `iterate` are in the same order in both versions and have the same
tests and calls:
| section | source | original (lines of `iterate`) | ours (instructions) |
| S1 loop head, `nchange = 0` | first lines | 1-6 | 1-6 |
| S2 redirect jumps to jumps (`nonlab`, `decref`, `nbrbr`) | JP/CJP/JSW with ref | 7-34 | 7-46 |
| S3 CJP over JP (`revbr`, `nskip`) | CJP block | 35-67 | 47-96 |
| S4 dead code after JP/RET | break set LABEL, DLABEL, OBRACE, CBRACE, 0, DATA | 68-112 | 97-151 |
| S5 jump to next label (`njp1`) | JP/CJP to label | 113-136 | 152-186 |
| S6 `xjump` and `codemove` | JP | 137-143 | 187-194 |
| loop step and return | | 144-150 | 195-202 |
The differences are register use and reloads (the original keeps `p` in `r14`, `rp` in `r13`, `rp2` in `r12` and saves them with
`ldm`; ours keeps them in the frame): D2, D3, and the prologue D1. The block order is the same, so the earlier suspicion of D15
(layout) was wrong. Classifier: "D only" is supported by this block map (a block map, not a byte proof).
`insertl` = 0x108c and `equop` = 0x23b4 (confirmed via callers). `insertl`: the original has the same three callers (`xjump` 0x100e,
`codemove` 0x1102, `backjmp` 0x133c), calls `allocnode` (0x2476) with size 14, and the same field assignments as `z21.c`; 34 against 51
instructions, the difference is the frame reloads (D2, D3). `equop`: same callers (`xjump`, `backjmp`), no calls, 42 against 43
instructions. With these, the tentative callees of `xjump` and `codemove` are confirmed. 32 functions confirmed.
`heapreset` and `allocnode` (heap model). The original never resets its allocation pointer: `main` has no call to a reset, and the
constant of `HEAP0` (0x1a48) is not stored anywhere in the code. The original `allocnode` (0x2476) is a bump allocator on the
pointer at data 0x163c with the limit at 0x163e; it grows the break in steps of 0x7d0 through a call at 0x5564 and calls an error
routine at a failed growth (the message is probably the one of `out of space`, the text address 0x0b36 is not verified). Ours calls `heapreset` in every pass of `main`, and our `allocnode` takes its memory
from `zalloc` (malloc) and keeps the bump only for the statistics (`heaplim`, `heapcur`). So: (1) the call to `heapreset` is a source
difference (not in the original); (2) the memory model of `allocnode` is a source difference (malloc against the original bump on the
break). Both change the behaviour when memory runs out: the corpus cases "C Optimizer: out of space" (29 of 272) are probably from
this (not measured separately). Decision for the owner: whether to rebuild the bump model on the break (as in the original, byte
identity) or keep malloc.
`output` (probable 0x06ba, not confirmed): the original calls 0x06ba from the separate loop function at 0x0610; ours calls `output`
from the loop inside `main`. Counts of the calls agree (3, 6, 3), order and length differ (178 against 235). So `output` stays open,
and the loop is a source-structure difference.
Initial rating, call-graph step (assignment only, no source change): the confirmed neighbours fix further functions uniquely.
The check runs in both directions; start addresses already taken by a confirmed function are excluded. Result:
`allocnode` (corrected below), `backjmp` 0x133c, `cleargroup` 0x2c16, `dualop` 0x2c72, `equstr` 0x3340, `findcon` 0x3196, `getnum` 0x05d2,
`ilen` 0x224c, `jswcheck` 0x02c8, `redunbr` 0x30ec, `repladdr` 0x2e2c, `savereg` 0x24f6, `toofar` 0x21fe, `adrlen` 0x2352,
`dest` 0x29f4 (`rating` in `findings.json`: "confirmed by call graph"). `isused` also points to 0x3196; `findcon` takes it because
its callees (`equstr`, `isreg`) are on the original 0x3196; `isused` stays open. `hashname` has no start in the original (its hash is
inline at three places). 47 functions confirmed; classifier: 44 "D only (pattern-verified)", 3 "other differences". Open (26):
`alloc`, `cclook`, `clearmem`, `compare`, `copy`, `fixldm`, `getch`, `getln`, `hashname`, `heapreset`, `isused`, `itoa_`, `markregs`,
`natural`, `newcode1`, `oplook`, `output`, `ovlap`, `prefix`, `regprefix`, `rename_regs`, `scan`, `setcc`, `setcon`, `singop`, `usesreg`.
No source file was changed in this step.

Open functions, second pass (assignment only): `markregs` 0x432c (calls itself as ours does; libc call agrees), `rename_regs` 0x36d0
and `fixldm` 0x37cc (called by `procopt` in our order), `regprefix` 0x2df8 (three calls in `repladdr`, as ours). 51 confirmed.
`copy` stays open: the candidate 0x0964 calls `allocnode` (0x2476), ours `copy` calls `alloc` (0x18f6).
Corrections of the tools: the original was disassembled only up to 0x3838 (the default limit `LIM`) in `classify.py`, `patterns.py`
and `pair.py`. But 0x3838 is a popt function (it calls `isreg` and itself), so the popt code goes beyond 0x3838 and the libc start is
not known yet (the "libc from ~0x3838" in `host/popt/NOTES.md` is a guess and wrong). The three tools now read the whole original
(`lim=0x10000`). The end rule of the classifier gives 145 instructions for `markregs` (we count 86) and 97 for `fixldm` (34): the
audits of these three are not reliable yet; their class "D only" is not checked.
procopt: the call order in the original starts with `dualop`, ours with `clearmem` twice; `markregs` appears twice in both, once
before and once after `dualop` in ours. The order of the calls differs: not classified yet.
Open functions, third pass (assignment only): by call graph and call order, `getln` 0x04d6 (called twice by `input` as ours; calls
`oplook`), `oplook` 0x0b02 (138 against 138 instructions), `cclook` 0x0a44 (66 against 67; the hash is inline in the original),
`compare` 0x31ec, `singop` 0x2c4a (both in `redunbr` in our order), `isused` 0x3838, `usesreg` 0x38d0 (calls `dualop`, `equstr`, `isreg`
and 0x3c5a), `natural` 0x3396 (the two callers 0x32f0 and 0x3362 are the callers of ours setcon and setcc), `setcon` 0x32f0 (between
`savereg` and `ccinval` in `rmove`, as ours; 32 against 56 instructions). 60 functions confirmed.
Not assigned: `getch` (the original 0x04d6 calls only libc here; the getc call is probably inline, as `hashname`), `hashname`
(inline, no start), `heapreset` (no start in the original), `setcc` (candidate 0x3362, order unclear), `ovlap` (candidate 0x3c5a: it
calls 0x3c06, 0x551c, 0x5016, not `scan` as ours), `prefix` (candidate 0x0630: three calls from the output loop; depends on `output`,
which is probable 0x06ba but not confirmed), `output`, `alloc`, `clearmem`, `copy`, `newcode1`, `scan`.
Open (count 13): `alloc`, `clearmem`, `copy`, `getch`, `hashname`, `heapreset`, `itoa_`, `newcode1`, `output`, `ovlap`, `prefix`, `scan`, `setcc`.
`output` = 0x06ba and `prefix` = 0x0630 (confirmed by structure). `output`: the same cases in both versions; `prefix` is called three
times (JP, CALL and CJP cases), six printf calls and three putchar of the newline; the original shares the printf tail (`jp` to 0x08e6
and 0x08ea), which is the tail merge of D11; 178 against 235 instructions. `prefix`: the tests of CALL (0x20), of `@` (0x40) and of `(`
(0x28) and the four strings are the same as ours; 47 against 57 instructions.
Caller difference (in `main`, not in `output`): the original has the output loop as a separate function at 0x0610 (it walks the node
list from `first` and calls 0x06ba for each node); `main` in ours contains this loop. So the original has a function without counterpart
in our source (0x0610), and `main` differs from it in structure. Not classified yet.
11 functions remain open: `alloc`, `clearmem`, `copy`, `getch`, `hashname`, `heapreset`, `itoa_`, `newcode1`, `ovlap`, `scan`, `setcc`.
`alloc` = 0x2476 and `copy` = 0x0964 (confirmed by structure). The original has ONE allocation function (0x2476) that takes the size
in `r7`: it rounds the size, compares the break pointer (data 0x163c) with the limit (0x163e), grows by 0x7d0 through the call at 0x5564
(error exit on failure) and returns the old pointer. Its callers pass 14 for nodes (`insertl`, `input`, `rmove`, `jswcheck`). Ours
`alloc` has this structure; our `allocnode` is a second function for the node size (zalloc of `sizeof node`), with no counterpart in the
original. So the earlier entry "allocnode 0x2476" was wrong: `allocnode` stays open (no own start in the original). The memory itself:
the original takes it from the break (sbrk, pointer at 0x163c), ours from `zalloc` (malloc); the bump is kept only for statistics.
`copy` 0x0964: the string length is computed by inline loops in the original, ours calls `strlen` (libc); the rest matches.
`clearmem` and `newcode1` have no own function in the original. `clearmem`: `procopt` (0x33d6) clears its two arrays with inline loops
(`clrb @r2` in two loops at the start), and `movedat` (0x2fc8) makes no call at all. `newcode1` (ours a wrapper of `copy(1, tmpl, 0)`):
`rmove` calls 0x0964 (`copy`) 11 times in the original; ours calls `copy` 5 times directly and `newcode1` 6 times (which calls `copy`),
so the wrapper is inlined in the original. Like `allocnode`, `hashname`, `getch` and `heapreset`, both are source differences
(function boundaries), not a different function.
`setcc` = 0x3362 (confirmed: the second caller of `natural` next to `setcon`; the copy of `strcpy` is inline in the original)
and `itoa_` = 0x3c06 (confirmed by structure: the only `div` by 10; the reversal is the separate function 0x3b9a in the original, which
calls `strlen` and swaps; ours has it inline). Open: `scan` (no start in the original matches: no call of a scan-like function in the
region of `ovlap`), `ovlap` (candidate 0x3c5a: calls `itoa_` as ours, but not `scan`, and its extent of 661 instructions has no end).
`ovlap` (second look): the original region 0x3c5a to 0x42f4 has one `ret` (at 0x42f4), 661 instructions, calls `itoa_` (0x3c06) eight
times, libc (0x551c seven times, 0x5016 three times), and no call of a scan-like function and no `r` compare. Ours `ovlap` and `scan`
together have 264 instructions and call `scan` about 17 times. So 0x3c5a is probably not `ovlap` with `scan` inline; no start for
`scan` and no clean candidate for `ovlap`. Both stay open.
`ovlap` = 0x3c5a (confirmed by call count): `usesreg` (0x38d0) calls it seven times, as ours calls `ovlap` seven times; the other calls of
`usesreg` agree (`isreg` 8/8, `equstr` 2/2, libc 1/1; `dualop` 6 against 7 is open). The libc calls of `ovlap` agree (10/10); `itoa_`
is called 8 times in the original against 7 in ours (open). `scan`: no start in the original; the 24 compares with `r` in 0x3c5a to
0x42f4 and our 18 calls of `scan` suggest that `scan` is inline in `ovlap` (probable, not confirmed).
Differences in `ovlap`, `itoa_`, `scan`, `dualop` (resolved to a cause, except the dualop location):
- `scan` has no function in the original: its body is repeated inline at each call site (19 references of the `isalnum` table in
  0x3c5a to 0x42f4, the same number as the 19 call sites in `ovlap`; the body is recognisable by the `r` compares). Ours is a function.
- `itoa_` and `scan` counts in our binary are one lower than in the source (`itoa_` 7 against 8, `scan` 18 against 19): a test with the
  WEGA cc shows that `return g(y)` in one case becomes a jump to the call of `g` in another case (tail merge, D11). The original keeps all
  calls. So the difference is D11 (compiler), not the source.
- `dualop` in `usesreg`: the source has 6 call sites, our binary 6, the original 7. The original has one call more than the source;
  the place is not determined (open).
Review of the classification (manual, replaces the pattern verdict as the basis; the pattern verdict is kept in `findings.json`, `audit`):

| review class | count | functions |
|---|---|---|
| reviewed: D-pattern | 5 | `findcopy`, `insertl`, `iterate`, `jumpsw`, `prefix` |
| reviewed: structure | 11 | `alloc`, `copy`, `ilen`, `itoa_`, `main`, `opsetup`, `output`, `ovlap`, `procopt`, `setcc`, `usesreg` |
| open | 2 | `ccinval`, `findlong` |
| pattern only, not supported | 16 | `clearreg`, `codemove`, `dest`, `dualop`, `equop`, `fixldm`, `getln`, `input`, `isused`, `markregs`, `regprefix`, `rename_regs`, `rmove`, `setcon`, `singop`, `xjump` |
| pattern only, not inspected | 32 | `absval`, `adrlen`, `backjmp`, `cclook`, `cleargroup`, `comjump`, `compare`, `decref`, `equstr`, `findbyte`, `findcon`, `findreg`, `findslot`, `findword`, `getnum`, `isreg`, `jswcheck`, `killreg`, `makereg`, `memreg`, `movedat`, `natural`, `nonlab`, `oplook`, `redunbr`, `refcount`, `repladdr`, `savereg`, `setreg`, `shortjump`, `statline`, `toofar` |

`reviewed: D-pattern`: read in detail; the differences are register use, reloads, frame and switch lowering (D1, D2, D3, D6, D8).
`reviewed: structure`: the source (or the function boundary) differs from the original; needs an owner decision before any change.
`pattern only, not supported`: the pattern verdict says "D only", but the instruction alignment is below 40 %; the verdict does not prove anything.
`pattern only, not inspected`: the pattern verdict only; not read in detail.
`open`: a difference found that the D classes do not explain (`ccinval`: `sub r2,r2`; `findlong`: static initialisation of `pat`).

Open functions without a start in the original: `allocnode`, `clearmem`, `getch`, `hashname`, `heapreset`, `newcode1`, `scan` (`allocnode`, `clearmem`, `newcode1`, `getch`, `hashname`, `heapreset`: no own function, `scan`: inline in `ovlap`; `ovlap` confirmed at 0x3c5a).
Open difference: one `dualop` call more in the original `usesreg` (location not found).

libc of the original `popt` (step 2 of the libc plan; read-only, no source changed). The libc part is 0x4422 to 0x56fc.
- Identical to the shipped `lib/libc.a` (14): ffltioc, cuexit, decimal, brk, chmod, close, creat, ioctl, lseek, open, read, umask, write,
  cerror.
- Identical to a module of `src/lib/oldlibc` (18 exact matches, all 65 versions assembled with cas and searched as one archive):
  freopen 0_1 (0x4422), printf 0_5 (0x444a), doprnt 0_5 (0x455a), ffltioc 0_1 (0x4b80), filbuf 0_3 (0x4b88), setbuf 0_1 (0x4c62),
  flsbuf 0_2 (0x4cb6), endopen 0_4 (0x4e94), cnvt 0_2 (0x506e), length 0_2 (0x51fe), malloc 0_3 or 0_5 (0x521c, both match; to decide),
  isatty 0_2 (0x5436), stty 0_2 (0x5458), strcat 0_2 (0x54ae), strncmp 0_1 (0x54d6), strcpy 0_2 (0x551c), strlen 0_2 (0x553a).
- Not in any source yet (call targets without a matched member): 0x5006 `exit` (a stub that calls cuexit at 0x500c and returns),
  0x5016 `atoi` (the digit loop, differs from the shipped member), 0x548c `strrchr` (keeps the last match; differs from the shipped member),
  0x5472 unknown (calls ioctl 0x55b4, constant 0x7408; 1 call). These need new versions in `src/lib/oldlibc`.
- Name tool notes: `libcver.py` names by fingerprint (ratio below 0.6 is a guess); the exact matches above are names with the instruction sequence.
  A false match of `getchar.o` on our own `getch` (0x0556) shows that single short members must be checked by hand.
Findings from the libc of the original (`findings.json`, key `findings`):
- `popt-01` `equop`: the original compares inline (no call); ours calls `strcmp` (libc).
- `popt-02` `fixldm`: the original calls `strrchr` (last occurrence of `#`), ours `strchr` (first). Behaviour differs if `#` occurs twice;
  decision of the owner (rebuild the original behaviour first, record in the quirks table).
- `popt-03` `isreg`, `rename_regs`, `scan`: `isalnum` and `isdigit` are inline table references in the original (table at data 0x0be5);
  ours use the table `__ctype` of the shipped `ctype_.o`. Calls agree; table contents and place differ (to check).
- `popt-04` `memreg`, `rmove`, `rename_regs`: `strncmp` at the same places in the original (0x54d6); no difference, closed when confirmed.
malloc in `oldlibc` (checked): version 0_3 is the complete object of the original malloc.o (0x521c to 0x5434): `_malloc`, `_free`,
and after the ret of `_free` the routine `_realloc` (it calls `_free` and `_malloc`, copies the data). `0_3.s` lacked the label `_realloc`
(added; the instructions are unchanged, the module still matches the original as 191 instructions). Version 0_5 is malloc and free only
(it ends at the ret of `_free`); there is no foreign code in 0_5. The shipped `malloc.o` exports `_malloc`, `_free`, `_realloc`, `_calloc`,
`_cfree`. popt uses 0_3 (malloc in popt is not called by realloc; the symbol is only completeness). Correction: the code after 0x5392 is `realloc`,
not an unknown routine.
popt corpus after the allocator change (`tmp/tmp/popt/run/cmp.sh`): 814 of 816 identical (272 cases, flags none, -l, -d). The earlier
21 differences ("C Optimizer: out of space") are gone. The 2 rest differ only in the -d line "K core" (31K against 30K): the break at the start
(sbrk(2) in main) is the end of data+bss, which is 0x7148 in the original and 0x7bf4 in ours (program size). Correction: the original DOES reset
the heap in main (inline: heapcur = base); the earlier statement that it does not was wrong. The original's base is the break from sbrk(2).
Also found: the original main calls setbuf (0x4c62, oldlibc setbuf 0_1) before the heap start; our main does not (open difference).
main, analysis before any edit (after masking all absolute addresses: 101 of 196 instructions equal). Differences:
- D-classes: entry (`dec #12` and `ldm` against `dec #14` and `ldl`, the register set), reloads from the frame, `ldk r13,#0` against a
  frame clear: D1, D2, D3.
- Structure, source level: (1) the original calls `setbuf` (0x4c62, oldlibc setbuf 0_1) before the heap start; ours does not. (2) the
  output loop is the separate function 0x0610 in the original (it calls 0x06ba = output for each node); ours loops in `main`. (3) argument
  order of `statline`: the original passes the text in r7 and the number in r6 (statline 0x0322 tests r6 first), ours passes the number
  first; the signature of `statline` in the original is `(s, n)`, to be checked against the other calls. (4) `sra` against `srl`: the
  original's `heaplim` expression is signed (`int`), ours `unsigned short`.
- The `-d` line of the corpus (K core) follows from (4) only in form; the value differs by the program layout (see above).
Order of the functions to be worked through (proposal, no edits yet): 1 statline (argument order), 2 main (setbuf, output loop as its own
function, heaplim type), 3 itoa_ (reversal as a separate function), 4 copy and setcc (inline strlen and strcpy), 5 equop (inline compare),
6 fixldm (strchr against strrchr: owner decision first), 7 isreg, rename_regs, scan (ctype table: compare the table first), 8 the functions
without own start in the original (allocnode, clearmem, newcode1, getch, hashname) as inline code or macros, 9 ovlap and usesreg (scan inline,
one dualop call more), 10 procopt (inline clear loops), 11 opsetup (inline hash), 12 ilen (order of five instructions), 13 open: ccinval,
findlong. The D-only functions need no source change.
Step 1 of the order done: `statline`. The original is `statline(s, n)`: it tests `n` first (r6), prints `%d ` with `n` and then `%s\n` with
`s`; the text is passed in r7, the number in r6. Ours was `statline(n, s)` with the same body. Changed: signature `statline(s, n)` and the 23
calls of `main` now pass the text first (z21.c; the old version is in `tmp/tmp/popt/z21.c.before_statline`). Compared after masking: the two
fprintf calls and the test are the same; the rest is register and frame use (D1 to D3). Corpus result: see the next entry.
Step 2 of the order: `main` (done in an experiment copy first, then applied; the old files are in `tmp/tmp/popt/*.before_main`).
- setbuf: after a successful `freopen` of argv[2] the original calls `setbuf(stdout, buf)` with a static `buf[BUFSIZ]` (512, from head/stdio.h).
  Added in the same place (only when argc > 2).
- output loop: a separate function (0x0610 in the original, `dumpnodes()` in ours), entered at the step of the loop (`p = &first`, then `p = p->forw`).
- heap reset: the original sets `heapcur = heapbase` inline after the `maxiter` test; `heapreset()` is removed (no function in the original).
- `heaplim`, `heapbase`, `heapcur` are `int` (the original shifts with `sra` in the `-d` line); the alloc compare uses unsigned casts.
- Result after masking absolute addresses: `main` 158 of 195 instructions equal, length 195 against 195; `dumpnodes` 12 of 12 with the same structure;
  the rest is register and frame use (D1 to D3). `alloc` 16 of 28 (register for n, D2).
`main`, second round (experiments in `tmp/tmp/popt/exp`, measured with `try.sh`): `register` for `niter`, `maxiter`, `isend` (the original
keeps them in r14, r13, r12); the freopen check as `if ... else setbuf(...)` (the original places the setbuf branch out of line). Result:
171 of 198 instructions equal (original 195; the rest: frame size 14 against 12 (D1), register choice for argv and the reloads (D2, D3)).
Calls: the original calls the global functions `opsetup`, `refcount`, `iterate` with absolute `call`; our compiler emits `calr` for them
(static functions are `calr` in both). A difference of the compiler revision, to be classified (not D1 to D16 as far as known; open).
Re-sync after the changes to `main` and `statline` (symbol build rebuilt from the current sources, `locate.py --update`, `classify.py`):
72 function ranges moved and were updated in `findings.json`; `heapreset` is no longer a function (its reset is inline in `main`, marked
open with no start); `dumpnodes` (the output loop, 0x002a to 0x004e) is added and confirmed against the original 0x0610. The classifier:
65 "D only (pattern-verified)", 2 "other differences" (`opsetup`, `ilen`). `main` is no longer "other differences" (the `sra` form matches).
`main`, end: the original calls `exit(0)` and returns; our `return 0;` after `exit(0)` was removed (the original has no such instruction;
byte identity first). `main`: 175 of 197 instructions equal (original 195). Remaining: frame size and register choice (D1 to D3) and the call form
of the global functions `opsetup`, `refcount`, `iterate` (the original uses absolute `call`, our compiler `calr`; no source form reproduces it:
static functions are `calr` in both). The call form is an open point of the compiler revision (candidate for the D classes; owner decision).
usesreg, after the register change: 143 of 242 instructions equal. Calls by target agree in number except dualop: the original calls it seven
times (0x3918, 0x393c, 0x39b0, 0x39f2, 0x3a40, 0x3a84, 0x3ac0), the source has six sites and the binary six calls. Open as popt-17 (the case of
the extra call is not mapped yet).
Byte identity: not reached for any function yet. Next: `absval`, `equstr` (small), then the functions with 50 % to 60 %.


- 0x79ca accepted (done, 2026-10-27): the message printer (EPU spills, argument block form in KNOWLEDGE). Dead entries
  0x7a5e and 0x7ae2 (not called, no references) stay open.
