/*
 * z2.h -- common definitions for popt, the peephole optimizer of the
 * WEGA C compiler (cc -O).  Reconstructed from the WEGA /lib/popt binary
 * (modules z20.c .. z23.c).  See NOTES.md.
 * The same sources, compiled with -DSPOPT, are spopt (scc -O): the segmented variant;
 * the places that differ are marked SPOPT.
 */
#include "stdio.h"

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
	int labno;
	char *code;
	int refc;
};

struct optab {
	char *opstring;
	int opcode;
};

extern struct optab optab[];
extern char revbr[];
extern int isn, lastseg;
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
int main();
void opsetup();
int oplook();
struct node * insertl();
struct node * codemove();
void xjump();
void comjump();
void refcount();
void iterate();
char * copy();
int getnum();
/* z22.c */
char *malloc();
char *sbrk();
char *strchr();
char *strrchr();
char *calloc();
char *alloc();
struct node * allocnode();
extern int heapbase, heaplim, heapcur;
struct node * nonlab();
void decref();
int equop();
int ilen();
void rmove();
int jumpsw();
void shortjump();
void clearreg();
void dualop();
void singop();
int isreg();
int equstr();
void movedat();
/* z23.c */
void procopt();
int isused();

/*
 * The globals in the order of their places in the bss of the original: every module includes this list, the
 * first one linked gives the commons their order.
 */
int nchange;
int nrename;
int npushpop;
char conval[20];
char debug;
int nshft;
int ncmot;
char ccloc[20];
int nredunj;
int nsaddr;
int njp1;
char regs[NREGS][32];
int nrlab;
char flag136a;
char conloc[20];
int ndirect;
int nbrbr;
int heapbase;
int ncomj;
char line[LSIZE];
int nskip;
int nrevbr;
struct node first;
int nxjump;
int npushdirect;
struct optab *ophash[OPHS];
int nlda;
int redunm;
int nclr;
int nsob;
int iaftbr;
int loopiv;
int heapcur;
int heaplim;
char *curlp;
int nrtst;
int keepcmt;
char _sobuf[512];
struct node *rmnode;
