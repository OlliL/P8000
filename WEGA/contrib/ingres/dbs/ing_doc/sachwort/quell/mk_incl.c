/* Erstellung des include-Files zur Vektordefinition des
** Sachwort-Katalogs;
** Aufruf:	mk_incl sawo.nr
*/

# include	<stdio.h>
# include	<ctype.h>

main(argc, argv) int argc; char *argv[];
{
	register int c, i;
	FILE *fs, *fk, *fopen();
	int	snr;
	int	ach;
	int	k;
	struct	tab
	{
		int	alpha;
		int	von;
	};
	struct	tab	Atab[26];
	char	buf[60];

if (argc != 2)
{
	printf("\nAnzahl Argumente: %d", argc);
	printf("\usage: mk_incl file_sachw_katalog\n");
	exit(0);
}
if ((fs = fopen(*++argv, "r")) == NULL)
{
	perror("sachw_kat not found");
	exit(0);
}
if ((fk = fopen("sawo.h", "w+")) == NULL)
{
	perror("sawo.h not open");
	exit(0);
}

/* Erstellung des include-Files zum Sachwort-Katalog */

fprintf(fk, "/* Sachwort-Katalog --> Vektordefinition */\n\n");
fprintf(fk, "struct kat\n{\n\tint len;\n\tchar *saw;\n\tint snr;\n};\n");
fprintf(fk, "struct kat Katalog[] =\n{\n");

/* Uebernahme aus dem Sachwort-Katalog und Eintrag: Wort Schluessel-Nr. */

snr = 0;
i   = 0;
k   = 0;
ach = 'z';
while ((c = getc(fs)) != EOF)
{
	if (c != '\n')
	{
		buf[i++] = tolower(c);
		if (c == '\b') buf[i-1] = '<';
		if (c == '"') --i;
	}
	else
	{
		if (i > 1)
		{
			buf[i] = '\0';
			fprintf(fk, "\t%2d,\t\"%s\",\t%d,\n", i, buf, ++snr);
			i = 0;
			if (ach != buf[0] && (buf[0] >= 'a' && buf[0] <= 'z'))
			{
				Atab[k].alpha = buf[0];
				Atab[k].von = snr;
				ach = buf[0];
				if (k < 25) ++k;
			}
		}
	}
}

/* Endebehandlung */

fprintf(fk,"};\n");
fprintf(fk,"\n/*Katalog zur Bezugnahme Buchstabenwechsel */\n\n");
fprintf(fk,"struct tab\n{\n\tint alpha;\n\tint von;\n};\n");
fprintf(fk,"struct tab Atab[] = \n{\n");
for (ach = 0; ach < k; ach++)
	fprintf(fk,"\t%d,%d,\n", Atab[ach].alpha,Atab[ach].von);
fprintf(fk,"\t%d,%d,\n", Atab[ach].alpha,snr);
fprintf(fk,"};\n");
fprintf(fk,"int bis = %d;\n", k);
fclose(fk);
fclose(fs);
}

