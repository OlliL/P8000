/*
 * login [ name ] [ hangup ]
 * if CONSOLE is defined, then uid 0 may only login from that device
 * if NOSHELL is defined, then SHELL will not be put in the environment
 * for non-standard login "shells"
 *
 * WEGA (login.c 2.3 07/26/83): System III login.c with the node name in the prompt, a - before the name
 * for no motd, setenv (HOME SHELL PATH LOGNAME TERM over the environment of getty, the type from ttytype.c),
 * /bin/csh, the motd by fgets and the mail check active.
 */

#include <sys/types.h>
#include <termio.h>
#include <utmp.h>
#include <signal.h>
#include <pwd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#define SCPYN(a, b)	strncpy(a, b, sizeof(a))
#ifdef PWB
#define CONSOLE	"/dev/console"
#endif

#undef	CONSOLE

static char sccsid[] = "@[$]login.c\t2.3  07/26/83 17:28:31 - 87wega3.2";
char	maildir[20] =	"/usr/spool/mail/";
struct	passwd nouser = {"", "nope"};
struct	termio ttyb;
struct	utmp utmp;
#define	NENV	5
char	newenv[NENV][100] = {
	"HOME=",
	"SHELL=",
	"PATH=:/bin:/usr/bin:/z/bin",
	"LOGNAME=",
	"TERM=",
};
#define	homedir	newenv[0]
#define	shell	newenv[1]
#define	logname	newenv[3]
#define	term	newenv[4]
char	minusnam[16] = "-";
int	stopmotd;
struct	passwd *pwd;

struct	passwd *getpwnam();
int	setpwent();
char	*ttyname();
char	*crypt();
char	*getpass();
char	*strrchr();
int	bailout();
extern	char **environ;

#define	WEEK	(24L * 7 * 60 * 60) /* 1 week in seconds */
time_t	when;
time_t	maxweeks;
time_t	minweeks;
time_t	now;
long 	a64l();
long	time();

main(argc, argv)
char **argv;
{
	register char *namep;
	int t, f, c, quiet;
	char *ttyn;
	int nopassword;
	struct	stat	sbuf;
	char prompt[50];
	struct	utsname	uts;

	alarm(argc>2?atoi(argv[2]):60);
	signal(SIGQUIT, 1);
	signal(SIGINT, 1);
	signal(SIGALRM, bailout);
	nice (-nice(0));
	ttyn = ttyname(0);
	if (ttyn==0)
		ttyn = "/dev/tty??";

    loop:
	SCPYN(utmp.ut_name, "");
	if (argc>1) {
		if (*argv[1] == '-') {
			quiet++;
			argv[1]++;
		}
		SCPYN(utmp.ut_name, argv[1]);
		argc = 0;
	}
	uname(&uts);
	while (utmp.ut_name[0] == '\0') {
		namep = utmp.ut_name;
		strcpy(prompt, uts.nodename);
		strcat(prompt, " login: ");
		printf("%s", prompt);
		if ((c = getchar()) == '-') {
			quiet++;
			c = getchar();
		}
		if (c == 0) {
			printf("\n");
			continue;
		}
		while (c != '\n') {
			if(c == ' ')
				c = '_';
			if (c == EOF)
				exit(0);
			if (namep < utmp.ut_name+8)
				*namep++ = c;
			c = getchar();
		}
	}
	setpwent();
	if ((pwd = getpwnam(utmp.ut_name)) == NULL)
		pwd = &nouser;
	endpwent();
	nopassword = 1;
	if (*pwd->pw_passwd != '\0') {
		nopassword = 0;
		namep = crypt(getpass("Password:"),pwd->pw_passwd);
		if (strcmp(namep, pwd->pw_passwd)) {
			printf("Login incorrect\n");
			goto loop;
		}
	}
	time(&utmp.ut_time);
	if ((f = open("/etc/utmp", 2)) >= 0) {
		strcpy(utmp.ut_line, strrchr(ttyn, '/')+1);
		if(utscan(f, utmp.ut_line)==0)
		{
			printf("No entry in utmp\n");
			exit(5);
		}
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	if ((f = open("/usr/adm/wtmp", O_WRONLY|O_APPEND)) >= 0) {
		write(f, (char *)&utmp, sizeof(utmp));
		close(f);
	}
	chown(ttyn, pwd->pw_uid, pwd->pw_gid);
	if(*pwd->pw_shell == '*')
	{
		if(chroot(pwd->pw_dir) < 0)
		{
			printf("No Root Directory\n");
			goto loop;
		}
		printf("Subsystem root: %s\n",pwd->pw_dir);
		execl("/etc/login", "login", 0);
		execl("/bin/login", "login", 0);
		printf("No login in /etc or /bin on root\n");
		exit(11);
	}
	if((setgid(pwd->pw_gid) != 0) || (setuid(pwd->pw_uid) != 0)) {
		printf("Invalid ID\n");
		exit(2);
	}
	if(chdir(pwd->pw_dir) < 0) {
		printf("No directory\n");
		exit(2);
	}
	if(pwd->pw_uid == 0)
	{
#ifdef CONSOLE
		if(strcmp(ttyn, CONSOLE))
		{
			printf("Not on system console\n");
			exit(10);
		}
#endif
	}
	if (*pwd->pw_shell == '\0')
		pwd->pw_shell = "/bin/csh";
	if ((namep = strrchr(pwd->pw_shell, '/')) == NULL)
		namep = pwd->pw_shell;
	else
		namep++;
	strcat(minusnam, namep);
	alarm(0);
	if (nopassword) {
		printf("You don't have a password.  Choose one.\n");
		execl("/bin/passwd", "passwd", utmp.ut_name, 0);
	}

/* is the age of the password to be checked? */
	if (*pwd->pw_age != NULL) {
		/* retrieve (a) week of previous change; 
			(b) maximum number of valid weeks	*/
		when = (long) a64l (pwd->pw_age);
		/* max, min and weeks since last change are packed radix 64 */
		maxweeks = when & 077;
		minweeks = (when >> 6) & 077;
		when >>= 12;
		now  = time(0)/WEEK;
		if ((now > when + maxweeks) && (maxweeks >= minweeks)) {
			printf ("Your password has expired. Choose a new one\n");
			execl("/bin/passwd", "passwd", utmp.ut_name, 0);
			exit(9);
		}
	}
	if (!quiet)
		showmotd();
	strcat(maildir, pwd->pw_name);
	if(stat(maildir, &sbuf) >= 0)
		if(sbuf.st_size > 0)
			printf("You have mail.\n");
	signal(SIGQUIT, 0);
	signal(SIGINT, 0);
	setenv();
	execlp(pwd->pw_shell, minusnam, 0);
	printf("No shell\n");
	exit(7);
}

catch()
{
	signal(SIGINT, 1);
	stopmotd++;
}

showmotd()
{
	FILE *mf;
	char buf[512];

	signal(SIGINT, catch);
	if((mf = fopen("/etc/motd","r")) != NULL) {
		while(fgets(buf, 512, mf) != NULL) {
			if (stopmotd)
				break;
			fputs(buf, stdout);
		}
		fclose(mf);
	}
	signal(SIGINT, 1);
}

bailout () {
/* invoked after SIGALRM */
	struct termio s;
	s.c_cflag = B0|HUPCL;
	ioctl(0, TCSETA, &s);
	exit (1);
}
utscan(utf,lnam)
register utf;
char *lnam;
{
	register i;
	struct utmp wtmp;

	lseek(utf, (long)0, 0);
	while(read(utf, (char *)&wtmp, sizeof(wtmp)) == sizeof(wtmp))
	{
		for(i=0; i<8; i++)
			if(wtmp.ut_line[i] != lnam[i])
				goto contin;
		lseek(utf, -(long)sizeof(wtmp), 1);
		return(1);
		contin:;
	}
	return(0);
}

char	**findenv();

/* put HOME, SHELL, PATH, LOGNAME and TERM into the environment of getty */
setenv()
{
	register i;
	register char **ep;
	char *ttyn, *type;

	strcat(homedir, pwd->pw_dir);
	strcat(shell, pwd->pw_shell);
	strcat(logname, pwd->pw_name);
	ttyn = ttyname(0);
	type = gettype(ttyn);
	strcat(term, type);
	for (i = 0; i < NENV; i++)
		if ((ep = findenv(newenv[i])) != 0)
			*ep = newenv[i];
}

char **
findenv(name)
char *name;
{
	register char **ep;

	for (ep = environ; *ep; ep++)
		if (envcmp(name, *ep))
			return(ep);
	return(0);
}

envcmp(s1, s2)
char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(1);
	if (*s1 == '\0' && s2[-1] == '=')
		return(1);
	return(0);
}
