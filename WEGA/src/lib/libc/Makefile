# libc: /lib/libc.a from its sources, with the shipped tools, the whole file byte-identical (headers: arhdr.s, mkar.sh). Each object is identical with the member of the original.
# The members of oldcc/ were made by an older cc that we do not have yet.
#
#   make        libc.a and obj/*.o, the members for ../oldlibc; csu/crt0.o (/lib/crt0.o)
#   make install PREFIX=dir   dir/lib/libc.a, dir/lib/crt0.o
#   make clean

CC = cc
CAS = cas
AS = as
AR = ar
PREFIX = /

SUBDIRS = gen stdio sys oldcc crt

all: libc.a csu/crt0.o

csu/crt0.o: csu/crt0.s
	cd csu ; $(AS) -o crt0.o crt0.s

obj:
	-mkdir obj

# the members of libc.a: each subdirectory makes its objects into obj/
objs: obj
	cd gen ; $(MAKE) CC='$(CC)' CAS='$(CAS)' AS='$(AS)' objs
	cd stdio ; $(MAKE) CC='$(CC)' CAS='$(CAS)' AS='$(AS)' objs
	cd sys ; $(MAKE) CC='$(CC)' CAS='$(CAS)' AS='$(AS)' objs
	cd oldcc ; $(MAKE) CC='$(CC)' CAS='$(CAS)' AS='$(AS)' objs
	cd crt ; $(MAKE) CC='$(CC)' CAS='$(CAS)' AS='$(AS)' objs

libc.a: objs arhdr.s mkar.sh
	rm -f libc.a
	cp Library.SC obj/Library.SC
	$(CAS) -u -o obj/arhdr.o arhdr.s
	cd obj ; sh ../mkar.sh ../libc.a arhdr.o getgrgid.o getgrnam.o getgrent.o cuserid.o getpass.o getpwnam.o getpwuid.o putpwent.o getpwent.o ctermid.o getopt.o fgetc.o fputc.o getchar.o putchar.o popen.o fltioc.o freopen.o fgets.o fputs.o getpw.o fseek.o ftell.o rew.o symscan.o readsym.o rdwr.o system.o tmpfile.o tmpnam.o fopen.o fdopen.o scanf.o doscan.o printf.o doprnt.o stfl.o ffltioc.o gets.o getw.o puts.o putw.o ungetc.o atof.o ecvt.o pow10.o filbuf.o setbuf.o strout.o flsbuf.o endopen.o findiop.o clrerr.o data.o __callex.o mcount.o cuexit.o execlp.o execvp.o ctime.o getenv.o getlogin.o perror.o sleep.o ttyslot.o ttyname.o abort.o abs.o atoi.o atol.o bsearch.o cnvt.o decimal.o length.o crypt.o malloc.o errlst.o fakcu.o isatty.o l3.o lsearch.o mktemp.o mon.o mpx.o nlist.o qsort.o rand.o setret.o ssignal.o stty.o swab.o tell.o ctype_.o index.o rindex.o toupper.o tolower.o strcat.o strncat.o strcmp.o strncmp.o strcpy.o strncpy.o strlen.o strtok.o strchr.o strcspn.o strpbrk.o strrchr.o strspn.o goodmagic.o swap.o a64l.o l64a.o access.o acct.o alarm.o brk.o chdir.o chroot.o chmod.o chown.o close.o creat.o dup.o dup2.o execl.o execle.o execv.o execve.o exit.o fcntl.o fork.o fstat.o ftime.o getgid.o getegid.o getpgrp.o getpid.o getppid.o getuid.o geteuid.o ioctl.o mdmctl.o kill.o lkdata.o link.o lock.o lseek.o mknod.o mount.o nice.o open.o pause.o pipe.o profil.o ptrace.o read.o sptrace.o setgid.o setpgrp.o setuid.o signal.o stat.o stime.o sync.o time.o times.o umask.o umount.o uname.o ulimit.o unlink.o unlk.o ustat.o utime.o wait.o write.o cerror.o Library.SC

install: all
	cp libc.a $(PREFIX)/lib/libc.a
	cp csu/crt0.o $(PREFIX)/lib/crt0.o

clean:
	rm -rf obj libc.a csu/crt0.o
	cd crt ; $(MAKE) clean

.PHONY: all install clean objs
