# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

static struct ret_list	Ret_list;	/* list of variables in a "ctlelm"  */

/*
**  RETRIEVE.C -- ret_list struct handlers
**
**	The Ret_list is a structure where references to C variables,
**	and their types, used in a target list of a "tupret" are kept
**	for outputting in the while loop generated.
**
**	Defines:
**		enter_ret()
**		add_ret()
**		w_ret()
**		free_ret()
**
**
**
**  ENTER_RET -- enter a variable in a ret_list
**	Concatenatest the strings in disp the calls add_ret
**	to add the new string to Ret_list.
**
**	Parameters:
**		disp -- display containing the reference to the variable
**		type -- type of the variable
*/
enter_ret(disp, type)
struct display		*disp;
int			type;
{
	register char			*srce;
	register char			*dest;
	register struct disp_node	*d;
	register int			i;
	char				buf[MAXSTRING + 2];
	extern char		*salloc();

	i = 0;
	dest = buf;
	for (d = disp->disp_first; d; d = d->d_next)
	{
		for (srce = d->d_elm; *srce; )
		{
			if (i < MAXSTRING)
			{
				i += 1;
				*++dest = *srce++;
			}
			else
				break;
		}

		if (i >= MAXSTRING)
		{
			yysemerr("reference to a variable too long, ']' probably missing from array subscription", (char *) 0);
			break;
		}
	}
	*++dest = '\0';
	add_ret(salloc(&buf[1]), type);
}


/*
**  ADD_RET -- add a string (reference to a variable) to the Ret_list
**
**	Parameters:
**		s -- string to add
**		type -- type of variable being added
*/
add_ret(s, type)
char		*s;
int		type;
{
	register struct ret_list	*list;
	register struct ret_var		*node;
	extern char			*nalloc();

	if (!s)
	{
		s = "ERROR_TOKEN";
		yysemerr("alloc error", s);
	}
	list = &Ret_list;
	node = (struct ret_var *) nalloc(sizeof *node);
	if (!node)
	{
		yysemerr("alloc error", s);
		xfree(s);
		return;
	}
	node->r_elm = s;
	node->r_next = 0;
	node->r_type = type;
	if (!list->ret_first)
		list->ret_first = list->ret_last = node;
	else
	{
		list->ret_last->r_next = node;
		list->ret_last = node;
	}
}


/*
**  W_RET -- Generates the IIn_get() calls for the Ret_list
**
**	Any variable whose type is not string gets an '&'
**	(adress of) operand prepended.
*/
w_ret()
{
	register struct ret_var	*node;
	char			type[3];

	for (node = Ret_list.ret_first; node; node = node->r_next)
	{
		w_raw("IIn_ret(");
		if (node->r_type != opSTRING)
			w_raw("&");
		w_raw(node->r_elm);
		w_raw(", ");
		AA_itoa(node->r_type, type);
		w_raw(type);
		w_raw(");");
	}
}


/*
**  FRE_RET -- Free up the storage used by the Ret_list
*/
free_ret()
{
	register struct ret_list	*list;
	register struct ret_var		*n;
	register struct ret_var		*f;

	list = &Ret_list;
	for (f = list->ret_first; f; f = n)
	{
		n = f->r_next;
		xfree(f->r_elm);
		xfree(f);
	}
	list->ret_first = list->ret_last = 0;
}
