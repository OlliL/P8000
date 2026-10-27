/*
 * z20.c -- tables and global data of popt (WEGA C optimizer)
 */
#include "z2.h"

static char sccsid[] = "@[$]z20.c\t1.3  12/21/82 17:53:10 Copr.1981 87wega3.2.";

/*
 * Mnemonic table.  Entry order matters: it determines the hash chains
 * built by opsetup() and the entry that output() picks for a node.
 * The word is (subop << 8) | op.  Condition codes live in the same table
 * (op CJP, subop = condition number); "ldk" is LD with subop SLDK.
 */
struct optab optab[] = {
	{ "eq",    0x0001 }, { "ne",    0x0101 }, { "le",    0x0201 },
	{ "ge",    0x0301 }, { "lt",    0x0401 }, { "gt",    0x0501 },
	{ "ult",   0x0601 }, { "ugt",   0x0701 }, { "ule",   0x0801 },
	{ "uge",   0x0901 }, { "feq",   0x0a01 }, { "fneu",  0x0b01 },
	{ "flt",   0x0c01 }, { "fle",   0x0d01 }, { "fgt",   0x0e01 },
	{ "fge",   0x0f01 },
	{ "jpr",   0x0002 },
	{ "{\0",    0x0005 },
	{ "ld",    0x0007 }, { "clr",   0x0008 }, { "com",   0x0009 },
	{ "inc",   0x000a }, { "dec",   0x000b }, { "neg",   0x000c },
	{ "test",  0x000d }, { "sra",   0x000e }, { "sla",   0x000f },
	{ "exts",  0x0010 }, { "cp",    0x0011 }, { "add",   0x0012 },
	{ "sub",   0x0013 }, { "bit",   0x0014 }, { "and",   0x0015 },
	{ "or",    0x0016 }, { "mult",  0x0017 }, { "div",   0x0018 },
	{ "sda",   0x0019 }, { "xor",   0x001a },
	{ ".word", 0x001d }, { ".code", 0x001b }, { ".data", 0x001c },
	{ ".long", 0x0030 }, { ".even", 0x001e },
	{ "djnz",  0x001f }, { "call",  0x0020 }, { "push",  0x0021 },
	{ "pop",   0x0022 }, { "ret",   0x0023 }, { "ldm",   0x0024 },
	{ "lda",   0x0025 }, { "ldk",   0x1207 }, { "res",   0x0028 },
	{ "set",   0x0027 }, { "rl",    0x0029 }, { "rr",    0x002b },
	{ "jp",    0x002c }, { "srl",   0x002d }, { "sll",   0x002e },
	{ "sdl",   0x002f }, { "}\0",    0x0032 }, { ".psec", 0x0033 },
	{ ".addr", 0x0035 }, { "callr", 0x0020 },
	{ 0, 0 }
};

/* condition code -> opposite condition code */
char revbr[] = { 1, 0, 5, 4, 3, 2, 9, 8, 7, 6, 11, 10, 15, 14, 13, 12 };

int isn = 20000;	/* next generated label number */
int lastseg = -1;

char line[LSIZE];
char *curlp;
struct node first;
int keepcmt;
int nchange;
int debug;
char flag136a;
char regs[NREGS][32];

int nrevbr, nskip, nrlab, nxjump, ncomj, ncmot, loopiv, nsob;
int iaftbr, njp1, redunm, nredunj, nsaddr, nshft, nrtst, nclr;
int nlda, nrmove, nrename, npushpop, npushdirect;
