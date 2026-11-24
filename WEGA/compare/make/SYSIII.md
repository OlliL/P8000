# make against System III and Plexus

`/bin/make` of WEGA against `usr/src/cmd/make` of System III (Plexus `cmd/make` has the same sources; System V and later
makes are further away: their messages for 5.0 archives, `Cannot touch`, `NOT REMOVED` are not in the binary). Built by the
shipped cc with `/lib/libc.a`, separate I and D (`-i`, as `make.mk`); the files in the order of `make.mk`, the rules file
`wegarules.c`. Measured 2026-11-24.

## WEGA's own

- The SCCS lines `@[$]main.c	2.1 11/18/83 13:33:40 - 87wega3.2` and alike in every file (`wegarules.c 2.2`), the ids of
  System III gone.
- `wegarules.c` instead of `rules.c` (the 155 strings of the binary): the suffixes `.c+ .y+ .f+ .p+ .s+ .sh+ .h+` of the
  chkout tree with `$(CHKOUT) $(CHKFLAGS) $<`, `.f` with `FC=f77`, `PLZ`, `PASCAL`, `ASFLAGS=-u`; no lex rules and no
  `markfile` rule.
- misc.c `get()`: a file of the chkout tree (`file+`, `is_chk`, `trychk`) is checked out first, by `$(CHKOUT) $(CHKFLAGS)`
  and `-v` for the release; the sccs file only else. The `IS_OFF(GET)` test after `access` is gone. The name kept for
  `rm_gots` is the file without its `+`.
- misc.c `exit()`: `_cleanup()` before `_exit`.
- main.c `intrupt()`: only a regular file is removed (`isreg`, a `stat` of the target).
- files.c: the s.out format of WEGA: `<s.out.h>`, `struct s_exec`, `goodmagic()` (libc) for the magic number, the skip
  `s_imsize` (doubled without `SF_STRIP`) plus `s_segt`. `lookarch` finds the entry point of an object archive by
  `symscan()` of the libc (`struct nlist`, `<nlist.h>`) and takes a defined external symbol only; `char s[128]`,
  `nc = LONGNAME` (127, the long names of s.out) instead of 8.
- dosys.c `doshell()`: `/bin/csh` as SHELL gets `-cf` / `-cfe` instead of `-c` / `-ce`.

## Forms that follow the bytes, not the System III text

- lookarch: `if(!eqstr(..)) continue;` and a goto loop around `symscan`, so that no back jump to the loop head is left
  (with one, popt turns the loop, `OLDCC.md` D7).
- get: `varptr(chk ? "CHKOUT" : "GET")->varval` inside the call of `copstr` (the frame temporary of the original).
- `int unused` in 14 functions and, by sed in the Makefile, in yyparse of `/usr/lib/yaccpar`: the unused frame words of the
  compiler of 11/83 (`host/OLDCC_REGSLOTS.md`).

## Not in the tree

`rules.c`, `pwbrules.c`, `make.mk`, `make.tm` and `markfile` of System III (replaced by `wegarules.c` and the Makefile).
