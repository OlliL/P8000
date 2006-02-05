/* uudecode.c */

static char sccsid[] = "@(#)uudecode.c	5.1 (Berkeley) 7/2/83";

/*
 * uudecode [input]
 *
 * create the specified file, decoding as you go.
 * used with uuencode.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

/* single character decode */
#define DEC(c)	(((c) - ' ') & 077)

int pflag;

main(argc, argv)
char **argv;
{
	FILE *in, *out;
	struct stat sbuf;
	int mode;
	char dest[128];
	char buf[80];

	if ( argc > 1 && strncmp(argv[1],"-p",2) == 0 ) { 
		pflag = 1;
		argv++; argc--;
	}

	/* optional input arg */
	if (argc  > 1) {
		if ((in = fopen(argv[1], "r")) == NULL) {
			perror(argv[1]);
			(void)usage();
		}
		argv++; argc--;
	} else
		in = stdin;

/*	if (argc != 1) {
		printf("Usage: uudecode [infile]\n");
		exit(2);
	}
*/

	/* search for header line */
	for (;;) {
		if (fgets(buf, sizeof buf, in) == NULL) {
			fprintf(stderr, "No begin line\n");
			exit(3);
		}
		if (strncmp(buf, "begin ", 6) == 0)
			break;
	}
	sscanf(buf, "begin %o %s", &mode, dest);

	/* handle ~user/file format */

	/* create output file */
	/* binary output file */
	if(pflag)
		out = stdout;
	else {
		out = fopen(dest, "wb");
		if (out == NULL) {
			perror(dest);
			exit(4);
		}
		chmod(dest, mode);
	}

	decode(in, out);

	if (fgets(buf, sizeof buf, in) == NULL || strcmp(buf, "end\n")) {
		fprintf(stderr, "No end line\n");
		exit(5);
	}
	exit(0);
}

/*
 * copy from in to out, decoding as you go along.
 */
decode(in, out)
FILE *in;
FILE *out;
{
	char buf[80];
	char *bp;
	int n;

	for (;;) {
		/* for each input line */
		if (fgets(buf, sizeof buf, in) == NULL) {
			printf("Short file\n");
			exit(10);
		}
		n = DEC(buf[0]);
		if (n <= 0)
			break;

		bp = &buf[1];
		while (n > 0) {
			outdec(bp, out, n);
			bp += 4;
			n -= 3;
		}
	}
}

/*
 * output a group of 3 bytes (4 input characters).
 * the input chars are pointed to by p, they are to
 * be output to file f.  n is used to tell us not to
 * output all of them at the end of the file.
 */
outdec(p, f, n)
char *p;
FILE *f;
{
	int c1, c2, c3;

	c1 = DEC(*p) << 2 | DEC(p[1]) >> 4;
	c2 = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
	c3 = DEC(p[2]) << 6 | DEC(p[3]);
	if (n >= 1)
		putc(c1, f);
	if (n >= 2)
		putc(c2, f);
	if (n >= 3)
		putc(c3, f);
}


/* fr: like read but stdio */
int
fr(fd, buf, cnt)
FILE *fd;
char *buf;
int cnt;
{
	int c, i;

	for (i=0; i<cnt; i++) {
		c = getc(fd);
		if (c == EOF)
			return(i);
		buf[i] = c;
	}
	return (cnt);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

#ifndef NULL
#define	NULL	0
#endif

/*
char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}
*/

usage()
{
        (void)fprintf(stderr, "usage: uudecode [-p] [file ...]\n");
        exit(1);
}
