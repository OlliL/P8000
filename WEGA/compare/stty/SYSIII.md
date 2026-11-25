# stty against System III

`/bin/stty` of WEGA (`stty.c 2.3 12/21/83`, the shipped cc, libc-v15) against `cmd/stty.c` of System III. The code of
System III is unchanged; WEGA changed the tables only. Measured 2026-11-25.

## WEGA's own

- The SCCS line `@[$]stty.c	2.3  12/21/83 12:03:38 - 87wega3.2`.
- The speeds of the WEGA `termio.h` (`B19200` 016, `EXTA` 017, no `EXTB`): the names `19200` in, `50`, `1800`, `extb` out;
  `speed[]` (the numbers that `prspeed` prints) has 19200 after 9600 (the names 50 and 1800 stay there).
- The cflag modes `-raw`, `cooked`: `CS8`, reset `CSIZE|PARENB` (System III `CS7|PARENB`, reset `CSIZE`); `sane`: `CS8`,
  reset `CSIZE|PARENB|PARODD|CLOCAL` (System III `CS7|PARENB|CREAD`): 8 bits, no parity, `CREAD` not set.
- The oflag mode `sane` sets `TAB3` too (tabs expanded): `OPOST|ONLCR|TAB3`.

## The library

The doprnt of stty is a version after the shipped one (11/83): `_doprnt` sets `base` and `val` to 0 after `fpn = 0`. It is
doprnt 0_10 of `src/lib/oldlibc`; libc-v15 is the shipped libc.a with it. No other libc-vN holds the members of stty in their
versions (the printf of the shipped libc.a).
