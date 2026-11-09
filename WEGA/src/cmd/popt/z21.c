/*
 * z21.c -- popt: main program, input, output, jump optimizations
 */
#include "z2.h"

extern char _sobuf[];

static char sccsid[] = "@[$]z21.c\t1.4  12/21/82 17:58:05 Copr.1981 87wega3.2.";

struct optab *ophash[OPHS];

/* hash of the first three characters, read without a test as the original does (names "{" and "}" are padded with a NUL, see z20.c) */
#define HASHNAME(s) (((((int)(char)(s)[0] << 3) + 2 * (int)(char)(s)[1] + (int)(char)(s)[2]) & 0x7fff) % OPHS)

static int getline();
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
struct node * insertl();
struct node * codemove();
void comjump();
#ifdef OLD_D29
void d29switch();
#endif























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
#ifdef OLD_D29
		d29switch();
#endif
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
				oper = getline();
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
		oper = getline();
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
			oper = getline();
			goto sw;
		case 0:
			if (*curlp == 0) {
				oper = getline();
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
getline()
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
		for (t = optab; t->opstring; t++) {
			if ((t->opcode & 0xff) == n->op &&
			    (t->opcode >> 8) == n->subop) {
				if (n->op == CJP) {
					prefix(n);
					printf("%s", t->opstring);
				} else
					printf("\t%s", t->opstring);
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
#ifdef OLD_D13
		printf("\t.addr\tL%d\n", n->labno);	/* old cas: bit 31 of the address */
#else
		printf("\t.long\tL%d\n", n->labno);
#endif
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

	for (o = optab; (s = o->opstring) != 0; o++) {
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
		q = o->opstring;
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
		np = o->opstring;
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
			xjump(p);
			p = codemove(p);
		}
	}
}




#ifdef OLD_D29
/* The next and the previous node that is not a kept comment */
static struct node *
d29nx(p)
register struct node *p;
{
	while ((p = p->forw) && p->op == KCOMMENT)
		;
	return p;
}

static struct node *
d29pv(p)
register struct node *p;
{
	while ((p = p->back) && p->op == KCOMMENT)
		;
	return p;
}

static struct node *
d29lab(n)
int n;
{
	register struct node *p;

	for (p = first.forw; p; p = p->forw)
		if (p->op == LABEL && p->labno == n)
			return p;
	return 0;
}

/* Take the nodes a .. b out of the list */
static void
d29cut(a, b)
struct node *a, *b;
{
	a->back->forw = b->forw;
	b->forw->back = a->back;
}

/* Put the nodes a .. b behind q */
static void
d29put(q, a, b)
struct node *q, *a, *b;
{
	a->back = q;
	b->forw = q->forw;
	q->forw->back = b;
	q->forw = a;
}

/*
 * vk: a table switch whose default stands in front of the dispatch and which is left by a jump to the next statement. The head goes
 * behind the block that follows the switch, the rest of the dispatch to the place of the head, the default behind the head; the break
 * label gets the code of the target of its jump. Only the layout changes. The raw code of the old compiler is not known: this is the
 * layout from which the shipped spopt makes ttiocom (TCXONC) of tty.o; the other objects of vk are unchanged by it.
 *
 *	Lh: ld r,x; jp Ld; ...bodies; jp Lb; Ldf: ...; jp Lb; Ld: cp r,#n; cjp ugt,Ldf; rl r,#2; ldl rr,t(r); jp @rr; Lb: jp Lo; Lk: ...; jp Y
 * becomes
 *	Lt: rl r,#2; ldl rr,t(r); jp @rr; ...bodies; Lb: (the code at Lo); Lk: ...; jp Y; Lh: ld r,x; cp r,#n; cjp ule,Lt; Ldf: ...; jp Lb
 */
void
d29switch()
{
	register struct node *p, *q;
	struct node *x, *lh, *ld, *cp, *cj, *r1, *r2, *r3, *lb, *jb, *lk, *ke, *ldf, *jd, *jl, *lo, *lt, *n;
	int i;

	for (p = first.forw; p; p = p->forw) {
		if (p->op != JP || p->labno == 0)
			continue;
		if ((x = d29pv(p)) == 0 || x->op != LD || (lh = d29pv(x)) == 0 || lh->op != LABEL)
			continue;
		if ((ld = d29lab(p->labno)) == 0)
			continue;
		if ((cp = d29nx(ld)) == 0 || cp->op != CP || (cj = d29nx(cp)) == 0 || cj->op != CJP ||
		    (r1 = d29nx(cj)) == 0 || r1->op != RL || (r2 = d29nx(r1)) == 0 || r2->op != LD ||
		    (r3 = d29nx(r2)) == 0 || r3->op != JP || r3->labno != 0)
			continue;
		if ((lb = d29nx(r3)) == 0 || lb->op != LABEL || (jb = d29nx(lb)) == 0 || jb->op != JP || jb->labno == 0)
			continue;
		if ((lk = d29nx(jb)) == 0 || lk->op != LABEL)
			continue;
		for (ke = lk; ke && (ke->op != JP || ke->labno == 0); ke = ke->forw)
			;
		if (ke == 0 || ke->forw == 0)
			continue;
		if ((ldf = d29lab(cj->labno)) == 0)
			continue;
		for (jd = ldf; jd && jd != ld && jd->op != JP; jd = jd->forw)
			;
		if (jd == 0 || jd->op != JP || jd->labno != lb->labno || d29nx(jd) != ld)
			continue;
		if ((jl = d29pv(ldf)) == 0 || jl->op != JP || jl->labno != lb->labno)
			continue;
		if ((lo = d29lab(jb->labno)) == 0)
			continue;
		/* the code at Lo: at most 3 instructions up to a jump */
		for (q = lo, i = 0; q && (q->op != JP || q->labno == 0) && i < 5; q = q->forw)
			i++;
		if (q == 0 || q->op != JP || q->labno == 0)
			continue;

		/* the head and the dispatch out of the list; the rest of the dispatch to the place of the head */
		n = lh->back;
		d29cut(lh, p);
		d29cut(ld, r3);
		lt = (struct node *)alloc(14);
		lt->op = LABEL;
		lt->subop = 0;
		lt->labno = isn++;
		lt->ref = 0;
		lt->code = 0;
		lt->refc = 0;
		lt->back = n;
		lt->forw = n->forw;
		n->forw->back = lt;
		n->forw = lt;
		d29put(lt, r1, r3);
		/* the default out of the list, the jump of the last body away */
		d29cut(ldf, jd);
		d29cut(jl, jl);
		/* the code at Lo in place of the jump at Lb */
		for (q = lo->forw, n = lb; ; q = q->forw) {
			register struct node *c;

			if (q->op == KCOMMENT || q->op == LABEL)
				continue;
			c = (struct node *)alloc(14);
			c->op = q->op;
			c->subop = q->subop;
			c->ref = 0;
			c->labno = q->labno;
			c->code = q->code;
			c->refc = 0;
			d29put(n, c, c);
			n = c;
			if (q->op == JP)
				break;
		}
		d29cut(jb, jb);
		/* behind the block: the head, the compare, the default */
		x->forw = cp;
		cp->back = x;
		cj->labno = lt->labno;
		cj->subop = revbr[(int)cj->subop];
		cj->forw = ldf;
		ldf->back = cj;
		d29put(ke, lh, jd);
		return;
	}
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
		while (p2 = p2->back) {
			if (p2->op == LABEL || p2->op == KCOMMENT)
				continue;
			break;
		}
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
					return p1;
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
		while (p1 = p1->back) {
			if (p1->op == LABEL)
				continue;
			break;
		}
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

