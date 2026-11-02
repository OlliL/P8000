/*
 * bss3.c - the common data, in the order of the original: linked first, it gives their order (no code, no data).
 */
#include "cg.h"
#include "cg2.h"

/* the common data, in the order of the original */
int unused1;
long frame0;
int nparm;
long tmpoff;	/* the next free temporary (bits) */
long parmax;
FILE *dfile;	/* the data file */
int ndelay;
int nlabsave;
struct node node[NTREE];
int dope[129];
long paroff;	/* next free place for the parameters of a call (bits) */
int regG;
int regA;
FILE *stfile;	/* the symbol table file */
int regC;
char *opst[129];
int minreg;
int regB;
int regtop;
int lflag;
struct node resc[3];	/* the work registers RESC1 .. RESC3 (0x8f24); the contents stay from call to call; a 4th entry because allo reads one past the last */
int callflag;
struct { struct sym *sym; int reg; } parmtab[11];
int ffregs;	/* set to 4 */
int fregs;	/* set to 8 by the start of the program */
int regD;	/* (7), 0x8f1e (0x10), 0x8e18 (0x13), 0x8f8c (1) */
int regF;
int maxreg;
struct node *stotree;	/* the subtree that has to be stored first */
long stg;	/* the offset of the global section in the symbol table file */
int nparmtab;
struct node *deltrees[NDELAY];
int regmask;
int regE;
int stocook;
int ifline;
int retlab;
long maxoff;	/* the largest frame offset so far */
int nrecur;	/* cleared by stmt() itself */
struct sym *symtab;
long nasciibytes;
struct sym *symtab0;
struct sym *fpsym1;
struct sym *fpsym2;
long symbase;	/* the start of the symbols of the global section */
int nmems;
int nnums;
int nsyms;	/* the symbols read so far */
struct rv regvar[12];	/* symbol, tag of the symbol or of the temporary, register, type, saved class */
int gnumcount;	/* only written */
int numtab[1001];	/* the number table, a word and a -1 per entry */
int gnumbytes;
struct { int tag; struct sym *sym; } tagtab[550];
long gasciibytes;	/* the global section */
int unused2;
int nglob;	/* the entries of tagtab */
long gsymbytes;
int nmembytes;
long nsymbytes;
int nnumbytes;
int entlab;
char fname[100];
int upperlval;	/* print the high word of a long constant */
int nsave;	/* registers saved by the cases 33 and 34 */
int jlab;
int saveorig[2];	/* the register that was saved */
int cbflag;
struct rs *rsfree;
int argstop;	/* the rest of the arguments go to the stack */
int fldsz;
int structret;	/* the function returns a structure */
int savecls[2];	/* its class */
int savereg[2];	/* where it went */
int fldoff;
struct rs *rsuse;
int fldshf;	/* the field that tshape found (size, offset, shift) */
int nargs;
int argreg;	/* the next word register for an argument, counts down from 7 */
int ordflag;	/* set to 0x14 while offstar orders a register that is not preferred */
int unused3[23];
int argfreg;	/* the next float register, counts down from 0x13 */
struct arg argl[11];
struct optab *rwdefault;	/* the entry with needs == 0x1000 (the default rewrite entry) */
struct optab *opptr[129];	/* the first table entry that can serve an operator */
int rstab1[3];
int rsflag1;	/* number of entries of rstab1 */
struct rs pool[NPOOL];
struct rs *rsexcl;	/* nodes taken away from rsuse by rsmerge */
struct rs *rspool;
struct { struct rs *list; int mark; } rsstk[3];	/* the saved lists and their marks, interleaved */
int rsdepth;	/* saved lists */
int rsflag2;	/* number of entries of rstab2 */
int rstab2[3];
struct node *lastfree;
int mcountlab;
int structlab;
long structsz;
struct tmpent tmptab[80];
#ifdef OLD_D16A
struct node *d16acmp;	/* the left operand of the last compare set up: its increment stays in memory (match) */
#endif
#ifdef OLD_D2RA
struct node *d2ranode;	/* the last r0 + rN and its rN (offstar) */
int d2rareg;
#endif
