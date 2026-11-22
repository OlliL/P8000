/*
 * gets - read one line of the standard input and write it to the standard output (reconstructed from the binary
 * of WEGA 3.1: no source; the standard input unbuffered, so that only the line is read). At the end of the input:
 * exit status 1 without an argument, an empty line with one.
 */
#include	<stdio.h>

static char sccsid[] = "@[$]gets.c\t1.2  07/24/82 00:39:46 - 87wega3.2";

main(argc, argv)
char **argv;
{
	char	buf[512];

	setbuf(stdin, NULL);
	if (gets(buf) == NULL) {
		if (argc == 1)
			exit(1);
		buf[0] = '\0';
	}
	printf("%s\n", buf);
	exit(0);
}
