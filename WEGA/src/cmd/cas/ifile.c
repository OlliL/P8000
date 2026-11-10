static char sccsid[] = "@[$]/as/as/ifile.c\t1.2 05/27/83 15:47:57 - 87wega3.2";

/*
 * ifile - the intermediate files
 *
 * Pass 1 writes the statements as records of bytes, words and longs; expression
 * trees are written in prefix order.  There are two files with the same set of
 * routines: the intermediate file (i) and the symbol file (s).
 */
#include "stdio.h"
#include "cas.h"

int curline = 1;			/* line number of the statement */
int lastline = 1;			/* line number of the last record */
FILE *sfile;
FILE *ifile;

extern char *ifname, *sfname;

iputb(c)
char c;
{
	if (fwrite(&c, 1, 1, ifile) != 1) {
		perror(ifname);
		done(-1);
	}
}

iputw(w)
int w;
{
	iputb((unsigned char)((unsigned)w >> 8));
	iputb((unsigned char)w);
}

iputl(l)
long l;
{
	iputw((int)((unsigned long)l >> 16));
	iputw((int)l);
}

sputb(c)
char c;
{
	if (fwrite(&c, 1, 1, sfile) != 1) {
		perror(sfname);
		done(-1);
	}
}

sputw(w)
int w;
{
	sputb((unsigned char)((unsigned)w >> 8));
	sputb((unsigned char)w);
}

sputl(l)
long l;
{
	sputw((int)((unsigned long)l >> 16));
	sputw((int)l);
}

unsigned char
sgetb()
{
	unsigned char c;

	if (fread(&c, 1, 1, sfile) != 1) {
		perror(sfname);
		done(-1);
	}
	return(c);
}

sgetw()
{
	register int w;

	w = sgetb() << 8;
	w += sgetb();
	return(w);
}

long
sgetl()
{
	register long l;

	l = (long)(unsigned)sgetw() << 16;
	l += (unsigned)sgetw();
	return(l);
}

unsigned char
igetb()
{
	unsigned char c;

	if (fread(&c, 1, 1, ifile) != 1) {
		perror(ifname);
		done(-1);
	}
	return(c);
}

igetw()
{
	register int w;

	w = igetb() << 8;
	w += igetb();
	return(w);
}

long
igetl()
{
	register long l;

	l = (long)(unsigned)igetw() << 16;
	l += (unsigned)igetw();
	return(l);
}

/* the address of an instruction row, as written by the segmented address code */
struct irow *
igetaddr()
{
	return((struct irow *)igetl());
}

/* write a node: the operator nodes get their type first */
iputnode(np)
NODE *np;
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

sputnode(np)
NODE *np;
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
iputexpr(np)
NODE *np;
{
	if (np == 0)
		return(0);
	switch (np->n_type) {
	case 23:
	case 24:
	case 25:
		iputb(np->n_u.cc.c2);
		break;
	default:
		iputb(np->n_type);
	}
	switch (np->n_type) {
	case 6:
		iputw(np->n_u.wl.w);
		iputnode(np->n_u.wp.p);
		break;
	case 7:
	case 8:
	case 9:
		iputw(np->n_u.pw.w);
		break;
	case 1:
		iputb((unsigned char)np->n_u.l);
		break;
	case 2:
		iputw((int)np->n_u.l);
		break;
	case 3:
		iputl(np->n_u.l);
		break;
	case 23:
	case 24:
	case 25:
		iputexpr(np->n_u.cp.p1);
		iputexpr(np->n_u.cp.p2);
		break;
	default:
		error(2, "Unknown expression type\n");
	}
	nfreed++;
}

sputexpr(np)
NODE *np;
{
	if (np == 0)
		return(0);
	switch (np->n_type) {
	case 23:
	case 24:
	case 25:
		sputb(np->n_u.cc.c2);
		break;
	default:
		sputb(np->n_type);
	}
	switch (np->n_type) {
	case 6:
		sputw(np->n_u.wl.w);
		sputnode(np->n_u.wp.p);
		break;
	case 7:
	case 8:
	case 9:
		sputw(np->n_u.pw.w);
		break;
	case 1:
		sputb((unsigned char)np->n_u.l);
		break;
	case 2:
		sputw((int)np->n_u.l);
		break;
	case 3:
		sputl(np->n_u.l);
		break;
	case 23:
	case 24:
	case 25:
		sputexpr(np->n_u.cp.p1);
		sputexpr(np->n_u.cp.p2);
		break;
	default:
		error(2, "Unknown expression type\n");
	}
	nfreed++;
}

/* the location counter of the current section */
iputloc()
{
	iputb(11);
	iputw((int)lctab[cursec][cursub]);
}

/* the line number: small steps get a short record */
iputline()
{
	switch (curline - lastline) {
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
