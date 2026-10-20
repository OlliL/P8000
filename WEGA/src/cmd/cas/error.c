static char sccsid[] = "@[$]error.c\t1.1 03/16/83 19:18:10 - 87wega3.2";

/*
 * error - the messages of the assembler
 *
 * error(1, msg) is a warning, error(2, msg) an error of the program (the first one of a line
 * is printed, and the statement is dropped), error(3, msg) a syntax error, error(4, msg) ends
 * the assembler at once.
 */
#include "cas.h"

int lasterr = 0;			/* the line of the last error */

error(n, msg)
int n;
char *msg;
{
	switch (n) {
	case 1:
		fprintf(stderr, "Warning: line %d: ", curline);
		fprintf(stderr, msg);
		return (0);
	case 2:
		if (curline != lasterr) {
			fprintf(stderr, "Fatal error: line %d: ", curline);
			fprintf(stderr, msg);
			semerr++;
		}
		nfreed = nalloc;
		lasterr = curline;
		return (0);
	case 3:
		fprintf(stderr, "Syntax error: line %d: ", curline);
		fprintf(stderr, msg);
		lasterr = curline;
		return (0);
	case 4:
		fprintf(stderr, "Immediately fatal error: line %d: ", curline);
		fprintf(stderr, msg);
		done(-1);
	}
}
