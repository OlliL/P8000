/*
 * clist - the listing program of cc -Sl (/lib/clist)
 *
 *	clist infile outfile
 *
 * Copies the assembler file infile to outfile; at each line "\t.line N \"file\"" of the compiler the lines of the
 * C source up to line N are put in front, as comments ("// " + the line). Lines of comments ("//", "/*") are dropped.
 * No source known; written from the binary (WEGA 3.1).
 */

#include <stdio.h>

char	buf[256];
FILE	*outfile;
char	cline[256];
FILE	*infile;
char	line[256];
FILE	*cfile;
static char oldname[64];
static char cfname[64];
static int curline;
static int lineno;

main(argc, argv)
char **argv;
{
	if (argc < 3)
		error("bad cmd line");
	infile = fopen(argv[1], "r");
	outfile = fopen(argv[2], "w");
	if (infile == NULL)
		error("can't open infile");
	if (outfile == NULL)
		error("can't open outfile");
	while (getline(line, infile)) {
		switch (classify(line)) {
		case 0:
			copysrc();
		case 2:
			fputs(line, outfile);
		case 1:
			break;
		}
	}
	done(0);
}

classify(s)
char *s;
{
	register char *p, *q;
	char c;

	if (s[0] == '/' && (s[1] == '/' || s[1] == '*'))
		return(1);
	if (s[0] == '\t' && s[1] == '.' && s[2] == 'l' && s[3] == 'i' && s[4] == 'n' && s[5] == 'e') {
		p = &s[7];
		q = cfname;
		lineno = atoi(p);
		while (*p++ != '\t')
			;
		if (*p == '"')
			p++;
		while ((c = *p++) != '"') {
			if (c == ' ' || c == '\t')
				break;
			*q++ = c;
		}
		*q = 0;
		return(0);
	}
	return(2);
}

copysrc()
{
	if (strcmp(oldname, cfname)) {
		while (getline(cline, cfile))
			putline();
		fclose(cfile);
		if ((cfile = fopen(cfname, "r")) == NULL)
			error("can't open C file");
		strcpy(oldname, cfname);
		curline = 0;
	}
	while (lineno > curline) {
		if (getline(cline, cfile) == 0)
			error("unexpected EOF");
		putline();
		curline++;
	}
}

putline()
{
	putc('/', outfile);
	putc('/', outfile);
	putc(' ', outfile);
	fputs(cline, outfile);
}

getline(s, fp)
register char *s;
FILE *fp;
{
	register c, n;

	n = 0;
	if (fp == NULL)
		return(0);
	while ((c = getc(fp)) != EOF) {
		*s++ = c;
		n++;
		if (c == '\n' || n >= 255)
			break;
	}
	if (c == '\n' || c == EOF)
		*s = 0;
	else
		error("buffer too small");
	return(n);
}

error(s)
char *s;
{
	fprintf(stderr, "loption: %s\n", s);
	done(1);
}

done(n)
{
	if (n == 0)
		while (getline(cline, cfile))
			putline();
	fclose(infile);
	fclose(outfile);
	fclose(cfile);
	exit(n);
}
