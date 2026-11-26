# Future project structure: ZEUS and WEGA in one tree

Status: plan, not implemented. Nothing moves before the restoration of
the original WEGA is finished; then only the old tool chain moves to
`old/` (see "Steps"). The split into `src/` and `variant/` follows
after ZEUS 3.21 has been restored.

## Repository

The new tree lives in its own git repository **`ZEUS-WEGA`**. It holds
only the operating system: ZEUS (machine S8000) and WEGA (machine
P8000), with everything that is under `WEGA/` today: sources, `host/`
(emulator, recon, tools), the corpora in `compare/`, and `contrib/`
(moves to `variant/wega/contrib/`). The other parts of the `P8000`
repository (firmware, UDOS, MAME) do not move.

## Background

- WEGA is a copy of an older ZEUS version. The copyright strings were
  replaced with a hex editor so that nothing points to Zilog. Some kernel
  objects were replaced by own developments for changed and new hardware.
  Some own programs were added.
- The current work restores this original WEGA byte-identically.
- Next, the latest ZEUS (3.21) is restored byte-identically.
- Then ZEUS 3.21 is integrated into WEGA for everything WEGA took from
  ZEUS. The result is version 3.3 of both systems: ZEUS 3.3 (ZEUS 3.21 with the bugs fixed that were found while restoring) and WEGA 3.3 (ZEUS 3.3 plus the WEGA hardware and programs).

## Versions

| Target | What | Kept as |
|---|---|---|
| original WEGA | the version restored now, byte-identical with the shipped one | git tag `WEGA_3.1` |
| original ZEUS 3.21 | latest ZEUS, byte-identical with the shipped one | git tag `ZEUS_3.21` |
| ZEUS 3.3 | its sources, built with the newest compiler and libc + all the fixed bugs we found during restoring | variant `zeus` in the tree |
| WEGA 3.3 | ZEUS 3.3 plus the WEGA hardware and WEGA programs | variant `wega` in the tree |

Older WEGA versions do not matter. The original WEGA 3.1 lives only in its
tag, the byte-identical ZEUS 3.21 in its own. After both tags the tree
builds ZEUS 3.3 and the WEGA 3.3.

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
ZEUS-WEGA/
  src/                 shared: cmd, lib, head, uts (shared objects only), etc
  variant/
    zeus/
      cmd/ head/ uts/ man/  ZEUS-only files and the ZEUS copy of differing files
    wega/
      cmd/ head/ uts/ man/  WEGA-only files and the WEGA copy of differing files
      brand.tab        string exchange in the binaries (see Branding)
      contrib/         today's WEGA/contrib
  src/tools/brand.c    the branding program (shared, not installed)
  host/                shared: emulator, recon, bincmp
  ref/                 shipped binaries as reference
    zeus-3.21/
    wega-3.1/
  compare/
    <name>/            one corpus for a program
      corpus.py        systems tested, with each system's original and candidate
      tests.txt        cases and their expected output
  native/              the native build in MAME (see "Native build in MAME")
    p8000-wega/
    s8000-zeus/
  build/               (.gitignore) out/zeus, out/wega
```

### Corpora for both systems

A corpus remains a program-level directory with `corpus.py` and
`tests.txt`, as today. `corpus.py` declares which systems this program's
corpus runs on and, for each one, the corresponding original binary,
candidate build and system root. A program that exists only on WEGA lists
only WEGA; one that exists on both lists both. This declaration is the
scope of the corpus, not a claim that a program is absent from every
unlisted system.

Add a `system:` field to the case format in `host/recon/corpusrun.py`.
It accepts `zeus` or `wega`; without the field, the case runs on every
system declared by that corpus. The existing `variant:` field keeps its
current meaning (an environment variation) and is not reused. The runner
selects the matching original and candidate for each run, compares them
on the same system, and checks that system's expected result. A case
without system-specific behavior therefore runs unchanged on all declared
systems. When behavior or output differs, write separate named cases
with `system: zeus` and `system: wega` and their respective expectations.

The runner must reject a `system:` value not declared in `corpus.py`;
it must not silently skip a case. Include the system in generated case
identities and result-cache keys so results from ZEUS and WEGA cannot be
mixed. Update the case parser/generator, corpus runner and corpus
configuration accordingly, and migrate existing corpora to declare their
system or systems. Keep each case's inputs and expectations in the
existing `tests.txt` format; there are no parallel `expect.wega/`
directories.

## Native build in MAME

The proof that the tree builds natively: per machine (P8000 with WEGA,
S8000 with ZEUS) one run that

1. makes a file system image that holds the repository (the git files
   only), as a second disk or partition the booted system can mount;
2. boots the machine in MAME in single-user mode;
3. mounts that file system and runs the complete build there with the
   native make, cc and tools;
4. ends MAME properly and takes the install tree out of the image;
5. compares it with the cross build (the same bytes).

`native/<machine>/` holds what is specific to the machine: the boot
dialogue, the mount and build script run in the guest, the disk layout.
Shared parts (making and reading the file system image, driving MAME)
are tools in `host/`. Their first versions come from increment 180 of
the restoration (`host/tools/wegafs.py` can already format a WEGA file
system and put files and directories into it; `host/tools/mamerun.py`
drives MAME and boots WEGA in single-user mode).

- Build: `make VARIANT=zeus` or `make VARIANT=wega`. Sources come from
  `src/` and `variant/$(VARIANT)/`.
- checkall takes `VARIANT`. ZEUS 3.3 and the WEGA 3.3 are built with
  the newest compiler and libc only (see "Old tool chain"), so they are
  no longer byte-identical with `ref/`; the corpora (original binary
  against the new build) are the test. Byte identity is checked only in
  the tags `WEGA_3.1` and `ZEUS_3.21`.

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

Before the tag `WEGA_3.1` everything old moves to its own tree:

```
old/
  cc/            the whole compiler source with the OLD_D... switches (all versions)
    oldcc.h      one block per version with all its switches, for all passes
  libc/          the old libraries
  BUILD.wega     per program: compiler, libc (and switches) of the original WEGA
  BUILD.zeus     the same for ZEUS 3.21 (added during its restoration)
  BUILD.mk       the one in use: a copy of BUILD.$(VARIANT)
```

- `src/` holds no compiler until `ZEUS_3.21`: the shipped WEGA cc is
  the version without switches of `old/cc`, built from there (no
  duplicate code). The newest compiler is probably the one of ZEUS 3.21,
  known only after its restoration, as one more version in `old/cc`.
- `src/` then holds the shipped libc only.
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
- The `OLD_D...` switches stay. They mark the places where the compiler
  generations differ; a ZEUS compiler not yet known will most likely
  differ at the same places, so it becomes a new combination of
  switches (plus a few new ones), and the matrix tools keep working.
  Whoever wants the effective source of one version runs `unifdef`
  with its switch set by hand; nothing generated is kept.
- One header `old/cc/oldcc.h` for all passes with switches (cparse,
  codgen, popt, cas, sld, the driver): one block per version with all its
  switches. The Makefiles name only the version (`-DOLD_V1`). Today only
  codgen works this way (`src/cmd/cc/codgen/oldcc.h`); cparse, popt,
  cas and sld get single `-DOLD_D...` from their Makefiles. Reasons:
  - one place per version ("what is ccv1?" is one block); the D numbers
    are global (D1 ... D64), so there is no clash between the passes,
    and a pass ignores the switches it does not know;
  - the native cc takes only about 160 characters of `-D` options
    (more overwrite the name of its temporary file, see the head of
    `codgen/oldcc.h`);
  - it fits `BUILD.*`: there the version, in `oldcc.h` the switches.
  `oldcc.sh` and `oldvar.sh` read or write the switches today and move to
  the common header; the role of the second copy
  `host/cc/codgen/oldcc.h` is to be cleared. This changes the compiler
  build: done in step 2, with checkall as proof, after asking the owner.
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

### After `ZEUS_3.21`

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
2. Move the contents of `WEGA/` from `P8000` to the new `ZEUS-WEGA`
   repository.
3. Move the old tool chain to `old/`: rename by evidence, write
   `BUILD.wega`, one `old/cc/oldcc.h` for all passes. checkall stays
   ALL OK.
4. Tag `WEGA_3.1`.
5. Extend the corpus format and runner for per-system originals and
   candidates, and add the `system:` case selector. Migrate the existing
   corpora to declare their supported systems (initially WEGA); add ZEUS
   configurations as ZEUS 3.21 is restored. This must be in place before
   ZEUS restoration so its binaries can be tested against the matching
   originals and expectations.
6. Restore ZEUS 3.21 byte-identically, with the same `old/` (extended
   as needed, `BUILD.zeus`).
7. Tag `ZEUS_3.21`: both originals build byte-identically.
8. Create the clean compiler in `src/`: `unifdef` once with the switch
   set of the newest compiler on `old/cc` gives `src/cmd/cc` without any
   `OLD_...`. Then remove `old/`; everything builds with the newest
   compiler and libc. The corpora are the test.
9. Split file by file:
   - identical (copyright string aside): `src/`
   - only in one variant: that variant
   - different: both variants; the WEGA copy becomes a merge task
   Files that differ only in exchanged strings go to `src/` with the
   ZEUS strings; the differences become lines of `brand.tab`. Until
   then the WEGA strings stay in the source, as now.
10. WEGA 3.3: for each differing file decide whether the WEGA change
   (hardware) is carried over onto the 3.21 version, or the 3.21 version
   is taken as is. Taken as is means it moves to `src/`.
11. Bug fixes for 3.3: until the tag `ZEUS_3.21` a bug of an original is
   rebuilt, not fixed (byte identity first). Only after that tag the
   bugs are fixed, each on the owner's word. The work list is the table
   "Bugs and quirks of the originals" in `host/KNOWLEDGE.md`, which
   holds every bug found while restoring; each fix is checked with the
   corpus of the program.

Goal: a large `src/` and small variant directories. What remains of
WEGA is mainly its hardware and its own programs.
