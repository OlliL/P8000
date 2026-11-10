static char sccsid[] = "@[$]/as/as/pst.c\t1.2 05/27/83 15:56:58 - 87wega3.2";

/*
 * pst - the permanent symbol table: mnemonics, registers, directives and the
 * other reserved words; a hash table with chains in front of this array
 * (pbuckets, the next module after error.c in the data).
 *
 * The array of the 403 entries and the names follows (generated from the
 * binary by host/tools/casgen.py, see pstdata.h).
 */
#include <ctype.h>
#include "cas.h"

#include "pstdata.h"

struct lwalk lwalk;		/* listp (parser.y) is its second member; the first word is never used */
int symmode;			/* how a name is looked up: 6 local, 0 global (not used) */

/* the permanent symbol with this name (letters in either case) */
struct pst *
pstlook(name)
char *name;
{
	register struct pst *pp;
	register int h;
	register struct pbucket *bp;

	h = hash(503, name);
	for (bp = &pbuckets[h]; bp; bp = bp->pb_next) {
		if (bp->pb_ent) {
			if (pstcmp(bp->pb_ent->p_name, name) != -1)
				return(bp->pb_ent);
		}
	}
	return(0);
}

/* t is in lower case */
pstcmp(t, s)
char *t, *s;
{
	register i;
	char c;

	i = 0;
	do {
		if ((c = isupper(s[i]) ? tolower(s[i]) : s[i]) != t[i])
			goto bad;
	} while (s[i++] != 0);
	return(0);
bad:
	return(-1);
}

/* -1 when the letters of the name are not all in the same case */
casechk(s)
char *s;
{
	register i, upper;

	i = 0;
	upper = 0;
	while (!isalpha(s[i])) {
		if (s[i] == 0)
			break;
		i++;
	}
	upper = isupper(s[i]);
	for (; s[i]; i++) {
		if (!isalpha(s[i]))
			continue;
		if (upper ? islower(s[i]) : isupper(s[i]))
			return(-1);
	}
	return(0);
}

/* the number in the digits of s of the base */
long
xtol(s, base)
char *s;
unsigned base;
{
	register long n;

	n = 0;
	for (; *s; s++)
		n = n * base + digval(*s);
	return(n);
}

static
digval(c)
register char c;
{
	if (isdigit(c))
		return(c - '0');
	if (isupper(c))
		return(c - 'A' + 10);
	return(c - 'a' + 10);
}
