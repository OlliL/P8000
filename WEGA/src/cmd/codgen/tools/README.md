# codgen analysis tools

Not part of the codgen build. The single source of truth for the analysis is `findings.json`.

Run everything from the directory `src/cmd/codgen` (after `make` has built `./codgen`):

    O=$WEGA/lib/codgen                                  # the original, /lib/codgen of WEGA
    python3 tools/locate.py  --mine ./codgen --check    # reproduction test: our functions found by name
    python3 tools/confirm.py --mine ./codgen --orig $O  # checks the stored pairings, writes ours/pairing/level
    python3 tools/patterns.py --mine ./codgen --orig $O # which instructions differ, D patterns (D1-D14 by form)
    python3 tools/classify.py --mine ./codgen --orig $O # writes the difference class per function
    python3 tools/inventory.py                          # writes INVENTORY.md
    sh tools/check.sh <corpus dir>                      # codgen against /lib/codgen on a test corpus (WEGA=...)

Files:
- `funcs.py`      library: disassembly, function extents, strict-1 comparison, original candidates
- `locate.py`     finds our functions by name in a built binary (object markers, source order)
- `confirm.py`    checks the pairings in findings.json (no discovery; decisions are stored there)
- `patterns.py`   pattern check of the D classes (instruction forms that still differ)
- `classify.py`   writes the difference classes (alignment, overrides in findings.json)
- `inventory.py`  writes INVENTORY.md from findings.json
- `findings.json` decisions, reviews, classes, findings (the one source)
- `check.sh`, `corpus.sh`  test of codgen against the original (corpus built with the WEGA tools)
