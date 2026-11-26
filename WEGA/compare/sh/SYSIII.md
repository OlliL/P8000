# sh against System III

`/bin/sh` of WEGA (= `/bin/rsh`, one inode; SCCS lines 08/06/83 .. 08/22/83, crt0 08/22/83) is System III's
`cmd/sh` as WEGA changed it. Measured 2026-11-26.

## Lines

Lines without comments and blanks that also stand in the System III file (one line less is the sccsid).

| File | System III file | Equal | Of |
|---|---|---:|---:|
| args.c | args.c | 124 | 129 |
| blok.c | blok.c | 85 | 86 |
| builtin.c | builtin.c | 23 | 24 |
| cmd.c | cmd.c | 315 | 316 |
| ctype.c | ctype.c | 38 | 39 |
| error.c | error.c | 54 | 56 |
| expand.c | expand.c | 204 | 205 |
| fault.c | fault.c | 109 | 110 |
| io.c | io.c | 129 | 130 |
| macro.c | macro.c | 199 | 200 |
| main.c | main.c | 153 | 156 |
| msg.c | msg.c | 128 | 134 |
| name.c | name.c | 311 | 361 |
| print.c | print.c | 72 | 73 |
| profile.c | profile.c | 30 | 31 |
| service.c | service.c | 337 | 389 |
| setbrk.c | setbrk.c | 8 | 9 |
| stak.c | stak.c | 57 | 58 |
| string.c | string.c | 37 | 38 |
| test.c | test.c | 139 | 140 |
| word.c | word.c | 120 | 121 |
| xec.c | xec.c | 393 | 397 |

## WEGA's own

- `setjmp`/`longjmp` are `setret`/`longret` (`ret_buf subshell, errshell`); `end` is `extern address end[]`.
- args.c: `loginsh` (-1 when argv[0] starts with `-`, else 0; set, never read in the shell); `copyargs` is global and
  stands behind `useargs`.
- name.c, xec.c, msg.c: the builtin `unset NAME` (the name leaves the binary tree of names: `unsetnam`, `rmnode`,
  `rplnode`, guessed names; "not in environment", "Node may not be removed" for the first three levels) and the empty
  builtin `ESC B` (as in csh: what a terminal key sends); `SYSESC` 19, `SYSUNSET` 21.
- service.c: on ENOEXEC `execscr` runs a script by the shell its first line names (`#!/path`, `#!sh`, `#!csh`; `#`
  alone csh, no `#` sh); a non-ASCII byte is "invalid shell script", another `#!` "Cannot determine type of shell to
  use."; the arguments are shifted by one in `dolv` and `execve` is called with the shell; only when the script cannot
  be opened does the old way (the shell reads the script itself) follow.
- msg.c: "not a login shell" and "no more processes, waiting for current ones to complete" (never used).

## Build

ccv1 with split I and D, the objects in alphabetical order with test.c last (as sh.mk), no `:fix` of ctype.c and msg.c.
service.c (08/22/83) was made by a newer compiler (register slots and padded strings between v1 and the shipped
compiler, `host/OLDCC.md`); it is not built, so service.c and the data behind it differ (phase B).

## Library

libc-v9 (ld, login), extended for sh (owner 2026-11-26) by atol 0_1 (`libc/common/gen/atol.c` by ccv1: one register
slot more than the atol of `../libc`), fakcu behind it, and setret, dup, execve, fcntl, fork, geteuid, pipe, profil,
times, wait of `../libc` at the places of sh's order; ld and login stay identical. The libc part of the build equals
the original's instruction by instruction; it starts 10 bytes later (initio, postclr, getch).
