AS=	as
ASFLAGS=
OBJ=	abort.s cuexit.s stty.c tell.c

all: $(OBJ)

clean:
	rm -f $(OBJ)

# the members of libc.a for ../obj (called by ../Makefile); CC and CAS are passed on
CC = cc
CAS = cas

LIBOBJS = \
	../obj/getgrgid.o ../obj/getgrnam.o ../obj/getgrent.o ../obj/getpwnam.o ../obj/getpwuid.o ../obj/putpwent.o \
	../obj/getpwent.o ../obj/getopt.o ../obj/getpw.o ../obj/readsym.o ../obj/stfl.o ../obj/atof.o \
	../obj/ecvt.o ../obj/pow10.o ../obj/__callex.o ../obj/cuexit.o ../obj/execvp.o ../obj/ctime.o \
	../obj/getenv.o ../obj/getlogin.o ../obj/perror.o ../obj/sleep.o ../obj/ttyslot.o ../obj/ttyname.o \
	../obj/abort.o ../obj/abs.o ../obj/atoi.o ../obj/atol.o ../obj/bsearch.o ../obj/cnvt.o \
	../obj/decimal.o ../obj/length.o ../obj/crypt.o ../obj/errlst.o ../obj/fakcu.o ../obj/isatty.o \
	../obj/l3.o ../obj/lsearch.o ../obj/mktemp.o ../obj/mon.o ../obj/mpx.o ../obj/qsort.o \
	../obj/rand.o ../obj/setret.o ../obj/ssignal.o ../obj/stty.o ../obj/swab.o ../obj/tell.o \
	../obj/ctype_.o ../obj/index.o ../obj/rindex.o ../obj/toupper.o ../obj/tolower.o ../obj/strcat.o \
	../obj/strncat.o ../obj/strcmp.o ../obj/strncmp.o ../obj/strcpy.o ../obj/strncpy.o ../obj/strlen.o \
	../obj/strtok.o ../obj/strchr.o ../obj/strcspn.o ../obj/strpbrk.o ../obj/strrchr.o ../obj/strspn.o \
	../obj/goodmagic.o ../obj/a64l.o ../obj/l64a.o

objs: $(LIBOBJS)

../obj/getgrgid.o: getgrgid.c
	$(CC) -O -c getgrgid.c ; mv getgrgid.o ../obj/getgrgid.o

../obj/getgrnam.o: getgrnam.c
	$(CC) -O -c getgrnam.c ; mv getgrnam.o ../obj/getgrnam.o

../obj/getgrent.o: getgrent.c
	$(CC) -O -c getgrent.c ; mv getgrent.o ../obj/getgrent.o

../obj/getpwnam.o: getpwnam.c
	$(CC) -O -c getpwnam.c ; mv getpwnam.o ../obj/getpwnam.o

../obj/getpwuid.o: getpwuid.c
	$(CC) -O -c getpwuid.c ; mv getpwuid.o ../obj/getpwuid.o

../obj/putpwent.o: putpwent.c
	$(CC) -O -c putpwent.c ; mv putpwent.o ../obj/putpwent.o

../obj/getpwent.o: getpwent.c
	$(CC) -O -c getpwent.c ; mv getpwent.o ../obj/getpwent.o

../obj/getopt.o: getopt.c
	$(CC) -O -c getopt.c ; mv getopt.o ../obj/getopt.o

../obj/getpw.o: getpw.c
	$(CC) -O -c getpw.c ; mv getpw.o ../obj/getpw.o

../obj/readsym.o: readsym.c
	$(CC) -O -c readsym.c ; mv readsym.o ../obj/readsym.o

../obj/stfl.o: stfl.s
	$(CAS) -o ../obj/stfl.o stfl.s

../obj/atof.o: atof.s
	$(AS) -f -o ../obj/atof.o atof.s

../obj/ecvt.o: ecvt.s
	$(AS) -f -o ../obj/ecvt.o ecvt.s

../obj/pow10.o: pow10.s
	$(AS) -f -o ../obj/pow10.o pow10.s

../obj/__callex.o: __callex.c
	$(CC) -O -c __callex.c ; mv __callex.o ../obj/__callex.o

../obj/cuexit.o: cuexit.s
	$(AS) -o ../obj/cuexit.o cuexit.s

../obj/execvp.o: execvp.c
	$(CC) -O -c execvp.c ; mv execvp.o ../obj/execvp.o

../obj/ctime.o: ctime.c
	$(CC) -O -c ctime.c ; mv ctime.o ../obj/ctime.o

../obj/getenv.o: getenv.c
	$(CC) -O -c getenv.c ; mv getenv.o ../obj/getenv.o

../obj/getlogin.o: getlogin.c
	$(CC) -O -c getlogin.c ; mv getlogin.o ../obj/getlogin.o

../obj/perror.o: perror.c
	$(CC) -O -c perror.c ; mv perror.o ../obj/perror.o

../obj/sleep.o: sleep.c
	$(CC) -O -c sleep.c ; mv sleep.o ../obj/sleep.o

../obj/ttyslot.o: ttyslot.c
	$(CC) -O -c ttyslot.c ; mv ttyslot.o ../obj/ttyslot.o

../obj/ttyname.o: ttyname.c
	$(CC) -O -c ttyname.c ; mv ttyname.o ../obj/ttyname.o

../obj/abort.o: abort.s
	$(AS) -o ../obj/abort.o abort.s

../obj/abs.o: abs.c
	$(CC) -O -c abs.c ; mv abs.o ../obj/abs.o

../obj/atoi.o: atoi.c
	$(CC) -O -c atoi.c ; mv atoi.o ../obj/atoi.o

../obj/atol.o: atol.c
	$(CC) -O -c atol.c ; mv atol.o ../obj/atol.o

../obj/bsearch.o: bsearch.c
	$(CC) -O -c bsearch.c ; mv bsearch.o ../obj/bsearch.o

../obj/cnvt.o: cnvt.c
	$(CC) -O -c cnvt.c ; mv cnvt.o ../obj/cnvt.o

../obj/decimal.o: decimal.s
	$(AS) -o ../obj/decimal.o decimal.s

../obj/length.o: length.c
	$(CC) -O -c length.c ; mv length.o ../obj/length.o

../obj/crypt.o: crypt.c
	$(CC) -O -c crypt.c ; mv crypt.o ../obj/crypt.o

../obj/errlst.o: errlst.c
	$(CC) -O -c errlst.c ; mv errlst.o ../obj/errlst.o

../obj/fakcu.o: fakcu.s
	$(AS) -o ../obj/fakcu.o fakcu.s

../obj/isatty.o: isatty.c
	$(CC) -O -c isatty.c ; mv isatty.o ../obj/isatty.o

../obj/l3.o: l3.c
	$(CC) -O -c l3.c ; mv l3.o ../obj/l3.o

../obj/lsearch.o: lsearch.c
	$(CC) -O -c lsearch.c ; mv lsearch.o ../obj/lsearch.o

../obj/mktemp.o: mktemp.c
	$(CC) -O -c mktemp.c ; mv mktemp.o ../obj/mktemp.o

../obj/mon.o: mon.c
	$(CC) -O -c mon.c ; mv mon.o ../obj/mon.o

../obj/mpx.o: mpx.c
	$(CC) -O -c mpx.c ; mv mpx.o ../obj/mpx.o

../obj/qsort.o: qsort.c
	$(CC) -O -c qsort.c ; mv qsort.o ../obj/qsort.o

../obj/rand.o: rand.c
	$(CC) -O -c rand.c ; mv rand.o ../obj/rand.o

../obj/setret.o: setret.s
	$(AS) -o ../obj/setret.o setret.s

../obj/ssignal.o: ssignal.c
	$(CC) -O -c ssignal.c ; mv ssignal.o ../obj/ssignal.o

../obj/stty.o: stty.c
	$(CC) -O -c stty.c ; mv stty.o ../obj/stty.o

../obj/swab.o: swab.c
	$(CC) -O -c swab.c ; mv swab.o ../obj/swab.o

../obj/tell.o: tell.c
	$(CC) -O -c tell.c ; mv tell.o ../obj/tell.o

../obj/ctype_.o: ctype_.c
	$(CC) -O -c ctype_.c ; mv ctype_.o ../obj/ctype_.o

../obj/index.o: index.c
	$(CC) -O -c index.c ; mv index.o ../obj/index.o

../obj/rindex.o: rindex.c
	$(CC) -O -c rindex.c ; mv rindex.o ../obj/rindex.o

../obj/toupper.o: toupper.c
	$(CC) -O -c toupper.c ; mv toupper.o ../obj/toupper.o

../obj/tolower.o: tolower.c
	$(CC) -O -c tolower.c ; mv tolower.o ../obj/tolower.o

../obj/strcat.o: strcat.c
	$(CC) -O -c strcat.c ; mv strcat.o ../obj/strcat.o

../obj/strncat.o: strncat.c
	$(CC) -O -c strncat.c ; mv strncat.o ../obj/strncat.o

../obj/strcmp.o: strcmp.c
	$(CC) -O -c strcmp.c ; mv strcmp.o ../obj/strcmp.o

../obj/strncmp.o: strncmp.c
	$(CC) -O -c strncmp.c ; mv strncmp.o ../obj/strncmp.o

../obj/strcpy.o: strcpy.c
	$(CC) -O -c strcpy.c ; mv strcpy.o ../obj/strcpy.o

../obj/strncpy.o: strncpy.c
	$(CC) -O -c strncpy.c ; mv strncpy.o ../obj/strncpy.o

../obj/strlen.o: strlen.c
	$(CC) -O -c strlen.c ; mv strlen.o ../obj/strlen.o

../obj/strtok.o: strtok.c
	$(CC) -O -c strtok.c ; mv strtok.o ../obj/strtok.o

../obj/strchr.o: strchr.c
	$(CC) -O -c strchr.c ; mv strchr.o ../obj/strchr.o

../obj/strcspn.o: strcspn.c
	$(CC) -O -c strcspn.c ; mv strcspn.o ../obj/strcspn.o

../obj/strpbrk.o: strpbrk.c
	$(CC) -O -c strpbrk.c ; mv strpbrk.o ../obj/strpbrk.o

../obj/strrchr.o: strrchr.c
	$(CC) -O -c strrchr.c ; mv strrchr.o ../obj/strrchr.o

../obj/strspn.o: strspn.c
	$(CC) -O -c strspn.c ; mv strspn.o ../obj/strspn.o

../obj/goodmagic.o: goodmagic.c
	$(CC) -O -c goodmagic.c ; mv goodmagic.o ../obj/goodmagic.o

../obj/a64l.o: a64l.c
	$(CC) -O -c a64l.c ; mv a64l.o ../obj/a64l.o

../obj/l64a.o: l64a.c
	$(CC) -O -c l64a.c ; mv l64a.o ../obj/l64a.o

.PHONY: objs
