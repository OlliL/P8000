/*
 * locntr - the location counter, the sections and the subsections
 *
 * A section has three subsections (0 code, 1 data, 2 bss), each with its own
 * location counter in lctab[section][subsection]. The code, data and bss
 * subsections of a section are written one after the other at the end of pass 1.
 */
#include "cas.h"

int cursec = 0;
int cursub = 1;
int nsect = 1;
long lctab[256][3];			/* location counter of every section and subsection */
struct lcaux lcaux[256][3];		/* see jpr.c, optim.c */

/* advance the location counter */
void
locadv(unsigned n)
{
	n &= 0xFFFF;
	if ((unsigned long)(0x10000L - lctab[cursec][cursub]) < n)
		error(2, "Segment overflow\n");
	else
		lctab[cursec][cursub] += n;
}

/* the subsection */
void
setsub(int n)
{
	cursub = n;
}

/* .psec name: select the section of that name, or make a new one */
void
secname(NODE *np)
{
	struct sym *sp, *ip;

	sp = stabp(np->n_w);
	switch (sp->s_type) {
	case 4:
		ip = itabp(sp->s_8);
		setsec((int)ip->s_2);
		setsub(1);
		iputb(0x29);
		iputb((unsigned char)ip->s_2);
		break;
	case 0:
		setsym(np->n_w, 4, 2);
		sp = stabp(np->n_w);
		ip = itabp(sp->s_8);
		ip->s_0 = (nsect << 8) | 1;
		ip->s_2 = (uint16_t)nsect;
		setsec(nsect);
		iputb(0x29);
		iputb((unsigned char)nsect);
		nsect++;
		setsub(1);
		break;
	default:
		error(2, "Invalid section name\n");
	}
	nfreed++;
}

/* the section */
void
setsec(int n)
{
	if ((uint16_t)n > 255)
		error(2, "Too many segments\n");
	else
		cursec = (uint16_t)n;
}

/* .even */
void
even(void)
{
	if ((lctab[cursec][cursub] & 1) == 1L) {
		locadv(1);
		iputloc();
	}
}

/* end of pass 1: every section ends with an even location counter in all subsections */
void
endpass1(void)
{
	int i, sec, sub;

	sec = cursec;
	sub = cursub;
	for (i = 0; i < nsect; i++) {
		setsec(i);
		iputb(0x29);
		iputb((unsigned char)i);
		setsub(1);
		iputb(13);
		even();
		setsub(0);
		iputb(12);
		even();
		setsub(2);
		iputb(14);
		even();
	}
	setsec(sec);
	iputb(0x29);
	iputb((unsigned char)sec);
	setsub(sub);
	switch (sub) {
	case 0:
		iputb(12);
		break;
	case 1:
		iputb(13);
		break;
	case 2:
		iputb(14);
		break;
	}
}

/* .odd */
void
odd(void)
{
	if ((lctab[cursec][cursub] & 1) == 0) {
		locadv(1);
		iputloc();
	}
}

/* the value of . */
NODE *
dotval(void)
{
	struct sym *sp, *ip;
	int idx;

	idx = stenter("", 0);
	setsym(idx, 1, 5);
	sp = stabp(idx);
	ip = itabp(sp->s_8);
	ip->s_2 = lctab[cursec][cursub];
	return (mknode7(idx));
}

/* .= expression */
void
setdot(NODE *np)
{
	int16_t n;

	switch (exprtype(np)) {
	case 1:
		n = (int16_t)(evalint(np) - lctab[cursec][cursub]);
		locadv(n);
		iputloc();
		nfreed += 3;
		break;
	}
}
