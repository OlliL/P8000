# makefile for libc/port/gen
#
#

SHELL=/bin/sh

CC=lcc
CFLAGS=-O

OBJECTS=\
a64l.o        abort.o       assert.o	\
atof.o        atoi.o        atol.o        bsearch.o	\
clock.o       crypt.o       ctime.o	\
ctype.o       drand48.o     ecvt.o	\
errlst.o      execvp.o      fakcu.o       frexp.o	\
ftok.o        ftw.o         gcvt.o        getcwd.o	\
getenv.o      getgrent.o    getgrgid.o    getgrnam.o	\
getlogin.o    getopt.o      getpw.o       getpwent.o	\
getpwnam.o    getpwuid.o    \
getut.o       hsearch.o     isatty.o	\
l3.o          l64a.o        ldexp.o       lsearch.o	\
mktemp.o      modf.o	\
nlist.o       perror.o      putpwent.o	\
qsort.o       rand.o        sleep.o       ssignal.o	\
strtol.o      swab.o        tell.o	\
tolower.o     toupper.o     tsearch.o     ttyname.o	\
ttyslot.o 

all: $(OBJECTS)

.c.o:
	@echo $*.c:
	$(CC) $(CFLAGS) -c $*.c

nlist.o:
	@echo $*.c:
	$(CC) $(CFLAGS) -c -DFLEXNAMES nlist.c

