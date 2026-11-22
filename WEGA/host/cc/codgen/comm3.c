/*
 * comm3.c - errors, the tree pool, tree walks, small helpers, memory
 */
#include "cg.h"
#include "cg2.h"


extern int dope[];
extern int lineno;
extern char ftitle[];
extern NODE *lastfree;
extern int nerrors;
extern NODE node[];
extern char *opst[];
extern int revrel[];
extern char *rnames[];
extern struct rs *rsfree;
extern void rsreturn();
extern struct rs *rsuse;
static char sccsid[] __attribute__((unused)) = "@[$]comm3.c\t2.1  11/15/82 12:48:27 Copr. 1981 87wega3.2.";
char unused6[16] = { 0 };
char ftitle[16] = { 0 };
#include "common.2.5"
