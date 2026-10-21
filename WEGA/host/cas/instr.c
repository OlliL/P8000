/*
 * instr - choose the form of an instruction and write it to the intermediate file
 */
#include "cas.h"

int shortaddr;				/* a short segmented address was seen in this instruction */

static int opmatch(int a, NODE *p);
static void putrow(struct irow *row, NODE *args);

/*
 * Look for the form of the instruction op that fits the operand list: the
 * first one whose template matches. Record 10 gets the table row and the operands.
 */
int
putins(struct pst *op, NODE *args)
{
	uint16_t i;
	unsigned char n;

	if (op == 0) {
		error(2, "Illegal opcode\n");
		return (-1);
	}
	n = op->p_n;
	for (i = 0; i <= (uint16_t)(n - 1); i++) {
		if (opmatch(op->p_rows[i].ir_cls, args) == 0) {
			putrow(&op->p_rows[i], args);
			switch (op->p_tok) {
			case 62:
				iputb(1);
				iputb(0);
				break;
			case 63:
				iputb(1);
				iputb(4);
				break;
			case 64:
				iputb(1);
				iputb(3);
				break;
			case 65:
				iputb(1);
				iputb(1);
				break;
			}
			return (0);
		}
	}
	error(2, "Invalid operand combination\n");
	putfill();
	return (-1);
}

/* does the template at opcls[a] fit the operands p? */
static int
opmatch(int a, NODE *p)
{
	int j;
	unsigned char c;

	for (j = 0; p; j++, p = p->n_next) {
		c = opcls[a + j];
		if (clstype[c] != p->n_type)
			if (p->n_type != '4' || c != 0x14)
				return (-1);
	}
	if (opcls[a + j] == 0x12 || opcls[a + j] == 0x1c)
		return (0);
	return (-1);
}

/* record 10: the table row and the operands, then the length of the instruction */
static void
putrow(struct irow *row, NODE *args)
{
	NODE *p;
	int j, len;

	len = 0;
	iputb(10);
	iputl(row - itab1);
	for (p = args; p; p = p->n_next) {
		iputnode(p->n_a);
		iputnode(p->n_b);
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
void
putfill(void)
{
	int i;

	iputb(10);
	for (i = 0; i < 4; i++)
		iputb(0);
}
