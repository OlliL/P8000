/*
** ---  RC_STAB.C ------- SYMBOL TABLE HANDLING  ------------------------------
**
**		SYMENT:		enter a symbol into the symbol table
**
**		FREESYM:	free all entries in the symbol table
**
**	Version: @(#)rc_stab.c		4.0	02/05/89
**
*/

# include	"rc.h"

static struct s_entry		*Symtab;

# ifdef MEM_TRACE
struct s_entry	*talloc(size)
register int	size;
{
	register char			*ptr;
	extern char			*xalloc();

	ptr = xalloc(size);
# ifdef MSDOS
	printf("ALLOC: %Fp\t(%4d)\n", ptr, size);
# else
	printf("ALLOC: %08x (%4d)\n", ptr, size);
# endif
	return ((struct s_entry *) ptr);
}

tfree(ptr)
register char	*ptr;
{
# ifdef MSDOS
	printf("FREE:  %Fp\n", ptr);
# else
	printf("FREE:  %08x\n", ptr);
# endif
	return (free(ptr));
}
# else
# define	talloc		xalloc
# define	tfree		free
# endif

char	*rc_syment(sym, len)
register char	*sym;
register int	len;
{
	register struct s_entry		*newent;
	extern char			*talloc();

	if (len & 01)
		len++;
	if (!(newent = (struct s_entry *) talloc(len + sizeof (struct s_entry *))))
	{
		/* symbol table overflow */
		rc_error(9);
		return ((char *) 0);
	}
	newent->nextsym = Symtab;
	Symtab = newent;
	AAbmove(sym, newent->symbl, len);
# ifdef RC_TRACE
	if (TR_LEX)
	{
# ifdef MSDOS
		printf("SYMTAB: len=%d (%Fp->%Fp)\n\t",
# else
		printf("SYMTAB: len=%d (%08x->%08x)\n\t",
# endif
			len, sym, newent->symbl);
		while (--len >= 0)
		{
			printf("%3o %c\t", *sym, *sym);
			if (!len)
				printf("\n");
			else if (!(len % 8))
				printf("\n\t");
			sym++;
		}
	}
# endif
	return (newent->symbl);
}


rc_freesym()
{
	register struct s_entry		*ent;
	register struct s_entry		*tab;

	tab = Symtab;
	while (tab)
	{
		ent = tab;
		tab = ent->nextsym;
		tfree(ent);
	}
	Symtab = tab;
}
