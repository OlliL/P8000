#include <stdio.h>
#include <ctype.h>
char buf[100];
main()
{
	char *p;
	int n;

	strcpy(buf, "Hello");
	strcat(buf, ", World");
	n = 0;
	for (p = buf; *p; p++)
		if (isupper(*p))
			n++;
	printf("%s %d %d\n", buf, strlen(buf), n);
	return 0;
}
