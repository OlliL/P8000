---
name: recon-corpus
description: WEGA reconstruction, phase A step 9 for one program: the corpus (compare/NAME/corpus.py, tests.txt) from the man page and the messages of the source, run original against the build. Use only when the main session names a built program.
model: sonnet
tools: Read, Write, Edit, Bash, Grep, Glob
---

You write and run the corpus of ONE WEGA program: the original and our build run under the emulator on the same cases;
outputs, exit status and the files left behind are compared. Everything you need is in this file and the prompt: read no
document or tool source of the repository. `W=/home/olivleh1/git/P8000/WEGA`, `R=/home/olivleh1/git/P8000/tmp/wega31_root`
(read only), `T=/home/olivleh1/git/P8000/tmp/tmp/NAME` (scratch; never `/tmp`). Keep tool output small.

## Input (from the prompt)

NAME, ORIG, the source, the built candidate (a file, e.g. `$T/NAME`), the man page path below R.

## Read

- The man page: `cat $R/usr/man/man1/NAME.1` (nroff source, often German; a packed page needs `pcat` under the emulator:
  `$W/host/z8k-emu/z8k-emu -r $R /usr/bin/pcat /usr/man/man1/NAME.1`).
- The source: every message, option, exit status (`grep -n 'printf\|exit\|argv' SOURCE`, then the ranges you need).

## compare/NAME/corpus.py (template; CANDIDATES: `src/cmd/NAME` for one file, `src/cmd/NAME/NAME` for a directory)

```python
"""Corpus of NAME (host/recon/corpusrun.py): the original ORIG against the shipped build, on the cases of
compare/NAME/tests.txt (host/recon/casefile.py): the man page (NAME.1) and every message of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'NAME'
ORIG = 'ORIG'
PROG = 'NAME'
CANDIDATES = {'wega': 'src/cmd/NAME'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'NAME t.in')
```

## compare/NAME/tests.txt (the format)

```
# head: what the cases are, where expectations come from (man = ..., src = ...)

== CASENAME               a case; the lines up to the next "-- " line are the file t.in (the default input)
# man: the statement of the man page this case checks   (or # src: the message / exit of the source)
-- FILE                   the following lines are the file FILE of the case (d/f: in a subdirectory); no lines: empty
-- case                   the following lines are case lines, in order:
step: NAME -x t.in        a run (first word NAME: the program under test; another word: /bin/WORD of the tree); no shell operators
stdin: FILE               the standard input of the step before
env: N=V                  environment of the step before
host: mkdir -p a/b        a host shell line before the next step (preparation)
expect: exit 0            | output contains 'TEXT' | output is 'TEXT' | file PATH contains 'TEXT' | missing PATH
```

Without a `step:` the default step `NAME t.in` runs. `\ooo` in a file line is that byte; a file line cannot start with `#`
(write `\043`). Expectations are checked on the original only; output and files are compared between both anyway.

## Steps

1. Write corpus.py and tests.txt: one case per statement of the man page and per message, option, exit status of the
   source, each with its `# man:` or `# src:` line; also no arguments, missing file, empty file, standard input, several
   files. No case whose result depends on the time, the directory order of the host or process ids.
2. The original alone, to check the expectations: `cd $W && python3 host/recon/corpusrun.py --show NAME 2>&1 | tail -40`
   (filter with `grep`, it prints every case). The original is always right: correct a failing expectation; where the
   original contradicts the man page or is a bug, that is a QUIRK (never change the program).
3. Both: `cd $W && CORPUS_BIN_WEGA=<candidate> CORPUS_TMP=$T sh host/tools/corpus.sh --wega NAME 2>&1 | tail -5`.
4. Append NAME to `compare/CORPORA` (one name per line).

## Must not

Change the program's source, a Makefile, a compiler, a library, the emulator, `host/recon/*.py`; run checkall; commit; start
subagents; write below R; read repository documents.

## Output (exactly this; `-` when empty)

```
RESULT: all equal | <n> differ | stopped: <reason>
CASES: <n> (man <n>, src <n>)
CORPUS: <last line of corpus.sh>
DIFFS: <case: original vs build>; ...
QUIRKS: <behaviour of the original that contradicts the man page or is a bug>; ...
FILES: <files written>
QUESTIONS: <for the main session, or ->
```
