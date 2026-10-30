/*
 * ld.h - declarations shared by the C files of ld (structs, variables). The variables are defined in ld.c.
 * Names follow sld's ld.h where the use is the same; the others keep their address names (v<addr>) until
 * their use is known.
 */

/* header of a page of the input buffer (sld: page); nuser counts the streams that use it */
struct pghdr {
	int nuser;
	int bno;
};
/* input stream: the words are read from the page pno (ptr at 0: get and mget use it, layout proven by the code) */
struct stream {
	int *ptr;
	int bno;
	int nibuf;
	int size;
	struct pghdr *pno;
};
/* input buffering (sld: page) */
struct page {
	int nuser;
	int bno;
	int nibuf;
	int buff[256];
};
/* tab entry: the state of the load before an object, saved by record and restored by -v (Plexus struct overlay) */
struct overlay {
	int argsav;
	int symsav;
	long *libsav;
	char *vname;
	int ctsav, cdsav, cbsav;
	int offt, offd, offb, offs;
};
/* output or input buffer with the byte count left (sld: struct buf) */
struct buf {
	int fildes;
	int nleft;
	char *xnext;
	char iobuf[256];
};
/* archive member header (sld: archdr) */
struct archdr {
	char aname[14];
	long atime;
	char auid;
	char agid;
	int amode;
	long asize;
};
/* local symbol table entry: index -> value, 4 bytes (sld: struct local with a symbol pointer, 6 bytes) */
struct local {
	int locindex;
	int locsymbol;
};

extern struct page page[2];
extern struct pghdr fpage;
extern int infil;
extern int ntab;
extern struct overlay tab[16];
extern int symindex;
extern long *libp;
extern int ttotal, dtotal, btotal, stotal;
extern char *ofilnam;
extern int nundef;
extern char tmpname[];
extern int Tflag, arflag;
extern struct stream text;
extern struct buf toutb;
extern struct archdr archdr;
extern char *pname;
extern char *filname;
extern int exstat;
extern struct local local[1000];
/* symbol table: functions that return a symbol pointer (sld: table16_entry, next_entry) */
struct symbol *table16_entry();
struct symbol *next_entry();
/* current symbol (sld: cursym). The layout is the one of ld's code: svalue at 0x1076, stype at 4, sname at 6 (the
   address of pack_name's target), 14 bytes; sld's struct symbol has sseg and a 16 byte entry. */
struct symbol {
	long svalue;
	char stype;
	char sxtra;
	char sname[8];
};
extern struct symbol cursym;
extern struct symbol sym16[600];
/* hash table of the symbols (sld: hshtab, indexed from 2; ld hashes with 0x0923), the name buffer of the
   current symbol (sld: lnbuf) and the flag of the -N option (sld: nlflag) */
extern struct symbol *hshtab[];
extern char lnbuf[];
extern int nlflag;
struct symbol **lookup_name();
/* symbol table (sld: symtab, nsym); entries are 14 bytes in ld */
extern struct symbol *symtab;
/* flag of -w (sld: wflag); put_name writes a symbol name and returns its length (copyname.c / printname.c) */
extern int wflag;
int put_name();
/* output of words and bytes (sld: mput, put_int_buf); the swap flag of the input (sld: swapflg) */
int mput();
extern int swapflg;
int longswap();
/* flags and output buffers of the symbol output (sld: sflag, xflag, soutb); torigin: start of the text of the current object (Plexus) */
extern int sflag, xflag;
extern struct buf soutb;
extern int torigin, nlocsym;
int unpack_run();
int put_sym();
/* relocation bases of the object: text, data, bss (sld: fctrel, fcdrel, fcbrel) */
extern int fctrel, fcdrel, fcbrel;
limit_msg();
extern struct symbol **symhash[];
extern struct symbol *lastsym;
int enter();
extern struct symbol *nsym;
int index16();
int getfile();
int dseek();
int load2();
int mkfsym();
int load2arg();
extern struct stream reloc;
extern char libname[];
extern char libpath[];
extern int noswap;
int getfile();
char *rindex();
char *grow_rlbuf();
/* entry of the ranlib table (sld: rlent, 12 bytes) */
struct rlent {
	long roff;
	char rname[8];
};
struct symbol **pslookup();
int chk_ranlib();
int sbrk();
int read_symbols();
int swap();
int load_header();
extern struct rlent *rltab;
extern int rlcnt;
int ranlib_scan();
char *ensure_buf();
int lock_rlbuf();
int unlock_rlbuf();
int chk_ranlib();
int getfile();
/* output header of ld (sld: filhdr), 16 bit fields at bss 0x1062: the offsets are those of the code (readhdr, init_entry);
   ftsize and fdsize are the sizes of text and data in the file (8-bit objects: from hy and hx, else tsize and dsize) */
struct filhdr {
	unsigned fmagic;
	unsigned tsize;
	unsigned ftsize;
	unsigned dsize;
	unsigned fdsize;
	unsigned bsize;
	unsigned ssize;
	unsigned entry;
	char fstamp[2];
	unsigned relflg;
};
extern struct filhdr filhdr;
/* header of an object or of the output (sld: objhdr) and a segment record of the output (sld: fseg) */
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
extern int hdrsz, segtsz;
extern int idsep;
int swab();
int swap_quad();
int readhdr();
extern int oflag, iflag, fttotal, fdtotal;
extern struct symbol *entrypt;
int load2td();
int half();
int read_entry_hdr();
int put_name();
int symreloc();
int put_sym();
int dseek();
struct symbol **lookup_name();
int swap();
int put_word();
struct areas { char kind; char pad; int tstart; int tsize; int tend; };
extern struct areas areas[4];
extern char secflg[5];
extern char *secnam[4];
extern int secorg[4];
int place_sec();
struct symbol **slookup();
