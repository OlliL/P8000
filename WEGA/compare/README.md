# `compare/` – everything we know about a program besides its source

*(Deutsche Fassung: [README.de.md](README.de.md))*

The WEGA project rebuilds the programs of the WEGA 3.1 system (the UNIX of the East German P8000 computer, a Zilog Z8000
machine) from source code, so that the rebuilt programs are **byte for byte identical** to the originals. The sources
live in `WEGA/src/`. This directory, `WEGA/compare/`, holds everything else that belongs to a program: how we test it
against the original, what we found out while reconstructing it, and how its source relates to UNIX System III.

`src/` must stay a clean source tree that could be copied onto a real WEGA machine and built there. That is why none of
the material here lives next to the sources.

This file explains what you find here and, in most detail, how the **corpus** works and how you write one yourself.

---

## Contents

1. [What is in this directory](#1-what-is-in-this-directory)
2. [Findings: `findings.json`, `FINDINGS.md`, `SYSIII.md`](#2-findings-findingsjson-findingsmd-sysiiimd)
3. [`progcfg.py`: settings of the reconstruction tools](#3-progcfgpy-settings-of-the-reconstruction-tools)
4. [The corpus: the idea](#4-the-corpus-the-idea)
5. [Running a corpus](#5-running-a-corpus)
6. [How a case runs, step by step](#6-how-a-case-runs-step-by-step)
7. [Three ways to write cases](#7-three-ways-to-write-cases)
8. [Reference: the case file](#8-reference-the-case-file)
9. [Reference: `corpus.py`](#9-reference-corpuspy)
10. [Tutorial: a corpus from scratch](#10-tutorial-a-corpus-from-scratch)
11. [Writing good cases](#11-writing-good-cases)
12. [What the emulator can and cannot do](#12-what-the-emulator-can-and-cannot-do)
13. [Troubleshooting](#13-troubleshooting)
14. [Glossary](#14-glossary)

---

## 1. What is in this directory

There is one subdirectory per program (or per group of programs that share one corpus). The name is the name of the
program: `compare/tar/` belongs to `/bin/tar`, `compare/cpp/` to `/lib/cpp`. A program that is one source file in
`src/cmd` (like `rm.c`) has its own directory here too.

| File or directory | What it is | Written by |
|---|---|---|
| `corpus.py` | The definition of the program's corpus: which original, which rebuilt binary, which cases. | by hand |
| `tests.txt` | Cases written as blocks of one text file (dc, rm, rmdir, mv, tar). | by hand |
| `cases/CASE/` | Cases written as one directory each: a file `case` and the starting files in `in/` (ar). | by hand |
| `fixtures.py`, `tree.py`, `test/`, `link/`, `cmdlines.txt` | Material from which a corpus makes its cases: test programs, object files, command lines. | by hand |
| `findings.json` | Per function of the program: where it lies in the original binary and whether our compiled function is identical. | tools + by hand |
| `FINDINGS.md` | A readable report made from `findings.json`. | generated |
| `SYSIII.md` | How the WEGA source relates to the System III source it was derived from. | by hand |
| `progcfg.py` | Settings of the reconstruction tools in `host/recon/` (addresses, modules, compiler). | by hand |
| `NOTES.md`, `HANDOFF.md`, `MODULES`, `ADDRESSES`, `DATA` | Working notes of a long reconstruction (popt, cas). | by hand |

Two files are not per program:

- **`CORPORA`** – the list of programs whose corpus `checkall` runs. A new corpus joins by one line here; nothing is
  found automatically.
- **`binutils/tree.py`** – a shared helper of the corpora of `size`, `strip` and `ranlib` (it prepares one tree of
  object files and archives all three use).

The state today (the number of cases changes when sources change):

| Program(s) | Corpus made of | Candidates |
|---|---|---|
| `ar` | hand-written `cases/` | wega, host |
| `size`, `strip`, `ranlib` | generated from `binutils/tree.py` | wega, host |
| `cc`, `scc` | command lines (`cc/cmdlines.txt`) | wega, host |
| `cpp`, `cparse`, `scparse`, `codgen`, `scodgen`, `popt`, `spopt` | generated: every C source of `src/cmd` and `src/lib` | wega, host |
| `cchain`, `scchain` | generated: the whole compiler driver on every C source, plus links of `cchain/link/` | host |
| `ld`, `sld`, `gld` | generated from `fixtures.py` and `test/` | wega, host (gld: host) |
| `cas`, `casold` | generated: C sources, opcode tables, jump tests | wega, host (casold: host) |
| `dc`, `rm`, `rmdir`, `mv` (with `cp`, `ln`), `tar` | `tests.txt` | wega |

---

## 2. Findings: `findings.json`, `FINDINGS.md`, `SYSIII.md`

Reconstructing a program means writing C source that the *old* compiler of its time turns into exactly the original
machine code. While that work goes on, we need to know, function by function, how far we are.

**`findings.json`** has one entry per function of the program:

```json
"iputb": { "file": "ifile.c", "orig": "0x0042", "status": "exact" }
```

- `file` – the source file the function is in,
- `orig` – its address in the original binary,
- `status` – `exact` (our compiled function is identical, jump and call targets included), `masked` (identical except
  for one known difference of the old assembler, `call` against `calr`), or `open` (still different),
- `note` – only when there is something odd to know: a guessed name, dead code, an unusual source form.

The head of the file says which compiler and which comparison were used, and at which commit it was measured. The
statuses are written by `host/recon/findings.py`; the notes are written by hand.

**`FINDINGS.md`** is generated from `findings.json` (`host/recon/findings.py NAME`). Never edit it by hand.

**`SYSIII.md`** compares the WEGA source with the System III source it comes from (and with the Plexus sources, a
related Z8000 UNIX): how many lines are equal, what WEGA added, and where the source had to take an unusual form to
give the original bytes.

Bugs found in an original program are *not* recorded here. They go into one central table, "Bugs and quirks of the
originals", in `host/KNOWLEDGE.md`.

---

## 3. `progcfg.py`: settings of the reconstruction tools

The tools in `host/recon/` (disassembly, function-by-function comparison, models of the old compilers) take a program
by name and read its `progcfg.py`: the path of the original, where its own code ends and the library begins, the list of
modules in link order, which compiler built it. `host/recon/README.md` explains the fields. The corpus does **not** use
this file; it has its own `corpus.py`.

---

## 4. The corpus: the idea

### Why a corpus at all?

Byte identity of the binaries is proven by `make -C WEGA/host checkall`: everything is built from `src/` and every
installed file is compared with the original. So why test behaviour as well?

Because byte identity only holds for the build with the **old compiler** the original was made with. The same sources
are also built a second time, with the **shipped** compiler of WEGA (`cc`, `scc`) and the shipped C library — the
"shipped build" (`make shipped` in `src/cmd`). That build is not byte-identical (another compiler, another library), so
it tests something else: that the source is *right*, not just that it happens to give the right bytes with one
particular compiler. For the C compiler chain there are also **host** versions (ports that run natively on Linux);
those must behave like the originals too.

The corpus answers one question: **does the rebuilt program do exactly what the original does, on many inputs?**

### The oracle

The originals are Z8000 programs. They run on Linux in the emulator `host/z8k-emu/z8k-emu`, which interprets Z8000
code and translates the WEGA system calls to Linux. The original therefore is the **oracle**: whatever it does is right
by definition — also its bugs. If the original has a bug, the rebuild must have the same bug.

### Original and candidates

A corpus runs the same **cases** on:

- the **original** – the program of the WEGA 3.1 file tree (`tmp/wega31_root`, e.g. `/bin/tar`), in the emulator;
- every **candidate** – a rebuilt program:
  - `wega`: the shipped build (`src/cmd/...`), also in the emulator;
  - `host`: the Linux port, run directly (only for the compiler chain).

Everything the program leaves behind is compared: what it prints, its exit status, and every file of its working
directory afterwards. If anything differs, the corpus reports the case.

### A case

A **case** is one small, self-contained test: a directory with starting files and a list of **steps**. A step runs one
program with arguments. Example (the case `mv-dir` of mv):

```
-- d1/f                  ← a starting file d1/f (empty)
-- case
step: mv d1 d2           ← run mv (original, or candidate)
expect: missing d1       ← what must be true of the original's run
expect: file d2/f contains ''
expect: exit 0
```

---

## 5. Running a corpus

### The normal way

```sh
host/tools/corpus.sh tar            # the corpus of tar, every candidate
host/tools/corpus.sh --wega tar     # only the shipped build
host/tools/corpus.sh --host cpp     # only the host build
host/tools/corpus.sh                # every corpus listed in compare/CORPORA
```

The candidates are taken from where `make` puts them (the paths in `corpus.py`, `CANDIDATES`). To test another binary:

```sh
CORPUS_BIN_WEGA=/some/where/tar host/tools/corpus.sh --wega tar
```

The output ends with one line per candidate:

```
tar [wega]: 34 of 34 inputs identical (0 sources skipped)
TIME tar      wall     0.4   34 cases
```

"sources skipped" counts generated cases the *original* tools could not even prepare (e.g. a C file the old compiler
rejects); those are not tests.

### Inside `checkall`

`make -C WEGA/host checkall WEGA=<WEGA_ROOT>` builds everything, then runs every corpus of `compare/CORPORA` in
parallel against the shipped build and the host builds. A line per candidate appears in its summary:

```
corpus tar [wega]            34 of 34 inputs identical
```

`make -C WEGA/host checkall-nocache` does the same but ignores every stored result (see "The store" below).

### Looking at the original: `--show`

When you write cases, you want to see what the original actually does. `--show` runs only the original, stores nothing
and needs no candidate:

```sh
python3 host/recon/corpusrun.py --show rmdir             # every case
python3 host/recon/corpusrun.py --show -f slash rmdir    # only cases whose name contains "slash"
```

```
=== slash-dot
    host 0: exit=0
    step 1 rmdir: exit=0
    files afterwards:
      d 755 d
    expectations: all hold (2)
```

### Options and environment

| Option / variable | Meaning |
|---|---|
| `-c wega` / `-c host` (`corpus.sh --wega/--host`) | only that candidate |
| `-f TEXT` | only cases whose name contains TEXT |
| `-k` | keep the work directories of differing cases (to look into them) |
| `--nocache`, `CORPUS_NOCACHE=1` | ignore the store, run everything again |
| `--show` | only the original, print every case |
| `CORPUS_BIN_WEGA`, `CORPUS_BIN_HOST` | another binary for that candidate |
| `JOBS` | how many cases run at the same time (default: number of CPUs − 1) |
| `WEGA_ROOT` | the WEGA 3.1 tree (default `../tmp/wega31_root` beside `WEGA/`) |
| `CORPUS_DB` | the store (default `WEGA/.corpus`) |
| `CORPUS_TMP` | where the work directories go (default `/tmp/wega_corpus`) |
| `CORPUS_OPTS` | options that `corpus.sh` passes on to `corpusrun.py` |

### The store (`WEGA/.corpus`)

Running the originals in the emulator is slow; a cold run of all corpora takes about a quarter of an hour. So results
are kept in `WEGA/.corpus/` (not in git):

- the **original's result** of every case, under a key made of: the case (its text and starting files), the variant,
  the original binary, the emulator, and the runner itself;
- an **"identical" mark** per case and candidate, under a key that adds the candidate binary.

If none of these changed, nothing runs: after a change of one program only that program's cases run again. In
addition, every run records which files of the WEGA tree it read through the emulator (the **measured dependencies**);
if one of those files changes, the result is made again.

The original is always run **twice**; only if both runs agree is the result kept. A case whose original gives different
results each time is reported as `UNSTABLE` — it would not be a fair test.

---

## 6. How a case runs, step by step

Knowing this helps to understand every message of the runner.

1. **Starting files.** The case's starting files are copied into a fresh directory, one per side:
   `CORPUS_TMP/corpusrun/NAME/orig/CASE.0`, `.../wega/CASE.0`, `.../host/CASE.0`. All sides use a directory **of the
   same name**, so that a program that prints or stores its working directory gives the same bytes everywhere.
2. **The candidate** is copied *beside* that directory (`CASE.0.prog/`), never into it, so a program that lists its
   own directory does not see it.
3. **The steps run in order**, each in that directory, with `umask 022`:
   - `step: PROG ARGS` – on the original side `z8k-emu -r WEGA_ROOT -0 PROG /ORIG ARGS` (e.g. `/bin/tar`), on the `wega`
     side the same with the candidate, on the `host` side the Linux binary directly. `argv[0]` is the word as typed (`-0`), as a shell
     would give it.
   - `step: OTHER ARGS` (a program that is not the one under test, e.g. `step: ls -l`) – the original `/bin/OTHER` of
     the WEGA tree on *every* side: it is a tool of the case, not tested.
   - `host: LINE` – a line of the Linux shell (`sh -c`), for preparation: making files, setting times and modes.
4. **Recorded** per step: stdout, then stderr (marked `-- stderr`), and the exit status. After the last step: every
   entry of the directory (type, mode, size, checksum, link count – not owner and not times).
5. **Expectations** (`expect:` lines) are checked on the original's run.
6. **Compared:** the record of every candidate against the record of the original.

The record of a case looks like this (what `--show` prints, and what a `DIFF` line refers to):

```
host 0: exit=0
step 1 tar: exit=0
step 2 tar: exit=0
f1
f2
-- stderr
tar: blocksize = 6
files afterwards:
  f 644 3072 c9110ef5 1 a.tar        ← type f(ile), mode, size, crc32, links, name
  f 664 4 6b9c2d9a 1 f1
  d 755 sub                          ← type d(irectory), mode, name
```

---

## 7. Three ways to write cases

All three produce the same kind of case; pick what fits the program.

### a) `tests.txt`: many small cases in one file (recommended for commands)

Used by dc, rm, rmdir, mv, tar. One text file, one block per case. The format is read by `host/recon/casefile.py`.

```
== interactive
# man: -i asks for every file; a line starting with y removes it
-- a
-- b
-- answers
y
n
-- case
step: rm -i a b
stdin: answers
expect: output is 'a: b: '
expect: missing a
expect: file b contains ''
```

Rules of a block:

- `== NAME` starts a case. Keep names short (they become directory names).
- The lines after `== NAME` up to the first `-- ` line are the default input file (`t.in`, or what the corpus says,
  e.g. `t.dc` for dc). Most cases use named files instead.
- `-- FILE` – the following lines are the file FILE (a path like `d/sub/f` makes the directories). A `-- FILE` with no
  lines is an empty file.
- `-- case` – the following lines are case-file lines (section 8): `host:`, `step:`, `stdin:`, `expect:` …
- `-- expect` – the following lines are the **exact** whole output of the last step (becomes `expect: output is …`).
- A line starting with `#` is a comment of `tests.txt` (dc itself has no comments).
- `\ooo` in a file line is the byte with that octal value. Use it for bytes that an editor would lose: `\040` for a
  space at the end of a line, `\011` for a tab, `\344` for a byte above 127.
- Without any `step:` the corpus's default step is added.

The `corpus.py` of such a corpus is tiny (the whole one of rmdir):

```python
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'rmdir'
ORIG = 'bin/rmdir'
PROG = 'rmdir'
CANDIDATES = {'wega': 'src/cmd/rmdir'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'rmdir t.in')
```

### b) `cases/CASE/`: one directory per case

Used by ar. Good when a case needs real binary starting files (put them into `cases/CASE/in/`) or is very long.

```
compare/ar/cases/libc.a/case
```
```
# ar: the table of contents of libc.a of the system
host: cp $WEGA_ROOT/lib/libc.a .
step: ar tv libc.a
variant:
variant: TZ=MET-1MEST
variant: TZ=PST8PDT
expect: exit 0
expect: output contains doprnt.o
```

The runner finds these directories by itself; `corpus.py` needs no `generate()` for them.

### c) A generator in `corpus.py`: cases made from the tree

Used by the compiler passes, the linkers, cas. A function `generate(g)` produces cases from existing material, for
example every C source of the tree. A case can have a **preparation**: a Python function that makes the starting files,
often by running *original* tools (the original preprocessor makes `t.i` for the test of `cparse`). Prepared inputs are
stored too (`.corpus/inputs/`), so a preparation runs only once until its sources change.

The corpus of cpp (shortened):

```python
import corpusc   # host/recon/corpusc.py: helpers for C sources

NAME = 'cpp'
ORIG = 'lib/cpp'
PROG = 'cpp'
CANDIDATES = {'wega': 'src/cmd/cpp/cpp', 'host': 'host/cpp/cpp'}

def generate(g):
    for src in g.c_sources():                       # every .c of src/cmd and src/lib
        b = os.path.basename(src)
        yield g.case(src,                           # made from this file (and its directory)
                     corpusc.prepare_cpp,           # preparation: copy the directory
                     'step: cpp -I. %s t.i\n' % b)  # the case text
```

What `g` offers:

| Call | Meaning |
|---|---|
| `g.c_sources()` | every C source git knows below `src/cmd` and `src/lib` |
| `g.case(src, prep, text, prep_files=(), name=None, sources=None)` | a case: `prep(src, workdir)` makes the starting files (returns `None` if the original tools cannot – the case is then *skipped*); `text` is the case file; `name` defaults to the path of `src`; `sources` are the files the preparation reads (default: the files of `src`'s directory) – they go into the key |
| `g.name(src)` | the name of a case made from `src` |
| `g.dir_files(src)` | the files git knows in the directory of `src` |

Variants of options: popt runs each case three times, plain, with `-l` and with `-d`:

```
step: popt $V in.s out.s
variant:
variant: -l
variant: -d
```

`$V` is replaced by the words of the variant line (an empty `variant:` line means: no words).

---

## 8. Reference: the case file

One field per line; a line starting with `#` is a comment.

| Field | Meaning |
|---|---|
| `step: WORDS` | Run a WEGA program in the emulator. Words are split like a shell does (quotes work), but there is **no shell**: no `>`, `|`, `;`, `$(...)`. A word with `*`, `?`, `[` is expanded in the case directory. `$V` is the words of the variant. The first word is `PROG` (the program under test) or a name of `ALIASES`; any other first word runs the original `/bin/WORD` on every side. |
| `stdin: FILE` | The standard input of the step before (a file of the case directory). Without it, stdin is empty. |
| `env: NAME=VALUE` | Environment of the step before; before the first step: of every step. |
| `host: LINE` | A line of the Linux shell between steps (`sh -c`): preparation, `chmod`, `touch -d`, `ln`. `$WEGA_ROOT` is set. Its output and exit status are recorded too. |
| `variant: NAME=VALUE … WORD …` | The case runs once per `variant:` line: `NAME=VALUE` pairs go into the environment, other words into `$V`. No `variant:` line: the case runs once. |
| `expect: exit N` | The last step ends with status N. |
| `expect: output contains TEXT` | Some step's output contains TEXT (quote it if it has blanks: `'unable to open'`). |
| `expect: output is TEXT` | The whole output of the last step is TEXT (`\n` for a newline). Stderr appears after `-- stderr\n`. |
| `expect: file PATH contains TEXT` | The file exists afterwards and contains TEXT (`''`: it just exists). |
| `expect: file PATH same-as OTHER` | Two files of the directory are equal (or `in/FILE` of a hand-written case). |
| `expect: missing PATH` | The file is gone afterwards. |
| `user:` `terminal:` `device:` `time:` `fs:` `signal:` | Reserved for later emulator models; using them is an error until the model exists. |

Expectations are checked on the **original**. They prove that a case really tests what it claims (more in section 11).

---

## 9. Reference: `corpus.py`

| Name | Required | Meaning |
|---|---|---|
| `NAME` | yes | the name of the corpus |
| `ORIG` | yes | the original, a path in the WEGA tree without the leading `/` (`'bin/tar'`) |
| `PROG` | yes | the word that means "the program under test" in `step:` lines |
| `CANDIDATES` | yes | `{'wega': 'src/cmd/tar'}` – path of each rebuilt binary, relative to `WEGA/` |
| `ALIASES` | | one program, several names: `{'cp': 'bin/cp', 'ln': 'bin/ln'}` (word → original). The candidate runs under the same name (mv chooses its work by its name). |
| `generate(g)` | | yields generated cases (section 7c, or `casefile.generate` for `tests.txt`) |
| `normalize(what, data)` | | makes undefined parts equal before comparing: `what` is `'output'` (a step's output, a string) or a file name (its bytes). dc uses it for addresses its debug command prints. |
| `after_step(argv, rc, out, err, dir)` | | runs after every step, may cut output or remove files the original leaves undefined; returns `(out, err)`. cas uses it: after an error the original leaves garbage files. |
| `TIMEOUT` | | seconds per step (default 600); a step that runs longer ends with exit status `-999`. |
| `IN_TMP` | | `True`: the case directory is the guest's `/tmp/CASE` (needed when a program asks `/bin/pwd` where it is, like tar). |
| `ROOT_FILES` | | files of the WEGA tree read *outside* the emulator (e.g. copied by a `host:` line); they go into the key. Reads through the emulator are measured anyway. |
| `ROOT_ENV` | | the original lives in another tree, named by this environment variable (casold: `OLDROOT`). |
| `DEPENDS` | | `{'host': ['host/ar']}` – directories whose contents a candidate depends on (programs it starts). |
| `HOST_ENV`, `host_setup(dir)` | | environment and setup for host candidates (e.g. where the host `cpp` finds the include files). |

---

## 10. Tutorial: a corpus from scratch

This walks through making a corpus for a small command. We use `rmdir` (it exists already, so you can compare your
result with `compare/rmdir/`).

### Step 1: know the program

Read the **manual page** (in the WEGA tree, read it with the original `pcat`):

```sh
cd WEGA
host/z8k-emu/z8k-emu -r ../tmp/wega31_root /usr/bin/pcat /usr/man/man1/rm.1    # rmdir is described in rm(1)
```

Then read the **source** (`src/cmd/rmdir.c`). Write down:

- everything the manual promises (each option, each form of call),
- every message the source can print, and how to reach it,
- everything odd: limits (buffer sizes), unusual tests, places where the source looks wrong.

For rmdir the source shows, among others: a usage message with exit status 2; "non-existent", "not a directory",
"not empty", "unreadable", "no permission", "cannot remove current directory", "cannot remove . or .."; and a
peculiar test for `.` and `..` (it looks at `strrchr(name, '/')`, which points at the slash, so `rmdir d/.` slips through).

### Step 2: make the directory and `corpus.py`

```sh
mkdir compare/rmdir
```

Copy the small `corpus.py` from section 7a and change `NAME`, `ORIG`, `PROG`, `CANDIDATES`. The candidate path is where
`make shipped` in `src/cmd` puts the binary.

### Step 3: write the first cases

`compare/rmdir/tests.txt`:

```
== empty
# man: rmdir removes the named directories, which must be empty
-- case
host: mkdir a b
step: rmdir a b
expect: missing a
expect: missing b
expect: exit 0

== not-empty
# src: "rmdir: NAME not empty" on the standard error, exit status 2
-- d/f
-- case
step: rmdir d
expect: output contains 'rmdir: d not empty'
expect: exit 2
```

Write the expectations **from the manual page and the source** – before you run anything.

### Step 4: look at the original

```sh
python3 host/recon/corpusrun.py --show rmdir
```

For every case, check what the original did against what you expected:

- **Expectation holds** – fine.
- **Expectation fails** – find out why *in the source*. Either your expectation was wrong (fix it, and write in the
  comment what you learned), or the original has a surprising behaviour or a bug (keep the case, the original is
  right, and record the bug in the table "Bugs and quirks of the originals" in `host/KNOWLEDGE.md`).
- Never copy the original's output into an expectation without understanding it. Then the expectation would prove
  nothing.

### Step 5: run against the candidate

Build the shipped program (`checkall` does it, or `make shipped` in `src/cmd` with the WEGA tools), then:

```sh
host/tools/corpus.sh rmdir
```

```
rmdir [wega]: 13 of 13 inputs identical (0 sources skipped)
```

If a case differs, you see a `DIFF` line (section 13). A difference is a finding: the rebuilt program does not behave
like the original. It is fixed on *our* side.

### Step 6: prove the corpus finds mistakes (mutation test)

A corpus that never fails is worthless. Make a deliberately broken copy of the program, build it, and run the corpus on
it: it must report differences. For rmdir, change `exit(Errors? 2: 0)` to `exit(Errors? 1: 0)`:

```sh
CORPUS_BIN_WEGA=/path/to/broken/rmdir host/tools/corpus.sh rmdir
```
```
DIFF mixed: [wega] -step 1 rmdir: exit=2 | +step 1 rmdir: exit=1
rmdir [wega]: 5 of 13 inputs identical (0 sources skipped)
```

Do not commit the broken copy.

### Step 7: add it to checkall

Add a line `rmdir` to `compare/CORPORA`. Run `make -C WEGA/host checkall WEGA=<WEGA_ROOT>`; it must end with `ALL OK`.

---

## 11. Writing good cases

### Every case must test something real

An emulator that lacks a feature makes the original and the candidate fail **the same way** – and the corpus says
"identical", although nothing was tested. Example from history: a case of ar whose shell loop had a typing error made
no archive on either side; both sides were "equal". Only an expectation (`expect: file x.a contains …`) revealed it.
So: **every case gets at least one expectation** that shows the original really did the thing the case is about.

### Expectations come from the manual and the source

Not from a run of the original. When `--show` disagrees with you, the source decides who is right. Write what you
learned into the case's comment, e.g.:

```
# src: % is the remainder of the quotient computed with the scale k (with 2k, 7.5/2 = 3.75 leaves 0)
```

### Cover both sides of the program

- **The public interface**: every option and form of call in the manual page, the examples of the manual, every
  diagnostic it lists.
- **What the source has besides**: hidden options, every message, limits, odd paths. Mark each case in its comment
  with `man:` or `src:` and the function it is about.

### Make cases deterministic

The original runs twice and once per candidate, at different moments and in different directories. Everything that
changes between runs makes a case unstable or a false difference:

| Danger | Remedy |
|---|---|
| **Times.** A file made by a `host:` line has the current time; programs that store times (tar) then differ. | Set times: `host: touch -d '1988-03-04 05:06:07 UTC' FILE`. The tar corpus does it for every file of every case. |
| **The order of directory entries** depends on the file system. | Both sides run on the same file system with the same names, so the order is equal; just do not write expectations that assume an order. |
| **Process ids** (temporary file names). | Do not let them into the output; use `normalize()` if a program prints them. |
| **Leftover memory** (uninitialised buffers written to files). | All sides use the same directory name; the rest must be equal by construction or normalised. |
| **Addresses** printed by debug commands. | `normalize()` (dc does this for `Y`). |
| **Blanks at line ends** in `tests.txt`. | Editors remove them: use `\040`. |

### What must not become a case

Behaviour that depends on the **memory layout of the binary** – writing past an array, reading uninitialised memory
that differs in every build. The shipped build is compiled differently and cannot show the same garbage. Such
behaviour is recorded as a bug in `host/KNOWLEDGE.md` and named in the head comment of `corpus.py` ("Not in the
corpus: …").

Behaviour the emulator cannot provide yet (a terminal, a device) is also listed there, until the model exists.

### Bugs of the original are kept

If the original has a bug, the case stays and the expected bug is the expected result. Example: dc loops for ever on a
number with two decimal points (`1.2.3`); the case `two-points` expects exactly that (`expect: exit -999`, the timeout).

---

## 12. What the emulator can and cannot do

The emulator (`host/z8k-emu/`, details in its `README.md`) runs a WEGA program as an ordinary Linux user:

**It provides:** arguments, environment, files below the WEGA tree and the working directory, `fork`/`exec`/`wait`,
pipes, directories in the WEGA format, a fixed time on request (`Z8K_TIME`), the guest `/tmp` as its own directory,
and some models built for corpora: a link to a directory (`mv dir1 dir2`), and the guest `/tmp` as a mount so that
`/bin/pwd` works (`IN_TMP`).

**It does not provide (yet):** a terminal (`isatty` is always false – `rm` therefore always acts as with `-f`), devices
(`/dev/...` goes to the Linux host), other users (everything runs as you, not as root), mounting file systems, guest
signal handlers. A program that needs one of these needs a **model** in the emulator first. Following the project's
rule, a model is built when the first program needs it, with tests of its own. The case file already reserves the
fields for it (`terminal:`, `device:`, …).

---

## 13. Troubleshooting

**`DIFF NAME: [wega] -line | +line`** – the candidate differs from the original in this case. Lines with `-` are the
original's, lines with `+` the candidate's (the first few differing lines are shown). Run with `-k` to keep the work
directories (`CORPUS_TMP/corpusrun/NAME/wega/CASE.0`) and look at the files.

**`DIFF NAME: EXPECT failed on the original: …`** – your expectation does not hold for the original. Look with
`--show`, then read the source.

**`DIFF NAME: UNSTABLE original: …`** – the original gave two different results. Something is not deterministic
(see the table in section 11). Such a case is never stored.

**`NAME: /path (wega) not built`** – the candidate binary does not exist. Build it, or name one with
`CORPUS_BIN_WEGA`.

**A step ends with `exit=-999` and `timeout`** – it ran longer than `TIMEOUT`. A loop in the case? (In dc, `[lyx]sy lyx`
loops for ever: a tail call does not nest.)

**`CASE …: unknown field` / `unknown expect`** – a typing error in the case, or a quoting problem: `expect: output
contains unable to open` has too many words; write `'unable to open'`.

**Results look stale** – use `--nocache` once. The keys cover the case, the binaries, the emulator, the runner and the
measured files, so this should be rare; if it happens, it is a fault worth reporting.

---

## 14. Glossary

| Term | Meaning |
|---|---|
| **original** | The program as shipped in WEGA 3.1 (`tmp/wega31_root`). |
| **oracle** | The original running in the emulator; what it does is right by definition. |
| **candidate** | A rebuilt program tested against the original: `wega` (shipped build) or `host` (Linux port). |
| **shipped build** | `src/cmd` built with the WEGA `cc`/`scc` and the WEGA C library (`make shipped`), as opposed to the byte-identical build with the old compilers. |
| **case** | One test: starting files, steps, expectations. |
| **step** | One run of a program inside a case. |
| **variant** | The same case once more with other environment or options. |
| **expectation** | A statement about the original's run that must hold (`expect:`). |
| **store** | `WEGA/.corpus/`, where results and prepared inputs are kept. |
| **measured dependencies** | The files of the WEGA tree a run read, recorded by the emulator (`Z8K_ROOTLOG`). |
| **checkall** | `make -C WEGA/host checkall`: builds everything, compares every installed file with the original, runs every corpus. |
| **model** | A part of the emulator that imitates something of the WEGA system the host does not have the same way (directory links, `/tmp` as a mount; later a terminal, devices). |

See also: `host/recon/corpusrun.py` (the runner; its head comment is the formal reference), `host/recon/casefile.py`
(the `tests.txt` format), `host/increments/110-corpus-architecture.md` (the design and its decisions),
`host/z8k-emu/README.md` (the emulator).
