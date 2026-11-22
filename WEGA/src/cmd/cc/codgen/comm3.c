/*
 * comm3.c - errors, the tree pool, tree walks, small helpers, memory
 */
#include "cg.h"
#include "cg2.h"

extern char *sbrk();
extern NODE node[], *lastfree;	/* bss3.c (cparse: common) */
extern char *opst[];
extern int lineno;
static char sccsid[] = "@[$]comm3.c\t2.1  11/15/82 12:48:27 Copr. 1981 87wega3.2.";
char unused6[16] = { 0 };
char ftitle[16] = { 0 };
#include "common.2.5"
