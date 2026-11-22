/* the code generator table: table3.nzcc.c (codgen) or table3.szcc.c (scodgen, -DSEG) */
#include "mfile3"

static char sccsid[] __attribute__((unused)) = "@[$]table3.c\t2.2  10/06/83 10:57:21 Copr. 1981 87wega3.2.";

#ifdef SEG
#include "table3.szcc.c"
#else
#include "table3.nzcc.c"
#endif
