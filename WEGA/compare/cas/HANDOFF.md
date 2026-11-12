# cas: handoff

State of the reconstruction of `/bin/cas` (src/cmd/cas) on 2026-11-11, branch toolchain, up to the commit of this file.
What is fact is measured; guesses are marked. The history of every attempt: `host/OLDCC_LOG.md` (entries 2026-11-09 to
2026-11-11, search for "cas"). Per function: `compare/cas/findings.json`, `FINDINGS.md`.

## 1 State

- Goal: `/bin/cas` byte-identical. Not reached. Model: 151 of 152 functions strictly exact. The real build (`make` in
  src/cmd/cas, stripped) has the size of the original (89152); 42524 bytes differ, all behind the first open function.
- Open:

| Function | File | Orig | Model | Lines | What differs |
|---|---|---|---|---|---|
| jpr | jpr.c | 324 | 324 | 24 | the `plain` block and the loop step are moved (popt) |

- Compiler: `sccvc` = v0p compiled for segmented code. `OLD_VC` in `src/cmd/codgen/oldcc.h` defines `OLD_V0P`, takes
  `OLD_D3KC` out and adds `OLD_D13`, `OLD_D6J`, `OLD_D24`, `OLD_D2S`. scparse (segvc rule of `src/cmd/cparse/Makefile`):
  the switches of v0p with `OLD_D13`, `OLD_D25`. spopt (segvc rule of `src/cmd/popt/Makefile`): the shipped one with
  `OLD_D13`, `OLD_D45`, `OLD_D20S` and `OLD_D49`. Assembler `/lib/old/cas`, library `src/lib/slibc-v11.a`. parser.y and assemble.c without -O.
  Description: `host/OLDCC.md`, "The old scc of cas"; witnesses per switch: `host/OLDCC_SWITCHES_VC.md` (generated).
- checkall on commit e08eefd2 (with `OLD_D20S`, `OLD_D49`): ALL OK, corpus cas 657 of 657.
- Switches with one witness in cas, provisional (`host/OLDCC.md` section 5): `OLD_D2S` (symtype), `popt:OLD_D45`
  (stabout), `popt:OLD_D20S` (exprreloc), `popt:OLD_D49` (yylex).

## 2 Tools

All commands from `WEGA/`. `O=/mnt/share/cvs/P8000/tmp/oldroot` (made by `make -C host oldroot`), `T` a scratch
directory with two wrappers: `cc_vc` = `z8k-emu -r $O /bin/sccvc "$@"`, `cas_old` = `z8k-emu -r $O /lib/old/cas "$@"`.

| Command | What it does |
|---|---|
| `RECON_PROG=cas RECON_TMP=$T/t CCHOST=$T/cc_vc CAS=$T/cas_old python3 host/recon/d12model.py -q` | the model: all modules compiled, linked under the data layout of the original (90 s) |
| the same with `D12_ONLY=jpr` (comma list) | only these modules again, the rest from the last model (7 s); `D12_SRC=dir` takes the sources from a copy |
| `RECON_PROG=cas RECON_TMP=$T/t python3 host/recon/strictcmp.py [-v FUNC]` | every function against the original; `STRICT_CTX=n` lines of context for `-v` |
| `RECON_PROG=cas python3 host/recon/odis.py LO HI` | disassemble a range of the original (hex addresses; `findings.json` has them) |
| `RECON_PROG=cas RECON_TMP=$T/t python3 host/recon/findings.py cas` | status into `findings.json`, writes `FINDINGS.md` |
| `MATRIX_VER=vc sh host/recon/oldcc.sh matrix [-j N] [SWITCH ...]` | each switch of vc taken out alone: `host/OLDCC_SWITCHES_VC.md` |
| `MATRIX_VER=vc sh host/recon/oldcc.sh matrix check` | the table against the sources; before a commit that touches a switch |
| `make -C host oldroot WEGA=<WEGA_ROOT>` | the old compilers into OLDROOT, from the working tree (2.5 min) |
| `make -C host checkall WEGA=<WEGA_ROOT>` | everything (20 min); builds from a copy, do not change OLDROOT while it runs |

`host/recon/poptveto/` (README there) holds the scratch tools of 2026-11-11. **Their paths name a scratch directory of
that session (`S=/tmp/claude-1000/...`): set `S` in each script before use.**

| File | What it does |
|---|---|
| `bv.sh "CFLAGS"` | spopt of vc with other switches, built in a copy of src/cmd/popt, into a private copy of OLDROOT |
| `pb.sh NAME "DEFINES"` | spopt of vc with the segvc defines replaced (a switch can be left out), `PSED`/`PPY` patch z21.c first, into `o_NAME` |
| `cgb.sh NAME "SW +SW"` | scodgen of vc without (or with, `+`) switches into the private root `o_NAME` |
| `cpb.sh NAME "FLAGS"` | scparse of vc with other flags into `o_NAME` |
| `realcas.sh NAME` | the real build of src/cmd/cas with the root `o_NAME` (`cur`: OLDROOT), stripped, `cmp` with /bin/cas |
| `par.py` | eight work copies of the model; `run(tasks)` measures source variants in parallel under any private root |
| `jgen_jpr.py` | generates source forms of jpr (case orders, nested or flat, places of the plain code) for `par.py` |
| `z21_veto.patch`, `veto.py` | popt with `-DVETO`: every block move (M), loop inversion (I) and jump turn (S) has a number; single ones can be left out. Finds which decisions of popt lead to the original |
| `popt_D41.patch` | the D41 change (dropped, section 3), against src/cmd/popt/z21.c |

A TRACE build of spopt (`-DTRACE`, run as `spopt + in.s out.s` on the raw code of one function, `sccvc -S` without -O)
prints every block codemove moves (`/*CM L.. refc ..*/`) and the nodes after each pass.

Rules that cost time when forgotten:
- The cpp of the old compilers reads 8 characters of a macro name (`OLD_D2POV` was `OLD_D2PO`).
- Measure on the whole model: a change of popt or of a source form shows in other functions of the same file.
- Print little: filter the output of searches (`sort -n | head`), long lists are expensive.
- Commit every finding at once (the owner's wish), with a dated entry in `host/OLDCC_LOG.md`.

## 3 The open functions: what is known

jpr: the same question: our spopt moves a block that the original leaves where it stands (`codemove` in
src/cmd/popt/z21.c, called from `iterate`).

### exprreloc: solved by `OLD_D20S`; D41 dropped

- `OLD_D20S` (OLDCC.md, D20): a straight block whose label only one jump names and that ends in a jump to the label right
  behind it is not moved. exprreloc exact, nothing lost. D41 (the sweep goes on behind a moved block) is dropped: the
  earlier "D41 without D45" had D45 in (bv.sh cannot remove it; `pb.sh` in poptveto replaces the defines); alone it leaves
  stabout open, exprval only with an odd string order, symtype at 5 (OLDCC_LOG 2026-11-11).

### jpr: state of 2026-11-12 morning (night search, OLDCC_LOG 2026-11-12)

- The original never moves the plain block (`jpput(); return (0);`): it stays where `default: plain:` puts it in the raw
  code, behind the body of case 0x36 (order 0x36, default, 0x56 in the first switch). Our spopt moves it at three sites
  (end of the first dispatch, the type test of case 0x19, case 3 of the second dispatch).
- The original also does not move the step block (`p = p->n_next; jpr test`) to the `continue` of case 0x36; the error
  default of the second switch falls into it (C' = `cp 0x19; jr eq; error`, then step, then the test).
- Rule u1 (`refuse2.py`): codemove refuses (a) a block that ends in a jump to the exit, is named by more than one jump and
  has more than two instructions, (b) a block that ends in the jump back to a loop head that is fallen into, at a site of
  one instruction. With the form `default: plain:` pass 1 then gives A, plain, F with the second dispatch, C', step, as
  wanted except F; jpr 50, model loses iputline, symreloc, stlook, exprreloc, opshort, opseg, opoff, genbop.
  The veto search on top of u1 (all iterate decisions as events) ends at 30 lines (depth 2); the loop test is then
  inverted to the end again.
- Best so far: u1 with three vetoes, 20 lines (OLDCC_LOG 2026-11-12, end of the night): only C (error default) and G (case 7)
  stand wrong, because the second dispatch is split after `cp 0x19` instead of after `cp 7`.
- Not found: the rule that puts the step and the test in front of the first dispatch (the original rotates the loop so that
  the dispatch E is entered from the test). Tools: poptveto/README (pveto.py with pattern search, condrun.py, one.py).

### jpr (24 lines; 22 with other forms)

- The original read as a loop inversion with the far conditional jump `cp r2,#0x36`: behind the entry jump stand the
  body of case 0x36, the plain block (`jpput(tok, list); return (0);`), the second half of the second dispatch with
  the error, the loop step and the test; the rest of the body follows.
- With the veto popt and `default: plain: jpput(tok, list); return (0);` in the first switch (`goto plain` in case 3
  and at the type test): leaving out the two moves of the plain block (label 115) gives 10 lines, for every order of
  the first switch. The 10 left are the order of the plain block and the body of case 0x36; no further veto pair
  helps, so the original makes a move ours does not make.
- Tried without success: about 1500 generated forms under the committed spopt (best 22), 72 under D41 (best 26), if
  chains in place of the switches (28 to 48), three plain copies (26), the second switch nested in case 0x56.
- Same kind as exprreloc: a default block that ends in `return (0)` is not moved to the end of its dispatch. `OLD_D20S`
  does not reach it: 48 forms (each plain site inline or `goto plain`, all orders of the first switch) give 22 at best.

### yylex: solved by `OLD_D49` and a source form

- The done switch behind `tok = 3` (`goto invalid` / `goto unknown`, `ret: return (tok);`), the two bodies at the end of
  the function ending in `goto ret`. `OLD_D49` (OLDCC.md): refcount keeps an unnamed label in front of another label, so
  the loop inversion at the `%` block is not made. main follows.

## 4 What is settled (do not search again)

- `ldk r2,#4` (igetaddr, putrow): `switch (sizeof(pointer)) { case 4: ...; case 2: ...; }`; the compiler keeps the load
  of the constant and only the matching case.
- sgetnode, jprange: no `return` behind the switch (r2 holds the value by chance).
- yyparse: listp is the second member of `struct lwalk` (the unused word behind yyv); a guess, only the layout is known.
- igetexpr, sgetexpr: `l = igetexpr(); return (opseg(l));`, no switch.
- stenter: `OLD_D2PO` of v0p. symtype: `OLD_D2S` (seven source forms without it failed).
- stabout: exact with `popt:OLD_D45` or with D41. The mechanism: at `jpr ne,L183; jpr L181; L183:` of `if (*p == 0)
  break; p++;` the original lets codemove take the head of `switch (tag)` before the conditional jump is turned.
- The quirks of the original: `host/KNOWLEDGE.md`, "Bugs and quirks", rows `/bin/cas`.
- Not exact but by design: escape and digval are static (no symbol in the real build; the model gives them one).

## 5 Lessons for the search

- A supposed compiler difference was a source form five times on 2026-11-10/11 (constant switch, missing return,
  `v |= 0`, a struct member, the uniform `l = ...; return (f(l));`). Try forms first; but a form found under one popt
  can hide a rule of popt, and a rule can make an odd form unnecessary.
- A register difference in one statement can come from what an earlier statement left idle in a register: replace the
  statements in front one by one.
- When a function resists every form, find the single decisions of popt that differ (`veto.py`), then look for the
  rule or the form behind them.
- Evidence rules: `host/HOWTO_reconstruct.md`. Natural form before switch, switch before odd form; a new switch with
  one witness is provisional and listed in `OLDCC.md` section 5.
