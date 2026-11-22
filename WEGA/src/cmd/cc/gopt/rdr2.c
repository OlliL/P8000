/*
 * rdr2.c - gopt: the readers of the symbol table file and of the intermediate file (the pcc file "rdr", SCCS 2.4, included
 * in the original by rdr2.c 1.3 03/04/83). Written from /lib/gopt; the relative is ../codgen/rdr3.c (rdr 2.5 08/24/83).
 */
static char sccsid[] = "@[$]rdr2.c\t1.3  03/04/83 16:01:05 Copr. 1981 87wega3.2.";
#include "gopt.h"

static char rdrsccs[] = "@[$]rdr\t2.4  03/04/83 15:31:15 Copr. 1981 87wega3.2.";

extern char *taballoc();
extern int cerror();
extern int dflag;
extern FILE *stfile;
extern FILE *infile;
extern long ifpos;
extern int eflag;
extern NODE *eread();
extern long rdin(), rdin2();
extern NODE *talloc();
extern long ftell();
extern int nglob;
extern long nsymbytes, nasciibytes;
extern int nnumbytes, nmembytes;
extern int nsyms, nnums, nmems;
extern struct sym *symtab;
extern long numtab[];
extern struct { int tag; struct sym *sym; } tagtab[];
extern void readsyms();
extern void readtab();
extern void readnums();
extern void readmems();
extern void sortsyms();
extern void prtags();
extern int tagfind();
extern void mapsym();
extern struct sym *mapn();
extern long stg;
extern long gsymbytes, gasciibytes, symbase;
extern int gnumbytes, gnumcount;

static char *ttype[] = {
	"UNDEF", "FARG", "CHAR", "SHORT", "INT", "LONG", "FLOAT", "DOUBLE", "STRTY", "UNIONTY", "ENUMTY", "MOETY",
	"UCHAR", "USHORT", "UNSIGNED", "ULONG",
};
static char *tmod[] = { "undef", "PTR", "FTN", "ARY" };
static char *tclass[] = {
	"undef", "AUTO", "EXTERN", "STATIC", "REGISTER", "EXTDEF", "LABEL", "undef", "MOS", "PARAM", "STNAME", "MOU", "UNAME",
	"TYPEDEF", "unused", "ENAME", "MOE", "unused", "USTATIC",
};
char ascii[MAXASCII] = "GLOBAL";
int stflag = 2;
long stpos = 0;
int firstsect = 1;

/* the symbol table file: the local section, then the member table of the global one */
int
rdsym()
{
	nglob = 0;
	fseek(stfile, stpos, 0L);
	stflag = getw(stfile);
	if (stflag <= 0)
		return 0;
	fread(&nsymbytes, 4, 1, stfile);
	fread(&nasciibytes, 4, 1, stfile);
	fread(&nnumbytes, 2, 1, stfile);
	fread(&nmembytes, 2, 1, stfile);
	nsyms = nsymbytes / 22;
	if (dflag)
		printf("// readstf: stlen = %ld, ntlen = %ld, mtlen = %d, rtlen=%d\n", nsymbytes, nasciibytes, nnumbytes, nmembytes);
	readsyms(nsyms, symtab + 1, 7);
	nsyms++;
	if (nasciibytes + 7 > MAXASCII)
		cerror("local ascii table overflow\n");
	readtab(ascii + 7, nasciibytes);
	nnums = nnumbytes / 2;
	readnums(nnums);
	if (stflag) {
		nmems = nmembytes / 6;
		readmems(nmems);
	}
	if (dflag)
		prtags();
	sortsyms();
	if (dflag)
		prtags();
	if (dflag) {
		printf("\n Offset to globals = %ld\n\n", stg);
		printf(" Local tables:\n\n");
		printf(" symbol table length = %ld\n", nsymbytes);
		printf(" ascii table length = %ld\n", nasciibytes);
		printf(" member table length = %d\n", nnumbytes);
		printf(" reference table length = %d\n", nmembytes);
	}
	return 1;
}

/* n symbols of 22 bytes into tab; kind is added to the name offsets */
void
readsyms(n, tab, kind)
	int n;
	struct sym *tab;
	int kind;
{
	int k;
	int t;
	int x;
	struct sym *q;

	if (n > 0) {
		taballoc(n * 22, 0);
		if ((x = fread(tab, 22, n, stfile)) != n)
			printf("readst: error in reading symbol table file stfile\n");
		for (k = n, q = tab; k > 0; k--, q = q + 1) {
			tagtab[nglob].tag = (q->sflags >> 4) & 0xFFF;
			tagtab[nglob].sym = q;
			nglob++;
			q->nameoff += kind;
			if (kind == 7)
				q->sflags &= ~4;
			else {
				q->sflags |= 4;
				q->sx14 = 0;
			}
		}
		if (dflag) {
			printf("Symbol Table Entries:\n\n");
			for (k = n; k > 0; k--, tab = tab + 1) {
				printf("index = %d\n", tab);
				printf("name offset = %d", tab->nameoff);
				printf(", type = %s", ttype[tab->stype & 0xF]);
				for (t = tab->stype >> 4; t != 0; t >>= 2)
					printf(" %s", tmod[t & 3]);
				printf(", class = %s\n", tclass[tab->sclass]);
				printf("        offset = %ld, reg = %d, size = %ld, tag = %d\n", tab->soffset, tab->sbyte5, tab->ssize, (tab->sflags >> 4) & 0xFFF);
				printf("        block = %d, global = %d,", tab->sx6, (tab->sflags >> 2) & 1);
				if (tab->sclass == 10)
					printf(" members = %d\n", tab->sx12);
				else
					printf(" sttype = %d\n", tab->sx12);
			}
			printf("\n");
		}
	}
}

/* the ascii table */
void
readtab(buf, n)
	char *buf;
	long n;
{
	int x, got;

	if (n > 0) {
		if ((got = fread(buf, 1, (int)n, stfile)) != n)
			printf("readnt: error in reading ascii table from symbol file\n");
		if (dflag) {
			printf("Ascii Table Entries:\n\n");
			for (x = got; x > 0; ) {
				printf("(%d)\t%s\n", got - x, buf);
				while (*buf) {
					buf++;
					x--;
				}
				buf++;
				x--;
			}
			printf("\n");
		}
	}
}

/* the number table of a section, behind the numbers already there */
void
readnums(n)
	int n;
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
	if (fread(buf, 2, n, stfile) != n)
		printf("readmt: error in reading number table from file stfile\n");
	if (dflag) {
		printf("readmt(%d)\n\n", n);
		printf("Member table:\n\n");
	}
	src = buf;
	dst = (int *)&numtab[base];
	for (k = n; k > 0; k--, src++, dst += 2) {
		if (dflag)
			printf("(%d)\ttag = %d\n", n - k, *src);
		*dst = *src;
		dst[1] = -1;
	}
}

/* the member table of the global section: the symbols it points to are read in runs of consecutive ones */
void
readmems(n)
	int n;
{
	static struct stent { long off; int x; } mem[550];
	struct stent *cur, *first;
	struct sym *dp;
	long base;
	int k, rem, dest, cnt, w;

top:
	if (n <= 0)
		return;
	fread(mem, 6, n, stfile);
	stpos = ftell(stfile);
	cur = mem;
	if (dflag) {
		printf("\nReference Table:\n\n");
		for (k = n; k > 0; k--, cur++)
			printf(" i = %d, offset = %ld\n", k, cur->off);
		printf("\n");
		cur = mem;
	}
	if (firstsect) {
		symbase = stg + 14;
		fseek(stfile, stg + 2, 0);
		fread(&gsymbytes, 4, 1, stfile);
		fread(&gasciibytes, 4, 1, stfile);
		fread(&gnumbytes, 2, 1, stfile);
		fseek(stfile, symbase + gsymbytes, 0);
		if (dflag) {
			printf("\nGlobal table sizes:\n\n");
			printf(" symbol table length = %ld\n", gsymbytes);
			printf(" ascii table length = %ld\n", gasciibytes);
			printf(" member table length = %d\n\n", gnumbytes);
		}
		if (gasciibytes + nasciibytes + 7 > MAXASCII)
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
		if (dflag > 1)
			printf("// readrt: fseek(%ld)\n", symbase + base);
		fseek(stfile, symbase + base, 0);
		if (dflag > 1) {
			fread(&w, 2, 1, stfile);
			printf("// readrt: word = %x\n", w);
			fseek(stfile, symbase + base, 0);
		}
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

void
prtags()
{
	int i, n;

	for (i = n = nglob; i > 0; i--)
		printf("(%d)\ttag %d\tsindex %d\n", n - i, tagtab[n - i].tag, tagtab[n - i].sym);
}

/* the index of the tag in tagtab, or -1 */
int
tagfind(tag)
	int tag;
{
	int lo, hi, mid;

	if (dflag)
		printf("search(%d), tagcnt = %d\n", tag, nglob);
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
		if (dflag)
			printf("search: i=%d, j=%d, k=%d\n", lo, hi, mid);
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
	NODE *p;
{
	register int i;

	i = tagfind(p->tn.rval);
	if (i < 0)
		cerror("mapn: tag %d not found", p->tn.rval);
	p->in.sname = tagtab[i].sym;
}

/* a leaf of the tree from the file */
void
mapnm(p)
	NODE *p;
{
	int n, t, ot;
	NODE *m;
	long ofs;

	if (eflag > 1)
		printf("mapnm, p->op=%d, p->rval=%d\n", p->in.op, p->tn.rval);
	if (p->in.op == NAME && p->tn.rval >= 0)
		mapsym(p);
	else if (p->in.op == ICON)
		p->in.sname = 0;
}

void
mkhash()
{
	(symtab = (struct sym *)taballoc(22, 0))->nameoff = 0;
	symtab->sflags |= 4;
	symtab->sx14 = 0;
}

/* the end of a line of the intermediate file */
void
nlcheck()
{
	if (getc(infile) != '\n')
		cerror("intermediate file format error");
}

/* a tree of the intermediate file */
int erflag = 0;

NODE *
eread()
{
	register NODE *p;
	register int op;
	register int spare1, spare2, spare3;	/* unused */
	int x;
	char c;

	for (; (c = getc(infile)) == '/'; ) {
		putchar(c);
		while (putchar(getc(infile)) != '\n')
			;
	}
	ungetc(c, infile);
	op = rdin(10);
	p = talloc();
	p->in.rall = 0;
	p->in.op = op;
	op = optype(op);
	if (op == 0x73)
		erflag = 1;
	if (op == LTYPE)
		p->tn.lval = rdin(10);
	if (op != BITYPE && p->in.op != FLD) {
		p->tn.rval = rdin(10);
		if (eflag > 2)
			printf("eread: reading rval = %d\n", p->tn.rval);
	}
	if (p->in.op == TEMP)
		p->tn.temp = rdin(10);
	if (p->in.op == LAB)
		p->in.op = NAME;
	p->in.type = rdin(8);
	if (p->in.op == STASG || p->in.op == STARG || p->in.op == STCALL || p->in.op == USTCALL) {
		p->stn.stsize = (rdin(10) + 7) / 8;
		p->stn.stalign = rdin(10) / 8;
		if (getc(infile) != '\n')
			cerror("illegal \n");
	} else
		nlcheck();
	if (erflag) {
		erflag = 0;
		ifpos = ftell(infile);
	}
	if (op != LTYPE)
		p->in.left = eread();
	if (op == BITYPE || p->in.op == FLD)
		p->in.right = eread();
	return p;
}

long
rdin(base)
	int base;
{
	return rdin2(base, infile);
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
		if (c >= '0' && c <= '9') {
			val *= base;
			if (sign > 0)
				val += c - '0';
			else
				val -= c - '0';
			continue;
		}
		fflush(stderr);
		cerror("illegal character `%x' in input file", c);
		break;
	}
	if (c <= 0)
		cerror("unexpected EOF");
	return val;
}
