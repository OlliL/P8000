static char sccsid[] = "@[$]listing.c\t1.1 05/27/83 14:57:19 - 87wega3.2.";

/*
 * listing - the listing (option -l)
 *
 * A line is "LOC  OBJ CODE  STMT SOURCE": the location counter, up to 16 columns of code
 * (a byte, a word, with a mark * for a relocatable and ' for a symbolic value) and the number
 * and the text of the source line, which is read from stdin a second time.  lcol is the
 * column (1 = start of the line), lline the number of the next source line to print,
 * lnl the number of lines of code that wait for their source line.
 */
#include "cas.h"

unsigned lcol = 1;
unsigned lline = 1;
int lnl = 0;

int copyerr;

/* copy the file src to dst; the names are for the messages */
copyfile(dst, dname, src, sname)
FILE *dst;
char *dname;
FILE *src;
char *sname;
{
	char buf[512];
	char *p;
	int n;
	int m;

	p = buf;
	copyerr = 0;
	while ((n = fread(p, 1, 512, src)) != 0) {
		m = fwrite(p, 1, n, dst);
		if (m != n) {
			perror(dname);
			done(-1);
		}
	}
	if (n == 0 && copyerr) {
		perror(sname);
		done(-1);
	}
}

listhead()
{
	fprintf(listf, "LOC    OBJ CODE    STMT SOURCE STATEMENT\n");
}

/* the location counter at the start of a line */
lstloc()
{
	if (lcol == 1) {
		fprintf(listf, "%04x ", lctab[cursec][cursub]);
		lcol += 5;
	}
}

/* a word of code */
lstw(w, a)
int w;
int a;
{
	lstloc();
	fprintf(listf, "%04x", w);
	lcol += 4;
	lstflag(a);
	lstend();
}

/* a byte of code */
lstb(c, a)
unsigned char c;
int a;
{
	lstloc();
	fprintf(listf, "%02x", c);
	lcol += 2;
	lstflag(a);
	lstend();
}

/* the mark behind a value: * for a relocatable (bit 3), ' for a symbolic one (bits 1, 2) */
lstflag(a)
int a;
{
	if (a & 8)
		fprintf(listf, "* ");
	else if (a & 6)
		fprintf(listf, "' ");
	else
		fprintf(listf, "  ");
	lcol += 2;
}

/* after 16 columns: the line of code is full */
lstend()
{
	if (lcol > 16) {
		if (lnl) {
			fprintf(listf, "\n");
			lcol = 1;
		} else {
			lstline();
			lnl++;
		}
	}
}

/* the source lines up to n are printed; lines of code without a source line wait */
lstsync(n)
int n;
{
	while (n > lline) {
		if (lnl) {
			if (lcol < 16 && lcol > 1) {
				fprintf(listf, "\n");
				lcol = 1;
			}
			lnl--;
		} else
			lstline();
		lline++;
	}
}

/* the line number and the source line, behind the code */
lstline()
{
	int c;

	while (lcol < 19) {
		fprintf(listf, " ");
		lcol++;
	}
	fprintf(listf, "%5d ", lline);
	while ((c = getc(stdin)) != '\n')
		putc(c, listf);
	putc('\n', listf);
	lcol = 1;
}

/* the rest of the source */
lstrest()
{
	int c;

	while ((c = getc(stdin)) != EOF) {
		ungetc(c, stdin);
		lstline();
		lline++;
	}
}
