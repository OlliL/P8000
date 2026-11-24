static char sccsid[] = "@[$]dog.c\t1.8  03/14/83 14:27:00 - 87wega3.2";

/*
 * dog - page the files on the terminal (WEGA; no source, written from /bin/dog 1.8). The names are guesses.
 * After 23 lines it waits for the standard error: RETURN 22 more lines, a number that many, -number skips that many
 * lines and shows 11 (or the number after it), the end of input 11.
 */

#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include <fcntl.h>

struct	termio otty;
struct	termio ntty;
int	nlines;
int	more;
char	ch;
char	obuf[BUFSIZ];

int	done();
char	*ttyname();

main(argc, argv)
char **argv;
{
	register c;
	FILE *fp;

	if (isatty(fileno(stderr)))
		setup();
	setbuf(stdout, obuf);
	ioctl(1, TCGETA, &otty);
	ioctl(1, TCGETA, &ntty);
	ntty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL);
	ioctl(1, TCSETAW, &ntty);
	signal(SIGINT, done);
	nlines = 23;
	argc--;
	argv++;
	fp = stdin;
	do {
		if (argc > 0) {
			close(0);
			if ((fp = fopen(*argv, "r")) == NULL) {
				fflush(stdout);
				perror(*argv);
				fflush(stdout);
				done();
			}
			argc--;
			argv++;
		}
		while ((c = getc(fp)) != EOF) {
			if (nlines < 0) {
				if (++nlines == 0)
					nlines = more;
			} else if (--nlines == 0) {
				fflush(stdout);
				ask();
			}
			if (nlines > 0)
				putchar(c);
			while (c != '\n') {
				if ((c = getc(fp)) == EOF)
					goto eof;
				if (nlines > 0)
					putchar(c);
			}
			fflush(stdout);
		}
	eof:
		fflush(stdout);
	} while (argc > 0);
	ioctl(1, TCSETAW, &otty);
	exit(0);
}

done()
{
	signal(SIGINT, SIG_IGN);
	ioctl(1, TCSETAW, &otty);
	exit(1);
}

ask()
{
	int n;

	if ((n = getnum()) == 0) {
		nlines = 22;
		return;
	}
	nlines = n;
	if (n < 0) {
		if (ch == '\n')
			more = 11;
		else {
			n = getnum();
			more = n == 0 ? 11 : n;
		}
	}
}

getnum()
{
	int n, f;

	n = 0;
	f = 0;
	while (read(2, &ch, 1) == 1) {
		switch (ch) {
		case ' ':
			if (f == 0)
				continue;
		case '\n':
			return (n * f);
		case '-':
			f = -1;
			continue;
		}
		if (ch >= '0' && ch <= '9') {
			n *= 10;
			n += ch - '0';
			if (f == 0)
				f = 1;
		}
	}
	return (11);
}

setup()
{
	char *p;

	if (fcntl(fileno(stderr), F_GETFL, 0) != O_RDWR) {
		p = ttyname(fileno(stderr));
		if (freopen(p, "r+", stderr) != stderr) {
			fprintf(stdout, "Error in set-up of stderr\n");
			exit(5);
		}
	}
	setbuf(stderr, NULL);
}
