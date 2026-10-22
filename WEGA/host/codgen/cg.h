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
	int	global;		/* 6 */
	union {
		struct sym *sname;	/* 8: symbol of a NAME or ICON; non-zero: the constant has a name */
		int	label;		/* the target label of a CBRANCH */
	};
	int	stsize;		/* 0xA: STASG size, TEMP number */
	long	lval;		/* 0xC */
	struct node *left;	/* 0xE */
	int	rval;		/* 0x10 (BITYPE: the right subtree) */
	struct node *right;
};

/* a symbol of the symbol table file (22 bytes there), see ../KNOWLEDGE.md */
struct sym {
	int	nameoff;	/* +0: offset of the name in the ascii table */
	int	stype;		/* +2 */
	int	sclass;		/* +4 (high byte of the third word): 1 auto, 3 static, 9 parameter ... */
	long	soffset;	/* +8: bit offset in the frame, or the label number of a static */
	int	sflags;		/* +0x10 */
};
struct sym;
extern char ascii[];		/* the ascii table (names of the symbols) */
extern struct sym *fpsym1, *fpsym2;	/* the pseudo symbols ~L1 and ~L2 of the frame */

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

struct indope { int op; int flags; char *name; };
extern struct indope indope[];
extern int dope[];		/* dope flags by operator, filled from indope[] */
