# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/access.h"

AAdumptid(tid)
register struct tup_id	*tid;
{
	register int		lineid;
	long			pageid;

	AApluck_page(tid, &pageid);
	if (pageid == 0xffffff)
		pageid = -1;
	if ((lineid = ctou(tid->line_id)) == SCANTID)
		lineid = -1;
	printf("tid: %03ld/%03d\n", pageid, lineid);
}


/*
** AApluck_page extracts the three byte page_id from the tup_id struct
** and puts it into a long variable with proper allignment.
*/
AApluck_page(tid, var)
register struct tup_id	*tid;
register struct lpage	*var;
{
	var->lpg0 = tid->pg0;
	var->lpg1 = tid->pg1;
	var->lpg2 = tid->pg2;
	var->lpgx = 0;
}


/*	AAstuff_page is the reverse of AApluck_page	*/
AAstuff_page(tid, var)
register struct tup_id	*tid;
register struct lpage	*var;
{
	tid->pg0 = var->lpg0;
	tid->pg1 = var->lpg1;
	tid->pg2 = var->lpg2;
}
