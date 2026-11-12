/* @[$] getpass.c	2.3  09/12/83 11:12:28 - 87wega3.2. */

#include <stdio.h>
#include <signal.h>
#include <termio.h>

char *
getpass(prompt)
char *prompt;
{
	struct termio ttyb;
	int flags;
	register char *p;
	register c;
	FILE *fi;
	static char pbuf[9];
	int (*signal())();
	int (*sig)();

	if ((fi = fopen("/dev/tty", "r")) == NULL)
		return(0);
	else
		setbuf(fi, (char *)NULL);
	sig = signal(SIGINT, SIG_IGN);
	ioctl(fileno(fi), 0x5401, &ttyb);
	flags = ttyb.c_lflag;
	ttyb.c_lflag &= 0xff87;
	ioctl(fileno(fi), 0x5404, &ttyb);
	fprintf(stderr, prompt);
	for (p=pbuf; (c = getc(fi))!='\n' && c!=EOF;) {
		if (p < &pbuf[8])
			*p++ = c;
	}
	*p = '\0';
	fprintf(stderr, "\n");
	ttyb.c_lflag = flags;
	ioctl(fileno(fi), 0x5403, &ttyb);
	signal(SIGINT, sig);
	if (fi != stdin)
		fclose(fi);
	return(pbuf);
}
