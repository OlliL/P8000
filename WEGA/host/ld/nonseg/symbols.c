#include <stdio.h>

static char sccsid[] __attribute__((unused)) = "@[$]symbols.c\t1.8  03/11/83 10:29:38 - 87wega3.2";

#include "ld.h"

uint16_t rlsize = 0;
char *rlbuf = 0;

int16_t rlbusy = 0;	/* the ranlib buffer is in use (lock_rlbuf) */

/* read_entry_hdr: read the next symbol entry of the text stream into cursym (a long name takes its continuation
   entries); the number of continuation entries (sld read_entry_hdr; 0x249a..0x2502 of /bin/ld) */
int16_t read_entry_hdr(void)
{
	char cnt;
	register int16_t k;
	register char *cur;
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
void ldrsym(struct symbol *asp, int32_t v, char c)
{
	register struct symbol *sp;

	sp = asp;
	if (!sp)
		return;
	if (sp->stype != 0x20 || SVALUE(sp) != 0) {
		if (put_name(sp->sname) < 8)
			putc(9, stderr);
		putc(0x20, stderr);
		error(1, "-- Multiply defined");
		return;
	}
	sp->stype = c;
	SETSV(sp, v);
}

/* mkfsym: write the file symbol of an object to the symbol output; its value is the text base torigin (sld: 0) (sld
   mkfsym, Plexus mkfsym; 0x25a2..0x25dc of /bin/ld) */
void mkfsym(char *s)
{
	if (sflag)
		return;
	if (xflag)
		return;
	unpack_run(s, cursym.sname);
	cursym.stype = 0x1f;
	SETSV(&cursym, (uint16_t)torigin);
	put_sym(&soutb, &cursym);
}

/* lookup_name: find the hash slot of cursym in the symbol hash table (an empty slot when the name is not there) (sld
   lookup_name, Plexus lookup; 0x25dc..0x26ac of /bin/ld) */
struct symbol **lookup_name(void)
{
	int16_t h;
	int16_t flag;
	register struct symbol **slot;
	register char *cur;
	register char *rec;
	register char *start;
	register char *end;

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
		if (slot >= hshtab + 2341)	/* the end of hshtab (the table `local` follows it in memory) */
			slot = hshtab;
	}
	return slot;
}

/* slookup: hash slot of the C string x (the result is the one of lookup_name, there is no return statement) (sld
   slookup, Plexus slookup; 0x26ac..0x26ca of /bin/ld) */
struct symbol **slookup(char *x)
{

	cp8c(x, cursym.sname);
	cursym.stype = ' ';
	SETSV(&cursym, 0);
	return lookup_name();
}

/* pslookup: hash slot of a name in table form (ranlib entries; the result is the one of lookup_name, there is no
   return statement) (sld pslookup; 0x26ca..0x26e8 of /bin/ld) */
struct symbol **pslookup(char *x)
{

	unpack_run(x, cursym.sname);
	cursym.stype = ' ';
	SETSV(&cursym, 0);
	return lookup_name();
}

/* enter: enter cursym into the empty hash slot a: a new entry of the symbol table; 1 when it was entered, 0 when the
   slot was in use (lastsym is the symbol) (sld enter, Plexus enter; 0x26e8..0x2766 of /bin/ld) */
int16_t enter(struct symbol **hp)
{
	register struct symbol *sp;

	if (*hp == 0) {
		if (symindex >= 0x0923)
			error(2, "Symbol table overflow");
		symhash[symindex] = hp;
		*hp = lastsym = sp = table16_entry(symindex++);
		symindex = unpack_run(cursym.sname, sp->sname) + symindex;
		sp->stype = cursym.stype;
		SETSV(sp, SVALUE(&cursym));
		return 1;
	}
	lastsym = *hp;
	return 0;
}

/* symreloc: relocate the value of cursym by the text, data or bss base of the object (sld symreloc, Plexus symreloc;
   0x2766..0x27be of /bin/ld) */
void symreloc(void)
{
	switch (cursym.stype) {
	case 2:
	case 0x22:
		SETSV(&cursym, (uint16_t)((uint16_t)SVALUE(&cursym) + ctrel));
		return;
	case 3:
	case 0x23:
		SETSV(&cursym, (uint16_t)((uint16_t)SVALUE(&cursym) + cdrel));
		return;
	case 4:
	case 0x24:
		SETSV(&cursym, (uint16_t)((uint16_t)SVALUE(&cursym) + cbrel));
		return;
	case 0x20:
		return;
	}
	if (cursym.stype & 0x20)
		cursym.stype = 0x21;
}

/* put_sym: write the symbol b (and the continuation entries of a long name) to the buffer a; the number of entries
   (sld put_sym; 0x27be..0x282e of /bin/ld) */
int16_t put_sym(struct buf *a, struct symbol *b)
{
	register struct symbol *p;
	char cnt;
	register int16_t k;

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
void put_all(void)
{
	register struct symbol *p;
	register struct symbol *end;

	p = table16_entry(0);
	end = table16_entry(symindex);
	while (entry_lt(p, end)) {
		put_sym(&toutb, p);
		p = next_entry(p);
	}
}

/* warn_sym_redef: warning that a symbol of an archive member is defined already (the archive symbol is ignored). The
   (char) casts of the putc forms give the sign extension of the original stdio (char *_ptr) (sld warn_sym_redef;
   0x2862..0x294e of /bin/ld) */
void warn_sym_redef(struct symbol *p, struct symbol *a)
{
	char c;
	register char *s;

	if (wflag)
		return;
	if (p != 0) {
		c = p->stype;
		if ((!c) & 0x20 || c == 0x20)
			return;
	}
	c = a->stype;
	if ((!c) & 0x20 || c == 0x20)
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
		putc(9, stderr);
	putc(0x20, stderr);
	error(0, "- warning: symbol redefined in archive");
}

/* chk_ranlib: check the ranlib table of n entries at p against the defined symbols and warn about redefinitions (sld
   chk_ranlib; 0x294e..0x29d6 of /bin/ld) */
void chk_ranlib(struct rlent *p, int16_t n)
{
	struct rlent *e;
	char *q;
	struct symbol *q2;

	for (q = (char *)p, e = p + n; p < e; p++) {
		if ((char *)p >= q) {
			if ((p->rname[0] & 0x80) != 0)
				q = p->rname + (p->rname[0] & 0x7f);
			if ((q2 = *pslookup(p->rname)) != 0) {
				if (q2->stype != 0x20)
					warn_sym_redef(0, q2);
			}
		}
	}
}

/* symtab and nsym follow the jump table of symreloc in the data of the original (0x069a, 0x069c) */
struct symbol *symtab = 0;
struct symbol *nsym = sym16 + 600;

/* table16_entry: address of entry x of the symbol table: below 600 in the static table sym16, else in symtab, which is
   taken on the first use (sld table16_entry; 0x29d6..0x2aa6 of /bin/ld). The original takes it from the heap (the break of
   the process, 64 KB in all) or from the free ranlib buffer and stops with "Symbol table overflow" when the heap is
   full; the host takes it from malloc. */
struct symbol *table16_entry(uint16_t x)
{
	if (x < 0x258)
		return sym16 + x;
	if (x > 0x923)
		error(2, "Symbol table overflow");
	if (symtab == 0) {
		/* 64 entries more: the continuation entries of a long name behind the last symbol */
		symtab = calloc(0x923 - 0x258 + 64, sizeof (struct symbol));
		if (symtab == 0)
			error(2, "Symbol table overflow");
	}
	return symtab + (x - 0x258);
}

/* the order of two entries of the symbol table: the static part sym16 and symtab follow each other in the original (addresses),
   here they are two arrays */
static int32_t entry_pos(struct symbol *p)
{
	if (symtab != 0 && p >= symtab && p < symtab + 0x923 - 0x258 + 64)
		return 0x258 + (p - symtab);
	return p - sym16;
}

int entry_lt(struct symbol *a, struct symbol *b)
{
	return entry_pos(a) < entry_pos(b);
}

/* next_entry: the entry after p in the symbol table (a long name takes several entries); past the static table it
   continues in symtab (sld next_entry; 0x2aa6..0x2b04 of /bin/ld) */
struct symbol *next_entry(struct symbol *p)
{
	register int16_t b;
	register char *e;
	register char *q;
	int32_t pos;

	b = p->sname[0];
	pos = entry_pos(p);
	if (b & 0x80) {
		e = p->sname + (b & 0x7f);
		q = (char *)p;
		do {
			q += sizeof (struct symbol);
			pos++;
		} while (e > q);
	} else
		pos++;
	if (pos < 0x258)
		return sym16 + pos;
	return symtab + (pos - 0x258);
}

/* index16: index of the entry x in the symbol table (the static part has 600 entries) (sld index16; 0x2b04..0x2b3a of
   /bin/ld) */
int16_t index16(struct symbol *x)
{

	return (int16_t)entry_pos(x);
}

/* ensure_buf: make the ranlib buffer at least n bytes large (the heap of the original: grow_heap/grow_rlbuf; realloc here); the
   buffer (sld ensure_buf; 0x2b3a..0x2b92 of /bin/ld) */
char *ensure_buf(uint32_t n)
{
	if (rlsize < n) {
		if (n > 0xffff)
			error(2, "ranlib table of contents too large");
		rlbuf = realloc(rlbuf, n);
		if (rlbuf == 0)
			error(2, "table of contents overflow");
		rlsize = n;
	}
	return rlbuf;
}

/* lock_rlbuf: mark the ranlib buffer as in use while a ranlib table is read (load1arg); table16_entry does not
   take it over for the symbol table then (rlbusy counts) (no sld counterpart; 0x2bf6..0x2bfc of /bin/ld) */
void lock_rlbuf(void)
{
	rlbusy++;
}

/* unlock_rlbuf: the ranlib buffer is free again (see lock_rlbuf) (no sld counterpart; 0x2bfc..0x2c02 of /bin/ld) */
void unlock_rlbuf(void)
{
	rlbusy = 0;
}

/* limit_msg: print the number of symbols and the number available (-T) (sld limit_msg; 0x2c02..0x2c20 of /bin/ld) */
void limit_msg(void)
{
	printf("%d symbols / %d available\n", symindex, symtab ? 0x0923 : 0x0258);
}
