/* nice */

static char sccsid[] = "@[$]nice.c\t2.1  01/30/83 16:54:46 - 87wega3.2";

#include <stdio.h>

main(argc, argv)
int argc;
char *argv[];
{
	int nicarg = 10;
	extern errno;
	extern char *sys_errlist[];

	if(argc > 1 && argv[1][0] == '-') {
		nicarg = atoi(&argv[1][1]);
		argc--;
		argv++;
	}
	if(argc < 2) {
		fputs("usage: nice [ -n ] command\n", stderr);
		exit(2);
	}
	nice(nicarg);
	execvp(argv[1], &argv[1]);
	fprintf(stderr, "%s: %s\n", sys_errlist[errno], argv[1]);
	exit(2);
}
