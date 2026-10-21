/*
 * error - the messages of the assembler
 *
 * error(1, msg) is a warning, error(2, msg) an error of the program (the first one of a line
 * is printed, and the statement is dropped), error(3, msg) a syntax error, error(4, msg) ends
 * the assembler at once.
 */
#include "cas.h"

static int lasterr = 0;			/* the line of the last error */

void
error(int n, const char *msg)
{
	switch (n) {
	case 1:
		fprintf(stderr, "Warning: line %d: ", (int16_t)curline);
		fputs(msg, stderr);
		break;
	case 2:
		if (curline != lasterr) {
			fprintf(stderr, "Fatal error: line %d: ", (int16_t)curline);
			fputs(msg, stderr);
			semerr++;
		}
		nfreed = nalloc;
		lasterr = curline;
		break;
	case 3:
		fprintf(stderr, "Syntax error: line %d: ", (int16_t)curline);
		fputs(msg, stderr);
		lasterr = curline;
		break;
	case 4:
		fprintf(stderr, "Immediately fatal error: line %d: ", (int16_t)curline);
		fputs(msg, stderr);
		done(-1);
	}
}
