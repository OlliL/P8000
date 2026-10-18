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

    z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]

| option | meaning |
|---|---|
| `-r ROOT` | WEGA tree. Absolute guest paths (`/bin/ar`, `/lib/cpp`, `/usr/include/...`) are looked up below `ROOT`. `/dev/*` is passed to the host. `ROOT/tmp` is created and serves as guest `/tmp`. |
| `-t` | trace every instruction (address, opcode word, r0-r7, SP, flags) on stderr |
| `-v` | report lazily allocated memory segments |
| `-e NAME=VALUE` | add a variable to the guest environment (default: `PATH=/bin:/usr/bin`, `HOME=/`) |

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

## System calls

`sc #n` uses the numbers of `sys/sysent.c`. Arguments are read from r0, r1, ...
(16-bit value: one register, 32-bit value: two, pointer: one or two in
segmented mode); results are returned in r4 (r5), carry set and errno in r4 on
error. Implemented: exit, fork, read, write, open, close, wait, creat, link,
unlink, exec, exece, chdir, time, chmod, chown (no-op), break, ssgbrk, stat,
fstat, fcntl, lseek, getpid, getuid/getgid, alarm, pause, utime, access, nice
(no-op), sync, kill, dup/dup2, pipe, times, signal (SIG_DFL/SIG_IGN only),
ioctl (always ENOTTY), umask. Anything else prints
`unimplemented system call N` and fails with EINVAL.
Details: `../KNOWLEDGE.md`, section "Oracle: z8k-emu".

## Limitations

- EPU (floating point) arithmetic is not emulated; only loads and stores of
  EPU registers (as plain data moves, enough for varargs prologues).
  Consequence: `cparse` cannot compile floating constants (blocks `doscopy`).
- Guest signal handlers are not run.
- Some flags that nothing in the tool chain evaluates are approximate (V after
  shifts/rotates, C/V after `div`, DA/H except after byte add/sub).
- Files: sources (`z8k.c` CPU core, `wega.c` loader and system calls, `z8k.h`).
