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

| Binary | Object | D1 frame | D2 scratch regs | D3 reloads | D4 arg spill | D5 const index | D6 switch | D7 loops | D8 bytes | D9 >>8 | D10 long shift | D11 tail merge | D12 data order | D13 pointer const | D14 varargs, post-incr | D15 if/else layout | D16 frame operand |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cas | ifile.c | x | x | x |  |  | x |  | x | x | x |  | x |  |  |  |  |
| cas | instr.c | x | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |
| cas | scanner.c | x | x | x |  |  | ? | ? |  |  |  | x | x |  |  |  |  |
| cas | main.c |  | x |  |  |  | ? | ? |  |  |  |  | x |  |  |  |  |
| cas | nodes.c | x | x |  |  | x | x | ? |  |  |  |  | x |  |  |  |  |
| cas | parser.y (parser.c) | x | x | x |  | x | x | ? |  |  | x |  | x |  |  |  |  |
| cas | expr.c | x | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |
| cas | p2loop.c |  | x | x |  |  | x |  | x |  |  |  |  |  |  |  |  |
| cas | ofile.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |
| cas | gen.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |  |  |
| cas | assemble.c |  | x | x |  |  | x |  |  |  | x |  | x |  |  |  |  |
| cas | getops.c |  | x | x |  |  | x |  |  |  |  | x | x |  |  |  |  |
| cas | error.c |  |  | x |  |  | x |  |  |  |  |  | x |  |  |  |  |
| cas | listing.c |  |  | x |  |  |  | x | x |  |  |  | x |  |  |  |  |
| cas | optim.c |  |  | x | x |  | x | x |  |  |  | x | x |  |  |  |  |
| cas | jpr2.c |  |  | x |  |  |  |  |  |  |  |  | x |  |  |  |  |
| cas | jpr.c |  |  | x | x |  | x | x | x |  |  |  | x |  |  |  |  |
| cas | locntr.c | x | x | x | x |  | x |  |  |  |  |  | x |  |  |  |  |
| cas | ssymtab.c |  |  |  |  |  | x |  |  |  |  | x | x | x |  |  |  |
| cas | symbols.c | x | x |  |  |  | x |  | x |  | x | x | x |  |  |  |  |
| cas | pst.c | x | x |  |  |  |  | ? | x |  |  |  |  |  |  |  |  |
| cas | subr.c |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |
| ranlib | ranlib.c, readsym.c | x | x |  | x |  |  |  |  |  |  |  | x |  |  |  |  |
| strip | strip.c |  | x |  |  |  |  |  |  |  |  |  | x |  |  |  |  |
| size | size.c |  |  |  |  |  |  |  |  |  |  |  | x |  |  |  |  |
| libc.a | malloc.o (gen/malloc.c) |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | nlist.o (gen/nlist.c) | x |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | length.o (gen/length.c) | x |  |  |  |  |  |  | x |  |  |  |  |  |  |  |  |
| libc.a | swap.o (gen/swap.c) |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| size | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |
| size | libc: rdwr.o (fread, fwrite) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |
| size | libc: doprnt.o (_doprnt) |  |  |  |  |  |  |  |  |  |  |  |  |  | ? |  |  |
| strip | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |
| cas | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |  |  |
| codgen, scodgen | main3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | reader3.c |  |  |  | x |  | x | x | ? |  |  |  |  |  |  |  |  |
| codgen, scodgen | rdr3.c |  |  | x | x |  | x |  | ? |  |  |  |  |  |  |  |  |
| codgen, scodgen | efcode3.c | x |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | local3.c |  | x |  | x |  | ? |  |  |  | x |  |  |  |  |  |  |
| codgen, scodgen | gen3.c |  |  |  | x |  |  |  |  |  |  | ? |  |  |  |  |  |
| codgen, scodgen | order3.c |  |  |  | x |  | x |  |  |  |  | x |  |  |  |  | x |
| codgen, scodgen | match3.c |  |  |  | x |  | x | ? |  |  |  |  |  |  |  |  | x |
| codgen, scodgen | allo3.c |  | x | x | x |  | x | ? |  |  | x |  |  |  |  |  |  |
| codgen, scodgen | regallo3.c |  |  |  | x |  | x |  |  |  | ? |  |  |  |  |  |  |
| codgen, scodgen | regfile3.c |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tmp3.c |  |  |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | zzz3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  | x |  |
| codgen, scodgen | comm3.c |  |  | x | x |  | x |  | x |  |  |  |  |  |  |  |  |
| codgen, scodgen | out3.c |  | x |  |  |  | x | x | x |  | x | x |  |  |  |  |  |
| codgen, scodgen | regs3.c |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | lcl3.c |  | x |  | x |  | x |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tr3.c |  |  |  | x |  |  | x | ? |  |  |  |  |  |  | x |  |
| codgen, scodgen | optim3.c |  |  |  |  |  | x |  |  |  |  |  |  |  |  |  |  |

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
Byte identity: not reached for any function yet. Next: `absval`, `equstr` (small), then the functions with 50 % to 60 %.

