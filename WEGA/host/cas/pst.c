/*
 * pst - the permanent symbol table: mnemonics, registers, directives and the
 * other reserved words; a hash table with chains in front of this array
 * (pbuckets, pstab.c).
 *
 * The array of the 403 entries and the names (pstdata.h) are generated from the
 * original by host/tools/casgen.py. The original also has casechk(), a function
 * that nobody calls; it is left out.
 */
#include <ctype.h>
#include "cas.h"

#include "pstdata.h"

/* t is in lower case */
static int
pstcmp(const char *t, const char *s)
{
	int i, c;

	i = 0;
	do {
		c = (unsigned char)s[i];
		if (isupper(c))
			c = tolower(c);
		if (c != (unsigned char)t[i])
			return (-1);
	} while (s[i++] != 0);
	return (0);
}

/* the permanent symbol with this name (letters in either case) */
struct pst *
pstlook(const char *name)
{
	struct pbucket *bp;

	for (bp = &pbuckets[hash(503, name)]; bp; bp = bp->pb_next) {
		if (bp->pb_ent) {
			if (pstcmp(bp->pb_ent->p_name, name) != -1)
				return (bp->pb_ent);
		}
	}
	return (0);
}

/* the number in the digits of s of the base */
static int
digval(int c)
{
	if (isdigit(c))
		return (c - '0');
	if (isupper(c))
		return (c - 'A' + 10);
	return (c - 'a' + 10);
}

int32_t
xtol(const char *s, unsigned base)
{
	uint32_t n;

	n = 0;
	for (; *s; s++)
		n = n * base + digval((unsigned char)*s);
	return ((int32_t)n);
}
