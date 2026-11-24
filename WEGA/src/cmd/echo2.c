/*
 * echo2 - the echo of V7 (-n) on the standard error (WEGA; no source, written from /bin/echo2)
 */

static char sccsid[] = "@[$]echo2.c\t1.1  07/23/82 23:45:58 - 87wega3.2";

#include <stdio.h>

main(argc, argv)
int argc;
char *argv[];
{
	register int i, nflg;

	nflg = 0;
	if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
		nflg++;
		argc--;
		argv++;
	}
	for(i=1; i<argc; i++) {
		fputs(argv[i], stderr);
		if (i < argc-1)
			putc(' ', stderr);
	}
	if(nflg == 0)
		putc('\n', stderr);
	exit(0);
}
