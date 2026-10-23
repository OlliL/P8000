# MAME review rules: submission

Read in step 6 of SKILL.md, for the PR title and description and the commit messages. The legend, tiers and source notation are described at the top of [RULES.md](RULES.md); the calibration in [SKILL.md](SKILL.md) governs every rule.

## S. Submission

### S-1 AI use · Blocking
A pull request made with AI assistance says so in its initial description, naming the model and version; a team member committing directly says so in the commit message (`AI disclosure: assisted by <model and version>` is the form in use). AI help isn't grounds for rejection; hiding it, or submitting work the author doesn't understand, is. The description and every reply to reviewers are the author's own, concise words — pasted chatbot analysis has been called "ban-worthy" and "AI slop" reverted. Flag a missing disclosure when the PR or its commits show AI involvement (`Co-authored-by:` an AI, "Analysis by …"/"Generated with …" credits, narration comments per L-4), or when the user says AI helped. Flag a description that reads as generated — templated section headings, walls of bullets, claims the diff doesn't support — as Tidy, asking for a short account in the author's words; polished prose or thorough citations alone are not evidence.
- Source: contrib: Use of AI; #15168, #15031, #15032, #16043

### S-2 Titles, descriptions and commit messages · Tidy
The PR title says what the change affects and what it does ("Your pull request title needs to say what your pull request affects"), conventionally prefixed with the path: `sega/segas16a.cpp: Fixed sprite priority.`, `cpu/m68000: …`, or several comma-separated paths when a change spans areas (`homebrew/arduboy.cpp, video/ssd1306.cpp: …`). The description is concise, never empty — detail isn't left to comments — and matches the diff: dump credits for new sets, working-status claims that match the flags, sources for metadata or behaviour changes. Commit messages are descriptive enough that a reader of the log needn't open the diff, and are never GitHub's defaults ("Update foo.cpp"). New systems, clones and software items, and promotions to working, are listed in the commit message under the release-notes headings, each underlined with dashes, one `Title (qualifier) [credits]` per line. A clone goes under a clones heading, never a systems one:

```
New working systems / New systems marked not working
New working clones / New clones marked not working
New working software list items / New software list items marked not working
Systems promoted to working / Clones promoted to working / Software list items promoted to working
```

- Source: contrib: Contributing to MAME's source code; 87c975f418a, 2969c05d51d; #15559, #14609

### S-3 Scope · Tidy
A PR is about one thing; independent or core changes go in separate PRs so they can be discussed on their own. It doesn't reformat, rename or re-whitespace code it doesn't otherwise change ("It's too hard to see what you've changed"), doesn't reverse srcclean output, and doesn't slip in changes to defaults, `supported` status, short names or behaviour. It carries no unrelated files, build output or editor settings, and a merge from master doesn't overwrite upstream changes. Existing bad code is no precedent: "There's a lot of bad code in MAME. That doesn't mean we want more of it."
- Grep: whitespace-only or rename-only hunks in files outside the PR's purpose; `.gitignore`, `.vscode/`, build products in the file list
- Source: 6e2d6cd2c56; cxx: Introduction; #14942, #13264, #14759, #12674, #15668

### S-4 Testing · Tidy
A full build completes; a `DEBUG=1` build compiles its assertions and doesn't trip them; `mame -validate` passes; the changed system runs; CI is green on the contributor's own branch before the PR opens ("you can check CI on your branch before opening a pull request"). A red CI check is Blocking. Work in progress is a draft PR. A new computer or device that needs media to start should come with a software list containing at least one example, and a non-arcade system that needs setup should get usage notes on the MAME wiki (both Nits).
- Source: contrib: Contributing to MAME's source code; #13738
