# include	"gen.h"
# include	"../h/dbs.h"

/*
**  PRINT RELATION DESCRIPTOR (for debugging)
**
**	A pointer of a file descriptor is passed.  All pertinent
**	info in that descriptor is printed on the standard output.
**
**	For debugging purposes only
*/

AAprdesc(d)
register DESC		*d;
{
	register int			i;
	register int			end;

	printf("Descriptor 0%o %.12s %.2s\n",
		d, d->reldum.relid, d->reldum.relowner);
	printf("\tspec %d, indxd %d, stat 0%o\n",
		ctoi(d->reldum.relspec), ctoi(d->reldum.relindxd), d->reldum.relstat);
	printf("\tsave %ld, tups %ld, atts %d, wid %d, prim %ld\n",
		d->reldum.relsave, d->reldum.reltups, d->reldum.relatts,
		d->reldum.relwid, d->reldum.relprim);
	printf("\tfp %d, opn %d, tid %ld, adds %ld\n",
		d->relfp, d->relopn, d->reltid, d->reladds);

	end = d->reldum.relatts;
	for (i = 0; i <= end; i++)
		printf("\t[%2d] off %3d frmt %2d frml %3d, xtra %3d, given %04o\n",
			i, d->reloff[i], d->relfrmt[i],
			ctou(d->relfrml[i]), d->relxtra[i], d->relgiven[i]);
}
