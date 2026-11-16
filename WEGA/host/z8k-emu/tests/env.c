/* -e NAME=VALUE (tests/env.args): a name the emulator sets itself (HOME, PATH) is replaced, another one is added;
   getenv takes the first match, so a second HOME= would not be seen */
#include <stdio.h>

main()
{
	char *getenv();
	char *h = getenv("HOME"), *p = getenv("PATH"), *f = getenv("FOO");

	printf("HOME=%s PATH=%s FOO=%s\n", h ? h : "(none)", p ? p : "(none)", f ? f : "(none)");
	exit(0);
}
