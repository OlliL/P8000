# OLD_D3W: idle word registers go back at the end of a whole condition (v1, libcisam)

Diff: `OLD_D3W.diff` (reader3.c; codgen and scodgen v1 in src/cmd/codgen/Makefile). Not in the tree: it is a picture
of what the compiler of libcisam does, measured on libcisam only (owner 2026-11-18).

## What it does

v1 has `OLD_D3`: at the end of `&&` and `||` the idle registers keep their contents (the shipped codgen gives them back
with `rfreeall(1)`). `OLD_D3W` (under `OLD_D3`) counts the nesting of `&&`, `||` and `!` in `cbranch`. Inside a condition
nothing changes. When the whole condition is done (nesting back to 0), the idle **word** registers (class 2) go back to
the free list; pairs (class 3), register variables and busy registers stay. The same is done behind the condition of a
`?:` (reader3 QUEST) when it is not nested in an `&&`/`||`/`!`.

## Witnesses (libcisam, lines with label addresses masked, nonsegmented/segmented)

| Place | v1 | with OLD_D3W |
|---|---|---|
| btree.o btsearch: `openp->o_flags \|= 020` behind `r == 0 && mode == 2 && bp->bf_level == 0` (original takes r2, which held bp; segmented it keeps the pair rr2 = bp and takes rr4) | 12/0 | 0/0 |
| open.o isexit: `bfflush(filep->f_fileid)` behind `!(openp->o_mode & ISEXCLLOCK)` (original takes rr2, r2 held openp; segmented rr2 stays) | part of 10/0 | 0/0 |
| open.o isexit: `isstat2 = openp->o_flags & 020 ? '2' : '0'` (original takes r3 for the value, r3 held openp) | part of 10/0 | 0/0 |

Alone: libcisam 38/44 > 16/44. No other libcisam member changes.

## Why a new switch

- Source forms tried (host/OLDCC_LOG.md 2026-11-18): btsearch nested `if`, `!bp->bf_level`, statements swapped, `!= 0`
  forms, `== '\0'`, parentheses; isexit `== 0`, `if (x) ; else { ... }` (fixes nonsegmented, breaks segmented),
  isstat2 six forms (`!= 0`, `== 0`, if/else, octal constants, swapped arms). None gives both versions.
- Existing switches: every D2/D3 switch alone on top of v1 (nonsegmented 18, segmented 23 runs): none gets nearer.
- As a change of `OLD_D3` itself: the kernel breaks (vk has `OLD_D3` too: sys1.o, break.o differ, LIB1 does not link).
- Split of `OLD_D3` (owner's plan): `OLD_D3E` = "keep at the end" for vk, v1q, v1r; v1 without it, only `&&`/`||`
  (checkall d3a, 2026-11-18): cp, ln, mv, rm, ld, strip, libl.a, slibl.a, libPW.a differ. The same with `!` (d3b). So the
  other v1 programs keep the registers at the end of the condition and libcisam does not: libcisam was probably built
  by another compiler state (guess), and the rule is not a narrowing of `OLD_D3`.

## Open

- The rule "words go back, pairs stay" is taken from three places only; segmented libcisam always kept the pair.
- At the end of a plain condition (no `&&`, `||`, `!`) the original does not free the words (a general rule there
  made libcisam much worse: 287/374), except for the `?:` condition of isstat2.
