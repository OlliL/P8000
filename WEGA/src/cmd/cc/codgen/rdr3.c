/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "mfile3"

extern char *getmem();
extern int firstsect;
extern long gasciibytes;
extern int gnumbytes;
extern int gnumcount;
extern long gsymbytes;
extern long nasciibytes;
extern int nglob;
extern int nmembytes;
extern int nmems;
extern int nnumbytes;
extern int nnums;
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern void readmems();
extern void readnums();
extern void readsyms();
extern void readtab();
extern void sortsyms();
extern int stflag;
extern long symbase;
extern struct tagent tagtab[];
static char sccsid[] = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
#include "../mip/rdr.2.5"
