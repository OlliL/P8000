/*
** ---  RG_REP.C -------- REPORT OUTPUT  --------------------------------------
**
**	Version: @(#)rg_rep.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg_file.h"

static char	ibuf[MAX_FIELD];
static char	obuf[BUFSIZ];
static char	*bp = obuf;
static char	PROMPT[] = "> \007";


rg_report(b, olen)
register char	*b;
int		olen;
{
	register int	len;
	register char	*p;
# ifdef RG_TRACE
	register int	c;

	if (TR_REPORT)
	{
		printf("REPORT RUN %d\tlen=%d, `", RUNoutput, len = olen);
		for (p = b; len; --len)
		{
			if ((c = *p++) == '\n')
				printf("\\n");
			else
				putchar(c);
			if (c == ' ')
			{
				while (*p == ' ')
				{
					p++;
					--len;
				}
			}
		}
		printf("'\n");
	}
# endif

	if (RUNoutput)
		return;		/* nothing to print */

	if ((len = &obuf[BUFSIZ] - (p = bp)) > olen)
		len = olen;
	olen -= len;

	while (--len >= 0)
		*p++ = *b++;

	bp = p;
	if (olen)
	{
		rg_flush();		/* flush the buffer  */
		rg_report(b, olen);	/* write the rest    */
	}
}


rg_flush()
{
	register FILETYPE	*f;
	register int		len;
	extern int		errno;

	len = bp - obuf;

# ifdef RG_TRACE
	if (TR_REPORT)
		printf("FLUSH\tlen=%d\n", len);
# endif

	if (!len)
		goto done;	/* nothing to print */

	if (!(f = &Rg_file[F_REPORT - 1])->rf_open)
	{	/* report not open, what can we do ? */
		rg_error("REPORT NOT OPEN");
		rg_exit(EXIT_OPEN);
	}

	errno = 0;
	if (write(f->rf_fd, obuf, len) != len)
	{
		rg_error("REPORT WRITE ERROR");
		rg_exit(EXIT_WRITE);
	}
done:
	bp = obuf;
}


rg_input(output, input)
char	*output;
char	**input;
{
	register char	*b;
	register int	len;
	register int	c;

	write(2, output, R_val);
	write(2, PROMPT, sizeof PROMPT - 1);

	*input = b = ibuf;
	len = 0;

	/* no test for ibuf overflowe werde made */
	while ((c = getchar()) > 0)
	{
		if (c == '\n')
			break;
		*b++ = c;
		len++;
	}

	return (len);
}
