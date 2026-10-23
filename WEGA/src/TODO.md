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

The numbers D1 ... D14 are the sections of "Compiler revisions" in `host/KNOWLEDGE.md`. `x` = the difference
was seen in this object; `?` = the construct is in the source but the difference was not separated from the
others yet; empty = not seen (no proof that it is absent). All cas modules with code are listed (table1,
table2, pstab and the three table segments are data only). The old libc members that are only inside the binaries (size, strip, ranlib, cas) are listed with
the marks of D14 only: that is all that was checked for them.

| Binary | Object | D1 frame | D2 scratch regs | D3 reloads | D4 arg spill | D5 const index | D6 switch | D7 loops | D8 bytes | D9 >>8 | D10 long shift | D11 tail merge | D12 data order | D13 pointer const | D14 varargs, post-incr |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cas | ifile.c | x | x | x |  |  | x |  | x | x | x |  | x |  |  |
| cas | instr.c | x | x |  |  |  | ? | ? |  |  |  |  | x |  |  |
| cas | scanner.c | x | x | x |  |  | ? | ? |  |  |  | x | x |  |  |
| cas | main.c |  | x |  |  |  | ? | ? |  |  |  |  | x |  |  |
| cas | nodes.c | x | x |  |  | x | x | ? |  |  |  |  | x |  |  |
| cas | parser.y (parser.c) | x | x | x |  | x | x | ? |  |  | x |  | x |  |  |
| cas | expr.c | x | x | x |  |  | x |  |  |  | x |  | x |  |  |
| cas | p2loop.c |  | x | x |  |  | x |  | x |  |  |  |  |  |  |
| cas | ofile.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |
| cas | gen.c |  | x | x |  |  | x |  | x |  |  |  | x |  |  |
| cas | assemble.c |  | x | x |  |  | x |  |  |  | x |  | x |  |  |
| cas | getops.c |  | x | x |  |  | x |  |  |  |  | x | x |  |  |
| cas | error.c |  |  | x |  |  | x |  |  |  |  |  | x |  |  |
| cas | listing.c |  |  | x |  |  |  | x | x |  |  |  | x |  |  |
| cas | optim.c |  |  | x | x |  | x | x |  |  |  | x | x |  |  |
| cas | jpr2.c |  |  | x |  |  |  |  |  |  |  |  | x |  |  |
| cas | jpr.c |  |  | x | x |  | x | x | x |  |  |  | x |  |  |
| cas | locntr.c | x | x | x | x |  | x |  |  |  |  |  | x |  |  |
| cas | ssymtab.c |  |  |  |  |  | x |  |  |  |  | x | x | x |  |
| cas | symbols.c | x | x |  |  |  | x |  | x |  | x | x | x |  |  |
| cas | pst.c | x | x |  |  |  |  | ? | x |  |  |  |  |  |  |
| cas | subr.c |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |
| ranlib | ranlib.c, readsym.c | x | x |  | x |  |  |  |  |  |  |  | x |  |  |
| strip | strip.c |  | x |  |  |  |  |  |  |  |  |  | x |  |  |
| size | size.c |  |  |  |  |  |  |  |  |  |  |  | x |  |  |
| libc.a | malloc.o (gen/malloc.c) |  |  | x |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | nlist.o (gen/nlist.c) | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | length.o (gen/length.c) | x |  |  |  |  |  |  | x |  |  |  |  |  |  |
| libc.a | swap.o (gen/swap.c) |  |  |  | x |  |  |  |  |  |  |  |  |  |  |
| size | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| size | libc: rdwr.o (fread, fwrite) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| size | libc: doprnt.o (_doprnt) |  |  |  |  |  |  |  |  |  |  |  |  |  | ? |
| strip | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| cas | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| codgen, scodgen | main3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |
| codgen, scodgen | reader3.c |  |  |  | x |  | x | x | ? |  |  |  |  |  |  |
| codgen, scodgen | rdr3.c |  |  | x | x |  | x |  | ? |  |  |  |  |  |  |
| codgen, scodgen | efcode3.c | x |  |  | x |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | local3.c |  | x |  | x |  | ? |  |  |  | x |  |  |  |  |
| codgen, scodgen | gen3.c |  |  |  | x |  |  |  |  |  |  | ? |  |  |  |
| codgen, scodgen | order3.c |  |  |  | x |  | x |  |  |  |  | x |  |  |  |
| codgen, scodgen | match3.c |  |  |  | x |  | x | ? |  |  |  |  |  |  |  |
| codgen, scodgen | allo3.c |  | x | x | x |  | x |  |  |  |  |  |  |  |  |
| codgen, scodgen | regallo3.c |  |  |  | x |  | x |  |  |  | ? |  |  |  |  |
| codgen, scodgen | regfile3.c |  |  |  | ? |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | tmp3.c |  |  |  | x |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | zzz3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |
| codgen, scodgen | comm3.c |  |  | x | x |  | x |  | x |  |  |  |  |  |  |
| codgen, scodgen | out3.c |  |  |  |  |  | x |  | x |  | x | ? |  |  |  |
| codgen, scodgen | regs3.c |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| codgen, scodgen | lcl3.c |  | x |  | x |  | x |  |  |  |  |  |  |  |  |
| codgen, scodgen | tr3.c |  |  |  | x |  |  |  | ? |  |  |  |  |  |  |
| codgen, scodgen | optim3.c |  |  |  |  |  | ? |  |  |  |  |  |  |  |  |

The codgen / scodgen rows come from the function comparison of `host/tools/cgcmp.py` (KNOWLEDGE.md "codgen fidelity"; the original has the same source for both): `x` = seen in a function of the module, `?` = construct present, not yet separated; D12 (data order) and D13 / D14 are not checked yet. What was seen: D1/D4 functions with register parameters have no home slots in the original and copy the parameters first-to-last (the shipped cc reserves the slots and copies last-to-first: `funcend`, `epilog`); D2 register variables of leaf functions sit in r8-r14 in the original (`freetemp`, `symreg`); D3 the old popt reloads globals (`freeparm`, `mkhash`, `tagfind`); D4 spills (`getlr`, `member`, `nullsyms`); D6 up to 3 case labels as a compare chain and `ld r,off(r3)` after `cpir` (`rsfreeall`, `store`, `geninc`); D7 loop inversion (`delay`); D8 `extsb`/`subb` (`adrput`, `cerror`); D10 `sdal` for shifts (`ispow2`, `adrput`); D11 shared tails (`setasop`, `fwalkd`). The choice of `call` or `calr` depends on the distance and is not a compiler difference.

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
  between large files, several overflowing start segments in combined mode, see KNOWLEDGE.md). `gld` (`host/ld/gld.c`, same files and messages as `/bin/gld`; not reproduced: the original's undefined output when no object is loaded, its memory limit ("out of memory(load2td)" with sections of about 50000 bytes) and hangs with sections near 64K). Still under the
  emulator: `cpp`, `cparse`/`scparse`, `codgen`/`scodgen`, `gopt`, `as`. `spopt` is done (`host/popt`, `make spopt`). `codgen`/`scodgen` exist as source (`src/cmd/codgen`; text-identical on the corpus; byte equality per function in progress, register variables done, see KNOWLEDGE.md "codgen fidelity"; the `src/cmd/Makefile` entry open). Next: `gopt`, the front end (order in `host/HANDOFF_wega_disasm.md`). `src/cmd/ld` (a source that builds the byte-identical
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
