/* chkin: put a new version of a file into its chkfile (WEGA qcv(5), no System III source; reconstructed from /bin/chkin,
   the names are guessed) */
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

static char sccsid[] = "@[$]chkin.c\t2.2  09/12/83 21:59:34 - 87wega3.2";
char tmpl[] = "/tmp/chkinXXXXXX";
char tmpl1[] = "chkinXXXXXX";
char tmpl2[] = "chkinXXXXXX";
char usage[] = "Usage: %s [-r] [-b] [-v rel.lev] [-c comment] [-d dir] file ...\n";
int sigint = 0;
int sigterm = 0;
int sigquit = 0;
int sighup = 0;

int errors;
int kwflag;
char outname[512];
char lockname[512];
char chkname[512];
char *dir;
char *progname;
int rflag;
int bflag;
int vflag;
int currel;
int rel;
int lev;
char *comment;
char tmpname[100];
char tmpfile[18];
char tmp1[14];
char tmp2[14];

#include "chkout.h"

struct comment {
	char	*text;
	struct comment *next;
};

struct passwd *getpwuid();
char *getlogin();
char *ctime();
char *malloc();
char *strcpy();
char *basename();
long time();
FILE *popen();
int onintr();

#define USAGE	{ fprintf(stderr, usage, progname); exit(1); }

main(argc, argv)
int argc;
char **argv;
{
	progname = *argv++;
	argc--;
	if (argc == 0)
		USAGE
	while (argc > 0 && **argv == '-') {
		switch ((*argv)[1]) {
		case 'r':
			rflag++;
			break;
		case 'b':
			bflag++;
			break;
		case 'v':
			vflag++;
			argc--;
			argv++;
			if (argc <= 0)
				USAGE
			getvers(*argv, &rel, &lev);
			break;
		case 'c':
			argc--;
			argv++;
			if (argc <= 0)
				USAGE
			comment = *argv;
			break;
		case 'd':
			argc--;
			argv++;
			if (argc <= 0)
				USAGE
			dir = *argv;
			break;
		default:
			USAGE
		}
		argv++;
		argc--;
	}
	setsigs();
	if (argc == 0)
		USAGE
	while (argc--)
		doit(*argv++);
	exit(errors);
}

doit(name)
char *name;
{
	FILE *tf;
	FILE *cf;
	int r, l;
	struct ver *head;
	struct ver *vp;
	struct stat st;
	int ok;
	int bad;
	int pid;
	char *b;

	bad = 0;
	if (setnames(name) == 0)
		return;
	if (!*outname || access(outname, 0) == -1) {
		fprintf(stderr, "%s: cannot access file %s\n", progname, name);
		return;
	}
	strcpy(tmpname, chkname);
	dirname(tmpname);
	if (strcmp(tmpname, "") == 0)
		strcpy(tmpname, ".");
	pid = getpid();
	b = basename(outname);
	sprintf(tmpname, "%.8s%d", b, pid);
	ok = 1;
	if (stat(chkname, &st) == 0) {
		if (access(chkname, 2) == -1) {
			fprintf(stderr, "%s: cannot write to %s\n", progname, chkname);
			errors++;
			return;
		}
		if (stat(lockname, &st) == -1) {
			fprintf(stderr, "%s: no lock file for %s\n", progname, outname);
			errors++;
			return;
		}
		strcpy(tmpfile, tmpl);
		mktemp(tmpfile);
		if ((tf = fopen(tmpfile, "w")) == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", progname, tmpfile);
			errors++;
			return;
		}
		copyfile(chkname, tmpname);
		if ((cf = fopen(tmpname, "r")) == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", progname, tmpname);
			fclose(tf);
			errors++;
			return;
		}
		head = readchk(cf, 0, 0);
		apply(cf, tf, head, "");
		for (vp = head; vp->next; vp = vp->next)
			;
		currel = r = vp->rel;
		l = vp->lev + 1;
		if (bflag) {
			r++;
			l = 1;
		}
		if (vflag) {
			r = rel;
			l = lev;
		}
		freevers(head);
		fclose(tf);
		fclosechk(cf);
		ignsigs();
		bad = delta(tmpfile);
		setsigs();
		unlink(tmpfile);
		if (!bad)
			unlink(lockname);
	} else {
		if ((cf = fopen(tmpname, "w")) == NULL) {
			fprintf(stderr, "%s: cannot open %s\n", progname, tmpname);
			errors++;
			return;
		}
		ok = newfile(cf);
		fclosechk(cf);
		if (vflag) {
			r = rel;
			l = lev;
		} else {
			r = 1;
			l = 1;
		}
	}
	if (!bad) {
		if (ok) {
			if ((tf = fopen(chkname, "w")) == NULL) {
				fprintf(stderr, "%s: cannot truncate %s\n", progname, chkname);
				errors++;
				return;
			}
			fclose(tf);
			ignsigs();
			copyfile(tmpname, chkname);
		} else
			unlink(chkname);
	}
	unlink(tmpname);
	if (rflag) {
		if (unlink(outname) < 0)
			fprintf(stderr, "%s: cannot remove %s\n", progname, outname);
	} else if (ok)
		mkversion(outname, r, l);
	setsigs();
}

delta(name)
char *name;
{
	FILE *out;
	FILE *in;
	FILE *pp;
	char line[512];
	char cmd[512];

	if ((out = fopen(tmpname, "a")) == NULL) {
		fprintf(stderr, "%s: cannot append to %s\n", progname, tmpname);
		errors++;
		return (0);
	}
	header(out);
	fclosechk(out);
	if ((in = fopen(outname, "r")) == NULL) {
		fprintf(stderr, "%s: cannot open %s\n", progname, outname);
		errors++;
		return (0);
	}
	sprintf(cmd, "/bin/diff -f %s - >> %s", name, tmpname);
	pp = popen(cmd, "w");
	while (fgets(line, 512, in)) {
		if (line[0] == '&' || line[0] == '.')
			putc('&', pp);
		fprintf(pp, "%s", line);
	}
	fclose(in);
	if (pclose(pp) == 512)
		return (1);
	if ((out = fopen(tmpname, "a")) == NULL) {
		fprintf(stderr, "%s: unable to append to %s\n", progname, tmpname);
		errors++;
		return (0);
	}
	wfprintf(out, "&D\n");
	fclosechk(out);
	return (0);
}

header(fp)
FILE *fp;
{
	char *who;
	struct comment *cp;
	struct comment **cpp;
	char line[512];
	int i;
	long t;
	static int first = 1;
	static struct comment *clist = 0;

	if (bflag)
		wfprintf(fp, "&R\n");
	if (vflag)
		for (i = currel; i < rel; i++)
			wfprintf(fp, "&R\n");
	if ((who = getlogin()) == NULL)
		who = getpwuid(getuid())->pw_name;
	t = time(0);
	wfprintf(fp, "&H %s at %s", who, ctime(&t));
	if (comment) {
		wfprintf(fp, "&C %s\n", comment);
		return;
	}
	if (first && isatty(1)) {
		int j;
		int bad;
		int n;
		char *s;

		cpp = &clist;
		printf("COMMENTS:\n");
		printf("> ");
		while (fgets(line, 512, stdin)) {
			bad = 0;
			for (j = 0; line[j] != '\n'; j++)
				if (line[j] != '\t' && line[j] < ' ') {
					if (line[j] == 033)
						printf(" ");
					printf("invalid character - enter comment again\n> ");
					bad = 1;
					break;
				}
			if (bad)
				continue;
			cp = (struct comment *)malloc(sizeof(struct comment));
			cp->next = 0;
			n = strlen(line) + 1;
			s = malloc(n);
			cp->text = strcpy(s, line);
			*cpp = cp;
			cpp = &cp->next;
			printf("> ");
		}
		printf("END\n");
	}
	for (cp = clist; cp; cp = cp->next)
		wfprintf(fp, "&C %s", cp->text);
	first = 0;
}

newfile(fp)
FILE *fp;
{
	FILE *in;
	char line[512];
	int i;

	currel = 1;
	if ((in = fopen(outname, "r")) == NULL) {
		fprintf(stderr, "%s: unable to open %s\n", progname, outname);
		errors++;
		return (0);
	}
	wfprintf(fp, "&R\n");
	header(fp);
	wfprintf(fp, "a0\n");
	while (fgets(line, 512, in)) {
		if (line[0] == '.' || line[0] == '&')
			wfprintf(fp, "&%s", line);
		else {
			for (i = 0; i < 512; i++)
				if (line[i] == '\0' && line[i - 1] != '\n') {
					line[i] = '\n';
					line[i + 1] = '\0';
				}
			wfprintf(fp, "%s", line);
		}
	}
	wfprintf(fp, ".\n");
	wfprintf(fp, "&D\n");
	fclose(in);
	return (1);
}

putline(in, out, vp, line)
FILE *in, *out;
struct ver *vp;
char *line;
{
	if (vp->next == 0)
		wfprintf(out, "%s", line);
	else
		apply(in, out, vp->next, line);
}

mkversion(name, r, l)
char *name;
{
	FILE *out;
	FILE *in;
	char line[512];
	int mask;

	if ((in = fopen(name, "r")) == NULL) {
		fprintf(stderr, "%s: unable to open %s\n", progname, name);
		errors++;
		return;
	}
	strcpy(tmp1, tmpl1);
	strcpy(tmp2, tmpl2);
	mktemp(tmp1);
	mktemp(tmp2);
	if ((out = fopen(tmp1, "w")) == NULL) {
		fprintf(stderr, "%s: unable to open %s\n", progname, tmp1);
		errors++;
		fclose(in);
		return;
	}
	while (fgets(line, 512, in))
		keyword(out, r, l, line);
	fclose(out);
	fclose(in);
	link(name, tmp2);
	unlink(name);
	link(tmp1, name);
	unlink(tmp1);
	mask = umask(0);
	umask(mask);
	chmod(name, ~mask & 0444);
}

copyfile(from, to)
char *from, *to;
{
	FILE *in;
	FILE *out;
	int c;

	if ((out = fopen(to, "w")) == NULL) {
		fprintf(stderr, "%s: cannot open %s\n", progname, to);
		exit(1);
	}
	if ((in = fopen(from, "r")) == NULL) {
		fprintf(stderr, "%s: cannot open %s\n", progname, from);
		exit(1);
	}
	while ((c = getc(in)) != EOF) {
		putc(c, out);
		if (ferror(out) != 0) {
			fprintf(stderr, "%s: %s: disk I/O error\n", progname, to);
			unlink(to);
			exit(1);
		}
	}
	fclose(out);
	if (ferror(out) != 0) {
		fprintf(stderr, "%s: %s: disk I/O error\n", progname, to);
		unlink(to);
		exit(1);
	}
	fclose(in);
}

dirname(s)
char *s;
{
	char *q;

	q = s;
	for (; *s; s++)
		if (*s == '/')
			q = s;
	*q = '\0';
}

wfprintf(fp, fmt, a1, a2, a3, a4, a5, a6, a7)
FILE *fp;
char *fmt;
{
	fprintf(fp, fmt, a1, a2, a3, a4, a5, a6, a7);
	if (ferror(fp) != 0)
		ioerror();
}

fclosechk(fp)
FILE *fp;
{
	fclose(fp);
	if (ferror(fp) != 0)
		ioerror();
}

ioerror()
{
	fprintf(stderr, "%s: %s: disk I/O error\n", progname, outname);
	unlink(tmpfile);
	unlink(tmp1);
	unlink(tmp2);
	unlink(tmpname);
	exit(1);
}

onintr()
{
	signal(SIGINT, onintr);
	signal(SIGTERM, onintr);
	signal(SIGQUIT, onintr);
	signal(SIGHUP, onintr);
	unlink(tmpfile);
	unlink(tmp1);
	unlink(tmp2);
	unlink(tmpname);
	exit(0);
}

setsigs()
{
	if (signal(SIGINT, SIG_IGN) == SIG_DFL || sigint == 1) {
		sigint = 1;
		signal(SIGINT, onintr);
	}
	if (signal(SIGTERM, SIG_IGN) == SIG_DFL || sigterm == 1) {
		sigterm = 1;
		signal(SIGTERM, onintr);
	}
	if (signal(SIGQUIT, SIG_IGN) == SIG_DFL || sigquit == 1) {
		sigquit = 1;
		signal(SIGQUIT, onintr);
	}
	if (signal(SIGHUP, SIG_IGN) == SIG_DFL || sighup == 1) {
		sighup = 1;
		signal(SIGHUP, onintr);
	}
}

ignsigs()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
}

getvers(s, rp, lp)
char *s;
int *rp, *lp;
{
	int r, l;
	char *p;

	r = 0;
	l = 0;
	for (p = s; isdigit(*p); p++)
		r = r * 10 + *p - '0';
	if (*p++ != '.')
		USAGE
	for (; isdigit(*p); p++)
		l = l * 10 + *p - '0';
	if (r == 0 || l != 1) {
		fprintf(stderr, "%s: %d.%d: bad version\n", progname, r, l);
		exit(1);
	}
	*rp = r;
	*lp = l;
}
