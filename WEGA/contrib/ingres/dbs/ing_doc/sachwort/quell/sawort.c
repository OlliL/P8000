/* Erstellung eines Sachwortverzeichnisses (Rohformat)
** fuer aufbereitete nroff-Texte (file_all);
** Das File file_sachw wird mit do_swvz aufbereitet, und muss dann
** manuell korrigiert werden; anschliessend mit nroff aufbereiten
** mk_incl file_sachw abarbeiten um das include-File "sawo.h" zu
** erstellen;
** Aufruf:	sawort file_all
*/

# include	<stdio.h>
# include	<ctype.h>

# define	PL	72
# define	APL	75
/* am Anfang Seitenangabe auf Zl: 75 */

# define	MAXZLEN	80

# include	"sawo.h"

main(argc, argv) int argc; char *argv[];
{
	register int	c;
	FILE *fpo, *fps, *fopen();
	int flag;
	int flagp;
	int flagh;
	int flagn;
	int flagb;
	int flagw;
	int i;
	int j;
	int k;
	int l;
	int lo;
	int bvon;
	int bbis;
	int zlen;
	int pl = APL;
	int pgnr;
	char page[8];
	char hbuf[80];	/* Speicher fuer Vergleichswort im Text */
	char buf[80];

if ((fpo = fopen(*++argv, "r")) == NULL)
{
	perror("file_nroff not found");
	exit(0);
}
if (argc != 2)
{
	printf("\nAnzahl Argumente: %d", argc);
	printf("\nusage: sawort file\n");
	exit(0);
}

if ((fps = fopen("swverz", "w+")) == NULL)
{
	perror("swverz not open");
	exit(0);
}
printf("Zur Orientierung wird in Zehnerschritten die Seitennr. ausgegeben!\n");
flag = 0;
flagb = 0;
pgnr = 1;
i = 0;
while (( c = getc(fpo)) != EOF)
{
/*	putchar(c); */
	switch(c)
	{
	case '\b':	++flagb;
			++flagh;
			break;
	case '\n':	if (!(--pl))
			{
				++pgnr;
				if (!(pgnr % 10)) printf("Seite %d\n",pgnr);
				pl = PL;
			}
			if (i >= 1 && hbuf[i-1] == '-')
			{
				--i;
				break;
			}
	case ' ':
	case '.':
	case ',':
	case ';':
	case '!':
	case '?':
	case ':':
			hbuf[i] = '\0';
			if (i > 0)
			{
				flagw = 0;
				if (hbuf[0] >= 'a' && hbuf[0] >= 'z')
				{
					for (l = 0; l < bis; l++)
					{
						if (hbuf[0] == Atab[l].alpha)
						{
							++flagw;
							break;
						}
					}
					if (flagw)
					{
						bvon = Atab[l].von;
						bbis = Atab[l+1].von;
					}
					else goto nocomp;
				}
				else
				{
					bvon = 0;
					bbis = Atab[bis].von;
				}
				for (k = bvon; k < bbis; k++)
				{
				if (!strncmp(hbuf, Katalog[k].saw, Katalog[k].len))
					{
					fprintf(fps, "%03d\t%03d\n",Katalog[k].snr,pgnr);
					break;
					}
				}
			}
nocomp:
			i = 0;
			++flag;
			++flagn;
			flagb = 0;
			break;
	case '"' :
			k = hbuf[i-1];
			if ((k == 'A' || k == 'U' || k == 'O'
			 || k == 'a' || k == 'u' || k == 'o')
			 && (flagb == 1)) hbuf[i++] = '<';
			flagb = 0;
			flagh = 0;
			break;
	default  :	if (!flagh)
			{
				if (i < 80) hbuf[i++] = tolower(c);
				else printf("\n***FEHLER\n");
			}
			flagh = 0;
			flagb = 0;
			flagn = 0;
	}
}
fclose(fps);
fclose(fpo);
printf("Sortierung\n");
system("sort -du -o sawo swverz");
printf("Zusammenstellung Sachwort->Seite\n");
if ((fpo = fopen("sawo", "r")) == NULL)
{
	perror("sawo not found");
	exit(0);
}
if ((fps = fopen("swverz.q", "w+")) == NULL)
{
	perror("swverz.q not open");
	exit(0);
}
flag = 0;
i = 0;
k = 0;
while ((c = getc(fpo)) != EOF)
{
	switch(c)
	{
	case '\t':	hbuf[i] = '\0';
			l = atoi(hbuf);
			if (l != k)
			{
				strcpy(hbuf, Katalog[l-1].saw);
				j = 0;
				for (i = 0; i < Katalog[l-1].len; i++)
				{
					if (hbuf[i] == '<')
					{
						buf[j++] = '\b';
						buf[j++] = '"';
					}
					else buf[j++] = hbuf[i];
				}
				buf[j] = '\0';
				zlen = j;
				k = l;
				fprintf(fps, "\n%s ",buf);
				++flag;
			}
			else flag = 0;
			i = 0;
			break;
	case '\n':	hbuf[i] = '\0';
			l = atoi(hbuf);
			if (flag)
			{
				zlen += 4;
				if (MAXZLEN < zlen)
				{
					fprintf(fps, "\n");
					zlen = 0;
				}
				fprintf(fps, "%3d",l);
				lo = l;
			}
			else 
			{
				zlen += 4;
				if (MAXZLEN < zlen)
				{
					fprintf(fps, "\n");
					zlen = 0;
				}
				if (lo != l) fprintf(fps, ", %3d",l);
			}
			i = 0;
			break;
	default:	hbuf[i++] = c;
	}
}
fclose(fpo);
fclose(fps);
}
