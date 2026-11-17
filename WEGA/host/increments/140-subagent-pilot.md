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
   the commit); run the pilot; measure.
3. Adjust, write down.

## Decisions

- 2026-11-17 (owner): the models per agent as proposed: recon-setup haiku (commands and copying, a fixed report),
  recon-match sonnet (source forms need reading of code), recon-corpus sonnet (expectations from the man page and the source).
- 2026-11-17 (owner): the three definitions agreed; pilot `bin/sum` agreed.

## Not part of it

The other programs: 150, 160.

## Log
