/*
 * rdr3.c - the readers: numbers and trees of the intermediate file, the symbol table file and the data file
 */
#include "cg.h"
#include "cg2.h"


extern char ascii[];
extern FILE *dfile;
extern int dope[];
extern int eflag;
extern int firstsect;
extern struct sym *fpsym1;
extern struct sym *fpsym2;
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
extern NODE node[];
extern long nsymbytes;
extern int nsyms;
extern int numtab[];
extern struct rv regvar[];
extern int rflag;
extern FILE *stfile;
extern int stflag;
extern long stg;
extern long stpos;
extern long symbase;
extern struct sym *symtab;
extern struct sym *symtab0;
extern struct { int tag; struct sym *sym; } tagtab[];
extern char *tnames[];
static char sccsid[] __attribute__((unused)) = "@[$]rdr3.c\t2.1  11/15/82 12:49:01 Copr. 1981 87wega3.2.";
#include "rdr.2.5"
