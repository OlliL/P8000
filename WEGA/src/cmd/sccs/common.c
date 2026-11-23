/* common.c: reading a chkfile and making a version from it (WEGA qcv(5), no System III source; reconstructed from
   /bin/chkout, the names are guessed) */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "chkout.h"

static char sccsid[] = "@[$]common.c\t1.3  07/23/82 18:25:26 - 87wega3.2";

char *malloc();
char *basename();
long time();
struct tm *localtime();

struct ver *
readchk(fp, rel, lev)
FILE *fp;
{
	struct ver *head;
	struct ver **pp;
	struct ver *vp;
	int r, l;
	int body;
	long off;
	char line[512];

	head = 0;
	pp = &head;
	vp = 0;
	r = 0;
	body = 0;
	off = 0;
	for (; fgets(line, 512, fp); off += strlen(line)) {
		if (line[0] == '&') {
			switch (line[1]) {
			case 'R':
				r++;
				l = 0;
				break;
			case 'D':
				if (vp == 0) {
					fprintf(stderr, "%s: %s: corrupt chkfile\n", progname, chkname);
					exit(1);
				}
				vp->end = off;
				if (vp->start == 0) {
					vp->start = off;
					vp->pos = off;
				}
				body = 0;
				if (rel && rel == r && lev == l)
					return (head);
				break;
			case 'H':
				l++;
				vp = (struct ver *)malloc(sizeof(struct ver));
				vp->start = 0;
				vp->pos = 0;
				vp->next = 0;
				vp->cmd = 0;
				vp->curline = 0;
				vp->rel = r;
				vp->lev = l;
				*pp = vp;
				pp = &vp->next;
				break;
			case 'C':
				break;
			}
		} else if (!body) {
			vp->start = off;
			vp->pos = off;
			body = 1;
		}
	}
	if (rel) {
		fprintf(stderr, "%s: %s: version %d.%d not found\n", progname, outname, rel, lev);
		errors++;
	}
	return (head);
}

apply(in, out, vp, line)
FILE *in, *out;
struct ver *vp;
char *line;
{
	if (vp->cmd == 0)
		nextcmd(in, vp);
	if (vp->cmd != '*' && vp->curline >= vp->n1)
		switch (vp->cmd) {
		case 'a':
			putline(in, out, vp, line);
			copytext(in, out, vp);
			vp->cmd = 0;
			break;
		case 'd':
			if (vp->curline == vp->n2)
				vp->cmd = 0;
			break;
		case 'c':
			if (vp->curline == vp->n2) {
				copytext(in, out, vp);
				vp->cmd = 0;
			}
			break;
		}
	else
		putline(in, out, vp, line);
	vp->curline++;
}

nextcmd(in, vp)
FILE *in;
struct ver *vp;
{
	char line[512];
	char *p;
	int n;

	n = 0;
	if (vp->pos >= vp->end) {
		vp->cmd = '*';
		return;
	}
	getat(in, vp, line);
	p = line;
	if (*p != 'a' && *p != 'd' && *p != 'c') {
		fprintf(stderr, "%s: corrupt chkfile %s", progname, line);
		exit(1);
	} else
		vp->cmd = *p++;
	while (isdigit(*p)) {
		n = n * 10 + *p - '0';
		p++;
	}
	vp->n1 = n;
	if (*p == ' ') {
		p++;
		n = 0;
		while (isdigit(*p)) {
			n = n * 10 + *p - '0';
			p++;
		}
		vp->n2 = n;
	} else
		vp->n2 = vp->n1;
}

copytext(in, out, vp)
FILE *in, *out;
struct ver *vp;
{
	char line[512];

	while (getat(in, vp, line), strcmp(line, ".\n"))
		putline(in, out, vp, line);
}

getat(in, vp, line)
FILE *in;
struct ver *vp;
char *line;
{
	fseek(in, vp->pos, 0);
	fgets(line, 512, in);
	vp->pos += strlen(line);
}

char *
setnames(name)
char *name;
{
	int n;
	char *p;
	char *q;

	strcpy(chkname, name);
	n = strlen(chkname) - 1;
	if (chkname[n] != '+') {
		chkname[++n] = '+';
		chkname[n + 1] = '\0';
	}
	p = basename(chkname);
	if (&chkname[n + 1] - p > 14) {
		fprintf(stderr, "%s: %s: filename too long\n", progname, chkname);
		errors++;
		return (0);
	}
	if (dir) {
		q = basename(chkname);
		sprintf(outname, "%s/%s", dir, q);
	} else {
		q = basename(chkname);
		strcpy(outname, q);
	}
	n = strlen(outname) - 1;
	outname[n] = '\0';
	strcpy(lockname, chkname);
	n = strlen(lockname) - 1;
	lockname[n] = '-';
	if (notplain(chkname))
		return (0);
	if (notplain(outname))
		return (0);
	if (notplain(lockname))
		return (0);
	return (chkname);
}

char *
basename(s)
char *s;
{
	char *b, *p;

	for (b = p = s; *p; p++)
		if (*p == '/')
			b = p + 1;
	return (b);
}

notplain(name)
char *name;
{
	struct stat st;

	if (stat(name, &st) == 0 && (st.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: %s is not a plain file\n", progname, name);
		return (1);
	}
	return (0);
}

freevers(vp)
struct ver *vp;
{
	struct ver *n;

	while (vp) {
		n = vp->next;
		free(vp);
		vp = n;
	}
}

keyword(out, rel, lev, line)
FILE *out;
char *line;
{
	register char *p;
	long t;
	struct tm *tm;
	char *what;

	what = "@[$]";
	for (p = line; *p; p++)
		if (*p == '$')
			break;
	if (*p == 0) {
		wfprintf(out, "%s", line);
		return;
	}
	p = line;
	goto test;
loop:
	if (*p == '$' && p[2] == '$') {
		switch (p[1]) {
		case 'R':
			wfprintf(out, "%d.%d", rel, lev);
			break;
		case 'M':
			wfprintf(out, "%s", outname);
			break;
		case 'D':
			t = time(0);
			tm = localtime(&t);
			wfprintf(out, "%02d/%02d/%02d %02d:%02d:%02d", tm->tm_mon + 1, tm->tm_mday, tm->tm_year,
			    tm->tm_hour, tm->tm_min, tm->tm_sec);
			break;
		case 'Z':
			wfprintf(out, "%s", what);
			break;
		case 'W':
			wfprintf(out, "%s%s\t%d.%d", what, outname, rel, lev);
			break;
		default:
			wfprintf(out, "%c", *p);
			goto next;
		}
		kwflag++;
		p += 3;
		goto test;
	} else
		wfprintf(out, "%c", *p);
next:
	p++;
test:
	if (*p)
		goto loop;
}
