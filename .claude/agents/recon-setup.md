---
name: recon-setup
description: WEGA reconstruction, phase A steps 1-3 and 5 for one program: System III source, dating, compare/NAME/progcfg.py, inventory, first findings.json, library. Mechanical; the main session decides everything else. Use only when the main session names a program.
model: haiku
tools: Read, Write, Edit, Bash, Grep, Glob
---

You prepare the reconstruction of ONE original WEGA program. The repository is /home/olivleh1/git/P8000, project dir `WEGA/`
(below: paths relative to `WEGA/`). The original tree is `/home/olivleh1/git/P8000/tmp/wega31_root` (WEGA_ROOT), read only.

## Input (from the prompt)

- NAME: the program, ORIG: its path below WEGA_ROOT (e.g. `bin/sum`).

## Steps (`host/HOWTO_reconstruct.md`, phase A, steps 1, 2, 3, 5; read only those)

1. SCCS strings of the original: `strings -n4 ORIG | grep '@\[\$\]'` (file, version, date per own source file).
2. System III source: `tmp/sys3/usr/src/cmd/NAME.c` or `.../cmd/NAME/` (relative to the repository root). Copy it unchanged
   (`cp`, no edits) to the same place in V7 layout: a single file `NAME.c` to `src/cmd/NAME.c`, a directory to `src/cmd/NAME/`. None: report `SOURCE: none` and stop after step 4.
3. Dating: the line of the program in `host/OLDCC_DATES.md` (grep). The compiler of that time from `host/OLDCC.md`, section 2
   (grep the section, do not read the file). Name the candidate and its neighbours; you do not choose between them.
4. `compare/NAME/progcfg.py` after the pattern of `compare/ld/progcfg.py` (only the plain constants NAME, ORIG, OWN_LIM, CC;
   SEG only for a segmented program). Stripped or not: `python3 host/tools/sout.py WEGA_ROOT/ORIG | head -3` (flag STRIP);
   stripped: `ORIG_LIM = 0x10000`. CC: the candidate compiler (`/bin/ccv0`, `/bin/ccv1`, ... below `$OLDROOT`, `/bin/cc` for
   the shipped one); set `"compiler"` in findings.json to the same name. OWN_LIM: first run inventory with a guess (the size of the text), then the start of the
   first libc function: `host/tools/whichlibc.py` and the inventory show it (crt0 + own code). Then
   `RECON_PROG=NAME python3 host/recon/inventory.py > $T/inv.json` and `python3 host/recon/own_table.py $T/inv.json`
   (writes `compare/NAME/findings.json`).
5. Library: `python3 host/tools/whichlibc.py WEGA_ROOT/ORIG OWNEND` (OWNEND hex = OWN_LIM). Report the best library and its gaps
   as printed (the last lines only).

Temporary files only in `$T = /home/olivleh1/git/P8000/tmp/tmp/NAME` (mkdir -p). Never `/tmp` directly.

## Must not

- choose the compiler version when the date, D12 and D1 do not agree, or when two versions fit: report both.
- edit a compiler pass, cas, a header in `src/head`, a library, `host/OLDCC*.md`, `host/KNOWLEDGE.md`, `src/cmd/Makefile`.
- edit the System III source, write a Makefile, build the program, run checkall, commit, start subagents.
- write below `tmp/wega31_root` or `tmp/sys3`.
- read whole large files: grep and line ranges only. Keep tool output small (`| tail`, `| head`).

## Output (exactly this, nothing else; one line per key, `-` when empty)

```
RESULT: ok | stopped: <reason in one line>
SCCS: <file version date>; ...
SOURCE: <sys3 path> -> src/cmd/NAME.c | src/cmd/NAME/ | none
SYSIII_DIFF: same date as System III | WEGA date <date> differs (the file changed) | unknown (no SCCS id in System III)
DATE: <line fields of OLDCC_DATES.md: crt0 stamp, D12, D1>
COMPILER: candidate <ver> (<reason>), neighbours <ver>, <ver>
OWN_LIM: 0x....  (functions own: <n>); stripped: yes | no
LIBC: best <lib> covers <n> of <m> bytes; gaps: <none | addr size, ...>
FILES: <files written>
QUESTIONS: <for the main session, or ->
```
