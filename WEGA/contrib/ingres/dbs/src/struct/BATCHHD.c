# include	<stdio.h>
# include	"../conf/conf.h"
# include	"../h/ing_db.h"
# include	"../h/access.h"
# include	"../h/batch.h"

# define	off(x)		i = ((char *) x) - ((char *) d)
# define	out(arg)	printf("%4d %06o #%04x:\t%s\n", i, i, i, arg)

main()
{
	register struct batchhd		*d;
	register int			i;
	register int			k;
	struct batchhd			desc;
	char				buf[BUFSIZ];

	setbuf(stdout, buf);
	printf("struct batchhd [h/batch.h]\n");
	d = &desc;
	off(d->db_name);
	out("db_name (char [MAXNAME + 1])");
	off(d->rel_name);
	out("rel_name (char [MAXNAME + 1])");
	off(d->userid);
	out("userid (char [2])");
	off(&d->num_updts);
	out("num_updts (long)");
	off(&d->mode_up);
	out("mode_up (short)");
	off(&d->tido_size);
	out("tido_size (short)");
	off(&d->tupo_size);
	out("tupo_size (short)");
	off(&d->tupn_size);
	out("tupn_size (short)");
	off(&d->tidn_size);
	out("tidn_size (short)");
	off(&d->si_dcount);
	out("si_dcount (short)");
	off(d->si);
	out("si (struct si_doms [MAXDOM + 1])");
	for (k = 0; k <= 1; k++)
	{
		off(&d->si[k]);
		printf("%4d %06o #%04x:\tsi[%d] (struct si_doms)\n", i, i, i, k);
		off(&d->si[k].rel_off);
		printf("%4d %06o #%04x:\tsi[%d].rel_off (int)\n", i, i, i, k);
		off(&d->si[k].tupo_off);
		printf("%4d %06o #%04x:\tsi[%d].tupo_off (int)\n", i, i, i, k);
		off(&d->si[k].dom_size);
		printf("%4d %06o #%04x:\tsi[%d].dom_size (int)\n", i, i, i, k);
	}
	fflush(stdout);
}
