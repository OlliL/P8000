/* @[$] swab.c	2.1  09/12/83 11:33:27 - 87wega3.2 */

/*
 * Swap bytes in 16-bit [half-]words
 * for going between the 11 and the interdata
 */

swab(pf, pt, n)
register short *pf, *pt;
register n;
{

	n /= 2;
	while (--n >= 0) {
		*pt++ = (*pf << 8) + ((*pf >> 8) & 0377);
		pf++;
	}
}
