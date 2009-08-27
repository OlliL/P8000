/* @[$] getpass.c	2.3  09/12/83 11:12:28 - 87wega3.2. */

#include <stdio.h>
#include <signal.h>
#include <sgtty.h>

char *
getpass(prompt)
char *prompt;
{
	struct sgttyb ttyb;
	int flags; /*,flags2,flags3,flags4,flags5;*/
	struct sgttyb foo;
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
	flags = ttyb.sg_flags;
	ttyb.sg_flags &= 0xff87;
	stty(fileno(fi), 0x5404, &ttyb);
	fprintf(stderr, prompt);
	for (p=pbuf; (c = getc(fi))!='\n' && c!=EOF;) {
		if (p < &pbuf[8])
			*p++ = c;
	}
	*p = '\0';
	fprintf(stderr, "\n");
	ttyb.sg_flags = flags;
	stty(fileno(fi), 0x5403, &ttyb);
	signal(SIGINT, sig);
	if (fi != stdin)
		fclose(fi);
	return(pbuf);
}
