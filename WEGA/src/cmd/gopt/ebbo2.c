/*
 * ebbo2.c - gopt: the optimization of the extended basic blocks (segment 5). Written from /lib/gopt (no source known); names guessed, the
 * callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]ebbo2.c\t1.8  03/04/83 15:54:14 Copr. 1981 87wega3.2.";

extern int s4d80, s4d94, a0d4, a0d2, s71e4, s71e6, s71e8, s71ea;
extern struct quad *qhead;
extern char *a0cc;
extern struct quad *htab[32];
extern char *taballoc();
extern void tabfree(), prqall();
extern struct quad *fn5_0114();
extern void fn5_0b94(), fn5_1ffc(), fn5_206c();

/* the tables of the extended basic block are cleared (name guessed) */
void
ebbinit()
{
	int i;

	a0cc = 0;
	s71e8 = 1;
	for (i = 0; i < 32; i++)
		htab[i] = 0;
	s71e6 = 0;
	s71e4 = 0;
	s71ea = 0;
	a0d2 = 0;
}

/* every extended basic block of the quads is optimized (name guessed) */
void
ebbopt()
{
	struct quad *q, *r;
	char *t;

	if (s4d80 > 1) {
		printf("// before extended basic block optimization\n");
		prqall();
	}
	s4d94 = a0d4;
	t = taballoc(0, 0);
	for (q = qhead; q != 0; ) {
		ebbinit();
		r = q;
		q = fn5_0114(q);
		fn5_0b94(r, q);
		if (a0cc != 0)
			tabfree(0, a0cc);
		fn5_1ffc();
	}
	fn5_206c();
	tabfree(0, t);
	a0d4 = s4d94;
	if (s4d80 > 1) {
		printf("// after extended basic block optimization\n");
		prqall();
	}
}
