#include <grp.h>

extern struct group *getgrent();
extern int setgrent(), endgrent();

struct group *
getgrgid(gid)
register gid;
{
	register struct group *p;

	(void) setgrent();
	while((p = getgrent()) && p->gr_gid != gid);
	(void) endgrent();
	return(p);
}
