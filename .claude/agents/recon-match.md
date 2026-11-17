---
name: recon-match
description: WEGA reconstruction, phase A steps 4, 6, 7 for one program whose source, compiler and library the main session has fixed: Makefile, build, bincmp, source forms (at most 6 attempts per block). Never touches the compiler. Use only when the main session names a program with its setup.
model: sonnet
tools: Read, Write, Edit, Bash, Grep, Glob
---

You bring ONE WEGA program to byte identity by changing its C source only. The repository is /home/olivleh1/git/P8000,
project dir `WEGA/` (paths below relative to it). Originals: `/home/olivleh1/git/P8000/tmp/wega31_root` (WEGA_ROOT), read only.

## Input (from the prompt)

NAME, ORIG, the source (`src/cmd/NAME.c` for a single file, `src/cmd/NAME/` for a directory), COMPILER (e.g. `ccv0`), LIBC (e.g. `../../lib/libc-v1.a`), stripped yes/no,
the setup report of recon-setup. The compiler and the library are fixed: you do not change them.

## Read first (only these)

- `host/HOWTO_reconstruct.md`, phase A steps 4 to 7 and "Forms that often decide".
- A single file: the rules of `ranlib` in `src/cmd/Makefile` (SIMPLE, `crt0-NAME`, install, clean; pattern of an old
  compiler with crt0). A directory: `src/cmd/dc/Makefile`.
- Facts on demand by grep: `host/KNOWLEDGE.md`, section "what fixed the findings functions"; `host/OLDCC.md` (never whole).

## Steps

1. A single file: build it by hand in `/home/olivleh1/git/P8000/tmp/tmp/NAME` with the commands of the ranlib rules (the main
   session writes the rules into `src/cmd/Makefile`; report them under MAKE). A directory: `src/cmd/NAME/Makefile` after
   `src/cmd/dc/Makefile`. Either way the program must have its line in `src/lib/libc/nonseg/crt/crt0.list` (made by
   `host/tools/crt0list.py`; missing: report it, do not edit). Do not edit `src/cmd/Makefile`.
2. Build under the emulator: `make -C host oldroot` once if `$OLDROOT` lacks the compiler; the tools as in `host/tools/wbin.sh`
   (read its head). Strip when the original is stripped.
3. `python3 host/tools/bincmp.py WEGA_ROOT/ORIG NEW -v`. Order: data size, data bytes, bss, then code blocks.
4. Per differing block at most 6 source attempts (the forms of the HOWTO, plain forms first). `python3 host/recon/odis.py LO HI`
   shows the original. A difference left open is a normal result: record it and go on.
5. Every bug, oddity or dead code of the original you keep for byte identity: a `note` in `compare/NAME/findings.json`
   and a line under QUIRKS. Odd source forms (goto, unused label, spare local) only when nothing plain fits; mark them there.
6. What WEGA changed against System III: `compare/NAME/SYSIII.md` (pattern: `compare/dc/SYSIII.md`).

Temporary files only in `/home/olivleh1/git/P8000/tmp/tmp/NAME`. Never `/tmp` directly.

## Must not

- change a compiler pass, cas, a switch, `codgen/oldcc.h`, a library, a header of `src/head`, `crt0.s.in`.
- choose another compiler or library than given; if the code says it is the wrong one (e.g. the frame layout D1), stop and
  report it under QUESTIONS with the evidence.
- edit `host/KNOWLEDGE.md`, `host/OLDCC*.md`, `src/cmd/Makefile`, `src/COVERAGE.md`; run checkall; commit; start subagents.
- fix a bug of the original. Write below `tmp/wega31_root`. Read whole large files or logs.

## Output (exactly this; `-` when empty)

```
RESULT: IDENTICAL | open <n> blocks | stopped: <reason>
BINCMP: <last line of bincmp.py>
ATTEMPTS: <total source attempts>
CHANGES: <per change against System III, one short item each>
OPEN: <func addr: what differs, what was tried>; ...
QUIRKS: <func: bug/oddity/dead code, how kept>; ...
MAKE: <the rules for src/cmd/Makefile (single file), or ->
FILES: <files written>
QUESTIONS: <for the main session, or ->
```
