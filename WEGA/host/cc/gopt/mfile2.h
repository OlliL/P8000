/*
 * mfile2.h - the header of the global optimizer /lib/gopt (the second part of the C compiler beside codgen:
 * it reads the intermediate language of cparse, reader2.c ... reg2.c). Reconstructed from the binary; the name is that of the
 * original (its SCCS line: mfile2.h 1.7 03/04/83). Layout of NODE (22 bytes, pointers 32 bits) and the tables are measured.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include "../z8000/macdefs"
#include "../mip/manifest"
#include "../mip/rdr.h"

union ndu {
	struct {
		int	op;
		int	rall;
		unsigned type;
		char	su;
		char	fsu;
		struct sym *sname;	/* 8 */
		int	pad;		/* 0xC */
		NODE	*left;		/* 0xE */
		NODE	*right;		/* 0x12 */
	}in;
	struct {
		int	op;
		int	rall;
		unsigned type;
		int	w6;		/* 6: for a TEMP */
		int	pad1;		/* 8 */
		int	pad2;		/* 0xA */
		int	temp;		/* 0xC: the number of a TEMP */
		long	lval;		/* 0xE */
		int	rval;		/* 0x12 */
		int	pad4;		/* 0x14 */
	}tn;
	struct {
		int	op;
		int	rall;
		unsigned type;
		char	su;
		char	fsu;
		int	stsize;		/* 8 */
		int	stalign;	/* 0xA */
	}stn;
};

struct qa { unsigned w0; char b2; unsigned fl:1; int w4; };	/* an operand of a quad: 6 bytes (names guessed) */

struct pos { char *name; int line; };

/* the quad operators: the names of qtab (debug2.c) with Q, as the strings of the original say QSTORE and QIDR */
#define QADD	1
#define QADDFLD	2
#define QADDR	3
#define QAND	4
#define QANDL	5
#define QBLOCK	6
#define QCALL	7
#define QCASE	8
#define QCTABLE	9
#define QCASEL	10
#define QCEND	11
#define QCOMPL	12
#define QCONV	13
#define QDIV	14
#define QEBLOCK	15
#define QELOOP	16
#define QEND	17
#define QENTRY	18
#define QEQ	19
#define QFIELD	20
#define QFREE	21
#define QGE	22
#define QGT	23
#define QIDR	24
#define QINDEX	25
#define QJP	26
#define QJPC	27
#define QLABEL	28
#define QLE	29
#define QLOAD	30
#define QLOOP	31
#define QLT	32
#define QMOVE	33
#define QMULT	34
#define QNE	35
#define QNOT	36
#define QOR	37
#define QORL	38
#define QREM	39
#define QREGISTER	40
#define QRESTORE	41
#define QRET	42
#define QSAVE	43
#define QSHIFTL	44
#define QSHIFTR	45
#define QSTART	46
#define QSTORE	47
#define QSUB	48
#define QXOR	49
#define QSPR	50
#define QPARR	51
#define QPARV	52
#define QEPR	53
#define QNOCALLS	54
#define QNOOPT	55
#define QOPT	56
#define QSEARCH	57
#define QDESC	58
#define QBBLOCK	59
#define QCOPY	60

struct quad {
	struct quad *next;	/* 0 */
	unsigned op:8;		/* 4 */
	unsigned f7:1, f6:1, f5:1, f4:1, f3:1, f2:1, f1:1, f0:1;
	struct qa c;		/* 6 */
	struct qa a;		/* 0xC */
	struct qa b;		/* 0x12 */
	struct pos pos;		/* 0x18: the file name and the line of the source */
	struct brec *rec;	/* host: the record of a BBLOCK quad that the original keeps in the bytes of c (at 6) */
};

static char mfile2sccs[] __attribute__((unused)) = "@[$]mfile2.h\t1.7  03/04/83 16:06:50 Copr. 1981 87wega3.2.";

/* host: the structures that the modules of the original declare each for itself (the same memory, some with more fields
   or other names); one definition here. Sizes in the comments are those of the Z8000. */
struct edge { int next; int blk; };				/* the edge lists of the flow graph: 4 bytes */
struct pr { int x; int y; };					/* the same entries, seen by lpinv2.c and reg2.c */
struct bl { struct bl *next; int blk; };			/* a list of blocks: 6 */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt;
	unsigned b15:1, b14:1, b13:1, b12:1, b11:1, b10:1; };	/* a loop: 20 */
struct dnode { struct dnode *next; struct dnode *down; int dblkno; };	/* the dominator tree: 10 */
struct brec { unsigned *f0, *f4, *f8, *fc, *f10, *f14, *f18; int w12; int callfnd; };
	/* the record of a block that the BBLOCK quad points to (28 bytes): the bit vectors of live2.c (lrec), seen by
	   lpinv2.c and reg2.c as hrec with two words at 12 (w12, callfnd) where lrec has fc, which nobody uses */
#define lrec brec
#define hrec brec
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, b12:1, fl:1, b10:1, rest:10; };	/* betatbl: 10 */
int bbit(int j, int n);						/* host: bit n of the flag word of betatbl[j] (15 = f15) */
struct aln { int x0; struct aln *next; struct aln *link; };	/* the alias lists: 10 */
struct rn { struct rn *next; int idx; } __attribute__((packed, aligned(4)));	/* the reach map: 6; the numbers of the
					   definitions follow idx in memory (*++pi), so no padding behind idx (host) */
struct lv { unsigned kind:1, alias:1, var:14; };		/* the live variables: 2 */
struct nd { struct qa c0; unsigned f15:1, f14:15; long v; struct qa c12; struct nd *link; };	/* induction variables: 22 */
struct dl { int x0; struct quad *q; struct nd *l6; struct dl *l10; struct qa e14; int x20;
	unsigned f15:1, f14:1, f13:1, f12:1, f11:1, f10:1, rest:10; struct dl *next; };	/* definitions of the loops: 28 */
struct hent { struct hent *next; int op; int v1; int v2; int val; unsigned cst:1, num:15; struct quad *q; };	/* htab: 18 */
struct vn { struct vn *next; struct qa a; int val; int al; unsigned cst:1, dead:1, rest:14; };	/* value numbers: 16 */
struct gedge { int k; struct gedge *next; };			/* the edges of the interference graph (reg2.c: edge): 6 */
struct gn { int row; struct gn *link; struct gedge *edges; int cnt; int x12; int x14; int cost; unsigned f15:1; };	/* 20 */
struct gnn { int x0; struct quad *q; int blk; int kind; };	/* the table gnodes: 10 */

/* host: the tables that the modules declare with an anonymous structure (the same layout in every module) */
struct ctmp { int a, b; };
struct tagent { int tag; struct sym *sym; };
struct a71e { struct quad *q; struct sym *s; };
struct fconst { int rv; int d[4]; };	/* d: the four words of the double, big endian (the Z8000 format); fcget(), fcput() */
double fcget(int i);
void fcput(int i, double x);
int rd16(FILE *f);
long rd32(FILE *f);
int rdsyms22(struct sym *tab, int n, FILE *f);
FILE *fopenrw(char *name);
struct nument { int tag; int x; };	/* numtab: the tags of the members (4 bytes) */
#define L32(x)	((long)(int32_t)(uint32_t)(x))	/* host: a long of the Z8000 (32 bits, wraps) */
#define zl(v)	((long)(v) == -2147483648L ? 0L : (long)(v))	/* host: %ld of the libc of gopt prints the
							   most negative long as 0 (measured; doprnt/0_3) */
extern unsigned zseg0[];
void brecinit(struct brec *r);
#define BLKUNSET	(-32768)	/* host: an argument the original does not pass */
#include "goptproto.h"

/* what more than one module uses, in the same form (a module declares what only it uses, or what it sees in a
   form of its own) */
extern int nbeta;
extern struct quad *phquad;
extern int ivagain;
extern struct dl *lpdefs;
extern int nqmoved;
extern int nstemps;
extern int nltemps;
extern int copyins;
extern struct dl *flpdefs;
extern int nivelim;
extern int nbeta0;
extern int aflag;
extern int *altab;
extern char *arhigh[];
extern char ascii[];
extern int blkno;
extern struct quad **blks;
extern long constbl[];
extern int ctmpoff;
extern char ctneg[];
extern char ctpos[];
extern struct pos curpos;
extern NODE *curproc;
extern struct quad **defs;
extern FILE *dfile;
extern int dflag;
extern int dfwords;
extern struct dnode *domroot;
extern int eflag;
extern struct fconst fconsttbl[];
extern char ftitle[];
extern int hdrblk;
extern int *idfn;
extern long ifpos;
extern FILE *infile;
extern struct quad *lastq;
extern int lineno;
extern struct loop *loops;
extern int lvwords;
extern int maxblk;
extern int nadr;
extern int nal;
extern int nconst;
extern int nctmp;
extern int nctmpuses;
extern int ndefs;
extern int nfconst;
extern NODE node[];
extern int nptr;
extern int ntl;
extern struct nument numtab[];
extern int qflag;
extern struct quad *qfree;
extern struct quad *qhead;
extern struct quad *qtail;
extern int olevel;
extern int zflag;
extern int nlabrm;
extern int njmprm;
extern int ndeadrm;
extern int hascall;
extern int bdebug;
extern int ddebug;
extern int ldebug;
extern int pdebug;
extern int fdebug;
extern int rdebug;
extern int sdebug;
extern int maxtmp;
extern int startblk;
extern FILE *stfile;
extern long stg;
extern struct sym *symtab;
extern int tflag;
extern int tl[];
extern int nlabsave;
extern int nparm;
extern int nexttmp;
extern int vafc;
extern long frame0;
extern int *vertex;
