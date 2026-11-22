/*
 * gopt.h - definitions of the global optimizer /lib/gopt (the second part of the C compiler beside codgen:
 * it reads the intermediate language of cparse, reader2.c ... reg2.c). Reconstructed from the binary; the name of the
 * original is mfile2.h (SCCS 1.7 03/04/83). Layout of NODE (22 bytes, pointers 32 bits) and the tables are measured.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>

/* operators (the dope table in ops_*.c has the flags: arity in bits 1-3) */
#define NAME	2
#define STRING	3
#define ICON	4
#define FCON	5
#define STAR	13		/* unary * */
#define ASG	1+		/* the assignment form of an operator: ASG PLUS is 7 */
#define PLUS	6
#define MINUS	8
#define MUL	11
#define AND	14
#define OR	17
#define ER	19
#define ANDAND	23
#define OROR	24
#define CM	56
#define QUEST	21
#define UMINUS	10
#define UCALL	72
#define FCALL	73
#define UFCALL	75
#define INIT	116
#define COMOP	59
#define ASSIGN	58
#define DIV	60
#define MOD	62
#define LS	64
#define RS	66
#define CALL	70
#define NOT	76
#define COMPL	77
#define UAND	16
#define STREF	69
#define INCR	78
#define DECR	79
#define EQ	80
#define NE	81
#define LE	82
#define LT	83
#define GE	84
#define GT	85
#define ULE	86
#define ULT	87
#define UGE	88
#define UGT	89
#define FEQ	90
#define FNE	91
#define FLE	92
#define FLT	93
#define FGE	94
#define FGT	95
#define RET	114
#define FREE	100		/* sentinel in the table / free node */
#define STASG	101
#define STARG	102
#define STCALL	103
#define USTCALL	105
#define CBRANCH	115
#define FLD	109
#define REG	96
#define OREG	97
#define CCODES	99
#define PROC	118
#define UPROC	119
#define SUBSR	120
#define TEMP	121
#define LAB	127

#define TYFLG	016
#define ASGFLG	01
#define LOGFLG	020
#define DIVFLG	0200
#define CALLFLG	02000
#define MULFLG	04000
#define optype(o)	(dope[o] & TYFLG)
#define asgop(o)	(dope[o] & ASGFLG)
#define logop(o)	(dope[o] & LOGFLG)
#define callop(o)	(dope[o] & CALLFLG)
#define LTYPE	2		/* arity bits of the dope flags */
#define UTYPE	4
#define BITYPE	8

#define NTREE	350		/* nodes of the pool: node[0] .. node[NTREE - 1] */

typedef union ndu NODE;

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

#define MAXASCII	0x1194

struct sym {
	int	nameoff;
	unsigned stype;
	char	sclass;
	char	sbyte5;
	int	sx6;
	long	soffset;
	long	ssize;		/* 0xC: size in bits */
	int	sflags;
	int	sx12;
	int	sx14;
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
	struct brec *rec;	/* host: the record of a BBLOCK quad that the original keeps in the bytes of c (at 6) */
};

struct dopest { int dopeop; char opst[8]; int dopeval; };	/* 12 bytes */
extern struct dopest indope[];
extern int dope[];
extern char *opst[];

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
struct gnn { int x0; struct quad *q; int blk; int kind; };	/* the table a11a: 10 */

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
