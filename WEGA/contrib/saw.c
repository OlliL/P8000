/***********************************************************************
************************************************************************

	saw.c			(c) zft/keaw abt. basissoftware wae
				wega p8000/16-bit-teil
				c-program

				version    : 1.0
				datum      : 05.01.1990

				bearbeiter : r. kuehle

***********************************************************************

	cmdsaw: 		scc -c saw.c
				gld /lib/scrt0.o saw.o -o saw -lc

***********************************************************************

Das Programm 'saw' ist ein Filterprogramm.
Mit diesem Programm werden segmentierte Programme entsprechend
den Segmentnummern in nichtsegmentierte Programme aufgesplittet.

Die so erzeugten Programme koennen damit mit dem LOAD-Kommando einzeln
in ein entsprechenden Entwicklungsmodul (Emulator) geladen werden.

Das Programm 'saw' wird ueber folgendes wega-kommando aufgerufen:

		saw filename

und erzeugt nachfolgend aufgefuehrte files

filename.segment_attribut

filename ... entspricht dem Namen der zu splittenden Datei
segment  ... segment nummer
attribut ... c - segment enthaelt code
             d - segment enthaelt initialisierte daten
             b - segment enthaelt uninitialisierte Daten

mit nachfolgender characteristik:

object module header:
---------------------
s_magic	:	N_MAGIC1 (0xE707 nonsegmented executable)
s_imsize:	size code oder data portion
		bss = 0
s_bss	:	size bss portion
		code/data = 0
s_segt	:	0x10
s_syms	:	0
s_entry	:	0
s_flag	:	0x0001
s_codesz:	0
s_lines	:	0

segment table:
--------------
		entsprechend dem selektierten segment

**********************************************************************
*********************************************************************/


/* the valid magic numbers for the a.out are defined below: */

#define S_MAGIC1	0xE607		/* segmented executable */
#define S_MAGIC3	0xE611		/* segmented separate I & D */
#define S_MAGIC4	0xE605		/* segmented overlay */
#define N_MAGIC1	0xE707		/* nonsegmented executable */
#define N_MAGIC3	0xE711		/* nonsegmented separate I& D */
#define N_MAGIC4	0xE705		/* nonsegmented overlay */

/* valid flags in the s_flags field are defined as follow: */

#define	SF_STRIP	0x0001		/* relocation info has been stripped */

/* these definitions apply to the sg_atr field in the segment table */

#define	SG_CODE		0x0001		/* segment contains code */
#define SG_DATA		0x0002		/* contains initialized data */
#define SG_BSS		0x0004		/* contains uninitialized data */
#define	SG_STACK	0x0008		/* the segment grows downward */
#define	SG_OCODE	0x0010		/* code section is offset */
#define	SG_ODATA	0x0020		/* data section is offset */
#define SG_OBSS		0x0040		/* bss section is offset */
#define	SG_BOUND	0x0080		/* segment number bound to this section */

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
char *pname, *pname1, *pname2;		/* zeiger filename.segment.attribut */
char name[50];				/* filename.segment.attibut */
char zi_ascii[10];			/* ziffern 0 ... 9 */
char buffer[0xfffe];			/* buffer */
int fd, fd1;				/* file deskriptor */
int segno;				/* segment nummer */
int i;					/* indizes */
long zseg;				/* zaehler segment table */
long nbytes;				/* read n bytes */
long n_read;				/* tatsaechlich gelesene bytes */
long n_write;				/* tatsaechlich geschriebene bytes */
long seg_offset;			/* segment offset */
long dat_offset;			/* code und data offset */

main (argc,argv)

int	argc;
char	*argv[];

{
	printf ("(c) zft/keaw v. 1.0 splitted:\n");
	if (argc != 2)	{printf ("error - command line mismatch\n");
			exit (0);}
	pfile=argv[1];

	/* open file command line */
	fd=open (pfile,0);
	if (fd==-1)	{printf ("error - can't open %s\n",pfile);
			exit (0);}
	for (pname1=name; *pfile!='\0'; pfile++, pname1++)
		*pname1= *pfile;
	*pname1='.';
	*pname1++;
	pfile=argv[1];
	pname=name;
	nbytes=24L;
	n_read=read (fd, &s_exec, nbytes);
	if (n_read!=nbytes||n_read==0L)
			{printf ("error - %s read error\n",pfile);
			close (fd);
			exit (0);}

	/* pruefung gueltige MAGIC-Number */
	if (s_exec.s_magic!=S_MAGIC1&&s_exec.s_magic!=S_MAGIC3&&s_exec.s_magic!=S_MAGIC4&&s_exec.s_magic!=N_MAGIC1&&s_exec.s_magic!=N_MAGIC3&&s_exec.s_magic!=N_MAGIC4)
			{printf ("error - %s invalid file type\n",pfile);
			close (fd);
			exit (0);}

	/* pruefung mehr als ein Segment */
	if (s_exec.s_segt == 0x10)
			{printf ("error - %s can't splitt, 1 segment\n",pfile);
			close (fd);
			exit (0);} 
	zseg=s_exec.s_segt & 0xffff;
	seg_offset=24L;
	dat_offset=24L+s_exec.s_segt & 0xffff;

	s_exec.s_magic=N_MAGIC1;
	s_exec.s_segt=0x10;
	s_exec.s_syms=0;
	s_exec.s_entry=0L;
	s_exec.s_flag=0x0001;
	s_exec.s_codesz=0;
	s_exec.s_lines=0;

	zi_ascii[0]='0';
	zi_ascii[1]='1';
	zi_ascii[2]='2';
	zi_ascii[3]='3';
	zi_ascii[4]='4';
	zi_ascii[5]='5';
	zi_ascii[6]='6';
	zi_ascii[7]='7';
	zi_ascii[8]='8';
	zi_ascii[9]='9';
	do
	{
	lseek (fd,seg_offset,0);
	s_exec.s_imsize=0L;
	s_exec.s_bss=0L;
	nbytes=16L;
	n_read=read (fd, &segt, nbytes);
	if (n_read!=nbytes||n_read==0)
			{printf ("error3 - %s read error\n",pfile);
			close (fd);
			exit (0);}
	nbytes=segt.sg_code + segt.sg_data;
	nbytes=nbytes & 0xffff;
	lseek (fd,dat_offset,0);
	if (nbytes!=0L)
		{n_read=read (fd, buffer, nbytes);
		if ((n_read & 0xffff)!=nbytes)
			{printf ("error - %s read error\n",pfile);
			close (fd);
			exit (0);}
		s_exec.s_imsize=nbytes;}

		/*segment in filename.segment. ...*/
		segno=segt.sg_segno & 0x00ff;
		pname2=pname1;
		do
		{
		for (i= -1; segno>=0; i++)
			segno=segno-10;
		segno=segno+10;
		if (i==0)	*pname2=zi_ascii[segno];
			else	*pname2=zi_ascii[i];
		pname2++;
		} while (i!=0);

		/*attribut in filename. ... .attribut*/
		*pname2='_';
		pname2++;
		if ((segt.sg_atr & 0x0001)>0)	{*pname2='c';
						pname2++;}
		if ((segt.sg_atr & 0x0002)>0)	{*pname2='d';
						pname2++;}
		if ((segt.sg_atr & 0x0004)>0)	{*pname2='b';
						s_exec.s_bss=segt.sg_bss;
						pname2++;}
		*pname2='\0';

		fd1=creat (pname,0755);
		if (fd1==-1)	{printf ("error - can't open %s\n",pname);
				exit (0);}
		n_write=write (fd1,&s_exec,24L);
		if (n_write!=24L)	{printf ("error - %s write error\n",pname);
					close (fd1);
					break;}
		n_write=write (fd1,&segt,16L);
		if (n_write!=16L)	{printf ("error - %s write error\n",pname);
					close (fd1);
					break;}
		n_write=write (fd1,buffer,nbytes);
		if ((n_write & 0xffff)!=nbytes)	
					{printf ("error - %s write error\n",pname);
					close (fd1);
					break;}
		
		printf ("%s\n",pname);
		close (fd1);

	seg_offset=seg_offset+16L;
	dat_offset=dat_offset+nbytes;

	zseg=zseg-16L;
	} while (zseg!=0L);
	close (fd);
}
