# libc modules of /bin/ld that are not in oldlibc yet (2026-10-28, revision 3). Module = source file.
# doscan.c: _doscan 2d28, _innum 2f1e, _instr 31ba, _getccl 32ea -> 2d28..3342 (one module).
# printf.c: _printf 3342, _fprintf 33b4, _sprintf 3424 -> 3342..34d2 (names as in printf/0_4.s).
# doprnt.c: __align 34d2, __doprnt 352e, __atoip 3df8 (names as in size.spec) -> 34d2..3e70 (one module).
# __pfile (705c) and _rc (705a, comm 2 as in printf/0_4.s). The constant 0x0900 is __iob+8 (set by hand).
# Data: doscan 07c8..0868 (160 bytes, ends where doprnt starts), doprnt 0868..08f8 (144 bytes, __iob at 08f8). printf and
# f473a has no data here (not known).
# NAME     KIND  START END  DATA  SYMBOLS
doscan     old   2d28 3342  160 dstart=07c8     _doscan=2d28 _innum=2f1e _instr=31ba _getccl=32ea
printf     old   3342 34d2  0     _printf=3342 _fprintf=33b4 _sprintf=3424 comm:_rc=705a:2
doprnt     old   34d2 3e72  144 dstart=0868     __align=34d2 __doprnt=352e __atoip=3df8
ffltioc    shipped 3e72 3e78  0
strncpy   old   473a 4786  0     _strncpy=473a
sym __filbuf=3ece __flsbuf=3fb0 __cnvt=41a2 __length=4322 _strcpy=4714 _ungetc=3e78 _tolower=467c __inflt=3e76 __fltuse=3e72 __fegcvt=3e74 __pfile=705c __iob+8=0900 __iob=08f8
noown
