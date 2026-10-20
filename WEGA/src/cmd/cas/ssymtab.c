static char sccsid[] = "@[$]ssymtab.c\t1.3 05/06/83 13:26:37 - 87wega3.2";

/*
 * ssymtab - the symbol tables of the user symbols
 *
 * They live in three segments that grow by ssgbrk: the symbol table (segment 7, the
 * entries of struct sym, addressed by their offset), the information table (8, the
 * values) and the name table (9).  The first 0x30 bytes of each segment are not
 * used, and an index is the offset in the segment less 0x30, so that 0 means
 * "no symbol".  hashtab holds the first symbol of the chain of each hash value, blkstack
 * the list of the local symbols of each block level.
 */
#include "cas.h"

#define STAB	0x87000030L
#define ITAB	0x88000030L
#define NTAB	0x89000030L

int blklevel = 1;			/* nesting level of { } */
unsigned stoff = 2;			/* used part of the segments, and their size */
unsigned stlim = 0x400;
unsigned itoff = 2;
unsigned itlim = 0x400;
unsigned ntoff = 2;
unsigned nmlim = 0x400;

unsigned blkstack[10];
unsigned hashtab[1000];

/* room for n bytes in the symbol table */
struct sym *
stalloc(n)
int n;
{
	register struct sym *p;
	register int seg;
	char *base;

	if (stoff + n > stlim) {
		base = (char *)STAB;
		seg = *(char *)&base & 0xF;
		if (ssgbrk(seg, 0x400) == -1L) {
			perror("symtab");
			done(-1);
		}
		stlim += 0x400;
	}
	p = (struct sym *)STAB;
	p = (struct sym *)((char *)p + stoff);
	stoff += n;
	return (p);
}

/* room for n bytes in the information table */
struct sym *
italloc(n)
int n;
{
	register struct sym *p;
	register int seg;
	char *base;

	if (itoff + n > itlim) {
		base = (char *)ITAB;
		seg = *(char *)&base & 0xF;
		if (ssgbrk(seg, 0x400) == -1L) {
			perror("infotab");
			done(-1);
		}
		itlim += 0x400;
	}
	p = (struct sym *)ITAB;
	p = (struct sym *)((char *)p + itoff);
	itoff += n;
	return (p);
}

/* a copy of the string in the name table */
char *
namalloc(s)
char *s;
{
	register char *p;
	register int seg;
	char *base;

	if (strlen(s) + ntoff + 1 > nmlim) {
		base = (char *)NTAB;
		seg = *(char *)&base & 0xF;
		if (ssgbrk(seg, 0x400) == -1L) {
			perror("namtab");
			done(-1);
		}
		nmlim += 0x400;
	}
	p = (char *)NTAB;
	p += ntoff;
	strcpy(p, s);
	ntoff += strlen(s) + 1;
	return (p);
}

/* the entry of a symbol table index */
struct sym *
stabp(idx)
int idx;
{
	register char *p;

	if (idx) {
		p = (char *)STAB;
		return ((struct sym *)(p + idx));
	}
	return (0);
}

/* the entry of the information table */
struct sym *
itabp(idx)
int idx;
{
	register char *p;

	if (idx) {
		p = (char *)ITAB;
		return ((struct sym *)(p + idx));
	}
	return (0);
}

/* enter a name that is not in the table yet; returns the index of the new entry */
stenter(name, mode)
char *name;
unsigned char mode;
{
	register int h;
	register struct sym *p;
	register struct sym *p2;

	if (*name) {
		h = hash(1000, name);
		if (hashtab[h] == 0L && (mode & 6) != 6) {
			p = stalloc(10);
			p->s_2 = (long)namalloc(name);
			p->s_type = 0;
			p->s_7 = 0;
			p->s_8 = 0;
			hashtab[h] = (int)p - 0x30;
			return (hashtab[h]);
		}
		p2 = stalloc(16);
		p2->s_2 = (long)namalloc(name);
		p2->s_type = 0;
		p2->s_8 = 0;
		if (hashtab[h] != 0L) {
			p2->s_14 = hashtab[h];
			p2->s_7 = 8;
		}
		if ((mode & 6) == 6) {
			p2->s_10 = blkstack[blklevel - 1];
			blkstack[blklevel - 1] = (int)p2 - 0x30;
			p2->s_7 = p2->s_7 | 6;
			p2->s_12 = blklevel;
		}
		hashtab[h] = (int)p2 - 0x30;
		return (hashtab[h]);
	}
	p = stalloc(10);
	p->s_2 = 0;
	p->s_type = 0;
	p->s_7 = 0;
	p->s_8 = 0;
	return ((int)p - 0x30);
}

/* set the type and the kind of a symbol and give it an entry in the information table */
setsym(idx, type, kind)
int idx;
unsigned char type;
unsigned char kind;
{
	register struct sym *sp;

	sp = stabp(idx);
	sp->s_type = type;
	sp->s_7 = sp->s_7 | kind;
	switch (type) {
	case 1:
	case 3:
	case 4:
		sp->s_8 = (int)italloc(6) - 0x30;
		break;
	case 2:
		sp->s_8 = (int)italloc(12) - 0x30;
		break;
	}
}

/* the index of the symbol with this name, or 0 */
stlook(name, mode)
char *name;
unsigned char mode;
{
	register int i;
	register int h;
	register struct sym *p;
	register struct sym *q;

	h = hash(1000, name);
	i = hashtab[h];
	while ((p = stabp(i)) != 0) {
		if (strcmp(name, (char *)p->s_2) == 0)
			return (i);
		if ((p->s_7 & 8) == 8) {
			q = p;
			i = q->s_14;
		} else
			i = 0;
	}
	return (0);
}

/* { */
blkopen()
{
	if (blklevel == 10)
		error(2, "Nesting too deep\n");
	else
		blklevel++;
}

/* }: the local symbols of the block are taken out of the hash chains */
blkclose()
{
	register struct sym *sp;
	register struct sym *q;
	register int h;
	register int i;
	unsigned n;
	unsigned a;
	unsigned b;

	blklevel--;
	if (blkstack[blklevel] != 0L) {
		n = blkstack[blklevel];
		blkstack[blklevel] = 0;
		while (n != 0L) {
			sp = stabp(n);
			a = 0;
			b = 0;
			if ((sp->s_7 & 6) == 6) {
				q = sp;
				a = q->s_10;
				sp->s_10 = 0;
			}
			if ((sp->s_7 & 8) == 8) {
				q = sp;
				b = q->s_14;
				sp->s_14 = 0;
			}
			h = hash(1000, (char *)sp->s_2);
			i = hashtab[h];
			if (i == n)
				hashtab[h] = b;
			else {
				while (i != n) {
					q = stabp(i);
					i = q->s_14;
				}
				q->s_14 = b;
			}
			n = a;
		}
	}
}
