/*
 * symbols.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

/* read_entry_hdr..6800: findings:D2 */
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
			swap_pair(&bE46A);
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
			bE15D4 = swap_bytes(bE15D4);
			bE15D6 = swap_bytes(bE15D6);
		}
		f6800(bE15CC, &bE46A, bE4B0);
	}
	bE46A &= 0xff00ffffL;
	bE46E &= ~0x40;
	return result;
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

/* pack_entry..68a2: findings:D4 */
pack_entry(p, q)
char *p;
char *q;
{
	*(int *)(q + 10) = (int)*(long *)p;
	*(int *)(q + 8) = *(char *)(p + 4);
	f0178(p + 8, q);
}

/* f68a2..6a08: findings:D2,D16 */
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
		putchar(9);
	putchar(0x20);
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

/* f6a08 0x6a08..0x6a58: done */
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

/* lookup_name..6b5a: done */
char *lookup_name()
{
	register char *slot;
	register char *cur;
	register char *rec;
	int h;
	int flag;
	char *start;
	char *end;

	h = 0;
	cur = bE472;
	start = cur;
	end = bE47A;
	if (*cur & 0x80) {
		start++;
		if (b04EA == 0)
			end++;
		else
			end = bE472 + (*cur & 0x7f);
	}
	for (cur = start; cur < end; cur++)
		h = h * 2 + *cur;
	slot = tab44E8 + ((h & 0x7fff) % 0x0ffa + 2) * 4;
	while (*(char **)slot) {
		rec = *(char **)slot + 8;
		flag = 0;
		if (b04EA == 0)
			goto l1;
		cur = bE472;
		goto l2;
l1:
		cur = start;
		if (*rec & 0x80)
			rec++;
l2:
		while (cur < end) {
			if (*cur++ != *rec++) {
				flag = 1;
				break;
			}
		}
		if (!flag)
			break;
		slot += 4;
		if (slot >= tab84d8)
			slot = tab44E8;
	}
	return slot;
}

/* f6b5a 0x6b5a..0x6b84: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
char *f6b5a(p)
char *p;
{
	long dummy;

	pack_name(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	lookup_name();
}

/* f6b84 0x6b84..0x6bae: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
long f6b84(p)
char *p;
{
	long dummy;

	unpack_run(p, bE472);
	bE46E = 0x20;
	bE46A = 0;
	lookup_name();
}

/* f6bae..6c6a: findings:D2 */
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

/* f6c6a..6da8: findings:D2,D6,D15 */
f6c6a()
{
	register char *w;
	register char *p;
	int i;

	i = bE3C02 + 0x0470;
	p = get_block(i);
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
	p = get_block(i);
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

/* f6da8..6e1a: findings:D6 */
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

/* f6e1a..6f46: findings:D4 */
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
		swap_pair(&v);
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

/* f6efe 0x6efe..0x6f46: done */
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
		p = next_entry(p);
	}
}

/* f6f46..6fce: findings:D7 */
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

/* table16_entry 0x6fd0..0x6ffe: findings:D1 */
char *table16_entry(x)
unsigned x;
{
	if (x < g04ee)
		return gE47E + (x << 4);
	report_error(2, "x");
}

/* f6ffe 0x6ffe..0x702a: done */
f6ffe(a)
int a;
{
	g1688 = calloc(a, 16);
	if (!g1688)
		report_error(2, "x");
}

/* f702a 0x702a..0x705e: done */
f702a(a)
int a;
{
	gE47E = realloc(gE47E, a << 4);
	if (!gE47E)
		report_error(2, "x");
}

/* next_entry..7112: findings:D2 */
char *next_entry(p)
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

/* index16..7112: findings:D4 */
int index16(x, a)
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

/* ensure_buf..7186: done */
long ensure_buf(n)
unsigned long n;
{
	if (bE15DA < n) {
		if (n > 0xffff)
			report_error(2, "x");
		if (bE15DA != 0)
			free(bE190DE);
		bE190DE = calloc(1, (int)n);
		if (bE190DE == 0)
			report_error(2, "y");
		bE15DA = (unsigned)n;
	}
	return bE190DE;
}

/* limit_msg..719e: done */
limit_msg()
{
	printf("x", bE4FA, 0x0ffa);
}

/* table32_entry 0x719e..0x71ca: done */
char *table32_entry(x)
unsigned x;
{
	if (x < 0x0100)
		return tab1c02 + (x << 5);
	report_error(2, "x");
}
