/*
 * uusend: the uusend of 2.11BSD (RECOVER, no DEBUG, no RUUSEND) as WEGA changed it: -fname in the usage line,
 * a directory as source is refused ("no such file as"), uux without -gn -z, /usr/lib/uucp and
 * /usr/spool/uucp/uucppublic/, makedir without the mode (chmod in main). No sccs string.
 */

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * define RECOVER to permit requests like 'uusend file sys1!sys2!~uucp'
 * (abbreviation for 'uusend file sys1!sys2!~uucp/file').
 * define DEBUG to keep log of uusend uusage.
 * define RUUSEND if neighboring sites permit 'ruusend',
 * which they certainly should to avoid security holes
 */
#define	RECOVER
/*#define	DEBUG	"/usr/spool/uucp/uusend.log"/**/

FILE	*in, *out;
FILE	*dout;

extern FILE	*popen();
extern char	*index(), *strcpy(), *strcat(), *ctime();

int	mode = -1;	/* mode to chmod new file to */
char	*nextsys;	/* next system in the chain */
char	dnbuf[200];	/* buffer for result of ~user/file */
char	cmdbuf[256];	/* buffer to build uux command in */
char	*rflg = "";	/* default value of rflg  ccw -- 1 Nov '82 */

struct	passwd *user;	/* entry  in /etc/passwd for ~user */
struct	passwd *getpwnam();
struct	stat	stbuf;

char	*excl;		/* location of first ! in destname */
char	*sl;		/* location of first / in destname */
char	*sourcename;	/* argv[1] */
char	*destname;	/* argv[2] */
char	*UULIB = "/usr/lib/uucp";	  /* UUCP lib directory */

char	*UUPUB = "/usr/spool/uucp/uucppublic/";  /* public UUCP directory */
char	*filename;	/* file name from end of destname */
char	*getfname();	/* routine to get filename from destname */
int	fflg;
char	f[100];		/* name of default output file */

main(argc, argv)
int	argc;
char	**argv;
{
	register int c;
	long count = 0;
	extern char **environ;


	while (argc > 1 && argv[1][0] == '-' && argv[1][1]) {
		switch(argv[1][1]) {
		case 'm':
			sscanf(argv[2], "%o", &mode);
			mode &= 0777;  /* fix set[ug]id loophole */
			argc--; argv++;
			break;
		case 'r':		/* -r flag for uux */
			rflg = "-r ";
			break;
		case 'f':
			fflg++;
			strcpy(f, argv[1]);
			break;
		default:
			fprintf(stderr, "Bad flag: %s\n", argv[1]);
			break;
		}
		argc--; argv++;
	}

	if (argc != 3) {
		fprintf(stderr, "Usage: uusend [-fname] [-m ooo] [-r] -/file sys!sys!..!rfile\n");
		exit(1);
	}

	sourcename = argv[1];
	destname = argv[2];

	if (sourcename[0] == '-')
		in = stdin;
	else {
		if (stat(sourcename, &stbuf) == 0 && (stbuf.st_mode & S_IFMT) == S_IFDIR) {
			fprintf(stderr, "no such file as %s\n", sourcename);
			exit(6);
		}
		in = fopen(sourcename, "r");
		if (in == NULL) {
			perror(argv[1]);
			exit(2);
		}
		if (!fflg || f[2] == '\0') {
			strcpy(f, "-f");
			strcat(f, getfname(sourcename));
			fflg++;
		}
	}

	excl = index(destname, '!');
	if (excl) {
		/*
		 * destname is on a remote system.
		 */
		nextsys = destname;
		*excl++ = 0;
		destname = excl;
		if (mode < 0) {
			fstat(fileno(in), &stbuf);
			mode = stbuf.st_mode & 0777;
		}
		sprintf(cmdbuf, "uux %s - \"%s!uusend %s -m %o - (%s)\"",
			rflg, nextsys, f, mode, destname);
		out = popen(cmdbuf, "w");
	} else {
		/*
		 * destname is local.
		 */
		if (destname[0] == '~') {
			sl = index(destname, '/');
			if (sl == NULL && !fflg) {
				fprintf(stderr, "Illegal ~user\n");
				exit(3);
			}
			for (sl = destname; *sl != '\0'; sl++)
				;	/* boy, is this a hack! */
			user = getpwnam(destname+1);
			if (user == NULL) {
				fprintf(stderr, "No such user as %s\n",
					destname);
				if ((filename =getfname(sl)) == NULL &&
				     !fflg)
					exit(4);
				strcpy(dnbuf, UUPUB);
				if (fflg)
					strcat(dnbuf, &f[2]);
				else
					strcat(dnbuf, filename);
			}
			else {
				strcpy(dnbuf, user->pw_dir);
				strcat(dnbuf, "/");
				strcat(dnbuf, sl);
			}
			destname = dnbuf;
		}
		else
			destname = strcpy(dnbuf, destname);
		if(strncmp(UULIB, destname, strlen(UULIB)) == 0) {
			fprintf(stderr, "illegal file: %s", destname);
			exit(4);
		}
		if (stat(destname, &stbuf) == 0 &&
		    (stbuf.st_mode & S_IFMT) == S_IFDIR &&
		     fflg) {
			strcat(dnbuf, "/");
			strcat(dnbuf, &f[2]);
		}
		out = fopen(destname, "w");
		if (out == NULL) {
			perror(destname);
			if (strncmp(destname,UUPUB,strlen(UUPUB)) == 0)
				exit(5);	/* forget it! */
			filename = getfname(destname);
			if (destname == dnbuf) /* cmdbuf is scratch */
				filename = strcpy(cmdbuf, filename);
			destname = strcpy(dnbuf, UUPUB);
			if (user != NULL) {
				strcat(destname, user->pw_name);
				if (stat(destname, &stbuf) == -1) {
					makedir(destname);
					chmod(destname, 0777);
				}
				strcat(destname, "/");
			}
			if (fflg)
				strcat(destname, &f[2]);
			else
				strcat(destname, filename);
			if ((out = fopen(destname, "w")) == NULL)
				exit(5); /* all for naught! */
		}
		if (mode > 0)
			chmod(destname, mode);	/* don't bother to check it */
	}

	/*
	 * Now, in any case, copy from in to out.
	 */

	while ((c=getc(in)) != EOF) {
		putc(c, out);
		count++;
	}

	fclose(in);
	fclose(out);	/* really should pclose in that case */
	exit(0);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found.  Included so I don't have to fight the
 * index/strchr battle.
 */

#define	NULL	0

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}

char *
getfname(p)
register char *p;
{
	register char *s;
	s = p;
	while (*p != '\0')
		p++;
	if (p == s)
		return (NULL);
	for (;p != s; p--)
		if (*p == '/') {
			p++;
			break;
		}
	return (p);
}

makedir(dirname)
char *dirname;
{
	register int pid;
	int retcode, status;
	switch ((pid = fork())) {
	    default:		/* parent */
		while ((retcode=wait(&status)) != pid && retcode != -1)
			;
		break;
	    case -1:		/* error */
		return (-1);
	    case 0:		/* child */
		umask(0);
		execl("/bin/mkdir", "mkdir", dirname, (char *)0);
		exit(1);
		return;
	}
	if (retcode == -1)
		return  -1;
	else
		return status;
}
