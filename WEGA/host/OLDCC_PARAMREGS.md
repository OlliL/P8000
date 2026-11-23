# OLDCC_PARAMREGS: parameters taken from the frame instead of their register

Working file of one question (owner 2026-11-23): the old compilers read a parameter from its frame home in places where
our compilers take it from the register it arrived in. Today we reproduce this with source forms that are unlikely to be
what the authors wrote: unused labels (`OLD_D3K` forgets the parameters at a label no jump names) and dead `if (0) p = 0;`
statements. Goal: find the rule of the compiler, build it into the existing switch it refines, and take the forced source
forms out again. Facts here are measured (the place is named); guesses are marked as such. The facts that hold in the end
go to `OLDCC.md`; this file keeps the analysis and, at the end, the log of what was tried.

## 1. The witnesses we already patched with a source form

Found in every `compare/*/findings.json` and by a scan of `src` for labels no `goto` names and for `if (0)` (labels that
are already in the System III or Plexus source are left out: ex choice, samef, out; find cerror; tgoto casedot; text.c loop).

| Compiler | Place | Source form in our tree | What the original does |
|---|---|---|---|
| v0 | chkout `main` | label `again:` in front of `progname = *argv++` | argv from the frame (`ld r2,0(r15)`) |
| v0 | mkstr `main` (f0042) | label `start:` in front of `argc--, progname = *argv++` | argv from the frame |
| v0 | libc calloc 0_1 | label `start:` in front of `num *= size` | `mult rr2,0(r15)`: size from the frame |
| v0p | find `scomp` | label `minus:` behind `if (s == '+') return (a > b);` | s from the frame (`cpb 1(r15),#'-'`), stored at the entry |
| v0p | codgen, scodgen `readmems`, `rwprint` | label `top:` in front of the first test (allo3.c) | the parameter from the frame |
| v0p | cpp `subst` | label `top:` in front of the first use of sp | sp from the frame |
| v0p | popt z22.c | label `clr_tail:` | (to be read) |
| vc | gopt quads2.c (rl1..rl4), dom2.c (rl1), lpinv2.c (rl1, rl2), wrtil2.c (l7e) | lone labels | register contents forgotten |
| vc | gopt mkleaf, qmkstore, deledge | dead `if (0) a = 0;` at the start | the parameter stays in the frame |
| v1 | ld symbols.c | label `known:` | (to be read) |
| shipped scc | sld symbols.c `known:`, ld.c `addhi:` | labels | (to be read) |

Not yet patched, the same symptom:

| Compiler | Place | What the original does |
|---|---|---|
| v0 | ex, 24 functions (`compare/ex/findings.json`, phase B "reload"): putchar, cerror, substitute, merror, smerror, getline, tail, tail2of, partreg, vdellin, _p_emit, gTTY, sTTY, setin, vreplace, printf, _align, tailspec, dosubcon, vclrlin, plodput, normal, exit, vputch | a parameter from its frame home in front of its first use (`ld r7,@r15`) |
| v0 | ex vopen, YANKreg, YANKline (phase B "register knowledge") | a global just stored from r2 is loaded again from memory in the next statement |
| v0 | libc ctime 0_2 `gmtime` (kept as cas source) | `ld r6,0(r15); ldl rr6,@r6` for `hms = *tim % 86400L`, ours `ld r6,r7` |
| v0 | chkout `putline` | see table 2 |

## 2. v0 in detail: where the original takes a parameter from the frame and where from its register

chkout (07/82), mkstr (11/82), libc calloc 0_1 and ctime 0_2 (07/82), all v0. "Entry" is the store of the register
parameters at the start (`ld` one, `ldl` a pair, `ldm` several). "From" is where the original takes the parameter at the
place named; "ccv0" is what ours does today without a forced form.

| Function | Entry | Statements in front of the place | The place | From (original) | ccv0 |
|---|---|---|---|---|---|
| chkout main (argc, argv) | ldl rr6 | `rel = 0; lev = 0;` | `progname = *argv++` | frame | register |
| mkstr main (argc, argv) | ldl rr6 | `x = 0;` (clrb) | `argc--, progname = *argv++` | frame | register |
| calloc (num, size) | ldl rr6 | none | `num *= size` | frame, both: `ld r3,2(r15); mult rr2,0(r15)` | register |
| ctime gmtime (tim) | ld r7 | none | `hms = *tim % 86400L` (long through tim, wanted in rr6 for divl) | frame | register |
| chkout putline (in, out, vp, line) | ldm r4..r7 | `if (*line == '&') line++` | `keyword(out, vp->rel, vp->lev, line)` | frame (vp, out, line) | register (vp) |
| chkout putline | ldm r4..r7 | the same, other branch | `wfprintf(out, "%s", line)` | register (out: `ld r7,r6`) | frame |
| chkout putline | ldm r4..r7 | `if (vp->next == 0)` false | `apply(in, out, vp->next, line)` | register (all) | register |
| chkout getvers (s, rp, lp) | ldm r5..r7 | `r = 0; l = 0;` | `p = s` | register | register |
| chkout nextcmd (in, vp) | ldl rr6 | `n = 0;` | `vp->pos >= vp->end` | register | register |
| chkout apply (in, out, vp, line) | ldm r4..r7 | none | `vp->cmd == 0` | register | register |
| chkout getat (in, vp, line) | ldm r5..r7 | none | `fseek(in, vp->pos, 0)` | register | register |
| chkout basename (s) | ld r7 | none | `b = p = s` | register | register |
| chkout notplain (name) | ld r7 | none | `stat(name, &st)` (whole argument, in its register) | register | register |
| chkout doit (name, rel, lev) | ldm r5..r7 | none | `setnames(name)` (whole argument, in its register) | register | register |
| chkout keyword (out, rel, lev, line) | ldm r4..r7 | none | `p = line` | register | register |
| chkout summary (vp) | ld r7 | `if (pflag) return;` | `vp->next` | frame | frame |
| ctime ct_numb (cp, n) | ld r7 only (n is never stored) | `cp++` | `n >= 10`, `n / 10`, `n % 10` | register r6 all through | ours stores n and divides in rr6 |

### ex (v0, 10/82)

Measured 2026-11-23: our ex of the tree (`src/cmd/ex/ex`, unstripped) against /bin/ex, each function aligned on the first
equal instructions (scratch script `tmp/tmp/exmap.py`). All with one store at the entry unless named.

| Function | Entry | The place | Original | ccv0 |
|---|---|---|---|---|
| tail (comm), tail2of | ld | `tailprim(comm, 1, 0)`: comm the whole first argument, wanted in r7 | `ld r7,@r15` (frame) | nothing (r7 as it is) |
| putchar (c) | ld | `(*Putchar)(c)`: whole argument in r7 | `ld r7,@r15` | nothing |
| cerror, normal, exit, vputch, gTTY | ld | the parameter the whole first argument of a call | `ld r7,@r15` | nothing |
| substitute, getline | ldm | the same | `ld r7,@r15` | nothing |
| merror, smerror, dosubcon, vclrlin | ld r14 / ldl rr6 | the same (home at 2) | `ld r7,2(r15)` | nothing |
| printf | ldm r2..r7 | the same (home at 10) | `ld r7,10(r15)` | nothing |
| partreg (c) | ld | c (int) passed as a char | `ldb rl7,1(r15); extsb r7` | `extsb r7` on the register |
| setin (addr) | ld | `getline(*addr)`: the parameter as the address of the argument, wanted in r7, behind a label | `ld r7,@r15; ld r7,@r7` | `ld r2,r7; ld r7,@r2` |
| tailspec (c) | ld | `foocmd[0] = c`: no call | `ldb rl2,1(r15)` (the byte from the frame) | `ld r2,r7; ldb ..,rl2` |
| vdellin (p, cnt, ..) | ldm r5..r7 | p (r5) as an index, behind `if (cnt == 0)`, a test of a global and `if (cnt < 0)` | `ld r2,@r15` | `ld r2,r5` |
| _align | ld r5 | (the store differs, to be read) | | |
| plodput, sTTY, vreplace, vopen, YANKreg, YANKline | | first difference elsewhere (layout, register knowledge of a global) | | |

### Counter-examples, v0 of 07/82

In chkout the same situation as most of ex, a parameter as the whole argument in its own register, keeps the register:
notplain `stat(name, &st)`, doit `setnames(name)`, putline `apply(in, out, vp->next, line)`, ctime `localtime(t)`.
mkstr (11/82) has no such call (checked: its calls with a parameter pass it in another register). Still to check: the
exact programs of 07/82 (dc, bc, tail, sum, mkdir, ranlib).

## 3. Observations and guesses

- (fact) A parameter gets its frame store at the entry only if its register no longer holds it at the end of the function
  (`regargs`, local3.c). Hence ct_numb: n stays known in r6 all through, so there is no store and its frame slot is empty.
- (fact) Many forced labels sit right at the start of a function, in front of the first use of a parameter, or behind an
  `if (...) return`.
- (guess) Assignment: the place in both `main`s changes the parameter itself (`argv++`), calloc `num *= size` changes the
  other one. The rule "a parameter a statement assigns to is forgotten at the start of the statement" exists (v0, `OLD_D3K`,
  scalint `p = removc(p, n)`), but ccv0 applies it to `=` only, not to `++`, `--`, `op=`. If it forgot every parameter at a
  statement that assigns one (or at least the one changed by `++`), main and calloc would follow; getvers, nextcmd (no
  assignment to a parameter) stay as they are.
- (guess) putline: the original has 2 frame bytes more than the four parameters and `ld r2,0(r15); inc r2,#1; ld 0(r15),r2`
  for the skip of `&`. That is what v0p `OLD_D16A` does for `++` of a frame slot (the value to a temporary: load,
  increment, store). If the source is `line++` and the old v0 had D16A, the temporary would explain the frame word, and
  the statement that assigns line would be where vp is forgotten (the first guess).
- (guess) gmtime: the long through tim is wanted in rr6 (divl). The original seems to take the pair rr6 first, which
  loses tim in r7, and then loads the address from the frame into r6; ccv0 copies `ld r6,r7` first.
- (fact) 22 of the 27 ex witnesses are the rule of v0p `OLD_D3AW` (argplace): a parameter that is the whole argument of
  a call, its address or goes through a cast, in the register the argument is wanted in, is taken from the frame (table
  ex). So ex (10/82) was built by a v0 with D3AW, chkout (07/82) by a v0 without it: two programs of "v0" that contradict
  each other in this point (guess: a later state of the compiler between v0 and v0p; evidence rule 4 of HOWTO_reconstruct).
- (fact) Left over in ex without a call: tailspec (`foocmd[0] = c` reads the byte of c from the frame) and vdellin (p as an
  index behind three tests). Two forms of their own, not D3AW.
- (fact) v0p `OLD_D3AW` (a parameter that is a whole argument in its wanted register is forgotten) does not hold for v0 of 07/82:
  chkout passes `name` to stat and setnames, and in, out, line to apply, straight from their registers. ex `tail(comm) {
  tailprim(comm, 1, 0); }` reloads comm: ex is later (10/82), perhaps a later state of the compiler (guess).
- (fact) The "reload" functions of ex were tried with forms on tail without effect (no blank line, `char comm[]`,
  `return (...)`, `return;`, a block, a null statement, an extra local, the parameter undeclared); an unused label gives
  putchar and tail exactly (compare/ex/findings.json, phase B).

## 4. Log (what was tried, and what came of it)

- 2026-11-23 chkout putline: unused label in front of `if (!eflag)`: the keyword() arguments as in the original, but
  apply() of the else branch then reloads every parameter (worse). Branches swapped (`if (eflag) wfprintf else keyword`):
  keyword() and wfprintf() registers as in the original, but the blocks the other way round and apply() reloads. Outer if
  inverted: no. An unused local gives the frame size. Open.
- 2026-11-23 chkout main: unused label in front of `progname = *argv++`: exact (kept for now, to be replaced by the rule).
  Without it: `progname = *argv++`, `argv[0]`, comma forms, `argv += 1`, `argv = argv + 1`, `argc--` first, `rel = lev = 0`:
  all `ld r2,r6`.
- 2026-11-23 libc ctime 0_2 ct_numb: `OLD_D2MP` (v0: OLD_D2M does not take the pair of a parameter that is divided):
  ct_numb exact, bench v0 and chkout unchanged, v0p not touched. Removed again (owner): only one witness.
- 2026-11-23 the 27 ex witnesses read against our build (exmap.py): 22 are D3AW (table ex); calloc reads both parameters
  from the frame. Nothing built.
- 2026-11-23 `OLD_D2Q` into v0 with `OLD_D2QI` (committed 729ad896): chkout readchk and getat exact; not a parameter question,
  noted here because it moved gmtime's second load to the original form.
