#
/*
 *	 C object code improver
 */

#include "c2.h"
int dddflg = 0;
char *rline();
struct optab cctab[] = {
	"eq",	JEQ,
	"z",	JEQ,
	"ne",	JNE,
	"nz",	JNE,
	"le",	JLE,
	"ge",	JGE,
	"lt",	JLT,
	"gt",	JGT,
	"c",	JLO,
	"ult",	JLO,
	"ugt",	JHI,
	"ule",	JLOS,
	"uge",	JHIS,
	"nc",	JHIS,
	"pl",	PL,
	"mi",	MI,
	0,	0};

struct optab optab[] = {
	"jpr	eq",	CBR | JEQ<<8,
	"jpr	ne",	CBR | JNE<<8,
	"jpr	le",	CBR | JLE<<8,
	"jpr	ge",	CBR | JGE<<8,
	"jpr	lt",	CBR | JLT<<8,
	"jpr	gt",	CBR | JGT<<8,
	"jpr	c",	CBR | JLO<<8,
	"jpr	ugt",	CBR | JHI<<8,
	"jpr	ule",	CBR | JLOS<<8,
	"jpr	uge",	CBR | JHIS<<8,
	"jpr	pl",	CBR | PL<<8,
	"jpr	mi",	CBR | MI<<8,
	"jpr",	JMP,
	"djnz",	DJNZ,
	"{",	EROU,
	"ld",	LD,
	"ldk",	LDK,
	"ldir",	LDI,
	"lddr",	LDD,
	"clr",	CLR,
	"com",	COM,
	"inc",	INC,
	"dec",	DEC,
	"neg",	NEG,
	"test",	TST,
	"sra",	ASR,
	"sla",	ASL,
	"exts",	SXT,
	"cp",	CMP,
	"add",	ADD,
	"sub",	SUB,
	"or",	OR,
	"and",	AND,
	"set",	SET,
	"res",	RES,
	"bit",	BIT,
	"mult",	MUL,
	"div",	DIV,
	"ex",	EX,
	"pop",	POP,
	"push",	PUSH,
	"srl",	SRL,
	"sll",	SLL,
	"sdl",	SDL,
	"sda",	SDA,
	"xor",	XOR,
	".psec",	CP,
	".data",	CD,
	".data",	BSS,
	".code",	CODE,
	"//",	CMT,
	"	.even",	EVEN,
	"callr",	CALL,
	0,	0};

char revbr[] = {JNE, JEQ, MI, PL, JGT, JLT, JGE, JLE, JHIS, JLOS, JHI, JLO};
int	isn	= 9000;
int	lastseg	= CD;

main(argc, argv)
char **argv;
{
	register int niter, maxiter, isend;
	extern end;
	register c;

	if (argc>1 && argv[1][0]=='+') {
		debug++;
		if(argv[1][1]=='+'){
			debug++;
			if(argv[1][2]=='+')
				debug++;
		}
		argc--;
		argv++;
	}
	if (argc>1) {
		if (freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "lpopt: can't find %s\n", argv[1]);
			exit(1);
		}
	}
	for(c = 0; c < 10; c++)
		address[c] = (char *)0;
	if (argc>2) {
		if (freopen(argv[2], "w", stdout) == NULL) {
			fprintf(stderr, "lpopt: can't create %s\n", argv[2]);
			exit(1);
		}
	}
	addrseg = 0;
	lasta = (char *)malloc(0xfff8);
	firstr = lasta;
	address[0] = lasta;
	lastr = lasta + 0xfff8;
	maxiter = 0;
	opsetup();
	do {
		nchange = 0;
		isend = input();
		if(!isend) break;
		niter = 0;
		do {
			refcount();
			do {
				iterate();
				niter++;
			} while (nchange);
			clearreg();
			comjump();
			rmove();
		} while (nchange );
		addsob();
		output();
		if (niter > maxiter)
			maxiter = niter;
		addrseg = 0;
		lasta = address[0];
		lastr = lasta + 0xfff8;
		
	} while (isend);
	if (debug) {
		fprintf(stderr, "%d iterations\n", maxiter);
		fprintf(stderr, "%d jumps to jumps\n", nbrbr);
		fprintf(stderr, "%d inst. after jumps\n", iaftbr);
		fprintf(stderr, "%d jumps to .+2\n", njp1);
		fprintf(stderr, "%d redundant labels\n", nrlab);
		fprintf(stderr, "%d cross-jumps\n", nxjump);
		fprintf(stderr, "%d code motions\n", ncmot);
		fprintf(stderr, "%d branches reversed\n", nrevbr);
		fprintf(stderr, "%d redundant moves\n", redunm);
		fprintf(stderr, "%d simplified addresses\n", nsaddr);
		fprintf(stderr, "%d loops inverted\n", loopiv);
		fprintf(stderr, "%d redundant jumps\n", nredunj);
		fprintf(stderr, "%d common seqs before jmp's\n", ncomj);
		fprintf(stderr, "%d skips over jumps\n", nskip);
		fprintf(stderr, "%d sob's added\n", nsob);
		fprintf(stderr, "%d redundant tst's\n", nrtst);
		fprintf(stderr, "%d literals eliminated\n", nlit);
		for(addrseg=0;address[addrseg];addrseg++);
		fprintf(stderr, "%d *64K core\n",addrseg+1);
	}
	exit(0);
}

input()
{
	register struct node *p, *lastp;
	register int oper;
	register char *cc, *c1, *c2;
	struct optab *ct;
	static char tp[10];
	lastp = &first;
	do {
		oper = getchar();
		if(oper == EOF)
			exit(0);
		putchar(oper);
	} while ( oper != '{');
	getchar(); putchar('\n');
	for (;;) {
		oper = getline();
		if(oper == CP && *(line+5)) {
			if(!(strcmp(line+5," data"))) {
				dddflg = 1;
				if(debug > 1)
					puts("dddflg erhoeht");
				goto weiter;
			}
			printf("\t%s\n",line);
			do {
				cc = rline();
				puts(cc);
			} while(oper = strcmp(cc,"\t.code"));
			continue;
		}
weiter:
		p = (struct node *)alloc(sizeof first);
		p->op = oper&0377;
		p->code = (char *)0;
		p->subop = 0;

		switch (oper&0377) {
	
		case LABEL:
			if (line[0] == 'L') {
				p->op = LABEL;
				p->labno = getnum(line+1);
				if(dddflg){
					if(debug > 1)
						puts("label mit 100");
					p->ref = (struct node*)100;
				}
			} else {
				p->op = DLABEL;
				p->labno = 0;
				p->code = copy(1, line);
			}
			break;

	
		case CMT:
			p->op = 0;
			p->labno = 0;
			p->code = copy(1,line);
			break;

		case CBR:
			p->subop = oper >> 8;
		case JMP:
		case JSW:
			if(*curlp != 'L') {
				c2 = tp;
				for(cc = curlp; *cc && *cc!='\t' && *cc!=' '
					&& *cc!=',' && *cc!='\n';)
					*c2++ = *cc++;
				*c2++ = 0;

				ct = cctab;

				for(;ct->opstring!=0;ct++) {
					c1 = ct->opstring;
					c2 = tp;
					while(*c2 == *c1++)
						if(*c2++ == 0) {
							p->subop = ct->opcode;
							p->op -= 2;
							curlp = ++cc;
							goto jp;
						}
				}
			}
jp:
			if (*curlp=='L' && (p->labno = getnum(curlp+1)))
				p->code = 0;
			else {
				p->labno = 0;
				p->code = copy(1, curlp);
			}
			break;

		case XL1:
			xl1 = getnum(line);
			break;

		case XL2:
			xl2 = getnum(line);
			break;

		case EROU:
			break;

		case CODE:
			if(debug > 1)
				puts("dddflg runter!");
			dddflg = 0;
		default:
			p->subop = oper>>8;
			p->labno = 0;
			p->code = copy(1, curlp);
			break;

		}
		p->forw = 0;
		p->back = lastp;
		lastp->forw = p;
		lastp = p;
		if(p->op != LABEL || !dddflg)
			p->ref = 0;

		if(debug > 2) {
			printf("input line is: ");
			dbprint(p);
		}
		if (oper==EROU)
			return(1);
		if (oper==END)
			return(0);
	}
}

getline()
{
	register char *lp;
	register c;
	register c1;

	lp = line;
	while ((c = getchar())==' ' || c=='\t')
		;
	if(c=='}') {
		getchar();
		return(EROU);
	}
	do {
		if (c == '/')
			if ((c1=getchar()) != c)
				ungetc(c1,stdin);
			else
			{
				*lp++ = c;
				do
					*lp++ = c;
				while((c = getchar()) != '\n');
				*lp = 0;
				return(CMT);
			}

		if (c == ':') {
			c = LABEL;
			if ((c1=getchar()) == ':')
			{
				getchar();
				*lp++ = ':';
			} else if(c1 == '=' && *line == '~') {
				getchar();

				if(*(line+2)=='1')
					c = XL1;
				else
					c = XL2;
				lp = line;
				while((c1 = getchar()) != '\n')
					*lp++ = c1;
			}
			*lp = 0;
			return(c);
		}
		if (c=='\n') {
			*lp++ = 0;
			return(oplook());
		}
		if (lp >= &line[LSIZE-2]) {
			fprintf(stderr, "lpopt: Sorry, input line too long\n");
			exit(1);
		}
		*lp++ = c;
	} while ((c = getchar()) != EOF);
	*lp++ = 0;
	return(END);
}

getnum(ap)
char *ap;
{
	register char *p;
	register n, c;

	p = ap;
	n = 0;
	while ((c = *p++) >= '0' && c <= '9')
		n = n*10 + c - '0';
	if (*--p != 0)
		return(0);
	return(n);
}

output()
{
	register struct node *t;
	register struct optab *oper;
	register int byte;

	t = &first;
	while (t = t->forw) {

	switch (t->op) {
	case END:
		return;

	case LABEL:
		printf("L%d:\n", t->labno);
		continue;

	case DLABEL:
		printf("%s:\n", t->code);
		continue;

	case XL1:
		printf("	~L1 := %d\n",xl1);
		continue;

	case XL2:
		printf("	~L2 := %d\n",xl2);
		continue;

	case CD:
		lastseg = DATA;
		goto def;

	case BSS:
		lastseg = BSS;
		goto def;

	case EROU:
		printf("\t}\n");
		return;

	case CODE:
	case CP:
		lastseg = CODE;

	def:
	default:
		if ((byte = t->subop)==BYTE ||byte==LONG)
			t->subop = 0;
		for (oper = optab; oper->opstring!=0; oper++) 
			if ((oper->opcode&0377) == t->op
			 && (oper->opcode>>8) == t->subop) {
				printf("\t%s", oper->opstring);
				if (byte==BYTE)
					putchar('b');
				else if(byte==LONG)
					putchar('l');
				break;
			}
		if (t->code)
			printf("\t%s\n", t->code);
		else if (t->op == JMP)
			printf("\tL%d\n", t->labno);
		else if (t->op == CBR)
			printf(",L%d\n", t->labno);
		else
			printf("\n");
		continue;

	case JSW:
		printf("\t.addr L%d\n", t->labno);
		continue;

	case DJNZ:
		printf("\tdjnz	%s",t->code);
		if (t->labno)
			printf(",L%d\n",t->labno);
		continue;
	case 0:
		if (t->code)
			printf("\t%s\n", t->code);
		continue;
	}
	}/*while*/
}

char *
copy(na, ap)
char *ap;
{
	register char *p, *np;
	register char *onp;
	register n;

	p = ap;
	n = 0;
	if (*p==0)
		return(0);
	do
		n++;
	while (*p++);
	if (na>1) {
		p = (&ap)[1];
		while (*p++)
			n++;
	}
	if(strchr(ap,'_'))
		n += 2;
	onp = np = alloc(n);
	p = ap;
	while (*np++ = *p++)
		;
	if (na>1) {
		p = (&ap)[1];
		np--;
		while (*np++ = *p++);
	}
	return(onp);
}

opsetup()
{
	register struct optab *optp, **ophp;
	register char *p;

	for (optp = optab; p = optp->opstring; optp++) {
		ophp = &ophash[(((p[0]<<3)+(p[1]<<1)+p[2])&077777) % OPHS];
		while (*ophp++)
			if (ophp > &ophash[OPHS])
				ophp = ophash;
		*--ophp = optp;
	}
}

oplook()
{
	register struct optab *optp;
	register char *lp, *np;
	char c;
	static char tmpop[32];
	register struct optab **ophp;

	if (line[0]=='\0') {
		curlp = line;
		return(0);
	}
	np = tmpop;
	for (lp = line; *lp && *lp!=' ' && *lp!='\t';)
		*np++ = *lp++;
	*np++ = 0;
	while (*lp=='\t' || *lp==' ')
		lp++;
	curlp = lp;
	c = tmpop[2];
	if((tmpop[2]=='b'||tmpop[2]=='l'||tmpop[2]=='s'||tmpop[2]=='d') &&
	((tmpop[0]=='l'&&tmpop[1]=='d')||(tmpop[0]=='c'&&tmpop[1]=='p')||
	(tmpop[0]=='o'&&tmpop[1]=='r')||(tmpop[0]=='e'&&tmpop[1]=='x')))
		tmpop[2] = 0;
	ophp = &ophash[(((tmpop[0]<<3)+(tmpop[1]<<1)+tmpop[2])&077777) % OPHS];
		tmpop[2] = c;
	while (optp = *ophp) {
		np = optp->opstring;
		lp = tmpop;
		while (*lp == *np++)
			if (*lp++ == 0)
				return(optp->opcode);
		c = *lp++;
		if((c=='b'||c=='l'||c=='s'||c=='d') && *lp++==0 && *--np==0)
			if(c=='b')
				return(optp->opcode + (BYTE<<8));
			else if(c=='l')
				return(optp->opcode + (LONG<<8));
		ophp++;
		if (ophp >= &ophash[OPHS])
			ophp = ophash;
	}
	if (line[0] == '\.' && line[1] == 'a' && line[6] == 'L') {
		lp = line+7;
		while (*lp)
			if (*lp<'0' || *lp++>'9')
				return(0);
		curlp = line+6;
		return(JSW);
	}
	curlp = line;
	return(0);
}

refcount()
{
	register struct node *p, *lp;
	static struct node *labhash[LABHS];
	register struct node **hp, *tp;
	char *sss;

	if(debug > 1)
		printf("refcount works\n");
	for (hp = labhash; hp < &labhash[LABHS];)
		*hp++ = 0;
	for (p = first.forw; p!=0; p = p->forw) {
		if(debug > 2){
			printf("refcount (1) works on:");
			dbprint(p);
		}
		if(p->code&&!strchr(p->code,'|')&&(sss = strchr(p->code,'_')))
			mycode(sss);
		if(p->op==0 && p->code && p->code[0]=='I') {
			if(p->forw->op==END ||p->forw->op==0 &&p->forw->code &&
				(p->forw->code[0]=='G'||p->forw->code[0]=='I')){
					elim(p);
					p = p->forw;
					continue;
			}
		}
		if (p->op==LABEL) {
			labhash[p->labno % LABHS] = p;
			if(p->ref == (struct node *)100)
				p->refc = 100;
			else
				p->refc = 0;
		}
	}
	for (p = first.forw; p!=0; p = p->forw) {
		if(debug > 2){
			printf("refcount (2) works on:");
			dbprint(p);
		}
		if (p->op && p->op <= JSW) {
			p->ref = 0;
			lp = labhash[p->labno % LABHS];
			if (lp==0 || p->labno!=lp->labno)
			for (lp = first.forw; lp!=0; lp = lp->forw) {
				if (lp->op==LABEL && p->labno==lp->labno)
					break;
			}
			if (lp) {
				tp = nonlab(lp)->back;
				if (tp!=lp) {
					p->labno = tp->labno;
					lp = tp;
				}
				p->ref = lp;
				lp->refc++;
			}
		}
	}
	for (p = first.forw; p!=0; p = p->forw)
		if (p->op==LABEL && p->refc==0
		 && (lp = nonlab(p))->op && lp->op!=JSW)
			decref(p);
}

iterate()
{
	register struct node *p, *rp, *p1;

	if(debug > 1)
		printf("iterate works\n");
	nchange = 0;
	for (p = first.forw; p!=0; p = p->forw) {
		if (p->op && p->op <= JSW && p->ref) {
		if(debug > 2){
			printf("iterate works on:");
			dbprint(p);
		}
			rp = nonlab(p->ref);
			if (rp->op==JMP && rp->labno && p->labno!=rp->labno) {
				nbrbr++;
				p->labno = rp->labno;
				decref(p->ref);
				rp->ref->refc++;
				p->ref = rp->ref;
				nchange++;
			}
		}
		if (p->op==CBR && (p1 = p->forw)->op==JMP && p1->labno) {
			rp = p->ref;
			do
				rp = rp->back;
			while (rp->op==LABEL);
			if (rp==p1) {
				decref(p->ref);
				p->ref = p1->ref;
				p->labno = p1->labno;
				p1->forw->back = p;
				p->forw = p1->forw;
				p->subop = revbr[p->subop];
				nchange++;
				nskip++;
			}
		}
		if(p->op==CBR && (p1 = p->forw)->op==CBR &&
		p->labno==p1->labno && p->subop==revbr[p1->subop]) {
			elim(p1);
			p->subop = 0;
			p->op += 2;
			nchange++;
		}

		if (p->op==JMP) {
			while (p->forw && p->forw->op!=LABEL
				&& p->forw->op!=DLABEL
				&& p->forw->op!=EROU && p->forw->op!=END
				&& p->forw->op!=0 && p->forw->op!=DATA) {
				nchange++;
				iaftbr++;
				if (p->forw->ref)
					decref(p->forw->ref);
				p->forw = p->forw->forw;
				p->forw->back = p;
			}
			rp = p->forw;
			while (rp && rp->op==LABEL) {
				if (p->ref == rp) {
					elim(p);
					p = p->back;
					decref(rp);
					nchange++;
					njp1++;
					break;
				}
				rp = rp->forw;
			}
		}
		if (p->op==JMP) {
			xjump(p);
			p = codemove(p);
		}

		if ((p->op==CBR || p->op == JMP) && p->ref == p->forw) {
			decref(p->forw);
			elim(p);
			p = p->back;
			nchange++;
		}
	}
}

xjump(p1)
register struct node *p1;
{
	register struct node *p2, *p3;

	if ((p2 = p1->ref)==0)
		return;
	if(debug > 2){
		printf("xjump works on:");
		dbprint(p1);
	}
	for (;;) {
		while ((p1 = p1->back) && p1->op==LABEL);
		while ((p2 = p2->back) && p2->op==LABEL);
		if (!equop(p1, p2) || p1==p2 || p1->op>=CODE && p1->op<=CMT)
			return;
		p3 = insertl(p2);
		p1->op = JMP;
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
		return(oldp);
	}
	if (oldp->back->op == LABEL) {
		oldp = oldp->back;
		oldp->refc++;
		return(oldp);
	}
	lp = (struct node *)alloc(sizeof first);
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
	if(debug > 2){
		printf("insertl inserts oper:");
		dbprint(lp);
	}
	return(lp);
}

struct node *
codemove(p)
struct node *p;
{
	register struct node *p1, *p2, *p3;
	register struct node *t, *tl;
	register n;

	p1 = p;
	if(debug > 2){
		printf("codemove works on:");
		dbprint(p1);
	}
	if ((p1->op!=JMP) || (p2 = p1->ref)==0)
		return(p1);
	while (p2->op == LABEL)
		if ((p2 = p2->back) == 0)
			return(p1);
	if (p2->op!=JMP)
		goto ivloop;
	p2 = p2->forw;
	p3 = p1->ref;
	while (p3) {
		if (p3->op==JMP) {
			if (p1==p3)
				return(p1);
			ncmot++;
			nchange++;
			p1->back->forw = p2;
			p1->forw->back = p3;
			p2->back->forw = p3->forw;
			p3->forw->back = p2->back;
			p2->back = p1->back;
			p3->forw = p1->forw;
			decref(p1->ref);
			return(p2);
		} else
			p3 = p3->forw;
	}
	return(p1);
ivloop:
	if (p1->forw->op!=LABEL)
		return(p1);
	p3 = p2 = p2->forw;
	n = 16;
	do {
		if ((p3 = p3->forw) == 0 || p3==p1 || --n==0)
			return(p1);
	} while (p3->op && p3->op > CBR || p3->labno!=p1->forw->labno);
	do 
		if ((p1 = p1->back) == 0)
			return(p);
	while (p1!=p3);
	p1 = p;
	tl = insertl(p1);
	p3->subop = revbr[p3->subop];
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
	if (tl->refc<=0)
		nrlab--;
	loopiv++;
	nchange++;
	return(p3);
}

comjump()
{
	register struct node *p1, *p2, *p3;

	if(debug > 1)
		printf("comjump works\n");
	for (p1 = first.forw; p1!=0; p1 = p1->forw)
		if (p1->op==JMP && (p2 = p1->ref) && p2->refc > 1)
			for (p3 = p1->forw; p3!=0; p3 = p3->forw)
				if (p3->op && p3->op<=JMP && p3->ref == p2)
					backjmp(p1, p3);
}

/*
reljmp()
{
	register struct node *p;
	nchange = 0;

	for( p = first.forw; p != 0; p = p->forw)
		if((p->op==JMP || p->op==CBR) && p->labno && !toofar(p)) {
			p->op--;
			nchange++;
		}
}
*/

backjmp(ap1, ap2)
struct node *ap1, *ap2;
{
	register struct node *p1, *p2, *p3;

	if(debug > 1)
		printf("backjmp works\n");
	p1 = ap1;
	p2 = ap2;
	for(;;) {
		while ((p1 = p1->back) && p1->op==LABEL);
		p2 = p2->back;
		if (equop(p1, p2)) {
			p3 = insertl(p1);
			elim(p2);
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

/*
retc()
{
	register struct node *p, *p1;
	p = first.forw;
	while(p!=0 &&(p->op!=CALL || !equstr(p->code,"csv")))
		p = p->forw;
	p1 = p->forw;
	while(p1->op==0 && p1->code[0]=='!')
		p1 = p1->forw;
	if(p1->op==JMP && equstr(p1->code,"cret")) {
		elim(p);
		p1->op = 0;
		p1->code = "ret";
		return(1);
	}
	return(0);
}
*/

/* mycode for short addresses */
mycode(str)
char *str;
{
	register char *s = str;
	char *anf, *ende;
	char *stkp;
	unsigned xli = 256;

	while(*s) {
		while(*s && *s != '_')
			s++;
		if(!*s)
			return;
			/* _... gefunden */
		anf = s++;
		if(strncmp(s,"stkseg",6) || *(s+6) != '+') {
			/* kein stkseg */
			s = ++anf;
			continue;
		}
		/* stkseg gefunden, hinter '+' */
		s += 7;
		xli = 0;
		if(*s == '~' && *(s+1) == 'L') {
			s += 2;
			if(*s == '1')
				xli = xl1;
			else if(*s == '2')
				xli = xl2;
			else {
				fprintf(stderr, "lpopt: illegal ~L");
				continue;
			}
			s += 2;
		}
		/* hinter ~Li+ auf '%' */
		if(*s == '%') {
			s++;
			if(*(s-2) == '+')
				xli += hexa(s);
			else
				xli -= hexa(s);
		}
		while(*s && *s!='(')
			s++;
		if(*s != '(')
			return;
		ende = s;
		break;
	} /* while */
	if(xli > 0xff)
		return;
	while (*s++);
	s--;
	for(;s >= ende; s--)
		*(s+2) = *s;
	*(s+2) = '|';
	for(; s >= anf; s--)
		*(s+1) = *s;
	*(s+1) = '|';
	if(debug > 2)
		printf("mycode gives: %s\n",str);
	return;
}

dbprint(t)
struct node *t;
{
	register struct optab *oper;
	register int byte;

	printf("\t*(%lx)*",t);
	byte = t->subop;
	switch(t->op) {

	case END:
		printf("end\n");
		return;

	case LABEL:
		printf("L%d:\n", t->labno);
		return;

	case DLABEL:
		printf("%s:\n", t->code);
		return;

	case XL1:
		printf("	~L1 := %d\n",xl1);
		return;

	case XL2:
		printf("	~L2 := %d\n",xl2);
		return;

	case CD:
		lastseg = DATA;
		goto def;

	case BSS:
		lastseg = BSS;
		goto def;

	case EROU:
		printf("\t{\n");
		return;

	def:
	default:
		if (byte == BYTE ||byte==LONG)
			t->subop = 0;
		for (oper = optab; oper->opstring!=0; oper++) 
			if ((oper->opcode&0377) == t->op
			 && (oper->opcode>>8) == t->subop) {
				printf("\t%s", oper->opstring);
				if (byte==BYTE)
					putchar('b');
				else if(byte==LONG)
					putchar('l');
				t->subop = byte;
				break;
			}
		if (t->code)
			printf("\t%s\n", t->code);
		else if (t->op == JMP)
			printf("\tL%d\n", t->labno);
		else if (t->op == CBR)
			printf(",L%d\n", t->labno);
		else
			printf("\n");
		t->subop = byte;
		return;

	case JSW:
		printf("\t.addr L%d\n", t->labno);
		return;

	case DJNZ:
		printf("\tdjnz	%s",t->code);
		if (t->labno)
			printf(",L%d\n",t->labno);
		return;
	case 0:
		if (t->code)
			printf("\t%s\n", t->code);
		else
			printf("Leerzeile\n");
		return;
	}
}

char *
rline()
{
	static char fff[120];
	register char *c;
	register x;
	
	c = fff;
	while((x = getchar())!='\n'){
		*c++ = x;
	}
	*c = 0;
	return(fff);
}

