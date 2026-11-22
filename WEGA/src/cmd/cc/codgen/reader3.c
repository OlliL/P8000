/*
 * reader3.c - the program and the driver of the code generation: p2init, commands of the intermediate file, order of the trees, stores, conditional branches
 */
#include "mfile3"

extern void allocinit();
extern NODE *argprep();
extern int callflag;
extern void cbranch();
extern char *cnames[];
extern void codgen();
extern void constore();
extern void d12kout();
extern void d3kreset();
extern NODE *deltrees[];
extern void mkdope();
#ifdef OLD_D2FA
int d2faandor();
extern int d2fanone;
#endif
extern void ffld();
extern void fwalk();
extern void init();
extern int lmulflag;
extern char *malloc();
extern void markcall();
extern int ndelay;
extern int nerrors;
extern int nlabsave;
extern int nparm;
extern int nrecur;
extern int oflag;
extern void prcook();
extern int regA;
extern int regB;
extern int regC;
extern int regD;
extern int regE;
extern int regF;
extern int regG;
extern int regmask;
extern int regtop;
extern void p2compile();
extern void stoarg();
extern void store();
extern void tcheck();
extern void tinit();
static char sccsid[] = "@[$]reader3.c\t2.5  08/24/83 13:32:32 Copr. 1981 87wega3.2.";
int lmulflag = 0;
int pflag = 0;
int rflag = 0;
int eflag = 0;
int unused4[3] = { 0 };
int oflag = 0;
int unused5 = 0;

#ifdef OLD_D57
/*
 * vl: the frame offset (bits) of a register parameter that the function uses as memory (its address is taken), -1 when
 * there is none: its entry store is written with its offset (0(r15)), swap of libc.a.
 */
long d57off = -1;
#endif
long baseoff = 0;
long maxtemp = 0;

#ifdef OLD_D14Q
static void d14qarm();
#endif
#ifdef OLD_D12K
int d12kname();
#endif
#if defined(OLD_D24) || defined(OLD_D63)
int optpre(), optpost();
#endif
#ifdef OLD_D3K
void d3kjump(), d3kreset();
int d3klabel();
static int d3kpend;		/* a label waits for its rsfreeall (main) */
static int d3jdead;		/* v0p: the record before was an unconditional jump */
#endif


#define FREE_	100

#undef DECREF		/* the DECREF of manifest shifts the type as it is (int: arithmetic) */
#define DECREF(t)	((((unsigned)(t) >> 2) & ~0xF) | ((t) & 0xF))

/* the relation that is true when the one in the index is false */

#define MAXRCOUNT 5000

static void delstrip();
static void delay2();
static int delay1();
static void parmwalk();

/* the p2init: the number of the first argument that is no option, or 0 */
int
p2init(argc, argv)
	int argc;
	char **argv;
{
	register int i;
	register char *cp;
	register int err, r11, r10;
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
					fread(&stg, 4, 1, stfile);
					stpos = ftell(stfile);
					x = strcpy(buf, argv[i]);
					x = strcat(x, ".D");
					dfile = fopen(x, "r");
					if (!dfile)
						printf("// error in opening data file %s\n", argv[i - 1]);
					else {
						b = malloc(0x200);
						setbuf(dfile, b);
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
	register NODE *p;
	register int a, b;
	int x, y;
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
		freopen(argv[files], "r", stdin);
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
		if (d3jdead)	/* every record counts, also the end of a block (gopt prquad: return; } behind the jump) */
			d3jdead--;	/* 1: the record before this one was a jump */
#endif
		switch (c) {
		case '{':
#ifdef OLD_D57
			d57off = -1;
#endif
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
#ifdef OLD_D12K
			if (!d12kdata())
#else
			if (!rddata())
#endif
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
			lineno = rdin(10);
			for (cp = ftitle; (*cp = getchar()) != '\n'; ++cp)
				;
			*cp = '\0';
			if (lflag)
				lineout(lineno, ftitle);
			paroff = baseoff;
#ifndef OLD_D14Q
			tmpoff = 0;
#endif
			p = eread();
#ifdef OLD_D12K
			fwalk(p, d12kname, 0);	/* the double constants of the statement go out in front of its code */
#endif
			p2compile(p);
			continue;
		case 'L':
			temp = rdin(10);
			deflab(temp);
			nlcheck();
			if (nparm <= 0)
#ifdef OLD_D3K
				if (!d3klabel(temp, d3jdead))
					d3kpend = 1;
#else
				rsfreeall(1);
#endif
			if (rflag)
				regreset(1);
			continue;
		case 'B':
			temp = rdin(10);
#ifdef OLD_D3K
			d3kjump(temp);
#endif
			branch(temp);
			nlcheck();
#ifdef OLD_D3J
			d3jdead = 2;	/* (1 at the next record) */
#endif
			continue;
#ifdef OLD_DNR
		case 'F':
		case 'R':
		case 'S':
		case 'T':
			cerror("register records of the global optimizer (-O3, -O5) are not read by this code generator");
#else
		case 'F':
			fcmd(rdin(10));
			nlcheck();
			continue;
#endif
		case ']':
			nlcheck();
			continue;
#ifndef OLD_DNR
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
#endif
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
#ifdef OLD_D12K
		d12kdata();
#else
		rddata();
#endif
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
	register NODE *p;
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
p2compile(p)
	register NODE *p;
{
	int tag;

	nrecur = 0;
	if (p->in.op == PROC) {
		mapnm(p->in.left);
		fbegin(p->in.left->in.sname);
#ifdef OLD_D4O
		parmwalk(p->in.right);	/* v0: the parameters in the order of the declaration (the list is reversed) */
#else
		fwalk(p->in.right, parminit);	/* no third argument: the original passes only two, down is what r5 holds */
#endif
		tfree(p);
		return;
	}
	if (p->in.op == UPROC) {
		mapnm(p->in.left);
		fbegin(p->in.left->in.sname);
		tfree(p);
		return;
	}
	if (p->in.op == SWTCH) {
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

/* a statement tree */
void
delay(p)
	register NODE *p;
{
	register int i, ret, sz;
	long off;
	int x;

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
#ifdef OLD_D53
			/* vl: none in a function with more than 10000 bits (1250 bytes) of automatic variables (nlist) */
			if (baseoff <= 10000L)
#endif
			off = freetemp(sz) / 8;
#endif
		}
	}
	fwalk(p, mapnm, 0);
#if defined(OLD_D3K) && !defined(OLD_D3KC)
	/* v0: a parameter the statement assigns to is not known in its register from its start on */
	if (p->in.op == ASSIGN && p->in.left->in.op == NAME && p->in.left->in.sname && p->in.left->in.sname->sclass == PARAM) {
		register struct rs *n;

		for (n = rsuse; n; n = n->next)
			if (n->sym == p->in.left->in.sname)
				n->sym = 0;
	}
#endif
#ifdef OLD_D2FA
	/* vc (gopt): in a condition with || at the top no pair is found by its frame place (killdefs; linkreachdefs with && finds it) */
	d2fanone = p->in.op == CBRANCH && p->in.left->in.op == OROR;
	/* vc (gopt): an automatic variable the statement assigns to is not known in the pair of its frame place from its start on */
	if (p->in.op == ASSIGN && p->in.left->in.op == NAME && p->in.left->in.sname &&
	    (p->in.left->in.sname->sclass == AUTO || p->in.left->in.sname->sclass == PARAM)) {
		register struct rs *n;
		register struct sym *f;

		f = p->in.left->in.sname->sclass == PARAM && (p->in.left->in.sname->sflags >> 3 & 1) == 1 ? fpsym2 : fpsym1;
		for (n = rsuse; n; n = n->next)
			if (n->cls == RCPAIR && n->sym == f && n->off == p->in.left->in.sname->soffset / 8)
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
			rfree(2, LONG);
		} else {
#ifdef SEG
			printf("\tld\t_stkseg+~L1+%ld(fp),r2\n", off);
#else
			printf("\tld\t~L1+%ld(fp),r2\n", off);
#endif
			rfree(2, INT);
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
	register NODE *p;
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
delstrip(p)
	register NODE *p;
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
delay2(p)
	register NODE *p;
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
codgen(p, cookie)
	register NODE *p;
	register int cookie;
{
	for (;;) {
#if defined(OLD_D24) || defined(OLD_D63)
		fwalk(p, optpre, 0);
		p = optim(p);
		fwalk(p, optpost, 0);
		p = canon(p);
#else
		p = canon(optim(p));
#endif
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

	if (cookie & SPECIAL) {
		switch (cookie) {
		case SZERO:
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

#ifdef OLD_D3Q
/* OLD_D3Q: does the tree p hold a ++ or -- of a pointer */
static int
d3qincr(p)
	register NODE *p;
{
	register int t;

	if ((p->in.op == INCR || p->in.op == DECR) && ISPTR(p->in.type))
		return 1;
	t = optype(p->in.op);
	if (t == BITYPE && d3qincr(p->in.right))
		return 1;
	if (t != LTYPE)
		return d3qincr(p->in.left);
	return 0;
}
#endif

#ifdef OLD_D3W
static int d3depth;	/* nesting of &&, || and ! in cbranch */

/* OLD_D3W (libcisam, under OLD_D3): at the end of a whole condition of &&, || and ! the idle word registers go back,
   pairs stay; inside the condition they keep their contents (compare/libcisam/COMPILER.md) */
static void
d3words()
{
	register struct rs *n, *next;

	for (n = rsuse; n; n = next) {
		next = n->next;
		if (n->busy > 0 || n->kind == RKTEMP)
			continue;
		if (n->cls == RCWORD)
			rsreturn(n->cls, n->reg);
	}
}
#endif

/* generate code for p; cook says in which form the result is wanted */
void
order(p, cook)
	register NODE *p;
	register int cook;
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
		if (p->in.type == FLOAT || p->in.type == DOUBLE) {
			p->in.rall = 0x1010;
			order(p, INTBREG);
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
#ifdef OLD_D3W
		if (d3depth == 0)
			d3words();
#endif
#ifdef OLD_D3Q
		if (d3qincr(p2)) {	/* OLD_D3Q (libcisam): behind the condition of a ?: whose arm increments a
					   pointer the scratch pairs are free, also a busy one (compare/libcisam/COMPILER.md) */
			register struct rs *n, *next;

			for (n = rsuse; n; n = next) {
				next = n->next;
				if (n->cls == RCPAIR && n->kind != RKTEMP && n->reg < 8 && n->busy > 0)
					rsreturn(n->cls, n->reg);
			}
		}
#endif
#ifdef OLD_D14Q
		if (cookie == INTEMP && p->in.type != FLOAT && p->in.type != DOUBLE) {
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
				if (n->cls <= RCFLOAT && n->sym && n->sym->sclass == PARAM && n->reg == (p->in.rall & 0xff))
					n->sym = 0;
		}
#endif
		p2->in.left->in.rall = p->in.rall;
		if (p2->in.type == DOUBLE || p2->in.type == FLOAT)
			codgen(p2->in.left, INTBREG);
		else
			codgen(p2->in.left, INTAREG);
		p2->in.right->in.rall = p2->in.left->tn.rval | MUSTDO;
		reclaim(p2->in.left, 0, 0);
		cbgen(0, m1 = getlab(), 'I');
		deflab(m);
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
		if (p2->in.type == DOUBLE || p2->in.type == FLOAT)
			codgen(p2->in.right, INTBREG);
		else
			codgen(p2->in.right, INTAREG);
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
		if (cookie == INTEMP) {
			long t = freeparm(1) / 8;

			d14qarm(p1, t, 4);
#ifdef SEG
			printf("\ttest\t|_stkseg+%ld|(fp)\n", t);
#else
			if (t)
				printf("\ttest\t%ld(fp)\n", t);
			else
				printf("\ttest\t@fp\n");
#endif
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
		order(p, INTAREG);
		goto cleanup;
	case FLD:
		if (p1->in.op == STAR) {
			offstar(p1->in.left);
			goto again;
		}
	case UMINUS:
		if (p1->in.type == FLOAT || p1->in.type == DOUBLE) {
			order(p1, INTBREG);
			goto again;
		}
		order(p1, INAREG);
		goto again;
	case NAME:
		if (o == REG)
			goto nomat;
		order(p, INTAREG);
		goto again;
	case INIT:
		uerror("illegal initialization: order()");
		return;
	case UFCALL:
		p->in.right = 0;
	case FCALL:
		o = p->in.op = UFCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case UCALL:
		p->in.right = 0;
	case CALL:
		o = p->in.op = UCALL;
		if (gencall(p, cookie))
			goto nomat;
		goto cleanup;
	case USTCALL:
		p->in.right = 0;
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
		if (p->in.type == DOUBLE || p->in.type == FLOAT)
			order(p2->in.left, INTBREG);
		else
			order(p2->in.left, INTAREG);
		if (p->in.type == DOUBLE || p->in.type == FLOAT)
			order(p2, INTBREG);
		else
			order(p2, INTAREG);
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
store(p)
	register NODE *p;
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
			stocook = INTEMP;
		}
		store(p->in.left);
		return;
	case ANDAND:
	case OROR:
	case QUEST:
		markcall(p->in.right);
		if (p->in.right->in.su > fregs || p->in.right->in.fsu > ffregs) {
			stotree = p;
			stocook = INTEMP;
		}
#ifdef OLD_D14Q
		if (o == QUEST && p->in.type != FLOAT && p->in.type != DOUBLE) {	/* the value of a ?: goes through a temporary */
			stotree = p;
			stocook = INTEMP;
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
		if (p->in.left->in.type == LONG || p->in.left->in.type == ULONG)
			if (lmulflag++) {
				stotree = p;
				stocook = INTEMP;
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
d14qarm(q, t, ty)
	NODE *q;
	long t;
	int ty;
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
	if (q->in.type == CHAR && ty == INT) {		/* a char value of an arm */
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
constore(p)
	register NODE *p;
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
			stocook = INTEMP;
		}
#endif
	case NOT:
		constore(p->in.left);
		return;
	}
	store(p);
}

/* count the calls below p */
void
markcall(p)
	register NODE *p;
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
	case UTYPE:
		p = p->in.left;
		goto again;
	case LTYPE:
		return;
	}
}

/* store the arguments of a call */
void
stoarg(p, calltype)
	register NODE *p;
	int calltype;
{
	if (p->in.op == CM) {
		stoarg(p->in.left, calltype);
		p = p->in.right;
	}
	if (calltype == 0x46 || calltype == 0x67)
		;		/* the original tests this and does nothing */
	callflag = 0;
	store(p);
	if (callflag) {
		stotree = p;
		stocook = INTEMP;
		store(p);
	}
}

/* evaluate p for the truth value and jump to true or false; a label < 0 means to fall through */
int negrel[] = { NE, EQ, GT, GE, LT, LE, UGT, UGE, ULT, ULE, FNE, FEQ, FGT, FGE, FLT, FLE };	/* 0x274 */

#ifdef OLD_D3H
static void cbranch1();
int d3hdepth;	/* OLD_D3H (libcisam): cbranch nesting; allo3.c marks (RSD3H) the registers it gives out meanwhile.
			   At the end of a whole condition an idle register given out inside it that holds the frame home of
			   a parameter goes back (compare/libcisam/COMPILER.md) */

/* OLD_D3H: does the node hold the frame home of a parameter */
static int
d3hparm(n)
	register struct rs *n;
{
	register int i;

	if (n->sym == 0 || (n->sym != fpsym1 && n->sym != fpsym2))
		return 0;
	for (i = 0; i < nparmtab; i++)
		if (parmtab[i].sym && parmtab[i].sym->soffset / 8 == n->off)
			return 1;
	return 0;
}

void
cbranch(p, true, false)
	NODE *p;
	int true, false;
{
	register struct rs *n, *next;

	if (d3hdepth == 0)
		for (n = rsuse; n; n = n->next)
			n->flags &= ~RSD3H;
	d3hdepth++;
	cbranch1(p, true, false);
	if (--d3hdepth == 0)
		for (n = rsuse; n; n = next) {
			next = n->next;
			if (n->cls <= RCPAIR && n->busy <= 0 && n->kind != RKTEMP && n->reg < 8 && (n->flags & RSD3H) && d3hparm(n))
				rsreturn(n->cls, n->reg);
		}
}

static void
cbranch1(p, true, false)
#else
void
cbranch(p, true, false)
#endif
	register NODE *p;
	int true, false;
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
	case FEQ:
	case FNE:
	case FLE:
	case FLT:
	case FGE:
	case FGT:
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
			case FEQ:
			case FNE:
			case FLE:
			case FLT:
			case FGE:
			case FGT:
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
#ifdef OLD_D3W
		d3depth++;
#endif
		lab = false < 0 ? getlab() : false;
		cbranch(p->in.left, -1, lab);
		cbranch(p->in.right, true, false);
		if (false < 0)
			deflab(lab);
		p->in.op = FREE;
#ifdef OLD_D3W
		if (--d3depth == 0)
			d3words();
#endif
#ifndef OLD_D3
#ifdef OLD_D3K
		rfreeall(2);	/* v0: a parameter stays known in its register */
#else
		rfreeall(1);
#endif
#endif
		return;
	case OROR:
#ifdef OLD_D3W
		d3depth++;
#endif
		lab = true < 0 ? getlab() : true;
		cbranch(p->in.left, lab, -1);
		cbranch(p->in.right, true, false);
		if (true < 0)
			deflab(lab);
		p->in.op = FREE;
#ifdef OLD_D3W
		if (--d3depth == 0)
			d3words();
#endif
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
#ifdef OLD_D3W
		d3depth++;
#endif
		cbranch(p->in.left, false, true);
		p->in.op = FREE;
#ifdef OLD_D3W
		if (--d3depth == 0)
			d3words();
#endif
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
	default: {
		int t;

		t = p->in.type;
		codgen(p, FORCC);
		if (t == FLOAT || t == DOUBLE) {
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
	NODE *p;
	int down;
	int *down1, *down2;
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
oreg2(p)
	register NODE *p;
{
	register NODE *q;
	register int spare1;		/* unused: the original reserves this register */
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
#ifdef SEG
				if (ql->in.sname->sclass == STATIC && !(ql->in.sname->sflags >> 2 & 1))
#else
				if (ql->in.sname && ql->in.sname->sclass == STATIC && !(ql->in.sname->sflags >> 2 & 1))
#endif
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
canon(p)
	register NODE *p;
{
	fwalk(p, ffld, 0);
	walkf(p, oreg2);
	walkf(p, mycanon);
	walkf(p, sucomp);
	return p;
}

/* a structure argument is stored through a temporary: STARG becomes an STASG to the stack */
NODE *
argprep(p, cookie)
	register NODE *p;
	register int cookie;
{
	register NODE *copy;
	struct optab fake;
	NODE *n;

	if (p->in.op == STARG && cookie == INTEMP) {
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

#ifdef OLD_D2FA
/* OLD_D2FA: the tree p has an && or || */
int
d2faandor(p)
	register NODE *p;
{
	register int k;

	if (p->in.op == ANDAND || p->in.op == OROR)
		return 1;
	k = optype(p->in.op);
	if (k == LTYPE)
		return 0;
	if (d2faandor(p->in.left))
		return 1;
	return k == BITYPE && d2faandor(p->in.right);
}
#endif

#if defined(OLD_D24) || defined(OLD_D63)
/* fwalk in front of optim (mip/optim.2.5) for the older versions: a conversion the older optim keeps is hidden from rmconv
   as PCONV (no record of cparse has one): vk, vl, vc (OLD_D24) one between a pointer and a non-pointer, v1 (OLD_D63) a
   float widened to double */
int
optpre(p)
	register NODE *p;
{
	register int t, u;

	if (p->in.op != SCONV)
		return 0;
	t = p->in.type;
	u = p->in.left->in.type;
#ifdef OLD_D24
	if ((t & 0x30) != (u & 0x30) && (ISPTR(t) || ISPTR(u)))
		p->in.op = PCONV;
#endif
#ifdef OLD_D63
	if (t == DOUBLE && u == FLOAT)
		p->in.op = PCONV;
#endif
	return 0;
}

/* fwalk behind optim: the hidden conversions are SCONV again; vk, vl, vc (OLD_D24): the conversion of a long to a
   pointer masks the address (template 040a), so a mask 0x7f00ffff in front of it is dropped */
int
optpost(p)
	register NODE *p;
{
	register NODE *a;

	if (p->in.op != PCONV)
		return 0;
	p->in.op = SCONV;
#ifdef OLD_D24
	if (ISPTR(p->in.type) && (a = p->in.left)->in.op == AND && a->in.right->in.op == ICON &&
			a->in.right->in.sname == 0 && a->in.right->tn.lval == 0x7F00FFFFL) {
		p->in.left = a->in.left;
		a->in.right->in.op = FREE;
		a->in.op = FREE;
	}
#endif
	return 0;
}
#endif

/* ---- v0 and v0p (OLD_D3K, OLD_D3J): the parameters known in their registers at labels and behind jumps ---- */
#ifdef OLD_D3K
/* the labels a jump has named in this function */
#define D3KMAX	16384
static char d3kbits[D3KMAX / 8];
#ifdef OLD_D3J
/* v0p: a record for each label jumps have named and that is not defined yet: the parameters known at all those jumps */
#define D3JMAX	8
static struct d3j {
	int	lab;		/* -1: free */
	int	n;
	int	reg[4], cls[4];
	struct sym *sym[4];
} d3jtab[D3JMAX];
static void d3jmark();
extern void rfree(), rsreturn();
#endif

/* the start of a function */
void
d3kreset()
{
	register int i;

	for (i = 0; i < D3KMAX / 8; i++)
		d3kbits[i] = 0;
#ifdef OLD_D3J
	for (i = 0; i < D3JMAX; i++)
		d3jtab[i].lab = -1;
#endif
}

/* a jump to the label l */
void
d3kjump(l)
	int l;
{
#ifdef OLD_D3J
	register struct d3j *j;
	register struct rs *n;
	register int i, k;
	struct d3j *f;
#endif

	if (l >= 0 && l < D3KMAX)
		d3kbits[l >> 3] |= 1 << (l & 7);
#ifdef OLD_D3J
	f = 0;
	for (j = d3jtab; j < &d3jtab[D3JMAX]; j++) {
		if (j->lab == l)
			break;
		if (j->lab < 0 && f == 0)
			f = j;
	}
	if (j == &d3jtab[D3JMAX]) {	/* the first jump: the known parameters */
		if ((j = f) == 0)
			return;
		j->lab = l;
		j->n = 0;
		for (n = rsuse; n; n = n->next)
			if (n->cls <= RCPAIR && n->sym && n->sym->sclass == PARAM && j->n < 4) {
				j->reg[j->n] = n->reg;
				j->cls[j->n] = n->cls;
				j->sym[j->n] = n->sym;
				j->n++;
			}
		return;
	}
	for (i = 0; i < j->n; i++) {	/* a later one: those known at both */
		for (n = rsuse; n; n = n->next)
			if (n->reg == j->reg[i] && n->cls == j->cls[i] && n->sym == j->sym[i])
				break;
		if (n == 0) {
			for (k = i + 1; k < j->n; k++) {
				j->reg[k - 1] = j->reg[k];
				j->cls[k - 1] = j->cls[k];
				j->sym[k - 1] = j->sym[k];
			}
			j->n--;
			i--;
		}
	}
#endif
}

/* the label lab: 0 when no jump has named it (v0p: all records go, a loop head); else the idle registers go back and
   a parameter stays known in its register (v0p: only one known at all the jumps; dead: the code in front of the label
   ends with a jump, the parameters of the jumps come back) */
int
d3klabel(lab, dead)
	int lab, dead;
{
#ifdef OLD_D3J
	register struct d3j *j;
	register int i, r;
	register struct rs *n;
	struct rs *next;
#endif

	if (lab < 0 || lab >= D3KMAX || !(d3kbits[lab >> 3] >> (lab & 7) & 1)) {
#ifdef OLD_D3J
		for (j = d3jtab; j < &d3jtab[D3JMAX]; j++)
			j->lab = -1;
#endif
		return 0;
	}
	rsfreeall(4);
#ifdef OLD_D3J
	for (j = d3jtab; j < &d3jtab[D3JMAX]; j++)
		if (j->lab == lab)
			break;
	if (j == &d3jtab[D3JMAX])
		return 1;
	j->lab = -1;
	for (n = rsuse; n; n = next) {	/* a parameter not known at every jump is forgotten */
		next = n->next;
		if (n->cls > RCPAIR || !n->sym || n->sym->sclass != PARAM || n->busy > 0)
			continue;
		if (!dead) {
			for (i = 0; i < j->n; i++)
				if (n->reg == j->reg[i] && n->cls == j->cls[i] && n->sym == j->sym[i])
					break;
			if (i < j->n)
				continue;
		}
		rsreturn(n->cls, n->reg);
	}
	if (dead)
		for (i = 0; i < j->n; i++) {
			r = getreg(j->cls[i], j->reg[i], 0, j->sym[i]);
			if (r == j->reg[i]) {
				rfree(r, j->cls[i] == RCPAIR ? LONG : j->cls[i] == RCBYTE ? CHAR : INT);
				/* a long parameter keeps its home, not a pointer (a pair in segmented code: gopt canopt) */
				if (j->cls[i] == RCPAIR && !ISPTR(j->sym[i]->stype))
					d3jmark(j->cls[i], r);
			}
		}
#endif
	return 1;
}

#ifdef OLD_D3J
/* the node of (cls, reg) got its parameter back at a label: it still needs its home in the frame (regargs) */
static void
d3jmark(cls, reg)
	int cls, reg;
{
	register struct rs *n;

	for (n = rsuse; n; n = n->next)
		if (n->cls == cls && n->reg == reg)
			n->flags |= RSD3J;
}
#endif
#endif

/* ---- v0p (OLD_D12K): the double constants at their statements (from rdr.2.5; rddata is d12kdata there) ---- */
#ifdef OLD_D12K
static int d12kcopy();

/* fwalk: a NAME of a label (eread made it of an FCON) puts out the double constant of that label if it waits */
int
d12kname(p)
	register NODE *p;
{
	if (p->in.op == NAME && p->tn.rval < 0)
		d12kout(-p->tn.rval);
	return 0;
}

/* rddata of rdr.2.5 for v0p: the section goes to d12kcopy */
int
d12kdata()
{
	long n;

	n = rdin2(10, dfile);
	if (getc(dfile) != '\n')
		cerror("data file format error\n");
	return d12kcopy(n);
}
#endif
#ifdef OLD_D12K
/* v0p: a double constant is written at its statement, not with the data at the head of the function: the blocks
   "Ln: " and four words of the data wait in d12ktab for d12kout */
#define D12KMAX	16
static struct { int lab; char text[120]; } d12ktab[D12KMAX];

static int
d12kcopy(n)
	long n;
{
	static char line[300];
	register int i, c;
	int k, lab;
	char *q;

	while (n > 0) {
		i = 0;
		while (n > 0) {
			c = getc(dfile);
			if (c == EOF)
				return 0;
			n--;
			if (i < 298)
				line[i++] = c;
			if (c == '\n')
				break;
		}
		line[i] = 0;
		if (line[0] == 'L' && i > 3 && line[i - 3] == ':' && line[i - 2] == ' ' && line[i - 1] == '\n') {
			lab = 0;
			for (q = line + 1; *q >= '0' && *q <= '9'; q++)
				lab = lab * 10 + *q - '0';
			for (k = 0; k < D12KMAX && d12ktab[k].lab; k++)
				;
			if (k < D12KMAX) {
				d12ktab[k].lab = lab;
				q = d12ktab[k].text;
				sprintf(q, "%s", line);
				q += strlen(q);
				for (i = 0; i < 4 && n > 0; i++) {
					for (;;) {
						c = getc(dfile);
						if (c == EOF)
							return 0;
						n--;
						*q++ = c;
						if (c == '\n' || n <= 0)
							break;
					}
				}
				*q = 0;
				continue;
			}
		}
		if (fwrite(line, 1, strlen(line), stdout) != strlen(line))
			return 0;
	}
	return 1;
}

/* the constant with the label lab (0: all that wait) goes out now */
void
d12kout(lab)
	register int lab;
{
	register int k;

	for (k = 0; k < D12KMAX; k++)
		if (d12ktab[k].lab && (lab == 0 || d12ktab[k].lab == lab)) {
#ifdef SEG
			printf("\t.psec data\n");
#endif
			printf("\t.data\n\t.even\n%s", d12ktab[k].text);
#ifdef SEG
			printf("\t.psec\n");
#endif
			printf("\t.code\n");
			d12ktab[k].lab = 0;
		}
}
#endif
