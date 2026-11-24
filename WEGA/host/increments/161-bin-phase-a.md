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

- [x] test (2478)
- [x] tr (6532)
- [x] uuname (6538)
- [x] nice (7218)
- [x] od (7526); link: hd
- [x] df (7718)
- [x] time (7812)
- [x] date (8550)
- [x] dd (8612)
- [x] regcmp (8844)
- [x] file (9254) -- deferred: WEGA rewrote it (Z8000, PLZ, s.out types; about 3 KB of own code)
- [x] getfile (10002)
- [x] local (10398) -- deferred: 02/84, linked with a libc we do not have (large gaps)
- [x] who (11540)
- [x] nm (12026) -- deferred: WEGA rewrote it for s.out (vs.out, segmented, line tables)
- [x] diff (12258)
- [x] egrep (12618) -- deferred: built by yacc; the yacc parser of WEGA is not reconstructed yet
- [x] uulog (12784)
- [x] ls (13512) -- deferred: 2BSD ls with WEGA options; needs a libc with setbuf, qsort, fgetc, fread
- [x] newgrp (13898)
- [x] passwd (15188)
- [x] stty (15702) -- deferred: 12/83, its doprnt/printf are of a libc we do not have
- [x] ed (16750) -- deferred: needs libc-v4 members (fakcu, strncpy, pipe, dup, chown) and setret; 69 functions
- [x] mail (18572); link: rmail
- [x] login (19030); same file as etc/login -- deferred: rewritten by WEGA (ttytype, termcap, csh) and a libc with large gaps
- [x] uux (21136)
- [x] ps (22794; segmented E607, last of this group) -- deferred: segmented, with the WEGA modules kva.c and kpa.c

### todo (no source)

- [x] echo2 (1834)
- [x] printenv (3998)
- [x] whereis (6444)
- [x] dog (8330)
- [x] putfile (9752)
- [x] uusend (13216) -- deferred: a WEGA form of the 4.2BSD uusend (-f, directories, mkdir) with two libc gaps
- [x] remote (15162) -- deferred: about 5 KB of own code (24 functions), no source
- [x] gpasswd (15362)
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
- 2026-11-23 step 0 ex worked on: 361 of 394 exact (360 + crt0); open: the compiler clusters (indirect call, structure copy
  register, getone, dosub: the shipped cc folds `a - b` of two arrays to 0, KNOWLEDGE.md quirks) and functions with only
  call/calr; no source change taken.
- 2026-11-23 sys3 batch 1 (state in each findings.json; COVERAGE after the next checkall -b):
  IDENTICAL: test (V7 test.c + System III operators, ccv1, libc-v6 + fakcu), tr (usage of WEGA), uuname and uulog (BSD
  uucp: 4BSD uuname.c, 3BSD uulog and modules, uucp.h of 3BSD), nice (V7, libc-v4 + fputs 0_2, execvp 0_2, sleep 0_3 ...),
  who (-g of WEGA, shipped cc), diff (Plexus diff.c, spare locals), newgrp (V7 with WEGA changes, libc-v12), passwd (lock
  with lkdata/unlk, a spare long). Partial (installed, OPEN of checkall): od/hd, df, time, date, dd, regcmp, getfile.
  Deferred with the reason in the list: file, local, nm, egrep, ls, stty, ed. corpusrun: RC_UNDEFINED (od).
- 2026-11-24 sys3 batch 2: mail IDENTICAL (System III mail.c with WEGA changes, libc-v12, rmail a link), uux partial (3BSD
  uux and modules with WEGA changes, the frame of main); login deferred. The corpus of uulog wrote LOGFILE into the WEGA
  root once (removed again); uulog and uux corpora now write into the layer (ROOT_COPY of usr/spool/uucp/remotelines).
- 2026-11-24 todo batch 1 (no sources): IDENTICAL echo2 (V7 echo on stderr), printenv (2BSD), whereis (2.9BSD with the WEGA
  directories), dog (written from the code; libc-v6 + freopen, setbuf, ttyname 0_2, strcat 0_1, fcntl, fstat), putfile
  (written from the code); partial: gpasswd (two temporaries of main). Deferred: uusend, remote, ps.
- 2026-11-24 phase B ex: byte-identical (with edit, vi, view): v0a OLD_D65 (dosub), OLD_D66 (setty, gTTY), OLD_D5S extended
  (getone), shrink.sh (indirect call temporary; ASCII sort), the old cas (the 24 call/calr functions), exbss.c (the order of
  the commons), /lib/ex2.13strings, libc-v11 extended by the members of ex. checkall -b ALL OK, 847 files.
- 2026-11-24 phase B od/hd, df, time, date, dd: byte-identical (OLDCC_LOG.md of the day: v1r OLD_D67 and its own popt
  without the v1 switches, v1 codgen with OLD_D53, calloc 0_2 with int size, the (long) casts of df alloc not under SHIPPED,
  the register declarations of time and date where their frame words lie (OLD_D1), libc-v13 and two forms in dd).

## Phase B regcmp: open, for a later session (2026-11-24)

State: `/bin/regcmp` partial. main is the System III regcmp.c and exact; open is only the member regcmp of libPW in it
(`compare/regcmp/findings.json`, functions from 0x061e). No change of the tree was made for it; nothing in the scratch
directories is worth keeping (all forms below were trial copies).

What is known:
- Source: `src/lib/libPW/regcmp.c` (WEGA, 281 lines, also the member of /usr/lib/libPW.a, which is byte-identical) is the
  nearest: ccv1 238 differing lines. The System III and Plexus versions (libPW, acct/lib) are farther (323-326).
- Masking the frame offsets, about 24 lines per side differ. The frame of the original is 4 bytes larger: two words at 0
  and 2 below the parameter homes (`ldm %0004(r15),r2,#6` against `ldm @r15,r2,#6`), i.e. `~L1 := 4`: temporaries of the
  compiler, not variables of the source (a source variable, also an array, lands above the homes).
- Two places use them, a condition with `||` / `&&` evaluated as a value into the temporary and tested at once:
  `if ((c=='*') || (c=='+') || (c=='{'))`: `cp r12,#'*'; ldk r2,#0; tcc eq,r2; ld @r15,r2; test r2; jr ne` and the same
  for '+' with `test @r15`; the last operand '{' is a plain `jp eq`.
  `while (((c = *sp++) != '}') && (c != ','))`: `tcc ne,r2; ld 2(r15),r2; inc r10,#1; test r2` (the increment of sp
  behind the store), then `cp r12,#','; tcc ne; ld 2(r15),r2; test 2(r15)`.
- No compiler of the tree makes this: ccv0, ccv0p, ccv1, ccv1q, ccv1r, ccv1c, cc, with and without -O (all `cp; jpr eq`).
- Source forms tried (about 40, with ccv1, ccv1q, ccv1r, all three alike): `?:` chains, `(a||b)?1:c`, `?1:0` per operand,
  `|`, `&`, sums, `!= 0`, de Morgan, `while` with `?:`; flag variables `(t = (c=='*')) || ...` declared first, last, as
  arrays (best: 30 lines, the instruction forms right, the flags above the parameter homes, not at 0/2); constant forms
  (`0 ||`, `|| 0`, `1 &&`, `sizeof(int) &&`, a constant `?:`; the owner's idea of a folded constant leaving code behind as in
  switch(sizeof)): cparse folds them, no tcc at all.

Other witnesses of the same compiler form (`host/recon/tccscan.py` over the WEGA root):

| Binary | Places | Note |
|---|---|---|
| bin/regcmp | 4 | the libPW member |
| usr/bin/acctcom | 4 | the same regcmp code (at 0x172e) |
| usr/bin/lex | 36 | System III lex source exists: the best ground for the rule |
| etc/shut | 8 | `c == ' ' \|\| c == '\t'` among them |
| usr/bin/error | 3 | `tcc gt` into 0x82(r15) |
| usr/bin/units | 2 | 01/14/83; System III units.c getflt: `do c = get(); while(c == ' ' \|\| c == '\t');` |
| bin/ed, adb, ex (vi, view, edit), usr/bin/strip, usr/lib/diff3prog, lint2 | 1 each | not looked at (may be plain values) |

units getflt in detail: the original `sub r15,#0x20`, the temporary at `@r15`, the local f separately at 0x18 (`clr 0x18(r15)`);
both compares into `@r15` and tested, `jr ne` back into the loop. With `(f = (c==' ')) || (f = (c=='\t'))` ccv1 gives the
instruction forms but f at 0x16 and no extra word: again a temporary of the compiler. units (01/83) also has `get()`
returning a char (extsb) and a smaller frame than ccv1 (0x20 against 0x26): the WEGA units differs from System III.

Next steps proposed:
1. Derive the rule from lex (36 places, System III source): classify every `||` / `&&` of lex.c into "through a temporary"
   and "plain jumps" and find what separates them (context: loop condition, if; operand kinds: register variable against a
   constant; position in the chain: the last operand is always a plain jump).
2. Check the rule on units, shut, error, regcmp; find which compiler built them (units 01/14/83: v1, v1q, v1r time;
   host/OLDCC_DATES.md).
3. Build it as a switch only in a compiler version these programs need (a new one if necessary), so that the users of
   ccv1, ccv1q, ccv1r stay untouched (owner: fear of other users); then regcmp, units, acctcom, lex could follow.
