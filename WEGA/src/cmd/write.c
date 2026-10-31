static char sccsid[] = "@[$]write.c\t2.1  01/05/83 17:36:32 - 87wega3.2";

/*
**	write to another user
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<signal.h>
#include	<utmp.h>

struct	utmp	ubuf;

int	signum[] = {SIGHUP, SIGINT, SIGQUIT, 0};
char	me[10]	= "???";
char	*them;
char	*mytty;
char	thatty[32];
char	*thattya;
char	*ttyname();
char	*strrchr();

int	logcnt;
int	eof();
int	timout();

FILE	*tf;

main(argc, argv)
char *argv[];
{
	struct	stat	stbuf;
	register i;
	register FILE *uf;
	int	c1, c2;

	if(argc < 2) {
		fprintf(stderr, "usage: write user [ttyname]\n");
		exit(2);
	}
	them = argv[1];
	if(argc > 2)
		thattya = argv[2];
	if ((uf = fopen("/etc/utmp", "r")) == NULL) {
		fprintf(stderr, "write: cannot open /etc/utmp\n");
		goto cont;
	}
	if ((mytty=ttyname(1)) == NULL)
		if ((mytty=ttyname(2)) == NULL)
			mytty = ttyname(0);
/*	Omitting this if statement insures that write can work
		in shell procedures
	if (mytty == NULL) {
		fprintf(stderr, "write: cannot find your tty\n");
		exit(2);
	}
*/
	if (mytty == NULL)
		mytty = "/tty???";
	mytty = strrchr(mytty, '/') + 1;
	if (thattya) {
		strcpy(thatty, "/dev/");
		strcat(thatty, thattya);
	}
	while (fread(&ubuf, sizeof(ubuf), 1, uf) == 1) {
		if (strcmp(ubuf.ut_line, mytty)==0) {
			for(i=0; i<8; i++) {
				c1 = ubuf.ut_name[i];
				if(c1 == ' ')
					c1 = 0;
				me[i] = c1;
				if(c1 == 0)
					break;
			}
		}
		if(them[0] != '-' || them[1] != 0)
		for(i=0; i<8; i++) {
			c1 = them[i];
			c2 = ubuf.ut_name[i];
			if(c1 == 0)
				if(c2 == 0 || c2 == ' ')
					break;
			if(c1 != c2)
				goto nomat;
		}
		logcnt++;
		if (thatty[0]==0) {
			strcpy(thatty, "/dev/");
			strcat(thatty, ubuf.ut_line);
		}
	nomat:
		;
	}
cont:
	if (logcnt==0 && thatty[0]=='\0') {
		fprintf(stderr,"%s is not logged in.\n", them);
		exit(2);
	}
	fclose(uf);
	if (thattya==0 && logcnt > 1) {
		fprintf(stderr,"%s is logged in more than once\nwriting to %s.\n", them, thatty+5);
	}
	if(thatty[0] == 0) {
		fprintf(stderr,"%s",them);
		if(logcnt)
			fprintf(stderr," is not on that tty.\n"); else
			fprintf(stderr," is not logged in.\n");
		exit(2);
	}
	if (access(thatty, 0) < 0) {
		fprintf(stderr, "write: \"%s\" -- ", thatty);
		fprintf(stderr, "no such tty.\n");
		exit(2);
	}
	signal(SIGALRM, timout);
	alarm(5);
	if ((tf = fopen(thatty, "w")) == NULL)
		goto perm;
	alarm(0);
	if (fstat(fileno(tf), &stbuf) < 0)
		goto perm;
	if ((stbuf.st_mode&02) == 0)
		goto perm;
	sigs(eof);
	fprintf(tf, "Message from ");
#ifdef interdata
	fprintf(tf, "(Interdata) " );
#endif
	fprintf(tf, "%s %s...\n\7\7\7", me, mytty);
	fflush(tf);
	for(;;) {
		char buf[128];
		i = read(0, buf, 128);
		if(i <= 0)
			eof();
		if(buf[0] == '!') {
			buf[i] = 0;
			ex(buf);
			continue;
		}
		write(fileno(tf), buf, i);
	}

perm:
	fprintf(stderr, "write: permission denied.\n");
	exit(2);
}

timout()
{

	fprintf(stderr, "write: timeout opening their tty.\n");
	exit(2);
}

eof()
{

	fprintf(tf, "EOF\n");
	exit(0);
}

ex(bp)
char *bp;
{
	register i;

	sigs(SIG_IGN);
	i = fork();
	if(i < 0) {
		printf("write: cannot fork -- try again.\n");
		goto out;
	}
	if(i == 0) {
		sigs(SIG_DFL);
		execl("/bin/sh", "sh", "-c", bp+1, 0);
		exit(2);
	}
	while(wait((int *)NULL) != i)
		;
	printf("!\n");
out:
	sigs(eof);
}

sigs(sig)
int (*sig)();
{
	register i;

	for(i=0;signum[i];i++)
		signal(signum[i],sig);
}
