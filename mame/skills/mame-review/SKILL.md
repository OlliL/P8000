---
name: mame-review
description: MAME code review against MAME's C++ coding guidelines and the conventions core developers enforce when they tidy merged code. Use when reviewing a mamedev/mame pull request, a branch or commit range, or uncommitted changes in a MAME checkout, or when checking code before submitting it to MAME.
argument-hint: "[PR number or URL | git ref or range | paths]"
---

# MAME review

Review a change the way a MAME core developer would: flag what MAME requires, and what a core developer would otherwise **tidy** after merging it. Every finding cites a rule. Style that MAME accepts is never a finding, however it differs from the reviewer's taste.

## Calibration

These govern every step:

- **The file's existing style wins.** The guidelines say so first: "the style present within an existing source file should be favoured". Match layout and naming findings against the file, not an ideal.
- **Existing bad code is no precedent.** Style follows the file; APIs and idioms don't. A legacy pattern copied from elsewhere (`address_map_bank_device`, `MCFG_` macros, static buffers, tag lookups) is still a finding in new code.
- **Scope is the change.** Judge lines the change adds or modifies. Legacy code in untouched lines stays out of the review, unless the change makes it wrong. Tidying a line the change already touches is fair to ask for; reformatting others is churn.
- **Accepted variation** (never a finding on its own): K&R or Allman bracing; braces or none when neither branch of an `if`/`else` exceeds one statement; `case` labels level with `switch` or indented; `//` or `/* */` comments, a comment on its own line before `else`, and one or two spaces between sentences; `const T *` or `T const *`; `u8`/`u16`/`u32` or `uint8_t`/`uint16_t`/`uint32_t`; in-class member initialisers in a class whose constructor is defined in the class body; leading-comma or trailing-comma initialiser lists (one style per file); `static constexpr` or `static inline constexpr`; `0.5f` or `0.5F`; the `m_` prefix (encouraged, used consistently); plain shift-and-mask used consistently instead of `BIT()`.
- **Tiers**, from the core-developer view:
  - **Blocking**: would hold up or revert the change — incorrect emulation behaviour, broken save states, side effects visible to the debugger, undefined behaviour, a device assuming things about its host, a build, CI or `-validate` failure, or a contribution-policy breach.
  - **Tidy**: a documented or routinely enforced convention a core developer would ask for in review or fix after merge.
  - **Nit**: optional polish. Report a few at most. Core developers merge "good enough for a first cut" work: performance and cleanup never block a correct change alone.

## Steps

### 1. Pin the change

The argument picks the mode:

- **PR number or `github.com/mamedev/mame/pull/N` URL**: `gh pr view N -R mamedev/mame --json number,title,body,author,url,headRefOid,commits,files` and `gh pr diff N -R mamedev/mame`. Read a whole file at the PR head with `gh api -H 'Accept: application/vnd.github.raw' "repos/mamedev/mame/contents/<path>?ref=<headRefOid>"`, saving copies in a scratch directory outside the checkout. PR mode writes nothing to the checkout (no fetch, no checkout) and nothing to GitHub. A local checkout, if present, holds the base or a newer master, never the PR: use it for tools and surrounding context only. `gh pr diff` shows only the PR's own changes, even when the PR has merged master into itself.
- **Ref or range** (`master`, `abc123..HEAD`): `git diff <base>...<head>` and `git log --format='%h %s%n%n%b' <base>..<head>`.
- **Paths, or nothing**: the working tree. `git diff HEAD -- <paths>` plus untracked files from `git status --porcelain -- <paths>` (a new driver is often still untracked: read it whole). When the tree holds several unrelated in-flight changes, list the areas and ask which to review.

Done when you hold the diff, the added/modified/deleted file list, and the commit messages and PR title and body where they exist.

### 2. Read each file and note its style

Read new files and files under about 1,000 lines whole, post-change. For a longer file, read every touched function or block whole plus enough of the rest to write its style line; for a long repetitive table (an opcode list, a ROM or input-port block), the whole family around each change; for a list file (`mame.lst`, `scripts/src/*.lua`, `*_cards.cpp`), the hunk and its neighbours. For each file, note:

- **Area**: core (`src/emu`, `src/lib`, `src/osd`), device (`src/devices`), driver (`src/mame`), generator input (a CPU core's `*.lst`, `*make.py`, `*gen.py`), build (`scripts/`, `makefile`), software list (`hash/*.xml`), layout (`*.lay`), translation (`language/`), plugin (`plugins/`), docs.
- **Style line**: bracing, `case` indent, integer-type spelling, `const` placement, comment style. For a new file, write "new file": all rules apply to all of it.

The areas decide which rule files step 4 reads:

| When the change touches | Read |
|---|---|
| any C++ source (`.cpp`, `.h`, `.ipp`, `.hxx`) or generator input | [RULES.md](RULES.md) and [STYLE.md](STYLE.md) |
| input ports, ROM definitions, or `GAME`/`SYST`/`CONS`/`COMP` lines | [SYSTEMS.md](SYSTEMS.md) |
| software lists (`hash/*.xml`), layouts (`*.lay`), translations (`language/`) | [DATAFILES.md](DATAFILES.md) |

Plugins (`plugins/`) get the srcclean check from step 3 (it handles Lua) and SUBMISSION.md; Lua idiom isn't reviewed — hoisting library functions into locals, as `plugins/data` does, is an optimisation core developers make themselves, not one they ask contributors for.

[SUBMISSION.md](SUBMISSION.md) is read in step 6 whatever the change touches.

Done when every touched file has an area and a style line, and the rule files to read are listed.

### 3. Run the mechanical checks

Run what the environment allows. In the report, mark a check with nothing to check `n/a` and a check that couldn't run `skipped (<reason>)`.

- **Include guards** (headers under `src/devices`, `src/mame`): `python3 scripts/build/check_include_guards.py src/devices src/mame`, reporting only touched headers. In PR mode, lay the head copies out as `<scratch>/head/src/devices/…` and `<scratch>/head/src/mame/…` and run the script on those two directories. CI runs this.
- **Software lists**: `xmllint --noout --valid hash/<list>.xml`. CI runs this. Layouts: `xmllint --noout <file>.lay` for well-formedness.
- **Source format**: when a `srcclean` binary exists (`make TOOLS=1` builds `./srcclean` at the checkout root), copy each touched file — the post-change version: the PR-head copy in PR mode — into two scratch directories, `pristine/` and `cleaned/`, keeping its file name and extension, since `srcclean` picks C, Lua or XML handling by extension. Run `srcclean` on the copy in `cleaned/` only and diff the two directories. `srcclean` rewrites files in place, so it only ever runs on copies.
- **PR mode**: `gh pr checks N -R mamedev/mame` stands in for local builds (for a specific commit: `gh api repos/mamedev/mame/commits/<sha>/check-runs`). A red check is Blocking under S-4: pull its log with `gh run view <run-id> -R mamedev/mame --log-failed` (the run ID is in the check's URL) and report the error at its source line.

### 4. Apply the rules

Read each rule file step 2 listed in full, then walk every added or modified line against every rule in them. The `Grep` hints find candidates fast; a hint is where to look, never a finding by itself. A section that can't apply to a file's area (video rules in a CPU core) counts as considered.

Then check the change's **siblings**. When it fixes or changes one member of a family — an opcode across its addressing modes, one of several register handlers, one of several drivers or devices sharing code — read the other members. A sibling the change should also have covered is a finding against the change; a sibling with an unrelated old defect goes under **Outside this change** in the report. For emulation fixes, read the result against the datasheet or reference the PR cites.

For a change above roughly 4,000 changed lines, when you can start subagents and aren't one yourself, split the files into groups and review each group in a parallel subagent, handing each the diff for its files, the style lines from step 2, and the paths of the rule files to read. Step 5 still verifies every candidate they return.

Done when every rule in the listed files has been considered against every touched file, and every sibling family has been read.

### 5. Verify every finding

For each candidate, reopen the exact line and confirm all four:

1. The change added or modified the line.
2. No **Not when** clause of the rule applies.
3. For a layout or naming rule, the file's existing style does not already do the same thing. (For API, idiom, correctness and metadata rules, the file's precedent exempts nothing: existing bad code is no precedent.)
4. For a Blocking finding, the concrete failure is traced: which save state breaks, which read has a side effect, which reset leaves which value wrong.

A finding with no line (most Submission rules) takes checks 2 and 4 only. Drop any candidate that fails checks 1–3 or turns out to be outside its rule's scope. A Blocking candidate whose failure can't be traced (check 4) drops to Tidy if a core developer would still ask for it, and is dropped otherwise. A survivor that is only taste is a Nit or nothing.

### 6. Check the submission

Read [SUBMISSION.md](SUBMISSION.md) and apply it to the PR title and body and to the commit messages. In working-tree mode with no commits yet, carry the submission reminders into the report instead.

### 7. Report

```
## MAME review: <PR #N title | range | paths>

Checks: include guards ✓ · xmllint n/a · srcclean ✗ (bar.cpp: 3 lines) · CI skipped (no PR)

### Blocking
- `src/mame/foo/bar.cpp:123` [C-1] `status_r()` clears `m_irq_pending` on every read, so opening a debugger memory view acknowledges the interrupt. → compute the value, then clear the flag only under `if (!machine().side_effects_disabled())`.

### Tidy
- ...

### Nits
- ...

### Submission
- ...

### Outside this change
- `src/mame/foo/bar.cpp:456` (untouched) ...

<one line: counts per tier and the single most important fix>
```

Each finding: `path:line`, rule ID (several when one line breaks several rules, most specific first), the offending code quoted, and the concrete fix. Within each section, order by file, then line. A rule hit many times in one file is one finding: the first line, the count and the range ("~30 uppercase hex literals, :250–:490"). When Tidy passes about fifteen findings, lead with the ones that need thought and fold the mechanical ones (hex case, spacing, srcclean, `ATTR_COLD`) into one line per file. For a PR that describes itself as unfinished, report Blocking findings and structural Tidy ones, summarise the rest by rule, and suggest marking it as a draft. An empty section is left out; a review with no findings says so in one line. **Outside this change** holds a few pre-existing problems spotted in untouched code, worth a follow-up but not the change's responsibility; they aren't findings and aren't counted.

### 8. Post to GitHub (only when asked)

Posting happens only when the user explicitly asks for it. Show the exact text first and wait for confirmation, then post a comment review: `gh pr review N -R mamedev/mame --comment --body-file <file>`, or inline comments through `gh api repos/mamedev/mame/pulls/N/reviews`. Approving or requesting changes needs its own explicit request. Write it the way MAME reviewers do: short, specific, one point per comment, with the reason — often as a question ("Should this be `constexpr`?", "Why is this public?"), which contributors read as a change request.
