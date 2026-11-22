# 155 The compiler sources under src/cmd/cc, the shared parts in mip

## Status

done (2026-11-22; checkall ALL OK on 45c2e3d4's tree; 160 active again). The decisions marked "Claude, for review" wait for the owner.

## Goal

The sources of the C compiler in one tree, cut as the original was most likely cut (the SCCS lines of the binaries):

```
src/cmd/cc/
  README.md       the tree, the rules below (what is fact, no log)
  Makefile        builds the passes in their order (as cc.mk of System III)
  cc.c            the driver (cc, scc, ccvN)
  mip/            the machine-independent parts that the passes share, as include files, no build of their own:
                  common.2.4, common.2.5, rdr.2.4, rdr.2.5, optim.2.5 ... (the SCCS units: one file per version),
                  shared headers where they are the same in the passes
  z8000/          what is machine-dependent but shared by passes (only if the measurement finds such parts)
  cpp/ cparse/ gopt/ codgen/ popt/
                  one pass each: its Makefile, its own files (the 1, 2, 3 of the names: cparse, gopt, codgen; z2x:
                  popt), the wrappers that include mip (comm1.c = #include "../mip/common.2.5"), build/ of the old
                  compilers
host/cc/          the same cut for the host ports (host/cc/mip, host/cc/cparse ...)
```

Rules of the target:
- mip/ holds only what was shared in the original: a unit with an SCCS line of its own (common, rdr, optim); headers only
  when they are the same in the passes. What belongs to one pass stays in the pass.
- Versions: one file per version (`common.2.4`, `common.2.5`), the two as alike as possible, so that a diff shows exactly
  what the versions change.
- No OLD_* (D) switch in mip/: the code a switch needs goes to the pass that needs it (`d3kjump` of comm3.c: code nobody
  wrote so; it can be called before the users of `branch()` in codgen). Where that does not work, the exception is
  written down here.
- Near System III: every source under cc/ is as close to cc/pcc of System III (and Plexus, a System III) as byte
  identity allows, in the text of the files and in the structure of the files (names, cut, order). The base for mip is the
  shared part of cparse: comm1.c (common) and optim1.c (optim) are already brought close to System III.
- compare/NAME stays named by the programs (compare/gopt, compare/scodgen ...), whatever the tree.
- cpp and popt share nothing with mip; they are under cc/ because they are passes of the compiler.

## Done when

- `src/cmd/cc/{cc.c, mip, cpp, cparse, gopt, codgen, popt}` and `host/cc/...` hold the compiler; the old directories are gone.
- Every mip unit exists once per version; no OLD_* switch in mip/ (or each exception listed under Decisions).
- Every build is byte-identical as before: checkall ALL OK; `oldcc.sh build`/`model` of v0, v0p (strictly equal) and the
  bench; `oldcc.sh matrix check`, `MATRIX_VER=vc ... matrix check`; /lib/gopt of the vc build byte-identical.
- The tools and documents use the new paths (compare/*/progcfg.py and corpus.py, checkall, oldcc.sh, switchmatrix,
  goptmodel, host/recon/poptveto, src/cmd/Makefile, KNOWLEDGE, OLDCC, HOWTO, COVERAGE).
- `src/cmd/cc/README.md` describes the tree; CLAUDE.md names the exception to "V7 layout in src/cmd" (with the owner's OK).

## Read first

- This file; `compare/gopt/HANDOFF.md` (the checks after a compiler change, the vc build of gopt).
- `host/OLDCC.md` section 3 for a switch that has to move out of mip.
- System III: `tmp/sys3/usr/src/cmd/cc/pcc/mip` (INDEX, comm1.c and comm2.c: wrappers of `common`); Plexus:
  `tmp/plexus/cmd/cc/pcc.d/mip`.

## Known

- The pass of a file is in its name (SCCS lines of the binaries): 1 cparse (cgram1, trees1, pftn1, scan1, code1, local1,
  ...), 2 gopt (quads2, flow2, reg2, ...), 3 codgen (allo3, order3, match3, ...), z2x popt.
- Shared units: `common` 2.5 (08/24/83) in cparse (comm1.c 2.1) and codgen (comm3.c 2.1), 2.4 (03/04/83) in gopt
  (comm2.c 1.1); `rdr` 2.5 in codgen (rdr3.c 2.1), 2.4 in gopt (rdr2.c 1.3); `optim` 2.5 (10/06/83) in codgen (optim3.c
  2.4); cparse has optim1.c 2.1. The wrappers with an SCCS line of their own beside the unit's line: as comm1.c of System
  III (`#include "mfile1"`, `#include "common"`). Today the unit is written into each wrapper.
- cparse is already brought close to cc/pcc of System III: the shared parts in comm1.c (`common`) and optim1.c (`optim`)
  follow the System III sources (as far as the bytes allow).
- Measured 2026-11-22 (step 1, the SCCS lines of the binaries; host: `fdiff.py`-like comparison of the functions after
  removing comments and blanks):
  - Units and their wrappers, in the data of each binary the wrapper's line directly before the unit's: cparse/scparse
    comm1.c 2.1 + common 2.5, optim1.c 2.1 + optim 2.5; codgen/scodgen comm3.c 2.1 + common 2.5, rdr3.c 2.1 + rdr 2.5,
    optim3.c 2.4 + optim 2.5; gopt comm2.c 1.1 + common 2.4, rdr2.c 1.3 + rdr 2.4. So optim 2.5 is in cparse and in
    codgen; rdr is a unit of WEGA (System III has none, nor Plexus).
  - common: the functions of System III (uerror, cerror, werror, tinit, talloc, tcheck, tfree, tfree1, fwalk, walkf,
    mkdope; tprint in 2.4 only) plus WEGA's (bsize, szty, argsize, where = prline, tbytes, dexit, branch, ncopy, tcopy,
    ispow2, plainicon). cparse follows System III (most functions equal); gopt and codgen are written alike between
    them (most equal or a few lines) but not after System III (5 to 30 lines differ per function).
  - Only in one pass (the wrapper's or the pass's code, not common): cparse syserr, nncon, getmem; codgen syserror,
    callreg, the D3K/D3J helpers (d3kjump ...), alloc, allocinit; gopt taballoc, tabfree, tabavail.
  - optim: cparse fortarg, optim, conval, treeeq, rmconv, notflt; codgen optim, conval, treeeq, convelim, isint
    (optim of codgen differs in 227 lines from System III's, cparse's in 84).
  - rdr: codgen and gopt share rdsym, readsyms, readtab, readnums, readmems, sortsyms, tagfind, mapn, mapsym, mapnm,
    mkhash, nlcheck, eread, rdin, rdin2 (2.5 against 2.4: mostly a few lines; mapnm 58); codgen only rddata and the
    D12K helpers, gopt only prtags.
  - The names of the functions are ours where the binaries are stripped (where in cparse, prline in codgen and gopt:
    the same function).
- `/usr/lib/lint1`, `/usr/lib/lint2` (not rebuilt yet) use the same mip in System III: later members of the tree.
- References to the old paths (2026-11-22, without findings.json): src/cmd/cc 9 files, cpp 8, cparse 11, codgen 16,
  popt 20, gopt 11; most in host/recon (poptveto), host, compare.

## Steps

1. (done 2026-11-22) Measure the mip candidates, from the System III-near parts of cparse as the base: the shared text of common, rdr,
   optim (and of manifest, mfile1, mfile2, the operator tables, macdefs) in cparse, gopt, codgen; per unit the versions
   and their differences; where pass-specific code and OLD_* switches sit inside the shared text. (Units measured, see
   Known; the headers follow in step 4.)
2. (done 2026-11-22) Move 1:1: cpp, cparse, gopt, codgen, popt below `src/cmd/cc/` (git mv, nothing else changed); the Makefiles (relative
   paths, SUBDIRS and the case of `make shipped` in src/cmd/Makefile, src/Makefile); the tools and documents on the new
   paths (see Done when); all builds byte-identical; checkall.
3. (done 2026-11-22) Align in the present directories (cparse, gopt, codgen), not yet in mip: move code between the files of a pass, put
   the declarations into their headers (mfile1, mfile2, ...), take the units common, optim, rdr out of comm1/2/3.c,
   optim1/3.c, rdr2/3.c into files of their own (the wrapper keeps its SCCS line and includes the unit), then bring the
   unit texts of the passes together (from cparse's System III-near text), the OLD_* and pass-specific code out of the
   units into the passes; all builds byte-identical after each step.
4. (done 2026-11-22) When the future mip parts are as equal as they can be: move them to `src/cmd/cc/mip/` (one file per version; equal
   headers); the wrappers include `../mip/...`; byte-identical; checkall.
5. (done 2026-11-22) host/cc/: the same cut for the host ports (one host mip shared by the host passes); the corpora equal.
6. (done 2026-11-22) `src/cmd/cc/README.md`; CLAUDE.md (with the owner's OK); a line in host/HANDOFF_wega_disasm.md (decisions).

## Decisions

- 2026-11-22 (owner): target A (one tree src/cmd/cc with mip/); mip holds only what was shared (SCCS units, equal
  headers); one file per version (`common.2.4`, `common.2.5`), as alike as possible; no D switches in mip/, their code
  goes to the passes that need it (aim, exceptions written down); host/ mirrors the cut; compare/ stays by program;
  cpp and popt under cc/ without mip. 160 paused for this increment.
- 2026-11-22 (owner): every source under cc/ as close to cc/pcc of System III / Plexus as byte identity allows (text and
  file structure); mip starts from the shared parts of cparse (comm1.c, optim1.c), which are already close to System III.
- 2026-11-22 (owner): first the 1:1 move below cc/; then the alignment in the present pass directories (code moved,
  declarations into headers, the units into files of their own, their texts aligned); only when the future mip parts
  are as equal as possible, they move to mip/.

- 2026-11-22 (owner): for 155 codgen may be restructured without asking (moving code, aligning texts) as long as every
  build stays byte-identical; no new or changed switch. Names in the shared parts: System III's (or Plexus'), else
  cparse's, renamed in all passes and the host ports. A switch part that cannot leave a mip unit without losing the
  bytes: it stays, the exception goes here with its reason, the work goes on. CLAUDE.md: one line at "V7 layout"
  (src/cmd/cc is a tree, see its README).

- 2026-11-22 (Claude, for review): what only one pass has inside a shared unit stands under a macro of the pass,
  defined in its header: PASS3 for codgen (mfile3); cparse's text is the # else. (common 2.5 is one SCCS file in cparse
  and codgen with different code in both: FLD of the dope table, argsize, callreg, branch, tcopy, nncon, the memory of
  getmem; so the original had such conditionals too, their form is not known.) The declarations a pass needs beyond
  its headers are in its wrapper (comm3.c: sbrk returns a pointer, node, lastfree). The variants of the old
  compilers built with cc (v0 ... vl) need not be byte-identical with their build before such a change, only behave
  the same: models, bench, matrix checks and checkall (the programs they build).

- 2026-11-22 (Claude, exception to "no OLD_* in mip", for review): optim 2.5 keeps OLD_D23 (codgen v0p: no zero
  addend away). Outside optim every way changes how optim folds constants (a hidden zero is no constant for conval);
  it is an older optim without that rule. The other switches went out of mip (owner 2026-11-22: other logic for the
  same aim): OLD_D12K by a walk behind eread (only an FCON gives the record "Ln: " with a blank in the data file, so a
  NAME of a label that waits there was an FCON); OLD_D24, OLD_D63 and OLD_D13 by a walk in front of optim and one
  behind it (reader3.c optpre, optpost): a conversion the older rmconv keeps is hidden as PCONV (rmconv acts only on
  the direct children of a node, before optim goes below), the mask of OLD_D24 is dropped behind optim (optim has no
  SCONV case under PASS3), the swap of OLD_D13 is done in front (not the same when optim makes the right side a
  constant address only on its way; no case in the measurements).

## Not part of it

- lint1, lint2 (later members of the tree).
- The consolidation of the switches (compare/gopt/HANDOFF.md, steps 10 and 11), except moving a switch's code out of mip.

## Log

- 2026-11-22: increment made (owner); 160 paused.
- 2026-11-22: step 2: cpp, cparse, gopt, codgen, popt moved below src/cmd/cc (git mv); Makefiles (one level more to
  head/ and lib/, OLDPRINTF5 in src/Makefile, SUBDIRS cc/PASS in src/cmd/Makefile), .gitignore, tools and documents
  (not OLDCC_LOG.md, not the corpus inputs); checkall ALL OK.
- 2026-11-22: step 2 checks: models 179/179/209/210/73/73/26 strictly equal, bench 153/132/212, both matrix checks as
  in the table (they print "77 files of the passes changed since": the move); switchmatrix, goptmodel, datastr,
  progcfg (passes below cc/) and the progcfg of cpp, cparse, codgen, popt (host/ld, lib/ one level up) fixed.
- 2026-11-22: step 3: units split from the wrappers, manifest and macdefs shared, names unified, common 2.5 and optim
  2.5 one file for cparse and codgen (PASS3), common 2.4 from the text of 2.5, OLD_D3K/D3J and most of OLD_D12K out
  into reader3.c. Step 4: mip/ (common.2.4, common.2.5, optim.2.5, rdr.2.4, rdr.2.5, manifest), z8000/macdefs;
  the wrappers include ../mip/UNIT (not -I: the cpp command of the shipped cc is limited to cmd[200]); the tools
  that copy a pass copy mip and z8000 too. All variants byte-identical; models, bench, matrix checks as before.
- 2026-11-22: step 5: host/cpp, cparse, codgen, gopt, popt below host/cc (674f0d0c); the host units split, named as
  in src, common 2.5 and optim 2.5 one text for host cparse and codgen (PASS3), common 2.4 from it, manifest and
  macdefs shared, host/cc/mip and host/cc/z8000 (fcf10df7, b97b433e); host corpora equal (452, 427, 854). A checkall
  under the load of parallel model runs failed by timeouts only (UNSTABLE, exit 15); the clean run follows.
- 2026-11-22: checkall ALL OK (796 files identical, gopt 854 of 854 for wega and host); Done when holds; done.
