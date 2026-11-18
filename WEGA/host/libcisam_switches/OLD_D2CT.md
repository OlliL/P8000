# OLD_D2CT: a copy takes over a register given out in the last condition (v1, libcisam; needs OLD_D3H)

Diff: `OLD_D3H+OLD_D2CT.diff` (allo3.c allocreg; it needs the marks of OLD_D3H: rs flag 0x2000 = given out while a
condition was compiled; the flags stay until the next condition starts). Not in the tree.

## What it does

- Word copy (both): a copy (op REG) into its wanted register, of a scratch word that holds a name (not a parameter),
  is busy only for this use and was given out in the last condition: the name goes with the copy and the source register
  goes back to the free list at once.
- Pair copy (segmented): a copy of such a pair into the pair of a wanted odd word register (the address of a word that is
  wanted in r7): no copy, the pair itself is taken.

## Witnesses

audit.o isaudit, AUDSTOP: `if (dictp[DI_AUDIT] & 0200) if (filep->f_flags & 04) { close(filep->f_audfd); ...`
- nonsegmented: the inner test loads filep into r3; the original copies it to r7 and loads the value into r3 again
  (`ld r7,r3; ld r3,18(r7); ld r7,r3`), v1 into r6.
- segmented: the inner test loads filep into rr4; the original loads straight through it (`ld r7,rr4(#20)`), v1 copies
  rr4 to rr6 first (`ldl rr6,rr4; ld r0,rr6(#20); ld r7,r0`).
- Counter-witnesses: audit isaudit `write(filep->f_audfd, ...)` copies filep from r4, loaded in a statement before the
  condition: r4 stays, the value goes to r5 (so: only registers given out in the last condition). open.o (segmented):
  a long wanted in rr6 through a copy of rr2 keeps the copy (`ldl rr6,rr2; ldl rr4,rr6(#4)`): only a wanted odd word.
- Without the condition mark the word copy rule breaks audit (the write above) and lock.

With OLD_D3W, OLD_D2PW, OLD_D3Q, OLD_D3H: libcisam 8/10 > 4/6 (audit exact).

## Why a new switch

Source forms (one `&&` condition, `!= 0`, `< 0`, casts): none. Existing switches (nonsegmented and segmented, each
alone): none; `OLD_D2C` (a name found in a scratch register is that register) does not build alone on v1. The rule is the
copy side of OLD_D3H: a register the last condition gave out is not kept beyond the next use.
