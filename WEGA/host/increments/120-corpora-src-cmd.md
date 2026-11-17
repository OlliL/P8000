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
- 2026-11-16: step c for mfs: z8k-emu mount and umount, read only (Z8K_MOUNTS: the image unpacked by wegafs.py, the
  paths below the directory there; the kernel's errors from sys3.c); the runner: Z8K_MOUNTS per case, ROOT_COPY (a copy
  of a file of the tree in the case's layer: /etc/mnttab). Corpus mfs (mfs, umfs; the script at /etc/mfs): mount,
  umount, umount-not-mounted, mount-twice: 4 of 4 identical, a changed copy is found. Found: WEGA's mount warns when the
  name in the super block is not that of the directory; the /etc/mnttab of the shipped tree lists z, tmp, usr as
  mounted; smount of this kernel does not check for the super-user.
- 2026-11-16: corpus sync (manM/sync.M, src/cmd/sync.c): plain, arguments, stdin, user.
- 2026-11-16: the corpora of the other doscopy programs (version 1.0, man page dos.1): dosls (25 cases), dosdir (25),
  doscat (32), dosmkdir (30), dosrm (24), dosrmdir (26), dosformat (25), in `compare/NAME/tests.txt`, the floppy prefix in
  `host/recon/dospre.py` (the disk of doscp: formatted by the original dosformat). New: `host/tools/dcpimg.py` (free
  clusters, FAT chain and entries, attribute, volume label, a full FAT, and `mkhd`: an image with a DCP file system of
  small clusters, as a floppy or behind a partition table of type 1 or 4: the hard disk path and the FAT16 branch of
  dosdev.c); its hash and that of udosfs.py, wegafs.py are now part of the runner key of the store (a change of a tool of
  the host: lines made the stored results stale before). Emulator: `-e` replaces HOME=/ and PATH of its own (`$HOME/.dos`
  could not be set), open knows FCREAT, FTRUNC, FEXCL and the third argument (dosformat writes its image so), with the
  tests `env` and `openflags`. Each expectation was derived from the source before the original's output was looked at;
  what differed is in the table of KNOWLEDGE.md: the time of an entry (2045 for 2048), the year of `.` and `..`,
  `-1` for the first entry of a second listing (bytefree's fdopen), control-Z and the chunk-end CR of doscat, dosrmdir in
  the second cluster of the parent (writes E5 into the data behind the cluster) and its empty check running past the
  cluster, dosformat choosing the format by a substring of the device name. A mutant of each program (a letter of the
  output, the control-Z byte, the month, the read-only test, the dot of the empty check, a FAT byte) is found.
- 2026-11-16: write (20 cases), dump (18), dumpdir (9), restor (18), and mvdir (14; not asked for, kept apart in its own
  commit). write: `/etc/utmp` made by `host/tools/mkutmp.py` in the layer over the tree, the terminals of the other users are
  `device: ttyN image:FILE`, the own one the console; an unopenable terminal (a FIFO nobody reads) gives the 5 second alarm.
  dump, dumpdir, restor: `host/recon/dumppre.py` (a WEGA file system image of `host/tools/wegafs.py` as /dev/rz = rmd4, two
  floppies, /etc/ddate in the layer, a level 0 dump by the original as the prefix of dumpdir and restor); the archive of two
  disks with the operator (`insert disk 2: `); restor r into an image, read back with wegafs.py ls and cat. Found on the way
  (KNOWLEDGE.md, table): the steps of a corpus that name the program under test must be the bare name (`dump`), not the path
  (`/etc/dump` runs the original on both sides); restor x finds nothing (direq's dangling else); dump u appends a new entry
  every time. Mutants of dump, dumpdir, restor, write are found. Not modelled, so no cases: the permissions of a raw device
  for a user (the user cases of dump), and a restor x that extracts (it cannot: see the quirk).
- 2026-11-16: left of the inventory, with the reason: the csh scripts (adduser, down, chkdiff, makelost+found, makewhatis,
  man.dbmgr, UGETT1: the runner starts scripts with /bin/sh only, a csh option is the first step; makewhatis writes into
  the tree below /usr/man), rmuser (passwd on the terminal, find /), halt, killall, whodo (the process table), install,
  new.install, sysgen (floppies and the system), BOOT0 and standalone (MAME, 180). Next by the owner's order.
- 2026-11-16: the shell of a script step is chosen as execvp() of the libc does it (`whichshell`: first byte # is /bin/csh,
  else /bin/sh; the kernel only gives ENOEXEC). Emulator: a directory that is in the layer and in ROOT lists both (csh found no
  `chmog` of /etc for the candidate, the layer's /etc listed only the script). Corpora makelost+found (6 cases) and chkdiff
  (14; versions made by the original chkin and chkout), mutants found. Not an increment of its own: the same 120.
- 2026-11-16: uuclear (7 cases), halt (4), down (4). halt and down need no process table (my earlier note was wrong): halt is
  wall, who, sleep 30, sync and `kill -1 1`; shut only warns the users (shut.M). The cases work with a utmp in the layer, the
  second user's terminal an image file, /etc/motd a copy per case (ROOT_COPY); down is ended by the interrupt (shut waits
  minutes). uuclean removes nothing in any case tried (KNOWLEDGE.md: open whether program or emulator). Emulator: fds 0 to 2 are the
  console only while they were not closed or replaced (csh closes them and opens its directories on them: every command was
  "not found" with a terminal; dup of the console is the console again). The csh scripts left: adduser, man.dbmgr, makewhatis
  (writes /usr/man), UGETT1, sysgen, new.install.
- 2026-11-16: checkall started every corpus at once (about 65 with 2 jobs each on 8 cores, load 35 with two runs); now
  `xargs -P` runs at most CHECK_CORPORA corpora at a time (default half the cores) in the order of `compare/CORPORA`, which is
  sorted by the time of a cold run, the slowest first (the header says how to keep it). A failed run (NAME=y) showed two
  weaknesses of my cases, both fixed: `signal: after N` is sent when every process of the group sleeps, which under load can be
  a moment before the program has reached the place (halt, down now send the signal at a text, and down's t2 is removed: it is
  written a little after the console text); and a case that reads the guest /tmp of the layer found nothing in a checkall
  run, where Z8K_TMPDIR is set (uuclear): a /tmp in the layer now comes first in the emulator. mfs failed there with a
  60 second timeout in a run of load 35 (the pool should make that rare; to be seen in the next run).
- 2026-11-16: makewhatis (7 cases): /usr/man, /tmp and /usr/lib/whatis are directories of the layer (the pages are symbolic links
  to those of the tree, a case replaces some by changed copies): the script writes there and not into the tree. Found: csh's
  glob opens the empty name, which is the current directory in the kernel (nami.c: no component, flag 0): the emulator gave
  ENOENT, every glob of a csh script failed ("No match" was the symptom: 0 words); `hostpath` of "" is now "." (test
  emptypath). Mutant found (the first tab stop of expand). The csh scripts left: adduser, man.dbmgr, UGETT1, sysgen,
  new.install.
- 2026-11-16: man.dbmgr (11 cases): `df -f /dev/tmp` needs /dev/kmem (df reads _Nmount at the address 0x01000032 and counts the free
  blocks itself): `device: kmem image:km.img` with the file from `host/tools/mkkmem.py` is all the model df needs. The layer's
  /tmp is now the whole guest /tmp (the tree has an empty tmp/man that would have shown through and been written to). The
  runner: the work directories of a run no longer count as dependencies (find looks up the path of its working directory, which
  differs by side and run: UNSTABLE for no reason); `explain` names the dependencies that differ. Mutants found (mode, age step,
  limit). The csh scripts left: adduser, UGETT1, sysgen, new.install.
- 2026-11-16: the first checkall with the pool failed in rm and rmdir (UNSTABLE: "cannot remove current directory", at random): the
  emulator gave the guest the low 16 bits of the host's i-node numbers, and two directories of a tree often agree in them on ext4
  (groups of 8192 i-nodes); rmdir compares the i-nodes of "." and of the argument. It is a hash of the number now. This had
  passed by luck in the runs before.
- 2026-11-17: every program and script of src/cmd now has a corpus (not BOOT0, standalone and the old compilers, as agreed): new.install
  (9 cases: images of the file systems, mkfs, labelit, dd, mfs, lost+found, unpack), sysgen (7: /usr/sys is a copy in the layer, the
  kernel and the boot program are built and compared), adduser (15) and rmuser (7) with passwd (the salt of its password is made of
  the process number: the numbers of the guest are fixed now), whodo (5) and killall (6) on the process table of the kernel as the
  original ps reads it (mkproc.py makes /dev/kmem, /dev/mem, /dev/swap; the kill log). On the way: lkdata, whiteouts and the guest's
  working directory in the layer, relative names in /dev, the numbers of the processes by the tree of forks (see host/z8k-emu/README.md).
  Open (for the owner): the arguments of a process in the ps model (ps -ef then shows /etc/cron etc., killall's second group would be
  tested), find's "cannot read" of the last entry (emulator or program?), unpack of packed pages in new.install, the vi question of
  sysgen (answered n), GETTY on a line that is no device waits for ever (UGETT1 cases left out), the pack of the corpora under a
  pool of CHECK_CORPORA.
- 2026-11-17 (later): checkall found, and the owner's "things running since 8:33" confirmed: with fixed guest pids the temporary names
  (/tmp/cc.100.s) are the same in every run, parallel runs sharing a /tmp clashed and cas looped for ever. Now: `PIDS = True` in
  corpus.py (adduser, rmuser, whodo, killall only), every case has a /tmp of its own (rd.gtmp) unless IN_TMP, tmpclash.py keys the names
  by the host directory of the guest /tmp (z8k-emu logs it in "start"), the Z8K_TMPLOG descriptor is >= 500 (the guest's descriptor
  numbers are the host's: a low one changed csh's). z8k-emu: stat/fstat of a directory give its WEGA size (16 bytes per entry), which
  solved find's "cannot read" (checked in MAME: find and unpack are fine there); du, makewhatis and mandbmgr cases follow the real sizes
  (51200-byte files, whiteouts for removed pages). newinstall `packed-pages` checks unpack in the mounted /usr.

