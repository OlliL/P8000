AS=	as
ASFLAGS=
OBJ=	access.o acct.o alarm.o brk.o chdir.o chmod.o chown.o chroot.o close.o \
	creat.o dup.o dup2.o execl.o execle.o execlp.o execv.o execve.o exit.o \
	fcntl.o fork.o fstat.o ftime.o getegid.o geteuid.o getgid.o getpgrp.o \
	getpid.o getppid.o getuid.o ioctl.o kill.o link.o lkdata.o lock.o \
	lseek.o mdmctl.o mknod.o mount.o nice.o open.o pause.o pipe.o profil.o \
	ptrace.o read.o setgid.o setpgrp.o setuid.o signal.o sptrace.o stat.o \
	stime.o sync.o time.o times.o ulimit.o umask.o umount.o uname.o \
	unlink.o unlk.o ustat.o utime.o wait.o write.o

all: $(OBJ)

clean:
	rm -f $(OBJ)

# the members of libc.a for ../obj (called by ../Makefile); CC and CAS are passed on
CC = cc
CAS = cas

LIBOBJS = \
	../obj/execlp.o ../obj/access.o ../obj/acct.o ../obj/alarm.o ../obj/brk.o ../obj/chdir.o \
	../obj/chroot.o ../obj/chmod.o ../obj/chown.o ../obj/close.o ../obj/creat.o ../obj/dup.o \
	../obj/dup2.o ../obj/execl.o ../obj/execle.o ../obj/execv.o ../obj/execve.o ../obj/exit.o \
	../obj/fcntl.o ../obj/fork.o ../obj/fstat.o ../obj/ftime.o ../obj/getgid.o ../obj/getegid.o \
	../obj/getpgrp.o ../obj/getpid.o ../obj/getppid.o ../obj/getuid.o ../obj/geteuid.o ../obj/ioctl.o \
	../obj/mdmctl.o ../obj/kill.o ../obj/lkdata.o ../obj/link.o ../obj/lock.o ../obj/lseek.o \
	../obj/mknod.o ../obj/mount.o ../obj/nice.o ../obj/open.o ../obj/pause.o ../obj/pipe.o \
	../obj/profil.o ../obj/ptrace.o ../obj/read.o ../obj/sptrace.o ../obj/setgid.o ../obj/setpgrp.o \
	../obj/setuid.o ../obj/signal.o ../obj/stat.o ../obj/stime.o ../obj/sync.o ../obj/time.o \
	../obj/times.o ../obj/umask.o ../obj/umount.o ../obj/uname.o ../obj/ulimit.o ../obj/unlink.o \
	../obj/unlk.o ../obj/ustat.o ../obj/utime.o ../obj/wait.o ../obj/write.o

objs: $(LIBOBJS)

../obj/execlp.o: execlp.s
	$(AS) -o ../obj/execlp.o execlp.s

../obj/access.o: access.s
	$(AS) -o ../obj/access.o access.s

../obj/acct.o: acct.s
	$(AS) -o ../obj/acct.o acct.s

../obj/alarm.o: alarm.s
	$(AS) -o ../obj/alarm.o alarm.s

../obj/brk.o: brk.s
	$(AS) -o ../obj/brk.o brk.s

../obj/chdir.o: chdir.s
	$(AS) -o ../obj/chdir.o chdir.s

../obj/chroot.o: chroot.s
	$(AS) -o ../obj/chroot.o chroot.s

../obj/chmod.o: chmod.s
	$(AS) -o ../obj/chmod.o chmod.s

../obj/chown.o: chown.s
	$(AS) -o ../obj/chown.o chown.s

../obj/close.o: close.s
	$(AS) -o ../obj/close.o close.s

../obj/creat.o: creat.s
	$(AS) -o ../obj/creat.o creat.s

../obj/dup.o: dup.s
	$(AS) -o ../obj/dup.o dup.s

../obj/dup2.o: dup2.s
	$(AS) -o ../obj/dup2.o dup2.s

../obj/execl.o: execl.s
	$(AS) -o ../obj/execl.o execl.s

../obj/execle.o: execle.s
	$(AS) -o ../obj/execle.o execle.s

../obj/execv.o: execv.s
	$(AS) -o ../obj/execv.o execv.s

../obj/execve.o: execve.s
	$(AS) -o ../obj/execve.o execve.s

../obj/exit.o: exit.s
	$(AS) -o ../obj/exit.o exit.s

../obj/fcntl.o: fcntl.s
	$(AS) -o ../obj/fcntl.o fcntl.s

../obj/fork.o: fork.s
	$(AS) -o ../obj/fork.o fork.s

../obj/fstat.o: fstat.s
	$(AS) -o ../obj/fstat.o fstat.s

../obj/ftime.o: ftime.s
	$(AS) -o ../obj/ftime.o ftime.s

../obj/getgid.o: getgid.s
	$(AS) -o ../obj/getgid.o getgid.s

../obj/getegid.o: getegid.s
	$(AS) -o ../obj/getegid.o getegid.s

../obj/getpgrp.o: getpgrp.s
	$(AS) -o ../obj/getpgrp.o getpgrp.s

../obj/getpid.o: getpid.s
	$(AS) -o ../obj/getpid.o getpid.s

../obj/getppid.o: getppid.s
	$(AS) -o ../obj/getppid.o getppid.s

../obj/getuid.o: getuid.s
	$(AS) -o ../obj/getuid.o getuid.s

../obj/geteuid.o: geteuid.s
	$(AS) -o ../obj/geteuid.o geteuid.s

../obj/ioctl.o: ioctl.s
	$(AS) -o ../obj/ioctl.o ioctl.s

../obj/mdmctl.o: mdmctl.s
	$(AS) -o ../obj/mdmctl.o mdmctl.s

../obj/kill.o: kill.s
	$(AS) -o ../obj/kill.o kill.s

../obj/lkdata.o: lkdata.s
	$(AS) -o ../obj/lkdata.o lkdata.s

../obj/link.o: link.s
	$(AS) -o ../obj/link.o link.s

../obj/lock.o: lock.s
	$(AS) -o ../obj/lock.o lock.s

../obj/lseek.o: lseek.s
	$(AS) -o ../obj/lseek.o lseek.s

../obj/mknod.o: mknod.s
	$(AS) -o ../obj/mknod.o mknod.s

../obj/mount.o: mount.s
	$(AS) -o ../obj/mount.o mount.s

../obj/nice.o: nice.s
	$(AS) -o ../obj/nice.o nice.s

../obj/open.o: open.s
	$(AS) -o ../obj/open.o open.s

../obj/pause.o: pause.s
	$(AS) -o ../obj/pause.o pause.s

../obj/pipe.o: pipe.s
	$(AS) -o ../obj/pipe.o pipe.s

../obj/profil.o: profil.s
	$(AS) -o ../obj/profil.o profil.s

../obj/ptrace.o: ptrace.s
	$(AS) -o ../obj/ptrace.o ptrace.s

../obj/read.o: read.s
	$(AS) -o ../obj/read.o read.s

../obj/sptrace.o: sptrace.s
	$(AS) -o ../obj/sptrace.o sptrace.s

../obj/setgid.o: setgid.s
	$(AS) -o ../obj/setgid.o setgid.s

../obj/setpgrp.o: setpgrp.s
	$(AS) -o ../obj/setpgrp.o setpgrp.s

../obj/setuid.o: setuid.s
	$(AS) -o ../obj/setuid.o setuid.s

../obj/signal.o: signal.s
	$(AS) -o ../obj/signal.o signal.s

../obj/stat.o: stat.s
	$(AS) -o ../obj/stat.o stat.s

../obj/stime.o: stime.s
	$(AS) -o ../obj/stime.o stime.s

../obj/sync.o: sync.s
	$(AS) -o ../obj/sync.o sync.s

../obj/time.o: time.s
	$(AS) -o ../obj/time.o time.s

../obj/times.o: times.s
	$(AS) -o ../obj/times.o times.s

../obj/umask.o: umask.s
	$(AS) -o ../obj/umask.o umask.s

../obj/umount.o: umount.s
	$(AS) -o ../obj/umount.o umount.s

../obj/uname.o: uname.s
	$(AS) -o ../obj/uname.o uname.s

../obj/ulimit.o: ulimit.s
	$(AS) -o ../obj/ulimit.o ulimit.s

../obj/unlink.o: unlink.s
	$(AS) -o ../obj/unlink.o unlink.s

../obj/unlk.o: unlk.s
	$(AS) -o ../obj/unlk.o unlk.s

../obj/ustat.o: ustat.s
	$(AS) -o ../obj/ustat.o ustat.s

../obj/utime.o: utime.s
	$(AS) -o ../obj/utime.o utime.s

../obj/wait.o: wait.s
	$(AS) -o ../obj/wait.o wait.s

../obj/write.o: write.s
	$(AS) -o ../obj/write.o write.s

.PHONY: objs
