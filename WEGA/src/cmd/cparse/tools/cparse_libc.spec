# libc part of /lib/cparse cut as ONE module (2026-10-30, provisional): text 9f06..bf00, data 41a4..4898.
# The split into the single members (shipped members, oldlibc versions, new versions for the gaps) is open, see
# HANDOFF_cparse.md, step 2. Names: oldlibc and libc.a symbols at their offsets (tools of step 2).
noown
# NAME   KIND START END  DATA SYMBOLS
cplibc   old  9f06 bf00 1780 dstart=41a4 _fseek=a40c _ftell=a548 _fread=a5fa _fwrite=a66e _fopen=a6e6 _fdopen=a706 _printf=a790 _fprintf=a7e0 _sprintf=a830 _ungetc=aec8 _atofs=af18 _atof=af20 __filbuf=b2de _setbuf=b3b8 __flsbuf=b40c _fflush=b4ee __cleanup=b54c _fclose=b56c __callex=b78a _exit=b790 _perror=b79a _malloc=b9e0 _free=bb3e _realloc=bb56 _isatty=bbfa _gtty=bc1c _strcat=bc50 _strcmp=bc78 _strcpy=bca6 _strlen=bcc4 _brk=bcdc _sbrk=bcee _chmod=bd12 _close=bd22 _creat=bd30 _ioctl=bd3e _lseek=bd50 _open=bd62 _read=bd72 _signal=bd82 _umask=be04 _write=be0c cerror=be1c __iob=4360 __ctype=4790 comm:__sobuf=b4f4:512 comm:__sibuf=b6f4:512 comm:_sigtab=b8f4:40
