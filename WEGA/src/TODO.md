# WEGA source tree: TODO

Living list of what in `src/` is not yet byte-identical with the original
WEGA, why, and what we use instead. Everything in `src/` should build with
`make`; a file listed here is built from a workaround until the cause is gone.
Git content is in English; the German notes of the original WEGA sources stay as they are.

Last checked: 2026-10-17. Verification (from `WEGA/host`):
`make check` in host/ (cc: 74 command lines identical; popt: 522 corpus runs
identical; libc: 170 identical, 7 different, 3 without C source; oldcc: all
ok) and `make verify-wega`, which builds `src/cmd` with the original cc, cas,
as, ld, ar and strip (under z8k-emu), runs `make strip-programs`, and reports
per program. Result of this run, for the ar/ranlib/size/strip/
getud/putud/copydisk/dump/dumpdir/format/restor/sync/tar/verify group: ar,
dump, dumpdir, restor and sync are identical; pb.image and sa.timer are
identical; everything else differs or is not built, see the table.

## Differences and workarounds

Workaround `oldcc/...s` means: the file in `src/lib/libc/oldcc/` is the cas
assembler source of the original object; `cas -u` on it gives the
byte-identical member. Such a `.s` is only used where the C source cannot
produce the same bytes with the shipped cc.

| Source | Object | Status | Commit with the original build | Cause | Workaround |
|---|---|---|---|---|---|
| `lib/libc/gen/malloc.c` | malloc.o | differs | - | Older cc: calloc uses `mult rr2,r6` (register still known) instead of a memory operand | `lib/libc/oldcc/malloc.s` |
| `lib/libc/gen/nlist.c` | nlist.o | differs | - | Older cc: stack frame 2 bytes smaller | `lib/libc/oldcc/nlist.s` |
| `lib/libc/gen/length.c` | length.o | differs | - | Older cc: `ldb rl3,@r2` / `testb rl3`, frame 2 bytes smaller | `lib/libc/oldcc/length.s` |
| `lib/libc/gen/swap.c` | swap.o | differs | - | Older cc: `0(r15)` form for the spill of an address-taken argument | `lib/libc/oldcc/swap.s` |
| `lib/libc/stdio/doscan.c` | doscan.o | differs | - | Different revision (Nov 1983), floating point, calls `_inflt`; the source is the older version | `lib/libc/oldcc/doscan.s` |
| `lib/libc/stdio/printf.c` | printf.o | differs | - | Different revision (Nov 1983): printf/fprintf/sprintf via `_pfile`, `_flsbuf`, new `_doprnt` | `lib/libc/oldcc/printf.s` |
| `lib/libc/stdio/scanf.c` | scanf.o | differs | - | Different revision (Nov 1983), floating point | `lib/libc/oldcc/scanf.s` |
| (none in src/) | cnvt.o | no C source | - | C source not reconstructed | `lib/libc/oldcc/cnvt.s` |
| (none in src/) | fltioc.o | no C source | - | C source not reconstructed | `lib/libc/oldcc/fltioc.s` |
| (none in src/) | symscan.o | no C source | - | C source not reconstructed | `lib/libc/oldcc/symscan.s` |
| `lib/oldlibc/doprnt.c` | - | not built | - | Structural reconstruction of the Jan 1983 `_doprnt`, untested | - |
| `cmd/ar.c` + `lib/libc/crt/crt0-ar.s` | ar | identical (built with the WEGA cc) | - | - | `crt0-ar.s` is the start-up variant of this binary |
| `cmd/size.c` + `lib/libc/crt/crt0-size.s` | size | own code identical, file differs | - | The libc part of the original is an older libc revision | none; verify compares own code only |
| `cmd/strip.c` + `lib/libc/crt/crt0-strip.s` | strip | differs in the own code (2 blocks, 0x2b0 and 0x2bc, `cmpins` range 0x68c) | - | Not the crt0 change: the old and new `crt0-strip.s` assemble to identical objects, and every commit back to 14098cf gives the same 2 blocks. The code differs in expression form (`addl rr2,rr4` / `ldl %2490` vs `ld r6,r4` / `addl rr4,rr6`), so the shipped strip seems to come from another source revision. Earlier note 'own code identical' is not reproduced | none yet |
| `cmd/ranlib.c` | ranlib | builds with the WEGA cc, not identical, unfinished | - | Needs the older cc | none yet |
| `cmd/getud.c`, `cmd/putud.c` | getud, putud | builds, differs (8422 vs 8506, 8260 vs 8346 bytes) | - | Not analysed | none yet |
| `cmd/copydisk.c` | copydisk | builds, differs (7318 vs 7148 bytes) | - | Not analysed | none yet |
| `cmd/format.c` | format | differs (6738 vs 6568 bytes) in the current version 1.6 | 77770f2 | Newer version in tree: version 1.6 adds the `signal(..., SIG_IGN)` block (SIGALRM, SIGHUP, SIGINT, SIGQUIT, SIGTERM) and a new date. Version 1.5 (77770f2) builds byte-identical to `bin/format` | use 77770f2's `format.c` if the byte-identical build is wanted |
| `cmd/tar.c` | tar | builds, differs (21692 vs 22494 bytes) | - | Not analysed | none yet |
| `cmd/verify.c` | verify | builds, differs (6696 vs 6526 bytes) | - | Not analysed | none yet |
| `cmd/BOOT0/boot0.fd.s`, `boot0.ud.s`, `boot0.rm.s` | boot0.fd, boot0.ud, boot0.rm | builds, not compared | - | No original in `arc/cpio` | none |
| `cmd/doscopy/*.c` (8 commands) | doscat ... dosformat | not built | - | The emulator stops at EPU instruction `8E04` (not emulated, see `host/z8k-emu/README.md`) while the original cc compiles `dosopn.c`, which has no floating point. Cause in the cc not yet traced (cc at 00:b270) | none; blocked by the emulator |
| `cmd/pburn/*.c` | pburn | **exception: newer source than in WEGA (allowed), expected to differ; not built** | - | Newer version in tree (2.6). 2.3 (ffc49bb45) and 2.6 do not link (`bss overflow`); the 2.6 sources disagree on `namef[]` (`pb.cmdline.c` 80, `pb.buf.c`/`pb.file.c` 50). 1.4 (71cf2e46a) links but is 27674 bytes vs 41570 for the original (both without `-s`). The original has its symbol table, so it is not stripped and `-s` in pburn/Makefile is wrong for it. None of the three matches | none; the sources are not changed |
| `cmd/standalone/sa.cat.c`, `sa.format.c`, `sa.verify.c`, `sa.mkfs.c`, `sa.install.c`, `sa.shipdisk.c` | sa.cat, sa.format, sa.verify, sa.mkfs, sa.install, sa.shipdisk | differ (sizes 1060/1036, 8916/10204, 4050/5162, 4284/4260, 7146/7122, 1816/2464) | - | Not analysed. `libb.a` is not the cause: `sa.timer` (same library) is identical. Probably a different source revision | none yet |
| `cmd/standalone/sa.diags/` | sa.diags | builds, no original to compare | - | The MON16 modules `p.gesa.s`, `p.ldsd.s`, `p.term.s` are copied from `OlliL/P8000` `firmware/MON16` into the build directory (not in git). `sa.test.s` needs `as -u` (without it, `as` stops with ERROR 50 at `call _printf`) | none |

## Scripts without an object check

`cmd/chkdiff`, `cmd/uuclear` and `cmd/whodo` are text scripts, not compiled
programs, so there is no object to compare. Their originals are not checked
(the old status 100% from the earlier `cmd/TODO` is not confirmed).

## Layout still open

- `cc` and `popt` are Linux-side tools and live in `host/` (not WEGA sources).

- The single-file commands are flat in `cmd/` (V7 layout); `copydisk`, `dump`,
  `dumpdir`, `format`, `getud`, `putud`, `restor`, `sync`, `tar`, `verify`
  are built by `make` there, `strip-programs` strips them before the compare.
- `BOOT0`, `doscopy`, `pburn`, `standalone` have their own template Makefiles
  and are built by `make` in `cmd/` (`subdirs`). `standalone/sa.diags` is
  separate and not built (see the table).
- `make` (the `all` target) does not strip; `make strip-programs` does, and
  `make verify-wega` in `host/` runs it before comparing.
- `cmd/doscopy/mk` is a one-line helper (`make doscat ...`) that duplicates
  the `all` target; not used.
- `lib/libc` has no Makefile of its own yet.

## Original programs without source in src/

From the earlier `cmd/TODO`; the old percentages are dropped, only the list is kept.

**bin/** (93): adb, as, bc, bcmp, bd, cas, cat, chgrp, chkin, chkout, chkwhat, chmod, chown, cmp, cp, csh, date, dc, dd, df, diff, dog, du, echo, echo2, ed, edit, egrep, env, ex, file, find, getfile, gets, gpasswd, grep, hd, kill, ld, ln, local, login, ls, mail, make, mkdir, more, mv, newgrp, nice, nl, nm, nohup, od, page, passwd, pr, printenv, ps, pstat, putfile, pwd, regcmp, remote, rm, rmail, rmdir, rsh, scc, sh, sld, sleep, sort, stty, su, sum, tail, tee, test, time, touch, tr, uname, uucp, uulog, uuname, uusend, uux, vi, view, whereis, who, write

**etc/** (52): GETTY, INIT, UGETT1, UGETTY, adduser, chmog, chog, chroot, clri, cron, datem, dcheck, devnm, down, dqueuer, fsck, fsdb, getty, halt, icheck, init, install, kdb, killall, labelit, link, login, makelost+found, makewhatis, man.dbmgr, mfs, mkfs, mknod, mlf, mount, mvdir, ncheck, new.install, rc, rc_csh, rmuser, setlp, setmnt, shut, sysgen, ttyconfig, umfs, umount, unlink, update, upkeep, wall

**usr/** (137): bin/LOAD, bin/SEND, bin/acctcom, bin/admin, bin/apropos, bin/at, bin/awk, bin/banner, bin/basename, bin/bcheck, bin/bdiff, bin/cal, bin/calendar, bin/cb, bin/cdc, bin/checkcw, bin/checkeq, bin/code, bin/col, bin/comb, bin/comm, bin/cpio, bin/cref, bin/crypt, bin/csplit, bin/ct, bin/ctags, bin/cu, bin/cut, bin/cw, bin/cxref, bin/daytime, bin/delta, bin/deroff, bin/dfilter, bin/diff3, bin/diffmk, bin/dircmp, bin/dirname, bin/eqn, bin/error, bin/expand, bin/expr, bin/false, bin/fgrep, bin/flow, bin/get, bin/getNAME, bin/getopt, bin/grpck, bin/head, bin/help, bin/hyphen, bin/id, bin/join, bin/lex, bin/line, bin/lint, bin/logname, bin/look, bin/lorder, bin/lpr, bin/m4, bin/man, bin/mesg, bin/mkmenu, bin/mkstr, bin/mm, bin/mmchek, bin/mmt, bin/mvt, bin/neqn, bin/news, bin/nq, bin/nroff, bin/nroffv7, bin/objdu, bin/objhdr, bin/objsu, bin/pack, bin/paste, bin/pcat, bin/plz, bin/plzcg, bin/plzsys, bin/prof, bin/prs, bin/ptx, bin/pwck, bin/quot, bin/ranlib, bin/reset, bin/rmchg, bin/rmdel, bin/sact, bin/sccsdiff, bin/script, bin/sdiff, bin/sed, bin/spell, bin/spellin, bin/spellout, bin/split, bin/sprof, bin/str, bin/strings, bin/strip, bin/strprint, bin/tabs, bin/talk, bin/tbl, bin/timex, bin/troff, bin/troffv7, bin/true, bin/tsort, bin/tty, bin/uimage, bin/unget, bin/uniq, bin/units, bin/unpack, bin/users, bin/val, bin/vls, bin/vnews, bin/wc, bin/what, bin/whatis, bin/whoami, bin/whois, bin/xargs, bin/xq, bin/xstr, bin/yacc, bin/zmenu, bin/zmprint
