/*
 * comm3.c - errors, the tree pool, tree walks, small helpers, memory
 */
#include "cg.h"
#include "cg2.h"

extern char *getmem();
extern void allocinit();
extern int argsize();
extern void branch();
extern int bsize();
extern int cerror();
extern void d3jlabel();
extern void d3jsave();
extern void d3kjump();
extern int d3knamed();
extern void d3kreset();
extern int dope[];
extern void mkdope();
extern void dexit();
extern void walkf();
extern void fwalk();
extern int getreg();
extern int lineno;
extern char ftitle[];
extern struct dopest indope[];
extern int ispow2();
extern NODE *lastfree;
extern void ncopy();
extern int nerrors;
extern NODE node[];
extern char *opst[];
extern int nncon();
extern int callreg();
extern void rbusy();
extern int revrel[];
extern void rfree();
extern char *rnames[];
extern struct rs *rsfree;
extern void rsreturn();
extern struct rs *rsuse;
extern char *sbrk();
extern void syserr();
extern int szty();
extern NODE *talloc();
extern int tbytes();
extern void tcheck();
extern NODE *tcopy();
extern void tfree();
extern void tinit();
extern void uerror();
extern void werror();
static char sccsid[] = "@[$]comm3.c\t2.1  11/15/82 12:48:27 Copr. 1981 87wega3.2.";
char unused6[16] = { 0 };
char ftitle[16] = { 0 };
#include "common.2.5"
