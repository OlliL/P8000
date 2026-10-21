/*
 * main - options, the temporary files and the two passes
 *
 * Files: i (the intermediate file written by pass 1), s (the symbols, sym.out),
 * r (relocation, r.out), m (the output symbols, osym.out), the object file
 * (a.out) and, with -l, the listing. Unless an option asks to keep a file
 * (-i, -t, -r, -m) it is a temporary file that is removed at once after it
 * was opened. The source is read from stdin; with a file name argument it
 * is reopened as stdin.
 */
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include "cas.h"

char *outname = "a.out";		/* -o */
static char ibuf[PATH_MAX], lbuf[PATH_MAX], rbuf[PATH_MAX], sbuf[PATH_MAX], obuf[PATH_MAX], tbuf[PATH_MAX];
char *ifname = ibuf;
char *rfname = rbuf;
char *sfname = sbuf;
char *ofname = obuf;
static char *lfname = lbuf;
int uflag = 0;				/* -u: undefined symbols are external */
int vflag = 0;				/* -v: messages about the passes */
int echoflag = 0;			/* -^: echo the source */
int dflag = 0;				/* -d: internal labels into the symbol table */
int lflag = 0;				/* -l: listing */
int synerr = 0;				/* errors of pass 1 */
int semerr = 0;				/* semantic errors */

FILE *outf;
FILE *relf;
FILE *listf;
static char *srcname;
static void (*oldint)(int);
static void (*oldquit)(int);
static void (*oldterm)(int);

/* the end of the program */
void
done(int code)
{
	if (vflag && code != -1)
		printf("Assembly Complete\n");
	if (code == -1)
		fprintf(stderr, "Assembler Abort\n");
	signal(SIGINT, oldint);
	signal(SIGQUIT, oldquit);
	signal(SIGTERM, oldterm);
	exit(code);
}

static void
sigdone(int sig)
{
	(void)sig;
	done(-1);
}

/* a file that is read and written: a temporary file, removed at once, or a named one that stays */
static FILE *
openfile(char **namep, char *buf, const char *tmpl, int keep, int derive, const char *dflt, int suffix)
{
	FILE *f;
	int fd;

	if (keep && derive) {
		strcpy(buf, srcname);
		buf[strlen(buf) - 1] = suffix;
		*namep = buf;
	} else if (keep) {
		strcpy(buf, dflt);
		*namep = buf;
	} else {
		snprintf(buf, PATH_MAX, "%s", tmpl);
		*namep = buf;
		fd = mkstemp(buf);
		if (fd == -1) {
			perror(buf);
			done(-1);
		}
		f = fdopen(fd, "w+");
		if (f == 0 || unlink(buf) == -1) {
			perror(buf);
			done(-1);
		}
		return (f);
	}
	unlink(buf);
	f = fopen(buf, "w+");
	if (f == 0) {
		perror(buf);
		done(-1);
	}
	return (f);
}

int
main(int argc, char **argv)
{
	int i, rflag, iflag, tflag, mflag, nfiles;
	char errbuf[80];

	tflag = rflag = 0;
	mflag = iflag = 0;
	nfiles = 0;
	oldint = signal(SIGINT, sigdone);
	if (oldint == SIG_IGN)
		signal(SIGINT, SIG_IGN);
	oldterm = signal(SIGTERM, sigdone);
	if (oldterm == SIG_IGN)
		signal(SIGTERM, SIG_IGN);
	oldquit = signal(SIGQUIT, sigdone);
	if (oldquit == SIG_IGN)
		signal(SIGQUIT, SIG_IGN);
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'o':
				if (++i < argc)
					outname = argv[i];
				break;
			case 'i':
				iflag++;
				break;
			case 'r':
				rflag++;
				break;
			case 't':
				tflag++;
				break;
			case 'm':
				mflag++;
				break;
			case 'l':
				lflag++;
				break;
			case 'u':
				uflag++;
				break;
			case 'v':
				vflag++;
				break;
			case '^':
				echoflag++;
				break;
			case 'd':
				dflag++;
				break;
			default:
				snprintf(errbuf, sizeof errbuf, "invalid option -%c\n", argv[i][1]);
				error(4, errbuf);
			}
		} else {
			srcname = argv[i];
			nfiles++;
		}
	}
	if (vflag)
		printf("U8000 assembler\n");

	/* with a listing the source is read twice: from stdin it is copied to a temporary file first */
	if (lflag && nfiles == 0) {
		FILE *tmpf;
		int fd;

		snprintf(tbuf, sizeof tbuf, "/tmp/astXXXXXX");
		srcname = tbuf;
		fd = mkstemp(tbuf);
		tmpf = fd == -1 ? 0 : fdopen(fd, "w+");
		if (tmpf == 0) {
			perror(srcname);
			done(-1);
		}
		copyfile(tmpf, srcname, stdin, "stdin");
		if (fclose(tmpf)) {
			perror(srcname);
			done(-1);
		}
	}
	if (nfiles || lflag) {
		if (freopen(srcname, "r", stdin) == 0) {
			perror(srcname);
			done(-1);
		}
	}
	if (lflag && nfiles == 0) {
		if (unlink(srcname) == -1) {
			perror(srcname);
			done(-1);
		}
	}

	/* the intermediate file and the symbol file */
	ifile = openfile(&ifname, ibuf, "/tmp/asiXXXXXX", iflag, nfiles, "i.out", 'i');
	sfile = openfile(&sfname, sbuf, "/tmp/assXXXXXX", tflag, nfiles, "sym.out", 't');

	/* pass 1 */
	if (vflag)
		printf("Pass 1\n");
	yyparse();
	if (synerr || semerr) {
		fprintf(stderr, "Abort because of syntax errors\n");
		done(-1);
	}
	endpass1();
	for (i = 0; i != nsect; i++) {
		optim(1, i);
		optim(0, i);
		optim(2, i);
	}
	iputb(0xF0);
	sputb(0xF0);

	/* the listing: the source is read again from the start */
	if (lflag == 0)
		fclose(stdin);
	else {
		if (fseek(stdin, 0L, SEEK_SET) == -1) {
			perror(srcname);
			done(-1);
		}
		if (nfiles == 0)
			strcpy(lfname, "list.out");
		else {
			strcpy(lfname, srcname);
			lfname[strlen(lfname) - 1] = 'l';
		}
		unlink(lfname);
		listf = fopen(lfname, "w+");
		if (listf == 0) {
			perror(lfname);
			done(-1);
		}
		listhead();
	}

	/* the relocation file */
	relf = openfile(&rfname, rbuf, "/tmp/asrXXXXXX", rflag, nfiles, "r.out", 'r');
	if (fseek(ifile, 0L, SEEK_SET) == -1) {
		perror(rfname);
		done(-1);
	}

	/* the output symbols */
	osymf = openfile(&ofname, obuf, "/tmp/asoXXXXXX", mflag, nfiles, "osym.out", 'm');

	/* the object file */
	unlink(outname);
	outf = fopen(outname, "w+");
	if (outf == 0) {
		perror(outname);
		done(-1);
	}

	/* pass 2 */
	if (vflag)
		printf("Pass 2\n");
	pass2();
	if (lflag)
		fclose(stdin);
	if (semerr) {
		fprintf(stderr, "Abort because of semantic errors\n");
		done(-1);
	} else
		done(0);
	return (0);
}
