/*
 * sld - segmented loader of WEGA (work file)
 *
 * Working file: functions are reconstructed here in the order of
 * tools/own_functions.txt, verified per function against sld_own.s.
 * Split into the final source files (copyname.c, dump.c, ext.c, ld.c,
 * ldm.c, printname.c, swap.c, symbols.c, malloc.c) comes later;
 * this file is then removed.
 */

/* f0762: 42 bytes, one caller (0x085c); calls _f9184 and _f3408 */
long g03ba;
long f9184();

f0762(a)
int a;
{
	g03ba = f9184(a, 4);
	if (!g03ba)
		f3408(2, "x");
}

/* init_signals..0b04: byte-identical (fncheck) */
long f9638();
long f0a5c();

init_signals()
{
	if (f9638(2, 1L) != 1)
		f9638(2, f0a5c);
	if (f9638(15, 1L) != 1)
		f9638(15, f0a5c);
}

/* high_byte..2e76: byte-identical (fncheck) */
high_byte(x)
int x;
{
	if ((x & 0xff00) != x)
		f3408(2, "x");
	return ((unsigned)x >> 8) & 0xff;
}

/* f3090..30a4: byte-identical (fncheck) */
f3090(x)
{
	return (x >> 1) & 0x7fff;
}

/* f6ffe..702a: byte-identical (fncheck) */
long g1688;
long f9184();

f6ffe(a)
{
	g1688 = f9184(a, 16);
	if (!g1688)
		f3408(2, "x");
}

/* table32_entry..71c8: done */
char tab1c02[];

char *table32_entry(x)
unsigned x;
{
	if (x < 0x0100)
		return tab1c02 + (x << 5);
	f3408(2, "x");
}

/* table16_entry..6ffc: findings:D1 */
unsigned g04ee;
char *gE47E;

char *table16_entry(x)
{
	if (x < g04ee)
		return gE47E + (x << 4);
	f3408(2, "x");
}

/* f702a..705e: done */
char *f9072();

f702a(a)
{
	gE47E = f9072(gE47E, a << 4);
	if (!gE47E)
		f3408(2, "x");
}

/* str_compare..3a88: findings:D2 */
int g0476;

int str_compare(p, q)
char *p;
char *q;
{
	do {
		if (*q == *p) {
			p++;
			q++;
		} else {
			if (g0476)
				return *q - *p;
			return 0;
		}
	} while (*q);
	if (!g0476)
		return 1;
	return 0;
}
