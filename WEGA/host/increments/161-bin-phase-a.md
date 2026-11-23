# 161 The programs of bin: phase A only (unattended run)

## Status

active (owner's go 2026-11-23)

## Goal

Every program of `bin` that COVERAGE shows as `sys3` or `todo` worked on once with source forms only (phase A), after ex;
what stays open is recorded in the program's `findings.json`. Part of 160; a batch that runs without questions to the owner.

## Done when

- Every program of the list below is ticked "worked on" (exact or not; the state is in `findings.json` and COVERAGE).
- checkall ALL OK on the last commit; `src/COVERAGE.md` regenerated.

## Read first

- `../../CLAUDE.md`, `host/HOWTO_reconstruct.md` (setup.py, phase A, source forms, "No source").
- `host/OLDCC.md` section 2 (which compiler a dating points to), `host/OLDCC_DATES.md`.

## Rules of this run (owner 2026-11-23)

1. **Source forms only.** No new compiler switch, no change of an existing switch, no change to the compiler at all
   (cpp, cparse, codgen, popt, cas, ld, the drivers). What a source form cannot remove stays as a difference in `findings.json`.
2. **Libraries.** A new oldlibc version is allowed. First check whether an existing `libc-vXXX.a` can be extended by the
   member without another program losing (all programs of that library still identical); only then a new version.
3. **Links and copies.** A program that is a hard link or a 1:1 copy of another (same inode or `cmp` equal) is no program of
   its own: it goes into the Makefile of the original as `ln` (or `cp`), with the install target, nothing else.
4. **Time limit.** About 1 h per `sys3` program, about 2 h per `todo` program (no source), about 2-3 h for ex. Then record
   the state in `findings.json`, tick it here, go on.
5. **Setup.** Every program starts with `python3 host/recon/setup.py NAME [ORIG]` (nonsegmented); the HOWTO's steps follow.
   A segmented program (magic E6xx) by hand after the HOWTO. setup.py stops at a split I/D program (magic E711: ex, diff,
   sh, make, csh, adb); the recon tools (host/recon, not the compiler) may be extended for E711, measured on ex first.
   setup.py keeps an existing `findings.json` and its functions (own_table.py adds the missing ones by orig).
6. **Commits.** Per program (or a few together) after `checkall` ALL OK; no push. A change that breaks checkall and cannot be
   fixed in the time limit is reverted; the finding goes to `findings.json`.
7. **Documents.** `findings.json` per program (the setup format: `functions` with name, file, orig, size_orig, status, note);
   the quirks table of KNOWLEDGE.md and the table "Objects and their differences" of OLDCC.md kept current; this file ticked.
8. **Processes.** timeout on every z8k-emu run; long runs in the background; no leftovers (`ps`).
9. **No questions.** What would need the owner goes to "Decisions" below as an open question; the run goes on with the next program.

## Steps

0. ex: run setup.py (progcfg.py missing; `findings.json` is not in the setup format: move it aside, let setup write the
   function table, carry the old notes over into the notes of the functions and a `note` key), CC ccv0a; then phase A
   on the open functions (indirect call: listchar, normchar, vgotab; structure copy register: setty, gTTY; getone; dosub).
   The links edit, vi, view of ex: `ln` in the install target of ex once ex is installed (not yet: not exact).
   Done 2026-11-23: setup.py takes E711 (and the termlib before the libc, the zero padding of the code); compare/ex has
   progcfg.py (ccv0a, OWN_LIM 0xe96e) and findings.json in the setup format (394 functions by name, 359 exact).

The list, in this order (smallest first; the size of the original in bytes). Ticked = worked on, no statement whether exact.

### sys3 (source in tmp/sys3)

- [ ] test (2478)
- [ ] tr (6532)
- [ ] uuname (6538)
- [ ] nice (7218)
- [ ] od (7526); link: hd
- [ ] df (7718)
- [ ] time (7812)
- [ ] date (8550)
- [ ] dd (8612)
- [ ] regcmp (8844)
- [ ] file (9254)
- [ ] getfile (10002)
- [ ] local (10398)
- [ ] who (11540)
- [ ] nm (12026)
- [ ] diff (12258)
- [ ] egrep (12618)
- [ ] uulog (12784)
- [ ] ls (13512)
- [ ] newgrp (13898)
- [ ] passwd (15188)
- [ ] stty (15702)
- [ ] ed (16750)
- [ ] mail (18572); link: rmail
- [ ] login (19030); same file as etc/login
- [ ] uux (21136)
- [ ] ps (22794; segmented E607, last of this group)

### todo (no source)

- [ ] echo2 (1834)
- [ ] printenv (3998)
- [ ] whereis (6444)
- [ ] dog (8330)
- [ ] putfile (9752)
- [ ] uusend (13216)
- [ ] remote (15162)
- [ ] gpasswd (15362)
- [ ] sh (24032); link: rsh
- [ ] make (39048)
- [ ] csh (45728)
- [ ] adb (46996)
- [ ] fsc (2078; segmented E611)
- [ ] fc (3074; segmented E607)
- [ ] pstat (15638; segmented E607)
- [ ] gld (27616; segmented E607)

Links found 2026-11-23 (same inode in the WEGA root): hd = od, rmail = mail, page = more (more: partial, src/cmd/more),
rsh = sh, edit = vi = view = ex, bin/login = etc/login. Not in this run: `as` (170), `chkout` (exact).

## Decisions

- 2026-11-23 (owner): phase A only, rules above.
- 2026-11-23 (owner): extending setup.py / inventory.py for E711 counts as tool work, not compiler work.

## Not part of it

Compiler changes of any kind; `as` and the PLZ tools (170); usr/bin, etc, usr/lib (after this run).

## Log
