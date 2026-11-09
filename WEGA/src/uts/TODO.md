# uts: TODO

Goal: `make install` in `src/` writes `boot`, `wega`, `usr/boot` and `usr/sys/...` below PREFIX, and `host/tools/checkall.sh`
finds every one of them byte-identical (`cmp`) with the WEGA root. Everything below is ordered towards that goal.
Rule: finished (sub)items are deleted from this file, so it gets shorter with every step.
`uts` is built by `all` in `src/Makefile` but NOT by `install` (and not added to `checkall.sh`) until `wega`, `LIB1`, `LIB2` and the rest are
identical: before that `checkall.sh` would stop at the first difference. The install targets in `uts/*/Makefile` are written and tested by hand
with `make install PREFIX=dir`; they are connected to `src/Makefile install` at the very end (2.).
State measured with the build in a copy of `src/uts` (tools from `checkall.sh`), objects against `tmp/wega31_kernel`.

## State

`make install PREFIX=dir` (by hand) writes 31 files below `usr/sys`, `boot`, `usr/boot` and `wega`: all identical with the root (`cmp`). The compiler of the kernel objects is `sccvk` (`host/OLDCC.md`, section 2).

## 1. Build and install

1. `COVERAGE.md`: regenerate (`host/tools/coverage.py`) when files become exact.

## 2. At the end

1. Connect `uts` to `install` in `src/Makefile`; `checkall.sh -b`: `boot`, `wega`, `usr/boot`, `usr/sys/...` all identical, nothing in OPEN for uts.
2. Update `README.md` (what is still object/assembler), `COVERAGE.md`, the tables in `host/OLDCC.md` and `host/KNOWLEDGE.md`, `HANDOFF_wega_disasm.md`.
