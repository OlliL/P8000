/*
** ---  RC_GTCHR.C ------ SCANNER I/O  ----------------------------------------
**
**	GTCHAR
**		If 'Lcase' is set, all upper-case alphabetics are
**		mapped into lower-case.
**		The correct return is > 0; a return = 0 indicates end-of-file.
**
**	BACKUP
**		Saves the character argument in the global stack 'Pchar'
**
**	Version: @(#)rc_gtchr.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rc.h"

/* INPUT FILE */
FILE	*Input;

/* TWO CHARACTER STACK FOR 'UNGETC' BACKUP */
static char	Pchar[2];
static int	Pctr;

rc_gtchar()
{
	register int	chr;

	/* USE STACKED CHARACTERS IF POSSIBLE */
	if (Pctr)
		chr = Pchar[--Pctr];
	else
	{
		if ((chr = getc(Input)) <= 0)
			chr = 0;
		else
		{
			if (chr == '\n')
				ungetc(chr = '\r', Input);
			else
			{
				if (chr != '\r')
				{
					if (!Iflag && Sflag)
						putchar(chr);
				}
				else
				{
					chr = '\n';
					if (Sflag)
					{
						if (!Iflag)
							putchar('\n');
						fflush(stdout);
						printf("%4d: ",Line+1);
					}
				}
			}
		}
	}

	/* UPDATE LINE COUNTER */
	if (chr == '\n')
		Line++;

	if (Lcase && ((chr >= 'A'
# ifdef EBCDIC
	   && chr <= 'I') || (chr >= 'J'
	   && chr <= 'R') || (chr >= 'S'
# endif
	   && chr <= 'Z')))
		chr += 'a' - 'A';

# ifdef RC_TRACE
	if (TR_INPUT)
		printf("GTCHAR: ret %c (0%o)\n", chr, chr);
# endif
	return (chr);
}


rc_backup(chr)
register int	chr;
{
	if (Pctr == 2)
	{
		/* "BACKUP: overflow" */
		rc_error(18);
		return;
	}
	Pchar[Pctr++] = chr;
	if (chr == '\n')
		Line--;
}
