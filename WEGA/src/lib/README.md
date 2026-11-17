# lib - the libraries of WEGA

`make` builds the shipped libraries from their sources, each byte-identical with the file of the WEGA 3.1 tree, and the libraries of
the time of the old programs; `make install PREFIX=dir` installs the shipped ones. Each Makefile names the members in the order of the original archive
(`arhdr*.s`: the member headers of the original, `mkar.sh` joins them).

| Directory | Builds | Compiler | Notes |
|---|---|---|---|
| `libc` | `/lib/libc.a`, `/lib/crt0.o` (`build/nonseg`); `/lib/slibc.a`, `/lib/scrt0.o` (`build/seg`) | ccvl, sccvl (vl, `host/OLDCC.md`) | one tree: `common/` both, `nonseg/` only libc.a, `seg/` only slibc.a (C with `#ifdef SEG`; PLZ/ASM and cas sources); `host/KNOWLEDGE.md`, "libc.a and slibc.a: one tree" |
| `libl` | `/lib/libl.a`, `/lib/slibl.a`, `/usr/lib/lex/ncform` | ccv1, sccv1 | System III sources: `host/KNOWLEDGE.md`, "libl.a, slibl.a, ncform" |
| `liby` | `/lib/liby.a`, `/lib/sliby.a` | ccv1, sccv1 | System III sources: `host/KNOWLEDGE.md`, "liby.a, sliby.a" |
| `libm` | `/lib/libm.a`, `/lib/slibm.a` | ccv1, sccv1; `as -f` | System III sources and `fputil.s`; open (compiler): `host/KNOWLEDGE.md`, "libm.a, slibm.a" |
| `oldlibc` | `libc-v1.a` .. `libc-v10.a` | several | the libc of the old programs: [`oldlibc/README.md`](oldlibc/README.md) |
| `oldslibc` | `slibc-v9.a`, `slibc-v11.a` | shipped cas, sccvc | the slibc of sld and cas: [`oldslibc/README.md`](oldslibc/README.md) |

The old compilers come from OLDROOT (`make -C host oldroot`); the libraries of the old programs are needed by stage 2 of `src/`.

## State

libc.a, crt0.o, slibc.a, scrt0.o, libl.a, slibl.a, ncform, liby.a, sliby.a: every member from source; libm.a, slibm.a: from source, fputil.o identical, the C members open (`compare/libm/findings.json`); every C member of libc.a and slibc.a by vl
(`host/OLDCC.md`, "vl: the compiler of libc.a and slibc.a"). The old libraries: see their README.

## Open

The plan: `host/increments/` (150 the other libraries and objects). vl keeps two switches with one witness
(`OLD_D53`, `OLD_D57`; owner's exception, `host/OLDCC.md` section 5).

## Measuring a member

- Bench: `host/recon/poptveto/s1114/cg/`. `cg.sh NAME "DEFINES"` (with `PPY=patch.py`: a trial patch of the codgen sources)
  builds codgen and scodgen with DEFINES (`-DOLD_VL` and the trial macro) into a copy of OLDROOT (`/tmp/SCRATCH/c_NAME`) as
  `/lib/codgen`, `/lib/scodgen`; for vl the scparse with `OLD_D25` (`rm -f ROOT/lib/scparse` first: in a copy it is a symbolic link
  to the original; then `cp OLDROOT/lib/vk/scparse`). `libtest.sh ROOT` compiles the 206 C members of libc (cc) and slibc (scc -DSEG)
  (with the `-I` of `src/head/oldv7` where the Makefile has it) and lists those whose object differs from the member of the
  original library (extracted to `/tmp/SCRATCH/orig/LIB`); all 206 are identical. After a reboot: `host/tools/wbin.sh /tmp/SCRATCH/wbin WEGA_ROOT OLDROOT EMU`.
- One member: compile in a scratch copy (a compile with `-S` in the source directory can overwrite a tracked `NAME.s`), then
  `host/tools/sdis.py` on both objects and `diff`. For `-I` the guest needs a relative directory below the working directory.
- A rule counts when the wanted member gets exact and no other member changes; try source forms first (by script: statement
  forms, the order of blocks and declarations), a new switch only when none fits.
