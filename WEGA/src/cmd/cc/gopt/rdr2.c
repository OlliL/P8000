/*
 * rdr2.c - gopt: the readers of the symbol table file and of the intermediate file (the pcc file "rdr", SCCS 2.4, included
 * in the original by rdr2.c 1.3 03/04/83). Written from /lib/gopt; the relative is ../codgen/rdr3.c (rdr 2.5 08/24/83).
 */
static char sccsid[] = "@[$]rdr2.c\t1.3  03/04/83 16:01:05 Copr. 1981 87wega3.2.";
#include "mfile2.h"

extern long rdin2();
extern int nglob;
extern long nsymbytes, nasciibytes;
extern int nnumbytes, nmembytes;
extern int nsyms, nnums, nmems;
extern long numtab[];
extern struct { int tag; struct sym *sym; } tagtab[];
extern void readsyms();
extern void readtab();
extern void readnums();
extern void readmems();
extern void sortsyms();
extern void mapsym();
extern long gsymbytes, gasciibytes, symbase;
extern int gnumbytes, gnumcount;

#include "../mip/rdr.2.4"
