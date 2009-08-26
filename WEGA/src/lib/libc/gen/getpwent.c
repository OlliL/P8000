/* @[$] getpwent.c	2.1  09/12/83 11:13:03 - 87wega3.2 */

#include <stdio.h>
#include <pwd.h>

static char PASSWD[] = "/etc/passwd";
static char EMPTY[] = "";
static FILE *pwf = NULL;
static char line[BUFSIZ+1];
static struct passwd passwd;

setpwent()
{
	if (pwf == NULL)
		pwf = fopen (PASSWD, "r");
	else
		rewind (pwf);
}

endpwent()
{
	if (pwf != NULL){
		fclose (pwf);
		pwf = NULL;
	}
}

static char *
pwskip (p)
register char *p;
{
	while (*p && *p != ':' && *p != '\n')
		++p;
	if (*p == '\n')
		*p = '\0';
	else if (*p)
		*p++ = '\0';
	return p;
}

struct passwd *
getpwent()
{
	register char *p;

	if (pwf == NULL) {
		if ((pwf = fopen (PASSWD, "r")) == NULL)
			return 0;
	}
	p = fgets (line, BUFSIZ, pwf);
	if (p==NULL)
		return 0;
	passwd.pw_name = p;
	p = pwskip (p);
	passwd.pw_passwd = p;
	p = pwskip (p);
	passwd.pw_uid = atoi (p);
	p = pwskip (p);
	passwd.pw_gid = atoi (p);
	passwd.pw_comment = EMPTY;
	p = pwskip (p);
	passwd.pw_gecos = p;
	p = pwskip (p);
	passwd.pw_dir = p;
	p = pwskip (p);
	passwd.pw_shell = p;
	pwskip(p);

	p = passwd.pw_passwd;
	while (*p && *p != ',')
		p++;
	if (*p)
		*p++ = '\0';
	passwd.pw_age = p;
	return &passwd;
}
