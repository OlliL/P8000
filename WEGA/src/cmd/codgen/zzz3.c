/*
 * zzz3.c - the escape codes "Z" of the templates (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: psuffix() 0x2780, geninc() 0x2e4c, zzzcode() 0x2edc, cbgen() 0x460e.
 * The template macro Zx of the table calls zzzcode(p, x) while expand() prints the line; zzzcode() prints parts of
 * the instruction itself or rewrites the tree (the sizes of fields and structures, registers around calls).
 * The variable and function names are guesses, the binary has only addresses.
 */
#include "cg.h"

#define FOREFF	01
#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))
#define DECREF(t)	((((t) >> 2) & ~0xF) | ((t) & 0xF))

#define DIVFLAG	0x80		/* dope flag of the division operators: the dividend gets extended */


extern int cerror();
extern struct node *getlr();
extern struct node *tcopy();
extern struct rs *rfind();
extern int tshape(), order(), getreg(), szty(), rclass(), notoff();
extern int getlab(), deflab(), plainicon();
extern void cbgen();
extern int ispow2();
extern void tsuffix(), hopcode(), expand(), adrput(), rfree(), rfreenode(), rbusy(), rsreturn(), rfreeall();
extern void tfree(), fwalk();
extern char *rnames[];		/* 0x3fca */
extern char *rhnames[];		/* 0x27ba */
extern char *opst[];		/* 0x8e1a */
extern int dope[];		/* 0x8d0a */
extern int negrel[];		/* 0x274: the relation that is true when the one in the index is false */
extern int rmode, rlmode;	/* 0x280a, 0x280c */
extern int nosuffix;		/* 0x280e: Zt, the next Zi, ZB ... print no type suffix */
extern int upperlval;		/* 0xa148 */
extern int nsave, saveorig[], savecls[], savereg[];	/* regfile3.c: the registers moved around a call */
extern int fldsz, fldoff, fldshf;	/* the field that tshape found */
extern struct rs *rsuse;

/* the test condition codes for ZN, by relation */
static char *tcctab[] = {
	"\ttcc\teq,A1\n",
	"\ttcc\tne,A1\n",
	"\ttcc\tle,A1\n",
	"\ttcc\tlt,A1\n",
	"\ttcc\tge,A1\n",
	"\ttcc\tgt,A1\n",
	"\ttcc\tule,A1\n",
	"\ttcc\tult,A1\n",
	"\ttcc\tuge,A1\n",
	"\ttcc\tugt,A1\n",
	"\ttcc\tfeq,A1\n",
	"\ttcc\tfneu,A1\n",
	"\ttcc\tfle,A1\n",
	"\ttcc\tflt,A1\n",
	"\ttcc\tfge,A1\n",
	"\ttcc\tfgt,A1\n",
};

/* the type suffix of the type of the node */
void
psuffix(p)
	struct node *p;
{
	tsuffix(p->type);
}

/* print inc/dec or add/sub for the operator, depending on the size of the constant n; returns the constant to print */
long
geninc(op, n)
	int op;
	long n;
{
	switch (op) {
	case PLUS:
	case ASG PLUS:
	case INCR:
		if (n < 0) {
			n = -n;
			goto dec;
		}
	inc:
		if (n != 0 && n <= 16)
			printf("inc");
		else
			printf("add");
		break;
	case MINUS:
	case ASG MINUS:
	case DECR:
		if (n < 0) {
			n = -n;
			goto inc;
		}
	dec:
		if (n != 0 && n <= 16)
			printf("dec");
		else
			printf("sub");
		break;
	default:
		cerror("geninc: bad op: %s\n", opst[op]);
	}
	return n;
}

void
zzzcode(p, c)
	register struct node *p;
	int c;
{
	register struct node *q, *r;
	register int m;
	struct node *l, *tmp, *rg;
	struct rs *rp, *rn2;
	int n, k, cls, rn;
	unsigned t;
	char *name;

	switch (c) {
	case '<':
	case '>':
		/* the type suffix of the left or the right operand */
		l = getlr(p, c == '<' ? 'L' : 'R');
	case 'B':
		/* the type suffix of the node, none when a float operation works on registers only */
		if (p->type == 6 || p->type == 7) {
			q = getlr(p, 'L');
			r = getlr(p, 'R');
			if (q->op == REG && r->op == REG)
				return;
		}
		psuffix(c == 'B' ? p : l);
		return;
	case 'R':
		/* "sgl," or "dbl," */
		q = getlr(p, 'L');
		if (q->type == 6)
			printf("sgl,");
		else if (q->type == 7)
			printf("dbl,");
		return;
	case 'f':
		if (p->op == FNE || p->op == FEQ)
			return;
		putchar('x');
		return;
	case 'U':
		printf(".word\t");
		return;
	case 'V':
		printf(".byte\t");
		return;
	case 'W':
		printf(".long\t");
		return;
	case 'F':
	case 'I':
		cbgen(p->op, p->label, c);
		return;
	case '2':
		/* log2 of the constant */
		m = ispow2(p->right->lval);
		printf("%d", m);
		return;
	case '~':
		m = ispow2(~p->right->lval);
		printf("%d", m);
		return;
	case 'i':
		/* inc/dec/add/sub or the mnemonic of the operator, with the type suffix */
		q = r = getlr(p, 'R');
		if (!tshape(q, SCON))
			cerror("bad Zi");
		if (plainicon(q))
			q->lval = geninc(p->op, q->lval);
		else
			hopcode(p);
		psuffix(p);
		return;
	case 't':
		nosuffix++;
		return;
	case 'C':
		if (p->type == 2)
			p->type = 4;
		else if (p->type == 12)
			p->type = 14;
		else
			cerror("bad ZC");
		return;
	case 'c':
		/* float <-> double: the operand has to be in a B register */
		p->type = p->type == 6 ? 7 : 6;
		if (!tshape(p->left, STBREG))
			order(p->left, STBREG);
		return;
	case 'N':
		/* a comparison used as a value: 0 or 1 */
		switch ((int)p->lval) {
		case EQ:
		case NE:
		case LE:
		case LT:
		case GE:
		case GT:
		case ULE:
		case ULT:
		case UGE:
		case UGT:
		case FEQ:
		case FNE:
		case FLE:
		case FLT:
		case FGE:
		case FGT:
			deflab(p->label);
			expand(p, FOREFF, "\tldk\tA1,#0\n");
			expand(p, FOREFF, tcctab[negrel[p->lval - EQ] - EQ]);
			return;
		}
		expand(p, FOREFF, "\tldk\tA1,#1\n");
		n = getlab();
		cbgen(0, n, 'I');
		deflab(p->label);
		expand(p, FOREFF, "\tldk\tA1,#0\n");
		deflab(n);
		return;
	case 'H':
	case 'h':
		/* the low byte register of a word register */
		p = getlr(p, c == 'H' ? 'L' : 'R');
		if (p->rval > 7 || p->rval < 0)
			cerror("bad %c call: register = %s\n", c, rnames[p->rval]);
		printf(rhnames[p->rval]);
		return;
	case 'l':
	case 'r':
		/* the upper half of a long operand */
		upperlval = 1;
		l = getlr(p, c);
		adrput(l);
		upperlval = 0;
		return;
	case 'Q':
		if (p->type == 5)
			printf("\textsl\trq4\n");
		else
			printf("\tsubl\trr4,rr4\n");
		return;
	case '0':
		/* give the registers back that were moved around a call (nxwork 33 and 34) */
		while (nsave) {
			nsave--;
			n = savecls[nsave] == 3 ? 5 : 4;
			m = rmode;
			k = rlmode;
			rmode = n == 5;
			rlmode = 0;
			rmove(saveorig[nsave], savereg[nsave], n);
			rp = rfind(saveorig[nsave], savecls[nsave]);
			if (!rp)
				cerror("zzzcode : can't find %d on rslist\n", saveorig[nsave]);
			rp->busy = 1;
			rp = rfind(savereg[nsave], savecls[nsave]);
			if (!rp)
				cerror("zzzcode : can't find %d on rslist\n", savereg[nsave]);
			rp->busy = 0;
			if ((p->left->op == REG || p->left->op == OREG) && p->left->rval == savereg[nsave])
				p->left->rval = saveorig[nsave];
			if ((p->right->op == REG || p->right->op == OREG) && p->right->rval == savereg[nsave])
				p->right->rval = saveorig[nsave];
			rmode = m;
			rlmode = k;
		}
		return;
	case 'u':
#ifdef SEG
		q = p->left;
		if (q->op == OREG && q->lval == 0 && q->sname == 0 && q->stsize >= 0) {
			printf("\tinc\t%s,#2\n", rnames[q->rval + 1]);
			expand(p, FOREFF, "\tZtZi\tAL,AR\n");
			printf("\tdec\t%s,#2\n", rnames[q->rval + 1]);
			return;
		}
#endif
		expand(p, FOREFF, "\tZtZi\tUL,AR\n");
		return;
	case 'K':
		/* store into a field: bit instructions, a byte, a word or the general mask and shift */
		switch (fldsz) {
		case 1:
		case 2:
			if (p->right->lval & 1)
				expand(p, FOREFF, "\tset\tAL,");
			else
				expand(p, FOREFF, "\tres\tAL,");
			printf("#%d\n", fldshf);
			if (fldsz == 1)
				return;
			if (p->right->lval & 2)
				expand(p, FOREFF, "\tset\tAL,");
			else
				expand(p, FOREFF, "\tres\tAL,");
			printf("#%d\n", fldshf + 1);
			return;
		case 8:
			if (fldoff % 8)
				break;
			if (fldoff) {
#ifdef SEG
				q = p->left->left;
				if (q->lval != 0)
					q->lval++;
				else
					printf("\tinc\t%s,#1\n", rnames[q->rval + 1]);
#else
				p->left->left->lval++;
#endif
			}
			if (p->right->lval & 0xFFFF)
				expand(p, FOREFF, "\tldb\tWAL,AR\n");
			else
				expand(p, FOREFF, "\tclrb\tWAL\n");
			return;
		case 16:
			if (p->right->lval)
				expand(p, FOREFF, "\tld\tAL,AR\n");
			else
				expand(p, FOREFF, "\tclr\tAL\n");
			return;
		}
		expand(p, FOREFF, "\tld\tA1,AL\n\tand\tA1,#N\n");
		if (p->right->lval) {
			expand(p, FOREFF, "\tor\tA1,");
			printf("#%d\n", ((int)p->right->lval & ~(-1 << fldsz)) << fldshf);
		}
		expand(p, FOREFF, "\tld\tAL,A1\n");
		return;
	case 'L':
		/* the address of the left operand into the register A1 */
		q = getlr(p, 'L');
		if (q->op == NAME) {
			expand(p, FOREFF, "\tldZB\tA1,#AL\n");
			return;
		}
		if (q->rval >= 0x100 || q->lval != 0 || q->sname != 0 || (q->op == OREG && q->stsize < 0)) {
#ifdef SEG
			expand(p, FOREFF, "\tlda\tVA1X,AL\n");
			expand(p, FOREFF, "\tand\tA1,#%7f00\n");
#else
			expand(p, FOREFF, "\tlda\tA1,AL\n");
#endif
			return;
		}
		expand(p, FOREFF, "\tldZB\tA1");
#ifdef SEG
		printf(",r%s\n", rnames[q->rval]);
#else
		printf(",%s\n", rnames[q->rval]);
#endif
		return;
	case 'M':
		/* load the operand into U1 and extend it */
		q = p->left;
		t = q->type;
		if (t == 12)
			expand(p, FOREFF, "\tldk\tU1,#0\n");
		if (q->op != REG || getlr(p, '1')->rval + 1 != q->rval)
			expand(q, FOREFF, "\tldZB\tU1,AL\n");
		if (t == 2)
			expand(p, FOREFF, "\textsb\tU1\n");
		if (!(dope[p->op] & DIVFLAG))
			return;
		if (t >= 12 && t <= 15)
			expand(p, FOREFF, "\tldk\tA1,#0\n");
		else
			expand(p, FOREFF, "\texts\tVA1\n");
		return;
	case 'S':
	case 's':
		/* the structure assignment: a word, a long or a block move */
		if (p->op != STASG)
			cerror("zzzcode: bad ZS");
		q = p->left;
		r = p->right;
		n = p->stsize;
		rg = getlr(p, '1');
		rn = rg->rval;
		name = rnames[rn];
		k = szty(rg->type) == 2 ? 5 : 4;
		cls = rclass(rg->type, -1);
		rp = rfind(rn, cls);
		rp->busy = 1;
		tmp = 0;
		if (n <= 4) {
			if (r->op == ICON) {
				tmp = tcopy(r);
				r->op = NAME;
				r->type = DECREF(r->type);
			} else if (!tshape(r, STAREG | SAREG))
				order(r, STAREG | SAREG);
			if (r->op == REG) {
				if (notoff(r->type, r->rval))
					order(r, STAREG);
				tmp = tcopy(r);
				r->op = OREG;
				r->sname = 0;
				r->stsize = 0;
			}
			printf("\tld");
			if (n == 4) {
				printf("l\tr%s,", name);
				rmode++;
			} else
				printf("\t%s,", name);
			expand(p, FOREFF, "AR\n\tld");
			if (n == 4) {
				putchar('l');
				rmode++;
			}
			expand(p, FOREFF, "\tAL,");
			printf(n == 4 ? "r%s\n" : "%s\n", name);
			rfree(rn, k);
			if (tmp) {
				fwalk(r, rfreenode);
				tfree(r);
				p->right = tmp;
			}
		} else {
			if (!tshape(r, STAREG))
				order(r, STAREG);
			if (notoff(r->type, r->rval))
				order(r, STAREG);
			if (q->op == OREG && q->lval == 0 && q->sname == 0 && q->rval < 0x100) {
				tmp = tcopy(q);
				q->op = REG;
				if ((q->type & 0x30) != 0x10)
					q->type = INCREF(q->type);
			}
			if (!tshape(q, STAREG)) {
				if (!tmp)
					tmp = tcopy(q);
				order(q, STAREG);
			}
			if (notoff(q->type, q->rval))
				order(q, STAREG);
			n >>= 1;
			if (n < 16)
				printf("\tldk\t%s,#%d\n", name, n);
			else
				printf("\tld\t%s,#%d\n", name, n);
#ifdef SEG
			expand(p, FOREFF, "\tldir\t@rAL,@rAR,");
#else
			expand(p, FOREFF, "\tldir\t@AL,@AR,");
#endif
			printf("%s\n", name);
			rfree(rn, k);
			if (c == 's') {
				if (!tmp || ((tmp->op == REG || tmp->op == OREG) && tmp->rval == q->rval))
#ifdef SEG
					printf("\tsub\t%s,#%d\n", rnames[q->rval + 1], n * 2);
#else
					printf("\tsub\t%s,#%d\n", rnames[q->rval], n * 2);
#endif
			} else
#ifdef SEG
				printf("\tsub\t%s,#%d\n", rnames[r->rval + 1], n * 2);
#else
				printf("\tsub\t%s,#%d\n", rnames[r->rval], n * 2);
#endif
			if (tmp) {
				if (c == 's') {
					fwalk(q, rfreenode);
					tfree(q);
					p->left = tmp;
				} else {
					fwalk(tmp, rfreenode);
					tfree(tmp);
				}
			}
		}
		if (c == 's')
			p->type = p->left->type;
		return;
	case 'A':
		/* an argument that is a structure: up to 4 bytes through a register, else ldm */
		if (p->op != STARG)
			cerror("zzzcode: bad ZA");
		q = p->left;
		n = p->stsize;
		if (n <= 0 || n > 0x6b5d)
			cerror("structure size out of range: %ld", (long)n);
		if (n > 4) {
			if (!tshape(q, STAREG))
				order(q, STAREG);
			if (notoff(q->type, q->rval))
				order(q, STAREG);
			n >>= 1;
			printf("\tldm\t%s,", rnames[p->rall & 0xEFFF]);
#ifdef SEG
			expand(p, FOREFF, "@VAL");
#else
			expand(p, FOREFF, "@AL");
#endif
			printf(",#%d\n", n);
			return;
		}
		if (q->op == ICON)
			q->op = NAME;
		if (!tshape(q, SNAME | STAREG | SAREG))
			order(q, SNAME | STAREG | SAREG);
		if (q->op == REG) {
			if (notoff(q->type, q->rval))
				order(q, STAREG);
			q->op = OREG;
			q->sname = 0;
			q->stsize = 0;
		}
		printf("\tld");
		if (n == 4) {
			printf("l\tr");
			rmode++;
		} else
			putchar('\t');
		m = p->rall & 0xEFFF;
		printf("%s,", rnames[m]);
		expand(p, FOREFF, "AL\n");
		k = getreg(n == 4 ? 3 : 2, m, 0x1000, p->sname);
		if (m == k) {
			rbusy(m, n == 4 ? 5 : 4, p->sname, p->lval);
			return;
		}
		rfree(k, n == 4 ? 5 : 4);
		rsreturn(n == 4 ? 3 : 2, k);
		return;
	case 'T':
		/* the second word of a long operand, for the conversion to a shorter type */
		m = p->type;
		p = p->left;
		k = p->type;
		switch (p->op) {
		case NAME:
		case OREG:
#ifdef SEG
			if (p->rval != 15) {
				order(p, 6);
				goto longreg;
			}
#endif
			if (p->sname->sclass == 3 && p->op == NAME && !(p->sname->sflags >> 2 & 1) && p->rval >= 0) {
				if (szty(k) == 2) {
					if (m == 2 || m == 12)
						p->rval += 3;
					else
						p->rval += 2;
				} else
					p->rval += 1;
				return;
			}
			if (szty(k) == 2) {
				if (m == 2 || m == 12)
					p->lval += 3;
				else
					p->lval += 2;
			} else
				p->lval += 1;
			return;
		case REG:
#ifdef SEG
		longreg:
#endif
			if (szty(k) != 2)
				return;
			rfree(p->rval, p->type);
			rfreeall(0);
			p->rval++;
			p->type = m;
			if (p->rval <= 7 && getreg(2, p->rval, 0x1000, 0) != p->rval)
				cerror("zzzcode: case 'T'  reg not found");
			return;
		default:
			cerror("Illegal ZT type conversion");
			return;
		}
	case 'P':
		/* the argument registers r4 .. r7 on the use list are free again after a call */
		for (rp = rsuse; rp; rp = rn2) {
			rn2 = rp->next;
			switch (rp->reg) {
			case 4:
			case 5:
			case 6:
			case 7:
				rsreturn(9, rp->reg);
			}
		}
		return;
	default:
		cerror("illegal zzzcode: %c", c);
	}
}

/* the table of the conditional jumps by relation */
static char *jprtab[] = {
	"\tjpr\teq,L%d\n",
	"\tjpr\tne,L%d\n",
	"\tjpr\tle,L%d\n",
	"\tjpr\tlt,L%d\n",
	"\tjpr\tge,L%d\n",
	"\tjpr\tgt,L%d\n",
	"\tjpr\tule,L%d\n",
	"\tjpr\tult,L%d\n",
	"\tjpr\tuge,L%d\n",
	"\tjpr\tugt,L%d\n",
	"\tjpr\tfeq,L%d\n",
	"\tjpr\tfneu,L%d\n",
	"\tjpr\tfle,L%d\n",
	"\tjpr\tflt,L%d\n",
	"\tjpr\tfge,L%d\n",
	"\tjpr\tfgt,L%d\n",
};

/* the relation with the operands exchanged (EQ stays EQ, LE becomes GE ...) */
static int flip[] = {
	EQ, NE, GE, GT, LE, LT, UGE, UGT, ULE, ULT,
};

int cbflag;			/* 0xa152 */

/* a jump to the label lab when the relation o holds (o 0: always); mode 'F': the operands are exchanged */
void
cbgen(o, lab, mode)
	int o;
	int lab;
	int mode;
{
	if (o == 0) {
		printf("\tjpr\tL%d\n", lab);
		return;
	}
	if (o > FGT) {
		cerror("bad conditional branch: %s", opst[o]);
		return;
	}
	if (mode == 'F')
		o = flip[o - EQ];
	printf(jprtab[o - EQ], lab);
	cbflag = 0;
}
