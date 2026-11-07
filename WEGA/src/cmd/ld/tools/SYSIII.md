# ld against System III and Plexus

`/bin/ld` of WEGA (E711, SCCS ld.c 1.18) against `cmd/ld/ld.pdp.c` and `ld.vax.c` of System III and `ld.z8000.c` of Plexus. ld is a loader of its own design (`ld.h`, `symbols.c`, `copyname.c`, `printname.c`); System III and Plexus give only the plan of the passes and some names. Measured 2026-11-07.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| ld.c | ld/ld.pdp.c | 569 | ld/ld.z8000.c | 561 | 1438 |

## Names

Functions, globals and locals of the functions that have a System III counterpart carry its names: `delexit`, `endload`, `record`, `restore`, `load1arg`, `load1`, `middle`, `setupout`, `tcreat`, `load2arg`, `load2`, `load2td`, `finishout`, `error`, `lookloc`, `eq`, `putw`, `flush`, `cp8c`, `add`; `ofilename`, `tfname`, `premeof`, `vnodes`, `vindex`, `tsize`, `dsize`, `bsize`, `ssize`, `ctrel`, `cdrel`, `cbrel`, `errlev`, `trace`. Names do not change the bytes (stripped). Functions of WEGA's own design keep their names (`place_sec`, `init_entry`, `load_header`, `ranlib_scan`, `reg_sect_addr`, ...).

## WEGA's own

- The bss: its declarations stand in the order of the original with spare variables between them.
