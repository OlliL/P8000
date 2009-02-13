/*
	FILE: unix.c
	
	Routines: This file contains the following routines:
		fileinit()   (moved this to main.c so MS-DOS could use it)
		sysreset()
		eihalt()
		kbread()
		clksec()
		tmpfile()	 for  Coherent - K5JB 
		restore()
		stxrdy()
		disable()
		memstat()
		filedir()
		check_time()
		getds()
		audit()
		doshell()
		dodir()
		rename()
		docd()
		ether_dump()
		mkdir()	 for those who don't have it 
		rmdir()
		onshellrtn()	part of forked shell process
		
	Written by Mikel Matthews, N9DVG
	SYS5 stuff added by Jere Sandidge, K4FUM
	Further cleanup, maintenance and etc. by Joe Buswell, K5JB
	#define FORK_SHELL for a child process that handles foreground
	while you are shelled out.

*/
#ifndef _OSK			/* none of this applies to _OSK */
#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <time.h>
#include <fcntl.h>		/* K5JB */

#include "global.h"
#include "cmdparse.h"
#include "iface.h"
#include "unix.h"
#include "unixopt.h"
#include "ndir.h"

#define	MAXCMD	1024

int asy_attach();

extern struct cmds attab[];
extern struct termio savecon;
extern struct interface *ifaces;


unsigned nasy;

/* action routine for remote reset */
void
sysreset()
{
	extern int exitval;
	int doexit();	/* in main.c */

	exitval = 3;
	doexit();
#ifdef NOGOOD	/* use exitval and a shell script */
	extern char *netexe;

	execl(netexe,netexe,0);
	execl("net","net",0);
	printf("reset failed: exiting\n");
	exit();
#endif
}

#ifdef NOTUSED
void
eihalt()
{
	tnix_scan();
}
#endif

#ifdef FORKSHELL
extern int mode;
#include "session.h"
int shellpid;
extern int backgrd;
int onquit();
extern int iostop();
#endif

int
kbread()
{
	unsigned char c;
#if defined(COH386) && !defined(COH4)
	int sleep2();

	sleep2(COHWAIT);	/* only needed when no async ports are used */
#endif
#ifdef FORKSHELL /* note that if background, kbread won't be called */
	if(shellpid)	/* might put a micro sleep here to give up cycles */
		return -1;
#endif
#ifdef USE_QUIT
	if(backgrd)
		return -1;
#endif
	if (read(fileno(stdin), &c, 1) <= 0)
		return -1;

	return ((int) c);
}

int
clksec()
{
	long tim;
	(void) time(&tim);

	return ((int) tim);
}

#if defined(COH386) && !defined(COH4)
FILE *
tmpfile()
{
	char *buf;
	FILE *fp;
#ifdef LATER	/* may come back later and do cleanup - K5JB */
	static int nr_tmpfiles;
	if((buf = (char *)malloc(L_tempnam)) == NULLCHAR)
		return 0;
#else
	buf = tmpnam(NULLCHAR);
#endif
	if((fp = fopen(buf,"w+")) == NULLFILE)
		return 0;
	return fp;
}
#endif

void
restore()
{
}

int
stxrdy()
{
	return 1;
}

int
disable()
{
	return 1;	/*  dummy function -- not used in Unix */
}

int
memstat()
{
	return 0;
}

#define NULLDIR (DIR *)0
#define NULLDIRECT (struct direct *)0


/* wildcard filename lookup */
void
filedir(name, times, ret_str)
char	*name;
int	times;
char	*ret_str;
{
	static char	dname[128], fname[128];
	static DIR *dirp = NULLDIR;
	struct direct *dp;
	struct stat sbuf;
	char	*cp, temp[128];

	/*
	 * Make sure that the NULL is there in case we don't find anything
	 */
	ret_str[0] = '\0';

	if (times == 0) {
		/* default a null name to *.* */
		if (name == NULLCHAR || *name == '\0')
			name = "*.*";
		/* split path into directory and filename */
		if ((cp = strrchr(name, '/')) == NULLCHAR) {
			strcpy(dname, ".");
			strcpy(fname, name);
		} else {
			strcpy(dname, name);
			dname[cp - name] = '\0';
			strcpy(fname, cp + 1);
			/* root directory */
			if (dname[0] == '\0')
				strcpy(dname, "/");
			/* trailing '/' */
			if (fname[0] == '\0')
				strcpy(fname, "*.*");
		}
		/* close directory left over from another call */
		if (dirp != NULLDIR)
			closedir(dirp);
		/* open directory */
		if ((dirp = opendir(dname)) == NULLDIR) {
			printf("Could not open DIR (%s)\n", dname);
			return;
		}
	} else {
		/* for people who don't check return values */
		if (dirp == NULLDIR)
			return;
	}

	/* scan directory */
	while ((dp = readdir(dirp)) != NULLDIRECT) {
		/* test for name match */
		if (wildmat(dp->d_name, fname)) {
			/* test for regular file */
			sprintf(temp, "%s/%s", dname, dp->d_name);
			if (stat(temp, &sbuf) < 0)
				continue;
			if ((sbuf.st_mode & S_IFMT) != S_IFREG)
				continue;
			strcpy(ret_str, dp->d_name);
			break;
		}
	}

	/* close directory if we hit the end */
	if (dp == NULLDIRECT) {
		closedir(dirp);
		dirp = NULLDIR;
	}
}


/* checks the time then ticks and updates ISS */
void
check_time()
{
	int32 iss();
	void tickle();
#if defined(COH386) && !defined(COH4)
	long time();
#else
	struct tms tb;
#ifndef COH4
	long times();
#endif
#endif

	static long clkval;
	long ntime, offset;

	/* With older Coherent, read elapsed real time in seconds, others
    * are done in machine clock ticks.  Later Coherent has times()
    * fixed but it returns an int instead of a long
    */
#if defined(COH386) && !defined(COH4)
	time(&ntime);
#else
#ifdef COH4	/* damned thing returns an int */
	ntime = (long)times(&tb);
#else
	ntime = times(&tb);
#endif
#endif

	/* resynchronize if the error is large (10 seconds or more) */
	offset = ntime - clkval;
	if (offset > (10000/MSPTICK) || offset < 0)
		clkval = ntime;

	/* Handle possibility of several missed ticks */
	while (ntime != clkval) {
		++clkval;
		icmpclk();
		tickle();
		(void) iss();
	}
}

int
getds()
{
	return 0;
}

void
audit()
{
}

#ifdef FORKSHELL
void
onshellrtn()
{
	(void)signal(SIGQUIT, onquit);	/* restore normal signals */
   (void)signal(SIGHUP, iostop);
   (void)signal(SIGINT, iostop);
   (void)signal(SIGTERM, iostop);
	/* in case someone had a session with us while we were gone */
	if(mode != CMD_MODE)
		mode = CMD_MODE;
}
#endif

int
doshell(argc, argv)		/* modified to make shell "stick" K5JB */
char	**argv;			/* later modified to make shell a child process */
{

#ifndef FORKSHELL
	char	str[MAXCMD];
	int i,retn;
#else
	int parentpid;
#endif
	char	*cp;
	struct termio tt_config;
	int saverunflg;
	char	*getenv();

#ifndef FORKSHELL
	str[0] = '\0';
	for (i = 1; i < argc; i++) {
		strcat(str, argv[i]);
		strcat(str, " ");
	}

	ioctl(0, TCGETA, &tt_config);
	ioctl(0, TCSETAW, &savecon);
	if ((saverunflg = fcntl(0, F_GETFL, 0)) == -1) {
		perror("Could not read console flags");
		return -1;
	}
	fcntl(0, F_SETFL, saverunflg & ~O_NDELAY); /* blocking I/O */

	if (argc > 1)
		retn = system(str);
	else if ((cp = getenv("SHELL")) != NULLCHAR && *cp != '\0')
		retn = system(cp);
	else
		retn = system("exec /bin/sh");

	fcntl(0, F_SETFL, saverunflg); /* restore non-blocking I/O, if so */
	ioctl(0, TCSETAW, &tt_config);
	printf("Returning you to net.\n");
	return retn;
#else
	/* will fork a child process */
	signal(SIGQUIT,SIG_IGN);	/* ignore these signals, and... */
   signal(SIGHUP,SIG_IGN);
   signal(SIGINT,SIG_IGN);
   signal(SIGTERM,SIG_IGN);
	signal(SIGUSR1,onshellrtn);	/* on return, this will restore them */

	parentpid = getpid();	/* this will work if we don't detach */

	printf("Type exit, or do Ctrl-D, to return to NET.\n");
	fflush(stdout);

	if(!(shellpid = fork())){	/* is child process */
		ioctl(0, TCGETA, &tt_config);
		ioctl(0, TCSETAW, &savecon);
		if ((saverunflg = fcntl(0, F_GETFL, 0)) == -1) {
			perror("Could not read console flags");
			return -1;
		}
		fcntl(0, F_SETFL, saverunflg & ~O_NDELAY); /* blocking I/O */
#ifdef LATER_IFNEEDED
	while (ifaces != NULLIF) {
		if (ifaces->stop != NULLFP)
			(*ifaces->stop)(ifaces->dev);
		ifaces = ifaces->next;
	}
#endif
		/* Note that system() can cause problems with forked process */
		/* we use it though so we can signal our return with kill() */
		if((cp = getenv("SHELL")) != NULLCHAR && *cp != '\0')
			system(cp);
		else
			system("exec /bin/sh");
		fcntl(0, F_SETFL, saverunflg); /* restore non-blocking I/O, if so */
		ioctl(0, TCSETAW, &tt_config);

		kill(parentpid,SIGUSR1);	/* first restore signals, then use one */
		/* this makes a newline so if we return with Ctrl-D NET> looks better */
		printf("\n");
		fflush(stdout);
		/* apparently need some time before second kill */
		sleep(1);
		/* this should kill us and restore foreground */
		kill(parentpid,SIGQUIT);
		sleep(10);	/* will probably not awake from this one */
		/* but if sleep time was insufficient... */
		printf("You MAY have to use Ctrl-\\ to return to NET...\n");
		fflush(stdout);
		/* quit signal handler will take care of killing child properly */
		/* but if it don't... */
		exit(0);
	}else
		return 0;
#endif
}

int
dodir(argc, argv)
int	argc;
char	**argv;
{
	int	i, stat;
	char	str[MAXCMD];
	struct termio tt_config;

	strcpy(str, "ls -l ");
	for (i = 1; i < argc; i++) {
		strcat(str, argv[i]);
		strcat(str, " ");
	}

	ioctl(0, TCGETA, &tt_config);
	ioctl(0, TCSETAW, &savecon);

	stat = system(str);

	ioctl(0, TCSETAW, &tt_config);

	return stat;
}

int
rename(s1, s2)
char *s1, *s2;
{
	char tmp[MAXCMD];

	(void) sprintf(tmp, "mv %s %s", s1, s2);
	return(system(tmp));
}


/* enabled this command with k28 -- it was incomplete.  Tried to do it
in most portable way, though in SysV, it would be appropriate to feed
getcwd a null pointer and free the memory pointed to by getcwd's return
value.  This command is used for "pwd" and "cd".  - K5JB
*/
int
docd(argc, argv)
int argc;
char **argv;
{
	static char tmp[MAXCMD];
	char *getcwd();
	extern char *homedir;

	if(*argv[0] == 'c')
		if(chdir(argc > 1 ? argv[1] : homedir) == -1){
			printf("Can't change directory\n");
			return 1;
		}

	if (getcwd(tmp, sizeof(tmp) - 2) != NULL)
		printf("%s\n", tmp);

	return 0;
}

void
ether_dump()
{
}

#ifndef HAVEMKDIR
int
mkdir(s, m)
char	*s;
int	m;
{
	char tmp[MAXCMD];

	sprintf(tmp, "mkdir %s", s);
	if (system(tmp))
		return -1;
	if (chmod(s, m) < 0)
		return -1;

	return 0;
}
#endif

int
rmdir(s)
char	*s;
{
	char tmp[MAXCMD];

	sprintf(tmp, "rmdir %s", s);
	if (system(tmp))
		return -1;

	return 0;
}

#endif /* OSK */
