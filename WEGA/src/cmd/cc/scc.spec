# libc part of /bin/scc: one line per libc module in link order
# NAME     KIND    START END  DATA  SYMBOLS
freopen    shipped 15fe 1626    0   
fopen      shipped 1626 1644    0   
printf     old     1644 17d8    0   _printf=1644 _fprintf=16b6 _sprintf=1726 comm:_rc=3be2:2
doprnt     old     17d8 2164  124   __align=17d8 __doprnt=1838 __atoip=20ec comm:__pfile=3be4:2
ffltioc    old     2164 216c    0   __fltuse=2164 __fegcvt=2166 __inflt=2168
flsbuf     shipped 216c 2348    0   
endopen    shipped 2348 24c8   12   
findiop    shipped 24c8 24f6    0   
data       shipped -    -     162   comm:__sobuf=3be6:512 comm:__sibuf=3de6:512
__callex   shipped 24f6 24fc    0   
cuexit     shipped 24fc 2506    0   
abort      shipped 2506 2512    0   
cnvt       old     2512 2606   12   __cnvt=2512
decimal    shipped 2606 2692    0   
length     old     2692 26b0    0   __length=2692
malloc     old     26b0 293a   14   _malloc=26b0 _free=2840 _calloc=28de
ctype_     shipped -    -     258   
isatty     shipped 293a 2958    0   
stty       shipped 2958 2984    0   
strcat     old     2984 29b4    0   _strcat=2984
strcmp     shipped 29b4 29e2    0   
strcpy     old     29e2 2a08    0   _strcpy=29e2
strlen     old     2a08 2a20    0   _strlen=2a08
access     shipped 2a20 2a30    0   
brk        shipped 2a30 2a66    2   
chmod      shipped 2a66 2a76    0   
close      shipped 2a76 2a84    0   
creat      shipped 2a84 2a92    0   
execl      shipped 2a92 2ab2    0   
execv      shipped 2ab2 2ac2    0   
fork       shipped 2ac2 2ad6    2   
getpid     shipped 2ad6 2adc    0   
ioctl      shipped 2adc 2aee    0   
lseek      shipped 2aee 2b00    0   
open       shipped 2b00 2b10    0   
signal     shipped 2b10 2b92    0   
stat       shipped 2b92 2ba2    0   
umask      shipped 2ba2 2baa    0   
unlink     shipped 2baa 2bb8    0   
wait       shipped 2bb8 2bc8    0   
write      shipped 2bc8 2bd8    0   
cerror     shipped 2bd8 2be8    4   
