/*
 * cas.h - declarations shared by the modules of the host cas
 *
 * The host fork of the cas assembler (the U8000 assembler of WEGA), derived from
 * ../../src/cmd/cas. Same behaviour, same output; changes for a 32/64 bit host:
 *
 *  - the 16 bit int of the Z8000 is int16_t/uint16_t where the width matters
 *  - char is unsigned (-funsigned-char, as in scc)
 *  - pointers do not travel in longs: a node holds node pointers, the symbol tables hold
 *    offsets into three arenas (as the original: a symbol is its offset in the table)
 *  - all binary files are written in the byte order of the Z8000 (big endian)
 *  - the instruction row of a record 10 is its index in itab1
 */
#ifndef CAS_H
#define CAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* expression and statement nodes, allocated by nodes.c */
typedef struct node NODE;
struct node {
	uint8_t	n_type;
	NODE	*n_next;
	int32_t	n_val;			/* constant: the value */
	int16_t	n_w;			/* name: symbol index; type 6: the repeat count */
	uint8_t	n_c1;			/* name: how it is defined; operation: type of the result */
	uint8_t	n_c2;			/* operation: the operator */
	NODE	*n_a;			/* operation: left; operand: expression; type 6, 0x16: the node */
	NODE	*n_b;			/* operation: right; operand: second part */
	NODE	*n_link;		/* nodes beyond the pool (nodes.c) */
};

#define NNODES	100			/* size of the node pool */

extern NODE	nodes[NNODES];
extern unsigned	nalloc;			/* nodes handed out */
extern unsigned	nfreed;			/* nodes written to the intermediate file */

/* the instruction tables (table1.c, table2.c) */
struct irow {				/* one operand form of an instruction */
	uint16_t ir_0;			/* base opcode */
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

extern char tokbuf[];			/* the text of the last token */
extern int toklen;			/* its length, for a string (it may hold a NUL) */

struct pst {				/* the permanent symbols: mnemonics, registers, directives */
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

/*
 * An entry of the user symbol table (or of the information table). Entries are 10 or 16 bytes
 * (6 or 12 in the information table); the table is a byte arena, an index is an offset in it.
 * s_2 of a symbol is the offset of its name in the name arena, of an information entry its
 * value (or, for an alias, the index of the permanent symbol).
 */
#pragma pack(push, 1)
struct sym {
	int16_t	s_0;
	int32_t	s_2;
	uint8_t	s_type;			/* 3: alias of a permanent symbol */
	uint8_t	s_7;			/* flags: bit 0, bits 1 and 2 (scope) */
	int16_t	s_8;
	int16_t	s_10;
	int16_t	s_12;			/* block level of the definition */
	int16_t	s_14;			/* next symbol with the same hash value */
};

struct jent {				/* an undecided jpr in the information table (jpr.c, jpr2.c): 14 bytes */
	int16_t	j_0;
	int16_t	j_2;
	int16_t	j_4;
	int32_t	j_6;
	int16_t	j_10;
	uint8_t	j_12;
	uint8_t	j_13;
};
#pragma pack(pop)

struct seekent {			/* where a section starts in the object file and in the relocation file */
	long	sk_off;
	long	sk_end;
};

struct lcaux {				/* a pair per location counter (jpr.c, optim.c) */
	int16_t	la_0;
	int16_t	la_1;
};

/* the value of the last token is yylval (YYSTYPE, the %union of parser.y: y.tab.h) */

/* nodes.c */
NODE *newnode(void);
NODE *errnode(void);
NODE *synnode(void);
void chknodes(void);
NODE *mknode1(int type, int32_t val);
NODE *mknode2(int type, NODE *a, NODE *b);
NODE *mknode6(int w, NODE *np);
NODE *mknode7(int w);
NODE *mkstring(const char *s);
NODE *mknode25(int c1, int c2, NODE *a, NODE *b);

/* expr.c */
NODE *opadd(NODE *l, NODE *r), *opsub(NODE *l, NODE *r), *opmul(NODE *l, NODE *r);
NODE *opdiv(NODE *l, NODE *r), *opshl(NODE *l, NODE *r), *opshr(NODE *l, NODE *r);
NODE *opand(NODE *l, NODE *r), *opor(NODE *l, NODE *r), *opxor(NODE *l, NODE *r);
NODE *opshort(NODE *np), *opoff(NODE *np), *opseg(NODE *np), *opneg(NODE *np), *opplus(NODE *np);
int exprtype(NODE *np);			/* the type of an expression */
int evalint(NODE *np);			/* its value as an int */
int32_t exprval(NODE *np), exprreloc(NODE *np);	/* its value and relocation (pass 2) */
extern int shortop;			/* a short address was seen (expr.c, gen.c, assemble.c) */

/* ifile.c */
extern int curline;
extern FILE *ifile, *sfile;		/* the intermediate file and the symbol file */
void iputb(int c), iputw(int w), iputl(int32_t l);
void sputb(int c), sputw(int w), sputl(int32_t l);
int igetb(void), igetw(void), sgetb(void), sgetw(void);
int32_t igetl(void), sgetl(void);
void iputnode(NODE *np), sputnode(NODE *np), iputexpr(NODE *np), sputexpr(NODE *np);
void iputloc(void), iputline(void);

/* getops.c */
NODE *igetexpr(void), *sgetexpr(void), *sgetnode(void);
void getop(int n);
extern NODE *ops[];			/* operands of the instruction (assemble.c) */

/* ssymtab.c */
struct sym *stabp(int idx);		/* index -> symbol (0 for the index 0) */
struct sym *symof(int idx);		/* index -> symbol (an empty one for the index 0) */
struct sym *itabp(int idx);		/* index -> entry of the information table */
int stidx(struct sym *p);
int itidx(void *p);
struct sym *stalloc(int n), *italloc(int n);
const char *symname(struct sym *sp);
int stenter(const char *name, int mode);
int stlook(const char *name, int mode);
void setsym(int idx, int type, int kind);
void blkopen(void), blkclose(void);
extern int blklevel;
extern unsigned stoff;			/* used part of the symbol table */

/* symbols.c */
void defsym(NODE *np, NODE *val);
void defalias(NODE *np, struct pst *val);
int32_t symval(int idx), symreloc(int idx);
int isdef(int idx);
void defcomm(NODE *size, NODE *list);
void deflabels(void);
void putstabn(NODE *a, NODE *b, NODE *c, const char *s);
void putstabp(NODE *a, NODE *b, NODE *c, NODE *d, const char *s);
void putstabr(NODE *a, NODE *b, const char *s);
extern NODE *plist;			/* labels of the statement, not yet defined (parser.y) */

/* locntr.c */
extern long lctab[][3];			/* location counter of every section and subsection */
extern struct lcaux lcaux[][3];
extern int cursec, cursub;
extern int nsect;
void locadv(unsigned n);
void setsub(int n), setsec(int n), secname(NODE *np);
void even(void), odd(void), endpass1(void);
NODE *dotval(void);
void setdot(NODE *np);

/* pst.c, subr.c */
struct pst *pstlook(const char *name);
unsigned hash(unsigned n, const char *s);
int32_t xtol(const char *s, unsigned base);
extern struct pst pst[];
extern struct pbucket pbuckets[];
extern struct irow itab1[];

/* instr.c */
int putins(struct pst *op, NODE *args);
void putfill(void);

/* jpr.c, jpr2.c, optim.c */
void jpr(int tok, NODE *list);
void jpr2(int tag);
void optim(int sub, int sec);

/* assemble.c, gen.c */
void asmins(struct irow *row);
void putbits(int n, int v);
void rangechk(int32_t v, int32_t mask);
void genb(int c, int a, unsigned n);
void genw(int w, int a, unsigned n);
void genl(int32_t l, int32_t a, unsigned n);
void genlop(NODE *np, int flag), genwop(NODE *np, int flag), genbop(NODE *np, int flag);
void genaddr(NODE *np);
void seekout(void);
extern int subsw;

/* p2loop.c, ofile.c */
void pass2(void);
void p2tag(int tag);
extern struct seekent *seektab;
extern int imoff;
extern FILE *outf, *relf;
extern FILE *osymf;
void ohdrinit(void), ohdrput(void), osymout(void), ofinish(void), symadjust(void);

/* listing.c */
extern FILE *listf;
void copyfile(FILE *dst, const char *dname, FILE *src, const char *sname);
void listhead(void), lstw(int w, int a), lstb(int c, int a), lstsync(int n), lstrest(void);

/* error.c, main.c, done.c */
void error(int n, const char *msg);
void done(int code);
extern int echoflag, uflag, vflag, dflag, lflag, synerr, semerr;
extern char *outname, *rfname, *ofname, *sfname, *ifname;

/* scanner.c, parser.y */
int yylex(void);
int getch(void);
int yyparse(void);

#endif
