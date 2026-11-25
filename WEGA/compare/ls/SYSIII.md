# ls against System III and Plexus

`/bin/ls` of WEGA is not the ls of System III (`cmd/ls.c`: options -asdglrtucifo, no columns) but the Berkeley ls
(columns, -C -F -R -m -x), in the form of Plexus' `cmd/plx/bls.c` ("Modified by Paul Fronberg June 1982 (SYS III)").
Measured 2026-11-25.

## Lines

Lines without comments and blanks that also stand in the file.

| File | Plexus file | Equal | System III file | Equal | Of |
|---|---|---:|---|---:|---:|
| ls.c | plx/bls.c | 655 | ls.c | 351 | 867 |

(`tmp/tmp/ls_2bsd.c`, the 2BSD ls of 1979: 494.)

## WEGA's own

- The SCCS line `ls.c 1.7 02/23/83`; the usage line names ls and `-1ACDFRacdfgilmnqrstux` (no -b; -D, -g, -o new; -o is
  in the switch but not in the usage line).
- -D: gstat drops every entry that is no directory (`Dflg`); readdir stats each entry also for -D.
- -g leaves out the owner, -o the group (and makes a long listing by itself); -l sets gflg (the group column).
- The names by `getpwuid` and `getgrgid` (owner `%-9.9s`/`%-9d`, group `%-8.8s`/`%-8d`); `getname` and the hash of
  bls are gone, but main still opens `/etc/passwd` (or `/etc/group`) into `pwdf`, which nothing reads.
- -R: the subdirectories are collected in pentry (appended to `dfirst` with `cdfirst` as the tail) and listed by main
  after the arguments; bls collects them in pdirectory.
- -s: the blocks of every file (no case for devices); nblock is the plain `(size+511)>>9` of 2BSD.
- pdirectory keeps the tab width block that bls has under `#ifdef notdef`; both it and pem round to 8 and 16 at <= 8 and
  <= 16 (bls: <= 9, <= 17). The name of the program: a name ending in l, x or f selects -m, -x or -F on a terminal.
- -F: a blank after a plain file also in the stream format (bls: not with -m).
- Its own printf (`lsprintf`, name guessed; `#define printf lsprintf`) and `_doprnt`: integers and strings, the output
  through pputchar (so outcol counts everything), no floating point. The printf member of libc stays in the link for
  fprintf and calls this `_doprnt` with six arguments where it takes four (KNOWLEDGE.md, quirks).

## Forms that follow the bytes

- `_doprnt`, the l conversions: `if ((c = *fmt++) == 'o') goto lo; if (c == 'x') goto lx; if (c == 'd') case 'D':
  base = 10;` with the blocks of O and X in front (the original compares the value three times in a row).
- pputchar: `if (qflg) if (c < ' ' || c >= 0177) c = '?';` (bls tests the character first).
