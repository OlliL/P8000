/* the empty name is the current directory for open, stat and chdir (nami.c: a name without a component, flag 0): the glob of
   csh opens "" to read the directory */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

main()
{
	struct stat st;
	char buf[64];
	int fd, n;

	fd = open("", 0);
	printf("open: %s\n", fd >= 0 ? "ok" : "failed");
	if (fd >= 0) {
		n = read(fd, buf, 32);
		printf("read: %s\n", n >= 32 ? "entries" : "none");
	}
	printf("stat: %s\n", stat("", &st) == 0 && (st.st_mode & 040000) ? "a directory" : "failed");
	printf("chdir: %s\n", chdir("") == 0 ? "ok" : "failed");
	exit(0);
}
