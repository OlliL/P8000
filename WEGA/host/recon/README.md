# host/recon: shared tools for reconstructing a binary

Used by codgen, popt, sld, ld and cparse (see `host/HOWTO_reconstruct.md`). The tools of the old way (D classes against the shipped cc) are not here: sld and ld keep what they still use in their `tools/` (`classify.py`, `patterns.py`, `funcs.py`, `findings_md.py`) until they get their old compiler; then they go. Each program keeps in its `tools/` directory:

- `progcfg.py`: the configuration (NAME, ORIG, OWN_LIM, OWN_OBJ, ORIG_LIM, ORIG_BIN, see `host/recon/progcfg.py`);
- launchers with the old script names, which set `RECON_PROG` and run the script from here.
- the program-specific files (function table `own_functions.txt`, `findings.json`, locate/pair/confirm, ...).

| Script | Purpose |
|---|---|
| `progcfg.py` | loads the program configuration |
| `findings.py PROG` | new form of `findings.json` (HOWTO, "Since 2026-11-02"): status from strictcmp, writes `FINDINGS.md`; through `oldcc.sh findings PROG` |
| `odis.py NAME` / `odis.py LO HI` | disassemble the original code of a function |
| `callcmp.py OLD NEW NM` | call-trace comparison of two emulator traces (corpus test) |
| `zcode.py` | instruction lists and the two normal forms (masked, strict) of the comparison |
| `strictcmp.py [-m] [-v NAME]` | (`STRICT_CTX=n`: lines of context for -v) every function of our linked program against the original; strict: only code addresses masked (the authority; `-m` is the old masked form) |
| `fncheck.py DRAFT LO HI` | a draft compiled alone against a range of the original (masked, a first look) |
| `d12model.py` | our modules linked under the data layout of the old cc (strings at the end of each file, unpadded); data compared; the input of strictcmp |
| `rename.py OLD NEW ...` | rename everywhere: sources, tables, findings.json, the .md files of the tools and of the project |
| `inventory.py`, `own_table.py` | function starts and call graph of a nonsegmented original, the table own_functions.txt |
| `datemap.py [-m] [WEGA]` | the build dates of all original programs (crt0 stamp, own SCCS ids, libc marks, D12 read from the data), the input of `host/OLDCC_DATES.md` |
| `witness.py [-c ARGS] [PROG ...]` | test bench of the old compiler v1: the System III sources (SYS3SRC, not in git) of 26 WEGA programs of the v1 time compiled with ccv1 (or another compiler) and aligned with the originals; equal functions per program, compared with and without a switch |
| `castrig/` | the early jump optimization of the old cas: trigger points from an original (`windows.py`), table offsets and fits (`offsets.py`, `fit.py`, `reset.py`, `memsum.py`), test cas (`hostcas.patch`, `triggers.txt`) |
| `oldvar.sh NAME PATCH.py [PASS]` | a change of a pass of ccv0p measured in a copy: the three models with it, the functions that changed against the last `oldcc.sh model` |
| `poptrace.sh PROG FUNC` | what popt does with one function: the nodes after each iteration, the blocks moved by codemove |
| `stagecorpus.py OURS` | corpus test of a compiler pass: original and ours on the same inputs, outputs, exit status, messages |

Still in `src/cmd/sld/tools` (sld specific so far, to be moved when the next binary needs them): `fncheck.py`,
`strict_audit.py`, `accept.py`, `accfn.py`, `rename.py`, `firstdiff.py`, `sync_findings.py`, `check_names.py`,
`classify_sld.py`, the corpus runner `host/ld/check.py`.

Moved from `src/cmd/ld/tools` (2026-10-30): strictcmp (with wholecmp as `-m`), fncheck (ldcheck), d12model, rename,
inventory, own_table; ld keeps launchers with the old names, and its results are unchanged (d12model 2694 equal data
bytes, strictcmp 51 exact / 8, inventory identical). The progcfg settings they read are listed in `progcfg.py`.

Popt trace: build `src/cmd/popt/v0p` with `-DTRACE` in addition to its switches, run it under the emulator as `popt + in.s out.s` on the codgen output of one function (`ccv0p -S` without -O, cut to the function): out.s holds the node list after every iterate and comjump round and every block move (`/*CM ...*/`).
