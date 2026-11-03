/*
 * z21.c -- popt: main program, input, output, jump optimizations
 */
#include "z2.h"

extern char _sobuf[];

static char sccsid[] = "@[$]z21.c\t1.4  12/21/82 17:58:05 Copr.1981 87wega3.2.";

struct optab *ophash[OPHS];

/* hash of the first three characters, read without a test as the original does (names "{" and "}" are padded with a NUL, see z20.c) */
#define HASHNAME(s) (((((int)(char)(s)[0] << 3) + 2 * (int)(char)(s)[1] + (int)(char)(s)[2]) & 0x7fff) % OPHS)

static int getln();
static int input();
static void output();
static void prefix();
static struct node *jswcheck();
static int cclook();
static void backjmp();
static void statline();
int getnum();
static void dumpnodes();
char * copy();
void opsetup();
int oplook();
void refcount();
void iterate();
void xjump();
#ifdef OLD_D20B
static int d20bover();
#endif
struct node * insertl();
struct node * codemove();
#ifdef OLD_D20C
static void cjmove();
#endif
void comjump();























int
main(argc, argv)
int argc;
char **argv;
{
	register int niter, maxiter, isend;
	int nflag;

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
		} else
			setbuf(stdout, _sobuf);
	}
	heapcur = heapbase = heaplim = (unsigned short)sbrk(2);
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
#ifdef TRACE
				if (debug) {
					printf("/*TRACE iterate %d*/\n", niter);
					dumpnodes();
				}
#endif
			} while (nchange);
			comjump();
#ifdef TRACE
			if (debug) {
				printf("/*TRACE comjump*/\n");
				dumpnodes();
			}
#endif
			rmove();
		} while (nchange || jumpsw());
		shortjump();
		if (!flag136a)
			procopt();
		dumpnodes();
		if (niter > maxiter)
			maxiter = niter;
		heapcur = heapbase;
	} while (isend);
	if (nflag) {
		statline("iterations", maxiter);
		statline("branches reversed", nrevbr);
		statline("clears \"simpilified\"", nclr);
		statline("code motions", ncmot);
		statline("common seqs before jp's", ncomj);
		statline("cross-jumps", nxjump);
		statline("djnz's added", nsob);
		statline("inst. after jumps", iaftbr);
		statline("jumps to .+2", njp1);
		statline("jumps to jumps", nbrbr);
		statline("lda + ld modified", nlda);
		statline("loops inverted", loopiv);
		statline("operations made more direct", ndirect);
		statline("push and pops generated", npushpop);
		statline("push/pop's made more direct", npushdirect);
		statline("redundant jumps", nredunj);
		statline("redundant labels", nrlab);
		statline("redundant ld's", redunm);
		statline("redundant test's", nrtst);
		statline("registers renamed", nrename);
		statline("shifts modified", nshft);
		statline("simplified addresses", nsaddr);
		statline("skips over jumps", nskip);
		fprintf(stderr, "%dK core\n", (int)(((heaplim + 0x3ff) >> 10) & 0x3f));
	}
	exit(0);
}

/* .word / .addr with an Lnnn operand becomes a JSW node */
static struct node *
jswcheck()
{
	register struct node *n;
	register char *p;

	for (p = curlp; *p; p++)
		if (*p == '\n' || *p == 'L')
			break;
	if (*p == 'L') {
		n = (struct node *)alloc(14);
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

static void
statline(s, n)
char *s;
register int n;
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
input()
{
	register struct node *p, *lastp;
	register int oper;
	char echo;

	echo = 1;
	lastp = &first;
	for (;;) {
		if (echo)
			for (;;) {
				oper = getln();
				switch (oper & 0xff) {
				case LABEL:
					printf("%s:\n", line);
					continue;
				case EOFOP:
					echo = 0;
					goto sw;
				case OBRACE:
					echo = 0;
					printf("%s\n", line);
					break;
				default:
					printf("%s\n", line);
					continue;
				}
				break;
			}
		oper = getln();
	sw:
		switch (oper & 0xff) {
		case LABEL:
			p = (struct node *)alloc(14);
			if (line[0] == 'L') {
				p->op = LABEL;
				p->subop = 0;
				p->labno = getnum(line + 1);
				p->code = 0;
			} else {
				p->op = DLABEL;
				p->subop = 0;
				p->labno = 0;
				p->code = copy(1, line);
			}
			break;
#ifdef SPOPT
		case LONG:
#else
		case WORD:
#endif
		case ADDR:
			if ((p = jswcheck()) != 0)
				break;
		default:
			p = (struct node *)alloc(14);
			p->op = oper & 0xff;
			p->subop = oper >> 8;
			p->labno = 0;
			p->code = copy(1, curlp);
			break;
		case CJP:
		case JP:
			p = (struct node *)alloc(14);
			p->op = oper & 0xff;
			p->subop = oper >> 8;
			if (*curlp == 'L' && (p->labno = getnum(curlp + 1)) != 0)
				p->code = 0;
			else {
				p->labno = 0;
				p->code = copy(1, curlp);
			}
			break;
		case COMMENT:
			oper = getln();
			goto sw;
		case 0:
			if (*curlp == 0) {
				oper = getln();
				goto sw;
			}
		case CBRACE:
		case KCOMMENT:
			p = (struct node *)alloc(14);
			p->op = oper & 0xff;
			p->code = copy(1, line);
			p->labno = 0;
			p->subop = 0;
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

static int
getln()
{
	register char *lp;
	register int c;

	lp = line;
	do
		c = getc(stdin);
	while (c == ' ' || c == '\t');
	for (;;) {
		if (c == ':') {
			c = getc(stdin);
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
		c = getc(stdin);
		if (c == EOF) {
			*lp++ = 0;
			return EOFOP;
		}
	}
}

int
getnum(p)
register char *p;
{
	register int n;
	register int c;

	n = 0;
	while ((c = (char)*p++) >= '0' && c <= '9')
		n = n * 10 + c - '0';
	if (*--p != 0)
		return 0;
	return n;
}

static void
dumpnodes()
{
	register struct node *p;

	for (p = &first; (p = p->forw) != 0;)
		output(p);
}

/*
 * Print the mnemonic of a jump or call: absolute (jp/call) when the
 * operand is indirect or indexed, relative (jpr/callr) otherwise.
 */
static void
prefix(n)
register struct node *n;
{
	register char *p;
	register int op;

	op = n->op;
	if (n->code) {
		if (n->code[0] == '@')
			printf(op == CALL ? "\tcall\t" : "\tjp\t");
		else {
			for (p = n->code; *p && *p != '('; p++)
				;
			if (*p)
				printf(op == CALL ? "\tcall\t" : "\tjp\t");
			else
				printf(op == CALL ? "\tcallr\t" : "\tjpr\t");
		}
	} else
		printf(op == CALL ? "\tcallr\t" : "\tjpr\t");
}

/*
 * Print a node in assembler syntax.
 */
static void
output(n)
register struct node *n;
{
	register struct optab *t;
	register int sub;

	switch (n->op) {
	case CBRACE:
		printf("\t%s\n", n->code);
		return;
	case JP:
		prefix(n);
		if (n->code)
			printf("%s\n", n->code);
		else
			printf("L%d\n", n->labno);
		return;
	case CALL:
		prefix(n);
		if (n->code)
			printf("%s\n", n->code);
		return;
	case LABEL:
		printf("L%d:\n", n->labno);
		return;
	case DLABEL:
		printf("%s:\n", n->code);
		return;
#ifdef SPOPT
	case PSEC:
		n->subop = 0;		/* movedat() leaves the kind of the section here */
		lastseg = n->op;
		goto def;
#endif
	case CODE:
	case DATA:
		lastseg = n->op;
	default:
#ifdef SPOPT
	def:
#endif
		sub = n->subop;
		if (sub == SBYTE || sub == SLONG || sub == SLDK)
			n->subop = 0;
		for (t = optab; t->name; t++) {
			if ((t->opcode & 0xff) == n->op &&
			    (t->opcode >> 8) == n->subop) {
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
		return;
	case JSW:
#ifdef SPOPT
		printf("\t.long\tL%d\n", n->labno);
#else
		printf("\t.word\tL%d\n", n->labno);
#endif
		return;
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
	case EOFOP:
		return;
	case KCOMMENT:
		printf("%s\n", n->code);
		return;
	case 0:
		if (n->code)
			printf("\t%s\n", n->code);
		return;
	}
}

/* the node list on stderr */
dumperr()
{
	int fd;

	fd = stdout->_file;
	fflush(stdout);
	stdout->_file = stderr->_file;
	dumpnodes();
	fflush(stdout);
	stdout->_file = fd;
}

/* one node on stderr */
outerr(n)
struct node *n;
{
	int fd;

	fd = stdout->_file;
	fflush(stdout);
	stdout->_file = stderr->_file;
	output(n);
	fflush(stdout);
	stdout->_file = fd;
}

char *
copy(na, a, b)
int na;
char *a;
char *b;
{
	register char *p, *np;
	register int n;
	char *onp;

	p = a;
	n = 0;
	if (*p == 0)
		return 0;
	do
		n++;
	while (*p++);
	if (na > 1) {
		p = b;
		while (*p++)
			n++;
	}
	onp = np = alloc(n);
	for (p = a; (*np++ = *p++) != 0;)
		;
	if (na > 1) {
		p = b;
		np--;
		while ((*np++ = *p++) != 0)
			;
	}
	return onp;
}

void
opsetup()
{
	register struct optab *o;
	register struct optab **h;
	register char *s;

	for (o = optab; (s = o->name) != 0; o++) {
		h = &ophash[HASHNAME(s)];
		while (*h++)
			if (h >= &ophash[OPHS])
				h = ophash;
		*--h = o;
	}
}

/*
 * Look up a condition code after a "jpr": the mnemonic in `opw' is
 * returned unchanged unless the operand starts with "cc,".
 */
static int
cclook(opw)
int opw;
{
	register char *p, *q;
	struct optab **h, *o;
	static char tmpcc[32];

	q = tmpcc;
	p = curlp;
	while (*p && *p != '\n' && *p != ',')
		*q++ = *p++;
	*q++ = 0;
	if (*p != ',')
		return opw;
	curlp = p + 1;
	h = &ophash[HASHNAME(tmpcc)];
	while ((o = *h) != 0) {
		q = o->name;
		p = tmpcc;
		while (*q++ == *p)
			if (*p++ == 0)
				return o->opcode;
		h++;
		if (h >= &ophash[OPHS])
			h = ophash;
	}
	return opw;
}

/*
 * Classify the line in `line': returns the opcode word, with the
 * subop in the high byte, or 0 for an unknown instruction.  Sets curlp
 * to the operand field.
 */
int
oplook()
{
	register struct optab *o;
	register char *lp, *np;
	struct optab **h, **start;
	int op, suffix;
	static char tmpop[32];

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
	h = &ophash[HASHNAME(tmpop)];
	start = h;
	suffix = np[-2] == 'b' || np[-2] == 'l';
	for (;;) {
		o = *h;
		np = o->name;
		lp = tmpop;
		while (*np++ == *lp)
			if (*lp++ == 0) {
				/* exact match */
				if (((op = o->opcode) & 0xff) == JP)
					return cclook(op);
				if (op == JPX)
					return JP;
				return op;
			}
		if (*lp == 'l' && lp[1] == 0 && np[-1] == 0)
			return o->opcode + 0x1100;
		if (*lp++ == 'b' && *lp++ == 0 && *--np == 0)
			return o->opcode + 0x1000;
		h++;
		if (h >= &ophash[OPHS])
			h = ophash;
		if (h == start)
			break;
		if (!suffix && *h == 0)
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
refcount()
{
	register struct node *p, *lp;
	register struct node **h;
	register struct node *tp;
	register int n;
	static struct node *labhash[LABHS];

	for (h = labhash; h < &labhash[LABHS]; h++)
		*h = 0;
	for (p = first.forw; p; p = p->forw)
		if (p->op == LABEL) {
			labhash[p->labno % LABHS] = p;
			p->refc = 0;
		}
	for (p = first.forw; p; p = p->forw) {
		if (p->op == JP || p->op == CJP || p->op == JSW) {
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
			if ((lp = nonlab(p))->op == 0 || lp->op == JSW || lp->op == WORD ||
			    lp->op == LONG)
				continue;
			decref(p);
		}
}

void
iterate()
{
	register struct node *p, *rp, *p1;

	nchange = 0;
	for (p = first.forw; p; p = p->forw) {
		if ((p->op == JP || p->op == CJP || p->op == JSW) && p->ref) {
			rp = nonlab(p->ref);
			if (rp->op == JP && rp->labno && p->labno != rp->labno
#ifdef OLD_D20B
			    && !(p->op == CJP && d20bover(rp) && d20xinv(rp))
#endif
			) {
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				++rp->ref->refc;
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
			rp = p;
			while (p->forw->op == KCOMMENT)
				p = p->forw;
			while (p->forw) {
				if (p->forw->op == LABEL || p->forw->op == DLABEL ||
				    p->forw->op == OBRACE || p->forw->op == CBRACE ||
				    p->forw->op == 0 || p->forw->op == DATA)
					break;
				nchange++;
				iaftbr++;
				if (p->forw->ref)
					decref(p->forw->ref);
				p->forw = p->forw->forw;
				p->forw->back = p;
			}
			p = rp;
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
#ifdef OLD_D20X
			/* the old popt: a loop inversion (the target is not behind a jump) before the cross-jump */
			if (d20xinv(p)) {
				p = codemove(p);
				if (p->op == JP)
					xjump(p);
			} else {
				xjump(p);
				p = codemove(p);
			}
#else
			xjump(p);
			p = codemove(p);
#endif
		}
#ifdef OLD_D20C
		if (p->op == CJP)
			cjmove(p);
#endif
	}
}

#ifdef OLD_D20C
/* is l one of the labels of the run that starts at f */
static int
d20cin(f, l)
register struct node *f, *l;
{
	for (; f && (f->op == LABEL || f->op == KCOMMENT); f = f->forw)
		if (f == l)
			return 1;
	return 0;
}

/*
 * The old popt: a block T that a conditional jump p names, that ends with a jump and that the program reaches
 * otherwise only by falling out of a conditional jump c2 over it (c2 names the label right behind T), goes behind p,
 * when a label follows p: p and c2 are turned round (p falls into T, c2 jumps to T and falls behind it).
 */
static void
cjmove(p)
register struct node *p;
{
	register struct node *t0, *t1, *te, *c2, *x, *y;

	if ((t1 = p->ref) == 0)
		return;
	for (x = p->forw; x && x != t1; x = x->forw)	/* only a block behind p moves up */
		;
	if (x == 0)
		return;
	for (t0 = t1; t0->back && (t0->back->op == LABEL || t0->back->op == KCOMMENT); t0 = t0->back)
		;
	c2 = t0->back;
	if (c2 == 0 || c2->op != CJP || c2 == p || c2->ref == 0)
		return;
	for (te = t0; te && (te->op == LABEL || te->op == KCOMMENT); te = te->forw)
		if (te == p)
			return;
	for (; te && te->op != JP; te = te->forw)
		if (te == p || te->op == LABEL || te->op == CJP || te->op == JSW)
			return;			/* a label or a branch inside the block, or p in it */
	if (te == 0 || te->ref == 0)
		return;
	for (y = te->forw; y && (y->op == LABEL || y->op == KCOMMENT); y = y->forw)
		if (y == c2->ref)
			break;
	if (y != c2->ref)
		return;
	for (x = p->forw; x && x->op == KCOMMENT; x = x->forw)
		;
	if (x == 0 || x->op != LABEL)
		return;
	/* no jump (only conditional ones) leads to T */
	for (y = first.forw; y; y = y->forw)
		if (y->op == JP && y->ref && d20cin(t0, y->ref))
			return;
	y = c2->ref;
	/* not the then part of a diamond: T jumps right behind the block at y */
	for (y = nonlab(y); y && y->op != LABEL && y->op != JP && y->op != CJP && y->op != RET; y = y->forw)
		;
	for (; y && (y->op == LABEL || y->op == KCOMMENT); y = y->forw)
		if (y == te->ref)
			return;
	/* take the block t0 .. te out and put it behind p */
	c2->forw = te->forw;
	te->forw->back = c2;
	te->forw = p->forw;
	p->forw->back = te;
	p->forw = t0;
	t0->back = p;
	/* p falls into T now and jumps to x; c2 jumps to T and falls into y */
	p->subop = revbr[(int)p->subop];
	t1->refc++;
	y = c2->ref;
	c2->ref = t1;
	c2->labno = t1->labno;
	c2->subop = revbr[(int)c2->subop];
	decref(y);
	t1->refc--;
	p->ref = x;
	p->labno = x->labno;
	x->refc++;
	nchange++;
	ncmot++;
}
#endif

#ifdef OLD_D20B
/* `if (c) break;` as the end of a loop: in front of the label of the jump p stands a jump to the label behind p
   (`jpr L2; L1: jpr head; L2:`), and the first test of the loop asks for equality */
static int
d20bover(p)
register struct node *p;
{
	register struct node *j, *x;

	for (j = p->back; j && (j->op == LABEL || j->op == KCOMMENT); j = j->back)
		;
	if (j == 0 || j->op != JP || j == p)
		return 0;
	for (x = p->forw; x && (x->op == LABEL || x->op == KCOMMENT); x = x->forw)
		if (x == j->ref)
			break;
	if (x == 0 || x != j->ref)
		return 0;
	/* the first test of the loop asks for equality (a pointer that ends the loop) */
	for (x = p->ref; x && x != p && x->op != CJP; x = x->forw)
		;
	return x && x->op == CJP && x->subop == 0;
}
#endif


void
xjump(p1)
register struct node *p1;
{
	register struct node *p2, *p3;

	if ((p2 = p1->ref) == 0)
		return;
	for (;;) {
		while (p1 = p1->back)
			if (p1->op != LABEL && p1->op != KCOMMENT)
				break;
		while (p2 = p2->back)
			if (p2->op != LABEL && p2->op != KCOMMENT)
				break;
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
insertl(oldp)
register struct node *oldp;
{
	register struct node *lp;

	if (oldp->op == LABEL) {
		oldp->refc++;
		return oldp;
	}
	if (oldp->back->op == LABEL) {
		oldp = oldp->back;
		oldp->refc++;
		return oldp;
	}
	lp = (struct node *)alloc(14);
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


#ifdef OLD_D20X
/*
 * The old popt inverts the loop of the back jump p before the cross-jump when its target is not behind a jump or
 * return, its test comes within 5 nodes of its head, and the cross-jump would take one instruction at most (defid:
 * the store of temp; the getchar of the init and the step of a for loop are cross-jumped first: lxtitle, lxmore).
 */
static int
d20xinv(p)
register struct node *p;
{
	register struct node *p2;
	register struct node *x;
	register int n;

	if (p->op != JP || (p2 = p->ref) == 0)
		return 0;
	while (p2->op == LABEL || p2->op == KCOMMENT)
		if ((p2 = p2->back) == 0)
			return 0;
	if (p2->op == JP || p2->op == RET)
		return 0;
	/* and the test of the loop comes within a short reach of its head (5 nodes) */
	for (x = p->forw; x && x->op == KCOMMENT; x = x->forw)
		;
	if (x == 0 || x->op != LABEL)
		return 0;
	for (p2 = p->ref, n = 6; p2 && p2 != p && --n > 0; p2 = p2->forw)
		if (p2->op == CJP && p2->labno == x->labno)
			break;
	if (p2 == 0 || p2 == p || n <= 0)
		return 0;
	/* and the cross-jump at p would take one instruction at most */
	x = p;
	p2 = p->ref;
	for (n = 0; n < 2; n++) {
		do
			x = x->back;
		while (x && (x->op == LABEL || x->op == KCOMMENT));
		do
			p2 = p2->back;
		while (p2 && (p2->op == LABEL || p2->op == KCOMMENT));
		if (x == 0 || p2 == 0 || x == p2 || !equop(x, p2))
			break;
	}
	return n <= 1;
}
#endif

struct node *
codemove(p)
struct node *p;
{
	register struct node *p1, *p2, *p3;
	struct node *t, *tl;
	int n;
#ifdef OLD_D20
	struct node *rp;
#endif

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
#ifdef OLD_D20
			/* the old popt has removed a jump to the next label already: the block goes on behind it */
			if (p3->op == JP && p3->ref) {
				for (rp = p3->forw; rp && (rp->op == LABEL || rp->op == KCOMMENT); rp = rp->forw)
					if (rp == p3->ref)
						break;
				if (rp == p3->ref)
					continue;
			}
#endif
			if (p3->op == JP) {
				if (p1 == p3)
#ifdef OLD_D20L
				{
					/* the old popt: the block reaches the jump itself, and the jump in front of the
					   head leads into the loop: a loop to invert (optim) */
					for (t = p1->ref; t && t != p1; t = t->forw)
						if (t == p2->back->ref) {
							p2 = p2->back;	/* the node in front of the target again */
							goto inv;
						}
					return p1;
				}
#else
					return p1;
#endif
#ifdef TRACE
				if (debug) {
					register struct node *q;
					int tn, jn, jc, jl, pre, tc;

					tn = tc = 0;
					for (q = p2; q && q != p3->forw; q = q->forw) {
						if (q->op != LABEL && q->op != KCOMMENT)
							tn++;
						if (q->op == CJP)
							tc++;
					}
					jn = jc = jl = pre = 0;
					for (q = p1->back; q && q->op != LABEL; q = q->back) {
						if (q->op == KCOMMENT)
							continue;
						jn++;
						if (q->op == CJP)
							jc++;
						if (q->op == JP || q->op == RET || q->op == JSW)
							break;
					}
					if (q && q->op == LABEL) {
						jl = q->refc;
						while (q->back && (q->back->op == LABEL || q->back->op == KCOMMENT))
							q = q->back;
						pre = q->back && (q->back->op == JP || q->back->op == RET);
					}
					printf("/*CM L%d refc %d tlen %d tcjp %d tend %d jlen %d jcjp %d jlabrefc %d jpre %d*/\n", p1->labno,
					    p1->ref->refc, tn, tc, p3->ref == 0 ? -1 : p3->labno, jn, jc, jl, pre);
				}
#endif
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
#ifdef OLD_D20L
inv:
#endif
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
comjump()
{
	register struct node *p1, *p2, *p3;

	for (p1 = first.forw; p1; p1 = p1->forw)
		if (p1->op == JP && (p2 = p1->ref) && p2->refc > 1)
			for (p3 = p1->forw; p3; p3 = p3->forw)
				if (p3->op == JP && p3->ref == p2)
					backjmp(p1, p3);
}

static void
backjmp(ap1, ap2)
struct node *ap1;
struct node *ap2;
{
	register struct node *p1, *p2, *p3;

	p1 = ap1;
	p2 = ap2;
	for (;;) {
		while (p1 = p1->back)
			if (p1->op != LABEL)
				break;
		p2 = p2->back;
		if (!equop(p1, p2))
			return;
		p3 = insertl(p1);
		p2->back->forw = p2->forw;
		p2->forw->back = p2->back;
		p2 = p2->forw;
		decref(p2->ref);
		p2->labno = p3->labno;
		p2->ref = p3;
		nchange++;
		ncomj++;
	}
}

