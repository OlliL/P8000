# libm against System III

`/lib/libm.a` and `/lib/slibm.a` against `usr/src/lib/libm` of System III (the same sources for both). Measured 2026-11-17.

- `exp.c`: `maxf` is 709 (System III: 10000; the data word `%4086 %2800`).
- `jn.c` (2.1, 01/19/83; the others 1.1 or 2.1 of 01/18/83): `jn` has `int sign;` behind `i`, `sign = 0;` first, after
  `if(x == 0.) return(0.);` the lines `if(x < 0.) { x = -x; sign = n & 1; }`, and returns `sign ? -b : b` and
  `sign ? -t*j0(x)/b : t*j0(x)/b`.
- `pow.c`: `if((l & 1) != 0)` for `if(l & 1)` (the bytes of the nonsegmented member; the meaning is the same).
- `fputil.s`: no System III source (frexp, ldexp, modf are in `libc/pdp11/gen` and `libc/vax/gen` for those machines); WEGA's own for the EPU.
- The other members are taken unchanged; they are open for the compiler, not for the source (`findings.json`).
