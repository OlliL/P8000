/*
** ---  RG_DOM.C -------- DOMAIN HANDLING  ------------------------------------
**
**	Version: @(#)rg_dom.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_int.h"

# define	FRMT		tos->sy_frmt
# define	FRML		tos->sy_frml


rg_push_dom(tos, tup, from, to)
register SYMBOL		*tos;
register char		*tup;
int			from;
int			to;
{
	register DESC_T		*d;
	register union anytype	*any;
	int			len;
	extern DESC_T		*ck_dom();
	extern char		*AAbmove();

	d = ck_dom(R_val);
	tup += d->domoff;
	len = ctou(d->domfrml);
	if (d->domfrmt == CHAR)
	{
		from = ck_len(&len, from, to);
		tup += from;
		push_str(tos, tup, len);
# ifdef RG_TRACE
		if (TR_PUSH)
		{
			printf("PUSH-DOM CHAR len=%d |", len);
			while (len--)
			{
				putchar(*tup);
				tup++;
			}
			printf("|\n");
		}
# endif
		return;
	}
	/* d->domfrmt == INT | FLOAT */
	FRMT = FLOAT;
	FRML = sizeof (double);
	any = &tos->sy_val;
	AAbmove(tup, any->c0type, len);

	/* now convert to double */
	switch (len)
	{
	  case sizeof (char):	/* must	be INT */
		any->f8type = ctoi(any->i1type);
		break;

	  case sizeof (short):	/* must	be INT */
		any->f8type = any->i2type;
		break;

	  case sizeof (long):
		if (d->domfrmt == INT)
			any->f8type = any->i4type;
# ifndef NO_F4
		else
			any->f8type = any->f4type;
# endif
	}
# ifdef RG_TRACE
	if (TR_PUSH)
		printf("PUSH-DOM FLOAT %10.3f\n", any->f8type);
# endif
}


rg_pr_dom(tup)
register char	*tup;
{
	register DESC_T		*d;
	register SYMBOL		*tos;
	register union anytype	*any;
	extern char		*AAbmove();
	extern DESC_T		*ck_dom();

	d = ck_dom(R_val);
	tup += d->domoff;
	tos = Print;
	FRML = ctou(d->domfrml);
	any = &tos->sy_val;
	if ((FRMT = d->domfrmt)	== CHAR)
		any->cptype = tup;
	else
		AAbmove(tup, any->c0type, ctou(FRML));
}
