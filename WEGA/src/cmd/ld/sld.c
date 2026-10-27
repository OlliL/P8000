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
		report_error(2, "x");
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
		report_error(2, "x");
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
	ld_printf("x", (int)a);
	report_error(2, "x");
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
char *lookup_name();

f6b5a(p)
char *p;
{
	long dummy;

	f00ee(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	lookup_name();
}

/* f6b84 0x6b84..0x6bae: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
char bE472[];
char bE46E;
long bE46A;
int unpack_run();
int f00ee();
char *lookup_name();

long f6b84(p)
char *p;
{
	long dummy;

	unpack_run(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	lookup_name();
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
	report_error(2, "x");
}

/* f6ffe 0x6ffe..0x702a: done */
long g1688;
long f9184();

f6ffe(a)
int a;
{
	g1688 = f9184(a, 16);
	if (!g1688)
		report_error(2, "x");
}

/* f702a 0x702a..0x705e: done */
char *gE47E;
char *f9072();

f702a(a)
int a;
{
	gE47E = f9072(gE47E, a << 4);
	if (!gE47E)
		report_error(2, "x");
}

/* table32_entry 0x719e..0x71ca: done */
char tab1c02[];

char *table32_entry(x)
unsigned x;
{
	if (x < 0x0100)
		return tab1c02 + (x << 5);
	report_error(2, "x");
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
int put_name();
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
			put_name(s);
			report_error(1, "x");
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
			report_error(2, "x");
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
		report_error(2, "x");
	else {
		p = gE47E + (g04ee << 4);
		if (p > x)
			return ((int)x - (int)a) >> 4;
		report_error(2, "x");
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
		report_error(1);
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
			report_error(2, "x");
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
	n = put_name(a + 8);
	if (n < 8)
		put_char(9);
	put_char(0x20);
	report_error(0, "x");
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
int report_error();

f2d4c()
{
	char buf[12];
	char rest[12];
	int n;

	n = f95fe(bE0494, 1);
	if (n == -1)
		report_error(2, "x");
	if (f95da(n, buf, 0) == -1)
		report_error(2, "y");
	bE4F0 = bE4F0 * 14;
	if (f970a(n, &bE4F0, 2) != 2)
		report_error(2, "z");
}

/* f2e76..2f3a: findings:D2 */
int bE418;
int bE41E;
char *bE41A;
char bE414[];
int getw_buf();

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
		*(int *)p = getw_buf(bE414);
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
		report_error(2, "x");
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
int refill_wbuf();
int f2e76();
int f946e();
int f1696();

load_header(p)
long p;
{
	int magic;

	refill_wbuf(bE414, p, 0x1c);
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

/* put_char..7212: findings:D4 */
char *bE194E;
int bE1950;
int bE1952;
int f880e();

int put_char(c)
register int c;
{
	if (--bE1952 < 0)
		f880e(c, &bE194E);
	else {
		bE1950++;
		*bE194E = c;
	}
	return (unsigned char)c;
}

/* f06c6..0762: findings:D7 */
int bE4EC;
int bE3B8;
long bE3BA[];
long f9184();

long get_block(n)
int n;
{
	long p;

	if (n > bE4EC)
		report_error(2, "x");
	while (n > bE3B8) {
		p = f9184(1, 0x36);
		bE3B8++;
		bE3BA[bE3B8] = p;
		if (p == 0)
			report_error(2, "y");
	}
	return bE3BA[n];
}

/* f22a4..2346: findings:D7 */
char *bE494;
char bE49E[];
int f9586();
int f9576();
int f95fe();
int f96fa();

open_tmp(out, flag)
char *out;
int flag;
{
	register int fd;
	char *name;

	if (flag)
		name = bE49E;
	else
		name = bE494;
	fd = f9586(name, 0x1b6);
	if (fd < 0) {
		if (flag)
			report_error(2, "x");
		else
			report_error(2, "y");
	}
	f9576(fd);
	*(int *)out = f95fe(name, 2);
	if (flag)
		f96fa(bE49E);
	*(int *)(out + 2) = 0x100;
	*(char **)(out + 4) = out + 8;
}

/* f00ee..0178: findings:D2 */
int unpack_run();

pack_name(src, dst)
register char *src;
register char *dst;
{
	register char *s;
	register char *q;
	char cnt;

	cnt = 0;
	s = src;
	while (*s++ != 0)
		cnt++;
	if (cnt > 8) {
		*dst++ = cnt | 0x80;
		s--;
		q = src;
		while (q < s)
			*dst++ = *q++;
		cnt -= 8;
		while (cnt > 0)
			cnt -= 16;
		do {
			*dst++ = 0;
		} while (cnt-- != 0);
	} else
		unpack_run(src, dst);
}

/* f6e1a..6f46: findings:D4 */
int f2f04();
int f65ec();

f6e1a(a, b)
char *a;
char *b;
{
	char buf[8];
	char c4;
	char c7;
	long v;
	int i;
	int k;
	int cnt;
	char *q;

	c4 = b[4];
	c7 = b[7];
	v = *(long *)b;
	for (i = 0; i < 8; i++)
		buf[i] = b[i];
	if (bE4B6)
		f65ec(&v);
	f2f04(a, &v, 14);
	bE4F0++;
	if (b[8] & 0x80) {
		q = b + 16;
		k = 1;
		cnt = (b[8] & 0x7f) - 8;
		while (cnt > 0) {
			f2f04(a, q, 14);
			bE4F0++;
			q += 14;
			k++;
			cnt -= 14;
		}
		return k;
	}
	return 1;
}

/* put_name..65c6: done */
int bE4EA;
int bE1532;
int put_char();
int report_error();
int ld_printf();

int put_name(p)
char *p;
{
	register char *q;
	register int n;
	register int k;
	char flag;

	q = p;
	flag = 0;
	n = *q & 0x7f;
	if (n != *q) {
		q++;
		n--;
		if (bE4EA == 0) {
			n = 8;
			if (bE1532 == 0) {
				bE1532++;
				report_error(0, "x");
			}
		} else if (n > 0x2c) {
			flag++;
			n = 0x28;
		}
	} else
		n = 8;
	k = 0;
	while (n-- != 0) {
		if (*q == 0)
			break;
		put_char(*q);
		k++;
		q++;
	}
	if (flag) {
		ld_printf("y");
		k += 3;
	}
	return k;
}

/* ensure_buf..7186: done */
unsigned bE15DA;
long bE190DE;
long f9184();
int f903a();
int report_error();

long ensure_buf(n)
unsigned long n;
{
	if (bE15DA < n) {
		if (n > 0xffff)
			report_error(2, "x");
		if (bE15DA != 0)
			f903a(bE190DE);
		bE190DE = f9184(1, (int)n);
		if (bE190DE == 0)
			report_error(2, "y");
		bE15DA = (unsigned)n;
	}
	return bE190DE;
}

/* f23fe..2492: findings:D2 */
long bE486;
int bE4BE;
int f3144();
int refill_wbuf();
int f2e76();
int f2492();
int f9576();

f23fe(p)
char *p;
{
	register char *q;
	register long *lp;

	q = p;
	if (f3144(p) == 0) {
		while (*q != 0)
			q++;
		for (;;) {
			if (q < p)
				break;
			q--;
			if (*q == 0x2f)
				break;
		}
		q++;
		f2492(0L, q);
	} else {
		lp = (long *)bE486;
		while (*lp != -1) {
			refill_wbuf(bE414, *lp, 26);
			f2e76(bE438, 26);
			f2492(*lp + 13, bE438);
			lp++;
		}
		bE486 = (long)lp;
	}
	f9576(bE4BE);
}

/* f1f96..2056: findings:D4 */
int bE3C04;
char *bF324;
char *bF328;
char *table32_entry();
int report_error();

f1f96(p, kind)
char *p;
long kind;
{
	register char *e;

	if (bE3C04 > 0x100)
		report_error(2, "x");
	if ((int)kind == 2) {
		e = table32_entry(bE3C04++);
		bF324 = e;
		*e = 0xff;
		*(int *)(e + 6) = *(int *)(p + 6);
		*(int *)(e + 10) |= 2;
		*(int *)(p + 6) = 0;
		*(int *)(p + 10) &= ~2;
		*(char *)(p + 2) = 0;
	} else {
		e = table32_entry(bE3C04++);
		bF328 = e;
		*e = 0xff;
		*(int *)(e + 8) = *(int *)(p + 8);
		*(int *)(e + 10) |= 4;
		*(int *)(p + 8) = 0;
		*(int *)(p + 10) &= ~4;
		*(char *)(p + 3) = 0;
	}
}

/* swab12..3b58: findings:D2 */
swab12(p)
register char *p;
{
	register int k;
	register int t;

	k = 12;
	do {
		t = p[0];
		p[0] = p[1];
		p[1] = t;
		p += 2;
	} while (--k);
}

/* pack_entry..68a2: findings:D4 */
int f0178();

pack_entry(p, q)
char *p;
char *q;
{
	*(int *)(q + 10) = (int)*(long *)p;
	*(int *)(q + 8) = *(char *)(p + 4);
	f0178(p + 8, q);
}

/* f01d2..0220: done */
char *table16_entry();
char *f705e();
int f0220();
int ld_printf();

walk_entries()
{
	register char *a;
	register char *b;

	ld_printf("x");
	b = table16_entry(bE4FA);
	a = table16_entry(0);
	while (a < b) {
		f0220(a);
		a = f705e(a);
	}
}

/* report_limit..719e: done */
int ld_printf();

report_limit()
{
	ld_printf("x", bE4FA, 0x0ffa);
}

/* f705e..7112: findings:D2 */
char *bE47E;

next_entry(p)
char *p;
{
	register char *q;
	register char *e;
	int b;

	b = *(char *)(p + 8);
	b = (b << 8) >> 8;
	if (b < 0) {
		e = p + 8 + (b & 0x7f);
		q = p + 16;
		while (e > q)
			q += 16;
	} else
		q = p + 16;
	if (q < bE47E)
		report_error(2, "x");
	if (q >= bE47E + (g04ee << 4))
		report_error(2, "y");
	return q;
}

/* f6f46..6fce: findings:D7 */
int f3b58();
int report_error();

f6f46(p, n)
char *p;
int n;
{
	register char *e;
	char *q;
	long q2;

	e = p + n * 12;
	while (p < e) {
		if (p[4] & 0x80)
			q = p + 4 + (p[4] & 0x7f);
		q2 = *(long *)f6b84(p + 4);
		if (q2 != 0) {
			if (*(char *)(q2 + 4) != 0x20)
				f3b58(0, q2);
		}
		p += 12;
	}
}

/* getw_buf..3144: findings:D2 */
int bE410;
int refill_wbuf();
int report_error();

getw_buf(p)
char *p;
{
	register char *q;
	long v;
	int w;

	q = p;
	*(int *)(q + 4) -= 1;
	if (*(int *)(q + 4) < 0) {
		v = (long)(*(int *)(q + 2) + 1) << 8;
		refill_wbuf(q, v, -1);
		*(int *)(q + 4) -= 1;
	}
	*(int *)(q + 10) -= 1;
	if (*(int *)(q + 10) <= 0) {
		if (*(int *)(q + 10) < 0)
			report_error(2, "x");
		bE410++;
		*(int *)(q + 12) -= 1;
		*(char **)(q + 12) = (char *)&bE410;
	}
	w = *(int *)(*(char **)(q + 6));
	*(int *)(q + 8) += 2;
	return w;
}

/* scan_field..7956: findings:D2 */
int refill_wbuf();
int f8688();
int f86fa();

scan_field(dest, wc, stream, nret)
char *dest;
long wc;
char *stream;
int *nret;
{
	int width;
	int mode;
	int c;
	int conv;
	char *save;

	width = (int)(wc >> 16);
	conv = (int)wc;
	*nret = 0;
	save = dest;
	if (conv == 0x63 && width == 0x7530)
		width = 1;
	mode = 0;
	if (conv == 0x73)
		mode = 1;
	do {
		*(int *)(stream + 4) -= 1;
		if (*(int *)(stream + 4) >= 0) {
			c = *(unsigned char *)(*(char **)stream);
			*(char **)stream += 1;
		} else
			c = f86fa(stream);
		if (c == -1)
			break;
	} while (bE17BA[c] & mode);
	if (conv == 0x63)
		mode = 0;
	else if (conv == 0x5b)
		mode = 2;
	else
		mode = 1;
	while (c != -1) {
		if (bE17BA[c] & mode)
			break;
		if (dest != 0)
			*dest++ = c;
		width -= 1;
		if (width <= 0)
			break;
		*(int *)(stream + 4) -= 1;
		if (*(int *)(stream + 4) >= 0) {
			c = *(unsigned char *)(*(char **)stream);
			*(char **)stream += 1;
		} else
			c = f86fa(stream);
	}
	if (c == -1)
		*nret = 1;
	else {
		if (width > 0)
			f8688(c, stream);
		*nret = 0;
	}
	if (dest == 0 || dest == save)
		return 0;
	if (conv != 0x63)
		*dest++ = 0;
	return 1;
}

/* read_entry_hdr..6800: findings:D2 */
char bE47A[];
char bE15CC[];
int bE15D4;
int bE15D6;
int bE4B0;
int bE4B2;
int f2e76();
int f6800();
int f65ec();
int f65c6();

read_entry_hdr()
{
	char buf[14];
	char cnt;
	char *cur;
	char *end;
	int total;
	int result;
	int i;

	result = 0;
	if (bE4B2) {
		f2e76(buf, 14);
		bE46A = *(long *)buf;
		bE46E = buf[4];
		bE470 = buf[5];
		for (i = 0; i < 8; i++)
			bE472[i] = buf[6 + i];
		if (bE4B6)
			f65ec(&bE46A);
		if (bE472[0] & 0x80) {
			cnt = bE472[0] & 0x7f;
			end = buf + 6 + cnt;
			cur = buf;
			total = 0;
			while (cur < end) {
				f2e76(bE47A, 14);
				total += 14;
				cur += 14;
			}
			result = total / 16;
			if (total % 16 != 0)
				result += 1;
		}
	} else {
		f2e76(bE15CC, 12);
		if (bE4B6) {
			bE15D4 = f65c6(bE15D4);
			bE15D6 = f65c6(bE15D6);
		}
		f6800(bE15CC, &bE46A, bE4B0);
	}
	bE46A &= 0xff00ffffL;
	bE46E &= ~0x40;
	return result;
}

/* f64d0..6522: findings:D2 */
int f3a88();

int f64d0(n)
register int n;
{
	if (bE4A2)
		return 0;
	while (n > 0) {
		f3a88(0, bE4DA);
		if (bE4A2)
			f3a88(0, bECFA);
		n = n - 2;
	}
	return n;
}

/* f0f26..1070: findings:D15 */
char buf6988[];
char buf9098[];
char tab1aad[];
char *tab4a48[];
char **g4a40;
int g4a44;

int read_words(name)
char *name;
{
	int fd;
	int n;
	int idx;
	char c;
	int inword;
	char *p;

	fd = f95fe(name, 0);
	if (fd < 0)
		report_error(2, "x");
	p = buf6988;
	tab4a48[0] = buf6988;
	inword = 0;
	idx = 0;
	goto rd;
body:
	if (n == -1)
		report_error(2, "x");
	if (!(tab1aad[c] & 8)) {
		inword = 0;
		*p = c;
		p++;
		if (p > buf9098)
			report_error(2, "x");
	} else if (!inword) {
		inword++;
		*p = 0;
		p++;
		if (p > buf9098)
			report_error(2, "x");
		idx++;
		if (idx > 2000)
			report_error(2, "x");
		tab4a48[idx] = p;
	}
rd:
	n = f9610(fd, &c, 1) != 0;
	if (n)
		goto body;
	g4a40 = tab4a48;
	g4a44 = idx;
	f9576(fd);
	return 0;
}

/* f3408..34b8: done */
char *pE4D6;
char *pE4C0;
int ld_printf();
int sig_handler();

int report_error(level, msg, a, b, c, d, e)
int level;
char *msg;
int a;
int b;
int c;
int d;
int e;
{
	if (!bE4A3A)
		ld_printf("x", pE4D6);
	if (pE4C0)
		ld_printf("x", pE4C0);
	if (bE438[0])
		ld_printf("x", bE438);
	ld_printf("x");
	ld_printf(msg, b, a, c, d, e);
	ld_printf("x");
	if (level > 1)
		sig_handler();
	bE4A3A = level;
}

/* f4968..4a16: findings:D5 */
int tabF338[];
char *table32_entry();
int f5eec();
int report_error();

int f4968(q, p, x, res)
char *q;
int *p;
int x;
int *res;
{
	int lcl;

	if (bE4AE && x == 0x7f) {
		(*p)++;
		return 0;
	}
	q = table32_entry(bE3C04);
	if (bE3C04++ > 0x100)
		report_error(2, "x");
	if (*(int *)(tabF338 + x * 8) == 0x0bb8)
		*(int *)(q + 10) |= 8;
	else
		*(int *)q = 4;
	lcl = 1;
	*res = f5eec(q, p, 0, 0x72, &lcl);
}

/* f2f3a..3090: findings:D5,D9,D15 */
char b0004[];
char b020a[];
int f95da();
int f9610();
int f3090();
int report_error();

int refill_wbuf(q, v, n)
char *q;
long v;
int n;
{
	long hi;
	long lo;
	register char *cur;
	register char *obj;
	int nw;

	obj = q;
	hi = (v >> 8) & 0xff;
	lo = v & 0xff;
	*(int *)(*(char **)(obj + 12)) -= 1;
	cur = b0004;
	if ((long)*(int *)(cur + 2) == hi)
		goto common;
	cur = b020a;
	if ((long)*(int *)(cur + 2) == lo)
		goto common;
	if (*(int *)cur) {
		cur = b0004;
		if (*(int *)cur) {
			report_error(2, "x");
			goto common;
		}
	} else if (!*(int *)b0004 && *(int *)(b0004 + 2) < *(int *)(b020a + 2))
		cur = b0004;
	*(int *)(cur + 2) = hi;
	f95da(bE4BE, (v & 0xff00) << 1, 0);
	nw = f9610(bE4BE, cur + 6, 0x200) >> 1;
	if (nw < 0)
		nw = 0;
	*(int *)(cur + 4) = nw;
common:
	*(int *)cur += 1;
	*(int *)(obj + 2) = hi;
	*(char **)(obj + 12) = cur;
	*(char **)(obj + 6) = cur + 6 + (lo << 1);
	if (n != -1)
		*(int *)(obj + 10) = f3090(n);
	*(int *)(obj + 4) = *(int *)(cur + 4) - (int)lo;
	if (*(int *)(obj + 4) <= 0)
		*(int *)(obj + 10) = 0;
}

/* f42f8..4444: findings:D6,D15,D16 */
int bE474;
int report_error();
int f42f8();

int f42f8(x)
long x;
{
	if (((int)x) < 0 || ((int)x) >= 0x80)
		report_error(2, "x", ((int)x));
	if (((int)x) > bE3E82)
		bE3E82 = ((int)x);
	switch (((char *)&x)[1]) {
	case 'M':
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x07d0) {
			report_error(0, "x");
			bE474 = -1;
		}
		*(int *)(tabF338 + ((int)x) * 8) = 0x01f4;
		break;
	case 'r':
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x07d0) {
			report_error(0, "x");
			bE474 = -1;
		}
		*(int *)(tabF338 + ((int)x) * 8) = 0x03e8;
		break;
	case 's':
		if (bE474 > -1)
			*(int *)(tabF338 + bE474 * 8) = -1;
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x03e8) {
			report_error(0, "x");
			bE474 = -1;
		} else {
			*(int *)(tabF338 + ((int)x) * 8) = 0x07d0;
			bE474 = ((int)x);
		}
		break;
	}
}

/* f6c6a..6da8: findings:D2,D6,D15 */
int bE3C02;
int bEF32C;
char *f06c6();
int f6c6a();

f6c6a()
{
	register char *w;
	register char *p;
	int i;

	i = bE3C02 + 0x0470;
	p = f06c6(i);
	w = (char *)&bE46A;
	switch (bE46E) {
	case 2:
	case 0x22:
		goto h2;
	case 3:
	case 0x23:
		goto h3;
	case 4:
	case 0x24:
		goto h4;
	case 0x1f:
		goto loop;
	case 0x20:
		goto end;
	default:
		if (bE46E & 0x20)
			bE46E = 0x21;
		goto end;
	}
loop:
	if (i >= bEF32C)
		goto clear;
	p = f06c6(i);
	if (*(int *)(p + 4))
		goto h2;
	i++;
	goto loop;
clear:
	bE470 = 0;
	bE46A = 0;
	goto end;
h2:
	*(int *)w = *(*(char **)(p + 0x16)) << 8;
	w += 2;
	*(int *)w = *(int *)(p + 0x10) + *(int *)(p + 0x22) + *(int *)(*(char **)(p + 0x16) + 0x1a) + *(int *)w;
	bE470 = *(int *)(*(char **)(p + 0x16) + 0x10);
	goto end;
h3:
	*(int *)w = *(*(char **)(p + 0x1a)) << 8;
	w += 2;
	*(int *)w = *(int *)(p + 0x12) + *(int *)(p + 0x24) + *(int *)(*(char **)(p + 0x1a) + 0x1c) + *(int *)w;
	bE470 = *(int *)(*(char **)(p + 0x1a) + 0x10);
	goto end;
h4:
	*(int *)w = *(*(char **)(p + 0x1e)) << 8;
	w += 2;
	*(int *)w = *(int *)(p + 0x14) + *(int *)(p + 0x26) + *(int *)(*(char **)(p + 0x1e) + 0x1e) + *(int *)w;
	bE470 = *(int *)(*(char **)(p + 0x1e) + 0x10);
end:
	;
}

/* f1560..1664: findings:D2,D5,D8 */
char *bE3E84;
int bE3E88;
long f6b84();
int f65ec();
int f14a4();
int f1560();

int f1560()
{
	long sv;
	char *end;
	int i;
	long t;

	sv = bE486;
	end = bE3E84;
	i = 0;
	goto test;
body:
	if (bE3E84 + i * 12 < end)
		goto next;
	if (*(bE3E84 + i * 12 + 4) & 0x80)
		end = bE3E84 + i * 12 + 4 + (*(bE3E84 + i * 12 + 4) & 0x7f);
	t = *(long *)f6b84(bE3E84 + i * 12 + 4);
	if (!t)
		goto next;
	if (*(char *)(t + 4) != 0x20)
		goto next;
	if (bE4B6)
		f65ec(bE3E84 + i * 12);
	f14a4((*(long *)(bE3E84 + i * 12)) >> 1);
next:
	i++;
test:
	if ((unsigned)i < (unsigned)bE3E88)
		goto body;
	return sv != bE486;
}

/* f1364..14a4: findings:D7,D15 */
long bE44E;
int bE488;
int f3144();
int f1696();
int f1560();
int f6f46();
int f95da();
int f9610();
int f9576();
int load_header();
long ensure_buf();
int report_error();

int f1364(p)
char *p;
{
	register char *r;
	long n;

	r = p;
	switch (f3144(p)) {
	case 0:
		f1696(0, 0L);
		goto end;
	case 1:
		n = 1;
		if (!load_header(n))
			goto end;
		goto step;
	case 2:
		bE3E88 = bE44E / 12;
		bE3E84 = ensure_buf(bE44E);
		f95da(bE4BE, 28L, 0);
		f9610(bE4BE, bE3E84, bE3E88 * 12);
		f6f46(bE3E84, bE3E88);
		while (f1560())
			;
		*(long *)bE486 = -1;
		bE488 += 4;
		goto end;
	case 3:
		report_error(0, "x");
		n = ((bE44E + 27) >> 1) + 1;
		goto check;
	}
step:
	n = ((bE44E + 27) >> 1) + n;
check:
	if (load_header(n))
		goto step;
end:
	f9576(bE4BE);
}

/* f724e..7286: findings:D5,D16 */
int f7300();
int f724e();

int f724e(a, b, c, d, e)
long a;
long b;
long c;
long d;
long e;
{
	long arr[2];

	arr[0] = c;
	arr[1] = d;
	f7300(a, b, arr, &e);
}

/* f7212..724e: findings:D5,D16 */
int f7300();
int f7212();

int f7212(a, b, c, d)
long a;
long b;
long c;
long d;
{
	long arr[2];

	arr[0] = b;
	arr[1] = c;
	f7300("x", a, arr, &d);
}

/* f7286..7300: findings:D4,D16 */
int f7300();
int f7286();

int f7286(s, b, a, c, d)
char *s;
long b;
long a;
long c;
long d;
{
	register char *q;
	long p1;
	int len;
	long p2;
	char ch;
	long a1;
	long c1;

	p2 = s;
	p1 = s;
	len = 0;
	ch = 'A';
	q = s;
	while (*q++)
		len++;
	a1 = a;
	c1 = c;
	f7300(&p1, b, &a1, &d);
}

/* f1070..11ea: findings:D6,D15 */
char bE42B2;
int bE49A;
char tab42b4[];
char tab4534[];
int f7286();
int report_error();

int f1070(s, t)
char *s;
char *t;
{
	register int k;
	register int g;
	register char *u;
	char *fmt;

	bE42B2++;
	switch (*(s + 1)) {
	case 0:
		k = 0;
		break;
	case 'b':
		k = 3;
		break;
	case 'd':
		k = 2;
		break;
	case 't':
		k = 1;
		break;
	default:
		report_error(2, "x", s);
		break;
	}
	u = t;
	g = 0;
	if (bE49A && (k == 2 || k == 3))
		g = 1;
	if (tab42b4[g * 5 + k])
		report_error(2, "x", s, tab42b4[g * 5 + k], s[1]);
	tab42b4[g * 5 + k] = *s;
	fmt = "x";
	if (*u != '0')
		fmt = "x";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "x";
	}
	if (f7286(u, fmt, tab4534 + g * 10 + k * 2) != 1)
		report_error(2, "x", s, t);
	return k != 0;
}

/* f11ea..1364: findings:D2,D5,D6,D7,D15,D16 */
long bE4A40;
int bE4A44;
char tab3e8a[];
int f44e6();
int f2056();
int f23fe();
int f2cbc();
char *f92c6();
int parse_args();

int parse_args(argc, argv)
int argc;
char **argv;
{
	register int k;
	register char *a;
	register char **av;
	register int j;
	int saved_k;
	char **saved_av;
	char **base;
	int saved_argc;
	int flag;
	char *p;

	pE4C0 = 0;
	f44e6();
	f2056();
	av = argv + 1;
	bEF32C = 0;
	bE3C02 = 0;
	bE486 = (long)tab3e8a;
	k = 1;
	goto test;
body:
	a = *av++;
	if (*a != '-') {
		f23fe(a);
		goto next;
	}
	j = 1;
scan:
	if (!a[j])
		goto next;
	switch (a[j]) {
	case 'A':
	case 'e':
	case 'o':
	case 'u':
	case 'v':
		goto consume;
	case 'b':
	case 't':
		if (!a[j + 1])
			goto consume;
		j++;
		goto consume;
	case 'N':
		j += 2;
		goto scan;
	case 'M':
		p = f92c6(a, ':');
		if (p && !p[2]) {
			k++;
			av++;
		}
		if (**av != '+')
			goto next;
		do {
			k++;
			av++;
		} while (**av != '+');
		k++;
		av++;
		goto next;
	case 'f':
		saved_k = k;
		k = -1;
		saved_av = av;
		base = argv;
		saved_argc = argc;
		argc = bE4A44;
		av = (char **)bE4A40;
		flag = 1;
		goto next;
	case 'l':
		j--;
		a[j] = '-';
		f23fe(a + j);
		goto next;
	case 'R':
		goto next;
	default:
		j++;
		goto scan;
	}
consume:
	k++;
	av++;
	j++;
	goto scan;
next:
	k++;
test:
	if (k < argc)
		goto body;
	if (flag) {
		k = saved_k + 2;
		av = saved_av + 1;
		argv = base;
		argc = saved_argc;
		flag = 0;
		goto test;
	}
	f2cbc();
}

/* f68a2..6a08: findings:D2,D16 */
char *pE3C0A;
char *pE3C06;
char *pE3C0E;
int put_name();
int put_char();
int report_error();
int f68a2();

int f68a2(p, c)
register char *p;
int c;
{
	char *q;

	if (!p)
		goto end;
	if (p[4] == 0x20 && *(long *)p == 0)
		goto init;
	if (put_name(p + 8) < 8)
		put_char(9);
	put_char(0x20);
	report_error(1, "x");
	goto end;
init:
	p[4] = c;
	*(long *)p = 0;
	*(int *)(p + 6) = 0;
	if (c == 0x22) {
		q = pE3C0A;
		if (!q)
			goto end;
		*(long *)p = ((long)(*q) << 24) + *(int *)(q + 4);
		goto tail;
	}
	if (c == 0x23) {
		q = pE3C06;
		if (!q)
			goto end;
		*(long *)p = *(int *)(q + 4) + *(int *)(q + 6);
		goto add;
	}
	if (c == 0x24) {
		q = pE3C0E;
		if (!q)
			goto end;
		*(long *)p = *(int *)(q + 4) + *(int *)(q + 6) + *(int *)(q + 8);
		goto add;
	}
	if (c != 0x21)
		goto end;
	*(long *)p = 0;
	if (bE474 == -1 || bE474 == -3)
		*(int *)(p + 6) = bE3E82;
	else
		*(int *)(p + 6) = bE474;
	*(long *)p = *(long *)p + ((long)*(int *)(p + 6) << 24);
	goto end;
add:
	*(long *)p = *(long *)p + ((long)(*q) << 24);
tail:
	*(int *)(p + 6) = *(int *)(q + 0x10);
end:
	;
}

/* f2492..2624: findings:D2,D7 */
int bE434;
int bE436;
long bE454;
long bE458;
int bE460;
int bE4A6;
int bE4AC;
int f3516();
int f6a08();
int f6c6a();
int f6e1a();
char *lookup_name();
int report_error();
int read_entry_hdr();
int put_name();
int put_char();
int f2624();
int refill_wbuf();

int f2492(a, b)
long a;
char *b;
{
	register int k;
	register char *rec;
	register char *t;
	long v;
	int n;
	char c;
	char d;

	f3516(a, 2);
	f6a08(b);
	t = tab84d8;
	v = (unsigned)((bE436 + bE434) & ~1) >> 1;
	v = v + a;
	refill_wbuf(bE414, v + bE454 + bE458, bE460);
	k = 0;
	while (bE41E > 0) {
		n = read_entry_hdr() + 1;
		f6c6a();
		c = bE46E;
		if (bE4A6) {
			d = c & 0x1f;
			if (d == 1 || d > 4)
				goto next;
		}
		if ((c & 0x20) == 0) {
			if (bE4A4 || bE4AA)
				goto next;
			if (bE4AC && bE472[0] == 0x4c)
				goto next;
			f6e1a(bF10A, &bE46A);
			goto next;
		}
		rec = *(char **)lookup_name();
		if (!rec)
			report_error(2, "x");
		if (bE46E != 0x20)
			goto other;
		if (t >= (char *)&gE47E)
			report_error(2, "x");
		*(int *)t = k;
		*(char **)(t + 2) = rec;
		t += 6;
		goto next;
other:
		if (*(char *)(rec + 4) == bE46E && bE46A == *(long *)rec)
			goto next;
		if (put_name(bE472) < 8)
			put_char(9);
		put_char(9);
		report_error(1, "x");
next:
		k += n;
	}
	f2624(v, t);
}

/* f2624..27d8: findings:D7,D16 */
char bE424[];
int f3090();
int f2e76();
int swap_quad();
int refill_wbuf();
int f27d8();

int f2624(a, b)
long a;
char *b;
{
	int i;
	long acc;
	long base;
	long half;
	int lim;
	char tmp[16];
	int key;
	int n;
	acc = a;
	base = a - (bE434 / 2);
	lim = bE434 / (n * 2);
	half = (bE454 + bE458) >> 1;
	for (i = 0; i < lim; i++) {
		refill_wbuf(bE414, base + i, n * 2);
		f2e76(tmp, n * 2);
		if (bE4B6)
			swap_quad(tmp);
		key = *(int *)(tmp + 10) & 3;
		switch (key) {
		case 1:
			refill_wbuf(bE414, acc, *(int *)(tmp + 4));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 4));
			acc += f3090(*(int *)(tmp + 4));
			f27d8(b, bE6E2, bEF02);
			break;
		case 3:
			refill_wbuf(bE414, acc, *(int *)(tmp + 4));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 4));
			acc += f3090(*(int *)(tmp + 4));
			f27d8(b, bE6E2, bEF02);
		case 2:
			refill_wbuf(bE414, acc, *(int *)(tmp + 6));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 6));
			acc += f3090(*(int *)(tmp + 6));
			f27d8(b, bE8EA, bEAF2);
			break;
		}
	}
}

/* f27d8..2cbc: findings:D2,D7,D15 */
int bEF312;
int f3a88();
int f6c6a();
int f65c6();
char *table_lookup();
int table16_index();
int getw_buf();
int report_error();

int f27d8(p, q, r)
char *p;
char *q;
char *r;
{
	int flag;
	char *save;
	register int w13;
	register int w12;
	register int w11;
	register char *e;
	register int w10;

	flag = 0;

top:
	bE41E -= 1;
	if (bE41E > 0)
		goto c2826;
	if (bE41E >= 0)
		goto c2814;
	goto end;
c280e:
	*(int *)(bE414 + 4) += 1;
	goto c2814;
c2814:
	bE41E += 1;
	w12 = getw_buf(bE414);
	goto c2844;
c2826:
	*(int *)(bE414 + 4) -= 1;
	if (*(int *)(bE414 + 4) < 0)
		goto c280e;
	w12 = *(int *)*(char **)(bE414 + 6);
	*(int *)(bE414 + 8) += 2;
c2844:
	*(int *)(bE424 + 10) -= 1;
	if (*(int *)(bE424 + 10) > 0)
		goto c2882;
	if (*(int *)(bE424 + 10) >= 0)
		goto c2870;
	report_error(2, "x");
	goto c2870;
c286a:
	*(int *)(bE424 + 4) += 1;
c2870:
	*(int *)(bE424 + 10) += 1;
	w13 = getw_buf(bE424);
	goto c28a0;
c2882:
	*(int *)(bE424 + 4) -= 1;
	if (*(int *)(bE424 + 4) < 0)
		goto c286a;
	w13 = *(int *)*(char **)(bE424 + 6);
	*(int *)(bE424 + 8) += 2;
	goto c28a0;
c28a0:
	if (bE4B6) {
		w12 = f65c6(w12);
		w13 = f65c6(w13);
	}
	if (!(w13 & 8)) {
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 0:
		goto c2a3c;
	case 1:
		goto c29be;
	case 2:
		goto c28fa;
	}
	goto e1;
c28fa:
	if (!(w13 & 1))
		report_error(2, "x");
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)(w12 & 0xff);
	bE470 = (unsigned)w13 >> 8;
	f6c6a();
	w12 = (int)bE46A;
	if (w12 & 0xff00)
		report_error(2, "x");
c2954:
	w12 += ((int)(bE46A >> 16)) & 0xff00;
c2964:
	w13 = ((bE470 & 0xff) << 8) + (w13 & 0xff);
emit:
	if (bE4B6) {
		w12 = f65c6(w12);
		w13 = f65c6(w13);
	}
	f3a88(w12, q);
	if (bE4A2)
		f3a88(w13, r);
	goto top;
c29be:
	if (!(w13 & 1))
		report_error(2, "x");
	w10 = w12;
	bE470 = (unsigned)w13 >> 8;
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)w12;
	f6c6a();
	w10 = (w10 & 0x8000) | (int)(bE46A >> 16);
	if (bE4B6)
		w10 = f65c6(w10);
	f3a88(w10, q);
	w11 = w13;
	flag = 1;
	goto top;
c2a3c:
	if (!(w13 & 0xe))
		goto c2a96;
	if (!flag)
		goto c2a68;
	if (((w13 != 0xff0f) & w11 & 0xff0f) == 0)
		goto c2a68;
	w13 &= 0xf0;
	w13 |= w11 & 0xff0f;
c2a68:
	bE470 = (unsigned)w13 >> 8;
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)w12;
	f6c6a();
c2a96:
	if (!flag)
		goto c2ad6;
	flag = 0;
	w11 = ((bE470 & 0xff) << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = f65c6(w11);
	if (bE4A2)
		f3a88(w11, r);
c2ad6:
	if (!(w13 & 0xe))
		goto emit;
	w12 = (int)bE46A;
	goto c2964;
e1:
	report_error(2, "x");
	goto emit;
	} else {
		e = table_lookup(p, w13);
		if (e[4] != 0x20)
			goto c2b46;
		w13 = (w13 & 7) + ((table16_index(e) + bEF312) << 4) + 8;
		goto emit;
	}

c2b46:
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 0:
		goto c2c12;
	case 1:
		goto c2bcc;
	case 2:
		goto c2b7c;
	}
	goto e2;
c2b7c:
	w13 = (e[4] - 0x21) * 2 + 0x21;
	w13 = ((*(int *)(e + 6)) << 8) + (w13 & 0xff);
	w12 = (w12 + 0xff) & (w12 & 0xffff);
	w12 = (w12 & 0xffff);
	if (w12 & 0xff00)
		report_error(2, "x");
	w12 += ((int)(*(long *)e >> 16)) & 0xff00;
	goto emit;
c2bcc:
	if (!(w13 & 1))
		report_error(2, "x");
	w10 = (w12 & 0x8000) | ((int)(*(long *)e >> 16) & 0xff00);
	if (bE4B6)
		w10 = f65c6(w10);
	f3a88(w10, q);
	save = e;
	w11 = w13;
	flag = 1;
	goto top;
c2c12:
	if (!flag)
		goto c2c72;
	flag = 0;
	if (e != save)
		report_error(2, "x");
	w11 = (e[4] - 0x21) * 2 + 0x11;
	w11 = ((*(int *)(e + 6)) << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = f65c6(w11);
	if (bE4A2)
		f3a88(w11, r);
c2c72:
	w13 = (e[4] - 0x21) * 2 + 1;
	w13 = ((*(int *)(e + 6)) << 8) + (w13 & 0xff);
	w12 = w12 + (int)*(long *)e;
	goto emit;
e2:
	report_error(2, "x");
	goto emit;
end:
	;
}

/* f3516..3760: findings:D2,D16 */
int bE4B4;
long bE45C;
char bE466;
char bE467;
int f2e76();
int refill_wbuf();
int f65c6();
int f6620();
int f932e();
int f3760();
int f3408();

int f3516(a, b)
long a;
int b;
{
	int magic;
	char tmp[24];
	char rec[8208];
	register char *p;
	register int n;
	register int w;

	refill_wbuf(bE414, a, 2);
	f2e76(&magic, 2);
	if (bE4B6)
		magic = f65c6(magic);
	if (magic == 0xe607 || magic == 0xe611)
		bE4B4 = 1;
	if (magic != 0xe707 && magic != 0xe711) {
		if (!bE4B4)
			return;
	}
	bE4B2 += 1;
	bE436 = 0x18;
	refill_wbuf(bE414, a, bE436);
	f2e76(tmp, bE436);
	if (bE4B6)
		f932e(tmp, tmp, bE436);
	bE434 = *(int *)(tmp + 10);
	n = bE434 / 16;
	refill_wbuf(bE414, a + ((bE436 & ~1) >> 1), bE434);
	f2e76(rec, bE434);
	if (bE4B6) {
		for (p = rec; p < rec + n * 16; p += 16)
			f6620(p);
	}
	bE452 = magic & 0xf7ff;
	bE454 = 0;
	bE458 = 0;
	bE45C = *(long *)(tmp + 6);
	bE466 = 0;
	bE467 = 0;
	w = 0;
	for (p = rec; p < rec + n * 16; p += 16) {
		*(int *)(p + 4) = (*(int *)(p + 4) + 1) & 0xfffe;
		*(int *)(p + 6) = (*(int *)(p + 6) + 1) & 0xfffe;
		*(int *)(p + 8) = (*(int *)(p + 8) + 1) & 0xfffe;
		bE454 += (unsigned)*(int *)(p + 4);
		bE458 += (unsigned)*(int *)(p + 6);
		w |= *(int *)(p + 10);
	}
	if (w & 0xf8)
		f3408(2, "x");
	bE462 = *(long *)(tmp + 14);
	bE468 = *(int *)(tmp + 18) & 1;
	bE460 = *(int *)(tmp + 12);
	f3760(rec, n, b);
	return 1;
}

/* f410c..42f8: findings:D2,D5 */
int bE476;
int bE478;
char tab3c22[];
int f3a30();
int report_error();

int f410c(x, y, c)
int x;
char *y;
char c;
{
	int i;
	int prev;
	int k;
	int r;

	if ((*(int *)bE47A) == 0x32)
		report_error(2, "x", 0x32);
	i = bE478;
	prev = 0;
	bE476 = 1;
	k = 0;
	goto cond;
body:
	r = f3a30(y, *(char **)(tab3c22 + i * 12 + 8));
	if (r < 0)
		goto nxt;
	if (r == 0)
		goto attr;
	prev = i;
	i = *(int *)(tab3c22 + i * 12 + 6);
nxt:
	k++;
cond:
	if (k < (*(int *)bE47A))
		goto body;
	if (i != bE478 || k != 0)
		goto l41de;
	*(int *)(tab3c22 + (*(int *)bE47A) * 12 + 6) = bE478;
	bE478 = (*(int *)bE47A);
	goto l4218;
l41de:
	*(int *)(tab3c22 + (*(int *)bE47A) * 12 + 6) = *(int *)(tab3c22 + prev * 12 + 6);
	*(int *)(tab3c22 + prev * 12 + 6) = (*(int *)bE47A);
l4218:
	k = (*(int *)bE47A);
	(*(int *)bE47A)++;
	*(char **)(tab3c22 + k * 12 + 8) = y;
	*(int *)(tab3c22 + k * 12 + 4) = -1;
	*(int *)(tab3c22 + k * 12 + 2) = -1;
	*(int *)(tab3c22 + k * 12) = -1;
attr:
	if (c == 0x61 || c == 0x74)
		*(int *)(tab3c22 + k * 12) = x;
	if (c == 0x61 || c == 0x64)
		*(int *)(tab3c22 + k * 12 + 2) = x;
	if (c == 0x61 || c == 0x62)
		*(int *)(tab3c22 + k * 12 + 4) = x;
	bE476 = 0;
}

/* ld_printf..7a5e: done */
int bE90E2;
char *pE90E4;
char str194e[];
int ld_doprnt();

int ld_printf(fmt, w1, w2, w3, w4, d1, d2, d3, d4, s1)
long fmt;
int w1;
int w2;
int w3;
int w4;
double d1;
double d2;
double d3;
double d4;
long s1;
{
	int wb0;
	int wb1;
	int wb2;
	int wb3;
	double db0;
	double db1;
	double db2;
	double db3;

	wb0 = w1;
	wb1 = w2;
	wb2 = w3;
	wb3 = w4;
	db0 = d4;
	db1 = d3;
	db2 = d2;
	db3 = d1;
	pE90E4 = str194e;
	bE90E2 = ld_doprnt(fmt, &wb0, &db0, &s1, 1, str194e);
}

/* ld_doprnt..85c6: findings:D2,D7 */
struct sio {
	unsigned char *ptr;
	int cnt;
	char *base;
	char flag;
};
int f8c86();
int f880e();
int f85c6();

int ld_doprnt(f, ap, dp, sptr, scnt, iop)
register unsigned char *f;
char *ap;
char *dp;
char *sptr;
int scnt;
struct sio *iop;
{
	register int c;
	register int rem;
	register int pn;
	int ncnt;
	char *wptr;
	char pa12[4];
	int prec;
	int width;
	int size1a;
	char pd1c[4];
	char *pe20;
	char buf24[64];
	char *bufp;
	char pad68[1];
	char b69[1];
	char pad6a[1];
	char b6b[1];
	char pad6c[1];
	char sign[1];
	char pb6e[4];
	int fleft;
	int padn;
	int fhash;
	int fsp;
	int fplus;
	int outc;
	int nout;
	char c80[1];
	char padc[1];
	int conv82;

	wptr = ap;
	nout = 0;
	ncnt = 0;
	rem = f8c86(f);
	goto l7c8c;
l7c50:
	--iop->cnt < 0 ? f880e(c, iop) : (int)(*iop->ptr++ = c);
	nout += 1;
l7c8c:
	if (rem-- == 0)
		goto l85c6;
	if ((c = *f++) != 0x25)
		goto l7c50;
	if (rem == 0)
		goto l85c6;
	rem -= 1;
	c = *f++;
	prec = 0;
	width = -1;
	padc[0] = 0x20;
	outc = 0;
	fsp = 0;
	fplus = 0;
	fhash = 0;
	fleft = 0;
l7d26:
	if (c - 0x20 > 0x0d)
		goto l7d50;
	switch (c - 0x20) {
	case 0:
		goto l7d02;
	case 1:
		goto l7cf4;
	case 3:
		goto l7d10;
	case 13:
		goto l7d3c;
	}
l7d50:
	if (!fplus)
		goto l7ce0;
	sign[0] = 0x2b;
l7d5c:
	if (c < 0x61)
		goto l7e26;
	goto l7e3e;
l7ce0:
	if (!fsp)
		goto l7d56;
	sign[0] = 0x20;
	fplus = 1;
	goto l7d5c;
l7d56:
	sign[0] = 0x2b;
	goto l7d5c;
l7d64:
	if (c != 0x2a)
		goto l7d94;
	prec = *(int *)wptr;
	wptr += 2;
	scnt += 1;
	if (scnt == 5)
		wptr = sptr;
	goto l7dbc;
l7d94:
	if (c != 0x30)
		goto l7da0;
	padc[0] = 0x30;
l7da0:
	f -= 1;
	rem += 1;
	pn = f85f8(f, &prec, rem);
	f += pn;
	rem -= pn;
l7dbc:
	if (rem == 0)
		goto l7e3e;
	if (*f != 0x2e)
		goto l7e18;
	f += 1;
	rem -= 1;
	if (rem == 0)
		goto l7e3e;
	if (*f != 0x2a)
		goto l7e00;
	width = *(int *)wptr;
	wptr += 2;
	pn = 1;
	scnt += 1;
	if (scnt == 5)
		wptr = sptr;
	goto l7e10;
l7e00:
	pn = f85f8(f, &width, rem);
l7e10:
	rem -= pn;
	f += pn;
l7e18:
	if (rem == 0)
		goto l7e3e;
	rem -= 1;
	c = *f++;
l7e26:
	if (c == 0x2a)
		goto l7d64;
	if (c < 0x30)
		goto l7e38;
	if (c <= 0x39)
		goto l7d64;
l7e38:
	if (c == 0x2e)
		goto l7d64;
l7e3e:
	if (width >= 0)
		goto l7e6a;
	switch (c) {
	case 'E':
	case 'G':
	case 'e':
	case 'f':
	case 'g':
		width = 6;
		goto l7e6a;
	case 's':
		width = 0;
		goto l7e6a;
	}
l7e6a:
	conv82 = c;
	switch (c) {
	case 'D':
		goto l7f46;
	case 'O':
		goto l7edc;
	case 'U':
		goto l7f3e;
	case 'X':
		goto l7ee4;
	case 'o':
		goto l7e9a;
	case 'u':
		goto l7eaa;
	case 'x':
		goto l7ea2;
	case 'l':
		goto l7f4e;
	case 'c':
		goto l8212;
	case 'd':
		goto l8020;
	case 'E':
	case 'G':
	case 'e':
	case 'f':
	case 'g':
		goto l824e;
	case 's':
		goto l819a;
	}
	goto l849a;
l7e9a:
	size1a = 8;
	goto l7eb0;
l7ea2:
	size1a = 16;
	goto l7eb0;
l7eaa:
	size1a = 10;
l7eb0:
	*(long *)pd1c = (long)*(int *)wptr;
	wptr += 2;
	*(int *)pd1c = 0;
	scnt += 1;
	if (scnt == 5)
		wptr = sptr;
	goto l8052;
l7edc:
	size1a = 8;
	goto l7eea;
l7ee4:
	size1a = 16;
	goto l7eea;
l7f3e:
	size1a = 10;
	goto l7eea;
l7f46:
	size1a = -10;
l7eea:
	if (scnt >= 5)
		goto l7f0c;
	f7bbc(&scnt, (char *)&wptr, sptr);
l7f0c:
	*(long *)pd1c = *(long *)wptr;
	wptr += 4;
	scnt += 2;
	if (scnt != 5)
		goto l8052;
	wptr = sptr;
	goto l8052;
l7f4e:
	if (rem == 0)
		goto l7fee;
	rem -= 1;
	if ((c = *f++) == 'o')
		goto l7edc;
	if (c == 'x')
		goto l7ee4;
	if (c == 'd')
		goto l7f46;
	if (c == 'u')
		goto l7f3e;
	--iop->cnt < 0 ? f880e(0x6c, iop) : (int)(*iop->ptr++ = 0x6c);
	--iop->cnt < 0 ? f880e(c, iop) : (int)(*iop->ptr++ = c);
	nout += 2;
l7fee:
	if (width == 0)
		goto l8006;
	if (conv82 != 's')
		goto l8006;
	if (pn > width)
		pn = width;
l8006:
	if (pn >= prec)
		goto l84b6;
	padn = prec - pn;
	goto l84ba;
l8020:
	size1a = -10;
	*(long *)pd1c = (long)*(int *)wptr;
	wptr += 2;
	scnt += 1;
	if (scnt == 5)
		wptr = sptr;
	goto l8052;
l8052:
	if (!fplus)
		goto l808a;
	if (size1a != -10)
		goto l808a;
	if (*(long *)pd1c < 0)
		goto l808a;
	buf24[0] = sign[0];
	outc = 1;
	bufp = buf24 + 1;
	goto l8100;
l808a:
	if (size1a != 8)
		goto l80b8;
	if (!fhash)
		goto l80b8;
	if (*(long *)pd1c == 0)
		goto l80b8;
	buf24[0] = 0x30;
	outc = 1;
	bufp = buf24 + 1;
	goto l80fc;
l80b8:
	if (size1a != 16)
		goto l80f4;
	if (!fhash)
		goto l80f4;
	if (*(long *)pd1c == 0)
		goto l80f4;
	buf24[0] = 0x30;
	buf24[1] = 0x78;
	outc = 2;
	bufp = buf24 + 2;
	goto l80fc;
l80f4:
	bufp = buf24;
l80fc:
l8100:
	if (size1a != -10)
		goto l8170;
	if (*(long *)pd1c >= 0)
		goto l8170;
	if (*(long *)pd1c == 0x80000000)
		size1a = 10;
	*(long *)pd1c *= -1;
	--iop->cnt < 0 ? f880e(0x2d, iop) : (int)(*iop->ptr++ = 0x2d);
	nout += 1;
l8170:
	pe20 = buf24;
	pn = f8ad0(bufp, size1a, *(long *)pd1c) + outc;
	goto l7fee;
l8212:
	padc[0] = 0x20;
	pn = 1;
	pe20 = wptr;
	pe20 += 1;
	wptr += 2;
	scnt += 1;
	if (scnt != 5)
		goto l7fee;
	wptr = sptr;
	goto l7fee;
l819a:
	padc[0] = 0x20;
	if (scnt >= 5)
		goto l81c2;
	f7bbc(&scnt, (char *)&wptr, sptr);
l81c2:
	pe20 = (char *)*(long *)wptr;
	wptr += 4;
	if (pe20 == 0)
		pe20 = (char *)0x0100186AL;
	pn = f8c86(pe20);
	scnt += 2;
	if (scnt != 5)
		goto l7fee;
	wptr = sptr;
	goto l7fee;
l824e:
	if (ncnt >= 4)
		goto l8270;
	*(long *)pa12 = (long)wptr;
	wptr = dp;
	dp += 8;
	ncnt += 1;
	goto l8286;
l8270:
	*(long *)pa12 = 0;
	wptr = sptr;
	scnt += 5;
l8286:
	pe20 = f8684(width, buf24, *(double *)wptr, c);
	if (*(long *)pa12 == 0)
		wptr += 8;
	else
		wptr = (char *)*(long *)pa12;
	if (!fplus)
		goto l8312;
	if (pe20[0] == 0x2d)
		goto l8312;
	if (pe20[0] == 0)
		goto l8312;
	bufp = pe20;
	b69[0] = bufp[0];
	bufp[0] = sign[0];
	bufp += 1;
	goto l8306;
l82e6:
	b6b[0] = bufp[0];
	bufp[0] = b69[0];
	bufp += 1;
	b69[0] = b6b[0];
l8306:
	if (b69[0] != 0)
		goto l82e6;
	bufp[0] = 0;
l8312:
	if (fhash)
		goto l83c4;
	if (c == 0x65)
		goto l8324;
	if (c != 0x45)
		goto l8370;
l8324:
	bufp = pe20;
	goto l8342;
l832e:
	bufp[0] = bufp[1];
	bufp += 1;
l8366:
	if (bufp[0] != 0)
		goto l832e;
	goto l845e;
l833e:
	bufp += 1;
l8342:
	if (bufp[0] == 0)
		goto l845e;
	if (bufp[0] != 0x2e)
		goto l833e;
	bufp += 1;
	if (bufp[0] != 0x65)
		goto l833e;
	bufp -= 1;
	goto l8366;
l8370:
	if (c == 0x67)
		goto l837c;
	if (c != 0x47)
		goto l845e;
l837c:
	bufp = pe20;
	goto l839a;
l8396:
	bufp += 1;
l839a:
	if (bufp[0] == 0)
		goto l845e;
	if (bufp[0] != 0x2e)
		goto l8396;
	*(char **)pb6e = bufp;
	bufp += 1;
l83b2:
	if (bufp[0] != 0x30)
		goto l83bc;
	bufp += 1;
	goto l83b2;
l83bc:
	if (bufp[0] == 0x65)
		goto l8386;
	goto l845e;
l8386:
	f943c(*(char **)pb6e, bufp);
	goto l845e;
l83c4:
	if (c != 0x66)
		goto l83fc;
	bufp = pe20;
	goto l83e2;
l83d4:
	if (bufp[0] == 0x2e)
		goto l83ea;
	bufp += 1;
l83e2:
	if (bufp[0] != 0)
		goto l83d4;
l83ea:
	if (bufp[0] != 0)
		goto l845e;
	bufp[0] = 0x2e;
	bufp += 1;
	goto l8458;
l83fc:
	if (c == 0x67)
		goto l8408;
	if (c != 0x47)
		goto l845e;
l8408:
	bufp = pe20;
	goto l8420;
l8412:
	if (bufp[0] == 0x2e)
		goto l8428;
	bufp += 1;
l8420:
	if (bufp[0] != 0)
		goto l8412;
l8428:
	if (bufp[0] != 0)
		goto l845e;
	bufp[0] = 0x2e;
	bufp += 1;
	outc = 0;
	goto l844e;
l843e:
	bufp[0] = 0x30;
	bufp += 1;
	outc += 1;
l844e:
	if (outc < width)
		goto l843e;
l8458:
	bufp[0] = 0;
l845e:
	if (c == 0x45)
		goto l846a;
	if (c != 0x47)
		goto l8484;
l846a:
	bufp = pe20;
l8472:
	if (bufp[0] == 0)
		goto l8484;
	if (bufp[0] != 0x65)
		goto l8494;
	bufp[0] = 0x45;
l8494:
	bufp += 1;
	goto l8472;
l8484:
	pn = f8c86(pe20);
	goto l7fee;
l849a:
	goto l7c50;
l84b6:
	padn = 0;
l84ba:
	if (!fleft)
		goto l8504;
	goto l8560;
l84c2:
	--iop->cnt < 0 ? f880e(padc[0], iop) : (int)(*iop->ptr++ = padc[0]);
	nout += 1;
l8504:
	if (padn-- != 0)
		goto l84c2;
	goto l8560;
l8512:
	--iop->cnt < 0 ? f880e(*pe20++, iop) : (int)(*iop->ptr++ = *pe20++);
	nout += 1;
l8560:
	if (pn-- != 0)
		goto l8512;
	if (!fleft)
		goto l7c8c;
l8572:
	if (padn-- == 0)
		goto l7c8c;
	--iop->cnt < 0 ? f880e(padc[0], iop) : (int)(*iop->ptr++ = padc[0]);
	nout += 1;
	goto l8572;
l7d02:
	if (fsp)
		goto l7d50;
	fsp = 1;
	goto l7d48;
l7cf4:
	if (fplus)
		goto l7d50;
	fplus = 1;
	goto l7d48;
l7d10:
	if (fhash)
		goto l7d50;
	fhash = 1;
	goto l7d48;
l7d3c:
	if (fleft)
		goto l7d50;
	fleft = 1;
l7d48:
	rem -= 1;
	if (rem != 0)
		goto l7d1e;
	goto l7d50;
l7d1e:
	c = *f++;
	goto l7d26;
l85c6:
	if (pE90E4 == 0)
		goto l85e6;
	if (!(pE90E4[10] & 0x20))
		goto l85e6;
	return -1;
l85e6:
	return nout;
}

/* ld_fprintf..7ae2: done */
int ld_doprnt();

int ld_fprintf(iop, fmt, w1, w2, d1, d2, d3, d4, s1)
char *iop;
long fmt;
int w1;
int w2;
double d1;
double d2;
double d3;
double d4;
long s1;
{
	int wb0;
	int wb1;
	double db0;
	double db1;
	double db2;
	double db3;

	wb0 = w1;
	wb1 = w2;
	db0 = d4;
	db1 = d3;
	db2 = d2;
	db3 = d1;
	pE90E4 = iop;
	bE90E2 = ld_doprnt(fmt, &wb0, &db0, &s1, 3, iop);
}

/* ld_sprintf..7bbc: done */
int ld_doprnt();
int f880e();

int ld_sprintf(buf, fmt, w1, w2, d1, d2, d3, d4, s1)
char *buf;
long fmt;
int w1;
int w2;
double d1;
double d2;
double d3;
double d4;
long s1;
{
	struct sio io;
	int wb0;
	int wb1;
	double db0;
	double db1;
	double db2;
	double db3;

	io.flag = 0x42;
	io.ptr = buf;
	io.cnt = 0x7fff;
	wb0 = w1;
	wb1 = w2;
	db0 = d4;
	db1 = d3;
	db2 = d2;
	db3 = d1;
	pE90E4 = (char *)&io;
	bE90E2 = ld_doprnt(fmt, &wb0, &db0, &s1, 3, &io);
	--io.cnt < 0 ? f880e(0, &io) : (int)(*io.ptr++ = 0);
	return bE90E2;
}
