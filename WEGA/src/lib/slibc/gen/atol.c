/* @[$] atol.c	2.1  09/12/83 11:20:00 - 87wega3.2 */

long
atol(p)
register char *p;
{
	long n;
	register int f;

	n = 0;
	f = 0;
	for(;;p++) {
		switch(*p) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			p++;
		}
		break;
	}
	while(*p >= '0' && *p <= '9')
		n = n*10 + *p++ - '0';
	return(f? -n: n);
}
