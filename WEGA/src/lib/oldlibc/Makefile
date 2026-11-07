# oldlibc: the libraries libc-vN.a of the time of the old programs, in ..
#
# Each original program was linked from a libc of its time. Their modules are here in the versions the originals hold:
# NAME/0_n, from C with the compiler of the rule (a source of ../libc or NAME/...) or, where a compiler or a source is still
# missing, from NAME/0_n.s; the other members are those of ../libc. The order of a library is the order of the members in
# the original programs, ld takes what is referenced. Version numbers of the modules are invented.
#
#   libc-v1.a  dc, ranlib (07/82)
#   libc-v2.a  cpp (12/82)
#   libc-v3.a  size (01/83)
#   libc-v4.a  strip (01/83)
#   libc-v5.a  codgen, scodgen, popt, spopt
#   libc-v6.a  rm, mv, cp, ln (05/83 .. 07/83)
#   libc-v7.a  cparse, scparse (10/83)
#   libc-v8.a  cc, scc
#   libc-v9.a  ld (07/83)
#   libc-v10.a write, rmdir (01/83)
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

LIBS = ../libc-v1.a ../libc-v2.a ../libc-v3.a ../libc-v4.a ../libc-v5.a ../libc-v6.a ../libc-v7.a ../libc-v8.a ../libc-v9.a ../libc-v10.a

all: $(LIBS)

LIBv1 = fseek/0_2.o ftell/0_1.o readsym/0_1.o rdwr/0_3.o system/0_1.o fopen/0_1.o printf/0_3.o doprnt/0_3.o \
	 ../libc/obj/ffltioc.o ungetc/0_1.o filbuf/0_3.o setbuf/0_1.o flsbuf/0_2.o endopen/0_3.o findiop/0_2.o \
	 ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o ../libc/obj/abort.o cnvt/0_2.o \
	 ../libc/obj/decimal.o length/0_2.o malloc/0_3.o isatty/0_1.o stty/0_1.o swab/0_1.o strncmp/0_1.o \
	 strncpy/0_1.o goodmagic/0_2.o swap/0_2.o ../libc/obj/brk.o ../libc/obj/close.o ../libc/obj/creat.o \
	 ../libc/obj/execl.o ../libc/obj/exit.o ../libc/obj/fork.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 open/0_1.o ../libc/obj/read.o ../libc/obj/signal.o ../libc/obj/time.o ../libc/obj/unlink.o \
	 ../libc/obj/wait.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v1.a: objs
	sh mklib.sh $@ $(AR) $(LIBv1)

LIBv2 = fopen/0_1.o printf/0_5.o doprnt/0_7.o ffltioc/0_1.o setbuf/0_1.o ../libc/obj/data.o flsbuf/0_2.o \
	 endopen/0_3.o findiop/0_2.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_2.o ../libc/obj/decimal.o \
	 length/0_3.o malloc/0_3.o isatty/0_2.o stty/0_3.o strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strncpy/0_1.o \
	 ../libc/obj/brk.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 open/0_1.o ../libc/obj/read.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v2.a: objs
	sh mklib.sh $@ $(AR) $(LIBv2)

LIBv3 = rdwr/0_1.o ../libc/obj/fopen.o printf/0_1.o doprnt/0_1.o ../libc/obj/ffltioc.o filbuf/0_1.o flsbuf/0_1.o \
	 endopen/0_1.o findiop/0_1.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_1.o \
	 ../libc/obj/decimal.o length/0_1.o malloc/0_1.o ../libc/obj/isatty.o ../libc/obj/stty.o \
	 ../libc/obj/swab.o goodmagic/0_1.o swap/0_1.o ../libc/obj/brk.o ../libc/obj/chmod.o ../libc/obj/close.o \
	 ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/umask.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v3.a: objs
	sh mklib.sh $@ $(AR) $(LIBv3)

LIBv4 = fseek/0_1.o rdwr/0_2.o ../libc/obj/fopen.o printf/0_2.o doprnt/0_2.o ../libc/obj/ffltioc.o filbuf/0_2.o \
	 flsbuf/0_1.o endopen/0_2.o findiop/0_1.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o \
	 cnvt/0_1.o ../libc/obj/decimal.o length/0_1.o malloc/0_2.o ../libc/obj/isatty.o mktemp/0_1.o \
	 ../libc/obj/stty.o ../libc/obj/swab.o ../libc/obj/strcmp.o strcpy/0_1.o goodmagic/0_1.o swap/0_1.o \
	 ../libc/obj/access.o ../libc/obj/brk.o ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o \
	 ../libc/obj/getpid.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v4.a: objs
	sh mklib.sh $@ $(AR) $(LIBv4)

LIBv5 = freopen/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o printf/0_5.o doprnt/0_5.o \
	 ../libc/obj/ffltioc.o getw/0_1.o ungetc/0_1.o filbuf/0_3.o setbuf/0_1.o flsbuf/0_2.o endopen/0_4.o \
	 findiop/0_2.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o perror/0_1.o atoi/0_1.o \
	 cnvt/0_2.o ../libc/obj/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o stty/0_3.o \
	 ../libc/obj/ctype_.o strrchr/0_1.o strcat/0_2.o strncmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 ../libc/obj/open.o ../libc/obj/read.o ../libc/obj/umask.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v5.a: objs
	sh mklib.sh $@ $(AR) $(LIBv5)

LIBv6 = system/0_2.o printf/0_6.o doprnt/0_6.o ../libc/obj/ffltioc.o gets/0_1.o filbuf/0_2.o flsbuf/0_1.o \
	 ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o perror/0_2.o ../libc/obj/sleep.o \
	 cnvt/0_1.o ../libc/obj/decimal.o length/0_1.o malloc/0_2.o errlst/0_2.o ../libc/obj/isatty.o \
	 ../libc/obj/stty.o strrchr/0_2.o ../libc/obj/strcmp.o strcpy/0_1.o strncpy/0_2.o strlen/0_1.o \
	 ../libc/obj/access.o ../libc/obj/alarm.o ../libc/obj/brk.o ../libc/obj/chmod.o ../libc/obj/close.o \
	 ../libc/obj/execl.o ../libc/obj/creat.o ../libc/obj/exit.o ../libc/obj/getuid.o ../libc/obj/fork.o \
	 ../libc/obj/ioctl.o ../libc/obj/link.o ../libc/obj/open.o ../libc/obj/pause.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/setuid.o ../libc/obj/stat.o ../libc/obj/unlink.o ../libc/obj/wait.o \
	 ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v6.a: objs
	sh mklib.sh $@ $(AR) $(LIBv6)

LIBv7 = fltioc/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o fdopen/0_1.o printf/0_5.o doprnt/0_5.o \
	 ungetc/0_1.o ../libc/obj/atof.o ecvt/0_1.o ../libc/obj/pow10.o filbuf/0_3.o data/0_1.o setbuf/0_1.o \
	 flsbuf/0_2.o endopen/0_4.o findiop/0_2.o ../libc/obj/__callex.o ../libc/obj/cuexit.o perror/0_1.o \
	 cnvt/0_2.o ../libc/obj/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o stty/0_3.o \
	 strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/obj/ctype_.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 ../libc/obj/open.o ../libc/obj/read.o ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/write.o \
	 ../libc/obj/cerror.o
../libc-v7.a: objs
	sh mklib.sh $@ $(AR) $(LIBv7)

LIBv8 = ../libc/obj/freopen.o ../libc/obj/fopen.o printf/0_4.o doprnt/0_4.o ../libc/obj/ffltioc.o \
	 ../libc/obj/flsbuf.o ../libc/obj/endopen.o ../libc/obj/findiop.o ../libc/obj/data.o \
	 ../libc/obj/__callex.o ../libc/obj/cuexit.o ../libc/obj/abort.o cnvt/0_1.o ../libc/obj/decimal.o \
	 length/0_1.o malloc/0_4.o ../libc/obj/ctype_.o ../libc/obj/isatty.o ../libc/obj/stty.o strcat/0_1.o \
	 ../libc/obj/strcmp.o strcpy/0_1.o strlen/0_1.o ../libc/obj/access.o ../libc/obj/brk.o ../libc/obj/chmod.o \
	 ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/execl.o ../libc/obj/execv.o ../libc/obj/fork.o \
	 ../libc/obj/getpid.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/signal.o \
	 ../libc/obj/stat.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/wait.o ../libc/obj/write.o \
	 ../libc/obj/cerror.o
../libc-v8.a: objs
	sh mklib.sh $@ $(AR) $(LIBv8)

LIBv9 = ../libc/obj/scanf.o doscan/0_1.o printf/0_6.o doprnt/0_6.o ../libc/obj/ffltioc.o ../libc/obj/ungetc.o \
	 filbuf/0_2.o data/0_1.o flsbuf/0_1.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_1.o \
	 ../libc/obj/decimal.o length/0_1.o malloc/0_2.o ../libc/obj/isatty.o mktemp/0_1.o ../libc/obj/ctype_.o \
	 ../libc/obj/stty.o ../libc/obj/swab.o rindex/0_1.o ../libc/obj/tolower.o strcat/0_1.o \
	 ../libc/obj/strncmp.o strcpy/0_1.o strncpy/0_2.o swap/0_1.o ../libc/obj/access.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/fstat.o ../libc/obj/getpid.o \
	 ../libc/obj/ioctl.o ../libc/obj/link.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v9.a: objs
	sh mklib.sh $@ $(AR) $(LIBv9)

LIBv10 = rdwr/0_4.o fopen/0_2.o printf/0_7.o doprnt/0_8.o ../libc/obj/ffltioc.o filbuf/0_4.o flsbuf/0_3.o endopen/0_5.o \
	 findiop/0_3.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o ttyname/0_1.o cnvt/0_3.o \
	 ../libc/obj/decimal.o length/0_4.o malloc/0_6.o isatty/0_3.o stty/0_4.o strcat/0_3.o strcmp/0_2.o \
	 strcpy/0_3.o strlen/0_3.o strrchr/0_3.o ../libc/obj/access.o ../libc/obj/alarm.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/execl.o ../libc/obj/fork.o \
	 ../libc/obj/fstat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/stat.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/wait.o \
	 ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v10.a: objs
	sh mklib.sh $@ $(AR) $(LIBv10)

# the modules: each directory makes its objects with its own Makefile
objs:
	-@mkdir atoi 2>/dev/null ; cd atoi ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir cnvt 2>/dev/null ; cd cnvt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir data 2>/dev/null ; cd data ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir doprnt 2>/dev/null ; cd doprnt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir doscan 2>/dev/null ; cd doscan ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir ecvt 2>/dev/null ; cd ecvt ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir endopen 2>/dev/null ; cd endopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir errlst 2>/dev/null ; cd errlst ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir fdopen 2>/dev/null ; cd fdopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir ffltioc 2>/dev/null ; cd ffltioc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir filbuf 2>/dev/null ; cd filbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir findiop 2>/dev/null ; cd findiop ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir flsbuf 2>/dev/null ; cd flsbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir fltioc 2>/dev/null ; cd fltioc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir fopen 2>/dev/null ; cd fopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir freopen 2>/dev/null ; cd freopen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir fseek 2>/dev/null ; cd fseek ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir ftell 2>/dev/null ; cd ftell ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir gets 2>/dev/null ; cd gets ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir getw 2>/dev/null ; cd getw ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir goodmagic 2>/dev/null ; cd goodmagic ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir isatty 2>/dev/null ; cd isatty ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir length 2>/dev/null ; cd length ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir malloc 2>/dev/null ; cd malloc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir mktemp 2>/dev/null ; cd mktemp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir open 2>/dev/null ; cd open ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir perror 2>/dev/null ; cd perror ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir printf 2>/dev/null ; cd printf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir rdwr 2>/dev/null ; cd rdwr ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir readsym 2>/dev/null ; cd readsym ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir rindex 2>/dev/null ; cd rindex ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir setbuf 2>/dev/null ; cd setbuf ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strcat 2>/dev/null ; cd strcat ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strcmp 2>/dev/null ; cd strcmp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strcpy 2>/dev/null ; cd strcpy ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strlen 2>/dev/null ; cd strlen ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strncmp 2>/dev/null ; cd strncmp ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strncpy 2>/dev/null ; cd strncpy ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir strrchr 2>/dev/null ; cd strrchr ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir stty 2>/dev/null ; cd stty ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir swab 2>/dev/null ; cd swab ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir swap 2>/dev/null ; cd swap ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir system 2>/dev/null ; cd system ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir ungetc 2>/dev/null ; cd ungetc ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs
	-@mkdir ttyname 2>/dev/null ; cd ttyname ; $(MAKE) CC='$(CC)' CCV1='$(CCV1)' CCV1Q='$(CCV1Q)' CCV1R='$(CCV1R)' CCV0='$(CCV0)' CCV0P='$(CCV0P)' CAS='$(CAS)' objs

clean:
	rm -f */0_*.o */*.o $(LIBS)
	rm -rf */*.d

.PHONY: all clean objs
