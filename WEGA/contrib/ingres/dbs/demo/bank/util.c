# include	<stdio.h>

struct cmd
{
	char	*cmd_msg;
	int	(*cmd_func)();
};

extern int	relclose();
extern int	new_name();
extern int	new_vorname();
extern int	new_ort();
extern int	new_str();
extern int	new_bank();
extern int	new_kunde();
extern int	new_konto();
extern int	new_grund();
extern int	new_buchung();
extern int	auszug();
extern int	auszuege();
extern int	mahnung();

struct cmd	Cmd[] =
{
	"Ende",				relclose,
	"Eingabe Vorname",		new_vorname,
	"Eingabe Name",			new_name,
	"Eingabe Strasse",		new_str,
	"Eingabe Ort",			new_ort,
	"Eingabe Grund",		new_grund,
	"Eingabe Bank",			new_bank,
	"Eingabe Kunde",		new_kunde,
	"Eingabe Konto",		new_konto,
	"Eingabe Buchung",		new_buchung,
	"Ausgabe Kontoauszug",		auszug,
	"Ausgabe Kontoauszuege",	auszuege,
	"Mahnungen",			mahnung,
};
int		Cmds = sizeof Cmd / sizeof (struct cmd);


menue()
{
	register struct cmd	*c;
	register int		i;
	char			buf[36];

try_again:
	cls(1, 1, 24, 80);
	box(1, 48, Cmds + 4, 80);
	on_underline();
	cursor(2, 54);
	printf("                         ");
	off_underline();
	for (i = 0, c = Cmd; i < Cmds; i++, c++)
	{
		cursor(i + 3, 50);
		on_revers();
		printf("%2d", i);
		off_revers();
		printf("  ");
		on_underline();
		printf("  %-23s", c->cmd_msg);
		off_underline();
	}

	cursor(1, 1);
	on_revers();
	printf("           \n");
	printf("  Auswahl  \n");
	printf("  Vorname  \n");
	printf("     Name  \n");
	printf("  Strasse  \n");
	printf("      Ort  \n");
	printf("      PLZ  \n");
	printf("    Grund  \n");
	printf("     Plus  \n");
	printf("     Bank  \n");
	printf("    Konto  \n");
	printf("   Betrag  \n");
	printf("           ");
	off_revers();
	input(2, buf);
	if ((i = atoi(buf)) >= Cmds)
	{
		error("ungueltige Auswahl");
		goto try_again;
	}
	cls(25, 1, 25, 80);
	c = &Cmd[i];
	on_revers();
	on_blinking();
	cursor(i + 3, 50);
	printf("%2d", i);
	off_blinking();
	off_revers();
	(*c->cmd_func)();
	if (!i)
	{
		cls(1, 1, 25, 80);
		cursor(1, 1);
		exit(0);
	}
}

new_datum()
{
	extern long	datum;
	extern long	time();

	time(&datum);
}

input(line, buf)
int	line;
char	*buf;
{
	register int	len;
	register int	c;
	register char	*p;

	cursor(line, 1);
	on_revers();
	on_blinking();
	putchar(' ');
	cursor(line, 11);
	putchar(' ');
	off_blinking();
	off_revers();
	cls(line, 13, line, 47);
	cursor(line, 13);
	on_cursor();
	fflush(stdout);
	p = buf;
	len = 36;
	while (--len)
	{
		if ((c = getchar()) <= 0 || c == '\n')
			break;
		*p++ = c;
	}
	*p = 0;
	if (c > 0 && c != '\n')
	{
		cls(24, 1, 24, 80);
		cursor(24, 1);
		while ((c = getchar()) > 0 && c != '\n')
			continue;
		cls(24, 1, 24, 80);
	}
	off_cursor();
	cursor(line, 1);
	on_revers();
	putchar(' ');
	cursor(line, 11);
	putchar(' ');
	off_revers();
	return (p - buf);
}

error(msg)
register char	*msg;
{
	register int	i;
	extern short	Batch;

	on_revers();
	on_blinking();
	if (!Batch)
	{
		cursor(25, 1);
		for (i = 1; i <= 79; i++)
			putchar(' ');
		cursor(25, 15);
	}
	printf("%s", msg);
	if (Batch)
		putchar('\n');
	off_blinking();
	off_revers();
	fflush(stdout);
}

struct byte
{
	char	byte0;
	char	byte1;
	char	byte2;
	char	byte3;
};

hash_konto(konto)
register struct byte	*konto;
{
	char		b0;
	char		b1;

	b0 = konto->byte0;
	b1 = konto->byte1;
	konto->byte0 = konto->byte3;
	konto->byte1 = konto->byte2;
	konto->byte2 = b1;
	konto->byte3 = b0;
}
