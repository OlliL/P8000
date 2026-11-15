# 09 Whole system: self-hosted build, boot in MAME

## Status

open (last)

## Goal

Two tests of the whole: (1) the tree built a second time with the tools built from source (cc, cas, ld, ar, ...) gives the
same bytes; (2) a disk image of the install tree boots in the MAME P8000 machine and works (init, sh, login, commands).

## Done when

- A make target (or a checkall part) for the self-hosted build, byte-identical with the first build.
- A script that makes the disk image and a recorded boot in MAME with a list of checked commands.

## Read first

- `CLAUDE.md` (MAME fork, `mame/LOG.md` in the repository beside `WEGA/`).
- `host/tools/checkall.sh` (the install tree).

## Known

- The install tree of checkall is byte-identical with the WEGA tree in every file `make install` puts there.

## Steps

1. Self-hosted build: the PATH of the build names the tools of the install tree instead of the originals.
2. Disk image from the install tree; boot in MAME; the list of commands.

## Decisions

## Not part of it

## Log
