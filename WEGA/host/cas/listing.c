/*
 * listing - the listing (option -l)
 *
 * A line is "LOC  OBJ CODE  STMT SOURCE": the location counter, up to 16 columns of code
 * (a byte, a word, with a mark * for a relocatable and ' for a symbolic value) and the number
 * and the text of the source line, which is read from stdin a second time. lcol is the
 * column (1 = start of the line), lline the number of the next source line to print,
 * lnl the number of lines of code that wait for their source line.
 */
#include <errno.h>
#include "cas.h"

static uint16_t lcol = 1;
static uint16_t lline = 1;
static int lnl = 0;

static void lstend(void);
static void lstline(void);

/* copy the file src to dst; the names are for the messages */
void
copyfile(FILE *dst, const char *dname, FILE *src, const char *sname)
{
	char buf[512];
	size_t n;

	errno = 0;
	while ((n = fread(buf, 1, sizeof buf, src)) != 0) {
		if (fwrite(buf, 1, n, dst) != n) {
			perror(dname);
			done(-1);
		}
	}
	if (errno) {
		perror(sname);
		done(-1);
	}
}

void
listhead(void)
{
	fprintf(listf, "LOC    OBJ CODE    STMT SOURCE STATEMENT\n");
}

/* the location counter at the start of a line */
static void
lstloc(void)
{
	if (lcol == 1) {
		fprintf(listf, "%04x ", (unsigned)(lctab[cursec][cursub] & 0xFFFF));
		lcol += 5;
	}
}

/* the mark behind a value: * for a relocatable (bit 3), ' for a symbolic one (bits 1, 2) */
static void
lstflag(int a)
{
	if (a & 8)
		fprintf(listf, "* ");
	else if (a & 6)
		fprintf(listf, "' ");
	else
		fprintf(listf, "  ");
	lcol += 2;
}

/* a word of code */
void
lstw(int w, int a)
{
	lstloc();
	fprintf(listf, "%04x", (uint16_t)w);
	lcol += 4;
	lstflag(a);
	lstend();
}

/* a byte of code */
void
lstb(int c, int a)
{
	lstloc();
	fprintf(listf, "%02x", (uint8_t)c);
	lcol += 2;
	lstflag(a);
	lstend();
}

/* after 16 columns: the line of code is full */
static void
lstend(void)
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
void
lstsync(int n)
{
	while ((uint16_t)n > lline) {
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
static void
lstline(void)
{
	int c;

	while (lcol < 19) {
		fprintf(listf, " ");
		lcol++;
	}
	fprintf(listf, "%5d ", (int16_t)lline);
	while ((c = getc(stdin)) != '\n' && c != EOF)
		putc(c, listf);
	putc('\n', listf);
	lcol = 1;
}

/* the rest of the source */
void
lstrest(void)
{
	int c;

	while ((c = getc(stdin)) != EOF) {
		ungetc(c, stdin);
		lstline();
		lline++;
	}
}
