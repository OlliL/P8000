# include	<stdio.h>
# include	<time.h>


/*******************/
/*                 */
/*     B O O T     */
/*                 */
/*******************/



/* Code, Sprung zum boot-Programm					*/

# define	B00_OFF		      0
# define	B00_LEN		      2

# define	BOOT_JMP	0032353
# define	CODE_END	0052652


/* Feld  1: Offset des boot-Programms innerhalb des boot-Sektors	*/

# define	B01_OFF		      2
# define	B01_LEN		      1

# define	BOOT_OFFSET	0000220


/* Feld  2: Systembezeichnung						*/

# define	B02_OFF		      3
# define	B02_LEN		      8


/* Feld  3: Bytes pro Sektor						*/

# define	B03_OFF		     11
# define	B03_LEN		      2

# define	BL_NORM		    512	/* normale Sektorlaenge		*/
# define	BL_MAX		   4096	/* maximale Sektorlaenge	*/


/* Feld  4: Sektoren pro Cluster					*/

# define	B04_OFF		     13
# define	B04_LEN		      1

# define	CL_BL_MAX	(BL_MAX / BL_NORM)


/* Feld  5: Anzahl reservieter Sektoren am Beginn (immer 1)		*/

# define	B05_OFF		     14
# define	B05_LEN		      2


/* Feld  6: Anzahl FAT-Kopien						*/

# define	B06_OFF		     16
# define	B06_LEN		      1


/* Feld  7: Anzahl der Verzeichniseintraege				*/

# define	B07_OFF		     17
# define	B07_LEN		      2


/* Feld  8: Anzahl der Sektoren (gesamt)				*/

# define	B08_OFF		     19
# define	B08_LEN		      2


/* Feld  9: Formatkennzeichen						*/
# define	B09_OFF		     21
# define	B09_LEN		      1

# define	FMT_S8 		0000376
# define	FMT_S9		0000374
# define	FMT_D8		0000377
# define	FMT_D9		0000375
# define	FMT_QD15	0000371
# define	FMT_QD9		0000371

/* Feld 10: Laenge FAT (in Sektoren)					*/
# define	B10_OFF		     22
# define	B10_LEN		      2


/* Feld 11: Anzahl Sektoren pro Spur					*/

# define	B11_OFF		     24
# define	B11_LEN		      2


/* Feld 12: Anzahl der Seiten (Koepfe)					*/

# define	B12_OFF		     26
# define	B12_LEN		      2


/* Feld 13: Anzahle der reservierten Sektoren				*/

# define	B13_OFF		     28
# define	B13_LEN		      2


/* globale Variablen */

# define	BOOT_adr	      0	/* boot-Sektor			*/
# define	BOOT_len	      1	/* boot-Laenge (in Bloecken)	*/



/*******************/
/*                 */
/*      F A T      */
/*                 */
/*******************/



# define	FAT_FREE	0000000
# define	FAT_START	0000002
# define	FAT_BAD		0007767
# define	FAT_LAST	0007777

/* globale Variablen */

# define	FAT_MAX		   2373
# define	FAT_LOG2	      4
# define	FAT_MASK	0007777



/*******************/
/*                 */
/*   VERZEICHNIS   */
/*                 */
/*******************/



/* Dateiname: Feld 1 des Verzeichniseintrages				*/

# define	D1_OFF		      0
# define	D1_LEN		      8

# define	DIR_EMPTY	0000000	/* unbenutzter Eintrag		*/
# define	DIR_DEL		0000345	/* Eintrag geloescht		*/
# define	DIR_FMT		0000366	/* Eintrag frisch formatiert	*/


/* Dateinamenserweiterung: Feld 2 des Verzeichniseintages		*/

# define	D2_OFF		      8
# define	D2_LEN		      3


/* file attribute: Feld 3 des Verzeichniseintrages			*/

# define	D3_OFF		     11
# define	D3_LEN		      1

# define	M_READ		0000001	/* nur lesbar			*/
# define	M_HIDDEN	0000002	/* versteckt			*/
# define	M_SYS		0000004	/* Systemdatei			*/
# define	M_PACK_NAME	0000010	/* Datentraegername		*/
# define	M_SUB_DIR	0000020	/* Unterverzeichnis		*/
# define	M_ARCHIV	0000040	/* file ist veraendert worden	*/


/* Reserviert: Feld 4 des Verzeichniseintrages				*/

# define	D4_OFF		     12
# define	D4_LEN		     10


/* Zeitangaben: Feld 5 des Verzeichniseintrages				*/

# define	D5_OFF		     22
# define	D5_LEN		      2

# define	M_SECOND	0000037	/* Maske fuer die Sekunden	*/
# define	F_SECOND	0000002	/* Divisor fuer die Sekunden	*/
# define	M_MINUTE	0003740	/* Maske fuer die Minuten	*/
# define	F_MINUTE	0000040	/* Faktor fuer die Minuten	*/
# define	M_HOUR		0174000	/* Maske fuer die Stunden	*/
# define	F_HOUR		0004000	/* Faktor fuer die Stunden	*/

# define	SECOND(s)	((s & M_SECOND) * F_SECOND)
# define	MINUTE(m)	((m & M_MINUTE) / F_MINUTE)
# define	HOUR(h)		((h & M_HOUR) / F_HOUR)
# define	TIME(h, m, s)	(h * F_HOUR + m * F_MINUTE + s / F_SECOND)


/* Datumsangaben: Feld 6 des Verzeichniseintrages			*/

# define	D6_OFF		     24
# define	D6_LEN		      2

# define	M_DAY		0000037	/* Maske fuer die Tage		*/
# define	M_MONTH		0000740	/* Maske fuer die Monate	*/
# define	F_MONTH		0000040	/* Faktor fuer die Monate	*/
# define	M_YEAR		0177000	/* Maske fuer die Jahre		*/
# define	F_YEAR		0001000	/* Faktor fuer die Jahre	*/

# define	DAY(d)		(d & M_DAY)
# define	MONTH(m)	((m & M_MONTH) / F_MONTH)
# define	YEAR(y)		((y & M_YEAR) / F_YEAR + 1980)
# define	DATE(y, m, d)	((y - 80) * F_YEAR + (m + 1) * F_MONTH + d)


/* erster FAT-Eintrag: Feld 7 des Verzeichniseintrages			*/

# define	D7_OFF		     26
# define	D7_LEN		      2


/* Laenge der Datei: Feld 8 des Verzeichniseintrages			*/

# define	D8_OFF		     28
# define	D8_LEN		      4


/* Verzeichnisstruktur */

struct ms_dir
{
	char		dir_name[D1_LEN];
	char		dir_ext[D2_LEN];
	char		dir_att;
/*	char		dir_reserverd[D4_LEN];				*/
	unsigned short	dir_time;
	unsigned short	dir_date;
	short		dir_fat;
	long		dir_len;
};
typedef struct ms_dir		MS_DIR;

/* globale Variablen */

# define	DIR_SIZE	     32
# define	DIR_BL		(BL_NORM / DIR_SIZE)
# define	DIR_MAX		    224



/*******************/
/*                 */
/*     D A T A     */
/*                 */
/*******************/



# define	BYTE_MASK	0000377
# define	BYTE_LOG2	      8
# define	WORD_MASK	0177777
# define	WORD_LOG2	     16
# define	LONG_MASK	  65535L

# define	cl_to_bl(cl)	((cl - FAT_START) * CL_BL + DATA_adr)
# define	fat_adr(fat)	((fat * 3) / 2)
# define	next_fat(fat)	FAT[fat]

# define	S8 		      1
# define	S9		      2
# define	D8		      3
# define	D9		      4
# define	QD15		      5
# define	QD9		      6
# define	DEV_NO		      7

char		DEV_S8[] =	"/dev/fd148ss8";
char		DEV_S9[] =	"/dev/fd148ss9";
char		DEV_D8[] =	"/dev/fd148ds8";
char		DEV_D9[] =	"/dev/fd148ds9";
char		DEV_QD15[] =	"/dev/fd196ds15";
char		DEV_QD9[] =	"/dev/fd196ds9";

struct floppy_tab
{
	char	*ft_dev;
	char	ft_fmt;
	char	ft_sectors;
	char	ft_sides;
	char	ft_cl_bl;
	short	ft_bls;
	short	ft_dirs;
};
typedef struct floppy_tab	FL_TAB;

FL_TAB		TAB[] =
{/*----------------------------------------------------------------------------
	device       format    sectors    sides  cluster   blocks     dirs
-----------------------------------------------------------------------------*/
	DEV_D9,      FMT_D9,         0,       0,       0,       0,       0,
	DEV_S8,      FMT_S8,         8,       1,       1,     320,      64,
	DEV_S9,      FMT_S9,         9,       1,       1,     360,      64,
	DEV_D8,      FMT_D8,         8,       2,       2,     640,     112,
	DEV_D9,      FMT_D9,         9,       2,       2,     720,     112,
	DEV_QD15,    FMT_QD15,      15,       2,       1,    2400,     224,
	DEV_QD9,     FMT_QD9,        9,       2,       2,    1440,     112
};/*-------------------------------------------------------------------------*/


/* globale Variablen */
int		Diskette;		/* Filedeskriptor		*/
char		Sektor[BL_MAX + 1];	/* Puffer			*/
MS_DIR		Direc[DIR_MAX];		/* Verzeichnisse		*/
short		FAT[FAT_MAX];		/* file allocation table	*/
int		BL = BL_NORM;		/* Sektorlaenge (in Bytes)	*/
int		BLS;			/* Anzahl Sektoren (gesamt)	*/
int		CL;			/* Clusterlaenge (in Bytes)	*/
int		CLS;			/* Anzahl der Cluster		*/
int		CL_BL;			/* Sektoren pro Cluster		*/
int		FAT_adr = BOOT_adr + BOOT_len;
int		FAT_len;		/* Laenge FAT (in Sektoren)	*/
int		FAT_copy;		/* Anzahl der FAT-Kopien	*/
int		FAT_ptr = FAT_START;	/* FAT-Freispeicherzeiger	*/
int		DIR_adr;		/* Startsektor Verzeichnis	*/
int		DIR_len;		/* Laenge Verzeichnis		*/
int		DIRS;			/* Anzahle Verzeichniseintraege	*/
int		DATA_adr;		/* Startadresse Datenbereich	*/
int		DATA_len;		/* Laenge Datenbereich		*/
short		Ascii;			/* cr/lf conversion flag	*/
short		Big;			/* upper/lower conversion flag	*/
short		Create;			/* create-flag			*/
short		Dev_no;			/* unix-Geraetenummer		*/
char		*Device;		/* unix-Geraet			*/
char		*Floppy;		/* floppy-type (dcp-Geraet)	*/
short		Header;			/* Print out header-Flag	*/
short		Table;			/* table of contents-flag	*/
short		Verbose;		/* verbose-Flag			*/
short		Xtract;			/* extract-flag			*/
short		Kind;			/* Formatkennzeichen		*/
char		**File_list;		/* gewuenschte file-Liste	*/
short		InOut;			/* E/A von stdin bzw. stdout	*/
char		IObuf[BUFSIZ];		/* E/A Puffer			*/


main(argc, argv)
int		argc;
char		**argv;
{
	register char		**av;
	register char		*a;

	if (--argc == 0)
	{
usage:
		fprintf(stderr, "USAGE:\t%s [-0abcdfhtvx] [typ] [dev] [file_list]\n", argv[0]);
		fprintf(stderr, "-\tin/output from stdin/out\n");
		fprintf(stderr, "0\tdevice number 0\n");
		fprintf(stderr, "a\tcr/lf conversion of ascii files\n");
		fprintf(stderr, "b\tupper/lower conversion of file names\n");
		fprintf(stderr, "c\tcreate %s floppy\n", argv[0]);
		fprintf(stderr, "d typ\t%s device (default: q9)\n", argv[0]);
		fprintf(stderr, "f dev\tunix device (default: %s)\n", DEV_QD9);
		fprintf(stderr, "\t\ts8  ss/sd 40 tracks/ 8 sectors 160 KB from %s\n", DEV_S8);
		fprintf(stderr, "\t\ts9  ss/sd 40 tracks/ 9 sectors 180 KB from %s\n", DEV_S9);
		fprintf(stderr, "\t\td8  ds/sd 40 tracks/ 8 sectors 320 KB from %s\n", DEV_D8);
		fprintf(stderr, "\t\td9  ds/sd 40 tracks/ 9 sectors 360 KB from %s\n", DEV_D9);
		fprintf(stderr, "\t\tq9  ds/dd 80 tracks/ 9 sectors 720 KB from %s\n", DEV_QD9);
		fprintf(stderr, "\t\tq15    hd 80 tracks/15 sectors 1,2 MB from %s\n", DEV_QD15);
		fprintf(stderr, "\t\tx   user device (interactive defined)\n");
		fprintf(stderr, "h\theader of %s floppy\n", argv[0]);
		fprintf(stderr, "t\ttable (list) %s floppy\n", argv[0]);
		fprintf(stderr, "v\tverbose\n");
		fprintf(stderr, "x\textract %s floppy [file_list]\n", argv[0]);
		exit(0);
	}
	av = &argv[1];
	a = *av++;
	do
	{
		switch (*a)
		{
		  case '-':
			InOut++;
			break;

		  case '0':
			Dev_no++;
			break;

		  case 'a':
			Ascii++;
			break;

		  case 'b':
			Big++;
			break;

		  case 'c':
			Create++;
			break;

		  case 'd':
			if (--argc == 0)
			{
				fprintf(stderr, "missing floppy type\n");
				goto usage;
			}
			Floppy = *av++;
			break;

		  case 'f':
			if (--argc == 0)
			{
				fprintf(stderr, "missing device\n");
				goto usage;
			}
			Device = *av++;
			break;

		  case 'h':
			Header++;
			break;

		  case 't':
			Table++;
			break;

		  case 'v':
			Verbose++;
			break;

		  case 'x':
			Xtract++;
			break;

		  default:
			fprintf(stderr, "unknown flag -%c\n", *a);
			goto usage;
		}
	} while (*++a);

	File_list = av;
	switch (Create + Xtract + Table)
	{
	  case 0:
		fprintf(stderr, "missing function key\n");
		goto usage;

	  case 1:
		break;

	  default:
		fprintf(stderr, "too many function keys\n");
		goto usage;
	}

	if (Create)
		create_floppy();
	else
	{
		if (Floppy)
		{
			fprintf(stderr, "floppy type not expected\n");
			goto usage;
		}
		get_boot();
		get_dir();
		if (Xtract)
			xtract_floppy();
		else
			table_floppy();
	}
	close(Diskette);
	exit(0);
}


create_floppy()
{
	register char		**wanted;
	register char		*file;


	Big++;
	put_boot();
	DIRS = 0;
	if (*(wanted = File_list))
		while (file = *wanted++)
			create(file);
	put_dir();
	put_fat();
}


xtract_floppy()
{
	register MS_DIR		*d;
	register int		k;
	register int		att;
	register char		*name;
	extern char		*unix_name();

	get_fat();
	for (d = Direc, k = DIRS; k; --k, d++)
	{
		name = unix_name(d->dir_name);
		att = d->dir_att;
		if ((att & M_SUB_DIR) || (att & M_PACK_NAME) || notwanted(name))
			continue;
		if (Verbose)
			fprintf(stderr, "x %-12s %12ld\n", name, d->dir_len);
		xtract(name, d->dir_len, d->dir_fat);
	}
}


table_floppy()
{
	register MS_DIR		*d;
	register int		k;
	register int		att;
	register long		i;
	extern char		*unix_name();

	for (d = Direc, k = DIRS; k; --k, d++)
	{
		fprintf(stderr, "t %-12s %12ld", unix_name(d->dir_name), d->dir_len);
		if (Verbose)
		{
			i = d->dir_time;
			i &= WORD_MASK;
			fprintf(stderr, " %2ld:%2ld:%2ld", HOUR(i), MINUTE(i), SECOND(i));
			i = d->dir_date ;
			i &= WORD_MASK;
			fprintf(stderr, " %2ld.%2ld %4ld", DAY(i), MONTH(i), YEAR(i));
			att = d->dir_att;
			if (att & M_HIDDEN)
				fprintf(stderr, " HIDDEN");
			if (att & M_READ)
				fprintf(stderr, " READ-ONLY");
			if (att & M_SYS)
				fprintf(stderr, " SYS-DATEI");
			if (att & M_PACK_NAME)
				fprintf(stderr, " ARCHIV-NAME");
			if (att & M_SUB_DIR)
				fprintf(stderr, " SUB_DIR");
		}
		putc('\n', stderr);
	}
}


create(file)
register char	*file;
{
	register MS_DIR		*d;
	register char		*p;
	register FILE		*f;
	register int		c;
	register int		fat;
	register int		size;
	register struct tm	*tm;
	long			len;
	extern char		*dos_name();
	extern struct tm	*localtime();
	extern long		time();

	if (InOut)
		f = stdin;
	else if ((f = fopen(file, "r")) == (FILE *) 0)
	{
		fprintf(stderr, "cannot open input `%s'\n", file);
		return;
	}
	setbuf(f, IObuf);
	if (Verbose)
		fprintf(stderr, "a %-50s ", file);
	file = dos_name(file);
	if (Verbose)
		fprintf(stderr, "%-12s\n", file);
	d = &Direc[DIRS];
	for (p = d->dir_name; p < &d->dir_att; )
		*p++ = ' ';
	for (p = d->dir_name; *file && *file != '.'; )
		*p++ = *file++;
	if (*file++ == '.')
		for (p = d->dir_ext; *file; )
			*p++ = *file++;
	d->dir_att = 0;
	len = time((long *) 0);
	tm = localtime(&len);
	d->dir_time = TIME(tm->tm_hour, tm->tm_min, tm->tm_sec);
	d->dir_date = DATE(tm->tm_year, tm->tm_mon, tm->tm_mday);
	d->dir_fat = FAT_ptr;
	len = 0;
	while ((c = getc(f)) >= 0)
	{
restart:
		size = 0;
		p = Sektor;
		if (FAT_ptr >= CLS)
		{
			fprintf(stderr, "device full\n");
			if (!InOut)
				fclose(f);
			for (FAT_ptr = fat = d->dir_fat; fat < CLS; fat++)
				FAT[fat] = FAT_FREE;
			clear_sector();
			return;
		}
		do
		{
			if (Ascii > 0 && c == '\n')
				size++, *p++ = '\r';
			if (Ascii < 0)
				Ascii = 1;
			if (!Ascii || c != '\r')
				size++, *p++ = c;
		} while (size < CL && (c = getc(f)) >= 0);
		fat = FAT_ptr++;
		FAT[fat] = FAT_ptr;
		put_sector(cl_to_bl(fat), CL_BL);
		clear_sector();
		len += size;
		if (size > CL)
		{
			Ascii = -1;
			c = '\n';
			--len;
			goto restart;
		}
	}
	if (len)
		FAT[fat] = FAT_LAST;
	else
		d->dir_fat = FAT_FREE;
	d->dir_len = len;
	if (!InOut)
		fclose(f);
	DIRS++;
}


xtract(file, cnt, fat)
register char	*file;
long		cnt;
register int	fat;
{
	register FILE		*f;
	register int		size;
	register char		*p;

	if (InOut)
		f = stdout;
	else if ((f = fopen(file, "w")) == (FILE *) 0)
	{
		fprintf(stderr, "cannot open output `%s'\n", file);
		return;
	}
	setbuf(f, IObuf);
	while (cnt)
	{
		size = CL;
		if (fat == FAT_FREE)
			goto done;
		if (fat < FAT_START || fat >= CLS)
		{
			fprintf(stderr, "unexpected file allocation: %d\n", fat);
			goto done;
		}
		if (cnt < CL)
			size = cnt;
		get_sector(cl_to_bl(fat), CL_BL);
		cnt -= size;
		for (p = Sektor; size; --size, p++)
			if (!Ascii || *p != '\r')
				putc(*p, f);
		switch (fat = next_fat(fat))
		{
		  case FAT_LAST:
			goto done;

		  case FAT_BAD:
			fprintf(stderr, "unexpected bad block: %d\n", cl_to_bl(fat));
			goto done;
		}
	}
done:
	if (!InOut)
		fclose(f);
}


# define	get_8_bit(o)	 (Sektor[o] & BYTE_MASK)
# define	put_8_bit(w, o)	 Sektor[o] = (w) & BYTE_MASK


get_16_bit(off)
register int	off;
{
	register int		w1;
	register int		w2;

	w1 = get_8_bit(off++);
	w2 = get_8_bit(off);
	w1 += w2 << BYTE_LOG2;
	return (w1);
}


put_16_bit(word, off)
register int	word;
register int	off;
{
	put_8_bit(word, off++);
	put_8_bit(word >> BYTE_LOG2, off);
}


long	get_32_bit(off)
register int	off;
{
	long			w1;
	long			w2;

	w1 = get_16_bit(off);
	w1 &= LONG_MASK;
	w2 = get_16_bit(off + 2);
	w2 &= LONG_MASK;
	w1 += w2 << WORD_LOG2;
	return (w1);
}


put_32_bit(word, off)
register long	word;
register int	off;
{
	register int		w;

	w = word & LONG_MASK;
	put_16_bit(w, off);
	w = (word >> WORD_LOG2) & LONG_MASK;
	put_16_bit(w, off + 2);
}


get_char(off, adr, len)
register int	off;
register char	*adr;
register int	len;
{
	while (len--)
		*adr++ = Sektor[off++];
}


put_char(off, adr, len)
register int	off;
register char	*adr;
register int	len;
{
	while (len--)
		Sektor[off++] = *adr++;
}


get_sector(adr, no)
register int	adr;
register int	no;
{
	register int		len;
	long			off;
	extern long		lseek();

	off = adr * ((long) BL);
	if (lseek(Diskette, off, 0) != off)
	{
		fprintf(stderr, "seek error\n");
		exit(1);
	}
	len = no * BL;
	if (read(Diskette, Sektor, len) != len)
	{
		fprintf(stderr, "read error\n");
		exit(1);
	}
}


put_sector(adr, no)
register int	adr;
register int	no;
{
	register int		len;
	long			off;
	extern long		lseek();

	off = adr * ((long) BL);
	if (lseek(Diskette, off, 0) != off)
	{
		fprintf(stderr, "seek error\n");
		exit(1);
	}
	len = no * BL;
	if (write(Diskette, Sektor, len) != len)
	{
		fprintf(stderr, "write error\n");
		exit(1);
	}
}


clear_sector()
{
	register int		i;
	register char		*s;

	s = Sektor;
	for (i = 0; i < BL_MAX; i++)
		*s++ = 0;
}


get_boot()
{
	register int		sectors;
	register int		sides;
	register int		tracks;
	register char		*dev;

	if (Device)
		dev = Device;
	else
	{
		dev = DEV_D9;
		if (Dev_no)
			dev[DEV_NO] = '0';
	}
	if ((Diskette = open(dev, 0)) < 0)
	{
open_error:
		fprintf(stderr, "cannot open input `%s'\n", dev);
		exit(1);
	}
	get_sector(BOOT_adr, BOOT_len);
	sectors = get_16_bit(B11_OFF);
	sides = get_16_bit(B12_OFF);
	BL = get_16_bit(B03_OFF);
	if (BL != BL_NORM)
	{
		fprintf(stderr, "unexpected sector length (in bytes): %d\n", BL);
		exit(1);
	}
	BLS = get_16_bit(B08_OFF);
	if (BLS < 100)
	{
		fprintf(stderr, "unexpected number of sectors: %d\n", BLS);
		exit(1);
	}
	CL_BL = get_8_bit(B04_OFF);
	if (CL_BL <= 0 || CL_BL > CL_BL_MAX)
	{
		fprintf(stderr, "unexpected sectors per cluster: %d\n", CL_BL);
		exit(1);
	}
	CL = CL_BL * BL;
	if (CL > BL_MAX)
	{
		fprintf(stderr, "unexpected cluster length (in bytes): %d\n", CL);
		exit(1);
	}
	FAT_len = get_16_bit(B10_OFF);
	if (FAT_len <= 0 || FAT_len > CL_BL_MAX)
	{
		fprintf(stderr, "unexpected fat length (in sectors): %d\n", FAT_len);
		exit(1);
	}
	FAT_copy = get_8_bit(B06_OFF);
	if (FAT_copy <= 0 || FAT_copy > 2)
	{
		fprintf(stderr, "unexpected number of fat copies: %d\n", FAT_copy);
		exit(1);
	}
	DIR_adr = FAT_adr + FAT_len * FAT_copy;
	DIRS = get_16_bit(B07_OFF);
	if (DIRS <= 0 || DIRS % DIR_BL || DIRS > DIR_MAX)
	{
		fprintf(stderr, "unexpected number of directories: %d\n", DIRS);
		exit(1);
	}
	DIR_len = DIRS / (BL / DIR_SIZE);
	DATA_adr = DIR_adr + DIR_len;
	DATA_len = BLS - DATA_adr;
	CLS = (DATA_len / CL_BL) + FAT_START;
	if (CLS > FAT_MAX)
	{
		fprintf(stderr, "unexpected number of clusters: %d\n", CLS);
		exit(1);
	}

	tracks = BLS / (sides * sectors);
	Kind = (tracks == 80)? QD15: ((sides == 1)? S8: D8);
	if (sectors == 9)
		Kind++;

	if (!Device)
	{
		close (Diskette);
		dev = TAB[Kind].ft_dev;
		if (Dev_no)
			dev[DEV_NO] = '0';
		if ((Diskette = open(dev, 0)) < 0)
			goto open_error;
	}

	if (Header)
		header(sides, tracks, sectors, dev);
}


header(sides, tracks, sectors, dev)
register int	sides;
register int	tracks;
register int	sectors;
register char	*dev;
{
	fprintf(stderr, "FLOPPY\t%d-sided %d-tracks %d-sectors\n", sides, tracks, sectors);
	fprintf(stderr, "%8s\n", &Sektor[B02_OFF]);
	fprintf(stderr, "--------\n");
	fprintf(stderr, "BOOT\t  block %2d %4d blocks\n", BOOT_adr, BOOT_len);
	fprintf(stderr, "FAT\t  block %2d %4d blocks # %4d\n", FAT_adr, FAT_len, FAT_copy);
	fprintf(stderr, "DIR\t  block %2d %4d blocks # %4d\n", DIR_adr, DIR_len, DIRS);
	fprintf(stderr, "DATA\t  block %2d %4d blocks\n", DATA_adr, DATA_len);
	fprintf(stderr, "CLUSTER\t%4d bytes %4d blocks # %4d\n", CL, CL_BL, CLS);
	fprintf(stderr, "BLOCK\t%4d bytes %4d blocks\n", BL, BLS);
	fprintf(stderr, "DEV\t%s\n", dev);
	fprintf(stderr, "--------\n");
}


put_boot()
{
	register FL_TAB		*p;
	register int		i;
	register int		j;
	register int		k;
	register int		sides;
	register int		tracks;
	register int		sectors;

	if (Floppy)
	{
		switch (*Floppy)
		{
		  case 'x':
		  case 'X':
			j = 0;
			break;

		  case 's':
		  case 'S':
			j = S8;
			break;

		  case 'q':
		  case 'Q':
			j = QD15;
			break;

		  case 'd':
		  case 'D':
		  default:
			j = D8;
		}
		if (Floppy[1] == '9')
			j++;
	}
	else
		j = QD9;

	p = &TAB[j];
	if (!Device)
	{
		Device = p->ft_dev;
		if (Dev_no)
			Device[DEV_NO] = '0';
	}
	if ((Diskette = open(Device, 1)) < 0)
	{
		fprintf(stderr, "cannot open output `%s'\n", Device);
		exit(1);
	}

	put_16_bit(BOOT_JMP, B00_OFF);
	put_8_bit(BOOT_OFFSET, B01_OFF);
	put_char(B02_OFF, "UNIX-DOS", B02_LEN);
	put_16_bit(BL = BL_NORM, B03_OFF);
	if ((i = p->ft_sides) == 0)
		while ((j = i = interactive("-------Sides-------")) > 2)
			;
	put_16_bit(sides = i, B12_OFF);
	if ((i = p->ft_sectors) == 0)
		k = i = interactive("-Sectors per Track-");
	put_16_bit(sectors = i, B11_OFF);
	if ((i = p->ft_bls) == 0)
		while ((i = interactive("-------Tracks------") * j * k) < 100)
			;
	put_16_bit(BLS = i, B08_OFF);
	tracks = i / (sides * sectors);
	if ((i = p->ft_cl_bl) == 0)
		while ((i = interactive("Sectors per Cluster")) > CL_BL_MAX)
			;
	put_8_bit(CL_BL = i, B04_OFF);
	put_16_bit(1, B05_OFF);
	put_8_bit(FAT_copy = 2, B06_OFF);
	if ((i = p->ft_dirs) == 0)
		while ((i = interactive("----Directories----")) % DIR_BL || i > DIR_MAX)
			;
	put_16_bit(DIRS = i, B07_OFF);
	put_8_bit(Kind = p->ft_fmt, B09_OFF);
	put_16_bit(0, B13_OFF);
	CL = CL_BL * BL;
	DIR_len = DIRS / (BL / DIR_SIZE);
	i = 1024 * CL_BL + 3 * FAT_copy;
	j = (BLS - BOOT_len - DIR_len + 2 * CL_BL) * 3 + i - 1;
	put_16_bit(FAT_len = j / i, B10_OFF);
	DIR_adr = FAT_adr + FAT_len * FAT_copy;
	DATA_adr = DIR_adr + DIR_len;
	DATA_len = BLS - DATA_adr;
	CLS = DATA_len / CL_BL + FAT_START;
	put_sector(BOOT_adr, BOOT_len);
	clear_sector();
	if (Header)
		header(sides, tracks, sectors, Device);
}


get_dir()
{
	register int		i;
	register MS_DIR		*d;
	register int		off;
	register char		*dp;
	register int		di;

	i = DIRS;
	DIRS = 0;
	off = BL;
	for (d = Direc; i; --i)
	{
		if (off == BL)
		{
			get_sector(DIR_adr++, 1);
			off = 0;
		}
		get_char(off, d, D1_LEN + D2_LEN + D3_LEN);
		off += D1_LEN + D2_LEN + D3_LEN + D4_LEN;
		d->dir_time = get_16_bit(off);
		off += D5_LEN;
		d->dir_date = get_16_bit(off);
		off += D6_LEN;
		d->dir_fat = get_16_bit(off);
		off += D7_LEN;
		d->dir_len = get_32_bit(off);
		off += D8_LEN;
		di = *d->dir_name;
		di &= BYTE_MASK;
		if (di == DIR_EMPTY || di == DIR_DEL || di == DIR_FMT)
			continue;
		if (Big)
			for (dp = d->dir_name, di = D1_LEN + D2_LEN; di; --di, dp++)
				if (*dp >= 'A' && *dp <= 'Z')
					*dp += 'a' - 'A';
		DIRS++;
		d++;
	}
}


put_dir()
{
	register int		i;
	register MS_DIR		*d;
	register int		off;

	i = DIRS;
	off = 0;
	for (d = Direc; i; --i)
	{
		put_char(off, d, D1_LEN + D2_LEN + D3_LEN);
		off += D1_LEN + D2_LEN + D3_LEN + D4_LEN;
		put_16_bit(d->dir_time, off);
		off += D5_LEN;
		put_16_bit(d->dir_date, off);
		off += D6_LEN;
		put_16_bit(d->dir_fat, off);
		off += D7_LEN;
		put_32_bit(d->dir_len, off);
		off += D8_LEN;
		d++;
		if (off == BL || i == 1)
		{
			DIR_len--;
			put_sector(DIR_adr++, 1);
			clear_sector();
			off = 0;
		}
	}
	for (i = DIR_len; i; --i)
		put_sector(DIR_adr++, 1);
}


get_fat()
{
	register int		fat;
	register int		i;

	get_sector(FAT_adr, FAT_len);
	for (i = 0; i < CLS; i++)
	{
		fat = get_16_bit(fat_adr(i));
		FAT[i] = ((i & 01)? fat >> FAT_LOG2: fat) & FAT_MASK;
	}
}


put_fat()
{
	register int		i;
	register int		off;

	FAT[0] = 07400 | Kind;
	FAT[1] = FAT_MASK;
	for (i = 0; i < CLS; i++)
	{
		FAT[i] |= (FAT[i + 1] & 017) << (WORD_LOG2 - FAT_LOG2);
		FAT[i + 1] >>= FAT_LOG2;
		put_16_bit(FAT[i], off = fat_adr(i));
		put_8_bit(FAT[++i], off + sizeof (short));
	}
	for (i = FAT_copy; i; --i)
	{
		put_sector(FAT_adr, FAT_len);
		FAT_adr += FAT_len;
	}
}


notwanted(file)
register char	*file;
{
	register char	**wantlist;

	if (!*(wantlist = File_list))
		return (0);

	while (*wantlist)
		if (equal(file, *wantlist++))
			return (0);
	return (1);
}


equal(file, name)
register char	*file;
register char	*name;
{
	for ( ; ; )
	{
		if (*name != *file++)
			return (0);
		if (*name++ == 0)
			return (1);
	}

}

char	*unix_name(name)
register char	*name;
{
	register char		*b;
	register int		i;
	register int		first;
	static char		buf[D1_LEN + D2_LEN + 2];

	b = buf;
	for (i = D1_LEN; i; --i, name++)
		if (*name != ' ')
			*b++ = *name;
	first = 0;
	for (i = D2_LEN; i; --i, name++)
		if (*name != ' ')
		{
			if (first++ == 0)
				*b++ = '.';
			*b++ = *name;
		}
	*b = 0;

	return (buf);
}


char	*dos_name(name)
register char	*name;
{
	register char		*p;
	register int		point;
	register int		cnt;

	for (p = name; *p; p++)
		continue;
	while (*--p == '/' && p >= name)
		continue;
	*++p = 0;
	if (p == name)
	{
		fprintf(stderr, "illegal null name\n");
		exit(1);
	}
	while (p > name && *--p != '/')
		continue;
	if (p > name || *p == '/')
		p++;
	name = p--;
	cnt = point = 0;
	while (*++p)
	{
		if (*p == '.')
		{
			if (point)
			{
				*p = 0;
				break;
			}
			point++;
			continue;
		}
		if (point)
			point++;
		else
			cnt++;
	}
	if (point)
	{
		if (point > D2_LEN + 1)
			name[cnt + D2_LEN + 1] = 0;
		if ((cnt -= D1_LEN) > 0)
			name += cnt;
	}
	else
		if ((cnt -= D1_LEN + D2_LEN) > 0)
			name += cnt;
	if (Big)
	{
		for (p = name; *p; *p++)
			if (*p >= 'a' && *p <= 'z')
				*p -= 'a' - 'A';
	}
	return (name);
}


interactive(prompt)
register char	*prompt;
{
	register int		c;
	register char		*p;
	char			linebuf[100];

	c = 0;
	while (c <= 0)
	{
		fprintf(stderr, "<--%s-->   ", prompt);
		fflush(stdout);
		p = linebuf;
		while ((c = getchar()) >= 0)
			if (c == '\n')
				break;
			else
				*p++ = c;
		*p = 0;
		c = atoi(linebuf);
	}
	return (c);
}
