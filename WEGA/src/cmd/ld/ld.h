/*
 * ld.h - declarations shared by the C files of sld (structs, functions, variables).
 * The uninitialized variables are defined in copyname.c (in the order of their allocation, see there), the initialized ones in the
 * file that owns them.
 */

/* pass two segment table entry (dump strings: segno, s_coff, s_doff, s_boff, s_code, s_data, s_bss, atr, torig, dorig, borig) */
struct p2seg {
	char segno;
	char s_coff;
	char s_doff;
	char s_boff;
	int s_code;
	int s_data;
	int s_bss;
	int atr;
	char pad[4];
	int segidx;
	struct p2seg *nxt;
	struct p2seg *prv;
	int torig;
	int dorig;
	int borig;
};
/* segment table entry of an object file: the first 16 bytes of a p1seg */
struct fseg {
	char segno;
	char s_coff;
	char s_doff;
	char s_boff;
	int s_code;
	int s_data;
	int s_bss;
	int atr;
	char pad1[4];
};
struct bytes2 {
	char c0;
	char c1;
};
/* input buffering (Plexus ld: page, stream) */
struct page {
	int nuser;
	int bno;
	int nibuf;
	int buff[256];
};
/* archive member header (Plexus ld: archdr; the times and the size are longs here) */
struct archdr {
	char aname[14];
	long atime;
	char auid;
	char agid;
	int amode;
	long asize;
};
/* symbol table entry and the current symbol (Plexus ld: symbol, cursym; the layout is WEGA's: 16 bytes) */
struct symbol {
	long svalue;
	char stype;
	char sxtra;
	int sseg;
	char sname[8];
};
/* symbol of a nonsegmented object file (Plexus ld: symbol; 12 bytes) */
struct fsym {
	char fname[8];
	int ftype;
	unsigned fvalue;
};
/* symbol of a segmented object file (14 bytes) */
struct ssym {
	long ssval;
	char sstyp;
	char ssseg;
	char ssnam[8];
};
/* entry of the ranlib table of contents of an archive (12 bytes) */
struct rlent {
	long roff;
	char rname[8];
};
/* local symbol table of a file: index in the file -> symbol (Plexus ld: local; 6 bytes) */
struct local {
	int locindex;
	struct symbol *locsymbol;
};
/* header of an object file as it is read (hx: not used here) and the header of the output (Plexus ld: filhdr; 24 bytes) */
struct objhdr {
	int hmagic;
	long hx;
	long hbss;
	int hsegsz;
	int hsymsz;
	long hentry;
	int hrelflg;
	int hy;
	int hz;
};
struct filhdr {
	unsigned fmagic;
	long tsize;
	long dsize;
	long bsize;
	unsigned ssize;
	long entry;
	char fstamp[2];
	int relflg;
};
/* output buffer (Plexus ld: buf) */
struct buf {
	int fildes;
	int nleft;
	int *xnext;
	int iobuf[256];
};
struct pghdr {
	int nuser;
	int bno;
};
struct stream {
	int sres;
	int bno;
	int nibuf;
	int *ptr;
	int size;
	struct page *pno;
};
/* reservation list of the segments (dump_res: txtptr, datptr, bssptr, res); one row per segment number */
struct reslist { int txtptr; int datptr; int bssptr; int res; };
/* entry of the list of files of -M (mlist_add): the segments for t, d, b of a file name */
struct mlist { int mtext; int mdata; int mbss; int mnext; char *mname; };
/* the areas of a section while it is placed (place_sec): kind, start, size, end */
struct areas { char kind; unsigned tstart; unsigned tsize; unsigned tend; };
/* pass one segment table entry (one per segment of an input file; dump strings: s_code, ctrel, ncoff, ncode, xtnext ...) */
struct p1seg {
	char segno;
	char s_coff;
	char s_doff;
	char s_boff;
	int s_code;
	int s_data;
	int s_bss;
	int atr;
	char pad1[4];
	int ctrel;
	int cdrel;
	int cbrel;
	struct p2seg *ncode;
	struct p2seg *ndata;
	struct p2seg *nbss;
	int ncoff;
	int ndoff;
	int nboff;
	long tbase;
	long dbase;
	int xtnext;
	int datnext;
	int bssnext;
};

/* functions */
long calloc();
long signal();
int flush_buffer();
int swap_bytes();
int unpack_run();
int map_sym();
put_sym();
int pack_name();
struct symbol **lookup_name();
struct symbol *table16_entry();
struct symbol *next_entry();
char *realloc();
int put_int_buf();
int lseek();
int read();
int close();
int put_name();
int write();
struct p1seg *get_block();
int copy_blocks();
int copy_segs();
int copy();
int put_all();
int unlink();
int link();
int patch_hdr();
int sig_handler();
int open();
int report_error();
int get();
int dseek();
int mget();
int swap();
int read_symbols();
int creat();
int mput();
int swap_pair();
int putchar();
int printf();
int free();
int getfile();
int load2();
struct p2seg *table32_entry();
int trunc_name();
int dump_sym();
int warn_sym_redef();
int cvt_fsym();
int alloc_segment();
int half();
int resv_seg();
struct symbol **pslookup();
int load_header();
int ranlib_scan();
int chk_ranlib();
long ensure_buf();
int sscanf();
int end_pass1();
int start_output();
int load2arg();
int finish_output();
char *strchr();
int parse_args();
int ldrsym();
int readhdr();
int mkfsym();
int read_entry_hdr();
int load2seg();
int swap_quad();
int load2td();
struct symbol *table_lookup();
int index16();
int swab();
int make_blk();
int str_compare();
int seg_pad();
int enter();
int symreloc();
int add16_checked();
char *rindex();
int strcpy();
int strcat();
int strncmp();
int fstat();
int copy_leafname();
int mktemp();
int open_tmp();
int init_entry();
int init_signals();
int exit();
int symtab_alloc();
int scan_options();
int segtab_alloc();
int load_lib_syms();
int reg_sect_addr();
int scan_m_opt();
int read_words();
struct symbol **slookup();
int resize_symtab();
int access();
int lay_out();
int atoi();
char *strrchr();
int mlist_add();
int high_byte();
int split_seg();
struct p2seg *ins_block();
int place_sec();
int alloc_resv();
int list_move_after();
swab12();
pack_entry();
walk_entries();
limit_msg();
int put_pad();
int main();
int dmp_p1();
int dump_p2();
int dump_res();

/* variables */
extern int cmpmode;
extern struct p1seg **blktab;
extern int delarg;
extern int nundef;
extern int arflag;
extern int rflag;
extern char tmpname[];
extern char tfname[];
extern char *ofilnam;
extern struct local local[];
extern int ncopied;
extern int sflag;
extern int xflag;
extern struct buf soutb;
extern int symindex;
extern struct buf toutb;
extern unsigned nsym;
extern struct symbol *symtab;
extern struct p2seg p2tab[];
extern int maxseg;
extern struct buf tmpoutb;
extern struct buf doutb;
extern struct buf troutb;
extern struct buf droutb;
extern struct reslist reslist[];
extern struct symbol *lastsym;
extern struct symbol **symhash[];
extern int fctrel;
extern int fcdrel;
extern int fcbrel;
extern struct p2seg *p2head;
extern struct p2seg *p2tail;
extern int wflag;
extern int ofilfnd;
extern int exstat;
extern struct buf routb;
extern char *aoutnam;
extern int nwsym;
extern long ttotal;
extern long dtotal;
extern long btotal;
extern int tbflag;
extern int zflag;
extern long *libp;
extern int swapflg;
extern struct page page[2];
extern struct pghdr fpage;
extern struct stream text;
extern struct stream reloc;
extern struct archdr archdr;
extern struct fsym fsym;
extern struct symbol cursym;
extern struct filhdr filhdr;
extern int maxblk;
extern int nblks;
extern int unused3be;
extern int nrflag;
extern int nlflag;
extern int pnflag;
extern unsigned rlsize;
extern long rlbuf;
extern struct symbol *p_edata;
extern int infil;
extern int np2seg;
extern struct p2seg *dsplseg;
extern struct p2seg *bsplseg;
extern int unused15d8;
extern int fsegflg;
extern int segobj;
extern char cmdbuf[];
extern char libname[];
extern char *cmdargv[];
extern char **cmdav;
extern int cmdac;
extern char *pname;
extern char *filname;
extern int stksegn;
extern int fblk;
extern int nblk;
extern struct rlent *rltab;
extern int rlcnt;
extern char secspec;
extern int iflag;
extern int secorg[][5];
extern char liblist[];
extern char *lastt;
extern char *lastd;
extern char *lastb;
extern int segtsz;
extern int hdrsz;
extern int Sflag;
extern int Xflag;
extern int nsymf;
extern int segmode;
extern int mlhead;
extern struct mlist mlist[];
extern struct symbol *hshtab[];
extern int symbytes;
extern char libpath[];
extern long tcum;
extern long dcum;
extern int dseg;
extern int tseg;
extern int bseg;
extern int Oflag;
extern struct symbol *entrypt;
extern char **curarg;
extern int incmd;
extern int mcnt;
extern int dflag;
extern char **cmdfile;
extern struct symbol *p_etext;
extern struct symbol *p_end;
extern struct symbol *p_stkseg;
extern long comsize;
extern char secflg[][5];
extern struct areas areas[];
extern int dsplf;
extern int bsplf;
extern int placekd;
extern char *secnam[];
extern int np2new;
extern int mlcnt;
extern int unused484;
extern char lnbuf[];
extern char symdef[];
extern char premeof[];

/* names that are part of a larger variable (same address in the original) */
extern char _ctype[];
#define tab42b4 ((char *)secflg)
#define ctab (_ctype + 1)
