/*
** ---  RG_COMP.C ------- COMPARE OPERATIONS  ---------------------------------
**
**	Version: @(#)rg_comp.c		4.0	02/05/89
**
*/

# include	"rg_int.h"

static char	*cmp_op[] =
{
	"<",	/* bdLT	*/
	"==",	/* bdEQ	*/
	"<=",	/* bdLE	*/
	">",	/* bdGT	*/
	"!=",	/* bdNE	*/
	">="	/* bdGE	*/
};


rg_compare(c, c1, c2)
register int		c;
register SYMBOL		*c1;
register SYMBOL		*c2;
{
	register union anytype	*any1;
	register union anytype	*any2;


	any1 = &c1->sy_val;
	any2 = &c2->sy_val;
	if (c1->sy_frmt == CHAR && c2->sy_frmt == CHAR)
	{
		c = AAscompare(any1->cptype, c1->sy_frml,
			     any2->cptype, c2->sy_frml);
		if (!c)
			return ((int) bdEQ);
		if (c < 0)
			return ((int) bdLT);
		return ((int) bdGT);
	}
	if (ck_2_frm(c1, c2, FLOAT, sizeof (double), cmp_op[--c]))
		return (-1);
	if (any1->f8type == any2->f8type)
		return ((int) bdEQ);
	if (any1->f8type > any2->f8type)
		return ((int) bdGT);
	return ((int) bdLT);
}
