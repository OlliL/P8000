/* ldconf - see ldconf.h */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "conf.h"
#include "ldconf.h"

void
ldconf_load(const char *argv0)
{
	char *path = conf_sibling(argv0, "ld.conf");
	struct conf *c;
	const char *v;

	if (path == NULL || (c = conf_read(path)) == NULL) {
		if (path != NULL && errno != ENOENT)
			exit(4);		/* the message is printed */
		return;
	}
	if ((v = conf_get(c, "root")) != NULL)
		setenv("WEGA_ROOT", v, 0);
	if ((v = conf_get(c, "libpath")) != NULL)
		setenv("WEGA_LIBPATH", v, 0);
}
