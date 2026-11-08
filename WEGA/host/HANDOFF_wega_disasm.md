# Handoff: WEGA build chain back to C

Entry point. Read `../CLAUDE.md` first (rules, how to build, where to look things up), then this
file: the inventory and the decisions of the whole project. The state of a program is in its `compare/NAME/findings.json`. Its readable (generated) form is `compare/NAME/FINDINGS.md`. The state of the old compilers is in `OLDCC.md`, `OLDCC_SWITCHES.md` and everything we learn in form of a logbook in `OLDCC_LOG.md`. A section on the current work is written here only when a handoff is due.

## Goal

1. restore all sources of the whole WEGA installation

2. create a tool chain which runs on Linux for crosscompiling

## Inventory: what belongs to the tool chain

| Tool                      | Original (WEGA 3.1 tree)                         | WEGA source                                     | WEGA status                           | Host source                                      | Host status               |
| ------------------------- | ------------------------------------------------ | ----------------------------------------------- | ------------------------------------- | ------------------------------------------------ | ------------------------- |
| `cc`, `scc`               | `/bin/cc`, `/bin/scc`                            | `src/cmd/cc`                                    | exact                                 | `host/cc`                                        | exact                     |
| `cpp`                     | `/lib/cpp`                                       | `src/cmd/cpp`                                   | exact                                 | `host/cpp`                                       | exact                     |
| `cparse`, `scparse`       | `/lib/cparse`, `/lib/scparse`                    | `src/cmd/cparse`                                | exact                                 | -                                                | missing                   |
| `codgen`, `scodgen`       | `/lib/codgen`, `/lib/scodgen`                    | `src/cmd/codgen`                                | exact                                 | `host/codgen` (gcc `-Wall`; `-DSEG` for scodgen) | needs rechecking          |
| `gopt`                    | `/lib/gopt` (only with `-Ol`/`-Or`)              | -                                               | **missing**                           | -                                                | **missing**               |
| `popt`, `spopt`           | `/lib/popt`, `/lib/spopt`                        | `src/cmd/popt`                                  | exact                                 | `host/popt`                                      | needs rechecking          |
| `cas`                     | `/bin/cas`                                       | `src/cmd/cas`                                   | **see its FINDINGS.md**               | `host/cas`                                       | needs rechecking          |
| `as`                      | `/bin/as` (PLZ/ASM)                              | -                                               | **missing**                           | -                                                | **missing**               |
| `ld`                      | `/bin/ld`                                        | `src/cmd/ld`                                    | exact                                 | `host/ld/ld`                                     | needs rechecking          |
| `sld`                     | `/bin/sld`                                       | `src/cmd/sld`                                   | exact                                 | `host/ld/sld`                                    | needs rechecking          |
| `gld`                     | `/bin/gld`                                       | -                                               | **missing**                           | `host/ld/gld`                                    | needs rechecking          |
| `ar`                      | `/bin/ar`                                        | `src/cmd/ar.c`                                  | exact                                 | `host/ar/ar`                                     | exact                     |
| `ranlib`, `strip`, `size` | `/usr/bin/ranlib`, `/usr/bin/strip`, `/bin/size` | `src/cmd/ranlib.c` `src/cmd/strip.c` `src/cmd/size.c` | exact                                 | `host/binutils`                                  | exact                     |
| `libc`, `slibc`           | `/lib/libc.a`, `/lib/slibc.a`                    | `src/lib/libc`                                  | **libc: see TODO.md; slibc: missing** | `host/libc`                                      | `make -C host check` (libc); to be dropped with step 8 of TODO.md |
| `crt0` (start-up files)   | `/lib/crt0.o`, `scrt0.o`, `mcrt0.o`, `smcrt0.o` | `src/lib/libc/csu` (`crt0.o`), `src/lib/libc/crt` (the start-up files of the programs) | **crt0.o and those of the programs: exact; scrt0, mcrt0, smcrt0: missing** | - | not needed |
| `yacc`                    | `/usr/bin/yacc` (used by the build of cas)      | -                                               | **missing**                           | -                                                | **missing**               |

Host status: **exact** = the host program is made from the WEGA source by the port rules (below) and the corpus of its host candidate (`compare/NAME`, "Check of a program") is identical with the original in `host/tools/checkall.sh`; **needs rechecking** = the host source is an older reconstruction that is not taken over from `src/cmd` yet.

Not a tool of the chain but the oracle: `host/z8k-emu` (runs the original binaries). Analysis tools: `host/tools`.

## Decided: equality criteria per code base

- `src/cmd` (K&R C, WEGA `cc`): byte identity with the original is the top priority. Original defects are rebuilt first and recorded in the quirks table in `KNOWLEDGE.md`; a fix needs the owner's decision. Second priority to keep the sources as equal to its SYSIII counterparts as possible (Copy&Paste is allowed).
- `host/` (ANSI C, gcc): functional equality only. Fixing defects of the original is allowed where the original's result is undefined or wrong; the fix is documented in the quirks table and in `src/TODO.md`. Functional equality with the original is the top goal here and has to be assured by corpus runs.

## Check of a program: one mechanism, the corpus

Decided (owner, 2026-11-08): every program is checked by the corpus runner (`host/tools/corpus.sh`, `host/recon/stagecorpus.py`), not by an own `check.sh`, and there is no `check` target in a Makefile. The original (under `z8k-emu`) runs on the cases once; every candidate is compared with it: output files, messages, exit status.

- `WEGA/compare/NAME/` holds, for every program, what belongs to it besides its source (`src/cmd` has only sources and Makefiles): `progcfg.py` (the corpus), `findings.json`, `FINDINGS.md` (generated), `SYSIII.md`, test data. Programs of one file (ar, ranlib, strip, size) and those without a `src/cmd` directory have one too.
- `CANDIDATES` in `progcfg.py`: a dict, `wega` (the program built from `src/cmd`, runs under the emulator) and/or `host` (the host build, runs natively), each the path of the binary relative to `WEGA/`. A program without a host counterpart (dc, perhaps others in future) has only `wega`. The corpus is the same for all candidates by construction (one preparation of the inputs, one run of the original).
- Call: `host/tools/corpus.sh [--wega | --host] [PROG ...]`. Without a switch every candidate is tested, without PROG every program with a corpus; `--wega` / `--host` test only that candidate (with the switch and no PROG: the programs that have it). The binaries are those made by `make` (`CORPUS_BIN_WEGA`, `CORPUS_BIN_HOST` name another file); `checkall` runs it in its copy of the tree, where `CANDIDATES` points to the programs it built there.
- Two shapes of a case: a *run* (an input file, `CORPUS_PREPARE`, `CORPUS_RUN`, `CORPUS_OUTPUTS`; cpp, cparse, codgen, popt, cas, ld) and a *script* (`CORPUS_SCRIPT`: a shell script with many commands and `PROG` as the program, for tools that are driven by commands: ar; its output, exit status and the files left behind are compared; variants are environments such as `TZ`). `CORPUS_VARIANTS` runs every case once per variant.
- A host build writes host paths (the include directory) or program names: `CORPUS_HOST_ENV`, `CORPUS_NORMALIZE`, and in script mode the files of the case are copied with their directories and a candidate is run under its own name (`.prog/NAME`).
- Converting a `check.sh` or `check.py`: the new cases must find everything the old script finds. Prove it with deliberately broken builds (change an output format, a comparison, a constant in the host source, run the old and the new check): the new check finds at least the same ones; then remove the old script and the `check` target. Done for `ar`, ranlib, strip, size, `cc`, `scc` (every mutant of the host source that was tried is found; the old `binutils/check.sh` expanded its globs in the wrong directory and tested nothing of the many-file cases). Still to convert: `host/cas/check.sh`, `host/popt/check.sh`, `host/ld/check.py`, `host/libc/check.sh`.

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
- Rule: `register i;` (implicit `int`) becomes `int i;`, other `register` is dropped; `static` for every function and variable that is not used by another file; an own name that is a C library or `<unistd.h>` name (`select`) gets a trailing underscore (`select_`). Find: compile errors "undeclared", `-Wshadow`/link errors.
- Rule: a parameter that the body does not use (signal handlers) gets `(void)sig;` as first statement; an intentional `switch` fall through gets the comment `/* fall through */`; a string that only exists for `what(1)` (`sccsid`) gets `__attribute__((unused))`. Why: `-Wextra`. Find: `-Wunused-parameter`, `-Wimplicit-fallthrough`, `-Wunused-variable`.
- Rule: `mktemp` and `creat`+`open` for a temporary file become `mkstemp` (template with six `X`, a writable array, not a string literal); the name is not visible to the user. Find: `mktemp`.
- Rule: a `time_t` or `st_size` stored in a 32 bit field gets an explicit `(int32_t)`; `ctime` of a WEGA time uses `wega_ctime` (`host/tools/lib/wctime.c`, TZ as on WEGA, default EST5EDT). Find: `ctime`, `localtime`, `time_t`.
- Rule: the result of `fread`/`write`/`system` that the original ignores: `got = fread(...); (void)got;` (a `(void)` cast alone does not silence glibc's `warn_unused_result`; never `-Wno-unused-result`). Find: `-Wunused-result`.
- Rule: `strncpy(dst, src, n)` that cuts a longer string on purpose (archive names): `memcpy(dst, src, strnlen(src, n))` with `dst` zeroed before. Find: `-Wstringop-truncation`.
- Rule: a loop `for(i = 0; i <= n - 1; ...)` with a WEGA `unsigned n` of 16 bit: `n - 1` wraps at 0 on the Z8000 (65536 passes); the host loop is `i < n`, and a table that the original overruns on the Z8000 gets room for the 16 bit maximum (`s_segt >> 4`: 4095 entries). Record both in the quirks table. Find: arrays of 256 filled from a 16 bit count.
- Rule: a function of the original whose result depends on uninitialised variables or on the byte order inside a word (a `char` read into the low or high byte of an `int` by `fread(&c, 1, 1, f)`) is not ported: the host program refuses that case with a message (record it in the quirks table), unless a file exists in the system that needs it. Find: `fread(&` or `fwrite(&` with a size smaller than the variable.
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

- Rule: every host program has a `Makefile` (`make`, `make clean`), is listed in `host/Makefile` (`all`), has `CANDIDATES` with `host` in `compare/NAME/progcfg.py` and is run by `host/tools/checkall.sh`; objects and binaries of the build are not in git.
- Rule: host programs read their settings (`cc.conf`, `ld.conf`) as before; a program does not contain the path of the owner's machine.

- Rule: a program with its **own `stdio.h`** (cpp, cparse, popt: `char *_ptr`, V7 forms of `getc`/`putc`, the macros `getc`, `putc`, `_filbuf`, `_flsbuf`) works only with that header on the Z8000. The host version uses the system `<stdio.h>`: the program must not use the macros of the own header or members of `FILE` (`_cnt`, `_ptr`, `_base`, `_flag`, `_file`); every use of those becomes `getc`/`putc`/`fgetc`/`fputc`/`ungetc` of the system, and anything that peeks into `FILE` (counting characters, testing EOF by `_flag`) is replaced by `feof`/`ferror`/counters of the program. Also `EOF` handling: the system `getc` returns `int` -1, the own `getc` returned the byte masked with `0377`; `char c = getc(f)` and comparisons with `EOF` are checked. Why: the system header is mandatory on the host. Find: `grep -n '_ptr\|_cnt\|_base\|_flag\|_filbuf\|_flsbuf\|stdio.h'` in the source; the own `stdio.h` is not copied to `host/`.
- Rule (cpp, as the first program with an own `stdio.h`): the system `<stdio.h>` has `BUFSIZ` 8192, the own one 512. Every use of `BUFSIZ` that is a size in the program (buffers, tests for "too much") becomes an own constant (`BUFSZ 512`); the output buffer of `setbuf(f, buf)` becomes `setvbuf(f, buf, _IOFBF, BUFSZ)`. Why: a program that tests `savch > sbf + SBSIZE - BUFSIZ` fails at once with 8192. Find: `BUFSIZ` in the source; the symptom is "too much" or overflow messages on small inputs.
- Rule: `FILE *fout = stdout;` as an initialiser is not constant with glibc: assign in `main`. `fclose(stdout)` followed by use of the other file is fine.
- Rule: compile time macros that the original was built with and that change behaviour (`unix`, `z8000`, `COFF 128` for a signed `char`) are fixed at the top of the host source (`#undef unix`, `#define unix 1`, `#define z8000 1`); gcc predefines `unix`, `linux`, no `z8000`. Without them the program predefines other symbols (the symbol table of cpp: `unix`, `z8000`) and indexes its tables wrongly. Build with `-fsigned-char`. Find: `#if pdp11 | vax | z8000`, `#ifdef unix`.
- Rule: a function that the original calls with fewer or more arguments than it declares (`pperror(s, x, y)`, `VARARGS`) becomes `f(char *s, ...)` with `va_list`; a function that passes its arguments on (`ppwarn`) needs a `v` version (`vpperror`) that takes the `va_list`: never pass only the first argument on. Find: the message shows a stray value (`\t20 redefined` instead of `NSIG redefined`).
- Rule: a function that is used from another file of the program (`skipbl`, `lookup`, `pperror` used by `yylex.c`) is not `static`; the other file gets prototypes (in `cpy.y` the prologue `%{ ... %}`).
- Rule: yacc dialect of the original (`%term`, `%binary`, `={ action }`) to the host yacc (`bison -y`): `%token`, `%nonassoc`, `{ action }`; the value type of the stack (`YYSTYPE`) set to the WEGA `int` (`int16_t`); `bison` writes `cpy.c`, which is generated and in `.gitignore`.
- Rule: arithmetic in the program's own expression evaluator (cpp `#if`) is 16 bit; the places that C leaves undefined get the result of the Z8000 (division by 0 and `-32768 / -1`, shift counts that are negative, 16 or more, or that need more than a byte): helper functions named after the width (`div16`, `mod16`, `shift16`). The facts come from the original under `z8k-emu` (write the expressions as input, compare the result): never from a guess. Numbers that are read with `n = n*b + t` use `unsigned` (wrap around, no signed overflow).
- Rule: the corpus of a program whose output holds absolute paths of the host (the include directory): the host program reads the system include directory from an environment variable (`CPP_INCLUDE`, default `/usr/include`); the corpus runs it natively (`host/tools/corpus.sh --host BINARY PROG`, which sets `CORPUS_HOST=1`; `CORPUS_HOST_ENV(root)` and `CORPUS_NORMALIZE` in `compare/NAME/progcfg.py` map the path back to `/usr/include`, in the output and in the messages).
- Rule (cc, a driver): `signal()` returns a pointer on the host: `intign = signal(SIGINT, SIG_IGN) == SIG_IGN` (the original masks the int with `& 01`); a handler is `void f(int)` and `sigfn` is `void (*)(int)`; `(sigfn)0` is SIG_DFL. `alloc((argc + 20) * 2)` for an array of pointers: `sizeof (char *)`. `execl(..., 0)`: `(char *)0`. Find: `-Wint-conversion`, `-Wformat=` (sentinel).
- Rule: a message function called with different numbers of arguments (`error1(s, x)`, `fatal3(s, x, y)`) is `f(char *s, ...)` with `vsnprintf` into a buffer that is larger than the original's. A function of the original that has no `return` but is declared `int`: `void`; one that falls off its end where the caller uses the value (`alloc`): add the `return` of what the original has in the return register; record it in the quirks table. Find: `-Wreturn-type`.
- Rule: a variable the original never sets but reads (a `register` that holds 0, `r` of `cutsuf`) is initialised to what the Z8000 shows (proved by a check against the original); a NULL pointer that the original reads (address 0 holds a zero byte) is tested in the function that reads it. The results of `freopen`, `fread` that the original ignores: `fp = freopen(...); (void)fp;`.
- Rule: the host-only additions of a ported program (cc: `cc.conf` next to the executable, a pass given as a command with arguments, `tmpdir`, `mv`, `libc` as variables, `passexists`, `ldexec`) stay in one marked place of the source and in the header comment; the rest of the file is the original's text.

State: `cc` and `scc` are done too (`host/cc`; `check.sh` 74 of 74 for each, also with the sanitizers); `cpp` is done too (`host/cpp`; corpus 288 of 288 identical, also with `-fsanitize=undefined,address -ftrapv`, plus 400 expressions of `#if` and 35 error cases against the original); `ar`, `ranlib`, `strip` and `size` are done (`host/ar`, `host/binutils`; `make check` identical, `-Wall -Wextra` clean, also checked with `-fsanitize=undefined,address -ftrapv`); `ar` is the pilot (`host/ar`, checked: 79 commands for five time zones, `tv` of libc.a and slibc.a identical; `-Wall -Wextra` clean; the check also run with `-fsanitize=undefined,address -ftrapv` without a finding); the rules above are confirmed by it except those for the compiler passes, which are still the state before the first port. Extend with every failure.

## Next steps

A more detailed plan (steps) is in `../src/TODO.md`
