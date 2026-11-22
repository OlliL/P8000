# libc: /lib/libc.a and /lib/crt0.o (nonsegmented, build/nonseg) and /lib/slibc.a and /lib/scrt0.o (segmented, build/seg) from
# one tree, byte-identical with the WEGA files; the members by vl (ccvl, sccvl from OLDROOT, host/OLDCC.md), the assembler
# sources by the shipped as and cas. common/: the sources of both (C with #ifdef SEG where the segmented member differs),
# nonseg/: only libc.a, seg/: only slibc.a; the archive headers: V/arhdr.s, V/Library.SC, mkar.sh. As src/cmd/cc/codgen: each
# variant is made in its build directory by this Makefile (V=nonseg or seg), the objects in place; safe with -j.
#
#   make        build/nonseg/libc.a, crt0.o; build/seg/slibc.a, scrt0.o (and the members, used by ../oldlibc)
#   make install PREFIX=dir   dir/lib/libc.a, crt0.o, slibc.a, scrt0.o
#   make clean

CC = ccvl
SCC = sccvl
CAS = cas
AS = as
PREFIX = /

ALL_TARGETS = build/nonseg/libc.a build/seg/slibc.a

all: $(ALL_TARGETS)

build/nonseg/libc.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile V=nonseg CC="$(CC)" CFLAGS="-O" CAS="$(CAS)" AS="$(AS)" libc.a crt0.o
build/seg/slibc.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile V=seg CC="$(SCC)" CFLAGS="-O -DSEG" CAS="$(CAS)" AS="$(AS)" slibc.a scrt0.o

install: $(ALL_TARGETS)
	cp build/nonseg/libc.a $(PREFIX)/lib/libc.a
	cp build/nonseg/crt0.o $(PREFIX)/lib/crt0.o
	cp build/seg/slibc.a $(PREFIX)/lib/slibc.a
	cp build/seg/scrt0.o $(PREFIX)/lib/scrt0.o

clean:
	for d in build/nonseg build/seg ; do rm -f $$d/*.o $$d/*.a $$d/Library.SC ; done

FORCE:

# the members in the order of the original archives
LIBC_MEMBERS =arhdr.o getgrgid.o getgrnam.o getgrent.o cuserid.o getpass.o getpwnam.o getpwuid.o putpwent.o getpwent.o \
	ctermid.o getopt.o fgetc.o fputc.o getchar.o putchar.o popen.o fltioc.o freopen.o fgets.o fputs.o getpw.o fseek.o \
	ftell.o rew.o symscan.o readsym.o rdwr.o system.o tmpfile.o tmpnam.o fopen.o fdopen.o scanf.o doscan.o printf.o \
	doprnt.o stfl.o ffltioc.o gets.o getw.o puts.o putw.o ungetc.o atof.o ecvt.o pow10.o filbuf.o setbuf.o strout.o \
	flsbuf.o endopen.o findiop.o clrerr.o data.o __callex.o mcount.o cuexit.o execlp.o execvp.o ctime.o getenv.o \
	getlogin.o perror.o sleep.o ttyslot.o ttyname.o abort.o abs.o atoi.o atol.o bsearch.o cnvt.o decimal.o length.o \
	crypt.o malloc.o errlst.o fakcu.o isatty.o l3.o lsearch.o mktemp.o mon.o mpx.o nlist.o qsort.o rand.o setret.o \
	ssignal.o stty.o swab.o tell.o ctype_.o index.o rindex.o toupper.o tolower.o strcat.o strncat.o strcmp.o strncmp.o \
	strcpy.o strncpy.o strlen.o strtok.o strchr.o strcspn.o strpbrk.o strrchr.o strspn.o goodmagic.o swap.o a64l.o l64a.o \
	access.o acct.o alarm.o brk.o chdir.o chroot.o chmod.o chown.o close.o creat.o dup.o dup2.o execl.o execle.o execv.o \
	execve.o exit.o fcntl.o fork.o fstat.o ftime.o getgid.o getegid.o getpgrp.o getpid.o getppid.o getuid.o geteuid.o \
	ioctl.o mdmctl.o kill.o lkdata.o link.o lock.o lseek.o mknod.o mount.o nice.o open.o pause.o pipe.o profil.o ptrace.o \
	read.o sptrace.o setgid.o setpgrp.o setuid.o signal.o stat.o stime.o sync.o time.o times.o umask.o umount.o uname.o \
	ulimit.o unlink.o unlk.o ustat.o utime.o wait.o write.o cerror.o
SLIBC_MEMBERS =arhdr.o getgrgid.o getopt.o segmon.o ctermid.o cuserid.o getgrnam.o getgrent.o getpass.o getpwnam.o \
	getpwuid.o putpwent.o getpwent.o fgetc.o fputc.o getchar.o putchar.o popen.o fltioc.o freopen.o fgets.o fputs.o \
	getpw.o fseek.o ftell.o rew.o symscan.o readsym.o rdwr.o system.o tmpfile.o tmpnam.o fopen.o fdopen.o scanf.o \
	doscan.o printf.o doprnt.o stfl.o ffltioc.o gets.o getw.o puts.o putw.o ungetc.o atof.o ecvt.o pow10.o filbuf.o \
	setbuf.o strout.o flsbuf.o endopen.o findiop.o clrerr.o data.o __callex.o mcount.o cuexit.o execlp.o execvp.o ctime.o \
	getenv.o getlogin.o perror.o sleep.o ttyslot.o ttyname.o abort.o abs.o atoi.o atol.o bsearch.o cnvt.o decimal.o \
	length.o crypt.o malloc.o errlst.o fakcu.o isatty.o l3.o lsearch.o mktemp.o mpx.o nlist.o qsort.o rand.o setret.o \
	ssignal.o stty.o strtok.o strchr.o strcspn.o strpbrk.o strrchr.o strspn.o swab.o tell.o ctype_.o index.o rindex.o \
	strcat.o strncat.o strcmp.o strncmp.o strcpy.o strncpy.o strlen.o goodmagic.o swap.o a64l.o l64a.o access.o tolower.o \
	toupper.o acct.o alarm.o chdir.o chroot.o chmod.o chown.o close.o creat.o dup.o dup2.o execl.o execle.o execv.o \
	execve.o exit.o fcntl.o fork.o fstat.o ftime.o getgid.o getegid.o getpgrp.o getpid.o getppid.o getuid.o geteuid.o \
	ioctl.o mdmctl.o kill.o link.o lkdata.o lock.o lseek.o mknod.o mount.o mkseg.o nice.o open.o pause.o pipe.o profil.o \
	ptrace.o read.o setgid.o setpgrp.o setuid.o sgbrk.o sptrace.o sgstat.o ssgbrk.o signal.o stat.o stime.o sync.o time.o \
	times.o umask.o umount.o ulimit.o uname.o unlink.o unlk.o ustat.o utime.o wait.o write.o chkseg.o sprofil.o cerror.o

##################
# the archives and the start-up files (made in build/V)
##################
libc.a: $(LIBC_MEMBERS) Library.SC ../../mkar.sh
	rm -f libc.a
	sh ../../mkar.sh libc.a $(LIBC_MEMBERS) Library.SC

slibc.a: $(SLIBC_MEMBERS) Library.SC ../../mkar.sh
	rm -f slibc.a
	sh ../../mkar.sh slibc.a $(SLIBC_MEMBERS) Library.SC

Library.SC: ../../$(V)/Library.SC
	cp ../../$(V)/Library.SC Library.SC

arhdr.o: ../../$(V)/arhdr.s
	$(CAS) -u -o arhdr.o ../../$(V)/arhdr.s

crt0.o: ../../nonseg/csu/crt0.s
	$(AS) -o crt0.o ../../nonseg/csu/crt0.s

scrt0.o: ../../seg/csu/scrt0.s
	$(AS) -o scrt0.o ../../seg/csu/scrt0.s

##################
# the members
##################
__callex.o: ../../common/gen/__callex.c
	$(CC) $(CFLAGS) -c ../../common/gen/__callex.c
a64l.o: ../../common/gen/a64l.c
	$(CC) $(CFLAGS) -c ../../common/gen/a64l.c
abort.o: ../../$(V)/gen/abort.s
	$(AS) -o abort.o ../../$(V)/gen/abort.s
abs.o: ../../common/gen/abs.c
	$(CC) $(CFLAGS) -c ../../common/gen/abs.c
access.o: ../../$(V)/sys/access.s
	$(AS) -o access.o ../../$(V)/sys/access.s
acct.o: ../../$(V)/sys/acct.s
	$(AS) -o acct.o ../../$(V)/sys/acct.s
alarm.o: ../../$(V)/sys/alarm.s
	$(AS) -o alarm.o ../../$(V)/sys/alarm.s
atof.o: ../../$(V)/gen/atof.s
	$(AS) -f -o atof.o ../../$(V)/gen/atof.s
atoi.o: ../../common/gen/atoi.c
	$(CC) $(CFLAGS) -c ../../common/gen/atoi.c
atol.o: ../../common/gen/atol.c
	$(CC) $(CFLAGS) -c ../../common/gen/atol.c
brk.o: ../../nonseg/sys/brk.s
	$(AS) -o brk.o ../../nonseg/sys/brk.s
bsearch.o: ../../common/gen/bsearch.c
	$(CC) $(CFLAGS) -c ../../common/gen/bsearch.c
cerror.o: ../../$(V)/crt/cerror.s
	$(AS) -o cerror.o ../../$(V)/crt/cerror.s
chdir.o: ../../$(V)/sys/chdir.s
	$(AS) -o chdir.o ../../$(V)/sys/chdir.s
chkseg.o: ../../seg/sys/chkseg.s
	$(AS) -o chkseg.o ../../seg/sys/chkseg.s
chmod.o: ../../$(V)/sys/chmod.s
	$(AS) -o chmod.o ../../$(V)/sys/chmod.s
chown.o: ../../$(V)/sys/chown.s
	$(AS) -o chown.o ../../$(V)/sys/chown.s
chroot.o: ../../$(V)/sys/chroot.s
	$(AS) -o chroot.o ../../$(V)/sys/chroot.s
close.o: ../../$(V)/sys/close.s
	$(AS) -o close.o ../../$(V)/sys/close.s
clrerr.o: ../../common/stdio/clrerr.c
	$(CC) $(CFLAGS) -c ../../common/stdio/clrerr.c
cnvt.o: ../../common/gen/cnvt.c
	$(CC) $(CFLAGS) -c ../../common/gen/cnvt.c
creat.o: ../../$(V)/sys/creat.s
	$(AS) -o creat.o ../../$(V)/sys/creat.s
crypt.o: ../../common/gen/crypt.c
	$(CC) $(CFLAGS) -c ../../common/gen/crypt.c
ctermid.o: ../../common/stdio/ctermid.c
	$(CC) $(CFLAGS) -c ../../common/stdio/ctermid.c
ctime.o: ../../common/gen/ctime.c
	$(CC) $(CFLAGS) -c ../../common/gen/ctime.c
ctype_.o: ../../common/gen/ctype_.c
	$(CC) $(CFLAGS) -c ../../common/gen/ctype_.c
cuexit.o: ../../$(V)/gen/cuexit.s
	$(AS) -o cuexit.o ../../$(V)/gen/cuexit.s
cuserid.o: ../../common/stdio/cuserid.c
	$(CC) $(CFLAGS) -c ../../common/stdio/cuserid.c
data.o: ../../common/stdio/data.c
	$(CC) $(CFLAGS) -c ../../common/stdio/data.c
decimal.o: ../../$(V)/gen/decimal.s
	$(AS) -o decimal.o ../../$(V)/gen/decimal.s
doprnt.o: ../../common/stdio/doprnt.c
	$(CC) $(CFLAGS) -c ../../common/stdio/doprnt.c
doscan.o: ../../common/stdio/doscan.c
	$(CC) $(CFLAGS) -c ../../common/stdio/doscan.c
dup.o: ../../$(V)/sys/dup.s
	$(AS) -o dup.o ../../$(V)/sys/dup.s
dup2.o: ../../$(V)/sys/dup2.s
	$(AS) -o dup2.o ../../$(V)/sys/dup2.s
ecvt.o: ../../$(V)/gen/ecvt.s
	$(AS) -f -o ecvt.o ../../$(V)/gen/ecvt.s
endopen.o: ../../common/stdio/endopen.c
	$(CC) $(CFLAGS) -c ../../common/stdio/endopen.c
errlst.o: ../../common/gen/errlst.c
	$(CC) $(CFLAGS) -c ../../common/gen/errlst.c
execl.o: ../../$(V)/sys/execl.s
	$(AS) -o execl.o ../../$(V)/sys/execl.s
execle.o: ../../$(V)/sys/execle.s
	$(AS) -o execle.o ../../$(V)/sys/execle.s
execlp.o: ../../$(V)/sys/execlp.s
	$(AS) -o execlp.o ../../$(V)/sys/execlp.s
execv.o: ../../$(V)/sys/execv.s
	$(AS) -o execv.o ../../$(V)/sys/execv.s
execve.o: ../../$(V)/sys/execve.s
	$(AS) -o execve.o ../../$(V)/sys/execve.s
execvp.o: ../../common/gen/execvp.c
	$(CC) $(CFLAGS) -c ../../common/gen/execvp.c
exit.o: ../../$(V)/sys/exit.s
	$(AS) -o exit.o ../../$(V)/sys/exit.s
fakcu.o: ../../$(V)/gen/fakcu.s
	$(AS) -o fakcu.o ../../$(V)/gen/fakcu.s
fcntl.o: ../../$(V)/sys/fcntl.s
	$(AS) -o fcntl.o ../../$(V)/sys/fcntl.s
fdopen.o: ../../common/stdio/fdopen.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fdopen.c
ffltioc.o: ../../common/stdio/ffltioc.c
	$(CC) $(CFLAGS) -c ../../common/stdio/ffltioc.c
fgetc.o: ../../common/stdio/fgetc.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fgetc.c
fgets.o: ../../common/stdio/fgets.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fgets.c
filbuf.o: ../../common/stdio/filbuf.c
	$(CC) $(CFLAGS) -c ../../common/stdio/filbuf.c
findiop.o: ../../common/stdio/findiop.c
	$(CC) $(CFLAGS) -c ../../common/stdio/findiop.c
flsbuf.o: ../../common/stdio/flsbuf.c
	$(CC) $(CFLAGS) -c ../../common/stdio/flsbuf.c
fltioc.o: ../../common/stdio/fltioc.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fltioc.c
fopen.o: ../../common/stdio/fopen.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fopen.c
fork.o: ../../$(V)/sys/fork.s
	$(AS) -o fork.o ../../$(V)/sys/fork.s
fputc.o: ../../common/stdio/fputc.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fputc.c
fputs.o: ../../common/stdio/fputs.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fputs.c
freopen.o: ../../common/stdio/freopen.c
	$(CC) $(CFLAGS) -c ../../common/stdio/freopen.c
fseek.o: ../../common/stdio/fseek.c
	$(CC) $(CFLAGS) -c ../../common/stdio/fseek.c
fstat.o: ../../$(V)/sys/fstat.s
	$(AS) -o fstat.o ../../$(V)/sys/fstat.s
ftell.o: ../../common/stdio/ftell.c
	$(CC) $(CFLAGS) -c ../../common/stdio/ftell.c
ftime.o: ../../$(V)/sys/ftime.s
	$(AS) -o ftime.o ../../$(V)/sys/ftime.s
getchar.o: ../../common/stdio/getchar.c
	$(CC) $(CFLAGS) -c ../../common/stdio/getchar.c
getegid.o: ../../$(V)/sys/getegid.s
	$(AS) -o getegid.o ../../$(V)/sys/getegid.s
getenv.o: ../../common/gen/getenv.c
	$(CC) $(CFLAGS) -c ../../common/gen/getenv.c
geteuid.o: ../../$(V)/sys/geteuid.s
	$(AS) -o geteuid.o ../../$(V)/sys/geteuid.s
getgid.o: ../../$(V)/sys/getgid.s
	$(AS) -o getgid.o ../../$(V)/sys/getgid.s
getgrent.o: ../../common/gen/getgrent.c
	$(CC) $(CFLAGS) -c ../../common/gen/getgrent.c
getgrgid.o: ../../common/gen/getgrgid.c
	$(CC) $(CFLAGS) -c ../../common/gen/getgrgid.c
getgrnam.o: ../../common/gen/getgrnam.c
	$(CC) $(CFLAGS) -c ../../common/gen/getgrnam.c
getlogin.o: ../../common/gen/getlogin.c
	$(CC) $(CFLAGS) -c ../../common/gen/getlogin.c
getopt.o: ../../common/gen/getopt.c
	$(CC) $(CFLAGS) -c ../../common/gen/getopt.c
getpass.o: ../../common/stdio/getpass.c
	$(CC) $(CFLAGS) -c ../../common/stdio/getpass.c
getpgrp.o: ../../$(V)/sys/getpgrp.s
	$(AS) -o getpgrp.o ../../$(V)/sys/getpgrp.s
getpid.o: ../../$(V)/sys/getpid.s
	$(AS) -o getpid.o ../../$(V)/sys/getpid.s
getppid.o: ../../$(V)/sys/getppid.s
	$(AS) -o getppid.o ../../$(V)/sys/getppid.s
getpw.o: ../../common/gen/getpw.c
	$(CC) $(CFLAGS) -c ../../common/gen/getpw.c
getpwent.o: ../../common/gen/getpwent.c
	$(CC) $(CFLAGS) -c ../../common/gen/getpwent.c
getpwnam.o: ../../common/gen/getpwnam.c
	$(CC) $(CFLAGS) -c ../../common/gen/getpwnam.c
getpwuid.o: ../../common/gen/getpwuid.c
	$(CC) $(CFLAGS) -c ../../common/gen/getpwuid.c
gets.o: ../../common/stdio/gets.c
	$(CC) $(CFLAGS) -c ../../common/stdio/gets.c
getuid.o: ../../$(V)/sys/getuid.s
	$(AS) -o getuid.o ../../$(V)/sys/getuid.s
getw.o: ../../common/stdio/getw.c
	$(CC) $(CFLAGS) -c ../../common/stdio/getw.c
goodmagic.o: ../../common/gen/goodmagic.c
	$(CC) $(CFLAGS) -c ../../common/gen/goodmagic.c
index.o: ../../common/gen/index.c
	$(CC) $(CFLAGS) -c ../../common/gen/index.c
ioctl.o: ../../$(V)/sys/ioctl.s
	$(AS) -o ioctl.o ../../$(V)/sys/ioctl.s
isatty.o: ../../common/gen/isatty.c
	$(CC) $(CFLAGS) -c ../../common/gen/isatty.c
kill.o: ../../$(V)/sys/kill.s
	$(AS) -o kill.o ../../$(V)/sys/kill.s
l3.o: ../../common/gen/l3.c
	$(CC) $(CFLAGS) -c ../../common/gen/l3.c
l64a.o: ../../common/gen/l64a.c
	$(CC) $(CFLAGS) -c ../../common/gen/l64a.c
length.o: ../../common/gen/length.c
	$(CC) $(CFLAGS) -c ../../common/gen/length.c
link.o: ../../$(V)/sys/link.s
	$(AS) -o link.o ../../$(V)/sys/link.s
lkdata.o: ../../$(V)/sys/lkdata.s
	$(AS) -o lkdata.o ../../$(V)/sys/lkdata.s
lock.o: ../../$(V)/sys/lock.s
	$(AS) -o lock.o ../../$(V)/sys/lock.s
lsearch.o: ../../common/gen/lsearch.c
	$(CC) $(CFLAGS) -c ../../common/gen/lsearch.c
lseek.o: ../../$(V)/sys/lseek.s
	$(AS) -o lseek.o ../../$(V)/sys/lseek.s
malloc.o: ../../common/gen/malloc.c
	$(CC) $(CFLAGS) -c ../../common/gen/malloc.c
mcount.o: ../../$(V)/crt/mcount.s
	$(AS) -o mcount.o ../../$(V)/crt/mcount.s
mdmctl.o: ../../$(V)/sys/mdmctl.s
	$(AS) -o mdmctl.o ../../$(V)/sys/mdmctl.s
mknod.o: ../../$(V)/sys/mknod.s
	$(AS) -o mknod.o ../../$(V)/sys/mknod.s
mkseg.o: ../../seg/sys/mkseg.s
	$(AS) -o mkseg.o ../../seg/sys/mkseg.s
mktemp.o: ../../common/gen/mktemp.c
	$(CC) $(CFLAGS) -c ../../common/gen/mktemp.c
mon.o: ../../nonseg/gen/mon.c
	$(CC) $(CFLAGS) -c ../../nonseg/gen/mon.c
mount.o: ../../$(V)/sys/mount.s
	$(AS) -o mount.o ../../$(V)/sys/mount.s
mpx.o: ../../common/gen/mpx.c
	$(CC) $(CFLAGS) -c ../../common/gen/mpx.c
nice.o: ../../$(V)/sys/nice.s
	$(AS) -o nice.o ../../$(V)/sys/nice.s
nlist.o: ../../common/gen/nlist.c
	$(CC) $(CFLAGS) -c ../../common/gen/nlist.c
open.o: ../../$(V)/sys/open.s
	$(AS) -o open.o ../../$(V)/sys/open.s
pause.o: ../../$(V)/sys/pause.s
	$(AS) -o pause.o ../../$(V)/sys/pause.s
perror.o: ../../common/gen/perror.c
	$(CC) $(CFLAGS) -c ../../common/gen/perror.c
pipe.o: ../../$(V)/sys/pipe.s
	$(AS) -o pipe.o ../../$(V)/sys/pipe.s
popen.o: ../../common/stdio/popen.c
	$(CC) $(CFLAGS) -c ../../common/stdio/popen.c
pow10.o: ../../$(V)/gen/pow10.s
	$(AS) -f -o pow10.o ../../$(V)/gen/pow10.s
printf.o: ../../common/stdio/printf.c
	$(CC) $(CFLAGS) -I../../../../head/oldv7 -c ../../common/stdio/printf.c
profil.o: ../../$(V)/sys/profil.s
	$(AS) -o profil.o ../../$(V)/sys/profil.s
ptrace.o: ../../$(V)/sys/ptrace.s
	$(AS) -o ptrace.o ../../$(V)/sys/ptrace.s
putchar.o: ../../common/stdio/putchar.c
	$(CC) $(CFLAGS) -c ../../common/stdio/putchar.c
putpwent.o: ../../common/gen/putpwent.c
	$(CC) $(CFLAGS) -c ../../common/gen/putpwent.c
puts.o: ../../common/stdio/puts.c
	$(CC) $(CFLAGS) -c ../../common/stdio/puts.c
putw.o: ../../common/stdio/putw.c
	$(CC) $(CFLAGS) -c ../../common/stdio/putw.c
qsort.o: ../../common/gen/qsort.c
	$(CC) $(CFLAGS) -c ../../common/gen/qsort.c
rand.o: ../../common/gen/rand.c
	$(CC) $(CFLAGS) -c ../../common/gen/rand.c
rdwr.o: ../../common/stdio/rdwr.c
	$(CC) $(CFLAGS) -c ../../common/stdio/rdwr.c
read.o: ../../$(V)/sys/read.s
	$(AS) -o read.o ../../$(V)/sys/read.s
readsym.o: ../../common/gen/readsym.c
	$(CC) $(CFLAGS) -c ../../common/gen/readsym.c
rew.o: ../../common/stdio/rew.c
	$(CC) $(CFLAGS) -c ../../common/stdio/rew.c
rindex.o: ../../common/gen/rindex.c
	$(CC) $(CFLAGS) -c ../../common/gen/rindex.c
scanf.o: ../../common/stdio/scanf.c
	$(CC) $(CFLAGS) -c ../../common/stdio/scanf.c
segmon.o: ../../seg/gen/segmon.c
	$(CC) $(CFLAGS) -c ../../seg/gen/segmon.c
setbuf.o: ../../common/stdio/setbuf.c
	$(CC) $(CFLAGS) -c ../../common/stdio/setbuf.c
setgid.o: ../../$(V)/sys/setgid.s
	$(AS) -o setgid.o ../../$(V)/sys/setgid.s
setpgrp.o: ../../$(V)/sys/setpgrp.s
	$(AS) -o setpgrp.o ../../$(V)/sys/setpgrp.s
setret.o: ../../$(V)/gen/setret.s
	$(AS) -o setret.o ../../$(V)/gen/setret.s
setuid.o: ../../$(V)/sys/setuid.s
	$(AS) -o setuid.o ../../$(V)/sys/setuid.s
sgbrk.o: ../../seg/sys/sgbrk.s
	$(AS) -o sgbrk.o ../../seg/sys/sgbrk.s
sgstat.o: ../../seg/sys/sgstat.s
	$(AS) -o sgstat.o ../../seg/sys/sgstat.s
signal.o: ../../$(V)/sys/signal.s
	$(AS) -o signal.o ../../$(V)/sys/signal.s
sleep.o: ../../common/gen/sleep.c
	$(CC) $(CFLAGS) -c ../../common/gen/sleep.c
sprofil.o: ../../seg/sys/sprofil.s
	$(AS) -o sprofil.o ../../seg/sys/sprofil.s
sptrace.o: ../../$(V)/sys/sptrace.s
	$(AS) -o sptrace.o ../../$(V)/sys/sptrace.s
ssgbrk.o: ../../seg/sys/ssgbrk.s
	$(AS) -o ssgbrk.o ../../seg/sys/ssgbrk.s
ssignal.o: ../../common/gen/ssignal.c
	$(CC) $(CFLAGS) -c ../../common/gen/ssignal.c
stat.o: ../../$(V)/sys/stat.s
	$(AS) -o stat.o ../../$(V)/sys/stat.s
stfl.o: ../../$(V)/gen/stfl.s
	$(CAS) -o stfl.o ../../$(V)/gen/stfl.s
stime.o: ../../$(V)/sys/stime.s
	$(AS) -o stime.o ../../$(V)/sys/stime.s
strcat.o: ../../common/gen/strcat.c
	$(CC) $(CFLAGS) -c ../../common/gen/strcat.c
strchr.o: ../../common/gen/strchr.c
	$(CC) $(CFLAGS) -c ../../common/gen/strchr.c
strcmp.o: ../../common/gen/strcmp.c
	$(CC) $(CFLAGS) -c ../../common/gen/strcmp.c
strcpy.o: ../../common/gen/strcpy.c
	$(CC) $(CFLAGS) -c ../../common/gen/strcpy.c
strcspn.o: ../../common/gen/strcspn.c
	$(CC) $(CFLAGS) -c ../../common/gen/strcspn.c
strlen.o: ../../common/gen/strlen.c
	$(CC) $(CFLAGS) -c ../../common/gen/strlen.c
strncat.o: ../../common/gen/strncat.c
	$(CC) $(CFLAGS) -c ../../common/gen/strncat.c
strncmp.o: ../../common/gen/strncmp.c
	$(CC) $(CFLAGS) -c ../../common/gen/strncmp.c
strncpy.o: ../../common/gen/strncpy.c
	$(CC) $(CFLAGS) -c ../../common/gen/strncpy.c
strout.o: ../../common/stdio/strout.c
	$(CC) $(CFLAGS) -c ../../common/stdio/strout.c
strpbrk.o: ../../common/gen/strpbrk.c
	$(CC) $(CFLAGS) -c ../../common/gen/strpbrk.c
strrchr.o: ../../common/gen/strrchr.c
	$(CC) $(CFLAGS) -c ../../common/gen/strrchr.c
strspn.o: ../../common/gen/strspn.c
	$(CC) $(CFLAGS) -c ../../common/gen/strspn.c
strtok.o: ../../common/gen/strtok.c
	$(CC) $(CFLAGS) -c ../../common/gen/strtok.c
stty.o: ../../common/gen/stty.c
	$(CC) $(CFLAGS) -c ../../common/gen/stty.c
swab.o: ../../common/gen/swab.c
	$(CC) $(CFLAGS) -c ../../common/gen/swab.c
swap.o: ../../common/gen/swap.c
	$(CC) $(CFLAGS) -c ../../common/gen/swap.c
symscan.o: ../../common/gen/symscan.c
	$(CC) $(CFLAGS) -c ../../common/gen/symscan.c
sync.o: ../../$(V)/sys/sync.s
	$(AS) -o sync.o ../../$(V)/sys/sync.s
system.o: ../../common/stdio/system.c
	$(CC) $(CFLAGS) -c ../../common/stdio/system.c
tell.o: ../../common/gen/tell.c
	$(CC) $(CFLAGS) -c ../../common/gen/tell.c
time.o: ../../$(V)/sys/time.s
	$(AS) -o time.o ../../$(V)/sys/time.s
times.o: ../../$(V)/sys/times.s
	$(AS) -o times.o ../../$(V)/sys/times.s
tmpfile.o: ../../common/stdio/tmpfile.c
	$(CC) $(CFLAGS) -c ../../common/stdio/tmpfile.c
tmpnam.o: ../../common/stdio/tmpnam.c
	$(CC) $(CFLAGS) -c ../../common/stdio/tmpnam.c
tolower.o: ../../common/gen/tolower.c
	$(CC) $(CFLAGS) -c ../../common/gen/tolower.c
toupper.o: ../../common/gen/toupper.c
	$(CC) $(CFLAGS) -c ../../common/gen/toupper.c
ttyname.o: ../../common/gen/ttyname.c
	$(CC) $(CFLAGS) -c ../../common/gen/ttyname.c
ttyslot.o: ../../common/gen/ttyslot.c
	$(CC) $(CFLAGS) -c ../../common/gen/ttyslot.c
ulimit.o: ../../$(V)/sys/ulimit.s
	$(AS) -o ulimit.o ../../$(V)/sys/ulimit.s
umask.o: ../../$(V)/sys/umask.s
	$(AS) -o umask.o ../../$(V)/sys/umask.s
umount.o: ../../$(V)/sys/umount.s
	$(AS) -o umount.o ../../$(V)/sys/umount.s
uname.o: ../../$(V)/sys/uname.s
	$(AS) -o uname.o ../../$(V)/sys/uname.s
ungetc.o: ../../common/stdio/ungetc.c
	$(CC) $(CFLAGS) -c ../../common/stdio/ungetc.c
unlink.o: ../../$(V)/sys/unlink.s
	$(AS) -o unlink.o ../../$(V)/sys/unlink.s
unlk.o: ../../$(V)/sys/unlk.s
	$(AS) -o unlk.o ../../$(V)/sys/unlk.s
ustat.o: ../../$(V)/sys/ustat.s
	$(AS) -o ustat.o ../../$(V)/sys/ustat.s
utime.o: ../../$(V)/sys/utime.s
	$(AS) -o utime.o ../../$(V)/sys/utime.s
wait.o: ../../$(V)/sys/wait.s
	$(AS) -o wait.o ../../$(V)/sys/wait.s
write.o: ../../$(V)/sys/write.s
	$(AS) -o write.o ../../$(V)/sys/write.s
