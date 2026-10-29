# libc part of /bin/ld: one line per libc module in link order (2026-10-29), the format of unlink.py
# (src/cmd/size/size.spec is the model). Shipped members are members of /lib/libc.a; old modules are versions in
# src/lib/oldlibc (rindex is a new version, see HANDOFF_ld.md).
# DATA: only the data sizes that are checked are given (doscan, doprnt, dstart from the data references).
#       malloc 0_2: data 12 at 09a6 (as in strip; the references of the code start there).
#       doprnt 124 (as in cc and strip, the 20 bytes after it are filbuf at 08e4), filbuf 20 at 08e4.
#       Open, not yet placed: filbuf 0_2 (20 as in strip), cnvt 0_1 (12 as in cc, size),
#       mktemp 0_1 (2 as in strip), shipped brk (2) and cerror (4) (their data sections in libc.a).
# NAME     KIND    START END  DATA  SYMBOLS
scanf      shipped 2c4c 2d28  0
doscan     old     2d28 3342  160 dstart=07c8   _doscan=2d28 _innum=2f1e _instr=31ba _getccl=32ea
printf     old     3342 34d2  0     _printf=3342 _fprintf=33b4 _sprintf=3424 comm:_rc=705a:2
doprnt     old     34d2 3e72  124 dstart=0868  __align=34d2 __doprnt=352e __atoip=3df8
ffltioc    shipped 3e72 3e78  0
ungetc     shipped 3e78 3ece  0
filbuf     old     3ece 3fb0  20 dstart=08e4  __filbuf=3ece
flsbuf     old     3fb0 4192  0     __flsbuf=3fb0 _fflush=4096 __cleanup=40f4 _fclose=4114
__callex    shipped 4192 4198  0
cuexit     shipped 4198 41a2  0
cnvt       old     41a2 4296  0     __cnvt=41a2
decimal    shipped 4296 4322  0
length     old     4322 4340  0     __length=4322
malloc     old     4340 455a  12 dstart=09a6  _malloc=4340 _free=449c
isatty     shipped 455a 4578  0
mktemp     old     4578 45f2  2 dstart=09b2  _mktemp=4578
stty       shipped 45f2 461e  0
swab       shipped 461e 465c  0
rindex     old     465c 467c  0     _rindex=465c
tolower    shipped 467c 469e  0
strcat     old     469e 46ce  0     _strcat=469e
strncmp    shipped 46ce 4714  0
strcpy     old     4714 473a  0     _strcpy=4714
strncpy    old     473a 4786  0     _strncpy=473a
swap       old     4786 4828  0     _swap=4786 _longswap=47ac
access     shipped 4828 4838  0
brk        shipped 4838 486e  0
chmod      shipped 486e 487e  0
close      shipped 487e 488c  0
creat      shipped 488c 489a  0
fstat      shipped 489a 48aa  0
getpid     shipped 48aa 48b0  0
ioctl      shipped 48b0 48c2  0
link       shipped 48c2 48d2  0
lseek      shipped 48d2 48e4  0
open       shipped 48e4 48f4  0
read       shipped 48f4 4904  0
signal     shipped 4904 4986  0
umask      shipped 4986 498e  0
unlink     shipped 498e 499c  0
write      shipped 499c 49ac  0
cerror     shipped 49ac 49bc  0
sym __iob+15=0907 __lastbuf=0998 __sobuf=705e __filbuf=3ece __flsbuf=3fb0 __cnvt=41a2 __length=4322 _strcpy=4714 _ungetc=3e78 _tolower=467c __inflt=3e76 __fegcvt=3e74 __fltuse=3e72 __pfile=705c __iob+8=0900 __iob=08f8
noown
