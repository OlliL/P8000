/*
** ---  RG_BATCH.C ------ BATCH LINE PROCESSOR  -------------------------------
**
**	Version: @(#)rg_batch.c		4.0	02/05/89
**
*/

# include	<stdio.h>
# include	"rg.h"

/*------------------------------------------------------------------*/
/*								    */
/*	 BATCH MODULE	 (very hidden)				    */
/*								    */
/*	- to enter a new batch,	use RG_BATCH()			    */
/*	- to modify the	batch count, use BATCH_COUNT()		    */
/*	- BATCH_LINE() ist the BATCH PROCESSOR,			    */
/*		called from NEW-LINE()				    */
/*	- DO_BATCH() batches one entity,			    */
/*		called from RG_BATCH() and BATCH_LINE()		    */
/*	- EXCEPTION() is the executer for all  ON ... DO,	    */
/*		which depends on batch handling			    */
/*								    */
/*------------------------------------------------------------------*/


# define	MAX_BATCH		32

struct batch_t
{
	int		out_len;	/* wanted output length     */
	int		bat_len;	/* batch length             */
	int		bat_col;	/* position of batch        */
	int		str_len;	/* (rest) length of string  */
	char		*str_adr;	/* address of (rest) string */
};

static struct	batch_t	Btab[MAX_BATCH];
static struct	batch_t	*Bstart = Btab;
static struct	batch_t	*Bfree = Btab;
static int		Bcount;
static BOOL		RUNbatch;		/* = FALSE */

# ifdef	RG_TRACE
static int		E_level;	/* level of exception	    */
# endif


rg_batch(olen, blen, slen, sadr)
register int	olen;
register int	blen;
int		slen;
char		*sadr;
{
	register struct	batch_t		*b;
# ifdef RG_TRACE
	register int			i;
	register char			*c;

	if (TR_BATCH)
	{
# ifdef MSDOS
		printf("RG-BATCH: olen=%d, blen=%d, slen=%d, sadr=%Fp #",
# else
		printf("RG-BATCH: olen=%d, blen=%d, slen=%d, sadr=%08x #",
# endif
			olen, blen, i = slen, c = sadr);
		while (--i >= 0)
		{
			putchar(*c);
			c++;
		}
		printf("#\n");
	}
# endif

	if (!olen)
		/* nothing to batch */
		return;

	if ((b = Bfree++) == &Btab[MAX_BATCH])
	{
		rg_error("BATCH TABLE OVERFLOW");
		rg_exit(EXIT_R_CODE);
	}

	b->out_len = olen;
	if (blen <= A_BATCH)
		b->bat_len = olen;
	else
		b->bat_len = blen;
	b->bat_col = SYS_col - Col_begin;
	b->str_len = slen;
	b->str_adr = sadr;

	if (do_batch(b))
		--Bfree;
	else
		Bcount++;
}


batch_count(count)
register int	count;
{
	register int	i;

	i = Bcount; Bcount = count;
	return (i);
}


batch_line()
{
	register struct	batch_t		*b;
	register char			*bl;
	extern char			*AApad();

# ifdef RG_TRACE
	if (TR_BATCH)
		printf("BATCH\t%d\n", SYS_line);
# endif

	while (Bcount)
	{
# ifdef RG_TRACE
		if (TR_BATCH)
			printf("BATCH\tBcount=%d\n", Bcount);
# endif

		AApad(SYS_col, MAX_LINE);
		bl = Col_end;

		if (ONbatch)
		{
			Col_end = &Line[MAX_LINE];
			RUNline++;
			exception(&RUNbatch, ONbatch, FALSE);
			RUNline = 0;
			Col_end = bl;
			bl = SYS_col;
		}
		for (b = Bstart; b < Bfree; b++)
		{
			if (!b->out_len)
				continue;

			if (do_batch(b))
				--Bcount;

			if (SYS_col > &Line[MAX_LINE])
			{
				/* line	overflow, throw	away the rest	*/
				Bcount = 0;
				break;
			}
		}

		if (SYS_col < bl)
			SYS_col = bl;

		check_line();
		new_line();
	}

	Bfree =	Bstart;
	/* Bcount = 0 */
}


static do_batch(b)
register struct batch_t	*b;
{
	register char		*p;
	register char		*col;
	register int		len;
	extern char		*AAbmove();
	extern char		*AApad();

# ifdef RG_TRACE
	if (TR_BATCH)
	{
# ifdef MSDOS
		printf("DO-BATCH (%Fp) olen=%d blen=%d bcol=%d\n",
# else
		printf("DO-BATCH (%08x) olen=%d blen=%d bcol=%d\n",
# endif
			b, b->out_len, b->bat_len, b->bat_col);
# ifdef MSDOS
		printf("\tslen=%d sadr=%Fp #",
# else
		printf("\tslen=%d sadr=%08x #",
# endif
			len = b->str_len, p = b->str_adr);
		while (--len >= 0)
		{
			putchar(*p);
			p++;
		}
		printf("#\n");
	}
# endif

	p = b->str_adr;
	len = b->bat_len;
	col = Col_begin + b->bat_col;
	SYS_col = col + len;

	if (len	> b->out_len)
		len = b->out_len;

	b->out_len -= len;

	if (len	>= b->str_len)
		len = b->str_len;
	else
		len = seperate(p, len);

	col = AAbmove(p, col, len);
	p += len;
	AApad(col, b->bat_len - len);

	if (len	< b->str_len)
		/* skip	to next	word */
		while (*p == ' ')
			p++;

	b->str_len -= p - b->str_adr;
	b->str_adr = p;

	if (!b->out_len)
		return (TRUE);
	return (FALSE);
}


exception (run, on, nl)
BOOL		*run;
char		*on;
register int	nl;	/* if line output must finished */
			/* for ON PAGE exceptions       */
{
	struct batch_t			*start;
	register struct batch_t		*free;
	int				count;

# ifdef RG_TRACE
	if (TR_EXCEPTION)
	{
		E_level++;
		printf("<-%d->\t%d, nl=%d\n", E_level, on - R_code, nl);
	}
# endif

	/* save batch inf */
	start = Bstart;
	Bstart = free = Bfree;
	count = Bcount; Bcount = 0;

	/* do exception */
	(*run)++;
	R_rc = on;
	rg_interp();
	RUN = *run = 0;

	/* check for incomplete line */
	if (nl && SYS_col != Col_begin)
		new_line();

	/* restore batch inf */
	Bstart = start;
	Bfree =	free;
	Bcount = count;

# ifdef RG_TRACE
	if (TR_EXCEPTION)
		--E_level;
# endif
}
