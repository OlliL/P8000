/******************************************************************************
*******************************************************************************

	W E G A - Quelle

	KERN 3.2	Modul: sysent.c


	Bearbeiter:	O. Lehmann
	Datum:		09.05.08
	Version:	1.0

*******************************************************************************
******************************************************************************/

char sysentwstr[] = "@[$]sysent.c	Rev : 4.1 	08/27/83 12:00:12";

#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/systm.h>

/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */

int sysiret();
int rexit();
int fork();
int read();
int write();
int open();
int close();
int wait();
int creat();
int link();
int unlink();
int exec();
int chdir();
int gtime();
int mknod();
int chmod();
int chown();
int sbreak();
int stat();
int seek();
int getpid();
int smount();
int sumount();
int setuid();
int getuid();
int stime();
int ptrace();
int alarm();
int fstat();
int pause();
int utime();
int stty();
int gtty();
int saccess();
int nice();
int ftime();
int sync();
int kill();
int fcntl();
int setpgrp();
int bpt();
int dup();
int pipe();
int times();
int profil();
int ssgbrk();
int setgid();
int getgid();
int ssig();
int lkdata();
int unlk();
int sysacct();
int sprofil();
int syslock();
int ioctl();
int mkseg();
int sgstat();
int utssys();
int exece();
int umask();
int chroot();
int mdmctl();
int nosys();

struct sysent sysent[] =
{
	0, sysiret,		     /*  0 = indir */
	1, rexit,		     /*  1 = exit */
	0, fork,		     /*  2 = fork */
	4, read,		     /*  3 = read */
	4, write,		     /*  4 = write */
	4, open,		     /*  5 = open */
	1, close,		     /*  6 = close */
	0, wait,		     /*  7 = wait */
	3, creat,		     /*  8 = creat */
	4, link,		     /*  9 = link */
	2, unlink,		     /* 10 = unlink */
	4, exec,		     /* 11 = exec */
	2, chdir,		     /* 12 = chdir */
	0, gtime,		     /* 13 = time */
	4, mknod,		     /* 14 = mknod */
	3, chmod,		     /* 15 = chmod */
	4, chown,		     /* 16 = chown; now 3 args */
	2, sbreak,		     /* 17 = break */
	4, stat,		     /* 18 = stat */
	4, seek,		     /* 19 = seek; now 3 args */
	0, getpid,		     /* 20 = getpid */
	5, smount,		     /* 21 = mount */
	2, sumount,		     /* 22 = umount */
	1, setuid,		     /* 23 = setuid */
	0, getuid,		     /* 24 = getuid */
	2, stime,		     /* 25 = stime */
	5, ptrace,		     /* 26 = ptrace */
	1, alarm,		     /* 27 = alarm */
	3, fstat,		     /* 28 = fstat */
	0, pause,		     /* 29 = pause */
	4, utime,		     /* 30 = utime */
	3, stty,		     /* 31 = stty */
	3, gtty,		     /* 32 = gtty */
	3, saccess,		     /* 33 = access */
	1, nice,		     /* 34 = nice */
	2, ftime,		     /* 35 = ftime; formerly sleep */
	0, sync,		     /* 36 = sync */
	2, kill,		     /* 37 = kill */
	3, fcntl,   		     /* 38 = fcntl */
	1, setpgrp,   		     /* 39 = setpgrp */
	0, bpt,   		     /* 40 = bpt */
	2, dup,   		     /* 41 = dup */
	0, pipe,   		     /* 42 = pipe */
	2, times,   		     /* 43 = times */
	5, profil,		     /* 44 = prof */
	2, ssgbrk,		     /* 45 = ssgbrk */
	1, setgid,		     /* 46 = setgid */
	0, getgid,		     /* 47 = getgid */
	3, ssig,		     /* 48 = sig */
	4, lkdata,		     /* 49 = lkdata */
	4, unlk,		     /* 50 = unlnk */
	2, sysacct,		     /* 51 = turn acct off/on */
	6, sprofil,		     /* 52 = sprofil */
	1, syslock,		     /* 53 = lock user in core */
	4, ioctl,		     /* 54 = ioctl */
	2, mkseg,		     /* 55 = mkseg */
	2, sgstat,		     /* 56 = sgstat */
	4, utssys,		     /* 57 = utssys */
	5, ptrace, 		     /* 58 = sptrace */
	6, exece, 		     /* 59 = exece */
	1, umask, 		     /* 60 = umask */
	2, chroot,		     /* 61 = chroot */
	4, mdmctl,		     /* 62 = mdmctl */
	0, nosys,		     /* 63 = x */
	3, nosys,		     /* 64 = x */
	0, nosys,		     /* 65 = x */
	0, nosys,		     /* 66 = x */
	0, nosys,		     /* 67 = x */
	0, nosys,		     /* 68 = x */
	0, nosys,		     /* 69 = x */
	0, nosys,		     /* 70 = x */
	0, nosys,		     /* 71 = x */
	0, nosys,		     /* 72 = x */
	0, nosys,		     /* 73 = x */
	0, nosys,		     /* 74 = x */
	0, nosys,		     /* 75 = x */
	0, nosys,		     /* 76 = x */
	0, nosys,		     /* 77 = x */
	0, nosys,		     /* 78 = x */
	0, nosys,		     /* 79 = x */
};
