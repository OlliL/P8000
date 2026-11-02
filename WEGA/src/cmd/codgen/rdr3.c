/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "cg.h"
#include "cg2.h"

extern char *alloc();
extern char ascii[];
extern struct node *bcon();
extern struct node *buildstar();
extern int cerror();
extern struct node *clocal();
extern void d12kout();
extern FILE *dfile;
extern int dope[];
extern int eflag;
extern struct node *eread();
extern int firstsect;
extern void fixfld();
extern void fixtemp();
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern long ftell();
extern long gasciibytes;
extern int getreg();
extern int gnumbytes;
extern int gnumcount;
extern long gsymbytes;
extern struct node *makeblock();
extern struct sym *mapn();
extern void mapnm();
extern void mapsym();
extern void mkhash();
extern long nasciibytes;
extern int nglob;
extern void nlcheck();
extern int nmembytes;
extern int nmems;
extern int nnumbytes;
extern int nnums;
extern struct node node[];
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern int rclass();
extern int rddata();
extern long rdin();
extern long rdin2();
extern int rdsym();
extern void readmems();
extern void readnums();
extern void readsyms();
extern void readtab();
extern struct rv regvar[];
extern int rflag;
extern void sortsyms();
extern FILE *stfile;
extern int stflag;
extern long stg;
extern long stpos;
extern long symbase;
extern struct sym *symtab;
extern struct sym *symtab0;
extern int tagfind();
extern struct { int tag; struct sym *sym; } tagtab[];
extern struct node *talloc();
extern char *tnames[];
static char sccsid[] = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
static char rdrsccs[] = "@[$]rdr\t2.5  08/24/83 16:48:00 Copr. 1981 87wega3.2.";
char *tnames[] = {
	"UNDEF",
	"FARG",
	"CHAR",
	"SHORT",
	"INT",
	"LONG",
	"FLOAT",
	"DOUBLE",
	"STRTY",
	"UNIONTY",
	"ENUMTY",
	"MOETY",
	"UCHAR",
	"USHORT",
	"UNSIGNED",
	"ULONG",
	"undef",
	"PTR",
	"FTN",
	"ARY",
	"undef",
	"AUTO",
	"EXTERN",
	"STATIC",
	"REGISTER",
	"EXTDEF",
	"LABEL",
	"undef",
	"MOS",
	"PARAM",
	"STNAME",
	"MOU",
	"UNAME",
	"TYPEDEF",
	"unused",
	"ENAME",
	"MOE",
	"unused",
	"USTATIC",
};
char ascii[MAXASCII] = "NULL\0~L1\0~L2";		/* 0x630: the ascii table; "NULL", "~L1", "~L2" come first */
int stflag = 2;
long stpos = 0;
int firstsect = 1;


#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))




#ifdef OLD_D12K
static int d12kcopy();
#endif

static int d12kcopy();

/* copy a section of the data file to the output */
int
rddata()
{
	long n;
	char buf[512];
	int chunk, got;

	n = rdin2(10, dfile);
	if (getc(dfile) != '\n')
		cerror("data file format error\n");
#ifdef OLD_D12K
	return d12kcopy(n);
#endif
	while (n > 0) {
		if (n <= 0x200) {
			chunk = (int)n;
			n = 0;
		} else {
			chunk = 0x200;
			n -= 0x200;
		}
		got = fread(buf, 1, chunk, dfile);
		if (got != chunk)
			return 0;
		got = fwrite(buf, 1, chunk, stdout);
		if (got != chunk)
			return 0;
	}
	return 1;
}

/* the symbol table file: the local section, then the member table of the global one */
int
rdsym()
{
	nglob = 0;
	fseek(stfile, stpos, 0);
	stflag = getw(stfile);
	if (stflag <= 0)
		return 0;
	fread(&nsymbytes, 4, 1, stfile);
	fread(&nasciibytes, 4, 1, stfile);
	fread(&nnumbytes, 2, 1, stfile);
	fread(&nmembytes, 2, 1, stfile);
	nsyms = nsymbytes / 22;
	readsyms(nsyms, (struct sym *)((char *)symtab + 0x42), 0x15);
	nsyms += 3;
	if (nasciibytes + 0x15 > MAXASCII)
		cerror("local ascii table overflow\n");
	readtab(ascii + 0x15, nasciibytes);
	nnums = nnumbytes / 2;
	readnums(nnums);
	if (stflag) {
		nmems = nmembytes / 6;
		readmems(nmems);
	}
	sortsyms();
	return 1;
}

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

/* n symbols of 22 bytes into tab; kind is added to the name offsets */
void
readsyms(n, tab, kind)
	int n;
	struct sym *tab;
	int kind;
{
	register struct sym *q;
	register int k;

	if (n <= 0)
		return;
	alloc(n * 22);
	if (fread(tab, 22, n, stfile) != n)
		printf("readst: error in reading symbol table file stfile\n");
	k = n;
	for (q = tab; k > 0; k--, q = (struct sym *)((char *)q + 22)) {
		tagtab[nglob].tag = (q->sflags >> 4) & 0xFFF;
		tagtab[nglob].sym = q;
		nglob++;
		q->nameoff += kind;
		if (kind == 0x15)
			q->sflags &= ~4;
		else {
			q->sflags |= 4;
			q->sx14 = 0;
		}
	}
}

/* the ascii table */
void
readtab(buf, n)
	char *buf;
	long n;
{
	if (n > 0)
		if (fread(buf, 1, (int)n, stfile) != n)
			printf("readnt: error in reading ascii table from symbol file\n");
}

/* the number table of a section, behind the numbers already there */
void
readnums(n)
	int n;
{
	int *src, *dst;
	int base, k;
	int buf[500];

	if (n <= 0)
		return;
	if (n > 500)
		cerror(" member table overflow");
	base = firstsect ? 0 : nsyms;
	if (fread(buf, 2, n, stfile) != n)
		printf("readmt: error in reading number table from file stfile\n");
	src = buf;
	dst = &numtab[base * 2];
	for (k = n; k > 0; k--) {
		dst[0] = *src++;
		dst[1] = -1;
		dst += 2;
	}
}

/* the member table of the global section: the symbols it points to are read in runs of consecutive ones */
void
readmems(n)
	int n;
{
	static struct { long off; int x; } mem[550];
	struct { long off; int x; } *cur, *first;
	long base;
	struct sym *dp;
	int rem, dest, cnt, k;

	if (n <= 0)
		return;
	fread(mem, 6, n, stfile);
	stpos = ftell(stfile);
	cur = mem;
	if (firstsect) {
		symbase = stg + 14;
		fseek(stfile, stg + 2, 0);
		fread(&gsymbytes, 4, 1, stfile);
		fread(&gasciibytes, 4, 1, stfile);
		fread(&gnumbytes, 2, 1, stfile);
		fseek(stfile, symbase + gsymbytes, 0);
		if (gasciibytes + nasciibytes + 0x15 > MAXASCII)
			cerror("global ascii table overflow");
		readtab(ascii + (MAXASCII - gasciibytes), gasciibytes);
		readnums(gnumcount = gnumbytes / 2);
		firstsect = 0;
	}
	rem = n;
	dest = nsyms;
	while (rem > 0) {
		cnt = 1;
		first = cur;
		base = cur->off;
		cur++;
		for (k = rem - 1; k > 0; k--) {
			if (cur->off - first->off != 0x16)
				break;
			first = cur;
			cur++;
			cnt++;
		}
		dp = (struct sym *)((char *)symtab + dest * 22);
		fseek(stfile, symbase + base, 0);
		readsyms(cnt, dp, MAXASCII - gasciibytes);
		dest += cnt;
		rem -= cnt;
	}
	nsyms = dest;
}

/* sort the tags */
void
sortsyms()
{
	register int i, j;
	int t;
	struct sym *s;

	for (i = nglob - 1; i >= 0; i--)
		for (j = i - 1; j >= 0; j--)
			if (tagtab[i].tag < tagtab[j].tag) {
				t = tagtab[j].tag;
				s = tagtab[j].sym;
				tagtab[j] = tagtab[i];
				tagtab[i].tag = t;
				tagtab[i].sym = s;
			}
}

/* the index of the tag in tagtab, or -1 */
int
tagfind(tag)
	int tag;
{
	int lo, hi, mid;

	if (tag < 0 || nglob == 0)
		return -1;
	lo = 0;
	hi = nglob - 1;
	if (tag == tagtab[lo].tag)
		return lo;
	if (tag == tagtab[hi].tag)
		return hi;
	mid = (lo + hi) / 2;
	while (lo != mid) {
		if (hi == mid)
			return -1;
		if (tag == tagtab[mid].tag)
			return mid;
		if (tag < tagtab[mid].tag)
			hi = mid;
		else
			lo = mid;
		mid = (lo + hi) / 2;
	}
	return -1;
}

struct sym *
mapn(tag)
	int tag;
{
	int i;

	i = tagfind(tag);
	if (i < 0)
		cerror("rvmapn: tag %d not found", tag);
	return tagtab[i].sym;
}

/* the symbol of a node from its tag */
void
mapsym(p)
	struct node *p;
{
	int i;

	if (p->rval) {
		i = tagfind(p->rval);
		if (i < 0)
			cerror("mapn: tag %d not found", p->rval);
		p->sname = tagtab[i].sym;
		p->rval = 0;
	}
}

/* a leaf of the tree from the file: symbols, register variables and fields */
void
mapnm(p)
	struct node *p;
{
	struct sym *s;
	struct node *m, *nb;
	int n, t, rg, nt, ot;
	long ofs;

	if (eflag > 1)
		printf("mapnm, p->op=%d, p->rval=%d\n", p->op, p->rval);
	if (p->op == NAME && p->rval >= 0) {
		mapsym(p);
		s = p->sname;
		if (s->sclass == 4) {
			if (rflag) {
				n = s->sbyte5;
				t = s->stype;
#ifdef SEG
				if (t == 6 || t == 7)
					n -= 13;
#else
				if (t == 6 || t == 7)
					n -= 12;
#endif
				else
					n -= 7;
			}
			if (rflag && regvar[n].sym != p->sname) {
				s->sclass = regvar[n].rcls;
				goto done;
			}
			n = s->sbyte5;
			if (!rflag) {
				rg = rclass(p->type, n);
				n = getreg(rg, n, 0x1000, p->sname);
			}
			p->op = REG;
			p->rval = n;
			p->lval = 0;
		}
	done:
		if (eflag > 1)
			printf("mapnm, p->sname = %d, p->global=%d\n", p->sname, p->sname->sflags >> 2 & 1);
		return;
	}
	if (p->op == ICON) {
		if (p->sname != fpsym1 && p->sname != fpsym2)
			p->sname = 0;
		return;
	}
	if (p->op == TEMP) {
		fixtemp(p);
		return;
	}
	if (p->op != FLD) {
		p->sname = 0;
		return;
	}
	m = p->right;
	mapsym(m);
	mapnm(p->left);
	t = p->left->type;
	fixfld(p);
	s = m->sname;
	ofs = s->soffset / 16 * 2;
	p->rval = (int)(s->soffset % 16 << 6) | (s->sclass & 0x3F);
	if (p->left->op == NAME)
		p->left->lval += ofs;
	else if (ofs != 0) {
		nt = INCREF(s->stype);
		p->left->type = nt;
		if ((t & 0x30) != 0x10 && p->left->op == STAR) {
			p->left->op = FREE;
			nb = bcon(ofs << 3);
			n = (int)p->left->left;
		} else {
			nb = bcon(ofs << 3);
			n = (int)p->left;
		}
		p->left = clocal(makeblock(PLUS, n, nb, nt));
		p->left = buildstar(STAR, p->left, 0);
	}
	m->op = FREE;
}

/* the two pseudo symbols ~L1 and ~L2 of the frame */
void
mkhash()
{
	symtab = symtab0 = (struct sym *)alloc(0x42);
	fpsym1 = (struct sym *)((char *)symtab + 0x16);
	fpsym2 = (struct sym *)((char *)symtab + 0x2C);
	symtab->nameoff = 0;
	symtab[1].nameoff = 5;
	symtab[2].nameoff = 9;
}

/* the end of a line of the intermediate file */
void
nlcheck()
{
	if (getchar() != '\n')
		cerror("intermediate file format error");
}

/* a tree of the intermediate file */
int erflag = 0;

struct node *
eread()
{
	register struct node *p;
	register int op;
	register int spare1, spare2;	/* unused: the original reserves these registers */
	register int ty;
	char c;

	for (;;) {
		c = getchar();
		if (c != '/')
			break;
		putchar(c);
		do {
			c = getchar();
			putchar(c);
		} while (c != '\n');
	}
	ungetc(c, stdin);
	op = rdin(10);
	p = talloc();
	p->rall = 0;
	p->op = op;
	ty = dope[op] & 0xE;
	if (ty == LTYPE)
		p->lval = rdin(10);
	if (ty != BITYPE && p->op != FLD)
		p->rval = rdin(10);
	if (p->op == TEMP)
		p->stsize = rdin(10);
	if (p->op == LAB)
		p->op = NAME;
	if (p->op == FCON) {
#ifdef OLD_D12K
		d12kout(-p->rval);
#endif
		p->op = NAME;
		p->lval = 0;
	}
	p->type = rdin(8);
	p->rall = NOPREF;
	if (p->op == STASG || p->op == STARG || p->op == STCALL || p->op == USTCALL) {
		p->stsize = (rdin(10) + 7) / 8;
		p->lval = (long)((int)(rdin(10) / 8)) << 16;
		if (getchar() != '\n')
			cerror("illegal \n");
	} else
		nlcheck();
	if (ty != LTYPE)
		p->left = eread();
	if (ty == BITYPE || p->op == FLD)
		p->right = eread();
	return p;
}

long
rdin(base)
	int base;
{
	return rdin2(base, stdin);
}

/* one number from the intermediate file or the data file; the input is f */
long
rdin2(base, f)
	register int base;
	register FILE *f;
{
	register int sign, c;
	long val;

	sign = 1;
	val = 0;
	while ((c = getc(f)) > 0) {
		if (c == '-') {
			if (val != 0)
				cerror("illegal -");
			sign = -sign;
			continue;
		}
		if (c == '\t')
			break;
		if (c >= '0') {
			if (c > '9')
				goto bad;
			val *= base;
			if (sign > 0)
				val += c - '0';
			else
				val -= c - '0';
			continue;
		}
	bad:
		fflush(stderr);
		cerror("illegal character `%x' in input file", c);
		break;
	}
	if (c <= 0)
		cerror("unexpected EOF");
	return val;
}
