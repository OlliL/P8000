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
../libc-v1.a: $(LIBv1)
	sh mklib.sh $@ $(AR) $(LIBv1)

LIBv2 = fopen/0_1.o printf/0_5.o doprnt/0_7.o ffltioc/0_1.o setbuf/0_1.o ../libc/obj/data.o flsbuf/0_2.o \
	 endopen/0_3.o findiop/0_2.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_2.o ../libc/obj/decimal.o \
	 length/0_3.o malloc/0_3.o isatty/0_2.o stty/0_3.o strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strncpy/0_1.o \
	 ../libc/obj/brk.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 open/0_1.o ../libc/obj/read.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v2.a: $(LIBv2)
	sh mklib.sh $@ $(AR) $(LIBv2)

LIBv3 = rdwr/0_1.o ../libc/obj/fopen.o printf/0_1.o doprnt/0_1.o ../libc/obj/ffltioc.o filbuf/0_1.o flsbuf/0_1.o \
	 endopen/0_1.o findiop/0_1.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_1.o \
	 ../libc/obj/decimal.o length/0_1.o malloc/0_1.o ../libc/obj/isatty.o ../libc/obj/stty.o \
	 ../libc/obj/swab.o goodmagic/0_1.o swap/0_1.o ../libc/obj/brk.o ../libc/obj/chmod.o ../libc/obj/close.o \
	 ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/umask.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v3.a: $(LIBv3)
	sh mklib.sh $@ $(AR) $(LIBv3)

LIBv4 = fseek/0_1.o rdwr/0_2.o ../libc/obj/fopen.o printf/0_2.o doprnt/0_2.o ../libc/obj/ffltioc.o filbuf/0_2.o \
	 flsbuf/0_1.o endopen/0_2.o findiop/0_1.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o \
	 cnvt/0_1.o ../libc/obj/decimal.o length/0_1.o malloc/0_2.o ../libc/obj/isatty.o mktemp/0_1.o \
	 ../libc/obj/stty.o ../libc/obj/swab.o ../libc/obj/strcmp.o strcpy/0_1.o goodmagic/0_1.o swap/0_1.o \
	 ../libc/obj/access.o ../libc/obj/brk.o ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o \
	 ../libc/obj/getpid.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v4.a: $(LIBv4)
	sh mklib.sh $@ $(AR) $(LIBv4)

LIBv5 = freopen/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o printf/0_5.o doprnt/0_5.o \
	 ../libc/obj/ffltioc.o getw/0_1.o ungetc/0_1.o filbuf/0_3.o setbuf/0_1.o flsbuf/0_2.o endopen/0_4.o \
	 findiop/0_2.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o perror/0_1.o atoi/0_1.o \
	 cnvt/0_2.o ../libc/obj/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o stty/0_3.o \
	 ../libc/obj/ctype_.o strrchr/0_1.o strcat/0_2.o strncmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 ../libc/obj/open.o ../libc/obj/read.o ../libc/obj/umask.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v5.a: $(LIBv5)
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
../libc-v6.a: $(LIBv6)
	sh mklib.sh $@ $(AR) $(LIBv6)

LIBv7 = fltioc/0_1.o fseek/0_2.o ftell/0_2.o rdwr/0_3.o fopen/0_1.o fdopen/0_1.o printf/0_5.o doprnt/0_5.o \
	 ungetc/0_1.o ../libc/obj/atof.o ecvt/0_1.o ../libc/obj/pow10.o filbuf/0_3.o data/0_1.o setbuf/0_1.o \
	 flsbuf/0_2.o endopen/0_4.o findiop/0_2.o ../libc/obj/__callex.o ../libc/obj/cuexit.o perror/0_1.o \
	 cnvt/0_2.o ../libc/obj/decimal.o length/0_3.o malloc/0_3.o errlst/0_1.o isatty/0_2.o stty/0_3.o \
	 strcat/0_2.o strcmp/0_1.o strcpy/0_2.o strlen/0_2.o ../libc/obj/ctype_.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o \
	 ../libc/obj/open.o ../libc/obj/read.o ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/write.o \
	 ../libc/obj/cerror.o
../libc-v7.a: $(LIBv7)
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
../libc-v8.a: $(LIBv8)
	sh mklib.sh $@ $(AR) $(LIBv8)

LIBv9 = ../libc/obj/scanf.o doscan/0_1.o printf/0_6.o doprnt/0_6.o ../libc/obj/ffltioc.o ../libc/obj/ungetc.o \
	 filbuf/0_2.o data/0_1.o flsbuf/0_1.o ../libc/obj/__callex.o ../libc/obj/cuexit.o cnvt/0_1.o \
	 ../libc/obj/decimal.o length/0_1.o malloc/0_2.o ../libc/obj/isatty.o mktemp/0_1.o ../libc/obj/ctype_.o \
	 ../libc/obj/stty.o ../libc/obj/swab.o rindex/0_1.o ../libc/obj/tolower.o strcat/0_1.o \
	 ../libc/obj/strncmp.o strcpy/0_1.o strncpy/0_2.o swap/0_1.o ../libc/obj/access.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/fstat.o ../libc/obj/getpid.o \
	 ../libc/obj/ioctl.o ../libc/obj/link.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v9.a: $(LIBv9)
	sh mklib.sh $@ $(AR) $(LIBv9)

atoi/0_1.o: ../libc/gen/atoi.c
	-@mkdir atoi 2>/dev/null
	cd atoi ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/atoi.c ; mv atoi.o 0_1.o

cnvt/0_1.o: ../libc/gen/cnvt.c
	cd cnvt ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/cnvt.c ; mv cnvt.o 0_1.o

cnvt/0_2.o: cnvt/0_2.s
	cd cnvt ; $(CAS) -u -o 0_2.o 0_2.s

data/0_1.o: data/0_1.c
	cd data ; $(CCV0P) -O -c 0_1.c

doprnt/0_1.o: doprnt/0_1.s
	cd doprnt ; $(CAS) -u -o 0_1.o 0_1.s

doprnt/0_2.o: doprnt/0_2.c
	cd doprnt ; $(CCV1) -O -c 0_2.c

doprnt/0_3.o: doprnt/0_3.s
	cd doprnt ; $(CAS) -u -o 0_3.o 0_3.s

doprnt/0_4.o: doprnt/0_4.c
	cd doprnt ; $(CC) -O -c 0_4.c

doprnt/0_5.o: doprnt/0_5.s
	cd doprnt ; $(CAS) -u -o 0_5.o 0_5.s

doprnt/0_6.o: doprnt/0_6.c
	cd doprnt ; $(CCV1) -O -c 0_6.c

doprnt/0_7.o: doprnt/0_7.s
	cd doprnt ; $(CAS) -u -o 0_7.o 0_7.s

doscan/0_1.o: doscan/0_1.c
	cd doscan ; $(CCV1) -O -I../../../cmd/cparse -c 0_1.c

ecvt/0_1.o: ecvt/0_1.s
	cd ecvt ; $(CAS) -u -o 0_1.o 0_1.s

endopen/0_1.o: endopen/0_5.c
	-@mkdir endopen/0_1.d 2>/dev/null
	cd endopen/0_1.d ; $(CCV1Q) -O -I../../../../cmd/cparse -c ../0_5.c ; mv 0_5.o ../0_1.o

endopen/0_2.o: ../libc/stdio/endopen.c
	cd endopen ; $(CCV1) -O -I../../libc/stdio -c ../../libc/stdio/endopen.c ; mv endopen.o 0_2.o

endopen/0_3.o: endopen/0_3.c
	cd endopen ; $(CCV0P) -O -c 0_3.c

endopen/0_4.o: ../libc/stdio/endopen.c
	cd endopen ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/endopen.c ; mv endopen.o 0_4.o

errlst/0_1.o: errlst/0_1.c
	cd errlst ; $(CCV0P) -O -c 0_1.c

errlst/0_2.o: errlst/0_2.c
	cd errlst ; $(CCV1) -O -c 0_2.c

fdopen/0_1.o: ../libc/stdio/fdopen.c
	-@mkdir fdopen 2>/dev/null
	cd fdopen ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/fdopen.c ; mv fdopen.o 0_1.o

ffltioc/0_1.o: ffltioc/0_1.c
	cd ffltioc ; $(CCV0P) -O -c 0_1.c

filbuf/0_1.o: ../libc/stdio/filbuf.c
	-@mkdir filbuf 2>/dev/null
	cd filbuf ; $(CCV1Q) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/filbuf.c ; mv filbuf.o 0_1.o

filbuf/0_2.o: ../libc/stdio/filbuf.c
	-@mkdir filbuf 2>/dev/null
	cd filbuf ; $(CCV1) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/filbuf.c ; mv filbuf.o 0_2.o

filbuf/0_3.o: ../libc/stdio/filbuf.c
	-@mkdir filbuf 2>/dev/null
	cd filbuf ; $(CCV0P) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/filbuf.c ; mv filbuf.o 0_3.o

findiop/0_1.o: ../libc/stdio/findiop.c
	-@mkdir findiop 2>/dev/null
	cd findiop ; $(CCV1) -O -I../../libc/stdio -c ../../libc/stdio/findiop.c ; mv findiop.o 0_1.o

findiop/0_2.o: ../libc/stdio/findiop.c
	-@mkdir findiop 2>/dev/null
	cd findiop ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/findiop.c ; mv findiop.o 0_2.o

flsbuf/0_1.o: flsbuf/flsbuf.c
	cd flsbuf ; $(CCV1) -O -c flsbuf.c ; mv flsbuf.o 0_1.o

flsbuf/0_2.o: flsbuf/flsbuf.c
	cd flsbuf ; $(CCV0P) -O -c flsbuf.c ; mv flsbuf.o 0_2.o

fltioc/0_1.o: fltioc/0_1.s
	cd fltioc ; $(CAS) -u -o 0_1.o 0_1.s

fopen/0_1.o: ../libc/stdio/fopen.c
	-@mkdir fopen 2>/dev/null
	cd fopen ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/fopen.c ; mv fopen.o 0_1.o

freopen/0_1.o: ../libc/stdio/freopen.c
	-@mkdir freopen 2>/dev/null
	cd freopen ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/freopen.c ; mv freopen.o 0_1.o

fseek/0_1.o: ../libc/stdio/fseek.c
	cd fseek ; $(CCV1) -O -I../../libc/stdio -c ../../libc/stdio/fseek.c ; mv fseek.o 0_1.o

fseek/0_2.o: fseek/0_2.s
	cd fseek ; $(CAS) -u -o 0_2.o 0_2.s

ftell/0_1.o: ../libc/stdio/ftell.c
	-@mkdir ftell 2>/dev/null
	cd ftell ; $(CCV0) -O -I../../libc/stdio -c ../../libc/stdio/ftell.c ; mv ftell.o 0_1.o

ftell/0_2.o: ../libc/stdio/ftell.c
	-@mkdir ftell 2>/dev/null
	cd ftell ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/ftell.c ; mv ftell.o 0_2.o

gets/0_1.o: ../libc/stdio/gets.c
	-@mkdir gets 2>/dev/null
	cd gets ; $(CCV1) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/gets.c ; mv gets.o 0_1.o

getw/0_1.o: ../libc/stdio/getw.c
	-@mkdir getw 2>/dev/null
	cd getw ; $(CCV0P) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/getw.c ; mv getw.o 0_1.o

goodmagic/0_1.o: ../libc/gen/goodmagic.c
	-@mkdir goodmagic 2>/dev/null
	cd goodmagic ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/goodmagic.c ; mv goodmagic.o 0_1.o

goodmagic/0_2.o: ../libc/gen/goodmagic.c
	-@mkdir goodmagic 2>/dev/null
	cd goodmagic ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/goodmagic.c ; mv goodmagic.o 0_2.o

isatty/0_1.o: ../libc/gen/isatty.c
	-@mkdir isatty 2>/dev/null
	cd isatty ; $(CCV0) -O -I../../libc/gen -c ../../libc/gen/isatty.c ; mv isatty.o 0_1.o

isatty/0_2.o: ../libc/gen/isatty.c
	-@mkdir isatty 2>/dev/null
	cd isatty ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/isatty.c ; mv isatty.o 0_2.o

length/0_1.o: ../libc/gen/length.c
	cd length ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/length.c ; mv length.o 0_1.o

length/0_2.o: length/0_2.c
	cd length ; $(CCV0P) -O -c 0_2.c

length/0_3.o: ../libc/gen/length.c
	cd length ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/length.c ; mv length.o 0_3.o

malloc/0_1.o: malloc/malloc.c
	-@mkdir malloc/0_1.d 2>/dev/null
	cd malloc/0_1.d ; $(CCV1Q) -O -c ../malloc.c ; mv malloc.o ../0_1.o

malloc/0_2.o: malloc/malloc.c
	cd malloc ; $(CCV1) -O -c malloc.c ; mv malloc.o 0_2.o

malloc/0_3.o: malloc/0_3.s
	cd malloc ; $(CAS) -u -o 0_3.o 0_3.s

malloc/0_4.o: malloc/0_4.s
	cd malloc ; $(CAS) -u -o 0_4.o 0_4.s

mktemp/0_1.o: ../libc/gen/mktemp.c
	-@mkdir mktemp 2>/dev/null
	cd mktemp ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/mktemp.c ; mv mktemp.o 0_1.o

open/0_1.o: open/0_1.s
	cd open ; $(CAS) -u -o 0_1.o 0_1.s

perror/0_1.o: ../libc/gen/perror.c
	cd perror ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/perror.c ; mv perror.o 0_1.o

perror/0_2.o: perror/0_2.c
	cd perror ; $(CCV1) -O -c 0_2.c

printf/0_1.o: printf/0_2.c
	-@mkdir printf/0_1.d 2>/dev/null
	cd printf/0_1.d ; $(CCV1Q) -O -I../../../../cmd/cparse -c ../0_2.c ; mv 0_2.o ../0_1.o

printf/0_2.o: printf/0_2.c
	cd printf ; $(CCV1) -O -I../../../cmd/cparse -c 0_2.c

printf/0_3.o: printf/0_5.c
	-@mkdir printf/0_3.d 2>/dev/null
	cd printf/0_3.d ; $(CCV0) -O -I../../../../cmd/cparse -c ../0_5.c ; mv 0_5.o ../0_3.o

printf/0_4.o: printf/0_4.c
	cd printf ; $(CCV1) -O -c 0_4.c

printf/0_5.o: printf/0_5.c
	cd printf ; $(CCV0P) -O -I../../../cmd/cparse -c 0_5.c

printf/0_6.o: printf/0_4.c
	cd printf ; $(CCV1) -O -I../../../cmd/cparse -c 0_4.c ; mv 0_4.o 0_6.o

rdwr/0_1.o: ../libc/stdio/rdwr.c
	-@mkdir rdwr 2>/dev/null
	cd rdwr ; $(CCV1Q) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/rdwr.c ; mv rdwr.o 0_1.o

rdwr/0_2.o: ../libc/stdio/rdwr.c
	-@mkdir rdwr 2>/dev/null
	cd rdwr ; $(CCV1) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/rdwr.c ; mv rdwr.o 0_2.o

rdwr/0_3.o: ../libc/stdio/rdwr.c
	-@mkdir rdwr 2>/dev/null
	cd rdwr ; $(CCV0P) -O -I../../../cmd/cparse -I../../libc/stdio -c ../../libc/stdio/rdwr.c ; mv rdwr.o 0_3.o

readsym/0_1.o: readsym/0_1.c
	cd readsym ; $(CCV0) -O -c 0_1.c

rindex/0_1.o: ../libc/gen/rindex.c
	-@mkdir rindex 2>/dev/null
	cd rindex ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/rindex.c ; mv rindex.o 0_1.o

setbuf/0_1.o: ../libc/stdio/setbuf.c
	-@mkdir setbuf 2>/dev/null
	cd setbuf ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/setbuf.c ; mv setbuf.o 0_1.o

strcat/0_1.o: ../libc/gen/strcat.c
	-@mkdir strcat 2>/dev/null
	cd strcat ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/strcat.c ; mv strcat.o 0_1.o

strcat/0_2.o: ../libc/gen/strcat.c
	-@mkdir strcat 2>/dev/null
	cd strcat ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strcat.c ; mv strcat.o 0_2.o

strcmp/0_1.o: ../libc/gen/strcmp.c
	-@mkdir strcmp 2>/dev/null
	cd strcmp ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strcmp.c ; mv strcmp.o 0_1.o

strcpy/0_1.o: ../libc/gen/strcpy.c
	-@mkdir strcpy 2>/dev/null
	cd strcpy ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_1.o

strcpy/0_2.o: ../libc/gen/strcpy.c
	-@mkdir strcpy 2>/dev/null
	cd strcpy ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_2.o

strlen/0_1.o: ../libc/gen/strlen.c
	-@mkdir strlen 2>/dev/null
	cd strlen ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/strlen.c ; mv strlen.o 0_1.o

strlen/0_2.o: ../libc/gen/strlen.c
	-@mkdir strlen 2>/dev/null
	cd strlen ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strlen.c ; mv strlen.o 0_2.o

strncmp/0_1.o: ../libc/gen/strncmp.c
	-@mkdir strncmp 2>/dev/null
	cd strncmp ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strncmp.c ; mv strncmp.o 0_1.o

strncpy/0_1.o: ../libc/gen/strncpy.c
	-@mkdir strncpy 2>/dev/null
	cd strncpy ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strncpy.c ; mv strncpy.o 0_1.o

strncpy/0_2.o: ../libc/gen/strncpy.c
	-@mkdir strncpy 2>/dev/null
	cd strncpy ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/strncpy.c ; mv strncpy.o 0_2.o

strrchr/0_1.o: ../libc/gen/strrchr.c
	-@mkdir strrchr 2>/dev/null
	cd strrchr ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/strrchr.c ; mv strrchr.o 0_1.o

strrchr/0_2.o: ../libc/gen/strrchr.c
	-@mkdir strrchr 2>/dev/null
	cd strrchr ; $(CCV1) -O -I../../libc/gen -c ../../libc/gen/strrchr.c ; mv strrchr.o 0_2.o

stty/0_1.o: ../libc/gen/stty.c
	-@mkdir stty 2>/dev/null
	cd stty ; $(CCV0) -O -I../../libc/gen -c ../../libc/gen/stty.c ; mv stty.o 0_1.o

stty/0_3.o: ../libc/gen/stty.c
	-@mkdir stty 2>/dev/null
	cd stty ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/stty.c ; mv stty.o 0_3.o

swab/0_1.o: ../libc/gen/swab.c
	-@mkdir swab 2>/dev/null
	cd swab ; $(CCV0P) -O -I../../libc/gen -c ../../libc/gen/swab.c ; mv swab.o 0_1.o

swap/0_1.o: swap/0_1.s
	cd swap ; $(CAS) -u -o 0_1.o 0_1.s

swap/0_2.o: ../libc/gen/swap.c
	cd swap ; $(CCV0) -O -I../../libc/gen -c ../../libc/gen/swap.c ; mv swap.o 0_2.o

system/0_1.o: system/0_1.c
	cd system ; $(CCV0) -O -c 0_1.c

system/0_2.o: ../libc/stdio/system.c
	cd system ; $(CCV1) -O -I../../libc/stdio -c ../../libc/stdio/system.c ; mv system.o 0_2.o

ungetc/0_1.o: ../libc/stdio/ungetc.c
	-@mkdir ungetc 2>/dev/null
	cd ungetc ; $(CCV0P) -O -I../../libc/stdio -c ../../libc/stdio/ungetc.c ; mv ungetc.o 0_1.o

LIBv10 = rdwr/0_4.o fopen/0_2.o printf/0_7.o doprnt/0_8.o ../libc/obj/ffltioc.o filbuf/0_4.o flsbuf/0_3.o endopen/0_5.o \
	 findiop/0_3.o ../libc/obj/data.o ../libc/obj/__callex.o ../libc/obj/cuexit.o ttyname/0_1.o cnvt/0_3.o \
	 ../libc/obj/decimal.o length/0_4.o malloc/0_6.o isatty/0_3.o stty/0_4.o strcat/0_3.o strcmp/0_2.o \
	 strcpy/0_3.o strlen/0_3.o strrchr/0_3.o ../libc/obj/access.o ../libc/obj/alarm.o ../libc/obj/brk.o \
	 ../libc/obj/chmod.o ../libc/obj/close.o ../libc/obj/creat.o ../libc/obj/execl.o ../libc/obj/fork.o \
	 ../libc/obj/fstat.o ../libc/obj/ioctl.o ../libc/obj/lseek.o ../libc/obj/open.o ../libc/obj/read.o \
	 ../libc/obj/signal.o ../libc/obj/stat.o ../libc/obj/umask.o ../libc/obj/unlink.o ../libc/obj/wait.o \
	 ../libc/obj/write.o ../libc/obj/cerror.o
../libc-v10.a: $(LIBv10)
	sh mklib.sh $@ $(AR) $(LIBv10)

rdwr/0_4.o: ../libc/stdio/rdwr.c
	-@mkdir rdwr 2>/dev/null
	cd rdwr ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/stdio/rdwr.c ; mv rdwr.o 0_4.o

fopen/0_2.o: ../libc/stdio/fopen.c
	-@mkdir fopen 2>/dev/null
	cd fopen ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/stdio/fopen.c ; mv fopen.o 0_2.o

filbuf/0_4.o: ../libc/stdio/filbuf.c
	-@mkdir filbuf 2>/dev/null
	cd filbuf ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/stdio/filbuf.c ; mv filbuf.o 0_4.o

findiop/0_3.o: ../libc/stdio/findiop.c
	-@mkdir findiop 2>/dev/null
	cd findiop ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/stdio/findiop.c ; mv findiop.o 0_3.o

ttyname/0_1.o: ../libc/gen/ttyname.c
	-@mkdir ttyname 2>/dev/null
	cd ttyname ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/ttyname.c ; mv ttyname.o 0_1.o

cnvt/0_3.o: ../libc/gen/cnvt.c
	-@mkdir cnvt 2>/dev/null
	cd cnvt ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/cnvt.c ; mv cnvt.o 0_3.o

length/0_4.o: ../libc/gen/length.c
	-@mkdir length 2>/dev/null
	cd length ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/length.c ; mv length.o 0_4.o

isatty/0_3.o: ../libc/gen/isatty.c
	-@mkdir isatty 2>/dev/null
	cd isatty ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/isatty.c ; mv isatty.o 0_3.o

stty/0_4.o: ../libc/gen/stty.c
	-@mkdir stty 2>/dev/null
	cd stty ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/stty.c ; mv stty.o 0_4.o

strcat/0_3.o: ../libc/gen/strcat.c
	-@mkdir strcat 2>/dev/null
	cd strcat ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/strcat.c ; mv strcat.o 0_3.o

strcmp/0_2.o: ../libc/gen/strcmp.c
	-@mkdir strcmp 2>/dev/null
	cd strcmp ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/strcmp.c ; mv strcmp.o 0_2.o

strcpy/0_3.o: ../libc/gen/strcpy.c
	-@mkdir strcpy 2>/dev/null
	cd strcpy ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/strcpy.c ; mv strcpy.o 0_3.o

strlen/0_3.o: ../libc/gen/strlen.c
	-@mkdir strlen 2>/dev/null
	cd strlen ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/strlen.c ; mv strlen.o 0_3.o

strrchr/0_3.o: ../libc/gen/strrchr.c
	-@mkdir strrchr 2>/dev/null
	cd strrchr ; $(CCV1R) -O -I../../../cmd/cparse -c ../../libc/gen/strrchr.c ; mv strrchr.o 0_3.o

flsbuf/0_3.o: flsbuf/flsbuf.c
	cd flsbuf ; $(CCV1R) -O -I../../../cmd/cparse -c flsbuf.c ; mv flsbuf.o 0_3.o

endopen/0_5.o: endopen/0_5.c
	cd endopen ; $(CCV1R) -O -I../../../cmd/cparse -c 0_5.c

printf/0_7.o: printf/0_2.c
	-@mkdir printf/0_7.d 2>/dev/null
	cd printf/0_7.d ; $(CCV1R) -O -I../../../../cmd/cparse -c ../0_2.c ; mv 0_2.o ../0_7.o

malloc/0_6.o: malloc/malloc.c
	-@mkdir malloc/0_6.d 2>/dev/null
	cd malloc/0_6.d ; $(CCV1R) -O -c ../malloc.c ; mv malloc.o ../0_6.o

doprnt/0_8.o: doprnt/0_8.s
	cd doprnt ; $(CAS) -u -o 0_8.o 0_8.s

clean:
	rm -f */0_*.o */*.o $(LIBS)
	rm -rf *.d

.PHONY: all clean
