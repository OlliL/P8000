#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
	int16_t s_code;
	int16_t s_data;
	int16_t s_bss;
	int16_t atr;
	char pad[4];
	int16_t segidx;
	struct p2seg *nxt;
	struct p2seg *prv;
	int16_t torig;
	int16_t dorig;
	int16_t borig;
};
/* segment table entry of an object file: the first 16 bytes of a p1seg */
struct fseg {
	char segno;
	char s_coff;
	char s_doff;
	char s_boff;
	int16_t s_code;
	int16_t s_data;
	int16_t s_bss;
	int16_t atr;
	char pad1[4];
};
struct bytes2 {
	char c0;
	char c1;
};
/* input buffering (Plexus ld: page, stream) */
struct page {
	int16_t nuser;
	int16_t bno;
	int16_t nibuf;
	uint8_t buff[512];	/* the words as they are in the file: big endian */
};
/* archive member header (Plexus ld: archdr; the times and the size are longs here) */
struct archdr {
	char aname[14];
	int32_t atime;
	char auid;
	char agid;
	int16_t amode;
	int32_t asize;
};
/* symbol table entry and the current symbol (Plexus ld: symbol, cursym; the layout is WEGA's: 16 bytes) */
struct symbol {
	int32_t svalue;
	char stype;
	char sxtra;
	int16_t sseg;
	char sname[8];
};
/* symbol of a nonsegmented object file (Plexus ld: symbol; 12 bytes) */
struct fsym {
	char fname[8];
	int16_t ftype;
	uint16_t fvalue;
};
/* symbol of a segmented object file (14 bytes) */
struct ssym {
	int32_t ssval;
	char sstyp;
	char ssseg;
	char ssnam[8];
};
/* entry of the ranlib table of contents of an archive (12 bytes) */
struct rlent {
	uint8_t roffb[4];	/* roff: a big endian long */
	char rname[8];
};
#define ROFF(p) ((int32_t)rd32((p)->roffb))
/* local symbol table of a file: index in the file -> symbol (Plexus ld: local; 6 bytes) */
struct local {
	int16_t locindex;
	struct symbol *locsymbol;
};
/* header of an object file as it is read (hx: not used here) and the header of the output (Plexus ld: filhdr; 24 bytes) */
struct objhdr {
	int16_t hmagic;
	int32_t hx;
	int32_t hbss;
	int16_t hsegsz;
	int16_t hsymsz;
	int32_t hentry;
	int16_t hrelflg;
	int16_t hy;
	int16_t hz;
};
struct filhdr {
	uint16_t fmagic;
	int32_t tsize;
	int32_t dsize;
	int32_t bsize;
	uint16_t ssize;
	int32_t entry;
	char fstamp[2];
	int16_t relflg;
};
/* output buffer (Plexus ld: buf) */
struct buf {
	int16_t fildes;
	int16_t nleft;		/* words left */
	uint8_t *xnext;
	uint8_t iobuf[512];	/* the words of the output: big endian */
};
struct pghdr {
	int16_t nuser;
	int16_t bno;
};
struct stream {
	int16_t sres;
	int16_t bno;
	int16_t nibuf;
	uint8_t *ptr;
	int16_t size;
	struct page *pno;
};
/* reservation list of the segments (dump_res: txtptr, datptr, bssptr, res); one row per segment number */
struct reslist { int16_t txtptr; int16_t datptr; int16_t bssptr; int16_t res; };
/* entry of the list of files of -M (mlist_add): the segments for t, d, b of a file name */
struct mlist { int16_t mtext; int16_t mdata; int16_t mbss; int16_t mnext; char *mname; };
/* the areas of a section while it is placed (place_sec): kind, start, size, end */
struct areas { char kind; uint16_t tstart; uint16_t tsize; uint16_t tend; };
/* pass one segment table entry (one per segment of an input file; dump strings: s_code, ctrel, ncoff, ncode, xtnext ...) */
struct p1seg {
	char segno;
	char s_coff;
	char s_doff;
	char s_boff;
	int16_t s_code;
	int16_t s_data;
	int16_t s_bss;
	int16_t atr;
	char pad1[4];
	int16_t ctrel;
	int16_t cdrel;
	int16_t cbrel;
	struct p2seg *ncode;
	struct p2seg *ndata;
	struct p2seg *nbss;
	int16_t ncoff;
	int16_t ndoff;
	int16_t nboff;
	int32_t tbase;
	int32_t dbase;
	int16_t xtnext;
	int16_t datnext;
	int16_t bssnext;
};

/* variables */
extern int16_t cmpmode;
extern struct p1seg **blktab;
extern int16_t delarg;
extern int16_t nundef;
extern int16_t arflag;
extern int16_t rflag;
extern char tmpname[];
extern char tfname[];
extern char *ofilename;
extern struct local local[];
extern int16_t ncopied;
extern int16_t sflag;
extern int16_t xflag;
extern struct buf soutb;
extern int16_t symindex;
extern struct buf toutb;
extern uint16_t nsym;
extern struct symbol *symtab;
extern struct p2seg p2tab[];
extern int16_t maxseg;
extern struct buf tmpoutb;
extern struct buf doutb;
extern struct buf troutb;
extern struct buf droutb;
extern struct reslist reslist[];
extern struct symbol *lastsym;
extern struct symbol **symhash[];
extern int16_t ctrel;
extern int16_t cdrel;
extern int16_t cbrel;
extern struct p2seg *p2head;
extern struct p2seg *p2tail;
extern int16_t wflag;
extern int16_t ofilfnd;
extern int16_t errlev;
extern struct buf routb;
extern char *aoutnam;
extern int16_t nwsym;
extern int32_t tsize;
extern int32_t dsize;
extern int32_t bsize;
extern int16_t tbflag;
extern int16_t zflag;
extern int32_t *libp;
extern int16_t swapflg;
extern struct page page[2];
extern struct page fpage;
extern struct stream text;
extern struct stream reloc;
extern struct archdr archdr;
extern struct fsym fsym;
/* cursym is followed by lnbuf in memory: the continuation entries of a long name are read behind it */
struct curbuf { struct symbol sym; char more[128]; };
extern struct curbuf curbuf;
#define cursym (curbuf.sym)
#define lnbuf (curbuf.more)
extern struct filhdr filhdr;
extern int16_t maxblk;
extern int16_t nblks;
extern int16_t unused3be;
extern int16_t nrflag;
extern int16_t nlflag;
extern int16_t pnflag;
extern uint16_t rlsize;
extern char *rlbuf;
extern struct symbol *p_edata;
extern int16_t infil;
extern int16_t np2seg;
extern struct p2seg *dsplseg;
extern struct p2seg *bsplseg;
extern int16_t unused15d8;
extern int16_t fsegflg;
extern int16_t segobj;
extern char cmdbuf[];
extern char libname[];
extern char *cmdargv[];
extern char **cmdav;
extern int16_t cmdac;
extern char *pname;
extern char *filname;
extern int16_t stksegn;
extern int16_t fblk;
extern int16_t nblk;
extern struct rlent *rltab;
extern int16_t rlcnt;
extern char secspec;
extern int16_t iflag;
extern uint16_t secorg[][5];
extern char liblist[];
extern struct p2seg *lastt;
extern struct p2seg *lastd;
extern struct p2seg *lastb;
extern int16_t segtsz;
extern int16_t hdrsz;
extern int16_t Sflag;
extern int16_t Xflag;
extern int16_t nsymf;
extern int16_t segmode;
extern int16_t mlhead;
extern struct mlist mlist[];
extern struct symbol *hshtab[];
extern int16_t symbytes;
extern char libpath[];
extern int32_t tcum;
extern int32_t dcum;
extern int16_t dseg;
extern int16_t tseg;
extern int16_t bseg;
extern int16_t Oflag;
extern struct symbol *entrypt;
extern char **curarg;
extern int16_t incmd;
extern int16_t mcnt;
extern int16_t dflag;
extern char **cmdfile;
extern struct symbol *p_etext;
extern struct symbol *p_end;
extern struct symbol *p_stkseg;
extern int32_t comsize;
extern char secflg[][5];
extern struct areas areas[];
extern int16_t dsplf;
extern int16_t bsplf;
extern int16_t placekd;
extern char *secnam[];
extern int16_t np2new;
extern int16_t mlcnt;
extern int16_t unused484;
extern char symdef[];
extern char premeof[];

/* names that are part of a larger variable (same address in the original) */
/* The memory image of the Z8000 is kept for the structures that are read from or written to a file or that hold a name
   across several entries: bytes, big endian, accessed by rd16/rd32/wr16/wr32. */
static inline uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static inline uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
static inline void wr16(uint8_t *p, uint16_t v) { p[0] = v >> 8; p[1] = v; }
static inline void wr32(uint8_t *p, uint32_t v) { wr16(p, v >> 16); wr16(p + 2, v); }

/* the libc functions of the original that work on the memory image */
static inline int16_t swap(int16_t x) { return (int16_t)(((uint16_t)x << 8 | (uint16_t)x >> 8) & 0xffff); }
static inline void swap_words(uint8_t *p, int n) { while (n-- > 0) { uint8_t t = p[0]; p[0] = p[1]; p[1] = t; p += 2; } }
/* swab: the bytes of every word of n bytes */
static inline void ld_swab(const void *src, void *dst, int n) { memmove(dst, src, n); swap_words((uint8_t *)dst, n / 2); }

/* the structures of the files as the Z8000 holds them: read from and written to a byte image */
static inline void rd_archdr(const uint8_t *r, struct archdr *a)
{
	memcpy(a->aname, r, 14);
	a->atime = (int32_t)rd32(r + 14);
	a->auid = r[18];
	a->agid = r[19];
	a->amode = (int16_t)rd16(r + 20);
	a->asize = (int32_t)rd32(r + 22);
}
static inline void rd_objhdr(const uint8_t *r, struct objhdr *h)
{
	h->hmagic = (int16_t)rd16(r);
	h->hx = (int32_t)rd32(r + 2);
	h->hbss = (int32_t)rd32(r + 6);
	h->hsegsz = (int16_t)rd16(r + 10);
	h->hsymsz = (int16_t)rd16(r + 12);
	h->hentry = (int32_t)rd32(r + 14);
	h->hrelflg = (int16_t)rd16(r + 18);
	h->hy = (int16_t)rd16(r + 20);
	h->hz = (int16_t)rd16(r + 22);
}
static inline void wr_objhdr(uint8_t *r, const struct objhdr *h)
{
	wr16(r, h->hmagic);
	wr32(r + 2, h->hx);
	wr32(r + 6, h->hbss);
	wr16(r + 10, h->hsegsz);
	wr16(r + 12, h->hsymsz);
	wr32(r + 14, h->hentry);
	wr16(r + 18, h->hrelflg);
	wr16(r + 20, h->hy);
	wr16(r + 22, h->hz);
}
static inline void rd_fseg(const uint8_t *r, struct fseg *g)
{
	g->segno = r[0];
	g->s_coff = r[1];
	g->s_doff = r[2];
	g->s_boff = r[3];
	g->s_code = (int16_t)rd16(r + 4);
	g->s_data = (int16_t)rd16(r + 6);
	g->s_bss = (int16_t)rd16(r + 8);
	g->atr = (int16_t)rd16(r + 10);
	memcpy(g->pad1, r + 12, 4);
}
static inline void wr_fseg(uint8_t *r, const struct fseg *g)
{
	r[0] = g->segno;
	r[1] = g->s_coff;
	r[2] = g->s_doff;
	r[3] = g->s_boff;
	wr16(r + 4, g->s_code);
	wr16(r + 6, g->s_data);
	wr16(r + 8, g->s_bss);
	wr16(r + 10, g->atr);
	memcpy(r + 12, g->pad1, 4);
}

#include "proto.h"
