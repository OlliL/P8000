# libscreen against its source

No source is known: not in System III, 2.11BSD, 4.4BSD-Lite, the unix-history-repo, the local backups (S8000, P8000 disks:
only the binaries vls, vnews and the help files) or on the web (searched 2026-11-18). `/usr/include/screen.h` describes it
as the "screen-oriented" interface (Design Objectives and Requirements); vls and vnews use it. The four sources are written
from the members of `/usr/lib/libscreen.a` and `slibscreen.a` (Library.SC 01/30/83, "87wega3.2"); v1 (ccv1, sccv1) with -O,
curses.h and screen.h of WEGA, the V7 stdio.h. Measured 2026-11-18.

Forms the objects fix (the rest is plain C):
- `scrn.c`: `wmove(win, win->_maxy - 1, strlen(pgmsg))` (the length is a temporary, not a variable); `wmesg(win, msg,
  (char *)0)` (a long 0 in slibscreen); `whelp` leaves through `goto bad` to the "Cannot open %s" block at its end.
- `mesg.c`: `wmesg(win, s, d)` with `char *d` (a long in slibscreen); does not include screen.h (no commons).
- `mvcursor.c`: `whighlight` with `if (flag) waddch(win, *p); else waddch(win, *p & ~0200);`; in `wmvcursor` the DOWN
  branch moves one line first and then loops, `wforword` tests the bottom line first.
