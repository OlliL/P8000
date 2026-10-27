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
int swap_bytes();

swap_pair(a)
int *a;
{
	register int *p;

	p = a;
	if (*p)
		*p = swap_bytes(*p);
	p++;
	if (*p)
		*p = swap_bytes(*p);
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
int swap_bytes();

swap_quad(a)
char *a;
{
	register int *p;

	p = a;
	if (p[2])
		p[2] = swap_bytes(p[2]);
	if (p[3])
		p[3] = swap_bytes(p[3]);
	if (p[4])
		p[4] = swap_bytes(p[4]);
	if (p[5])
		p[5] = swap_bytes(p[5]);
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

/* table16_index..7112: findings:D4 */
int table16_index(x, a)
char *x;
char *a;
{
	char *p;

	if (gE47E > x)
		f3408(2, "x");
	else {
		p = gE47E + (g04ee << 4);
		if (p > x)
			return ((int)x - (int)a) >> 4;
		f3408(2, "x");
	}
}

/* add16_checked..1696: findings:D4 */
int add16_checked(a, x, y)
long a;
unsigned x;
unsigned y;
{
	long s;

	s = (long)x + y;
	if (s >= 0x10000)
		f3408(1);
	return s;
}

/* f6134..620e: done */
int bE3E7A;
int bE3E82;
char bE6E2[];
char bE8EA[];
char bEF02[];
char bEAF2[];
struct { int a; int b; int c; int d; } tblF332[];
char *f06c6();
int f620e();
int f3ac6();

f6134()
{
	register int i;

	f3ac6(bE6E2);
	f3ac6(bE8EA);
	if (bE4A2) {
		f3ac6(bEF02);
		f3ac6(bEAF2);
	}
	for (i = 0; i <= bE3E82; i++) {
		bE3E7A = 0;
		if (tblF332[i].a > -1)
			f620e(tblF332[i].a, 0x74, *(char **)(f06c6(tblF332[i].a) + 22));
		if (tblF332[i].b > -1)
			f620e(tblF332[i].b, 0x64, *(char **)(f06c6(tblF332[i].b) + 26));
	}
}

/* f85f8..8680: findings:D2 */
int f85f8(s, out, n)
char *s;
int *out;
int n;
{
	register int m;
	register int v;
	register char *p;
	char c;
	int sign;

	m = n;
	sign = 0;
	p = s;
	if (m != 0 && *p == 0x2d) {
		n--;
		sign = 1;
		p++;
	}
	v = 0;
	while (m != 0) {
		c = *p++;
		if (c < 0x30 || c > 0x39)
			break;
		v = v * 10 + (c - 0x30);
		m--;
	}
	if (sign)
		*out = -v;
	else
		*out = v;
	return n - m;
}

/* swap_bytes..65ec: findings:D4 */
int swap_bytes(u)
struct { int i; } u;
{
	char b[2];
	register char *p;

	p = b;
	*p = u.i;
	p++;
	*p = u.i >> 8;
	return *(int *)b;
}

/* f6bae..6c6a: findings:D2 */
int bE4FA;
char *bE4FC;
char *table16_entry();
int unpack_run();
char **tblE500[];

f6bae(a)
char **a;
{
	register char *r;

	if (*a == 0) {
		if (bE4FA >= 0x0ffa)
			f3408(2, "x");
		tblE500[bE4FA] = a;
		r = table16_entry(bE4FA++);
		bE4FC = r;
		*a = r;
		bE4FA = unpack_run(bE472, r + 8) + bE4FA;
		*(char *)(r + 4) = bE46E;
		*(long *)r = bE46A;
		*(int *)(r + 6) = bE470;
		return 1;
	}
	bE4FC = *a;
	return 0;
}

/* f6da8..6e1a: findings:D6 */
int bE4D0;
int bE4D2;
int bE4D4;

f6da8()
{
	switch (bE46E) {
	case 2:
	case 0x22:
		bE46A = (unsigned)((unsigned)bE46A + bE4D0);
		break;
	case 3:
	case 0x23:
		bE46A = (unsigned)((unsigned)bE46A + bE4D2);
		break;
	case 4:
	case 0x24:
		bE46A = (unsigned)((unsigned)bE46A + bE4D4);
		break;
	case 0x20:
		break;
	default:
		if (bE46E & 0x20)
			bE46E = 0x21;
	}
}

/* list_move_after..64d0: findings:D4 */
long bF32E;
long b3C12;

int list_move_after(p, e)
char *p;
char *e;
{
	register char *q;
	register char *r;

	q = *(char **)(p + 0x16);
	r = *(char **)(e + 0x12);
	if (p == bF32E)
		bF32E = e;
	else
		*(char **)(q + 0x12) = e;
	if (e == b3C12)
		b3C12 = p;
	else
		*(char **)(r + 0x16) = e;
	*(char **)(p + 0x12) = r;
	*(char **)(p + 0x16) = e;
	*(char **)(e + 0x12) = p;
	*(char **)(e + 0x16) = q;
}

/* make_ccl 0x7956..0x79ca: findings:D2 */
char bE17BA[];

char *make_ccl(p)
char *p;
{
	register char *q;
	register int neg;
	register int i;
	register int c;

	neg = 0;
	q = p;
	if (*q == 0x5e) {
		neg = 1;
		q++;
	}
	for (i = 0; i < 128; i++) {
		if (neg)
			bE17BA[i] &= ~2;
		else
			bE17BA[i] |= 2;
	}
	do {
		c = *q++;
		if ((c & 0x7f) == 0x5d)
			break;
		if (neg)
			bE17BA[c] |= 2;
		else
			bE17BA[c] &= ~2;
	} while (c != -1);
	return q;
}

/* f7bbc..7c26: findings:D4 */
int f7bbc(p, q, a)
int *p;
char *q;
char *a;
{
	register char *r;
	int c;

	if (*p == 0 || *p == 2) {
		(*p)++;
		*(int *)(q + 2) += 2;
	} else if (*p == 4) {
		*p += 2;
		*(char **)q = a;
		return;
	}
	r = *(char **)q;
	c = *(int *)r;
	*(int *)r = *(int *)(r + 2);
	*(int *)(r + 2) = c;
}

/* f6800..6866: findings:D2 */
int f6800(p, q, a)
char *p;
char *q;
long a;
{
	char b;

	*(long *)q = *(unsigned *)(p + 10);
	*(char *)(q + 4) = *(int *)(p + 8);
	*(int *)(p + 6) = 0;
	if ((int)a != 0) {
		b = *(char *)(p + 4) & 0x1f;
		if (b == 3 || b == 4)
			*(int *)(q + 6) = 1;
	}
}

/* f3b58..3c16: findings:D7 */
int bE4BA;

f3b58(p, a)
char *p;
char *a;
{
	register int k;
	int n;

	if (bE4BA)
		return;
	if (p != 0 && p[4] == 0x20)
		return;
	if (a[4] == 0x20)
		return;
	if (a[8] < 0)
		k++;
	if (a[8] == 0x5f) {
		k++;
		if (a[8] == 0x5f)
			return;
	}
	n = f6522(a + 8);
	if (n < 8)
		f71ca(9);
	f71ca(0x20);
	f3408(0, "x");
}

/* finish_output 0x2cbc..0x2d4c: done */
int bE4BC;
int bE4E8;
int bE4A3A;
char bECFA[];
char *bE048A;
char *bE0494;
int f6134();
int f2dde();
int f6efe();
int f3ac6();
int f9576();
int f96fa();
int f95c8();
int f2d4c();
int sig_handler();

finish_output()
{
	f6134();
	if (bE4A2)
		f2dde(bECFA);
	if (!bE4A4) {
		if (!bE4AA)
			f2dde(bF10A);
		f6efe();
	}
	f3ac6(bE4DA);
	f9576(*(int *)bE4DA);
	if (!bE4BC) {
		f96fa(bE048A);
		f95c8("x", bE048A);
		bE0494 = bE048A;
	}
	bE4E8 = bE4A3A;
	f2d4c();
	sig_handler();
}

/* f2d4c..2dde: findings:D2 */
int bE4F0;
int f95fe();
int f95da();
int f970a();
int f3408();

f2d4c()
{
	char buf[12];
	char rest[12];
	int n;

	n = f95fe(bE0494, 1);
	if (n == -1)
		f3408(2, "x");
	if (f95da(n, buf, 0) == -1)
		f3408(2, "y");
	bE4F0 = bE4F0 * 14;
	if (f970a(n, &bE4F0, 2) != 2)
		f3408(2, "z");
}

/* f2e76..2f3a: findings:D2 */
int bE418;
int bE41E;
char *bE41A;
char bE414[];
int f30a4();

f2e76(p, n)
register char *p;
int n;
{
	register int k;
	register char *q;

	k = n >> 1;
	bE418 -= k;
	if (bE418 >= 0) {
		bE41E -= k;
		if (bE41E > 0) {
			q = bE41A;
			do {
				*(int *)p = *(int *)q;
				q += 2;
				p += 2;
			} while (--k);
			bE41A = q;
			return;
		}
		bE41E += k;
	}
	bE418 += k;
	do {
		*(int *)p = f30a4(bE414);
		p += 2;
	} while (--k);
}

/* f2346..23fe: done */
int bE452;
long bE3C16;
long bE3C1A;
long b3C1E;
long bE462;
int bE468;
int bE4A8;
int bE4AE;

init_entry(p, a)
char *p;
int a;
{
	*(int *)p = bE452;
	*(long *)(p + 2) = bE3C16 + bE3C1A;
	if (*(long *)(p + 2) > 0x10000 && bE4A8)
		f3408(2, "x");
	*(long *)(p + 6) = b3C1E;
	*(int *)(p + 10) = a << 4;
	*(int *)(p + 18) = bE468;
	if (bE4AE)
		*(int *)(p + 18) |= 0x20;
	*(int *)(p + 12) = 0;
	*(long *)(p + 14) = bE462;
	*(int *)(p + 22) = 0;
	*(int *)(p + 20) = 0;
}

/* f14a4..1560: done */
long b3486;
int b488;
int bE4B6;
char bE438[];
int f2f3a();
int f2e76();
int f946e();
int f1696();

load_header(p)
long p;
{
	int magic;

	f2f3a(bE414, p, 0x1c);
	if (bE41E <= 0) {
		*(long *)b3486 = -1;
		b488 += 4;
		return 0;
	}
	f2e76(bE438, 0x1a);
	f2e76(&magic, 2);
	if (bE4B6)
		magic = f946e(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe607 || magic == 0xe611) {
		if (f1696(1, p + 13)) {
			*(long *)b3486 = p;
			b488 += 4;
		}
	}
	return 1;
}
