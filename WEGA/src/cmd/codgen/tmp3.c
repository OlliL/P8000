/*
 * tmp3.c - the table of the temporaries that hold a register's value (codgen / scodgen)
 *
 * Reconstructed from /lib/codgen: tmpcleanup 0x9308, tmpdump 0x93f4 (debug, never called).
 * The table at 0xa7be has 80 entries of 8 bytes; an entry with a negative off ends the used part.
 */
#include "cg.h"

extern struct rs *rfind();

struct tmpent {
	int	off;		/* +0: frame offset of the temporary, negative: end of the table */
	char	done;		/* +2: not zero: the temporary is still needed */
	char	cls;		/* +3: class of the register */
	int flags;		/* +4: bit 15: the register still holds the value */
	int	reg;		/* +6: the register */
};
struct tmpent tmptab[80];

/* drop the finished temporaries: the last entry takes the place of a finished one */
void
tmpcleanup()
{
	int i, j;
	struct rs *n;

	for (i = 0; i <= 0x4f; i++) {
		if (tmptab[i].off < 0)
			break;
		if (tmptab[i].done)
			continue;
		if ((tmptab[i].flags >> 15 & 1) == 1) {
			n = rfind(tmptab[i].reg, tmptab[i].cls);
			if (n) {
				n->kind = 8;
				n->busy = 0;
			}
		}
		for (j = i + 1; j <= 0x4f; j++) {
			if (tmptab[j].off >= 0)
				continue;
			if (i + 1 == j) {
				tmptab[i].off = -1;
				return;
			}
			j--;
			tmptab[i] = tmptab[j];
			tmptab[j].off = -1;
			i--;
			break;
		}
	}
}

void
tmpdump()
{
	register int i;

	printf("\tTEMP\tREFREM\tOFST/REG\n");
	for (i = 0; tmptab[i].off >= 0; ) {
		printf("\t%d\t%d\t%d\n", tmptab[i].off, tmptab[i].done, tmptab[i].reg);
		if (++i >= 0x4f)
			break;
	}
}
