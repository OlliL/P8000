/* Miscellaneous machine independent utilities */

#include "global.h"

/* Convert hex-ascii to integer */
int
htoi(s)
char *s;
{
	int i = 0;
	char c;

	while((c = *s++) != '\0'){
		if(c == 'x')
			continue;	/* allow 0x notation */
		if('0' <= c && c <= '9')
			i = (i * 16) + (c - '0');
		else if('a' <= c && c <= 'f')
			i = (i * 16) + (c - 'a' + 10);
		else if('A' <= c && c <= 'F')
			i = (i * 16) + (c - 'A' + 10);
		else
			break;
	}
	return i;
}
/* replace terminating end of line marker(s) with null */
void
rip(s)
register char *s;
{
	register char *cp;

	if((cp = index(s,'\015')) != NULLCHAR)
		*cp = '\0';
	if((cp = index(s,'\012')) != NULLCHAR)
		*cp = '\0';
}
#ifdef DEAD
/* Case-insensitive string comparison - replaced by standard function */
strncasecmp(a,b,n)
register char *a,*b;
register int n;
{
	char a1,b1;

	while(n-- != 0 && (a1 = *a++) != '\0' && (b1 = *b++) != '\0'){
		if(a1 == b1)
			continue;	/* No need to convert */
		a1 = tolower(a1);
		b1 = tolower(b1);
		if(a1 == b1)
			continue;	/* NOW they match! */
		if(a1 > b1)
			return 1;
		if(a1 < b1)
			return -1;
	}
	return 0;
}
#endif
