# libc part of /lib/cpp: one line per libc module in link order, the format of unlink.py (src/cmd/ld/tools/ld.spec is the
# model). /lib/cpp is a combined program (code and data at 0x3308); dstart is the address.
noown
immmin 3308
# The link uses the known versions: cpp.link is edited behind unlink.py where it only wrote other labels (printf, setbuf,
# isatty, strcat, strcpy, flsbuf) or took the number 0x4000 of length for an address.
# ffltioc has four stubs here, the shipped member three; __fegcvt is what doprnt calls, the other names are guesses.
# NAME     KIND    START END  DATA  SYMBOLS
fopen      old     2272 2292  0     _fopen=2272
printf     old     2292 23a2  0     _printf=2292 _fprintf=22e2 _sprintf=2332
doprnt     old     23a2 29b8  80 dstart=3f5e __align=23a2 __doprnt=23fa __atoip=2942
ffltioc    old     29b8 29c0  0     __fltuse=29b8 __ffltx=29ba __fegcvt=29bc __inflt=29be
setbuf     old     29c0 2a14  0     _setbuf=29c0
data       shipped -    -     162 dstart=3fae
flsbuf     old     2a14 2bf2  0     __flsbuf=2a14 _fflush=2af6 __cleanup=2b54 _fclose=2b74
endopen    old     2bf2 2d3a  0     __endopen=2bf2
findiop    old     2d3a 2d68  0     __findiop=2d3a
__callex   shipped 2d68 2d6e  0
cuexit     shipped 2d6e 2d78  0
cnvt       old     2d78 2e7c  0     __cnvt=2d78
decimal    shipped 2e7c 2f08  0
length     old     2f08 2f26  0     __length=2f08
malloc     old     2f26 3140  12 dstart=4050 _malloc=2f26 _free=3084 _realloc=309c
isatty     old     3140 3162  0     _isatty=3140
stty       old     3162 3196  0     _stty=3162 _gtty=317c
strcat     old     3196 31be  0     _strcat=3196
strcmp     old     31be 31ec  0     _strcmp=31be
strcpy     old     31ec 320a  0     _strcpy=31ec
strncpy    old     320a 3254  0     _strncpy=320a
brk        shipped 3254 328a  2 dstart=405c
close      shipped 328a 3298  0
creat      shipped 3298 32a6  0
ioctl      shipped 32a6 32b8  0
lseek      shipped 32b8 32ca  0
open       old     32ca 32d8  0     _open=32ca
read       shipped 32d8 32e8  0
write      shipped 32e8 32f8  0
cerror     shipped 32f8 3306  4 dstart=405e
