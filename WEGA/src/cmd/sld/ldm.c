/*
 * ldm.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ldm.c\t3.10  10/27/83 12:21:07 - 87wega3.2";
int np2new = -1;

/* scan_m_opt: scan a -M option (memory layout) and fill the segment lists (0x3c16..0x410c of /bin/sld) */
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
		mcnt++;
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
	if (zflag && v == 127)
		report_error(2, "segment number in illegal range with %d", v);
	if (v < 0 || v > 127)
		report_error(2, "segment number out of range with %d", v);
	if (v > maxseg)
		maxseg = v;
	if (s[k] == ':') {
		secspec++;
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
			if (secflg[v][w] != 0)
				report_error(2, "-%s: section already specified", s);
			secflg[v][w] = s[k];
			k++;
		} else
			report_error(2, "-%s: invalid location selector (must be t or b)", s);
		if (s[k] == 0) {
			s = *curarg++;
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
			if (sscanf(s + k, fmt, &secorg[v][w]) != 1)
				report_error(2, "-%s: bad offset", s);
		}
	} else if (s[k] != 0)
		report_error(2, "-%s: usage error", s);
	p = *curarg;
	if (p[0] == '+') {
		if (type == 's')
			report_error(2, "invalid -Ms option ");
		curarg++;
		p = *curarg;
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
			curarg++;
			p = *curarg;
		}
		if (*ip == argc)
			report_error(2, "no closing + for M file list");
		(*ip)++;
		curarg++;
	} else {
		switch (type) {
		case 't':
			tseg = v;
			break;
		case 'd':
			dseg = v;
			break;
		case 'b':
			bseg = v;
			break;
		case 'a':
			w = iflag ? v + 1 : v;
			bseg = w;
			dseg = w;
			tseg = v;
			if (dseg > maxseg)
				maxseg = dseg;
			break;
		case 's':
			if (zflag)
				report_error(2, "cannot use -Ms option after -z");
			resv_seg(v, 's');
			break;
		default:
			report_error(2, "%c illegal type for M; not s, t,d or b", type);
		}
	}
}

/* mlist_add: add an entry to the -M list, in order (0x410c..0x42f8 of /bin/sld) */
int mlist_add(c, y, x)
char c;
char *y;
int x;
{
	int i;
	int prev;
	int k;
	int r;

	if (mlcnt == 0x32)
		report_error(2, "-M list overflow, maximum number is %d", 0x32);
	i = mlhead;
	prev = 0;
	cmpmode = 1;
	k = 0;
	goto cond;
body:
	r = str_compare(y, mlist[i].mname);
	if (r < 0)
		goto nxt;
	if (r == 0)
		goto attr;
	prev = i;
	i = mlist[i].mnext;
nxt:
	k++;
cond:
	if (k < mlcnt)
		goto body;
	if (i != mlhead || k != 0)
		goto insmid;
	mlist[mlcnt].mnext = mlhead;
	mlhead = mlcnt;
	goto newent;
insmid:
	mlist[mlcnt].mnext = mlist[prev].mnext;
	mlist[prev].mnext = mlcnt;
newent:
	k = mlcnt;
	mlcnt++;
	mlist[k].mname = y;
	mlist[k].mbss = -1;
	mlist[k].mdata = -1;
	mlist[k].mtext = -1;
attr:
	if (c == 0x61 || c == 0x74)
		mlist[k].mtext = x;
	if (c == 0x61 || c == 0x64)
		mlist[k].mdata = x;
	if (c == 0x61 || c == 0x62)
		mlist[k].mbss = x;
	cmpmode = 0;
}

/* resv_seg: reserve a segment number given with an option (0x42f8..0x4444 of /bin/sld) */
int resv_seg(x, c)
int x;
char c;
{
	if (x < 0 || x >= 0x80)
		report_error(2, "reserved segment is out of range with %d", x);
	if (x > maxseg)
		maxseg = x;
	switch (c) {
	case 'r':
		if (reslist[x].res == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			stksegn = -1;
		}
		reslist[x].res = 0x03e8;
		break;
	case 's':
		if (stksegn > -1)
			reslist[stksegn].res = -1;
		if (reslist[x].res == 0x03e8) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			stksegn = -1;
		} else {
			reslist[x].res = 0x07d0;
			stksegn = x;
		}
		break;
	case 'M':
		if (reslist[x].res == 0x07d0) {
			report_error(0, "warning: stack seg has been moved because seg is reserved");
			stksegn = -1;
		}
		reslist[x].res = 0x01f4;
		break;
	}
}

/* copy_leafname: copy the leaf name of a path (0x4444..0x44e6 of /bin/sld) */
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

/* end_pass1: end of pass one: define _etext, _edata, _end, lay out the sections (0x44e6..0x4968 of /bin/sld) */
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
	p_etext = *slookup("_etext");
	p_edata = *slookup("_edata");
	p_end = *slookup("_end");
	p_stkseg = *slookup("_stkseg");
	if (p_stkseg == 0) {
		if (stksegn > -1)
			report_error(1, "warning: _stkseg undefined");
		else
			stksegn = -2;
	} else {
		if (!nrflag && !iflag && stksegn == -1 && mcnt < 2 && !zflag)
			stksegn = -3;
		else if (rflag)
			stksegn = -3;
	}
	end = table16_entry(symindex);
	if (!rflag) {
		ep = table16_entry(0);
		while (ep < end) {
			if (ep->stype == ' ' && ep->svalue == 0 && ep != p_end && ep != p_edata && ep != p_etext && ep != p_stkseg) {
				rflag++;
				dflag = 0;
				break;
			}
			ep = next_entry(ep);
		}
	}
	if (rflag) {
		Oflag = 0;
		sflag = 0;
		secspec = 0;
		for (i = 0; i < 128; i++) {
			for (t8 = 0; t8 < 4; t8++)
				secflg[i][(int)t8] = 0;
		}
	}
	comsize = 0;
	if (dflag == 0 && rflag)
		goto dolayout;
	ep = table16_entry(0);
	while (ep < end) {
		if (ep->stype == ' ') {
			t8 = ep->svalue & 0xffff;
			if (t8 != 0) {
				t8 = (t8 + 1) & 0xfffe;
				t4 = comsize + t8;
				if (t4 > 0x10000L || flg == 0) {
					if (nblk >= maxblk)
						report_error(2, "pass one segment table overflow -- Try -Ap option");
					comsize = 0;
					t4 = t8;
					blk2 = get_block(nblk++);
					blk2->bssnext = -1;
					blk2->s_code = 0;
					blk2->s_data = 0;
					blk2->nboff = bseg;
					flg++;
				}
				ep->sseg = nblk - 1;
				ep->svalue = comsize;
				ep->stype = '%';
				get_block(nblk - 1)->s_bss = (int)(comsize = t4);
			}
		}
		ep = next_entry(ep);
	}
dolayout:
	lay_out();
	if (dflag || !rflag) {
		ldrsym(p_etext, 0x22);
		ldrsym(p_edata, 0x23);
		ldrsym(p_end, 0x24);
		if (!rflag)
			ldrsym(p_stkseg, 0x21);
	}
	nundef = 0;
	ep = table16_entry(0);
	while (ep < end) {
		if (ep != p_etext && ep != p_edata && ep != p_end && ep != p_stkseg) {
			switch (ep->stype) {
			case 0x20:
				if (arflag)
					break;
				exstat |= 1;
				if (ep->svalue)
					break;
				if (nundef == 0)
					printf("Undefined:\n");
				nundef++;
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
	if (sflag || xflag)
		symbytes = 0;
	nsymf = (unsigned)symbytes >> 4;
}

/* alloc_resv: allocate the reserved segments (0x4968..0x4a16 of /bin/sld) */
int alloc_resv(q, p, x, res)
struct p2seg *q;
int *p;
int x;
int *res;
{
	int lcl;

	if (zflag && x == 0x7f) {
		(*p)++;
		return 0;
	}
	q = table32_entry(np2seg);
	if (np2seg++ > 0x100)
		report_error(2, "pass2 segment table overflow");
	if (reslist[x].res == 0x0bb8)
		q->atr |= 8;
	else
		q->atr = 4;
	lcl = 1;
	*res = alloc_segment(q, p, 0, 0x72, &lcl);
}

/* lay_out: assign the segments and addresses of all sections (the big layout routine) (0x4a16..0x5a2c of /bin/sld) */
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

	lastb = 0;
	lastt = 0;
	lastd = 0;
	p2tail = 0;
	p2head = 0;
	np2seg = 0;
	for (k = 1; k < 4; k++) {
		for (i = 0; i < nblk; i++) {
			blk = get_block(i);
			if (blk->s_code && k == 1) {
				blk->ncode = ins_block(2, blk->s_code, reslist[blk->ncoff].txtptr, i, blk->ncoff);
				blk->ncoff = blk->ncode->s_code;
				blk->ncode->s_code += blk->s_code;
				ttotal += (unsigned)blk->s_code;
				blk->ncode->atr |= 1;
			}
			if (blk->s_data && k == 2) {
				if (reslist[blk->ndoff].bssptr > -1)
					blk->ndata = ins_block(3, blk->s_data, reslist[blk->ndoff].bssptr - 0x4b0, i, blk->ndoff);
				else
					blk->ndata = ins_block(3, blk->s_data, reslist[blk->ndoff].datptr, i, blk->ndoff);
				blk->ndoff = blk->ndata->s_data;
				blk->ndata->s_data += blk->s_data;
				dtotal += (unsigned)blk->s_data;
				blk->ndata->atr |= 2;
			}
			if (blk->s_bss && k == 3) {
				if (reslist[blk->nboff].bssptr > 0x4af)
					blk->nbss = ins_block(4, blk->s_bss, reslist[blk->nboff].bssptr - 0x4b0, i, blk->nboff);
				else
					blk->nbss = ins_block(4, blk->s_bss, reslist[blk->nboff].bssptr, i, blk->nboff);
				blk->nboff = blk->nbss->s_bss;
				blk->nbss->s_bss += blk->s_bss;
				btotal += (unsigned)blk->s_bss;
				blk->nbss->atr |= 4;
			}
		}
	}
	if (stksegn >= 0 && stksegn < 128) {
		l1 = (struct p2seg *)0x7f00ffff;
		l2 = (struct p2seg *)0x7f00ffff;
		i = reslist[stksegn].txtptr;
		if (i != -1)
			l1 = (get_block(i))->ncode;
		else {
			i = reslist[stksegn].datptr;
			if (i != -1)
				l1 = (get_block(i))->ndata;
			else {
				i = reslist[stksegn].bssptr;
				if (i != -1)
					l1 = (get_block(i))->nbss;
			}
		}
		if (l2 == l1)
			reslist[stksegn].res = 3000;
	}
	p2head = 0;
	max = maxseg;
	cnt = 0;
	k = 0;
	while (k <= max) {
		if (reslist[k].res > 500 && reslist[k].res != 2000) {
			alloc_resv(l1, &cnt, k, &delta);
			cnt--;
			if (!(reslist[k].txtptr == -1 && reslist[k].datptr == -1 && reslist[k].bssptr == -1)) {
				for (j = 0; j < 128; j++) {
					if (reslist[j].txtptr == -1 && reslist[j].datptr == -1 && reslist[j].bssptr == -1 && reslist[j].res == -1) {
						report_error(0, "warning: image in seg %d has been moved to seg %d because of -R; origins are reset", k, j);
						reslist[j].txtptr = reslist[k].txtptr;
						reslist[j].datptr = reslist[k].datptr;
						reslist[j].bssptr = reslist[k].bssptr;
						reslist[j].res = 500;
						reslist[k].txtptr = -1;
						reslist[k].datptr = -1;
						reslist[k].bssptr = -1;
						if (j > maxseg)
							maxseg = j;
						break;
					}
				}
			}
		}
		k++;
		cnt++;
	}
	max = maxseg;
	k = 0;
	cnt = 0;
	for (; k <= max; k++) {
		if (reslist[k].res == 100 || reslist[k].res == 50 || (reslist[k].res == -1 && reslist[k].txtptr < 0 && reslist[k].datptr < 0 && (reslist[k].bssptr < 0 || reslist[k].bssptr > 0x4af))) {
			cnt++;
			continue;
		}
		if (reslist[k].res == 1000 || reslist[k].res == 3000) {
			cnt++;
			continue;
		}
		i = reslist[k].txtptr;
		if (i != -1)
			l1 = (get_block(i))->ncode;
		else {
			i = reslist[k].datptr;
			if (i != -1)
				l1 = (get_block(i))->ndata;
			else {
				i = reslist[k].bssptr;
				if (i == -1)
					continue;
				l1 = (get_block(i))->nbss;
			}
		}
		dsplf = 0;
		bsplf = 0;
		place_sec(l1, cnt);
		if (reslist[k].txtptr > -1) {
			lastt = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 't', &q);
			}
			if (!rflag) {
				if ((placekd == 1 && (l1->torig & 0xff)) || secflg[cnt - 1][1] == 't')
					delta = 0x100 - (l1->s_code & 0xff);
				else if (placekd == 1)
					delta = 0x100 - ((l1->torig - (l1->torig & 0xff00) + l1->s_code) & 0xff);
				else
					delta = 0;
				if (placekd == 1 && (unsigned)l1->s_code > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			ttotal += delta;
			l1->s_code += delta;
			if (l1->segno == stksegn)
				l1->atr |= 8;
			while ((i = (cur = get_block(i))->xtnext) > -1) {
				if (l1 != (get_block(i))->ncode) {
					if (secflg[k][1])
						report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, secnam[1]);
					l1 = (get_block(i))->ncode;
					l1->torig = 0;
					lastt = (char *)l1;
					place_sec(l1, cnt);
					q = i;
					delta = alloc_segment(l1, &cnt, 1, 't', &q);
					if (l1->s_bss + l1->s_data) {
						reslist[q].bssptr = reslist[k].bssptr;
						reslist[q].datptr = reslist[k].datptr;
						reslist[k].bssptr = -1;
						reslist[k].datptr = -1;
					}
					cur->xtnext = -1;
					if (placekd == 1) {
						ttotal += delta;
						l1->s_code += delta;
					} else if (placekd == 2) {
						dtotal += delta;
						l1->s_data += delta;
					} else if (placekd == 3) {
						btotal += delta;
						l1->s_bss += delta;
					}
				}
			}
		}
		if (reslist[k].datptr > -1) {
			i = reslist[k].datptr;
			blk = get_block(i);
			l2 = blk->ndata;
			if (dsplf) {
				l1 = dsplseg;
				blk->ndata = l1;
				cnt--;
			} else
				l1 = l2;
			lastd = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'd', &q);
			}
			if (!rflag) {
				if ((placekd == 2 && (l1->dorig & 0xff) == 0) || secflg[cnt - 1][2] == 't')
					delta = 0x100 - (l1->s_data & 0xff);
				else if (placekd == 2)
					delta = 0x100 - ((l1->dorig - (l1->dorig & 0xff00) + l1->s_data) & 0xff);
				else
					delta = 0;
				if (placekd == 2 && (unsigned)(l1->s_code + l1->s_data) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			dtotal += delta;
			l1->s_data += delta;
			if (l1->segno == stksegn)
				l1->atr |= 8;
			goto ltd;
lhd:
			if (secflg[k][2])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, secnam[2]);
			l1 = (get_block(i))->ndata;
			l1->dorig = 0;
			place_sec(l1, cnt);
			lastd = (char *)l1;
			q = i;
			delta = alloc_segment(l1, &cnt, 1, 'd', &q);
			cur->datnext = -1;
			if (placekd == 2) {
				dtotal += delta;
				l1->s_data += delta;
			} else {
				reslist[q].bssptr = reslist[k].bssptr;
				reslist[k].bssptr = -1;
				btotal += delta;
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
		if (reslist[k].bssptr > -1 && reslist[k].bssptr < 0x4b0) {
			i = reslist[k].bssptr;
			blk = get_block(i);
			l2 = blk->nbss;
			if (bsplf) {
				l1 = bsplseg;
				blk->nbss = l1;
				cnt--;
			} else
				l1 = l2;
			lastb = (char *)l1;
			if (*(char *)l1 == -1) {
				q = 1;
				delta = alloc_segment(l1, &cnt, 0, 'b', &q);
			}
			if (!rflag) {
				if ((placekd == 3 && (l1->borig & 0xff) == 0) || secflg[cnt - 1][3] == 't')
					delta = 0x100 - (l1->s_bss & 0xff);
				else if (placekd == 3)
					delta = 0x100 - ((l1->borig - (l1->borig & 0xff00) + l1->s_bss) & 0xff);
				else
					delta = 0;
				if (placekd == 3 && (unsigned)(l1->s_code + l1->s_data + l1->s_bss) > 0xff00)
					delta = delta - 2;
			} else
				delta = 0;
			btotal += delta;
			l1->s_bss += delta;
			if (l1->segno == stksegn)
				l1->atr |= 8;
			goto ltb;
lhb:
			if (secflg[k][3])
				report_error(2, "overflow in offset segment %d, %s section\n", cnt - 1, secnam[3]);
			l1 = (get_block(i))->nbss;
			l1->borig = l1->dorig + l1->s_data;
			lastb = (char *)l1;
			if (*(char *)l1 == -1) {
				q = i;
				delta = alloc_segment(l1, &cnt, 1, 'b', &q);
				cur->bssnext = -1;
				btotal += delta;
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
		if (reslist[k].bssptr > 0x4af)
			reslist[k].bssptr = -1;
	}
	if (stksegn == -1) {
		l1 = table32_entry(np2seg);
		if (np2seg++ > 0x100)
			report_error(2, "pass2 segment table overflow");
		maxseg++;
		l1->atr = 8;
		q = 1;
		delta = alloc_segment(l1, &cnt, 0, 's', &q);
		stksegn = cnt - 1;
	}
	if (p2tail != p2head) {
		do {
			swapped = 0;
			p = p2head;
			e = p->nxt;
			do {
				if (e->segno < p->segno) {
					swapped = 1;
					list_move_after(p, e);
				}
				e = (p = e)->nxt;
			} while (p != p2tail);
		} while (swapped);
	}
}

/* ins_block: insert a block into the segment list (0x5a2c..0x5eec of /bin/sld) */
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
				if (reslist[idx].bssptr > 0x4af) {
					s = (get_block(blk))->ncode;
					blk = -1;
					reslist[idx].bssptr = -1;
				} else {
					s = (b = get_block(blk))->ndata;
					blk = b->datnext;
				}
				break;
			case 4:
				if (reslist[idx].bssptr > 0x4af) {
					s = (get_block(blk))->ncode;
					blk = -1;
					reslist[idx].bssptr = -1;
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
				if (reslist[idx].bssptr == -1) {
					reslist[idx].bssptr = val;
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
				if (reslist[idx].datptr == -1) {
					reslist[idx].datptr = val;
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
		blk = reslist[idx].datptr;
		while (blk > -1) {
			s = (b = get_block(blk))->ndata;
			blk = b->datnext;
			if (size <= 0xfffe - (s->s_code + s->s_data + s->s_bss))
				goto gotblk;
		}
	}
	if (np2seg < 0x100) {
		np2new++;
		switch (kind) {
		case 2:
			reslist[idx].bssptr = val + 0x4b0;
			if (reslist[idx].txtptr < 0)
				reslist[idx].txtptr = val;
			else
				(get_block(prev))->xtnext = val;
			break;
		case 3:
			if (reslist[idx].datptr < 0)
				reslist[idx].datptr = val;
			else
				(get_block(prev))->datnext = val;
			break;
		case 4:
			if (reslist[idx].bssptr < 0 || reslist[idx].bssptr > 0x4af)
				reslist[idx].bssptr = val;
			else
				(get_block(prev))->bssnext = val;
			break;
		}
		if (p2head == 0) {
			p2tail = p2tab;
			p2head = p2tab;
			np2seg++;
			*(char *)p2head = 0xff;
			return (struct p2seg *)p2head;
		}
		s = table32_entry(np2seg++);
		*(char *)s = 0xff;
		return s;
	} else
		report_error(2, "pass two segment table overflow");
gotblk:
	if (reslist[idx].bssptr == -1) {
		reslist[idx].bssptr = val;
		return s;
	}
	tmp = (b = get_block(prev))->bssnext;
	b->bssnext = val;
	(get_block(val))->bssnext = tmp;
	return s;
}

/* alloc_segment: allocate the next free segment number (0x5eec..0x60f0 of /bin/sld) */
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
			if (reslist[i].txtptr == -1 && reslist[i].datptr == -1 && reslist[i].bssptr == -1 && reslist[i].res == -1) {
				rec->segidx = i;
				rec->segno = i;
				switch (b) {
				case 't':
					reslist[i].txtptr = *q;
					break;
				case 'd':
					reslist[i].datptr = *q;
					break;
				case 'b':
					reslist[i].bssptr = *q;
					break;
				}
				if (i > maxseg)
					maxseg = i;
				reslist[i].res = 100;
				*q = i;
				break;
			}
		}
		if (i == 128)
			report_error(2, "No more segments");
	} else {
		if (b != 'r') {
			while (reslist[*pidx].res == 1000 || reslist[*pidx].res == 100 || reslist[*pidx].res == 3000)
				(*pidx)++;
			if (reslist[*pidx].res == -1)
				reslist[*pidx].res = 50;
		}
		rec->segidx = *pidx;
		rec->segno = *pidx;
		*pidx = *pidx + 1;
	}
	if (p2head == 0) {
		p2tail = rec;
		p2head = rec;
	} else {
		p2tail->nxt = rec;
		rec->prv = p2tail;
		p2tail = rec;
	}
	sum = rec->s_code + rec->s_data + rec->s_bss;
	if ((unsigned)sum > 0xff00)
		sum += 2;
	sum = sum & 0xff;
	if (rflag == 0 && sum != 0)
		return 0x100 - sum;
	return 0;
}

/* seg_pad: pad the output of a segment (0x60f0..0x6134 of /bin/sld) */
seg_pad(q, a)
struct p2seg *q;
int a;
{
	int v;

	if ((char)a == 0x74) {
		v = q->s_code - ncopied;
		if (v)
			put_pad(v);
	} else {
		v = q->s_data - ncopied;
		if (v)
			put_pad(v);
	}
}

/* copy_segs: copy the segments from the temporary files to the output (0x6134..0x620e of /bin/sld) */
copy_segs()
{
	register int i;

	flush_buffer(&tmpoutb);
	flush_buffer(&doutb);
	if (rflag) {
		flush_buffer(&troutb);
		flush_buffer(&droutb);
	}
	for (i = 0; i <= maxseg; i++) {
		ncopied = 0;
		if (reslist[i].txtptr > -1)
			copy_blocks(reslist[i].txtptr, 0x74, get_block(reslist[i].txtptr)->ncode);
		if (reslist[i].datptr > -1)
			copy_blocks(reslist[i].datptr, 0x64, get_block(reslist[i].datptr)->ndata);
	}
}

/* copy_blocks: copy the blocks of one segment from the temporary files (0x620e..0x642e of /bin/sld) */
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
		lseek(tmpoutb.fildes, q, 0);
	} else {
		rec = get_block(idx);
		n = rec->s_data;
		q = rec->dbase;
		idx = rec->datnext;
		cur = get_block(idx)->ndata;
		lseek(doutb.fildes, q, 0);
	}
	if (rflag) {
		lseek(troutb.fildes, q, 0);
		lseek(droutb.fildes, q, 0);
	}
	ncopied += n;
	while (n) {
		if ((unsigned)n > 0x200) {
			n = n - 0x200;
			m = 0x200;
		} else {
			m = n;
			n = 0;
		}
		if (type == 0x74) {
			if (read(tmpoutb.fildes, buf, m) < 1)
				report_error(2, "i/o error reading from tmpoutb");
		} else {
			if (read(doutb.fildes, buf, m) < 1)
				report_error(2, "i/o error reading from doutb");
		}
		mput(&toutb, buf, m);
		if (rflag) {
			if (type == 0x74) {
				if (m != read(troutb.fildes, buf, m))
					report_error(2, "eof error on trout.b");
			} else {
				if (m != read(droutb.fildes, buf, m))
					report_error(2, "eof error on droutb");
			}
			mput(&routb, buf, m);
		}
	}
	goto head;
done:
	seg_pad(p, type);
	if (idx != -1) {
		p = cur;
		ncopied = 0;
		goto head;
	}
	ncopied = 0;
	return 0;
}

/* list_move_after: move the entry p behind the entry e of the doubly linked list of pass-two segments (0x642e..0x64d0 of /bin/sld) */
int list_move_after(p, e)
struct p2seg *p;
struct p2seg *e;
{
	register struct p2seg *q;
	register struct p2seg *r;
	int spare[2];	/* unused: the frame of the original has 4 more bytes */

	q = p->prv;
	r = e->nxt;
	if (p == p2head)
		p2head = e;
	else
		q->nxt = e;
	if (e == p2tail)
		p2tail = p;
	else
		r->prv = p;
	p->nxt = r;
	p->prv = e;
	e->nxt = p;
	e->prv = q;
}

/* put_pad: write n zero bytes (n/2 words) to the output (0x64d0..0x6522 of /bin/sld) */
int put_pad(n)
register int n;
{
	if (rflag)
		return 0;
	while (n > 0) {
		put_int_buf(0, &toutb);
		if (rflag)
			put_int_buf(0, &routb);
		n = n - 2;
	}
}
