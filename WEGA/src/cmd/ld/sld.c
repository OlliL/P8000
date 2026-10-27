/*
 * sld - segmented loader of WEGA (work file)
 *
 * Working file: functions are reconstructed here in the order of
 * tools/own_functions.txt, verified per function against sld_own.s.
 * Split into the final source files (copyname.c, dump.c, ext.c, ld.c,
 * ldm.c, printname.c, swap.c, symbols.c, malloc.c) comes later;
 * this file is then removed.
 */

/* unpack_run 0x0056..0x00ee: findings:D2 */
int g0476;

int unpack_run(src, dst)
register char *src;
register char *dst;
{
	int n;
	char ch;
	register int k;

	n = 0;
	k = 0;
	ch = *src;
	if (ch & 0x80) {
		ch &= 0x7f;
		n = (ch + 7) / 16;
		ch = n * 16 + 8;
	} else if (g0476 > 0)
		ch = 14;
	else
		ch = 8;
	while (ch-- != 0) {
		*dst++ = *src;
		if (*src)
			k++;
	}
	return n;
}

/* f0762 0x0762..0x078c: done */
long g03ba;
long f9184();

f0762(a)
int a;
{
	g03ba = f9184(a, 4);
	if (!g03ba)
		f3408(2, "x");
}

/* sig_handler 0x0a5c..0x0aba: done */
int g04e8;
int g04e6;
int bE496;
int bE4A2;
char tmpname[];
char *p0494;

sig_handler()
{
	f96fa(tmpname);
	if (!g04e8 && !(bE496 && g04e6) && !bE4A2) {
		f9564(p0494, (~umask(0)) & 0x1ff);
	}
	f8a50(g04e8);
}

/* init_signals 0x0aba..0x0b04: done */
long f9638();

init_signals()
{
	if (f9638(2, 1L) != 1)
		f9638(2, sig_handler);
	if (f9638(15, 1L) != 1)
		f9638(15, sig_handler);
}

/* high_byte 0x2e4c..0x2e76: done */
high_byte(x)
int x;
{
	if ((x & 0xff00) != x)
		f3408(2, "x");
	return ((unsigned)x >> 8) & 0xff;
}

/* f3090 0x3090..0x30a4: done */
f3090(x)
int x;
{
	return (x >> 1) & 0x7fff;
}

/* table_lookup 0x34b8..0x3516: findings:D2 */
char tab84d8[];

char *table_lookup(a, end)
long a;
char *end;
{
	register int k;
	register char *p;

	k = ((int)a >> 4) & 0x0fff;
	p = tab84d8;
	while (p < end) {
		if (k == *(int *)p)
			return *(char **)(p + 2);
		p += 6;
	}
	f79ca("x", (int)a);
	f3408(2, "x");
}

/* str_compare 0x3a30..0x3a88: findings:D2 */
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

/* f3a88 0x3a88..0x3ac6: findings:D5 */
int flush_buffer();

f3a88(v, a)
int v;
char *a;
{
	register int *p;

	p = a;
	p[2] = v;
	p[3] += 2;
	if (--p[1] <= 0)
		flush_buffer(p);
}

/* f60f0 0x60f0..0x6134: findings:D2 */
int g3e7a;

f60f0(q, a)
char *q;
long a;
{
	int v;

	if ((char)a == 0x74)
		v = *(int *)(q + 4) - g3e7a;
	else
		v = *(int *)(q + 6) - g3e7a;
	if (v)
		f64d0(v);
}

/* swap_pair 0x65ec..0x6620: findings:D3 */
int f65c6();

swap_pair(a)
int *a;
{
	register int *p;

	p = a;
	if (*p)
		*p = f65c6(*p);
	p++;
	if (*p)
		*p = f65c6(*p);
}

/* f6a08 0x6a08..0x6a58: done */
int bE4A4;
int bE4AA;
char bE472[];
char bE46E;
long bE46A;
int bE470;
char bF10A[];
int unpack_run();
int f6c6a();
f6e1a();

f6a08(p)
char *p;
{
	if (bE4A4)
		return;
	if (bE4AA)
		return;
	unpack_run(p, bE472);
	bE46E = 0x1f;
	bE46A = 0;
	bE470 = 0;
	f6c6a();
	f6e1a(bF10A, &bE46A);
}

/* f6b5a 0x6b5a..0x6b84: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
char bE472[];
char bE46E;
long bE46A;
int unpack_run();
int f00ee();
int f6a58();

f6b5a(p)
char *p;
{
	long dummy;

	f00ee(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	f6a58();
}

/* f6b84 0x6b84..0x6bae: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
char bE472[];
char bE46E;
long bE46A;
int unpack_run();
int f00ee();
int f6a58();

f6b84(p)
char *p;
{
	long dummy;

	unpack_run(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	f6a58();
}

/* f6efe 0x6efe..0x6f46: done */
int g04fa;
char bE4DA[];
char *table16_entry();
char *f705e();
f6e1a();

f6efe()
{
	register char *p;
	register char *end;

	p = table16_entry(0);
	end = table16_entry(g04fa);
	while (p < end) {
		p[4] |= 0x40;
		f6e1a(bE4DA, p);
		p[4] &= ~0x40;
		p = f705e(p);
	}
}

/* table16_entry 0x6fd0..0x6ffe: findings:D1 */
unsigned g04ee;
char *gE47E;

char *table16_entry(x)
unsigned x;
{
	if (x < g04ee)
		return gE47E + (x << 4);
	f3408(2, "x");
}

/* f6ffe 0x6ffe..0x702a: done */
long g1688;
long f9184();

f6ffe(a)
int a;
{
	g1688 = f9184(a, 16);
	if (!g1688)
		f3408(2, "x");
}

/* f702a 0x702a..0x705e: done */
char *gE47E;
char *f9072();

f702a(a)
int a;
{
	gE47E = f9072(gE47E, a << 4);
	if (!gE47E)
		f3408(2, "x");
}

/* table32_entry 0x719e..0x71ca: done */
char tab1c02[];

char *table32_entry(x)
unsigned x;
{
	if (x < 0x0100)
		return tab1c02 + (x << 5);
	f3408(2, "x");
}

/* swap_quad..6684: findings:D3 */
int f65c6();

swap_quad(a)
char *a;
{
	register int *p;

	p = a;
	if (p[2])
		p[2] = f65c6(p[2]);
	if (p[3])
		p[3] = f65c6(p[3]);
	if (p[4])
		p[4] = f65c6(p[4]);
	if (p[5])
		p[5] = f65c6(p[5]);
}

/* f2f04..2f3a: findings:D2 */
int f3a88();

f2f04(p, q, n)
register int *p;
char *q;
int n;
{
	register int k;

	k = n >> 1;
	do {
		f3a88(*p++, q);
	} while (--k);
}

/* f2dde..2e4c: done */
int flush_buffer();
int f95da();
int f9610();
int f9576();
char bE6EA[];
int f3a88();

f2dde(p)
char *p;
{
	register int n;
	register int c;
	register int *q;

	flush_buffer(p);
	n = *(int *)p;
	f95da(n, 0L, 0);
	while ((c = f9610(n, bE6EA, 0x200)) > 1) {
		c >>= 1;
		q = (int *)bE6EA;
		do {
			f3a88(*q++, bE4DA);
		} while (--c);
	}
	f9576(n);
}

/* f0178..01d2: findings:D2 */
int f6522();
int unpack_run();

f0178(src, dst)
char *src;
char *dst;
{
	register int k;
	register char *s;

	k = 0;
	s = src;
	if (*src & 0x80) {
		k++;
		if ((*src & 0x7f) > 8) {
			k--;
			f6522(s);
			f3408(1, "x");
			k++;
		}
	}
	return unpack_run(s, dst);
}

/* copy_leafname..44e6: done */
copy_leafname(s, d)
char *s;
char *d;
{
	int k;
	int last;
	int n;

	k = 0;
	last = -1;
	while (s[k] != 0) {
		if (s[k] == 0x2f)
			last = k;
		k++;
	}
	n = k - last - 15;
	if (n > 0)
		k = k - n;
	last++;
	n = 0;
	while (last < k) {
		d[n] = s[last];
		n++;
		last++;
	}
	d[n] = 0;
}

/* flush_buffer..3b1e: findings:D1 */
int f970a();

flush_buffer(a)
char *a;
{
	register char *p;
	register int n;

	p = a;
	n = *(char **)(p + 4) - (p + 8);
	if (n > 0) {
		if (f970a(*(int *)p, p + 8, n) != n)
			f3408(2, "x");
	}
	*(char **)(p + 4) = p + 8;
	*(int *)(p + 2) = 0x100;
}
