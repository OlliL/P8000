/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 *
 * Reconstructed from /lib/codgen: rddata() 0x19ac, rdsym() 0x1a86, readsyms() 0x1b64, readtab() 0x1c30, readnums() 0x1c6c,
 * readmems() 0x1d0e, sortsyms() 0x1ec2, bsearch() 0x1f82, mapn() 0x2038, mapsym() 0x206a, mapnm() 0x20c0, nlcheck() 0x244a,
 * eread() 0x2480, rdin() 0x269a, rdin2() 0x26ae, mkhash() 0x2406. Names are guesses.
 * The symbol table file (base.S) is described in ../KNOWLEDGE.md; the symbols of all its sections are kept in one table
 * and found by the tag in the node (rval of a NAME) through the sorted list tagtab[].
 */
#include "cg.h"

#define MAXASCII	0x1194
#define INCREF(t)	((((t) & ~0xF) << 2) | 0x10 | ((t) & 0xF))

extern char *alloc();
extern struct node *talloc();
extern struct node *makeblock(), *clocal(), *buildstar(), *bcon();
extern int rclass(), getreg(), rtmapfix();
extern void fixtemp(), fixfld();
void sortsyms();

FILE *stfile;			/* 0x8e14: the symbol table file */
FILE *dfile;			/* 0x69dc: the data file */
long stg;			/* 0x8f94: the offset of the global section in the symbol table file */
long stpos;			/* 0x17c6 */
long symbase;			/* 0x8fde: the start of the symbols of the global section */
int stflag = 2;			/* 0x17c4 */
int firstsect = 1;		/* 0x17ca */
int nglob;			/* 0xa0d4: the entries of tagtab */
long nsymbytes;			/* 0xa0dc */
long gsymbytes, gasciibytes;	/* 0xa0d6, 0xa0ce: the global section */
int gnumbytes;			/* 0x9834 */
int gnumcount;			/* 0x9060: only written */
long nasciibytes;		/* 0x8fd4 */
int nnumbytes;			/* 0xa0e0 */
int nmembytes;			/* 0xa0da */
int nsyms;			/* 0x8fe6: the symbols read so far */
int nnums;			/* 0x8fe4 */
int nmems;			/* 0x8fe2 */
struct sym *symtab;		/* 0x8fd2 */
struct sym *symtab0;		/* 0x8fd8 */
struct sym *fpsym1, *fpsym2;	/* 0x8fda, 0x8fdc */
char ascii[MAXASCII] = "NULL\0~L1\0~L2";		/* 0x630: the ascii table; "NULL", "~L1", "~L2" come first */
int numtab[2000];		/* 0x9062: the number table, a word and a -1 per entry */
struct { int tag; struct sym *sym; } tagtab[500];	/* 0x9836 */
extern int rflag, eflag;		/* 0x42, 0x44 */
struct rv { struct sym *sym; int rtag; int rreg; int rtype; int rcls; };
extern struct rv regvar[];
extern void fixmap();	/* 0x8fe8: the register variables */
int ifline;			/* 0x8fc8 */

/* one number from the intermediate file or the data file; the input is f */
long rdin2(register int base, register FILE *f)
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

long rdin(int base)
{
	return rdin2(base, stdin);
}

/* the end of a line of the intermediate file */
void nlcheck(void)
{
	if (getchar() != '\n')
		cerror("intermediate file format error");
}

/* the two pseudo symbols ~L1 and ~L2 of the frame */
void mkhash(void)
{
	symtab = symtab0 = (struct sym *)alloc(3 * sizeof(struct sym));
	fpsym1 = symtab + 1;
	fpsym2 = symtab + 2;
	symtab->nameoff = 0;
	symtab[1].nameoff = 5;
	symtab[2].nameoff = 9;
}

/* copy a section of the data file to the output */
int rddata(void)
{
	static char buf[512];
	long n;
	int chunk, got;

	n = rdin2(10, dfile);
	if (getc(dfile) != '\n')
		cerror("data file format error\n");
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

/* one symbol record of the symbol table file (SYMREC bytes, big endian), decoded into *s */
#define SYMREC	22
void get_sym(FILE *f, struct sym *s)
{
	unsigned char b[SYMREC];
	int k;

	for (k = 0; k < SYMREC; k++)
		b[k] = (unsigned char)getc(f);
	s->nameoff = S16(b[0] << 8 | b[1]);
	s->stype = S16(b[2] << 8 | b[3]);
	s->sclass = (char)b[4];
	s->sbyte5 = (char)b[5];
	s->sx6 = S16(b[6] << 8 | b[7]);
	s->soffset = (long)(S16(b[8] << 8 | b[9]) * 0x10000L + (b[10] << 8 | b[11]));
	s->ssize = (long)(S16(b[12] << 8 | b[13]) * 0x10000L + (b[14] << 8 | b[15]));
	s->sflags = S16(b[16] << 8 | b[17]);
	s->sx12 = S16(b[18] << 8 | b[19]);
	s->sx14 = S16(b[20] << 8 | b[21]);
}

int rd16be(FILE *f)
{
	int hi, lo;

	hi = getc(f) & 0377;
	lo = getc(f) & 0377;
	return S16(hi << 8 | lo);
}

long rd32be(FILE *f)
{
	long hi, lo;

	hi = rd16be(f) & 0xFFFFL;
	lo = rd16be(f) & 0xFFFFL;
	return (long)(S16(hi) * 0x10000L + lo);
}

/* n symbols of the file into tab; kind is added to the name offsets */
void
readsyms(int n, struct sym *tab, int kind)
{
	register struct sym *q;
	register int k;

	if (n <= 0)
		return;
	alloc(n * sizeof(struct sym));
	k = n;
	for (q = tab; k > 0; k--, q++) {
		get_sym(stfile, q);
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
void readtab(char *buf, long n)
{
	if (n > 0)
		if (fread(buf, 1, (int)n, stfile) != n)
			printf("readnt: error in reading ascii table from symbol file\n");
}

/* the number table of a section, behind the numbers already there */
void readnums(int n)
{
	static int buf[500];
	int *src, *dst;
	int base, k;

	if (n <= 0)
		return;
	if (n > 500)
		cerror(" member table overflow");
	base = firstsect ? 0 : nsyms;
	for (k = 0; k < n; k++)
		buf[k] = rd16be(stfile);
	src = buf;
	dst = &numtab[base * 2];
	for (k = n; k > 0; k--) {
		dst[0] = *src++;
		dst[1] = -1;
		dst += 2;
	}
}

/* the member table of the global section: the symbols it points to are read in runs of consecutive ones */
/* one member record of the symbol table file: the offset of a symbol record (4 bytes) and one word (2 bytes) */
struct memrec {
	long off;
	int x;
};

void readmems(int n)
{
	static struct memrec mem[500];
	struct memrec *cur, *first;
	long base;
	struct sym *dp;
	int rem, dest, cnt, k;

	if (n <= 0)
		return;
	for (k = 0; k < n; k++) {
		mem[k].off = rd32be(stfile);
		mem[k].x = rd16be(stfile);
	}
	stpos = ftell(stfile);
	cur = mem;
	if (firstsect) {
		symbase = stg + 14;
		fseek(stfile, stg + 2, 0);
		gsymbytes = rd32be(stfile);
		gasciibytes = rd32be(stfile);
		gnumbytes = rd16be(stfile);
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
			if (cur->off - first->off != SYMREC)
				break;
			first = cur;
			cur++;
			cnt++;
		}
		dp = symtab + dest;
		fseek(stfile, symbase + base, 0);
		readsyms(cnt, dp, MAXASCII - gasciibytes);
		dest += cnt;
		rem -= cnt;
	}
	nsyms = dest;
}

/* the symbol table file: the local section, then the member table of the global one */
int rdsym(void)
{
	nglob = 0;
	fseek(stfile, stpos, 0);
	stflag = rd16be(stfile);
	if (stflag <= 0)
		return 0;
	nsymbytes = rd32be(stfile);
	nasciibytes = rd32be(stfile);
	nnumbytes = rd16be(stfile);
	nmembytes = rd16be(stfile);
	nsyms = nsymbytes / SYMREC;
	readsyms(nsyms, symtab + 3, 0x15);
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

/* sort the tags */
void sortsyms(void)
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
int tagfind(int tag)
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

struct sym * mapn(int tag)
{
	int i;

	i = tagfind(tag);
	if (i < 0)
		cerror("rvmapn: tag %d not found", tag);
	return tagtab[i].sym;
}

/* the symbol of a node from its tag */
void mapsym(struct node * p)
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
void mapnm(struct node * p)
{
	struct sym *s;
	struct node *m, *nb, *lp;
	int n, t, rg, nt;
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
			printf("mapnm, p->sname = %p, p->global=%d\n", (void *)p->sname, p->sname->sflags >> 2 & 1);
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
			lp = p->left->left;
		} else {
			nb = bcon(ofs << 3);
			lp = p->left;
		}
		p->left = clocal(makeblock(PLUS, lp, nb, nt));
		p->left = buildstar(STAR, p->left, 0);
	}
	m->op = FREE;
}

/* a tree of the intermediate file */
struct node *eread(void)
{
	register struct node *p;
	register int op;
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
		p->op = NAME;
		p->lval = 0;
	}
	p->type = rdin(8);
	p->rall = NOPREF;
	if (p->op == STASG || p->op == STARG || p->op == STCALL || p->op == USTCALL) {
		p->stsize = (rdin(10) + 7) / 8;
		p->lval = (long)S16(rdin(10) / 8) << 16;
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
