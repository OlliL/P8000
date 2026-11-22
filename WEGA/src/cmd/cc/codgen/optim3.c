/*
 * optim3.c - the tree optimizer
 */
#include "cg.h"
#include "cg2.h"

extern int bsize();
extern int cerror();
extern int conval();
extern NODE *convelim();
extern int dope[];
extern int ispow2();
extern void ncopy();
extern NODE node[];
extern NODE *optim();
extern int nncon();
extern int revrel[];
extern NODE *tcopy();
extern void tfree();
extern int treeeq();
extern void uerror();
static char sccsid[] = "@[$]optim3.c\t2.4  03/04/83 15:46:52 Copr. 1981 87wega3.2.";
#include "optim.2.5"
