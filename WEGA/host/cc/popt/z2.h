/*
 * z2.h -- common definitions for popt, the peephole optimizer of the
 * WEGA C compiler (cc -O).  Reconstructed from the WEGA /lib/popt binary
 * (modules z20.c .. z23.c).  See NOTES.md.
 * The same sources, compiled with -DSPOPT, are spopt (scc -O): the segmented variant;
 * the places that differ are marked SPOPT.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* node types (the "op" byte of a node, also the low byte of an optab word) */
#define CJP	1	/* jpr cc,Lnnn  (subop = condition) */
#define JP	2	/* jpr Lnnn / jp x */
#define LABEL	3	/* Lnnn: */
#define DLABEL	4	/* named label */
#define OBRACE	5	/* { */
#define JSW	6	/* .word Lnnn */
#define LD	7
#define CLR	8
#define COM	9
#define INC	10
#define DEC	11
#define NEG	12
#define TST	13
#define SRA	14
#define SLA	15
#define EXTS	16
#define CP	17
#define ADD	18
#define SUB	19
#define BIT	20
#define AND	21
#define OR	22
#define MUL	23
#define DIV	24
#define SDA	25
#define XOR	26
#define CODE	27	/* .code */
#define DATA	28	/* .data */
#define WORD	29	/* .word */
#define EVEN	30
#define SOB	31	/* djnz */
#define CALL	32
#define PUSH	33
#define POP	34
#define RET	35
#define LDM	36
#define LDA	37
#define SET	39
#define RES	40
#define RL	41
#define EOFOP	42	/* end of input */
#define RR	43
#define JPX	44	/* jp (op word only; nodes get JP) */
#define SRL	45
#define SLL	46
#define SDL	47
#define LONG	48
#define COMMENT	49	/* dropped comment */
#define CBRACE	50	/* } */
#define PSEC	51
#define KCOMMENT 52	/* kept comment lines  / *~~ */
#define ADDR	53

/* subop values for size suffixes */
#define SBYTE	0x10
#define SLONG	0x11
#define SLDK	0x12

#define LSIZE	512
#define LABHS	127
#define OPHS	73
#define NREGS	50

struct node {
	char op;
	char subop;
	struct node *forw;
	struct node *back;
	struct node *ref;
	short labno;		/* 16 bit as in the original (the label numbers wrap) */
	char *code;
	int refc;
};

struct optab {
	char *opstring;
	int opcode;
};

extern struct optab optab[];
extern char revbr[];
extern short isn;
extern int lastseg;
extern char line[];
extern char *curlp;
extern struct node first;
extern int keepcmt;	/* -l flag: keep the / *~~ lines */
extern int nchange;
extern char debug;
extern char flag136a;
extern char regs[NREGS][32];
#define RT1 regs[48]
#define RT2 regs[49]

/* statistics */
extern int nbrbr, nrevbr, nskip, nrlab, nxjump, ncomj, ncmot, loopiv, nsob;
extern int iaftbr, njp1, redunm, nredunj, nsaddr, nshft, nrtst, nclr;
extern int nlda, nrename, npushpop, npushdirect, ndirect;

/* z21.c */
int main(int argc, char **argv);
void opsetup(void);
int oplook(void);
struct node *insertl(struct node *oldp);
struct node *codemove(struct node *p);
void xjump(struct node *p1);
void comjump(void);
void refcount(void);
void iterate(void);
char *copy(int na, char *a, char *b);
int getnum(char *p);
/* z22.c */
char *alloc(int n);
struct node * allocnode();
extern char nullcode[];	/* what the original reads at address 0 for a node without operand text */
void heapreset(void);
int heapcore(void);
struct node *nonlab(struct node *p);
void decref(struct node *p);
int equop(struct node *ap1, struct node *p2);
int ilen(struct node *p);
void rmove(void);
int jumpsw(void);
void shortjump(void);
void clearreg(void);
void dualop(struct node *ap);
void singop(struct node *p);
int isreg(char *s);
int equstr(char *a, char *b);
void movedat(void);
/* z23.c */
void procopt(void);
int isused(struct node *p, char *reg);

/*
 * The globals in the order of their places in the bss of the original: every module includes this list, the
 * first one linked gives the commons their order.
 * Host: defined once, in z20.c (DEFGLOBALS); the other modules see them as extern.
 */
#ifdef DEFGLOBALS
#define GLOBAL
#else
#define GLOBAL extern
#endif
GLOBAL int nchange;
GLOBAL int nrename;
GLOBAL int npushpop;
GLOBAL char conval[32];		/* the original has 20 */
GLOBAL char debug;
GLOBAL int nshft;
GLOBAL int ncmot;
GLOBAL char ccloc[32];		/* the original has 20 */
GLOBAL int nredunj;
GLOBAL int nsaddr;
GLOBAL int njp1;
GLOBAL char regs[NREGS][32];
GLOBAL int nrlab;
GLOBAL char flag136a;
GLOBAL char conloc[32];		/* the original has 20; setcon() reproduces the overflow */
GLOBAL int ndirect;
GLOBAL int nbrbr;
GLOBAL int ncomj;
GLOBAL char line[LSIZE];
GLOBAL int nskip;
GLOBAL int nrevbr;
GLOBAL struct node first;
GLOBAL int nxjump;
GLOBAL int npushdirect;
GLOBAL struct optab *ophash[OPHS];
GLOBAL int nlda;
GLOBAL int redunm;
GLOBAL int nclr;
GLOBAL int nsob;
GLOBAL int iaftbr;
GLOBAL int loopiv;
GLOBAL char *curlp;
GLOBAL int nrtst;
GLOBAL int keepcmt;
GLOBAL char _sobuf[512];
GLOBAL struct node *rmnode;
