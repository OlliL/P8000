# libc: /lib/libc.a from its sources, with the shipped tools. Each object is identical with the member of the original (host/libc/check.sh).
# The members of oldcc/ were made by an older cc that we do not have yet.
#
#   make        libc.a and obj/*.o, the members for ../oldlibc
#   make clean

CC = cc
CAS = cas
AS = as
AR = ar

OBJS = obj/getgrgid.o obj/getgrnam.o obj/getgrent.o obj/cuserid.o obj/getpass.o obj/getpwnam.o obj/getpwuid.o \
	 obj/putpwent.o obj/getpwent.o obj/ctermid.o obj/getopt.o obj/fgetc.o obj/fputc.o obj/getchar.o \
	 obj/putchar.o obj/popen.o obj/fltioc.o obj/freopen.o obj/fgets.o obj/fputs.o obj/getpw.o obj/fseek.o \
	 obj/ftell.o obj/rew.o obj/symscan.o obj/readsym.o obj/rdwr.o obj/system.o obj/tmpfile.o obj/tmpnam.o \
	 obj/fopen.o obj/fdopen.o obj/scanf.o obj/doscan.o obj/printf.o obj/doprnt.o obj/stfl.o obj/ffltioc.o \
	 obj/gets.o obj/getw.o obj/puts.o obj/putw.o obj/ungetc.o obj/atof.o obj/ecvt.o obj/pow10.o obj/filbuf.o \
	 obj/setbuf.o obj/strout.o obj/flsbuf.o obj/endopen.o obj/findiop.o obj/clrerr.o obj/data.o obj/__callex.o \
	 obj/mcount.o obj/cuexit.o obj/execlp.o obj/execvp.o obj/ctime.o obj/getenv.o obj/getlogin.o obj/perror.o \
	 obj/sleep.o obj/ttyslot.o obj/ttyname.o obj/abort.o obj/abs.o obj/atoi.o obj/atol.o obj/bsearch.o \
	 obj/cnvt.o obj/decimal.o obj/length.o obj/crypt.o obj/malloc.o obj/errlst.o obj/fakcu.o obj/isatty.o \
	 obj/l3.o obj/lsearch.o obj/mktemp.o obj/mon.o obj/mpx.o obj/nlist.o obj/qsort.o obj/rand.o obj/setret.o \
	 obj/ssignal.o obj/stty.o obj/swab.o obj/tell.o obj/ctype_.o obj/index.o obj/rindex.o obj/toupper.o \
	 obj/tolower.o obj/strcat.o obj/strncat.o obj/strcmp.o obj/strncmp.o obj/strcpy.o obj/strncpy.o \
	 obj/strlen.o obj/strtok.o obj/strchr.o obj/strcspn.o obj/strpbrk.o obj/strrchr.o obj/strspn.o \
	 obj/goodmagic.o obj/swap.o obj/a64l.o obj/l64a.o obj/access.o obj/acct.o obj/alarm.o obj/brk.o \
	 obj/chdir.o obj/chroot.o obj/chmod.o obj/chown.o obj/close.o obj/creat.o obj/dup.o obj/dup2.o obj/execl.o \
	 obj/execle.o obj/execv.o obj/execve.o obj/exit.o obj/fcntl.o obj/fork.o obj/fstat.o obj/ftime.o \
	 obj/getgid.o obj/getegid.o obj/getpgrp.o obj/getpid.o obj/getppid.o obj/getuid.o obj/geteuid.o \
	 obj/ioctl.o obj/mdmctl.o obj/kill.o obj/lkdata.o obj/link.o obj/lock.o obj/lseek.o obj/mknod.o \
	 obj/mount.o obj/nice.o obj/open.o obj/pause.o obj/pipe.o obj/profil.o obj/ptrace.o obj/read.o \
	 obj/sptrace.o obj/setgid.o obj/setpgrp.o obj/setuid.o obj/signal.o obj/stat.o obj/stime.o obj/sync.o \
	 obj/time.o obj/times.o obj/umask.o obj/umount.o obj/uname.o obj/ulimit.o obj/unlink.o obj/unlk.o \
	 obj/ustat.o obj/utime.o obj/wait.o obj/write.o obj/cerror.o

all: obj libc.a

obj:
	-mkdir obj

libc.a: obj $(OBJS)
	rm -f libc.a
	cp Library.SC obj/Library.SC
	cd obj ; $(AR) r ../libc.a Library.SC getgrgid.o getgrnam.o getgrent.o cuserid.o getpass.o getpwnam.o getpwuid.o putpwent.o getpwent.o ctermid.o getopt.o fgetc.o fputc.o getchar.o putchar.o popen.o fltioc.o freopen.o fgets.o fputs.o getpw.o fseek.o ftell.o rew.o symscan.o readsym.o rdwr.o system.o tmpfile.o tmpnam.o fopen.o fdopen.o scanf.o doscan.o printf.o doprnt.o stfl.o ffltioc.o gets.o getw.o puts.o putw.o ungetc.o atof.o ecvt.o pow10.o filbuf.o setbuf.o strout.o flsbuf.o endopen.o findiop.o clrerr.o data.o __callex.o mcount.o cuexit.o execlp.o execvp.o ctime.o getenv.o getlogin.o perror.o sleep.o ttyslot.o ttyname.o abort.o abs.o atoi.o atol.o bsearch.o cnvt.o decimal.o length.o crypt.o malloc.o errlst.o fakcu.o isatty.o l3.o lsearch.o mktemp.o mon.o mpx.o nlist.o qsort.o rand.o setret.o ssignal.o stty.o swab.o tell.o ctype_.o index.o rindex.o toupper.o tolower.o strcat.o strncat.o strcmp.o strncmp.o strcpy.o strncpy.o strlen.o strtok.o strchr.o strcspn.o strpbrk.o strrchr.o strspn.o goodmagic.o swap.o a64l.o l64a.o access.o acct.o alarm.o brk.o chdir.o chroot.o chmod.o chown.o close.o creat.o dup.o dup2.o execl.o execle.o execv.o execve.o exit.o fcntl.o fork.o fstat.o ftime.o getgid.o getegid.o getpgrp.o getpid.o getppid.o getuid.o geteuid.o ioctl.o mdmctl.o kill.o lkdata.o link.o lock.o lseek.o mknod.o mount.o nice.o open.o pause.o pipe.o profil.o ptrace.o read.o sptrace.o setgid.o setpgrp.o setuid.o signal.o stat.o stime.o sync.o time.o times.o umask.o umount.o uname.o ulimit.o unlink.o unlk.o ustat.o utime.o wait.o write.o cerror.o

obj/getgrgid.o: gen/getgrgid.c
	cd gen ; $(CC) -O -c getgrgid.c ; mv getgrgid.o ../obj/getgrgid.o

obj/getgrnam.o: gen/getgrnam.c
	cd gen ; $(CC) -O -c getgrnam.c ; mv getgrnam.o ../obj/getgrnam.o

obj/getgrent.o: gen/getgrent.c
	cd gen ; $(CC) -O -c getgrent.c ; mv getgrent.o ../obj/getgrent.o

obj/cuserid.o: stdio/cuserid.c
	cd stdio ; $(CC) -O -c cuserid.c ; mv cuserid.o ../obj/cuserid.o

obj/getpass.o: stdio/getpass.c
	cd stdio ; $(CC) -O -c getpass.c ; mv getpass.o ../obj/getpass.o

obj/getpwnam.o: gen/getpwnam.c
	cd gen ; $(CC) -O -c getpwnam.c ; mv getpwnam.o ../obj/getpwnam.o

obj/getpwuid.o: gen/getpwuid.c
	cd gen ; $(CC) -O -c getpwuid.c ; mv getpwuid.o ../obj/getpwuid.o

obj/putpwent.o: gen/putpwent.c
	cd gen ; $(CC) -O -c putpwent.c ; mv putpwent.o ../obj/putpwent.o

obj/getpwent.o: gen/getpwent.c
	cd gen ; $(CC) -O -c getpwent.c ; mv getpwent.o ../obj/getpwent.o

obj/ctermid.o: stdio/ctermid.c
	cd stdio ; $(CC) -O -c ctermid.c ; mv ctermid.o ../obj/ctermid.o

obj/getopt.o: gen/getopt.c
	cd gen ; $(CC) -O -c getopt.c ; mv getopt.o ../obj/getopt.o

obj/fgetc.o: stdio/fgetc.c
	cd stdio ; $(CC) -O -c fgetc.c ; mv fgetc.o ../obj/fgetc.o

obj/fputc.o: stdio/fputc.c
	cd stdio ; $(CC) -O -c fputc.c ; mv fputc.o ../obj/fputc.o

obj/getchar.o: stdio/getchar.c
	cd stdio ; $(CC) -O -c getchar.c ; mv getchar.o ../obj/getchar.o

obj/putchar.o: stdio/putchar.c
	cd stdio ; $(CC) -O -c putchar.c ; mv putchar.o ../obj/putchar.o

obj/popen.o: stdio/popen.c
	cd stdio ; $(CC) -O -c popen.c ; mv popen.o ../obj/popen.o

obj/fltioc.o: stdio/fltioc.c
	cd stdio ; $(CC) -O -c fltioc.c ; mv fltioc.o ../obj/fltioc.o

obj/freopen.o: stdio/freopen.c
	cd stdio ; $(CC) -O -c freopen.c ; mv freopen.o ../obj/freopen.o

obj/fgets.o: stdio/fgets.c
	cd stdio ; $(CC) -O -c fgets.c ; mv fgets.o ../obj/fgets.o

obj/fputs.o: stdio/fputs.c
	cd stdio ; $(CC) -O -c fputs.c ; mv fputs.o ../obj/fputs.o

obj/getpw.o: gen/getpw.c
	cd gen ; $(CC) -O -c getpw.c ; mv getpw.o ../obj/getpw.o

obj/fseek.o: stdio/fseek.c
	cd stdio ; $(CC) -O -c fseek.c ; mv fseek.o ../obj/fseek.o

obj/ftell.o: stdio/ftell.c
	cd stdio ; $(CC) -O -c ftell.c ; mv ftell.o ../obj/ftell.o

obj/rew.o: stdio/rew.c
	cd stdio ; $(CC) -O -c rew.c ; mv rew.o ../obj/rew.o

obj/symscan.o: oldcc/symscan/0_1.s
	cd oldcc/symscan ; $(CAS) -u -o ../../obj/symscan.o 0_1.s

obj/readsym.o: gen/readsym.c
	cd gen ; $(CC) -O -c readsym.c ; mv readsym.o ../obj/readsym.o

obj/rdwr.o: stdio/rdwr.c
	cd stdio ; $(CC) -O -c rdwr.c ; mv rdwr.o ../obj/rdwr.o

obj/system.o: stdio/system.c
	cd stdio ; $(CC) -O -c system.c ; mv system.o ../obj/system.o

obj/tmpfile.o: stdio/tmpfile.c
	cd stdio ; $(CC) -O -c tmpfile.c ; mv tmpfile.o ../obj/tmpfile.o

obj/tmpnam.o: stdio/tmpnam.c
	cd stdio ; $(CC) -O -c tmpnam.c ; mv tmpnam.o ../obj/tmpnam.o

obj/fopen.o: stdio/fopen.c
	cd stdio ; $(CC) -O -c fopen.c ; mv fopen.o ../obj/fopen.o

obj/fdopen.o: stdio/fdopen.c
	cd stdio ; $(CC) -O -c fdopen.c ; mv fdopen.o ../obj/fdopen.o

obj/scanf.o: stdio/scanf.c
	cd stdio ; $(CC) -O -c scanf.c ; mv scanf.o ../obj/scanf.o

obj/doscan.o: stdio/doscan.c
	cd stdio ; $(CC) -O -c doscan.c ; mv doscan.o ../obj/doscan.o

obj/printf.o: stdio/printf.c
	cd stdio ; $(CC) -O -I../../../cmd/cparse -c printf.c ; mv printf.o ../obj/printf.o

obj/doprnt.o: stdio/doprnt.c
	cd stdio ; $(CC) -O -c doprnt.c ; mv doprnt.o ../obj/doprnt.o

obj/stfl.o: gen/stfl.s
	cd gen ; $(CAS) -o ../obj/stfl.o stfl.s

obj/ffltioc.o: stdio/ffltioc.c
	cd stdio ; $(CC) -O -c ffltioc.c ; mv ffltioc.o ../obj/ffltioc.o

obj/gets.o: stdio/gets.c
	cd stdio ; $(CC) -O -c gets.c ; mv gets.o ../obj/gets.o

obj/getw.o: stdio/getw.c
	cd stdio ; $(CC) -O -c getw.c ; mv getw.o ../obj/getw.o

obj/puts.o: stdio/puts.c
	cd stdio ; $(CC) -O -c puts.c ; mv puts.o ../obj/puts.o

obj/putw.o: stdio/putw.c
	cd stdio ; $(CC) -O -c putw.c ; mv putw.o ../obj/putw.o

obj/ungetc.o: stdio/ungetc.c
	cd stdio ; $(CC) -O -c ungetc.c ; mv ungetc.o ../obj/ungetc.o

obj/atof.o: gen/atof.s
	cd gen ; $(AS) -f -o ../obj/atof.o atof.s

obj/ecvt.o: gen/ecvt.s
	cd gen ; $(AS) -f -o ../obj/ecvt.o ecvt.s

obj/pow10.o: gen/pow10.s
	cd gen ; $(AS) -f -o ../obj/pow10.o pow10.s

obj/filbuf.o: stdio/filbuf.c
	cd stdio ; $(CC) -O -c filbuf.c ; mv filbuf.o ../obj/filbuf.o

obj/setbuf.o: stdio/setbuf.c
	cd stdio ; $(CC) -O -c setbuf.c ; mv setbuf.o ../obj/setbuf.o

obj/strout.o: stdio/strout.c
	cd stdio ; $(CC) -O -c strout.c ; mv strout.o ../obj/strout.o

obj/flsbuf.o: stdio/flsbuf.c
	cd stdio ; $(CC) -O -c flsbuf.c ; mv flsbuf.o ../obj/flsbuf.o

obj/endopen.o: stdio/endopen.c
	cd stdio ; $(CC) -O -c endopen.c ; mv endopen.o ../obj/endopen.o

obj/findiop.o: stdio/findiop.c
	cd stdio ; $(CC) -O -c findiop.c ; mv findiop.o ../obj/findiop.o

obj/clrerr.o: stdio/clrerr.c
	cd stdio ; $(CC) -O -c clrerr.c ; mv clrerr.o ../obj/clrerr.o

obj/data.o: stdio/data.c
	cd stdio ; $(CC) -O -c data.c ; mv data.o ../obj/data.o

obj/__callex.o: gen/__callex.c
	cd gen ; $(CC) -O -c __callex.c ; mv __callex.o ../obj/__callex.o

obj/mcount.o: crt/mcount.s
	cd crt ; $(AS) -o ../obj/mcount.o mcount.s

obj/cuexit.o: gen/cuexit.s
	cd gen ; $(AS) -o ../obj/cuexit.o cuexit.s

obj/execlp.o: sys/execlp.s
	cd sys ; $(AS) -o ../obj/execlp.o execlp.s

obj/execvp.o: gen/execvp.c
	cd gen ; $(CC) -O -c execvp.c ; mv execvp.o ../obj/execvp.o

obj/ctime.o: gen/ctime.c
	cd gen ; $(CC) -O -c ctime.c ; mv ctime.o ../obj/ctime.o

obj/getenv.o: gen/getenv.c
	cd gen ; $(CC) -O -c getenv.c ; mv getenv.o ../obj/getenv.o

obj/getlogin.o: gen/getlogin.c
	cd gen ; $(CC) -O -c getlogin.c ; mv getlogin.o ../obj/getlogin.o

obj/perror.o: gen/perror.c
	cd gen ; $(CC) -O -c perror.c ; mv perror.o ../obj/perror.o

obj/sleep.o: gen/sleep.c
	cd gen ; $(CC) -O -c sleep.c ; mv sleep.o ../obj/sleep.o

obj/ttyslot.o: gen/ttyslot.c
	cd gen ; $(CC) -O -c ttyslot.c ; mv ttyslot.o ../obj/ttyslot.o

obj/ttyname.o: gen/ttyname.c
	cd gen ; $(CC) -O -c ttyname.c ; mv ttyname.o ../obj/ttyname.o

obj/abort.o: gen/abort.s
	cd gen ; $(AS) -o ../obj/abort.o abort.s

obj/abs.o: gen/abs.c
	cd gen ; $(CC) -O -c abs.c ; mv abs.o ../obj/abs.o

obj/atoi.o: gen/atoi.c
	cd gen ; $(CC) -O -c atoi.c ; mv atoi.o ../obj/atoi.o

obj/atol.o: gen/atol.c
	cd gen ; $(CC) -O -c atol.c ; mv atol.o ../obj/atol.o

obj/bsearch.o: gen/bsearch.c
	cd gen ; $(CC) -O -c bsearch.c ; mv bsearch.o ../obj/bsearch.o

obj/cnvt.o: gen/cnvt.c
	cd gen ; $(CC) -O -c cnvt.c ; mv cnvt.o ../obj/cnvt.o

obj/decimal.o: gen/decimal.s
	cd gen ; $(AS) -o ../obj/decimal.o decimal.s

obj/length.o: gen/length.c
	cd gen ; $(CC) -O -c length.c ; mv length.o ../obj/length.o

obj/crypt.o: gen/crypt.c
	cd gen ; $(CC) -O -c crypt.c ; mv crypt.o ../obj/crypt.o

obj/malloc.o: oldcc/malloc/3_3.s
	cd oldcc/malloc ; $(CAS) -u -o ../../obj/malloc.o 3_3.s

obj/errlst.o: gen/errlst.c
	cd gen ; $(CC) -O -c errlst.c ; mv errlst.o ../obj/errlst.o

obj/fakcu.o: gen/fakcu.s
	cd gen ; $(AS) -o ../obj/fakcu.o fakcu.s

obj/isatty.o: gen/isatty.c
	cd gen ; $(CC) -O -c isatty.c ; mv isatty.o ../obj/isatty.o

obj/l3.o: gen/l3.c
	cd gen ; $(CC) -O -c l3.c ; mv l3.o ../obj/l3.o

obj/lsearch.o: gen/lsearch.c
	cd gen ; $(CC) -O -c lsearch.c ; mv lsearch.o ../obj/lsearch.o

obj/mktemp.o: gen/mktemp.c
	cd gen ; $(CC) -O -c mktemp.c ; mv mktemp.o ../obj/mktemp.o

obj/mon.o: gen/mon.c
	cd gen ; $(CC) -O -c mon.c ; mv mon.o ../obj/mon.o

obj/mpx.o: gen/mpx.c
	cd gen ; $(CC) -O -c mpx.c ; mv mpx.o ../obj/mpx.o

obj/nlist.o: oldcc/nlist/0_1.s
	cd oldcc/nlist ; $(CAS) -u -o ../../obj/nlist.o 0_1.s

obj/qsort.o: gen/qsort.c
	cd gen ; $(CC) -O -c qsort.c ; mv qsort.o ../obj/qsort.o

obj/rand.o: gen/rand.c
	cd gen ; $(CC) -O -c rand.c ; mv rand.o ../obj/rand.o

obj/setret.o: gen/setret.s
	cd gen ; $(AS) -o ../obj/setret.o setret.s

obj/ssignal.o: gen/ssignal.c
	cd gen ; $(CC) -O -c ssignal.c ; mv ssignal.o ../obj/ssignal.o

obj/stty.o: gen/stty.c
	cd gen ; $(CC) -O -c stty.c ; mv stty.o ../obj/stty.o

obj/swab.o: gen/swab.c
	cd gen ; $(CC) -O -c swab.c ; mv swab.o ../obj/swab.o

obj/tell.o: gen/tell.c
	cd gen ; $(CC) -O -c tell.c ; mv tell.o ../obj/tell.o

obj/ctype_.o: gen/ctype_.c
	cd gen ; $(CC) -O -c ctype_.c ; mv ctype_.o ../obj/ctype_.o

obj/index.o: gen/index.c
	cd gen ; $(CC) -O -c index.c ; mv index.o ../obj/index.o

obj/rindex.o: gen/rindex.c
	cd gen ; $(CC) -O -c rindex.c ; mv rindex.o ../obj/rindex.o

obj/toupper.o: gen/toupper.c
	cd gen ; $(CC) -O -c toupper.c ; mv toupper.o ../obj/toupper.o

obj/tolower.o: gen/tolower.c
	cd gen ; $(CC) -O -c tolower.c ; mv tolower.o ../obj/tolower.o

obj/strcat.o: gen/strcat.c
	cd gen ; $(CC) -O -c strcat.c ; mv strcat.o ../obj/strcat.o

obj/strncat.o: gen/strncat.c
	cd gen ; $(CC) -O -c strncat.c ; mv strncat.o ../obj/strncat.o

obj/strcmp.o: gen/strcmp.c
	cd gen ; $(CC) -O -c strcmp.c ; mv strcmp.o ../obj/strcmp.o

obj/strncmp.o: gen/strncmp.c
	cd gen ; $(CC) -O -c strncmp.c ; mv strncmp.o ../obj/strncmp.o

obj/strcpy.o: gen/strcpy.c
	cd gen ; $(CC) -O -c strcpy.c ; mv strcpy.o ../obj/strcpy.o

obj/strncpy.o: gen/strncpy.c
	cd gen ; $(CC) -O -c strncpy.c ; mv strncpy.o ../obj/strncpy.o

obj/strlen.o: gen/strlen.c
	cd gen ; $(CC) -O -c strlen.c ; mv strlen.o ../obj/strlen.o

obj/strtok.o: gen/strtok.c
	cd gen ; $(CC) -O -c strtok.c ; mv strtok.o ../obj/strtok.o

obj/strchr.o: gen/strchr.c
	cd gen ; $(CC) -O -c strchr.c ; mv strchr.o ../obj/strchr.o

obj/strcspn.o: gen/strcspn.c
	cd gen ; $(CC) -O -c strcspn.c ; mv strcspn.o ../obj/strcspn.o

obj/strpbrk.o: gen/strpbrk.c
	cd gen ; $(CC) -O -c strpbrk.c ; mv strpbrk.o ../obj/strpbrk.o

obj/strrchr.o: gen/strrchr.c
	cd gen ; $(CC) -O -c strrchr.c ; mv strrchr.o ../obj/strrchr.o

obj/strspn.o: gen/strspn.c
	cd gen ; $(CC) -O -c strspn.c ; mv strspn.o ../obj/strspn.o

obj/goodmagic.o: gen/goodmagic.c
	cd gen ; $(CC) -O -c goodmagic.c ; mv goodmagic.o ../obj/goodmagic.o

obj/swap.o: oldcc/swap/0_1.s
	cd oldcc/swap ; $(CAS) -u -o ../../obj/swap.o 0_1.s

obj/a64l.o: gen/a64l.c
	cd gen ; $(CC) -O -c a64l.c ; mv a64l.o ../obj/a64l.o

obj/l64a.o: gen/l64a.c
	cd gen ; $(CC) -O -c l64a.c ; mv l64a.o ../obj/l64a.o

obj/access.o: sys/access.s
	cd sys ; $(AS) -o ../obj/access.o access.s

obj/acct.o: sys/acct.s
	cd sys ; $(AS) -o ../obj/acct.o acct.s

obj/alarm.o: sys/alarm.s
	cd sys ; $(AS) -o ../obj/alarm.o alarm.s

obj/brk.o: sys/brk.s
	cd sys ; $(AS) -o ../obj/brk.o brk.s

obj/chdir.o: sys/chdir.s
	cd sys ; $(AS) -o ../obj/chdir.o chdir.s

obj/chroot.o: sys/chroot.s
	cd sys ; $(AS) -o ../obj/chroot.o chroot.s

obj/chmod.o: sys/chmod.s
	cd sys ; $(AS) -o ../obj/chmod.o chmod.s

obj/chown.o: sys/chown.s
	cd sys ; $(AS) -o ../obj/chown.o chown.s

obj/close.o: sys/close.s
	cd sys ; $(AS) -o ../obj/close.o close.s

obj/creat.o: sys/creat.s
	cd sys ; $(AS) -o ../obj/creat.o creat.s

obj/dup.o: sys/dup.s
	cd sys ; $(AS) -o ../obj/dup.o dup.s

obj/dup2.o: sys/dup2.s
	cd sys ; $(AS) -o ../obj/dup2.o dup2.s

obj/execl.o: sys/execl.s
	cd sys ; $(AS) -o ../obj/execl.o execl.s

obj/execle.o: sys/execle.s
	cd sys ; $(AS) -o ../obj/execle.o execle.s

obj/execv.o: sys/execv.s
	cd sys ; $(AS) -o ../obj/execv.o execv.s

obj/execve.o: sys/execve.s
	cd sys ; $(AS) -o ../obj/execve.o execve.s

obj/exit.o: sys/exit.s
	cd sys ; $(AS) -o ../obj/exit.o exit.s

obj/fcntl.o: sys/fcntl.s
	cd sys ; $(AS) -o ../obj/fcntl.o fcntl.s

obj/fork.o: sys/fork.s
	cd sys ; $(AS) -o ../obj/fork.o fork.s

obj/fstat.o: sys/fstat.s
	cd sys ; $(AS) -o ../obj/fstat.o fstat.s

obj/ftime.o: sys/ftime.s
	cd sys ; $(AS) -o ../obj/ftime.o ftime.s

obj/getgid.o: sys/getgid.s
	cd sys ; $(AS) -o ../obj/getgid.o getgid.s

obj/getegid.o: sys/getegid.s
	cd sys ; $(AS) -o ../obj/getegid.o getegid.s

obj/getpgrp.o: sys/getpgrp.s
	cd sys ; $(AS) -o ../obj/getpgrp.o getpgrp.s

obj/getpid.o: sys/getpid.s
	cd sys ; $(AS) -o ../obj/getpid.o getpid.s

obj/getppid.o: sys/getppid.s
	cd sys ; $(AS) -o ../obj/getppid.o getppid.s

obj/getuid.o: sys/getuid.s
	cd sys ; $(AS) -o ../obj/getuid.o getuid.s

obj/geteuid.o: sys/geteuid.s
	cd sys ; $(AS) -o ../obj/geteuid.o geteuid.s

obj/ioctl.o: sys/ioctl.s
	cd sys ; $(AS) -o ../obj/ioctl.o ioctl.s

obj/mdmctl.o: sys/mdmctl.s
	cd sys ; $(AS) -o ../obj/mdmctl.o mdmctl.s

obj/kill.o: sys/kill.s
	cd sys ; $(AS) -o ../obj/kill.o kill.s

obj/lkdata.o: sys/lkdata.s
	cd sys ; $(AS) -o ../obj/lkdata.o lkdata.s

obj/link.o: sys/link.s
	cd sys ; $(AS) -o ../obj/link.o link.s

obj/lock.o: sys/lock.s
	cd sys ; $(AS) -o ../obj/lock.o lock.s

obj/lseek.o: sys/lseek.s
	cd sys ; $(AS) -o ../obj/lseek.o lseek.s

obj/mknod.o: sys/mknod.s
	cd sys ; $(AS) -o ../obj/mknod.o mknod.s

obj/mount.o: sys/mount.s
	cd sys ; $(AS) -o ../obj/mount.o mount.s

obj/nice.o: sys/nice.s
	cd sys ; $(AS) -o ../obj/nice.o nice.s

obj/open.o: sys/open.s
	cd sys ; $(AS) -o ../obj/open.o open.s

obj/pause.o: sys/pause.s
	cd sys ; $(AS) -o ../obj/pause.o pause.s

obj/pipe.o: sys/pipe.s
	cd sys ; $(AS) -o ../obj/pipe.o pipe.s

obj/profil.o: sys/profil.s
	cd sys ; $(AS) -o ../obj/profil.o profil.s

obj/ptrace.o: sys/ptrace.s
	cd sys ; $(AS) -o ../obj/ptrace.o ptrace.s

obj/read.o: sys/read.s
	cd sys ; $(AS) -o ../obj/read.o read.s

obj/sptrace.o: sys/sptrace.s
	cd sys ; $(AS) -o ../obj/sptrace.o sptrace.s

obj/setgid.o: sys/setgid.s
	cd sys ; $(AS) -o ../obj/setgid.o setgid.s

obj/setpgrp.o: sys/setpgrp.s
	cd sys ; $(AS) -o ../obj/setpgrp.o setpgrp.s

obj/setuid.o: sys/setuid.s
	cd sys ; $(AS) -o ../obj/setuid.o setuid.s

obj/signal.o: sys/signal.s
	cd sys ; $(AS) -o ../obj/signal.o signal.s

obj/stat.o: sys/stat.s
	cd sys ; $(AS) -o ../obj/stat.o stat.s

obj/stime.o: sys/stime.s
	cd sys ; $(AS) -o ../obj/stime.o stime.s

obj/sync.o: sys/sync.s
	cd sys ; $(AS) -o ../obj/sync.o sync.s

obj/time.o: sys/time.s
	cd sys ; $(AS) -o ../obj/time.o time.s

obj/times.o: sys/times.s
	cd sys ; $(AS) -o ../obj/times.o times.s

obj/umask.o: sys/umask.s
	cd sys ; $(AS) -o ../obj/umask.o umask.s

obj/umount.o: sys/umount.s
	cd sys ; $(AS) -o ../obj/umount.o umount.s

obj/uname.o: sys/uname.s
	cd sys ; $(AS) -o ../obj/uname.o uname.s

obj/ulimit.o: sys/ulimit.s
	cd sys ; $(AS) -o ../obj/ulimit.o ulimit.s

obj/unlink.o: sys/unlink.s
	cd sys ; $(AS) -o ../obj/unlink.o unlink.s

obj/unlk.o: sys/unlk.s
	cd sys ; $(AS) -o ../obj/unlk.o unlk.s

obj/ustat.o: sys/ustat.s
	cd sys ; $(AS) -o ../obj/ustat.o ustat.s

obj/utime.o: sys/utime.s
	cd sys ; $(AS) -o ../obj/utime.o utime.s

obj/wait.o: sys/wait.s
	cd sys ; $(AS) -o ../obj/wait.o wait.s

obj/write.o: sys/write.s
	cd sys ; $(AS) -o ../obj/write.o write.s

obj/cerror.o: crt/cerror.s
	cd crt ; $(AS) -o ../obj/cerror.o cerror.s

clean:
	rm -rf obj libc.a

.PHONY: all clean
