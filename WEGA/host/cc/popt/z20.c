/*
 * z20.c -- tables and global data of popt (WEGA C optimizer)
 */
#define DEFGLOBALS
#include "z2.h"

static char sccsid[] __attribute__((unused)) = "@[$]z20.c\t1.3  12/21/82 17:53:10 Copr.1981 87wega3.2.";

/* condition code -> opposite condition code */
char revbr[] = { JNE, JEQ, JGT, JLT, JGE, JLE, JHIS, JLOS, JHI, JLO, JFNE, JFEQ, JFGE, JFGT, JFLE, JFLT };

short isn = 20000;	/* next generated label number */
int lastseg = -1;
FILE *errfp;	/* initialised to stderr in the original (not constant here); */	/* name guessed: the original has a word initialized to stderr here that nothing uses */

/*
 * Mnemonic table.  Entry order matters: it determines the hash chains
 * built by opsetup() and the entry that output() picks for a node.
 * The word is (subop << 8) | op.  Condition codes live in the same table
 * (op CJP, subop = condition number); "ldk" is LD with subop SLDK.
 */
struct optab optab[] = {
	{ "eq",    CJP|JEQ<<8 }, { "ne",    CJP|JNE<<8 }, { "le",    CJP|JLE<<8 },
	{ "ge",    CJP|JGE<<8 }, { "lt",    CJP|JLT<<8 }, { "gt",    CJP|JGT<<8 },
	{ "ult",   CJP|JLO<<8 }, { "ugt",   CJP|JHI<<8 }, { "ule",   CJP|JLOS<<8 },
	{ "uge",   CJP|JHIS<<8 }, { "feq",   CJP|JFEQ<<8 }, { "fneu",  CJP|JFNE<<8 },
	{ "flt",   CJP|JFLT<<8 }, { "fle",   CJP|JFLE<<8 }, { "fgt",   CJP|JFGT<<8 },
	{ "fge",   CJP|JFGE<<8 },
	{ "jpr",   JP },
	{ "{\0",    OBRACE },
	{ "ld",    LD }, { "clr",   CLR }, { "com",   COM },
	{ "inc",   INC }, { "dec",   DEC }, { "neg",   NEG },
	{ "test",  TST }, { "sra",   SRA }, { "sla",   SLA },
	{ "exts",  EXTS }, { "cp",    CP }, { "add",   ADD },
	{ "sub",   SUB }, { "bit",   BIT }, { "and",   AND },
	{ "or",    OR }, { "mult",  MUL }, { "div",   DIV },
	{ "sda",   SDA }, { "xor",   XOR },
	{ ".word", WORD }, { ".code", CODE }, { ".data", DATA },
	{ ".long", LONG }, { ".even", EVEN },
	{ "djnz",  SOB }, { "call",  CALL }, { "push",  PUSH },
	{ "pop",   POP }, { "ret",   RET }, { "ldm",   LDM },
	{ "lda",   LDA }, { "ldk",   LD|SLDK<<8 }, { "res",   RES },
	{ "set",   SET }, { "rl",    RL }, { "rr",    RR },
	{ "jp",    JPX }, { "srl",   SRL }, { "sll",   SLL },
	{ "sdl",   SDL }, { "}\0",    CBRACE }, { ".psec", PSEC },
	{ ".addr", ADDR }, { "callr", CALL },
	{ 0, 0 }
};


char line[LSIZE];
char *curlp;
struct node first;
int keepcmt;
int nchange;
char debug;
char flag136a;
char regs[NREGS][32];

int nrevbr, nskip, nrlab, nxjump, ncomj, ncmot, loopiv, nsob;
int iaftbr, njp1, redunm, nredunj, nsaddr, nshft, nrtst, nclr;
int nlda, nrename, npushpop, npushdirect;
