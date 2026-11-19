/*
 * gopt.h - definitions of the global optimizer /lib/gopt (the second part of the C compiler beside codgen:
 * it reads the intermediate language of cparse, reader2.c ... reg2.c). Reconstructed from the binary; the name of the
 * original is mfile2.h (SCCS 1.7 03/04/83). Layout of NODE (22 bytes, pointers 32 bits) and the tables are measured.
 */
#include "stdio.h"

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
		int	type;
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
		int	type;
		char	su;
		char	fsu;
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
		int	type;
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

struct dopest { int dopeop; char opst[8]; int dopeval; };	/* 12 bytes */
extern struct dopest indope[];
extern int dope[];
extern char *opst[];
