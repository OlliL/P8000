/*
 * optim - the length of the jpr and callr instructions
 *
 * Called at the end of the first pass for every section and subsection (main). The entries
 * of the undecided jpr (struct jent, chained by j_10, the newest first, jn of them) are
 * resolved by iteration: jtab[i].jr_0 is the distance of entry i (0: surely short, 0x1004:
 * long), jtab[i].jr_2 the list of the entries whose distance changes when entry i becomes
 * long. An entry that becomes long grows by 2 bytes (4 with segments); the rest of
 * the entries are made long until nothing changes. Then jadj[i] is the growth of all
 * entries up to i, which moves the symbols (jsymadj) and the location counter.
 */
#include "cas.h"

struct jrec {				/* a distance or an entry of a list */
	int16_t	jr_0;
	struct jrec *jr_2;
};

#define JCHUNK	512

struct jchunk {				/* room for list entries */
	struct jchunk *next;
	struct jrec rec[JCHUNK];
};

static struct jent *jlist;		/* the newest entry */
static unsigned jn;			/* number of entries */
static int16_t *jadj;
static struct jrec *jtab;		/* jn records */
static struct jchunk *jchunks;
static unsigned jused;			/* list entries in use */

static void jinit(void);
static void jdeps(void);
static int jdist(struct jent *ip, int i);
static void jiter(void);
static void jsum(void);
static void jsymadj(int sub, int sec);

static struct jent *
jnext(struct jent *p)
{
	return ((struct jent *)itabp(p->j_10));
}

void
optim(int sub, int sec)
{
	struct jchunk *c;

	jn = (uint16_t)lcaux[sec][sub].la_0;
	jlist = (struct jent *)itabp(lcaux[sec][sub].la_1);
	if (jn == 0)
		return;
	jtab = calloc(jn, sizeof *jtab);
	jadj = calloc(jn, sizeof *jadj);
	jchunks = 0;
	jused = 0;
	if (jtab == 0 || jadj == 0) {
		perror("jump optimization");
		done(-1);
	}
	jinit();
	jdeps();
	jiter();
	jsum();
	jsymadj(sub, sec);
	lcaux[sec][sub].la_0 = 0;
	lcaux[sec][sub].la_1 = 0;
	lctab[sec][sub] += (uint16_t)jadj[jn - 1];
	free(jtab);
	free(jadj);
	while ((c = jchunks) != 0) {
		jchunks = c->next;
		free(c);
	}
}

/* the first distances */
static void
jinit(void)
{
	unsigned i;
	struct jent *p;

	p = jlist;
	for (i = 0; p != 0; i++) {
		jtab[i].jr_0 = jdist(p, i);
		p = jnext(p);
	}
}

/* room for a list entry */
static struct jrec *
newrec(void)
{
	struct jchunk *c;

	if (jused % JCHUNK == 0) {
		c = calloc(1, sizeof *c);
		if (c == 0) {
			perror("jump optimization");
			done(-1);
		}
		c->next = jchunks;
		jchunks = c;
	}
	return (&jchunks->rec[jused++ % JCHUNK]);
}

/* for every entry the list of the entries in between, whose growth changes its distance */
static void
jdeps(void)
{
	unsigned i, j;
	int32_t v, loc;
	struct jent *p, *q;
	struct jrec *r;

	p = jlist;
	i = 0;
	while (p != 0) {
		if (p->j_12 == 0 && jtab[i].jr_0 != 0) {
			v = symval(p->j_4);
			loc = (uint16_t)p->j_2;
			if (v < loc) {
				q = jnext(p);
				j = i + 1;
				while (q != 0 && (uint32_t)v <= (uint16_t)q->j_2) {
					r = newrec();
					r->jr_0 = i;
					r->jr_2 = jtab[j].jr_2;
					jtab[j].jr_2 = r;
					q = jnext(q);
					j++;
				}
			} else {
				q = jlist;
				j = 0;
				while (q != p) {
					if ((uint32_t)v >= (uint16_t)q->j_2) {
						r = newrec();
						r->jr_0 = i;
						r->jr_2 = jtab[j].jr_2;
						jtab[j].jr_2 = r;
					}
					q = jnext(q);
					j++;
				}
			}
		}
		p = jnext(p);
		i++;
	}
}

/*
 * The distance of entry ip (the i-th of the chain) to its symbol, and with all the entries
 * between made long. 0x1004 and j_12 = 1: surely long (no symbol or too far); 0: short
 * in both cases; otherwise the distance now.
 */
static int
jdist(struct jent *ip, int i)
{
	uint16_t n, m;
	int16_t k, d1, d2;
	int32_t v;
	struct sym *sp;

	sp = symof(ip->j_4);
	if (sp->s_type != 1 || (sp->s_7 & 6) == 0) {
		ip->j_12 = 1;
		return (0x1004);
	}
	m = sp->s_0;
	n = jn - i - 1;
	v = symval(ip->j_4);
	d1 = v + ip->j_6 - (uint16_t)ip->j_2;
	if (m > n) {
		k = m - n - 1;
		if (segmode)
			d2 = v + ip->j_6 + (uint16_t)(k << 2) - (uint16_t)ip->j_2;
		else
			d2 = v + ip->j_6 + (uint16_t)(k + k) - (uint16_t)ip->j_2;
	} else {
		k = n - m;
		if (segmode)
			d2 = v + ip->j_6 - (uint16_t)(k << 2) - (uint16_t)ip->j_2;
		else
			d2 = v + ip->j_6 - (uint16_t)(k + k) - (uint16_t)ip->j_2;
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
static void
jiter(void)
{
	unsigned i;
	int changed, pass, hi, lo;
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
			q = jnext(q);
		}
		pass++;
	} while (changed);
}

/* jadj[i]: the growth of the entries 0 ... i */
static void
jsum(void)
{
	int sum, i;
	struct jent *p;

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
		p = jnext(p);
	}
	for (i = 0; i < (int)jn; i++) {
		jadj[i] += sum;
		sum = jadj[i];
	}
}

/* move the labels of the section behind the entries that became long */
static void
jsymadj(int sub, int sec)
{
	struct sym *sp, *ip;
	unsigned i;

	for (i = 2; i < stoff; ) {
		sp = stabp(i);
		if (sp->s_type == 1 && (sp->s_7 & 1) == 1 && sp->s_0 != 0) {
			ip = itabp(sp->s_8);
			if (((uint16_t)ip->s_0 >> 8) == sec && (ip->s_0 & 0xFF) == sub)
				ip->s_2 += (uint16_t)jadj[sp->s_0 - 1];
		}
		if ((sp->s_7 & 6) == 6 || (sp->s_7 & 8) == 8)
			i += 16;
		else
			i += 10;
	}
}
