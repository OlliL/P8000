# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/catalog.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"
# include	"../h/access.h"

/*
**  DISPLAY -- display query corresponding to view, permission,
**		or intgerity declaration
**
**	Defines:
**		display() -- main function
**		disp() -- routes VIEW, PERMIT, and INTEGRITY
**		pr_def() -- print view definition
**		pr_integrity() -- print out integrity contstraints on a rel
*/

extern FILE	*Dbs_out;
extern int	Standalone;
extern QTREE	*gettree();	/* gets a tree from AA_TREE catalog [readtree.c] */
char		*Resrel;	/* Used in pr_tree() [pr_tree.c] */
static short	Mode;		/* 4 - View, 5 - permission, 6 - integrity
				 * query print
				 */

/*
**  DISPLAY -- display query
**
**	Parameters:
**		argc -- number of args in argv
**		argv -- argv [i] == 4 for VIEW, 5 for PERMIT, 6 for INTEGRITY
**				where i % 2 == 0
**		argv [i] -- relation names for which argv[i-1] is Mode
**				where i%2==1
**
**	Returns:
**		0 -- total success
**		err -- error number of last error
*/
display(argc, argv)
int		argc;
char		**argv;
{
	register int		ac;
	register char		**av;
	extern DESC		Treedes;
	char			err_array[MAXPARMS];	/* errors are coded
							** this array for
							** printing out
							** at the end
							*/
	int			err;

#	ifdef xZTR1
	if (AAtTfp(11, -1, "DISPLAY: "))
		AAprargs(argc, argv);
#	endif

	err = 0;
	if (argc % 2)
		AAsyserr(11023, argc);
	opencatalog(AA_TREE, 0);

	for (ac = 0, av = argv; ac < argc; av++, ac++)
	{
		if (AA_atoi(*av, &Mode))
			AAsyserr(11024, *av);
		Resrel = *++av;
		err_array[ac++] = 0;
		err_array[ac] = disp(*av);
	}
	for (ac = 0, av = argv; ac < argc; av++, ac++)
	{
		if (err_array[ac])
			err = error(5400 + err_array[ac], *av, (char *) 0);
	}
	return (err);
}

/*
** DISP -- display integrity, permit, or define query on a relation
**
**	Finds a relation owned by the user or the DBA and passes
**	the name and owner to the appropritae routine depending on
**	Mode.
**
**	Parameters:
**		relation -- relation on which query is to be printed
**
**	Returns:
**		0 -- success
**		1 -- no such relation, or none seeable by the user.
**		3 -- VIEW mode and relation not a view
**		4 -- PERMIT and no permissions on relation
**		5 -- INTEGRITY mode and no integrity constraints
*/
disp(relation)
char	*relation;
{
	DESC				d;
	register int			i;

#	ifdef xZTR1
	AAtTfp(11, 8, "DISP: relation %s\n", relation);
#	endif

	if ((i = AArelopen(&d, -1, relation)) < 0)
		AAsyserr(11025, relation, i);
	if (i > 0 || !AAbequal(AAusercode, d.reldum.relowner, 2))
		return (1);
	switch (Mode)
	{
	  case 4:		/* View query */
		if (d.reldum.relstat & S_VIEW)
			pr_def(relation, d.reldum.relowner);
		else
			return (3);
		break;

	  case 5:
		if (pr_prot(&d))
			return (4);
		break;

	  case 6:
		if (d.reldum.relstat & S_INTEG)
			pr_integrity(relation, d.reldum.relowner);
		else
			return (5);
		break;

	  default:
		AAsyserr(11026, Mode);
	}
	return (0);
}


/*
**  PR_DEF -- Print "define view" query of a view
**
**	Parameters:
**		relation -- relation in question
**		owner -- relowner
**
**	Side Effects:
**		reads a tree, clears range table
*/
pr_def(relation, owner)
char		*relation;
char		*owner;
{
	register QTREE		*t;

#	ifdef xZTR1
	AAtTfp(11, 9, "pr_def(relation=\"%s\", owner=%s)\n", relation, owner);
#	endif

	if (!Standalone)
		fprintf(Dbs_out, "View %s defined:\n\n", relation);
	clrrange();

	/* Treeid == 0 because views have only one definition */
	t = gettree(relation, owner, mdVIEW, 0);
	pr_range();
	fprintf(Dbs_out, "define view ");
	pr_tree(t);
}


/*
**  PR_INTEGRITY -- print out integrity constraints on a relation
**
**	Finds all integrity tuples for this unique relation, and
**	calls pr_int() to print a query from them.
**
**	Parameters:
**		relid -- rel name
**		relowner -- 2 byte owner id
*/
pr_integrity(relid, relowner)
char	*relid;
char	*relowner;
{
	extern DESC			Intdes;
	struct tup_id			hitid;
	struct tup_id			lotid;
	struct integrity		key;
	struct integrity		tuple;
	register int			i;


#	ifdef xZTR1
	AAtTfp(11, 9, "pr_integrity(relid =%s, relowner=%s)\n", relid, relowner);
#	endif

	if (!Standalone)
		fprintf(Dbs_out, "Integrity constraints on %s are:\n\n", relid);
	opencatalog(AA_INTEG, 0);

	/* get AA_INTEG tuples for relid, relowner */
	AAam_clearkeys(&Intdes);
	AAam_setkey(&Intdes, &key, relid, INTRELID);
	AAam_setkey(&Intdes, &key, relowner, INTRELOWNER);
	if (i = AAam_find(&Intdes, EXACTKEY, &lotid, &hitid, &key))
		AAsyserr(11027, i);
	for ( ; ; )
	{
		if (i = AAam_get(&Intdes, &lotid, &hitid, &tuple, TRUE))
			break;
		if (!AAkcompare(&Intdes, &tuple, &key))
			pr_int(&tuple);
	}
	if (i != 1)
		AAsyserr(11028, i);
}


/*
**  PR_INT -- print an integrity definition given a AA_INTEG tuple
**
**	Parameters:
**		g -- integrity tuple
*/
pr_int(g)
struct integrity	*g;
{
	register struct integrity	*i;
	register QTREE			*t;
	extern int			Resultvar;

	i = g;
	clrrange();
	t = gettree(i->intrelid, i->intrelowner, mdINTEG, i->inttree);
	if (!Standalone)
		fprintf(Dbs_out, "Integrity constraint %d -\n\n", i->inttree);
	pr_range();

	fprintf(Dbs_out, "define integrity on ");
	pr_rv(Resultvar = ctoi(i->intresvar));
	fprintf(Dbs_out, " is ");
	pr_qual(t->right);
	if (!Standalone)
		fprintf(Dbs_out, "\n\n\n");
}
