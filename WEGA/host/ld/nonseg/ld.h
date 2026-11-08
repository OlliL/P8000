#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/*
 * ld.h - declarations shared by the C files of ld (structs, variables). The variables are defined in ld.c.
 * Names follow sld's ld.h where the use is the same; the others keep their address names (v<addr>) until
 * their use is known.
 */

/* header of a page of the input buffer (sld: page); nuser counts the streams that use it */
struct pghdr {
	int16_t nuser;
	int16_t bno;
};
/* input stream: the words are read from the page pno (ptr at 0: get and mget use it, layout proven by the code) */
struct stream {
	uint8_t *ptr;
	int16_t bno;
	int16_t nibuf;
	int16_t size;
	struct pghdr *pno;
};
/* input buffering (sld: page) */
struct page {
	struct pghdr hdr;	/* nuser and bno */
	int16_t nibuf;
	uint8_t buff[512];	/* the words as they are in the file: big endian */
};
/* vnodes entry: the state of the load before an object, saved by record and restored by -v (Plexus struct overlay) */
struct overlay {
	int16_t argsav;
	int16_t symsav;
	int32_t *libsav;
	char *vname;
	int16_t ctsav, cdsav, cbsav;
	int16_t offt, offd, offb, offs;
};
/* output or input buffer with the byte count left (sld: struct buf) */
struct buf {
	int16_t fildes;
	int16_t nleft;
	char *xnext;
	char iobuf[256];
};
/* archive member header (sld: archdr) */
struct archdr {
	char aname[14];
	int32_t atime;
	char auid;
	char agid;
	int16_t amode;
	int32_t asize;
};
/* local symbol table entry: index -> value, 4 bytes (sld: struct local with a symbol pointer, 6 bytes) */
struct local {
	int16_t locindex;
	struct symbol *locsymbol;
};

/* The memory image of the Z8000 is kept for the structures that are read from or written to a file or that hold a name
   across several entries: bytes, big endian, accessed by rd16/rd32/wr16/wr32. */
static inline uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static inline uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
static inline void wr16(uint8_t *p, uint16_t v) { p[0] = v >> 8; p[1] = v; }
static inline void wr32(uint8_t *p, uint32_t v) { wr16(p, v >> 16); wr16(p + 2, v); }

extern struct page page[2];
extern struct pghdr fpage;
extern int16_t infil;
extern int16_t vindex;
extern struct overlay vnodes[16];
extern int16_t symindex;
extern int32_t *libp;
extern int16_t tsize, dsize, bsize, ssize;
extern char *ofilename;
extern int16_t nundef;
extern char tfname[];
extern int16_t trace, arflag;
extern struct stream text;
extern struct buf toutb;
extern struct archdr archdr;
extern char *pname;
extern char *filname;
extern int16_t errlev;
extern struct local local[1000];
/* symbol table: functions that return a symbol pointer (sld: table16_entry, next_entry) */
/* current symbol (sld: cursym). The layout is the one of ld's code: svalue at 0x1076, stype at 4, sname at 6 (the
   address of cp8c's target), 14 bytes; sld's struct symbol has sseg and a 16 byte entry. */
struct symbol {
	uint8_t svb[4];		/* svalue: a big endian long */
	char stype;
	char sxtra;
	char sname[8];
};
#define SVALUE(p) ((int32_t)rd32((p)->svb))
#define SETSV(p, v) wr32((p)->svb, (uint32_t)(v))
/* cursym is followed by lnbuf in memory: the continuation entries of a long name are read behind it */
struct curbuf { struct symbol sym; char more[126]; };
extern struct curbuf curbuf;
#define cursym (curbuf.sym)
#define lnbuf (curbuf.more)
extern struct symbol sym16[600 + 10];
/* hash table of the symbols (sld: hshtab, indexed from 2; ld hashes with 0x0923), the name buffer of the
   current symbol (sld: lnbuf) and the flag of the -N option (sld: nlflag) */
extern struct symbol *hshtab[];
extern int16_t nlflag;
extern struct symbol *symtab;
/* flag of -w (sld: wflag); put_name writes a symbol name and returns its length (copyname.c / printname.c) */
extern int16_t wflag;
extern int16_t swapflg;
extern int16_t sflag, xflag;
extern struct buf soutb;
extern int16_t torigin, nlocsym;
extern int16_t ctrel, cdrel, cbrel;
extern struct symbol **symhash[];
extern struct symbol *lastsym;
extern struct symbol *nsym;
extern struct stream reloc;
extern char libname[];
extern char libpath[];
extern int16_t noswap;
struct rlent {
	uint8_t roffb[4];	/* roff: a big endian long */
	char rname[8];
};
#define ROFF(p) ((int32_t)rd32((p)->roffb))
extern struct rlent *rltab;
extern int16_t rlcnt;
/* output header of ld (sld: filhdr), 16 bit fields at bss 0x1062: the offsets are those of the code (readhdr, init_entry);
   ftsize and fdsize are the sizes of text and data in the file (8-bit objects: from hy and hx, else tsize and dsize) */
struct filhdr {
	uint16_t fmagic;
	uint16_t tsize;
	uint16_t ftsize;
	uint16_t dsize;
	uint16_t fdsize;
	uint16_t bsize;
	uint16_t ssize;
	uint16_t entry;
	char fstamp[2];
	uint16_t relflg;
};
extern struct filhdr filhdr;
/* header of an object or of the output (sld: objhdr) and a segment record of the output (sld: fseg) */
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
extern int16_t hdrsz, segtsz;
extern int16_t idsep;
extern int16_t oflag, iflag, fttotal, fdtotal;
extern struct symbol *entrypt;
struct areas { char kind; char pad; int16_t tstart; int16_t tsize; int16_t tend; };
extern struct areas areas[5];
extern char secflg[5];
extern char *secnam[4];
extern int16_t secorg[4];

/* the libc functions of the original that work on the memory image */
static inline int16_t swap(int16_t x) { return (int16_t)(((uint16_t)x << 8 | (uint16_t)x >> 8) & 0xffff); }
static inline void swap_words(uint8_t *p, int n) { while (n-- > 0) { uint8_t t = p[0]; p[0] = p[1]; p[1] = t; p += 2; } }
/* swab: the bytes of every word of n bytes */
static inline void ld_swab(const void *src, void *dst, int n) { memmove(dst, src, n); swap_words((uint8_t *)dst, n / 2); }
static inline void longswap(void *l) { swap_words((uint8_t *)l, 2); }
static inline void swap_quad(void *l) { swap_words((uint8_t *)l + 4, 4); }

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
