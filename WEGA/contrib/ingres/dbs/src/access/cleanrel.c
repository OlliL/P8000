# include	"gen.h"
# include	"../h/dbs.h"
# include	"../h/aux.h"
# include	"../h/access.h"

/* CLEANREL -- */
/* If there are any buffers being used by the relation described */
/* in the descriptor struct, flush and zap the buffer. */
/* This will force a disk read the next time the relation */
/* is accessed which is useful to get the most up-to-date */
/* information from a file that is being updated by another */
/* program. */
cleanrel(d)
register DESC	*d;
{
	return (AAam_flush_rel(d, TRUE));	/* flush and reset all pages of this rel */
}
