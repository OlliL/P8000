/* lkdata and unlk (record locking, sys/lock.c): the emulator grants them (passwd locks /etc/passwd) */
#include <stdio.h>
#include <sys/lockblk.h>

main()
{
	struct lockblk b;
	int fd;
	long r1, r2;

	fd = creat("lk.tmp", 0644);
	b.lklen = 10L;
	b.lkoff = 0L;
	b.lkwhnce = 0;
	r1 = lkdata(fd, LKEXCLUSIVE | LKEXP | LKNOBLOCK, &b);
	r2 = unlk(fd, LKEXP, &b);
	printf("lkdata %s, unlk %s\n", r1 >= 0 ? "granted" : "refused", r2 >= 0 ? "done" : "refused");
	close(fd);
	unlink("lk.tmp");
	exit(0);
}
