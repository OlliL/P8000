# bc against System III and Plexus

`/bin/bc` of WEGA against `cmd/bc.y` of System III (built by `:mkcmd`: `yacc bc.y`, `y.tab.c` -> `bc.c`, cc; `bc.x` is the
yacc output that System III ships). Plexus `cmd/bc.y` has the same text. The yacc of WEGA gives the tables of `bc.x` (only the
number format differs). Measured 2026-11-23.

## WEGA's own

- The SCCS line `@[$]bc.y	1.2 07/26/82 17:03:37 - 87wega3.2` (the first data of the binary).
- `bundle(a)` of System III walks its arguments with `p = &a`, which needs them on the stack. On the Z8000 the first six
  arguments come in registers, so WEGA's bundle is `bundle(a, a1, a2, a3, a4, a5, args)`: it copies a1..a5 into
  `int regs[5]` and calls `xbundle(a, regs, &args)` (name guessed), which switches to `args` at the sixth element
  (`if (a - i == 6) p = args;`), the form of `printf` in the libc.

## Forms that follow the bytes, not the System III text

- The bss order: a block of the commons at the head of bc.y (`post`, `ifile`, `yyval`, `yyv`, `cary`, `string`, `pre`, `ss`,
  `sargc`, `sargv`, `yylval`, `b_space`, `ln`, `in`), the order of the original; the declarations of System III stay where
  they are. With the declarations alone ccv0 allocates them in source order.
