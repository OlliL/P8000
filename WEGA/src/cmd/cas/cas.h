/*
 * cas.h - declarations shared by the modules of the cas assembler
 *
 * Reconstructed from the WEGA binary /bin/cas; the names are ours (the binary
 * is stripped), the layout of the structures follows the binary.
 */

/* expression and statement nodes, allocated by nodes.c (16 bytes) */
typedef struct node NODE;
struct node {
	unsigned char n_type;		/* 0: node type */
	char	n_pad;			/* 1 */
	NODE	*n_next;		/* 2 */
	union {				/* 6 */
		long	l;
		struct { int w; long l; } wl;
		struct { int pad; int w; } pw;
		struct { long l1; long l2; } ll;
		struct { unsigned char c1; unsigned char c2; long l1; long l2; } cc;
	} n_u;
};

#include <stdio.h>
#define NNODES	100			/* size of the node pool */

extern NODE	nodes[NNODES];
extern unsigned	nalloc;			/* nodes handed out */
extern unsigned	nfreed;			/* nodes written to the intermediate file */

/* the instruction tables (table1.c, table2.c) */
/* a symbol of the permanent symbol table (mnemonics, registers, directives) */
struct irow {				/* 6 bytes: one operand form of an instruction */
	int	ir_0;
	int	ir_cls;			/* offset of its operand template in opcls[] */
	unsigned char ir_kind;		/* index into instlen[] */
	char	ir_5;
};


extern unsigned char opcls[];		/* operand templates, ended by 0x12 or 0x1c */
extern unsigned char clstype[];		/* node type that an operand class needs */
struct ilen {				/* length of an instruction and what the code generator needs */
	unsigned char len;
	char	x[5];
};
extern struct ilen instlen[];

extern int segmode;			/* set by .seg, cleared by .nonseg */
extern int shortaddr;			/* a short segmented address was seen */

/* the value of the last token is yylval (YYSTYPE, the %union of parser.y: y.tab.h) */
extern char tokbuf[];			/* the text of the last token */

struct pst {				/* 12 bytes */
	int	p_tok;			/* token to return; 62 .. 65: instructions with a trailer record */
	char	p_2;
	unsigned char p_n;		/* instructions: number of forms */
	struct irow *p_rows;		/* instructions: the forms (table1.c) */
	char	*p_name;
};

struct pbucket {			/* the hash table of the permanent symbols (503 entries) */
	struct pbucket *pb_next;
	struct pst *pb_ent;
};

/* an entry of the user symbol table (segment 7) */
struct sym {
	int	s_0;
	long	s_2;
	unsigned char s_type;		/* 3: alias of an entry of the itable */
	unsigned char s_7;		/* flags: bit 0, bits 1 and 2 (scope) */
	int	s_8;
	int	s_10;
	int	s_12;			/* block level of the definition */
};

extern NODE *mknode1(), *mknode2(), *mknode6(), *mknode7(), *mkstring();	/* nodes.c */
extern struct pst *pstlook();		/* name -> permanent symbol or 0 */
extern int stlook();			/* name, mode -> index of the symbol or 0 */
extern struct sym *stabp();		/* index -> symbol */
extern struct sym *itabp();		/* index -> entry of the itable */
extern long symval();			/* value of a defined symbol */
extern long xtol();			/* digits, base -> long */
extern int isdef();			/* is the symbol defined */

extern int curline;			/* ifile.c */
extern long lctab[][3];			/* location counter of every section and subsection (locntr.c) */
extern int cursec, cursub;		/* the current section and subsection (locntr.c) */
struct lcaux {				/* a pair per location counter (jpr.c, optim.c) */
	int	la_0;
	int	la_1;
};
extern struct lcaux lcaux[][3];
extern int blklevel;			/* nesting level of { } (ssymtab.c) */
extern NODE *plist;			/* labels of the statement, not yet defined (parser.y) */
extern int echoflag;			/* option -^ (main.c) */

extern FILE *ifile, *sfile;		/* the intermediate file and the symbol file (ifile.c) */
extern int nsect;			/* number of sections (locntr.c) */
extern int uflag, vflag, dflag, lflag, synerr, semerr;	/* main.c */

extern struct pbucket pbuckets[];
extern struct irow itab1[];
extern struct pst pst[];
