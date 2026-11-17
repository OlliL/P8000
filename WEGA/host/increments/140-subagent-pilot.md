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

The main session: choice of compiler and library, the Makefile hook, the objects row (ccv0 against cc on sum.c: D2, D12,
D18), the quirks, checkall. Its tokens are not separable from the rest of the session (not measured).

## Decisions

- 2026-11-17 (owner): the models per agent as proposed: recon-setup haiku (commands and copying, a fixed report),
  recon-match sonnet (source forms need reading of code), recon-corpus sonnet (expectations from the man page and the source).
- 2026-11-17 (owner): the three definitions agreed; pilot `bin/sum` agreed.
- 2026-11-17 (owner): a second, larger run after the checkall of sum: `bin/ls` (02/83, D1 slots: v1 time, 539 lines of System III).

## Not part of it

The other programs: 150, 160.

## Log

- 2026-11-17: agent definitions agreed and committed (b8865603).
- 2026-11-17: pilot sum by the agents: IDENTICAL, corpus 19 of 19 (2ddd31ec); checkall.sh fixed for a git ref as CHECK_PATCH (45d6a086).
- 2026-11-17 (owner): a program of one C file goes to `src/cmd/NAME.c` with its rules in `src/cmd/Makefile` (V7 layout), not to
  `src/cmd/NAME/`; sum moved, recon-setup, recon-match, recon-corpus and the HOWTO say so.
