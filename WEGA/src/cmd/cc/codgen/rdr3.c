/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "cg.h"
#include "cg2.h"

extern char *getmem();
extern char ascii[];
extern NODE *bcon();
extern NODE *buildstar();
extern int cerror();
extern NODE *clocal();
extern FILE *dfile;
extern int dope[];
extern int eflag;
extern NODE *eread();
extern int firstsect;
extern void fixfld();
extern void fixtemp();
extern struct sym *fpsym1;
extern struct sym *fpsym2;
extern long ftell();
extern long gasciibytes;
extern int getreg();
extern int gnumbytes;
extern int gnumcount;
extern long gsymbytes;
extern NODE *makeblock();
extern struct sym *mapn();
extern void mapnm();
extern void mapsym();
extern void mkhash();
extern long nasciibytes;
extern int nglob;
extern void nlcheck();
extern int nmembytes;
extern int nmems;
extern int nnumbytes;
extern int nnums;
extern NODE node[];
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern int rclass();
extern int rddata();
extern long rdin();
extern long rdin2();
extern int rdsym();
extern void readmems();
extern void readnums();
extern void readsyms();
extern void readtab();
extern struct rv regvar[];
extern int rflag;
extern void sortsyms();
extern FILE *stfile;
extern int stflag;
extern long stg;
extern long stpos;
extern long symbase;
extern struct sym *symtab;
extern struct sym *symtab0;
extern int tagfind();
extern struct { int tag; struct sym *sym; } tagtab[];
extern NODE *talloc();
extern char *tnames[];
static char sccsid[] = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
#include "../mip/rdr.2.5"
