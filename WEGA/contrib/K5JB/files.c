/* System-dependent definitions of various files, spool directories, etc
 * Jerked some of this stuff out of main.c which was getting too bulky.
 * Define SKIP_FSCHECK if you want to skip the file system checks and save
 * about 416 bytes on the MS-DOS version.  SKIP_FSCHECK is only used in
 * this file.  k34 - K5JB
 */
#include "options.h"
#include "config.h"
#include <stdio.h>
#ifdef UNIX
#include <sys/types.h>
#endif
#ifndef SKIP_FSCHECK	/* default is to do file system check on start */
#ifdef MSDOS
#include <sys\stat.h>
#else
#include <sys/stat.h>
#endif
#endif
#include "global.h"
#include "netuser.h"
#include "timer.h"
#include "tcp.h"
#include "mbuf.h"
#include <string.h>
#include <time.h>
#define LINES_PER_PAGE 22	/* for the jump arg */

#ifdef _OSK
#include "os9stat.h"
#endif

#ifdef MSDOS
/* Special open modes when reading binary files
 * Index definitions for this array are in global.h
 */
char *binmode[] = {
	"rb",	/* Read binary */
	"wb",	/* Write binary */
	"ab"    /* Append binary */
};
#else /* UNIX  and such */
char *binmode[] = {
	"r",	/* Read */
	"w",	/* Write */
	"a"     /* Append */
};
#endif

#ifdef MSDOS
char *homedir = "";
char *startup = "autoexec.net";	/* Initialization file */
char *userfile = "ftpusers";		/* Authorized FTP users and passwords */
char *hosts = "hosts.net";		/* Network host table */
char *spool = "/spool";
char *mailspool = "mail";	/* Incoming mail */
char *mailqdir = "mqueue";	/* Outgoing mail spool */
char *mailqueue = "mqueue/*.wrk";/* Outgoing mail work files */
char *routeqdir = "rqueue";	/* queue for router */
char *alias = "alias";			/* the alias file */
#ifdef REWRITE
char *Rewritefile = "rewrite.net";	/* note different name from NOS */
#endif
#ifdef AX25
char *helpbox = "helpbox.hlp";	/* help file for mbox */
#endif
char *public = "/public";		/* where downloadable files are */
#ifdef _FINGER
char *fingersuf = ".txt";		/* Text info for finger command */
char *fingerpath = "finger/";		/* Path to finger info files */
#endif
#endif

#if defined(AX25) && defined(MULPORT)
char *mdigilist = "digilist"; /* GRAPES mulport code */
char *mexlist = "exlist";
#endif

#ifdef	UNIX
char *homedir = ".";
char *netexe = "/usr/net/net";	/* where the binary lives for reset */
char *startup = "startup.net";	/* Initialization file */
/*char *config = "config.net";*/	/* Device configuration list - not used */
char *userfile = "ftpusers";
char *hosts = "hosts.net";
char *spool = "/usr/spool";		/* path to the mail files */
char *mailspool = "mail";
char *mailqdir = "mqueue";
char *mailqueue = "mqueue/*.wrk";
char *routeqdir = "rqueue";		/* queue for router */
char *alias = "aliases";			/* the alias file */
#ifdef REWRITE
char *Rewritefile = "rewrite.net";	/* note different name from NOS */
#endif
#ifdef AX25
char *helpbox = "helpbox.hlp";		/* mbox help file */
#endif
char *public = "/usr/public";		/* downloadable mbox files here */
#ifdef _FINGER
char *fingersuf = ".txt";		/* Text info for finger command */
char *fingerpath = "finger/";		/* Path to finger info files */
#endif
#endif

static char *logname;
static FILE *logfp;

extern char nospace[],hostname[];

char *
make_path(path_ptr,file_ptr,slash)
char *path_ptr;
char *file_ptr;
int slash;
{
	char *cp, *malloc();

	if ((cp = malloc(strlen(path_ptr) + strlen(file_ptr) + 2)) == NULLCHAR)
		perror(nospace);
	else {
		sprintf(cp, "%s%s%s", path_ptr,slash ? "/" : "",file_ptr);
		return(cp);
	}
	return(NULLCHAR);
}

/* found idea for this in sys5unix.c, but it wasn't completed - K5JB */
/* added SKIP_FSCHECK in k34
 */
void
fileinit(cmdarg)
char *cmdarg;
{
	char *ep;
	extern char *getenv();
#ifndef SKIP_FSCHECK
	int showstopper = 0;
	extern int exitval,doexit();
	struct stat statbuf;
	/* be sure and change these if you add tests below */
#define NOTEWORTHY 1
#define WARNING 2
#define SHOWSTOP 3

	struct reports {
		char *filepath;
		int type;
		};

	static struct reports rep[11];	/* BE CAREFUL to change this if you */
	struct reports *repp;			/* add tests below. It is tests + 1 */
	repp = rep;

#endif	/* SKIP_FSCHECK */

#ifdef UNIX
	if((ep = getenv("NETHOME")) != NULLCHAR ||
			(ep = getenv("HOME")) != NULLCHAR)
#else
	if((ep = getenv("NETHOME")) != NULLCHAR)
#endif
#ifndef SKIP_FSCHECK
	{
		homedir = make_path(ep,"",0);
		repp->filepath = homedir;
		(repp++)->type = SHOWSTOP;
	}else
		ep = homedir;

	/* Replace each of the file name strings with the complete path */
	if(cmdarg == NULLCHAR)
		startup = make_path(ep,startup,1);
	else
		startup = make_path(ep,cmdarg,1);
	repp->filepath = startup;
	(repp++)->type = WARNING;
	userfile = make_path(ep,userfile,1);
	repp->filepath = userfile;
	(repp++)->type = WARNING;
	hosts = make_path(ep,hosts,1);
	repp->filepath = hosts;
	(repp++)->type = WARNING;
	alias = make_path(ep,alias,1);
	repp->filepath = alias;
	(repp++)->type = NOTEWORTHY;
#ifdef AX25
	helpbox = make_path(ep,helpbox,1);
	repp->filepath = helpbox;
	(repp++)->type = NOTEWORTHY;
#endif
#ifdef _FINGER	/* note that stat() won't work on this one */
	fingerpath = make_path(ep,fingerpath,1);
#endif
#if defined(AX25) && defined(MULPORT)
	/* not too worried if these don't exist */
	mdigilist = make_path(ep,mdigilist,1); /* GRAPES mulport code */
	mexlist = make_path(ep,mexlist,1);
#endif
#ifdef REWRITE
	Rewritefile = make_path(ep,Rewritefile,1);
#endif
#ifdef UNIX
	netexe = make_path(ep,"net",1);
#endif
	if((ep = getenv("PUBLIC")) != NULLCHAR){
		public = make_path(ep,"",0);
		repp->filepath = public;
      /* not really a showstopper but is a directory */
		(repp++)->type = SHOWSTOP;
	}
	if((ep = getenv("NETSPOOL")) != NULLCHAR){
		spool = make_path(ep,"",0);
		repp->filepath = spool;	/* may revisit this after testing */
		(repp++)->type = SHOWSTOP;
	}
	mailspool = make_path(spool,mailspool,1);
	repp->filepath = mailspool;
	(repp++)->type = SHOWSTOP;
	mailqdir = make_path(spool,mailqdir,1);
	repp->filepath = mailqdir;	/* Make DAMN sure this doesn't violate mem */
	(repp++)->type = SHOWSTOP; /* See note where it is defined */
	mailqueue = make_path(spool,mailqueue,1);
	routeqdir = make_path(spool,routeqdir,1);

	for(repp = rep;repp->filepath != NULLCHAR; repp++){
		if(stat(repp->filepath,&statbuf) != 0){
			printf("%s: %s %s not found\n",repp->type == NOTEWORTHY ?
			"Note" : repp->type == WARNING ? "Warning" : "Error",
			repp->type == SHOWSTOP ? "Directory" : "File",
			repp->filepath);
			showstopper |= repp->type == SHOWSTOP;
			continue;
		}
		if(repp->type == SHOWSTOP && !(statbuf.st_mode & S_IFDIR)){
			printf("Error: %s is a file, not a directory\n",repp->filepath);
			showstopper |= repp->type == SHOWSTOP;
		}
	}
	if(showstopper){
		printf("\nSorry, Chapter 2 will help you set up correctly.\n");
		exitval = 1;
		doexit();
	}
#else	/* no file system checking */
		homedir = make_path(ep,"",0);
	else
		ep = homedir;

	/* Replace each of the file name strings with the complete path */
	if(cmdarg == NULLCHAR)
		startup = make_path(ep,startup,1);
	else
		startup = make_path(ep,cmdarg,1);

	userfile = make_path(ep,userfile,1);
	hosts = make_path(ep,hosts,1);
	alias = make_path(ep,alias,1);
#ifdef AX25
	helpbox = make_path(ep,helpbox,1);
#endif
#ifdef _FINGER	/* note that make_path() won't work on this one */
	fingerpath = make_path(ep,fingerpath,1);
#endif
#if defined(AX25) && defined(MULPORT)
	mdigilist = make_path(ep,mdigilist,1); /* GRAPES mulport code */
	mexlist = make_path(ep,mexlist,1);
#endif
#ifdef UNIX
	netexe = make_path(ep,"net",1);
#endif
	if((ep = getenv("PUBLIC")) != NULLCHAR)
		public = make_path(ep,"",0);
	if((ep = getenv("NETSPOOL")) != NULLCHAR)
		spool = make_path(ep,"",0);

	mailspool = make_path(spool,mailspool,1);
	mailqdir = make_path(spool,mailqdir,1);
	mailqueue = make_path(spool,mailqueue,1);
	routeqdir = make_path(spool,routeqdir,1);
#endif /* SKIP_FSCHECK */
}

int
dolog(argc,argv)
int argc;
char *argv[];
{
	void free();

	if(argc < 2){
		if(logname != NULLCHAR)
			printf("Log: %s. log off stops.\n",logname);
		else
			printf("Log off. log <fn> starts.\n");
		return 0;
	}
	if(logname != NULLCHAR){
		free(logname);
		logname = NULLCHAR;
	}
	if(strcmp(argv[1],"off") == 0)
		return 0;
	if(argv[1][0] == '/' || argv[1][0] == '\\')
		logname = make_path("",argv[1],0);
	else
		logname = make_path(homedir,argv[1],1);
	return 0;
}

int
dohelp1(argc,argv)
int argc;
char *argv[];
{
	void free();
	char *pp;
	int i;
	char helpname[16];	/* should be big enough even for Unix */
	char *defaulthelp = "net";
	int dohelpfile();
	int atoi();

	if(argc < 2)
		pp = defaulthelp;
	else
		pp = argv[1];

	for(i=0;i<11;i++){
		helpname[i] = pp[i];
		if(helpname[i] == '\0')
			break;
	}
	helpname[i] = '\0';	/* to be sure */
	strcat(helpname,".hlp");
	pp = make_path(homedir,helpname,1);
	if(pp != NULLCHAR){	/* ignore return value - argv[2] added k34 */
		dohelpfile(pp,argc > 2 ? LINES_PER_PAGE * atoi(argv[2]) : 0);
		free(pp);
	}
	return 0;
}

/* primitive way to peek at our mail, page at a time */
int
domail(argc,argv)
int argc;
char *argv[];
{
	void free();
	int atoi();
	char *pp;
	char *mfname;
	int i;
	char hostmail[16];	/* should be big enough even for Unix */

	mfname = argc < 2 ? hostname : argv[1];
	for(i=0;i<11;i++){
		if(mfname[i] == '.' || mfname[i] == '\0')
			break;
		hostmail[i] = mfname[i];
	}
	hostmail[i] = '\0';
	strcat(hostmail,".txt");
	pp = make_path(mailspool,hostmail,1);

	/* note, put this message in the command table where it should be */

	if(pp != NULLCHAR){
		i = dohelpfile(pp,argc > 2 ? LINES_PER_PAGE * atoi(argv[2]): 0);
		free(pp);
		return i;
	}
	return 0;	/* should we return error? */
}

int
dohelpfile(file,jump)
char *file;
int jump;
{
#define HFLINE 120	/* arbitrary, but this will keep
								cycles down on long lines */
	extern int Keyhit;
	FILE *hfile;
	extern char escape;
	extern char nospace[];
	char *hbuf;
	int i;
	void keep_things_going(),check_kbd(),free();
	extern char linebuf[];	/* these 2 are in ttydriv.c */
	extern char *kbcp;

	Keyhit = -1;
	if((hbuf = (char *)malloc(sizeof(char) * HFLINE)) == NULLCHAR){
		perror(nospace);
		return(0);
	}
	if((hfile = fopen(file,"r")) == NULLFILE){
		printf("Can't find %s\n",file);
		fflush(stdout);
		free(hbuf);
		return(-1);
	}
	if(jump)
		for(i=0;i<jump;i++)
			if(fgets(hbuf,HFLINE,hfile) == NULLCHAR){
				fclose(hfile);
				free(hbuf);
				return(0);
			}

	for(;;){
		for(i=0;i<LINES_PER_PAGE;i++){
			if(fgets(hbuf,HFLINE,hfile) == NULLCHAR || hbuf[0] == 0x1a){
				Keyhit = 0; /* fake an escape */
				break;
			}
			printf("%s",hbuf);	/* already has EOL appended */ /* k35 */
			fflush(stdout);
			keep_things_going();
		}
		if(Keyhit == 0)
			break;
		printf("\nHit any key to continue, ");

#ifdef UNIX
		printf("escape key to quit...");
#else
		printf("F10 to quit...");
#endif
		fflush(stdout);
		do{
			keep_things_going();
#ifdef UNIX
			Keyhit = kbread();
#else
			check_kbd();
#endif
		}while(Keyhit == -1);
#ifdef UNIX
		if(Keyhit == escape)
#else
		if(Keyhit == -2)	/* the F10 key */
#endif
			break;
		Keyhit = -1;
		printf("\n\n");
		fflush(stdout);
	}	/* for(ever) while there is a file and no escape */
	kbcp = linebuf;	/* gobble up those keys we've been hitting */
	printf("\n");
	fflush(stdout);
	fclose(hfile);
	free(hbuf);
	return(0);
}

/* If SOKNAME is defined, log messages of the form:
 * Mon Aug 26 14:32:17 1991 k5jb.okla.ampr:1003 open FTP
 * else:
 * Mon Aug 26 14:32:17 1991 44.78.0.2:1003 open FTP
 */

/*VARARGS2*/
void
log(tcb,fmt,arg1,arg2,arg3,arg4)
struct tcb *tcb;
char *fmt;
int arg1,arg2,arg3,arg4;
{
	time_t t,time();
	void rip();

	char *cp, *psocket();
#ifdef SOKNAME
	char *puname();
#endif

	if(logname == NULLCHAR)
		return;
	/* My Unix machine doesn't mind the "t" but some might */
#ifdef MSDOS
	if((logfp = fopen(logname,"a+t")) == NULLFILE)	/* can happen */
		return;
#else
	if((logfp = fopen(logname,"a+")) == NULLFILE)
		return;
#endif
	time(&t);
	cp = ctime(&t);
	cp[19] = '\0';	/* knock off the year, omit day below */
#ifdef AXMBX
	if(tcb == (struct tcb *)0)	/* from the mailbox */
		fprintf(logfp,"%s Mbox - ",cp+4);
	else
#endif
#ifdef SOKNAME
		fprintf(logfp,"%s %s - ",cp+4,puname(&tcb->conn.remote));
#else
		fprintf(logfp,"%s %s - ",cp+4,psocket(&tcb->conn.remote));
#endif
	fprintf(logfp,fmt,arg1,arg2,arg3,arg4);
	fprintf(logfp,"\n");
	fflush(logfp);
	fclose(logfp);
}

void
genlog(who,stuff)
char *who;
char *stuff;
{
extern FILE *logfp;
extern char *logname;
char *cp;
time_t t;

	if(logname == NULLCHAR)
		return;
#ifdef MSDOS
	if((logfp = fopen(logname,"a+t")) == NULLFILE)
		return;
#else
	if((logfp = fopen(logname,"a+")) == NULLFILE)
		return;
#endif
	time(&t);
	cp = ctime(&t);
	cp[19] = '\0';	/* shorten a bit */
	fprintf(logfp,"%s %s - %s\n",cp+4,who,stuff);
	fclose(logfp);
}

