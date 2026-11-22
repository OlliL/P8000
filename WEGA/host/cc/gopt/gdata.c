/*
gdata.c - gopt (host): the global variables that no module defines (src/cmd/cc/gopt/gdata.c has them as bytes in the order of the
data of /lib/gopt); here with their types (the extern declarations of the modules) and the element counts of the Z8000 sizes.
*/
#include "mfile2.h"
int v04;
long v06;
struct quad *qhead;
int v0e;
char ctneg[1002];
NODE *curproc;
int dope[129];
int v00;
int ntmp;
struct quad *qfree;
int nconst;
FILE *infile;
char ctpos[1002];
char *opst[129];
int vafc;
int vafe;
struct pos curpos;
NODE *saved[20];
long ifpos;
struct quad *lastq;
int s0b5e;
int nfconst;
int lineno;
struct quad *qtail;
char *arhigh[11];
long vb94;
FILE *dfile;
struct bt *betatbl;
long constbl[512];
struct ctmp ctmp[100];
struct fconst fconsttbl[200];
int tl[100];
struct sym *symtab;
long nasciibytes;
long symbase;
int nmems;
int nnums;
int nsyms;
int gnumcount;
FILE *stfile;
struct nument numtab[500];
int gnumbytes;
struct tagent tagtab[550];
long gasciibytes;
long stg;
int nglob;
long gsymbytes;
int nmembytes;
long nsymbytes;
int nnumbytes;
NODE *lastfree;
NODE node[350];
struct quad **blks;
struct edge *edges;
int *modval;
struct aln *alcur;
int matw;
int affdim;
int nlist;
struct aln *alfree;
int ndist;
struct quad *pcur;
int nadr;
struct qa opnd70c8;
int nptr;
struct aln *allist;
int *affect;
int op70d8;
int *altab;
int nal;
struct aln *alast;
int blkno;
int s70e6;
int nedge;
int maxblk;
int hdrblk;
struct loop *loops;
int *ancest;
int *label;
int *dfn;
int nbent;
struct edge *bent;
struct dnode *domroot;
int nbl2;
int ndfs;
int *parent;
int *stk;
struct bl *looplist;
int tailblk;
int nbl;
struct bl *freebl;
int *idfn;
int *vertex;
int nloops;
int *child;
struct dnode **darr;
int *size;
struct bl *pheaderlist;
int *dom;
int *bucket;
int dfwords;
struct lv *lvtab;
int nlv;
int lvidx;
int ndefs;
char *livevecs;
struct quad **defs;
int lvwords;
unsigned *tmpv;
int s715e;
struct rn *reachmap;
struct hent *htab[32];
int s71e4;
int s71e6;
int s71e8;
int s71ea;
struct a71e a71ec[500];
struct aln *usetbl[1000];
struct aln *killtbl[1000];
char *a0cc;
unsigned a0d0;
int a0d2;
int a0d4;
struct quad *a0d6;
int a0da;
struct dl *a0e0;
int a0e4;
int a0e6;
unsigned *a0e8;
int a0ec;
int a0ee;
int a0f0;
int a0f2;
struct dnode *a0f4;
struct dl *a0f8;
int a0fc;
int a0fe;
struct dl *a100;
struct nd *a104;
int a108;
unsigned *a10a;
int a10e;
unsigned *a110;
int a114;
int a116;
int a118;
struct gnn *a11a;
struct gn *a11e;

/* host: the double of the four words of fconsttbl[i] (big endian, IEEE 754 as the Z8070) and back */
double fcget(int i)
{
	uint64_t u = 0;
	double x;
	int k;

	for (k = 0; k < 4; k++)
		u = u << 16 | (fconsttbl[i].d[k] & 0xffff);
	memcpy(&x, &u, sizeof x);
	return x;
}

void fcput(int i, double x)
{
	uint64_t u;
	int k;

	memcpy(&u, &x, sizeof u);
	for (k = 3; k >= 0; k--, u >>= 16)
		fconsttbl[i].d[k] = (int)(u & 0xffff);
}

/* host: bit n of the flag word of betatbl[j] (the original reads the word at 8 and shifts) */
int bbit(int j, int n)
{
	struct bt *t = &betatbl[j];

	switch (n) {
	case 15: return t->f15;
	case 14: return t->f14;
	case 13: return t->bopt;
	case 12: return t->b12;
	case 11: return t->fl;
	case 10: return t->b10;
	}
	return (t->rest >> n) & 1;
}

/* host: the words of the symbol table file, big endian (the original reads them with fread and getw) */
int rd16(FILE *f)
{
	int h = getc(f), l = getc(f);

	return (int16_t)((h & 0xff) << 8 | (l & 0xff));
}

long rd32(FILE *f)
{
	long h = rd16(f) & 0xffff;

	return (int32_t)(h << 16 | (rd16(f) & 0xffff));
}

/* host: n symbol records of 22 bytes; the number of the records read */
int rdsyms22(struct sym *tab, int n, FILE *f)
{
	int k;

	for (k = 0; k < n; k++, tab++) {
		tab->nameoff = rd16(f);
		tab->stype = (unsigned)rd16(f) & 0xffff;
		tab->sclass = getc(f);
		tab->sbyte5 = getc(f);
		tab->sx6 = rd16(f);
		tab->soffset = rd32(f);
		tab->ssize = rd32(f);
		tab->sflags = rd16(f);
		tab->sx12 = rd16(f);
		tab->sx14 = rd16(f);
		if (feof(f))
			break;
	}
	return k;
}

/* host: "rw" of the original libc is "a+" there: read and write, at the end, the writes where the position is */
FILE *fopenrw(char *name)
{
	FILE *f = fopen(name, "r+");

	if (f == 0)
		f = fopen(name, "w+");
	if (f != 0)
		fseek(f, 0L, 2);
	return f;
}

/* host: segment 0 of the original, where a vector pointer of a block record that is not set (0) leads: z8k-emu gives
   a segment of zeros that the run reads and writes (a preheader of lpinv2.c has f0 and f4 only; buildgraph() reads f18) */
unsigned zseg0[32768];

void brecinit(struct brec *r)
{
	r->f0 = r->f4 = r->f8 = r->fc = r->f10 = r->f14 = r->f18 = zseg0;
}
