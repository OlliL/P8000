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
			mlist_add(type, p, v);
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
			resv_seg(v, 's');
			break;
		default:
			report_error(2, "%c illegal type for M; not s, t,d or b", type);
		}
	}
}

/* mlist_add..42f8: findings:D2,D5 */
int mlist_add(c, y, x)
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
	r = str_compare(y, tab3c22[i].mname);
	if (r < 0)
		goto nxt;
	if (r == 0)
		goto attr;
	prev = i;
	i = tab3c22[i].mnext;
nxt:
	k++;
cond:
	if (k < g047a)
		goto body;
	if (i != bE478 || k != 0)
		goto l41de;
	tab3c22[g047a].mnext = bE478;
	bE478 = g047a;
	goto l4218;
l41de:
	tab3c22[g047a].mnext = tab3c22[prev].mnext;
	tab3c22[prev].mnext = g047a;
l4218:
	k = g047a;
	g047a++;
	tab3c22[k].mname = y;
	tab3c22[k].mbss = -1;
	tab3c22[k].mdata = -1;
	tab3c22[k].mtext = -1;
attr:
	if (c == 0x61 || c == 0x74)
		tab3c22[k].mtext = x;
	if (c == 0x61 || c == 0x64)
		tab3c22[k].mdata = x;
	if (c == 0x61 || c == 0x62)
		tab3c22[k].mbss = x;
	g0476 = 0;
}

/* resv_seg 0x42f8..0x4444: done */
int resv_seg(x, c)
int x;
char c;
{
	if (x < 0 || x >= 0x80)
		report_error(2, "reserved segment is out of range with %d", x);
	if (x > bE3E82)
		bE3E82 = x;
	switch (c) {
	case 'r':
		if (tblF332[x].res == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		}
		tblF332[x].res = 0x03e8;
		break;
	case 's':
		if (g0474 > -1)
			tblF332[g0474].res = -1;
		if (tblF332[x].res == 0x03e8) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		} else {
			tblF332[x].res = 0x07d0;
			g0474 = x;
		}
		break;
	case 'M':
		if (tblF332[x].res == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			g0474 = -1;
		}
		tblF332[x].res = 0x01f4;
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

/* end_pass1 0x44e6..0x4968: done */
int end_pass1()
{
	register struct p1seg *blk;
	register struct p2seg *a;
	register struct symbol *ep;
	long u0;
	unsigned long t4;
	unsigned long t8;
	struct symbol *end;
	int flg;
	struct p1seg *blk2;
	int i;

	flg = 0;
	bE482 = *slookup("_etext");
	bE486 = *slookup("_edata");
	bE48A = *slookup("_end");
	bE48E = *slookup("_stkseg");
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
			if (ep->stype == ' ' && ep->svalue == 0 && ep != bE48A && ep != bE486 && ep != bE482 && ep != bE48E) {
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
		if (ep->stype == ' ') {
			t8 = ep->svalue & 0xffff;
			if (t8 != 0) {
				t8 = (t8 + 1) & 0xfffe;
				t4 = bE4C8 + t8;
				if (t4 > 0x10000L || flg == 0) {
					if (bEF32C >= bE4EC)
						report_error(2, "pass one segment table overflow -- Try -Ap option");
					bE4C8 = 0;
					t4 = t8;
					blk2 = get_block(bEF32C++);
					blk2->bssnext = -1;
					blk2->s_code = 0;
					blk2->s_data = 0;
					blk2->nboff = bE3E80;
					flg++;
				}
				ep->sseg = bEF32C - 1;
				ep->svalue = bE4C8;
				ep->stype = '%';
				get_block(bEF32C - 1)->s_bss = (int)(bE4C8 = t4);
			}
		}
		ep = next_entry(ep);
	}
l469e:
	lay_out();
	if (bE498 || !bE4A2) {
		ldrsym(bE482, 0x22);
		ldrsym(bE486, 0x23);
		ldrsym(bE48A, 0x24);
		if (!bE4A2)
			ldrsym(bE48E, 0x21);
	}
	g04e6 = 0;
	ep = table16_entry(0);
	while (ep < end) {
		if (ep != bE482 && ep != bE486 && ep != bE48A && ep != bE48E) {
			switch (ep->stype) {
			case 0x20:
				if (bE496)
					break;
				bE4A3A |= 1;
				if (ep->svalue)
					break;
				if (g04e6 == 0)
					printf("Undefined:\n");
				g04e6++;
				put_name(ep->sname);
				putchar(10);
				break;
			case 0x22:
				a = (blk = get_block(ep->sseg))->ncode;
				ep->svalue = (unsigned)ep->svalue + blk->ncoff + a->torig + ((long)a->segno << 24);
				ep->sseg = a->segidx;
				break;
			case 0x23:
				a = (blk = get_block(ep->sseg))->ndata;
				ep->svalue = (unsigned)ep->svalue + blk->ndoff + a->dorig + ((long)a->segno << 24);
				ep->sseg = a->segidx;
				break;
			case 0x25:
				ep->stype = '$';
			case 0x24:
				a = (blk = get_block(ep->sseg))->nbss;
				ep->svalue = (unsigned)ep->svalue + blk->nboff + a->borig + ((long)a->segno << 24);
				ep->sseg = a->segidx;
				break;
			}
		}
		ep = next_entry(ep);
	}
	if (bE4A4 || bE4AA)
		bE4CE = 0;
	bEF312 = (unsigned)bE4CE >> 4;
}

/* alloc_resv 0x4968..0x4a16: done */
int alloc_resv(q, p, x, res)
struct p2seg *q;
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
	if (tblF332[x].res == 0x0bb8)
		q->atr |= 8;
	else
		q->atr = 4;
	lcl = 1;
	*res = alloc_segment(q, p, 0, 0x72, &lcl);
}

/* f4a16..5a2c: findings:D2,D15 */
int lay_out()
{
	register struct p1seg *blk;
	register struct p1seg *cur;
	register struct p2seg *p;
	int max;
	int u0;
	int u1;
	int u2;
	int u3;
	struct p2seg *l1;
	struct p2seg *l2;
	int cnt;
	int k;
	int i;
	int delta;
	int j;
	int q;
	int u4;
	struct p2seg *e;
	int swapped;

	pE3C0E = 0;
	pE3C0A = 0;
	pE3C06 = 0;
	b3C12 = 0;
	bF32E = 0;
	bE3C04 = 0;
	for (k = 1; k < 4; k++) {
		for (i = 0; i < bEF32C; i++) {
			blk = get_block(i);
			if (blk->s_code && k == 1) {
				blk->ncode = ins_block(2, blk->s_code, tblF332[blk->ncoff].txtptr, i, blk->ncoff);
				blk->ncoff = blk->ncode->s_code;
				blk->ncode->s_code += blk->s_code;
				bE3C16 += (unsigned)blk->s_code;
				blk->ncode->atr |= 1;
			}
			if (blk->s_data && k == 2) {
				if (tblF332[blk->ndoff].bssptr > -1)
					blk->ndata = ins_block(3, blk->s_data, tblF332[blk->ndoff].bssptr - 0x4b0, i, blk->ndoff);
				else
					blk->ndata = ins_block(3, blk->s_data, tblF332[blk->ndoff].datptr, i, blk->ndoff);
				blk->ndoff = blk->ndata->s_data;
				blk->ndata->s_data += blk->s_data;
				bE3C1A += (unsigned)blk->s_data;
				blk->ndata->atr |= 2;
			}
			if (blk->s_bss && k == 3) {
				if (tblF332[blk->nboff].bssptr > 0x4af)
					blk->nbss = ins_block(4, blk->s_bss, tblF332[blk->nboff].bssptr - 0x4b0, i, blk->nboff);
				else
					blk->nbss = ins_block(4, blk->s_bss, tblF332[blk->nboff].bssptr, i, blk->nboff);
				blk->nboff = blk->nbss->s_bss;
				blk->nbss->s_bss += blk->s_bss;
				b3C1E += (unsigned)blk->s_bss;
				blk->nbss->atr |= 4;
			}
		}
	}
	if (g0474 >= 0 && g0474 < 128) {
		l1 = (struct p2seg *)0x7f00ffff;
		l2 = (struct p2seg *)0x7f00ffff;
		i = tblF332[g0474].txtptr;
		if (i != -1)
			l1 = (get_block(i))->ncode;
		else {
			i = tblF332[g0474].datptr;
			if (i != -1)
				l1 = (get_block(i))->ndata;
			else {
				i = tblF332[g0474].bssptr;
				if (i != -1)
					l1 = (get_block(i))->nbss;
			}
		}
		if (l2 == l1)
			tblF332[g0474].res = 3000;
	}
	bF32E = 0;
	max = bE3E82;
	cnt = 0;
	k = 0;
	while (k <= max) {
		if (tblF332[k].res > 500 && tblF332[k].res != 2000) {
			alloc_resv(l1, &cnt, k, &delta);
			cnt--;
			if (!(tblF332[k].txtptr == -1 && tblF332[k].datptr == -1 && tblF332[k].bssptr == -1)) {
				for (j = 0; j < 128; j++) {
					if (tblF332[j].txtptr == -1 && tblF332[j].datptr == -1 && tblF332[j].bssptr == -1 && tblF332[j].res == -1) {
						report_error(0, "warning: image in seg %d has been moved to seg %d because of -R; origins are reset", k, j);
						tblF332[j].txtptr = tblF332[k].txtptr;
						tblF332[j].datptr = tblF332[k].datptr;
						tblF332[j].bssptr = tblF332[k].bssptr;
						tblF332[j].res = 500;
						tblF332[k].txtptr = -1;
						tblF332[k].datptr = -1;
						tblF332[k].bssptr = -1;
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
		if (tblF332[k].res == 100 || tblF332[k].res == 50 || (tblF332[k].res == -1 && tblF332[k].txtptr < 0 && tblF332[k].datptr < 0 && (tblF332[k].bssptr < 0 || tblF332[k].bssptr > 0x4af))) {
			cnt++;
			continue;
		}
		if (tblF332[k].res == 1000 || tblF332[k].res == 3000) {
			cnt++;
			continue;
		}
		i = tblF332[k].txtptr;
		if (i != -1)
			l1 = (get_block(i))->ncode;
		else {
			i = tblF332[k].datptr;
			if (i != -1)
				l1 = (get_block(i))->ndata;
			else {
				i = tblF332[k].bssptr;
				if (i == -1)
					continue;
				l1 = (get_block(i))->nbss;
			}
		}
		bF320 = 0;
		bF322 = 0;
		place_sec(l1, cnt);
		if (tblF332[k].txtptr > -1) {
			pE3C0A = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 't', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 1 && (l1->torig & 0xff)) || bE42B4[cnt - 1][1] == 't')
					delta = 0x100 - (l1->s_code & 0xff);
				else if (f4a34 == 1)
					delta = 0x100 - ((l1->torig - (l1->torig & 0xff00) + l1->s_code) & 0xff);
				else
					delta = 0;
				if (f4a34 == 1 && (unsigned)l1->s_code > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			bE3C16 += delta;
			l1->s_code += delta;
			if (l1->segno == g0474)
				l1->atr |= 8;
			while ((i = (cur = get_block(i))->xtnext) > -1) {
				if (l1 != (get_block(i))->ncode) {
					if (bE42B4[k][1])
						report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[1]);
					l1 = (get_block(i))->ncode;
					l1->torig = 0;
					pE3C0A = (char *)l1;
					place_sec(l1, cnt);
					q = i;
					delta = alloc_segment(l1, &cnt, 1, 't', &q);
					if (l1->s_bss + l1->s_data) {
						tblF332[q].bssptr = tblF332[k].bssptr;
						tblF332[q].datptr = tblF332[k].datptr;
						tblF332[k].bssptr = -1;
						tblF332[k].datptr = -1;
					}
					cur->xtnext = -1;
					if (f4a34 == 1) {
						bE3C16 += delta;
						l1->s_code += delta;
					} else if (f4a34 == 2) {
						bE3C1A += delta;
						l1->s_data += delta;
					} else if (f4a34 == 3) {
						b3C1E += delta;
						l1->s_bss += delta;
					}
				}
			}
		}
		if (tblF332[k].datptr > -1) {
			i = tblF332[k].datptr;
			blk = get_block(i);
			l2 = blk->ndata;
			if (bF320) {
				l1 = bF324;
				blk->ndata = l1;
				cnt--;
			} else
				l1 = l2;
			pE3C06 = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'd', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 2 && (l1->dorig & 0xff) == 0) || bE42B4[cnt - 1][2] == 't')
					delta = 0x100 - (l1->s_data & 0xff);
				else if (f4a34 == 2)
					delta = 0x100 - ((l1->dorig - (l1->dorig & 0xff00) + l1->s_data) & 0xff);
				else
					delta = 0;
				if (f4a34 == 2 && (unsigned)(l1->s_code + l1->s_data) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			bE3C1A += delta;
			l1->s_data += delta;
			if (l1->segno == g0474)
				l1->atr |= 8;
			goto ltd;
lhd:
			if (bE42B4[k][2])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[2]);
			l1 = (get_block(i))->ndata;
			l1->dorig = 0;
			place_sec(l1, cnt);
			pE3C06 = (char *)l1;
			q = i;
			delta = alloc_segment(l1, &cnt, 1, 'd', &q);
			cur->datnext = -1;
			if (f4a34 == 2) {
				bE3C1A += delta;
				l1->s_data += delta;
			} else {
				tblF332[q].bssptr = tblF332[k].bssptr;
				tblF332[k].bssptr = -1;
				b3C1E += delta;
				l1->s_bss += delta;
			}
			l2 = l1;
			goto ltd;
lbd:
			blk = get_block(i);
			if (l2 != blk->ndata)
				goto lhd;
			blk->ndata = l1;
ltd:
			i = (cur = get_block(i))->datnext;
			if (i > -1)
				goto lbd;
		}
		if (tblF332[k].bssptr > -1 && tblF332[k].bssptr < 0x4b0) {
			i = tblF332[k].bssptr;
			blk = get_block(i);
			l2 = blk->nbss;
			if (bF322) {
				l1 = bF328;
				blk->nbss = l1;
				cnt--;
			} else
				l1 = l2;
			pE3C0E = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'b', &q);
			}
			if (!bE4A2) {
				if ((f4a34 == 3 && (l1->borig & 0xff) == 0) || bE42B4[cnt - 1][3] == 't')
					delta = 0x100 - (l1->s_bss & 0xff);
				else if (f4a34 == 3)
					delta = 0x100 - ((l1->borig - (l1->borig & 0xff00) + l1->s_bss) & 0xff);
				else
					delta = 0;
				if (f4a34 == 3 && (unsigned)(l1->s_code + l1->s_data + l1->s_bss) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			b3C1E += delta;
			l1->s_bss += delta;
			if (l1->segno == g0474)
				l1->atr |= 8;
			goto ltb;
lhb:
			if (bE42B4[k][3])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, ptrs04ac[3]);
			l1 = (get_block(i))->nbss;
			l1->borig = l1->dorig + l1->s_data;
			pE3C0E = (char *)l1;
			if (*(char *)l1 == -1) {
				q = i;
				delta = alloc_segment(l1, &cnt, 1, 'b', &q);
				cur->bssnext = -1;
				b3C1E += delta;
				l1->s_bss += delta;
			}
			l2 = l1;
			goto ltb;
lbb:
			blk = get_block(i);
			if (l2 != blk->nbss)
				goto lhb;
			blk->nbss = l1;
ltb:
			i = (cur = get_block(i))->bssnext;
			if (i > -1)
				goto lbb;
		}
		if (tblF332[k].bssptr > 0x4af)
			tblF332[k].bssptr = -1;
	}
	if (g0474 == -1) {
		l1 = table32_entry(bE3C04);
		if (bE3C04++ > 0x100)
			report_error(2, "pass2 segment table overflow");
		bE3E82++;
		l1->atr = 8;
		q = 1;
		delta = alloc_segment(l1, &cnt, 0, 's', &q);
		g0474 = cnt - 1;
	}
	if (b3C12 != bF32E) {
		do {
			swapped = 0;
			p = bF32E;
			e = p->nxt;
			do {
				if (e->segno < p->segno) {
					swapped = 1;
					list_move_after(p, e);
				}
				e = (p = e)->nxt;
			} while (p != b3C12);
		} while (swapped);
	}
}

/* ins_block..5eec: findings:D11 */
struct p2seg *ins_block(kind, size, blk, val, idx)
int kind;
register unsigned size;
int blk;
int val;
int idx;
{
	register struct p2seg *s;
	register struct p1seg *b;
	int prev;
	int tmp;

	if (blk > -1) {
		while (blk != -1) {
			prev = blk;
			switch (kind) {
			case 2:
				s = (b = get_block(blk))->ncode;
				blk = b->xtnext;
				break;
			case 3:
				if (tblF332[idx].bssptr > 0x4af) {
					s = (get_block(blk))->ncode;
					blk = -1;
					tblF332[idx].bssptr = -1;
				} else {
					s = (b = get_block(blk))->ndata;
					blk = b->datnext;
				}
				break;
			case 4:
				if (tblF332[idx].bssptr > 0x4af) {
					s = (get_block(blk))->ncode;
					blk = -1;
					tblF332[idx].bssptr = -1;
				} else {
					s = (b = get_block(blk))->nbss;
					blk = b->bssnext;
				}
				break;
			}
			if (size > 0xfffe - (s->s_code + s->s_data + s->s_bss))
				continue;
			switch (kind) {
			case 4:
				if (tblF332[idx].bssptr == -1) {
					tblF332[idx].bssptr = val;
					return s;
				}
				tmp = (b = get_block(prev))->bssnext;
				b->bssnext = val;
				(get_block(val))->bssnext = tmp;
				return s;
			case 2:
				blk = prev;
				while ((b = get_block(blk))->ncode == s) {
					if (b->xtnext == -1)
						break;
					prev = blk;
					blk = (get_block(blk))->xtnext;
				}
				if ((get_block(blk))->ncode == s)
					prev = blk;
				tmp = (b = get_block(prev))->xtnext;
				b->xtnext = val;
				(get_block(val))->xtnext = tmp;
				return s;
			case 3:
				if (tblF332[idx].datptr == -1) {
					tblF332[idx].datptr = val;
					return s;
				}
				blk = prev;
				while ((b = get_block(blk))->ndata == s) {
					if (b->datnext == -1)
						break;
					prev = blk;
					blk = (get_block(blk))->datnext;
				}
				if ((get_block(blk))->ndata == s)
					prev = blk;
				tmp = (b = get_block(prev))->datnext;
				b->datnext = val;
				(get_block(val))->datnext = tmp;
				return s;
			}
			return s;
		}
	}
	if (kind == 4) {
		blk = tblF332[idx].datptr;
		while (blk > -1) {
			s = (b = get_block(blk))->ndata;
			blk = b->datnext;
			if (size <= 0xfffe - (s->s_code + s->s_data + s->s_bss))
				goto l5e82;
		}
	}
	if (bE3C04 < 0x100) {
		bE0F8E++;
		switch (kind) {
		case 2:
			tblF332[idx].bssptr = val + 0x4b0;
			if (tblF332[idx].txtptr < 0)
				tblF332[idx].txtptr = val;
			else
				(get_block(prev))->xtnext = val;
			break;
		case 3:
			if (tblF332[idx].datptr < 0)
				tblF332[idx].datptr = val;
			else
				(get_block(prev))->datnext = val;
			break;
		case 4:
			if (tblF332[idx].bssptr < 0 || tblF332[idx].bssptr > 0x4af)
				tblF332[idx].bssptr = val;
			else
				(get_block(prev))->bssnext = val;
			break;
		}
		if (bF32E == 0) {
			b3C12 = tab1c02;
			bF32E = tab1c02;
			bE3C04++;
			*(char *)bF32E = 0xff;
			return (struct p2seg *)bF32E;
		}
		s = table32_entry(bE3C04++);
		*(char *)s = 0xff;
		return s;
	} else
		report_error(2, "pass two segment table overflow");
l5e82:
	if (tblF332[idx].bssptr == -1) {
		tblF332[idx].bssptr = val;
		return s;
	}
	tmp = (b = get_block(prev))->bssnext;
	b->bssnext = val;
	(get_block(val))->bssnext = tmp;
	return s;
}

/* alloc_segment..60f0: done */
int alloc_segment(rec, pidx, a, b, q)
register int *pidx;
register struct p2seg *rec;
int a;
char b;
int *q;
{
	register int sum;
	register int i;

	if (a) {
		for (i = 0; i < 128; i++) {
			if (tblF332[i].txtptr == -1 && tblF332[i].datptr == -1 && tblF332[i].bssptr == -1 && tblF332[i].res == -1) {
				rec->segidx = i;
				rec->segno = i;
				switch (b) {
				case 't':
					tblF332[i].txtptr = *q;
					break;
				case 'd':
					tblF332[i].datptr = *q;
					break;
				case 'b':
					tblF332[i].bssptr = *q;
					break;
				}
				if (i > bE3E82)
					bE3E82 = i;
				tblF332[i].res = 100;
				*q = i;
				break;
			}
		}
		if (i == 128)
			report_error(2, "No more segments");
	} else {
		if (b != 'r') {
			while (tblF332[*pidx].res == 1000 || tblF332[*pidx].res == 100 || tblF332[*pidx].res == 3000)
				(*pidx)++;
			if (tblF332[*pidx].res == -1)
				tblF332[*pidx].res = 50;
		}
		rec->segidx = *pidx;
		rec->segno = *pidx;
		*pidx = *pidx + 1;
	}
	if (bF32E == 0) {
		b3C12 = rec;
		bF32E = rec;
	} else {
		b3C12->nxt = rec;
		rec->prv = b3C12;
		b3C12 = rec;
	}
	sum = rec->s_code + rec->s_data + rec->s_bss;
	if ((unsigned)sum > 0xff00)
		sum += 2;
	sum = sum & 0xff;
	if (bE4A2 == 0 && sum != 0)
		return 0x100 - sum;
	return 0;
}

/* seg_pad 0x60f0..0x6134: done */
seg_pad(q, a)
struct p2seg *q;
int a;
{
	int v;

	if ((char)a == 0x74) {
		v = q->s_code - bE3E7A;
		if (v)
			put_pad(v);
	} else {
		v = q->s_data - bE3E7A;
		if (v)
			put_pad(v);
	}
}

/* copy_segs..620e: done */
copy_segs()
{
	register int i;

	flush_buffer(&bE6E2);
	flush_buffer(&bE8EA);
	if (bE4A2) {
		flush_buffer(&bEF02);
		flush_buffer(&bEAF2);
	}
	for (i = 0; i <= bE3E82; i++) {
		bE3E7A = 0;
		if (tblF332[i].txtptr > -1)
			copy_blocks(tblF332[i].txtptr, 0x74, get_block(tblF332[i].txtptr)->ncode);
		if (tblF332[i].datptr > -1)
			copy_blocks(tblF332[i].datptr, 0x64, get_block(tblF332[i].datptr)->ndata);
	}
}

/* copy_blocks..642e: done */
int copy_blocks(idx, type, p)
register int idx;
char type;
struct p2seg *p;
{
	register int m;
	register int n;
	register long q;
	struct p2seg *cur;
	int k;
	long w;
	char buf[512];
	int j;
	struct p1seg *rec;

	cur = p;
head:
	if (idx == -1)
		goto done;
	if (p != cur)
		goto done;
	if (type == 0x74) {
		rec = get_block(idx);
		n = rec->s_code;
		q = rec->tbase;
		idx = rec->xtnext;
		cur = get_block(idx)->ncode;
		lseek(bE6E2.fildes, q, 0);
	} else {
		rec = get_block(idx);
		n = rec->s_data;
		q = rec->dbase;
		idx = rec->datnext;
		cur = get_block(idx)->ndata;
		lseek(bE8EA.fildes, q, 0);
	}
	if (bE4A2) {
		lseek(bEF02.fildes, q, 0);
		lseek(bEAF2.fildes, q, 0);
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
			if (read(bE6E2.fildes, buf, m) < 1)
				report_error(2, "i/o error reading from tmpoutb");
		} else {
			if (read(bE8EA.fildes, buf, m) < 1)
				report_error(2, "i/o error reading from doutb");
		}
		mput(&bE4DA, buf, m);
		if (bE4A2) {
			if (type == 0x74) {
				if (m != read(bEF02.fildes, buf, m))
					report_error(2, "eof error on trout.b");
			} else {
				if (m != read(bEAF2.fildes, buf, m))
					report_error(2, "eof error on droutb");
			}
			mput(&bECFA, buf, m);
		}
	}
	goto head;
done:
	seg_pad(p, type);
	if (idx != -1) {
		p = cur;
		bE3E7A = 0;
		goto head;
	}
	bE3E7A = 0;
	return 0;
}

/* list_move_after 0x642e..0x64d0: done */
int list_move_after(p, e)
struct p2seg *p;
struct p2seg *e;
{
	register struct p2seg *q;
	register struct p2seg *r;
	int spare[2];	/* unused: the frame of the original has 4 more bytes */

	q = p->prv;
	r = e->nxt;
	if (p == bF32E)
		bF32E = e;
	else
		q->nxt = e;
	if (e == b3C12)
		b3C12 = p;
	else
		r->prv = p;
	p->nxt = r;
	p->prv = e;
	e->nxt = p;
	e->prv = q;
}

/* put_pad 0x64d0..0x6522: done */
int put_pad(n)
register int n;
{
	if (bE4A2)
		return 0;
	while (n > 0) {
		put_int_buf(0, &bE4DA);
		if (bE4A2)
			put_int_buf(0, &bECFA);
		n = n - 2;
	}
}
