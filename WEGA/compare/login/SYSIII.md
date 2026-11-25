# login against System III

`/bin/login` of WEGA (`login.c 2.3 07/26/83`, the same file as `/etc/login`) is System III's `cmd/login.c` with
changes of WEGA, and a second file (`ttytype.c`, the name is a guess). Plexus' `cmd/login.c` is the System III file.
Measured 2026-11-25.

## Lines

Lines without comments and blanks that also stand in the System III file.

| File | System III file | Equal | Of |
|---|---|---:|---:|
| login.c | login.c | 211 | 278 |
| ttytype.c | - | 0 | 41 |

## WEGA's own

- The prompt is the node name (`uname`) followed by ` login: `, printed with `printf("%s", ...)`; a `-` in front of the
  name (typed or in argv[1]) suppresses the motd (`quiet`, not initialised). A NUL as the first character prints a
  newline and asks again.
- The environment: no `envinit` and no `environ = envinit`; `setenv` writes HOME, SHELL, PATH (`:/bin:/usr/bin:/z/bin`),
  LOGNAME and TERM (`newenv[5][100]`) over the matching entries of the environment that getty passed. TERM comes from
  `/etc/ttytype` (the type of the tty name, `un` if there is none).
- The default shell is `/bin/csh`; the PATH of root (`/bin:/usr/bin:/etc`) is gone, its `if` stays with an empty body.
- The parts System III has commented out are active: the hint on an empty password (`/bin/passwd`), the motd (now with
  `fgets`/`fputs` and a 512 byte buffer, stopped by `catch`) and "You have mail." (`/usr/spool/mail/`).
- `struct termio ttyb` stays, unused.
- `ttytype.c` also has `getcl` (the `cl` string of termcap), which is never called; it links termcap (`tgetent`,
  `tgetstr`) of libtermlib into login.

## Library

libc-v9 (with ld), and `termcap.o` of `libtermlib`. The crt0 is of 07/26/83 00:05:04, the same as in mv and ld.
