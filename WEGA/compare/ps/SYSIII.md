# ps against System III and Plexus

`/bin/ps` of WEGA (`ps.c 3.2 11/18/83`, segmented) is Plexus' `cmd/ps.c` (System III's ps with `/dev/kmem` and
`/dev/mem`) as WEGA changed it, and two modules of 08/25/83, `kva.c` and `kpa.c`, which have no source anywhere (written
from the binary). Measured 2026-11-25.

## Lines

Lines without comments and blanks that also stand in the file.

| File | Plexus file | Equal | System III file | Equal | Of |
|---|---|---:|---|---:|---:|
| ps.c | cmd/ps.c | 684 | ps.c | 682 | 777 |
| kva.c | - | 0 | - | 0 | 45 |
| kpa.c | - | 0 | - | 0 | 14 |

## WEGA's own

- The namelist `/wega` with `_proc`, `_swplo` and `_Nproc` (an int: the size of the proc table; no `struct var`); the
  terminator of `nl[]` is `{ "" }`.
- `-k kcore`: a crash dump is both coref and memf; the kernel addresses go through the MMU table of the dump
  (`kva.c`: the segment descriptors at 256 of its first 1024 bytes, base, limit, the direction of a stack segment), the
  physical ones (the user blocks) through `kpa.c` (+1024). A swapped process gets a zeroed user block with `-k`.
- The cases `s` and `c` are gone from the switch; `s:` and `c:` stay in the getopt string (accepted, without effect).
  The usage line names `-k kcore`, not `-c` and `-s`.
- The user block is found by `p_addr` (clicks of 256 bytes) in `/dev/mem`, or by `p_addr + swplo` (blocks) in
  `/dev/swap`; the arguments of `-f` through a pointer in the last word of the user stack (its segment word with the
  bytes swapped), else from the last 512 bytes as in System III.
- Widths: `%7.6s` for the user name, `%6u%4d` for PPID and C, `%6x%4x` ADDR and SZ, `%9lx` WCHAN, `%.18s` for the
  arguments; the TIME minutes as `%3ld`.
- `access` reads into the caller's buffer and returns 1; `getbyte` and `getword` pass it. `within` takes its bounds as
  unsigned, the callers pass longs (a bug of the original, kept).
- An empty function behind `przom`.

## Build

ps.c by the shipped scc; kva.c and kpa.c by sccvk (the compiler of the kernel objects, `host/OLDCC.md`, vk); the
shipped `slibc.a`; sld `-z` (one segment: code, data, bss); the scrt0 of `crt0.list` with `_environ` as a common
(the first word of the bss).
