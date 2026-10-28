/*
 * symbols.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]symbols.c\t3.2  10/27/83 12:25:22 - 87wega3.2";
struct fsym fsym = {{0, 0, 0, 0, 0, 0, 0, 0}, 0, 0};
int unused15d8 = 0;
unsigned rlsize = 0;

/* read_entry_hdr 0x6684..0x6800: done */
read_entry_hdr()
{
	register int i;
	register int result;
	register char *cur;
	char cnt;
	char *end;
	struct ssym r;
	char *q;
	char *base;

	result = 0;
	if (segobj) {
		mget(&r, 14);
		cursym.svalue = r.ssval;
		cursym.stype = r.sstyp;
		cursym.sseg = r.ssseg;
		for (i = 0; i < 8; i++)
			cursym.sname[i] = r.ssnam[i];
		if (swapflg)
			swap_pair(&cursym);
		if (cursym.sname[0] & 0x80) {
			cnt = cursym.sname[0] & 0x7f;
			cur = &r;
			end = r.ssnam + cnt;
			base = lnbuf;
			q = lnbuf;
			goto test;
body:
			mget(q, 14);
			q += 14;
test:
			cur += 14;
			if (cur < end)
				goto body;
			result = ((int)q - (int)base) / 16;
			if (((int)q - (int)base) % 16 != 0)
				result++;
		}
	} else {
		mget(&fsym, 12);
		if (swapflg) {
			fsym.ftype = swap_bytes(fsym.ftype);
			fsym.fvalue = swap_bytes(fsym.fvalue);
		}
		cvt_fsym(&fsym, &cursym, fsegflg);
	}
	cursym.svalue &= 0xff00ffffL;
	cursym.stype &= ~0x40;
	return result;
}

/* cvt_fsym 0x6800..0x6866: done */
int cvt_fsym(p, q, a)
struct fsym *p;
struct symbol *q;
int a;
{
	char b;

	q->svalue = p->fvalue;
	q->stype = p->ftype;
	q->sseg = 0;
	if (a != 0) {
		b = q->stype & 0x1f;
		if (b == 3 || b == 4)
			q->sseg = 1;
	}
}

/* pack_entry..68a2: findings:D4 */
pack_entry(p, q)
struct symbol *p;
struct fsym *q;
{
	q->fvalue = (int)p->svalue;
	q->ftype = p->stype;
	trunc_name(p->sname, q);
}

/* ldrsym..6a08: findings:D2,D16 */
int ldrsym(a, c)
struct symbol *a;
char c;
{
	register struct symbol *p;
	struct p2seg *q;

	p = a;
	if (!p)
		return;
	if (p->stype != 0x20 || p->svalue != 0) {
		if (put_name(p->sname) < 8)
			putchar(9);
		putchar(0x20);
		report_error(1, "-- Multiply defined");
		return;
	}
	p->stype = c;
	p->svalue = 0;
	p->sseg = 0;
	if (c == 0x22) {
		q = (struct p2seg *)lastt;
		if (!q)
			return;
		p->svalue = ((long)(q->segno) << 24) + (unsigned)q->s_code;
		p->sseg = q->segidx;
		return;
	}
	if (c == 0x23) {
		q = (struct p2seg *)lastd;
		if (!q)
			return;
		p->svalue = (unsigned)(q->s_code + q->s_data);
		p->svalue = p->svalue + ((long)(q->segno) << 24);
		p->sseg = q->segidx;
		return;
	}
	if (c == 0x24) {
		q = (struct p2seg *)lastb;
		if (!q)
			return;
		p->svalue = (unsigned)(q->s_code + q->s_data + q->s_bss);
		p->svalue = p->svalue + ((long)(q->segno) << 24);
		p->sseg = q->segidx;
		return;
	}
	if (c != 0x21)
		return;
	p->svalue = 0;
	if (stksegn == -1 || stksegn == -3)
		p->sseg = maxseg;
	else
		p->sseg = stksegn;
	p->svalue = p->svalue + ((long)p->sseg << 24);
}

/* mkfsym 0x6a08..0x6a58: done */
mkfsym(p)
char *p;
{
	if (sflag)
		return;
	if (xflag)
		return;
	unpack_run(p, cursym.sname);
	cursym.stype = 0x1f;
	cursym.svalue = 0;
	cursym.sseg = 0;
	map_sym();
	put_sym(&soutb, &cursym);
}

/* lookup_name..6b5a: done */
struct symbol **lookup_name()
{
	register struct symbol **slot;
	register char *cur;
	register char *rec;
	int h;
	int flag;
	char *start;
	char *end;

	h = 0;
	cur = cursym.sname;
	start = cur;
	end = lnbuf;
	if (*cur & 0x80) {
		start++;
		if (nlflag == 0)
			end++;
		else
			end = cursym.sname + (*cur & 0x7f);
	}
	for (cur = start; cur < end; cur++)
		h = h * 2 + *cur;
	slot = &hshtab[(h & 0x7fff) % 0x0ffa + 2];
	while (*slot) {
		rec = (*slot)->sname;
		flag = 0;
		if (nlflag == 0)
			goto l1;
		cur = cursym.sname;
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
		slot++;
		if (slot >= (struct symbol **)local)
			slot = hshtab;
	}
	return slot;
}

/* slookup 0x6b5a..0x6b84: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
struct symbol **slookup(p)
char *p;
{
	long dummy;

	pack_name(p, cursym.sname);
	cursym.stype = 0x20;
	cursym.svalue = 0;
	lookup_name();
}

/* pslookup 0x6b84..0x6bae: done */
/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
struct symbol **pslookup(p)
char *p;
{
	long dummy;

	unpack_run(p, cursym.sname);
	cursym.stype = 0x20;
	cursym.svalue = 0;
	lookup_name();
}

/* enter..6c6a: findings:D2 */
enter(a)
struct symbol **a;
{
	register struct symbol *r;

	if (*a == 0) {
		if (symindex >= 0x0ffa)
			report_error(2, "Symbol table overflow");
		symhash[symindex] = a;
		r = table16_entry(symindex++);
		lastsym = r;
		*a = r;
		symindex = unpack_run(cursym.sname, r->sname) + symindex;
		r->stype = cursym.stype;
		r->svalue = cursym.svalue;
		r->sseg = cursym.sseg;
		return 1;
	}
	lastsym = *a;
	return 0;
}

/* map_sym..6da8: findings:D2,D6,D15 */
map_sym()
{
	register char *w;
	register struct p1seg *p;
	int i;

	i = fblk + cursym.sseg;
	p = get_block(i);
	w = (char *)&cursym.svalue;
	switch (cursym.stype) {
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
		if (cursym.stype & 0x20)
			cursym.stype = 0x21;
		goto end;
	}
loop:
	if (i >= nblk)
		goto clear;
	p = get_block(i);
	if (p->s_code)
		goto h2;
	i++;
	goto loop;
clear:
	cursym.sseg = 0;
	cursym.svalue = 0;
	goto end;
h2:
	*(int *)w = p->ncode->segno << 8;
	w += 2;
	*(int *)w = p->ctrel + p->ncoff + p->ncode->torig + *(int *)w;
	cursym.sseg = p->ncode->segidx;
	goto end;
h3:
	*(int *)w = p->ndata->segno << 8;
	w += 2;
	*(int *)w = p->cdrel + p->ndoff + p->ndata->dorig + *(int *)w;
	cursym.sseg = p->ndata->segidx;
	goto end;
h4:
	*(int *)w = p->nbss->segno << 8;
	w += 2;
	*(int *)w = p->cbrel + p->nboff + p->nbss->borig + *(int *)w;
	cursym.sseg = p->nbss->segidx;
end:
	;
}

/* symreloc..6e1a: findings:D6 */
symreloc()
{
	switch (cursym.stype) {
	case 2:
	case 0x22:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fctrel);
		break;
	case 3:
	case 0x23:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fcdrel);
		break;
	case 4:
	case 0x24:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fcbrel);
		break;
	case 0x20:
		break;
	default:
		if (cursym.stype & 0x20)
			cursym.stype = 0x21;
	}
}

/* put_sym 0x6e1a..0x6efe: done */
put_sym(a, b)
struct buf *a;
struct symbol *b;
{
	char cnt;
	struct ssym r;
	register char *q;
	register int k;
	register int i;

	r.ssval = b->svalue;
	r.sstyp = b->stype;
	r.ssseg = b->sseg;
	for (i = 0; i < 8; i++)
		r.ssnam[i] = b->sname[i];
	if (swapflg)
		swap_pair(&r.ssval);
	mput(a, &r, 14);
	nwsym++;
	if (b->sname[0] & 0x80) {
		q = (char *)(b + 1);
		cnt = (b->sname[0] & 0x7f) - 8;
		k = 1;
		while (cnt > 0) {
			mput(a, q, 14);
			nwsym++;
			q += 14;
			k++;
			cnt -= 14;
		}
		return k;
	}
	return 1;
}

/* put_all 0x6efe..0x6f46: done */
put_all()
{
	register struct symbol *p;
	register struct symbol *end;

	p = table16_entry(0);
	end = table16_entry(symindex);
	while (p < end) {
		p->stype |= 0x40;
		put_sym(&toutb, p);
		p->stype &= ~0x40;
		p = next_entry(p);
	}
}

/* chk_ranlib..6fce: findings:D7 */
chk_ranlib(p, n)
struct rlent *p;
int n;
{
	struct rlent *e;
	struct rlent *q;
	struct symbol *q2;

	q = p;
	e = p + n;
	goto test;
body:
	if (p >= q) {
		if ((p->rname[0] & 0x80) != 0)
			q = (struct rlent *)(p->rname + (p->rname[0] & 0x7f));
		q2 = *pslookup(p->rname);
		if (q2 != 0) {
			if (q2->stype != 0x20)
				warn_sym_redef(0, q2);
		}
	}
	p++;
test:
	if (p < e)
		goto body;
}

/* table16_entry 0x6fd0..0x6ffe: done */
struct symbol *table16_entry(x)
unsigned x;
{
	int spare[4];	/* unused: the frame of the original has 8 more bytes */

	if (x < nsym)
		return symtab + x;
	report_error(2, "Symbol table overflow --try -As option");
}

/* symtab_alloc 0x6ffe..0x702a: done */
symtab_alloc(a)
int a;
{
	symtab = calloc(a, 16);
	if (!symtab)
		report_error(2, "malloc unable to allocate enough memory for symbol table");
}

/* resize_symtab 0x702a..0x705e: done */
resize_symtab(a)
int a;
{
	symtab = realloc(symtab, a << 4);
	if (!symtab)
		report_error(2, "realloc unable to allocate enough memory for symbol table");
}

/* next_entry 0x705e..0x70ce: done */
struct symbol *next_entry(p)
register struct symbol *p;
{
	register int b;
	register char *e;

	b = p->sname[0];
	if (b & 0x80) {
		e = p->sname + (b & 0x7f);
		p++;
		while (e > (char *)p)
			p++;
	} else
		p++;
	if (p >= symtab && p < symtab + nsym)
		return p;
	report_error(2, "Error in stepsym");
}

/* index16 0x70ce..0x7112: done */
int index16(x)
struct symbol *x;
{
	int spare;	/* unused: the frame of the original has 2 more bytes */

	if (symtab <= x && symtab + nsym > x)
		return ((int)x - (int)symtab) >> 4;
	report_error(2, "Error in getindex");
}

/* ensure_buf..7186: done */
long ensure_buf(n)
unsigned long n;
{
	if (rlsize < n) {
		if (n > 0xffff)
			report_error(2, "ranlib table of contents too large");
		if (rlsize != 0)
			free(rlbuf);
		rlbuf = calloc(1, (int)n);
		if (rlbuf == 0)
			report_error(2, "malloc unable to allocate enough memory for ranlib contents");
		rlsize = (unsigned)n;
	}
	return rlbuf;
}

/* limit_msg..719e: done */
limit_msg()
{
	printf("%d symbols / %d available\n", symindex, 0x0ffa);
}

/* table32_entry 0x719e..0x71ca: done */
struct p2seg *table32_entry(x)
unsigned x;
{
	if (x < 0x0100)
		return p2tab + x;
	report_error(2, "Pass 2 segment table overflow");
}
