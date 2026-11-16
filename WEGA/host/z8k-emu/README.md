# z8k-emu

A user-mode Z8001/Z8002 emulator that runs the **original WEGA executables**
(cc, scc, cas, ld, sld, ar, ...) unmodified on Linux. WEGA system calls are
mapped onto Linux system calls. It is the reference ("oracle") for the tool
chain reconstruction: run the original tool, run the rebuilt tool, compare the
output byte for byte.

Only user mode is emulated: no MMU, no interrupts, no privileged or I/O
instructions.

## Build

    make            # builds ./z8k-emu (plain C, needs only cc and make)
    make clean

`make all` in the parent directory (`WEGA/host`) builds it as well.

## Prerequisites (not in git)

The emulator needs the original WEGA file tree (bin/, lib/, usr/include/, ...),
e.g. the extracted disk image. Original binaries are copyrighted and are not
part of this repository; see `../KNOWLEDGE.md`, section "Inputs".

## Usage

    z8k-emu [-r ROOT] [-t|-T] [-v] [-e NAME=VALUE] [-0 ARGV0] PROGRAM [ARG...]

| option | meaning |
|---|---|
| `-r ROOT` | WEGA tree. Absolute guest paths (`/bin/ar`, `/lib/cpp`, `/usr/include/...`) are looked up below `ROOT`. `/dev/*` is passed to the host. `ROOT/tmp` is created and serves as guest `/tmp`. |
| `-t` | trace every instruction (address, opcode word, r0-r7, SP, flags) on stderr |
| `-T` | like `-t`, and also every instruction of the floating point emulator (`fpe.s` line, r0-r15, flags) |
| `-v` | report lazily allocated memory segments |
| `Z8K_TMPDIR=DIR` (host environment) | the guest `/tmp` is DIR instead of `ROOT/tmp` (checkall gives each run its own, so that parallel runs do not share it) |
| `Z8K_ROOTLOG=FILE` (host environment) | every guest path looked up below ROOT (not `/tmp`, `/dev`) is appended to FILE, one per line (a relative FILE is taken from the starting directory; opened per line, so the guest never sees its descriptor): the files of the tree a run depends on, measured by the corpus runner |
| `Z8K_DEVTAB=FILE` (host environment) | the guest `/dev` is the table FILE (increment 117): `devtab.wega` (WEGA's /dev from the installation, names, types, majors, minors) and lines `model NAME KIND[:ARG] [excl] [console]`; a directory read of `/dev` lists the table, `stat`/`fstat` give type and numbers, `open` goes to the model: `image:FILE` (a floppy image), `host:PATH` (a host file such as /dev/null), `proc:CMD` (a byte stream to a model program: write sends, read waits for one byte and returns what is there, as the ud driver; close ends the stream and waits for the model), `console`; `excl` allows one open at a time, `console` only with the console as controlling terminal (udopen); no model: ENXIO; a name not in the table: ENOENT. Without it `/dev/...` is the host's. |
| `Z8K_CONSOLE=1` (host environment) | the minimal terminal: fds 0, 1, 2 are `/dev/console` (with `Z8K_DEVTAB`): `isatty`, `ttyname` and the tty ioctls (TIOCGETP/SETP/SETN, TIOCGETC/SETC, TCGETA/TCSETA...) work and keep their settings; the bytes are still the step's stdin and output. Default settings are the model's (B9600, erase #, kill @, ECHO|CRMOD; not measured). |
| `Z8K_TIME=SECONDS` (host environment) | the system call `time` returns this value (reproducible `$D$` of `chkout`; the guest zone is EST) |
| `-e NAME=VALUE` | add a variable to the guest environment (default: `PATH=/bin:/usr/bin`, `HOME=/`) |
| `-0 ARGV0` | the guest gets ARGV0 as `argv[0]` instead of the path of the program (as a shell gives the name typed; mv, cp, ln choose their work by it) |

`PROGRAM` is a guest path (looked up below `ROOT`) or, without `-r`, a host
path. Relative paths in the guest refer to the current host directory. The
exit status of the guest is the exit status of `z8k-emu`; 126 means the
program could not be loaded, 255 an emulation error (illegal or unimplemented
instruction, with a register dump).

Supported formats: non-segmented `E707` (combined I&D) and `E711` (separate
I&D), segmented `E607` and `E611`. Child processes started by the guest
(`fork`/`exec`, as `cc` does for its passes) run inside the emulator as well.

## Examples

Set the tree once:

    R=/path/to/wega-root
    EMU=./z8k-emu

List and inspect:

    $EMU -r $R /bin/ar t /lib/libc.a
    $EMU -r $R /bin/size /bin/ar

Compile with the original compiler (the C file is in the current directory,
the output lands there too):

    $EMU -r $R /bin/cc  -O -c foo.c        # Z8002, non-segmented
    $EMU -r $R /bin/scc -O -c foo.c        # Z8001, segmented
    $EMU -r $R /bin/cc  -S foo.c           # assembler output foo.s
    $EMU -r $R /bin/cc  -O -o foo foo.c    # compile and link
    $EMU -r $R ./foo                       # run the result

Trace a program:

    $EMU -t -r $R ./foo 2>trace.txt

Disassemble the objects and executables with `../tools/sdis.py` (objects) or
`../tools/z8k.py` (decoder library).

## Self test

`selftest.sh` rebuilds original objects with the original compilers and
compares them byte for byte with the objects shipped on the disk image:

    ./selftest.sh WEGA_ROOT UTS_DIR KERNEL_OBJ_DIR
    # or from WEGA/host:
    make check-oracle WEGA=... UTS=... KOBJ=...

`UTS_DIR` is the extracted kernel source tree (`dev/`, `conf/`, `h/`),
`KERNEL_OBJ_DIR` the directory with the kernel LIB2 members.

## Floating point (EPU)

The P8000 has no EPU (Z8070): an EPU instruction (opcodes 0E, 0F, 4E, 4F, 8E,
8F) traps, and the kernel runs its floating point emulator
`src/uts/conf/fpe.s` (`_fpepu`, called from `uniminstr` in `trap.c`). The
emulator does the same with a C port of that file:

- `fpe2c.py` translates `fpe.s` one instruction per C statement into
  `fpe_gen.c` (checked in; `make fpe` regenerates it, needs python3).
- `epu.c` runs it on a second CPU context (the kernel: own registers, one
  64K data segment with `struct state`, `u.u_fpstate` and the kernel stack),
  with the same instruction semantics as the CPU core (`z8k_int.h`). Calls
  push a return number, so the stack tricks of `fpe.s` work unchanged.
  `gettext`, `getmem` and `putmem` (in `mch.s`, not in `fpe.s`) are
  implemented in C; counts are in words, as in `mch.s`.
- The EPU state (`u.u_fpstate`: 8 registers of 80 bits, control registers)
  lives in `Z8K.fps`; exec sets all registers to 1.0 like `setregs()`.
- A trap enabled exception (SIGFPE) goes to the program's handler when it has
  one (the PC then is the next instruction: a guess); otherwise it, and a bad
  operand address (SIGSEG), stop the emulator with a message, like the default
  action of the signal.

Results are those of `fpe.s`, not of IEEE hardware. Checked with random
operands against exact arithmetic: add, sub, div and sqrt are correctly
rounded to 64 bits. `fmul` is not always: when the product of the mantissas
is below 2, `mulno` shifts bit 55 instead of bit 63 into the result and keeps
the unshifted guard byte, so the last bit of the 64 bit result can be off
(about 1 in 5 products; after rounding to double about 1 in 20000).
Defaults (control registers 0): round to nearest, projective infinity (so
inf+inf and sqrt(inf) are invalid and give NaN), no traps.

## System calls

`sc #n` uses the numbers of `sys/sysent.c`. Arguments are read from r0, r1, ...
(16-bit value: one register, 32-bit value: two, pointer: one or two in
segmented mode); results are returned in r4 (r5), carry set and errno in r4 on
error. Implemented: exit, fork, read, write, open, close, wait, creat, link,
unlink, exec, exece, chdir, time, mknod (directories via host `mkdir`),
chmod, chown (no-op), break, ssgbrk, stat,
fstat, fcntl, lseek, getpid, getuid/getgid, alarm, pause, utime, access, nice
(no-op), sync, kill, dup/dup2, pipe, times, signal (handlers run: see Signals),
ioctl (always ENOTTY), umask. Process ids: the guest sees `(hostpid - 1) % 32767 + 1`
(never 0 or negative: a cut host pid of 0 made the parent of a fork take itself for the child) in getpid, fork, wait;
kill maps such a pid back to the host process (itself, a child, the parent, another z8k-emu of the session). Anything else prints
`unimplemented system call N` and fails with EINVAL.
Directory reads are translated to WEGA's fixed-size `struct direct` records,
and directory `lseek` positions are logical byte offsets in those records;
unlinking a directory uses the host `rmdir` (the unlink of DIR/. and DIR/.. is taken as done; the log file of
`Z8K_ROOTLOG` is left out of a directory read); a link to a directory (mv dir1 dir2, as root on WEGA) renames it and
takes the unlink of the old name that follows as done. The guest /tmp of `Z8K_TMPDIR` is a mount: `..` of it (and
of /) is ROOT, and ROOT lists `tmp` with its inode, so that pwd (and tar, which runs it) finds /tmp/NAME for a working
directory below it (without `Z8K_TMPDIR` pwd walks the host tree and prints a wrong path). Details: `../KNOWLEDGE.md`,
section "Oracle: z8k-emu".

## Signals

The model of the kernel (`src/uts/sys/sig.c` psig, `sys4.c` ssig, `machdep.c` sendsig and sysiret), in `wega.c`:

- `signal(sig, f)` (sc 48; segmented: `f` is a long, segment word and offset) stores the handler; 0 is the default, an
  odd value ignore; what was pending of that signal is dropped. The host disposition follows: default and ignore are
  the host's own (the emulator dies by the signal), a handler is a host handler that only marks the signal pending.
- Between two instructions the lowest pending signal is delivered: the PC segment word, the PC and the FCW go on the
  guest stack (stack - 6), the PC is the handler; the handler goes back to the default first (not for ILL, TRAP). The
  libc stub (`signal.s`) saves the registers, calls the function and returns by sc 0 (sysiret: PC and the flags back).
  A system call interrupted by a caught signal (read of a pipe or terminal, pause, wait) ends with EINTR, then the
  handler runs (`trap.c`). The high byte of the FCW on the stack is a guess (no program reads it).
- The numbers of WEGA (System III) are not the host's: kill, the wait status and the host dispositions map them
  (EMT, which the host lacks, is a real-time signal). fork: nothing pending in the child; exec: caught signals go back to
  the default, ignored ones stay (also what the emulator inherits as ignored is ignored for the guest).
- At the start the emulator clears the host signal mask (WEGA has none) and sets the core limit to 1: no host core
  dump (also not by a piped `core_pattern`). The kernel's `core()` (a file `core`, status | 0200) is not modelled.
- Device models (`proc:`) run in a session of their own: a signal to the process group of a step does not reach them.
- Test: `tests/signal.c` (built with cc and scc, compared with `tests/signal.out`; `tests/run.sh`, run by checkall).

## Users

With `Z8K_UID=UID` (and `Z8K_GID`, `Z8K_OWNERS=FILE`) the guest runs as that WEGA user (0: root) whatever the host user
is; without it the guest sees the host's ids and the host decides (as before). The model (`wega.c` um_*), from the
kernel (`src/uts/sys/fio.c` access, owner, suser; `sys4.c` getuid, setuid, getgid, setgid, chmod, chown, utime,
saccess; a new name needs write permission on its directory; mknod and the unlink or link of a directory only for the
super-user):

- The owners: a table host (device, inode) -> uid, gid in `Z8K_OWNERS` (appended to by every process of the run);
  creat and mknod enter the effective ids, chown the new ones. A file without an entry belongs to root (0, 0) below
  ROOT outside the guest /tmp, to `Z8K_UID`, `Z8K_GID` elsewhere (a model choice: the owners of the original tree are
  not known to the emulator). stat and fstat show these owners.
- The permissions: open, creat, access (with the real ids), unlink, link, mknod, chmod, chown, chdir, utime are decided
  by the kernel's rules on the owners of the model and the host mode. The host operation runs as the host user, who
  owns the files; where the host mode is in the way of what the guest may (root writes a file of mode 444), the right
  is given to the host user for the moment and taken back.
- exec: a set-user-id or set-group-id program makes its owner the effective user or group (mkdir: root, then it gives
  the new directory to the real user with chown, as on WEGA).
- Not modelled: the search permission of the directories along a path; exec permission (the host decides).

## Limitations

- No core file when the guest dies by a signal (see Signals).
- Some flags that nothing in the tool chain evaluates are approximate (V after
  shifts/rotates, C/V after `div`, DA/H except after byte add/sub).
- Whether `fpe.s` in the tree is the code of the shipped kernel is not
  checked.
- Files: sources (`z8k.c` CPU core, `wega.c` loader and system calls,
  `epu.c` EPU trap, `fpe_gen.c` generated from `fpe.s` by `fpe2c.py`,
  `z8k.h`, `z8k_int.h` helpers shared by the CPU core and `epu.c`).
