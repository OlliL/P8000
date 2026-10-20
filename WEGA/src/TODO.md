# WEGA source tree: TODO

Living list of what in `src/` is not yet byte-identical with the original
WEGA, why, and what we use instead. Everything in `src/` should build with
`make`; a file listed here is built from a workaround until the cause is gone.
Git content is in English; the German notes of the original WEGA sources stay as they are.

Last checked: 2026-10-19. Verification (from `WEGA/host`):
`make check` in host/ (last run 2026-10-17: cc 74 command lines identical;
popt 522 corpus runs identical; libc 170 identical, 7 different, 3 without C
source; oldcc all ok) and the build of `src/cmd` with the original cc, scc,
cas, as, ld, gld, ar and strip under z8k-emu, followed by the compare per
program (see `host/KNOWLEDGE.md`). Programs that are identical or whose own
code is identical are marked in the table; the rest differs or is not built.

## Differences and workarounds

Workaround `oldcc/...s` means: the file in `src/lib/libc/oldcc/` is the cas
assembler source of the original object; `cas -u` on it gives the
byte-identical member. Such a `.s` is only used where the C source cannot
produce the same bytes with the shipped cc.

| Source | Object | Status | Cause | Workaround |
|---|---|---|---|---|
| `lib/libc/gen/malloc.c` | malloc.o | differs | Older cc (register contents, D3) | `lib/libc/oldcc/malloc.s` |
| `lib/libc/gen/nlist.c` | nlist.o | differs | Older cc (frame size, D1) | `lib/libc/oldcc/nlist.s` |
| `lib/libc/gen/length.c` | length.o | differs |  Older cc (frame size D1, byte loads D8) | `lib/libc/oldcc/length.s` |
| `lib/libc/gen/swap.c` | swap.o | differs |  Older cc (argument spill, D4) | `lib/libc/oldcc/swap.s` |
| `lib/libc/stdio/doscan.c` | doscan.o | differs | Different revision (Nov 1983), floating point, calls `_inflt`; the source is the older version | `lib/libc/oldcc/doscan.s` |
| `lib/libc/stdio/printf.c` | printf.o | differs | Different revision (Nov 1983): printf/fprintf/sprintf via `_pfile`, `_flsbuf`, new `_doprnt` | `lib/libc/oldcc/printf.s` |
| `lib/libc/stdio/scanf.c` | scanf.o | differs | Different revision (Nov 1983), floating point | `lib/libc/oldcc/scanf.s` |
| (none in src/) | cnvt.o | no C source | C source not reconstructed | `lib/libc/oldcc/cnvt.s` |
| (none in src/) | fltioc.o | no C source |  C source not reconstructed | `lib/libc/oldcc/fltioc.s` |
| (none in src/) | symscan.o | no C source | C source not reconstructed | `lib/libc/oldcc/symscan.s` |
| `lib/oldlibc/doprnt.c` | - | not built | Structural reconstruction of the Jan 1983 `_doprnt`, untested | - |
| `cmd/ar.c` + `lib/libc/crt/crt0-ar.s` | ar | identical (built with the WEGA cc) | - | `crt0-ar.s` is the start-up variant of this binary |
| `cmd/size.c` + `lib/libc/crt/crt0-size.s` | size | own code identical (0 differing blocks up to the first libc member), strings identical, file differs | The libc part of the original is an older libc revision; the program built from `size.c` gives the same output as the original on the corpus (objects, swapped objects, damaged files) | none; verify compares own code only |
| `cmd/strip.c` + `lib/libc/crt/crt0-strip.s` | strip | behaves like the original; own code differs in 2 blocks (0x2b0 and 0x2bc, `cmpins` range 0x68c) | Older cc (scratch registers, D2): one expression, `head.s_imsize = (long)segt[0].sg_code + segt[0].sg_data`. Source forms tried without success: both operands cast to long, operands swapped, cast of the sum, one cast moved. Strings and the rest of the code are identical; run on the whole corpus (objects, swapped objects, damaged files, `-h`) the program built from `strip.c` gives the same output and files as the original | none |
| `cmd/ranlib.c` + `lib/oldlibc/readsym.c` | ranlib | behaves like the original (same `__.SYMDEF` for libc.a, slibc.a, libcurses.a), code differs | Older cc (frame size D1, scratch registers D2, argument spill D4); the old `readsym` of the original is `lib/oldlibc/readsym.c` | none |
| `cmd/getud.c`, `cmd/putud.c` | getud, putud | own code identical, file differs | unstripped binary, temporary cc filename differs cause PID changes every run | none yet |
| `cmd/doscopy/*.c` (8 commands) | doscat ... dosformat | not built | `/lib/cparse` stops at EPU instruction `8E04` (not emulated, see `host/z8k-emu/README.md`) on the floating constant in `dosopn.c` line 175 (`1.5*first`); every doscopy command links `dosopn.o` | none; blocked by the emulator (EPU arithmetic) |

## Compiler differences (old compiler)

The numbers D1 ... D13 are the sections of "Compiler revisions" in `host/KNOWLEDGE.md`. `x` = the difference
was seen in this object; `?` = the construct is in the source but the difference was not separated from the
others yet; empty = not seen (no proof that it is absent). The cas modules that exist so far are listed;
new rows follow with every module. The old libc members that are only inside the binaries (size, strip, ranlib, cas) are listed with
the marks of D14 only: that is all that was checked for them.

| Binary | Object | D1 frame | D2 scratch regs | D3 reloads | D4 arg spill | D5 const index | D6 switch | D7 loops | D8 bytes | D9 >>8 | D10 long shift | D11 tail merge | D12 data order | D13 pointer const | D14 varargs, post-incr |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| cas | ifile.c | x | x | x |  |  | x |  | x | x | x |  | x |  |  |
| cas | instr.c | x | x |  |  |  | ? | ? |  |  |  |  | x |  |  |
| cas | scanner.c | x | x | x |  |  | ? | ? |  |  |  | x | x |  |  |
| cas | main.c |  | x |  |  |  | ? | ? |  |  |  |  | x |  |  |
| cas | nodes.c | x | x |  |  | x | x | ? |  |  |  |  | x |  |  |
| cas | parser.y (parser.c) | x | x | x |  | x | x | ? |  |  | x |  | x |  |  |
| cas | ssymtab.c |  |  |  |  |  | x |  |  |  |  | x | x | x |  |
| cas | symbols.c | x | x |  |  |  | x |  | x |  | x | x | x |  |  |
| cas | pst.c | x | x |  |  |  |  | ? | x |  |  |  |  |  |  |
| cas | subr.c |  |  |  |  |  |  | ? |  |  |  |  |  |  |  |
| ranlib | ranlib.c, readsym.c | x | x |  | x |  |  |  |  |  |  |  |  |  |  |
| strip | strip.c |  | x |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | malloc.o (gen/malloc.c) |  |  | x |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | nlist.o (gen/nlist.c) | x |  |  |  |  |  |  |  |  |  |  |  |  |  |
| libc.a | length.o (gen/length.c) | x |  |  |  |  |  |  | x |  |  |  |  |  |  |
| libc.a | swap.o (gen/swap.c) |  |  |  | x |  |  |  |  |  |  |  |  |  |  |
| size | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| size | libc: rdwr.o (fread, fwrite) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| size | libc: doprnt.o (_doprnt) |  |  |  |  |  |  |  |  |  |  |  |  |  | ? |
| strip | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |
| cas | libc: printf.o (printf, fprintf, sprintf) |  |  |  |  |  |  |  |  |  |  |  |  |  | x |

## In progress

- `cmd/cas/` (the cas assembler): decompilation from `/bin/cas`, module by module (`cmd/cas/MODULES`). Done:
  ifile, instr, scanner, main, done, nodes, parser, symbols, pst, subr and the tables. The original was built by an older scc, so the code differs (see "Compiler differences" below and
  `cmd/cas/README`); byte identity is not expected.

## Scripts without an object check

`cmd/chkdiff`, `cmd/uuclear` and `cmd/whodo` are text scripts; they were
compared using `cmp` and are identical.

## Layout still open

- `cc` and `popt` are Linux-side tools and live in `host/` (not WEGA sources).

- The single-file commands are flat in `cmd/` (V7 layout): `ar`, `dump`,
  `dumpdir`, `getud`, `putud`, `ranlib`, `restor`, `size`, `strip`, `sync`,
  `tar`.
- For some commands several versions exist. For those, subdirectories were
  created, one per version (even if it is a single C source). The `Makefile`
  in the subdirectory builds the version shipped with WEGA 3.1; the others
  are not built. This applies to `pburn` (2.1; others 1.4/2.3/2.6),
  `copydisk`, `format` and `verify` (1.5; 1.6 alternative).
- `BOOT0`, `doscopy`, `standalone` have their own template Makefiles and are
  built by `make` in `cmd/` (`SUBDIRS`). `format` is not in `SUBDIRS` yet.
  `standalone/sa.diags` is separate and built as well, but only the p8000
  version shipped with WEGA 3.1 (`p8000c` is an unreleased variant); its
  Makefile needs `ASFLAGS=-u` (the original `as` needs `-u` for external
  symbols like `_printf`).
- `cmd/Makefile`: `install` has typos (`donee`, lines 94-95); the `strip-programs`
  rule documented in its header does not exist yet.
- `cmd/doscopy/mk` is a one-line helper (`make doscat ...`) that duplicates
  the `all` target; not used.
- `lib/libc` has no Makefile of its own yet.

## Original programs without source in src/

From the earlier `cmd/TODO`; the old percentages are dropped, only the list is kept.

**bin/** (93): adb, as, bc, bcmp, bd, cas, cat, chgrp, chkin, chkout, chkwhat, chmod, chown, cmp, cp, csh, date, dc, dd, df, diff, dog, du, echo, echo2, ed, edit, egrep, env, ex, file, find, getfile, gets, gpasswd, grep, hd, kill, ld, ln, local, login, ls, mail, make, mkdir, more, mv, newgrp, nice, nl, nm, nohup, od, page, passwd, pr, printenv, ps, pstat, putfile, pwd, regcmp, remote, rm, rmail, rmdir, rsh, scc, sh, sld, sleep, sort, stty, su, sum, tail, tee, test, time, touch, tr, uname, uucp, uulog, uuname, uusend, uux, vi, view, whereis, who, write

**etc/** (52): GETTY, INIT, UGETT1, UGETTY, adduser, chmog, chog, chroot, clri, cron, datem, dcheck, devnm, down, dqueuer, fsck, fsdb, getty, halt, icheck, init, install, kdb, killall, labelit, link, login, makelost+found, makewhatis, man.dbmgr, mfs, mkfs, mknod, mlf, mount, mvdir, ncheck, new.install, rc, rc_csh, rmuser, setlp, setmnt, shut, sysgen, ttyconfig, umfs, umount, unlink, update, upkeep, wall

**usr/** (137): bin/LOAD, bin/SEND, bin/acctcom, bin/admin, bin/apropos, bin/at, bin/awk, bin/banner, bin/basename, bin/bcheck, bin/bdiff, bin/cal, bin/calendar, bin/cb, bin/cdc, bin/checkcw, bin/checkeq, bin/code, bin/col, bin/comb, bin/comm, bin/cpio, bin/cref, bin/crypt, bin/csplit, bin/ct, bin/ctags, bin/cu, bin/cut, bin/cw, bin/cxref, bin/daytime, bin/delta, bin/deroff, bin/dfilter, bin/diff3, bin/diffmk, bin/dircmp, bin/dirname, bin/eqn, bin/error, bin/expand, bin/expr, bin/false, bin/fgrep, bin/flow, bin/get, bin/getNAME, bin/getopt, bin/grpck, bin/head, bin/help, bin/hyphen, bin/id, bin/join, bin/lex, bin/line, bin/lint, bin/logname, bin/look, bin/lorder, bin/lpr, bin/m4, bin/man, bin/mesg, bin/mkmenu, bin/mkstr, bin/mm, bin/mmchek, bin/mmt, bin/mvt, bin/neqn, bin/news, bin/nq, bin/nroff, bin/nroffv7, bin/objdu, bin/objhdr, bin/objsu, bin/pack, bin/paste, bin/pcat, bin/plz, bin/plzcg, bin/plzsys, bin/prof, bin/prs, bin/ptx, bin/pwck, bin/quot, bin/ranlib, bin/reset, bin/rmchg, bin/rmdel, bin/sact, bin/sccsdiff, bin/script, bin/sdiff, bin/sed, bin/spell, bin/spellin, bin/spellout, bin/split, bin/sprof, bin/str, bin/strings, bin/strip, bin/strprint, bin/tabs, bin/talk, bin/tbl, bin/timex, bin/troff, bin/troffv7, bin/true, bin/tsort, bin/tty, bin/uimage, bin/unget, bin/uniq, bin/units, bin/unpack, bin/users, bin/val, bin/vls, bin/vnews, bin/wc, bin/what, bin/whatis, bin/whoami, bin/whois, bin/xargs, bin/xq, bin/xstr, bin/yacc, bin/zmenu, bin/zmprint
