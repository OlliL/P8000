# include	"amx.h"

/*
**	SYMBOL TABLE HANDLING
**
**		SYMENT:		enter a symbol into the symbol table
*/
char	*syment(sym, len)
register char	*sym;
register int	len;
{
	register struct s_entry	*newent;
	extern char		*talloc();
	extern char		*AAbmove();

	len = ALIGNMENT(len);
	newent = (struct s_entry *) talloc(len + ALIGNMENT(sizeof (struct s_entry *)));
	if (!newent)
		yyexit(15, sym);
	newent->nextsym = Symtab;
	Symtab = newent;
	AAbmove(sym, newent->symbl, len);
	return ((char *) newent->symbl);
}


/*
**		SYM_FREE:	free last entry in the symbol table
*/
sym_free()
{
	register struct s_entry	*ent;

	Symtab = (ent = Symtab)->nextsym;
	tfree(ent);
}


/*
**		FREE_SYM:	free all entries in the symbol table
*/
free_sym()
{
	register struct s_entry	*sent;
	register struct s_entry	*stab;

	stab = Symtab;
	while (stab)
	{
		sent = stab;
		stab = stab->nextsym;
		tfree(sent);
	}
	Symtab = (struct s_entry *) 0;

	if (File_src)
	{
		tfree(File_src);
		File_src = (char *) 0;
	}
}


AMX_REL	*getrel(uniq, which)
register int	uniq;
int		which;	/* which marks the relation buffer:	*/
			/* == 0: temp relation buffer		*/
			/* == 1: actual relation buffer		*/
			/* == 2: scan relation buffer		*/
{
	register AMX_REL	*t;
	register int		i;
	register AMX_REL	*relbuffer;
	extern int		read();
	extern char		*AAbmove();

	/* check if nothing to do */
	if (uniq >= Relcount)
	{
		relbuffer = (AMX_REL *) 0;
		goto in_core;
	}
	relbuffer = &Temp_rel[which];
	if (relbuffer->rel_uniq == uniq)
		goto in_core;

	/* search if relation `uniq' is already in core */
	for (i = 0, t = Temp_rel; i < 3; i++, t++)
		if (t->rel_uniq == uniq)
			break;

	if (i == 3)
	{
		/* not found, get relation `uniq' into core */
		relbuffer->rel_uniq = uniq;
		swaprel(relbuffer, read);
	}
	else	/* i != which */
	{
		/* found, copy relation into right relation buffer */
		AAbmove(t, relbuffer, sizeof (AMX_REL));
	}
in_core:
	return (relbuffer);
}


swaprel(r, rd_wr_func)
register AMX_REL	*r;
int			(*rd_wr_func)();	/* read/write function */
{
	register int		i;
	register AMX_REL	*t;
	long			reloff;
	extern int		errno;
	extern long		lseek();
	extern char		*AAbmove();

	errno = 0;
	reloff = r->rel_uniq * sizeof (AMX_REL);
	if (lseek(FILE_rel, reloff, 0) != reloff)
		goto error;
	if ((*rd_wr_func)(FILE_rel, r, sizeof (AMX_REL)) != sizeof (AMX_REL))
	{
error:
		printf("SWAP ERROR <%d> rel %s\n", errno, r->rel_id);
		abort_amx();
	}

	if (rd_wr_func == read)
		return;

	for (i = 0, t = Temp_rel; i < 3; i++, t++)
	{
		if (t == r)
			continue;
		if (t->rel_uniq == r->rel_uniq)
			AAbmove(r, t, sizeof (AMX_REL));
	}
}
