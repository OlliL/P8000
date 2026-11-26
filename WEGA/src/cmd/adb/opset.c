static char sccsid[] = "@[$]opset.c\t3.2  08/25/83 09:48:55 - 87wega3.2";

#include "defs.h"

MSG		NODIS;

ADDR		*findsym();

printins(ap, space)
ADDR	*ap;
{
	INSTR	ins;
	INT	i;
	BOOL	comma;
	ADDR	*np;

	comma = FALSE;
	IF space==0 THEN error(NODIS); FI
	np = disasm(ap, space, &ins);
	printf("%s%8t", ins.d_opcode);
	FOR i=0; i<4; i++
	DO	IF *ins.d_operand[i]
		THEN	IF comma THEN printc2(',', 0); FI
			printf("%s", ins.d_operand[i]);
			comma = TRUE;
		FI
	OD
	IF ins.d_comment THEN printf("%25t%s", ins.d_comment); FI
	IF np
	THEN dotinc = np->a_offset - ap->a_offset;
	ELSE dotinc = 2;
	FI
}

symfn(ap, type, buf)
ADDR	*ap;
STRING	buf;
{
	ADDR	*dp;
	STRING	p, q;

	svar[2] = svar[1];
	svar[1] = *ap;
	IF type==ISYMBOL
	THEN type = ISYM;
	ELSE type = DSYM;
	FI
	dp = findsym(ap, type);
	IF leng(maxoff) <= dp->a_offset
	THEN	IF segflg
		THEN sprintf(buf, "<<%%%x>>%%%04X", (INT)ap->a_segno, ap->a_offset);
		ELSE sprintf(buf, "%%%04X", ap->a_offset);
		FI
	ELSE	p = buf;
		q = symname;
		WHILE *q DO *p = *q; p++; q++; OD
		IF dp->a_offset THEN sprintf(p, "+%%%x", dp->a_offset); FI
	FI
}
