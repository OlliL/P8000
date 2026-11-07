# cpp against System III and Plexus

The preprocessor `/lib/cpp` of WEGA (SCCS cpp.c 1.5, cpy.y 1.2, yylex.c 1.1) against `cmd/cc/vax/cpp` of System III (`cmd/cc/pdp11/cpp.c` is the same text) and `cmd/cc/z8000/cpp` of Plexus. cpp was written after System III from the start. Measured 2026-11-07.

## Lines

Lines without comments and blanks that also stand in the file.

| File | System III file | Equal | Plexus file | Equal | Of |
|---|---|---:|---|---:|---:|
| cpp.c | cc/vax/cpp/cpp.c | 922 | cc/z8000/cpp/cpp.c | 924 | 986 |
| cpy.y | cc/vax/cpp/cpy.y | 78 | cc/z8000/cpp/cpy.y | 78 | 79 |
| yylex.c | cc/vax/cpp/yylex.c | 81 | cc/z8000/cpp/yylex.c | 82 | 83 |

## WEGA's own

- The data: System III declares its variables where they are used, WEGA's has them in one block in the order of the original (`/* the common data, in the order of the original */`); the order of the commons is the order of the bytes.
- WEGA's own: `sayflag`, `bflag` (`-B`), an escaped newline in a quotation, the escaped line feed in the actuals of a macro, the `/* line */` comment of a `#line`, `z8000` as a predefined name; System III has `WARN` and `speakup` for the line numbers of a macro call with line feeds, WEGA does not.
- Not changed because the bytes change: `DROP` as `'\376'` (System III) in place of `0376`.

## Forms that follow the bytes, not the System III text

- `top:` in `slookup` and `subst`: a label no jump names, in front of the first use of a register variable; it forgets the register contents and so makes the bytes (`findings.json`).
