# include	<errno.h>
# include	"form.h"

struct MM_tree
{
	struct MM_tree	*t_other;
	struct MM_tree	*t_next;
	short		t_char;
	short		t_code;
};
static struct MM_tree	*_Tree;

# define	PEEKS		10
static int		_Peek_ch[PEEKS];
static int		_Peek_ptr;

static int	_translate(key_string)
char				**key_string;
{
	register char			*k;
	register int			key_char;
	register int			c;

	k = *key_string;
	key_char = *k++;
	if (key_char == '\\')
	{
		key_char = 0;
		while ((c = *k++) >= '0' && c <= '7')
			key_char = (key_char << 3) | (c &= 7);
	}
	else
	{
		k++;
		key_char &= 0377;
	}
	*key_string = k;

	return ((key_char == '\t')? 0: key_char);
}


MM_incode(key_string, key_code)
char				*key_string;
int				key_code;
{
	register struct MM_tree		*tree;
	register struct MM_tree		*t;
	register int			key_char;
	register struct MM_tree		*prev;
	extern char			*calloc();

	prev = t = (struct MM_tree *) 0;
	tree = _Tree;
	while (key_char = _translate(&key_string))
	{
		if (!tree)
		{
new_node:
			if (!(t = (struct MM_tree *) calloc(1, sizeof (struct MM_tree))))
				return (0);

			t->t_char = key_char;
			if (tree)
				tree->t_other = t;
			else if (prev)
				prev->t_next = t;
			else
				_Tree = t;
			prev = t;
			tree = (struct MM_tree *) 0;
			continue;
		}
other_node:
		if (tree->t_char == key_char)
		{
			if (tree->t_code)
				return (0);
			tree = tree->t_next;
			continue;
		}
		if (t = tree->t_other)
		{
			tree = t;
			goto other_node;
		}
		else
			goto new_node;
	}
	if (!t || t->t_code)
		return (0);
	t->t_code = key_code;

	return (1);
}


MM_outcode(key_string, key_code)
char				*key_string;
int				key_code;
{
	register int			len;
	register char			*ka;
	register char			*ke;
	extern char			*malloc();

	ka = ke = key_string;
	len = 0;
	while (*ke++ = _translate(&key_string))
		len++;
	MM_cs[key_code] = ke = malloc(len + 1);
	MMsmove(ka, ke);
	if (key_code == CS_CM)
		MM_cs_cm = _translate(&key_string);

	return (1);
}


MM_backup(character)
register int			character;
{
	_Peek_ch[_Peek_ptr++] = character;

	return (1);
}


static int	_getchar()
{
	register int			ret_ch;
	char				ch;
	extern int			errno;

	if (_Peek_ptr)
		ret_ch = _Peek_ch[--_Peek_ptr];
	else
	{
interrupted_system_call:
		errno = 0;
		switch (read(0, &ch, 1))
		{
		  case -1:
			if (errno == EINTR)
				goto interrupted_system_call;
			/* fall through */

		  case 0:
			ret_ch = C_QUIT;
			break;

		  default:
			ret_ch = ch;
			ret_ch &= 0377;
		}
		MMclr_error();
	}

	return (ret_ch);
}


MM_getchar()
{
	register int			character;
	register struct MM_tree		*tree;
	register int			peekptr;
	char				peekch[PEEKS];

	tree = _Tree;
	peekptr = 0;
next_char:
	peekch[peekptr++] = character = _getchar();
same_char:
	if (tree->t_char == character)
	{
		if (tree->t_code)
			return (tree->t_code);
		tree = tree->t_next;
		goto next_char;
	}
	if (tree->t_other)
	{
		tree = tree->t_other;
		goto same_char;
	}
	while (--peekptr)
		MM_backup(peekch[peekptr]);

	return (peekch[0]);
}
