/*
 * ssymtab - the symbol tables of the user symbols
 *
 * They are three byte arenas of 64 KB: the symbol table (entries of struct sym, addressed
 * by their offset), the information table (the values) and the name table. The first
 * bytes of each arena are not used, so that the index 0 means "no symbol" (the original
 * keeps them in three segments that grow by ssgbrk and starts at offset 0x30).
 * hashtab holds the first symbol of the chain of each hash value, blkstack the list of
 * the local symbols of each block level.
 */
#include <errno.h>
#include "cas.h"

#define ARENA	0x10000

static uint8_t stab[ARENA];
static uint8_t itab[ARENA];
static uint8_t ntab[ARENA];

int blklevel = 1;			/* nesting level of { } */
unsigned stoff = 2;			/* used part of the arenas */
static unsigned itoff = 2;
static unsigned ntoff = 2;

static unsigned blkstack[10];		/* [level - 1]; the level is 1 ... 10, and 0 after one } too many */
static unsigned hashtab[1000];

static void *
arenaalloc(uint8_t *base, unsigned *off, unsigned n, const char *what)
{
	void *p;

	if (*off + n > ARENA) {
		errno = ENOMEM;
		perror(what);
		done(-1);
	}
	p = base + *off;
	*off += n;
	return (p);
}

/* room for n bytes in the symbol table */
struct sym *
stalloc(int n)
{
	return (arenaalloc(stab, &stoff, n, "symtab"));
}

/* room for n bytes in the information table */
struct sym *
italloc(int n)
{
	return (arenaalloc(itab, &itoff, n, "infotab"));
}

/* a copy of the string in the name table; returns its offset */
static int32_t
namalloc(const char *s)
{
	int32_t off = ntoff;

	strcpy(arenaalloc(ntab, &ntoff, strlen(s) + 1, "namtab"), s);
	return (off);
}

/* the name of a symbol (0 for the symbols without a name) */
const char *
symname(struct sym *sp)
{
	return (sp->s_2 ? (const char *)ntab + sp->s_2 : 0);
}

/* the entry of a symbol table index */
struct sym *
stabp(int idx)
{
	return ((uint16_t)idx ? (struct sym *)(stab + (uint16_t)idx) : 0);
}

/* the entry of the information table */
struct sym *
itabp(int idx)
{
	return ((uint16_t)idx ? (struct sym *)(itab + (uint16_t)idx) : 0);
}

/*
 * The entry of a symbol that has to exist: the index 0 (no symbol) gives an entry of zeros, a symbol
 * that is not defined, and not a null pointer.
 */
struct sym *
symof(int idx)
{
	static struct sym none;

	return ((uint16_t)idx ? (struct sym *)(stab + (uint16_t)idx) : &none);
}

/* the index of an entry of the symbol table, and of the information table */
int
stidx(struct sym *p)
{
	return ((uint8_t *)p - stab);
}

int
itidx(void *p)
{
	return ((uint8_t *)p - itab);
}

/* the list of the local symbols of the current block (the original uses what is in front of the array after a } too many) */
static int
blkslot(void)
{
	return (blklevel > 0 ? blklevel - 1 : 0);
}

/* enter a name that is not in the table yet; returns the index of the new entry */
int
stenter(const char *name, int mode)
{
	int h;
	struct sym *p, *p2;

	if (*name) {
		h = hash(1000, name);
		if (hashtab[h] == 0 && (mode & 6) != 6) {
			p = stalloc(10);
			p->s_2 = namalloc(name);
			p->s_type = 0;
			p->s_7 = 0;
			p->s_8 = 0;
			hashtab[h] = stidx(p);
			return (hashtab[h]);
		}
		p2 = stalloc(16);
		p2->s_2 = namalloc(name);
		p2->s_type = 0;
		p2->s_8 = 0;
		if (hashtab[h] != 0) {
			p2->s_14 = hashtab[h];
			p2->s_7 = 8;
		}
		if ((mode & 6) == 6) {
			p2->s_10 = blkstack[blkslot()];
			blkstack[blkslot()] = stidx(p2);
			p2->s_7 = p2->s_7 | 6;
			p2->s_12 = blklevel;
		}
		hashtab[h] = stidx(p2);
		return (hashtab[h]);
	}
	p = stalloc(10);
	p->s_2 = 0;
	p->s_type = 0;
	p->s_7 = 0;
	p->s_8 = 0;
	return (stidx(p));
}

/* set the type and the kind of a symbol and give it an entry in the information table */
void
setsym(int idx, int type, int kind)
{
	struct sym *sp;

	sp = stabp(idx);
	sp->s_type = type;
	sp->s_7 = sp->s_7 | kind;
	switch (type) {
	case 1:
	case 3:
	case 4:
		sp->s_8 = itidx(italloc(6));
		break;
	case 2:
		sp->s_8 = itidx(italloc(12));
		break;
	}
}

/* the index of the symbol with this name, or 0 */
int
stlook(const char *name, int mode)
{
	int i;
	struct sym *p;

	(void)mode;
	i = hashtab[hash(1000, name)];
	while ((p = stabp(i)) != 0) {
		if (strcmp(name, symname(p)) == 0)
			return (i);
		if ((p->s_7 & 8) == 8)
			i = (uint16_t)p->s_14;
		else
			i = 0;
	}
	return (0);
}

/* { */
void
blkopen(void)
{
	if (blklevel == 10)
		error(2, "Nesting too deep\n");
	else
		blklevel++;
}

/* }: the local symbols of the block are taken out of the hash chains */
void
blkclose(void)
{
	struct sym *sp, *q;
	int h;
	unsigned i, n, a, b;

	if (blklevel == 0)
		return;
	blklevel--;
	if (blkstack[blklevel] != 0) {
		n = blkstack[blklevel];
		blkstack[blklevel] = 0;
		while (n != 0) {
			sp = stabp(n);
			a = 0;
			b = 0;
			if ((sp->s_7 & 6) == 6) {
				a = (uint16_t)sp->s_10;
				sp->s_10 = 0;
			}
			if ((sp->s_7 & 8) == 8) {
				b = (uint16_t)sp->s_14;
				sp->s_14 = 0;
			}
			h = hash(1000, symname(sp));
			i = hashtab[h];
			if (i == n)
				hashtab[h] = b;
			else {
				q = 0;
				while (i != n && i != 0) {
					q = stabp(i);
					i = (uint16_t)q->s_14;
				}
				if (q)
					q->s_14 = b;
			}
			n = a;
		}
	}
}
