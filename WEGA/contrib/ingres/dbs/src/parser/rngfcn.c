# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/pipes.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"parser.h"

struct atstash		*Freeatt;		/* free list of attrib stash */

static struct rngtab	Rngtab[MAXVAR + 1];	/* range table */
static struct atstash	Attable[MAXATT];	/* attrib stash space, turned into a list later */

/*
**  RNGINIT
**	initializes the pointers in the range table
**	it should be called prior to starting the parsing
**	it also initializes the attrib stash stuff because
**	the attrib stash is really part of the range table
*/
rnginit()
{
	register int		i;
	register struct atstash	*atptr;
	register struct rngtab	*rptr;

	rptr = Rngtab;				/* ptr to head of range table */
	for (i = 0; i <= MAXVAR; i++, rptr++)	/* ">=" includes the internal entry */
	{
		rptr->ractiv = 0;
		rptr->rmark = 0;
		rptr->rposit = i;
		AAsmove("", rptr->varname);
		AAsmove("", rptr->relnm);
		rptr->rstat = 0;
		rptr->ratts = 0;
		rptr->attlist = (struct atstash *) 0;
	}
	rngreset();
	atptr = Attable;
	for (i = 0; i < MAXATT - 1; i++)
	{
		atptr->atbnext = atptr + 1;
		atptr++;
	}
	atptr->atbnext = (struct atstash *) 0;
	Freeatt = Attable;
}

/*
** RNGLOOK
**	returns a pointer to the range table entry else 0
**	type = LOOKREL	lookup relation
**	type = LOOKVAR	lookup variable
*/
struct rngtab	*rnglook(p, type)
char	*p;
int	type;
{
	register struct rngtab	*rptr;
	register char		*param;
	register int		i;

	param = p;
	rptr = Rngtab;
	switch (type)
	{
	  case LOOKVAR:
	  case LOOKREL:
		break;

	  default:
		AAsyserr(18019, type);
	}
	for (i = 0; i < MAXVAR; i++, rptr++)	/* search external vbles only */
	{
		if (rptr->ractiv == 1 && AAscompare(param, MAXNAME, (type == LOOKVAR ? rptr->varname : rptr->relnm), MAXNAME) == 0)
		{
			rptr->rmark = 1;
#			ifdef	xPTR2
			AAtTfp(21, 6, "fnd '%s' at '%d'\n", param, rptr->rentno);
#			endif
			rngfront(rptr);
			return (rptr);
		}
	}
	return ((struct rngtab *) 0);
}

/*
** RNGENT
**	Insert variable and relation in range table.
*/
struct rngtab	*rngent(type, var, rel, relown, atts, relstat)
int	type;
char	*var;
char	*rel;
char	*relown;
int	atts;
int	relstat;
{
	register struct rngtab	*rptr;
	extern struct rngtab	*rngat();
	extern struct rngtab	*rngatndx();

#	ifdef	xPTR2
	AAtTfp(21, 0, "RNGENT:\ttyp=%d\tvar=%s\trel=%s\n\town=%.2s\tatts=%d\tstat=0%o\n",
		type, var, rel, relown, atts, relstat);
#	endif
	if (type == R_INTERNAL)
		rptr = &Rngtab[MAXVAR];
	else if (type == R_EXTERNAL)
	{
		if (!(rptr = rnglook(var, LOOKVAR)))
		{
			/* not in range table */
			rptr = rngatndx(MAXVAR - 1);
		}
		rngfront(rptr);
	}
	else
		AAsyserr(18020, type);
	if (AAscompare(rel, MAXNAME, rptr->relnm, MAXNAME) != 0 ||
		!AAbequal(relown, rptr->relnowner, 2))
	{
		attfree(rptr->attlist);
		rptr->attlist = (struct atstash *) 0;
	}

	AApmove(var, rptr->varname, MAXNAME, '\0');
	AApmove(rel, rptr->relnm, MAXNAME, '\0');
	AAbmove(relown, rptr->relnowner, 2);
	rptr->ractiv = 1;
	rptr->ratts = atts;
	rptr->rstat = relstat;
	return (rptr);
}

/*
** RNGDEL
**	removes an entry from the range table
**	removes all variables for the relation name
*/
rngdel(rel)
char	*rel;
{
	register struct rngtab	*rptr;

	while ((rptr = rnglook(rel, LOOKREL)))
	{
		AAsmove("", rptr->relnm);
		AAsmove("", rptr->varname);
		rptr->rmark = 0;
		rptr->ractiv = 0;
		rngback(rptr);
		attfree(rptr->attlist);
		rptr->attlist = (struct atstash *) 0;
	}
}

/*
** RNGSEND
**	Writes range table information needed by DECOMP and OVQP.
*/
rngsend()
{
	register struct rngtab	*rptr;
	register int		i;

	rptr = Rngtab;
	for (i = 0; i < MAXVAR; i++, rptr++)
	{
		if (rptr->rmark != 0)
		{
			rngwrite(rptr);
		}
	}

	/* send result relation range entry if not already sent */
	if (Resrng && Resrng->rentno == MAXVAR)
	{
		rngwrite(Resrng);
	}
}

/*
** RNGFRONT
**	move entry 'r' to head of range table list
*/
rngfront(r)
struct rngtab	*r;
{
	register struct rngtab	*rptr;
	register struct rngtab	*fptr;
	register int		i;

	fptr = r;
	rptr = Rngtab;
	for (i = 0; i < MAXVAR; i++, rptr++)	/* check all external vbles */
	{
		if (rptr->rposit < fptr->rposit)
			rptr->rposit++;
	}
	fptr->rposit = 0;
}

/*
** RNGBACK
**	move entry 'r' to back of range table list
*/
rngback(r)
struct rngtab	*r;
{
	register struct rngtab	*rptr;
	register struct rngtab	*bptr;
	register int		i;

	bptr = r;
	rptr = Rngtab;
	for (i = 0; i < MAXVAR; i++, rptr++)	/* check all external vbles */
	{
		if (rptr->rposit > bptr->rposit)
			rptr->rposit--;
	}
	bptr->rposit = MAXVAR - 1;
}

/*
** RNGRESET
**	reset the used marks to '0'
*/
rngreset()
{
	register struct rngtab	*rptr;
	register int		i;

	rptr = Rngtab;
	for (i = MAXVAR - 1; i >= 0; i--, rptr++)	/* only do external ones */
	{
		rptr->rmark = 0;
		rptr->rentno = i;
	}
	Rngtab[MAXVAR].rentno = MAXVAR;		/* internal vble is always MAXVAR */
}

/*
**  RNGOLD -- find least recently used vble entry
*/
struct rngtab	*rngatndx(ndx)
int	ndx;
{
	register struct rngtab	*rptr;
	register int		i;

	rptr = Rngtab;
	for (i = 0; i < MAXVAR; i++, rptr++)	/* do external vbles */
	{
		if (rptr->rposit == ndx)
			return (rptr);
	}
	AAsyserr(18021, ndx);
}

/*
** CHECKUPD
**	checks to make sure that the user can update the relation 'name1'
**	the 'open' parameter is set if 'Reldesc' contains the AArelopen info
**	for the relation in question.
*/
checkupd(rptr)
struct rngtab	*rptr;
{
	if (!Noupdt)
		return;
	if (rptr->rstat & S_NOUPDT)
		/* no updates allowed on this relation */
		yyerror(CANTUPDATE, rptr->relnm, (char *) 0);
}

/*
** RNGFRESH -- check the range table relstat information for accuracy
**
**	If the command specified could have changed the relstat info
**	make the appropriate adjustments to the range table
*/
rngfresh(op)
int	op;
{
	register struct rngtab	*rptr;
	register int		i;
	DESC			desc;

	/* search the entire table! */
	for (i = 0, rptr = Rngtab; i <= MAXVAR; i++, rptr++)
	{
		if (!rptr->ractiv)
			continue;
		switch (op)
		{
		  case mdDESTROY:
		  case mdREMQM:
			if ((rptr->rstat & (S_VBASE | S_INTEG | S_PROTUPS | S_INDEX)) != 0)
			{
			fixordel:
				/*
				** AArelopen the relation, if it doesn't exist make
				** sure that all range table entries are gone
				*/
				if (!AArelopen(&desc, -1, rptr->relnm))
				{
					/* relation still there, copy bits anyway */
					rptr->rstat = desc.reldum.relstat;
				}
				else
				{
					/* relation not there, purge table */
					rngdel(rptr->relnm);
				}
			}
			break;

		  case mdVIEW:
			if ((rptr->rstat & S_VBASE) == 0)
			{
			fixorerr:
				/*
				** if the relation doesn't exist then it is
				** a AAsyserr, otherwise, copy the bits.
				*/
				if (!AArelopen(&desc, -1, rptr->relnm))
				{
					/* exists, copy bits */
					rptr->rstat = desc.reldum.relstat;
				}
				else	/* not there, AAsyserr */
					AAsyserr(18022, rptr->relnm);
			}
			break;

		  case mdPROT:
			if ((rptr->rstat & S_PROTUPS) == 0)
				goto	fixorerr;
			break;

		  case mdINTEG:
			if ((rptr->rstat & S_INTEG) == 0)
				goto	fixorerr;
			break;

		  case mdMODIFY:
			if ((rptr->rstat & S_INDEX) != 0)
				goto	fixordel;
			break;

		  default:
			return;	/* command ok, dont waste time on rest of table */
		}
	}
}

/*
**  RNGPRINT
**	prints the contents of the range table
*/
rngprint()
{
	register short		i;
	register short		j;
	register struct rngtab	*rptr;

	printf("    relation\tvariable\n");
	printf("----------------------------\n");

	for (i = 0; i < MAXVAR; i++)
	{
		rptr = Rngtab;
		for (j = 0; j < MAXVAR; j++, rptr++)
		{
			if (!rptr->ractiv || i != rptr->rposit)
				continue;
			printf("%12s\t%.12s\n", rptr->relnm, rptr->varname);
		}
	}
	printf("----------------------------\n");
}
