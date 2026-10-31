static char sccsid[] = "@[$]mv.c\t2.2  06/30/83 13:46:19 - 87wega3.2";

/*
** Combined mv/cp/ln command:
**	mv file1 file2
**	mv dir1 dir2
**	mv file1 ... filen dir1
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>

#define EQ(x,y)	!strcmp(x,y)
#define	DOT	"."
#define	DOTDOT	".."
#define	DELIM	'/'
#define	MAXN	128
#define MODEBITS 07777

char	*pname();
char	*dname();
char	*strrchr();
extern	errno;

struct	stat s1, s2;

char	*cmd;
char	errbuf[128];		/* the message for perror */
int	f_flag = 0;

main(argc, argv)
register char *argv[];
{
	register i, r;

	if (cmd = strrchr(argv[0], '/'))
		++cmd;
	else
		cmd = argv[0];
	if (!EQ(cmd, "cp") && !EQ(cmd, "mv") && !EQ(cmd, "ln")) {
		fprintf(stderr, "%s: command must be named cp | mv | ln -- defaults to `cp'\n", cmd);
		cmd = "cp";
	}

	if (EQ(argv[1], "-f"))
		++f_flag, --argc, ++argv;

	if (argc < 3)
		usage();
	stat(argv[1], &s1);
	if ((s1.st_mode & S_IFMT) == S_IFDIR) {
		if (!EQ(cmd, "mv"))
			usage();
		if (argc != 3)
			usage();
		exit(mvdir(argv[1], argv[2]));
	}
	setuid(getuid());
	if (argc > 3)
		if (stat(argv[argc-1], &s2) < 0) {
			fprintf(stderr, "%s: %s not found\n", cmd, argv[argc-1]);
			exit(2);
		} else if ((s2.st_mode & S_IFMT) != S_IFDIR)
			usage();
	r = 0;
	for (i=1; i<argc-1; i++)
		r += move(argv[i], argv[argc-1]);
	exit(r?2:0);
}

move(source, target)
char *source, *target;
{
	int r;
	register c, i;
	char	buf[MAXN];

	if (stat(source, &s1) < 0) {
		sprintf(errbuf, "%s: cannot access %s", cmd, source);
		perror(errbuf);
		return(1);
	}
	if ((s1.st_mode & S_IFMT) == S_IFDIR) {
		fprintf(stderr, "%s: %s directory\n", cmd, source);
		return(1);
	}
	if (EQ(cmd, "mv") && access(pname(source), 2) == -1) {
		sprintf(errbuf, "%s: cannot unlink %s", cmd, source);
		perror(errbuf);
		return(1);
	}
	s2.st_mode = S_IFREG;
	if (stat(target, &s2) >= 0) {
		if ((s2.st_mode & S_IFMT) == S_IFDIR) {
			sprintf(buf, "%s/%s", target, dname(source));
			target = buf;
		}
		if (stat(target, &s2) >= 0) {
			if (s1.st_dev==s2.st_dev && s1.st_ino==s2.st_ino) {
				fprintf(stderr, "%s: %s and %s are identical\n", cmd,
						source, target);
				return(1);
			}
			if(EQ(cmd, "cp"))
				goto skip;
			if (access(target, 2) < 0 && isatty(fileno(stdin))
			&& !f_flag) {
				fprintf(stderr, "%s: %s: %o mode ", cmd, target,
					s2.st_mode & MODEBITS);
				i = c = getchar();
				while (c != '\n' && c != EOF)
					c = getchar();
				if (i != 'y')
					return(1);
			}
			if (unlink(target) < 0) {
				sprintf(errbuf, "%s: cannot unlink %s", cmd, target);
				perror(errbuf);
				return(1);
			}
		}
	}
skip:
	return(r = copy(source, target));
}

copy(source, target)
char *source, *target;
{
	int from, to, ct, oflg;
	char fbuf[512];

	if (EQ(cmd, "cp") || link(source, target) < 0) {
		if (EQ(cmd, "ln")) {
			if(errno == EXDEV)
				fprintf(stderr, "%s: different file system\n", cmd);
			else
				fprintf(stderr, "%s: no permission for %s\n", cmd, target);
			return(1);
		}
		if((from = open(source, 0)) < 0) {
			sprintf(errbuf, "%s: cannot open %s", cmd, source);
			perror(errbuf);
			return 1;
		}
		oflg = access(target, 0) == -1;
		if((to = creat (target, 0666)) < 0) {
			sprintf(errbuf, "%s: cannot create %s", cmd, target);
			perror(errbuf);
			close(from);
			return 1;
		}
		while((ct = read(from, fbuf, 512)) > 0)
			if(ct < 0 || write(to, fbuf, ct) != ct) {
				sprintf(errbuf, "%s: bad copy to %s", cmd, target);
				perror(errbuf);
				if((s2.st_mode & S_IFMT) == S_IFREG)
					unlink(target);
				close(from);
				close(to);
				return 1;
			}
		close(from), close(to);
		if (oflg)
			chmod(target, s1.st_mode);
	}
	if (!EQ(cmd, "mv"))
		return 0;
	if (unlink(source) < 0) {
		sprintf(errbuf, "%s: cannot unlink %s", cmd, source);
		perror(errbuf);
		return(1);
	}
	return(0);
}

mvdir(source, target)
char *source, *target;
{
	register char *p;

	if (stat(target, &s2) >= 0) {
		fprintf(stderr, "%s: directory %s exists\n", cmd, target);
		return(2);
	}
	if (EQ(source, target)) {
		fprintf(stderr, "%s: source == target, source exists and target does not\n", cmd);
		return(2);
	}
	p = dname(source);
	if (EQ(p, DOT) || EQ(p, DOTDOT) || !*p || p[strlen(p)-1]=='/') {
		sprintf(errbuf, "%s: cannot rename %s", cmd, p);
		perror(errbuf);
		return(2);
	}
	p = pname(source);
	if (stat(p, &s1) < 0 || stat(pname(target), &s2) < 0) {
		fprintf(stderr, "%s: cannot locate parent\n", cmd);
		return(2);
	}
	if (s1.st_dev != s2.st_dev || s1.st_ino != s2.st_ino) {
		fprintf(stderr, "%s: directory rename only\n", cmd);
		return(2);
	}
	if (access(p, 2) < 0) {
		sprintf(errbuf, "%s: no write access to %s", cmd, p);
		perror(errbuf);
		return(2);
	}
	if (link(source, target) < 0) {
		sprintf(errbuf, "%s: cannot link %s and %s", cmd,
			source, target);
		perror(errbuf);
		return(2);
	}
	if (unlink(source) < 0) {
		sprintf(errbuf, "%s: cannot unlink %s", cmd, source);
		perror(errbuf);
		return(2);
	}
	return(0);
}

char *
pname(name)
register char *name;
{
	register c;
	register char *p, *q;
	static	char buf[MAXN];

	p = q = buf;
	while (c = *p++ = *name++)
		if (c == DELIM)
			q = p-1;
	if (q == buf && *q == DELIM)
		q++;
	*q = 0;
	return buf[0]? buf : DOT;
}

char *
dname(name)
register char *name;
{
	register char *p;

	p = name;
	while (*p)
		if (*p++ == DELIM && *p)
			name = p;
	return name;
}

usage()
{
	switch (*cmd) {
	case 'c':
		fprintf(stderr, "%s%s", "Usage: cp file1 file2\n", "       cp f1...fn dir\n");
		break;
	case 'm':
		fprintf(stderr, "%s%s%s", "Usage: mv file1 file2\n", "       mv f1...fn dir\n",
			"       mv dir1 dir2\n");
		break;
	case 'l':
		fprintf(stderr, "Usage: ln name1 name2\n");
		break;
	}
	exit(2);
}
