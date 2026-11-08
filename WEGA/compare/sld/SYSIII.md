# sld against System III and Plexus

`/bin/sld`, the segmented loader of WEGA, against `cmd/ld/ld.pdp.c` and `ld.vax.c` of System III and `ld.z8000.c` of Plexus. sld is a loader of its own design (`ldm.c`, `ext.c`, `swap.c`, `dump.c`). Measured 2026-11-07.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| ld.c | ld/ld.pdp.c | 434 | ld/ld.z8000.c | 434 | 1724 |

## Names

Functions, globals and locals of the functions that have a System III counterpart carry its names: `delexit`, `endload`, `record`, `restore`, `load1arg`, `load1`, `middle`, `setupout`, `tcreat`, `load2arg`, `load2`, `load2td`, `finishout`, `error`, `lookloc`, `eq`, `putw`, `flush`, `cp8c`, `add`; `ofilename`, `tfname` (`tmpname` stays: another variable in sld), `premeof`, `vnodes`, `vindex`, `tsize`, `dsize`, `bsize`, `ssize`, `ctrel`, `cdrel`, `cbrel`, `errlev`, `trace`. Names do not change the bytes (stripped). Functions of WEGA's own design keep their names (`place_sec`, `init_entry`, `load_header`, `ranlib_scan`, `reg_sect_addr`, ...).

## WEGA's own

- Fixed warnings: casts at `calloc`, `realloc` (the types of the tables), `lastt`, `lastd`, `lastb` assigned without cast (same pointer type), `entrypt = lastsym`.

## Forms that follow the bytes, not the System III text

- `load2arg(a)` in `case 'R'` behind `goto next;` (it does not run, but the compiler emits it): `if (1) goto next;` avoids the warning "statement not reached" and gives the same bytes.
