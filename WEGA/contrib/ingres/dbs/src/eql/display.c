# include	<stdio.h>
# include	"gen.h"
# include	"../h/bs.h"
# include	"constants.h"
# include	"globals.h"

static struct display	Symsp;		/* storage for symbols read by	    */
					/* yylex() 			    */
static struct display	Displays[1];	/* Cv_display is set to point at    */
					/* this, so that Cv_display may be  */
					/* passed a parameter instead of    */
					/* &Cv_display			    */
struct display		*Cv_display =	&Displays[0];

/*
**  DISPLAY.C -- display manipulation routines
**
**	a display is a list of strings, the symbol
**	space Symsp is impemented by means of displays.
**
**	Defines:
**		enter_display()
**		addsym()
**		free_display()
**		symspfree()
**		w_display()
**		eat_display()
**
**
**
** this avoids having to specify &Cv_display in the semantic
** routines, by making Cv_display a pointer to the pertinent
** display structure.
*/

/*
**  ENTER_DISPLAY -- enter a new string into a display
**
**	Parameters:
**		disp -- display to be added to
**		string -- string to add
**
**	Returns:
**		pointer to new disp_node structure.
**
**	Side Effects:
**		allocates a new disp_node structure.
**		Expects a dynamic "string", i.e. one that it can
**		dispose of as it wishes, so it is the users responsability
**		to allocate space for this string.
**		If the string passed is 0, the string
**		"ERROR_TOKEN" is substituted, and a message is printed,
**		so a caller may call enter_display with the return of an
**		salloc() directly.
**		If no space is available for the allocation of the
**		new node, an error message is given, and a longjump(3)
**		is performed (this should goto eql()[main.c])
*/
struct disp_node	*enter_display(disp, string)
register struct display	*disp;
register char		*string;
{
	register struct disp_node	*node;
	extern char			*nalloc();

	node = (struct disp_node *) nalloc(sizeof *node);
	if (!node)
	{
		yysemerr("symbol space overflow", string);
		reset();
	}
	if (!disp->disp_first)
		disp->disp_first = disp->disp_last = node;
	else
	{
		disp->disp_last->d_next = node;
		disp->disp_last = node;
	}
	node->d_next = 0;
	if (!(node->d_elm = string))
	{
		printf("alloc error in display\n");
		node->d_elm = "ERROR_TOKEN";
	}
	return (node);
}


/*
**  ADDSYM -- add a token to the symbol space
**
**	The node's .d_line field is set to the value of yyline,
**	which, if this routine is called from a lexical routine
**	taking lexemes that can't include newlines (and back them up),
**	is the line the lexeme was read from. This fact is used
**	be yyserror()[yyerror.c] to report accurate line numbers
**	for errors.
**
**	Parameters:
**		s -- string to add
**
**	Returns:
**		pointer to node added
*/
struct disp_node	*addsym(s)
register char		*s;
{
	register struct disp_node	*d;

	d = enter_display(&Symsp, s);
	d->d_line = yyline;
	return (d);
}


/*
**  FREE_DISPLAY -- frees all elements of a display
**
**	Parameters:
**		disp -- the display to free
*/
free_display(disp)
register struct display		*disp;
{
	register struct disp_node	*f;
	register struct disp_node	*n;

	for (f = disp->disp_first; f; f = n)
	{
		n = f->d_next;
		xfree(f->d_elm);
		xfree(f);
	}
	disp->disp_first = disp->disp_last = 0;
}


/*
**  SYSMSPFREE -- Frees symbol space
**	Symspfree frees all the symbol table, EXCEPT
**	for the last element in it, as this is the lookahead
**	element. That is to say, the element which forced reduction
**	to the non-terminal program, which called on symspfree()
*/
symspfree()
{
	register struct display		*d;
	register struct disp_node	*f;
	register struct disp_node	*n;

	d = &Symsp;
	for (f = d->disp_first; f && f->d_next; f = n)
	{
		n = f->d_next;
		xfree(f->d_elm);
		xfree(f);
	}
	d->disp_first = d->disp_last;
}


/*
**  W_DISPLAY -- write out the contents of a display
**
**	Parameters:
**		disp -- display to take write out
**
**	Side Effects:
**		Writes onto Out_file
**		the contents of the display,
**		each string is comsidered a separate
**		word to be written out(so w_raw may
**		break a line between words).
**		Calls are made to w_raw() and w_key() so as
**		to have the minimum number of spaces in the text.
*/
w_display(disp)
register struct display		*disp;
{
	register struct disp_node	*n;
	register int			i;

	for (n = disp->disp_first; n; n = n->d_next)
	{
		i = AAlength(n->d_elm);
		i = n->d_elm[i - 1];
		switch (CMAP(i))
		{
		  case OPATR :
		  case PUNCT :
			w_raw(n->d_elm);
			break;

		  default :
			w_key(n->d_elm);
			break;
		}
	}
}


/*
**  EAT_DISPLAY -- enter text from In_file into a display
**	Enters all text gotten through getch()[getch.c]
**	lying between one character delimiters, which may
**	be nested, into a display or w_raw()'s it.
**	eat_display() assumes that when it is called a
**	"left_ch" has just been seen. It will "eat" up to
**	MAXSTRING characters.
**	Newline is played with because :
**		a) a newline may have been read by eat_display
**		   instead of yylex(), therefore if eat_display
**		   stops there, yylex() must know that a newline
**		   has just been seen and it must test for C_CODE.
**		b) Newline may have to be set to 0 if an include()
**		   was done which sets it to 1, and no yylex() was
**		   done afterwards to reset it.
**
**		NOTE : This playing with Newline is needed because
**		yylex() and not getch() maintains Newline. If getch()
**		maintained Newline then it would be automatically right.
**
**	Parameters:
**		disp -- display to add elements to if != 0 else
**			characters are written out.
**		left_ch -- left delimiter character
**		right_ch -- right delimiter character
**
**	Side Effects:
**		advances input to after the close of the
**		left_ch, right_ch pair.
**		may back up 2 characters.
**
**	Bugs:
**		#include's encountered, are treated wrongly
**		because the "#include ...c.h" is put out immediately,
**		while the display is put out only later.
*/
eat_display(disp, left_ch, right_ch)
struct display	*disp;
char		left_ch;
char		right_ch;
{
	register char	*cp;
	register int	level;
	register int	i;
	register int	pk;
	char		r_c[2];
	char		buf[MAXSTRING + 1];
	extern char	*salloc();

	cp = buf;
	level = i = 1;
	*cp = left_ch;
	do
	{
		i++;
		if (i >= sizeof buf)
		{
			yysemerr("display too long", (char *) 0);
			break;
		}
		if ((*++cp = getch()) == left_ch)
			level++;
		else if (*cp == right_ch)
			level -= 1;
		else if (*cp == EOF_TOK)
		{
			backup(*cp);
missing :
			r_c[1] = '\0';
			r_c[0] = right_ch;
			yysemerr("missing closing character", r_c);
			*cp = right_ch;
			/* make next line be read as possible
			 * C_CODE by yylex()[yylex.c]
			 */
			Newline = 1;
			break;
		}
		else if (*cp == '\n')
		{
			/* test that next line is valid EQL line,
			 * and strip "##"
			 */
			if ((pk = getch()) != '#')
			{
				backup(pk);
				goto missing;
			}
			if ((pk = getch()) != '#')
			{
				backup(pk);
				backup('#');
				goto missing;
			}
		}
	} while (*cp != right_ch || level > 0);
	if (!level)
		Newline = 0;
	*++cp = '\0';
	if (disp)
		enter_display(disp, salloc(buf));
	else
		w_raw(buf);
}
