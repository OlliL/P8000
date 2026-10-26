# codgen analysis tools

Not part of the codgen build. The single source of truth for the analysis is `findings.json`.

Run from `src/cmd/codgen`:

    make -C tools pipeline WEGA=$WEGA CC="<compiler command>"   # all steps, builds ./codgen first
    make -C tools check WEGA=$WEGA CORPUS=<corpus dir>      # codgen against the original on a corpus

Or step by step (the same commands as the Makefile runs):

    O=$WEGA/lib/codgen                                  # the original, /lib/codgen of WEGA
    python3 tools/locate.py  --mine ./codgen --check    # reproduction test: our functions found by name
    python3 tools/confirm.py --mine ./codgen --orig $O  # checks the stored pairings, writes ours/pairing/level
    python3 tools/patterns.py --mine ./codgen --orig $O # which instructions differ, D patterns (D1-D14 by form)
    python3 tools/classify.py --mine ./codgen --orig $O # writes the difference class per function
    python3 tools/inventory.py                          # writes INVENTORY.md

Files:
- `funcs.py`      library: disassembly, function extents, strict-1 comparison, original candidates
- `locate.py`     finds our functions by name in a built binary (object markers, source order)
- `confirm.py`    checks the pairings in findings.json (no discovery; decisions are stored there)
- `patterns.py`   pattern check of the D classes (instruction forms that still differ); also used by classify.py
- `Makefile`      the pipeline and the corpus check (the codgen Makefile contains only the build)
- `classify.py`   writes the difference classes (alignment, overrides in findings.json)
- `inventory.py`  writes INVENTORY.md from findings.json
- `findings.json` decisions, reviews, classes, findings (the one source)
- `check.sh`, `corpus.sh`  test of codgen against the original (corpus built with the WEGA tools)
