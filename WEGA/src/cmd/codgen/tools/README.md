# codgen analysis tools

Not part of the codgen build. The state of the functions is in `findings.json`, the report in `FINDINGS.md` (generated).

Run through `src/cmd/cparse/tools/oldcc.sh`: `model codgen`, `lines codgen`, `findings codgen`.

In `src/cmd/codgen`:

- `python3 tools/strictcmp.py [-c|-a|-m] [-v NAME]`: the functions of the model against /lib/codgen.
- `python3 tools/olist.py NAME`: the instructions of a function of the original.
- `python3 tools/d12model.py`: builds the model and shows the data differences.
- `tools/corpus.sh SRCDIR OUTDIR`, `tools/check.sh CORPUSDIR`: the corpus of the passes, made with the original tools, and codgen (`PROG=scodgen`: scodgen) of the normal build against the original.

Build `ccv0p` from these sources only when the normal build passes the corpus: this source is also the measuring tool (`host/OLDCC_LOG.md`, 2026-11-02).
