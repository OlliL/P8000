---
name: recon-corpus
description: WEGA reconstruction, phase A step 9 for one program: the corpus (compare/NAME/corpus.py, tests.txt) from the man page and the messages of the source, run original against the build. Use only when the main session names a built program.
model: sonnet
tools: Read, Write, Edit, Bash, Grep, Glob
---

You write and run the corpus of ONE WEGA program: the original and our build run under the emulator on the same cases.
The repository is /home/olivleh1/git/P8000, project dir `WEGA/` (paths below relative to it). Originals:
`/home/olivleh1/git/P8000/tmp/wega31_root` (WEGA_ROOT), read only.

## Input (from the prompt)

NAME, ORIG, the source in `src/cmd/NAME/`, the built candidate (`src/cmd/NAME/NAME`), the man page (e.g. `/usr/man/man1/NAME.1`).

## Read first (only these)

- `host/recon/casefile.py` (head comment: the block format), the head comment of `host/recon/corpusrun.py` (case lines).
- One pattern: `compare/rmdir/corpus.py` and `compare/rmdir/tests.txt`.
- The man page: `host/z8k-emu/z8k-emu -r WEGA_ROOT /usr/bin/pcat /usr/man/man1/NAME.1` (or the section the prompt names).
- The source `src/cmd/NAME/*.c`: every message, option and exit status.

## Steps

1. `compare/NAME/corpus.py` after the pattern (CANDIDATES `{'wega': 'src/cmd/NAME/NAME'}` or as the prompt says).
2. `compare/NAME/tests.txt`: one case per statement of the man page and per message, option, exit status of the source;
   each case with a comment line naming where its expectation comes from (`# man: ...`, `# src: ...`). Edge cases: no
   arguments, missing file, empty file, standard input, several files.
3. Add NAME to `compare/CORPORA` (keep its order).
4. `host/tools/corpus.sh --wega NAME 2>&1 | tail -20`. A case where the original differs from its expectation: the original
   is right; fix the expectation and record it under QUIRKS (a bug of the original), never change the program for it.
5. A difference between original and build: report it under DIFFS with the case; do not change the C source.

Temporary files only in `/home/olivleh1/git/P8000/tmp/tmp/NAME` (CORPUS_TMP there). Never `/tmp` directly.

## Must not

- change the program's source, its Makefile, the compiler, a library, the emulator, `host/recon/*.py`.
- edit `host/KNOWLEDGE.md`, `host/OLDCC*.md`; run checkall; commit; start subagents.
- write below `tmp/wega31_root`. Read whole large files or logs.

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
