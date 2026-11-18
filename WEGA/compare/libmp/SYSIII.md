# libmp against its source

System III has no libmp; the source is `usr.lib/libmp` of 2.11BSD (versions of 1985, older code). `/usr/lib/libmp.a` and
`/usr/lib/slibmp.a` (Library.SC 02/11/83) against it. Measured 2026-11-18.

- All members: no `sccsid` block (`#ifndef lint ... #endif`).
- `gcd.c` (`invert`): `xalloc(1)`, without the name `"invert"` that the other calls pass.
- `mout.c`: `sdiv` has `if(qval[qlen-1]==0) qlen--;` (2.11BSD: `if(qlen && ...)`), `m_out` declares `char *obuf;`
  without `*malloc()`.
- The other members unchanged.
