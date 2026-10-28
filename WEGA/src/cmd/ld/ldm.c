/*
 * ldm.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ldm.c\t3.10  10/27/83 12:21:07 - 87wega3.2";
int bE0F8E = -1;

/* scan_m_opt..410c: findings:D2,D11 */
int scan_m_opt(s, ip, argc)
char *s;
int *ip;
int argc;
{
	char *p;
	char type;
	char digs[4];
	char *fmt;
	int j;
	int k;
	int m;
	int v;
	int w;
	char *q;
	int cnt;

	if (s[1] <= '9' && s[1] >= '0') {
		type = 'a';
		k = 1;
	} else {
		k = 2;
		type = s[1];
		bE49C++;
	}
	if (s[k] == 0)
		report_error(2, "usage error: -%s", s);
	j = k;
	while (s[j] <= '9') {
		if (s[j] < '0')
			break;
		j++;
	}
	if (s[j] != 0 && s[j] != ':')
		report_error(2, "usage error: -%s", s);
	m = 0;
	while (s[k] <= '9') {
		if (s[k] < '0' || m >= 3)
			break;
		digs[m] = s[k];
		k++;
		m++;
	}
	digs[m] = 0;
	v = atoi(digs);
	if (bE4AE && v == 127)
		report_error(2, "segment number in illegal range with %d", v);
	if (v < 0 || v > 127)
		report_error(2, "segment number out of range with %d", v);
	if (v > bE3E82)
		bE3E82 = v;
	if (s[k] == ':') {
		bE42B2++;
		k++;
		switch (type) {
		case 'a':
			w = 0;
			break;
		case 'b':
			w = 3;
			break;
		case 'd':
			w = 2;
			break;
		case 't':
			w = 1;
			break;
		default:
			report_error(2, "-%s: invalid section selector (must be t, d or b)", s);
		}
		if (s[k] == 't' || s[k] == 'b') {
			if (bE42B4[v][w] != 0)
				report_error(2, "-%s: section already specified", s);
			bE42B4[v][w] = s[k];
			k++;
		} else
			report_error(2, "-%s: invalid location selector (must be t or b)", s);
		if (s[k] == 0) {
			s = *bE4A36++;
			(*ip)++;
			k = 0;
		}
		fmt = "%o%c";
		if (s[k] != '0')
			fmt = "%d%c";
		else {
			k++;
			if ((s[k] | 0x20) == 'x') {
				k++;
				fmt = "%x%c";
			}
		}
		if (s[k] != 0) {
			if (sscanf(s + k, fmt, &tab4534[v][w]) != 1)
				report_error(2, "-%s: bad offset", s);
		}
	} else if (s[k] != 0)
		report_error(2, "-%s: usage error", s);
	p = *bE4A36;
	if (p[0] == '+') {
		if (type == 's')
			report_error(2, "invalid -Ms option ");
		bE4A36++;
		p = *bE4A36;
		(*ip)++;
		while (p[0] != '+') {
			if (*ip >= argc)
				break;
			q = strrchr(p, '/');
			if (q) {
				cnt++;
				p = q;
			}
			f410c(type, p, v);
			(*ip)++;
			bE4A36++;
			p = *bE4A36;
		}
		if (*ip == argc)
			report_error(2, "no closing + for M file list");
		(*ip)++;
		bE4A36++;
	} else {
		switch (type) {
		case 't':
			bE3E7E = v;
			break;
		case 'd':
			bE3E7C = v;
			break;
		case 'b':
			bE3E80 = v;
			break;
		case 'a':
			w = bE49A ? v + 1 : v;
			bE3E80 = w;
			bE3E7C = w;
			bE3E7E = v;
			if (bE3E7C > bE3E82)
				bE3E82 = bE3E7C;
			break;
		case 's':
			if (bE4AE)
				report_error(2, "cannot use -Ms option after -z");
			f42f8(v, 's');
			break;
		default:
			report_error(2, "%c illegal type for M; not s, t,d or b", type);
		}
	}
}

/* f410c..42f8: findings:D2,D5 */
int f410c(c, y, x)
char c;
char *y;
int x;
{
	int i;
	int prev;
	int k;
	int r;

	if (g047a == 0x32)
		report_error(2, "-M list overflow, maximum number is %d", 0x32);
	i = bE478;
	prev = 0;
	g0476 = 1;
	k = 0;
	goto cond;
body:
	r = str_compare(y, tab3c22[i].str);
	if (r < 0)
		goto nxt;
	if (r == 0)
		goto attr;
	prev = i;
	i = tab3c22[i].nxt;
nxt:
	k++;
cond:
	if (k < g047a)
		goto body;
	if (i != bE478 || k != 0)
		goto l41de;
	tab3c22[g047a].nxt = bE478;
	bE478 = g047a;
	goto l4218;
l41de:
	tab3c22[g047a].nxt = tab3c22[prev].nxt;
	tab3c22[prev].nxt = g047a;
l4218:
	k = g047a;
	g047a++;
	tab3c22[k].str = y;
	tab3c22[k].c = -1;
	tab3c22[k].b = -1;
	tab3c22[k].a = -1;
attr:
	if (c == 0x61 || c == 0x74)
		tab3c22[k].a = x;
	if (c == 0x61 || c == 0x64)
		tab3c22[k].b = x;
	if (c == 0x61 || c == 0x62)
		tab3c22[k].c = x;
	g0476 = 0;
}

/* f42f8..4444: findings:D6,D15,D16 */
int f42f8(x)
long x;
{
	if (((int)x) < 0 || ((int)x) >= 0x80)
		report_error(2, "reserved segment is out of range with %d", ((int)x));
	if (((int)x) > bE3E82)
		bE3E82 = ((int)x);
	switch (((char *)&x)[1]) {
	case 'M':
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		}
		*(int *)(tabF338 + ((int)x) * 8) = 0x01f4;
		break;
	case 'r':
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		}
		*(int *)(tabF338 + ((int)x) * 8) = 0x03e8;
		break;
	case 's':
		if (g0474 > -1)
			*(int *)(tabF338 + g0474 * 8) = -1;
		if (*(int *)(tabF338 + ((int)x) * 8) == 0x03e8) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		} else {
			*(int *)(tabF338 + ((int)x) * 8) = 0x07d0;
			g0474 = ((int)x);
		}
		break;
	}
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

/* end_pass1..4968: findings:D2 */
int end_pass1()
{
	register char *blk;
	register char *a;
	register char *ep;
	long u0;
	unsigned long t4;
	unsigned long t8;
	char *end;
	int flg;
	char *blk2;
	int i;

	flg = 0;
	bE482 = *(long *)f6b5a("_etext");
	bE486 = *(long *)f6b5a("_edata");
	bE48A = *(long *)f6b5a("_end");
	bE48E = *(long *)f6b5a("_stkseg");
	if (bE48E == 0) {
		if (g0474 > -1)
			report_error(1, "warning: _stkseg undefined");
		else
			g0474 = -2;
	} else {
		if (!*(int *)bE49E && !bE49A && g0474 == -1 && bE49C < 2 && !bE4AE)
			g0474 = -3;
		else if (bE4A2)
			g0474 = -3;
	}
	end = table16_entry(bE4FA);
	if (!bE4A2) {
		ep = table16_entry(0);
		while (ep < end) {
			if (ep[4] == ' ' && *(long *)ep == 0 && (long)ep != bE48A && (long)ep != bE486 && (long)ep != bE482 && (long)ep != bE48E) {
				bE4A2++;
				bE498 = 0;
				break;
			}
			ep = next_entry(ep);
		}
	}
	if (bE4A2) {
		bE4A0 = 0;
		bE4A4 = 0;
		bE42B2 = 0;
		for (i = 0; i < 128; i++) {
			for (t8 = 0; t8 < 4; t8++)
				bE42B4[i][(int)t8] = 0;
		}
	}
	bE4C8 = 0;
	if (bE498 == 0 && bE4A2)
		goto l469e;
	ep = table16_entry(0);
	while (ep < end) {
		if (ep[4] == ' ') {
			t8 = *(long *)ep & 0xffff;
			if (t8 != 0) {
				t8 = (t8 + 1) & 0xfffe;
				t4 = bE4C8 + t8;
				if (t4 > 0x10000L || flg == 0) {
					if (bEF32C >= bE4EC)
						report_error(2, "pass one segment table overflow -- Try -Ap option");
					bE4C8 = 0;
					t4 = t8;
					blk2 = get_block(bEF32C++);
					*(int *)(blk2 + 0x34) = -1;
					*(int *)(blk2 + 4) = 0;
					*(int *)(blk2 + 6) = 0;
					*(int *)(blk2 + 0x26) = bE3E80;
					flg++;
				}
				*(int *)(ep + 6) = bEF32C - 1;
				*(long *)ep = bE4C8;
				ep[4] = '%';
				*(int *)(get_block(bEF32C - 1) + 8) = (int)(bE4C8 = t4);
			}
		}
		ep = next_entry(ep);
	}
l469e:
	lay_out();
	if (bE498 || !bE4A2) {
		f68a2(bE482, 0x22);
		f68a2(bE486, 0x23);
		f68a2(bE48A, 0x24);
		if (!bE4A2)
			f68a2(bE48E, 0x21);
	}
	g04e6 = 0;
	ep = table16_entry(0);
	while (ep < end) {
		if ((long)ep != bE482 && (long)ep != bE486 && (long)ep != bE48A && (long)ep != bE48E) {
			switch (ep[4] - 0x20) {
			case 0:
				if (bE496)
					break;
				bE4A3A |= 1;
				if (*(long *)ep)
					break;
				if (g04e6 == 0)
					printf("Undefined:\n");
				g04e6++;
				put_name(ep + 8);
				putchar(10);
				break;
			case 2:
				a = *(char **)((blk = get_block(*(int *)(ep + 6))) + 0x16);
				*(long *)ep = (unsigned)(*(long *)ep) + *(int *)(blk + 0x22) + *(int *)(a + 0x1a) + ((long)a[0] << 24);
				*(int *)(ep + 6) = *(int *)(a + 0x10);
				break;
			case 3:
				a = *(char **)((blk = get_block(*(int *)(ep + 6))) + 0x1a);
				*(long *)ep = (unsigned)(*(long *)ep) + *(int *)(blk + 0x24) + *(int *)(a + 0x1c) + ((long)a[0] << 24);
				*(int *)(ep + 6) = *(int *)(a + 0x10);
				break;
			case 5:
				ep[4] = '$';
			case 4:
				a = *(char **)((blk = get_block(*(int *)(ep + 6))) + 0x1e);
				*(long *)ep = (unsigned)(*(long *)ep) + *(int *)(blk + 0x26) + *(int *)(a + 0x1e) + ((long)a[0] << 24);
				*(int *)(ep + 6) = *(int *)(a + 0x10);
				break;
			}
		}
		ep = next_entry(ep);
	}
	if (bE4A4 || bE4AA)
		bE4CE = 0;
	bEF312 = (unsigned)bE4CE >> 4;
}

/* f4968..4a16: findings:D5 */
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
		report_error(2, "pass2 segment table overflow");
	if (*(int *)(tabF338 + x * 8) == 0x0bb8)
		*(int *)(q + 10) |= 8;
	else
		*(int *)q = 4;
	lcl = 1;
	*res = alloc_segment(q, p, 0, 0x72, &lcl);
}

/* f4a16..5a2c: findings:D2,D15 */
int lay_out()
{
	register struct ent *blk;
	register struct ent *cur;
	register struct seg *p;
	int max;
	int u0;
	int u1;
	int u2;
	int u3;
	struct seg *l1;
	struct seg *l2;
	int cnt;
	int k;
	int i;
	int delta;
	int j;
	int q;
	int u4;
	struct seg *e;
	int swapped;

	pE3C0E = 0;
	pE3C0A = 0;
	pE3C06 = 0;
	b3C12 = 0;
	bF32E = 0;
	bE3C04 = 0;
	for (k = 1; k < 4; k++) {
		for (i = 0; i < bEF32C; i++) {
			blk = (struct ent *)get_block(i);
			if (blk->f4 && k == 1) {
				blk->s2 = ins_block(2, blk->f4, tblF332[blk->f22].a, i, blk->f22);
				blk->f22 = blk->s2->f4;
				blk->s2->f4 += blk->f4;
				bE3C16 += (unsigned)blk->f4;
				blk->s2->flags |= 1;
			}
			if (blk->f6 && k == 2) {
				if (tblF332[blk->f24].c > -1)
					blk->s3 = ins_block(3, blk->f6, tblF332[blk->f24].c - 0x4b0, i, blk->f24);
				else
					blk->s3 = ins_block(3, blk->f6, tblF332[blk->f24].b, i, blk->f24);
				blk->f24 = blk->s3->f6;
				blk->s3->f6 += blk->f6;
				bE3C1A += (unsigned)blk->f6;
				blk->s3->flags |= 2;
			}
			if (blk->f8 && k == 3) {
				if (tblF332[blk->f26].c > 0x4af)
					blk->s4 = ins_block(4, blk->f8, tblF332[blk->f26].c - 0x4b0, i, blk->f26);
				else
					blk->s4 = ins_block(4, blk->f8, tblF332[blk->f26].c, i, blk->f26);
				blk->f26 = blk->s4->f8;
				blk->s4->f8 += blk->f8;
				b3C1E += (unsigned)blk->f8;
				blk->s4->flags |= 4;
			}
		}
	}
	if (g0474 >= 0 && g0474 < 128) {
		l1 = (struct seg *)0x7f00ffff;
		l2 = (struct seg *)0x7f00ffff;
		i = tblF332[g0474].a;
		if (i != -1)
			l1 = ((struct ent *)get_block(i))->s2;
		else {
			i = tblF332[g0474].b;
			if (i != -1)
				l1 = ((struct ent *)get_block(i))->s3;
			else {
				i = tblF332[g0474].c;
				if (i != -1)
					l1 = ((struct ent *)get_block(i))->s4;
			}
		}
		if (l2 == l1)
			tblF332[g0474].d = 3000;
	}
	bF32E = 0;
	max = bE3E82;
	cnt = 0;
	k = 0;
	while (k <= max) {
		if (tblF332[k].d > 500 && tblF332[k].d != 2000) {
			f4968(l1, &cnt, k, &delta);
			cnt--;
			if (!(tblF332[k].a == -1 && tblF332[k].b == -1 && tblF332[k].c == -1)) {
				for (j = 0; j < 128; j++) {
					if (tblF332[j].a == -1 && tblF332[j].b == -1 && tblF332[j].c == -1 && tblF332[j].d == -1) {
						report_error(0, "warning: image in seg %d has been moved to seg %d because of -R; origins are reset", k, j);
						tblF332[j].a = tblF332[k].a;
						tblF332[j].b = tblF332[k].b;
						tblF332[j].c = tblF332[k].c;
						tblF332[j].d = 500;
						tblF332[k].a = -1;
						tblF332[k].b = -1;
						tblF332[k].c = -1;
						if (j > bE3E82)
							bE3E82 = j;
						break;
					}
				}
			}
		}
		k++;
		cnt++;
	}
	max = bE3E82;
	k = 0;
	cnt = 0;
	for (; k <= max; k++) {
		if (tblF332[k].d == 100 || tblF332[k].d == 50 || (tblF332[k].d == -1 && tblF332[k].a < 0 && tblF332[k].b < 0 && (tblF332[k].c < 0 || tblF332[k].c > 0x4af))) {
			cnt++;
			continue;
		}
		if (tblF332[k].d == 1000 || tblF332[k].d == 3000) {
			cnt++;
			continue;
		}
		i = tblF332[k].a;
		if (i != -1)
			l1 = ((struct ent *)get_block(i))->s2;
		else {
			i = tblF332[k].b;
			if (i != -1)
				l1 = ((struct ent *)get_block(i))->s3;
			else {
				i = tblF332[k].c;
				if (i == -1)
					continue;
				l1 = ((struct ent *)get_block(i))->s4;
			}
		}
		bF320 = 0;
		bF322 = 0;
		place_sec(l1, cnt);
		if (tblF332[k].a > -1) {
			pE3C0A = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 't', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 1 && (l1->f1a & 0xff)) || bE42B4[cnt - 1][1] == 't')
					delta = 0x100 - (l1->f4 & 0xff);
				else if (f4a34 == 1)
					delta = 0x100 - ((l1->f1a - (l1->f1a & 0xff00) + l1->f4) & 0xff);
				else
					delta = 0;
				if (f4a34 == 1 && (unsigned)l1->f4 > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			bE3C16 += delta;
			l1->f4 += delta;
			if (l1->c[0] == g0474)
				l1->flags |= 8;
			while ((i = (cur = (struct ent *)get_block(i))->n2) > -1) {
				if (l1 != ((struct ent *)get_block(i))->s2) {
					if (bE42B4[k][1])
						report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[1]);
					l1 = ((struct ent *)get_block(i))->s2;
					l1->f1a = 0;
					pE3C0A = (char *)l1;
					place_sec(l1, cnt);
					q = i;
					delta = alloc_segment(l1, &cnt, 1, 't', &q);
					if (l1->f8 + l1->f6) {
						tblF332[q].c = tblF332[k].c;
						tblF332[q].b = tblF332[k].b;
						tblF332[k].c = -1;
						tblF332[k].b = -1;
					}
					cur->n2 = -1;
					if (f4a34 == 1) {
						bE3C16 += delta;
						l1->f4 += delta;
					} else if (f4a34 == 2) {
						bE3C1A += delta;
						l1->f6 += delta;
					} else if (f4a34 == 3) {
						b3C1E += delta;
						l1->f8 += delta;
					}
				}
			}
		}
		if (tblF332[k].b > -1) {
			i = tblF332[k].b;
			blk = (struct ent *)get_block(i);
			l2 = blk->s3;
			if (bF320) {
				l1 = bF324;
				blk->s3 = l1;
				cnt--;
			} else
				l1 = l2;
			pE3C06 = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'd', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 2 && (l1->f1c & 0xff) == 0) || bE42B4[cnt - 1][2] == 't')
					delta = 0x100 - (l1->f6 & 0xff);
				else if (f4a34 == 2)
					delta = 0x100 - ((l1->f1c - (l1->f1c & 0xff00) + l1->f6) & 0xff);
				else
					delta = 0;
				if (f4a34 == 2 && (unsigned)(l1->f4 + l1->f6) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			bE3C1A += delta;
			l1->f6 += delta;
			if (l1->c[0] == g0474)
				l1->flags |= 8;
			goto ltd;
lhd:
			if (bE42B4[k][2])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[2]);
			l1 = ((struct ent *)get_block(i))->s3;
			l1->f1c = 0;
			place_sec(l1, cnt);
			pE3C06 = (char *)l1;
			q = i;
			delta = alloc_segment(l1, &cnt, 1, 'd', &q);
			cur->n3 = -1;
			if (f4a34 == 2) {
				bE3C1A += delta;
				l1->f6 += delta;
			} else {
				tblF332[q].c = tblF332[k].c;
				tblF332[k].c = -1;
				b3C1E += delta;
				l1->f8 += delta;
			}
			l2 = l1;
			goto ltd;
lbd:
			blk = (struct ent *)get_block(i);
			if (l2 != blk->s3)
				goto lhd;
			blk->s3 = l1;
ltd:
			i = (cur = (struct ent *)get_block(i))->n3;
			if (i > -1)
				goto lbd;
		}
		if (tblF332[k].c > -1 && tblF332[k].c < 0x4b0) {
			i = tblF332[k].c;
			blk = (struct ent *)get_block(i);
			l2 = blk->s4;
			if (bF322) {
				l1 = bF328;
				blk->s4 = l1;
				cnt--;
			} else
				l1 = l2;
			pE3C0E = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'b', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 3 && (l1->f1e & 0xff) == 0) || bE42B4[cnt - 1][3] == 't')
					delta = 0x100 - (l1->f8 & 0xff);
				else if (f4a34 == 3)
					delta = 0x100 - ((l1->f1e - (l1->f1e & 0xff00) + l1->f8) & 0xff);
				else
					delta = 0;
				if (f4a34 == 3 && (unsigned)(l1->f4 + l1->f6 + l1->f8) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			b3C1E += delta;
			l1->f8 += delta;
			if (l1->c[0] == g0474)
				l1->flags |= 8;
			goto ltb;
lhb:
			if (bE42B4[k][3])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[3]);
			l1 = ((struct ent *)get_block(i))->s4;
			l1->f1e = l1->f1c + l1->f6;
			pE3C0E = (char *)l1;
			if (*(char *)l1 == -1) {
				q = i;
				delta = alloc_segment(l1, &cnt, 1, 'b', &q);
				cur->n4 = -1;
				b3C1E += delta;
				l1->f8 += delta;
			}
			l2 = l1;
			goto ltb;
lbb:
			blk = (struct ent *)get_block(i);
			if (l2 != blk->s4)
				goto lhb;
			blk->s4 = l1;
ltb:
			i = (cur = (struct ent *)get_block(i))->n4;
			if (i > -1)
				goto lbb;
		}
		if (tblF332[k].c > 0x4af)
			tblF332[k].c = -1;
	}
	if (g0474 == -1) {
		l1 = (struct seg *)table32_entry(bE3C04);
		if (bE3C04++ > 0x100)
			report_error(2, "pass2 segment table overflow");
		bE3E82++;
		l1->flags = 8;
		q = 1;
		delta = alloc_segment(l1, &cnt, 0, 's', &q);
		g0474 = cnt - 1;
	}
	if (b3C12 != bF32E) {
		do {
			swapped = 0;
			p = (struct seg *)bF32E;
			e = p->nxt;
			do {
				if (e->c[0] < p->c[0]) {
					swapped = 1;
					list_move_after(p, e);
				}
				e = (p = e)->nxt;
			} while (p != (struct seg *)b3C12);
		} while (swapped);
	}
}

/* ins_block..5eec: findings:D11 */
struct seg *ins_block(kind, size, blk, val, idx)
int kind;
register unsigned size;
int blk;
int val;
int idx;
{
	register struct seg *s;
	register struct ent *b;
	int prev;
	int tmp;

	if (blk > -1) {
		while (blk != -1) {
			prev = blk;
			switch (kind) {
			case 2:
				s = (b = (struct ent *)get_block(blk))->s2;
				blk = b->n2;
				break;
			case 3:
				if (tblF332[idx].c > 0x4af) {
					s = ((struct ent *)get_block(blk))->s2;
					blk = -1;
					tblF332[idx].c = -1;
				} else {
					s = (b = (struct ent *)get_block(blk))->s3;
					blk = b->n3;
				}
				break;
			case 4:
				if (tblF332[idx].c > 0x4af) {
					s = ((struct ent *)get_block(blk))->s2;
					blk = -1;
					tblF332[idx].c = -1;
				} else {
					s = (b = (struct ent *)get_block(blk))->s4;
					blk = b->n4;
				}
				break;
			}
			if (size > 0xfffe - (s->f4 + s->f6 + s->f8))
				continue;
			switch (kind) {
			case 2:
				blk = prev;
				while ((b = (struct ent *)get_block(blk))->s2 == s) {
					if (b->n2 == -1)
						break;
					prev = blk;
					blk = ((struct ent *)get_block(blk))->n2;
				}
				if (((struct ent *)get_block(blk))->s2 == s)
					prev = blk;
				tmp = (b = (struct ent *)get_block(prev))->n2;
				b->n2 = val;
				((struct ent *)get_block(val))->n2 = tmp;
				return s;
			case 3:
				if (tblF332[idx].b == -1) {
					tblF332[idx].b = val;
					return s;
				}
				blk = prev;
				while ((b = (struct ent *)get_block(blk))->s3 == s) {
					if (b->n3 == -1)
						break;
					prev = blk;
					blk = ((struct ent *)get_block(blk))->n3;
				}
				if (((struct ent *)get_block(blk))->s3 == s)
					prev = blk;
				tmp = (b = (struct ent *)get_block(prev))->n3;
				b->n3 = val;
				((struct ent *)get_block(val))->n3 = tmp;
				return s;
			case 4:
				if (tblF332[idx].c == -1) {
					tblF332[idx].c = val;
					return s;
				}
				tmp = (b = (struct ent *)get_block(prev))->n4;
				b->n4 = val;
				((struct ent *)get_block(val))->n4 = tmp;
				return s;
			}
			return s;
		}
	}
	if (kind == 4) {
		blk = tblF332[idx].b;
		while (blk > -1) {
			s = (b = (struct ent *)get_block(blk))->s3;
			blk = b->n3;
			if (size <= 0xfffe - (s->f4 + s->f6 + s->f8))
				goto l5e82;
		}
	}
	if (bE3C04 < 0x100) {
		bE0F8E++;
		switch (kind) {
		case 2:
			tblF332[idx].c = val + 0x4b0;
			if (tblF332[idx].a < 0)
				tblF332[idx].a = val;
			else
				((struct ent *)get_block(prev))->n2 = val;
			break;
		case 3:
			if (tblF332[idx].b < 0)
				tblF332[idx].b = val;
			else
				((struct ent *)get_block(prev))->n3 = val;
			break;
		case 4:
			if (tblF332[idx].c < 0 || tblF332[idx].c > 0x4af)
				tblF332[idx].c = val;
			else
				((struct ent *)get_block(prev))->n4 = val;
			break;
		}
		if (bF32E == 0) {
			b3C12 = (long)tab1c02;
			bF32E = (long)tab1c02;
			bE3C04++;
			*(char *)bF32E = 0xff;
			return (struct seg *)bF32E;
		}
		s = (struct seg *)table32_entry(bE3C04++);
		*(char *)s = 0xff;
		return s;
	} else
		report_error(2, "pass two segment table overflow");
l5e82:
	if (tblF332[idx].c == -1) {
		tblF332[idx].c = val;
		return s;
	}
	tmp = (b = (struct ent *)get_block(prev))->n4;
	b->n4 = val;
	((struct ent *)get_block(val))->n4 = tmp;
	return s;
}

/* alloc_segment..60f0: done */
int alloc_segment(rec, pidx, a, b, q)
register int *pidx;
register char *rec;
int a;
char b;
int *q;
{
	register int sum;
	register int i;

	if (a) {
		for (i = 0; i < 128; i++) {
			if (tblF332[i].a == -1 && tblF332[i].b == -1 && tblF332[i].c == -1 && tblF332[i].d == -1) {
				*(int *)(rec + 16) = i;
				rec[0] = i;
				switch (b) {
				case 't':
					tblF332[i].a = *q;
					break;
				case 'd':
					tblF332[i].b = *q;
					break;
				case 'b':
					tblF332[i].c = *q;
					break;
				}
				if (i > bE3E82)
					bE3E82 = i;
				tblF332[i].d = 100;
				*q = i;
				break;
			}
		}
		if (i == 128)
			report_error(2, "No more segments");
	} else {
		if (b != 'r') {
			while (tblF332[*pidx].d == 1000 || tblF332[*pidx].d == 100 || tblF332[*pidx].d == 3000)
				(*pidx)++;
			if (tblF332[*pidx].d == -1)
				tblF332[*pidx].d = 50;
		}
		*(int *)(rec + 16) = *pidx;
		rec[0] = *pidx;
		*pidx = *pidx + 1;
	}
	if (bF32E == 0) {
		b3C12 = (long)rec;
		bF32E = (long)rec;
	} else {
		*(char **)((char *)b3C12 + 0x12) = rec;
		*(char **)(rec + 0x16) = (char *)b3C12;
		b3C12 = (long)rec;
	}
	sum = *(int *)(rec + 4) + *(int *)(rec + 6) + *(int *)(rec + 8);
	if ((unsigned)sum > 0xff00)
		sum += 2;
	sum = sum & 0xff;
	if (bE4A2 == 0 && sum != 0)
		return 0x100 - sum;
	return 0;
}

/* f60f0 0x60f0..0x6134: findings:D2 */
f60f0(q, a)
char *q;
long a;
{
	int v;

	if ((char)a == 0x74)
		v = *(int *)(q + 4) - bE3E7A;
	else
		v = *(int *)(q + 6) - bE3E7A;
	if (v)
		f64d0(v);
}

/* f6134..620e: done */
f6134()
{
	register int i;

	flush_buffer(bE6E2);
	flush_buffer(bE8EA);
	if (bE4A2) {
		flush_buffer(bEF02);
		flush_buffer(bEAF2);
	}
	for (i = 0; i <= bE3E82; i++) {
		bE3E7A = 0;
		if (tblF332[i].a > -1)
			copy_blocks(tblF332[i].a, 0x74, *(char **)(get_block(tblF332[i].a) + 22));
		if (tblF332[i].b > -1)
			copy_blocks(tblF332[i].b, 0x64, *(char **)(get_block(tblF332[i].b) + 26));
	}
}

/* copy_blocks..642e: done */
int copy_blocks(idx, type, p)
register int idx;
char type;
char *p;
{
	register int m;
	register int n;
	register long q;
	char *cur;
	int k;
	long w;
	char buf[512];
	int j;
	char *rec;

	cur = p;
head:
	if (idx == -1)
		goto done;
	if (p != cur)
		goto done;
	if (type == 0x74) {
		rec = get_block(idx);
		n = *(int *)(rec + 4);
		q = *(long *)(rec + 0x28);
		idx = *(int *)(rec + 0x30);
		cur = *(char **)(get_block(idx) + 0x16);
		lseek(*(int *)bE6E2, q, 0);
	} else {
		rec = get_block(idx);
		n = *(int *)(rec + 6);
		q = *(long *)(rec + 0x2c);
		idx = *(int *)(rec + 0x32);
		cur = *(char **)(get_block(idx) + 0x1a);
		lseek(*(int *)bE8EA, q, 0);
	}
	if (bE4A2) {
		lseek(*(int *)bEF02, q, 0);
		lseek(*(int *)bEAF2, q, 0);
	}
	bE3E7A += n;
	while (n) {
		if ((unsigned)n > 0x200) {
			n = n - 0x200;
			m = 0x200;
		} else {
			m = n;
			n = 0;
		}
		if (type == 0x74) {
			if (read(*(int *)bE6E2, buf, m) < 1)
				report_error(2, "i/o error reading from tmpoutb");
		} else {
			if (read(*(int *)bE8EA, buf, m) < 1)
				report_error(2, "i/o error reading from doutb");
		}
		f2f04(bE4DA, buf, m);
		if (bE4A2) {
			if (type == 0x74) {
				if (m != read(*(int *)bEF02, buf, m))
					report_error(2, "eof error on trout.b");
			} else {
				if (m != read(*(int *)bEAF2, buf, m))
					report_error(2, "eof error on droutb");
			}
			f2f04(bECFA, buf, m);
		}
	}
	goto head;
done:
	f60f0(p, type);
	if (idx != -1) {
		p = cur;
		bE3E7A = 0;
		goto head;
	}
	bE3E7A = 0;
	return 0;
}

/* list_move_after..64d0: findings:D4 */
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

/* f64d0..6522: findings:D2 */
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
