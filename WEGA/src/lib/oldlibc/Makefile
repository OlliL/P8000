# oldlibc: the libraries libc-vN.a of the time of the old programs, in ..
#
# Each original program was linked from a libc of its time. Their modules are here in the versions the originals hold:
# NAME/0_n, from C with the compiler of the rule (a source of ../libc or NAME/...) or, where a compiler or a source is still
# missing, from NAME/0_n.s; the other members are those of ../libc. The order of a library is the order of the members in
# the original programs, ld takes what is referenced. Version numbers of the modules are invented.
#
#   libc-v1.a  dc, ranlib, chmod, pwd, kill, mkdir, sleep, tail, env, nohup, gets, bc (07/82)
#   libc-v2.a  cpp (12/82), clist
#   libc-v3.a  size, od, lex (01/83; lex: atoi 0_3, calloc 0_2)
#   libc-v4.a  strip (01/83), su (02/83; the members of passwd too), nice, time (01/83), ed (01/83: fakcu, strncpy 0_2,
#              chown, dup, pipe, execlp (in front of execvp) added, mktemp in front of setret)
#   libc-v5.a  codgen, scodgen, popt, spopt
#   libc-v6.a  rm, mv, cp, ln (05/83 .. 07/83), chgrp, chown, sort, touch (02/83 .. 03/83), uname, test (01/83), tr (05/83), dog (03/83)
#              ls (02/83: getgrgid 0_1, getpwuid 0_1, rdwr 0_2, calloc 0_2, qsort 0_1, strncat 0_1 added; atoi behind getenv),
#              df (04/83: nlist 0_1, strncmp, sync added),
#              more (03/83: freopen before fputs, fseek 0_1, ungetc, setret, dup added; ctime before getenv, getenv before perror, sleep before ttyname, creat before execl, _exit before fcntl, fstat behind fork: orders no user forced, measured with host/recon/libcplace.py; owner 2026-11-26)
#   libc-v7.a  cparse, scparse, find (10/83)
#   libc-v8.a  cc, scc
#   libc-v9.a  ld (07/83), login (07/83: the members of getpass, getpwnam, fopen, execvp, crypt, ttyname, a64l etc.
#              added; data 0_1 of ld fits login too); sh (08/83: atol 0_1, setret, dup, execve, fcntl, fork, geteuid,
#              pipe, profil, times, wait and fakcu behind atol added), csh (08/83: perror 0_2, errlst 0_2,
#              abort, dup2, _exit, getgid, getuid added; owner 2026-11-26)
#   libc-v10.a write, rmdir (01/83)
#   libc-v11.a mkstr (11/82), cat (10/82), echo (08/82), ex (10/82; its members as in libc-v1, but length 0_3)
#   libc-v12.a chkin (09/83): the shipped libc.a (../libc) with printf and doprnt 0_4 in their places
#   libc-v13.a dd (12/82): libc-v10 with open 0_1 (two arguments)
#   libc-v14.a local (02/84): the shipped libc.a (../libc) with _NFILE 40 and data, filbuf, doscan, printf, doprnt, malloc of 1984
#   libc-v15.a stty (12/83): the shipped libc.a (../libc) with doprnt 0_10 (base and val set to 0 first)
#
#   make        (needs ../libc made, and ccv1, ccv0, ccv0p)
#   make clean

CC = cc
CCV1 = ccv1
CCV1Q = ccv1q
CCV1R = ccv1r
CCV0 = ccv0
CCV0P = ccv0p
CAS = cas
AR = ar

LIBS = ../libc-v1.a ../libc-v2.a ../libc-v3.a ../libc-v4.a ../libc-v5.a ../libc-v6.a ../libc-v7.a ../libc-v8.a ../libc-v9.a ../libc-v10.a ../libc-v11.a ../libc-v12.a ../libc-v13.a ../libc-v14.a ../libc-v15.a

all: $(LIBS)

LIBv1 = freopen/0_1.o getpwuid/0_2.o getpwent/0_2.o fgets/0_2.o fseek/0_2.o ftell/0_1.o rew/0_1.o readsym/0_1.o rdwr/0_3.o system/0_1.o fopen/0_1.o fdopen/0_2.o fputs/0_1.o printf/0_3.o doprnt/0_3.o \
	 ../libc/build/nonseg/ffltioc.o ungetc/0_1.o gets/0_2.o filbuf/0_3.o setbuf/0_1.o flsbuf/0_2.o endopen/0_3.o findiop/0_2.o \
	 ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o ../libc/build/nonseg/abort.o ctime/0_2.o \
	 abs/0_1.o execvp/0_1.o getenv/0_1.o atoi/0_1.o sleep/0_1.o cnvt/0_2.o ../libc/build/nonseg/decimal.o length/0_2.o malloc/0_3.o errlst/0_2.o isatty/0_1.o \
	 ../libc/build/nonseg/setret.o stty/0_1.o swab/0_1.o toupper/0_1.o index/0_1.o strcat/0_2.o strcmp/0_1.o strchr/0_1.o strcpy/0_2.o strncmp/0_1.o strncpy/0_1.o strlen/0_2.o \
	 goodmagic/0_2.o swap/0_2.o ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/access.o ../libc/build/nonseg/alarm.o \
	 ../libc/build/nonseg/brk.o ../libc/build/nonseg/chdir.o ../libc/build/nonseg/chmod.o ../libc/build/nonseg/chown.o \
	 ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o \
	 ../libc/build/nonseg/execv.o ../libc/build/nonseg/exit.o ../libc/build/nonseg/fork.o ../libc/build/nonseg/fstat.o ../libc/build/nonseg/getpid.o \
	 ../libc/build/nonseg/getgid.o ../libc/build/nonseg/getuid.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/kill.o \
	 ../libc/build/nonseg/link.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/mknod.o open/0_1.o \
	 ../libc/build/nonseg/pause.o ../libc/build/nonseg/pipe.o ../libc/build/nonseg/read.o ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o \
	 ../libc/build/nonseg/time.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o \
	 ../libc/build/nonseg/cerror.o
# ccv0 ccv0p
../libc-v1.a: objs
	sh mklib.sh $@ $(AR) $(LIBv1)

LIBv2 = fputs/0_1.o fopen/0_1.o printf/0_5.o doprnt/0_7.o ffltioc/0_1.o filbuf/0_3.o setbuf/0_1.o data/0_1.o flsbuf/0_2.o \
	 endopen/0_3.o findiop/0_2.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o atoi/0_1.o cnvt/0_2.o ../libc/build/nonseg/decimal.o \
	 length/0_3.o malloc/0_3.o isatty/0_2.o stty/0_3.o strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strncpy/0_1.o \
	 ../libc/build/nonseg/brk.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o \
	 open/0_1.o ../libc/build/nonseg/read.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv0p
../libc-v2.a: objs
	sh mklib.sh $@ $(AR) $(LIBv2)

LIBv3 = ../libc/build/nonseg/freopen.o fseek/0_3.o rdwr/0_1.o ../libc/build/nonseg/fopen.o printf/0_1.o doprnt/0_1.o ../libc/build/nonseg/ffltioc.o filbuf/0_1.o flsbuf/0_1.o \
	 endopen/0_1.o findiop/0_1.o ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o atoi/0_3.o cnvt/0_1.o \
	 ../libc/build/nonseg/decimal.o length/0_1.o calloc/0_2.o malloc/0_1.o ../libc/build/nonseg/isatty.o ../libc/build/nonseg/stty.o ../libc/build/nonseg/strcmp.o \
	 ../libc/build/nonseg/swab.o goodmagic/0_1.o swap/0_1.o ../libc/build/nonseg/brk.o ../libc/build/nonseg/chmod.o ../libc/build/nonseg/close.o \
	 ../libc/build/nonseg/creat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/open.o ../libc/build/nonseg/read.o \
	 ../libc/build/nonseg/umask.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv1 ccv1q
../libc-v3.a: objs
	sh mklib.sh $@ $(AR) $(LIBv3)

LIBv4 = fseek/0_1.o cuserid/0_1.o rdwr/0_2.o getpass/0_1.o putpwent/0_1.o getpwnam/0_1.o getpwuid/0_1.o getpwent/0_1.o \
	 fgets/0_1.o ../libc/build/nonseg/rew.o ../libc/build/nonseg/fopen.o fputs/0_2.o printf/0_2.o doprnt/0_2.o \
	 ../libc/build/nonseg/ffltioc.o filbuf/0_2.o ../libc/build/nonseg/setbuf.o flsbuf/0_1.o endopen/0_2.o findiop/0_1.o \
	 ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o ../libc/build/nonseg/execlp.o execvp/0_2.o ctime/0_1.o getenv/0_2.o sleep/0_3.o ../libc/build/nonseg/fakcu.o \
	 getlogin/0_1.o perror/0_2.o ttyslot/0_1.o ttyname/0_2.o atoi/0_2.o cnvt/0_1.o ../libc/build/nonseg/decimal.o \
	 length/0_1.o crypt/0_1.o malloc/0_2.o errlst/0_2.o ../libc/build/nonseg/isatty.o mktemp/0_1.o ../libc/build/nonseg/setret.o \
	 ../libc/build/nonseg/stty.o ../libc/build/nonseg/index.o ../libc/build/nonseg/swab.o strcat/0_1.o ../libc/build/nonseg/strcmp.o \
	 ../libc/build/nonseg/strncmp.o strcpy/0_1.o strncpy/0_2.o goodmagic/0_1.o swap/0_1.o strlen/0_1.o rindex/0_1.o a64l/0_1.o l64a/0_1.o \
	 ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/access.o ../libc/build/nonseg/alarm.o ../libc/build/nonseg/brk.o ../libc/build/nonseg/chdir.o \
	 ../libc/build/nonseg/chmod.o ../libc/build/nonseg/chown.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o \
	 ../libc/build/nonseg/execv.o ../libc/build/nonseg/fork.o ../libc/build/nonseg/fstat.o ../libc/build/nonseg/getpid.o ../libc/build/nonseg/getuid.o \
	 ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lkdata.o ../libc/build/nonseg/link.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/nice.o \
	 ../libc/build/nonseg/open.o ../libc/build/nonseg/pause.o ../libc/build/nonseg/pipe.o ../libc/build/nonseg/read.o ../libc/build/nonseg/setgid.o ../libc/build/nonseg/setuid.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/time.o times/0_1.o ../libc/build/nonseg/umask.o \
	 ../libc/build/nonseg/unlink.o ../libc/build/nonseg/unlk.o ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv1
../libc-v4.a: objs
	sh mklib.sh $@ $(AR) $(LIBv4)

LIBv5 = freopen/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o printf/0_5.o doprnt/0_5.o \
	 ../libc/build/nonseg/ffltioc.o getw/0_1.o ungetc/0_1.o filbuf/0_3.o setbuf/0_1.o flsbuf/0_2.o endopen/0_4.o \
	 findiop/0_2.o ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o perror/0_1.o atoi/0_1.o \
	 cnvt/0_2.o ../libc/build/nonseg/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o stty/0_3.o \
	 ../libc/build/nonseg/ctype_.o strrchr/0_1.o strcat/0_2.o strncmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/build/nonseg/brk.o \
	 ../libc/build/nonseg/chmod.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o \
	 ../libc/build/nonseg/open.o ../libc/build/nonseg/read.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv0p
../libc-v5.a: objs
	sh mklib.sh $@ $(AR) $(LIBv5)

LIBv6 = system/0_2.o getgrnam/0_1.o getgrgid/0_1.o getgrent/0_1.o getpwuid/0_1.o getpwnam/0_1.o getpwent/0_1.o fgets/0_1.o ../libc/build/nonseg/rew.o rdwr/0_2.o \
	 getopt/0_1.o fputc/0_1.o ../libc/build/nonseg/freopen.o fputs/0_2.o fseek/0_1.o ../libc/build/nonseg/fopen.o printf/0_6.o doprnt/0_6.o \
	 ../libc/build/nonseg/ffltioc.o gets/0_1.o ../libc/build/nonseg/ungetc.o filbuf/0_2.o ../libc/build/nonseg/setbuf.o flsbuf/0_1.o endopen/0_2.o findiop/0_1.o \
	 ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o ctime/0_1.o getenv/0_2.o perror/0_2.o \
	 ../libc/build/nonseg/sleep.o ttyname/0_2.o atoi/0_2.o cnvt/0_1.o ../libc/build/nonseg/decimal.o \
	 length/0_1.o calloc/0_2.o malloc/0_2.o errlst/0_2.o ../libc/build/nonseg/fakcu.o ../libc/build/nonseg/isatty.o ../libc/build/nonseg/setret.o nlist/0_1.o qsort/0_1.o ../libc/build/nonseg/stty.o strcat/0_1.o strncat/0_1.o strrchr/0_2.o \
	 ../libc/build/nonseg/strcmp.o ../libc/build/nonseg/strncmp.o strcpy/0_1.o strncpy/0_2.o strlen/0_1.o ../libc/build/nonseg/access.o \
	 ../libc/build/nonseg/alarm.o ../libc/build/nonseg/strchr.o ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/brk.o \
	 ../libc/build/nonseg/chmod.o ../libc/build/nonseg/chown.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o \
	 ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/exit.o ../libc/build/nonseg/fcntl.o ../libc/build/nonseg/getuid.o ../libc/build/nonseg/fork.o ../libc/build/nonseg/fstat.o \
	 ../libc/build/nonseg/ftime.o ../libc/build/nonseg/getpid.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/link.o \
	 ../libc/build/nonseg/lseek.o ../libc/build/nonseg/open.o ../libc/build/nonseg/pause.o ../libc/build/nonseg/read.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/setuid.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/sync.o ../libc/build/nonseg/stime.o \
	 ../libc/build/nonseg/time.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/uname.o ../libc/build/nonseg/unlink.o \
	 ../libc/build/nonseg/wait.o ../libc/build/nonseg/utime.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv1
../libc-v6.a: objs
	sh mklib.sh $@ $(AR) $(LIBv6)

LIBv7 = popen/0_1.o fgets/0_2.o fputs/0_1.o fltioc/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o fdopen/0_1.o \
	 printf/0_5.o doprnt/0_5.o ../libc/build/nonseg/ffltioc.o ungetc/0_1.o ../libc/build/nonseg/atof.o ecvt/0_1.o \
	 ../libc/build/nonseg/pow10.o puts/0_1.o filbuf/0_3.o data/0_1.o setbuf/0_1.o flsbuf/0_2.o endopen/0_4.o findiop/0_2.o \
	 ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o perror/0_1.o execvp/0_1.o getenv/0_1.o sleep/0_2.o \
	 atoi/0_1.o cnvt/0_2.o ../libc/build/nonseg/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o \
	 ../libc/build/nonseg/setret.o stty/0_3.o index/0_1.o strcat/0_2.o strcmp/0_1.o strncmp/0_1.o strcpy/0_2.o strlen/0_2.o \
	 strrchr/0_1.o ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/alarm.o ../libc/build/nonseg/brk.o \
	 ../libc/build/nonseg/chdir.o ../libc/build/nonseg/chmod.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o \
	 ../libc/build/nonseg/dup2.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/execv.o ../libc/build/nonseg/exit.o \
	 ../libc/build/nonseg/fork.o ../libc/build/nonseg/fstat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o \
	 ../libc/build/nonseg/open.o ../libc/build/nonseg/pause.o ../libc/build/nonseg/pipe.o ../libc/build/nonseg/read.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/time.o ../libc/build/nonseg/umask.o \
	 ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv0p
../libc-v7.a: objs
	sh mklib.sh $@ $(AR) $(LIBv7)

LIBv8 = ../libc/build/nonseg/freopen.o ../libc/build/nonseg/fopen.o printf/0_4.o doprnt/0_4.o ../libc/build/nonseg/ffltioc.o \
	 ../libc/build/nonseg/flsbuf.o ../libc/build/nonseg/endopen.o ../libc/build/nonseg/findiop.o ../libc/build/nonseg/data.o \
	 ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o ../libc/build/nonseg/abort.o cnvt/0_1.o ../libc/build/nonseg/decimal.o \
	 length/0_1.o malloc/0_4.o ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/isatty.o ../libc/build/nonseg/stty.o strcat/0_1.o \
	 ../libc/build/nonseg/strcmp.o strcpy/0_1.o strlen/0_1.o ../libc/build/nonseg/access.o ../libc/build/nonseg/brk.o ../libc/build/nonseg/chmod.o \
	 ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/execv.o ../libc/build/nonseg/fork.o \
	 ../libc/build/nonseg/getpid.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/open.o ../libc/build/nonseg/signal.o \
	 ../libc/build/nonseg/stat.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o \
	 ../libc/build/nonseg/cerror.o
# ccv1
../libc-v8.a: objs
	sh mklib.sh $@ $(AR) $(LIBv8)

LIBv9 = getpass/0_1.o getpwnam/0_1.o getpwent/0_1.o fgets/0_1.o fputs/0_2.o ../libc/build/nonseg/rew.o ../libc/build/nonseg/fopen.o \
	 ../libc/build/nonseg/scanf.o doscan/0_1.o printf/0_6.o doprnt/0_6.o ../libc/build/nonseg/ffltioc.o ../libc/build/nonseg/ungetc.o \
	 filbuf/0_2.o ../libc/build/nonseg/setbuf.o flsbuf/0_1.o endopen/0_2.o findiop/0_1.o data/0_1.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o \
	 ../libc/build/nonseg/execlp.o execvp/0_2.o getenv/0_2.o perror/0_2.o ../libc/build/nonseg/sleep.o ../libc/build/nonseg/abort.o ttyname/0_2.o atoi/0_2.o atol/0_1.o ../libc/build/nonseg/fakcu.o cnvt/0_1.o \
	 ../libc/build/nonseg/decimal.o length/0_1.o crypt/0_1.o calloc/0_2.o malloc/0_2.o ../libc/build/nonseg/isatty.o nlist/0_1.o mktemp/0_1.o ../libc/build/nonseg/setret.o ../libc/build/nonseg/ctype_.o \
	 ../libc/build/nonseg/stty.o ../libc/build/nonseg/index.o ../libc/build/nonseg/swab.o rindex/0_1.o ../libc/build/nonseg/tolower.o strcat/0_1.o \
	 ../libc/build/nonseg/strcmp.o ../libc/build/nonseg/strncmp.o strcpy/0_1.o strncpy/0_2.o strlen/0_1.o strrchr/0_2.o a64l/0_1.o errlst/0_2.o swap/0_1.o ../libc/build/nonseg/access.o \
	 ../libc/build/nonseg/alarm.o ../libc/build/nonseg/brk.o ../libc/build/nonseg/chdir.o ../libc/build/nonseg/chroot.o ../libc/build/nonseg/chmod.o ../libc/build/nonseg/chown.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/dup2.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/execv.o ../libc/build/nonseg/execve.o ../libc/build/nonseg/fcntl.o ../libc/build/nonseg/exit.o ../libc/build/nonseg/fork.o ../libc/build/nonseg/fstat.o ../libc/build/nonseg/getgid.o ../libc/build/nonseg/getpid.o ../libc/build/nonseg/geteuid.o ../libc/build/nonseg/getuid.o \
	 ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/link.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/nice.o ../libc/build/nonseg/open.o ../libc/build/nonseg/pause.o ../libc/build/nonseg/pipe.o ../libc/build/nonseg/profil.o ../libc/build/nonseg/read.o ../libc/build/nonseg/setgid.o ../libc/build/nonseg/setuid.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/sync.o ../libc/build/nonseg/time.o ../libc/build/nonseg/times.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/uname.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv0p ccv1
../libc-v9.a: objs
	sh mklib.sh $@ $(AR) $(LIBv9)

LIBv10 = rdwr/0_4.o fopen/0_2.o printf/0_7.o doprnt/0_8.o ../libc/build/nonseg/ffltioc.o filbuf/0_4.o flsbuf/0_3.o endopen/0_5.o \
	 findiop/0_3.o ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o perror/0_4.o ttyname/0_1.o cnvt/0_3.o \
	 ../libc/build/nonseg/decimal.o length/0_4.o malloc/0_6.o errlst/0_2.o isatty/0_3.o stty/0_4.o strcat/0_3.o strcmp/0_2.o \
	 strcpy/0_3.o strlen/0_3.o strrchr/0_3.o ../libc/build/nonseg/access.o ../libc/build/nonseg/alarm.o ../libc/build/nonseg/brk.o \
	 ../libc/build/nonseg/chmod.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/fork.o \
	 ../libc/build/nonseg/fstat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o ../libc/build/nonseg/open.o ../libc/build/nonseg/read.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o \
	 ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv1r
../libc-v10.a: objs
	sh mklib.sh $@ $(AR) $(LIBv10)

# dd (12/82): libc-v10 with the open of two arguments (open 0_1); write and rmdir have the one of three
LIBv13 = rdwr/0_4.o fopen/0_2.o printf/0_7.o doprnt/0_8.o ../libc/build/nonseg/ffltioc.o filbuf/0_4.o flsbuf/0_3.o endopen/0_5.o \
	 findiop/0_3.o ../libc/build/nonseg/data.o ../libc/build/nonseg/__callex.o ../libc/build/nonseg/cuexit.o perror/0_4.o ttyname/0_1.o cnvt/0_3.o \
	 ../libc/build/nonseg/decimal.o length/0_4.o malloc/0_6.o errlst/0_2.o isatty/0_3.o stty/0_4.o strcat/0_3.o strcmp/0_2.o \
	 strcpy/0_3.o strlen/0_3.o strrchr/0_3.o ../libc/build/nonseg/access.o ../libc/build/nonseg/alarm.o ../libc/build/nonseg/brk.o \
	 ../libc/build/nonseg/chmod.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/fork.o \
	 ../libc/build/nonseg/fstat.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o open/0_1.o ../libc/build/nonseg/read.o \
	 ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/umask.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o \
	 ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
../libc-v13.a: objs
	sh mklib.sh $@ $(AR) $(LIBv13)

LIBv11 = putchar/0_1.o freopen/0_1.o fseek/0_2.o ftell/0_1.o rew/0_1.o rdwr/0_3.o fopen/0_1.o printf/0_3.o doprnt/0_7.o ffltioc/0_1.o \
	 ungetc/0_1.o filbuf/0_3.o setbuf/0_1.o data/0_1.o flsbuf/0_2.o endopen/0_3.o findiop/0_2.o ../libc/build/nonseg/__callex.o getenv/0_1.o sleep/0_1.o ../libc/build/nonseg/cuexit.o \
	 perror/0_3.o cnvt/0_2.o ../libc/build/nonseg/decimal.o length/0_3.o calloc/0_1.o malloc/0_3.o errlst/0_1.o isatty/0_1.o ../libc/build/nonseg/setret.o stty/0_1.o \
	 swab/0_1.o toupper/0_1.o strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/build/nonseg/ctype_.o ../libc/build/nonseg/access.o ../libc/build/nonseg/alarm.o \
	 ../libc/build/nonseg/brk.o ../libc/build/nonseg/chdir.o ../libc/build/nonseg/close.o ../libc/build/nonseg/creat.o ../libc/build/nonseg/dup.o ../libc/build/nonseg/execl.o ../libc/build/nonseg/exit.o ../libc/build/nonseg/fork.o \
	 ../libc/build/nonseg/fstat.o ../libc/build/nonseg/getpid.o ../libc/build/nonseg/getuid.o ../libc/build/nonseg/ioctl.o ../libc/build/nonseg/lseek.o open/0_1.o ../libc/build/nonseg/pause.o ../libc/build/nonseg/pipe.o \
	 ../libc/build/nonseg/read.o ../libc/build/nonseg/signal.o ../libc/build/nonseg/stat.o ../libc/build/nonseg/time.o ../libc/build/nonseg/unlink.o ../libc/build/nonseg/wait.o ../libc/build/nonseg/write.o ../libc/build/nonseg/cerror.o
# ccv0 (putchar, perror, rew, calloc)
../libc-v11.a: objs
	sh mklib.sh $@ $(AR) $(LIBv11)

# the shipped library with two members replaced (ar r keeps their places)
../libc-v12.a: objs ../libc/build/nonseg/libc.a
	rm -rf v12.d ; mkdir v12.d ; cp printf/0_4.o v12.d/printf.o ; cp doprnt/0_4.o v12.d/doprnt.o
	rm -f $@ ; cp ../libc/build/nonseg/libc.a $@
	cd v12.d ; $(AR) r ../../libc-v12.a printf.o doprnt.o
	rm -rf v12.d

# local (02/84): the shipped library with six members replaced (the libc of ZEUS 3.21 has the same members; _NFILE 40)
../libc-v14.a: objs ../libc/build/nonseg/libc.a
	rm -rf v14.d ; mkdir v14.d ; cp data/0_2.o v14.d/data.o ; cp filbuf/0_5.o v14.d/filbuf.o ; cp doscan/0_2.o v14.d/doscan.o
	cp printf/0_8.o v14.d/printf.o ; cp doprnt/0_9.o v14.d/doprnt.o ; cp malloc/0_7.o v14.d/malloc.o
	rm -f $@ ; cp ../libc/build/nonseg/libc.a $@
	cd v14.d ; $(AR) r ../../libc-v14.a data.o filbuf.o doscan.o printf.o doprnt.o malloc.o
	rm -rf v14.d

# stty (12/83): the shipped library with doprnt 0_10
../libc-v15.a: objs ../libc/build/nonseg/libc.a
	rm -rf v15.d ; mkdir v15.d ; cp doprnt/0_10.o v15.d/doprnt.o
	rm -f $@ ; cp ../libc/build/nonseg/libc.a $@
	cd v15.d ; $(AR) r ../../libc-v15.a doprnt.o
	rm -rf v15.d

# the modules: each directory makes its objects with its own Makefile
objs:
	cd toupper ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd puts ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd popen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ttyslot ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd putpwent ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd l64a ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getpwuid ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getpass ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getlogin ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd cuserid ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd crypt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd a64l ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd abs ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd qsort ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strncat ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getgrgid ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd atoi ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd calloc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd cnvt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ctime ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd data ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd doprnt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd doscan ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ecvt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd endopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd errlst ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd times ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd nlist ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd execvp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fdopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ffltioc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd filbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd findiop ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd flsbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fltioc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fputc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fputs ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd freopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fseek ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ftell ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd fgets ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getenv ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd gets ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getw ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getgrent ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getgrnam ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getpwent ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getpwnam ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd getopt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd goodmagic ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd index ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd isatty ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd length ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd malloc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd atol ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd mktemp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd open ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd perror ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd printf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd rdwr ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd readsym ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd putchar ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd rew ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd rindex ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd setbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd sleep ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strcat ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strchr ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strcmp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strcpy ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strlen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strncmp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strncpy ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd strrchr ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd stty ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd swab ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd swap ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd system ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ungetc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	cd ttyname ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs

clean:
	rm -f */0_*.o */*.o $(LIBS)
	rm -rf */*.d

.PHONY: all clean objs
