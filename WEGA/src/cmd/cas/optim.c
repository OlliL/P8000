static char sccsid[] = "@[$]optim.c\t1.2 05/02/83 10:41:19 - 87wega3.2";

/*
 * optim - the length of the jpr and callr instructions
 *
 * Called at the end of the first pass for every section and subsection (main).  The entries
 * of the undecided jpr (struct jent, chained by j_10, the newest first, jn of them) are
 * resolved by iteration: jtab[i].jr_0 is the distance of entry i (0: surely short, 0x1004:
 * long), jtab[i].jr_2 the list of the entries whose distance changes when entry i becomes
 * long.  An entry that becomes long grows by 2 bytes (4 with segments); the rest of
 * the entries are made long until nothing changes.  Then jadj[i] is the growth of all
 * entries up to i, which moves the symbols (jsymadj) and the location counter.
 */
#include "cas.h"

struct jrec {				/* 6 bytes: a distance or an entry of a list */
	int	jr_0;
	struct jrec *jr_2;
};

struct jent *jlist;			/* the newest entry */
unsigned jn;				/* number of entries */
int *jadj;
struct jrec *jtab;			/* jn records, then the room for the list entries */
unsigned jcap;
struct jrec *jfree;

unsigned jused = 0;			/* list entries in use */

optim(sub, sec)
int sub;
int sec;
{
	register int size;

	jn = lcaux[sec][sub].la_0;
	jlist = (struct jent *)itabp(lcaux[sec][sub].la_1);
	jcap = jn * 2;
	if (jn == 0)
		return (0);
	size = jn * 6;
	size += jcap * 6;
	jtab = (struct jrec *)mkseg(0, size);
	jfree = jtab + jn;
	jinit();
	jdeps();
	jiter();
	jsum();
	jsymadj(sub, sec);
	lcaux[sec][sub].la_0 = 0;
	lcaux[sec][sub].la_1 = 0;
	lctab[sec][sub] += (unsigned)jadj[jn - 1];
}

/* the first distances */
jinit()
{
	register int i;
	register struct jent *p;

	p = jlist;
	for (i = 0; p != 0; i++) {
		jtab[i].jr_0 = jdist(p, i);
		p = (struct jent *)itabp(p->j_10);
	}
}

/* room for a list entry */
struct jrec *
newrec()
{
	register int n;
	struct jrec *p;

	if (jused >= jcap) {
		p = jfree;
		n = 0xC00;
		if (ssgbrk(*(char *)&p & 0x7F, n) == -1L) {
			perror("jump optimization");
			done(-1);
		}
		jcap += 0x200;
	}
	jused++;
	return (jfree + (jused - 1));
}

/* for every entry the list of the entries in between, whose growth changes its distance */
jdeps()
{
	register int i;
	register int j;
	register long v;
	register long loc;
	struct jent *p;
	struct jent *q;
	struct jrec *r;

	p = jlist;
	i = 0;
	while (p != 0) {
		if (p->j_12 == 0 && jtab[i].jr_0 != 0) {
			v = symval(p->j_4);
			loc = (unsigned)p->j_2;
			if (v < loc) {
				q = (struct jent *)itabp(p->j_10);
				j = i + 1;
				while (q != 0 && (unsigned long)v <= (unsigned long)(unsigned)q->j_2) {
					r = newrec();
					r->jr_0 = i;
					r->jr_2 = jtab[j].jr_2;
					jtab[j].jr_2 = r;
					q = (struct jent *)itabp(q->j_10);
					j++;
				}
			} else {
				q = jlist;
				j = 0;
				while (q != p) {
					if ((unsigned long)v >= (unsigned long)(unsigned)q->j_2) {
						r = newrec();
						r->jr_0 = i;
						r->jr_2 = jtab[j].jr_2;
						jtab[j].jr_2 = r;
					}
					q = (struct jent *)itabp(q->j_10);
					j++;
				}
			}
		}
		p = (struct jent *)itabp(p->j_10);
		i++;
	}
}

/*
 * The distance of entry ip (the i-th of the chain) to its symbol, and with all the entries
 * between made long.  0x1004 and j_12 = 1: surely long (no symbol or too far); 0: short
 * in both cases; otherwise the distance now.
 */
jdist(ip, i)
struct jent *ip;
int i;
{
	register unsigned n;
	register unsigned m;
	register int k;
	register int d1;
	register int d2;
	long v;
	struct sym *sp;

	sp = stabp(ip->j_4);
	if (sp->s_type != 1 || (sp->s_7 & 6) == 0) {
		ip->j_12 = 1;
		return (0x1004);
	}
	m = sp->s_0;
	n = jn - i - 1;
	v = symval(ip->j_4);
	d1 = v + ip->j_6 - (unsigned)ip->j_2;
	if (m > n) {
		k = m - n - 1;
		if (segmode)
			d2 = v + ip->j_6 + (unsigned)(k << 2) - (unsigned)ip->j_2;
		else
			d2 = v + ip->j_6 + (unsigned)(k + k) - (unsigned)ip->j_2;
	} else {
		k = n - m;
		if (segmode)
			d2 = v + ip->j_6 - (unsigned)(k << 2) - (unsigned)ip->j_2;
		else
			d2 = v + ip->j_6 - (unsigned)(k + k) - (unsigned)ip->j_2;
	}
	switch (ip->j_13) {
	case 0x50:
		if (d1 > 256 || d1 < -254) {
			ip->j_12 = 1;
			d1 = 0x1004;
		} else if (d2 <= 256 && d2 >= -254)
			d1 = 0;
		break;
	case 0x51:
		if (d1 > 4098 || d1 < -4092) {
			ip->j_12 = 1;
			d1 = 0x1004;
		} else if (d2 <= 4098 && d2 >= -4092)
			d1 = 0;
		break;
	}
	return (d1);
}

/* make the entries long that are out of range, and change the distances that depend on them */
jiter()
{
	register int i;
	register int changed;
	register int pass;
	register int hi;
	register int lo;
	struct jrec *r;
	struct jent *q;

	pass = 0;
	do {
		changed = 0;
		q = jlist;
		for (i = 0; i < jn; i++) {
			if (q->j_12 == 0 || pass == 0) {
				if (q->j_13 == 0x50) {
					hi = 0x100;
					lo = -254;
				} else {
					hi = 0x1002;
					lo = -4092;
				}
				if (lo > jtab[i].jr_0 || hi < jtab[i].jr_0) {
					changed = 1;
					for (r = jtab[i].jr_2; r != 0; r = r->jr_2) {
						if (jtab[r->jr_0].jr_0 < 0) {
							if (segmode)
								jtab[r->jr_0].jr_0 -= 4;
							else
								jtab[r->jr_0].jr_0 -= 2;
						} else {
							if (segmode)
								jtab[r->jr_0].jr_0 += 4;
							else
								jtab[r->jr_0].jr_0 += 2;
						}
					}
					q->j_12 = 1;
				}
			}
			q = (struct jent *)itabp(q->j_10);
		}
		pass++;
	} while (changed);
}

/* jadj[i]: the growth of the entries 0 ... i */
jsum()
{
	register int sum;
	register int i;
	register struct jent *p;

	jadj = (int *)jtab;
	p = jlist;
	sum = 0;
	for (i = jn - 1; i >= 0; i--) {
		if (p->j_12) {
			if (segmode)
				jadj[i] = 4;
			else
				jadj[i] = 2;
		} else
			jadj[i] = 0;
		p = (struct jent *)itabp(p->j_10);
	}
	for (i = 0; i < jn; i++) {
		jadj[i] += sum;
		sum = jadj[i];
	}
}

/* move the labels of the section behind the entries that became long */
jsymadj(sub, sec)
int sub;
int sec;
{
	register struct sym *sp;
	register struct sym *ip;
	unsigned i;

	for (i = 2; i < stoff; ) {
		sp = (struct sym *)0x87000030L;
		sp = (struct sym *)((char *)sp + i);
		if (sp->s_type == 1 && (sp->s_7 & 1) == 1 && sp->s_0 != 0) {
			ip = itabp(sp->s_8);
			if (((unsigned)ip->s_0 >> 8) == sec && (ip->s_0 & 0xFF) == sub)
				ip->s_2 += (unsigned)jadj[sp->s_0 - 1];
		}
		if ((sp->s_7 & 6) == 6 || (sp->s_7 & 8) == 8)
			i += 16;
		else
			i += 10;
	}
}
