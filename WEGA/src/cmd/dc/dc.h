#define FATAL 0
#define NFATAL 1
#define BLK sizeof(struct blk)
#define PTRSZ sizeof(int *)
#define HEADSZ 1024
#define STKSZ 100
#define RDSKSZ 100
#define TBLSZ 256
#define ARRAYST 0241
#define MAXIND 2048
#define NL 1
#define NG 2
#define NE 3
#define length(p) ((p)->wt-(p)->beg)
#define rewind(p) (p)->rd=(p)->beg
#define create(p)	(p)->rd = (p)->wt = (p)->beg
#define fsfile(p)	(p)->rd = (p)->wt
#define truncate(p)	(p)->wt = (p)->rd
#define sfeof(p)	(((p)->rd==(p)->wt)?1:0)
#define sfbeg(p)	(((p)->rd==(p)->beg)?1:0)
#define sungetc(p,c)	*(--(p)->rd)=c
#ifdef interdata
#define NEGBYTE 0200
#define MASK (-1 & ~0377)
#define sgetc(p)	( ((p)->rd==(p)->wt) ? EOF :( ((*(p)->rd & NEGBYTE) != 0) ? ( *(p)->rd++ | MASK): *(p)->rd++ ))
#define slookc(p)	( ((p)->rd==(p)->wt) ? EOF :( ((*(p)->rd & NEGBYTE) != 0) ? (*(p)->rd | MASK) : *(p)->rd ))
#define sbackc(p)	( ((p)->rd==(p)->beg) ? EOF :( ((*(--(p)->rd) & NEGBYTE) != 0) ? (*(p)->rd | MASK): *(p)->rd ))
#endif
#ifndef interdata
#define sgetc(p)	(((p)->rd==(p)->wt)?EOF:*(p)->rd++)
#define slookc(p)	(((p)->rd==(p)->wt)?EOF:*(p)->rd)
#define sbackc(p)	(((p)->rd==(p)->beg)?EOF:*(--(p)->rd))
#endif
#define sputc(p,c)	{if((p)->wt==(p)->last)more(p); *(p)->wt++ = c; }
#define salterc(p,c)	{if((p)->rd==(p)->last)more(p); *(p)->rd++ = c; if((p)->rd>(p)->wt)(p)->wt=(p)->rd;}
#define sunputc(p)	(*( (p)->rd = --(p)->wt))
#define zero(p)	for(pp=(p)->beg;pp<(p)->last;)*pp++='\0'
#define OUTC(x) {printf("%c",x); if(--count == 0){printf("\\\n"); count=ll;} }
#define TEST2	{if((count -= 2) <=0){printf("\\\n");count=ll;}}
#define EMPTY if(stkerr != 0){printf("stack empty\n"); continue; }
#define EMPTYR(x) if(stkerr!=0){pushp(x);printf("stack empty\n");continue;}
#define EMPTYS if(stkerr != 0){printf("stack empty\n"); return(1);}
#define EMPTYSR(x) if(stkerr !=0){printf("stack empty\n");pushp(x);return(1);}
#define error(p)	{printf(p); continue; }
#define errorrt(p)	{printf(p); return(1); }
struct blk {
	char	*rd;
	char	*wt;
	char	*beg;
	char	*last;
};
struct	blk *getwd();
struct	blk *lookwd();
struct	blk *getdec();
struct	blk *morehd();

struct	blk *pop(),*readin(),*add0(),*mult();
struct	blk *scalint();
struct	blk *removc();
struct	blk *add(),*div(),*removr();
struct	blk *exp();
struct	blk *sqrt();
struct	blk *salloc(),*copy();
struct	blk *scale();
struct	sym {
	struct	sym *next;
	struct	blk *val;
};
struct	wblk {
	struct blk **rdw;
	struct blk **wtw;
	struct blk **begw;
	struct blk **lastw;
};
FILE	*curfile;
struct	blk	*scalptr;
long	all;
struct	sym	*sptr;
int	fw;
struct	blk	**stkend;
int	ifile;
int	(*outdit)();
char	**svargv;
char	*pp;
struct	sym symlst[TBLSZ];
long	headmor;
char	*dummy;
long	nbytes;
long	rel;
int	k;
int	neg;
struct	blk	*rem;
struct	blk	*hfree;
struct	blk	*stack[STKSZ];
struct	sym	*sfree;
int	logo;
char	savk;
struct	blk	*arg1;
struct	blk	*inbas;
struct	blk	*arg2;
struct	blk	*readstk[RDSKSZ];
struct	blk	**readptr;
struct	sym	*stable[TBLSZ];
int	lastchar;
int	dbg;
int	log10;
struct	blk	*chptr;
struct	blk	**stkptr;
struct	blk	*strptr;
int	ll;
int	fw1;
int	svargc;
struct	blk	**stkbeg;
int	skd;
int	skr;
struct	blk	*basptr;
long	obase;
struct	blk	*sqtemp;
struct	blk	*tenptr;
struct	blk	*irem;
struct	blk	*divxyz;
int	stkerr;
FILE	*fsave;
int	count;
int	bigot(),hexot();
int	(*signal())();
int	onintr();
char	*malloc();
char	*nalloc();
char	*realloc();
