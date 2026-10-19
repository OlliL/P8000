/* test for conf.c: test_conf FILE KEY... prints the values, or "(unset)" */
#include <stdio.h>
#include <errno.h>
#include "conf.h"

int
main(int argc, char **argv)
{
	struct conf *c;
	const char *v;
	int i;

	c = conf_read(argv[1]);
	if (c == 0)
		return errno == ENOENT ? 2 : 3;
	for (i = 2; i < argc; i++) {
		v = conf_get(c, argv[i]);
		printf("%s=[%s]\n", argv[i], v ? v : "(unset)");
	}
	return 0;
}
