/* @[$] getpwnam.c	2.1  09/12/83 11:13:23 - 87wega3.2 */

#include <pwd.h>

struct passwd *
getpwnam(name)
char *name;
{
	register struct passwd *p;
	struct passwd *getpwent();

	setpwent();
	while( (p = getpwent()) && strcmp(name,p->pw_name) );
	endpwent();
	return(p);
}
