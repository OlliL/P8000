# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/tree.h"
# include	"../h/symbol.h"


struct proctemplate
{
	char	*proc_name;
	int	proc_code;
	char	proc_exec;
	char	proc_func;
};

static struct proctemplate	Proc_template[] =
{
	"copy",		(int) mdCOPY,		0,	0,
	"create",	(int) mdCREATE,		0,	0,
	"destroy",	(int) mdDESTROY,	0,	0,
	"display", 	(int) mdDISPLAY,	0,	0,
	"help",		(int) mdHELP,		0,	0,
	"index",	(int) mdINDEX,		0,	0,
	"modify",	(int) mdMODIFY,		0,	0,
	"print",	(int) mdPRINT,		0,	0,
	"save",		(int) mdSAVE,		0,	0,
	"update",	(int) mdUPDATE,		0,	0,
	"resetrel",	(int) mdRESETREL,	0,	0,
	"remqm",	(int) mdREMQM,		0,	0,
	(char *) 0
};


init_proctab(proc, my_id)
char	*proc;
char	my_id;

/*
**	Initialize from the run time process table
**
**	Each entry in the proctable is seached for in the
**	runtime process table. If an entry isn't found it
**	is considered a AAsyserr. If an entry is found the
**	first one which has an exec_id of "my_id" is taken.
**	Otherwise, the very first entry given will be used.
*/

{
	register struct proctemplate	*pt;
	register char			*p;
	register char			*name;
	char				*found;
	int				i;

#	ifdef xZTR1
	AAtTfp(0, 0, "INIT_PTAB:\n");
#	endif

	for (pt = Proc_template; name = pt->proc_name; pt++)
	{
		i = AAlength(name);
		p = proc;
		found = (char *) 0;

		while (*p)
		{
			if (AAbequal(p, name, i))
			{
				/* found a match. skip past the colon */
				while (*p++ != ':')
					continue;

				/* look for an alias in this dbu */
				found = p;
				do
				{
					if (*p == my_id)
					{
						found = p;
						break;	/* found one */
					}
					p++;
					p++;
				} while (*p++ == ',');

#				ifdef xZTR1
				AAtTfp(0, 1, "%s: %.2s\n", name, found);
#				endif

				pt->proc_exec = *found++;
				pt->proc_func = *found;
				break;

			}

			/* skip to next */
			while (*p++ != '\n')
				continue;
		}

		if (!found)
			AAsyserr(11017, name);
	}
}


get_proctab(code, exec, func)
int	code;
char	*exec;
char	*func;

/*
**	Find the correct exec & func for a given code
*/

{
	register struct proctemplate	*p;
	register int			i;

	i = code;
	for (p = Proc_template; p->proc_name; p++)
	{
		if (i == p->proc_code)
		{
			/* found entry */
			*exec = p->proc_exec;
			*func = p->proc_func;
			return;
		}
	}

	/* entry not there */
	AAsyserr(11018, i);
}
