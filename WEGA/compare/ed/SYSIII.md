# ed against System III and Plexus

`/bin/ed` of WEGA against `cmd/ed/ed.c` of System III. Plexus has the same text. Measured 2026-11-25.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| ed.c | ed/ed.c | 2166 | ed/ed.c | 2166 | 2177 |

## Build

- Built without `PWB` and with `RESEARCH`: no fspec, lenchk, expnd, stdtab (the tab specifications), no ustat test of
  the write, no `oldmask`. System III's ed.mk sets neither, but its eopen uses `fss` outside `#ifdef PWB`: without PWB
  the System III text does not link. The umask pair around the link of the write (`omask`) stays:
  it is not under RESEARCH. The original ends with save and undo.
- execlp comes from the libc (the assembler member `execlp.s` in front of execvp), as in System III.

## WEGA's own

- The SCCS line `ed.c 1.2 01/30/83` (System III has none in ed.c).
- `<setret.h>` with `ret_buf`, `setret`, `longret` for `<setjmp.h>`, `jmp_buf`, `setjmp`, `longjmp` (WEGA's libc has no
  setjmp; mail.c does the same).
- eopen: the read of the format line (`fflg`) stays, its fspec test is gone (the test needs PWB).

## Forms that follow the bytes, not the System III text

- putline: the loop is written with goto (`loop:`, `out:`, `more:`) and the return in front of the `--nl` part; the while
  loop of System III gives the usual layout with the test at the bottom.
