# include	<stdio.h>
# include	"gen.h"
# include	"constants.h"
# include	"globals.h"

/*
**  CVAR -- routines to manipulate the c variable trees
**
**	C variable trees are binary trees of cvar structs,
**	with the c_left < c_right with respect to c_id.
**
**	Defines:
**		decl_cvar() -- declare a C variable
**		decl_field() -- declare a structure field
**		getcvar() -- get cvar node of a C variable
**		getfield() -- get field's node
**		freecvar() -- frees cvar tree
**
**
**
**  DECL_CVAR -- Declare a C variable
**
**	Parameters:
**		name -- identifier string (makes its own copy for the tree)
**		type
**		indir_level -- level of indirection of declaration
**			(- 1 if string)
**		block_level -- 0 - global, else local var
**
**	Side Effects:
**		allocates a cvar node, and a copy of name, may put a node
**		in a cvar tree (if not previously declared).
*/
decl_cvar(name, type, indir_level, block_level)
char		*name;
int		type;
int		indir_level;
int		block_level;
{
	register struct cvar		*bad_node;
	extern struct cvar		*dec_var();

	if (bad_node = dec_var(name, type, indir_level, block_level,
				&C_locals, &C_globals))
	{
		yysemerr("re-declared identifier", bad_node->c_id);
		xfree(bad_node->c_id);
		xfree(bad_node);
	}
}


/*
**  DECL_FIELD -- Declare a structures field
**
**	Same as decl_cvar() for fields within C records (structs).
**	NOTE : if a !0 is returned from dec_var() (i.e. the field
**	was already declared) the storage for that node is freed
**	but no error has been comitted, as fields may be re-declared.
*/
decl_field(name, type, indir_level, block_level)
char		*name;
int		type;
int		indir_level;
int		block_level;
{
	register struct cvar		*bad_node;
	extern struct cvar		*dec_var();

	if (bad_node = dec_var(name, type, indir_level, block_level,
				&F_locals, &F_globals))
	{
		xfree(bad_node->c_id);
		xfree(bad_node);
	}
}


/*
**  DEC_VAR -- declare a C var or field.
**
**	Parameters:
**		same as decl_cvar() & decl_field plus
**		the local and global tree variables.
**
**	Returns:
**		0 -- successful
**		other -- cvar node pointer that couldn't be entered
**			to tree
*/
struct cvar	*dec_var(name, type, indir_level, block_level, local_tree, global_tree)
char		*name;
int		type;
int		indir_level;
int		block_level;
struct cvar	**local_tree;
struct cvar	**global_tree;
{
	register struct cvar		*cvarp;
	register int			i;
	extern char			*nalloc();
	extern char			*salloc();

	cvarp = (struct cvar *) nalloc(sizeof *cvarp);
	if (!cvarp)
	{
		yysemerr("unable to allocate space for a variable", name);
		return ((struct cvar *) 0);
	}
	if (!(cvarp->c_id = salloc(name)))
	{
		yysemerr("no space for variable name", name);
		xfree(cvarp);
		return ((struct cvar *) 0);
	}
	cvarp->c_type = type;
	cvarp->c_indir = indir_level;
	cvarp->c_left = cvarp->c_right = 0;
	i = c_enter(cvarp, (block_level > 0)? local_tree: global_tree);
	return (i? (struct cvar *) 0: cvarp);
}


/*
**   C_ENTER -- Enter a cvar node in a cvar tree
**
**	Parameters:
**		n -- the cvar node to insert
**		root -- a pointer to the root pointer
**
**	Returns:
**		1 -- if successful
**		0 -- otherwise (node of same name existed
**
**	Side Effects:
**		If a node of that name didn't exist one is inserted
*/
c_enter(n, root)
register struct cvar	*n;
register struct cvar	**root;
{
	register char		*name;
	register struct cvar	*r;

	r = *root;
	name = n->c_id;
	if (!r)
	{
		*root = n;
		return (1);
	}
	for ( ; ; )
	{
		switch (AAscompare(name, 0, r->c_id, 0))
		{

		  case -1 :
			if (!r->c_left)
			{
				r->c_left = n;
				return (1);
			}
			r = r->c_left;
			break;

		  case 0 :
			yysemerr("identifier re-declared", name);
			xfree(name);
			xfree(n);
			return (0);

		  case 1 :
			if (!r->c_right)
			{
				r->c_right = n;
				return (1);
			}
			r = r->c_right;
			break;
		}
	}
}


/*
**  GET_VAR -- get a cvar node from a local_tree, global_tree pair
**	searching first through the local then the global.
**
**	Parameters:
**		id -- c_id key
**		local_tree -- first tree
**		global_tree -- secomd tree to search
**
**	Returns:
**		0 -- if no node by that name
**		otherwise -- pointer to the node
*/
struct cvar	*get_var(id, local_tree, global_tree)
register char	*id;
struct cvar	*local_tree;
struct cvar	*global_tree;
{
	register struct cvar	*tree;
	register struct cvar	*node;
	int			flag;

	flag = 0;
	tree = local_tree;
	for ( ; ; )
	{
		for (node = tree; node; )
		{
			switch (AAscompare(id, 0, node->c_id, 0))
			{

			  case -1 :
				if (!node->c_left)
					break;
				else
					node = node->c_left;
				continue;

			  case 0 :
				return (node);

			  case 1 :
				if (!node->c_right)
					break;
				else
					node = node->c_right;
				continue;
			}
			break;
		}
		if (!flag)
		{
			flag++;
			tree = global_tree;
		}
		else
			return (0);
	}
}


/*
**  GETCVAR -- get the cvar node for a given identifier
**	Looks first in C_locals, then in C_globals.
**
**	Parameters:
**		id -- name of cvar to look for
**
**	Returns:
**		adress of cvar node if found
**		0 -- otherwise
*/
struct cvar	*getcvar(id)
register char	*id;
{
	extern struct cvar	*get_var();

	return (get_var(id, C_locals, C_globals));
}


/*
**  GETFIELD -- Same as getcvar() for structure fields
*/
struct cvar	*getfield(id)
register char	*id;
{
	extern struct cvar	*get_var();

	return (get_var(id, F_locals, F_globals));
}


/*
**  FREECVAR & F_CVAR -- Free up storage in a cvar tree
**	Freecvar calls f_cvar to free storage for a tree, then
**	0's out the root pointer passed it.
**
**	Usage:
**		Freecvar(rootp) -- should be called to free
**			a tree *rootp
*/
freecvar(rootp)
register struct cvar	**rootp;
{
	f_cvar(*rootp);
	*rootp = 0;
}


f_cvar(root)
register struct cvar	*root;
{
	if (root)
	{
		f_cvar(root->c_left);
		f_cvar(root->c_right);
		xfree(root->c_id);
		xfree(root);
	}
}
