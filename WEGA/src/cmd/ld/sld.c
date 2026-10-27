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
	f79ca("x", (int)a);
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

long f6b84(p)
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
int f79ca();

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
		f79ca("y");
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
int f79ca();

walk_entries()
{
	register char *a;
	register char *b;

	f79ca("x");
	b = table16_entry(bE4FA);
	a = table16_entry(0);
	while (a < b) {
		f0220(a);
		a = f705e(a);
	}
}

/* report_limit..719e: done */
int f79ca();

report_limit()
{
	f79ca("x", bE4FA, 0x0ffa);
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
int f79ca();
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
		f79ca("x", pE4D6);
	if (pE4C0)
		f79ca("x", pE4C0);
	if (bE438[0])
		f79ca("x", bE438);
	f79ca("x");
	f79ca(msg, b, a, c, d, e);
	f79ca("x");
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
