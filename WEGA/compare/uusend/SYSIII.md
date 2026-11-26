# uusend against System III and BSD

System III has no uusend. `/bin/uusend` of WEGA (no SCCS line, built by the shipped cc with `/lib/libc.a`) is the
uusend of 2.11BSD (`usr.bin/uucp/uusend.c`, the same program as 4.2BSD's) with RECOVER, without DEBUG and RUUSEND, as
WEGA changed it. Measured 2026-11-25.

## WEGA's own

- The usage line names `-fname`; the uux command is `uux %s - "%s!uusend %s -m %o - (%s)"` (no `-gn -z`).
- UULIB `/usr/lib/uucp`, UUPUB `/usr/spool/uucp/uucppublic/`.
- A directory as source is refused: `no such file as %s`, exit 6.
- A directory as local destination: the name is appended to `dnbuf` (BSD: to `destname`, the same buffer there).
- `makedir(dirname)` without the mode: main calls `chmod(destname, 0777)` after it; in makedir the default case
  comes first and a `return` stands behind `exit(1)`.
- `long count = 0` in the declaration.
