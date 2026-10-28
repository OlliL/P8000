# host/recon: shared tools for reconstructing a binary

Used by codgen, popt and sld (see `host/HOWTO_reconstruct.md`). Each program keeps in its `tools/` directory:

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

Still in `src/cmd/sld/tools` (sld specific so far, to be moved when the next binary needs them): `fncheck.py`,
`strict_audit.py`, `accept.py`, `accfn.py`, `rename.py`, `firstdiff.py`, `sync_findings.py`, `check_names.py`,
`classify_sld.py`, the corpus runner `host/ld/check.py`.

Test (2026-10-28): `findings_md.py` reproduces the three `FINDINGS.md` files unchanged; `classify.py`, `patterns.py`,
`confirm.py`, `pair.py` run on codgen and popt; sld `strict_audit.py` still 58 exact.
