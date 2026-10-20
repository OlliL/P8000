static char sccsid[] = "@[$]symbols.c\t1.3  07/18/83 13:31:43 Copr. 1981 87wega3.2.";

/*
 * symbols - define symbols and look at their values
 *
 * A name is a node (mknode7) that holds the index of the symbol in the symbol table;
 * the kind byte at offset 6 of the node says how it is defined (0 extern or common,
 * 2 ::=, 3 ::, 4 :=, 5 :).  The value of a symbol is in an entry of the itable.
 */
#include "cas.h"

extern unsigned char fn4ECC();		/* the type of an expression (expr.c) */
extern int fn4F52();			/* its value as an int */
extern int fn479E();			/* enter a name into the symbol table (ssymtab.c) */

/* define the symbol of np with the value val */
defsym(np, val)
NODE *np;
NODE *val;
{
	register struct sym *sp;
	register struct sym *q;
	register struct sym *p;
	struct sym *ip;
	int n;
	int redef;

	redef = 0;
	sp = stabp(np->n_u.pw.w);
	if (sp == 0)
		error(2, "Unknown symbol\n");
	if (sp->s_type != 0 && (sp->s_7 & 6) != 0) {
		if (np->n_u.cc.c1 == 0) {
			nfreed += 2;
			return (0);
		}
		if ((sp->s_7 & 6) == 6) {
			p = sp;
			if (p->s_12 == blklevel)
				error(2, "Symbol redefined\n");
			np->n_u.pw.w = fn479E(sp->s_2, 6);
			sp = stabp(np->n_u.pw.w);
		} else
			error(2, "Symbol redefined\n");
	}
	if (sp->s_type != 0)
		redef++;
	iputb(0x14);
	iputw(np->n_u.pw.w);
	if (redef == 0) {
		sputb(0x14);
		sputw(np->n_u.pw.w);
	}
	switch (fn4ECC(val)) {
	case 3:
		if (redef == 0)
			fn4926(np->n_u.pw.w, (unsigned char)1, np->n_u.cc.c1);
		else
			sp->s_7 = (sp->s_7 & ~6) | np->n_u.cc.c1;
		sp = stabp(np->n_u.pw.w);
		sp->s_0 = lcaux[cursec][cursub].la_0;
		ip = itabp(sp->s_8);
		if (redef == 0 || np->n_u.cc.c1 != 0 || (unsigned long)val->n_u.l > ip->s_2)
			ip->s_2 = val->n_u.l;
		ip->s_0 = (cursec << 8) | cursub;
		nfreed += 2;
		break;
	case 1:
		if (redef == 0)
			fn4926(np->n_u.pw.w, (unsigned char)1, np->n_u.cc.c1 | 1);
		else
			sp->s_7 = (sp->s_7 & ~6) | np->n_u.cc.c1;
		sp = stabp(np->n_u.pw.w);
		ip = itabp(sp->s_8);
		if (val->n_type == 0x19) {
			q = stabp(((NODE *)val->n_u.cc.l1)->n_u.pw.w);
			nfreed += 4;
		} else {
			q = stabp(val->n_u.pw.w);
			nfreed += 2;
		}
		sp->s_0 = q->s_0;
		ip->s_2 = (long)fn4F52(val);
		ip->s_0 = (cursec << 8) | cursub;
		break;
	case 0:
		error(2, "Invalid assignment\n");
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		error(2, "Invalid assignment\n");
		break;
	}
}

/* := with a reserved word as the value: the symbol becomes an alias of the entry */
defalias(np, val)
NODE *np;
struct pst *val;
{
	register struct sym *sp;
	register struct sym *ip;

	fn4926(np->n_u.pw.w, (unsigned char)3, np->n_u.cc.c1);
	sp = stabp(np->n_u.pw.w);
	ip = itabp(sp->s_8);
	ip->s_2 = (long)val;
	ip->s_0 = (cursec << 8) | cursub;
	nfreed++;
}

/* the value of a defined symbol; its section is in bits 24 ... 30 of a relocatable one */
long
symval(idx)
int idx;
{
	register struct sym *sp;
	register struct sym *ip;
	register long v;

	sp = stabp(idx);
	if (sp->s_type == 0) {
		error(2, "Undefined symbol\n");
		return (0L);
	}
	ip = itabp(sp->s_8);
	if ((sp->s_7 & 6) == 0)
		return (0L);
	if ((sp->s_7 & 1) == 1)
		v = ip->s_2 | ((long)(unsigned)ip->s_0 >> 8 << 24);
	else
		v = ip->s_2;
	return (v);
}

/* is the symbol defined (and not an external one)? */
isdef(idx)
int idx;
{
	register struct sym *sp;

	sp = stabp(idx);
	if (sp->s_type == 1 && (sp->s_7 & 6) != 0 && (sp->s_7 & 1) == 0)
		return (1);
	return (0);
}

/* the relocation information of a symbol in an expression */
long
symreloc(idx)
int idx;
{
	register struct sym *sp;
	register struct sym *ip;
	register long v;
	int seg;

	v = 0;
	sp = stabp(idx);
	if (sp->s_type == 0) {
		error(2, "Undefined symbol\n");
		return (0L);
	}
	switch (sp->s_7 & 6) {
	case 0:
		ip = itabp(sp->s_8);
		v = (ip->s_2 << 4) | 8;
		if (segmode != 0)
			v |= ((ip->s_2 << 4) | 9) << 16;
		break;
	case 2:
	case 4:
	case 6:
		v != 0;
		if ((sp->s_7 & 1) != 0) {
			ip = itabp(sp->s_8);
			switch (ip->s_0 & 0xFF) {
			case 0:
				v |= 2;
				break;
			case 1:
				v |= 4;
				break;
			case 2:
				v |= 6;
				break;
			default:
				error(2, "Unknown area\n");
			}
		}
		if (segmode != 0) {
			ip = itabp(sp->s_8);
			seg = (unsigned)ip->s_0 >> 8;
			v |= (unsigned)(seg << 8) | 1;
			v |= (v | 0x10) << 16;
		}
		break;
	default:
		error(2, "Unknown scope\n");
	}
	return (v);
}

/* .comm: the names of the list get the size */
defcomm(size, list)
NODE *size;
NODE *list;
{
	register NODE *p;
	register NODE *q;

	if (fn4ECC(size) != 3)
		error(2, "Invalid constant\n");
	p = list;
	q = p;
	while (q != 0) {
		q = p->n_next;
		defsym(p, size);
		p = q;
	}
}

/* define the labels that were read in front of a statement */
deflabels()
{
	register NODE *p;

	if (plist == 0)
		return (0);
	for (p = plist; p != 0; p = p->n_next)
		defsym(p, mknode1(3, lctab[cursec][cursub]));
	plist = 0;
}

/* .stabn */
putstabn(a, b, c, s)
NODE *a;
NODE *b;
NODE *c;
char *s;
{
	register NODE *np;
	char ch;

	sputb(0x2a);
	ch = *s++;
	while (*s != ch)
		sputb(*s++);
	sputb(0);
	sputnode(a);
	sputnode(b);
	np = mknode1(3, (long)(unsigned)cursec);
	sputnode(np);
	sputnode(c);
}

/* .stabp */
putstabp(a, b, c, d, s)
NODE *a;
NODE *b;
NODE *c;
NODE *d;
char *s;
{
	char ch;

	sputb(0x2a);
	ch = *s++;
	while (*s != ch)
		sputb(*s++);
	sputb(0);
	sputnode(a);
	sputnode(b);
	sputnode(d);
	sputnode(c);
}

/* .stabr */
putstabr(a, b, s)
NODE *a;
NODE *b;
char *s;
{
	register NODE *np;
	char ch;

	sputb(0x2b);
	ch = *s++;
	while (*s != ch)
		sputb(*s++);
	sputb(0);
	sputnode(a);
	np = mknode1(3, (long)(unsigned)cursec);
	sputnode(np);
	sputnode(b);
}
