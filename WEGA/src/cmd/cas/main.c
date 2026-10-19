static char sccsid[] = "@[$]/as/as/main.c\t1.2 05/27/83 15:48:39 - 87wega3.2";

/*
 * main - options, the temporary files and the two passes
 *
 * Files: i (the intermediate file written by pass 1), s (the symbols, sym.out),
 * r (relocation, r.out), m (the output symbols, osym.out), the object file
 * (a.out) and, with -l, the listing.  Unless an option asks to keep a file
 * (-i, -t, -r, -m) it is a temporary file that is removed at once after it
 * was opened.  The source is read from stdin; with a file name argument it
 * is reopened as stdin.
 */
#include <stdio.h>
#include <signal.h>
#include "cas.h"

char *mktemp();

extern char ibuf[], lbuf[], rbuf[], sbuf[], tbuf[], obuf[];

char *outname = "a.out";		/* -o */
char *itmpl = "/tmp/asiXXXXXX";
char *rtmpl = "/tmp/asrXXXXXX";
char *stmpl = "/tmp/assXXXXXX";
char *otmpl = "/tmp/asoXXXXXX";
char *ttmpl = "/tmp/astXXXXXX";
char *ifname = ibuf;
char *rfname = rbuf;
char *sfname = sbuf;
char *ofname = obuf;
char *lfname = lbuf;
int uflag = 0;				/* -u: undefined symbols are external */
int vflag = 0;				/* -v: messages about the passes */
int echoflag = 0;			/* -^: echo the source */
int dflag = 0;				/* -d: internal labels into the symbol table */
int lflag = 0;				/* -l: listing */
int synerr = 0;				/* errors of pass 1 */
int semerr = 0;				/* semantic errors */

FILE *osymf;
int (*oldterm)();
int (*oldquit)();
FILE *tmpf;
char errbuf[80];
FILE *outf;
FILE *relf;
FILE *listf;
int (*oldint)();
char *srcname;
char ibuf[14], lbuf[14], rbuf[14], sbuf[14], tbuf[14], obuf[14];

main(argc, argv)
int argc;
char **argv;
{
	register i, rflag, iflag, tflag, mflag, nfiles;
	int sigdone();

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
				i++;
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
				sprintf(errbuf, "invalid option -%c\n", argv[i][1]);
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
		srcname = tbuf;
		srcname = mktemp(ttmpl);
		unlink(srcname);
		tmpf = fopen(srcname, "w+");
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

	/* the intermediate file */
	if (iflag && nfiles) {
		strcpy(ifname, srcname);
		ifname[strlen(ifname) - 1] = 'i';
	} else if (iflag)
		strcpy(ifname, "i.out");
	else
		ifname = mktemp(itmpl);
	unlink(ifname);
	ifile = fopen(ifname, "w+");
	if (ifile == 0) {
		perror(ifname);
		done(-1);
	}
	if (!iflag) {
		if (unlink(ifname) == -1) {
			perror(ifname);
			done(-1);
		}
	}

	/* the symbol file */
	if (tflag && nfiles) {
		strcpy(sfname, srcname);
		sfname[strlen(sfname) - 1] = 't';
	} else if (tflag)
		strcpy(sfname, "sym.out");
	else
		sfname = mktemp(stmpl);
	unlink(sfname);
	sfile = fopen(sfname, "w+");
	if (sfile == 0) {
		perror(sfname);
		done(-1);
	}
	if (!tflag) {
		if (unlink(sfname) == -1) {
			perror(sfname);
			done(-1);
		}
	}

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
		if (fseek(stdin, 0L, 0) == -1) {
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
	if (rflag && nfiles) {
		strcpy(rfname, srcname);
		rfname[strlen(rfname) - 1] = 'r';
	} else if (rflag)
		strcpy(rfname, "r.out");
	else
		rfname = mktemp(rtmpl);
	unlink(rfname);
	relf = fopen(rfname, "w+");
	if (relf == 0) {
		perror(rfname);
		done(-1);
	}
	if (!rflag) {
		if (unlink(rfname) == -1) {
			perror(rfname);
			done(-1);
		}
	}
	if (fseek(ifile, 0L, 0) == -1) {
		perror(rfname);
		done(-1);
	}

	/* the output symbols */
	if (mflag && nfiles) {
		strcpy(ofname, srcname);
		ofname[strlen(ofname) - 1] = 'm';
	} else if (mflag)
		strcpy(ofname, "osym.out");
	else
		ofname = mktemp(otmpl);
	unlink(ofname);
	osymf = fopen(ofname, "w+");
	if (osymf == 0) {
		perror(ofname);
		done(-1);
	}
	if (!mflag) {
		if (unlink(ofname) == -1) {
			perror(ofname);
			done(-1);
		}
	}

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
}
