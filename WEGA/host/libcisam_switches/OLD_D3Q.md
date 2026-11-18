# OLD_D3Q: behind the condition of a ?: whose arm increments a pointer the busy pairs are free (v1, libcisam)

Diff: `OLD_D3Q.diff` (reader3.c QUEST; codgen and scodgen v1 in the Makefile). Not in the tree (owner 2026-11-18).

## What it does

In codgen's QUEST case, behind `cbranch` of the condition: when an arm holds `++` or `--` of a pointer, every scratch
pair (class 3, r0..r7) is given back to the free list, also a busy one. That reproduces a bug of the original.

## Witness

item.o itunpack, segmented, `it->it_leadcnt = kp->k_flags & LCOMPRESS ? *p++ & 0377 : 0;` (and the same for
`it_trailcnt`). The address of the target is computed into rr2 first (`ldl rr2,rr12; inc r3,#12`). The original's arm
takes rr2 again for the old p (`ldl rr2,rr8; inc r9,#1; ldb rl4,@rr2`) and stores through it (`ld @rr2,r4`): the value
goes into the key buffer at p, not into the item. v1 takes rr4 (correct code). Nonsegmented the target is an OREG
(`12(r14)`), no register, so the bug does not show there. Alone: item segmented 34 > 10, nothing else changes.

The bug belongs in the table "Bugs and quirks of the originals" (host/KNOWLEDGE.md) once the owner decides on it.

## Why a new switch

- No source form can give a store through the wrong pointer.
- Existing switches (segmented, each alone on v1): none changes it.
- Narrower and wider forms measured: freeing every scratch register (`rsfreeall(1)`) breaks buf and supp; freeing all
  pairs breaks buf (bfread: an idle pair holding `off` must stay); freeing busy pairs for every `?:` breaks supp
  (bfgetclr's `?:` of two constants keeps the busy rr2). Only the arm that needs a pair itself (a pointer `++`) shows
  the bug in libcisam. The real mechanism is not known (guess: the pair request of the arm does not see the busy count).
