# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/symbol.h"
# include	"../h/tree.h"
# include	"../h/pipes.h"
# include	"ovqp.h"
# include	"strategy.h"


/*
**	Exactkey checks to see if the relation described
**	by "ap" can be used in a hashed scan.
**	All the key domains of the relation must
**	have simple clauses of equality associated
**	with them in the qualification.
**
**	Returns 0 if the relation can't be used.
**
**	Returns > 0 if it can.
*/

exactkey(ap, key)
struct accessparam	*ap;
struct  key		*key;
{
	register struct accessparam	*a;
	register struct key		*k;
	register struct simp		*s;
	int				d;
	int				i;
	int				j;

#	ifdef xOTR1
	AAtTfp(25, -1, "EXACTKEY\n");
#	endif

	a = ap;
	k = key;
	i = 0;
	if (a->mode == EXACTKEY)
	{

		for (i = 0; d = a->keydno[i]; i++)
		{

			s = Simp;
			for (j = 0; j < Nsimp; j++)
			{
				if (s->relop == (int) opEQ && s->att == d)
				{
					k->keysym = s->konst;
					k->dnumber = (a->sec_index == TRUE) ? i+1 : d;
					k++;
#					ifdef xOTR1
					if (AAtTfp(25, 1, "exact key on dom %d\tvalue=", d))
						prsym(s->konst);
#					endif
					break;
				}
				s++;
			}
			if (j == Nsimp)
			{
				i = 0;	/* failure. at lease one key isn't used */
				break;
			}
		}
		k->dnumber = 0;	/* mark end of list */
	}
#	ifdef xOTR1
	AAtTfp(25, 9, "EXACTKEY: rets %d\n", i);
#	endif
	return (i);
}


/*
**	Range key checks if the relation described by
**	"ap" is ISAM and there are simple clauses
**	on the first key and any additional keys.
**
**	Rangekey accumulates both high and low keys,
**	which are not necessary the same. If it
**	every finds a high or a low key on the first
**	domain of the relation then success=TRUE.
**
**	Returns  1 if Rangekey ok
**		 0 if Rangekey is not ok
**		-1 if Rangekey ok and all clauses are equality clauses
*/

rangekey(ap, l, h)
struct accessparam	*ap;
struct key		*l;
struct key		*h;
{
	register struct key	*low;
	register struct key	*high;
	register struct simp	*s;
	struct accessparam	*a;
	int			sec_indx;
	int			d;
	int			i;
	int			rel;
	int			success;
	int			ns;
	int			lowkey;
	int			allexact;

#	ifdef xOTR1
	AAtTfp(25, 5, "RANGEKEY\n");
#	endif

	a = ap;
	sec_indx  = a->sec_index == TRUE;
	low = l;
	high = h;
	allexact = -1;	/* assume all clauses equality clauses */
	s = Simp;
	success = FALSE;
	if (a->mode == LRANGEKEY)
	{

		for (ns = 0; ns < Nsimp; ns++)
		{
			rel = s->relop;
			for (i = 0; d = a->keydno[i]; i++)
			{
				if (d == s->att)
				{
					/* this is either a high range value or low range value */
					lowkey = (rel == (int) opGTGE);
					if (lowkey || rel == (int) opEQ)
					{
						/* low range key */
#						ifdef xOTR1
						AAtTfp(25, 6, "low key on dom %d\t", d);
#						endif
						low->keysym = s->konst;
						low->dnumber = sec_indx ? i+1 : d;
						low++;
					}
					if (!lowkey || rel == (int) opEQ)
					{
						/* high range key */
#						ifdef xOTR1
						AAtTfp(25, 6, "high key on dom %d\t", d);
#						endif
						high->keysym = s->konst;
						high->dnumber = sec_indx ? i+1 : d;
						high++;
					}
#					ifdef xOTR1
					if (AAtTf(25, 6))
						prsym(s->konst);
#					endif
					if (i == 0)
						success = TRUE;
					if (rel != (int) opEQ)
						allexact = 1;	/* at least one inequality */
					break;
				}
			}
			s++;	/* try next simple clause */
		}
	}

	high->dnumber = 0;	/* mark end of list */
	low->dnumber = 0;	/* mask end of list */

	/* if success then return whether all clauses were equality */
	if (success)
		success = allexact;

#	ifdef xOTR1
	AAtTfp(25, 5, "RANGEKEY: rets %d\n", success);
#	endif
	return (success);
}


setallkey(relkey, keytuple)
struct key	*relkey;
char		*keytuple;

/*
**	Setallkey takes a key struct, decodes it and
**	calls setkey with each value.
**
**	Called from strategy().
**
**	returns 0 if ok.
**	returns -1 in the special case of a deblanked hashkey
**	being bigger than the corresponding domain.
*/
{
	register struct key	*k;
	register SYMBOL		*sy;
	register int		dnum;
	SYMBOL			**s;
	char			*p;
	char			temp[MAXFIELD + 1];
	int			l;

	AAam_clearkeys(Scanr);
	k = relkey;
	while (dnum = k->dnumber)
	{
		s = &k->keysym;
		sy = (SYMBOL *) Stack;
		getsymbol(sy, &s);	/* copy symbol to stack. caution:getsym changes the value of s. */
		rcvt(sy, Scanr->relfrmt[dnum], Scanr->relfrml[dnum]);	/* convert key to correct type */
		p = (char *) (sy->value);

		if (sy->type == CHAR)
		{
			/*
			** The length of a character key must
			** be made equal to the domain length.
			** The key is copied to a temp place
			** and a null byte is inserted at the
			** end. In addition, if the key without
			** blanks is longer than the domain and
			** this is an exactkey, then the query
			** is false.
			*/
			p = temp;
			l = cmove(sy, p);	/* copy symbol to temp removing blanks & nulls */
#			ifdef xOTR1
			AAtTfp(26, 9, "length is %d\n", l);
#			endif
			if (Fmode == EXACTKEY && l > ctou(Scanr->relfrml[dnum]))
				/* key too large. qualification is false */
				return (-1);
		}
		if (sy->type == INT && sy->len == sizeof (char))
		{
			*temp = ((I2TYPE *) p)->i2type;
			p = temp;
		}
		AAam_setkey(Scanr, keytuple, p, dnum);	/* set the key */
		k++;
	}
#	ifdef xOTR1
	if (AAtTf(26, 8))
		AAprkey(Scanr, keytuple);
#	endif
	return (0);
}


cmove(sym, dest)
struct stacksym	*sym;
char		*dest;

/*
**	Cmove copies a char symbol into "dest".
**	It stops when the length is reached or
**	when a null byte is found.
**
**	returns the number of non-blank chars
**	in the string.
*/
{
	register char	*d;
	register char	*s;
	register int	l;
	int		blank;

	s = (char *) ((SYMBOL *) sym)->value;
	s = ((STRINGP *) s)->stringp;		/* s points to the char string */
	d = dest;
	blank = 0;

	for (l = (ctou(((SYMBOL *) sym)->len)); l--; s++)
	{
		*d++ = *s;
		if (*s == ' ')
			blank++;
		if (*s == '\0')
		{
			d--;
			break;
		}
	}

	*d = '\0';
	return ((d - dest) - blank);	/* return length of string */
}

indexcheck()

/*
**	Indexcheck is called by scan() to check whether
**	a secondary index tuple satisfies the simple
**	clauses under which it was scanned.
**
**	Returns 1 if the tuple is ok,
**		0 otherwise.
*/
{
	register int	i;

	if (Fmode == EXACTKEY)
		i = keycheck(Lkey_struct, Keyl, 0);	/* check for equality */
	else
	{
		i = keycheck(Lkey_struct, Keyl, 1);	/* check for >= */
		/* If the lowkey passed, check the highkey also */
		if (i)
			i = keycheck(Hkey_struct, Keyh, -1);	/* check for <= */
	}
#	ifdef xOTR1
	AAtTfp(26, 10, "INDEXCHECK: rets %d\n", i);
#	endif
	return (i);
}

keycheck(keys, keytuple, mode)
struct key	*keys;
char		*keytuple;
int		mode;

/*
**	Keycheck compares Intup with keytuple
**	according to the domains specified in the
**	"keys" struct.
**
**	mode is either >0, =0, <0 depending on
**	whether check is for Intup >= keytuple,
**	Intup == keytuple, Intup <= keytuple respectively
**
**	returns TRUE or FALSE accordingly.
*/
{
	register struct key	*k;
	register char		*kp;
	register int		dnum;
	register int		offset;
	register int		i;
	register int		success;

	kp = keytuple;
	success = TRUE;

	for (k = keys; dnum = k->dnumber; k++)
	{

		offset = Scanr->reloff[dnum];
		if (i = AAicompare(&Intup[offset], &kp[offset], Scanr->relfrmt[dnum], ctou(Scanr->relfrml[dnum])))
		{
			if (i < 0 && mode < 0 || i > 0 && mode > 0)
				continue;
			success = FALSE;
			break;
		}
	}
	return (success);
}
