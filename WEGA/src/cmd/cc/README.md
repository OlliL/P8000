# src/cmd/cc: the C compiler of WEGA

The driver and the passes of the C compiler in one tree, cut as the SCCS lines of the binaries show the original was
cut, and as close to cc/pcc of System III (and Plexus) as byte identity allows (host/increments/155-compiler-tree.md).

```
cc.c        the driver: cc (nonsegmented), scc (-DSCC); the drivers of the old compilers (OLD_V0 ... OLD_VL)
Makefile    the driver
mip/        the machine-independent units the passes share, one file per SCCS version, included by the wrappers:
            common.2.4 (gopt), common.2.5 (cparse, codgen), optim.2.5 (cparse, codgen), rdr.2.4 (gopt),
            rdr.2.5 (codgen); manifest (all three); rdr.h (gopt, codgen: the symbol of the symbol table file)
z8000/      what is machine-dependent and shared: macdefs (all three), mac2defs (codgen: the registers)
cpp/        the preprocessor (/lib/cpp)
cparse/     pass 1 (/lib/cparse, /lib/scparse): the files ...1.c, mfile1; comm1.c and optim1.c are the wrappers
gopt/       pass 2, the global optimizer (/lib/gopt): the files ...2.c, mfile2.h; comm2.c and rdr2.c are the wrappers
codgen/     pass 3 (/lib/codgen, /lib/scodgen): the files ...3.c, mfile3, oldcc.h; comm3.c, optim3.c and rdr3.c
            are the wrappers
popt/       the peephole optimizer (/lib/popt, /lib/spopt): z20.c ... z23.c
```

- A wrapper has the SCCS line of its own (comm3.c 2.1) and includes the unit, which starts with its SCCS line, as
  comm1.c and comm2.c of System III include common: `#include "../mip/common.2.5"`; the pass headers include
  `../z8000/macdefs` and `../mip/manifest`. Relative paths, not -I: two more -I options overflow the command buffer
  of the shipped cc in the vc builds (host/KNOWLEDGE.md, quirks of /bin/cc). Tools that copy a pass (d12model,
  switchmatrix, oldvar.sh) copy mip/ and z8000/ too.
- A pass has one header, as in System III: mfile1 (cparse), mfile2.h (gopt, the name of the original), mfile3 (codgen); it includes macdefs, manifest
  and holds what the modules of the pass share. A module declares what only it uses (in codgen: the symbol table of
  cparse v0p, which builds the shipped codgen, has no room for all declarations in local3.c); functions that return
  int are not declared.
- A unit that two passes share is one file: what one pass has of its own stands under the macro of the pass, defined in
  its header (PASS3: codgen, mfile3); the other pass's text is the # else. The declarations a pass needs beyond its
  headers are in its wrapper.
- Two versions of a unit (common.2.4, common.2.5) are written as alike as possible: `diff mip/common.2.4
  mip/common.2.5` shows what the version changes.
- The switches of the old compilers (OLD_*) stand in the passes, not in mip/, except those listed in
  host/increments/155-compiler-tree.md (Decisions).
- compare/NAME is named by the program (compare/scodgen ...), the host ports are in host/cc with the same cut.
