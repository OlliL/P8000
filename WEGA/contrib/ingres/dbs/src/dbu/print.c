# include	<stdio.h>
# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"
# include	"../h/aux.h"
# include	"../h/lock.h"

/*
**  PRINT RELATION
**
**	Parmv[0] through Parmv [parmc -1] contain names
**	of relations to be printed on the standard output.
**	Note that this calls AApratt, so all the output formatting
**	features associated with AApratt are available here.
**
**	Parmv[parmc] is -1 for a "normal" print, -2 for headers on
**	every page, and -3 for all headers and footers suppressed.
**	err_array is set to 0 for a good relation, 1 for no
**	relation by that name, and -1 for a view.
**
**	Returns:
**		0 if no errors else the last error number
*/
print(parmc, parmv)
int	parmc;
char	**parmv;
{
	DESC				d;
	extern DESC			Attdes;
	struct attribute		att;
	char				tuple[MAXTUP];
	struct tup_id			tid;
	struct tup_id			limtid;
	register int			i;
	register int			ern;
	register char			*name;
	int				mode;
	int				lineno;
	int				pc;
	int				err_array[MAXPARMS];
	extern struct out_arg		AAout_arg;
	extern char			*AAcustomer();

# ifdef xZTR1
	if (AAtTf(8, -1) || AAtTf(0, 8))
		printf("entering PRINT\n");
# endif
	mode = ((int) parmv[parmc]) + 2;
	opencatalog(AA_ATT, 0);

	for (pc = 0; pc < parmc; pc++)
	{
		name = parmv[pc];

		ern = AArelopen(&d, 0, name);
		if (ern == AMOPNVIEW_ERR)
		{
			err_array[pc] = 5002;
			continue;
		}
		if (ern > 0)
		{
			err_array[pc] = 5001;
			continue;
		}
		if (ern < 0)
			AAsyserr(11116, name, ern);
		if ((d.reldum.relstat & S_PROTALL) && (d.reldum.relstat & S_PROTRET) &&
			!AAbequal(AAusercode, d.reldum.relowner, 2))
		{
			err_array[pc] = 5003;
next_rel:
			AArelclose(&d);
			continue;
		}
		/* set shared lock on relation*/
		if (AAsetrll(A_SLP, d.reltid, M_SHARE) < 0)
		{
			err_array[pc] = 5000;
			goto next_rel;
		}


		/* a printable relation */
		err_array[pc] = 0;
# ifdef xZTR2
		if (AAtTf(8, 1))
			AAprdesc(&d);
# endif
		lineno = AAout_arg.linesperpage;
		if (mode >= 0)
		{
			if (!mode)
				putchar('\014');	/* form feed */
			printf("\n%s / Relation: %s\n", AAcustomer(), name);
			AAbgnhdr();
			seq_init(&Attdes, &d);
			for (i = 1; seq_attributes(&Attdes, &d, &att); i++)
				AAprhdr(d.relfrmt[i], d.relfrml[i], att.attname);
			AApreol();
			AAendhdr();
			lineno -= 6;
		}
# ifdef xZTM
		if(AAtTf(76, 1))
			timtrace(29, 0);
# endif
		AAam_find(&d, NOKEY, &tid, &limtid);
		while (!(ern = AAam_get(&d, &tid, &limtid, tuple, TRUE)))
		{
			if (!mode && lineno-- <= 0)
			{
				lineno = AAout_arg.linesperpage - 5;
				putchar('\n');
				AAbgnhdr();
				seq_init(&Attdes, &d);
				for (i = 1; seq_attributes(&Attdes, &d, &att); i++)
					AAprhdr(d.relfrmt[i], d.relfrml[i], att.attname);
				AApreol();
				AAendhdr();
			}
			AAprtup(&d, tuple);
			fflush(stdout);
		}
# ifdef xZTM
		if(AAtTf(76, 1))
			timtrace(30, 0);
# endif
		if (ern < 0)
			AAsyserr(11117, ern);

		if (mode >= 0)
			AAendhdr();
		AAunlrl(d.reltid);	/* release relation lock */

		AArelclose(&d);
	}
	/* check for any error messages that should be printed */
	ern = 0;
	for (pc = 0; pc < parmc; pc++)
	{
		if (i = err_array[pc])
			ern = error(i, parmv[pc], (char *) 0);
	}
	fflush(stdout);
	return (ern);
}
