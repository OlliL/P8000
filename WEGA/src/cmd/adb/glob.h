/*
 *	UNIX debugger: the global variables (defined in the module that mentions them first, in the order of the
 *	original's bss: the order of the first mention in the objects in link order)
 */

/* access.c */
extern INT	kflag, kcorf, kdump, kmemf, segflg, pid, wtflag;
extern MAP	datmap, txtmap;
extern STRING	errflg;
extern CHAR	curseg;
extern L_INT	stksiz, maxstor;

/* command.c */
extern CHAR	*lp;
extern INT	symtype, adrflg;
extern ADDR	expv, dot, ditto, adrval;
extern INT	cntflg;
extern L_INT	cntval;
extern INT	dotinc, fsym, fcor;
extern L_INT	locval, locmsk;
extern INT	mkfault;
extern struct suser	*uptr;
extern POS	regs[];
extern ADDR	svar[];
extern L_INT	var[];
extern INT	executing;

/* expr.c */
extern CHAR	isymbol[], symname[];

/* findfn.c */
extern struct segsym	*segsyms;
extern L_INT	symoff;
extern SYMTAB	fsymbuf;

/* format.c */
extern INT	maxoff;
extern STRING	sigint;
extern INT	sigqit;

/* init.c: the initialised data */
extern CHAR	*printptr, eqformat[], lastc, stformat[], symrqd;
extern INT	lastcom, ibase, outfile;
extern REGLIST	reglist[];
extern STRING	corfil, memfil, kmemfil, signals[], kernfil, symfil;

/* init.c: the rest */
extern BKPTR	bkpthead;
extern INT	callpc;
extern CHAR	*digitptr, printbuf[], line[];
extern INT	argcount, lastframe, eof, exitflg, infile, savpc, magic, maxpos;
extern L_INT	localval;
extern INT	nsegs, signo, nsyms;
extern L_INT	datbas, datsiz, entrypt, symcnt, loopcnt, maxfile, savframe, txtsiz;
extern struct segtab	*segtab;
extern SYMPTR	symend, symnxt;
extern SYMTAB	symbuf[SYMSIZ];
extern TTY	adbtty, usrtty;
extern ret_buf	erradb;

/* runpcs.c, version.c */
extern ADDR	userpc;
extern STRING	version;

/* libc */
extern INT	errno;
