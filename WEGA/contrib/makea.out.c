/***********************************************************************
************************************************************************

	makea.out		(c) zft/keaw abt. basissoftware wae
				wega p8000/16-bit-teil
				c-program

				version    : 1.0
				datum      : 30.06.1988

				bearbeiter : r. kuehle

***********************************************************************

das programm makea.out dient der formatierung von ausfuehrbaren ausgabe-
dateien in a.out-format mit der nachfolgenden moeglichkeit der program-
mierung von prom's unter dem programmsystem pburn.
das programm makea.out wird ueber folgendes wega-kommando aufgerufen:
		makea.out filename
filename entspricht dem namen der zu formatierenden ausfuehrbaren
ausgabedatei. makea.out erzeugt eine a.out-datei mit dem namen
		filename.a

**********************************************************************
*********************************************************************/


/* the valid magic numbers for the a.out are defined below: */

#define X_MAGIC1	0xE507		/* 8-bit executable */

/* valid flags in the s_flags field are defined as follow: */

#define	SF_STRIP	0x0001		/* relocation info has been stripped */
#define	SF_Z80		0x0008		/* u880 program */

/* these definitions apply to the sg_atr field in the segment table */

#define	SG_CODE		0x0001		/* segment contains code */
#define SG_DATA		0x0002		/* contains initialized data */
#define SG_BSS		0x0004		/* contains uninitialized data */
#define	SG_STACK	0x0008		/* the segment grows downward */
#define	SG_OCODE	0x0010		/* code section is offset */
#define	SG_ODATA	0x0020		/* data section is offset */
#define SG_OBSS		0x0040		/* bss section is offset */
#define	SG_BOUND	0x0080		/* segment number bound to this section */

#define rmode 0				/* read mode */
#define wmode 1				/* write mode */

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
char *pname;				/* zeiger filename.p */
int fdi,fda;				/* file deskriptor */
long nbytes;				/* read n bytes */
long n_read;				/* tatsaechlich gelesene bytes */
long n_write;				/* tatsaechlich geschriebene bytes */
char aname[30];				/* name a.out-datei */
char buffer[512];			/* puffer */
long size;				/* laenge datei */

	size=0;
	printf ("makea.out (c) zft/keaw 1.0\n");
	if (argc != 2)	{printf ("error - command line mismatch\n");
			exit (0);}
	pfile=argv[1];

	/* open file command line */
	fdi=open (pfile,rmode);
	if (fdi==-1)	{printf ("error - can't open %s\n",pfile);
			exit (0);}
	for (pname=aname; *pfile!='\0'; pfile++,pname++)
		*pname= *pfile;
	*pname= '.';
	pname++;
	*pname= 'a';
	pname++;
	*pname= '\0';
	pname=aname;

	/* open filename.p */
	fda=creat (pname,0755);
	if (fda==-1)	{printf ("error - can't open %s\n",pname);
			exit (0);}
	lseek (fda, 40L, 0);
	nbytes=512;
	do
	{
	n_read=read (fdi,buffer,nbytes);
	if (n_read==-1)	{printf ("error - %s read error\n",pfile);
			close (fdi);
			exit(0);}
	if (n_read==0)	break;
	n_write=write (fda,buffer,n_read);
	if (n_read!=n_write)	{printf ("error - %s write error\n",pname);
				close (fdi);
				close (fda);
				exit(0);}
	size=size+n_read;
	} while (n_read!=0);
	if (size>0xffff)	{printf ("error - segment overlay %s\n",pfile);
				close (fdi);
				close (fda);
				exit (0);}

	s_exec.s_magic=X_MAGIC1;
	s_exec.s_imsize=size;
	s_exec.s_bss=0;
	s_exec.s_segt=0x10;
	s_exec.s_syms=0;
	s_exec.s_entry=0;
	s_exec.s_flag=0x0001;
	s_exec.s_codesz=0;
	s_exec.s_lines=0;

	segt.sg_segno=0;
	segt.sg_coff=0;
	segt.sg_doff=0;
	segt.sg_boff=0;
	segt.sg_code=size;
	segt.sg_data=0;
	segt.sg_bss=0;
	segt.sg_atr=1;
	segt.sg_unused=0;

	/* header filename.p */
	lseek (fda, 0L, 0);
	nbytes=24;
	n_write=write (fda,&s_exec,nbytes);
	if (n_write!=nbytes)	{printf ("error - %s write error\n",pname);
				close (fdi);
				close (fda);
				exit (0);}
	nbytes=16;
	n_write=write (fda,&segt,nbytes);
	if (n_write!=nbytes)	{printf ("error - %s write error\n",pname);
				close (fdi);
				close (fda);
				exit (0);}
	printf ("%s - %d bytes loaded\n",pname,size);
	close (fdi);
	close (fda);

}
