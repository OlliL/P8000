/*
	Copies `n' characters from string `a' to string `b'.
*/

char *move(a,b,n)
char *a,*b;
unsigned n;
{
	register char *x;
	register char *y;
	register int *xi;
	register int *yi;
	register int m;

	if (m=n) {
		x = a;
		y = b;
		if ((((int)x^(int)y)&1) == 0) {
			if ((int)x&1) {
				*y++ = *x++;
				n--;
			}
			xi = (int *)x;
			yi = (int *)y;
			for (m=n/2+1; --m; )
				*yi++ = *xi++;
			m = n&1;
			x = (char *)xi;
			y = (char *)yi;
		}
		for (++m; --m; ) *y++ = *x++;
	}
}
