/*
 * jpr2 - jpr and callr in the second pass
 *
 * The record (tag 38 jpr, 39 callr) holds the entry made by jprnew, whether there was a
 * condition code, and the operands. When the entry is marked long, the instruction is
 * assembled as jp/call and the location counter adjustment la_0 grows by 2 (4 with
 * segments); otherwise as jr/calr.
 */
#include "cas.h"

void
jpr2(int tag)
{
	int idx, cc;
	struct pst *op;
	struct jent *ip;

	idx = igetw();
	ip = (struct jent *)itabp(idx);
	cc = igetb();
	switch (tag) {
	case 0x26:
		if (ip->j_12) {
			op = pstlook("jp");
			if (cc == 1)
				asmins(op->p_rows + 4);
			else
				asmins(op->p_rows + 1);
			if (segmode)
				lcaux[cursec][cursub].la_0 += 4;
			else
				lcaux[cursec][cursub].la_0 += 2;
		} else {
			op = pstlook("jr");
			if (cc == 1)
				asmins(op->p_rows + 1);
			else
				asmins(op->p_rows);
		}
		break;
	case 0x27:
		if (ip->j_12) {
			op = pstlook("call");
			asmins(op->p_rows + 1);
			if (segmode)
				lcaux[cursec][cursub].la_0 += 4;
			else
				lcaux[cursec][cursub].la_0 += 2;
		} else {
			op = pstlook("calr");
			asmins(op->p_rows);
		}
		break;
	}
}
