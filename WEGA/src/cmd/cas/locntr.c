static char sccsid[] = "@[$]locntr.c\t1.2 05/06/83 13:25:13 - 87wega3.2";

/*
 * locntr - the location counter, the sections and the subsections
 *
 * A section has three subsections (0 code, 1 data, 2 bss), each with its own
 * location counter in lctab[section][subsection].  The code, data and bss
 * subsections of a section are written one after the other at the end of pass 1.
 */
#include "cas.h"

int cursec = 0;
int cursub = 1;
int nsect = 1;
long lctab[256][3];			/* location counter of every section and subsection (bss 9634) */
struct lcaux lcaux[256][3];		/* see jpr.c, optim.c (bss ac26) */


/* advance the location counter */
locadv(n)
unsigned n;
{
	if ((unsigned long)(0x10000L - lctab[cursec][cursub]) < n)
		error(2, "Segment overflow\n");
	else
		lctab[cursec][cursub] += n;
}

/* the subsection */
setsub(n)
int n;
{
	cursub = n;
}

/* .psec name: select the section of that name, or make a new one */
secname(np)
NODE *np;
{
	register struct sym *sp;
	register struct sym *ip;

	sp = stabp(np->n_u.pw.w);
	switch (sp->s_type) {
	case 4:
		ip = itabp(sp->s_8);
		setsec((int)ip->s_2);
		setsub(1);
		iputb(0x29);
		iputb((unsigned char)ip->s_2);
		break;
	case 0:
		setsym(np->n_u.pw.w, 4, 2);
		sp = stabp(np->n_u.pw.w);
		ip = itabp(sp->s_8);
		ip->s_0 = (nsect << 8) | 1;
		ip->s_2 = (long)(unsigned)nsect;
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
setsec(n)
unsigned n;
{
	if (n > 255)
		error(2, "Too many segments\n");
	else
		cursec = n;
}

/* .even */
even()
{
	if ((lctab[cursec][cursub] & 1) == 1L) {
		locadv(1);
		iputloc();
	}
}

/* end of pass 1: every section ends with an even location counter in all subsections */
endpass1()
{
	register unsigned i;
	register int sec;
	register int sub;

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
odd()
{
	if ((lctab[cursec][cursub] & 1) == 0) {
		locadv(1);
		iputloc();
	}
}

/* the value of . */
NODE *
dotval()
{
	register struct sym *sp;
	register struct sym *ip;
	register int idx;

	idx = stenter("", 0);
	setsym(idx, 1, 5);
	sp = stabp(idx);
	ip = itabp(sp->s_8);
	ip->s_2 = lctab[cursec][cursub];
	return (mknode7(idx));
}

/* .= expression */
setdot(np)
NODE *np;
{
	int n;

	switch (exprtype(np)) {
	case 1:
		n = (long)evalint(np) - lctab[cursec][cursub];
		locadv(n);
		iputloc();
		nfreed += 3;
		break;
	}
}
