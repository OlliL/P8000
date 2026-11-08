# host/recon: shared tools for reconstructing a binary

Used by the programs of `src/cmd` with a `tools/` directory. The layout of such a directory is the same for all of them:

- `progcfg.py`: the configuration (NAME, ORIG, OWN_LIM, ... see `progcfg.py` here) and the corpus (CORPUS_*);
- `findings.json`: per function file, orig, size_orig, status, and a note only for an odd source form, a guessed name,
  dead code; `FINDINGS.md` is generated from it (`findings.py NAME`);
- `SYSIII.md` (where it exists): what the source has that System III has not, what is WEGA's.

Nothing in `tools/` runs: the scripts are here and take the program by name (`RECON_PROG=NAME`, a name or a
directory); `host/tools/corpus.sh NAME` runs the corpus of a program, `oldcc.sh` the work on the old compilers. No
Makefile of `src` refers to `tools/` or `host/`, so `src` can go to a WEGA system without them.

| Script | Purpose |
|---|---|
| `progcfg.py` | loads the program configuration |
| `findings.py PROG` | `findings.json`: `compiler`, `measured_at`, per function `file`, `orig`, `size_orig`, `status` from strictcmp, `note` by hand, writes `FINDINGS.md`; through `oldcc.sh findings PROG` |
| `odis.py NAME` / `odis.py LO HI` | disassemble the original code of a function (NAME: orig and size_orig in findings.json) |
| `callcmp.py OLD NEW NM` | call-trace comparison of two emulator traces (the functions of findings.json) |
| `zcode.py` | instruction lists and the two normal forms (masked, strict) of the comparison |
| `strictcmp.py [-m] [-v NAME]` | (`STRICT_CTX=n`: lines of context for -v) every function of our linked program against the original; strict: only code addresses masked (the authority; `-m` is the old masked form) |
| `fncheck.py DRAFT LO HI` | a draft compiled alone against a range of the original (masked, a first look) |
| `d12model.py` | our modules linked under the data layout of the old cc (strings at the end of each file, unpadded); data compared; the input of strictcmp |
| `rename.py OLD NEW ...` | rename everywhere: sources, tables, findings.json, the .md files of the tools and of the project |
| `inventory.py`, `own_table.py` | function starts and call graph of an original (nonsegmented or segmented), the first `findings.json` |
| `datemap.py [-m] [WEGA]` | the build dates of all original programs (crt0 stamp, own SCCS ids, libc marks, D12 read from the data), the input of `host/OLDCC_DATES.md` |
| `witness.py [-c ARGS] [PROG ...]` | test bench of the old compiler v1: the System III sources (SYS3SRC, not in git) of 26 WEGA programs of the v1 time compiled with ccv1 (or another compiler) and aligned with the originals; equal functions per program, compared with and without a switch |
| `oldvar.sh NAME PATCH.py [PASS]` | a change of a pass of ccv0p measured in a copy: the three models with it, the functions that changed against the last `oldcc.sh model` |
| `poptrace.sh PROG FUNC` | what popt does with one function: the nodes after each iteration, the blocks moved by codemove |
| `stagecorpus.py` | corpus test of a program (through `host/tools/corpus.sh NAME`): original and ours on the same inputs, made fresh by `corpusc.py` from the C sources (or the command lines of `host/ld/check.py` for ld, sld); outputs, exit status, messages |
| `corpusc.py` | the inputs of the corpus of the compiler passes: the C sources and what the original earlier passes make of them |
| `oldcc.sh` | the work on the old compilers: model, lines, findings, matrix, bench |

Popt trace: build popt v0p (`src/cmd/popt/build/nonsegv0p`) with `-DTRACE` in addition to its switches, run it under the emulator as `popt + in.s out.s` on the codgen output of one function (`ccv0p -S` without -O, cut to the function): out.s holds the node list after every iterate and comjump round and every block move (`/*CM ...*/`).
