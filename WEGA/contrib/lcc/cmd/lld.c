/*
 * lld - string table version for P8000
 */

#include <sys/param.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <ar.h>
#include <s.out.h>
#include <sys/stat.h>
#include <sys/file.h>

#define SARMAG 2L
#define MAXPATHLEN 256
#define O_RDONLY 000


/*
 * Macros which take exec structures as arguments and tell whether
 * the file has a reasonable magic number or offsets to text|symbols|strings.
 */
#define	N_BADMAG(x) \
    (((x).s_magic)!=S_MAGIC3 && ((x).s_magic)!=S_MAGIC1)

#define N_TXTOFF(x) \
	(sizeof (struct s_exec) + (x).s_segt )
#define N_SYMOFF(x) \
	(N_TXTOFF(x) + (x).s_imsize + (((x).s_flag & SF_STRIP) == 0 ? \
							(x).s_imsize : 0 ))

/*
 * Internal data structures
 *
 * All internal data structures are segmented and dynamically extended.
 * The basic structures hold 1103 (NSYM) symbols, ~~200 (NROUT)
 * referenced library members, and 100 (NSYMPR) private (local) symbols
 * per object module.  For large programs and/or modules, these structures
 * expand to be up to 40 (NSEG) times as large as this as necessary.
 */
#define	NSEG	40		/* Number of segments, each data structure */
#define	NSYM	1103		/* Number of symbols per segment */
#define	NROUT	250		/* Number of library references per segment */
#define	NSYMPR	100		/* Number of private symbols per segment */

/*
 * Structure describing each symbol table segment.
 * Each segment has its own hash table.  We record the first
 * address in and first address beyond both the symbol and hash
 * tables, for use in the routine symx and the lookup routine respectively.
 * The symfree routine also understands this structure well as it used
 * to back out symbols from modules we decide that we don't need in pass 1.
 *
 * Csymseg points to the current symbol table segment;
 * csymseg->sy_first[csymseg->sy_used] is the next symbol slot to be allocated,
 * (unless csymseg->sy_used == NSYM in which case we will allocate another
 * symbol table segment first.)
 */
struct	symseg {
	struct	nlist *sy_first;	/* base of this alloc'ed segment */
	struct	nlist *sy_last;		/* end of this segment, for sn_strx */
	int	sy_used;		/* symbols used in this seg */
	struct	nlist **sy_hfirst;	/* base of hash table, this seg */
	struct	nlist **sy_hlast;	/* end of hash table, this seg */
} symseg[NSEG], *csymseg;

/*
 * The lookup routine uses quadratic rehash.  Since a quadratic rehash
 * only probes 1/2 of the buckets in the table, and since the hash
 * table is segmented the same way the symbol table is, we make the
 * hash table have twice as many buckets as there are symbol table slots
 * in the segment.  This guarantees that the quadratic rehash will never
 * fail to find an empty bucket if the segment is not full and the
 * symbol is not there.
 */
#define	HSIZE	(NSYM*2)

/*
 * Xsym converts symbol table indices (ala x) into symbol table pointers.
 * Symx (harder, but never used in loops) inverts pointers into the symbol
 * table into indices using the symseg[] structure.
 */
#define	xsym(x)	(symseg[(x)/NSYM].sy_first+((x)%NSYM))
/* symx() is a function, defined below */

struct  nlist {
	long	n_value;
	char	n_type;
	char	n_segt;
	union {
		char *n_name;
		char w_name[8];
	}n_un;
	int	n_hash;
};

#define LIBF 0x80	/* internes Bit in n_type
			   zeigt an, dass Fkt. aus einer Bibl. kommt */
struct	nlist cursym;		/* current symbol */
struct	nlist *lastsym;		/* last symbol entered */
struct	nlist *nextsym;		/* next available symbol table entry */
struct	nlist *addsym;		/* first sym defined during incr load */
int	nsym;			/* pass2: number of local symbols in a.out */
/* nsym + symx(nextsym) is the symbol table size during pass2 */

struct	nlist **lookup(), **slookup();
struct	nlist *p_etext, *p_edata, *p_end, *entrypt, *p_stkseg;

#define STKSEG 0x7f000000
#define N_COMM 0x05
/*
 * Definitions of segmentation for library member table.
 * For each library we encounter on pass 1 we record pointers to all
 * members which we will load on pass 2.  These are recorded as offsets
 * into the archive in the library member table.  Libraries are
 * separated in the table by the special offset value -1.
 */
off_t	li_init[NROUT];
struct	libseg {
	off_t	*li_first;
	int	li_used;
	int	li_used2;
} libseg[NSEG] = {
	li_init, 0, 0,
}, *clibseg = libseg;

/*
 * In processing each module on pass 2 we must relocate references
 * relative to external symbols.  These references are recorded
 * in the relocation information as relative to local symbol numbers
 * assigned to the external symbols when the module was created.
 * Thus before relocating the module in pass 2 we create a table
 * which maps these internal numbers to symbol table entries.
 * A hash table is constructed, based on the local symbol table indices,
 * for quick lookup of these symbols.
 */
#define	LHSIZ	31
struct	local {
	int	l_index;		/* index to symbol in file */
	struct	nlist *l_symbol;	/* ptr to symbol table */
	struct	local *l_link;		/* hash link */
} *lochash[LHSIZ], lhinit[NSYMPR];
struct	locseg {
	struct	local *lo_first;
	int	lo_used;
} locseg[NSEG] = {
	lhinit, 0
}, *clocseg;


/*
 * We open each input file or library only once, but in pass2 we
 * (historically) read from such a file at 2 different places at the
 * same time.  These structures are remnants from those days,
 * and now serve only to catch ``Premature EOF''.
 * In order to make I/O more efficient, we provide routines which
 * use the optimal block size returned by stat().
 */
#define BLKSIZE 512
typedef struct {
	short	*fakeptr;
	int	bno;
	int	nibuf;
	int	nuser;
	char	*buff;
	int	bufsize;
} PAGE;

PAGE	page[2];
int	p_blksize;
int	p_blkshift;
int	p_blkmask;

struct {
	short	*fakeptr;
	int	bno;
	int	nibuf;
	int	nuser;
} fpage;

typedef struct {
	char	*ptr;
	int	bno;
	int	nibuf;
	long	size;
	long	pos;
	PAGE	*pno;
} STREAM;

STREAM	text;
STREAM	reloc;


/*
 * Libraries are typically built with a table of contents,
 * which is the first member of a library with special file
 * name __.SYMDEF and contains a list of symbol names
 * and with each symbol the offset of the library member which defines
 * it.  The loader uses this table to quickly tell which library members
 * are (potentially) useful.  The alternative, examining the symbol
 * table of each library member, is painfully slow for large archives.
 *
 * See <ranlib.h> for the definition of the ranlib structure and an
 * explanation of the __.SYMDEF file format.
 */
struct ranlib {
	long	ran_off;
	char	ran_name[8];
};
int	tnum;		/* number of symbols in table of contents */
struct	ranlib *tab;	/* the table of contents (dynamically allocated) */

/*
 * Header from the a.out and the archive it is from (if any).
 */
struct	s_exec filhdr;
struct	ar_hdr archdr;
#define	OARMAG 0177545

unsigned fsyms;
/*
 * Options.
 */
int	trace;
int	Mflag;		/* print rudimentary load map */
int	sflag;
int	rflag;
int	wflag;		/* no warning for redefined */
char	**ytab;		/* the symbols */

/*
 * These are the cumulative sizes, set in pass 1, which
 * appear in the a.out header when the loader is finished.
 */
off_t	tsize, dsize, bsize, trsize, drsize, ssize;

/*
 * Symbol relocation: c?rel is a scale factor which is
 * added to an old relocation to convert it to new units;
 * i.e. it is the difference between segment origins.
 * (Thus if we are loading from a data segment which began at location
 * 4 in a .o file into an a.out where it will be loaded starting at
 * 1024, cdrel will be 1020.)
 */
long	ctrel, cdrel, cbrel;

/*
 * Textbase is the start address of all text, 0 unless given by -T.
 * Database is the base of all data, computed before and used during pass2.
 */
long	textbase, database;

/*
 * The base addresses for the loaded text, data and bss from the
 * current module during pass2 are given by torigin, dorigin and borigin.
 */
long	torigin, dorigin, borigin;

/*
 * Structuren und Konstanten fuer die Segmentzuordnung
 * und fuer die Adressberechnung
 */
#define MAXSEG 31 /* max. Anzahl der Segmente */
#define TEXTBASE 0x00000000
#define DATABASE 0x20000000
#define BSSBASE  0x40000000
#define FIRSTTSEG 0x00
#define FIRSTDSEG 0x20
#define FIRSTBSEG 0x40
#define LASTTSEG  0x1f
#define LASTDSEG  0x3f
#define LASTBSEG  0x5f
#define SEGSIZE 0x0000fffe /* Groesse eines Segmentes */
/* Segmentzuordnungstabellen
   mit der Nummer des Segmenttabelleneintrages und
   einer relativen Adresse fuer Text, Daten und BSS */
struct szt {
	char	tseg;
	long trel;
	char	dseg;
	long drel;
	char	bseg;
	long brel;
};
struct segt *tptr;	/* Zeiger auf Anfang der Text-Segmenttabellen */
struct segt *dptr;	/* Zeiger auf Anfang der Daten-Segmenttabellen */
struct segt *bptr;	/* Zeiger auf Anfang der BSS-Segmenttabellen */
struct segt *ctptr;	/* Aktuelle Segmenttabelle mit Text */
struct segt *cdptr;	/* Aktuelle Segmenttabelle mit Daten */
struct segt *cbptr;	/* Aktuelle Segmenttabelle mit BSS */
struct segt *cptr;	/* Naechste verfuegbare leere Segmenttabelle */
struct szt *szstr;	/* Zeiger auf erste Struktur der Segmentzuordnungs-
			   tabellen */
char ctnr, cdnr, cbnr, cnr; /* Segmentnummern */
/*
 * Errlev is nonzero when errors have occured.
 * Delarg is an implicit argument to the routine delexit
 * which is called on error.  We do ``delarg = errlev'' before normal
 * exits, and only if delarg is 0 (i.e. errlev was 0) do we make the
 * result file executable.
 */
int	errlev;
int	delarg	= 4;

/*
 * The biobuf structure and associated routines are used to write
 * into one file at several places concurrently.  Calling bopen
 * with a biobuf structure sets it up to write ``biofd'' starting
 * at the specified offset.  You can then use ``bwrite'' and/or ``bputc''
 * to stuff characters in the stream, much like ``fwrite'' and ``fputc''.
 * Calling bflush drains all the buffers and MUST be done before exit.
 */
struct	biobuf {
	short	b_nleft;		/* Number free spaces left in b_buf */
/* Initialize to be less than b_bufsize initially, to boundary align in file */
	char	*b_ptr;			/* Next place to stuff characters */
	char	*b_buf;			/* Pointer to the buffer */
	int	b_bufsize;		/* Size of the buffer */
	off_t	b_off;			/* Current file offset */
	struct	biobuf *b_link;		/* Link in chain for bflush() */
} *biobufs;
#define	bputc(c,b) ((b)->b_nleft ? (--(b)->b_nleft, *(b)->b_ptr++ = (c)) \
		       : bflushc(b, c))
int	biofd;
off_t	boffset;
struct	biobuf *tout, *dout, *trout, *drout, *sout, *strout;

/*
 * Offset is the current offset in the string file.
 * Its initial value reflects the fact that we will
 * eventually stuff the size of the string table at the
 * beginning of the string table (i.e. offset itself!).
 */
off_t	offset = sizeof (off_t);

int	ofilfnd;		/* -o given; otherwise move l.out to a.out */
char	*ofilename = "l.out";
int	ofilemode;		/* respect umask even for unsucessful ld's */
int	infil;			/* current input file descriptor */
char	*filname;		/* and its name */

#define	NDIRS	25
#define NDEFDIRS 3		/* number of default directories in dirs[] */
char	*dirs[NDIRS];		/* directories for library search */
int	ndir;			/* number of directories */

/*
 * Base of the string table of the current module (pass1 and pass2).
 */
char	*curstr;

/*
 * System software page size, as returned by getpagesize.
 */
long	pagesize;

char 	get();
int	delexit();
char	*savestr();
char	*malloc();
long	calloc();
long	round();

main(argc, argv)
char **argv;
{
	register int c, i; 
	int num;
	register char *ap, **p;
	char save;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
		signal(SIGINT, delexit);
		signal(SIGTERM, delexit);
	}
	if (argc == 1)
		exit(4);
	/*pagesize = (long)getpagesize();*/
	pagesize = 256L;

	/* 
	 * Pull out search directories.
	 */
	for (c = 1; c < argc; c++) {
		ap = argv[c];
		if (ap[0] == '-' && ap[1] == 'L') {
			if (ap[2] == 0)
				error(1, "-L: pathname missing");
			if (ndir >= NDIRS - NDEFDIRS)
				error(1, "-L: too many directories");
			dirs[ndir++] = &ap[2];
		}
	}
	/* add default search directories */
	dirs[ndir++] = "/z/LCC/lib";

	p = argv+1;
	/*
	 * Scan files once to find where symbols are defined.
	 */
	for (c=1; c<argc; c++) {
		filname = 0;
		ap = *p++;
		if (*ap != '-') {
			if (trace)
				printf("%s:\n", ap);
			load1arg(ap);
			continue;
		}
		for (i=1; ap[i]; i++) switch (ap[i]) {

		case 'o':
			if (++c >= argc)
				error(1, "-o where?");
			ofilename = *p++;
			ofilfnd++;
			continue;
		case 'e':
			if (++c >= argc)
				error(1, "-e: arg missing");
			enter(slookup(*p++));
			if (ap[i]=='e')
				entrypt = lastsym;
			continue;
		case 'l':
			if (trace)
				printf("%s:\n", ap);
			save = ap[--i]; 
			ap[i]='-';
			load1arg(&ap[i]); 
			ap[i]=save;
			goto next;
		case 'M':
			Mflag++;
			continue;
		case 't':
			trace++;
			printf("*** pass1 ***\n");
			continue;
		case 's':
			sflag++;
			continue;
		case 'w':
			wflag++;
			continue;
		case 'L':
			goto next;
		case 'x':
		case 'X':
		case 'z':
		case 'i':
			continue; /* kompatible sld */
		default:
			filname = savestr("-x");	/* kludge */
			filname[1] = ap[i];		/* kludge */
			archdr.ar_name[0] = 0;		/* kludge */
			error(1, "bad flag");
		}
next:
		;
	}
	endload(argc, argv);
	exit(0);
}


delexit()
{
	struct stat stbuf;
	long size;
	char c = 0;

	bflush();
	unlink("l.out");
	/*
	 * We have to insure that the last block of the data segment
	 * is allocated a full pagesize block. If the underlying
	 * file system allocates frags that are smaller than pagesize,
	 * a full zero filled pagesize block needs to be allocated so 
	 * that when it is demand paged, the paged in block will be 
	 * appropriately filled with zeros.
	 */
	fstat(biofd, &stbuf);
	size = round(stbuf.st_size, pagesize);
	if (delarg==0)
		(void) chmod(ofilename, ofilemode);
	exit (delarg);
}


/*
 * Scan file to find defined symbols.
 */
load1arg(cp)
	register char *cp;
{
	register struct ranlib *tp;
	off_t nloc;
	int kind;
	char *p;

	kind = getfile(cp);
	if (Mflag)
		printf("%s\n", filname);
	switch (kind) {

	/*
	 * Plain file.
	 */
	case 0:
		load1(0, 0L);
		break;

	/*
	 * Archive without table of contents.
	 * (Slowly) process each member.
	 */
	case 1:
		nloc = SARMAG;
		while (step(nloc))
			nloc += sizeof(archdr) +
			    round(archdr.ar_size, (long)(sizeof (short)));
		break;

	/*
	 * Archive with table of contents.
	 * Read the table of contents and its associated string table.
	 * Pass through the library resolving symbols until nothing changes
	 * for an entire pass (i.e. you can get away with backward references
	 * when there is a table of contents!)
	 */
	case 2:
		if(Mflag)
			printf("Archive with table of contents\n");
		nloc = SARMAG + sizeof (archdr);
		tab = (struct ranlib *)malloc((int)archdr.ar_size+1);
		if (tab==0)
			error(1,"ran out of memory (tab)");
		dseek(&text,nloc,archdr.ar_size);
		mget((char *)tab,archdr.ar_size,&text);
		p= (char *)tab + archdr.ar_size;
		*p=0;
		tnum = archdr.ar_size / sizeof(struct ranlib);
		while (ldrand())
			continue;
		free((char *)tab);
		nextlibp(-1L);
		break;

	/*
	 * Table of contents is out of date, so search
	 * as a normal library (but skip the __.SYMDEF file).
	 */
	case 3:
		error(-1,"warning: table of contents is out of date\n");
		nloc = SARMAG;
		do
			nloc += sizeof(archdr) + round(archdr.ar_size, (long)(sizeof(short)));
		while (step(nloc));
		break;
	}
	close(infil);
}

/*
 * Advance to the next archive member, which
 * is at offset nloc in the archive.  If the member
 * is useful, record its location in the liblist structure
 * for use in pass2.  Mark the end of the archive in libilst with a -1.
 */
step(nloc)
	off_t nloc;
{

	dseek(&text, nloc, (long) sizeof archdr);
	if (text.size <= 0) {
		nextlibp(-1L);
		return (0);
	}
	getarhdr();
	if (strncmp(archdr.ar_name,"Library.SC",sizeof(archdr.ar_name))==0)
		return(1);
	if (load1(1, nloc + (sizeof archdr)))
		nextlibp(nloc);
	return (1);
}

/*
 * Record the location of a useful archive member.
 * Recording -1 marks the end of files from an archive.
 * The liblist data structure is dynamically extended here.
 */
nextlibp(val)
	off_t val;
{

	if (clibseg->li_used == NROUT) {
		if (++clibseg == &libseg[NSEG])
			error(1, "too many files loaded from libraries");
		clibseg->li_first = (off_t *)malloc(NROUT * sizeof (off_t));
		if (clibseg->li_first == 0)
			error(1, "ran out of memory (nextlibp)");
	}
	clibseg->li_first[clibseg->li_used++] = val;
	if (val != -1 && Mflag)
		printf("\t%s\n", archdr.ar_name);
}



/*
 * One pass over an archive with a table of contents.
 * Remember the number of symbols currently defined,
 * then call step on members which look promising (i.e.
 * that define a symbol which is currently externally undefined).
 * Indicate to our caller whether this process netted any more symbols.
 */
ldrand()
{
	register struct nlist *sp, **hp;
	register struct ranlib *tp, *tplast;
	off_t loc;
	int nsymt = symx(nextsym);
	char str[150];

	tplast = &tab[tnum-1];
	for (tp = tab; tp <= tplast; tp++) {
		if ((hp = slookup(tp->ran_name)) == 0 || *hp == 0)
			continue;
		sp = *hp;
		if ((sp->n_type & 0x3f) != N_EXT+N_UNDF){ /*segmented bit
							   ausblenden ?? */
			if(((sp->n_type & 0x80) == 0) && wflag==0){
				sprintf(str,"- warning  symbol (%s) redefined",sp->n_un.n_name);
				error(-1,str);
			}
			continue;
		}
		step(tp->ran_off);
		loc = tp->ran_off;
		while (tp < tplast && (tp+1)->ran_off == loc){
			tp++;
		}
	}
	return (symx(nextsym) != nsymt);
}



/*
 * Examine a single file or archive member on pass 1.
 */
load1(libflg, loc)
	off_t loc;
{
	register struct nlist *sp;
	struct nlist *savnext;
	int ndef, nlocal,  size, nsymt, x;
	char type;
	register int i;
	off_t maxoff;
	struct stat stb;
	char *p;
	char *savecur;
	char *curptr, *segstr;
	struct szt *zp;
	struct segt *cp;
	char ctseg, cdseg, cbseg;
	unsigned curt, curd, curb;
	int segcnt;
	long cr;
	char str[150];

	/* reservieren des Speicherplatzes fuer die gesamte Segmenttabelle*/
	if ( tptr == 0 ) {
		tptr=(struct segt *)calloc(MAXSEG-1, sizeof (struct segt));
		dptr=(struct segt *)calloc(MAXSEG-1, sizeof (struct segt));
		bptr=(struct segt *)calloc(MAXSEG-1, sizeof (struct segt));
		if (tptr == 0 || dptr == 0 || bptr == 0)
			error(1, "ran out of space for segmenttable");
		/* initialisieren der ersten Segmente */
		ctptr=tptr;
		ctptr->sg_atr=SG_CODE;
		ctptr->sg_segno = ctnr = FIRSTTSEG;
		cdptr = dptr;
		cdptr->sg_atr=SG_DATA;
		cdptr->sg_segno = cdnr = FIRSTDSEG;
		cbptr = bptr;
		cbptr->sg_atr=SG_BSS;
		cbptr->sg_segno = cbnr = FIRSTBSEG;
	}
	curt = ctptr->sg_code;
	curd = cdptr->sg_data;
	curb = cbptr->sg_bss;
	ctseg = ctnr;
	cdseg = cdnr;
	cbseg = cbnr;

	readhdr(loc);
	if (filhdr.s_syms == 0) {
		if (filhdr.s_imsize == 0)
			return (0);
		error(1, "no namelist");
	}
	if (libflg)
		maxoff = archdr.ar_size;
	else {
		fstat(infil, &stb);
		maxoff = stb.st_size;
	}
	/* reservieren des Speicherplatzes fuer die Segmenttabelle der 
	   aktuellen Datei */
	segstr=(char *)malloc(filhdr.s_segt);
	if(segstr == NULL)
		error(1,"no space for segmenttable");
	loc += (long)sizeof(struct s_exec);
	dseek(&text, loc, (long)filhdr.s_segt);
	mget((char *)segstr,(long)filhdr.s_segt, &text);
	segcnt = filhdr.s_segt >> 4;  /*Anzahl der Segmente*/
	szstr= (struct szt *)calloc(segcnt,sizeof(struct szt));
	if (szstr == 0)
		error(1,"no space for segmenttable");
	cp = (struct segt *)segstr;
	zp = (struct szt *)szstr;
	for ( x=0 ; x<segcnt ; cp++ , x++ , zp++) {
		if(cp->sg_code) {
			if((long)curt+(long)cp->sg_code > SEGSIZE) {
				curt = 0 ; ctseg++ ;
			}
			zp->trel = (long)curt - ((long)cp->sg_coff<<8);
			zp->tseg = ctseg;
			curt += cp->sg_code;
		}
		if(cp->sg_data) {
			if((long)curd+(long)cp->sg_data > SEGSIZE) {
				curd = 0 ; cdseg++;
			}
			zp->drel = (long)curd - ((long)cp->sg_doff<<8);
			zp->dseg = cdseg;
			curd += cp->sg_data;
		}
		if(cp->sg_bss) {
			if((long)curb+(long)cp->sg_bss > SEGSIZE) {
				curb = 0 ; cbseg++;
			}
			zp->brel = (long)curb - ((long)cp->sg_boff<<8);
			zp->bseg = cbseg;
			curb += cp->sg_bss;
		}
	}		
	ndef = 0;
	nlocal = sizeof(cursym);
	savnext = nextsym;
	loc += filhdr.s_segt+filhdr.s_imsize+((filhdr.s_flag & SF_STRIP)==0 ? filhdr.s_imsize : 0 );
	dseek(&text, loc, (long)filhdr.s_syms);
	curstr = (char *)malloc(filhdr.s_syms);
	if (curstr == NULL)
		error(1, "no space for string table");
	curptr=curstr;
	while (text.size > 0) {
		mget((char *)&cursym, 14L , &text);
		savecur=curptr;
		p = (char *) &cursym.n_un.n_name;
		if( *p & LONGBIT) {
		/* Long name */
			x= *p & LONGMASK;
			for(i=0;i<7;i++)
				*(curptr++) = *(++p);
			for(i=14; i < (x-7); i += 14);
			mget((char *)curptr,(long)i,&text);
			curptr += i;
		}
		else /* short name */ {
			for(i=0;i<8;i++){
				if(p[i]==0) break;
				*(curptr++)=p[i];
			}
		}
		*(curptr++)=0;
		cursym.n_un.n_name=savecur;
		type = cursym.n_type;
		if(libflg && (type&0x3f)!=N_EXT+N_UNDF)
			cursym.n_type |=0x80;
		cursym.n_hash = 0;
		if ((type&N_EXT)==0) {
			nlocal += sizeof cursym;
			continue;
		}
		symreloc();
		if (enter(lookup()))
			continue;
		if (((sp = lastsym)->n_type & 0x3f) != N_EXT+N_UNDF){
			/* segmented bit ausblenden ? */
			if(((cursym.n_type&0x3f) != N_EXT+N_UNDF)&&libflg&&((sp->n_type&0x80)==0)&&wflag==0) {
				sprintf(str,"- warning  symbol (%s) redefined in archive",cursym.n_un.n_name);
				error(-1,str);
			}
			continue;
		}
		if ((cursym.n_type & 0x3f) == N_EXT+N_UNDF) {
			/* segmented bit ausbl. ?? */
			if (cursym.n_value > sp->n_value)
				sp->n_value = cursym.n_value;
			continue;
		}
		if (sp->n_value != 0 && (cursym.n_type & 0x3f) == N_EXT+N_CODE)
			/* seg. bit ausbl. ? */
			continue;
		ndef++;
		if(libflg) cursym.n_type |= 0x80;
		sp->n_type = cursym.n_type;
		sp->n_value = cursym.n_value;
	}
	if (libflg==0 || ndef) {
		cp = (struct segt *)segstr;
		for ( x=0 ; x<segcnt ; cp++ , x++ ) {
			if(cp->sg_code) {
				if((long)ctptr->sg_code+(long)cp->sg_code > SEGSIZE) {
					cr=round((long)ctptr->sg_code,pagesize);
					if(cr > SEGSIZE)
						cr = SEGSIZE;
					tsize += cr-(long)ctptr->sg_code;
					ctptr->sg_code=(int)cr;
					ctptr++;
					ctptr->sg_atr=SG_CODE;
					ctptr->sg_segno= ++ctnr;
					if(ctnr>LASTTSEG)
						error(1,"too many text segments");
				}
				ctptr->sg_code += cp->sg_code;
				tsize += cp->sg_code;
			}
			if(cp->sg_data) {
				if((long)cdptr->sg_data+(long)cp->sg_data > SEGSIZE) {
					cr=round((long)cdptr->sg_data,pagesize);
					if(cr > SEGSIZE)
						cr = SEGSIZE;
					dsize += cr-(long)cdptr->sg_data;
					cdptr->sg_data=(int)cr;
					cdptr++;
					cdptr->sg_atr=SG_DATA;
					cdptr->sg_segno= ++cdnr;
					if(cdnr > LASTDSEG)
						error(1,"too many data segments");
				}
				cdptr->sg_data += cp->sg_data;
				dsize += cp->sg_data;
			}
			if(cp->sg_bss) {
				if((long)cbptr->sg_bss+(long)cp->sg_bss > SEGSIZE) {
					cr=round((long)cbptr->sg_bss,pagesize);
					if(cr > SEGSIZE)
						cr = SEGSIZE;
					bsize += cr-(long)cbptr->sg_bss;
					cbptr->sg_bss=(int)cr;
					cbptr++;
					cbptr->sg_atr=SG_BSS;
					cbptr->sg_segno= ++cbnr;
					if(cbnr>LASTBSEG)
						error(1,"too many bss segments");
				}
				cbptr->sg_bss += cp->sg_bss;
				bsize += cp->sg_bss;
			}
		}
		ssize += nlocal;
		nsymt = symx(nextsym);
		for (i = symx(savnext); i < nsymt; i++) {
			sp = xsym(i);
			sp->n_un.n_name = savestr(sp->n_un.n_name);
		}
		free(curstr);
		free(segstr);
		free(szstr);
		return (1);
	}
	/*
	 * No symbols defined by this library member.
	 * Rip out the hash table entries and reset the symbol table.
	 */
	symfree(savnext);
	free(curstr);
	free(segstr);
	free(szstr);
	return(0);
}




endload(argc, argv)
	int argc; 
	char **argv;
{
	register int c, i; 
	long dnum;
	register char *ap, **p;

	clibseg = libseg;
	filname = 0;
	middle();

	setupout();
	p = argv+1;
	if(trace)
		printf("*** pass2 ***\n");
	for (c=1; c<argc; c++) {
		ap = *p++;
		if (*ap != '-') {
			if (trace)
				printf("%s:\n",ap);
			load2arg(ap);
			continue;
		}
		for (i=1; ap[i]; i++) switch (ap[i]) {

		case 'o':
		case 'e':
			++c; 
			++p;
		default:
			continue;
		case 'l':
			if (trace)
				printf("%s:\n",ap);
			ap[--i]='-'; 
			load2arg(&ap[i]);
			goto next;
		}
next:
		;
	}

	if(trace)
		printf("tsize = %lx\ndsize = %lx\nbsize = %lx\n",tsize,dsize,bsize);
	finishout();
}



middle()
{
	register struct nlist *sp;
	long csize, t, corigin, ocsize, cr;
	int nund, rnd;
	char s;
	register int i;
	int nsymt;

/*
	torigin = 0; 
	dorigin = 0; 
	borigin = 0;
	p_etext = *slookup("_etext");
	p_edata = *slookup("_edata");
	p_end = *slookup("_end");
*/
	p_stkseg = *slookup("_stkseg");
	p_stkseg->n_type = N_EXT+N_ABS;
	p_stkseg->n_value = STKSEG;
	/*
	 * If there are any undefined symbols, save the relocation bits.
	 */
	nsymt = symx(nextsym);
/*
	for (i = 0; i < nsymt; i++) {
		sp = xsym(i);
		if ((sp->n_type & 0x3f)==N_EXT+N_UNDF && sp->n_value==0 && sp!=p_stkseg )  {
			rflag++;
			printf("undefined:	%s\n",sp->n_un.n_name);
		}
	}
	if (rflag) 
		error(1,"undefined symbols");
*/
	/*
	 * Assign common locations.
	 */
	csize = 0;
	addsym = symseg[0].sy_first;
	for (i = symx(addsym); i < nsymt; i++) {
		sp = xsym(i);
		if ((s=(sp->n_type&0x3f) == N_EXT+N_UNDF) && ((t = sp->n_value&0xffff)!=0)) {
			/*
			if (t >= sizeof (double))
				rnd = sizeof (double);
			else if (t >= sizeof (long))
				rnd = sizeof (long);
			else
				rnd = sizeof (short);
			*/
			csize = (long)cbptr->sg_bss;
			if(t >= (long)sizeof(int))
			/* wenn >= 2byte wird auf gerades byte aufgerundet */
				csize = round(csize, (long)sizeof(int));
			if((csize + t) > SEGSIZE) {
				cr=round((long)cbptr->sg_bss,pagesize);
				if(cr > SEGSIZE)
					cr = SEGSIZE;
				bsize += cr-(long)cbptr->sg_bss;
				cbptr->sg_bss=(int)cr;
				csize = 0;
				cbptr++;
				cbptr->sg_atr = SG_BSS;
				cbptr->sg_segno = ++cbnr;
				if(cbnr > LASTBSEG)
					error(1,"too many bss segments");
			}
			bsize += csize - (long)cbptr->sg_bss;
			sp->n_value = (long)cbnr <<24;
			sp->n_value += csize;
			sp->n_type = N_EXT+N_COMM;
			cbptr->sg_bss = (int)csize + (int)t;
			bsize += t;
		}
	}
	/*
	 * Now set symbols to their final value
	 */
	torigin = TEXTBASE;
	dorigin = ((long)ctnr+1)<<24;
	borigin = dorigin+(((long)(cdnr&0x1f)+1)<<24);
	
	cr=round((long)ctptr->sg_code,pagesize);
	if (cr > SEGSIZE)
		cr = SEGSIZE;
	tsize += cr-(long)ctptr->sg_code;
	ctptr->sg_code=(int)cr;
	cr=round((long)cdptr->sg_data,pagesize);
	if (cr > SEGSIZE)
		cr = SEGSIZE;
	dsize += cr-(long)cdptr->sg_data;
	cdptr->sg_data=(int)cr;
	cr=round((long)cbptr->sg_bss,pagesize);
	if (cr > SEGSIZE)
		cr = SEGSIZE;
	bsize += cr-(long)cbptr->sg_bss;
	cbptr->sg_bss=(int)cr;
	nund=0;
	nsymt = symx(nextsym);
	for (i = symx(addsym); i<nsymt; i++) {
		sp = xsym(i);
		switch (sp->n_type & (N_TYPE+N_EXT)) {

		case N_EXT+N_UNDF:
			if (nund==0)
				printf("Undefined:\n");
			nund++;
			printf("%s\n", sp->n_un.n_name);
			continue;
		case N_EXT+N_ABS:
		default:
			continue;
		case N_EXT+N_CODE:
			continue;
		case N_EXT+N_DATA:
			sp->n_value = (sp->n_value&0x1fffffff) + dorigin;
			continue;
		case N_EXT+N_COMM:
			sp->n_type =  (N_EXT+N_BSS);
		case N_EXT+N_BSS:
			sp->n_value = (sp->n_value&0x1fffffff) + borigin;
			continue;
		}
	}
	if(nund)
		error(1,"");
	nsym =  0;
}

fixspec(sym,offset)
	struct nlist *sym;
	long offset;
{

	if(symx(sym) < symx(addsym) && sym!=0)
		sym->n_value += offset;
}

ldrsym(sp, val, type)
	register struct nlist *sp;
	long val;
	char type;
{

	if (sp == 0)
		return;
	if (((sp->n_type&0x3f) != N_EXT+N_UNDF || sp->n_value)) {
		printf("%s: ", sp->n_un.n_name);
		error(0, "user attempt to redfine loader-defined symbol");
		return;
	}
	sp->n_type = type;
	sp->n_value = val;
}


load2arg(acp)
char *acp;
{
	register char *cp;
	off_t loc;

	cp = acp;
	if (getfile(cp) == 0) {
		while (*cp)
			cp++;
		while (cp >= acp && *--cp != '/');
		mkfsym(++cp);
		load2(0L);
	} else {	/* scan archive members referenced */
		for (;;) {
			if (clibseg->li_used2 == clibseg->li_used) {
				if (clibseg->li_used < NROUT)
					error(1, "libseg botch");
				clibseg++;
			}
			loc = clibseg->li_first[clibseg->li_used2++];
			if (loc == -1)
				break;
			dseek(&text, loc, (long)sizeof(archdr));
			getarhdr();
			mkfsym(archdr.ar_name);
			load2(loc + (long)sizeof(archdr));
		}
	}
	close(infil);
}

load2(loc)
long loc;
{
	int size;
	register struct segt *cp;
	register struct szt *zp;
	struct nlist *sp;
	struct local *lp;
	register int symno,xsymno, i;
	int x;
	int type;
	char cdseg,ctseg,cbseg;
	unsigned curt,curd,curb;
	char *segstr, *savecur, *curptr, *p;
	struct nlist *savnext;
	int ndef, segcnt;
	int nlocal;
	long loc1;

	readhdr(loc);
	/*
	 * Reread the symbol table, recording the numbering
	 * of symbols for fixing external references.
	 */
	for (i = 0; i < LHSIZ; i++)
		lochash[i] = 0;
	clocseg = locseg;
	clocseg->lo_used = 0;
	symno = -1;
	curt = (unsigned)(torigin & 0xffff);
	curd = (unsigned)(dorigin & 0xffff);
	curb = (unsigned)(borigin & 0xffff);
	ctseg = (char)((torigin >> 24) & 0xff);
	cdseg = (char)((dorigin >> 24) & 0xff);
	cbseg = (char)((borigin >> 24) & 0xff);

	/* reservieren des Speicherplatzes fuer die Segmenttabelle der 
	   aktuellen Datei */
	segstr=(char *)malloc(filhdr.s_segt);
	if(segstr == NULL)
		error(1,"no space for segmenttable");
	loc += (long)sizeof(struct s_exec);
	dseek(&text, loc, (long)filhdr.s_segt);
	mget((char *)segstr,(long)filhdr.s_segt, &text);
	segcnt = filhdr.s_segt >> 4;  /*Anzahl der Segmente*/
	szstr= (struct szt *)calloc(segcnt,sizeof(struct szt));
	if (szstr == 0)
		error(1,"no space for segmenttable");
	cp = (struct segt *)segstr;
	zp = (struct szt *)szstr;
	for ( x=0 ; x<segcnt ; cp++ , x++ , zp++) {
		if(cp->sg_code) {
			if((long)curt+(long)cp->sg_code >= SEGSIZE) {
				curt = 0 ; ctseg++ ;
			}
			zp->trel = (long)curt - ((long)cp->sg_coff<<8);
			zp->tseg = ctseg;
			curt += cp->sg_code;
		}
		if(cp->sg_data) {
			if((long)curd+(long)cp->sg_data >= SEGSIZE) {
				curd = 0 ; cdseg++;
			}
			zp->drel = (long)curd - ((long)cp->sg_doff<<8);
			zp->dseg = cdseg;
			curd += cp->sg_data;
		}
		if(cp->sg_bss) {
			if((long)curb+(long)cp->sg_bss >= SEGSIZE) {
				curb = 0 ; cbseg++;
			}
			zp->brel = (long)curb - ((long)cp->sg_boff<<8);
			zp->bseg = cbseg;
			curb += cp->sg_bss;
		}
	}		
	ndef = 0;
	nlocal = sizeof(cursym);
	savnext = nextsym;
	loc += filhdr.s_segt;
	loc1=loc + filhdr.s_imsize +((filhdr.s_flag & SF_STRIP)==0 ? filhdr.s_imsize : 0);
	dseek(&text, loc1, (long)filhdr.s_syms);
	curstr = (char *)malloc(filhdr.s_syms);
	if (curstr == NULL)
		error(1, "no space for string table");
	curptr=curstr;
	xsymno = 0;
	while (text.size > 0) {
		symno++;
		symno += xsymno;
		xsymno = 0;
		mget((char *)&cursym, 14L , &text);
		savecur=curptr;
		p = (char *) &cursym.n_un.n_name;
		if( *p & LONGBIT) {
		/* Long name */
			x= *p & LONGMASK;
			for(i=0;i<7;i++)
				*(curptr++) = *(++p);
			x -= 7;
			while(x>0){
				mget((char *)curptr, 14L, &text);
				curptr += 14;
				x -= 14;
				xsymno++;
			}
		}
		else /* short name */ {
			for(i=0;i<8;i++){
				if(p[i]==0) break;
				*(curptr++)=p[i];
			}
		}
		*(curptr++)=0;
		cursym.n_un.n_name=savecur;
		symreloc();
		type = cursym.n_type;
		if ((type&N_EXT) == 0) {
			/*
			symwrite(&cursym, sout);
			*/
			continue;
		}
		if ((sp = *lookup()) == 0)
			error(1, "internal error: symbol not found");
		if ((cursym.n_type&0x3f) == N_EXT+N_UNDF) {
			if (clocseg->lo_used == NSYMPR) {
				if (++clocseg == &locseg[NSEG])
					error(1, "local symbol overflow");
				clocseg->lo_used = 0;
			}
			if (clocseg->lo_first == 0) {
				clocseg->lo_first = (struct local *)
				    malloc(NSYMPR * sizeof (struct local));
				if (clocseg->lo_first == 0)
					error(1, "out of memory (clocseg)");
			}
			lp = &clocseg->lo_first[clocseg->lo_used++];
			lp->l_index = symno;
			lp->l_symbol = sp;
			lp->l_link = lochash[symno % LHSIZ];
			lochash[symno % LHSIZ] = lp;
			continue;
		}
		if (((cursym.n_type&0x3f) != (sp->n_type&0x3f) || cursym.n_value!=sp->n_value)&&wflag==0) {
			printf("   %s: ", cursym.n_un.n_name);
			error(-1, " - warning: multiply defined");
		}
	}
	curt = (unsigned)(torigin & 0xffff);
	curd = (unsigned)(dorigin & 0xffff);
	ctseg = (char)((torigin >> 24) & 0xff);
	cdseg = (char)((dorigin >> 24) & 0xff);
	cp = (struct segt *)segstr;
	zp = (struct szt *)szstr;
	for(x=0; x<segcnt ; x++,cp++,zp++){
		if(cp->sg_code){
			if((long)curt+(long)cp->sg_code >= SEGSIZE) {
				while((curt&(pagesize-1))&&((long)curt<SEGSIZE)){
					bputc(0,tout);
					curt++;
				}
				curt=0;
				ctseg++;
			}
			dseek(&text,loc,(long)cp->sg_code);
			dseek(&reloc,loc+filhdr.s_imsize,(long)cp->sg_code);
			load2td(zp,tout,trout,curt,ctseg);
			curt += cp->sg_code;
			loc += (long)cp->sg_code;
		}
		if(cp->sg_data){
			if((long)curd+(long)cp->sg_data >= SEGSIZE) {
				while((curd&(pagesize-1))&&((long)curd<SEGSIZE)){
					bputc(0,dout);
					curd++;
				}
				curd=0;
				cdseg++;
			}
			dseek(&text,loc,(long)cp->sg_data);
			dseek(&reloc,loc+filhdr.s_imsize,(long)cp->sg_data);
			load2td(zp,dout,drout,curd,cdseg);
			curd += cp->sg_data;
			loc += (long)cp->sg_data;
		}
	}
	torigin = (long)curt + ((long)ctseg << 24);
	dorigin = (long)curd + ((long)cdseg << 24);
	borigin = (long)curb + ((long)cbseg << 24);
	free(curstr);
	free(segstr);
	free(szstr);
}


#define R_EXT	0x0008
#define R_TYPE	0x0007
#define R_OFF	0
#define R_SEG	1
#define R_SSEG	2
#define R_R12	3
#define R_R16	4
/*
 * This routine relocates the single text or data segment argument.
 * Offsets from external symbols are resolved by adding the value
 * of the external symbols.  Non-external reference are updated to account
 * for the relative motion of the segments (ctrel, cdrel, ...).  If
 * a relocation was pc-relative, then we update it to reflect the
 * change in the positioning of the segments by adding the displacement
 * of the referenced segment and subtracting the displacement of the
 * current segment (creloc).
 *
 * If we are saving the relocation information, then we increase
 * each relocation datum address by our base position in the new segment.
 */
load2td(zp, b1, b2,curadr,curseg)
	struct szt *zp;
	struct biobuf *b1, *b2;
	long curadr;
	char curseg;
{
	register int *rp, *cp;
	register int c;
	struct nlist *sp;
	struct local *lp;
	long tw;
	int *rpend, *relp;
	int *codep;
	int num;
	long relsz, codesz;
	int i;
	struct szt *sz;

	relsz = reloc.size;
	relp = (int *)malloc((unsigned)relsz);
	rpend = &relp[(unsigned)relsz / sizeof(int)];
	codesz = text.size;
	codep = (int *)malloc((unsigned)codesz);
	if(relp == 0 || codep == 0)
		error(1,"out of memory(load2td)");
	mget(relp,relsz,&reloc);
	mget(codep,codesz,&text);
	for(rp=relp,cp=codep;rp<rpend;rp++,cp++){
		curadr += 2;
		c = *rp;
		if(c == 0)
			continue;
		if(c & R_EXT) {
		/* externe referenz */
			num = c>>4;
			lp = lochash[num % LHSIZ];
			while(lp->l_index != num) {
				lp = lp->l_link;
				if( lp == 0 )
					error(1,"local symbol botch");
			}
			sp = lp->l_symbol;
			if(((sp->n_type)&0x3f) == N_EXT+N_UNDF)
				error(1,"undefined symbol");
			switch(c & R_TYPE) {
				case R_SEG:
					c= (int)((sp->n_value & 0x7f000000) >> 16);
					*cp = (*cp&0x8000) | c;
					break;
				case R_OFF:
					c= (int)(sp->n_value & 0x0ffff);
					*cp += c;
					break;
				case R_SSEG:
					c = (int)(sp->n_value & 0xff) + (int)((sp->n_value & 0x7f000000) >>16);
					*cp = (*cp&0x0ff) + c;
					break;
				case R_R12:
					printf("extern 12bit rel. adr. %s\n",sp->n_un.n_name);
					c = (char)((sp->n_value & 0x7f000000) >>24);
					if(c != curseg)
						error(1,"external relativ adress(12) to another segment");
					c = (int)(sp->n_value&0x0ffff) - curadr;
					if(c>=2048 || c<= -2048)
						error(1,"relativ adress(12) to large");
					*cp += (c&0x0fff);
					break;
				case R_R16:
					printf("extern 16bit rel. adr. %s\n",sp->n_un.n_name);
					c = (char)((sp->n_value & 0x7f000000) >>24);
					if(c != curseg)
						error(1,"external relativ adress(16) to another segment");
					c = (int)(sp->n_value&0x0ffff) - curadr;
					*cp += c;
					break;
			}
		}
		/* aufgeloeste referenz */
		else {
			if((c & 0x06)==0)
				continue;
			if((c & 0x01)==0)
				error(1,"nonsegmented referenz");
			sz =szstr;
			for (i=0;i < (c>>8); i++ ,sz++);
			switch((c&0x70)>>4) {
				case R_SEG:
					switch (c& 0x06){
						case 0x02 :
							c = ((int)sz->tseg)<<8;
							break;
						case 0x04 :
							c = ((int)sz->dseg)<<8;
							break;
						case 0x06 :
							c =((int)sz->bseg)<<8;
							break;
					}
					*cp = (*cp & 0x8000)+c;
					break;
					
				case R_OFF:
					switch (c& 0x06){
						case 0x02 :
							c = (int)sz->trel;
							break;
						case 0x04 :
							c = (int)sz->drel;
							break;
						case 0x06 :
							c =(int)sz->brel;
							break;
					}
					*cp += c;
					break;
				case R_SSEG:
					printf("aufgeloeste short seg adress\n");
				case R_R12:
					printf("aufgeloeste 12bit rel. adress\n");
				case R_R16:
					printf("aufgeloeste 16bit rel.adress\n");
			}
		}
	}
	bwrite(codep,(unsigned)codesz,b1);
	free(codep);
	free(relp);
}

finishout()
{
	register int i;
	int nsymt;
	unsigned curt,curd;

/*
	if (sflag==0) {
		nsymt = symx(nextsym);
		for (i = 0; i < nsymt; i++)
			symwrite(xsym(i), sout);
		bwrite(&offset, sizeof offset, sout);
	}
*/
	/*die letzen segmente werden mit 0 gefuellt */
	curt = (unsigned)(torigin & 0xffff);
	curd = (unsigned)(dorigin & 0xffff);
	while((curt & (pagesize-1)) && ((long)curt<SEGSIZE)){
		bputc(0,tout);
		curt++;
	}
	while((curd & (pagesize-1))&&((long)curd<SEGSIZE)){
		bputc(0,dout);
		curd++;
	}
	if(sflag == 0) {
		lseek(biofd,12L,0); /* Groesse der Symboltabelle */
		write(biofd,&fsyms,2); /* korrektur der Symboltabellengroesse */
		boffset = 14;
	}
	if (!ofilfnd) {
		unlink("a.out");
		if (link("l.out", "a.out") < 0)
			error(1, "cannot move l.out to a.out");
		ofilename = "a.out";
	}
	delarg = errlev;
	delexit();
}

mkfsym(s)
char *s;
{
	int x;

	if (sflag)
		return;
	if (trace)
		printf("load2arg: %s\n", s);
	cursym.n_un.n_name = s;
	cursym.n_type = N_FN;
	cursym.n_value = torigin;
	x = symwrite(&cursym, sout);
	fsyms += x;
}

struct {
	struct s_nlist bol;
	char longname[128];
}sym;

symwrite(sp,bp)
	struct nlist *sp;
	struct biobuf *bp;
{
	register int len;
	register char *str;
	char *tp, *s;
	int i;

	sym.bol.sn_value = sp->n_value;
	sym.bol.sn_type  = sp->n_type & 0x7f; /* LIBF wird zurueck */
	sym.bol.sn_segt = sp->n_segt;
		switch(sym.bol.sn_segt & 0x60) {
		case 0 :
			break;
		case 0x20 :
			sym.bol.sn_segt += ctnr+1;
			break;
		case 0x40 :
			sym.bol.sn_segt += ctnr+1+(cdnr&0x1f)+1;
			break;
		}
	str=sp->n_un.n_name;
	tp = (char *)sym.bol.sn_name;
	len=strlen(str);
	if(len>8)
		*tp++ = ((char)len + 1) | 0x80;
	strcpy(tp,str);
	tp += len;
	i=0;
	for(s=(char *)&sym ; s<tp ;) {
		s += 14; i += 14;
	}	
	while(tp <= s) *tp++ = 0;
	bwrite((char *)&sym, i, bp);
	return(i);
}
		

off_t	wroff;
struct	biobuf toutb;

setupout()
{
	int bss;
	struct stat stbuf;
	extern char *sys_errlist[];
	extern int errno;
	int i, x, nsymt;
	struct segt *pt;
	char p;

	ofilemode = 0777 & ~umask(0);
	biofd = creat(ofilename, 0666 & ofilemode);
	if (biofd < 0) {
		filname = ofilename;		/* kludge */
		archdr.ar_name[0] = 0;		/* kludge */
		error(1, sys_errlist[errno]);	/* kludge */
	}
	fstat(biofd, &stbuf);		/* suppose file exists, wrong*/
	if (stbuf.st_mode & 0111) {	/* mode, ld fails? */
		chmod(ofilename, stbuf.st_mode & 0666);
		ofilemode = stbuf.st_mode;
	}
	filhdr.s_magic =  S_MAGIC3;
	filhdr.s_imsize = tsize + dsize;
	filhdr.s_bss = bsize;
	filhdr.s_syms = 0;
	filhdr.s_segt = (unsigned)((ctnr&0x1f) + (cdnr&0x1f) + (cbnr&0x1f) +3) <<4;
	if (entrypt) {
		if ((entrypt->n_type&0x3f)!=N_EXT+N_CODE)
			error(0, "entry point not in text");
		else
			filhdr.s_entry = entrypt->n_value;
	} else
		filhdr.s_entry = 0;
	filhdr.s_flag = SF_7FSTK + SF_STRIP;
	filhdr.s_codesz = filhdr.s_lines = 0;
	tout = &toutb;
	bopen(tout, 0L, p_blksize);
	wroff = N_TXTOFF(filhdr)  + tsize;
	outb(&dout, dsize , p_blksize);
	if(sflag==0) {
		outb(&sout,filhdr.s_syms,p_blksize);
		nsymt = symx(nextsym);
		for(i=0;i<nsymt;i++) {
			x=symwrite(xsym(i),sout);
			filhdr.s_syms += x;
		}
		fsyms = filhdr.s_syms;
	}
	bwrite((char *)&filhdr, sizeof (filhdr), tout);
	/*Schreiben der segmenttabelle */
	bwrite(tptr,(((unsigned)ctnr&0x1f)+1) <<4,tout);
	for(pt=dptr,p=0; p<=(cdnr&0x1f); pt++,p++)
		pt->sg_segno = ctnr+p+1;
	bwrite(dptr,(((unsigned)cdnr&0x1f)+1) <<4,tout);
	for(pt=bptr,p=0; p<=(cbnr&0x1f); pt++,p++)
		pt->sg_segno = ctnr+(cdnr&0x1f)+p+2;
	bwrite(bptr,(((unsigned)cbnr&0x1f)+1) <<4,tout);
}

outb(bp, inc, bufsize)
	register struct biobuf **bp;
	long inc;
{

	*bp = (struct biobuf *)malloc(sizeof (struct biobuf));
	if (*bp == 0)
		error(1, "ran out of memory (outb)");
	bopen(*bp, wroff, bufsize);
	wroff += inc;
}

struct nlist **
lookup()
{
	register long sh; 
	register struct nlist **hp;
	register char *cp, *cp1;
	register struct symseg *gp;
	register int i;

	sh = 0;
	for (cp = cursym.n_un.n_name; *cp;)
		sh = (sh<<1) + *cp++;
	sh = (sh & 0x7fffffff) % HSIZE;
	for (gp = symseg; gp < &symseg[NSEG]; gp++) {
		if (gp->sy_first == 0) {
			gp->sy_first = (struct nlist *)
			    calloc(NSYM, sizeof (struct nlist));
			gp->sy_hfirst = (struct nlist **)
			    calloc(HSIZE, sizeof (struct nlist *));
			if (gp->sy_first == 0 || gp->sy_hfirst == 0)
				error(1, "ran out of space for symbol table");
			gp->sy_last = gp->sy_first + NSYM;
			gp->sy_hlast = gp->sy_hfirst + HSIZE;
		}
		if (gp > csymseg)
			csymseg = gp;
		hp = gp->sy_hfirst + sh;
		i = 1;
		do {
			if (*hp == 0) {
				if (gp->sy_used == NSYM)
					break;
				return (hp);
			}
			cp1 = (*hp)->n_un.n_name; 
			for (cp = cursym.n_un.n_name; *cp == *cp1++;)
				if (*cp++ == 0)
					return (hp);
			hp += i;
			i += 2;
			if (hp >= gp->sy_hlast)
				hp -= HSIZE;
		} while (i < HSIZE);
		if (i > HSIZE)
			error(1, "hash table botch");
	}
	error(1, "symbol table overflow");
	/*NOTREACHED*/
}

symfree(saved)
	struct nlist *saved;
{
	register struct symseg *gp;
	register struct nlist *sp;

	for (gp = csymseg; gp >= symseg; gp--, csymseg--) {
		sp = gp->sy_first + gp->sy_used;
		if (sp == saved) {
			nextsym = sp;
			return;
		}
		for (sp--; sp >= gp->sy_first; sp--) {
			gp->sy_hfirst[sp->n_hash] = 0;
			gp->sy_used--;
			if (sp == saved) {
				nextsym = sp;
				return;
			}
		}
	}
	if (saved == 0)
		return;
	error(1, "symfree botch");
}

struct nlist **
slookup(s)
	char *s;
{

	cursym.n_un.n_name = s;
	cursym.n_type = N_EXT+N_UNDF; /* segmented bit ?? */
	cursym.n_value = 0;
	return (lookup());
}

enter(hp)
register struct nlist **hp;
{
	register struct nlist *sp;

	if (*hp==0) {
		if (hp < csymseg->sy_hfirst || hp >= csymseg->sy_hlast)
			error(1, "enter botch");
		*hp = lastsym = sp = csymseg->sy_first + csymseg->sy_used;
		csymseg->sy_used++;
		sp->n_un.n_name = cursym.n_un.n_name;
		sp->n_type = cursym.n_type;
		sp->n_hash = hp - csymseg->sy_hfirst;
		sp->n_value = cursym.n_value;
		nextsym = lastsym + 1;
		return(1);
	} else {
		lastsym = *hp;
		return(0);
	}
}

symx(sp)
	struct nlist *sp;
{
	register struct symseg *gp;

	if (sp == 0)
		return (0);
	for (gp = csymseg; gp >= symseg; gp--)
		/* <= is sloppy so nextsym will always work */
		if (sp >= gp->sy_first && sp <= gp->sy_last)
			return ((gp - symseg) * NSYM + sp - gp->sy_first);
	error(1, "symx botch");
	/*NOTREACHED*/
}

symreloc()
{
	register struct szt *sp;
	register long val;
	int i;

	sp=szstr;
	for(i=0 ; i < (int)cursym.n_segt ; i++ ) sp++;
	switch (cursym.n_type & 0x3f) {

	case N_CODE:
	case N_EXT+N_CODE:
		cursym.n_segt = sp->tseg;
		val = ((long)sp->tseg)<<24;
		val += cursym.n_value&0xffff;
		cursym.n_value = val + sp->trel;
		break;

	case N_DATA:
	case N_EXT+N_DATA:
		cursym.n_segt = sp->dseg;
		val = ((long)sp->dseg)<<24;
		val += cursym.n_value&0xffff;
		cursym.n_value = val + sp->drel;
		break;

	case N_BSS:
	case N_EXT+N_BSS:
		cursym.n_segt = sp->bseg;
		val = ((long)sp->bseg)<<24;
		val += cursym.n_value&0xffff;
		cursym.n_value = val + sp->brel;
		break;

	case N_EXT+N_UNDF:
		break;

	default:
		if (cursym.n_type&N_EXT)
			cursym.n_type = N_EXT+N_ABS;
		break;
	}
}

getarhdr()
{
	register char *cp;

	mget((char *)&archdr,(long) sizeof (archdr), &text);
	for (cp=archdr.ar_name; cp<&archdr.ar_name[sizeof(archdr.ar_name)];)
		if (*cp++ == ' ') {
			cp[-1] = 0;
			return;
		}
}

mget(loc, n, sp)
register STREAM *sp;
long n;
register char *loc;
{
	register char *p;
	register long take;

top:
	if (n == 0)
		return;
	if (sp->size && sp->nibuf) {
		p = sp->ptr;
		take = sp->size;
		if (take > (long)sp->nibuf)
			take = (long)sp->nibuf;
		if (take > n)
			take = n;
		n -= take;
		sp->size -= take;
		sp->nibuf -= (int)take;
		sp->pos += take;
		do
			*loc++ = *p++;
		while (--take > 0);
		sp->ptr = p;
		goto top;
	}
	if (n > (long)p_blksize) {
		take = n - n % (long)p_blksize;
		lseek(infil, (long)(sp->bno+1)<<p_blkshift, 0);
		if (take > sp->size || read(infil, loc,(int)take) != (unsigned)take)
			error(1, "premature EOF");
		loc += take;
		n -= take;
		sp->size -= take;
		sp->pos += take;
		dseek(sp, (long)(sp->bno+1+(take>>p_blkshift))<<p_blkshift, -1L);
		goto top;
	}
	*loc++ = get(sp);
	--n;
	goto top;
}

dseek(sp, loc, s)
register STREAM *sp;
long loc, s;
{
	register PAGE *p;
	register long b, o;
	int n;

	b = loc>>p_blkshift;
	o = loc&(long)p_blkmask;
	if (o&01)
		error(1, "loader error; odd offset");
	--sp->pno->nuser;
	if(((p = &page[0])->bno != b) && (p = &page[1])->bno!=b)
		if (p->nuser==0 || (p = &page[0])->nuser==0) {
			if  (page[0].nuser==0 && page[1].nuser==0)
				if (page[0].bno < page[1].bno)
					p = &page[0];
			p->bno = (int)b;
			lseek(infil, loc & ~(long)p_blkmask, 0);
			if ((n = read(infil, p->buff, p_blksize)) < 0)
				n = 0;
			p->nibuf = n;
		} else
			error(1, "botch: no pages");
	++p->nuser;
	sp->bno = (int)b;
	sp->pno = p;
	if (s != -1) {sp->size = s; sp->pos = 0;}
	sp->ptr = (char *)(p->buff + o);
	if ((sp->nibuf = p->nibuf-(int)o) <= 0)
		sp->size = 0;
}

char
get(asp)
STREAM *asp;
{
	register STREAM *sp;

	sp = asp;
	if ((sp->nibuf -= (int)sizeof(char)) < 0) {
		dseek(sp, ((long)(sp->bno+1)<<p_blkshift), (long)-1);
		sp->nibuf -= sizeof(char);
	}
	if ((sp->size -= (long)sizeof(char)) <= 0) {
		if (sp->size < 0)
			error(1, "premature EOF");
		++fpage.nuser;
		--sp->pno->nuser;
		sp->pno = (PAGE *) &fpage;
	}
	sp->pos += sizeof(char);
	return(*sp->ptr++);
}

getfile(acp)
char *acp;
{
	register int c;
	char arcmag[SARMAG+1];
	struct stat stb;
	int mag;

	archdr.ar_name[0] = '\0';
	filname = acp;
	if (filname[0] == '-' && filname[1] == 'l')
		infil = libopen(filname + 2, O_RDONLY);
	else
		infil = open(filname, O_RDONLY);
	if (infil < 0)
		error(1, "cannot open");
	fstat(infil, &stb);
	page[0].bno = page[1].bno = -1;
	page[0].nuser = page[1].nuser = 0;
	/*c = stb.st_blksize;  bei WEGA nicht vorhanden*/
	c = BLKSIZE;
	if (c == 0 || (c & (c - 1)) != 0) {
		/* use default size if not a power of two */
		c = BLKSIZE;
	}
	if (p_blksize != c) {
		p_blksize = c;
		p_blkmask = c - 1;
		for (p_blkshift = 0; c > 1 ; p_blkshift++)
			c >>= 1;
		if (page[0].buff != NULL)
			free(page[0].buff);
		page[0].buff = (char *)malloc(p_blksize);
		if (page[0].buff == NULL)
			error(1, "ran out of memory (getfile)");
		if (page[1].buff != NULL)
			free(page[1].buff);
		page[1].buff = (char *)malloc(p_blksize);
		if (page[1].buff == NULL)
			error(1, "ran out of memory (getfile)");
	}
	text.pno = reloc.pno = (PAGE *) &fpage;
	fpage.nuser = 2;
	dseek(&text, 0L, SARMAG);
	if (text.size <= 0)
		error(1, "premature EOF");
	mget((char *)arcmag,2L, &text); 
	mag=ARMAG;
	if (strncmp(arcmag,&mag,2))
		return (0);
	dseek(&text, SARMAG, (long)sizeof archdr);
	if (text.size <= 0)
		return (1);
	getarhdr();
	if (strncmp(archdr.ar_name, "__.SYMDEF", sizeof(archdr.ar_name)) != 0)
		return (1);
	return (stb.st_mtime > archdr.ar_date ? 3 : 2);
}

/*
 * Search for a library with given name
 * using the directory search array.
 */
libopen(name, oflags)
char *name;
int oflags;
{
	register char *p, *cp;
	register int i;
	static char buf[MAXPATHLEN+1];
	int fd = -1;

	if (*name == '\0')			/* backwards compat */
		name = "a";
	for (i = 0; i < ndir && fd == -1; i++) {
		p = buf;
		for (cp = dirs[i]; *cp; *p++ = *cp++)
			;
		*p++ = '/';
		for (cp = "Llib"; *cp; *p++ = *cp++)
			;
		for (cp = name; *cp; *p++ = *cp++)
			;
		cp = ".a";
		while (*p++ = *cp++)
			;
		fd = open(buf, oflags);
	}
	if (fd != -1)
		filname = buf;
	return (fd);
}


error(n, s)
char *s;
{

	if (errlev==0)
		printf("lld:");
	if (filname) {
		printf("%s", filname);
		if (n != -1 && archdr.ar_name[0])
			printf("(%s)", archdr.ar_name);
		printf(": ");
	}
	printf("%s\n", s);
	if (n == -1)
		return;
	if (n)
		delexit();
	errlev = 2;
}

readhdr(loc)
off_t loc;
{

	dseek(&text, loc, (long)sizeof(filhdr));
	mget((short *)&filhdr,(long) sizeof(filhdr), &text);
	if (N_BADMAG(filhdr)) {
		if (filhdr.s_magic == OARMAG)
			error(1, "old archive");
		error(1, "bad magic number");
	}
	if (filhdr.s_imsize & 01)
		error(1, "text/data size odd");
}

long round(v, r)
long v;
long r;
{

	r--;
	v += r;
	v &= ~r;
	return(v);
}
#define NSAVETAB 8192
char *savetab;
int saveleft;

char *savestr(cp)
	register char *cp;
{
	register int len;

	len = strlen(cp) + 1;
	if (len > saveleft) {
		saveleft = NSAVETAB;
		if (len > saveleft)
			saveleft = len;
		savetab = malloc(saveleft);
		if (savetab == 0)
			error(1, "ran out of memory (savestr)");
	}
	strncpy(savetab, cp, len);
	cp = savetab;
	savetab += len;
	saveleft -= len;
	return (cp);
}


bopen(bp, off, bufsize)
	register struct biobuf *bp;
	off_t off;
{

	bp->b_ptr = bp->b_buf = (char *)malloc(bufsize);
	if (bp->b_ptr == (char *)0)
		error(1, "ran out of memory (bopen)");
	bp->b_bufsize = bufsize;
	bp->b_nleft = bufsize - (off % bufsize);
	bp->b_off = off;
	bp->b_link = biobufs;
	biobufs = bp;
}

int	bwrerror;

bwrite(p, cnt, bp)
	register char *p;
	register unsigned cnt;
	register struct biobuf *bp;
{
	register unsigned put;
	register char *to;

top:
	if (cnt == 0)
		return;
	if (bp->b_nleft) {
		put = bp->b_nleft;
		if (put > cnt)
			put = cnt;
		bp->b_nleft -= put;
		to = bp->b_ptr;
		bcopy(p, to, put);
		bp->b_ptr += put;
		p += put;
		cnt -= put;
		goto top;
	}
	if (cnt >= bp->b_bufsize) {
		if (bp->b_ptr != bp->b_buf)
			bflush1(bp);
		put = cnt - cnt % bp->b_bufsize;
		if (boffset != bp->b_off)
			lseek(biofd, bp->b_off, 0);
		if (write(biofd, p, put) != put) {
			bwrerror = 1;
			error(1, "output write error");
		}
		bp->b_off += put;
		boffset = bp->b_off;
		p += put;
		cnt -= put;
		goto top;
	}
	bflush1(bp);
	goto top;
}

bcopy(p1,p2,i)
register char *p1,*p2;
register int i;
{
	while(i>0){
		*p2++ = *p1++;
		i--;
	}
}
bflush()
{
	register struct biobuf *bp;

	if (bwrerror)
		return;
	for (bp = biobufs; bp; bp = bp->b_link)
		bflush1(bp);
}

bflush1(bp)
	register struct biobuf *bp;
{
	register long cnt = bp->b_ptr - bp->b_buf;

	if (cnt == 0)
		return;
	if (boffset != bp->b_off)
		lseek(biofd, bp->b_off, 0);
	if (write(biofd, bp->b_buf, (unsigned)cnt) != (unsigned)cnt) {
		bwrerror = 1;
		error(1, "output write error");
	}
	bp->b_off += cnt;
	boffset = bp->b_off;
	bp->b_ptr = bp->b_buf;
	bp->b_nleft = bp->b_bufsize;
}

bflushc(bp, c)
	register char c;
	register struct biobuf *bp;
{

	bflush1(bp);
	bputc(c, bp);
}

bseek(bp, off)
	register struct biobuf *bp;
	register off_t off;
{
	bflush1(bp);
	
	bp->b_nleft = bp->b_bufsize - (off % bp->b_bufsize);
	bp->b_off = off;
}

struct tynames {
	int	ty_value;
	char	*ty_name;
} tynames[] = {
	N_UNDF,	"undefined",
	N_ABS,	"absolute",
	N_CODE,	"text",
	N_DATA,	"data",
	N_BSS,	"bss",
	0,	0,
};

tracesym()
{
	register struct tynames *tp;

	printf("%s", filname);
	if (archdr.ar_name[0])
		printf("(%s)", archdr.ar_name);
	printf(": ");
	if ((cursym.n_type&N_TYPE) == N_UNDF && cursym.n_value) {
		printf("definition of common %s size %d\n",
		    cursym.n_un.n_name, cursym.n_value);
		return;
	}
	for (tp = tynames; tp->ty_name; tp++)
		if (tp->ty_value == (cursym.n_type&N_TYPE))
			break;
	printf((cursym.n_type&N_TYPE) ? "definition of" : "reference to");
	if (cursym.n_type&N_EXT)
		printf(" external");
	if (tp->ty_name)
		printf(" %s", tp->ty_name);
	printf(" %s\n", cursym.n_un.n_name);
}

