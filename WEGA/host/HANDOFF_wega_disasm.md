# Handoff: WEGA build chain back to C

Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file: the inventory and the decisions of the whole project. The state of a program is in its `tools/findings.json`. Its readable (generated) form is `tools/FINDINGS.md`. The state of the old compilers is in `OLDCC.md`, `OLDCC_SWITCHES.md` and everything we learn in form of a logbook in `OLDCC_LOG.md`. A section on the current work is written here only when a handoff is due.

## Goal

1. restore all sources of the whole WEGA installation

2. create a tool chain which runs on Linux for crosscompiling

## Inventory: what belongs to the tool chain

| Tool                      | Original (WEGA 3.1 tree)                         | WEGA source                                     | WEGA status                           | Host source                                      | Host status               |
| ------------------------- | ------------------------------------------------ | ----------------------------------------------- | ------------------------------------- | ------------------------------------------------ | ------------------------- |
| `cc`, `scc`               | `/bin/cc`, `/bin/scc`                            | `src/cmd/cc`                                    | exact                                 | `host/cc`                                        | needs rechecking          |
| `cpp`                     | `/lib/cpp`                                       | `src/cmd/cpp`                                   | exact                                 | -                                                | missing                   |
| `cparse`, `scparse`       | `/lib/cparse`, `/lib/scparse`                    | `src/cmd/cparse`                                | exact                                 | -                                                | missing                   |
| `codgen`, `scodgen`       | `/lib/codgen`, `/lib/scodgen`                    | `src/cmd/codgen`                                | exact                                 | `host/codgen` (gcc `-Wall`; `-DSEG` for scodgen) | needs rechecking          |
| `gopt`                    | `/lib/gopt` (only with `-Ol`/`-Or`)              | -                                               | **missing**                           | -                                                | **missing**               |
| `popt`, `spopt`           | `/lib/popt`, `/lib/spopt`                        | `src/cmd/popt`                                  | exact                                 | `host/popt`                                      | needs rechecking          |
| `cas`                     | `/bin/cas`                                       | `src/cmd/cas`                                   | **see its FINDINGS.md**               | `host/cas`                                       | needs rechecking          |
| `as`                      | `/bin/as` (PLZ/ASM)                              | -                                               | **missing**                           | -                                                | **missing**               |
| `ld`                      | `/bin/ld`                                        | `src/cmd/ld`                                    | exact                                 | `host/ld/ld`                                     | needs rechecking          |
| `sld`                     | `/bin/sld`                                       | `src/cmd/sld`                                   | exact                                 | `host/ld/sld`                                    | needs rechecking          |
| `gld`                     | `/bin/gld`                                       | -                                               | **missing**                           | `host/ld/gld`                                    | needs rechecking          |
| `ar`                      | `/bin/ar`                                        | `src/cmd/ar.c`                                  | exact                                 | `host/ar/ar`                                     | needs rechecking          |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | `src/cmd/ranlib.c` `src/cmd/strip.c` `src/cmd/size.c` | exact                                 | `host/binutils`                                  | needs rechecking          |
| `libc`, `slibc`           | `/lib/libc.a`, `/lib/slibc.a`                    | `src/lib/libc`                                  | **libc: see TODO.md; slibc: missing** | `host/libc`                                      | `make -C host check` (libc); to be dropped with step 8 of TODO.md |
| `crt0` (start-up files)   | `/lib/crt0.o`, `scrt0.o`, `mcrt0.o`, `smcrt0.o` | `src/lib/libc/csu` (`crt0.o`), `src/lib/libc/crt` (the start-up files of the programs) | **crt0.o and those of the programs: exact; scrt0, mcrt0, smcrt0: missing** | - | not needed |
| `yacc`                    | `/usr/bin/yacc` (used by the build of cas)      | -                                               | **missing**                           | -                                                | **missing**               |

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

## Decided: equality criteria per code base

- `src/cmd` (K&R C, WEGA `cc`): byte identity with the original is the top priority. Original defects are rebuilt first and recorded in the quirks table in `KNOWLEDGE.md`; a fix needs the owner's decision. Second priority to keep the sources as equal to its SYSIII counterparts as possible (Copy&Paste is allowed).
- `host/` (ANSI C, gcc): functional equality only. Fixing defects of the original is allowed where the original's result is undefined or wrong; the fix is documented in the quirks table and in `src/TODO.md`. Functional equality with the original is the top goal here and has to be assured by corpus runs.

## Port rules: `src/cmd/X` to `host/X`

How a host version is made from the exact WEGA source (TODO.md item 7). Goal: whoever repeats the port reaches the state of `host/` without trial and error. The list holds rules, not code lines; **every time a check or corpus fails and the cause is a difference between Z8000/K&R and host/ANSI, the rule that would have prevented it is added here (what to replace, what to look out for, how it shows, how to find it).** A rule has: *Rule*, *Why*, *Find*.

Steps of a port:

1. Copy the files of `src/cmd/X` to `host/X` under the same names (programs of one file: `ar.c` to `host/ar/ar.c`). Keep names, order, comments; `diff src/cmd/X host/X` must show only the port.
2. Apply the rules below in the order of the sections; compile with `gcc -std=gnu99 -O2 -Wall -Wextra` (no `-w`, no disabled warning), then once with `-fsanitize=undefined,address -ftrapv`.
3. Run the checks of the program against the original under `z8k-emu` (`host/*/check.sh`, `check.py`, `host/tools/corpus.sh`); every difference: find the cause, fix it by a rule, add the rule here.
4. Record what the host version does differently on purpose (a fixed defect of the original) in the quirks table of `KNOWLEDGE.md`; the host version has no other deviations.

Rules (extend while porting):

*Types and sizes*

- Rule: everything that is a data format (header of an object file, archive, symbol table, relocation, a record of a file written to disk or a pipe) uses fixed width types: WEGA `int`/`short`/`unsigned` = `int16_t`/`uint16_t`, `long` = `int32_t`/`uint32_t`. Why: host `int` is 32 bit, `long` 64 bit. Find: `sizeof(struct ...)` that is used for I/O, `long` in a struct, `%ld` with a WEGA long.
- Rule: files are read and written field by field, big endian (helpers `get16`, `get32`, `put16`, `put32` in one header of the program), never a struct by `fread`/`fwrite`, never a cast of a buffer to a struct. Why: Z8000 is big endian, host little endian, the structs have padding. Find: `fread(&`, `fwrite(&`, `read(fd, &`, casts of `char *` to a struct pointer.
- Rule: a pointer or an address stored in an `int` or `long` becomes `intptr_t`/`uintptr_t`, or better an index or an offset. Why: 64 bit pointers. Find: `(int) ptr`, `(long) ptr`, warnings "cast from pointer to integer of different size".
- Rule: `char` is signed on the Z8000; declare `signed char` where the sign matters (arithmetic, comparison, indexing), and build with `-fsigned-char`. Why: `char` is unsigned on ARM hosts. Find: `char` used as a number, as an array index or compared with a negative value or `EOF`.
- Rule: plain computation with `int` stays `int` at first. Only where the sanitizer run or a corpus shows that a 16 bit overflow, a sign extension of a `short`/`char`, a `>>` on a negative value or a shift beyond 16 bit changes the result, that place gets `int16_t`/`uint16_t` (or an explicit cast `(int16_t)`); each place is noted below as a rule of its kind, not as a line.
- Rule: `printf`/`scanf` formats: `%d` with an `int16_t`/`int32_t` needs the right conversion or a cast to `int`/`long`; WEGA `%ld` for a 32 bit value gets `(long)` on the argument. Find: `-Wformat`.

*K&R to ANSI*

- Rule: prototypes for every function and every function used before its definition; old style definitions are converted (`host/tools/kr_code_transform.py` does the plain cases, the rest by hand); functions without a type are `int`; `main` returns `int`. Implicit `int` that holds a pointer (a function that returns a pointer, not declared) must be declared with its real type: on the host this is the usual silent crash. Find: `-Wimplicit-function-declaration`, `-Wint-conversion`.
- Rule: system headers instead of own declarations: `<stdio.h> <stdlib.h> <string.h> <unistd.h> <fcntl.h> <sys/stat.h>`; the own `stdio.h` of the programs (cparse, cpp, popt) is not used on the host. Names that clash with the host libc (`index`, `rindex`, `swab`, `getline`, `abs`...) are renamed with a prefix, in the program only.
- Rule: `register`, `extern` without a type, `char *` returned from `malloc` without a cast: remove or cast as the compiler demands; do not change logic to quiet a warning.

*Behaviour that the host libc does differently*

- Rule: the order of evaluation of arguments and of operands that have side effects is not defined, and the WEGA cc has its own order (right to left for arguments). Where the original depends on it (`f(*p++, *p++)`, `a[i] = i++`), the statement is split in the order of the original. Find: `-Wsequence-point`, `-Wunsequenced`; or a difference that appears with another gcc `-O`.
- Rule: `sprintf`/`printf` of floating point, `atof`, `ecvt`: the WEGA libc has its own routines, the output may differ in the last digit; take the routine of `src/lib/libc` over if a program prints floating point.
- Rule: `strcpy`/`memcpy` on overlapping memory, reading before the start of an array, use of uninitialised variables, writing a string literal: the original may work by accident on the Z8000; make the host version initialise and copy as the original's result shows, and note the place as a bug of the original in the quirks table.
- Rule: no `unsigned` counter that is compared with `-1`, no reliance on `int` overflow wrapping; where it occurs use `uint16_t` arithmetic with a mask as the original computes.

*Passes of the compiler (cpp, cparse, codgen, popt)*

- Rule: the node and symbol structures that stay inside the program are not written to disk and need no fixed width except where a field is used as a 16 bit value in arithmetic (constants, offsets, `lval`): there the field is `int32_t` for a WEGA `long` and `int16_t` for a WEGA `int`. Find: constant folding differs from the original for large constants; the check shows it in the corpus.
- Rule: the yacc of cparse: the WEGA yacc and the host yacc (bison `-y`/byacc) make different tables; the token numbers come from `%term NAME n` and stay. Inputs with a syntax error are part of the corpus because the error recovery may differ. Find: a difference only in messages or after the first error.
- Rule: the SEG variants (`scparse`, `scodgen`, `spopt`, `sld`, `scc`) are built from the same files with `-DSEG`/`-DSCC`; the pointer size of the Z8001 (32 bit) is a data property of the compiled program, not of the host pointer, therefore no `sizeof(char *)` for it: use the macros of the program (`SZPOINT` or the like).

*Build*

- Rule: every host program has a `Makefile` (`make`, `make check WEGA=...`, `make clean`), is listed in `host/Makefile` (`all`, `check`) and in `host/tools/checkall.sh`; objects and binaries of the build are not in git.
- Rule: host programs read their settings (`cc.conf`, `ld.conf`) as before; a program does not contain the path of the owner's machine.

Open: the rules above are the state before the first port; they are confirmed or corrected with the pilot (`ar`) and extended with every failure.

## Next steps

A more detailed plan (steps) is in `../src/TODO.md`
