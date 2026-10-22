/*
 * sld - the WEGA segmented loader (U8001, sld) built for Linux (a cross tool).
 *
 * Written from the WEGA binary /bin/sld, its manual page and measurements (compared with the
 * original under z8k-emu by check.py); the structure follows host/ld/ld.c (see there and
 * KNOWLEDGE.md, "sld on the host").
 *
 * Search path of -l: $WEGA_LIBPATH (colon separated), default /lib:/usr/lib:/z/bin (below
 * $WEGA_ROOT when that is set); the library of -lx is slibx.a.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>

#define N_UNDF	0x00
#define N_ABS	0x01
#define N_CODE	0x02
#define N_DATA	0x03
#define N_BSS	0x04
#define N_SN	0x1E
#define N_FN	0x1F
#define N_EXT	0x20

#define MAGIC_N1 0xE707
#define MAGIC_N3 0xE711
#define MAGIC_N4 0xE705
#define MAGIC_X1 0xE507
#define MAGIC_X3 0xE511
#define MAGIC_X4 0xE505
#define ARMAG	0xFF65

/* ---- helpers ------------------------------------------------------------ */

static unsigned be16(const unsigned char *p) { return p[0] << 8 | p[1]; }
static uint32_t be32(const unsigned char *p) { return (uint32_t)p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]; }
static void put16(unsigned char *p, unsigned v) { p[0] = v >> 8; p[1] = v; }
static void put32(unsigned char *p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v; }

static int status;			/* exit status (bit 0: an error was reported) */

static void
fatal(const char *fmt, ...)
{
	__builtin_va_list ap;

	fprintf(stderr, "ld: ");
	__builtin_va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	__builtin_va_end(ap);
	fprintf(stderr, "\n");
	exit(4);
}

static const char *curfile = "";		/* the file that is being read, for messages */

static void
report(int level, const char *msg, const char *name)
{
	fprintf(stderr, "ld: %s: %s%s%s\n", curfile, name ? name : "", name ? " " : "", msg);
	status = level;			/* the original keeps the level of the last message */
	if (level == 2)
		exit(4);
}

static void *
xmalloc(size_t n)
{
	void *p = malloc(n ? n : 1);

	if (p == NULL)
		fatal("out of memory");
	return p;
}

static char *
xstrdup(const char *s)
{
	char *p = xmalloc(strlen(s) + 1);

	return strcpy(p, s);
}

/* ---- symbols ------------------------------------------------------------ */

/*
 * A symbol table entry is 14 bytes (s_nlist); a name of more than 8 characters continues in
 * the following entries. The index of a symbol in a relocation is its entry number, so a
 * symbol with a long name uses up several numbers (slots).
 */
struct sym {
	int fidx;			/* global: the file that defines it (-1 none) */
	int rawsegt;			/* segt of the object that made the entry (no entry base) */
	char *name;
	uint32_t val;
	int type;			/* the type byte of the object (N_EXT or'ed in) */
	int segt;
	int slot;			/* entry number in the table of its owner */
	int nslots;
};

static int
nslots_of(const char *name)
{
	int n = strlen(name);

	if (n <= 8)
		return 1;
	return (6 + n + 1 + 13) / 14;
}

#define NHASH	4093

struct gtab {
	struct sym *s;
	int n, max;
	int nslot;			/* slots used so far */
	int *hash[1];
};

static struct sym *gs;
static int ngs, maxgs, gslots;
/*
 * The symbol table of the original is a plain array of 14 byte entries: the type byte and the value
 * of a new entry are stored, the segt byte (offset 5) is not. After an archive member was dropped its
 * entries stay in the array, so a later entry in the same place carries the old byte. stale5[] is that
 * byte for every entry number.
 */
static unsigned char *stale5;
static int maxslots;
static int hashtab[NHASH];		/* index+1 into gs, chained by gnext */
static int *gnext;

static int fNshort;			/* -Ns: only 8 characters of a name are significant */

/* the names are stored whole, but with -Ns only the first 8 characters are compared */
static unsigned
hashname(const char *s)
{
	unsigned h = 0;
	int n = 0;

	while (*s && (!fNshort || n++ < 8))
		h = h * 31 + (unsigned char)*s++;
	return h % NHASH;
}

static int
samename(const char *a, const char *b)
{
	return fNshort ? strncmp(a, b, 8) == 0 : strcmp(a, b) == 0;
}

static int
glookup(const char *name)
{
	int i;

	for (i = hashtab[hashname(name)]; i; i = gnext[i - 1])
		if (samename(gs[i - 1].name, name))
			return i - 1;
	return -1;
}

static int
gnew(const char *name, int type, uint32_t val)
{
	unsigned h = hashname(name);

	if (ngs == maxgs) {
		maxgs = maxgs ? maxgs * 2 : 512;
		gs = realloc(gs, maxgs * sizeof gs[0]);
		gnext = realloc(gnext, maxgs * sizeof gnext[0]);
		if (gs == NULL || gnext == NULL)
			fatal("out of memory");
	}
	gs[ngs].fidx = -1;
	gs[ngs].name = xstrdup(name);
	gs[ngs].val = val;
	gs[ngs].segt = 0;
	gs[ngs].type = type;
	gs[ngs].slot = gslots;
	gs[ngs].nslots = nslots_of(name);
	if (gslots + gs[ngs].nslots > maxslots) {
		int old = maxslots;

		maxslots = (gslots + gs[ngs].nslots) * 2 + 64;
		stale5 = realloc(stale5, maxslots);
		if (stale5 == NULL)
			fatal("out of memory");
		memset(stale5 + old, 0, maxslots - old);
	}
	if (gs[ngs].nslots > 1) {
		int k, len = strlen(name);
		unsigned char *b = xmalloc(gs[ngs].nslots * 14);

		memset(b, 0, gs[ngs].nslots * 14);
		b[0] = 0x80 | (len + 1);
		memcpy(b + 1, name, len);
		for (k = 1; k < gs[ngs].nslots; k++)
			stale5[gslots + k] = b[8 + 14 * (k - 1) + 5];
		free(b);
	}
	gs[ngs].segt = stale5[gslots];
	gslots += gs[ngs].nslots;
	gnext[ngs] = hashtab[h];
	hashtab[h] = ++ngs;
	return ngs - 1;
}

/* undo the entries added after n (an archive member that was not needed) */
static void
gtruncate(int n)
{
	while (ngs > n) {
		unsigned h = hashname(gs[ngs - 1].name);

		hashtab[h] = gnext[ngs - 1];
		gslots -= gs[ngs - 1].nslots;
		free(gs[ngs - 1].name);
		ngs--;
	}
}
static unsigned char *
readfile(const char *path, size_t *len)
{
	FILE *f = fopen(path, "rb");
	unsigned char *p;
	long n;

	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	n = ftell(f);
	fseek(f, 0, SEEK_SET);
	p = xmalloc(n + 1);
	if (fread(p, 1, n, f) != (size_t)n)
		fatal("%s: read error", path);
	fclose(f);
	*len = n;
	return p;
}

/* parse the symbol table of an object; returns the number of symbols */
static int
parsesyms(const unsigned char *st, unsigned size, struct sym **out, int *nslot)
{
	struct sym *v = NULL;
	int n = 0, cap = 0;
	unsigned o = 0;

	while (o + 14 <= size) {
		const unsigned char *e = st + o;
		char buf[256];
		int len, ns;

		if (e[6] & 0x80) {
			int ln = e[6] & 0x7F;		/* length including the length byte */

			len = ln - 1;
			if (len < 0 || o + 7 + len > size)
				fatal("%s: Bad format", curfile);
			memcpy(buf, e + 7, len);
			ns = (6 + ln + 13) / 14;
		} else {
			len = 0;
			while (len < 8 && e[6 + len])
				len++;
			memcpy(buf, e + 6, len);
			ns = 1;
		}
		buf[len] = 0;
		if (n == cap) {
			cap = cap ? cap * 2 : 64;
			v = realloc(v, cap * sizeof v[0]);
			if (v == NULL)
				fatal("out of memory");
		}
		v[n].name = xstrdup(buf);
		v[n].val = be32(e);
		v[n].type = e[4];
		v[n].segt = e[5];
		v[n].slot = o / 14;
		v[n].nslots = ns;
		n++;
		o += ns * 14;
	}
	*out = v;
	*nslot = o / 14;
	return n;
}

/* ---- options ------------------------------------------------------------ */

static int fS, fX, fx, fs, fr, fr_explicit, fd, fi, fO, fw, fT, fz;
static const char *outname = "a.out";
static const char *entryname;
static int stackseg = -1;		/* -Ms */
static int reserved[128], nreserved;	/* -R */

#define MAXSEG	128
#define SEGMAX	0xFFFE			/* the highest used offset of a segment */

/* -M events in the order of the arguments; the state is replayed for every file */
struct mevent {
	char kind;			/* 0 all, 't', 'd', 'b' */
	int seg;
};
static struct mevent mev[256];
static int nmev;

/* ---- input files -------------------------------------------------------- */

#define MAXENT	64

struct file {			/* one object, or one loaded member of an archive */
	char *name;		/* for the FN symbol: the base name, or the member name */
	unsigned char *img;	/* text of all entries, then data of all entries */
	unsigned char *rel;
	struct sym *syms;	/* symbols of the file (names owned) */
	int nsyms, nslot;
	unsigned magic;
	int nent;
	unsigned code[MAXENT], data[MAXENT], bss[MAXENT];	/* sizes (even) */
	unsigned codeoff[MAXENT], dataoff[MAXENT];	/* offsets in img */
	unsigned imsize;
	int ts, ds, bs;		/* requested start segments (-M) */
	int entbase;		/* number of segment table entries of the files before */
	int tseg[MAXENT], dseg[MAXENT], bseg[MAXENT];	/* placement */
	unsigned toff[MAXENT], doff[MAXENT], boff[MAXENT];
	unsigned mc[MAXENT], md[MAXENT], mb[MAXENT];	/* offsets in the merged entry of -r */
	int *map;
	int nmap;
};

static struct file *files;
static int nfiles, maxfiles;
static int nentries;				/* segment table entries of the files so far (pass 1) */
static int nmevstate;				/* number of events already seen by loadobject */

static void
mstate(int upto, int *t, int *d, int *b)
{
	int k;

	*t = 0;
	*d = *b = fi ? 1 : 0;
	for (k = 0; k < upto; k++) {
		switch (mev[k].kind) {
		case 0:
			*t = mev[k].seg;
			*d = *b = fi ? mev[k].seg + 1 : mev[k].seg;
			break;
		case 't': *t = mev[k].seg; break;
		case 'd': *d = mev[k].seg; break;
		case 'b': *b = mev[k].seg; break;
		}
	}
}

static int
isobject(const unsigned char *p, size_t len)
{
	unsigned m;

	if (len < 24)
		return 0;
	m = be16(p);
	return m == 0xE607 || m == 0xE611 || m == 0xE605;
}

#define ARBASE(g) (g)

static void
addfile(struct file *f)
{
	if (nfiles == maxfiles) {
		maxfiles = maxfiles ? maxfiles * 2 : 64;
		files = realloc(files, maxfiles * sizeof files[0]);
		if (files == NULL)
			fatal("out of memory");
	}
	files[nfiles++] = *f;
}

static uint32_t ccommon;		/* common area (pass 1 only counts the symbols) */

/* the area of a symbol type: 2 text, 3 data, 4 bss, else 0 */
static int
sarea(int type)
{
	int t = type & 0x1F;

	return t >= 2 && t <= 4 ? t : 0;
}

/*
 * Pass 1 for one object (an object file, or a member of an archive when member is set); see ld.c.
 * The symbol values are kept as they are in the object (entry number in bits 24-30 and offset).
 */
static int
loadobject(const char *name, const unsigned char *data, size_t len, int member)
{
	static struct file f;
	unsigned segt, syms, flag, imsize, magic = be16(data);
	const unsigned char *sg, *st;
	int i, count = 0, saved = ngs, newl = 0;
	unsigned o;

	memset(&f, 0, sizeof f);
	if (magic != 0xE607 && magic != 0xE611 && magic != 0xE605)
		fatal("%s: Bad format", name);
	imsize = be32(data + 2);
	segt = be16(data + 10);
	syms = be16(data + 12);
	flag = be16(data + 18);
	if (flag & 1)
		report(1, "No relocation bits", NULL);
	if (segt < 16 || segt / 16 > MAXENT)
		fatal("%s: Invalid segment attributes", name);
	f.nent = segt / 16;
	sg = data + 24;
	o = 0;
	for (i = 0; i < f.nent; i++) {		/* the image holds code and data entry by entry */
		f.code[i] = be16(sg + 16 * i + 4);
		f.data[i] = be16(sg + 16 * i + 6);
		f.bss[i] = be16(sg + 16 * i + 8);
		f.codeoff[i] = o;
		o += f.code[i];
		f.dataoff[i] = o;
		o += f.data[i];
	}
	for (i = 0; i < f.nent; i++) {
		f.code[i] = (f.code[i] + 1) & ~1u;
		f.data[i] = (f.data[i] + 1) & ~1u;
		f.bss[i] = (f.bss[i] + 1) & ~1u;
	}
	f.magic = magic;
	f.imsize = imsize;
	f.img = (unsigned char *)data + 24 + segt;
	f.rel = f.img + imsize;
	st = f.rel + imsize;
	if (flag & 1)
		st = f.img + imsize;
	f.nsyms = parsesyms(st, syms, &f.syms, &f.nslot);
	f.name = xstrdup(name);
	mstate(nmevstate, &f.ts, &f.ds, &f.bs);
	f.entbase = nentries;
	(void)len;
	for (i = 0; i < f.nsyms; i++) {
		struct sym *s = &f.syms[i];
		int t = s->type & 0x1F, g, ty;
		uint32_t v;

		if (fS && (t == 1 || t > 4))
			continue;
		if (!(s->type & N_EXT))
			continue;
		ty = s->type & ~0x40;
		v = s->val;
		g = glookup(s->name);
		if (g < 0) {
			g = gnew(s->name, ty, v);
			gs[g].segt = (s->segt + f.entbase) & 0xFF;
			gs[g].rawsegt = s->segt;
			newl++;
			if (ty != (N_EXT | N_UNDF))
				gs[g].fidx = nfiles;
			continue;
		}
		if (gs[g].type == (N_EXT | N_UNDF)) {
			if (ty == (N_EXT | N_UNDF)) {
				if (v > gs[g].val)
					gs[g].val = v;
			} else {
				if (gs[g].val != 0 && ty == (N_EXT | N_CODE)) {
					if (member)
						continue;
					report(0, "warning: external data item refers to text symbol", s->name);
				}
				count++;
				gs[g].type = ty;
				gs[g].val = v;
				gs[g].segt = (s->segt + f.entbase) & 0xFF;
				gs[g].rawsegt = s->segt;
				gs[g].fidx = nfiles;
			}
		} else if (member && !fw) {
			const char *p = s->name;

			if (ty != (N_EXT | N_UNDF) && !(p[0] == '_' && p[1] == '_'))
				report(0, "- warning: symbol redefined in archive", s->name);
		}
	}
	if (member && count == 0) {
		gtruncate(saved);
		free(f.name);
		return 0;
	}
	nentries += f.nent;
	addfile(&f);
	return 1;
}

struct member {
	char name[15];
	uint32_t date;
	uint32_t size;
	size_t off;			/* offset of the header */
};

static int
armember(const unsigned char *ar, size_t len, size_t off, struct member *m)
{
	if (off + 26 > len)
		return 0;
	memcpy(m->name, ar + off, 14);
	m->name[14] = 0;
	m->date = be32(ar + off + 14);
	m->size = be32(ar + off + 22);
	m->off = off;
	if (off + 26 + m->size > len)
		fatal("%s: Bad format", curfile);
	return 1;
}

static int
loadmember(const char *aname, const unsigned char *ar, size_t len, size_t off)
{
	struct member m;
	const unsigned char *body;

	(void)aname;
	if (!armember(ar, len, off, &m))
		return -1;
	body = ar + off + 26;
	if (!isobject(body, m.size))
		return 0;
	return loadobject(m.name, body, m.size, 1);
}

static void
loadarchive(const char *path, const char *realpath, const unsigned char *ar, size_t len)
{
	struct member m;
	size_t off = 2;
	int symdef = 0;

	if (armember(ar, len, 2, &m) && strcmp(m.name, "__.SYMDEF") == 0) {
		struct stat sb;

		symdef = 1;
		if (stat(realpath, &sb) == 0 && (uint32_t)sb.st_mtime > m.date) {
			report(0, "out of date (warning)", NULL);
			symdef = 0;
		}
	}
	if (!symdef) {
		while (armember(ar, len, off, &m)) {
			if (strcmp(m.name, "__.SYMDEF") != 0)
				loadmember(path, ar, len, off);
			off += 26 + m.size + (m.size & 1);
		}
		return;
	}
	{
		const unsigned char *tab = ar + 2 + 26;
		unsigned n = m.size / 12, changed;
		unsigned i;

		do {
			changed = 0;
			for (i = 0; i < n; i++) {
				const unsigned char *e = tab + 12 * i;
				char buf[256];
				int g;

				if (e[4] & 0x80) {
					int ln = e[4] & 0x7F;		/* length including the length byte */

					if (i * 12 + 5 + ln - 1 > m.size)
						break;
					memcpy(buf, e + 5, ln - 1);
					buf[ln - 1] = 0;
					i += (4 + ln + 11) / 12 - 1;	/* the entries that hold the rest of the name */
				} else {
					int k = 0;

					while (k < 8 && e[4 + k])
						k++;
					memcpy(buf, e + 4, k);
					buf[k] = 0;
				}
				g = glookup(buf);
				if (g < 0 || gs[g].type != (N_EXT | N_UNDF))
					continue;
				if (loadmember(path, ar, len, be32(e)) > 0)
					changed = 1;
			}
		} while (changed);
	}
}

static const char *libdirs[8];
static int nlibdirs;

static const char *
rootpath(const char *path)
{
	static char buf[1024];
	char *root = getenv("WEGA_ROOT");

	if (root && path[0] == '/' && access(path, R_OK) != 0) {
		snprintf(buf, sizeof buf, "%s%s", root, path);
		return buf;
	}
	return path;
}

static char *
findlib(const char *l)
{
	static char path[1024];
	int i;

	for (i = 0; i < nlibdirs; i++) {
		snprintf(path, sizeof path, "%s/slib%s.a", libdirs[i], l);
		if (access(path, R_OK) == 0)
			return path;
	}
	return NULL;
}

static void
processfile(const char *path)
{
	size_t len;
	unsigned char *data = readfile(rootpath(path), &len);
	const char *base;

	if (data == NULL) {
		fprintf(stderr, "ld: %s: cannot open\n", path);
		exit(4);
	}
	curfile = path;
	if (len >= 2 && be16(data) == ARMAG) {
		loadarchive(path, rootpath(path), data, len);
		return;
	}
	base = strrchr(path, '/');
	base = base ? base + 1 : path;
	if (!isobject(data, len))
		fatal("%s: Bad format", path);
	loadobject(base, data, len, 0);
}

/* ---- pass 2 ------------------------------------------------------------- */


/* ---- layout ------------------------------------------------------------- */

struct oseg {
	int stack;			/* the stack segment (the segment may hold more) */
	int used;			/* is the segment in use */
	unsigned code, data, bss;	/* sizes (offsets are code, then data, then bss) */
	unsigned char *img;		/* code image */
};
static struct oseg oseg[MAXSEG];
static int isstart[MAXSEG];
static int fresh[MAXSEG];
static int nopad[MAXSEG];		/* a segment that was made for the bss block is not padded */		/* a start segment of a chain */

struct chain {
	int segs[MAXSEG];
	int n;
};
static struct chain chains[MAXSEG];	/* by start segment */
static int lastseg_t = -1, lastseg_d = -1, lastseg_b = -1;
static unsigned lastend_t, lastend_d, lastend_b;

static int
freeseg(void)
{
	int k;

	for (k = 0; k < MAXSEG; k++)
		if (!oseg[k].used && !isstart[k])
			return k;
	fatal("Segment table overflow");
	return -1;
}

/* the size of a segment so far: code + data + bss */
static unsigned
segsize(int s)
{
	return oseg[s].code + oseg[s].data + oseg[s].bss;
}

/*
 * Place a section of a stream (the chain of segments that begins at start): text and data by
 * first fit over the segments of the chain, bss and commons by next fit from the last segment.
 * A section that does not fit anywhere gets the lowest segment number that is not in use and
 * not the start of a chain. kind: 0 text, 1 data, 2 bss.
 */
static int
place(int start, int kind, unsigned size, unsigned *off)
{
	struct chain *c = &chains[start];
	int k, s = -1;

	if (size > SEGMAX)
		fatal("Segment overflow");
	if (c->n == 0) {
		c->segs[c->n++] = start;
		oseg[start].used = 1;
	}
	if (kind == 2) {
		if (segsize(c->segs[c->n - 1]) + size <= SEGMAX)
			s = c->segs[c->n - 1];
	} else {
		int k0 = 0;

		if (kind == 1)			/* data follows the last segment that holds text */
			for (k = 0; k < c->n; k++)
				if (oseg[c->segs[k]].code)
					k0 = k;
		for (k = k0; k < c->n; k++)
			if (segsize(c->segs[k]) + size <= SEGMAX) {
				s = c->segs[k];
				break;
			}
	}
	if (s < 0) {
		s = freeseg();
		oseg[s].used = 1;
		c->segs[c->n++] = s;
	}
	if (kind == 0) {
		*off = oseg[s].code;
		oseg[s].code += size;
	} else if (kind == 1) {
		*off = oseg[s].code + oseg[s].data;
		oseg[s].data += size;
	} else {
		*off = oseg[s].code + oseg[s].data + oseg[s].bss;
		oseg[s].bss += size;
	}
	return s;
}

/* value of a symbol of the object f (entry, area, offset in the object) */
static uint32_t
symval(const struct file *f, const struct sym *s)
{
	int e = (s->val >> 24) & 0x7F, a = sarea(s->type);
	uint32_t off = s->val & 0xFFFFFF;

	if (e >= f->nent)
		e = 0;
	switch (a) {
	case 2: return (uint32_t)f->tseg[e] << 24 | (f->toff[e] + off);
	case 3: return (uint32_t)f->dseg[e] << 24 | (f->doff[e] + off - f->code[e]);
	case 4: return (uint32_t)f->bseg[e] << 24 | (f->boff[e] + off - f->code[e] - f->data[e]);
	}
	return s->val;
}

static unsigned char *obuf;		/* output symbol table */
static size_t osize, ocap;

static void
putsym(uint32_t val, int type, int segt, const char *name)
{
	int len = strlen(name), ns = nslots_of(name);
	size_t need = (size_t)ns * 14;

	if (osize + need > ocap) {
		ocap = (osize + need) * 2;
		obuf = realloc(obuf, ocap);
		if (obuf == NULL)
			fatal("out of memory");
	}
	memset(obuf + osize, 0, need);
	put32(obuf + osize, val);
	obuf[osize + 4] = type;
	obuf[osize + 5] = segt;
	if (len <= 8)
		memcpy(obuf + osize + 6, name, len);
	else {
		obuf[osize + 6] = 0x80 | (len + 1);
		memcpy(obuf + osize + 7, name, len);
	}
	osize += need;
}

static void
putfn(const char *name, uint32_t val)
{
	char b[9];

	strncpy(b, name, 8);
	b[8] = 0;
	putsym(val, N_FN, (val >> 24) & 0x7F, b);
}

/* an object symbol entry copied from the object */
static void
putlocal(const struct sym *s, uint32_t val, int segt)
{
	putsym(val, s->type, segt, s->name);
}

/*
 * Relocatable output (-r, or undefined symbols): one segment table entry for everything, the code of all
 * files followed by the data, the relocation words kept and renumbered.
 */
int
main(int argc, char **argv)
{
	int i, k;
	char *lp = getenv("WEGA_LIBPATH");
	char *p;
	char **inputs;
	int *kind;			/* 0 file, 1 -l, 2 -u/-e symbol */
	int ni = 0;

	if (lp == NULL) {
		static char dflt[1024];
		char *root = getenv("WEGA_ROOT");

		if (root)
			snprintf(dflt, sizeof dflt, "%s/lib:%s/usr/lib:%s/z/bin", root, root, root);
		else
			strcpy(dflt, "/lib:/usr/lib:/z/bin");
		lp = dflt;
	}
	lp = xstrdup(lp);
	for (p = strtok(lp, ":"); p && nlibdirs < 8; p = strtok(NULL, ":"))
		libdirs[nlibdirs++] = p;
	if (argc == 1)
		exit(4);
	inputs = xmalloc(argc * sizeof *inputs);
	kind = xmalloc(argc * sizeof *kind);
	/* -i is needed before the -M events are replayed: look for it first */
	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-' && strchr(argv[i], 'i') && argv[i][1] != 'M' && argv[i][1] != 'o' && argv[i][1] != 'e' &&
		    argv[i][1] != 'u' && argv[i][1] != 'l')
			fi = 1;
	for (i = 1; i < argc; i++) {
		char *a = argv[i];

		if (a[0] != '-' || a[1] == 0) {
			inputs[ni] = a;
			kind[ni++] = 0;
			continue;
		}
		if (a[1] == 'M') {
			char *q = a + 2;
			char c = 0;
			int seg;

			if (*q == 't' || *q == 'd' || *q == 'b' || *q == 's')
				c = *q++;
			if (*q < '0' || *q > '9')
				fatal("-M%s: bad segment", a + 2);
			seg = strtol(q, &q, 0);
			if (*q)
				fatal("-M%s: not supported", a + 2);
			if (seg < 0 || seg > 127)
				fatal("-M: segment number out of range");
			if (c == 's')
				stackseg = seg;
			else {
				mev[nmev].kind = c;
				mev[nmev++].seg = seg;
				/* the position matters: the files that follow see it */
				inputs[ni] = NULL;
				kind[ni++] = 3;
			}
			continue;
		}
		if (a[1] == 'A') {
			if (a[2] == 0)
				i++;
			continue;
		}
		if (a[1] == 'R') {
			char *q = a[2] ? a + 2 : (i + 1 < argc ? argv[++i] : NULL);

			if (q == NULL)
				fatal("-R: arg missing");
			reserved[nreserved++] = atoi(q);
			continue;
		}
		for (p = a + 1; *p; p++) {
			switch (*p) {
			case 'S': fS++; break;
			case 'X': fX++; break;
			case 'x': fx++; break;
			case 's': fs++; fx++; break;
			case 'r': fr++; fr_explicit++; break;
			case 'd': fd++; break;
			case 'i': fi++; break;
			case 'O': fO++; break;
			case 'w': fw++; break;
			case 'T': fT++; break;
			case 'z': fz++; break;
			case 'N':
				if (p[1] == 's')
					fNshort = 1;
				else if (p[1] == 'l')
					fNshort = 0;
				else
					fatal("-N%c: bad argument (l or s)", p[1] ? p[1] : ' ');
				p++;
				break;
			case 'o':
				if (++i >= argc)
					fatal("Bad output file");
				outname = argv[i];
				break;
			case 'e':
			case 'u':
				if (++i >= argc)
					fatal("Bad 'use' or 'entry'");
				inputs[ni] = argv[i];
				kind[ni++] = 2;
				if (*p == 'e')
					entryname = argv[i];
				break;
			case 'l':
				inputs[ni] = p + 1;
				kind[ni++] = 1;
				p += strlen(p) - 1;
				break;
			default:
				fatal("-%c: bad flag", *p);
			}
		}
	}

	/* pass 1: in the order of the arguments */
	for (i = 0; i < ni; i++) {
		if (kind[i] == 3) {
			nmevstate++;
		} else if (kind[i] == 2) {
			if (glookup(inputs[i]) < 0)
				gnew(inputs[i], N_EXT | N_UNDF, 0);
		} else if (kind[i] == 1) {
			char *path = findlib(inputs[i]);

			if (path == NULL)
				fatal("-l%s: cannot open", inputs[i]);
			processfile(path);
		} else
			processfile(inputs[i]);
	}

	/* undefined symbols */
	{
		static const char *rs[4] = { "_etext", "_edata", "_end", "_stkseg" };
		int any = 0, warned8 = 0, special;

		for (i = 0; i < ngs; i++) {
			if (gs[i].type != (N_EXT | N_UNDF) || gs[i].val != 0)
				continue;
			special = 0;
			for (k = 0; k < 4; k++)
				if (strcmp(gs[i].name, rs[k]) == 0)
					special = 1;
			if (special || fr_explicit)
				continue;
			if (!any)
				fprintf(stderr, "Undefined:\n");
			any = 1;
			status |= 1;
			if (fNshort && strlen(gs[i].name) > 8 && !warned8) {
				warned8 = 1;
				report(0, "Warning - only 8 characters of name are significant", NULL);
			}
			fprintf(stderr, "%.*s\n", fNshort ? 8 : 256, gs[i].name);
		}
		if (any && !fr_explicit)
			fr = 1;
	}

	/* segments: the start segments are reserved */
	for (i = 0; i < nreserved; i++)
		isstart[reserved[i]] = 1;
	if (stackseg >= 0)
		isstart[stackseg] = 1;
	{
		int t, d, b;

		mstate(nmev, &t, &d, &b);
		isstart[b] = 1;
	}
	for (i = 0; i < nfiles; i++) {
		isstart[files[i].ts] = 1;
		isstart[files[i].ds] = 1;
		isstart[files[i].bs] = 1;
	}
	/* text */
	for (i = 0; i < nfiles; i++) {
		struct file *f = &files[i];

		for (k = 0; k < f->nent; k++) {
			f->tseg[k] = f->ts;
			f->toff[k] = 0;
			if (f->code[k])
				f->tseg[k] = place(f->ts, 0, f->code[k], &f->toff[k]);
		}
	}
	/* data */
	for (i = 0; i < nfiles; i++) {
		struct file *f = &files[i];

		for (k = 0; k < f->nent; k++) {
			f->dseg[k] = f->ds;
			f->doff[k] = 0;
			if (f->data[k])
				f->dseg[k] = place(f->ds, 1, f->data[k], &f->doff[k]);
		}
	}
	/*
	 * bss of the files, then the commons: if all of them fit into the last segment of the chain
	 * they go there, else into a segment of their own, and only if they are more than a segment
	 * they are distributed one after the other (next fit) from the last segment on.
	 */
	{
		int t, d, b, blocked;
		unsigned total = 0, off;
		struct chain *c;

		mstate(nmev, &t, &d, &b);
		for (i = 0; i < nfiles; i++)
			for (k = 0; k < files[i].nent; k++)
				total += files[i].bss[k];
		for (i = 0; i < ngs; i++)
			if (gs[i].type == (N_EXT | N_UNDF) && gs[i].val != 0 && (!fr || fd))
				total += (gs[i].val + 1) & ~1u;
		c = &chains[b];
		blocked = 0;
		if (total) {
			if (c->n == 0) {
				c->segs[c->n++] = b;
				oseg[b].used = 1;
			}
			if (segsize(c->segs[c->n - 1]) + total > SEGMAX && total <= SEGMAX) {
				int s, mx = -1;

				for (s = 0; s < MAXSEG; s++)
					if (oseg[s].used && s > mx)
						mx = s;
				s = mx + 1;
				while (s < MAXSEG && (oseg[s].used || isstart[s]))
					s++;
				if (s >= MAXSEG)
					s = freeseg();

				oseg[s].used = 1;
				fresh[s] = 1;
				nopad[s] = 1;
				c->segs[c->n++] = s;
			}
			blocked = 1;
		}
		(void)blocked;
		for (i = 0; i < nfiles; i++) {
			struct file *f = &files[i];

			for (k = 0; k < f->nent; k++) {
				f->bseg[k] = f->bs;
				f->boff[k] = 0;
				if (f->bss[k])
					f->bseg[k] = place(b, 2, f->bss[k], &f->boff[k]);
			}
		}
		for (i = 0; i < ngs; i++) {
			if (gs[i].type == (N_EXT | N_UNDF) && gs[i].val != 0 && (!fr || fd)) {
				unsigned sz = (gs[i].val + 1) & ~1u;
				int s = place(b, 2, sz, &off);

				gs[i].val = (uint32_t)s << 24 | off;
				gs[i].type = N_EXT | N_BSS;
				gs[i].fidx = -2;
			}
		}
	}
	/* a segment is padded to a multiple of 256 (the pad is added to its last area) */
	for (i = 0; i < MAXSEG; i++) {
		unsigned tot = segsize(i), r = (tot | 0xff) + 1, pad;

		if (tot == 0 || nopad[i] || fr)
			continue;
		if (r > SEGMAX)
			r = SEGMAX;
		pad = r - tot;
		if (oseg[i].bss) oseg[i].bss += pad;
		else if (oseg[i].data) oseg[i].data += pad;
		else oseg[i].code += pad;
	}
	/* the end of the areas: the last segment of the chain of the last file with such a section */
	{
		int t, d, b, s, lt = -1, ld = -1;

		mstate(nmev, &t, &d, &b);
		for (i = 0; i < nfiles; i++)
			for (k = 0; k < files[i].nent; k++) {
				if (files[i].code[k])
					lt = files[i].ts;
				if (files[i].data[k])
					ld = files[i].ds;
			}
		if (lt < 0) lt = files[nfiles - 1].ts;
		if (ld < 0) ld = lt;
		lastseg_t = lt;
		for (k = 0; k < chains[lt].n; k++)
			if (oseg[chains[lt].segs[k]].code)
				lastseg_t = chains[lt].segs[k];
		lastend_t = oseg[lastseg_t].code;
		lastseg_d = ld;
		for (k = 0; k < chains[ld].n; k++)
			if (oseg[chains[ld].segs[k]].data)
				lastseg_d = chains[ld].segs[k];
		lastend_d = oseg[lastseg_d].code + oseg[lastseg_d].data;
		lastseg_b = b;
		for (k = 0; k < chains[b].n; k++)
			if (oseg[chains[b].segs[k]].bss)
				lastseg_b = chains[b].segs[k];
		lastend_b = segsize(lastseg_b);
		/* measured: the original gives 0 for the end of the bss or of the data when it is in an overflow segment that also holds earlier areas */
		if (!fresh[lastseg_b] && lastseg_b != b && (oseg[lastseg_b].data || oseg[lastseg_b].code))
			lastend_b = 0, lastseg_b = 0;
		if (lastseg_d != ld && oseg[lastseg_d].code)
			lastend_d = 0, lastseg_d = 0;
	}
	/* the stack segment */
	for (i = 0; i < nreserved; i++)
		if (!oseg[reserved[i]].used)
			oseg[reserved[i]].used = 3;
	{
		int n = 0, only = -1, s, maxs = -1;
		int stkseg;
		int addstack = 0, anyfresh = 0;

		for (s = 0; s < MAXSEG; s++)
			if (oseg[s].used) {
				n++;
				only = s;
				maxs = s;
			}
		{
			int t, d, b, combined;

			mstate(nmev, &t, &d, &b);
			combined = !fi && t == d && d == b && nreserved == 0;
			for (k = 0; k < nmev; k++)
				if (mev[k].kind != 0 || mev[k].seg != mev[0].seg)
					combined = 0;
			for (s = 0; s < MAXSEG; s++)
				if (fresh[s] && oseg[s].used)
					anyfresh = 1;
			if (fz)
				stkseg = 127;
			else if (stackseg >= 0) {
				stkseg = stackseg;
				addstack = 1;
			} else if (n <= 1)
				stkseg = n ? only : 0;
			else if (combined) {	/* the stack shares the highest segment with code or data */
				stkseg = maxs;
				for (s = 0; s < MAXSEG; s++)
					if (oseg[s].used && (oseg[s].code || oseg[s].data))
						stkseg = s;
			} else {
				/* a segment of its own, above everything that was used or mentioned */
				int mx = maxs;

				for (k = 0; k < nmev; k++)
					if (mev[k].seg > mx)
						mx = mev[k].seg;
				for (k = 0; k < nreserved; k++)
					if (reserved[k] > mx)
						mx = reserved[k];
				stkseg = mx + 1;
				addstack = 1;
			}
		}
		if (fr_explicit || (stackseg < 0 && glookup("_stkseg") < 0))
			addstack = 0;
		if (addstack) {
			oseg[stkseg].stack = 1;
			if (!oseg[stkseg].used)
				oseg[stkseg].used = 2;
		}
		if (stackseg >= 0 && glookup("_stkseg") < 0) {
			printf("sld: warning: _stkseg undefined\n");
			status = 1;
		}
		/* the reserved segments */
		for (i = 0; i < nreserved; i++)
			if (!oseg[reserved[i]].used)
				oseg[reserved[i]].used = 3;
		{
			static const char *rs[4] = { "_etext", "_edata", "_end", "_stkseg" };
			uint32_t rv[4];
			int g;

			rv[0] = (uint32_t)lastseg_t << 24 | lastend_t;
			rv[1] = (uint32_t)lastseg_d << 24 | lastend_d;
			rv[2] = (uint32_t)lastseg_b << 24 | lastend_b;
			rv[3] = (uint32_t)stkseg << 24;
			for (k = 0; k < 4; k++) {
				g = glookup(rs[k]);
				if (g < 0 || (fr && (!fd || k == 3)))
					continue;
				if (gs[g].type == (N_EXT | N_UNDF) && gs[g].val == 0) {
					gs[g].type = k == 3 ? (N_EXT | N_ABS) : (N_EXT | (k == 0 ? N_CODE : k == 1 ? N_DATA : N_BSS));
					gs[g].val = rv[k];
					gs[g].fidx = -2;
				} else
					report(1, "-- Multiply defined", rs[k]);
			}
		}
		/* global symbol values */
		for (i = 0; i < ngs; i++) {
			struct sym *sy = &gs[i];

			if (sy->fidx >= 0)
				sy->val = symval(&files[sy->fidx], sy);
			if (sarea(sy->type) || sy->fidx == -2)
				sy->segt = (sy->val >> 24) & 0x7F;	/* else the stale byte, as in ld */
			else if (fr)
				sy->segt = sy->rawsegt;
			if (sy->type != (N_EXT | N_UNDF) || fr)
				sy->type |= 0x40;
		}
		{
			uint32_t imsize = 0, bsssz = 0;
			unsigned char *image, *reloc = NULL, hdr[24];
			int segidx[MAXSEG], nloc = 0;
			unsigned char *segtab;
			int nseg = 0, e = 0;
			unsigned off;
			FILE *out;
			uint32_t entry;

			for (s = 0; s < MAXSEG; s++)
				if (oseg[s].used) {
					imsize += oseg[s].code + oseg[s].data;
					bsssz += oseg[s].bss;
					nseg++;
				}
			image = xmalloc(imsize + 1);
			memset(image, 0, imsize + 1);
			reloc = xmalloc(imsize + 1);
			memset(reloc, 0, imsize + 1);
			{
				int n2 = 0;

				for (s = 0; s < MAXSEG; s++)
					segidx[s] = oseg[s].used ? n2++ : 0;
			}
			/* the images: all code (segments in order), then all data */
			{
				unsigned cbase[MAXSEG], dbase[MAXSEG], pos = 0;

				for (s = 0; s < MAXSEG; s++) {		/* per segment: code, then data */
					cbase[s] = pos;
					pos += oseg[s].code;
					dbase[s] = pos;
					pos += oseg[s].data;
				}
				/* pass 2: locals, then the words */
				for (i = 0; i < nfiles; i++) {
					struct file *f = &files[i];
					int j;

					f->map = xmalloc((f->nslot + 1) * sizeof(int));
					f->nmap = f->nslot;
					for (j = 0; j < f->nslot; j++)
						f->map[j] = -1;
					if (!fx) {
						int fs0 = -1;
						unsigned fo = 0;

						for (k = 0; k < f->nent; k++)
							if (f->code[k]) { fs0 = f->tseg[k]; fo = f->toff[k]; break; }
						putfn(f->name, fs0 < 0 ? 0 : ((uint32_t)fs0 << 24 | fo));
					}
					for (j = 0; j < f->nsyms; j++) {
						struct sym *sy = &f->syms[j];
						int t = sy->type & 0x1F, g;

						if (fS && (t == 1 || t > 4))
							continue;
						if (!(sy->type & N_EXT)) {
							uint32_t v = sy->val;

							if (fx)
								continue;
							if (fX && sy->name[0] == 'L')
								continue;
							if (sarea(sy->type))
								v = symval(f, sy);
							putlocal(sy, v, sarea(sy->type) ? (v >> 24) & 0x7F : sy->segt);
							continue;
						}
						g = glookup(sy->name);
						if (g < 0)
							fatal("internal error: symbol not found");
						f->map[sy->slot] = g;
						if ((sy->type & ~0x40) != (N_EXT | N_UNDF) && gs[g].fidx != i && gs[g].fidx != -2)
							report(1, "- Multiply defined", sy->name);
					}
				}
				nloc = osize / 14;
				for (i = 0; i < nfiles; i++) {
					struct file *f = &files[i];
					int ent;

					for (ent = 0; ent < 2 * f->nent; ent++) {
						int isdata = ent >= f->nent, en = isdata ? ent - f->nent : ent;
						unsigned size = isdata ? f->data[en] : f->code[en];
						unsigned base = isdata ? f->dataoff[en] : f->codeoff[en], w;
						int seg = isdata ? f->dseg[en] : f->tseg[en];
						unsigned boff = isdata ? f->doff[en] : f->toff[en];
						unsigned dst0 = (isdata ? dbase[seg] + (boff - oseg[seg].code) : cbase[seg] + boff);

						for (w = 0; w < size; w += 2) {
							unsigned x = be16(f->img + base + w), r = be16(f->rel + base + w);
							uint32_t v = 0;
							int has = 0, rarea = 0;
							unsigned rout = r;

							if (r & 8) {
								int g = f->map[r >> 4];

								if (g < 0)
									fatal("Relocation error");
								v = gs[g].val;
								has = 1;
								if (fr) {
									if ((gs[g].type & ~0x40) == (N_EXT | N_UNDF)) {
										has = 0;
										rout = (unsigned)(gs[g].slot + nloc) << 4 | 8 | (r & 7);
									} else {
										int a = sarea(gs[g].type);

										rarea = a == 2 ? 1 : a == 3 ? 2 : a == 4 ? 3 : 0;
										rout = 1 | (unsigned)rarea << 1 | (r & 7) << 4 | (unsigned)((v >> 24) & 0x7F) << 8;
									}
								}
							} else if (r & 1) {
								int e2 = (r >> 8) & 0x7F, area = (r >> 1) & 3;

								if (e2 >= f->nent)
									e2 = 0;
								if (fr && area)
									rout = 1 | (unsigned)area << 1 | ((r >> 4) & 7) << 4 | (unsigned)(area == 1 ? f->tseg[e2] : area == 2 ? f->dseg[e2] : f->bseg[e2]) << 8;
								if (area == 1) { v = (uint32_t)f->tseg[e2] << 24 | f->toff[e2]; has = 1; }
								else if (area == 2) { v = (uint32_t)f->dseg[e2] << 24 | (f->doff[e2] - f->code[e2]); has = 1; }
								else if (area == 3) { v = (uint32_t)f->bseg[e2] << 24 | (f->boff[e2] - f->code[e2] - f->data[e2]); has = 1; }
							}
							if (has) {
								int kd = (r & 8) ? (r & 7) : (r >> 4) & 7;

								switch (kd) {
								case 0: x = (x + (v & 0xFFFF)) & 0xFFFF; break;
								case 1: x = (x & 0x8000) | (((v >> 24) & 0x7F) << 8); break;
								case 2: x = (((v >> 24) & 0x7F) << 8) | (((x & 0xFF) + (v & 0xFFFF)) & 0xFF); break;
								}
							}
							(void)rarea;
							put16(image + dst0 + w, x);
							put16(reloc + dst0 + w, rout);
						}
					}
				}
			}
			/* the global symbols */
			for (i = 0; i < ngs; i++)
				putsym(gs[i].val, gs[i].type, gs[i].segt, gs[i].name);
			memset(hdr, 0, sizeof hdr);
			put16(hdr, fO ? 0xE605 : fi ? 0xE611 : 0xE607);
			put32(hdr + 2, imsize);
			put32(hdr + 6, bsssz);
			put16(hdr + 10, 16 * nseg);
			put16(hdr + 12, fs ? 0 : osize);
			if (entryname) {
				int g = glookup(entryname);

				if (g < 0 || (gs[g].type & 0x1F) != N_CODE) {
					report(1, "Entry point not in text", NULL);
					entry = 0;
				} else
					entry = gs[g].val;
			} else {
				entry = 0;
				for (s = 0; s < MAXSEG; s++)
					if (oseg[s].code) { entry = (uint32_t)s << 24; break; }
			}
			put32(hdr + 14, entry);
			put16(hdr + 18, (fr ? 0 : 1) | (fz ? 0x20 : 0));
			segtab = xmalloc(16 * nseg + 1);
			memset(segtab, 0, 16 * nseg + 1);
			for (s = 0; s < MAXSEG; s++) {
				unsigned atr;

				if (!oseg[s].used)
					continue;
				atr = 0;
				if (oseg[s].code) atr |= 1;
				if (oseg[s].data) atr |= 2;
				if (oseg[s].bss) atr |= 4;
				if (oseg[s].used == 2) atr = 8;
				else if (oseg[s].stack) atr |= 8;
				if (oseg[s].used == 3) atr = 4;
				segtab[16 * e] = s;
				put16(segtab + 16 * e + 4, oseg[s].code);
				put16(segtab + 16 * e + 6, oseg[s].data);
				put16(segtab + 16 * e + 8, oseg[s].bss);
				put16(segtab + 16 * e + 10, atr);
				e++;
			}
			(void)off;
			out = fopen(outname, "wb");
			if (out == NULL) {
				fprintf(stderr, "sld: cannot create output\n");
				exit(2);
			}
			fwrite(hdr, 1, 24, out);
			fwrite(segtab, 1, 16 * nseg, out);
			fwrite(image, 1, imsize, out);
			if (fr)
				fwrite(reloc, 1, imsize, out);
			if (!fs)
				fwrite(obuf, 1, osize, out);
			fclose(out);
			if (status == 0)
				chmod(outname, 0755);
		}
	}
	return status;
}
