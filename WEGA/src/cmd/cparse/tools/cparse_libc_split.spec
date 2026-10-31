# libc part of /lib/cparse split into its modules (2026-10-31), the format of host/tools/unlink.py; made from the
# positions of the oldlibc versions (host/recon/oldlibc_match.py), the shipped members (libc_spec.py) and the data
# references of each module (HANDOFF_cparse.md, step 2). Old modules without a known version become new versions.
# The names _dev_errlist and _dev_nerr (the device error list of the old perror) are guesses.
noown
# NAME     KIND    START END  DATA  SYMBOLS
fltioc     old     9f06 a40c   22 dstart=41a4 __fltuse=9f06 __fegcvt=a236
fseek      old     a40c a548    0 _fseek=a40c
ftell      old     a548 a5fa    0 _ftell=a548
rdwr       old     a5fa a6e6    0 _fread=a5fa _fwrite=a66e
fopen      old     a6e6 a706    0 _fopen=a6e6
fdopen     old     a706 a790    0 _fdopen=a706
printf     old     a790 a8a0    0 _printf=a790 _fprintf=a7e0 _sprintf=a830
doprnt     old     a8a0 aec8   80 dstart=41ba __align=a8a0 __doprnt=a8f8 __atoip=ae50
ungetc     old     aec8 af18    0 _ungetc=aec8
atof       shipped af18 b088  180 dstart=420a _atofs=af18 _atof=af20
ecvt       old     b088 b26a   46 dstart=42be _ecvt=b088
pow10      shipped b26a b2de   96 dstart=42ec
filbuf     old     b2de b3b8   20 dstart=434c __filbuf=b2de
data       shipped -    -     162 dstart=4360
setbuf     old     b3b8 b40c    0 _setbuf=b3b8
flsbuf     old     b40c b5ea    0 __flsbuf=b40c _fflush=b4ee __cleanup=b54c _fclose=b56c
endopen    old     b5ea b75c    0 __endopen=b5ea
findiop    old     b75c b78a    0 __findiop=b75c
__callex   shipped b78a b790    0 __callex=b78a
cuexit     shipped b790 b79a    0 _exit=b790
perror     old     b79a b832   20 dstart=4402 _perror=b79a
cnvt       old     b832 b936    0 __cnvt=b832
decimal    shipped b936 b9c2    0 
length     old     b9c2 b9e0    0 __length=b9c2
malloc     old     b9e0 bbfa   12 dstart=4416 _malloc=b9e0 _free=bb3e _realloc=bb56
errlst     old     -    -     878 dstart=4422 _sys_errlist=4422 _sys_nerr=446e _dev_errlist=4470 _dev_nerr=4480
isatty     old     bbfa bc1c    0 _isatty=bbfa
stty       old     bc1c bc50    0 _stty=bc1c _gtty=bc36
strcat     old     bc50 bc78    0 _strcat=bc50
strcmp     old     bc78 bca6    0 _strcmp=bc78
strcpy     old     bca6 bcc4    0 _strcpy=bca6
strlen     old     bcc4 bcdc    0 _strlen=bcc4
ctype_     shipped -    -     258 dstart=4790
brk        shipped bcdc bd12    2 dstart=4892 _brk=bcdc _sbrk=bcee
chmod      shipped bd12 bd22    0 _chmod=bd12
close      shipped bd22 bd30    0 _close=bd22
creat      shipped bd30 bd3e    0 _creat=bd30
ioctl      shipped bd3e bd50    0 _ioctl=bd3e
lseek      shipped bd50 bd62    0 _lseek=bd50
open       shipped bd62 bd72    0 _open=bd62
read       shipped bd72 bd82    0 _read=bd72
signal     shipped bd82 be04    0 _signal=bd82
umask      shipped be04 be0c    0 _umask=be04
write      shipped be0c be1c    0 _write=be0c
cerror     shipped be1c be2c    4 dstart=4894 cerror=be1c
