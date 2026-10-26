/*
 * main3.c - the program: options, start of the run, the commands of the intermediate file (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: options() 0x002a, init() 0x019e, main() 0x023c, the entry is crt0 at 0x0000.
 * Usage: codgen -E name [-e] [-l] [-O] [-p] [-R] [-M x]: the intermediate file comes on the standard input,
 * name.S (symbols) and name.D (data) are read, the assembler text goes to the standard output.
 * The commands (first character of a line): { begin of a function, } its end, [ register variables, . a statement
 * tree, / a comment, # skipped, L label, B branch, C a table of labels, F R S T lines for the frame.
 * Names are guesses.
 */
#include "cg.h"

extern int cerror(), uerror();
extern char *malloc();
extern long ftell();
extern long rdin();
extern struct node *eread();
extern void tinit(), dopeinit(), nlcheck(), mkhash(), stmt(), deflab(), branch(), rsfreeall();
extern int rddata(), rdsym();
extern void setrew(), allocinit(), tcheck();
extern void efcode(), lineout(), fcmd(), rcmd(), scmd(), tcmd(), regreset(), regtabinit(), rsinit(), rsbuild(), endrun();
extern int nparmtab;
extern struct { struct sym *sym; int reg; } parmtab[];
extern void hdrout();
extern FILE *stfile, *dfile;
extern long stg, stpos;
extern char ifname[];
extern int ifline;
extern int fregs, ffregs;
extern struct sym *symtab0;
extern int nerrors;

int pflag, rflag, eflag, oflag, lflag;	/* 0x40, 0x42, 0x44, 0x4c, 0x8f22 */
long frame0;			/* 0x69ce */
extern long maxoff, parmax, baseoff, maxtemp, paroff, tmpoff;
int nlabsave;			/* 0x69e0 */
int nparm;			/* 0x69d2 */
#ifdef SEG
int minreg = 0xD, maxreg = 0x17;	/* 0x958a, ... */
int regtop = 0xD;		/* 0x958e */
#else
int minreg = 0xE, maxreg = 0x17;	/* 0x8f1c, 0x8f90 */
int regtop = 0xE;		/* 0x8f20 */
#endif
int regmask;			/* 0x8fc2 */
int regA, regB, regC, regD;	/* 0x8e12 (7), 0x8f1e (0x10), 0x8e18 (0x13), 0x8f8c (1) */
int regE, regF, regG;		/* 0x8fc4, 0x8f8e, 0x8e10 */

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
main(argc, argv)
	register int argc;
	register char **argv;
{
	register int i, c;
	register int n, lab, k;
	long l1;
	int a, b;

	i = options(argc, argv);
	tinit();
	if (i)
		goto scan;
	goto loop;
scan:
	while (i < argc) {
		if (*argv[i] != '-')
			break;
		i++;
	}
	if (i > argc)
		goto end;
	freopen(argv[i], "r", stdin);
loop:
	c = getchar();
	if (c <= 0)
		goto eof;
	switch (c) {
	case '{':
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
		goto loop;
	case '}':
		if (parmax % 16)
			parmax = (parmax / 16 + 1) << 4;
		if (maxoff % 16)
			maxoff = (maxoff / 16 + 1) << 4;
		efcode();
		nlcheck();
		allocinit(symtab0);
		goto loop;
	case ']':
		nlcheck();
		goto loop;
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
		goto loop;
	case '.':
		ifline = rdin(10);
		{
			register char *cp;

			cp = ifname;
			do {
				c = getchar();
				*cp++ = c;
			} while (c != '\n');
			cp[-1] = 0;
		}
		if (lflag)
			lineout(ifline, ifname);
		paroff = baseoff;
		tmpoff = 0;
		stmt(eread());
		goto loop;
	case 'L':
		lab = rdin(10);
		deflab(lab);
		nlcheck();
		if (nparm <= 0)
			rsfreeall(1);
		if (rflag)
			regreset(1);
		goto loop;
	case 'B':
		branch(rdin(10));
		nlcheck();
		goto loop;
	case 'F':
		fcmd(rdin(10));
		nlcheck();
		goto loop;
	case 'R':
		a = rdin(10);
		b = rdin(10);
		if (b > 0)
			rcmd(a, b, 0);
		else
			rcmd(a, b, (int)rdin(10));
		nlcheck();
		goto loop;
	case 'S':
		scmd(rdin(10));
		nlcheck();
		goto loop;
	case 'T':
		tcmd(rdin(10));
		nlcheck();
		goto loop;
	case '/':
		putchar(c);
		do {
			c = getchar();
			putchar(c);
		} while (c != '\n');
		goto loop;
	case '#':
		do
			c = getchar();
		while (c != '\n');
		goto loop;
	case 'C':
		do
			c = getchar();
		while (c == ' ');
		ungetc(c, stdin);
		n = rdin(10);
#ifdef SEG
		printf("\t.psec data\n");
#endif
		printf("\t.data\n");
		printf("\t.even\n");
		printf("L%d:\n", n);
		rdin(10);
		for (c = getchar(); c != '\n'; c = getchar()) {
			ungetc(c, stdin);
			n = rdin(10);
			printf("\t.addr\tL%d\n", n);
		}
#ifdef SEG
		printf("\t.psec\n\t.code\n");
#else
		printf("\t.code\n");
#endif
		goto loop;
	default:
		cerror("intermediate file format error");
	}
eof:
	while ((c = getc(dfile)) != -1) {
		ungetc(c, dfile);
		rddata();
	}
	if (i)
		goto scan;
	endrun();
end:
	return nerrors;
}
