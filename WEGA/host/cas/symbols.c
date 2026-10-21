/*
 * symbols - define symbols and look at their values
 *
 * A name is a node (mknode7) that holds the index of the symbol in the symbol table;
 * n_c1 of the node says how it is defined (0 extern or common, 2 ::=, 3 ::, 4 :=, 5 :).
 * The value of a symbol is in an entry of the information table.
 */
#include "cas.h"

/* define the symbol of np with the value val */
void
defsym(NODE *np, NODE *val)
{
	struct sym *sp, *ip, *q;
	int redef;

	redef = 0;
	sp = stabp(np->n_w);
	if (sp == 0) {
		error(2, "Unknown symbol\n");
		return;
	}
	if (sp->s_type != 0 && (sp->s_7 & 6) != 0) {
		if (np->n_c1 == 0) {
			nfreed += 2;
			return;
		}
		if ((sp->s_7 & 6) == 6) {
			if (sp->s_12 == blklevel)
				error(2, "Symbol redefined\n");
			np->n_w = stenter(symname(sp), 6);
			sp = stabp(np->n_w);
		} else
			error(2, "Symbol redefined\n");
	}
	if (sp->s_type != 0)
		redef++;
	iputb(0x14);
	iputw(np->n_w);
	if (redef == 0) {
		sputb(0x14);
		sputw(np->n_w);
	}
	switch (exprtype(val)) {
	case 3:
		if (redef == 0)
			setsym(np->n_w, 1, np->n_c1);
		else
			sp->s_7 = (sp->s_7 & ~6) | np->n_c1;
		sp = stabp(np->n_w);
		sp->s_0 = lcaux[cursec][cursub].la_0;
		ip = itabp(sp->s_8);
		if (redef == 0 || np->n_c1 != 0 || (uint32_t)val->n_val > (uint32_t)ip->s_2)
			ip->s_2 = val->n_val;
		ip->s_0 = (cursec << 8) | cursub;
		nfreed += 2;
		break;
	case 1:
		if (redef == 0)
			setsym(np->n_w, 1, np->n_c1 | 1);
		else
			sp->s_7 = (sp->s_7 & ~6) | np->n_c1;
		sp = stabp(np->n_w);
		ip = itabp(sp->s_8);
		if (val->n_type == 0x19) {
			q = stabp(val->n_a->n_w);
			nfreed += 4;
		} else {
			q = stabp(val->n_w);
			nfreed += 2;
		}
		sp->s_0 = q->s_0;
		ip->s_2 = evalint(val);
		ip->s_0 = (cursec << 8) | cursub;
		break;
	default:
		error(2, "Invalid assignment\n");
		break;
	}
}

/* := with a reserved word as the value: the symbol becomes an alias of the entry */
void
defalias(NODE *np, struct pst *val)
{
	struct sym *sp, *ip;

	setsym(np->n_w, 3, np->n_c1);
	sp = stabp(np->n_w);
	ip = itabp(sp->s_8);
	ip->s_2 = val - pst;
	ip->s_0 = (cursec << 8) | cursub;
	nfreed++;
}

/* the value of a defined symbol; its section is in bits 24 ... 30 of a relocatable one */
int32_t
symval(int idx)
{
	struct sym *sp, *ip;

	sp = symof(idx);
	if (sp->s_type == 0) {
		error(2, "Undefined symbol\n");
		return (0);
	}
	ip = itabp(sp->s_8);
	if ((sp->s_7 & 6) == 0)
		return (0);
	if ((sp->s_7 & 1) == 1)
		return ((uint32_t)ip->s_2 | ((uint32_t)(uint16_t)ip->s_0 >> 8 << 24));
	return (ip->s_2);
}

/* is the symbol defined (and not an external one)? */
int
isdef(int idx)
{
	struct sym *sp;

	sp = symof(idx);
	if (sp->s_type == 1 && (sp->s_7 & 6) != 0 && (sp->s_7 & 1) == 0)
		return (1);
	return (0);
}

/* the relocation information of a symbol in an expression */
int32_t
symreloc(int idx)
{
	struct sym *sp, *ip;
	uint32_t v;
	int seg;

	v = 0;
	sp = symof(idx);
	if (sp->s_type == 0) {
		error(2, "Undefined symbol\n");
		return (0);
	}
	switch (sp->s_7 & 6) {
	case 0:
		ip = itabp(sp->s_8);
		v = ((uint32_t)ip->s_2 << 4) | 8;
		if (segmode != 0)
			v |= (((uint32_t)ip->s_2 << 4) | 9) << 16;
		break;
	case 2:
	case 4:
	case 6:
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
			seg = (uint16_t)ip->s_0 >> 8;
			v |= (uint16_t)(seg << 8) | 1;
			v |= (v | 0x10) << 16;
		}
		break;
	default:
		error(2, "Unknown scope\n");
	}
	return (v);
}

/* .comm: the names of the list get the size */
void
defcomm(NODE *size, NODE *list)
{
	NODE *p, *q;

	if (exprtype(size) != 3)
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
void
deflabels(void)
{
	NODE *p;

	if (plist == 0)
		return;
	for (p = plist; p != 0; p = p->n_next)
		defsym(p, mknode1(3, lctab[cursec][cursub]));
	plist = 0;
}

/* the text of a .stab string (the delimiters are not part of it) and a zero byte */
static void
stabstr(const char *s)
{
	char ch;

	ch = *s++;
	while (*s && *s != ch)
		sputb(*s++);
	sputb(0);
}

/* .stabn */
void
putstabn(NODE *a, NODE *b, NODE *c, const char *s)
{
	NODE *np;

	sputb(0x2a);
	stabstr(s);
	sputnode(a);
	sputnode(b);
	np = mknode1(3, (uint16_t)cursec);
	sputnode(np);
	sputnode(c);
}

/* .stabp */
void
putstabp(NODE *a, NODE *b, NODE *c, NODE *d, const char *s)
{
	sputb(0x2a);
	stabstr(s);
	sputnode(a);
	sputnode(b);
	sputnode(d);
	sputnode(c);
}

/* .stabr */
void
putstabr(NODE *a, NODE *b, const char *s)
{
	NODE *np;

	sputb(0x2b);
	stabstr(s);
	sputnode(a);
	np = mknode1(3, (uint16_t)cursec);
	sputnode(np);
	sputnode(b);
}
