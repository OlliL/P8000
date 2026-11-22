/*
 * gopt.h - definitions of the global optimizer /lib/gopt (the second part of the C compiler beside codgen:
 * it reads the intermediate language of cparse, reader2.c ... reg2.c). Reconstructed from the binary; the name of the
 * original is mfile2.h (SCCS 1.7 03/04/83). Layout of NODE (22 bytes, pointers 32 bits) and the tables are measured.
 */
#include "manifest"

/* operators (the dope table in ops_*.c has the flags: arity in bits 1-3) */
#define STAR	13		/* unary * */
#define UMINUS	10
#define UCALL	72
#define FCALL	73
#define UFCALL	75
#define UAND	16


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
};

struct dopest { int dopeop; char opst[8]; int dopeval; };	/* 12 bytes */
extern struct dopest indope[];
extern int dope[];
extern char *opst[];

static char mfile2sccs[] = "@[$]mfile2.h\t1.7  03/04/83 16:06:50 Copr. 1981 87wega3.2.";
