/*
 * gld - the WEGA segmented loader for large programs (gld.c V1.0, 06/22/88) built for Linux (a cross tool).
 *
 * Written from the WEGA binary /bin/gld, its manual page and measurements (compared with the original
 * under z8k-emu by check.py); see KNOWLEDGE.md, "gld on the host". It is not a transcription of the
 * original code. gld is a loader of its own (not the sld of host/ld/sld.c): text goes to the segments
 * 0-31, data to 32-63, bss and commons to 64-95, in the order of the files; there is no relocation
 * output, no -r, no -i and no -M with segment numbers.
 *
 * Search path of -l: the -L directories, then /lib, /usr/lib and /z/bin/lib (below $WEGA_ROOT when that is
 * set and the file is not found as given); the library of -lx is slibx.a.
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
#define N_EXT	0x20

#define ARMAG	0xFF65
#define SEGFULL	0xFFFE			/* a segment is left when the next section would reach this */

static unsigned be16(const unsigned char *p) { return p[0] << 8 | p[1]; }
static uint32_t be32(const unsigned char *p) { return (uint32_t)p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]; }
static void put16(unsigned char *p, unsigned v) { p[0] = v >> 8; p[1] = v; }
static void put32(unsigned char *p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v; }

/* ---- messages ----------------------------------------------------------- */

static int status;			/* 0, or 2 after a warning */
static const char *curfile;		/* the file that is being read */
static char curmember[15];		/* the archive member that is being read */
static const char *tmpout = "l.out";	/* the output file, "l.out" unless -o is given */
static int outcreated;

static void
cleanup_exit(int code)
{
	fflush(stdout);
	unlink("l.out");
	exit(code);
}

/*
 * The messages go to the standard output. "gld:" starts the first message only; level 1 ends the
 * program (status 4), 0 is a warning (the status becomes 2), -1 is a message only.
 */
static void
error(int level, const char *msg)
{
	if (status == 0)
		printf("gld:");
	if (curfile != NULL) {
		printf("%s", curfile);
		if (level != -1 && curmember[0])
			printf("(%s)", curmember);
		printf(": ");
	}
	printf("%s\n", msg);
	if (level == -1)
		return;
	if (level != 0)
		cleanup_exit(4);
	status = 2;
}

static void *
xmalloc(size_t n)
{
	void *p = malloc(n ? n : 1);

	if (p == NULL) {
		error(1, "ran out of memory");
		exit(4);
	}
	return p;
}

static char *
xstrdup(const char *s)
{
	return strcpy(xmalloc(strlen(s) + 1), s);
}

/* ---- options ------------------------------------------------------------ */

static int fs, fw, ft, fM;
static const char *outname;
static int outgiven;
static const char *libdirs[40];
static int nlibdirs;

/* ---- symbols ------------------------------------------------------------ */

struct gsym {
	char *name;
	uint32_t val;
	unsigned char type;		/* the type byte; 0x80: defined by an archive member */
};
static struct gsym *gs;
static int ngs, maxgs;
static int lastsym;			/* index+1 of the last entered symbol; stale after a rollback to nothing */
#define NHASH	4093
static int hashtab[NHASH];		/* index+1, chained by gnext */
static int *gnext;

static unsigned
hashname(const char *s)
{
	unsigned h = 0;

	while (*s)
		h = h * 31 + (unsigned char)*s++;
	return h % NHASH;
}

static int
glookup(const char *name)
{
	int i;

	for (i = hashtab[hashname(name)]; i; i = gnext[i - 1])
		if (strcmp(gs[i - 1].name, name) == 0)
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
			error(1, "ran out of memory (tab)");
	}
	gs[ngs].name = xstrdup(name);
	gs[ngs].val = val;
	gs[ngs].type = type;
	gnext[ngs] = hashtab[h];
	hashtab[h] = ++ngs;
	lastsym = ngs;
	return ngs - 1;
}

/*
 * Undo the entries added after the one that was last when the member was tried. That one is found by
 * its address: after an earlier rollback to an empty table it is stale, and the rollback then stops at
 * whatever entry has its place now, or fails when there is none.
 */
static void
gtruncate(int saved)
{
	int n = saved;

	if (saved > ngs)
		error(1, "symfree botch");
	while (ngs > n) {
		hashtab[hashname(gs[ngs - 1].name)] = gnext[ngs - 1];
		free(gs[ngs - 1].name);
		ngs--;
	}
	if (saved)
		lastsym = saved;
}

/* ---- objects ------------------------------------------------------------ */

struct osym {
	char *name;
	uint32_t val;
	int type, segt;
	int slot;			/* entry number: the index of a relocation */
};

/* where the sections of an entry went: segment number, offset, and the value to add to a position */
struct rec {
	int tseg, dseg, bseg;
	unsigned toff, doff, boff;
	int32_t tadj, dadj, badj;
};

struct obj {
	int argidx;			/* the argument that brought the object in */
	const char *file;		/* for the messages */
	char member[15];
	int nent;
	unsigned code[40], data[40], bss[40];
	unsigned codeoff[40], dataoff[40];	/* in the image */
	signed char sno[40][3];		/* the segment numbers of code, data and bss as the entry holds them */
	unsigned imsize, flag;
	const unsigned char *img, *rel;
	struct osym *syms;
	int nsyms;
	struct rec rec[40];
};
static struct obj **units;
static int nunits, maxunits;
static int curarg;

/* the segments being built */
static unsigned tsize[40], dsize[40], bsize[40];	/* size of each text, data and bss segment */
static int tn, dn, bn;					/* the last segment of each kind */
static uint32_t ttot, dtot, btot;			/* sizes incl. the padding */

static unsigned
round_up(unsigned v, unsigned a)
{
	return (v + a - 1) & ~(a - 1);
}

/* the layout state, saved while a member is tried */
struct lay {
	unsigned ts[40], ds[40], bs[40];
	int tn, dn, bn;
	uint32_t tt, dt, bt;
};

/*
 * Put the sections of the entries of o behind the ones already there (or compute where they would
 * go: commit is 0); the records tell where every entry went.
 */
static void
place(struct obj *o, int commit)
{
	struct lay save;
	int j, ltn = tn, ldn = dn, lbn = bn;
	unsigned lt = tsize[tn], ld = dsize[dn], lb = bsize[bn];

	(void)save;
	for (j = 0; j < o->nent; j++) {
		struct rec *r = &o->rec[j];

		memset(r, 0, sizeof *r);
		if (o->code[j]) {
			if ((uint32_t)lt + o->code[j] >= SEGFULL) {
				lt = 0;
				ltn++;
			}
			r->tseg = ltn;
			r->toff = lt;
			r->tadj = (int32_t)lt - ((int32_t)o->sno[j][0] << 8);
			lt += o->code[j];
		}
		if (o->data[j]) {
			if ((uint32_t)ld + o->data[j] >= SEGFULL) {
				ld = 0;
				ldn++;
			}
			r->dseg = 0x20 + ldn;
			r->doff = ld;
			r->dadj = (int32_t)ld - ((int32_t)o->sno[j][1] << 8);
			ld += o->data[j];
		}
		if (o->bss[j]) {
			if ((uint32_t)lb + o->bss[j] >= SEGFULL) {
				lb = 0;
				lbn++;
			}
			r->bseg = 0x40 + lbn;
			r->boff = lb;
			r->badj = (int32_t)lb - ((int32_t)o->sno[j][2] << 8);
			lb += o->bss[j];
		}
	}
	if (!commit)
		return;
	for (j = 0; j < o->nent; j++) {
		unsigned c;

		if ((c = o->code[j]) != 0) {
			if ((uint32_t)tsize[tn] + c >= SEGFULL) {
				unsigned v = round_up(tsize[tn], 0x100);

				if (v > SEGFULL)
					v = SEGFULL;
				ttot += v - tsize[tn];
				tsize[tn] = v;
				if (++tn > 0x1F)
					error(1, "too many text segments");
			}
			tsize[tn] += c;
			ttot += c;
		}
		if ((c = o->data[j]) != 0) {
			if ((uint32_t)dsize[dn] + c >= SEGFULL) {
				unsigned v = round_up(dsize[dn], 0x100);

				if (v > SEGFULL)
					v = SEGFULL;
				dtot += v - dsize[dn];
				dsize[dn] = v;
				if (++dn > 0x1F)
					error(1, "too many data segments");
			}
			dsize[dn] += c;
			dtot += c;
		}
		if ((c = o->bss[j]) != 0) {
			if ((uint32_t)bsize[bn] + c >= SEGFULL) {
				unsigned v = round_up(bsize[bn], 0x100);

				if (v > SEGFULL)
					v = SEGFULL;
				btot += v - bsize[bn];
				bsize[bn] = v;
				if (++bn > 0x1F)
					error(1, "too many bss segments");
			}
			bsize[bn] += c;
			btot += c;
		}
	}
}

/* the value of a symbol of the object o: segment and offset of the area it is in */
static uint32_t
symval(struct obj *o, const struct osym *s, int *type)
{
	int idx = s->segt, t = s->type & 0x3F;
	struct rec *r = &o->rec[idx < 0 || idx >= o->nent ? 0 : idx];

	switch (t) {
	case N_CODE: case N_CODE | N_EXT:
		return ((uint32_t)r->tseg << 24) + (s->val & 0xFFFF) + r->tadj;
	case N_DATA: case N_DATA | N_EXT:
		return ((uint32_t)r->dseg << 24) + (s->val & 0xFFFF) + r->dadj;
	case N_BSS: case N_BSS | N_EXT:
		return ((uint32_t)r->bseg << 24) + (s->val & 0xFFFF) + r->badj;
	case N_EXT:
		return s->val;
	}
	if (s->type & N_EXT)
		*type = N_EXT | N_ABS;
	return s->val;
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
		error(1, "premature EOF");
	fclose(f);
	*len = n;
	return p;
}

static int
nslots_of(const char *name)
{
	int n = strlen(name);

	return n <= 8 ? 1 : (6 + n + 1 + 13) / 14;
}

/* parse the symbol table of an object */
static int
parsesyms(const unsigned char *st, unsigned size, struct osym **out)
{
	struct osym *v = NULL;
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
				error(1, "premature EOF");
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
				error(1, "ran out of memory (tab)");
		}
		v[n].name = xstrdup(buf);
		v[n].val = be32(e);
		v[n].type = e[4];
		v[n].segt = (signed char)e[5];
		v[n].slot = o / 14;
		n++;
		o += ns * 14;
	}
	*out = v;
	return n;
}

/*
 * Pass 1 for one object (an object file, or a member of an archive when member is set). Returns 1
 * when the object was taken.
 */
static int
load1(const unsigned char *data, size_t len, int member, const char *file, const char *mname)
{
	struct obj *o = xmalloc(sizeof *o);
	unsigned magic = be16(data), segt, syms, i, pos;
	const unsigned char *st;
	int count = 0, saved = lastsym, k;

	memset(o, 0, sizeof *o);
	if (len < 24)
		error(1, "premature EOF");
	if (magic != 0xE611 && magic != 0xE607) {
		if (magic == ARMAG)
			error(1, "old archive");
		error(1, "bad magic number");
	}
	o->imsize = be32(data + 2);
	if (o->imsize & 1)
		error(1, "text/data size odd");
	segt = be16(data + 10);
	syms = be16(data + 12);
	o->flag = be16(data + 18);
	if (syms == 0) {
		if (o->imsize == 0) {
			free(o);
			return 0;
		}
		error(1, "no namelist");
	}
	o->nent = segt / 16;
	if (o->nent > 39)
		error(1, "no space for segmenttable");
	pos = 24;
	for (i = 0; i < (unsigned)o->nent; i++) {
		const unsigned char *e = data + pos + 16 * i;

		o->code[i] = be16(e + 4);
		o->data[i] = be16(e + 6);
		o->bss[i] = be16(e + 8);
		for (k = 0; k < 3; k++)
			o->sno[i][k] = (signed char)e[1 + k];
	}
	o->img = data + 24 + segt;
	{
		unsigned off = 0;

		for (i = 0; i < (unsigned)o->nent; i++) {	/* the image holds code and data entry by entry */
			o->codeoff[i] = off;
			off += o->code[i];
			o->dataoff[i] = off;
			off += o->data[i];
		}
	}
	o->rel = o->img + o->imsize;
	st = o->rel + (o->flag & 1 ? 0 : o->imsize);
	if ((size_t)(st - data) + syms > len)
		error(1, "premature EOF");
	o->file = file;
	o->argidx = curarg;
	if (mname)
		snprintf(o->member, sizeof o->member, "%s", mname);
	o->nsyms = parsesyms(st, syms, &o->syms);
	place(o, 0);
	for (k = 0; k < o->nsyms; k++) {
		struct osym *s = &o->syms[k];
		int t = s->type, g, ty;
		uint32_t v;

		if (member && (t & 0x3F) != N_EXT)
			t |= 0x80;
		if (!(s->type & N_EXT))
			continue;
		ty = t;
		v = symval(o, s, &ty);
		g = glookup(s->name);
		if (g < 0) {
			gnew(s->name, ty, v);
			continue;
		}
		if ((gs[g].type & 0x3F) == N_EXT) {		/* undefined, or a common */
			if ((ty & 0x3F) == N_EXT) {
				if ((int32_t)v > (int32_t)gs[g].val)
					gs[g].val = v;
			} else {
				if (gs[g].val != 0 && (ty & 0x3F) == (N_EXT | N_CODE))
					continue;
				count++;
				if (member)
					ty |= 0x80;
				gs[g].type = ty;
				gs[g].val = v;
			}
		} else if ((ty & 0x3F) != N_EXT && member && !(gs[g].type & 0x80) && !fw) {
			char buf[300];

			snprintf(buf, sizeof buf, "- warning  symbol (%s) redefined in archive", s->name);
			error(-1, buf);
		}
	}
	if (member && count == 0) {
		gtruncate(saved);
		for (k = 0; k < o->nsyms; k++)
			free(o->syms[k].name);
		free(o->syms);
		free(o);
		return 0;
	}
	place(o, 1);
	if (nunits == maxunits) {
		maxunits = maxunits ? maxunits * 2 : 64;
		units = realloc(units, maxunits * sizeof units[0]);
		if (units == NULL)
			error(1, "ran out of memory (tab)");
	}
	units[nunits++] = o;
	return 1;
}

/* ---- archives and files ------------------------------------------------- */

struct member {
	char name[15];
	uint32_t date, size;
};

static int
armember(const unsigned char *ar, size_t len, size_t off, struct member *m)
{
	int k;

	if (off + 26 > len)
		return 0;
	memcpy(m->name, ar + off, 14);
	m->name[14] = 0;
	for (k = 13; k >= 0 && m->name[k] == ' '; k--)
		m->name[k] = 0;
	m->date = be32(ar + off + 14);
	m->size = be32(ar + off + 22);
	return 1;
}

static void
loadmember(const char *file, const unsigned char *ar, size_t len, size_t off)
{
	struct member m;

	if (!armember(ar, len, off, &m)) {
		snprintf(curmember, sizeof curmember, "%s", "");
		return;
	}
	snprintf(curmember, sizeof curmember, "%s", m.name);
	if (strcmp(m.name, "Library.SC") == 0)
		return;
	if (off + 26 + m.size > len)
		error(1, "premature EOF");
	if (load1(ar + off + 26, m.size, 1, file, m.name) && fM)
		printf("\t%s\n", m.name);
}

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

static void
loadarchive(const char *path, const unsigned char *ar, size_t len)
{
	struct member m;
	size_t off = 2;
	int kind = 1;

	if (armember(ar, len, 2, &m) && strcmp(m.name, "__.SYMDEF") == 0) {
		struct stat sb;

		kind = 2;
		if (stat(rootpath(path), &sb) == 0 && (uint32_t)sb.st_mtime > m.date)
			kind = 3;
	}
	if (kind == 2) {
		const unsigned char *tab = ar + 2 + 26;
		unsigned n = m.size / 12, before;
		unsigned i;
		char *names;

		if (fM)
			printf("Archive with table of contents\n");
		names = xmalloc(m.size + 1);
		memcpy(names, tab, m.size);
		names[m.size] = 0;
		do {
			before = ngs;
			for (i = 0; i < n; i++) {
				const unsigned char *e = (const unsigned char *)names + 12 * i;
				int g;
				uint32_t o;
				unsigned j;

				g = glookup((const char *)e + 4);
				if (g < 0)
					continue;
				if ((gs[g].type & 0x3F) != N_EXT) {
					if (!(gs[g].type & 0x80) && !fw) {
						char buf[300];

						snprintf(buf, sizeof buf, "- warning  symbol (%s) redefined", gs[g].name);
						error(-1, buf);
					}
					continue;
				}
				o = be32(e);
				loadmember(path, ar, len, o);
				for (j = i + 1; j < n && be32((const unsigned char *)names + 12 * j) == o; j++)
					;
				i = j - 1;
			}
		} while ((unsigned)ngs != before);
		free(names);
		return;
	}
	if (kind == 3) {
		error(-1, "warning: table of contents is out of date\n");
		off += 26 + m.size + (m.size & 1);	/* the table itself is skipped */
	}
	while (armember(ar, len, off, &m)) {
		loadmember(path, ar, len, off);
		off += 26 + m.size + (m.size & 1);
	}
}

static char *
findlib(const char *l)
{
	static char path[1024];
	int i;

	if (*l == 0)
		l = "a";
	for (i = 0; i < nlibdirs; i++) {
		snprintf(path, sizeof path, "%s/slib%s.a", libdirs[i], l);
		if (access(rootpath(path), R_OK) == 0)
			return path;
	}
	return NULL;
}

/* one file argument (or -lx) in pass 1 */
static void
pass1file(const char *arg)
{
	const char *path = arg;
	size_t len;
	unsigned char *data;

	curfile = arg;
	curmember[0] = 0;
	if (arg[0] == '-' && arg[1] == 'l') {
		path = findlib(arg + 2);
		if (path == NULL)
			error(1, "cannot open");
		curfile = path;
	}
	data = readfile(rootpath(path), &len);
	if (data == NULL)
		error(1, "cannot open");
	if (fM)
		printf("%s\n", curfile);
	if (len >= 2 && be16(data) == ARMAG)
		loadarchive(path, data, len);
	else
		load1(data, len, 0, curfile, NULL);
}

/* ---- output ------------------------------------------------------------- */

static unsigned char *tbuf[40], *dbuf[40];

static int
relext(struct obj *o, int *map, int nmap, int idx)
{
	(void)o;
	return idx < nmap ? map[idx] : -1;
}

/* copy a section and apply its relocation words */
static void
relsec(struct obj *o, const unsigned char *src, const unsigned char *rel, unsigned size, unsigned char *dst,
    unsigned off, int seg, int *map, int nmap)
{
	unsigned w, n = size / 2, pc = off;

	memcpy(dst, src, size);
	for (w = 0; w < n; w++) {
		unsigned r = be16(rel + 2 * w), cur = be16(dst + 2 * w), res = cur;

		pc += 2;
		if (r == 0)
			continue;
		if (r & 8) {
			int g = relext(o, map, nmap, r >> 4);
			uint32_t v;
			int d;

			if (g < 0)
				error(1, "local symbol botch");
			if ((gs[g].type & 0x3F) == N_EXT)
				error(1, "undefined symbol");
			v = gs[g].val;
			switch (r & 7) {
			case 0:
				res = cur + (v & 0xFFFF);
				break;
			case 1:
				res = (cur & 0x8000) | ((v >> 16) & 0x7F00);
				break;
			case 2:
				res = (cur & 0xFF) + (v & 0xFF) + ((v >> 16) & 0x7F00);
				break;
			case 3:
				printf("extern 12bit rel. adr. %s\n", gs[g].name);
				if ((int)((v >> 24) & 0x7F) != seg)
					error(1, "external relativ adress(12) to another segment");
				d = (int16_t)(v & 0xFFFF) - (int)pc;
				d = (int16_t)d;
				if (d >= 0x800 || d <= -0x800)
					error(1, "relativ adress(12) to large");
				res = cur + (d & 0xFFF);
				break;
			case 4:
				printf("extern 16bit rel. adr. %s\n", gs[g].name);
				if ((int)((v >> 24) & 0x7F) != seg)
					error(1, "external relativ adress(16) to another segment");
				d = (int16_t)(v & 0xFFFF) - (int)pc;
				res = cur + d;
				break;
			}
		} else {
			int area = r & 6, idx = (int16_t)r >> 8, kind = (r & 0x70) >> 4;
			struct rec *rc;

			if (area == 0)
				continue;
			if (!(r & 1))
				error(1, "nonsegmented referenz");
			rc = &o->rec[idx < 0 || idx >= o->nent ? 0 : idx];
			switch (kind) {
			case 0:
				res = cur + (area == 2 ? rc->tadj : area == 4 ? rc->dadj : rc->badj);
				break;
			case 1: {
				int sg = area == 2 ? rc->tseg : area == 4 ? rc->dseg : rc->bseg;

				res = (cur & 0x8000) + (unsigned)((int)(signed char)sg << 8);
				break;
			}
			case 2:
				printf("aufgeloeste short seg adress\n");
				/* FALLTHROUGH */
			case 3:
				printf("aufgeloeste 12bit rel. adress\n");
				/* FALLTHROUGH */
			case 4:
				printf("aufgeloeste 16bit rel.adress\n");
				break;
			}
		}
		put16(dst + 2 * w, res & 0xFFFF);
	}
}

static unsigned char *obuf;
static size_t osize, ocap;

static void
putsym(uint32_t val, int type, const char *name)
{
	int len = strlen(name), ns = nslots_of(name);
	size_t need = (size_t)ns * 14;

	if (osize + need > ocap) {
		ocap = (osize + need) * 2;
		obuf = realloc(obuf, ocap);
		if (obuf == NULL)
			error(1, "ran out of memory (outb)");
	}
	memset(obuf + osize, 0, need);
	put32(obuf + osize, val);
	obuf[osize + 4] = type & 0x7F;
	obuf[osize + 5] = 0;
	if (len <= 8)
		memcpy(obuf + osize + 6, name, len);
	else {
		obuf[osize + 6] = 0x80 | (len + 1);
		memcpy(obuf + osize + 7, name, len);
	}
	osize += need;
}

/* pass 2 for one object: the symbols are checked, the sections copied and relocated */
static void
pass2unit(struct obj *o)
{
	int *map = NULL, nmap = 0, j;
	int maxslot = 0;

	curfile = o->file;
	snprintf(curmember, sizeof curmember, "%s", o->member);
	for (j = 0; j < o->nsyms; j++)
		if (o->syms[j].slot >= maxslot)
			maxslot = o->syms[j].slot + 1;
	nmap = maxslot + 1;
	map = xmalloc(nmap * sizeof *map);
	for (j = 0; j < nmap; j++)
		map[j] = -1;
	for (j = 0; j < o->nsyms; j++) {
		struct osym *s = &o->syms[j];
		int ty = s->type, gi;
		uint32_t v;

		if (!(s->type & N_EXT))
			continue;
		v = symval(o, s, &ty);
		gi = glookup(s->name);
		if (gi < 0)
			error(1, "internal error: symbol not found");
		if ((ty & 0x3F) == N_EXT) {
			map[s->slot] = gi;
		} else if (((gs[gi].type & 0x3F) != (ty & 0x3F) || gs[gi].val != v) && !fw) {
			printf("   %s: ", s->name);
			error(-1, " - warning: multiply defined");
		}
	}
	for (j = 0; j < o->nent; j++) {
		struct rec *r = &o->rec[j];
		unsigned size;

		if ((size = o->code[j]) != 0)
			relsec(o, o->img + o->codeoff[j], o->rel + o->codeoff[j], size, tbuf[r->tseg] + r->toff,
			    r->toff, r->tseg, map, nmap);
		if ((size = o->data[j]) != 0)
			relsec(o, o->img + o->dataoff[j], o->rel + o->dataoff[j], size,
			    dbuf[r->dseg - 0x20] + r->doff, r->doff, r->dseg, map, nmap);
	}
	free(map);
}

int
main(int argc, char **argv)
{
	int i, k, g, entryg = -1, stkg;
	char *lp;
	unsigned umaskv;
	FILE *out;
	unsigned char hdr[24];
	uint32_t entry = 0;
	mode_t mode;

	for (i = 1; i < argc; i++)		/* the -L directories come first */
		if (argv[i][0] == '-' && argv[i][1] == 'L') {
			if (argv[i][2] == 0)
				error(1, "-L: pathname missing");
			if (nlibdirs >= 22)
				error(1, "-L: too many directories");
			libdirs[nlibdirs++] = argv[i] + 2;
		}
	lp = getenv("WEGA_LIBPATH");
	if (lp) {
		char *s = xstrdup(lp), *c;

		for (c = strtok(s, ":"); c && nlibdirs < 38; c = strtok(NULL, ":"))
			libdirs[nlibdirs++] = c;
	} else {
		libdirs[nlibdirs++] = "/lib";
		libdirs[nlibdirs++] = "/usr/lib";
		libdirs[nlibdirs++] = "/z/bin/lib";
	}
	outname = "l.out";

	/* pass 1 */
	for (i = 1; i < argc; i++) {
		char *a = argv[i];

		curfile = NULL;
		if (a[0] != '-') {
			if (ft)
				printf("%s:\n", a);
			curarg = i;
			pass1file(a);
			continue;
		}
		for (k = 1; a[k]; k++) {
			switch (a[k]) {
			case 'L':
				goto nextarg;
			case 'X': case 'i': case 'x': case 'z':
				break;
			case 'M':
				fM++;
				break;
			case 'e':
				if (++i >= argc)
					error(1, "-e: arg missing");
				g = glookup(argv[i]);
				if (g < 0)
					g = gnew(argv[i], N_EXT, 0);
				entryg = g;
				break;
			case 'l': {
				char *copy = xstrdup(a + k - 1);

				copy[0] = '-';
				if (ft)
					printf("%s:\n", a);
				curarg = i;
				pass1file(copy);
				goto nextarg;
			}
			case 'o':
				if (++i >= argc)
					error(1, "-o where?");
				outname = argv[i];
				outgiven = 1;
				break;
			case 's':
				fs++;
				break;
			case 't':
				ft++;
				printf("*** pass1 ***\n");
				break;
			case 'w':
				fw++;
				break;
			default: {
				char buf[4] = { '-', a[k], 0 };

				curfile = buf;
				error(1, "bad flag");
			}
			}
		}
nextarg:	;
	}
	curfile = NULL;

	/* between the passes */
	stkg = glookup("_stkseg");
	if (stkg >= 0) {
		gs[stkg].type = N_EXT | N_ABS;
		gs[stkg].val = 0x7F000000;
	}
	for (g = 0; g < ngs; g++) {		/* the commons go to the bss */
		unsigned sz;

		if ((gs[g].type & 0x3F) != N_EXT || (sz = gs[g].val & 0xFFFF) == 0)
			continue;
		{
			uint32_t cur = bsize[bn];

			if (sz >= 2)
				cur = round_up(cur, 2);
			if (cur + sz > SEGFULL) {
				unsigned v = round_up(bsize[bn], 0x100);

				if (v > SEGFULL)
					v = SEGFULL;
				btot += v - bsize[bn];
				bsize[bn] = v;
				cur = 0;
				if (++bn > 0x1F)
					error(1, "too many bss segments");
			}
			btot += cur - bsize[bn];
			gs[g].val = ((uint32_t)(0x40 + bn) << 24) + cur;
			gs[g].type = 0x25;
			bsize[bn] = cur + sz;
			btot += sz;
		}
	}
	{
		unsigned v;

		v = round_up(tsize[tn], 0x100);
		ttot += v - tsize[tn];
		tsize[tn] = v;
		v = round_up(dsize[dn], 0x100);
		dtot += v - dsize[dn];
		dsize[dn] = v;
		v = round_up(bsize[bn], 0x100);
		btot += v - bsize[bn];
		bsize[bn] = v;
	}
	k = 0;
	for (g = 0; g < ngs; g++) {
		switch (gs[g].type & 0x3F) {
		case 0x20:
			if (!k++)
				printf("Undefined:\n");
			printf("%s\n", gs[g].name);
			break;
		case 0x25:
			gs[g].type = (gs[g].type & ~0x3F) | 0x24;
			break;
		}
	}
	if (k)
		error(1, "");

	if (nunits == 0 && ngs == 0) {
		fflush(stdout);
		fprintf(stderr, "gld: no object loaded (the original writes undefined data)\n");
		return 4;
	}

	/* pass 2 */
	if (ft)
		printf("*** pass2 ***\n");
	umask(umaskv = umask(0));
	mode = 0777 & ~umaskv;
	for (g = 0; g < ngs; g++)
		putsym(gs[g].val, gs[g].type, gs[g].name);
	if (entryg >= 0) {
		if ((gs[entryg].type & 0x3F) != (N_EXT | N_CODE))
			error(0, "entry point not in text");
		else
			entry = gs[entryg].val;
	}
	{
		struct stat sb;
		int existed = stat(outname, &sb) == 0;

		out = fopen(outname, "wb");
		if (out == NULL) {
			curfile = outname;
			error(1, "cannot create");
		}
		outcreated = 1;
		if (existed && (sb.st_mode & 0111)) {
			mode = sb.st_mode & 07777;
			chmod(outname, sb.st_mode & 0666);
		} else
			chmod(outname, 0666 & ~umaskv);
	}
	for (k = 0; k <= tn; k++)
		tbuf[k] = calloc(tsize[k] ? tsize[k] : 1, 1);
	for (k = 0; k <= dn; k++)
		dbuf[k] = calloc(dsize[k] ? dsize[k] : 1, 1);
	for (i = 1; i < argc; i++) {
		char *a = argv[i];
		int u;

		curfile = NULL;
		curmember[0] = 0;
		if (a[0] != '-') {
			if (ft)
				printf("%s:\n", a);
			for (u = 0; u < nunits; u++)
				if (units[u]->argidx == i)
					pass2unit(units[u]);
			continue;
		}
		for (k = 1; a[k]; k++) {
			if (a[k] == 'e' || a[k] == 'o') {
				i++;
			} else if (a[k] == 'l') {		/* any 'l' in an argument starts a library */
				char *copy = xstrdup(a + k - 1);

				copy[0] = '-';
				if (ft)
					printf("%s:\n", a);
				curfile = copy;
				if (findlib(copy + 2) == NULL)
					error(1, "cannot open");
				for (u = 0; u < nunits; u++)
					if (units[u]->argidx == i)
						pass2unit(units[u]);
				break;
			}
		}
	}
	curfile = NULL;
	curmember[0] = 0;
	if (ft)
		printf("tsize = %X\ndsize = %X\nbsize = %X\n", ttot, dtot, btot);

	/* the file */
	memset(hdr, 0, sizeof hdr);
	put16(hdr, 0xE611);
	put32(hdr + 2, ttot + dtot);
	put32(hdr + 6, btot);
	put16(hdr + 10, 16 * (tn + dn + bn + 3));
	put16(hdr + 12, fs ? 0 : osize);
	put32(hdr + 14, entry);
	put16(hdr + 18, 0x21);
	fwrite(hdr, 1, 24, out);
	for (k = 0; k <= tn; k++) {
		unsigned char e[16] = { 0 };

		e[0] = k;
		put16(e + 4, tsize[k]);
		put16(e + 10, 1);
		fwrite(e, 1, 16, out);
	}
	for (k = 0; k <= dn; k++) {
		unsigned char e[16] = { 0 };

		e[0] = 0x20 + k;
		put16(e + 6, dsize[k]);
		put16(e + 10, 2);
		fwrite(e, 1, 16, out);
	}
	for (k = 0; k <= bn; k++) {
		unsigned char e[16] = { 0 };

		e[0] = 0x40 + k;
		put16(e + 8, bsize[k]);
		put16(e + 10, 4);
		fwrite(e, 1, 16, out);
	}
	for (k = 0; k <= tn; k++)
		fwrite(tbuf[k], 1, tsize[k], out);
	for (k = 0; k <= dn; k++)
		fwrite(dbuf[k], 1, dsize[k], out);
	if (!fs)
		fwrite(obuf, 1, osize, out);
	fclose(out);
	if (!outgiven) {
		unlink("a.out");
		if (rename("l.out", "a.out") != 0) {
			error(1, "cannot move l.out to a.out");
		}
		outname = "a.out";
	}
	if (status == 0)
		chmod(outname, mode);
	fflush(stdout);
	return status;
}
