# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"decomp.h"

# define	FIRSTNUM	MAXRANGE + 3
# define	LASTNUM		100

/*
**	Internal numbers are used in decomp to
**	represent relation names. The numbers
**	from 0 to FIRSTNUM-1 refer to the names
**	stored in Name_table[].
**
**	The number from FIRSTNUM to LASTNUM have
**	names which are computed from aa, ab, etc.
*/
static char	Name_table[FIRSTNUM - 1][MAXNAME];
static char	Num_used[LASTNUM + 1];
char		SYSNAME[]	= "_SYS";
int		s_SYSNAME	= sizeof SYSNAME - 1;


rnum_assign(name)
char	*name;

/*
**	Assign an internal number rnum to name.
*/

{
	register int	i;

	for (i = 0; i < FIRSTNUM; i++)
		if (Num_used[i] == 0)
		{
			AAbmove(name, Name_table[i], MAXNAME);
			Num_used[i]++;
			return (i);
		}
	AAsyserr(12022);
}

rnum_alloc()

/*
**	Allocate the next available name
*/

{
	register int	i;
	register char	*cp;

	cp = &Num_used[FIRSTNUM];
	for (i = FIRSTNUM; i < LASTNUM; i++)
		if (*cp++ == 0)
		{
			--cp;
			(*cp)++;
			return (i);
		}
	AAsyserr(12023);
}


/*
**	Convert internal relation number
**	to its real name. Guarantee '\0' at end.
*/

# ifdef EBCDIC
static char	Rnum[] = "abcdefghijklmnopqrstuvwxyz";
# endif

char	*rnum_convert(num)
int	num;
{
	register int	i;
	register char	*ret;
	register char	*cp;
	static char	temp[MAXNAME+1];
	extern char	*Fileset;
	extern char	*AAconcat();

	i = num;
	if (i > LASTNUM || Num_used[i] == 0)
		AAsyserr(12024, i);

	ret = temp;

	if (i < FIRSTNUM)
		AAbmove(Name_table[i], ret, MAXNAME);
	else
	{
		/* compute temp name */
		cp = AAconcat(SYSNAME, Fileset, ret);
		AApad(ret, MAXNAME);
		i -= FIRSTNUM;
# ifdef EBCDIC
		*cp++ = Rnum[i / 26];
		*cp = Rnum[i % 26];
# else
		*cp++ = i / 26 + 'a';
		*cp = i % 26 + 'a';
# endif
	}
	return (ret);
}

rnum_remove(num)
int	num;

/*
**	Remove a num from the used list
*/

{
	register char	*cp;

	cp = &Num_used[num];

	if (*cp == 0)
		AAsyserr(12025, num);
	*cp = 0;
}


rnum_last()

/*
**	returns number of largest assigned temp number.
**	zero if none
*/

{
	register int	i;

	for (i = LASTNUM; i >= FIRSTNUM; i--)
		if (Num_used[i])
			return (i);

	return (0);
}


rnum_temp(rnum)
int	rnum;

/*
**	Predicate to check whether rnum is a temporary relation or not
*/

{
	register int	i;

	i = rnum;

	return (i >= FIRSTNUM || AAbequal(rnum_convert(i), SYSNAME, s_SYSNAME));
}

rnum_init()

/*
**	Clear tag fields from previous query
*/

{
	register char	*cp;
	register int	i;

	cp = Num_used;
	i = FIRSTNUM;
	while (--i)
		*cp++ = 0;
}
