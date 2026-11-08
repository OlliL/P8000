/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "cg.h"
#include "cg2.h"


extern char ascii[];
extern FILE *dfile;
extern int dope[];
extern int eflag;
extern int firstsect;
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern long gasciibytes;
extern int gnumbytes;
extern int gnumcount;
extern long gsymbytes;
extern long nasciibytes;
extern int nglob;
extern int nmembytes;
extern int nmems;
extern int nnumbytes;
extern int nnums;
extern NODE node[];
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern struct rv regvar[];
extern int rflag;
extern FILE *stfile;
extern int stflag;
extern long stg;
extern long stpos;
extern long symbase;
extern struct sym *symtab;
extern struct sym *symtab0;
extern struct { int tag; struct sym *sym; } tagtab[];
extern char *tnames[];
static char sccsid[] __attribute__((unused)) = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
static char rdrsccs[] __attribute__((unused)) = "@[$]rdr\t2.5  08/24/83 16:48:00 Copr. 1981 87wega3.2.";
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
#endif


/* copy a section of the data file to the output */
int
rddata(void)
{
	long n;
	int got, chunk;
	char buf[512];

	n = rdin2(10, dfile);
	if (getc(dfile) != '\n')
		cerror("data file format error\n");
#ifdef OLD_D12K
	if (1) return d12kcopy(n);
#endif
	for (; n > 0; ) {
		if (n > 0x200) {
			chunk = 0x200;
			n = n - 0x200;
		} else {
			chunk = (int)n;
			n = 0;
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

/*
 * The symbol table file is written by cparse of the Z8000: big endian, 16 bit words, 32 bit longs, records of 22 bytes. It is read
 * field by field (the host struct sym has another size and byte order).
 */
int
rd16(FILE *f)
{
	int h = getc(f);

	return (short)(h << 8 | getc(f));
}

long
rd32(FILE *f)
{
	long h = rd16(f);

	return (int32_t)((uint32_t)(h & 0xFFFF) << 16 | (rd16(f) & 0xFFFF));
}

/* getw of the original libc: two bytes, the first one the high one; -1 at the end of the file */
static int
getw16(FILE *f)
{
	int h = getc(f);

	if (feof(f))
		return -1;
	return (short)(h << 8 | getc(f));
}

/* the symbol table file: the local section, then the member table of the global one */
int
rdsym(void)
{
	nglob = 0;
	fseek(stfile, stpos, 0L);
	stflag = getw16(stfile);
	if (stflag <= 0)
		return 0;
	nsymbytes = rd32(stfile);
	nasciibytes = rd32(stfile);
	nnumbytes = rd16(stfile);
	nmembytes = rd16(stfile);
	nsyms = nsymbytes / 22;
	readsyms(nsyms, symtab + 3, 0x15);	/* behind the three symbols at the start (0x42 bytes) */
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
d12kcopy(long n)
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
d12kout(int lab)
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

/* a record of the symbol table file (22 bytes, big endian) into a symbol */
static void
symdecode(struct sym *q, const unsigned char *r)
{
#define W16(o)	((short)(r[o] << 8 | r[(o) + 1]))
#define W32(o)	((int32_t)((uint32_t)r[o] << 24 | (uint32_t)r[(o) + 1] << 16 | (uint32_t)r[(o) + 2] << 8 | r[(o) + 3]))
	q->nameoff = W16(0);
	q->stype = W16(2);
	q->sclass = (char)r[4];
	q->sbyte5 = (char)r[5];
	q->sx6 = W16(6);
	q->soffset = W32(8);
	q->ssize = W32(12);
	q->sflags = W16(16);
	q->sx12 = W16(18);
	q->sx14 = W16(20);
#undef W16
#undef W32
}

/* n symbols of 22 bytes into tab; kind is added to the name offsets */
void
readsyms(int n, struct sym *tab, int kind)
{
	int k, x;
	struct sym *q;

	if (n > 0) {
		alloc(n * sizeof (struct sym));
		for (x = 0; x < n; x++) {
			unsigned char rec[22];

			if (fread(rec, 22, 1, stfile) != 1)
				break;
			symdecode(&tab[x], rec);
		}
		if (x != n)
			printf("readst: error in reading symbol table file stfile\n");
		for (k = n, q = tab; k > 0; k--, q = q + 1) {
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
}

/* the ascii table */
void
readtab(char *buf, long n)
{
	int got;

	if (n > 0)
		if ((got = fread(buf, 1, (int)n, stfile)) != n)
			printf("readnt: error in reading ascii table from symbol file\n");
}

/* the number table of a section, behind the numbers already there */
void
readnums(int n)
{
	int buf[500];
	int k, base;
	int *dst, *src;

	if (n <= 0)
		return;
	if (n > 500)
		cerror(" member table overflow");
	if (!firstsect)
		base = nsyms;
	else
		base = 0;
	for (k = 0; k < n; k++) {
		int h = getc(stfile);

		buf[k] = (short)(h << 8 | getc(stfile));
	}
	if (feof(stfile))
		printf("readmt: error in reading number table from file stfile\n");
	src = buf;
	dst = numtab + (base << 1);	/* entries of 4 bytes: two words */
	for (k = n; k > 0; k--, src++, dst += 2) {
		*dst = *src;
		dst[1] = -1;
	}
}

/* the member table of the global section: the symbols it points to are read in runs of consecutive ones */
void
readmems(int n)
{
	static struct stent { long off; int x; } mem[550];
	struct stent *cur, *first;
	struct sym *dp;
	long base;
	int k, rem, dest, cnt;

	if (n <= 0)
		return;
	for (k = 0; k < n; k++) {
		mem[k].off = rd32(stfile);
		mem[k].x = rd16(stfile);
	}
	stpos = ftell(stfile);
	cur = mem;
	if (firstsect) {
		symbase = stg + 14;
		fseek(stfile, stg + 2, 0);
		gsymbytes = rd32(stfile);
		gasciibytes = rd32(stfile);
		gnumbytes = rd16(stfile);
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
		dp = symtab + dest;
		fseek(stfile, symbase + base, 0);
		readsyms(cnt, dp, MAXASCII - gasciibytes);
		dest += cnt;
		rem -= cnt;
	}
	nsyms = dest;
}

/* sort the tags */
void
sortsyms(void)
{
	int i, j;
	int t;
	struct sym *s;

	for (i = nglob - 1; i >= 0; i--)
		for (j = i - 1; j >= 0; j--)
			if (tagtab[i].tag < tagtab[j].tag) {
				t = tagtab[j].tag;
				s = tagtab[j].sym;
				tagtab[j].tag = tagtab[i].tag;
				tagtab[j].sym = tagtab[i].sym;
				tagtab[i].tag = t;
				tagtab[i].sym = s;
			}
}

/* the index of the tag in tagtab, or -1 */
int
tagfind(int tag)
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
mapn(int tag)
{
	int i;

	i = tagfind(tag);
	if (i < 0)
		cerror("rvmapn: tag %d not found", tag);
	return tagtab[i].sym;
}

/* the symbol of a node from its tag */
void
mapsym(NODE *p)
{
	register int i;

	if (p->tn.rval) {
		i = tagfind(p->tn.rval);
		if (i < 0)
			cerror("mapn: tag %d not found", p->tn.rval);
		p->in.sname = tagtab[i].sym;
		p->tn.rval = 0;
	}
}

/* a leaf of the tree from the file: symbols, register variables and fields */
void
mapnm(NODE *p)
{
	int n, t, ot;
	NODE *m;
	long ofs;

	if (p == 0)		/* gswitch passes the word lval of a REG node as a pointer: the original reads address 0, which is no NAME */
		return;
	if (eflag > 1)
		printf("mapnm, p->op=%d, p->rval=%d\n", p->in.op, p->tn.rval);
	if (p->in.op == NAME && p->tn.rval >= 0) {
		mapsym(p);
		if (p->in.sname->sclass == 4) {
			if (rflag) {
				n = p->in.sname->sbyte5;
				t = p->in.sname->stype;
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
			if (rflag && regvar[n].sym != p->in.sname)
				p->in.sname->sclass = regvar[n].rcls;
			else {
				n = p->in.sname->sbyte5;
				if (!rflag)
					n = getreg(rclass(p->in.type, n), n, 0x1000, p->in.sname);
				p->in.op = REG;
				p->tn.rval = n;
				p->tn.lval = 0;
			}
		}
		if (eflag > 1)
			printf("mapnm, p->sname = %d, p->global=%d\n", (int)(long)p->in.sname, p->in.sname->sflags >> 2 & 1);
		return;
	}
	if (p->in.op == ICON) {
		if (p->in.sname == fpsym1 || p->in.sname == fpsym2)
			return;
	} else if (p->in.op == TEMP) {
		fixtemp(p);
		return;
	} else if (p->in.op == FLD) {
		m = p->in.right;
		mapsym(m);
		mapnm(p->in.left);
		ot = p->in.left->in.type;
		fixfld(p);
		ofs = m->in.sname->soffset / 16 * 2;
		p->tn.rval = ((m->in.sname->soffset % 16) << 6) | (m->in.sname->sclass & 0x3F);
		if (p->in.left->in.op == NAME)
			p->in.left->tn.lval += ofs;
		else if (ofs != 0) {
			t = INCREF(m->in.sname->stype);
			p->in.left->in.type = t;
			if ((ot & 0x30) != 0x10 && p->in.left->in.op == STAR) {
				p->in.left->in.op = FREE;
				p->in.left = clocal(makeblock(PLUS, p->in.left->in.left, bcon(ofs << 3), t));
			} else
				p->in.left = clocal(makeblock(PLUS, p->in.left, bcon(ofs << 3), t));
			p->in.left = buildstar(STAR, p->in.left, 0);
		}
		m->in.op = FREE;
		return;
	}
	p->in.sname = 0;
}

/* the two pseudo symbols ~L1 and ~L2 of the frame */
void
mkhash(void)
{
	symtab0 = symtab = (struct sym *)alloc(3 * sizeof (struct sym));	/* 0x42 bytes in the original */
	fpsym1 = symtab + 1;	/* +0x16 */
	fpsym2 = symtab + 2;	/* +0x2C */
	symtab->nameoff = 0;
	symtab[1].nameoff = 5;
	symtab[2].nameoff = 9;
}

/* the end of a line of the intermediate file */
void
nlcheck(void)
{
	if (getchar() != '\n')
		cerror("intermediate file format error");
}

/* a tree of the intermediate file */
int erflag = 0;

NODE *
eread(void)
{
	register NODE *p;
	register int op;
	char c;

	for (; (c = getchar()) == '/'; ) {
		putchar(c);
		while (putchar(getchar()) != '\n')
			;
	}
	ungetc(c, stdin);
	op = rdin(10);
	p = talloc();
	p->in.rall = 0;
	p->in.op = op;
	op = optype(op);
	if (op == 0x73)
		erflag = 1;
	if (op == LTYPE)
		p->tn.lval = rdin(10);
	if (op != BITYPE && p->in.op != FLD)
		p->tn.rval = rdin(10);
	if (p->in.op == TEMP)
		p->stn.stsize = rdin(10);
	if (p->in.op == LAB)
		p->in.op = NAME;
	if (p->in.op == FCON) {
#ifdef OLD_D12K
		d12kout(-p->tn.rval);
#endif
		p->in.op = NAME;
		p->tn.lval = 0;
	}
	p->in.type = rdin(8);
	p->in.rall = NOPREF;
	if (p->in.op == STASG || p->in.op == STARG || p->in.op == STCALL || p->in.op == USTCALL) {
		p->stn.stsize = (rdin(10) + 7) / 8;
		p->stn.stalign = rdin(10) / 8;
		if (getchar() != '\n')
			cerror("illegal \n");
	} else
		nlcheck();
	if (op != LTYPE)
		p->in.left = eread();
	if (op == BITYPE || p->in.op == FLD)
		p->in.right = eread();
	return p;
}

long
rdin(int base)
{
	return rdin2(base, stdin);
}

/* one number from the intermediate file or the data file; the input is f */
long
rdin2(int base, FILE *f)
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
