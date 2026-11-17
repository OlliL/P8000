# 140 Subagents: the workflow, on a pilot program

## Status

active (2026-11-17)

## Goal

A tested way to rebuild a program with subagents: which steps a cheaper model does with a narrow brief and a fixed output,
which stay with the main session; proven on one pilot program from `bin` that needs no new compiler rule.

## Done when

- Agent definitions (`.claude/agents/*.md`) for the mechanical steps, each with input, output format, and what it must not
  decide; the owner has agreed to them.
- The pilot program is byte-identical (checkall) with its corpus, rebuilt with these agents; the cost in tokens per step noted.
- `host/HOWTO_reconstruct.md` names the agents in its steps.

## Read first

- `host/HOWTO_reconstruct.md` (the steps of a reconstruction, the evidence rules for the old compilers).
- `host/OLDCC_DATES.md` (the time of a program, its compiler).

## Known

- Mechanical steps that suit a narrow brief: inventory and first findings (`inventory.py`, `own_table.py`), the System III
  starting source, the library (`whichlibc.py`), trying source forms by script against a fixed compiler, corpus inputs.
- Steps that stay with the main session: a new compiler rule or switch, a new compiler version, an owner's exception
  (2026-11-15: of 8 switches of vl, 6 were replaced by source forms; a switch is the last resort).

## Steps

1. [x] Pick the pilot (from COVERAGE: `sys3`, small, a compiler that exists): `bin/sum` (5060 bytes, `sum.c 2.1 07/28/82`,
   D1 tight: ccv0 like dc; stdio, long arithmetic, a union).
2. Write the agent definitions (drafts: `.claude/agents/recon-setup.md`, `recon-match.md`, `recon-corpus.md`; the main
   session keeps the choice of compiler and library, the Makefile hook in `src/cmd/Makefile`, checkall, the two tables,
   the commit); [x] run the pilot; [x] measure.
3. [x] Adjust (recon-setup: stripped check, CC and compiler field; recon-corpus: `sh corpus.sh`, man page without pcat),
   [x] write down (`host/HOWTO_reconstruct.md`).

### Cost of the pilot (bin/sum)

Measured from the usage of each subagent (the task notification and the usage lines of its transcript). The agents ran as
`general-purpose` with the model override and the definition file as the brief (new agent types are loaded at the start
of a session only).

| Step | Agent, model | subagent_tokens (notification) | Output | Cache read | Cache written | Tool calls | Time | Result |
|---|---|---|---|---|---|---|---|---|
| 1, 2, 3, 5 | recon-setup, haiku | 57k | 12.0k | 669k | 55k | 19 | 64 s | ccv0 candidate, libc-v1 without gaps; missed: stripped, CC |
| 4, 6, 7 | recon-match, sonnet | 47k | 5.0k | 442k | 47k | 12 | 45 s | IDENTICAL after 2 attempts (SCCS string, order of two unions) |
| 9 | recon-corpus, sonnet | 46k | 4.7k | 421k | 25k | 12 | 42 s | 19 cases, all equal; 2 quirks |

Second run, bin/pr (shipped cc, /lib/libc.a; real agent types with the self-contained definitions, setup by setup.py):

| Step | Done by | subagent_tokens (notification) | Output | Cache read | Cache written | Tool calls | Time | Result |
|---|---|---|---|---|---|---|---|---|
| 1 | setup.py | 0 | - | - | - | - | seconds | ok, libc.a without a gap |
| 2 | recon-match, sonnet | 11k | 1.5k | 27k | 11k | 3 | 16 s | IDENTICAL after 1 attempt (SCCS line) |
| 4 | recon-corpus, sonnet | 20k | 4.7k | 92k | 16k | 6 | 38 s | 43 cases, all equal; 1 case by the main session (`time:`) |

The main session: choice of compiler and library, the Makefile hook, the objects row (ccv0 against cc on sum.c: D2, D12,
D18), the quirks, checkall. Its tokens are not separable from the rest of the session (not measured).

Third run, usr/bin/cpio (21 kB, 1051 lines of System III): the agents against one fresh session without agents
(`claude -p --model opus` in a worktree at d1889ace, the same task: HOWTO phase A steps 1 to 5, no checkall, no commit).

| Way | Part | Turns | Output | Cache read | Cache written | Time | Result |
|---|---|---|---|---|---|---|---|
| agents | setup.py | - | - | - | - | 1 s | ok |
| agents | recon-match, sonnet | 35 | 24.6k | 1.02M | 51k | 4 min | 4 bytes short: it had removed the `& 00000177777` of System III |
| agents | recon-corpus, sonnet | 35 | 21.2k | 1.32M | 50k | 3.3 min | 55 cases equal |
| agents | main session, opus (context about 370k) | 37 | 27.6k | 13.8M | 49k | | 10 attempts on the wrong block; left partial |
| fresh | one session, opus, no agents | 65 | 39.0k | 4.62M | 109k | 9.2 min | IDENTICAL; 37 cases equal; $2.58 (+$0.22 for a first try that broke off) |

Reading: the main session of the agent way cost more than the whole fresh session (its large context is read again in
every turn), and the cheap model made a wrong change that the main session then chased. The fresh session found the
forms (the mask kept, unused locals in gmatch and missdir). The source of the fresh session is the one taken
(byte-identical); the corpus of the agent way (55 cases) is kept. Not measured: agents with a fresh, small main session.

## Decisions

- 2026-11-17 (owner): the models per agent as proposed: recon-setup haiku (commands and copying, a fixed report),
  recon-match sonnet (source forms need reading of code), recon-corpus sonnet (expectations from the man page and the source).
- 2026-11-17 (owner): the three definitions agreed; pilot `bin/sum` agreed.
- 2026-11-17 (owner): the setup is a script, `host/recon/setup.py` (no model), step 1 of the HOWTO; the agent recon-setup is
  removed. The prompt of an agent gives every known value (no looking up). The definitions are self-contained (no
  repository documents to read).
- 2026-11-17 (owner): the revised definitions (recon-match, recon-corpus, self-contained; setup by setup.py) agreed.
- 2026-11-17 (owner): a second, larger run after the checkall of sum: `bin/ls` (02/83, D1 slots: v1 time, 539 lines of System III).
- 2026-11-18 (owner): at the end of a program its functions in findings.json carry their names and every function stays
  there; FINDINGS.md (`findings.py NAME`) leaves out the exact ones without a note (HOWTO step 5).
- 2026-11-18 (owner): `bin/pr` instead of `bin/ls`: ls needs libc members that no libc-vN has yet (setup.py: libc-v4 covers
  2780 of 5382 bytes); that is library work (150). pr: shipped cc, `/lib/libc.a` without a gap.

- 2026-11-18 (owner): measure the token cost: `usr/bin/cpio` with the agents (agents and main session counted apart, the
  main session from the usage lines of its transcript), then the same task by one fresh session without the agents
  (`claude -p --model opus --output-format json` in a git worktree at the commit before cpio; its result is thrown away).
  The cheaper way is the one for 160.

- Open (owner): the way for 160, from the table above (my proposal: one fresh session per program, without agents).

## Not part of it

The other programs: 150, 160.

## Log

- 2026-11-18: cpio: agents against one fresh session (table under Steps); cpio IDENTICAL by the source of the fresh session;
  host scparse: the heap of cparse (51730576); COVERAGE.md was not generated after sum and pr: now step 5 of the HOWTO.

- 2026-11-18: pr by setup.py and the agents: IDENTICAL, corpus 44 of 44; whichlibc.py and setup.py tried the shipped libc
  at a path gone since 100 (afa3f9b7).

- 2026-11-17: checkall ALL OK on 50d4434d (sum in the install tree and its corpus; host cparse fixed: dimtab[-1] is lregvar,
  the corpus input ex/printf.c failed on c1b7ea71 already).

- 2026-11-17: setup.py (checked on sum, dc, rmdir, cp, size, mv, write, ranlib: OWN_LIM and library as in their Makefiles),
  HOWTO phase A renumbered around it (1 setup, 2 build and compare, 3 no source, 4 corpus, 5 handoff); recon-setup removed.

- 2026-11-17: agent definitions agreed and committed (b8865603).
- 2026-11-17: pilot sum by the agents: IDENTICAL, corpus 19 of 19 (2ddd31ec); checkall.sh fixed for a git ref as CHECK_PATCH (45d6a086).
- 2026-11-17 (owner): a program of one C file goes to `src/cmd/NAME.c` with its rules in `src/cmd/Makefile` (V7 layout), not to
  `src/cmd/NAME/`; sum moved, recon-setup, recon-match, recon-corpus and the HOWTO say so.
