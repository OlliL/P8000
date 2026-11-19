/*
 * reg2.c - gopt: the optimizer driver and the register allocation (segment 5). Written from /lib/gopt (no source known);
 * names guessed, the callees by the address (fnXXXX).
 */
#include "gopt.h"

static char sccsid[] = "@[$]reg2.c\t1.11  03/04/83 16:04:05 Copr. 1981 87wega3.2.";

struct bl { struct bl *next; int blk; };	/* a list of blocks */
struct loop { struct bl *bls; int hdr; struct loop *link; struct bl *exits; int tail; int cnt; unsigned b15:1, b14:1, b13:1, b12:1, b11:1, b10:1; };
struct hrec { unsigned *f0; unsigned *f4; unsigned *f8; int w12; int callfnd; };
#define QHREC(q)	(*(struct hrec **)((char *)(q) + 6))
extern struct quad **blks;
extern struct loop *loops;
extern struct quad *qhead;
extern struct sym *symtab;
extern int s4d90;
extern char a0010[];
extern struct bt *betatbl;
struct bt { int x0; int x1; int x[2]; unsigned f15:1, f14:1, bopt:1, rest:13; };
#define BBIT(j, n)	((*(int *)((char *)betatbl + ((j) * 10 + 8)) >> (n)) & 1)
extern int a114, a116, a10e;
extern char *a11a;
extern char *taballoc();
extern int notlive(), fn5_9adc();
extern void prquad(), prvec(), fn5_73d0(), fn5_75dc(), fn5_7858(), fn5_7cb4(), fn5_8198(), fn5_9698();
extern void clearflags(), livevars(), reachdefs(), prquads(), ebbopt(), clearblocks(), marklpblocks();

extern void marklps(), symflags(), chkcalls(), assignreg();

/* the optimizer of the quads: the flow information, the loop information, the value numbering (name guessed) */
void
optimize()
{
	if (s4d90) {
		printf("// quads before register allocation\n");
		prquads();
	}
	clearflags();
	livevars();
	marklps();
	symflags();
	reachdefs();
	assignreg(0);
	assignreg(1);
}

/* the blocks of every loop are marked, and the calls of the loop are looked for (name guessed) */
void
marklps()
{
	struct loop *lp;

	clearblocks();
	for (lp = loops; lp != 0; lp = lp->link) {
		marklpblocks(lp, 1);
		chkcalls(lp);
	}
}

/* a call in a block of the loop sets callfnd in all blocks of the loop (name from the debug output) */
void
chkcalls(lp)
struct loop *lp;
{
	struct bl *p;
	struct quad *q;
	int x;

	p = (struct bl *)lp;
	for (;;) {
		p = p->next;
		if (p == 0)
			break;
		x = blks[p->blk]->a.w0;
		for (q = blks[p->blk]; (q = q->next) != 0; ) {
			if (q->op == 59)
				break;
			if (q->op == 7) {
				if (s4d90 > 4)
					printf("// call found in loop\n");
				for (p = lp->bls; p != 0; p = p->next) {
					if (s4d90 > 4)
						printf("// setting callfnd in block %d\n", p->blk);
					QHREC(blks[p->blk])->callfnd = 1;
				}
				return;
			}
		}
	}
	p = lp->bls;
}

/* the symbols that are stored by a quad of type 3 get a flag cleared; the value numbering is called then (name guessed) */
void
symflags()
{
	struct quad *q;
	int found;

	found = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (q->op == 3 && q->a.b2 == 4) {
			symtab[q->a.w4].sflags &= ~1;
			found = 1;
		}
	}
	if (found)
		ebbopt();
}

/* the registers are assigned to the temporaries (name from the debug output) */
void
assignreg(flag)
int flag;
{
	struct quad *q, *last;
	unsigned *dfptr;

	a11a = taballoc(0, 0);
	a116 = 0;
	a10e = 1;
	last = 0;
	for (q = qhead; q != 0; q = q->next) {
		if (s4d90 > 4) {
			printf("// assignreg: quad =");
			prquad(q);
		}
		if (q->op == 59) {
			a114 = q->a.w0;
			if (s4d90 > 4) {
				if (QHREC(blks[a114])->w12)
					printf("// block %d in loop\n", a114);
				if (QHREC(blks[a114])->callfnd)
					printf("// call in loop\n");
				printf("// assignreg:dfptr = %X, bblockno = %d\n", QHREC(q), a114);
			}
			dfptr = QHREC(q)->f0;
			if (s4d90 > 4) {
				printf("// assignreg : quad =");
				prquad(q);
				prvec(dfptr, 5, 1);
			}
			continue;
		}
		if (fn5_9adc(q)) {
			switch (q->c.b2) {
			case 4:
				if ((symtab[q->c.w4].sflags & 1) || q->f7)
					fn5_73d0(flag, q, last);
				break;
			case 8:
				if (BBIT(q->c.w4, 13)) {
					if ((notlive(q, dfptr) == 0 || q->op == 60) && q->op != 47)
						fn5_73d0(flag, q, last);
				}
				break;
			case 6:
				if (BBIT(q->c.w4, 13) && a0010[q->c.w4] > 2 && q->op != 47)
					fn5_73d0(flag, q, last);
				break;
			}
		}
		last = q;
	}
	if (a116) {
		fn5_75dc();
		fn5_7858();
		if (s4d90) {
			printf("// graph before coloring\n");
			fn5_9698();
		}
		fn5_7cb4(flag);
		if (s4d90) {
			printf("// graph after coloring\n");
			fn5_9698();
		}
		fn5_8198();
		if (s4d90) {
			printf("// quads after register allocation\n");
			prquads();
		}
	}
}
