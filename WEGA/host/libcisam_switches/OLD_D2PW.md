# OLD_D2PW: a register copy wanted in a busy register goes to the other half of the pair (v1, libcisam)

Diff: `OLD_D2PW.diff` (allo3.c allocreg; codgen and scodgen v1 in the Makefile). Not in the tree (owner 2026-11-18).

## What it does

In allocreg, for a copy of a register (op REG, 96) that is wanted in a word register which is busy: when the other
half of that pair is idle and holds a name that is not a parameter, the half is given back and taken for the copy.
Otherwise allocreg goes on as v1 (the lightest free word).

## Witness

item.o itpress, `bycmpr(it1->it_key, it2->it_key, kp->k_len)`: the argument is wanted in r5, r5 holds the parameter kp
(busy). v1 copies kp into r2 (`ld r2,r5; ld r5,52(r2)`), the original into r4 (`ld r4,r5; ld r5,52(r4)`); r4 held kp
from its frame home for the `k_flags` test in front of it. Alone: item nonsegmented 4 > 0, nothing else changes.

## Why a new switch

- Source forms of the condition (`!bycmpr`, parentheses, `(*kp).k_len`, `&it_key[0]`): no change.
- Existing switches: none of the D2/D3 switches alone moves this copy to r4 without large losses elsewhere.
- Narrower attempts: without the "holds a name" condition the entry copies of parameters change (read.o: `ld r6,r7`,
  wrong code: the parameter `rec` in r6 is overwritten); for loads through a pointer (OREG, 97) the same rule fixes
  supp freefile's first `close` but not its second (`r6`, `r3`, `r6` for three identical-looking calls) and changes
  key and lock: taken out.

## Open

One witness. getreg already prefers the register that holds the same name; here the names differ (the copy has the
parameter's symbol, r4 the frame slot of kp), so a rule "the frame home of a parameter counts as its name" would be
the other explanation (not tried).
