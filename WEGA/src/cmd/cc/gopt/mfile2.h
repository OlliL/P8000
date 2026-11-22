/*
 * mfile2.h - the header of the global optimizer /lib/gopt (the second part of the C compiler beside codgen:
 * it reads the intermediate language of cparse, reader2.c ... reg2.c). Reconstructed from the binary; the name is that of the
 * original (its SCCS line: mfile2.h 1.7 03/04/83). Layout of NODE (22 bytes, pointers 32 bits) and the tables are measured.
 */
#include "../z8000/macdefs"
#include "../mip/manifest"
#include "../mip/rdr.h"

/* TREESZ (manifest): 350 nodes in the pool, node[0] .. node[TREESZ - 1] */

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
};

static char mfile2sccs[] = "@[$]mfile2.h\t1.7  03/04/83 16:06:50 Copr. 1981 87wega3.2.";

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
extern struct dl *alloclpdef();
extern int *altab;
extern char *arhigh[];
extern char ascii[];
extern int blkno;
extern void clearflags();
extern void clrvec();
extern long constbl[];
extern int ctmpoff;
extern char ctneg[];
extern char ctpos[];
extern struct pos curpos;
extern NODE *curproc;
extern struct quad **defs;
extern void deledge();
extern struct quad *delquad();
extern void denorm();
extern FILE *dfile;
extern int dflag;
extern int dfwords;
extern struct dnode *domroot;
extern void ebbopt();
extern int eflag;
extern NODE *eread();
extern void freelist();
extern long ftell();
extern char ftitle[];
extern void genquads();
extern int hdrblk;
extern int *idfn;
extern long ifpos;
extern FILE *infile;
extern struct quad *insertquad();
extern struct quad *lastq;
extern int lineno;
extern void livevars();
extern struct loop *loops;
extern int lvwords;
extern struct sym *mapn();
extern int maxblk;
extern int nadr;
extern int nal;
extern int nconst;
extern int nctmp;
extern int nctmpuses;
extern int ndefs;
extern int nfconst;
extern NODE node[];
extern void normop();
extern int nptr;
extern int ntl;
extern void orvec();
extern void pr();
extern void prgraph();
extern void propnd();
extern void prqall();
extern void prquad();
extern void prquads();
extern void prtags();
extern void prvec();
extern int qflag;
extern struct quad *qmove();
extern long rdin();
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
extern void setbit();
extern struct quad *skiphdrs();
extern int *srchdef();
extern FILE *stfile;
extern long stg;
extern struct sym *symtab;
extern char *taballoc();
extern void tabfree();
extern NODE *talloc();
extern NODE *tcopy();
extern int tflag;
extern int tl[];
extern int nlabsave;
extern int nparm;
extern int nexttmp;
extern int vafc;
extern long frame0;
extern int *vertex;
