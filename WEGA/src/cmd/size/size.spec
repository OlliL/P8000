# libc part of /bin/size: one line per libc module in link order
# NAME     KIND    START END  DATA  SYMBOLS
rdwr       old     0224 031e    0   _fread=0224 _fwrite=02a0
fopen      shipped 031e 033c    0
printf     old     033c 04ec    0   _printf=033c _fprintf=03be _sprintf=0444
doprnt     old     04ec 0b4e   94   __align=04ec __doprnt=054c __atoip=0ace
ffltioc    shipped 0b4e 0b54    0
filbuf     old     0b54 0c3a   20   __filbuf=0b54
flsbuf     old     0c3a 0e1c    0   __flsbuf=0c3a _fflush=0d20 __cleanup=0d7e _fclose=0d9e
endopen    old     0e1c 0fd0   12   __endopen=0e1c
findiop    old     0fd0 0ffe    0   __findiop=0fd0
data       shipped -    -     162   comm:__sobuf=281e:512 comm:__sibuf=2a1e:512
__callex   shipped 0ffe 1004    0
cuexit     shipped 1004 100e    0
cnvt       old     100e 1102   12   __cnvt=100e
decimal    shipped 1102 118e    0
length     old     118e 11ac    0   __length=118e
malloc     old     11ac 13ca   12   _malloc=11ac _free=130c
isatty     shipped 13ca 13e8    0
stty       shipped 13e8 1414    0
swab       shipped 1414 1452    0
goodmagic  old     1452 14a4   20   _goodmagic=1452 _magics=1800 comm:_swap_flg=281c:2
swap       old     14a4 1546    0   _swap=14a4
brk        shipped 1546 157c    2
chmod      shipped 157c 158c    0
close      shipped 158c 159a    0
creat      shipped 159a 15a8    0
ioctl      shipped 15a8 15ba    0
lseek      shipped 15ba 15cc    0
open       shipped 15cc 15dc    0
read       shipped 15dc 15ec    0
umask      shipped 15ec 15f4    0
write      shipped 15f4 1604    0
cerror     shipped 1604 1614    4
