# libc part of /usr/bin/strip: one line per libc module in link order
# NAME     KIND    START END  DATA  SYMBOLS
fseek      old     068c 07ca    0   _fseek=068c
rdwr       old     07ca 08c2    0   _fread=07ca _fwrite=0844
fopen      shipped 08c2 08e0    0
printf     old     08e0 0a82    0   _printf=08e0 _fprintf=095e _sprintf=09e0
doprnt     old     0a82 13d0  124   __align=0a82 __doprnt=0ade __atoip=1356
ffltioc    shipped 13d0 13d6    0
filbuf     old     13d6 14b8   20   __filbuf=13d6
flsbuf     old     14b8 169a    0   __flsbuf=14b8 _fflush=159e __cleanup=15fc _fclose=161c
endopen    old     169a 181e   12   __endopen=169a
findiop    old     181e 184c    0   __findiop=181e
data       shipped -    -     162   comm:__sobuf=336e:512 comm:__sibuf=356e:512
__callex   shipped 184c 1852    0
cuexit     shipped 1852 185c    0
cnvt       old     185c 1950   12   __cnvt=185c
decimal    shipped 1950 19dc    0
length     old     19dc 19fa    0   __length=19dc
malloc     old     19fa 1c14   12   _malloc=19fa _free=1b56
isatty     shipped 1c14 1c32    0
mktemp     old     1c32 1cac    2   _mktemp=1c32
stty       shipped 1cac 1cd8    0
swab       shipped 1cd8 1d16    0
strcmp     shipped 1d16 1d44    0
strcpy     old     1d44 1d6a    0   _strcpy=1d44
goodmagic  old     1d6a 1dbc   20   _goodmagic=1d6a _magics=232e comm:_swap_flg=336c:2
swap       old     1dbc 1e5e    0   _swap=1dbc _longswap=1de2
access     shipped 1e5e 1e6e    0
brk        shipped 1e6e 1ea4    2
chmod      shipped 1ea4 1eb4    0
close      shipped 1eb4 1ec2    0
creat      shipped 1ec2 1ed0    0
getpid     shipped 1ed0 1ed6    0
ioctl      shipped 1ed6 1ee8    0
lseek      shipped 1ee8 1efa    0
open       shipped 1efa 1f0a    0
read       shipped 1f0a 1f1a    0
signal     shipped 1f1a 1f9c    0
umask      shipped 1f9c 1fa4    0
unlink     shipped 1fa4 1fb2    0
write      shipped 1fb2 1fc2    0
cerror     shipped 1fc2 1fd2    4
