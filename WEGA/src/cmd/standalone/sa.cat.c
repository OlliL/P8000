/**************************************************************************
 **************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW    Abt. Basissoftware - 1985
	Standalone:	sa.cat.c
 
 
	Bearbeiter	: P. Hoge
	Datum		: 04/01/87 11:15:40
	Version		: 1.1
 
 **************************************************************************
 **************************************************************************/



/* standalone cat */

char buf[0x200];
 
main()
{
	register c, i, t;

	do {
		printf("File: ");
		gets(buf, 100);
		if (buf[0] == 0)
			exit(1);
		i = open(buf, 0);
	} while (i <= 0);

	for (t=0; (c=getc(i)) != -1;) {
		if (c == 0x0a || (c >= 0x20 && c < 0x7f)) {
			putchar(c);
			t = ++t & 0x0007;
		}
		if (c == 0x09) {
			do {
				putchar(0x20);
				t = ++t & 0x0007;
			}while (t != 0);
		}
	}
	exit(0);
}
