/* open with FCREAT (O_CREAT 0400), FTRUNC (O_TRUNC 01000), FEXCL (O_EXCL 02000) and the mode as third argument
   (sys2.c copen): dosformat opens its image so */
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int errno;

main()
{
	int fd, n;
	struct stat st;
	char buf[16];

	unlink("of.tmp");
	fd = open("of.tmp", O_RDWR | O_CREAT | O_TRUNC, 0664);
	printf("create: %s\n", fd >= 0 ? "ok" : "failed");
	write(fd, "abcdef", 6);
	close(fd);
	stat("of.tmp", &st);
	printf("size %ld mode %o\n", (long)st.st_size, st.st_mode & 0777);
	fd = open("of.tmp", O_RDWR | O_CREAT | O_EXCL, 0600);
	printf("excl: fd %d errno %d\n", fd < 0 ? -1 : 0, errno);
	fd = open("of.tmp", O_RDONLY);
	n = read(fd, buf, 16);
	printf("read %d\n", n);
	close(fd);
	fd = open("of.tmp", O_RDWR | O_TRUNC);
	stat("of.tmp", &st);
	printf("trunc: size %ld\n", (long)st.st_size);
	close(fd);
	fd = open("of2.tmp", O_WRONLY);
	printf("no create: fd %d errno %d\n", fd < 0 ? -1 : 0, errno);
	unlink("of.tmp");
	exit(0);
}
