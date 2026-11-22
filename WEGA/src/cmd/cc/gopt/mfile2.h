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
extern int a0d4;
extern struct quad *a0d6;
extern int a0da;
extern struct dl *a0e0;
extern int a0e6;
extern int a0ec;
extern int a0ee;
extern int a0f0;
extern struct dl *a0f8;
extern int a0fe;
extern int a108;
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
extern int s0b5e;
extern int s4d72;
extern int s4d74;
extern int s4d76;
extern int s4d78;
extern int s4d7a;
extern int s4d80;
extern int s4d82;
extern int s4d84;
extern int s4d86;
extern int s4d88;
extern int s4d90;
extern int s4d92;
extern int s4d94;
extern int s70e6;
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
extern int v00;
extern int v04;
extern int v0e;
extern int vafc;
extern long vb94;
extern int *vertex;
