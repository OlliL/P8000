#include <pwd.h>

extern struct passwd *getpwent();
extern int setpwent(), endpwent();

struct passwd *
getpwuid(uid)
register uid;
{
	register struct passwd *p;

	(void) setpwent();
	while((p = getpwent()) && p->pw_uid != uid)
		;
	(void) endpwent();
	return(p);
}
