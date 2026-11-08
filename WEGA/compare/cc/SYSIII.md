# cc against System III

`/bin/cc` and `/bin/scc` of WEGA (one source, `scc` with `-DSCC`) against `cmd/cc/pcc/cc.c` of System III (`pdp11/cc.c`, `vax/cc.c` are the same plan). The WEGA driver was reconstructed from the binaries and follows them in its odd constructs (head of cc.c); the passes and options are others than those of System III (cpp > cparse > codgen > popt > cas > ld). Measured 2026-11-07.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Of |
|---|---|---:|---:|
| cc.c | cc/pcc/cc.c | 279 | 1013 |

## WEGA's own

The whole program: nothing to put in System III form beyond what the bytes allow.
