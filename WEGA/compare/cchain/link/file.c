#include <stdio.h>
main()
{
	FILE *f;
	int c, n;

	f = fopen("file.c", "r");
	if (f == NULL) {
		perror("file.c");
		exit(1);
	}
	n = 0;
	while ((c = getc(f)) != EOF)
		if (c == '\n')
			n++;
	fclose(f);
	printf("%d lines\n", n);
	return 0;
}
