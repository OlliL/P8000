/* a text file that starts with #!NAME is run by the program NAME (no path: /bin, then /usr/bin), with the file as its first argument
   (seen on the real system: /usr/bin/sccsdiff starts with #!sh) */
#include <stdio.h>

main()
{
	FILE *f;
	int pid, st;

	f = fopen("t.sh", "w");
	fputs("#!sh\necho $0 $1 $2\n", f);
	fclose(f);
	chmod("t.sh", 0755);
	if ((pid = fork()) == 0) {
		execl("./t.sh", "t.sh", "a", "b", 0);
		printf("exec failed\n");
		exit(1);
	}
	wait(&st);
	printf("status %d\n", st >> 8);
	unlink("t.sh");
	return 0;
}
