# libPW against System III

`/usr/lib/libPW.a` and `/usr/lib/slibPW.a` against `usr/src/lib/libPW` of System III (the same sources for both, built
with -DSEG for the segmented one). Measured 2026-11-18.

- All members: no `Sccsid` string (System III has one in 37 files).
- `cat.c`, `regcmp.c`, `regex.c`: WEGA passes the first arguments in registers (nonsegmented 6, segmented 3 words or
  pointers), so the address of the first argument does not reach the others. Each copies the register arguments into an
  array (`regs`) and walks it first, then the stack arguments (`&args`); `cat` and `regex` through a helper (`_cat`,
  `_zregex`). In `regex` the stack index is `__bravar[in-NREGS]` (not `__bravar[in]-NREGS`), and both
  `regex` and `regcmp` switch to the stack one argument late; `regcmp` (nonsegmented) goes on at `&regs[5]`.
- `fatal.c`: `longret(Fjmp, 1)` for `longjmp(Fjmp)`.
- `setsig.c`: "Segmentation violation" for "Memory fault"; `register int (*n)()` for `int n` with the cast.
- `xalloc.c`: no `SCCSID`; `printf("out of space \n"); return((int)cp);` for `return(fatal("out of space (ut9)"))`;
  in `xfree` the second coalesce test is the first one again (nonsegmented). Segmented: `char *xalloc`, `long Lastbrk`,
  the segment of `end` and the offset of a sum through a static union, `ssgbrk(seg, n)` and `sgbrk(&end)` for `sbrk`
  and `brk`, the pointer arithmetic on `struct fb` (`Lastbrk = cp + SIZINCRO`), the masks `& 0x7F00FFFF` by casts.
- `xcreat.c`, `xmsg.c`: `xalloc` (and in `xcreat` `xfree`) for `alloca`.
- `move.c`: no Z8000 source; `pdp11/move.c` with int pointers for its `x.ip` forms.
