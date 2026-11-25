# Future project structure: ZEUS and WEGA in one tree

Status: plan, not implemented. Nothing moves before the restoration of
the original WEGA is finished; then only the old tool chain moves to
`old/` (see "Steps"). The split into `src/` and `variant/` follows
after ZEUS 3.21 has been restored.

## Background

- WEGA is a copy of an older ZEUS version. The copyright strings were
  replaced with a hex editor so that nothing points to Zilog. Some kernel
  objects were replaced by own developments for changed and new hardware.
  Some own programs were added.
- The current work restores this original WEGA byte-identically.
- Next, the latest ZEUS (3.21) is restored byte-identically.
- Then ZEUS 3.21 is integrated into WEGA for everything WEGA took from
  ZEUS. The result is a new WEGA version.

## Versions

| Target | What | Kept as |
|---|---|---|
| original WEGA | the version restored now, byte-identical with the shipped one | git tag `wega-orig` only |
| original ZEUS 3.21 | latest ZEUS, byte-identical with the shipped one | git tag `zeus-orig` |
| ZEUS 3.21 | its sources, built with the newest compiler and libc | variant `zeus` in the tree |
| new WEGA | ZEUS 3.21 plus the WEGA hardware and WEGA programs | variant `wega` in the tree |

Older WEGA versions do not matter. The original WEGA lives only in its
tag, the byte-identical ZEUS 3.21 in its own. After both tags the tree
builds ZEUS 3.21 and the new WEGA.

## Rule: `src/` holds only what both variants share

| File | Place |
|---|---|
| same source in ZEUS and WEGA (copyright string aside) | `src/` |
| only ZEUS (e.g. drivers for Zilog hardware that WEGA replaced) | `variant/zeus/` |
| only WEGA (own programs, new hardware) | `variant/wega/` |
| in both, but with different content | two copies: `variant/zeus/` and `variant/wega/` |

- A path exists either in `src/` or in variant directories, never both.
  There is no shadowing. The build checks this.
- This applies to the kernel too: shared objects in `src/uts`, hardware
  specific objects in the variant. Hardware dependent headers
  (`head/sys`) the same way.
- Strings that WEGA exchanged in the binaries do not split a file. The
  source holds the ZEUS string; the WEGA build exchanges it as a final
  step (see "Branding"), the same way WEGA did with the hex editor.
- The whole tree must build natively under WEGA and ZEUS (and as a
  cross build): only sh, make, cc and C programs built in the tree; no
  host-only tools. WEGA's make has `VPATH` for prerequisites
  (`src/cmd/make/misc.c`, `findfl`), so `src/` plus `variant/X/` works
  natively.

## Branding

WEGA exchanged more than the copyright and SCCS strings: e.g. the
default terminal became "P8", and texts in printf calls changed. All of
these are exchanged in the binaries by one program, `brand`, run once
over the install root as the last step of the build. The Makefiles of
the programs do not call it.

### Control file `variant/wega/brand.tab`

One line per exchange, fields separated by tabs:

```
# file          count  old               new
bin/ls          1      "Zilog\0"         "WEGA\0\0"
etc/profile     1      "TERM=zterm"      "TERM=P8\0\0\0"
lib/libc.a      2      "..."             "..."
```

- file: path relative to the install root. A pattern (e.g. `bin/*`) is
  allowed for a string that occurs in many files; the count then holds
  per file.
- count: the expected number of matches per file. `+` means "at least
  one"; it weakens the check and is the exception.
- old, new: C escapes (`\0`, `\n`, `\t`, `\ooo`). The terminating `\0`
  belongs to the search text, so that a longer string with the same
  start is not hit.
- new must have the length of old. A shorter text is padded with `\0`
  explicitly, as the hex editor had to. A longer text is an error.
- The search is dynamic (find and replace), not by offset, so a source
  change does not require a new table. A source change that changes the
  number of matches stops the build and names the file.

### Program `brand`

- Source `src/tools/brand.c`, K&R C and stdio only, so it builds the
  same on WEGA, ZEUS and the cross host. Not installed.
- Reads the table, patches the files in the install root.
- Two passes per file: first count, then write. On a wrong count it
  stops with file, text and found count, before anything is changed.
- Streams through the file with a buffer that overlaps by the length of
  the search text minus 1 (large files such as the kernel or libc do not
  fit into memory on the Z8000).
- Can run again: if old is no longer found but new is found with the
  expected count, the file counts as branded, no error. After
  reinstalling single programs it simply runs over everything again.
- If s.out or ar files had a checksum, brand would have to update it.
  None is known; not measured.

### Build order

`make` builds, `make install` installs, then the root Makefile runs
`brand` if `variant/$(VARIANT)/brand.tab` exists. ZEUS has no table,
since the sources hold the ZEUS strings.

## Layout

```
P8000/
  src/                 shared: cmd, lib, head, uts (shared objects only), etc, man
  variant/
    zeus/
      cmd/ head/ uts/  ZEUS-only files and the ZEUS copy of differing files
      MANIFEST         per program: version, compiler, libc; kernel object list
    wega/
      cmd/ head/ uts/  WEGA-only files and the WEGA copy of differing files
      MANIFEST
      brand.tab        string exchange in the binaries (see Branding)
  src/tools/brand.c    the branding program (shared, not installed)
  host/                shared: emulator, recon, bincmp
  ref/                 shipped binaries as reference
    zeus-3.21/
    wega-orig/
  compare/
    <name>/            one corpus for both variants
      expect.wega/     only where the behaviour really differs
  build/               (.gitignore) out/zeus, out/wega
```

- Build: `make VARIANT=zeus` or `make VARIANT=wega`. Sources come from
  `src/` and `variant/$(VARIANT)/`.
- checkall takes `VARIANT`. ZEUS 3.21 and the new WEGA are built with
  the newest compiler and libc only (see "Old tool chain"), so they are
  no longer byte-identical with `ref/`; the corpora (original binary
  against the new build) are the test. Byte identity is checked only in
  the tags `wega-orig` and `zeus-orig`.

## Old tool chain

Today the tree holds many old compiler versions (`ccv0`, `ccv1`,
`ccvm`, ...) with many `OLD_D...` switches, and many libraries
`libc-v0` ... `libc-v14`. They exist only so that the originals build
byte-identically. The names follow the order in which they turned up,
and the switches make the compiler sources hard to read.

The original WEGA is based on an older ZEUS, so its old compilers are
old ZEUS compilers. The restoration of ZEUS 3.21 reuses them and will
probably add more. The old tool chain therefore stays until both
originals are tagged, and is then removed from the tree.

### `old/`

Before the tag `wega-orig` everything old moves to its own tree:

```
old/
  cc/            the old compilers, one clean source per version
  libc/          the old libraries
  BUILD.wega     per program: compiler, libc (and switches) of the original WEGA
  BUILD.zeus     the same for ZEUS 3.21 (added during its restoration)
  BUILD.mk       the one in use: a copy of BUILD.$(VARIANT)
```

- `src/` then holds only the newest compiler and the shipped libc.
- `BUILD.*` is the only place that says which compiler and libc a
  program uses. It is a make include file, one block per program:

  ```
  CC_ed = ../../../old/cc/<name>/cc
  LIBC_ed = ../../../old/libc/<name>.a
  ```

  The Makefile of the program reads it and uses the variables instead
  of fixed paths:

  ```
  include ../../../old/BUILD.mk
  ed: ed.o
  	$(CC_ed) -o ed ed.o $(LIBC_ed)
  ```

- WEGA's make (`src/cmd/make/gram.y`) has `include`, measured:
  - the path goes to `fopen` unchanged, macros are not expanded, so
    `include .../BUILD.$(VARIANT)` does not work: a fixed relative path
    only (depending on the depth of the directory);
  - exactly one blank after `include`: the loop meant to skip blanks
    tests `*p != TAB || *p != BLANK`, which is always true (original
    bug).
  Therefore the root Makefile copies `old/BUILD.$(VARIANT)` to
  `old/BUILD.mk` before it builds (`cp`, native).
- The host tools (coverage.py, setup.py, checkall) read the same file
  instead of parsing the Makefiles: one table for people, build and
  tools.
- The `OLD_D...` switches are resolved with `unifdef` and the switch set
  of each version into one clean source per version (e.g.
  `old/cc/<name>/` without any `#ifdef OLD_...`). checkall proves that
  each generated version still builds byte-identically. The switched
  source then goes (it stays in the git history).
- Names: versions named by evidence (e.g. a date from the sccsid of a
  compiler pass or from the programs built with it), so that the names
  also hold for ZEUS instead of continuing the numbering in the order
  of discovery. Many `libc-vN` are not historical libraries but mixes
  (the shipped libc with members exchanged); a date there would be a
  false precision, a descriptive name plus a table in `BUILD.*` is
  better. Where a rename is not worth it, the name stays; a comment in
  `BUILD.*` holds name and evidence.
- Both tags have the same `old/` layout, so the tools (matrix, switches,
  checkall) work for both.

### After `zeus-orig`

`git rm -r old/`; in all Makefiles one mechanical change: the `include`
line goes, `$(CC_x)` becomes `$(CC)` and `$(LIBC_x)` becomes `-lc`. Programs
built with an old compiler until then may behave differently with the
newest one (undefined behaviour, quirks of old library members). The
corpora find this; they therefore stay in the tree, not only in the
tags.

## Trade-offs

- A shared file that diverges later is copied out of `src/` into both
  variants; its git history follows only one copy. A file that becomes
  identical again moves back to `src/`.
- The WEGA change to a ZEUS file is only visible as
  `diff variant/zeus/X variant/wega/X`. A small tool lists all of these
  diffs (the "WEGA patches against ZEUS").

## Steps

1. Finish the restoration of the original WEGA in the current layout.
2. Move the old tool chain to `old/`: resolve the switches with
   `unifdef`, rename by evidence, write `BUILD.wega`. checkall stays
   ALL OK.
3. Tag `wega-orig`.
4. Restore ZEUS 3.21 byte-identically, with the same `old/` (extended
   as needed, `BUILD.zeus`).
5. Tag `zeus-orig`: both originals build byte-identically.
6. Remove `old/`; everything builds with the newest compiler and libc.
   The corpora are the test.
7. Split file by file:
   - identical (copyright string aside): `src/`
   - only in one variant: that variant
   - different: both variants; the WEGA copy becomes a merge task
   Files that differ only in exchanged strings go to `src/` with the
   ZEUS strings; the differences become lines of `brand.tab`. Until
   then the WEGA strings stay in the source, as now.
8. New WEGA: for each differing file decide whether the WEGA change
   (hardware) is carried over onto the 3.21 version, or the 3.21 version
   is taken as is. Taken as is means it moves to `src/`.

Goal: a large `src/` and small variant directories. What remains of
WEGA is mainly its hardware and its own programs.
