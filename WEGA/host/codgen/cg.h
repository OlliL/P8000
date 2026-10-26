/*
 * cg.h - common definitions of the rebuilt code generator (codgen / scodgen), the second pass of the
 * WEGA C compiler. Constants and layouts are read from the original binary; names follow the strings
 * in it (SAREG, TBYTE, RLEFT ...). See ../KNOWLEDGE.md, "codgen / scodgen".
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define TEMP	121
#define LAB	127

#define LTYPE	2		/* arity bits of the dope flags */
#define UTYPE	4
#define BITYPE	8

/* shapes of the table entries (octal as in the PCC) */
#define SANY	0000001
#define SAREG	0000002
#define STAREG	0000004
#define SBREG	0000010
#define STBREG	0000020
#define SCC	0000040
#define SNAME	0000100
#define SCON	0000200
#define SFLD	0000400
#define SOREG	0001000
#define STARNM	0002000
#define STARREG	0004000
#define INTEMP	0010000
#define FORARG	0020000
#define SWADD	0040000
#define SPECIAL	0100000
#define SZERO	SPECIAL

/* types of the table entries */
#define TCHAR	0000001
#define TSHORT	0000002
#define TINT	0000004
#define TLONG	0000010
#define TFLOAT	0000020
#define TDOUBLE	0000040
#define TPOINT	0000100
#define TUCHAR	0000200
#define TUSHORT	0000400
#define TUNSIGNED 0001000
#define TULONG	0002000
#define TPOINT2	0004000
#define TANY	0010000
#define TSTRUCT	0020000

#define NOPREF	0x2000

struct node {
	int	op;
	int	rall;		/* register preference, NOPREF when none */
	int	type;
	char	su;		/* 6: registers the subtree needs (word registers) */
	char	fsu;		/* 7: float registers the subtree needs */
	struct sym *sy;		/* 8: the symbol of a NAME or ICON (non-zero: the constant has a name) (host: own field, see TODO.md) */
	int	lb;		/* 8: the label of a CBRANCH (host: own field, see TODO.md) */
	int	stsize;		/* 0xA: STASG size, TEMP number */
	long	lvv;			/* 0xC: the value of a leaf (host: own field, see the note on the overlay in TODO.md) */
	struct node *lp;		/* the left subtree (host: own field; the original overlays it on the low word of lvv) */
	int	rvv;		/* 0x10: the register, the tag (host: own field, see TODO.md) */
	struct node *rpp;	/* 0x10: the right subtree (host: own field, see TODO.md) */
};
#define lval	lvv
#define left	lp
#define rval	rvv
#define right	rpp
#define sname	sy
#define label	lb

/* a symbol of the symbol table file (22 bytes there), see ../KNOWLEDGE.md */
struct sym {
	int	nameoff;	/* +0: offset of the name in the ascii table */
	int	stype;		/* +2 */
	char	sclass;		/* +4: 1 auto, 3 static, 4 register, 9 parameter ... */
	char	sbyte5;		/* +5: the register number of a register variable, the size of a field in bits (+4) */
	int	sx6;		/* +6 */
	long	soffset;	/* +8: bit offset in the frame, or the label number of a static */
	long	ssize;		/* +0xC: size in bits (a structure argument) */
	int	sflags;		/* +0x10 */
	int	sx12;		/* +0x12 */
	int	sx14;		/* +0x14: 0xFFFF */
};
extern char ascii[];		/* the ascii table (names of the symbols) */
extern struct sym *fpsym1, *fpsym2;	/* the pseudo symbols ~L1 and ~L2 of the frame */

struct rs {
	char reg;	/* register number (byte registers 0..15, word registers 0..15, long registers even) */
	char cls;
	char kind;	/* 8: holds a symbol */
	char busy;	/* use count */
	long	off;		/* frame offset of the symbol it holds */
	int	flags;		/* bit 15: ... */
	struct rs *next;
	struct sym *sym;
};
extern int fregs, ffregs;	/* 0x8f8a, 0x8f88: the number of word and float registers (8, 4) */
extern struct rs *rsfree, *rsuse, *rspool;

struct optab {
	int	op;
	int	visit;
	int	lshape;
	int	ltype;
	int	rshape;
	int	rtype;
	int	needs;		/* registers needed (counts in nibbles), 0x1000: the entry only rewrites the tree */
	int	nx;		/* 0xE: not used by match(); small flags (1, 2, 5, 0xd ...), meaning not yet known */
	int	rewrite;	/* 0x10: RLEFT, RRIGHT, RESC1 ... */
	char	*cstring;
};
extern struct optab table[];
extern int ntable;

struct indope { int op; char name[8]; int flags; };	/* 12 bytes like the original */
extern struct indope indope[];
extern int dope[];		/* dope flags by operator, filled from indope[] */

/* binary input of the intermediate files: big endian; WEGA int is 16 bit signed, long 32 bit signed */
extern int rd16be(FILE *f);
extern long rd32be(FILE *f);
/* the decimal value of a 16 bit int of the WEGA target as the host sees it */
#define S16(v)	((int)((((long)(v)) & 0xFFFFL) ^ 0x8000L) - 0x8000)

/* error functions: the arguments are passed to the printf formats of the messages (comm3.c) */
extern int cerror(char *, ...);
extern int uerror(char *, ...);
extern void werror(char *, ...);
