# Future project structure: ZEUS and WEGA in one tree

Status: plan, not implemented. Nothing moves before the restoration of
the original WEGA is finished and ZEUS 3.21 has been restored.

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
| ZEUS 3.21 | latest ZEUS, byte-identical with the shipped one | variant `zeus` in the tree |
| new WEGA | ZEUS 3.21 plus the WEGA hardware and WEGA programs | variant `wega` in the tree |

Older WEGA versions do not matter. The original WEGA lives only in its
tag. After the tag the tree builds ZEUS 3.21 and the new WEGA.

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
  host/                shared: emulator, recon, bincmp, tool chain (ccv0, ccv1, ccvm, ...)
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
- checkall takes `VARIANT` and compares against `ref/<variant>`.
- Libraries and compilers of several generations (`libc-vN`, `ccvN`)
  stay shared, as now.

## Trade-offs

- A shared file that diverges later is copied out of `src/` into both
  variants; its git history follows only one copy. A file that becomes
  identical again moves back to `src/`.
- The WEGA change to a ZEUS file is only visible as
  `diff variant/zeus/X variant/wega/X`. A small tool lists all of these
  diffs (the "WEGA patches against ZEUS").

## Steps

1. Finish the restoration of the original WEGA in the current layout.
   Tag `wega-orig`.
2. Record already now, per restored program, its origin:
   - Zb: ZEUS file, only the copyright string exchanged
   - Zm: ZEUS file changed by WEGA
   - W: WEGA's own development
   (e.g. a field in `findings.json` or a column in `COVERAGE.md`). This
   is the basis for the split.
3. Restore ZEUS 3.21 byte-identically.
4. Split file by file:
   - identical (copyright string aside): `src/`
   - only in one variant: that variant
   - different: both variants; the WEGA copy becomes a merge task
   Files that differ only in exchanged strings go to `src/` with the
   ZEUS strings; the differences become lines of `brand.tab`. Until
   then the WEGA strings stay in the source, as now.
5. New WEGA: for each differing file decide whether the WEGA change
   (hardware) is carried over onto the 3.21 version, or the 3.21 version
   is taken as is. Taken as is means it moves to `src/`.

Goal: a large `src/` and small variant directories. What remains of
WEGA is mainly its hardware and its own programs.
