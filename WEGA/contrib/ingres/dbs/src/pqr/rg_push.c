/*
** ---  RG_PUSH.C ------- PUSH SYMBOL INTO STACK  -----------------------------
**
**	Version: @(#)rg_push.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_int.h"


push_str(tos, str, len)
register SYMBOL	*tos;
register char	*str;
register int	len;
{
	register union anytype	*any;

	tos->sy_frmt = CHAR;
	tos->sy_frml = ctou(len);
	any = &tos->sy_val;
	any->cptype = str;
# ifdef RG_TRACE
	if (TR_PUSH)
	{
		printf("PUSH-STR on %d, len=%d '", tos - Stack, len);
		while (len--)
		{
			putchar(*str);
			str++;
		}
		printf("'\n");
	}
# endif
}


push_dbl(tos, val)
register SYMBOL	*tos;
double		val;
{
	register union anytype	*any;

	tos->sy_frmt = FLOAT;
	tos->sy_frml = sizeof (double);
	any = &tos->sy_val;
	any->f8type = val;
# ifdef RG_TRACE
	if (TR_PUSH)
		printf("PUSH-DBL on %d, %10.3f\n",
			tos - Stack, val);
# endif
}


push_bin(tos, val)
register SYMBOL	*tos;
register BOOL	val;
{
	register union anytype	*any;

	tos->sy_frmt = BIN;
	tos->sy_frml = sizeof (BOOL);
	any = &tos->sy_val;
	any->booltype = val;
# ifdef RG_TRACE
	if (TR_PUSH)
		printf("PUSH-BIN on %d, %d\n",
			tos - Stack, val);
# endif
}


char	*push_rc(tos, rc, len)
register SYMBOL	*tos;
register char	*rc;
register int	len;
{
# ifdef RG_TRACE
	if (TR_PUSH)
		printf("PUSH-RC\n");
# endif
	push_str(tos, rc, len);
	return (rc + len);
}


push_int(tos, val)
register SYMBOL	*tos;
register int	val;
{
	register union anytype	*any;

	tos->sy_frmt = INT;
	tos->sy_frml = sizeof (short);
	any = &tos->sy_val;
	any->i2type = val;
# ifdef RG_TRACE
	if (TR_PUSH)
		printf("PUSH-INT on %d, %d\n",
			tos - Stack, val);
# endif
}
