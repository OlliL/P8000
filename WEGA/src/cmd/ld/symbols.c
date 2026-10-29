#include <stdio.h>
#include "ld.h"

struct symbol cursym;
int v067c;
f2bf6()
{
	v067c++;
}

f2bfc()
{
	v067c = 0;
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

/* lookup_name: look up a name in the hash table of symbols (0x25dc..0x26ac of /bin/ld, finding: register
   variables start and end are in the frame here) */
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
