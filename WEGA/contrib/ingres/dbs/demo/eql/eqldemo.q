# include <stdio.h>
/*
*  Das ist ein Beispiel eines EQL-Programmes in C.
*  Es setzt die Existenz der personal-Relation inner-
*  halb der dt-Datenbank voraus.
*  Wenn Sie nach Aufforderung einen Namen eintippen,
*  wird das Programm entweder mit dem Gehalt antworten
*  oder die Nachricht ausgeben, dass der Name in der
*  Datenbank nicht enthalten ist.
*  Durch Eingabe eines '?' wird eine Liste aller ge-
*  speicherter Namen ausgegeben.
*
*  Programmbeendigung : Control-d bzw. Control-z
*
*  Compilierung  und Aufruf des Programmes:
*
*  eqc eqldemo.q
*  cc    eqldemo.c -lq
*  a.out
*/
main()
{
##	char	NAME[21];
##	char	NAM[21];
##	int	GEH;
	char	flag;

##	daba dt
##	range of p is personal
	while (eread(NAME))
	{
		if(NAME[0] == '?')
		{
##			retrieve (NAME=p.name)
##			{
				printf("%s\n",NAME);
##			}
			continue;
		}
		flag = 0;
##		retrieve (GEH = p.gehalt, NAM=p.name) where
##		p.name = NAME
##		{
		printf("Das Gehalt von %s betraegt %d\n",NAM,GEH);
			flag = 1;
##		}
	if(!flag) printf("%s ist nicht in der Datenbank\n",NAME);
	}
##	exit
}

eread(p)
char	*p;
{
	int 	c;
	printf("Name:");
	while((c = getchar()) != EOF)
	{
		if(c == '\n')
		{
			*p = 0;
			return(1);
		}
		*p++ = c;
	}
	return(0);
}
