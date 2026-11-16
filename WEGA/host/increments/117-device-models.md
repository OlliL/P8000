# 117 Device models: /dev/ud (the 8-bit part), floppies, the EPROM programmer

## Status

open (before 120 goes on; owner 2026-11-16)

## Goal

The emulator serves the devices through which the WEGA programs reach the 8-bit part of the P8000: the coupling channel
`/dev/ud` with every request of the coupling software (UDOS files, floppy blocks, format, verify, copydisk, EPROM
programmer), and the floppy devices `/dev/fd*`, `/dev/rfd*` as images. After this increment the corpora of getud, putud,
pburn, format, verify, copydisk, doscopy and tar on a floppy are pure case work: nobody needs to change the emulator for
them (owner 2026-11-16: they are to be written by a simpler AI).

## Done when

- z8k-emu has a device layer: a guest path below `/dev` is a model, not the host's `/dev`; `stat`/`fstat` of it give the
  type (character or block), major and minor as on WEGA; `open`, `read`, `write`, `lseek`, `close`, `ioctl` go to the model.
  A device without a model is an error (`ENXIO`), not the host device.
- `/dev/fd*`, `/dev/rfd*` (every name and minor of WEGA: drive 0/1, 48/96 tpi, single/double side, 8/9 sectors) read and
  write a floppy image chosen per run; images in the format of `tmp/wega31_disks` (RAW, 737280 bytes for 96ds9).
- `/dev/ud` serves every request of the coupling software (`UDOS/WEGA/koppeldoc`, the 8-bit sources `UDOS/WEGA/K.*.S`)
  with the bytes, acknowledgements and error codes the sources give: `L` (getud), `S` (putud), `R`, `W` (floppy blocks),
  `F`, `V`, `C`, and `P` (EPROM programmer: board query, read, write with read-back verify, erase check, CRC) with a
  virtual EPROM programmer: board 1 or 2, an EPROM per type of the man page (pburn(1) of 2.1), its contents as a file;
  programming behaves as an EPROM does (a 0 bit cannot become 1 without erasing); erasing (UV light) is an action of the
  case.
- The case format gives the reserved field `device:` its meaning (the image of a drive, the EPROM in the socket and its
  board, the state after the run kept as a file of the case directory and compared); runner and emulator pass it on;
  documented in `compare/README.md`, `compare/README.de.md` and `host/z8k-emu/README.md`.
- Corpora: getud and putud complete (every option, the round trip, every error path the sources have), pburn complete
  (every menu function, every EPROM type of both boards, the file formats and options of the man page, the error
  messages); a few cases each for format, verify, copydisk and doscopy as proof that the models carry them. checkall
  ALL OK.

## Read first

- `UDOS/WEGA/koppeldoc` (beside `WEGA/`): the coupling software, the requests (section 1.2 b), the modules.
- `UDOS/WEGA/K.ACT.S` (UDOS files: the remote telegram of getud/putud), `K.NEU.S` (the request dispatch, `R W F V C S`),
  `K.BLK.S` (floppy blocks), `K.PROM.S` and `K.P8000.S` (the EPROM programmer).
- `src/uts/dev/ud.c`, `src/uts/dev/koppel.s` (the WEGA driver of `/dev/ud`), the floppy driver in `src/uts/dev`,
  `src/uts/conf/conf.c` (majors).
- `src/cmd/getud.c`, `src/cmd/putud.c`, `src/cmd/pburn/2.1/*.c` and `pburn.1` (2.1 is the version the build makes and
  installs: `PBURN_VERSION` in `src/cmd/pburn/Makefile`; `src/man/man1/pburn.1` is its man page), `src/cmd/format/1.6/format.c` (the WEGA
  side of each request).
- `host/z8k-emu/wega.c` (`hostpath`: `/dev/` goes to the host today; open, read, write, stat).
- `compare/README.md` (the corpus), `host/increments/110-corpus-architecture.md` (Design 7: the surroundings of a case).

## Known

- `/dev/ud` is a byte channel to the 8-bit part (U880 with its own firmware and the coupling software); the 16-bit side
  writes a request letter and its parameters and reads acknowledgements and data. The requests (koppeldoc 1.2 b): `L`
  LOAD (receive a UDOS file: getud), `S` SEND (send a UDOS file: putud), `P` PROM, `R` READ and `W` WRITE (floppy blocks:
  the kernel's floppy driver), `F` FORMAT, `V` VERIFY, `C` COPYDISK; "further requests are not implemented".
- getud/putud: the remote telegram (K.ACT.S): request, flag byte (binary, exec, all-binary), the file name with an xor
  checksum, blocks of at most 64 bytes with SOH/STX/ETX, checksum, ACK/NAK, three retries, CAN; UDOS error codes such as
  46H "invalid file name" come from K.ACT.S.
- format, verify, copydisk send `F`/`V`/`C` and the minor number of the floppy device (`stat(...).st_rdev`, major
  `FLOPPY` = 1), then read ACK or an error code (e.g. C2H drive not ready); they ask before working only when
  `ttyname(0)` is `/dev/console`.
- pburn: request `P`, then read/write (1 read, 2 write, 3 erase check, 4 CRC, 5 board query), EPROM type, start address,
  length, Vpp, program mode (2.1/pb.devud.c `header`; K.PROM.S `prom`); write: data in blocks of 64 bytes, each block
  acknowledged with `q`, read-back compare in standard speed mode (error 3); erase check: every byte FFH; CRC over the
  range. Board 1: e2708 e2716 e2732 e2732a e2764; board 2: e2716 e2732 e2732a e2764 e2764a e27128 e27128a e27256
  e27512 (pburn.1). pburn is menu driven on the standard input.
- pburn 2.1 (the target) and 2.6 (in the tree beside it, not built) send the same requests (`header` with 1 to 5, the
  same EPROM types); they differ in a warning after a failed read-back (2.6: warning 16 in standard speed mode) and in
  the byte order of word-oriented files (`-w`: 2.1 low byte, high byte; 2.6 high byte, low byte). The coupling
  software's K.PROM.S is version 2.1 (18.4.1989). The corpus tests 2.1.
- doscopy (8 programs) reads and writes the floppy device directly (`read`, `lseek`, `write`; `/etc/default/dos`); tar
  `f /dev/rfd1` likewise.
- The extracted WEGA tree has an empty `/dev`: names, majors and minors must come from the sources (conf.c, the drivers,
  the installation: `sa.install`, CONTENTS of the floppies, the man pages fd(4), ud(4) if present).
- Today `hostpath` passes `/dev/...` to the host; `ioctl` always fails with ENOTTY; there is no terminal.
- MAME (`/mnt/share/git/mame`, machine `p8000`) runs the real 8-bit firmware and boots WEGA: an independent check of the
  model's bytes is possible there (`mame/LOG.md`); UDOS in MAME lists a floppy with `CAT D=1 F=L P=&` (LOG.md section 8):
  a check of the UDOS images the model writes.
- The UDOS file system (NDOS) is documented in the UDOS Systemhandbuch (`tmp/wega31_books/UDOS_Systemhandbuch.txt`,
  chapter 5: the directory, the descriptor sector, the pointer sector, the NDOS calls).
- Sample UDOS floppies (owner): `udossys.cqm` (the UDOS system disk, 75 files) and `w31start.cqm` (the WEGA start disk,
  57 files) in the owner's MAME floppy directory (`../mame/floppies` of the owner's download tree); `dskconv -itype copyqm
  ... -otype raw` (libdsk, installed) turns them into RAW images of 655360 bytes. The file lists of both: `mame/LOG.md`
  section 8.

## Steps

1. The facts: the UDOS file system from the handbook (chapter 5) and the two sample floppies (an own reader, compared
   with the file lists of LOG.md); a writer of the same format, its images checked in MAME (UDOS `CAT`, reading a file).
   The request protocols byte by byte from both sides (WEGA programs and driver, coupling software); the
   `/dev` table of WEGA (names, majors, minors, block or character); the UDOS file format on a floppy as far as L and S
   need it; the EPROM programmer as K.PROM.S/K.P8000.S drive it (board detection, timing is irrelevant here, errors).
   Written down in `host/KNOWLEDGE.md` (a section "The 8-bit part and /dev/ud").
2. Emulator: the device layer and the minimal terminal (`/dev/console` as stdin and stdout, `ttyname`, `isatty`, the
   ioctls of sgtty as far as the programs use them; the case field `terminal:`) (`/dev` of the guest from a table, stat with rdev, per-fd model state, ENXIO for the rest).
3. Floppy images: `/dev/fd*`, `/dev/rfd*` per drive from the case (`device:`); geometry from the minor.
4. `/dev/ud`: R, W, F, V, C on the same images; L, S (UDOS files); tests of the model of its own.
5. getud/putud: corpus (pilot of the device field), checkall.
6. `/dev/ud` P: the virtual EPROM programmer (board, EPROM types and sizes, contents file, programming as AND, erase as
   a case action, read-back verify, erase check, CRC, the error codes); tests of its own.
7. pburn: corpus, checkall.
8. format, verify, copydisk, doscopy: a few cases each (proof); the rest is 120.
9. Documentation: compare/README (`device:`), host/z8k-emu/README.md, 110 (Design 7), 120 (what is left).

## Decisions

- Owner 2026-11-16: number 117, done before 120 goes on; after getud/putud directly pburn; the EPROM programmer is
  emulated completely (burning and reading), not left out.
- Owner 2026-11-16: the target of pburn is version 2.1 (the one the build makes).
- Owner 2026-11-16: the UDOS files of `L`/`S` live in a real UDOS file system (NDOS) on a floppy image, so that the
  images are the ones UDOS and MAME read.
- Owner 2026-11-16: a minimal terminal is part of this increment (stdin is `/dev/console` for `ttyname`, `isatty`, the
  questions of format, verify, copydisk, pburn's console). Designed so that it can grow into the full terminal later
  (vi and the other screen programs: termio/sgtty modes, the keys as a byte stream, the screen as output).

## Design (decided by Claude 2026-11-16, for review)

- One device layer in z8k-emu, driven by a table (`Z8K_DEVTAB=FILE`, written by the corpus runner from the case's
  `device:` and `terminal:` lines; a default table without it): per guest name below `/dev` its type (`c`/`b`), major,
  minor, an inode number, and a model. The guest `/dev` is that table: a directory read of `/dev` lists it (ttyname
  scans it), `stat` of a name gives the type, `st_rdev`, `st_ino`; a name not in the table does not exist (ENOENT), the
  host `/dev` is never reached (except `/dev/null` and `/dev/tty` mapped by the table).
- Models, two kinds:
  - `image:FILE` – a floppy: `read`, `write`, `lseek` on a RAW image file (geometry from the minor, fd(4)); block and raw
    device the same bytes.
  - `proc:PROGRAM` – a byte stream to a model program on the host (a pipe pair): `write` sends the bytes, `read` waits
    for at least one byte and returns what is there (the semantics of the `ud` driver: udread). `/dev/ud` is such a
    model: `host/z8k-emu/models/ud8.py`, the 8-bit part (the coupling software K.*.S and the NDOS calls it makes,
    transcribed; the UDOS file system through `host/tools/udosfs.py`; the floppies of the drives are the same image files
    as the `image:` devices; the EPROM programmer with its EPROM contents as files; what the 8-bit part prints on its
    console goes to a file of the case). One process per open of `/dev/ud`; its exit status and files are part of the
    result.
- The console: `terminal: console` (the minimal terminal) makes fds 0, 1, 2 the device `/dev/console` (char, the major
  and minor of WEGA's console): `isatty` and the sgtty ioctls (TIOCGETP, TIOCSETP, TIOCSETN, the tchars/ltchars ones)
  succeed and keep their settings, `fstat` gives the console's type and numbers (ttyname finds `/dev/console`), the
  bytes still come from the step's stdin and go to its output. It grows later (vi): the settings will then act on the
  byte stream (echo, raw/cbreak, erase/kill), a screen model can be put behind it.
- `/dev/ud` opens only as on WEGA: the controlling terminal is the console (`terminal: console`) and no other process
  has it open (ENXIO otherwise; udopen).
- Why a separate model program and not C in the emulator: the 8-bit side is a protocol machine with a file system
  behind it (much logic, no speed need); Python reads like the sources it transcribes, and the UDOS reader exists
  already. The emulator gets only the generic layer (table, image, pipe, console).

## Not part of it

Timing of the hardware (the model answers at once), the terminal model in general (120 or its own increment), the
corpora of format, verify, copydisk, doscopy beyond the proof cases (120).

## Log

- 2026-11-16: written; the owner's decisions on UDOS (a real file system) and the minimal terminal; pburn target 2.1.
