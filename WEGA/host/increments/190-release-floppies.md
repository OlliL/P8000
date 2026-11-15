# 190 Release: installation floppy images

## Status

open (after 180)

## Goal

The WEGA 3.1 installation floppies (root1..5, usr1..9, doc1..2) made from the install tree of `src/`, as RAW images and, where
the tools allow, as CopyQM (`.cqm`) and TeleDisk (`.td0`); ideally byte-identical with the original images in
`tmp/wega31_disks`.

## Done when

- A script (`host/tools/`, beside `../mame/tools/wegafs.py`) builds every floppy from the install tree: file system, CONTENTS,
  boot block of root1; compared with the original RAW images (byte-identical, or every difference explained and listed here).
- The images install with `../mame/tools/wega_install.py` into a tree equal to the install tree, and the root1 floppy boots
  sa.install in MAME (`180`).
- CopyQM and TeleDisk images written where a writer exists (libdsk `dskconv` lists the drivers `copyqm` and `tele`; whether
  it writes them is to be measured).

## Read first

- `../mame/tools/wega_install.py` (head comment: CONTENTS format, how sa.install installs), `../mame/tools/wegafs.py` (the file
  system as read; it has no writer), `../mame/tools/wegafs.md`.
- `src/cmd/standalone/sa.install.c` (what the installer expects of a floppy).

## Known

- The originals: 16 RAW images of 737280 bytes (720K) in `tmp/wega31_disks` (`w31root1..5`, `w31usr1..9`, `w31doc1..2`).
- Each floppy holds a WEGA file system with the file `CONTENTS` in its root directory; CONTENTS names the target directory,
  mode, owner and the name on the floppy and on the disk (`FILE [NAME]`); files lie flat in the root of the floppy.
- root1 starts with a boot block: its first 512 bytes differ from `/pb.image` of the tree from byte 36 on (to be explained).
- The time stamps on the floppies are those of the build of the distribution (1988, 1989).

## Steps

1. Read the originals completely: per floppy the file system layout (block order, inode order, free list), CONTENTS, the
   boot block; what of it is determined by the files and what by the tool that made the floppy.
2. A writer of the WEGA file system (mkfs and the placement of files as the original tool did); compare with the originals.
3. CONTENTS from the install tree and the list of the original (which file on which floppy).
4. CopyQM, TeleDisk: measure what `dskconv` writes; otherwise a writer of our own or RAW only (owner).

## Decisions

- Open (owner): byte identity of the images (needs the layout of the original tool) or "installs equal" as the criterion.

## Not part of it

## Log
