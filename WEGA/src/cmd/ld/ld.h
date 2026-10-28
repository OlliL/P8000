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
/* entry of the list of files of -M (f410c): the segments for t, d, b of a file name */
struct mlist { int mtext; int mdata; int mbss; int mnext; char *mname; };
/* the areas of a section while it is placed (place_sec): kind, start, size, end */
struct tab428a { char kind; unsigned tstart; unsigned tsize; unsigned tend; };
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
int f6c6a();
f6e1a();
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
int f6134();
int f2dde();
int f6efe();
int unlink();
int link();
int f2d4c();
int sig_handler();
int open();
int report_error();
int get();
int dseek();
int mget();
int swap();
int read_symbols();
int creat();
int f2f04();
int swap_pair();
int putchar();
int printf();
int free();
int getfile();
int load2();
struct p2seg *table32_entry();
int f0178();
int dump_sym();
int warn_sym_redef();
int f6800();
int alloc_segment();
int half();
int f42f8();
struct symbol **f6b84();
int load_header();
int f1560();
int f6f46();
long ensure_buf();
int sscanf();
int end_pass1();
int start_output();
int load2arg();
int finish_output();
char *strchr();
int parse_args();
int f68a2();
int f3516();
int f6a08();
int read_entry_hdr();
int f2624();
int swap_quad();
int f27d8();
struct symbol *table_lookup();
int index16();
int swab();
int make_blk();
int str_compare();
int f60f0();
int f6bae();
int f6da8();
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
struct symbol **f6b5a();
int f702a();
int access();
int lay_out();
int atoi();
char *strrchr();
int f410c();
int high_byte();
int f1f96();
struct p2seg *ins_block();
int place_sec();
int f4968();
int list_move_after();
swab12();
pack_entry();
walk_entries();
limit_msg();
int f64d0();
int main();
int dmp_p1();
int dump_p2();
int dump_res();

/* variables */
extern int g0476;
extern struct p1seg **g03ba;
extern int g04e8;
extern int g04e6;
extern int bE496;
extern int bE4A2;
extern char tmpname[];
extern char g049e[];
extern char *p0494;
extern struct local tab84d8[];
extern int bE3E7A;
extern int bE4A4;
extern int bE4AA;
extern struct buf bF10A;
extern int bE4FA;
extern struct buf bE4DA;
extern unsigned g04ee;
extern struct symbol *gE47E;
extern struct p2seg tab1c02[];
extern int bE3E82;
extern struct buf bE6E2;
extern struct buf bE8EA;
extern struct buf bEF02;
extern struct buf bEAF2;
extern struct reslist tblF332[];
extern struct symbol *bE4FC;
extern struct symbol **tblE500[];
extern int bE4D0;
extern int bE4D2;
extern int bE4D4;
extern struct p2seg *bF32E;
extern struct p2seg *b3C12;
extern int bE4BA;
extern int bE4BC;
extern int bE4A3A;
extern struct buf bECFA;
extern char *bE048A;
extern int bE4F0;
extern long bE3C16;
extern long bE3C1A;
extern long b3C1E;
extern int bE4A8;
extern int bE4AE;
extern long *g0486;
extern int bE4B6;
extern struct page page[2];
extern struct pghdr fpage;
extern struct stream text;
extern struct stream reloc;
extern struct archdr archdr;
extern struct fsym fsym;
extern struct symbol cursym;
extern struct filhdr filhdr;
extern int bE4EC;
extern int bE3B8;
extern int g03be;
extern char bE49E[];
extern int g04ea;
extern int bE1532;
extern unsigned bE15DA;
extern long bE190DE;
extern struct symbol *bE486;
extern int bE4BE;
extern int bE3C04;
extern struct p2seg *bF324;
extern struct p2seg *bF328;
extern int g15d8;
extern int bE4B0;
extern int bE4B2;
extern char buf6988[];
extern char buf9098[];
extern char *tab4a48[];
extern char **g4a40;
extern int bE4A44;
extern char *pE4D6;
extern char *bE4C0;
extern int g0474;
extern int bE3C02;
extern int bEF32C;
extern struct rlent *bE3E84;
extern int bE3E88;
extern char bE42B2;
extern int bE49A;
extern int tab4534[][5];
extern char tab3e8a[];
extern char *pE3C0A;
extern char *pE3C06;
extern char *pE3C0E;
extern int bE434;
extern int bE436;
extern int bE4A6;
extern int bE4AC;
extern int bEF312;
extern int bE4B4;
extern int bE478;
extern struct mlist tab3c22[];
extern struct symbol *tab44E8[];
extern int bE4CE;
extern char buf90b6[];
extern long bE47C;
extern long bE480;
extern int bE3E7C;
extern int bE3E7E;
extern int bE3E80;
extern int bE4A0;
extern struct symbol *bE492;
extern char **bE4A36;
extern int bE4A46;
extern int bE49C;
extern int bE498;
extern char **bE4A3C;
extern struct symbol *bE482;
extern struct symbol *bE48A;
extern struct symbol *bE48E;
extern long bE4C8;
extern char bE42B4[][5];
extern struct tab428a tab428a[];
extern int bF320;
extern int bF322;
extern int f4a34;
extern char *ptrs04ac[];
extern int bE0F8E;
extern int g047a;
extern int g0484;
extern char k047a[];
extern char g04ce[];
extern char g04d8[];

/* names that are part of a larger variable (same address in the original) */
extern char _ctype[];
#define tab42b4 ((char *)bE42B4)
#define tab1aad (_ctype + 1)
