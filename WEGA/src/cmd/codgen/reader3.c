/*
 * reader3.c - the program and the driver of the code generation: options, commands of the intermediate file, order of the trees, stores, conditional branches
 */
#include "cg.h"
#include "cg2.h"

extern int allo();
extern void allocinit();
extern struct node *argprep();
extern long baseoff;
extern void branch();
extern int callflag;
extern struct node *canon();
extern void canon2();
extern void cbgen();
extern void cbranch();
extern int cerror();
extern char *cnames[];
extern void codgen();
extern void constore();
extern void d12kout();
extern int d2r0reg;
extern struct node *d2ranode;
extern void d3jlabel();
extern int d3knamed();
extern void d3kreset();
extern void deflab();
extern void delay();
extern struct node *deltrees[];
extern FILE *dfile;
extern int dope[];
extern void dopeinit();
extern void efcode();
extern int eflag;
extern void endrun();
extern struct node *eread();
extern void fbegin();
extern void fcmd();
extern void ffld();
extern int ffregs;
extern void fixtemp();
extern long frame0;
extern long freetemp();
extern int fregs;
extern long ftell();
extern void fwalk();
extern void fwalkd();
extern int gencall();
extern int genscall();
extern int getlab();
extern void gswitch();
extern void hdrout();
extern int ifline;
extern char ifname[];
extern void init();
extern int lastchance();
extern int lflag;
extern void lineout();
extern int lmulflag;
extern int mainp2();
extern char *malloc();
extern void mapnm();
extern void mapregs();
extern void markcall();
extern int match();
extern long maxoff;
extern int maxreg;
extern long maxtemp;
extern int minreg;
extern void mkadrs();
extern void mkhash();
extern void mycanon();
extern void ncopy();
extern int ndelay;
extern int negrel[];
extern int nerrors;
extern int nextcook();
extern int nlabsave;
extern void nlcheck();
extern struct node node[];
extern int notoff();
extern int nparm;
extern int nparmtab;
extern int nrecur;
extern void offstar();
extern int oflag;
extern char *opst[];
extern struct node *optim();
extern int options();
extern void order();
extern void oreg2();
extern long parmax;
extern void parminit();
extern struct { struct sym *sym; int reg; } parmtab[];
extern long paroff;
extern int pflag;
extern void prcook();
extern void rallo();
extern void rcmd();
extern void rcount();
extern int rddata();
extern long rdin();
extern int rdsym();
extern void reclaim();
extern int regA;
extern int regB;
extern int regC;
extern int regD;
extern int regE;
extern int regF;
extern int regG;
extern int regmask;
extern void regreset();
extern void regtabinit();
extern int regtop;
extern struct node resc[];
extern int rflag;
extern void rfree();
extern void rfreeall();
extern void rfreenode();
extern void rsbuild();
extern void rsfreeall();
extern void rsinit();
extern struct rs *rsuse;
extern void scmd();
extern int setasg();
extern int setasop();
extern int setbin();
extern void setrew();
extern int setstr();
extern FILE *stfile;
extern long stg;
extern void stmt();
extern void stoarg();
extern int stoasg();
extern int stocook;
extern void store();
extern struct node *stotree;
extern long stpos;
extern void sucomp();
extern struct sym *symtab;
extern struct sym *symtab0;
extern int szty();
extern void tabclear();
extern struct node *talloc();
extern void tcheck();
extern void tcmd();
extern struct node *tcopy();
extern void tfree();
extern void tinit();
extern void tmpcleanup();
extern long tmpoff;
extern int tshape();
extern void uerror();
static char sccsid[] = "@[$]reader3.c\t2.5  08/24/83 13:32:32 Copr. 1981 87wega3.2.";
int lmulflag = 0;
int pflag = 0;
int rflag = 0;
int eflag = 0;
int unused4[3] = { 0 };
int oflag = 0;
int unused5 = 0;
long baseoff = 0;
long maxtemp = 0;

#ifdef OLD_D3K
static int d3kpend;		/* a label waits for its rsfreeall (main) */
#ifdef OLD_D3J
static int d3jdead;		/* the record before was an unconditional jump */
extern void d3jlabel();
#endif
#endif


#define FOREFF	01

#define FORCC	040

#define INTEMP_	0x1000

#define MUSTDO	0x1000

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
static void parmwalk();

/* the options: the number of the first argument that is no option, or 0 */
int
options(argc, argv)
	int argc;
	char **argv;
{
	register int i, err;
	register char *cp;
	char buf[50];

	err = 0;
	for (i = 1; i < argc; i++) {
		cp = argv[i];
		if (*cp != '-') {
			err = 1;
			continue;
		}
		while (*++cp) {
			switch (*cp) {
			case 'E':
				i++;
				strcpy(buf, argv[i]);
				strcat(buf, ".S");
				stfile = fopen(buf, "r");
				if (!stfile)
					printf("// error in opening symbol table file %s\n", argv[i - 1]);
				fread(&stg, 4, 1, stfile);
				stpos = ftell(stfile);
				strcpy(buf, argv[i]);
				strcat(buf, ".D");
				dfile = fopen(buf, "r");
				if (!dfile) {
					printf("// error in opening data file %s\n", argv[i - 1]);
					break;
				}
				setbuf(dfile, malloc(0x200));
				break;
			case 'M':
				i++;
				break;
			case 'O':
				oflag++;
				break;
			case 'R':
				rflag++;
				break;
			case 'e':
				eflag++;
				break;
			case 'l':
				lflag++;
				break;
			case 'p':
				pflag++;
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
	}
	hdrout();
	dopeinit();
	setrew();
	return err;
}

/* the state of the run */
void
init()
{
	register int spare;

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
mainp2(argc, argv)
	int argc;
	char *argv[];
{
	register int files;
	register int temp;
	register int c;
	register char *cp;
	register struct node *p;
	register int a, b;
	int x, y;
	int n;
	char ch;

	files = options(argc, argv);
	tinit();
reread:
	if (files) {
		while (files < argc && argv[files][0] == '-')
			++files;
		if (files > argc)
			return nerrors;
		freopen(argv[files], "r", stdin);
	}
	while ((c = getchar()) > 0) {
#ifdef OLD_D3K
		if (d3kpend) {		/* a label no jump has named: forgotten unless the function ends behind it */
			if (c != '}')
				rsfreeall(1);
			d3kpend = 0;
		}
#endif
#ifdef OLD_D3J
		if (d3jdead)
			d3jdead--;	/* 1: the record before this one was a jump */
#endif
		switch (c) {
		case '{':
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
			allocinit(symtab);
			continue;
		case '[':
			baseoff = rdin(10);
			regE = rdin(10);
			regF = rdin(10);
			if (rflag) {
				regE = 0xE;
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
			tmpoff = 0;
			p = eread();
			stmt(p);
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
				} else
					d3kpend = 1;
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
				;
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
parmwalk(p)
	register struct node *p;
{
	switch (dope[p->op] & 0xE) {
	case BITYPE:
		parmwalk(p->right);
		parmwalk(p->left);
		return;
	case UTYPE:
		parmwalk(p->left);
		return;
	}
	parminit(p);
}
#endif

/* a statement of the intermediate file: a function head, a switch, or an expression tree */
void
stmt(p)
	register struct node *p;
{
	int tag;

	nrecur = 0;
#ifdef OLD_D2R0
	d2r0reg = -1;		/* it holds only inside a statement */
#endif
#ifdef OLD_D2RA
	d2ranode = 0;
#endif
	if (p->op == 118) {
		mapnm(p->left);
		fbegin(p->left->sname);
#ifdef OLD_D4O
		parmwalk(p->right);	/* v0: the parameters in the order of the declaration (the list is reversed) */
#else
		fwalkd(p->right, parminit);	/* no third argument: the original passes only two, down is what r5 holds */
#endif
		tfree(p);
		return;
	}
	if (p->op == 119) {
		mapnm(p->left);
		fbegin(p->left->sname);
		tfree(p);
		return;
	}
	if (p->op == 122) {
		if (p->left->op == TEMP) {
			tag = p->left->rval;
			fixtemp(p->left);
			if (p->left->op == REG)
				tabclear(tag);
		} else {
			tag = p->left->left->rval;
			fixtemp(p->left->left);
			if (p->left->left->op == REG)
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

/* a statement tree */
void
delay(p)
	register struct node *p;
{
	register int i, ret, sz;
	long off;
	int x;

	sz = ret = 0;
	ndelay = 0;
	if (p->op == RET) {
		switch (p->type) {
		case 6:
		case 7:
		case 8:
		case 9:
			break;
		default:
			ret++;
			sz = szty(p->type);
#ifndef OLD_D1R
			off = freetemp(sz) / 8;
#endif
		}
	}
	fwalkd(p, mapnm, 0);
	fwalk(p, mapregs);
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
delay1(p)
	register struct node *p;
{
	register int o, ty;
	register struct node *q;

	o = p->op;
	ty = dope[o] & 0xE;
	if (ty == LTYPE)
		return 0;
	if (ty == UTYPE)
		return delay1(p->left);
	switch (o) {
	case QUEST:
	case ANDAND:
	case OROR:
		return delay1(p->left);
	case COMOP:
		delay(p->left);
		q = p->right;
		ncopy(p, p->right);
		q->op = FREE;
		return 1;
	}
	return delay1(p->left) || delay1(p->right);
}

static void
delstrip(p)
	register struct node *p;
{
	register int o;
	register struct node *q;

	o = p->op;
	if (o == INCR || o == DECR) {
		p->right->op = FREE;
		q = p->left;
		*p = *p->left;
		q->op = FREE;
	}
}

static void
delay2(p)
	register struct node *p;
{
	register int o, ty;
	register struct node *q;

	o = p->op;
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
			fwalk(q->left, delstrip);
			fwalk(q->right, delstrip);
			deltrees[ndelay++] = q;
			q = p->left;
			p->right->op = FREE;
			ncopy(p, q);
			q->op = FREE;
		}
		break;
	case CBRANCH:
		return;
	}
	ty = dope[p->op] & 0xE;
	if (ty == BITYPE)
		delay2(p->right);
	if (ty != LTYPE)
		delay2(p->left);
}

/* generate the code for p: stores first, then the tree itself */
void
codgen(p, cookie)
	register struct node *p;
	register int cookie;
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
prcook(cookie)
	register int cookie;
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
			printf("SPECIAL+%%%0.2x", cookie & 0x7FFF);
		}
		return;
	}
	flag = 0;
	for (i = 0; cnames[i]; i++)
		if (cookie & (1 << i)) {
			if (flag)
				putchar('|');
			flag++;
			printf(cnames[i]);
		}
	printf(" [%%%0.2x]", cookie);
}

/* generate code for p; cook says in which form the result is wanted */
void
order(p, cook)
	register struct node *p;
	register int cook;
{
	register int o, ty, m;
	register int cookie;
	int m1;
	struct node *p1, *p2;

again:
	cookie = cook;
	rcount();
	p = argprep(p, cookie);
	p = canon(p);
	rallo(p, p->rall);
	o = p->op;
	ty = dope[o] & 0xE;
	switch (m = p->op) {
	case MUL:
	case ASG MUL:
		if (p->right->op == OREG && p->left->op == REG)
			p->right->rall = NOPREF;
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
	p1 = p->left;
	if (ty == BITYPE)
		p2 = p->right;
	else
		p2 = 0;
	switch (m) {
	default:
	nomat:
		cerror("no table entry for op %s", opst[p->op]);
	case RET:
		p = p->left;
		if (p->type == 6 || p->type == 7) {
			p->rall = 0x1010;
			order(p, 0x10);
			tfree(p);
			return;
		}
		cook = 4;
		goto again;
	case CBRANCH:
		o = p2->lval;
		cbranch(p1, -1, o);
		p2->op = FREE;
		p->op = FREE;
		return;
	case COMOP:
		codgen(p1, FOREFF);
		p2->rall = p->rall;
		codgen(p2, cookie);
		ncopy(p, p2);
		p2->op = FREE;
		goto cleanup;
	case QUEST:
		cbranch(p1, -1, m = getlab());
#ifdef OLD_D3AW
		/* v0p: the value goes to a register that holds a parameter: the parameter is forgotten */
		if (p->rall & MUSTDO) {
			register struct rs *n;

			for (n = rsuse; n; n = n->next)
				if (n->cls <= 4 && n->sym && n->sym->sclass == 9 && n->reg == (p->rall & 0xff))
					n->sym = 0;
		}
#endif
		p2->left->rall = p->rall;
		if (p2->type == 7 || p2->type == 6)
			codgen(p2->left, 0x10);
		else
			codgen(p2->left, 4);
		p2->right->rall = p2->left->rval | MUSTDO;
		reclaim(p2->left, 0, 0);
		cbgen(0, m1 = getlab(), 'I');
		deflab(m);
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
		if (p2->type == 7 || p2->type == 6)
			codgen(p2->right, 0x10);
		else
			codgen(p2->right, 4);
		deflab(m1);
		p->op = REG;
		p->lval = 0;
		p->rval = p2->right->rval;
		p->type = p2->right->type;
		tfree(p2->right);
		p2->op = FREE;
		goto cleanup;
	case ANDAND:
	case OROR:
	case NOT:
		cbranch(p, -1, m = getlab());
		p->op = CCODES;
		p->label = m;
		order(p, 4);
		goto cleanup;
	case FLD:
		if (p1->op == STAR) {
			offstar(p1->left);
			goto again;
		}
	case UMINUS:
		if (p1->type == 6 || p1->type == 7) {
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
		p->right = 0;
	case FCALL:
		o = p->op = UFCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case UCALL:
		p->right = 0;
	case CALL:
		o = p->op = UCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case USTCALL:
		p->right = 0;
	case STCALL:
		o = p->op = USTCALL;
		if (genscall(p, cookie))
			goto nomat;
		goto cleanup;
	case STAR:
		if (cook == FOREFF) {
			order(p->left, FOREFF);
			p->op = FREE;
			return;
		}
#ifdef SEG
		offstar(p);
		if (p->op != STAR)
			goto cleanup;
#else
		offstar(p->left);
#endif
		goto again;
	case INCR:
		if (cook & FOREFF) {
			p->op = p->op == INCR ? ASG PLUS : ASG MINUS;
			goto again;
		}
		p1 = tcopy(p);
		reclaim(p->left, 0, 0);
		p->left = p1;
		p1->op = p->op == INCR ? ASG PLUS : ASG MINUS;
		p->op = p->op == INCR ? MINUS : PLUS;
		goto again;
	case STASG:
		if (setstr(p))
			goto again;
		goto nomat;
	case ASG PLUS:
#ifdef OLD_D16B
		/* v0p: an unsigned word in memory: a = a op b at once (the address is evaluated twice) */
		if (!((p->op == ASG PLUS || p->op == ASG MINUS) && (p->left->op == OREG || p->left->op == STAR) &&
		    p->left->type == 14))
#endif
		if (setasop(p))
			goto again;
		p2 = tcopy(p);
		p->op = ASSIGN;
		reclaim(p->right, 0, 0);
		p->right = p2;
		p = canon(p);
		rallo(p, p->rall);
		if (p->type == 7 || p->type == 6)
			order(p2->left, 0x10);
		else
			order(p2->left, 4);
		if (p->type == 7 || p->type == 6)
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
			p->op = o + 1;
			goto again;
		}
		goto nomat;
	}

cleanup:
	if (cook & FOREFF) {
		reclaim(p, 0, 0);
		return;
	}
	if (p->op == FREE)
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
store(p)
	register struct node *p;
{
	register int o, ty;

	o = p->op;
	ty = dope[o] & 0xE;
	if (ty == LTYPE)
		return;
	switch (o) {
	case UCALL:
	case UFCALL:
	case USTCALL:
		callflag++;
		break;
	case STAR:
		if (dope[p->left->op] & 1)
			stoasg(p->left, STAR);
		break;
	case CALL:
	case FCALL:
	case STCALL:
		store(p->left);
		stoarg(p->right, o);
		callflag++;
		return;
	case COMOP:
		markcall(p->right);
		if (p->right->su > fregs || p->right->fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
		store(p->left);
		return;
	case ANDAND:
	case OROR:
	case QUEST:
		markcall(p->right);
		if (p->right->su > fregs || p->right->fsu > ffregs) {
			stotree = p;
			stocook = INTEMP_;
		}
		constore(p->left);
		return;
	case NOT:
	case CBRANCH:
		constore(p->left);
		return;
	case MUL:
	case ASG MUL:
	case DIV:
	case ASG DIV:
	case MOD:
	case ASG MOD:
		/* the long ones are calls of a library function */
		if (p->left->type == 5 || p->left->type == 15)
			if (lmulflag++) {
				stotree = p;
				stocook = INTEMP_;
			}
		break;
	}
	if (ty == UTYPE) {
		store(p->left);
		return;
	}
	if (dope[p->right->op] & 1)
		stoasg(p->right, o);
	if (p->su > fregs || p->fsu > ffregs)
		mkadrs(p);
	store(p->right);
	store(p->left);
}

/* store conditional expressions: not inside a conditional context */
void
constore(p)
	register struct node *p;
{
	switch (p->op) {
	case QUEST:
	case ANDAND:
	case OROR:
		markcall(p->right);
	case NOT:
		constore(p->left);
		return;
	}
	store(p);
}

/* count the calls below p */
void
markcall(p)
	register struct node *p;
{
again:
	switch (p->op) {
	case CALL:
	case UCALL:
	case FCALL:
	case UFCALL:
	case STCALL:
	case USTCALL:
		callflag++;
		return;
	}
	switch (dope[p->op] & 0xE) {
	case BITYPE:
		markcall(p->right);
	case UTYPE:
		p = p->left;
		goto again;
	case LTYPE:
		return;
	}
}

/* store the arguments of a call */
void
stoarg(p, calltype)
	register struct node *p;
	int calltype;
{
	if (p->op == CM) {
		stoarg(p->left, calltype);
		p = p->right;
	}
	if (calltype == 0x46 || calltype == 0x67)
		;		/* the original tests this and does nothing */
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
cbranch(p, true, false)
	register struct node *p;
	int true, false;
{
	register int o;
	register int lab, flab, tlab;

	lab = -1;
	switch (o = p->op) {
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
			o = p->op = negrel[o - EQ];
			true = false;
			false = -1;
		}
		if (p->right->op == ICON && p->right->lval == 0 && p->right->sname == 0) {
			switch (o) {
			case UGT:
			case ULE:
				o = p->op = o == UGT ? NE : EQ;
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
		p->label = true;
		codgen(p, FORCC);
	uncond:
		if (false >= 0)
			cbgen(0, false, 'I');
		reclaim(p, 0, 0);
		return;
	case ANDAND:
		lab = false < 0 ? getlab() : false;
		cbranch(p->left, -1, lab);
		cbranch(p->right, true, false);
		if (false < 0)
			deflab(lab);
		p->op = FREE;
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
		cbranch(p->left, lab, -1);
		cbranch(p->right, true, false);
		if (true < 0)
			deflab(lab);
		p->op = FREE;
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
#endif
		return;
	case COMOP:
		codgen(p->left, FOREFF);
		p->op = FREE;
		cbranch(p->right, true, false);
		return;
	case QUEST:
		flab = false < 0 ? getlab() : false;
		tlab = true < 0 ? getlab() : true;
		cbranch(p->left, -1, lab = getlab());
		cbranch(p->right->left, tlab, flab);
		deflab(lab);
		cbranch(p->right->right, true, false);
		if (true < 0)
			deflab(tlab);
		if (false < 0)
			deflab(flab);
		p->right->op = FREE;
		p->op = FREE;
		return;
	case NOT:
		cbranch(p->left, false, true);
		p->op = FREE;
		return;
	case ASSIGN:
		/* x = constant: the truth value is known */
		if (p->right->op == ICON) {
			struct node *c;

			c = tcopy(p->right);
			order(p, FOREFF);
			p = c;
	case ICON:
			if (p->lval != 0 || p->sname != 0) {
				if (true >= 0)
					cbgen(0, true, 'I');
			} else if (false >= 0)
				cbgen(0, false, 'I');
			p->op = FREE;
			return;
		}
	default: {
		int t;

		t = p->type;
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
rcount()
{
	if (++nrecur > MAXRCOUNT)
		cerror("expression causes compiler loop: try simplifying");
}

/* the fields that are not on the left of an assignment become mask and shift */
void
ffld(p, down, down1, down2)
	struct node *p;
	int down;
	int *down1, *down2;
{
	register struct node *shp;
	register int s, o, v, ty;

	*down1 = dope[p->op] & 1;
	*down2 = 0;
	if (down || p->op != FLD)
		return;
	ty = szty(p->type) == 2 ? 5 : 4;
	v = p->rval;
	s = v & 0x3F;
	o = szty(p->type) * 16 - s - (v >> 6);
	p->left->type = ty;
	p->op = AND;
	p->right = talloc();
	p->right->op = ICON;
	p->right->rall = NOPREF;
	p->right->type = ty;
	p->right->lval = 1;
	p->right->rval = 0;
	p->right->sname = 0;
	p->right->lval <<= s;
	p->right->lval -= 1;
	if (o != 0) {
		shp = talloc();
		shp->op = RS;
		shp->rall = NOPREF;
		shp->type = ty;
		shp->left = p->left;
		shp->right = talloc();
		shp->right->op = ICON;
		shp->right->rall = NOPREF;
		shp->right->type = ty;
		shp->right->rval = 0;
		shp->right->lval = o;
		shp->right->sname = 0;
		p->left = shp;
	}
}

/* an indirection of a register plus a constant becomes an OREG */
void
oreg2(p)
	register struct node *p;
{
	register struct node *q;
	register int spare1;		/* unused: the original reserves this register */
	register struct node *ql, *qr;
	register int r;
	register struct sym *cp;
	long temp;
	int stw;

	if (p->op != STAR)
		return;
	q = p->left;
	if (q->op == REG) {
		temp = q->lval;
		r = q->rval;
		stw = 0;
		cp = 0;
	} else {
#ifdef SEG
		if (q->op != PLUS)
			return;
#else
		if (q->op != PLUS && q->op != MINUS)
			return;
#endif
		ql = q->left;
		qr = q->right;
#ifndef SEG
		if (ql->op == REG && qr->op == ICON) {
			temp = qr->lval;
			if (q->op == MINUS)
				temp = -temp;
			r = ql->rval;
			stw = 0;
			cp = qr->sname;
		} else
#endif
		if (q->op == PLUS && ql->op == ICON && qr->op == REG) {
			if (ql->rval >= 0)
				stw = 0;
			else if (ql->sname && ql->sname->sclass == 3 && !(ql->sname->sflags >> 2 & 1))
				stw = -ql->sname->soffset;
			else
				stw = ql->rval;
			temp = ql->lval;
			r = qr->rval;
			cp = ql->sname;
		} else
			return;
	}
	if (notoff(p->type, r, temp, cp))
		return;
	p->op = OREG;
	p->rval = r;
	p->lval = temp;
	p->stsize = stw;
	p->sname = cp;
	tfree(q);
}

/* put p in canonical form: fields, OREG nodes, register need */
struct node *
canon(p)
	register struct node *p;
{
	fwalkd(p, ffld, 0);
	fwalk(p, oreg2);
	fwalk(p, mycanon);
	fwalk(p, sucomp);
	return p;
}

/* a structure argument is stored through a temporary: STARG becomes an STASG to the stack */
struct node *
argprep(p, cookie)
	register struct node *p;
	register int cookie;
{
	register struct node *copy;
	struct optab fake;
	struct node *n;

	if (p->op == STARG && cookie == INTEMP_) {
		copy = tcopy(p->left);
		fwalk(p->left, rfreenode);
		tfree(p->left);
		n = p->left = talloc();
		ncopy(n, p);
		p->left->op = STASG;
		p->left->type = INCREF(p->left->type);
		p->left->right = copy;
		fake.nx = 0x20;
		fake.needs = 0x100;
		if (!allo(p, &fake))
			cerror("bad STASG allocation");
		resc[0].type = DECREF(p->left->type);
		p->left->left = tcopy(&resc[0]);
		return p->left;
	}
	return p;
}
