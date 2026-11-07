# dc against System III and Plexus

`/bin/dc` of WEGA against `cmd/dc/dc.c` and `dc.h` of System III. Plexus has the same text. Measured 2026-11-07.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| dc1.c | dc/dc.c | 986 | dc/dc.c | 986 | 988 |
| dc2.c | dc/dc.c | 906 | dc/dc.c | 906 | 907 |

## WEGA's own

- dc1.c and dc2.c are `dc.c` of System III cut in two (about the first 1000 lines and the rest); only the SCCS line of dc1.c is not in System III. `dc.h` has the same macros.

## Forms that follow the bytes, not the System III text

- `div` returns a pointer and uses `errorrt`, which returns `1`: the warning is avoided with `return((struct blk *)1)` written out at that one place.
