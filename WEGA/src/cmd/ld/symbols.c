#include <stdio.h>

static char sccsid[] = "@[$]symbols.c\t1.8  03/11/83 10:29:38 - 87wega3.2";

#include "ld.h"

unsigned int rlsize = 0;
char *rlbuf = 0;

int rlbusy = 0;	/* the ranlib buffer is in use (lock_rlbuf) */

/* read_entry_hdr: read the next symbol entry of the text stream into cursym (a long name takes its continuation
   entries); the number of continuation entries (sld read_entry_hdr; 0x249a..0x2502 of /bin/ld) */
int read_entry_hdr()
{
	register int k;
	register char *cur;
	char cnt;
	int spare2, spare3;
	char *end;

	k = 0;
	mget(&cursym, 14);
	if (swapflg)
		longswap(&cursym);
	if (cursym.sname[0] & 0x80) {
		cnt = cursym.sname[0] & 0x7f;
		cur = (char *)&cursym;
		end = cursym.sname + cnt;
		goto test;
body:
		k++;
		mget(cur, 14);
test:
		cur += 14;
		if (cur < end)
			goto body;
	}
	return k;
}

/* ldrsym: define the linker symbol p with value v and type c; "Multiply defined" when it is defined already. x is not
   used and not passed by the callers; the parameter order follows the frame of the original (sld ldrsym, Plexus
   ldrsym; 0x2502..0x25a2 of /bin/ld) */
ldrsym(a, x, v, c)
struct symbol *a;
int x;
long v;
char c;
{
	int spare;

	register struct symbol *p;

	p = a;
	if (!p)
		return;
	if (p->stype != 0x20 || p->svalue != 0) {
		if (put_name(p->sname) < 8)
			(--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = 9) : _flsbuf((unsigned)9, stderr));
		(--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = 0x20) : _flsbuf((unsigned)0x20, stderr));
		report_error(1, "-- Multiply defined");
		return;
	}
	p->stype = c;
	p->svalue = v;
}

/* mkfsym: write the file symbol of an object to the symbol output; its value is the text base torigin (sld: 0) (sld
   mkfsym, Plexus mkfsym; 0x25a2..0x25dc of /bin/ld) */
mkfsym(p)
char *p;
{
	if (sflag)
		return;
	if (xflag)
		return;
	unpack_run(p, cursym.sname);
	cursym.stype = 0x1f;
	cursym.svalue = (unsigned)torigin;
	put_sym(&soutb, &cursym);
}

/* lookup_name: find the hash slot of cursym in the symbol hash table (an empty slot when the name is not there) (sld
   lookup_name, Plexus lookup; 0x25dc..0x26ac of /bin/ld) */
struct symbol **lookup_name()
{
	register struct symbol **slot;
	register char *cur;
	register char *rec;
	register char *start;
	register char *end;
	int h;
	int flag;
	int spare1, spare2, spare3, spare4, spare5;

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
	slot = &hshtab[(h & 0x7fff) % 0x0923 + 2];
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

/* slookup: hash slot of the C string x (the result is the one of lookup_name, there is no return statement) (sld
   slookup, Plexus slookup; 0x26ac..0x26ca of /bin/ld) */
struct symbol **slookup(x)
int x;
{
	int spare;

	pack_name(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

/* pslookup: hash slot of a name in table form (ranlib entries; the result is the one of lookup_name, there is no
   return statement) (sld pslookup; 0x26ca..0x26e8 of /bin/ld) */
struct symbol **pslookup(x)
int x;
{
	int spare;

	unpack_run(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

/* enter: enter cursym into the empty hash slot a: a new entry of the symbol table; 1 when it was entered, 0 when the
   slot was in use (lastsym is the symbol) (sld enter, Plexus enter; 0x26e8..0x2766 of /bin/ld) */
enter(a)
struct symbol **a;
{
	int spare;
	register struct symbol *r;

	if (*a == 0) {
		if (symindex >= 0x0923)
			report_error(2, "Symbol table overflow");
		symhash[symindex] = a;
		*a = lastsym = r = table16_entry(symindex++);
		symindex = unpack_run(cursym.sname, r->sname) + symindex;
		r->stype = cursym.stype;
		r->svalue = cursym.svalue;
		return 1;
	}
	lastsym = *a;
	return 0;
}

/* symreloc: relocate the value of cursym by the text, data or bss base of the object (sld symreloc, Plexus symreloc;
   0x2766..0x27be of /bin/ld) */
symreloc()
{
	switch (cursym.stype) {
	case 2:
	case 0x22:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fctrel);
		return;
	case 3:
	case 0x23:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fcdrel);
		return;
	case 4:
	case 0x24:
		cursym.svalue = (unsigned)((unsigned)cursym.svalue + fcbrel);
		return;
	case 0x20:
		return;
	}
	if (cursym.stype & 0x20)
		cursym.stype = 0x21;
}

/* put_sym: write the symbol b (and the continuation entries of a long name) to the buffer a; the number of entries
   (sld put_sym; 0x27be..0x282e of /bin/ld) */
put_sym(a, b)
struct buf *a;
struct symbol *b;
{
	int spare1;
	char cnt;
	int spare2;
	register struct symbol *p;
	register int k;

	p = b;
	if (swapflg)
		longswap(p);
	mput(a, p, 14);
	if (p->sname[0] & 0x80) {
		cnt = (p->sname[0] & 0x7f) - 8;
		p++;
		k = 1;
		goto test;
body:
		mput(a, p++, 14);
		k++;
		cnt -= 14;
test:
		if (cnt > 0)
			goto body;
		return k;
	}
	return 1;
}

/* put_all: write all symbols of the symbol table to the symbol output (sld put_all; 0x282e..0x2862 of /bin/ld) */
put_all()
{
	int spare1, spare2;

	register struct symbol *p;
	register struct symbol *end;

	p = table16_entry(0);
	end = table16_entry(symindex);
	while (p < end) {
		put_sym(&toutb, p);
		p = next_entry(p);
	}
}

/* warn_sym_redef: warning that a symbol of an archive member is defined already (the archive symbol is ignored). The
   (char) casts of the putc forms give the sign extension of the original stdio (char *_ptr) (sld warn_sym_redef;
   0x2862..0x294e of /bin/ld) */
warn_sym_redef(p, a)
struct symbol *p;
struct symbol *a;
{
	register char *s;
	char c;
	int n;

	if (wflag)
		return;
	if (p != 0) {
		c = p->stype;
		if (!c & 0x20 || c == 0x20)
			return;
	}
	c = a->stype;
	if (!c & 0x20 || c == 0x20)
		return;
	s = a->sname;
	if (*s & 0x80)
		s++;
	if (*s == 0x5f) {
		s++;
		if (*s == 0x5f)
			return;
	}
	if (put_name(a->sname) < 8)
		(--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = 9) : _flsbuf((unsigned)9, stderr));
	(--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = 0x20) : _flsbuf((unsigned)0x20, stderr));
	report_error(0, "- warning: symbol redefined in archive");
}

/* chk_ranlib: check the ranlib table of n entries at p against the defined symbols and warn about redefinitions (sld
   chk_ranlib; 0x294e..0x29d6 of /bin/ld) */
chk_ranlib(p, n)
struct rlent *p;
int n;
{
	int spare;
	register int r;
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
		if ((q2 = *pslookup(p->rname)) != 0)
			if (q2->stype != 0x20)
				warn_sym_redef(0, q2);
	}
	p++;
test:
	if (p < e)
		goto body;
}

/* symtab and nsym follow the jump table of symreloc in the data of the original (0x069a, 0x069c) */
struct symbol *symtab = 0;
struct symbol *nsym = sym16 + 600;

/* table16_entry: address of entry x of the symbol table: below 600 in the static table sym16, else in symtab, which is
   taken from the heap or from the free ranlib buffer on the first use (sld table16_entry; 0x29d6..0x2aa6 of /bin/ld) */
struct symbol *table16_entry(x)
unsigned x;
{
	int n;
	char *p;

	if (x < 0x258)
		return sym16 + x;
	if (x > 0x923)
		report_error(2, "Symbol table overflow");
	if (symtab == 0) {
		if (rlbusy != 0 || rlsize == 0) {
			n = sbrk(0);
			symtab = (struct symbol *)grow_heap(0x5fa6);
			if ((int)symtab != n)
				report_error(2, "Symbol table overflow");
		} else {
			n = sbrk(0);
			symtab = (struct symbol *)grow_heap(0x5fa6 - rlsize);
			if ((int)symtab != n)
				report_error(2, "Symbol table overflow");
			symtab = (struct symbol *)rlbuf;
			for (p = rlbuf; p < rlbuf + rlsize; p++)
				*p = 0;
			rlsize = 0;
		}
	}
	return symtab + (x - 0x258);
}

/* next_entry: the entry after p in the symbol table (a long name takes several entries); past the static table it
   continues in symtab (sld next_entry; 0x2aa6..0x2b04 of /bin/ld) */
struct symbol *next_entry(p)
register struct symbol *p;
{
	int spare1, spare2;
	register int b;
	register char *e;

	b = p->sname[0];
	if (b & 0x80) {
		e = p->sname + (b & 0x7f);
		while (e > (char *)++p)
			;
	} else
		p++;
	if (p < nsym || p > symtab)
		return p;
	return symtab + (p - nsym);
}

/* index16: index of the entry x in the symbol table (the static part has 600 entries) (sld index16; 0x2b04..0x2b3a of
   /bin/ld) */
index16(x)
struct symbol *x;
{
	int spare;

	if (x < nsym)
		return x - sym16;
	return ((int)x - (int)symtab) / 14 + 0x258;
}

/* ensure_buf: make the ranlib buffer at least n bytes large (from the heap on the first use, else grow_rlbuf); the
   buffer (sld ensure_buf; 0x2b3a..0x2b92 of /bin/ld) */
char *ensure_buf(n)
unsigned long n;
{
	if (rlsize < n) {
		if (rlsize == 0) {
			if (n > 0xffff)
				report_error(2, "ranlib table of contents too large");
			rlbuf = (char *)grow_heap((int)n);
			if (rlbuf == (char *)-1)
				report_error(2, "table of contents overflow");
			rlsize = n;
		} else
			return grow_rlbuf(n);
	}
	return rlbuf;
}

/* grow_rlbuf: make the ranlib buffer n bytes large: in place by grow_heap when it is the last block of the heap, else
   a new buffer through ensure_buf (no sld counterpart, sld uses realloc; 0x2b92..0x2bf6 of /bin/ld) */
char *grow_rlbuf(n)
unsigned long n;
{
	int k;

	if (n > 0xffff)
		report_error(2, "ranlib table of contents too large");
	if ((char *)sbrk(0) != rlbuf + rlsize) {
		rlsize = 0;
		ensure_buf(n);
	} else {
		k = (int)n - (int)rlsize;
		if (grow_heap(k) == -1)
			report_error(2, "table of contents overflow");
		rlsize = n;
	}
	return rlbuf;
}

/* lock_rlbuf: mark the ranlib buffer as in use while a ranlib table is read (load_lib_syms); table16_entry does not
   take it over for the symbol table then (rlbusy counts) (no sld counterpart; 0x2bf6..0x2bfc of /bin/ld) */
lock_rlbuf()
{
	rlbusy++;
}

/* unlock_rlbuf: the ranlib buffer is free again (see lock_rlbuf) (no sld counterpart; 0x2bfc..0x2c02 of /bin/ld) */
unlock_rlbuf()
{
	rlbusy = 0;
}

/* limit_msg: print the number of symbols and the number available (-T) (sld limit_msg; 0x2c02..0x2c20 of /bin/ld) */
limit_msg()
{
	printf("%d symbols / %d available\n", symindex, symtab ? 0x0923 : 0x0258);
}

/* grow_heap: grow the heap by x bytes with sbrk; the old break, or -1 when the new break is below it (the request did
   not fit) (no sld counterpart, sld uses malloc; 0x2c20..0x2c4c of /bin/ld) */
grow_heap(x)
int x;
{
	unsigned b;
	unsigned a;

	a = sbrk(x);
	b = sbrk(0);
	if (b < a)
		return -1;
	return a;
}
