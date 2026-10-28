# libc part of /bin/cc: one line per libc module in link order
# NAME     KIND    START END  DATA  SYMBOLS
freopen    shipped 15f2 161a    0
fopen      shipped 161a 1638    0
printf     old     1638 17cc    0   _printf=1638 _fprintf=16aa _sprintf=171a comm:_rc=3bbc:2
doprnt     old     17cc 215a  124   __align=17cc __doprnt=182c __atoip=20e0 comm:__pfile=3bbe:2
ffltioc    shipped 215a 2160    0
flsbuf     shipped 2160 233c    0
endopen    shipped 233c 24bc   12
findiop    shipped 24bc 24ea    0
data       shipped -    -     162   comm:__sobuf=3bc0:512 comm:__sibuf=3dc0:512
__callex   shipped 24ea 24f0    0
cuexit     shipped 24f0 24fa    0
abort      shipped 24fa 2506    0
cnvt       old     2506 25fa   12   __cnvt=2506
decimal    shipped 25fa 2686    0
length     old     2686 26a4    0   __length=2686
malloc     old     26a4 292e   14   _malloc=26a4 _free=2834 _calloc=28d2
ctype_     shipped -    -     258
isatty     shipped 292e 294c    0
stty       shipped 294c 2978    0
strcat     old     2978 29a8    0   _strcat=2978
strcmp     shipped 29a8 29d6    0
strcpy     old     29d6 29fc    0   _strcpy=29d6
strlen     old     29fc 2a14    0   _strlen=29fc
access     shipped 2a14 2a24    0
brk        shipped 2a24 2a5a    2
chmod      shipped 2a5a 2a6a    0
close      shipped 2a6a 2a78    0
creat      shipped 2a78 2a86    0
execl      shipped 2a86 2aa6    0
execv      shipped 2aa6 2ab6    0
fork       shipped 2ab6 2aca    2
getpid     shipped 2aca 2ad0    0
ioctl      shipped 2ad0 2ae2    0
lseek      shipped 2ae2 2af4    0
open       shipped 2af4 2b04    0
signal     shipped 2b04 2b86    0
stat       shipped 2b86 2b96    0
umask      shipped 2b96 2b9e    0
unlink     shipped 2b9e 2bac    0
wait       shipped 2bac 2bbc    0
write      shipped 2bbc 2bcc    0
cerror     shipped 2bcc 2bdc    4
