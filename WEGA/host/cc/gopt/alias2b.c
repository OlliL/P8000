/*
 * alias2b.c - gopt: a rest of the alias module (segment 5). Written from /lib/gopt (no source known); names guessed.
 */
static char sccsid[] __attribute__((unused)) = "@[$]alias2.c\t1.3  08/30/82 17:11:29 Copr. 1981 87wega3.2.";
#include "mfile2.h"

/* the block b ends an extended basic block (name guessed) */
int isebbend(int b)
{
	struct quad *q;

	if (blks[b]->a.w4 < 0)
		return 1;
	for (q = blks[b]->next; q != 0; q = q->next) {
		if (q->op == QBBLOCK)
			break;
		if (q->op == QLABEL)
			return 1;
	}
	return 0;
}

