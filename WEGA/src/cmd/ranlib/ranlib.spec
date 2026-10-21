# libc part of /usr/bin/ranlib: one line per libc module in link order
# the own code of ranlib.o differs from the original, so the symbol addresses are given here
noown
sym _environ=1e0c
# NAME     KIND    START END  DATA  SYMBOLS
fseek      old     053a 0676    0   _fseek=053a
ftell      old     0676 0724    0   _ftell=0676
readsym    old     0724 079c    0   _readsym=0724
rdwr       old     079c 0888    0   _fread=079c _fwrite=0810
system     old     0888 0946   30   _system=0888 _csystem=0896 __system=08a4
fopen      old     0946 0966    0   _fopen=0946
printf     old     0966 0a72    0   _printf=0966 _fprintf=09b2 _sprintf=0a02
doprnt     old     0a72 101a   80   __align=0a72 __doprnt=0aca __atoip=0fa4
ffltioc    old     101a 1022    0   __fltuse=101a __fegcvt=101c __inflt=101e
filbuf     old     1022 10fc   20   __filbuf=1022
flsbuf     old     10fc 12da    0   __flsbuf=10fc _fflush=11de __cleanup=123c _fclose=125c
endopen    old     12da 1422    0   __endopen=12da
findiop    old     1422 1450    0   __findiop=1422
data       shipped -    -     162   comm:__sobuf=3f3c:512 comm:__sibuf=413c:512
__callex   shipped 1450 1456    0
cuexit     shipped 1456 1460    0
cnvt       old     1460 1564    0   __cnvt=1460
decimal    shipped 1564 15f0    0
length     old     15f0 160e    0   __length=15f0
malloc     old     160e 1828   12   _malloc=160e _free=176c
isatty     old     1828 1846    0   _isatty=1828
stty       old     1846 1872    0   _stty=1846 _gtty=185c
swab       old     1872 18b0    0   _swab=1872
strncmp    old     18b0 18f6    0   _strncmp=18b0
strncpy    old     18f6 1940    0   _strncpy=18f6
goodmagic  old     1940 1994   20   _goodmagic=1940 _magics=1df0 comm:_swap_flg=3ee6:2
swap       old     1994 1a3a    0   _swap=1994 _longswap=19bc
brk        shipped 1a3a 1a70    2
close      shipped 1a70 1a7e    0
creat      shipped 1a7e 1a8c    0
execl      shipped 1a8c 1aac    0
exit       shipped 1aac 1ab2    0
fork       shipped 1ab2 1ac6    2
ioctl      shipped 1ac6 1ad8    0
lseek      shipped 1ad8 1aea    0
open       old     1aea 1af8    0   _open=1aea
read       shipped 1af8 1b08    0
signal     shipped 1b08 1b8a    0
time       shipped 1b8a 1b96    0
unlink     shipped 1b96 1ba4    0
wait       shipped 1ba4 1bb4    0
write      shipped 1bb4 1bc4    0
cerror     shipped 1bc4 1bd4    4
