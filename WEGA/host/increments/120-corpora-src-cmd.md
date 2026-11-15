# 120 Corpora for every program and script of src/cmd

## Status

open (after 110)

## Goal

Every program and script that `src/cmd` builds or installs is tested against the original on a corpus, so that every later
change (a port to the host, a fix the owner allows) is checked against an expectation.

## Done when

- Every installed file of `src/cmd` that is a program or a script has a corpus in `compare/NAME` (or is listed here with the
  reason why not, e.g. needs real hardware).
- checkall runs them; ALL OK within the budget decided in 110.

## Read first

- This increment's design from 110 (`host/increments/110-corpus-architecture.md`, then `host/HOWTO_reconstruct.md` step 9).
- `src/COVERAGE.md` (what `src/cmd` installs, status exact).

## Known

- Ground rule (owner 2026-11-15): each corpus compares the original of `wega31_root` with our build by the shipped cc/scc and
  libc (`make shipped`); host candidates only for the C compiler chain.
- A model of the emulator (terminal, a device such as `/dev/ud`, user, a second file system, guest signal handlers) is built
  when the first program needs it, together with that program's corpus (owner 2026-11-15), with tests of its own; the
  inventory of 110 says which program needs which.
- A corpus is added only when its expectations show that the original really does what its cases are for (110, Design 9).

- Inventory (110 step 5, 2026-11-15; by grep of the sources, a first hint, to be confirmed per program): what a program uses of
  its surroundings. Markers: terminal (ioctl, stty/gtty, isatty, ttyname; scripts: stty, tty, read), user (uid calls,
  getpw*, utmp; scripts: passwd, chown, su), device (a path in /dev), time (time, ctime, utime; scripts: date, touch),
  signal (signal, kill, alarm; scripts: kill, trap), fs (mount, ustat; scripts: mount, mkfs, fsck), exec (fork, exec),
  processes (scripts: ps, who, utmp). The emulator serves today: argv, environment, files below ROOT and the working
  directory, fork/exec/wait, pipes, a fixed time on request (Z8K_TIME); not: a terminal, devices, other users, a second file
  system, guest signal handlers.

  | Program (src/cmd) | Uses | Program (src/cmd) | Uses |
  |---|---|---|---|
  | ar, size, strip, ranlib | (corpus exists) | cc, scc, cpp, cparse, codgen, popt, cas, ld, sld, gld | (corpus exists) |
  | sync, rmdir | nothing | rm | terminal (-i), exec |
  | mv (cp, ln) | terminal (prompt), user | dc | signal, exec |
  | tar | terminal, user, device, time, signal, exec | write | terminal, user, device, signal, exec |
  | dump, restor | terminal, device, time, signal | dumpdir | terminal, device, time |
  | getud, putud | device, signal | copydisk, format, verify | terminal, device, signal |
  | doscopy | device, time, signal | pburn | device |
  | BOOT0, standalone | the bare machine (no WEGA system): MAME (180), not a corpus | | |
  | chkdiff, uuclear, down, makelost+found, makewhatis, mvdir | nothing marked | halt | time, signal, processes |
  | adduser, rmuser | user, device, processes (rmuser: signal) | install, man.dbmgr, sysgen, UGETT1 | device (install: user) |
  | mfs, new.install | device, fs | killall, whodo | terminal, time, signal, processes (killall: device) |

  So the first batch needs no model (sync, rmdir, rm without -i, mv, dc, the scripts without a marker); devices are the
  largest group (a file standing in for the device is the likely first model: getud/putud, dump/restor, tar f).

- Today corpora exist for the tool chain only (cc, cpp, cparse, codgen, popt and their segmented twins, cas, ld, sld, gld,
  ar, ranlib, strip, size, cchain, scchain).

## Steps

1. The list of programs and scripts (from COVERAGE: source in `src/cmd`, status exact), grouped by the kind of input.
2. Per group a corpus pattern; then per program its inputs (System III tests, man pages, own cases for every option).
3. Add them in batches, each batch with checkall; a batch that needs a new model of the emulator builds it first, with
   tests of its own, and records it in `host/z8k-emu/README.md`.

## Decisions

## Not part of it

Programs not yet rebuilt (increment 160 adds their corpus when it rebuilds them).

## Log
