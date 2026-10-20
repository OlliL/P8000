static char sccsid[] = "@[$]instr.c\t1.1 03/16/83 18:59:17 - 87wega3.2";

/*
 * instr - choose the form of an instruction and write it to the intermediate file
 */
#include "cas.h"

int shortaddr;				/* a short segmented address was seen in this instruction (bss a234) */

/*
 * Look for the form of the instruction op that fits the operand list: the
 * first one whose template matches.  Record 10 gets the table row and the operands.
 */
putins(op, args)
struct pst *op;
NODE *args;
{
	register unsigned i;
	unsigned char n;

	if (op == 0)
		error(2, "Illegal opcode\n");
	n = op->p_n;
	for (i = 0; i <= n - 1; i++) {
		if (opmatch(op->p_rows[i].ir_cls, args) == 0) {
			putrow(&op->p_rows[i], args);
			switch (op->p_tok) {
			case 62:
				iputb(1);
				iputb(4);
				break;
			case 63:
				iputb(1);
				iputb(3);
				break;
			case 64:
				iputb(1);
				iputb(1);
				break;
			case 65:
				iputb(1);
				iputb(0);
				break;
			}
			return(0);
		}
	}
	error(2, "Invalid operand combination\n");
	putfill();
	return(-1);
}

/* does the template at opcls[a] fit the operands p? */
opmatch(a, p)
int a;
register NODE *p;
{
	register int j;
	unsigned char c;

	for (j = 0; p; j++, p = p->n_next) {
		c = opcls[a + j];
		if (clstype[c] != p->n_type)
			if (p->n_type != '4' || c != 0x14)
				return(-1);
	}
	if (opcls[a + j] == 0x12 || opcls[a + j] == 0x1c)
		return(0);
	return(-1);
}

/* record 10: the table row and the operands, then the length of the instruction */
putrow(row, args)
struct irow *row;
NODE *args;
{
	register NODE *p;
	register int j;
	int len;

	len = 0;
	iputb(10);
	iputl(row);
	for (p = args; p; p = p->n_next) {
		iputnode((NODE *)p->n_u.ll.l1);
		iputnode((NODE *)p->n_u.ll.l2);
	}
	while (args) {
		args = args->n_next;
		nfreed++;
	}
	if (segmode) {
		if (row->ir_kind != 'Q' && row->ir_kind != 'R') {
			for (j = row->ir_cls; opcls[j] != 0x1c; j++) {
				if (opcls[j] == 0x12)
					break;
				switch (opcls[j]) {
				case 5:
				case 6:
					if (shortaddr == 0)
						len = 2;
				}
			}
		}
		shortaddr = 0;
	}
	locadv(instlen[row->ir_kind].len + len);
}

/* a record for an instruction that could not be assembled: four zero bytes */
putfill()
{
	register int i;

	iputb(10);
	for (i = 0; i < 4; i++)
		iputb(0);
}
