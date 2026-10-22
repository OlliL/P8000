/*
 * z21.c -- popt: main program, input, output, jump optimizations
 */
#include "z2.h"

static struct optab *ophash[OPHS];
static struct node *labhash[LABHS];
static char tmpop[32];
static char tmpcc[32];

static int getln(void);
static int input(void);
static void output(struct node *);
static void prefix(struct node *);
static struct node *jswcheck(void);
static int cclook(int);
static void backjmp(struct node *, struct node *);
static void statline(int, char *);

int
main(int argc, char **argv)
{
	int niter, maxiter, isend, nflag;

	if (argc > 1 && argv[1][0] == '+') {
		argc--;
		argv++;
		debug++;
	}
	nflag = 0;
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd') {
		argc--;
		argv++;
		nflag++;
	}
	keepcmt = 0;
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'l') {
		argc--;
		argv++;
		keepcmt++;
	}
	if (argc > 1) {
		if (freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "C2: can't find %s\n", argv[1]);
			exit(1);
		}
	}
	if (argc > 2) {
		if (freopen(argv[2], "w", stdout) == NULL) {
			fprintf(stderr, "C2: can't create %s\n", argv[2]);
			exit(1);
		}
	}
	maxiter = 0;
	opsetup();
	do {
		isend = input();
		movedat();
		niter = 0;
		do {
			refcount();
			do {
				iterate();
				clearreg();
				niter++;
			} while (nchange);
			comjump();
			rmove();
		} while (nchange || jumpsw());
		shortjump();
		if (!flag136a)
			procopt();
		{
			struct node *p;

			for (p = first.forw; p; p = p->forw)
				output(p);
		}
		heapreset();
		if (niter > maxiter)
			maxiter = niter;
	} while (isend);
	if (nflag) {
		statline(maxiter, "iterations");
		statline(nrevbr, "branches reversed");
		statline(nclr, "clears \"simpilified\"");
		statline(ncmot, "code motions");
		statline(ncomj, "common seqs before jp's");
		statline(nxjump, "cross-jumps");
		statline(nsob, "djnz's added");
		statline(iaftbr, "inst. after jumps");
		statline(njp1, "jumps to .+2");
		statline(nbrbr, "jumps to jumps");
		statline(nlda, "lda + ld modified");
		statline(loopiv, "loops inverted");
		statline(ndirect, "operations made more direct");
		statline(npushpop, "push and pops generated");
		statline(npushdirect, "push/pop's made more direct");
		statline(nredunj, "redundant jumps");
		statline(nrlab, "redundant labels");
		statline(redunm, "redundant ld's");
		statline(nrtst, "redundant test's");
		statline(nrename, "registers renamed");
		statline(nshft, "shifts modified");
		statline(nsaddr, "simplified addresses");
		statline(nskip, "skips over jumps");
		fprintf(stderr, "%dK core\n", (int)(((heaplim + 0x3ff) >> 10) & 0x3f));
	}
	exit(0);
	return 0;
}

static void
statline(int n, char *s)
{
	if (n) {
		fprintf(stderr, "%d ", n);
		fprintf(stderr, "%s\n", s);
	}
}

/*
 * Read one function (everything up to and including the closing '}'
 * line) into the node list.  Lines before the '{' are copied to the
 * output unchanged.  Returns 0 at end of input, 1 after a function.
 */
static int
input(void)
{
	struct node *p, *lastp;
	int oper, op, echo;

	echo = 1;
	lastp = &first;
	for (;;) {
		if (echo) {
			for (;;) {
				oper = getln();
				op = oper & 0xff;
				if (op == LABEL) {
					printf("%s:\n", line);
				} else if (op == OBRACE) {
					echo = 0;
					printf("%s\n", line);
					oper = getln();
					break;
				} else if (op == EOFOP) {
					echo = 0;
					break;
				} else
					printf("%s\n", line);
			}
		} else
			oper = getln();
		op = oper & 0xff;
		switch (op) {
		case 0:
			if (*curlp == 0)
				continue;
			p = allocnode();
			p->op = op;
			p->code = copy(1, line, 0);
			p->labno = 0;
			p->subop = 0;
			break;
		case CJP:
		case JP:
			p = allocnode();
			p->op = op;
			p->subop = oper >> 8;
			if (*curlp == 'L' && (p->labno = getnum(curlp + 1)) != 0)
				p->code = 0;
			else {
				p->labno = 0;
				p->code = copy(1, curlp, 0);
			}
			break;
		case LABEL:
			p = allocnode();
			if (line[0] == 'L') {
				p->op = LABEL;
				p->subop = 0;
				p->labno = getnum(line + 1);
				p->code = 0;
			} else {
				p->op = DLABEL;
				p->subop = 0;
				p->labno = 0;
				p->code = copy(1, line, 0);
			}
			break;
#ifdef SPOPT
		case LONG:
#else
		case WORD:
#endif
		case ADDR:
			p = jswcheck();
			if (p == 0)
				goto dflt;
			break;
		case COMMENT:
			continue;
		case CBRACE:
		case KCOMMENT:
			p = allocnode();
			p->op = op;
			p->code = copy(1, line, 0);
			p->labno = 0;
			p->subop = 0;
			break;
		default:
		dflt:
			p = allocnode();
			p->op = op;
			p->subop = oper >> 8;
			p->labno = 0;
			p->code = copy(1, curlp, 0);
			break;
		}
		p->forw = 0;
		p->back = lastp;
		lastp->forw = p;
		lastp = p;
		p->ref = 0;
		if (oper == EOFOP)
			return 0;
		if (oper == CBRACE)
			return 1;
	}
}

/* .word / .addr with an Lnnn operand becomes a JSW node */
static struct node *
jswcheck(void)
{
	char *p;
	struct node *n;

	for (p = curlp; *p; p++)
		if (*p == '\n' || *p == 'L')
			break;
	if (*p == 'L') {
		n = allocnode();
		n->labno = getnum(p + 1);
		if (n->labno) {
			n->code = 0;
			n->op = JSW;
			n->subop = 0;
			return n;
		}
	}
	return 0;
}

static int
getch(void)
{
	return getc(stdin);
}

static int
getln(void)
{
	char *lp;
	int c;

	lp = line;
	do
		c = getch();
	while (c == ' ' || c == '\t');
	for (;;) {
		if (c == ':') {
			c = getch();
			if (c == '\n' || c == ' ' || c == '\t') {
				*lp++ = 0;
				return LABEL;
			}
			*lp++ = ':';
		}
		if (c == '\n') {
			*lp++ = 0;
			return oplook();
		}
		if (lp >= &line[LSIZE - 2]) {
			fprintf(stderr, "C2: Sorry, input line too long\n");
			exit(1);
		}
		*lp++ = c;
		c = getch();
		if (c == EOF) {
			*lp++ = 0;
			return EOFOP;
		}
	}
}

int
getnum(char *ap)
{
	char *p;
	short n;
	int c;

	p = ap;
	n = 0;
	while ((c = (signed char)*p++) >= '0' && c <= '9')
		n = n * 10 + c - '0';
	if (*--p != 0)
		return 0;
	return n;
}

/*
 * Print a node in assembler syntax.
 */
static void
output(struct node *n)
{
	struct optab *t;
	int sub;

	switch (n->op) {
	case 0:
		if (n->code)
			printf("\t%s\n", n->code);
		return;
	case JP:
		prefix(n);
		if (n->code)
			printf("%s\n", n->code);
		else
			printf("L%d\n", n->labno);
		return;
	case LABEL:
		printf("L%d:\n", n->labno);
		return;
	case DLABEL:
		printf("%s:\n", n->code);
		return;
	case JSW:
#ifdef SPOPT
		printf("\t.long\tL%d\n", n->labno);
#else
		printf("\t.word\tL%d\n", n->labno);
#endif
		return;
#ifdef SPOPT
	case PSEC:
		n->subop = 0;		/* movedat() leaves the kind of the section here */
		lastseg = n->op;
		break;
#endif
	case CODE:
	case DATA:
		lastseg = n->op;
		break;
	case SOB:
		if (n->subop == SBYTE)
			printf("\tdjnzb");
		else
			printf("\tdjnz");
		printf("\t%s", n->code);
		if (n->labno)
			printf(",L%d", n->labno);
		putchar('\n');
		return;
	case CALL:
		prefix(n);
		if (n->code)
			printf("%s\n", n->code);
		return;
	case EOFOP:
		return;
	case CBRACE:
		printf("\t%s\n", n->code);
		return;
	case KCOMMENT:
		printf("%s\n", n->code);
		return;
	}
	sub = n->subop;
	if (sub == SBYTE || sub == SLONG || sub == SLDK)
		n->subop = 0;
	for (t = optab; t->name; t++) {
		if ((t->opcode & 0xff) == n->op &&
		    (signed char)(t->opcode >> 8) == n->subop) {
			if (n->op == CJP) {
				prefix(n);
				printf("%s", t->name);
			} else
				printf("\t%s", t->name);
			if (sub == SBYTE)
				putchar('b');
			else if (sub == SLONG)
				putchar('l');
			else if (sub == SLDK)
				putchar('k');
			break;
		}
	}
	if (n->op == CJP) {
		if (n->code)
			printf(",%s", n->code);
		else
			printf(",L%d", n->labno);
	} else if (n->code)
		printf("\t%s", n->code);
	putchar('\n');
	n->subop = sub;
}

/*
 * Print the mnemonic of a jump or call: absolute (jp/call) when the
 * operand is indirect or indexed, relative (jpr/callr) otherwise.
 */
static void
prefix(struct node *n)
{
	char *s, *p;
	int call;

	call = n->op == CALL;
	if (n->code) {
		if (n->code[0] == '@')
			s = call ? "\tcall\t" : "\tjp\t";
		else {
			for (p = n->code; *p && *p != '('; p++)
				;
			if (*p)
				s = call ? "\tcall\t" : "\tjp\t";
			else
				s = call ? "\tcallr\t" : "\tjpr\t";
		}
	} else
		s = call ? "\tcallr\t" : "\tjpr\t";
	printf("%s", s);
}

char *
copy(int na, char *a, char *b)
{
	char *p, *np, *onp;
	int n;

	if (*a == 0)
		return 0;
	n = strlen(a) + 1;
	if (na > 1)
		n += strlen(b);
	onp = np = alloc(n);
	for (p = a; (*np++ = *p++) != 0;)
		;
	if (na > 1) {
		np--;
		for (p = b; (*np++ = *p++) != 0;)
			;
	}
	return onp;
}

static int
hashname(char *s)
{
	int c0, c1, c2;

	c0 = (signed char)s[0];
	c1 = c0 ? (signed char)s[1] : 0;
	c2 = c1 ? (signed char)s[2] : 0;
	return (((c0 << 3) + (c1 + c1) + c2) & 0x7fff) % OPHS;
}

void
opsetup(void)
{
	struct optab *o;
	int h;

	for (o = optab; o->name; o++) {
		h = hashname(o->name);
		while (ophash[h]) {
			if (++h >= OPHS)
				h = 0;
		}
		ophash[h] = o;
	}
}

/*
 * Look up a condition code after a "jpr": the mnemonic in `opw' is
 * returned unchanged unless the operand starts with "cc,".
 */
static int
cclook(int opw)
{
	char *p, *q;
	int h, start;
	struct optab *o;
	char *a, *b;

	q = tmpcc;
	p = curlp;
	while (*p && *p != '\n' && *p != ',')
		*q++ = *p++;
	*q++ = 0;
	if (*p != ',')
		return opw;
	curlp = p + 1;
	h = start = hashname(tmpcc);
	while ((o = ophash[h]) != 0) {
		a = o->name;
		b = tmpcc;
		while (*a == *b) {
			if (*b == 0)
				return o->opcode;
			a++;
			b++;
		}
		if (++h >= OPHS)
			h = 0;
	}
	return opw;
}

/*
 * Classify the line in `line': returns the opcode word, with the
 * subop in the high byte, or 0 for an unknown instruction.  Sets curlp
 * to the operand field.
 */
int
oplook(void)
{
	char *lp, *np, *name, *tp;
	int h, start, suffix;
	struct optab *o;

	if (line[0] == 0) {
		curlp = line;
		return 0;
	}
	tmpop[0] = tmpop[1] = tmpop[2] = 0;
	np = tmpop;
	for (lp = line; *lp && *lp != ' ' && *lp != '\t';)
		*np++ = *lp++;
	*np++ = 0;
	while (*lp == '\t' || *lp == ' ')
		lp++;
	curlp = lp;
	if (line[0] == '/' && line[1] == '*') {
		if (line[2] == '~' && keepcmt)
			return KCOMMENT;
		return COMMENT;
	}
	h = start = hashname(tmpop);
	suffix = np[-2] == 'b' || np[-2] == 'l';
	for (;;) {
		o = ophash[h];
		if (o) {
			name = o->name;
			tp = tmpop;
			while (*name == *tp) {
				if (*tp == 0) {
					/* exact match */
					if ((o->opcode & 0xff) == JP)
						return cclook(o->opcode);
					if (o->opcode == JPX)
						return JP;
					return o->opcode;
				}
				name++;
				tp++;
			}
			if (*tp == 'l' && tp[1] == 0 && *name == 0)
				return o->opcode + 0x1100;
			if (*tp == 'b' && tp[1] == 0 && *name == 0)
				return o->opcode + 0x1000;
		}
		if (++h >= OPHS)
			h = 0;
		if (h == start)
			break;
		if (!suffix && ophash[h] == 0)
			break;
	}
	curlp = line;
	return 0;
}

/*
 * Count the references to every label, resolve the reference of every
 * jump, and delete labels nobody refers to.
 */
void
refcount(void)
{
	struct node *p, *lp, *tp;
	int i, n;

	for (i = 0; i < LABHS; i++)
		labhash[i] = 0;
	for (p = first.forw; p; p = p->forw)
		if (p->op == LABEL) {
			labhash[p->labno % LABHS] = p;
			p->refc = 0;
		}
	for (p = first.forw; p; p = p->forw) {
		if (p->op == CJP || p->op == JP || p->op == JSW) {
			p->ref = 0;
			lp = labhash[p->labno % LABHS];
			if (lp == 0 || p->labno != lp->labno)
				for (lp = first.forw; lp; lp = lp->forw)
					if (lp->op == LABEL && p->labno == lp->labno)
						break;
			if (lp) {
				tp = nonlab(lp)->back;
				while (tp->back && tp->op == KCOMMENT)
					tp = tp->back;
				if (tp != lp) {
					p->labno = tp->labno;
					lp = tp;
				}
				p->ref = lp;
				lp->refc++;
			}
		} else if (p->op == LD) {
			/* immediate "#Lnnn" operands refer to labels too */
			dualop(p);
			if (RT1[0] == '#' && RT1[1] == 'L') {
				n = getnum(RT1 + 2);
				lp = labhash[n % LABHS];
				if (lp == 0 || n != lp->labno)
					for (lp = first.forw; lp; lp = lp->forw)
						if (lp->op == LABEL && n == lp->labno)
							break;
				if (lp)
					lp->refc++;
			}
		}
	}
	for (p = first.forw; p; p = p->forw)
		if (p->op == LABEL && p->refc == 0) {
			lp = nonlab(p);
			if (lp->op == 0 || lp->op == JSW || lp->op == WORD ||
			    lp->op == LONG)
				continue;
			decref(p);
		}
}

void
iterate(void)
{
	struct node *p, *rp, *p1, *start;

	nchange = 0;
	for (p = first.forw; p; p = p->forw) {
		if ((p->op == JP || p->op == CJP || p->op == JSW) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op == JP && rp->labno && p->labno != rp->labno) {
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
		if (p->op == CJP) {
			p1 = p->forw;
			while (p1->op == KCOMMENT)
				p1 = p1->forw;
			if (p1->op == JP) {
				rp = p->ref;
				do
					rp = rp->back;
				while (rp->op == LABEL || rp->op == KCOMMENT);
				if (rp == p1) {
					decref(p->ref);
					p->ref = p1->ref;
					p->labno = p1->labno;
					p1->forw->back = p;
					p->forw = p1->forw;
					p->subop = revbr[(int)p->subop];
					nchange++;
					nskip++;
				}
			}
		}
		if (p->op == JP || p->op == RET) {
			start = p;
			while (p->forw->op == KCOMMENT)
				p = p->forw;
			while (p->forw) {
				p1 = p->forw;
				if (p1->op == LABEL || p1->op == DLABEL ||
				    p1->op == OBRACE || p1->op == CBRACE ||
				    p1->op == 0 || p1->op == DATA)
					break;
				nchange++;
				iaftbr++;
				if (p1->ref)
					decref(p1->ref);
				p->forw = p1->forw;
				p->forw->back = p;
			}
			p = start;
		}
		if (p->op == JP || p->op == CJP) {
			for (rp = p->forw; rp; rp = rp->forw) {
				if (rp->op != LABEL && rp->op != KCOMMENT)
					break;
				if (rp == p->ref) {
					p->back->forw = p->forw;
					p->forw->back = p->back;
					p = p->back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
			}
		}
		if (p->op == JP) {
			xjump(p);
			p = codemove(p);
		}
	}
}

void
xjump(struct node *p1)
{
	struct node *p2, *p3;

	if ((p2 = p1->ref) == 0)
		return;
	for (;;) {
		do
			p1 = p1->back;
		while (p1 && (p1->op == LABEL || p1->op == KCOMMENT));
		do
			p2 = p2->back;
		while (p2 && (p2->op == LABEL || p2->op == KCOMMENT));
		if (!equop(p1, p2) || p1 == p2)
			return;
		p3 = insertl(p2);
		p1->op = JP;
		p1->subop = 0;
		p1->ref = p3;
		p1->labno = p3->labno;
		p1->code = 0;
		nxjump++;
		nchange++;
	}
}

struct node *
insertl(struct node *oldp)
{
	struct node *lp;

	if (oldp->op == LABEL) {
		oldp->refc++;
		return oldp;
	}
	if (oldp->back->op == LABEL) {
		oldp = oldp->back;
		oldp->refc++;
		return oldp;
	}
	lp = allocnode();
	lp->op = LABEL;
	lp->subop = 0;
	lp->labno = isn++;
	lp->ref = 0;
	lp->code = 0;
	lp->refc = 1;
	lp->back = oldp->back;
	lp->forw = oldp;
	oldp->back->forw = lp;
	oldp->back = lp;
	return lp;
}

struct node *
codemove(struct node *p)
{
	struct node *p1, *p2, *p3, *t, *tl;
	int n;

	p1 = p;
	if (p1->op != JP)
		return p1;
	p2 = p1->ref;
	if (p2 == 0)
		return p1;
	while (p2->op == LABEL || p2->op == KCOMMENT)
		if ((p2 = p2->back) == 0)
			return p1;
	if (p2->op == JP || p2->op == RET) {
		p2 = p2->forw;
		for (p3 = p1->ref; p3; p3 = p3->forw) {
			if (p3->op == JP) {
				if (p1 == p3)
					return p1;
				ncmot++;
				nchange++;
				p1->back->forw = p2;
				p1->forw->back = p3;
				p2->back->forw = p3->forw;
				p3->forw->back = p2->back;
				p2->back = p1->back;
				p3->forw = p1->forw;
				decref(p1->ref);
				return p2;
			}
		}
		return p1;
	}
	/* loop inversion */
	p3 = p1;
	while (p3->forw->op == KCOMMENT)
		p3 = p3->forw;
	if (p3->forw->op != LABEL)
		return p1;
	p1->forw = p3->forw;
	p3->forw->back = p1;
	while (p2->forw->op == KCOMMENT)
		p2 = p2->forw;
	p2 = p2->forw;
	p3 = p2;
	n = 24;
	for (;;) {
		if ((p3 = p3->forw) == 0 || p3 == p1 || --n == 0)
			return p1;
		if (p3->op == CJP && p3->labno == p1->forw->labno)
			break;
	}
	do
		if ((p1 = p1->back) == 0)
			return p;
	while (p1 != p3);
	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[(int)p3->subop];
	decref(p3->ref);
	p2->back->forw = p1;
	p3->forw->back = p1;
	p1->back->forw = p2;
	p1->forw->back = p3;
	t = p1->back;
	p1->back = p2->back;
	p2->back = t;
	t = p1->forw;
	p1->forw = p3->forw;
	p3->forw = t;
	p2 = insertl(p1->forw);
	p3->labno = p2->labno;
	p3->ref = p2;
	decref(tl);
	if (tl->refc <= 0)
		nrlab--;
	loopiv++;
	nchange++;
	return p3;
}

void
comjump(void)
{
	struct node *p1, *p2, *p3;

	for (p1 = first.forw; p1; p1 = p1->forw)
		if (p1->op == JP && (p2 = p1->ref) && p2->refc > 1)
			for (p3 = p1->forw; p3; p3 = p3->forw)
				if (p3->op == JP && p3->ref == p2)
					backjmp(p1, p3);
}

static void
backjmp(struct node *p1, struct node *p2)
{
	struct node *p3;

	for (;;) {
		while ((p1 = p1->back) && p1->op == LABEL)
			;
		p2 = p2->back;
		if (equop(p1, p2)) {
			p3 = insertl(p1);
			p2->back->forw = p2->forw;
			p2->forw->back = p2->back;
			p2 = p2->forw;
			decref(p2->ref);
			p2->labno = p3->labno;
			p2->ref = p3;
			nchange++;
			ncomj++;
		} else
			return;
	}
}
