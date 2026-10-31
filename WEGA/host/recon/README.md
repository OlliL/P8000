# host/recon: shared tools for reconstructing a binary

Used by codgen, popt, sld, ld and cparse (see `host/HOWTO_reconstruct.md`). Each program keeps in its `tools/` directory:

- `progcfg.py`: the configuration (NAME, ORIG, OWN_LIM, OWN_OBJ, ORIG_LIM, ORIG_BIN, see `host/recon/progcfg.py`);
- launchers with the old script names (`classify.py`, `findings_md.py`, `odis.py`, `callcmp.py`), which set
  `RECON_PROG` and run the script from here; `funcs.py` and `patterns.py` are import shims (`import funcs as F`).
- the program-specific files (function table `own_functions.txt`, `findings.json`, locate/pair/confirm, ...).

| Script | Purpose |
|---|---|
| `progcfg.py` | loads the program configuration |
| `funcs.py` | disassembly, strict instruction form, comparison, function candidates |
| `patterns.py` | check of the D patterns (compiler differences) of a differing function |
| `classify.py` | difference class of every function, written to `findings.json` |
| `findings_md.py` | `findings.json` to `FINDINGS.md` (generated, never edited by hand) |
| `odis.py NAME` / `odis.py LO HI` | disassemble the original code of a function |
| `callcmp.py OLD NEW NM` | call-trace comparison of two emulator traces (corpus test) |
| `zcode.py` | instruction lists and the two normal forms (masked, strict) of the comparison |
| `strictcmp.py [-m] [-v NAME]` | every function of our linked program against the original; strict: only code addresses masked (the authority; `-m` is the old masked form) |
| `strictlayers.py [-w] [-v NAME]` | the strict difference of every function in layers (as it is, frame hidden, frame and registers hidden) and the shapes of the hunks that remain (candidate D classes by form, else "open"); `-w` writes `strict_layers` into findings.json (shown as a column of FINDINGS.md) |
| `fncheck.py DRAFT LO HI` | a draft compiled alone against a range of the original (masked, a first look) |
| `d12model.py` | our modules linked under the data layout of the old cc (strings at the end of each file, unpadded); data compared; the input of strictcmp |
| `rename.py OLD NEW ...` | rename everywhere: sources, tables, findings.json, the .md files of the tools and of the project |
| `inventory.py`, `own_table.py` | function starts and call graph of a nonsegmented original, the table own_functions.txt |
| `witness.py [-c ARGS] [PROG ...]` | test bench of the old compiler v1: the System III sources (SYS3SRC, not in git) of 26 WEGA programs of the v1 time compiled with ccv1 (or another compiler) and aligned with the originals; equal functions per program, compared with and without a switch |
| `stagecorpus.py OURS` | corpus test of a compiler pass: original and ours on the same inputs, outputs, exit status, messages |

Still in `src/cmd/sld/tools` (sld specific so far, to be moved when the next binary needs them): `fncheck.py`,
`strict_audit.py`, `accept.py`, `accfn.py`, `rename.py`, `firstdiff.py`, `sync_findings.py`, `check_names.py`,
`classify_sld.py`, the corpus runner `host/ld/check.py`.

Test (2026-10-28): `findings_md.py` reproduces the three `FINDINGS.md` files unchanged; `classify.py`, `patterns.py`,
`confirm.py`, `pair.py` run on codgen and popt; sld `strict_audit.py` still 58 exact.

Moved from `src/cmd/ld/tools` (2026-10-30): strictcmp (with wholecmp as `-m`), fncheck (ldcheck), d12model, rename,
inventory, own_table; ld keeps launchers with the old names, and its results are unchanged (d12model 2694 equal data
bytes, strictcmp 51 exact / 8, inventory identical). The progcfg settings they read are listed in `progcfg.py`.
