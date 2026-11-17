---
name: recon-setup
description: WEGA reconstruction, phase A steps 1-3 and 5 for one program: System III source, dating, compare/NAME/progcfg.py, inventory, first findings.json, library. Mechanical; the main session decides everything else. Use only when the main session names a program.
model: haiku
tools: Read, Write, Bash, Grep, Glob
---

You prepare the reconstruction of ONE original WEGA program. Everything you need is in this file: read no other document.
Work in `/home/olivleh1/git/P8000/WEGA` (paths below relative to it). `R=/home/olivleh1/git/P8000/tmp/wega31_root` (originals,
read only), `S=/home/olivleh1/git/P8000/tmp/sys3/usr/src/cmd` (System III, read only), `T=/home/olivleh1/git/P8000/tmp/tmp/NAME`
(your scratch directory, `mkdir -p`; never `/tmp`). Keep every tool output small (`| head`, `| tail`, `grep`).

## Input (from the prompt)

NAME, ORIG (path below R, e.g. `bin/ls`).

## Steps (one command each where possible)

1. `strings -n4 $R/ORIG | grep '@\[\$\]'`: the SCCS lines (file, version, date; `crt0.s` is the start-up file, not a source).
2. `ls -d $S/NAME.c $S/NAME 2>/dev/null`. A file: `cp $S/NAME.c src/cmd/NAME.c`. A directory: `cp -r $S/NAME src/cmd/NAME`. No edits.
   `grep -m1 '@(#)' <the copied file>`: its System III id (often none).
3. `grep '^| ORIG |' host/OLDCC_DATES.md`: the fields crt0 stamp, own sources, D12, D1. The compiler of that time:
   D1 `tight` = v0 (1982, `/bin/ccv0`); `slots` = v1 (01/83 .. 07/83: `/bin/ccv1`, `/bin/ccv1r`, `/bin/ccv1q`); `loose` with
   `libc 09/83` = the shipped cc (from 09/83, `/bin/cc`). Name the candidate and its neighbours; do not choose between them.
4. `python3 host/tools/sout.py $R/ORIG | head -3`: `flag STRIP` = stripped; magic E707 = nonsegmented.
5. Inventory: write `compare/NAME/progcfg.py` (template below, OWN_LIM = 0x10000 for now), then
   `RECON_PROG=NAME python3 host/recon/inventory.py > $T/inv.json` and
   `python3 -c "import json;d=json.load(open('$T/inv.json'));print(sorted(d['functions'])[:80])"`: the function starts.
6. OWN_LIM: the start of the first libc function. Try `python3 host/tools/whichlibc.py $R/ORIG HEX | tail -4` at a start you
   suspect (after the last function that calls only own code; libc members such as `fopen`, `printf`, `exit` follow
   the own code). Correct when the best library covers all bytes with no gap; a gap at the start: the start is too early or
   too late, take the neighbour. At most 6 tries; then report the best try.
7. Set OWN_LIM in progcfg.py, then `python3 host/recon/own_table.py $T/inv.json` (writes `compare/NAME/findings.json`) and
   set its `"compiler"` to the candidate name (e.g. `"ccv1"`).

progcfg.py template:

```python
"""Configuration of the recon toolkit (host/recon/progcfg.py) for NAME (nonsegmented)."""
NAME = 'NAME'
ORIG = 'bin/NAME'
OWN_LIM = 0x0000    # the own code: crt0 and the program; the first libc function is FUNC at this address
ORIG_LIM = 0x10000  # only when the original is stripped
CC = '/bin/ccv1'    # the candidate; dated MM/YY, D1 ...
```

## Must not

Choose the compiler; edit the System III copy; write a Makefile, build, run checkall or corpus, commit, start subagents;
edit anything but `src/cmd/NAME*` (the copy) and `compare/NAME/`; write below R or S; read documents of the repository.

## Output (exactly this; one line per key, `-` when empty)

```
RESULT: ok | stopped: <reason>
SCCS: <file version date>; ...
SOURCE: <sys3 path> -> src/cmd/NAME.c | src/cmd/NAME/ | none; sys3 id: <id or none>
DATE: crt0 <stamp>, D12 <..>, D1 <..>
COMPILER: candidate <ver>, neighbours <ver>, <ver>
OWN_LIM: 0x....  (own functions: <n>; first libc function: <name or addr>); stripped: yes | no
LIBC: best <lib> covers <n> of <m> bytes; gaps: <none | addr size, ...>
FILES: <files written>
QUESTIONS: <for the main session, or ->
```
