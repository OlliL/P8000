/* chkout: make a version of a file from its chkfile (WEGA qcv(5), no System III source; reconstructed from /bin/chkout) */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include "chkout.h"

static char sccsid[] = "@[$]chkout.c\t1.6  07/23/82 18:25:34 - 87wega3.2";
char usage[] = "Usage: %s [-e] [-p] [-h] [-v rel.lev] [-d dir] file ...\n";

char lockname[512];
int eflag;
int hflag;
int pflag;
char *progname;
int nfiles;
int nlines;
int errors;
int kwflag;
char chkname[512];
char outname[512];
char *dir;

struct hist {			/* a version for -h */
	int	rel;
	int	lev;
	char	*who;
	struct comment *comments;
	struct hist *next;
};

struct comment {
	char	*text;
	struct comment *next;
};

struct passwd *getpwuid();
char *ctime();
char *malloc();
char *strcpy();

#define USAGE	{ fprintf(stderr, usage, progname); exit(1); }

main(argc, argv)
int argc;
char **argv;
{
	int rel, lev;

	rel = 0;
	lev = 0;
again:	/* not used; the original reads argv from its home here */
	progname = *argv++;
	argc--;
	if (argc == 0)
		USAGE
	while (argc > 0 && **argv == '-') {
		switch ((*argv)[1]) {
		case 'e':
			eflag++;
			break;
		case 'p':
			pflag++;
			break;
		case 'h':
			hflag++;
			break;
		case 'v':
			argc--;
			argv++;
			if (argc <= 0)
				USAGE
			getvers(*argv, &rel, &lev);
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
		argc--;
		argv++;
	}
	if (argc == 0)
		USAGE
	nfiles = argc;
	while (argc--) {
		nlines = 0;
		doit(*argv++, rel, lev);
	}
	exit(errors);
}

doit(name, rel, lev)
char *name;
{
	char *date;
	struct ver *vp;
	FILE *in;
	FILE *out;
	int fd;
	int lfd;
	int mode;
	int nerr;
	struct stat st;
	struct passwd *pw;

	if (setnames(name) == 0)
		return;
	if ((in = fopen(chkname, "r")) == NULL) {
		fprintf(stderr, "%s: cannot open %s\n", progname, chkname);
		errors++;
		return;
	}
	if (hflag) {
		history(in);
		fclose(in);
		return;
	}
	if (!pflag) {
		if (stat(lockname, &st) != -1 && eflag) {
			pw = getpwuid(st.st_uid);
			date = ctime(&st.st_mtime);
			fprintf(stderr, "%s: %s already checked out by %s on %s", progname, name, pw->pw_name, date);
			errors++;
			return;
		}
		if (stat(outname, &st) != -1) {
			if (st.st_mode & 0222) {
				fprintf(stderr, "%s: writable %s exists\n", progname, outname);
				errors++;
				return;
			}
			unlink(outname);
		}
		mode = eflag ? 0644 : 0444;
		if ((fd = creat(outname, mode)) == -1) {
			fprintf(stderr, "%s: cannot create %s\n", progname, outname);
			fclose(in);
			errors++;
			return;
		}
		out = fdopen(fd, "w");
	} else {
		out = stdout;
		if (nfiles > 1)
			printf("%s:\n", outname);
	}
	nerr = errors;
	vp = readchk(in, rel, lev);
	if (nerr == errors) {
		apply(in, out, vp, "");
		if (fflush(out) == -1) {
			fprintf(stderr, "%s: %s: write error\n", progname, outname);
			unlink(outname);
			exit(1);
		}
		if (eflag) {
			if ((lfd = creat(lockname, 0444)) == -1) {
				fprintf(stderr, "%s: cannot create lockfile %s\n", progname, lockname);
				unlink(outname);
				errors++;
				return;
			}
			close(lfd);
		}
		summary(vp);
	}
	fclose(in);
	if (out != stdout)
		fclose(out);
	freevers(vp);
}

history(fp)
FILE *fp;
{
	int rel, lev;
	char line[512];
	struct hist *hlist;
	struct hist *hp;
	struct comment *cp;
	struct comment **cpp;
	int n;
	char *s;

	rel = 0;
	hlist = 0;
	if (nfiles > 1)
		printf("%s:\n", outname);
	while (fgets(line, 512, fp)) {
		if (line[0] != '&')
			continue;
		switch (line[1]) {
		case 'R':
			rel++;
			lev = 0;
			break;
		case 'H':
			lev++;
			hp = (struct hist *)malloc(sizeof(struct hist));
			hp->rel = rel;
			hp->lev = lev;
			n = strlen(&line[3]) + 1;
			s = malloc(n);
			hp->who = strcpy(s, &line[3]);
			hp->comments = 0;
			hp->next = hlist;
			hlist = hp;
			cpp = &hp->comments;
			break;
		case 'C':
			cp = (struct comment *)malloc(sizeof(struct comment));
			n = strlen(&line[3]) + 1;
			s = malloc(n);
			cp->text = strcpy(s, &line[3]);
			cp->next = 0;
			*cpp = cp;
			cpp = &cp->next;
			break;
		case 'D':
			break;
		}
	}
	for (hp = hlist; hp; hp = hp->next) {
		printf("Version %d.%d by %s", hp->rel, hp->lev, hp->who);
		for (cp = hp->comments; cp; cp = cp->next)
			printf("%s", cp->text);
		printf("\n");
	}
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
	if (r == 0 || l == 0) {
		fprintf(stderr, "%s: %d.%d: bad version\n", progname, r, l);
		exit(1);
	}
	*rp = r;
	*lp = l;
}

summary(vp)
struct ver *vp;
{
	if (pflag)
		return;
	while (vp->next)
		vp = vp->next;
	if (nfiles > 1)
		printf("%s:\n", outname);
	printf("\tVersion %d.%d\n", vp->rel, vp->lev);
	printf("\t%d lines\n", nlines - 1);
	if (!eflag && !kwflag)
		printf("\tNo keywords substituted\n");
}

putline(in, out, vp, line)
FILE *in, *out;
struct ver *vp;
char *line;
{
	char *s;	/* not used */

	if (vp->next == 0) {
		if (*line == '&')
			line = line + 1;
		if (!eflag)
			keyword(out, vp->rel, vp->lev, line);
		else
			wfprintf(out, "%s", line);
		nlines++;
	} else
		apply(in, out, vp->next, line);
}

wfprintf(fp, fmt, a1, a2, a3, a4, a5, a6, a7)
FILE *fp;
char *fmt;
{
	fprintf(fp, fmt, a1, a2, a3, a4, a5, a6, a7);
	if (ferror(fp) != 0) {
		fprintf(stderr, "%s: %s: write error\n", progname, outname);
		unlink(outname);
		exit(1);
	}
}
