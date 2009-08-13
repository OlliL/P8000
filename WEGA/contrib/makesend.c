/***********************************************************************
************************************************************************

	makesend		(c) zft/keaw abt. basissoftware wae
				wega p8000/16-bit-teil
				c-program

				version    : 1.0
				datum      : 20.04.1989

				bearbeiter : r. kuehle

***********************************************************************

Auf dem U8-Entwicklungsmodul koennen uber das Kommnado 'SEND' Programme
erzeugt werden (DOWNLOAD). Diese so erzeugten Programme lassen sich 
nicht mit dem Kommando 'LOAD' nicht wieder laden (UPLOAD).
Das Programm 'makesend' ist ein Filterprogramm und dient der Formatierung
von ueber das Kommando 'SEND' erzeugten Dateien mit der nachfolgenden
Moeglichkeit, diese formatierten Dateien ueber das Kommando 'LOAD' wieder
in den U8-Entwicklungsmodul laden zu koennen.
Das Programm 'makesend' wird ueber folgendes wega-kommando aufgerufen:

		makesend filename

Filename entspricht dem Namen der zu formatierenden Datei.

**********************************************************************
*********************************************************************/


/* the valid magic numbers for the a.out are defined below: */

#define X_MAGIC1	0xE507		/* 8-bit executable */

/* valid flags in the s_flags field are defined as follow: */

#define	SF_STRIP	0x0001		/* relocation info has been stripped */
#define SF_SEND		0x0010		/* module created by SEND */

/* these definitions apply to the sg_atr field in the segment table */

#define	SG_CODE		0x0001		/* segment contains code */
#define SG_DATA		0x0002		/* contains initialized data */
#define SG_BSS		0x0004		/* contains uninitialized data */
#define	SG_STACK	0x0008		/* the segment grows downward */
#define	SG_OCODE	0x0010		/* code section is offset */
#define	SG_ODATA	0x0020		/* data section is offset */
#define SG_OBSS		0x0040		/* bss section is offset */
#define	SG_BOUND	0x0080		/* segment number bound to this section */

main (argc,argv)
int	argc;
char	*argv[];
{

/* the object module header structure is as defined below for a.out object file format */

struct st1
{
	int		s_magic;	/* magic number */
	long		s_imsize;	/* size of memory image section */
	long		s_bss;		/* size of bss section */
	unsigned	s_segt;		/* size of segment table section */
	unsigned	s_syms;		/* size of symbol table section */
	long		s_entry;	/* entry point address */
	unsigned	s_flag;		/* flags word */
	unsigned	s_codesz;	/* 8-bit padded code size */
	unsigned	s_lines;	/* number of line table entries */
}; 
struct st1 s_exec;

/* the segment table is an array of the following structure. Each*/
/* entry relates to one segment in the program. For file types	 */
/* with magic number S_MAGIC1 there is a maximum of 128 such	 */
/* entries. For file types S_MAGIC3 there is a maximum of	 */
/* 256 entries. For the remaining file types there is always 1	 */
/* entry in the segment table					 */

struct st2
{
	char		sg_segno;	/* segment number */
	char		sg_coff;	/* offset/256 for code section */
	char		sg_doff;	/* offset/256 for data section */
	char		sg_boff;	/* offset/256 for bss section */
	unsigned	sg_code;	/* size of the code portion */
	unsigned	sg_data;	/* size of the data portion */
	unsigned	sg_bss;		/* size of the bss portion */
	int		sg_atr;		/* attributes */
	long		sg_unused;	/* unused */
}; 
struct st2 segt;

char *pfile;				/* zeiger kommando */
int fd;					/* file deskriptor */
long nbytes;				/* read n bytes */
long n_read;				/* tatsaechlich gelesene bytes */
long n_write;				/* tatsaechlich geschriebene bytes */

	printf ("makesend (c) zft/keaw 1.0\n");
	if (argc != 2)	{printf ("error - command line mismatch\n");
			exit (0);}
	pfile=argv[1];

	/* open file command line */
	fd=open (pfile,2);
	if (fd==-1)	{printf ("error - can't open %s\n",pfile);
			exit (0);}
	nbytes=24L;
	n_read=read (fd, &s_exec, nbytes);
	if (n_read!=nbytes||n_read==0L)
			{printf ("error - %s read error\n",pfile);
			close (fd);
			exit (0);}
	if (s_exec.s_magic!=X_MAGIC1)
			{printf ("error - %s invalid file type\n",pfile);
			close (fd);
			exit (0);}
	if ((s_exec.s_flag & SF_SEND)==0)
			{printf ("error - %s not created by SEND\n",pfile);
			close (fd);
			exit (0);} 
	s_exec.s_flag=SF_STRIP;
	s_exec.s_imsize--;

	nbytes=16L;
	n_read=read (fd, &segt, nbytes);
	if (n_read!=nbytes||n_read==0L)
			{printf ("error - %s read error\n",pfile);
			close (fd);
			exit (0);}
	segt.sg_code--;

	/* header formatierte file */
	lseek (fd, 0L, 0);
	nbytes=24L;
	n_write=write (fd, &s_exec, nbytes);
	if (n_write!=nbytes)
			{printf ("error - %s write error\n",pfile);
			close (fd);
			exit (0);}
	nbytes=16L;
	n_write=write (fd, &segt, nbytes);
	if (n_write!=nbytes)
			{printf ("error - %s write error\n",pfile);
			close (fd);
			exit (0);}
	printf ("%s - load formatted\n",pfile);
	close (fd);
}
