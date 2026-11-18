# OLD_D3H: the frame home of a parameter loaded in a condition goes back at its end (v1, libcisam)

Diff: `OLD_D3H.diff` (reader3.c cbranch, allo3.c allocreg; codgen and scodgen v1 in the Makefile). Not in the tree.

## What it does

`cbranch` counts its nesting. allocreg marks (rs flag 0x2000) every register it gives out while a condition is being
compiled. When the whole condition is done, an idle register that was given out inside it and holds the frame home of
a register parameter (fpsym1/fpsym2 with the offset of an entry of parmtab) goes back to the free list, word or pair.
The parameter in its own register (sclass 9) is not touched.

## Witnesses

- supp.o freefile, nonsegmented: `if (fp->f_flags & 04) close(fp->f_audfd);` loads fp from its home into r2 for the bit
  test. The original takes r6 for the value of the argument (`ld r7,r2; ld r6,18(r7); ld r7,r6`), the next
  `close(fp->f_idxfd)` (no condition in front of it, same register state in v1) takes r3. With r2 given back, r2 and r3
  merge into rr2 again and r6 is the only free word: r6 there and r3 at the second call, as in the original.
- item.o itpress, segmented: `if (kp->k_flags & TCOMPRESS)` and `if (kp->k_flags & ISDUPS)` load kp from its home into
  rr2; behind the test the original takes rr2 (`ld r2,rr10(#2)`, `ldl rr2,rr10; inc r3,#2`).
- Counter-witnesses for the limits: supp allocrec `if (type == 0)` keeps the parameter `type` in r7 (so not the
  parameter's own register); item itpress `if (i < it2->it_leadcnt)` behind `i = kp->k_len - ...` keeps rr2 with kp
  loaded in the statement before (so only registers given out inside the condition; a comparison of register and
  offset before/after is not enough: the original reloads kp in the TCOMPRESS test although rr2 held it before).
- util.o longmult: freeing at every level of a condition (not only at its end) loses the long parameter b in rr4
  that the body uses (`addl rr2,rr4`): only at the end.

Alone: libcisam 38/44 > 34/34 (item seg 34 > 24, supp 4 > 0). With OLD_D3W, OLD_D2PW, OLD_D3Q: 12/20 > 8/10.

## Why a new switch

Source forms of these tests (`!= 0`, `(*kp)`, casts, `(kp->k_flags & X) != 0`, the assignment split or joined): none.
Existing switches (all D2/D3, nonsegmented and segmented, each alone): none. Freeing all idle registers that hold a parameter
home at the end of a condition (not only those given out inside it): item segmented 6 lines instead of 0 (with the
other switches); freeing the parameter registers too: supp nonsegmented 4 > 27, item 10/25.
