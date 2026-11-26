static char sccsid[] = "@[$]init.c\t3.4  09/28/83 12:03:47 - 87wega3.2";

#include "defs.h"

CHAR	*printptr = printbuf;
CHAR	eqformat[128] = "x";
CHAR	lastc = EOR;
CHAR	stformat[128] = "x\"=\"^i";
CHAR	symrqd = -1;
INT	lastcom = '=';
INT	ibase = 2;
INT	outfile = 1;
REGLIST	reglist[] = {
	"r0", 0, "r1", 1, "r2", 2, "r3", 3, "r4", 4, "r5", 5, "r6", 6, "r7", 7,
	"r8", 8, "r9", 9, "r10", 10, "r11", 11, "r12", 12, "r13", 13, "r14", 14,
	"sp", 15, "fcw", 17, "pc", 19,
	0, 0
};
STRING	corfil = "core";
STRING	memfil = "/dev/mem";
STRING	kmemfil = "/dev/kmem";
STRING	signals[] = {
	"",
	"hangup",
	"interrupt",
	"quit",
	"illegal instruction",
	"trace/BPT",
	"IOT",
	"EMT",
	"floating exception",
	"killed",
	"bus error",
	"memory fault",
	"bad system call",
	"broken pipe",
	"alarm call",
	"terminated",
	"user signal 1",
	"user signal 2",
	"child death",
	"power fail",
};
STRING	kernfil = "/wega";
STRING	symfil = "a.out";

BKPTR	bkpthead;
INT	callpc;
CHAR	*digitptr;
CHAR	printbuf[MAXLIN];
CHAR	line[LINSIZ];
INT	argcount;
INT	lastframe;
INT	eof;
INT	exitflg;
INT	infile;
INT	savpc;
INT	magic;
INT	maxpos;
L_INT	localval;
INT	nsegs;
INT	signo;
INT	nsyms;
L_INT	datbas;
L_INT	datsiz;
L_INT	entrypt;
L_INT	symcnt;
L_INT	loopcnt;
L_INT	maxfile;
L_INT	savframe;
L_INT	txtsiz;
struct segtab	*segtab;
SYMPTR	symend;
SYMPTR	symnxt;
SYMTAB	symbuf[SYMSIZ];
TTY	adbtty;
TTY	usrtty;
ret_buf	erradb;
