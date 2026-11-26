# csh against Plexus

System III has no csh. `/bin/csh` of WEGA (SCCS lines 08/10/83, crt0 08/11/83) is Plexus' `cmd/plx/csh` (the Berkeley
csh of 2.7BSD, `csh.mk`) as WEGA changed it; its `printf.c` is not Plexus' but the small printf of WEGA's ls. Measured
2026-11-26.

## Lines

Lines without comments and blanks that also stand in the Plexus file.

| File | Plexus file | Equal | Of |
|---|---|---:|---:|
| alloc.c | alloc.c | 145 | 146 |
| getpwent.c | getpwent.c | 75 | 76 |
| getpwnam.c | getpwnam.c | 79 | 80 |
| getpwuid.c | getpwuid.c | 69 | 70 |
| printf.c | printf.c | 19 | 230 |
| sh.c | sh.c | 437 | 473 |
| sh.dol.c | sh.dol.c | 523 | 524 |
| sh.err.c | sh.err.c | 71 | 72 |
| sh.exec.c | sh.exec.c | 225 | 258 |
| sh.exp.c | sh.exp.c | 479 | 481 |
| sh.func.c | sh.func.c | 682 | 753 |
| sh.glob.c | sh.glob.c | 629 | 633 |
| sh.hist.c | sh.hist.c | 74 | 75 |
| sh.init.c | sh.init.c | 169 | 179 |
| sh.lex.c | sh.lex.c | 1074 | 1075 |
| sh.misc.c | sh.misc.c | 273 | 274 |
| sh.parse.c | sh.parse.c | 517 | 519 |
| sh.print.c | sh.print.c | 83 | 86 |
| sh.sem.c | sh.sem.c | 300 | 308 |
| sh.set.c | sh.set.c | 478 | 480 |
| sh.wait.c | sh.wait.c | 305 | 323 |

(The one line less of most files is the sccsid.)

## WEGA's own

- Build: `ccv1 -O -DSYS3` without `-DTELL` and without `-DBUFSIZ=1024` (BUFSIZ 512, btell instead of settell), no xstr,
  split I and D, the objects in alphabetical order; no `doprnt.c`, `pprintf.c`, `dup2.c`. `setjmp` is `setret`/`longret`
  (`reset()` calls `longret(reslab)` with one argument). The shell is `/bin/csh`, the search path `. /usr/ucb /bin /usr/bin`.
- `sh.c` main: every environment variable becomes a shell variable with its name in lower case; `prompt` and a flag for
  "interruptible" (instead of writing `-` into argv[0]) are globals; a login shell reads `/etc/cshprofile` before
  `~/.cshrc`, then `~/.login`; process prompts only when `prompt` is set.
- `sh.exec.c` texec: a script without a shell alias is run by the shell its first line names (`#!/path`, `#!sh`,
  `#!csh`; `#` alone csh, no `#` sh), a non-ASCII script and an unknown `#!` are errors.
- `sh.func.c`: echo with `-n` and the escapes of System V (also behind ESC), flush at the end; `unsetenv` (4.1BSD);
  `exit` refused while `ignoreexit` is set; `login` also tries `/usr/bin/login`; `newgrp` only in a login shell;
  `search` and `getword` know `if(` and `while(` (a word ends at `(` and `)`); an empty builtin `ESC B`.
- `sh.glob.c`: matchdir opens `gpath` as it is (no `.` for an empty path); one more slot in `agargv` and `apargv`.
- `sh.sem.c`: `source` with an output redirection is an error; `nice` as donice; fork failing waits for the children.
- `sh.set.c` exportpath: a `:` behind every directory.
- `sh.wait.c`: `time -v` (the long words, " real-time,"), the percentage as 5/3; `nice` without argument 7, only `-N`;
  `times0` and the new `realtime` initialised (data, not bss).
- `getpwent.c`: no `pw_quota`; `pw_comment` stays.

## Library

libc-v9 (ld, login, sh), extended for csh (owner 2026-11-26) by perror 0_2, errlst 0_2 (packed data), abort, dup2,
_exit, getgid and getuid of `../libc` at the places of csh's order (v8 and v9 are the variants without a member of
another version; v9 is the nearest in time); ld and login stay identical. The libc code of the build equals the
original's instruction by instruction; its data differs only in code addresses (the own code is 8 bytes shorter:
main, Dword, execute).
