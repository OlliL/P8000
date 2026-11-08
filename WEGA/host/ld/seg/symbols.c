/*
 * symbols.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include "ld.h"

static char sccsid[] __attribute__((unused)) = "@[$]symbols.c\t3.2  10/27/83 12:25:22 - 87wega3.2";
struct fsym fsym = {{0, 0, 0, 0, 0, 0, 0, 0}, 0, 0};
int16_t unused15d8 = 0;
uint16_t rlsize = 0;

/* read_entry_hdr: read the header of an entry of the symbol table (0x6684..0x6800 of /bin/sld) */
int16_t read_entry_hdr(void)
{
	register int16_t i;
	register int16_t result;
	register char *cur;
	char cnt;
	char *end;
	uint8_t r[14];	/* a symbol of a segmented object: ssval (4), sstyp, ssseg, ssnam (8) */
	uint8_t raw[12];
	char *q;
	char *base;

	result = 0;
	if (segobj) {
		mget(r, 14);
		if (swapflg)
			swap_pair(r);
		cursym.svalue = (int32_t)rd32(r);
		cursym.stype = (char)r[4];
		cursym.sseg = (signed char)r[5];
		for (i = 0; i < 8; i++)
			cursym.sname[i] = (char)r[6 + i];
		if (cursym.sname[0] & 0x80) {
			cnt = cursym.sname[0] & 0x7f;
			cur = (char *) r;
			end = (char *) r + 6 + cnt;
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
			result = (int16_t)((q - base) / 16);
			if ((q - base) % 16 != 0)
				result++;
		}
	} else {
		mget(raw, 12);
		memcpy(fsym.fname, raw, 8);
		fsym.ftype = (int16_t)rd16(raw + 8);
		fsym.fvalue = rd16(raw + 10);
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

/* cvt_fsym: convert a file symbol (fsym) to a symbol; the segment flag is set for data and bss symbols when a is set (0x6800..0x6866 of /bin/sld) */
void cvt_fsym(struct fsym *p, struct symbol *q, int16_t a)
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

/* pack_entry: pack an entry of the symbol table (never called) (0x6866..0x68a2 of /bin/sld) */
void pack_entry(struct symbol *p, struct fsym *q)
{
	q->fvalue = (int16_t)p->svalue;
	q->ftype = p->stype;
	trunc_name(p->sname, (char *)q);
}

/* ldrsym: load a symbol of a relocation entry, error for multiply defined (0x68a2..0x6a08 of /bin/sld) */
void ldrsym(struct symbol *a, char c)
{
	register struct symbol *p;

	p = a;
	if (!p)
		return;
	if (p->stype != 0x20 || p->svalue != 0) {
		if (put_name(p->sname) < 8)
			putchar(9);
		putchar(0x20);
		error(1, "-- Multiply defined");
		return;
	}
	p->stype = c;
	p->svalue = 0;
	p->sseg = 0;
	if (c == 0x22) {
		if (!lastt)
			return;
		p->svalue = (uint16_t)lastt->s_code + ((int32_t)(lastt->segno) << 24);
		p->sseg = lastt->segidx;
		return;
	}
	if (c == 0x23) {
		if (!lastd)
			return;
		p->svalue = (uint16_t)(lastd->s_code + lastd->s_data);
		p->svalue = ((int32_t)(lastd->segno) << 24) + p->svalue;
		p->sseg = lastd->segidx;
		return;
	}
	if (c == 0x24) {
		if (!lastb)
			return;
		p->svalue = (uint16_t)(lastb->s_code + lastb->s_data + lastb->s_bss);
		p->svalue = ((int32_t)(lastb->segno) << 24) + p->svalue;
		p->sseg = lastb->segidx;
		return;
	}
	if (c != 0x21)
		return;
	p->svalue = 0;
	if (stksegn == -1 || stksegn == -3)
		p->sseg = maxseg;
	else
		p->sseg = stksegn;
	p->svalue = ((int32_t)p->sseg << 24) + p->svalue;
}

/* mkfsym: make the file symbol of a symbol (0x6a08..0x6a58 of /bin/sld) */
void mkfsym(char *s)
{
	if (sflag)
		return;
	if (xflag)
		return;
	unpack_run(s, cursym.sname);
	cursym.stype = 0x1f;
	cursym.svalue = 0;
	cursym.sseg = 0;
	map_sym();
	put_sym(&soutb, &cursym);
}

/* lookup_name: look up a name in the hash table of symbols (0x6a58..0x6b5a of /bin/sld) */
struct symbol **lookup_name(void)
{
	register struct symbol **slot;
	register char *cur;
	register char *rec;
	int16_t h;
	int16_t flag;
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
		if (slot >= hshtab + 4092)	/* the end of hshtab (the table `local` follows it in memory) */
			slot = hshtab;
	}
	return slot;
}

/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
/* slookup: look up a symbol by name (packs the name first) (0x6b5a..0x6b84 of /bin/sld) */
struct symbol **slookup(char *p)
{

	cp8c(p, cursym.sname);
	cursym.stype = 0x20;
	cursym.svalue = 0;
	return lookup_name();
}

/* frame: the original reserves 8 bytes; the unused local dummy reproduces that (role unknown) */
/* pslookup: look up a symbol by a name already packed (0x6b84..0x6bae of /bin/sld) */
struct symbol **pslookup(char *p)
{

	unpack_run(p, cursym.sname);
	cursym.stype = 0x20;
	cursym.svalue = 0;
	return lookup_name();
}

/* enter: enter a symbol into the symbol table (0x6bae..0x6c6a of /bin/sld) */
int16_t enter(struct symbol **hp)
{
	register struct symbol *sp;

	if (*hp == 0) {
		if (symindex >= 0x0ffa)
			error(2, "Symbol table overflow");
		symhash[symindex] = hp;
		*hp = lastsym = sp = table16_entry(symindex++);
		symindex = unpack_run(cursym.sname, sp->sname) + symindex;
		sp->stype = cursym.stype;
		sp->svalue = cursym.svalue;
		sp->sseg = cursym.sseg;
		return 1;
	}
	lastsym = *hp;
	return 0;
}

/* map_sym: map a symbol of the object to its entry in the symbol table (0x6c6a..0x6da8 of /bin/sld) */
void map_sym(void)
{
	register struct p1seg *p;
	int16_t i;

	p = get_block(i = fblk + cursym.sseg);
	/* the value is a long: the segment number in the high word, the offset in the low word */
	switch (cursym.stype) {
	case 2:
	case 0x22:
	text:
		cursym.svalue = (int32_t)((uint32_t)(uint16_t)(p->ncode->segno << 8) << 16 | (uint16_t)(p->ctrel + p->ncoff + p->ncode->torig + (int16_t)cursym.svalue));
		cursym.sseg = p->ncode->segidx;
		return;
	case 3:
	case 0x23:
		cursym.svalue = (int32_t)((uint32_t)(uint16_t)(p->ndata->segno << 8) << 16 | (uint16_t)(p->cdrel + p->ndoff + p->ndata->dorig + (int16_t)cursym.svalue));
		cursym.sseg = p->ndata->segidx;
		return;
	case 4:
	case 0x24:
		cursym.svalue = (int32_t)((uint32_t)(uint16_t)(p->nbss->segno << 8) << 16 | (uint16_t)(p->cbrel + p->nboff + p->nbss->borig + (int16_t)cursym.svalue));
		cursym.sseg = p->nbss->segidx;
		return;
	case 0x1f:
		for (; i < nblk; i++) {
			p = get_block(i);
			if (p->s_code)
				goto text;
		}
		cursym.sseg = 0;
		cursym.svalue = 0;
		return;
	case 0x20:
		return;
	}
	if (cursym.stype & 0x20)
		cursym.stype = 0x21;
}


/* symreloc: relocate the value of cursym by the text, data or bss relocation of the object (0x6da8..0x6e1a of /bin/sld) */
void symreloc(void)
{
	switch (cursym.stype) {
	case 2:
	case 0x22:
		cursym.svalue = (uint16_t)((uint16_t)cursym.svalue + ctrel);
		return;
	case 3:
	case 0x23:
		cursym.svalue = (uint16_t)((uint16_t)cursym.svalue + cdrel);
		return;
	case 4:
	case 0x24:
		cursym.svalue = (uint16_t)((uint16_t)cursym.svalue + cbrel);
		return;
	case 0x20:
		return;
	}
	if (cursym.stype & 0x20)
		cursym.stype = 0x21;
}

/* put_sym: write one symbol to the symbol output (0x6e1a..0x6efe of /bin/sld) */
int16_t put_sym(struct buf *a, struct symbol *b)
{
	char cnt;
	uint8_t r[14];
	register char *q;
	register int16_t k;
	register int16_t i;

	wr32(r, (uint32_t)b->svalue);
	r[4] = b->stype;
	r[5] = (uint8_t)b->sseg;
	for (i = 0; i < 8; i++)
		r[6 + i] = b->sname[i];
	if (swapflg)
		swap_pair(r);
	mput(a, r, 14);
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

/* put_all: write all symbols of the table (0x6efe..0x6f46 of /bin/sld) */
void put_all(void)
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

/* chk_ranlib: check an archive member against the ranlib table (0x6f46..0x6fd0 of /bin/sld) */
void chk_ranlib(struct rlent *p, int16_t n)
{
	struct rlent *e;
	char *q;
	struct symbol *q2;

	q = (char *)p;
	e = p + n;
	goto test;
body:
	if ((char *)p >= q) {
		if ((p->rname[0] & 0x80) != 0)
			q = p->rname + (p->rname[0] & 0x7f);
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

/* table16_entry: address of entry x of the symbol table, error beyond nsym (0x6fd0..0x6ffe of /bin/sld) */
struct symbol *table16_entry(uint16_t x)
{

	if (x < nsym)
		return symtab + x;
	error(2, "Symbol table overflow --try -As option");
	return 0;
}

/* symtab_alloc: allocate the symbol table (0x6ffe..0x702a of /bin/sld) */
void symtab_alloc(int16_t a)
{
	symtab = (struct symbol *) calloc(a, 16);
	if (!symtab)
		error(2, "malloc unable to allocate enough memory for symbol table");
}

/* resize_symtab: change the size of the symbol table (0x702a..0x705e of /bin/sld) */
void resize_symtab(int16_t a)
{
	symtab = (struct symbol *) realloc(symtab, a << 4);
	if (!symtab)
		error(2, "realloc unable to allocate enough memory for symbol table");
}

/* next_entry: next entry of the symbol table (a long name occupies several entries) (0x705e..0x70ce of /bin/sld) */
struct symbol *next_entry(struct symbol *p)
{
	register int16_t b;
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
	error(2, "Error in stepsym");
	return 0;
}

/* index16: index of an entry of the symbol table (byte offset / 16) (0x70ce..0x7112 of /bin/sld) */
int16_t index16(struct symbol *x)
{

	if (symtab <= x && symtab + nsym > x)
		return (int16_t)(x - symtab);
	error(2, "Error in getindex");
	return 0;
}

/* ensure_buf: make sure the ranlib buffer is large enough (0x7112..0x7186 of /bin/sld) */
char *ensure_buf(uint32_t n)
{
	if (rlsize < n) {
		if (n > 0xffff)
			error(2, "ranlib table of contents too large");
		if (rlsize != 0)
			free(rlbuf);
		rlbuf = calloc(1, (uint16_t)n);
		if (rlbuf == 0)
			error(2, "malloc unable to allocate enough memory for ranlib contents");
		rlsize = (uint16_t)n;
	}
	return rlbuf;
}

/* limit_msg: print the size usage "n symbols / m available" (0x7186..0x719e of /bin/sld) */
void limit_msg(void)
{
	printf("%d symbols / %d available\n", symindex, 0x0ffa);
}

/* table32_entry: address of an entry of the pass-two segment table (0x719e..0x71ca of /bin/sld) */
struct p2seg *table32_entry(uint16_t x)
{
	if (x < 0x0100)
		return p2tab + x;
	error(2, "Pass 2 segment table overflow");
	return 0;
}
