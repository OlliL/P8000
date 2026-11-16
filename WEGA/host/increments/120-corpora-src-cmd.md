# 120 Corpora for every program and script of src/cmd

## Status

active (2026-11-16)

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
- 2026-11-16: dc: `compare/dc/corpus.py`, the cases in `compare/dc/tests.txt` (47: every command of the man page, the
  example, the diagnostics; from the source Y, Q, K, !< !> !=, x of a number, p of a string, the output bases above 16 and
  1/0/-1, the registers of 0241 and above, the limits, the syntax of numbers, the line continuation, the order of ! output).
  Every case has an expectation written from the man page or the source; 17 of my first expectations were wrong and each
  was explained by the source before it was changed (e.g. 10i in base 2 is 2, k stays set, the remainder of % at scale k).
  Bugs of the original: the endless loop on a second point (a case, ended by `TIMEOUT`), and three of memory behind arrays
  (not cases: their result depends on the layout); `host/KNOWLEDGE.md`. Two mutants of the shipped dc (line length 69, a
  digit format) are found. Runner: `expect: output is`, `TIMEOUT` of a corpus. 47 of 47 identical.
- 2026-11-16: rm (21 cases), rmdir (13), mv with cp and ln (41; one program under three names: `ALIASES` of the corpus,
  argv[0] = the word of the step, z8k-emu `-0`), tar (34). The cases in `compare/NAME/tests.txt`, read by
  `host/recon/casefile.py` (the format of dc, now shared). Each case's original output was read against the source before
  it was kept; a mutant of each program (rm: Y no longer yes; rmdir: exit 1; mv: no chmod of a new copy; tar: "A" for
  "a") is found. Emulator models, built with the first program that needs them: a link to a directory is a rename (mv dir1
  dir2); the guest /tmp of `Z8K_TMPDIR` is a mount (pwd, which tar runs, finds /tmp/CASE); the log of `Z8K_ROOTLOG` is left
  out of directory reads. Runner: the candidate lies beside the case directory (rm -i alone lists its directory), every
  side runs in a directory of the same name (tar writes leftover memory holding it into an archive), `IN_TMP`, `TIMEOUT`,
  `same-as` of a file of the run. Not cases, until their model exists: what needs a terminal (rm -i questions about modes,
  mv's question), devices (tar's default device, a device as a name), signals. Bugs of the originals:
  `host/KNOWLEDGE.md`, "Bugs and quirks".
- 2026-11-16: the first checkall of the batch failed twice, both on my side: the ".." of the /tmp model had first been
  resolved by name for every path (dir/../s.a with dir a file then worked, where the host and the WEGA kernel say no:
  ar's host candidate differed); now only a ".." that leaves the guest /tmp or would go above ROOT is the model's. And a
  file of a tar case was made without the fixed time (its time is in the archive: equal only when both sides ran in the
  same second).
- 2026-11-16: the programs with devices wait for 117 (owner): getud, putud, pburn first, then format, verify, copydisk,
  doscopy, tar on a floppy.
- 2026-11-16: 117 done: corpora putud, getud, pburn, format, verify, copydisk, doscp exist (the device fields
  `device:`, `terminal:`, `time:`); left with devices: dosformat, doscat, dosls, dosdir, dosmkdir, dosrm, dosrmdir (the
  pattern: compare/doscp), tar on a floppy.
- 2026-11-16: the signal model of the emulator (owner): handlers of the guest run (host/z8k-emu/README.md, "Signals";
  own test host/z8k-emu/tests, run by checkall); the case field `signal: SIG after SECONDS | at 'TEXT'` (to the process
  group of the step); dc has the cases interrupt and sigquit. Left of the surroundings: user, fs, processes, a terminal
  that acts on its settings; a core file is not modelled.
- 2026-11-16: signal cases in every existing corpus whose source handles signals (owner): ar, cas, ld, sld, cparse,
  scparse, cc, scc, strip (cases/signal), tar, getud, putud, pburn (tests.txt), dc (before). The program waits at a
  fixed point (a FIFO in its open, stdin left open, getud's endless wait, the EPROM card busy: `busy=S`) and gets the
  signal there. Not a case: strip's window with ignored signals (pure file copies; strip seeks on its input, a FIFO
  cannot hold it there). Without signal handling in their sources (no cases): rm, rmdir, mv/cp/ln, cpp, codgen, popt,
  format, verify, copydisk, doscp; gld, casold, cchain, scchain have the host candidate only and share the code of ld,
  cas, cc (their signal cases are in those corpora). The runner: "files afterwards" lists a FIFO without reading it;
  the steps start with no signal blocked.
- 2026-11-16: the user model of the emulator (owner: first with the corpus of widest use, tar): `user: UID GID`
  (z8k-emu Z8K_UID, host/z8k-emu/README.md "Users"); tar has the cases user-ids, user-root-extract,
  user-extract-gives-away, user-read-only, user-root-read-only. Left for the model: mv (setuid(getuid()), access),
  rm, rmdir (access: root may write), ar (the uid in the archive); mv's candidate lies outside ROOT, so its set-user-id
  bit would not make it root as /bin/mv is (to be solved with mv).
- 2026-11-16: the user cases of mv (cp, ln), rm, rmdir, ar (owner): mv's question and root's overwrite, mvdir as
  set-user-id root, cp's creat, ln as root; rm's question on the terminal, rm -r, unlink in an unwritable directory;
  rmdir set-user-id root with access of the parent; ar's uid/gid in the member header (chars: 200 is -56, a quirk).
  The runner: the candidate belongs to root (as the original in the tree); a native candidate does not run user cases.
  The emulator: set-id also for the first program of a step, link and the rename of a directory with the rights given
  for the moment.
- 2026-11-16: the floppy for tar (owner): z8k-emu answers ioctl SHOWFD for the floppy devices (fd.c fdblocks); the
  runner makes every drive a link to the disk in it and has the field `operator: [at 'TEXT'] [insert driveN FILE]
  [type 'TEXT']` (the disk changed, the answer typed when the program asks). tar: disks-write-read (an archive over two
  disks of 360 blocks: c, t, x), disks-capacity, disks-default-device (/dev/tardev, the keys 0 and 1).
- 2026-11-16: for the scripts and mfs (owner, step a of three: a layer over ROOT and script candidates; b: wegafs.py
  format/put; c: mount/umount read-only, then the corpus of mfs): z8k-emu Z8K_OVERLAY (a layer over ROOT), the runner
  gives every case one; corpus option INSTALLED (the candidate at its place in the tree, ALIASES too); a step whose
  program is a shell script runs it with /bin/sh ($0 the path). Checked with a corpus made for it (mfs.sh at /etc/mfs,
  /etc/umfs; a changed copy is found). Measured on the way: WEGA's mount keeps its table in /etc/mnttab (in the tree; /etc/mtab is not used).
- 2026-11-16: step b for mfs: host/tools/wegafs.py format (byte-identical with WEGA's /etc/mkfs for 100, 360, 720, 1440,
  2880, 5000, 20000, 65000 blocks; mkfs measured under z8k-emu: BLOCKS/25 i-node blocks, m/n 1 72, s_tfree one too high,
  a quirk), put and mkdir (direct, single and double indirect blocks; WEGA's /etc/fsck passes the result). Next: c.
