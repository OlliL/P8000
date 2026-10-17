# WEGA source tree: TODO

Living list of what in `src/` is not yet byte-identical with the original
WEGA, why, and what we use instead. Everything in `src/` should build with
`make`; a file listed here is built from a workaround until the cause is gone.
Git content is in English; the German notes of the original WEGA sources stay as they are.

Last checked: 2026-10-17. Verification (from `WEGA/host`):
`make check` in host/ (cc: 74 command lines identical; popt: 522 corpus runs
identical; libc: 170 identical, 7 different, 3 without C source; oldcc: all
ok) and `make verify-wega` (ar identical; size and strip: own code only).

## Differences and workarounds

Workaround `oldcc/...s` means: the file in `src/lib/libc/oldcc/` is the cas
assembler source of the original object; `cas -u` on it gives the
byte-identical member. Such a `.s` is only used where the C source cannot
produce the same bytes with the shipped cc.

| Source | Object | Status | Cause | Workaround |
|---|---|---|---|---|
| `lib/libc/gen/malloc.c` | malloc.o | differs | Older cc: calloc uses `mult rr2,r6` (register still known) instead of a memory operand | `lib/libc/oldcc/malloc.s` |
| `lib/libc/gen/nlist.c` | nlist.o | differs | Older cc: stack frame 2 bytes smaller | `lib/libc/oldcc/nlist.s` |
| `lib/libc/gen/length.c` | length.o | differs | Older cc: `ldb rl3,@r2` / `testb rl3`, frame 2 bytes smaller | `lib/libc/oldcc/length.s` |
| `lib/libc/gen/swap.c` | swap.o | differs | Older cc: `0(r15)` form for the spill of an address-taken argument | `lib/libc/oldcc/swap.s` |
| `lib/libc/stdio/doscan.c` | doscan.o | differs | Different revision (Nov 1983), floating point, calls `_inflt`; the source is the older version | `lib/libc/oldcc/doscan.s` |
| `lib/libc/stdio/printf.c` | printf.o | differs | Different revision (Nov 1983): printf/fprintf/sprintf via `_pfile`, `_flsbuf`, new `_doprnt` | `lib/libc/oldcc/printf.s` |
| `lib/libc/stdio/scanf.c` | scanf.o | differs | Different revision (Nov 1983), floating point | `lib/libc/oldcc/scanf.s` |
| (none in src/) | cnvt.o | no C source | C source not reconstructed | `lib/libc/oldcc/cnvt.s` |
| (none in src/) | fltioc.o | no C source | C source not reconstructed | `lib/libc/oldcc/fltioc.s` |
| (none in src/) | symscan.o | no C source | C source not reconstructed | `lib/libc/oldcc/symscan.s` |
| `lib/oldlibc/doprnt.c` | - | not built | Structural reconstruction of the Jan 1983 `_doprnt`, untested | - |
| `cmd/ar.c` + `lib/libc/crt/crt0-ar.s` | ar | identical (built with the WEGA cc) | - | `crt0-ar.s` is the start-up variant of this binary |
| `cmd/size.c` + `lib/libc/crt/crt0-size.s` | size | own code identical, file differs | The libc part of the original is an older libc revision | none; verify compares own code only |
| `cmd/strip.c` + `lib/libc/crt/crt0-strip.s` | strip | own code identical, file differs | Same as size | none; verify compares own code only |
| `cmd/ranlib.c` | ranlib | builds with the WEGA cc, not identical, unfinished | Needs the older cc | none yet |

## Not yet checked

Old status 100% in the earlier `cmd/TODO`, sources still flat in `cmd/`
(to be moved to `cmd/<name>/` with the layout step, then checked):
chkdiff, format, getud, pburn, putud, sync, tar, uuclear, verify, whodo,
dump, dumpdir, restor.

## Layout still open

- `cc` and `popt` are Linux-side tools and live in `host/` (not WEGA sources).

- `cmd/copydisk`, `dump`, `dumpdir`, `format`, `getud`, `putud`, `restor`,
  `sync`, `tar`, `verify` are still flat in `cmd/` (no own directory, no
  check yet).
- `cmd/standalone` has its own Makefile with hard-coded `cc`/`ld`; it is not
  part of `make` in `cmd/` yet.
- `lib/libc` has no Makefile of its own yet.

## Original programs without source in src/

From the earlier `cmd/TODO`; the old percentages are dropped, only the list is kept.

**bin/** (93): adb, as, bc, bcmp, bd, cas, cat, chgrp, chkin, chkout, chkwhat, chmod, chown, cmp, cp, csh, date, dc, dd, df, diff, dog, du, echo, echo2, ed, edit, egrep, env, ex, file, find, getfile, gets, gpasswd, grep, hd, kill, ld, ln, local, login, ls, mail, make, mkdir, more, mv, newgrp, nice, nl, nm, nohup, od, page, passwd, pr, printenv, ps, pstat, putfile, pwd, regcmp, remote, rm, rmail, rmdir, rsh, scc, sh, sld, sleep, sort, stty, su, sum, tail, tee, test, time, touch, tr, uname, uucp, uulog, uuname, uusend, uux, vi, view, whereis, who, write

**etc/** (52): GETTY, INIT, UGETT1, UGETTY, adduser, chmog, chog, chroot, clri, cron, datem, dcheck, devnm, down, dqueuer, fsck, fsdb, getty, halt, icheck, init, install, kdb, killall, labelit, link, login, makelost+found, makewhatis, man.dbmgr, mfs, mkfs, mknod, mlf, mount, mvdir, ncheck, new.install, rc, rc_csh, rmuser, setlp, setmnt, shut, sysgen, ttyconfig, umfs, umount, unlink, update, upkeep, wall

**usr/** (137): bin/LOAD, bin/SEND, bin/acctcom, bin/admin, bin/apropos, bin/at, bin/awk, bin/banner, bin/basename, bin/bcheck, bin/bdiff, bin/cal, bin/calendar, bin/cb, bin/cdc, bin/checkcw, bin/checkeq, bin/code, bin/col, bin/comb, bin/comm, bin/cpio, bin/cref, bin/crypt, bin/csplit, bin/ct, bin/ctags, bin/cu, bin/cut, bin/cw, bin/cxref, bin/daytime, bin/delta, bin/deroff, bin/dfilter, bin/diff3, bin/diffmk, bin/dircmp, bin/dirname, bin/eqn, bin/error, bin/expand, bin/expr, bin/false, bin/fgrep, bin/flow, bin/get, bin/getNAME, bin/getopt, bin/grpck, bin/head, bin/help, bin/hyphen, bin/id, bin/join, bin/lex, bin/line, bin/lint, bin/logname, bin/look, bin/lorder, bin/lpr, bin/m4, bin/man, bin/mesg, bin/mkmenu, bin/mkstr, bin/mm, bin/mmchek, bin/mmt, bin/mvt, bin/neqn, bin/news, bin/nq, bin/nroff, bin/nroffv7, bin/objdu, bin/objhdr, bin/objsu, bin/pack, bin/paste, bin/pcat, bin/plz, bin/plzcg, bin/plzsys, bin/prof, bin/prs, bin/ptx, bin/pwck, bin/quot, bin/ranlib, bin/reset, bin/rmchg, bin/rmdel, bin/sact, bin/sccsdiff, bin/script, bin/sdiff, bin/sed, bin/spell, bin/spellin, bin/spellout, bin/split, bin/sprof, bin/str, bin/strings, bin/strip, bin/strprint, bin/tabs, bin/talk, bin/tbl, bin/timex, bin/troff, bin/troffv7, bin/true, bin/tsort, bin/tty, bin/uimage, bin/unget, bin/uniq, bin/units, bin/unpack, bin/users, bin/val, bin/vls, bin/vnews, bin/wc, bin/what, bin/whatis, bin/whoami, bin/whois, bin/xargs, bin/xq, bin/xstr, bin/yacc, bin/zmenu, bin/zmprint
