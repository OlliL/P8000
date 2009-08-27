/* @[$] perror.c	2.1  09/12/83 11:28:30 - 87wega3.2 */

/*
 * Print the error indicated
 * in the cerror cell.
 */

int	errno;
int	sys_nerr;
char	*sys_errlist[];
int	deverr;
int	dev_nerr;
char	*dev_errlist[];
perror(s)
char *s;
{
	register char *c;
	register n;

	c = "Unknown error";

	if(errno == 5)
		goto deve;
	else if (errno != 6)
		goto syse;
deve:
	if(deverr) {
		if(deverr < dev_nerr)
			c = dev_errlist[deverr];
	} else {
syse:
		if(errno < sys_nerr)
			c = sys_errlist[errno];
	}
	n = strlen(s);
	if(n) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	write(2, c, strlen(c));
	write(2, "\n", 1);
}
