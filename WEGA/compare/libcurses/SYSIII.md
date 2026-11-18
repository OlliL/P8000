# libcurses against its source

System III has no curses, and the one of 2.11BSD is the later 4.3BSD curses (tstp, idlok, `_FULLLINE`). WEGA's
`/usr/include/curses.h` is the 4.2BSD one (1.14, 7/4/83) without the `unctrl` lines at the end, and the archives have the
34 members of 4.2BSD less `tstp.o` (ZEUS 3.21 has the same members, SCCS "Zilog Inc" 05/26/84). The source is
`usr/src/usr.lib/libcurses` of 4.2BSD, `initscr.c` and `scroll.c` of 4.1cBSD (`usr/src/usr.lib/libcurses`, tag `BSD-4_1c_2`
of the unix-history-repo, the TUHS trees; the owner chose TUHS 2026-11-18). `/usr/lib/libcurses.a` and `slibcurses.a`
(members dated 02/18/88 and 02/19/88, no Library.SC) are built by the shipped compiler (cc, scc) without -O. Measured
2026-11-18.

- `initscr.c` (4.1cBSD): no `signal(SIGTSTP, tstp)`, `curscr->_clear = TRUE` only (no `_FULLLINE`).
- `scroll.c` (4.1cBSD 1.2): `for (i = 0; i < win->_maxy - 2; i++)`, `sp - temp < win->_maxx`, no `touchwin`.
- `cr_tty.c`: `LINES` at most 48, `COLS` at most 1000 (as 4.1BSD); `SG = tgetnum("sg"); UG = tgetnum("ug");
  if ((SG > 0 || !SO) && (UG <= 0 && US))` (4.1cBSD's form with `> 0`, `<= 0`).
- `getstr.c`: also stops at `'\r'`.
- `newwin.c`, `subwin`: the two messages "returning ERR (1)" and "SUBWIN(...)" to stderr are not under `# ifdef DEBUG`.
- `printw.c`, `scanw.c`, `mvprintw.c`, `mvscanw.c`: no `&args`, `_doprnt`, `_doscan`; up to ten arguments after the format,
  passed on one by one (`char *a1, ..., *a10`) to `_sprintw` (`sprintf` into `char buf[256]`, then `waddstr`) and
  `_sscans` (`wgetstr` into `char buf[100]`, `sscanf`, returns OK). `_sprintw` declares `char *fmt, *a1, a2, ..., a10`.
