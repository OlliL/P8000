#
# makefile for libc/u8k/sys
#

SHELL=/bin/sh
CC=lcc
CFLAGS=-O

OBJECTS=\
dup2.o        execl.o       execle.o	  execv.o	  \
ftime.o       getpgrp.o	    lkdata.o	  mdmctl.o	  \
mkseg.o       sgbrk.o	    sgstat.o	  sptrace.o	  \
ssgbrk.o      stime.o	    unlk.o	  \
access.o      acct.o        alarm.o       chdir.o	\
chmod.o       chown.o       chroot.o      close.o	\
creat.o       dup.o         \
execve.o      exit.o        fcntl.o	\
fork.o        fstat.o       getegid.o     geteuid.o	\
getgid.o      getpid.o      getppid.o     getuid.o	\
ioctl.o       kill.o        link.o	\
lseek.o       mknod.o       mount.o       \
nice.o        open.o        pause.o       pipe.o	\
plock.o       profil.o      ptrace.o      read.o	\
setgid.o      setpgrp.o     setuid.o	\
signal.o      stat.o        stime.o	      \
stty.o        sync.o        time.o	\
times.o       ulimit.o      umask.o       umount.o	\
uname.o       unlink.o      ustat.o       utime.o	\
wait.o        write.o

all: $(OBJECTS)

#-------------------------

.s.o:
	@echo $*.s:
	$(CC) $(CFLAGS) -c $*.s

