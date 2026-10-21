/*
 * subr - the hash of the symbol names (permanent and user symbols)
 */
#include "cas.h"

/*
 * The letters are folded to one case; the characters go alternately into the high
 * byte of k and into its low byte (the other byte stays), and the sum of all the k
 * (16 bits) is the hash.
 */
unsigned
hash(unsigned n, const char *s)
{
	uint16_t h, k;
	int low;

	h = 0;
	k = 0;
	low = 1;				/* the first character goes into the high byte */
	while (*s) {
		unsigned char c = (*s & 0xDF) - 0xDA;

		s++;
		low = !low;
		if (low)
			k = (k & 0xFF00) | c;
		else
			k = (k & 0x00FF) | (c << 8);
		h += k;
	}
	return (h % (uint16_t)n);
}
