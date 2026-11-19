# libcisam: the compiler v1c

libcisam.a and slibcisam.a (C-ISAM 1.03, Library.SC 02/11/83) are byte-identical when the sources of this directory are
compiled with -O by **v1c** (`ccv1c`, `sccv1c` from OLDROOT): the passes of v1 (cparse, popt) with a code generator of
its own (`/lib/v1c/codgen`, `/lib/v1c/scodgen`): the v1 switches (`OLD_D3`, `OLD_D61`, `OLD_D62`, `OLD_D63`; segmented also
`OLD_D24`, `OLD_D2CW`) and five more that only libcisam shows: `OLD_D3W`, `OLD_D2PW`, `OLD_D3Q`, `OLD_D3H`, `OLD_D2CT`
(src/cmd/codgen/Makefile, targets build/nonsegv1c, build/segv1c). No other program is built with v1c, so v1 is unchanged.
A later session can try to get rid of them (source forms, narrowing existing switches); the measurements are in
host/OLDCC_LOG.md (2026-11-18, 2026-11-19).

Each switch is needed: left out alone, `OLD_D3W` leaves btree 12/0 and open 10/0 lines (nonsegmented/segmented, label
addresses masked), `OLD_D2PW` item 4/0, `OLD_D3Q` item 0/24, `OLD_D2CT` audit 4/4, `OLD_D3H` (with OLD_D2CT) audit 4/4,
item 0/10, supp 4/0. All of them are rules on the register state (end of a condition, given out inside a condition,
the frame home of a parameter, a copy, word or pair), none asks for a file or a function.

Measuring a change: compile each member with ccv1c/sccv1c -O, disassemble with `host/tools/sdis.py` and compare with the
member of `/usr/lib/libcisam.a`, `slibcisam.a` (label addresses masked: `sed 's/L0[td]_[0-9a-fA-F]*/L/g'`).

## OLD_D3W: idle word registers go back at the end of a whole condition (v1c)

### What it does

v1 has `OLD_D3`: at the end of `&&` and `||` the idle registers keep their contents (the shipped codgen gives them back
with `rfreeall(1)`). `OLD_D3W` (under `OLD_D3`) counts the nesting of `&&`, `||` and `!` in `cbranch`. Inside a condition
nothing changes. When the whole condition is done (nesting back to 0), the idle **word** registers (class 2) go back to
the free list; pairs (class 3), register variables and busy registers stay. The same is done behind the condition of a
`?:` (reader3 QUEST) when it is not nested in an `&&`/`||`/`!`.

### Witnesses (libcisam, lines with label addresses masked, nonsegmented/segmented)

| Place | v1 | with OLD_D3W |
|---|---|---|
| btree.o btsearch: `openp->o_flags \|= 020` behind `r == 0 && mode == 2 && bp->bf_level == 0` (original takes r2, which held bp; segmented it keeps the pair rr2 = bp and takes rr4) | 12/0 | 0/0 |
| open.o isexit: `bfflush(filep->f_fileid)` behind `!(openp->o_mode & ISEXCLLOCK)` (original takes rr2, r2 held openp; segmented rr2 stays) | part of 10/0 | 0/0 |
| open.o isexit: `isstat2 = openp->o_flags & 020 ? '2' : '0'` (original takes r3 for the value, r3 held openp) | part of 10/0 | 0/0 |

Alone: libcisam 38/44 > 16/44. No other libcisam member changes.

### Why a new switch

- Source forms tried (host/OLDCC_LOG.md 2026-11-18): btsearch nested `if`, `!bp->bf_level`, statements swapped, `!= 0`
  forms, `== '\0'`, parentheses; isexit `== 0`, `if (x) ; else { ... }` (fixes nonsegmented, breaks segmented),
  isstat2 six forms (`!= 0`, `== 0`, if/else, octal constants, swapped arms). None gives both versions.
- Existing switches: every D2/D3 switch alone on top of v1 (nonsegmented 18, segmented 23 runs): none gets nearer.
- As a change of `OLD_D3` itself: the kernel breaks (vk has `OLD_D3` too: sys1.o, break.o differ, LIB1 does not link).
- Split of `OLD_D3` (owner's plan): `OLD_D3E` = "keep at the end" for vk, v1q, v1r; v1 without it, only `&&`/`||`
  (checkall d3a, 2026-11-18): cp, ln, mv, rm, ld, strip, libl.a, slibl.a, libPW.a differ. The same with `!` (d3b). So the
  other v1 programs keep the registers at the end of the condition and libcisam does not: libcisam was probably built
  by another compiler state (guess), and the rule is not a narrowing of `OLD_D3`.

### Open

- The rule "words go back, pairs stay" is taken from three places only; segmented libcisam always kept the pair.
- At the end of a plain condition (no `&&`, `||`, `!`) the original does not free the words (a general rule there
  made libcisam much worse: 287/374), except for the `?:` condition of isstat2.

## OLD_D2PW: a register copy wanted in a busy register goes to the other half of the pair (v1c)

### What it does

In allocreg, for a copy of a register (op REG, 96) that is wanted in a word register which is busy: when the other
half of that pair is idle and holds a name that is not a parameter, the half is given back and taken for the copy.
Otherwise allocreg goes on as v1 (the lightest free word).

### Witness

item.o itpress, `bycmpr(it1->it_key, it2->it_key, kp->k_len)`: the argument is wanted in r5, r5 holds the parameter kp
(busy). v1 copies kp into r2 (`ld r2,r5; ld r5,52(r2)`), the original into r4 (`ld r4,r5; ld r5,52(r4)`); r4 held kp
from its frame home for the `k_flags` test in front of it. Alone: item nonsegmented 4 > 0, nothing else changes.

### Why a new switch

- Source forms of the condition (`!bycmpr`, parentheses, `(*kp).k_len`, `&it_key[0]`): no change.
- Existing switches: none of the D2/D3 switches alone moves this copy to r4 without large losses elsewhere.
- Narrower attempts: without the "holds a name" condition the entry copies of parameters change (read.o: `ld r6,r7`,
  wrong code: the parameter `rec` in r6 is overwritten); for loads through a pointer (OREG, 97) the same rule fixes
  supp freefile's first `close` but not its second (`r6`, `r3`, `r6` for three identical-looking calls) and changes
  key and lock: taken out.

### Open

One witness. getreg already prefers the register that holds the same name; here the names differ (the copy has the
parameter's symbol, r4 the frame slot of kp), so a rule "the frame home of a parameter counts as its name" would be
the other explanation (not tried).

## OLD_D3Q: behind the condition of a ?: whose arm increments a pointer the busy pairs are free (v1c)

### What it does

In codgen's QUEST case, behind `cbranch` of the condition: when an arm holds `++` or `--` of a pointer, every scratch
pair (class 3, r0..r7) is given back to the free list, also a busy one. That reproduces a bug of the original.

### Witness

item.o itunpack, segmented, `it->it_leadcnt = kp->k_flags & LCOMPRESS ? *p++ & 0377 : 0;` (and the same for
`it_trailcnt`). The address of the target is computed into rr2 first (`ldl rr2,rr12; inc r3,#12`). The original's arm
takes rr2 again for the old p (`ldl rr2,rr8; inc r9,#1; ldb rl4,@rr2`) and stores through it (`ld @rr2,r4`): the value
goes into the key buffer at p, not into the item. v1 takes rr4 (correct code). Nonsegmented the target is an OREG
(`12(r14)`), no register, so the bug does not show there. Alone: item segmented 34 > 10, nothing else changes.

### Why a new switch

- No source form can give a store through the wrong pointer.
- Existing switches (segmented, each alone on v1): none changes it.
- Narrower and wider forms measured: freeing every scratch register (`rsfreeall(1)`) breaks buf and supp; freeing all
  pairs breaks buf (bfread: an idle pair holding `off` must stay); freeing busy pairs for every `?:` breaks supp
  (bfgetclr's `?:` of two constants keeps the busy rr2). Only the arm that needs a pair itself (a pointer `++`) shows
  the bug in libcisam. The real mechanism is not known (guess: the pair request of the arm does not see the busy count).

## OLD_D3H: the frame home of a parameter loaded in a condition goes back at its end (v1c)

### What it does

`cbranch` counts its nesting. allocreg marks (rs flag 0x2000) every register it gives out while a condition is being
compiled. When the whole condition is done, an idle register that was given out inside it and holds the frame home of
a register parameter (fpsym1/fpsym2 with the offset of an entry of parmtab) goes back to the free list, word or pair.
The parameter in its own register (sclass 9) is not touched.

### Witnesses

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

### Why a new switch

Source forms of these tests (`!= 0`, `(*kp)`, casts, `(kp->k_flags & X) != 0`, the assignment split or joined): none.
Existing switches (all D2/D3, nonsegmented and segmented, each alone): none. Freeing all idle registers that hold a parameter
home at the end of a condition (not only those given out inside it): item segmented 6 lines instead of 0 (with the
other switches); freeing the parameter registers too: supp nonsegmented 4 > 27, item 10/25.

## OLD_D2CT: a copy takes over a register given out in the last condition (v1c; needs OLD_D3H)

### What it does

- Word copy (both): a copy (op REG) into its wanted register, of a scratch word that holds a name (not a parameter),
  is busy only for this use and was given out in the last condition: the name goes with the copy and the source register
  goes back to the free list at once.
- Pair copy (segmented): a copy of such a pair into the pair of a wanted odd word register (the address of a word that is
  wanted in r7): no copy, the pair itself is taken.

### Witnesses

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

### Why a new switch

Source forms (one `&&` condition, `!= 0`, `< 0`, casts): none. Existing switches (nonsegmented and segmented, each
alone): none; `OLD_D2C` (a name found in a scratch register is that register) does not build alone on v1. The rule is the
copy side of OLD_D3H: a register the last condition gave out is not kept beyond the next use.
