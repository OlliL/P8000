# WEGA: TODO

The plan and nothing else. What is known goes to `host/KNOWLEDGE.md`, the old compilers to `host/OLDCC.md`, the state per function to
`compare/NAME/FINDINGS.md` of a program. A finished step leaves this file. No new programs before step 9 (owner 2026-11-04).

What is byte-identical is checked by `make -C host checkall` (16 minutes): every file that `make install` in `src/` puts below an empty root is compared with the WEGA tree; a program joins by its install target (stripped by `install` when the original is). Run it after every change of a compiler and before a commit of a source of a program. Step 5 (the open programs in `cmd/`) is done; the owner decides on the next. Everything builds with `make` under `src/`: old compilers with the shipped tools and libc, `lib/`, old compilers again with the libc of their time, `cmd/` (7 minutes; `make -C host src` on Linux, `host/tools/wbin.sh` puts the tools under the emulator in the PATH).

## 2. `lib/` from C, what is left

libc.a, crt0.o, slibc.a, scrt0.o: every member from source (C by vl, `host/OLDCC.md`). Open:
- The switches of vl: each has one source as witness; check whether source forms with the existing switches replace them
  ([`lib/README.md`](lib/README.md)).
- slibc into the tree of libc (one tree with `#ifdef`; the differences: `host/KNOWLEDGE.md`, "slibc: its sources against those of libc").
- The other segmented files of `/lib` (`smcrt0.o`, `scnvt.o`, `sdoprnt.o`), `slibm.a` and the other `s*` libraries.

## 9. New programs

`gld` and everything else under `bin/`, `lib/`, `usr/bin`, ... `host/OLDCC_DATES.md` lists all programs with their time.

## 10. Last, when all other binaries stand

- `/bin/cas`: done, byte-identical (sccvc, `host/OLDCC.md` "The old scc of cas"; `compare/cas/HANDOFF.md`). Left there: the natural `for` loop of jpr.
- `/bin/as`, `/lib/as*`: PLZ/ASM. All of PLZ: `/usr/bin/plzsys`, `plz`, `/usr/lib/plzcg*`. To be learned from the literature of the owner first.
