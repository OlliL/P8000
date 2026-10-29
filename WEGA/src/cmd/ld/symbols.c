#include <stdio.h>
#include "ld.h"

struct symbol cursym;
int v067c;

/* read_entry_hdr: read the entry of a symbol into cursym; a long name takes the continuation entries (sld read_entry_hdr,
   the nonsegmented part; 0x249a..0x2502 of /bin/ld). Module: printname.c as assigned (sld has it in symbols.c) */
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
		swap_pair(&cursym);
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

slookup(x)
int x;
{
	int spare;

	pack_name(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

pslookup(x)
int x;
{
	int spare;

	unpack_run(x, cursym.sname);
	cursym.stype = ' ';
	cursym.svalue = 0;
	lookup_name();
}

/* enter: enter a symbol into the symbol table (sld enter; 0x26e8..0x2766 of /bin/ld; finding: the store *a = r is
   loaded through r2 in the original; the table size is 0x0923) */
enter(a)
struct symbol **a;
{
	int spare;
	register struct symbol *r;

	if (*a == 0) {
		if (symindex >= 0x0923)
			report_error(2, "Symbol table overflow");
		symhash[symindex] = a;
		r = table16_entry(symindex++);
		lastsym = r;
		*a = r;
		symindex = unpack_run(cursym.sname, r->sname) + symindex;
		r->stype = cursym.stype;
		r->svalue = cursym.svalue;
		return 1;
	}
	lastsym = *a;
	return 0;
}

/* symreloc: relocate the value of cursym by the text, data or bss relocation of the object (sld symreloc; 0x2766..0x27be of /bin/ld) */
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

/* put_sym: write a symbol entry (and its continuation entries of a long name) to the output (0x27be..0x282e of /bin/ld; finding) */
put_sym(a, b)
struct buf *a;
struct symbol *b;
{
	int spare1, spare2;
	char cnt;
	register struct symbol *p;
	register int k;

	p = b;
	if (swapflg)
		swap_pair(b);
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

/* put_all: write all symbols of the table to the output (0x282e..0x2862 of /bin/ld) */
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

/* ldrsym: load a symbol of a relocation entry, error for multiply defined (sld ldrsym; 0x2502..0x25a2 of /bin/ld).
   The parameter x is not used here; the order of the parameters follows the frame of the original */
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

/* mkfsym: enter the file symbol of an object into the output (sld mkfsym; 0x25a2..0x25dc of /bin/ld). svalue is the word at
   0x6bf8 here (sld: 0) */
mkfsym(p)
char *p;
{
	if (sflag)
		return;
	if (xflag)
		return;
	unpack_run(p, cursym.sname);
	cursym.stype = 0x1f;
	cursym.svalue = (unsigned)v6bf8;
	put_sym(&soutb, &cursym);
}

/* lookup_name: look up a name in the hash table of symbols (0x25dc..0x26ac of /bin/ld, finding: register
   variables start and end are in the frame here) */

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

/* next_entry: next entry of the symbol table (a long name occupies several entries) (0x2aa6..0x2b04 of /bin/ld; finding) */
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

/* warn_sym_redef: warn that a symbol is redefined in an archive (0x2862..0x294e of /bin/ld). The (char) casts give the
   sign extension of the original, whose stdio had char *_ptr (the header of src/head has unsigned char *) */

/* index16: index of an entry of the symbol table (sld index16; 0x2b04..0x2b3a of /bin/ld). ld has a static part of 0x258
   entries at 0x1102 in front of the table at symtab (14 byte entries); v069c is the end of the static part (name open) */
index16(x)
struct symbol *x;
{
	int spare;

	if (x < v069c)
		return ((int)x - 0x1102) / 14;
	return ((int)x - (int)symtab) / 14 + 0x258;
}

f2bf6()
{
	v067c++;
}

f2bfc()
{
	v067c = 0;
}

/* limit_msg: print the size usage "n symbols / m available" (sld limit_msg; 0x2c02..0x2c20 of /bin/ld). The available count is
   0x0923 when the symbol table is allocated, else 0x0258 */
limit_msg()
{
	printf("%d symbols / %d available\n", symindex, symtab ? 0x0923 : 0x0258);
}

/* f2c20: grow the heap by x bytes and return the old break, or -1 when the break moved (no sld counterpart; calls the entry 0x484a of the
   shipped brk member, presumably sbrk; 0x2c20..0x2c4c of /bin/ld). Used by table16_entry (f29d6) */
f2c20(x)
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
