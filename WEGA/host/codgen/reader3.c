/*
 * reader3.c - the program and the driver of the code generation: p2init, commands of the intermediate file, order of the trees, stores, conditional branches
 */
#include "cg.h"
#include "cg2.h"

static int delay1(NODE *p);
static void delstrip(NODE *p);
static void delay2(NODE *p);

extern long baseoff;
extern int callflag;
extern char *cnames[];
extern NODE *deltrees[];
extern FILE *dfile;
extern int dope[];
extern int eflag;
extern int ffregs;
extern long frame0;
extern int fregs;
extern int ifline;
extern char ifname[];
extern int lflag;
extern int lmulflag;
extern long maxoff;
extern int maxreg;
extern long maxtemp;
extern int minreg;
extern int ndelay;
extern int negrel[];
extern int nerrors;
extern int nlabsave;
extern NODE node[];
extern int nparm;
extern int nparmtab;
extern int nrecur;
extern int oflag;
extern char *opst[];
extern long parmax;
extern struct { struct sym *sym; int reg; } parmtab[];
extern long paroff;
extern int pflag;
extern int regA;
extern int regB;
extern int regC;
extern int regD;
extern int regE;
extern int regF;
extern int regG;
extern int regmask;
extern int regtop;
extern NODE resc[];
extern int rflag;
extern struct rs *rsuse;
extern FILE *stfile;
extern long stg;
extern int stocook;
extern NODE *stotree;
extern long stpos;
extern struct sym *symtab;
extern struct sym *symtab0;
extern long tmpoff;
static char sccsid[] __attribute__((unused)) = "@[$]reader3.c\t2.5  08/24/83 13:32:32 Copr. 1981 87wega3.2.";
int lmulflag = 0;
int pflag = 0;
int rflag = 0;
int eflag = 0;
int unused4[3] = { 0 };
int oflag = 0;
int unused5 = 0;
long baseoff = 0;
long maxtemp = 0;

#ifdef OLD_D14Q
#endif
#ifdef OLD_D3K
static int d3kpend;		/* a label waits for its rsfreeall (main) */
#ifdef OLD_D3J
static int d3jdead;		/* the record before was an unconditional jump */
#endif
#endif


#define INTEMP_	0x1000

#define NOMATCH	0x1000

#define MDONE	0x1001

#define FORREW	0x4000

#define FREE_	100

#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

#define DECREF(t)	((((unsigned)(t) >> 2) & ~0xF) | ((t) & 0xF))


/* the relation that is true when the one in the index is false */

#define MAXRCOUNT 5000

static void delstrip();
static void delay2();
static int delay1();

/* the p2init: the number of the first argument that is no option, or 0 */
int
p2init(int argc, char **argv)
{
	register int i;
	register char *cp;
	register int err;
	char buf[50];
	char *x, *b;
	extern char *strcpy(), *strcat();

	err = 0;
	for (i = 1; i < argc; i++) {
		cp = argv[i];
		if (*cp == '-') {
			while (*++cp) {
				switch (*cp) {
				case 'l':
					lflag++;
					break;
				case 'e':
					eflag++;
					break;
				case 'O':
					oflag++;
					break;
				case 'M':
					i++;
					break;
				case 'E':
					x = strcpy(buf, argv[++i]);
					x = strcat(x, ".S");
					stfile = fopen(x, "r");
					if (!stfile)
						printf("// error in opening symbol table file %s\n", argv[i - 1]);
					stg = rd32(stfile);
					stpos = ftell(stfile);
					x = strcpy(buf, argv[i]);
					x = strcat(x, ".D");
					dfile = fopen(x, "r");
					if (!dfile)
						printf("// error in opening data file %s\n", argv[i - 1]);
					else {
						b = malloc(0x200);
						setvbuf(dfile, b, _IOFBF, 0x200);	/* setbuf of the host writes BUFSIZ (8192) bytes */
					}
					break;
				case 'p':
					pflag++;
					break;
				case 'R':
					rflag++;
					break;
				case 'a':
				case 'f':
				case 'o':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'x':
				case 'y':
					break;
				default:
					uerror("bad option: %c", *cp);
				}
			}
		} else
			err = 1;
	}
	hdrout();
	mkdope();
	setrew();
	return err;
}

/* the state of the run */
void
init(void)
{

	regD = 1;
	fregs = 8;
	ffregs = 4;
	regG = 0;
	regA = 7;
	regB = 0x10;
	regC = 0x13;
	frame0 = 0;
	maxoff = 0;
	maxtemp = 0;
	parmax = 0;
#ifdef SEG
	minreg = 0xD;
#else
	minreg = 0xE;
#endif
	maxreg = 0x17;
#ifdef SEG
	regtop = 0xD;
#else
	regtop = 0xE;
#endif
	regmask = (regtop - 1) & ~1;
	regreset(0);
	nparmtab = 0;
	parmtab[nparmtab].sym = 0;
	parmtab[nparmtab].reg = -1;
	regtabinit();
	rsinit();
	rsbuild();
}

int
mainp2(int argc, char *argv[])
{
	register int files;
	register int temp;
	register int c;
	register char *cp;
	register NODE *p;
	register int a, b;
	int n;
	char ch;

	files = p2init(argc, argv);
	tinit();
reread:
	if (files) {
		while (files < argc && argv[files][0] == '-')
			++files;
		if (files > argc)
			return nerrors;
		if (freopen(argv[files], "r", stdin) == NULL)
			(void)0;	/* the original does not test it */
	}
	for (; (c = getchar()) > 0; ) {
#ifdef OLD_D3K
		if (d3kpend) {		/* a label no jump has named: forgotten unless the function ends behind it */
			if (c != '}')
				rsfreeall(1);
			d3kpend = 0;
		}
#endif
#ifdef OLD_D3J
		if (d3jdead && c != ']')
			d3jdead--;	/* 1: the record before this one was a jump */
#endif
		switch (c) {
		case '{':
#ifdef OLD_D14Q
			tmpoff = 0;
#endif
#ifdef OLD_D3K
			d3kreset();
#endif
			init();
			mkhash();
			nlabsave = rdin(10);
			frame0 = rdin(10);
			maxoff = rdin(10) << 3;
			maxoff += frame0 << 3;
			nparm = rdin(10);
			nlcheck();
			if (!rddata())
				cerror("error in reading data file");
			if (!rdsym())
				cerror("error in reading symbol table file");
			continue;
		case '}':
			if (parmax % 16)
				parmax = (parmax / 16 + 1) << 4;
			if (maxoff % 16)
				maxoff = (maxoff / 16 + 1) << 4;
			efcode();
#ifdef OLD_D12K
			d12kout(0);
#endif
			nlcheck();
			allocinit((char *)symtab);
			continue;
		case '[':
			baseoff = rdin(10);
			regE = rdin(10);
			regF = rdin(10);
			if (rflag) {
#ifdef SEG
				regE = 0xD;
#else
				regE = 0xE;
#endif
				regF = 0x17;
			}
			nlcheck();
			if (baseoff > parmax)
				parmax = baseoff;
			if (minreg > regE)
				minreg = regE;
			if (maxreg > regF)
				maxreg = regF;
			continue;
		case '.':
			/* compile code for an expression */
			ifline = rdin(10);
			for (cp = ifname; (*cp = getchar()) != '\n'; ++cp)
				;
			*cp = '\0';
			if (lflag)
				lineout(ifline, ifname);
			paroff = baseoff;
#ifndef OLD_D14Q
			tmpoff = 0;
#endif
			p = eread();
			p2compile(p);
			continue;
		case 'L':
			temp = rdin(10);
			deflab(temp);
			nlcheck();
			if (nparm <= 0)
#ifdef OLD_D3K
				if (d3knamed(temp)) {
					rsfreeall(4);
#ifdef OLD_D3J
					d3jlabel(temp, d3jdead);
#endif
				} else {
#ifdef OLD_D3J
					d3jclear();	/* a label no jump has named: a loop head */
#endif
					d3kpend = 1;
				}
#else
				rsfreeall(1);
#endif
			if (rflag)
				regreset(1);
			continue;
		case 'B':
			temp = rdin(10);
			branch(temp);
			nlcheck();
#ifdef OLD_D3J
			d3jdead = 2;	/* (1 at the next record) */
#endif
			continue;
		case 'F':
			fcmd(rdin(10));
			nlcheck();
			continue;
		case ']':
			nlcheck();
			continue;
		case 'R':
			a = rdin(10);
			b = rdin(10);
			if (b > 0)
				rcmd(a, b, 0);
			else
				rcmd(a, b, (int)rdin(10));
			nlcheck();
			continue;
		case 'S':
			scmd((int)rdin(10));
			nlcheck();
			continue;
		case 'T':
			tcmd((int)rdin(10));
			nlcheck();
			continue;
		case '#':
			while ((ch = getchar()) != '\n')
				;
			continue;
#ifdef OLD_D6T
		case 'V':		/* v0: the values of a cpir switch, written right in front of its targets (the guess of OLDCC.md) */
			while ((ch = getchar()) == ' ')
				;
			ungetc(ch, stdin);
			rdin(10);
			rdin(10);
			n = rdin(10);
#ifdef SEG
			printf("\t.psec data\n");
#endif
			printf("\t.data\n");
			printf("\t.even\n");
			printf("L%d:\n", n);
			while ((ch = getchar()) != '\n') {
				ungetc(ch, stdin);
				printf("\t.word\t%ld\n", rdin(10));
			}
			continue;
#endif
		case 'C':
			while ((ch = getchar()) == ' ')
				continue;
			ungetc(ch, stdin);
			n = rdin(10);
#ifdef SEG
			printf("\t.psec data\n");
#endif
			printf("\t.data\n");
			printf("\t.even\n");
			printf("L%d:\n", n);
			rdin(10);
			while ((ch = getchar()) != '\n') {
				ungetc(ch, stdin);
				n = rdin(10);
				printf("\t.addr\tL%d\n", n);
			}
#ifdef SEG
			printf("\t.psec\n\t.code\n");
#else
			printf("\t.code\n");
#endif
			continue;
		case '/':
			putchar(c);
			while (putchar(getchar()) != '\n')
				;
			continue;
		default:
			cerror("intermediate file format error");
		}
	}
	/* EOF */
	while ((ch = getc(dfile)) != -1) {
		ungetc(ch, dfile);
		rddata();
	}
	if (files)
		goto reread;
	endrun();
	return nerrors;
}

#ifdef OLD_D4O
/* the parameter list from its last element to its first */
static void
parmwalk(NODE *p)
{
	switch optype(p->in.op) {
	case BITYPE:
		parmwalk(p->in.right);
		parmwalk(p->in.left);
		return;
	case UTYPE:
		parmwalk(p->in.left);
		return;
	}
	parminit(p);
}
#endif

/* a statement of the intermediate file: a function head, a switch, or an expression tree */
void
p2compile(NODE *p)
{
	int tag;

	nrecur = 0;
	if (p->in.op == 118) {
		mapnm(p->in.left);
		fbegin(p->in.left->in.sname);
#ifdef OLD_D4O
		parmwalk(p->in.right);	/* v0: the parameters in the order of the declaration (the list is reversed) */
#else
		fwalk(p->in.right, parminit, 0);	/* no third argument: the original passes only two, down is what r5 holds */
#endif
		tfree(p);
		return;
	}
	if (p->in.op == 119) {
		mapnm(p->in.left);
		fbegin(p->in.left->in.sname);
		tfree(p);
		return;
	}
	if (p->in.op == 122) {
		if (p->in.left->in.op == TEMP) {
			tag = p->in.left->tn.rval;
			fixtemp(p->in.left);
			if (p->in.left->in.op == REG)
				tabclear(tag);
		} else {
			tag = p->in.left->in.left->tn.rval;
			fixtemp(p->in.left->in.left);
			if (p->in.left->in.left->in.op == REG)
				tabclear(tag);
		}
		gswitch(p);
		tmpcleanup();
		tfree(p);
		return;
	}
	delay(p);
	reclaim(p, 0, 0);
	tcheck();
}

/* mapnm for fwalk (the original passes mapnm itself and fwalk calls it with more arguments) */
static void
mapnm_walk(NODE *p, int down, int *a, int *b)
{
	(void)down;
	(void)a;
	(void)b;
	mapnm(p);
}

/* a statement tree */
void
delay(NODE *p)
{
	register int i, ret, sz;
	long off = 0;

	sz = ret = 0;
	ndelay = 0;
	if (p->in.op == RET) {
		switch (p->in.type) {
		case 6:
		case 7:
		case 8:
		case 9:
			break;
		default:
			ret++;
			sz = szty(p->in.type);
#ifndef OLD_D1R
			off = freetemp(sz) / 8;
#endif
		}
	}
	fwalk(p, mapnm_walk, 0);
#if defined(OLD_D3K) && !defined(OLD_D3KC)
	/* v0: a parameter the statement assigns to is not known in its register from its start on */
	if (p->in.op == ASSIGN && p->in.left->in.op == NAME && p->in.left->in.sname && p->in.left->in.sname->sclass == 9) {
		register struct rs *n;

		for (n = rsuse; n; n = n->next)
			if (n->sym == p->in.left->in.sname)
				n->sym = 0;
	}
#endif
	walkf(p, mapregs);
	canon2(p);
	while (delay1(p))
		;
	ndelay = 0;
	delay2(p);
	codgen(p, FOREFF);
#ifdef OLD_D1R
	ret = 0;	/* no temporary for the value of a return: r2 stays taken while the delayed x++ runs */
#endif
	if (ret && ndelay) {
		printf("/* delay */\n");
		if (sz == 2) {
#ifdef SEG
			printf("\tldl\t_stkseg+~L1+%ld(fp),rr2\n", off);
#else
			printf("\tldl\t~L1+%ld(fp),rr2\n", off);
#endif
			rfree(2, 5);
		} else {
#ifdef SEG
			printf("\tld\t_stkseg+~L1+%ld(fp),r2\n", off);
#else
			printf("\tld\t~L1+%ld(fp),r2\n", off);
#endif
			rfree(2, 4);
		}
	}
	if (ndelay > 0)
		for (i = 0; i < ndelay; i++)
			codgen(deltrees[i], FOREFF);
	if (ret && ndelay) {
		printf("/* delay */\n");
		if (sz == 2)
#ifdef SEG
			printf("\tldl\trr2,_stkseg+~L1+%ld(fp)\n", off);
#else
			printf("\tldl\trr2,~L1+%ld(fp)\n", off);
#endif
		else
#ifdef SEG
			printf("\tld\tr2,_stkseg+~L1+%ld(fp)\n", off);
#else
			printf("\tld\tr2,~L1+%ld(fp)\n", off);
#endif
	}
}

/* look for a COMOP that can be done first: 1 if a rewrite happened */
static int
delay1(NODE *p)
{
	register int o, ty;
	register NODE *q;

	o = p->in.op;
	ty = optype(o);
	if (ty == LTYPE)
		return 0;
	if (ty == UTYPE)
		return delay1(p->in.left);
	switch (o) {
	case QUEST:
	case ANDAND:
	case OROR:
		return delay1(p->in.left);
	case COMOP:
		delay(p->in.left);
		q = p->in.right;
		ncopy(p, p->in.right);
		q->in.op = FREE;
		return 1;
	}
	return delay1(p->in.left) || delay1(p->in.right);
}

static void
delstrip(NODE *p)
{
	register int o;
	register NODE *q;

	o = p->in.op;
	if (o == INCR || o == DECR) {
		p->in.right->in.op = FREE;
		q = p->in.left;
		*p = *p->in.left;
		q->in.op = FREE;
	}
}

static void
delay2(NODE *p)
{
	register int o, ty;
	register NODE *q;

	o = p->in.op;
	switch (o) {
	case QUEST:
	case ANDAND:
	case OROR:
	case COMOP:
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case NOT:
	case STCALL:
	case USTCALL:
		return;
	case INCR:
	case DECR:
		if (ndelay < NDELAY) {
			q = tcopy(p);
			walkf(q->in.left, delstrip);
			walkf(q->in.right, delstrip);
			deltrees[ndelay++] = q;
			q = p->in.left;
			p->in.right->in.op = FREE;
			ncopy(p, q);
			q->in.op = FREE;
		}
		break;
	case CBRANCH:
		return;
	}
	ty = optype(p->in.op);
	if (ty == BITYPE)
		delay2(p->in.right);
	if (ty != LTYPE)
		delay2(p->in.left);
}

/* generate the code for p: stores first, then the tree itself */
void
codgen(NODE *p, int cookie)
{
	for (;;) {
		p = canon(optim(p));
		stotree = 0;
		store(p);
		lmulflag = 0;
		if (stotree == 0)
			break;
		order(stotree, stocook);
	}
	order(p, cookie);
}

/* print a cookie (debug) */
char *cnames[] = {		/* 0x120 */
	"SANY", "SAREG", "STAREG", "SBREG", "STBREG", "SCC", "SNAME", "SCON", "SFLD", "SOREG", "STARNM", "STARREG",
	"INTEMP", "FORARG", "SWADD", 0
};

void
prcook(int cookie)
{
	register int i, flag;

	if (cookie & 0x8000) {
		switch (cookie) {
		case 0x8000:
			printf("SZERO");
			break;
		case 0x8002:
			printf("SSCON");
			break;
		case 0x8004:
			printf("SICON");
			break;
		default:
			printf("SPECIAL+%%%.2x", cookie & 0x7FFF);
		}
		return;
	}
	flag = 0;
	for (i = 0; cnames[i]; i++)
		if (cookie & (1 << i)) {
			if (flag)
				putchar('|');
			flag++;
			fputs(cnames[i], stdout);
		}
	printf(" [%%%.2x]", cookie);
}

/* generate code for p; cook says in which form the result is wanted */
void
order(NODE *p, int cook)
{
	register int o, ty, m;
	register int cookie;
	int m1;
	NODE *p1, *p2;

again:
	cookie = cook;
	rcount();
	p = argprep(p, cookie);
	p = canon(p);
	rallo(p, p->in.rall);
	o = p->in.op;
	ty = optype(o);
	switch (m = p->in.op) {
	case MUL:
	case ASG MUL:
		if (p->in.right->in.op == OREG && p->in.left->in.op == REG)
			p->in.right->in.rall = NOPREF;
	/* fall through */
	default:
		for (;;) {
			if ((m = match(p, cookie)) == MDONE)
				goto cleanup;
			else if (m == NOMATCH) {
				if (!(cookie = nextcook(p, cookie)))
					goto nomat;
				continue;
			} else
				break;
		}
		break;
	case COMOP:
	case CBRANCH:
	case QUEST:
	case ANDAND:
	case OROR:
	case NOT:
	case UCALL:
	case CALL:
	case USTCALL:
	case STCALL:
	case UFCALL:
	case FCALL:
	case RET:
		;
	}
	p1 = p->in.left;
	if (ty == BITYPE)
		p2 = p->in.right;
	else
		p2 = 0;
	switch (m) {
	default:
	nomat:
		cerror("no table entry for op %s", opst[p->in.op]);
	case RET:
		p = p->in.left;
		if (p->in.type == 6 || p->in.type == 7) {
			p->in.rall = 0x1010;
			order(p, 0x10);
			tfree(p);
			return;
		}
		cook = 4;
		goto again;
	case CBRANCH:
		o = p2->tn.lval;
		cbranch(p1, -1, o);
		p2->in.op = FREE;
		p->in.op = FREE;
#if defined(OLD_D3K) && !defined(OLD_D3KC)
		rsfreeall(4);	/* v0: the idle registers go back behind the jump, the known parameters stay */
#endif
		return;
	case COMOP:
		codgen(p1, FOREFF);
		p2->in.rall = p->in.rall;
		codgen(p2, cookie);
		ncopy(p, p2);
		p2->in.op = FREE;
		goto cleanup;
	case QUEST:
		cbranch(p1, -1, m = getlab());
#ifdef OLD_D14Q
		if (cookie == INTEMP_ && p->in.type != 6 && p->in.type != 7) {
			long t = freeparm(szty(p->in.type)) / 8;

			d14qarm(p2->in.left, t, p->in.type);
			cbgen(0, m1 = getlab(), 'I');
			deflab(m);
			rfreeall(1);
			d14qarm(p2->in.right, t, p->in.type);
			deflab(m1);
			p->in.op = OREG;
			p->tn.rval = 15;
			p->tn.lval = t;
			p->in.sname = 0;
			p->stn.stsize = 0;
			p2->in.op = FREE;
			goto cleanup;
		}
#endif
#ifdef OLD_D3AW
		/* v0p: the value goes to a register that holds a parameter: the parameter is forgotten */
		if (p->in.rall & MUSTDO) {
			register struct rs *n;

			for (n = rsuse; n; n = n->next)
				if (n->cls <= 4 && n->sym && n->sym->sclass == 9 && n->reg == (p->in.rall & 0xff))
					n->sym = 0;
		}
#endif
		p2->in.left->in.rall = p->in.rall;
		if (p2->in.type == 7 || p2->in.type == 6)
			codgen(p2->in.left, 0x10);
		else
			codgen(p2->in.left, 4);
		p2->in.right->in.rall = p2->in.left->tn.rval | MUSTDO;
		reclaim(p2->in.left, 0, 0);
		cbgen(0, m1 = getlab(), 'I');
		deflab(m);
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
		if (p2->in.type == 7 || p2->in.type == 6)
			codgen(p2->in.right, 0x10);
		else
			codgen(p2->in.right, 4);
		deflab(m1);
		p->in.op = REG;
		p->tn.lval = 0;
		p->tn.rval = p2->in.right->tn.rval;
		p->in.type = p2->in.right->in.type;
		tfree(p2->in.right);
		p2->in.op = FREE;
		goto cleanup;
#ifdef OLD_D14Q
	case ANDAND:
		if (cookie == INTEMP_) {
			long t = freeparm(1) / 8;

			d14qarm(p1, t, 4);
			if (t)
				printf("\ttest\t%ld(fp)\n", t);
			else
				printf("\ttest\t@fp\n");
			printf("\tjpr\teq,L%d\n", m = getlab());
			d14qarm(p2, t, 4);
			deflab(m);
			rfreeall(1);
			p->in.op = OREG;
			p->tn.rval = 15;
			p->tn.lval = t;
			p->in.sname = 0;
			p->stn.stsize = 0;
			p->in.type = 4;
			goto cleanup;
		}
	case OROR:
	case NOT:
#else
	case ANDAND:
	case OROR:
	case NOT:
#endif
		cbranch(p, -1, m = getlab());
		p->in.op = CCODES;
		p->bn.label = m;
		order(p, 4);
		goto cleanup;
	case FLD:
		if (p1->in.op == STAR) {
			offstar(p1->in.left);
			goto again;
		}
	/* fall through */
	case UMINUS:
		if (p1->in.type == 6 || p1->in.type == 7) {
			order(p1, 0x10);
			goto again;
		}
		order(p1, 2);
		goto again;
	case NAME:
		if (o == REG)
			goto nomat;
		order(p, 4);
		goto again;
	case INIT:
		uerror("illegal initialization: order()");
		return;
	case UFCALL:
		p->in.right = 0;
	/* fall through */
	case FCALL:
		o = p->in.op = UFCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case UCALL:
		p->in.right = 0;
	/* fall through */
	case CALL:
		o = p->in.op = UCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case USTCALL:
		p->in.right = 0;
	/* fall through */
	case STCALL:
		o = p->in.op = USTCALL;
		if (genscall(p, cookie))
			goto nomat;
		goto cleanup;
	case STAR:
		if (cook == FOREFF) {
			order(p->in.left, FOREFF);
			p->in.op = FREE;
			return;
		}
#ifdef SEG
		offstar(p);
		if (p->in.op != STAR)
			goto cleanup;
#else
		offstar(p->in.left);
#endif
		goto again;
	case INCR:
		if (cook & FOREFF) {
			p->in.op = p->in.op == INCR ? ASG PLUS : ASG MINUS;
			goto again;
		}
		p1 = tcopy(p);
		reclaim(p->in.left, 0, 0);
		p->in.left = p1;
		p1->in.op = p->in.op == INCR ? ASG PLUS : ASG MINUS;
		p->in.op = p->in.op == INCR ? MINUS : PLUS;
		goto again;
	case STASG:
		if (setstr(p))
			goto again;
		goto nomat;
	case ASG PLUS:
		if (setasop(p))
			goto again;
		p2 = tcopy(p);
		p->in.op = ASSIGN;
		reclaim(p->in.right, 0, 0);
		p->in.right = p2;
		p = canon(p);
		rallo(p, p->in.rall);
		if (p->in.type == 7 || p->in.type == 6)
			order(p2->in.left, 0x10);
		else
			order(p2->in.left, 4);
		if (p->in.type == 7 || p->in.type == 6)
			order(p2, 0x10);
		else
			order(p2, 4);
		goto again;
	case ASSIGN:
		if (setasg(p))
			goto again;
		goto nomat;
	case BITYPE:
		if (setbin(p))
			goto again;
		switch (o) {
		case PLUS:
		case MINUS:
		case MUL:
		case AND:
		case OR:
		case ER:
		case DIV:
		case MOD:
		case LS:
		case RS:
			p->in.op = o + 1;
			goto again;
		}
		goto nomat;
	}

cleanup:
	if (cook & FOREFF) {
		reclaim(p, 0, 0);
		return;
	}
	if (p->in.op == FREE)
		return;
	if (tshape(p, cook))
		return;
	if ((m = match(p, cook)) == MDONE)
		return;
	if (lastchance(p, cook))
		goto again;
	goto nomat;
}

/* find the subtree of p that has to be stored: the result is stotree / stocook */
void
store(NODE *p)
{
	register int o, ty;

	o = p->in.op;
	ty = optype(o);
	if (ty == LTYPE)
		return;
	switch (o) {
	case UCALL:
	case UFCALL:
	case USTCALL:
		callflag++;
		break;
	case STAR:
		if asgop(p->in.left->in.op)
			stoasg(p->in.left, STAR);
		break;
	case CALL:
	case FCALL:
	case STCALL:
		store(p->in.left);
		stoarg(p->in.right, o);
		callflag++;
		return;
	case COMOP:
		markcall(p->in.right);
		if (p->in.right->in.su > fregs || p->in.right->in.fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
		store(p->in.left);
		return;
	case ANDAND:
	case OROR:
	case QUEST:
		markcall(p->in.right);
		if (p->in.right->in.su > fregs || p->in.right->in.fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
#ifdef OLD_D14Q
		if (o == QUEST && p->in.type != 6 && p->in.type != 7) {	/* the value of a ?: goes through a temporary */
			stotree = p;
			stocook = INTEMP_;
		}
#endif
		constore(p->in.left);
		return;
	case NOT:
	case CBRANCH:
		constore(p->in.left);
		return;
	case MUL:
	case ASG MUL:
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		/* the long ones are calls of a library function */
		if (p->in.left->in.type == 5 || p->in.left->in.type == 15)
			if (lmulflag++) {
				stotree = p;
				stocook = INTEMP_;
			}
		break;
	}
	if (ty == UTYPE) {
		store(p->in.left);
		return;
	}
	if asgop(p->in.right->in.op)
		stoasg(p->in.right, o);
	if (p->in.su > fregs || p->in.fsu > ffregs)
		mkadrs(p);
	store(p->in.right);
	store(p->in.left);
}

#ifdef OLD_D14Q
/* an arm of a ?: is assigned to the temporary at offset t */
static void
d14qarm(NODE *q, long t, int ty)
{
	register NODE *n, *a;
	int i, i0;

	n = talloc();
	n->in.op = OREG;
	n->in.type = ty;
	n->tn.rval = 15;
	n->tn.lval = t;
	n->in.sname = 0;
	n->stn.stsize = 0;
	n->in.su = 0;
	n->in.fsu = 0;
	n->in.rall = NOPREF;
	a = talloc();
	a->in.op = ASSIGN;
	a->in.type = ty;
	a->in.left = n;
	if (q->in.type == 2 && ty == 4) {		/* a char value of an arm */
		a->in.right = q;
		q = talloc();
		q->in.op = 110;		/* SCONV */
		q->in.type = 4;
		q->in.left = a->in.right;
		q->in.rall = NOPREF;
	}
	a->in.right = q;
	a->in.rall = NOPREF;
	sucomp(a);
	i0 = ndelay;
	delay2(a);
	codgen(a, FOREFF);
	reclaim(a, 0, FOREFF);
	for (i = i0; i < ndelay; i++)
		codgen(deltrees[i], FOREFF);
	ndelay = i0;
}

#endif
/* store conditional expressions: not inside a conditional context */
void
constore(NODE *p)
{
	switch (p->in.op) {
	case QUEST:
	case ANDAND:
	case OROR:
		markcall(p->in.right);
#ifdef OLD_D14Q
		/* an && on the left of a || is evaluated into a temporary */
		if (p->in.op == OROR && p->in.left->in.op == ANDAND) {
			stotree = p->in.left;
			stocook = INTEMP_;
		}
#endif
	/* fall through */
	case NOT:
		constore(p->in.left);
		return;
	}
	store(p);
}

/* count the calls below p */
void
markcall(NODE *p)
{
again:
	switch (p->in.op) {
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case STCALL:
	case USTCALL:
		callflag++;
		return;
	}
	switch optype(p->in.op) {
	case BITYPE:
		markcall(p->in.right);
	/* fall through */
	case UTYPE:
		p = p->in.left;
		goto again;
	case LTYPE:
		return;
	}
}

/* store the arguments of a call */
void
stoarg(NODE *p, int calltype)
{
	if (p->in.op == CM) {
		stoarg(p->in.left, calltype);
		p = p->in.right;
	}
	callflag = 0;
	store(p);
	if (callflag) {
		stotree = p;
		stocook = INTEMP_;
		store(p);
	}
}

/* evaluate p for the truth value and jump to true or false; a label < 0 means to fall through */
int negrel[] = { NE, EQ, GT, GE, LT, LE, UGT, UGE, ULT, ULE, FNE, FEQ, FGT, FGE, FLT, FLE };	/* 0x274 */

void
cbranch(NODE *p, int true, int false)
{
	register int o;
	register int lab, flab, tlab;

	lab = -1;
	switch (o = p->in.op) {
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
	case 90:
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
		if (true < 0) {
			o = p->in.op = negrel[o - EQ];
			true = false;
			false = -1;
		}
		if (p->in.right->in.op == ICON && p->in.right->tn.lval == 0 && p->in.right->in.sname == 0) {
			switch (o) {
			case UGT:
			case ULE:
				o = p->in.op = o == UGT ? NE : EQ;
				break;
			case EQ:
			case NE:
			case LE:
			case LT:
			case GE:
			case GT:
			case 90:
			case 91:
			case 92:
			case 93:
			case 94:
			case 95:
				break;
			case UGE:
				cbgen(0, true, 'I');
			case ULT:
			default:
				goto uncond;
			}
		}
		p->bn.label = true;
		codgen(p, FORCC);
	uncond:
		if (false >= 0)
			cbgen(0, false, 'I');
		reclaim(p, 0, 0);
		return;
	case ANDAND:
		lab = false < 0 ? getlab() : false;
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
		if (false < 0)
			deflab(lab);
		p->in.op = FREE;
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
#endif
		return;
	case OROR:
		lab = true < 0 ? getlab() : true;
		cbranch(p->in.left, lab, -1);
		cbranch(p->in.right, true, false);
		if (true < 0)
			deflab(lab);
		p->in.op = FREE;
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
#endif
		return;
	case COMOP:
		codgen(p->in.left, FOREFF);
		p->in.op = FREE;
		cbranch(p->in.right, true, false);
		return;
	case QUEST:
		flab = false < 0 ? getlab() : false;
		tlab = true < 0 ? getlab() : true;
		cbranch(p->in.left, -1, lab = getlab());
		cbranch(p->in.right->in.left, tlab, flab);
		deflab(lab);
		cbranch(p->in.right->in.right, true, false);
		if (true < 0)
			deflab(tlab);
		if (false < 0)
			deflab(flab);
		p->in.right->in.op = FREE;
		p->in.op = FREE;
		return;
	case NOT:
		cbranch(p->in.left, false, true);
		p->in.op = FREE;
		return;
	isfalse:
		if (false >= 0)
			cbgen(0, false, 'I');
		goto done;
	asgn: {
		NODE *c;

		c = tcopy(p->in.right);
		order(p, FOREFF);
		p = c;
	}
	/* fall through */
	case ICON:
		/* x = constant: the truth value is known */
		if (p->tn.lval == 0 && p->in.sname == 0)
			goto isfalse;
		if (true >= 0)
			cbgen(0, true, 'I');
	done:
		p->in.op = FREE;
		return;
	case ASSIGN:
		if (p->in.right->in.op == ICON)
			goto asgn;
	/* fall through */
	default: {
		int t;

		t = p->in.type;
		codgen(p, FORCC);
		if (t == 6 || t == 7) {
			if (true >= 0)
				cbgen(FNE, true, 'I');
			if (false >= 0)
				cbgen(true >= 0 ? 0 : FEQ, false, 'I');
		} else {
			if (true >= 0)
				cbgen(NE, true, 'I');
			if (false >= 0)
				cbgen(true >= 0 ? 0 : EQ, false, 'I');
		}
		reclaim(p, 0, 0);
	}
	}
}

void
rcount(void)
{
	if (++nrecur > MAXRCOUNT)
		cerror("expression causes compiler loop: try simplifying");
}

/* the fields that are not on the left of an assignment become mask and shift */
void
ffld(NODE *p, int down, int *down1, int *down2)
{
	register NODE *shp;
	register int s, o, v, ty;

	*down1 = asgop(p->in.op);
	*down2 = 0;
	if (down || p->in.op != FLD)
		return;
	ty = szty(p->in.type) == 2 ? 5 : 4;
	v = p->tn.rval;
	s = v & 0x3F;
	o = szty(p->in.type) * 16 - s - (v >> 6);
	p->in.left->in.type = ty;
	p->in.op = AND;
	p->in.right = talloc();
	p->in.right->in.op = ICON;
	p->in.right->in.rall = NOPREF;
	p->in.right->in.type = ty;
	p->in.right->tn.lval = 1;
	p->in.right->tn.rval = 0;
	p->in.right->in.sname = 0;
	p->in.right->tn.lval <<= s;
	p->in.right->tn.lval -= 1;
	if (o != 0) {
		shp = talloc();
		shp->in.op = RS;
		shp->in.rall = NOPREF;
		shp->in.type = ty;
		shp->in.left = p->in.left;
		shp->in.right = talloc();
		shp->in.right->in.op = ICON;
		shp->in.right->in.rall = NOPREF;
		shp->in.right->in.type = ty;
		shp->in.right->tn.rval = 0;
		shp->in.right->tn.lval = o;
		shp->in.right->in.sname = 0;
		p->in.left = shp;
	}
}

/* an indirection of a register plus a constant becomes an OREG */
void
oreg2(NODE *p)
{
	register NODE *q;
	register int r;
	register struct sym *cp;
	register NODE *ql, *qr;
	long temp;
	int stw;

	if (p->in.op != STAR)
		return;
	q = p->in.left;
	if (q->in.op == REG) {
		temp = q->tn.lval;
		r = q->tn.rval;
		stw = 0;
		cp = 0;
	} else {
#ifdef SEG
		if (q->in.op != PLUS)
			return;
#else
		if (q->in.op != PLUS && q->in.op != MINUS)
			return;
#endif
		ql = q->in.left;
		qr = q->in.right;
#ifndef SEG
		if (ql->in.op == REG && qr->in.op == ICON) {
			temp = qr->tn.lval;
			if (q->in.op == MINUS)
				temp = -temp;
			r = ql->tn.rval;
			stw = 0;
			cp = qr->in.sname;
		} else
#endif
#ifdef SEG
		if (ql->in.op == ICON && qr->in.op == REG) {
#else
		if (q->in.op == PLUS && ql->in.op == ICON && qr->in.op == REG) {
#endif
			if (ql->tn.rval < 0) {
				if (ql->in.sname && ql->in.sname->sclass == 3 && !(ql->in.sname->sflags >> 2 & 1))	/* SEG: the original reads address 0 for no symbol */
					stw = -ql->in.sname->soffset;
				else
					stw = ql->tn.rval;
			} else
				stw = 0;
			temp = ql->tn.lval;
			r = qr->tn.rval;
			cp = ql->in.sname;
		} else
			return;
	}
	if (notoff(p->in.type, r, temp, cp))
		return;
	p->in.op = OREG;
	p->tn.rval = r;
	p->tn.lval = temp;
	p->stn.stsize = stw;
	p->in.sname = cp;
	tfree(q);
}

/* put p in canonical form: fields, OREG nodes, register need */
NODE *
canon(NODE *p)
{
	fwalk(p, ffld, 0);
	walkf(p, oreg2);
	walkf(p, mycanon);
	walkf(p, sucomp);
	return p;
}

/* a structure argument is stored through a temporary: STARG becomes an STASG to the stack */
NODE *
argprep(NODE *p, int cookie)
{
	register NODE *copy;
	struct optab fake;
	NODE *n;

	if (p->in.op == STARG && cookie == INTEMP_) {
		copy = tcopy(p->in.left);
		walkf(p->in.left, recl2);
		tfree(p->in.left);
		n = p->in.left = talloc();
		ncopy(n, p);
		p->in.left->in.op = STASG;
		p->in.left->in.type = INCREF(p->in.left->in.type);
		p->in.left->in.right = copy;
		fake.nx = 0x20;
		fake.needs = 0x100;
		if (!allo(p, &fake))
			cerror("bad STASG allocation");
		resc[0].in.type = DECREF(p->in.left->in.type);
		p->in.left->in.left = tcopy(&resc[0]);
		return p->in.left;
	}
	return p;
}
