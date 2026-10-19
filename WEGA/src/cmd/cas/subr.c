static char sccsid[] = "@[$]subr.c\t1.1 03/16/83 19:05:51 - 87wega3.2";

/*
 * subr - the hash of the symbol names (permanent and user symbols)
 */

/*
 * The letters are folded to one case; the characters go alternately into the high
 * byte of k and into its low byte, and the sum of all the k is the hash.
 */
hash(n, s)
unsigned n;
char *s;
{
	int h, k;
	char c;
	register char *p;

	h = 0;
	k = 0;
	p = 0;
	while (*s) {
		c = (*s & 0xDF) - 0xDA;
		s++;
		if (p == (char *)&k)
			p++;
		else
			p = (char *)&k;
		*p = c;
		h += k;
	}
	return((unsigned)h % n);
}
