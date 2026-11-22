/*
 * rdr2.c - gopt: the readers of the symbol table file and of the intermediate file (the pcc file "rdr", SCCS 2.4, included
 * in the original by rdr2.c 1.3 03/04/83). Written from /lib/gopt; the relative is ../codgen/rdr3.c (rdr 2.5 08/24/83).
 */
static char sccsid[] __attribute__((unused)) = "@[$]rdr2.c\t1.3  03/04/83 16:01:05 Copr. 1981 87wega3.2.";
#include "gopt.h"

extern int dflag;
extern FILE *stfile;
extern FILE *infile;
extern long ifpos;
extern int eflag;
extern int nglob;
extern long nsymbytes, nasciibytes;
extern int nnumbytes, nmembytes;
extern int nsyms, nnums, nmems;
extern struct sym *symtab;
extern struct nument numtab[];
extern struct tagent tagtab[];
extern long stg;
extern long gsymbytes, gasciibytes, symbase;
extern int gnumbytes, gnumcount;

#include "../mip/rdr.2.4"
