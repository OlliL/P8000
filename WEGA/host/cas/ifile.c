/*
 * ifile - the intermediate files
 *
 * Pass 1 writes the statements as records of bytes, words and longs; expression
 * trees are written in prefix order. There are two files with the same set of
 * routines: the intermediate file (i) and the symbol file (s). Words and longs are
 * written with the high byte first.
 */
#include "cas.h"

int curline = 1;			/* line number of the statement */
static int lastline = 1;		/* line number of the last record */
FILE *sfile;
FILE *ifile;

void
iputb(int c)
{
	if (putc((unsigned char)c, ifile) == EOF) {
		perror(ifname);
		done(-1);
	}
}

void
iputw(int w)
{
	iputb(w >> 8);
	iputb(w);
}

void
iputl(int32_t l)
{
	iputw((int)(l >> 16));
	iputw((int)l);
}

void
sputb(int c)
{
	if (putc((unsigned char)c, sfile) == EOF) {
		perror(sfname);
		done(-1);
	}
}

void
sputw(int w)
{
	sputb(w >> 8);
	sputb(w);
}

void
sputl(int32_t l)
{
	sputw((int)(l >> 16));
	sputw((int)l);
}

int
sgetb(void)
{
	int c;

	if ((c = getc(sfile)) == EOF) {
		perror(sfname);
		done(-1);
	}
	return (c);
}

int
sgetw(void)
{
	int w;

	w = sgetb() << 8;
	w += sgetb();
	return ((int16_t)w);
}

int32_t
sgetl(void)
{
	uint32_t l;

	l = (uint32_t)(uint16_t)sgetw() << 16;
	l += (uint16_t)sgetw();
	return ((int32_t)l);
}

int
igetb(void)
{
	int c;

	if ((c = getc(ifile)) == EOF) {
		perror(ifname);
		done(-1);
	}
	return (c);
}

int
igetw(void)
{
	int w;

	w = igetb() << 8;
	w += igetb();
	return ((int16_t)w);
}

int32_t
igetl(void)
{
	uint32_t l;

	l = (uint32_t)(uint16_t)igetw() << 16;
	l += (uint16_t)igetw();
	return ((int32_t)l);
}

/* write a node: the operator nodes get their type first */
void
iputnode(NODE *np)
{
	if (np) {
		switch (np->n_type) {
		case 23:
		case 24:
		case 25:
			iputb(np->n_type);
		}
		iputexpr(np);
	}
}

void
sputnode(NODE *np)
{
	if (np) {
		switch (np->n_type) {
		case 23:
		case 24:
		case 25:
			sputb(np->n_type);
		}
		sputexpr(np);
	}
}

/* write an expression tree */
void
iputexpr(NODE *np)
{
	if (np == 0)
		return;
	iputb((np->n_type == 23 || np->n_type == 24 || np->n_type == 25) ? np->n_c2 : np->n_type);
	switch (np->n_type) {
	case 1:
		iputb((unsigned char)np->n_val);
		break;
	case 2:
		iputw((int)np->n_val);
		break;
	case 3:
		iputl(np->n_val);
		break;
	case 6:
		iputw(np->n_w);
		iputnode(np->n_a);
		break;
	case 7:
	case 8:
	case 9:
		iputw(np->n_w);
		break;
	case 23:
	case 24:
	case 25:
		iputexpr(np->n_a);
		iputexpr(np->n_b);
		break;
	default:
		error(2, "Unknown expression type\n");
	}
	nfreed++;
}

void
sputexpr(NODE *np)
{
	if (np == 0)
		return;
	sputb((np->n_type == 23 || np->n_type == 24 || np->n_type == 25) ? np->n_c2 : np->n_type);
	switch (np->n_type) {
	case 1:
		sputb((unsigned char)np->n_val);
		break;
	case 2:
		sputw((int)np->n_val);
		break;
	case 3:
		sputl(np->n_val);
		break;
	case 6:
		sputw(np->n_w);
		sputnode(np->n_a);
		break;
	case 7:
	case 8:
	case 9:
		sputw(np->n_w);
		break;
	case 23:
	case 24:
	case 25:
		sputexpr(np->n_a);
		sputexpr(np->n_b);
		break;
	default:
		error(2, "Unknown expression type\n");
	}
	nfreed++;
}

/* the location counter of the current section */
void
iputloc(void)
{
	iputb(11);
	iputw((int)lctab[cursec][cursub]);
}

/* the line number: small steps get a short record */
void
iputline(void)
{
	switch ((int16_t)(curline - lastline)) {
	case 1:
		iputb(16);
		break;
	case 2:
		iputb(17);
		break;
	case 3:
		iputb(18);
		break;
	default:
		iputb(15);
		iputw(curline);
	}
	lastline = curline;
}
