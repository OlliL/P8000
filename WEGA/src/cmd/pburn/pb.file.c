/************************************************************************
*									*
*			pburn - prom programmer				*
*									*
*	pb2.file.c	: wega p8000 16-bit-teil			*
*				c-quelle				*
*									*
************************************************************************/

/* the valid magic numbers for the a.out are defined below: */

#define	S_MAGIC1	0xE607		/* segmented executable */
#define S_MAGIC3	0xE611		/* segemented separate I & D */
#define S_MAGIC4	0xE605		/* segmented overlay */
#define N_MAGIC1	0xE707		/* nonsegmented executable */
#define N_MAGIC3	0xE711		/* nonsegmented separate I & D */
#define N_MAGIC4	0xE705		/* nonsegmented overlay */
#define X_MAGIC1	0xE507		/* 8-bit executable */
#define X_MAGIC3	0xE511		/* 8-bit separate I & D */
#define X_MAGIC4	0xE505		/* 8-bit overlay */

/* valid flags in the s_flags field are defined as follow: */

#define	SF_STRIP	0x0001		/* relocation info has been stripped */
#define	SF_OPREP	0x0002		/* module changed by OPREP program */
#define	SF_Z8		0x0004		/* u8 program */
#define	SF_Z80		0x0008		/* u880 program */
#define	SF_SEND		0x0010		/* module created by SEND */
#define	SF_7FSTK	0x0020		/* stack resides in segment 0x7f */

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
#define true 1
#define false 0
#define fil "\n\nfile"
#define to  "to"

/*	identifier file
	--------------- */
extern char namef[50], namee1[20], bre, sect;
extern int kenns, kennfo, kennB, kennw, kennD;
extern long segnr, begadrf, begadre1, begadre2, entryf, lange1, lange2, langf, prlang;
extern int answer, next;

/*	identifier error
	---------------- */
extern int error;
extern int warning;
extern int eof;

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
	unsigned char	sg_segno;	/* segment number */
	unsigned char	sg_coff;	/* offset/256 for code section */
	unsigned char	sg_doff;	/* offset/256 for data section */
	unsigned char	sg_boff;	/* offset/256 for bss section */
	unsigned	sg_code;	/* size of the code portion */
	unsigned	sg_data;	/* size of the data portion */
	unsigned	sg_bss;		/* size of the bss portion */
	int		sg_atr;		/* attributes */
	long		sg_unused;	/* unused */
}; 
struct st2 segt, bsegt;

int fd,fd1;				/* file deskriptor */
long nbytes;				/* read n bytes */
long n_read;				/* tatsaechlich gelesene bytes */
long n_write;				/* tatsaechlich geschriebene bytes */
int offset256;				/* offset/256 */
int zsegt;
unsigned zhl1, zhl2;
long adr, len;
int cfd, cfd1;
long aoffset, hoffset, boffset, offset;
char progbuf[0xfffe];			/*program buffer*/
char progffff;				/*adr ffff prom e27512 program buffer*/
char *peprombuf;
char *pprogbuf;
char *p_progbuf;
char eprombuf[0xfffe];			/*prom buffer*/
char epromffff;				/*adr ffff prom e27512 prom buffer*/
long offprom, p_lange, p_begadre;
/*	open filename
	------------- */

openfile()
{
	fd=open (namef,rmode);
	cfd=1;
	if (fd==-1) {error=3; outerr(); return;}

	if (kennB==1)
		{	/* behandlung binary-dateien (ausfuehrbare dateien)*/
		nbytes=512L;
		boffset=0L;
		do
		{
		n_read=read (fd, progbuf, nbytes);
		if (n_read==-1)	{error=8; outerr(); return;}
		if (n_read==0L)	break;
		boffset=boffset+n_read;
		}while (n_read!=0L);
		if (boffset>0xffffL)	{warning=13; outwar();}
		if (kennw==1)	s_exec.s_magic=N_MAGIC1;
			else	s_exec.s_magic=X_MAGIC1;
		s_exec.s_imsize=boffset;
		s_exec.s_bss=0;
		s_exec.s_segt=0x10;
		s_exec.s_syms=0;
		s_exec.s_entry=0;
		s_exec.s_flag=0x0001;
		s_exec.s_codesz=0;
		s_exec.s_lines=0;
 
		segt.sg_segno=segnr;
		segt.sg_coff=0;
		segt.sg_doff=0;
		segt.sg_boff=0;
		segt.sg_bss=0;
		segt.sg_unused=0;
		segt.sg_code=boffset;
		segt.sg_data=0;
		segt.sg_atr=1;

		boffset=0L;
		}
	else	{	/*behandlung a.out-dateien*/
		nbytes=24L;
		n_read=read (fd,&s_exec,nbytes);
		if (n_read!=nbytes||n_read==0L) {error=8; outerr(); return;}

		if (s_exec.s_magic!=S_MAGIC1&&s_exec.s_magic!=S_MAGIC3&&s_exec.s_magic!=S_MAGIC4&&
    	    	s_exec.s_magic!=N_MAGIC1&&s_exec.s_magic!=N_MAGIC3&&s_exec.s_magic!=N_MAGIC4&&
    	    	s_exec.s_magic!=X_MAGIC1&&s_exec.s_magic!=X_MAGIC3&&s_exec.s_magic!=X_MAGIC4)
			        {error=5; outerr(); return;}
		nbytes=16L;
		zsegt=s_exec.s_segt;
		zhl1=0;
		zhl2=0;
		boffset=24L;

		do
		{
		n_read=read (fd,&bsegt,nbytes);
		if (n_read!=nbytes||n_read==0) {error=8; outerr(); return;}
		if (zhl2==0)	{if (segnr==bsegt.sg_segno) 	{zhl2=1; 
								segt.sg_segno=bsegt.sg_segno;
								segt.sg_coff=bsegt.sg_coff;
								segt.sg_doff=bsegt.sg_doff;
								segt.sg_boff=bsegt.sg_boff;
								segt.sg_code=bsegt.sg_code;
								segt.sg_data=bsegt.sg_data;
								segt.sg_bss=bsegt.sg_bss;
								segt.sg_atr=bsegt.sg_atr;
								segt.sg_unused=bsegt.sg_unused;}}
		hoffset=16L;
		boffset=boffset+hoffset;
		if (zhl2==0)	{hoffset=bsegt.sg_code;	
				boffset=boffset+hoffset;
				hoffset=bsegt.sg_data;
				boffset=boffset+hoffset;}
		zsegt=zsegt-16;
		}while (zsegt!=0);
		if (zhl2==0) {error=6; outerr(); return;}
		if (sect=='d') {if (s_exec.s_magic!=S_MAGIC3&&s_exec.s_magic!=N_MAGIC3)
						{error=7; outerr(); return;}
				if (kennfo==0)	{begadrf=segt.sg_doff;
						begadrf=begadrf << 8;}}
			else	{if (kennfo==0)	{eof=segt.sg_atr & 0x0003;
						if (eof==2)	begadrf=segt.sg_doff;
							else	begadrf=segt.sg_coff;
						begadrf=begadrf << 8;}}
		}
	}
	

/*	read file
	--------- */

readfile ()
{
int bwarning, beof;
	for (zhl2=0;zhl2<langf;zhl2++) progbuf[zhl2]=0xff;
	for (zhl2=0;zhl2<langf;zhl2++) eprombuf[zhl2]=0xff;
	bwarning=true, beof=true;
	offset=boffset;
if (s_exec.s_magic==X_MAGIC3||s_exec.s_magic==N_MAGIC3||s_exec.s_magic==S_MAGIC3)
		/* separate I & D */
{
	if (sect=='i')		/* instruction */
		{if ((segt.sg_atr & 1)>0)
			{adr=segt.sg_coff; len=segt.sg_code;}
		else	{warning=1; outwar(); return;}}
	else			/* data */
		{if ((segt.sg_atr & 2)>0)
			{adr=segt.sg_doff; len=segt.sg_data; 
			aoffset=segt.sg_code;offset=offset+aoffset;}
		else	{warning=2; outwar(); return;}}
}
else		/* executable and overlay */
{
	eof=segt.sg_atr & 0x0003;
	if (eof==0) {warning=3; outwar(); return;}
	if (eof==1) {adr=segt.sg_coff; len=segt.sg_code;}
	if (eof==2) {adr=segt.sg_doff; len=segt.sg_data;}
	if (eof==3)	{if (segt.sg_coff==segt.sg_doff)
				{adr=segt.sg_coff+segt.sg_doff;
				len=segt.sg_code+segt.sg_data;}
			else	{if (segt.sg_coff<segt.sg_doff)
					{adr=segt.sg_coff; len=segt.sg_code;
					sfile();
					bwarning=warning; beof=eof;
					if (eof==false&&warning==false) 
						{lseek (fd,offset,0);
					n_read=read (fd,peprombuf,nbytes);
						if (n_read==0)
							{eof=true; return;}

						switch (bre)
							{
							case 's': zhl1=1; break;
							case 'h': zhl1=2; 
								  if ((nbytes % 2) > 0)	nbytes=nbytes/2+1;
									else		nbytes=nbytes/2; 
								  break;
							case 'l': zhl1=2; nbytes=nbytes/2; peprombuf++; break;
							}
						p_lange=nbytes;
					
						for (;nbytes>0;pprogbuf++,peprombuf=peprombuf+zhl1,nbytes--)
							*pprogbuf= *peprombuf;}


					aoffset=segt.sg_code;
					offset=boffset+aoffset;
					adr=segt.sg_doff; len=segt.sg_data;}
				else
					{adr=segt.sg_doff; len=segt.sg_data;
					aoffset=segt.sg_code;
					offset=offset+aoffset;
					sfile();
					bwarning=warning; beof=eof;
					if (warning==false&&eof==false)
						{lseek (fd,offset,0);
						n_read=read (fd,peprombuf,nbytes);
						if (n_read==0)
							{eof=true; return;}

						switch (bre)
							{
							case 's': zhl1=1; break;
							case 'h': zhl1=2; 
								  if ((nbytes % 2) > 0)	nbytes=nbytes/2+1;
									else		nbytes=nbytes/2; 
								  break;
							case 'l': zhl1=2; nbytes=nbytes/2; peprombuf++; break;
							}
						p_lange=nbytes;
					
						for (;nbytes>0;pprogbuf++,peprombuf=peprombuf+zhl1,nbytes--)
							*pprogbuf= *peprombuf;}
					offset=boffset;
					adr=segt.sg_coff; len=segt.sg_code;}}	
			}
}
	sfile();
	if (eof==false&&warning==false) 
		{lseek (fd,offset,0);
		n_read=read (fd,peprombuf,nbytes);
		if (n_read==0)
			{eof=true; return;}

		switch (bre)
			{
			case 's': zhl1=1; break;
			case 'h': zhl1=2; 
				  if ((nbytes % 2) > 0)	nbytes=nbytes/2+1;
					else		nbytes=nbytes/2; 
				  break;
			case 'l': zhl1=2; nbytes=nbytes/2; peprombuf++; break;
			}
			p_lange=nbytes;
					
		for (;nbytes>0;pprogbuf++,peprombuf=peprombuf+zhl1,nbytes--)
			*pprogbuf= *peprombuf;}
		
		
	if (bwarning==true&&warning==true) {warning=3; outwar(); return;}
	if (beof==true&&eof==true) {eof=true; return;}
		else	eof=false; 

for (warning=3,pprogbuf=progbuf,beof=lange1;beof!=0;pprogbuf++,beof--)
			{if (*pprogbuf!=0xff) {warning=0;break;}}
if (warning==3) {outwar(); return;}

if (s_exec.s_magic==X_MAGIC1||s_exec.s_magic==X_MAGIC3||s_exec.s_magic==X_MAGIC4)
				{if (bre=='l'||bre=='h')
					{warning=4; outwar(); return;}}
		else		{if (bre=='s')
					{warning=5; outwar(); return;}}
	
}

	
	/* bestimmung seek-adresse */

sfile()
{
	eof=false; warning=false;
	adr=adr << 8;
	if (begadrf<adr)	{
				if ((begadrf+langf)<=adr)	warning=true;
						else		{
								offprom=adr-begadrf;
								peprombuf= &eprombuf[adr-begadrf];
								pprogbuf= &progbuf[adr-begadrf];
								if ((begadrf+langf)>=(adr+len))	nbytes=len;

									else			nbytes=begadrf+langf-adr;
								}
				}
		else		{
				if (begadrf>=(adr+len))		eof=true;
						else		{
								offprom=0L;
								peprombuf= &eprombuf[0];
								pprogbuf= &progbuf[0];
								aoffset=begadrf;
								offset=offset+aoffset;
								aoffset=adr;
								offset=offset-aoffset;
								if ((begadrf+langf)>(adr+len))
										nbytes=adr+len-begadrf;
									else	nbytes=langf;
								}
				}
	p_progbuf=pprogbuf;
	p_begadre=begadre1+offprom;
}

/*	prom to file
	------------ */

promfile() 	
{
long offs;

	if (next==false)	{
				fd1=open (namef,wmode);
				cfd1=1;
				if (fd1!= -1)	{warning=6; outwar();
						if (warning==true) return;}
				}
	printf ("%s %s %s %s\n",fil,namee1,to,namef);
	if (next==false)	{
				fd1=creat (namef,0755);
				if (fd1== -1)	{error=3; outerr(); return;}
				if (kennB==0)
						{	/*behandlung a.out-datei*/
						s_exec.s_imsize=0;
						segt.sg_code=0;
						segt.sg_data=0;
						segt.sg_atr=0;
						s_exec.s_bss=0;
						s_exec.s_segt=0x10;
						s_exec.s_syms=0;
						s_exec.s_entry=entryf & 0x0000ffff;
						s_exec.s_flag=0x0001;
						s_exec.s_codesz=0;
						s_exec.s_lines=0;
	
						segt.sg_segno=segnr;
						segt.sg_atr=0;
						offs=begadrf;
						offs=offs >> 8;
						offset256=begadrf & 0xff00;
						if ((begadrf & 0x00ff) > 0)	{offs++; offset256=offset256+0x0100;
										if (sect=='i')	{warning=7; outwar();}
											else	{warning=8; outwar();}}
						if (sect=='i')	{segt.sg_coff=offs & 0x00ff; segt.sg_doff=0;
								segt.sg_atr=segt.sg_atr | 0x0001;
								if (bre=='s')	s_exec.s_magic=X_MAGIC1;
									else	{if (kenns==0)	s_exec.s_magic=N_MAGIC1;
											else	s_exec.s_magic=S_MAGIC1;}}
							else	{segt.sg_doff=offs & 0x00ff; segt.sg_coff=0; 
								segt.sg_atr=segt.sg_atr | 0x0002;
								if (bre=='s')	s_exec.s_magic=X_MAGIC3;
									else	{if (kenns==0)	s_exec.s_magic=N_MAGIC3;
											else	s_exec.s_magic=S_MAGIC3;}}
						if (begadrf>0)	{if (sect=='i')	segt.sg_atr=segt.sg_atr | 0x0010;
									else	segt.sg_atr=segt.sg_atr | 0x0020;}
						segt.sg_boff=0;
						segt.sg_bss=0;
						segt.sg_unused=0;
		
						offset=40;
						}
					else		/*behandlung binary-datei*/
						offset=0;	
				}
/* next prom */

	nbytes=lange1;
	begadre2=begadre1+begadre1; pprogbuf= &progbuf[0];
	switch (bre)
		{
		case 's':	hoffset=prlang; zhl1=1;
				for (zhl2=0;zhl2<langf;zhl2++)
					eprombuf[zhl2]=0xff;
				peprombuf= &eprombuf[begadre1];
				break;
		case 'l':	lange2=prlang; zhl1=2;
				for (zhl2=0;zhl2<langf;zhl2++)
					eprombuf[zhl2]=0xff;
				peprombuf= &eprombuf[begadre2];
				peprombuf++;
				break;
		case 'h':	zhl1=2;
				peprombuf= &eprombuf[begadre2];
				if (lange2 > prlang)	{hoffset=lange2;
							prlang=lange2;}
						else	hoffset=prlang;
				hoffset=hoffset+hoffset;
				prlang=prlang+prlang;
				break;	
		}
	for (;nbytes>0;pprogbuf++,peprombuf=peprombuf+zhl1,nbytes--)
			*peprombuf= *pprogbuf;
	switch (bre)
		{
		case 's': break;
		case 'l': bre='h'; return;
		case 'h': bre='l'; begadrf=begadrf+prlang; break;
		}

	lseek (fd1, offset, 0);
	offset=offset + hoffset;
	n_write=write (fd1,eprombuf,prlang);
	if ((n_write & 0xffffL) != prlang)	{error=9; outerr(); return;}

	aoffset=prlang;
	s_exec.s_imsize=s_exec.s_imsize + aoffset;
	if (sect=='i')	segt.sg_code=segt.sg_code + prlang;
		else	segt.sg_data=segt.sg_data + prlang;
}

header_file()
{
	if (kennB==0)
			{	/*behandlung a.out-datei*/
			lseek (fd1, 0L, 0);

			nbytes=24;
			n_write=write (fd1,&s_exec,nbytes);
			if (n_write != nbytes) {error=9; outerr(); return;}

			nbytes=16;
			n_write=write (fd1,&segt,nbytes);
			if (n_write != nbytes) {error=9; outerr(); return;}
			}
	close (fd1);
}
