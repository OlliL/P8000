/*
 * ld - the WEGA non-segmented loader (ld.c 1.18, 06/27/83) built for Linux (a cross tool).
 *
 * Written from the WEGA binary /bin/ld, its manual page and measurements: the output is
 * compared with the original under z8k-emu (check.sh). It is not a transcription of the
 * original source; the structure follows the original (pass 1 collects the symbols and
 * the sizes while the arguments are read, pass 2 writes the image).
 *
 * Supported: non-segmented 16 bit objects (E707, E711), archives with and without
 * __.SYMDEF, the options -b -d -e -i -l -N -o -r -s -t -u -w -x -X -S -O -T. Not
 * supported (diagnosed): 8 bit objects (E507, E511).
 *
 * Search path of -l: the directories in $WEGA_LIBPATH (colon separated), default
 * /lib:/usr/lib:/z/bin. A file name that begins with / and does not exist is also looked up
 * below $WEGA_ROOT (the file tree of the original system, where /lib/crt0.o and
 * /lib/libc.a are), and the default of the search path then is $WEGA_ROOT/lib:...
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

/* ---- options ------------------------------------------------------------ */

static int fS, fX, fx, fs, fr, fr_explicit, fd, fi, fO, fw, fT;
static int sel[5];			/* -b ('b') or -t ('t') given for: [0] program, 1 text, 2 data, 3 bss */
static uint32_t seladdr[5];
static const char *outname = "a.out";
static const char *entryname;
static struct sym *entrysym;
static int entryidx = -1;

/* ---- input files -------------------------------------------------------- */

struct file {			/* one object, or one loaded member of an archive */
	char *name;		/* for the FN symbol: the base name, or the member name */
	unsigned char *img;	/* text, data */
	unsigned char *rel;
	struct sym *syms;	/* symbols of the file (names owned) */
	int nsyms, nslot;
	unsigned magic;
	unsigned ts, ds, bs;	/* text, data, bss size (even) */
	uint32_t tb, db, bb;	/* the bases when the file was loaded (offsets in the areas) */
	int dadj, badj;		/* object address adjustments (E711: 0, -ts) */
	int *map;		/* file symbol slot -> global index (-1 for locals) */
	int nmap;
};

static struct file *files;
static int nfiles, maxfiles;

static uint32_t ttotal, dtotal, btotal, ctotal;		/* sizes loaded so far; ctotal: commons */
static int nlocalslots;						/* local symbol slots counted in pass 1 */

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

static uint32_t
symvalue(const struct file *f, const struct sym *s, uint32_t tb, uint32_t db, uint32_t bb)
{
	switch (s->type) {
	case N_CODE: case N_CODE | N_EXT:
		return s->val + tb;
	case N_DATA: case N_DATA | N_EXT:
		return s->val + db + f->dadj;
	case N_BSS: case N_BSS | N_EXT:
		return s->val + bb + f->badj;
	}
	return s->val;
}

/*
 * Pass 1 for one object (an object file, or a member of an archive when member is set).
 * Returns 1 when the object was taken. A member is only taken when it defines a symbol
 * that is undefined at that time.
 */
static int
loadobject(const char *name, const unsigned char *data, size_t len, int member)
{
	struct file f;
	unsigned magic = be16(data);
	unsigned segt, syms, flag, imsize;
	const unsigned char *sg, *st;
	int i, count = 0, saved = ngs, savedslots = gslots, nl;
	int newl = 0;

	memset(&f, 0, sizeof f);
	if (magic == MAGIC_X1 || magic == MAGIC_X3 || magic == MAGIC_X4)
		fatal("%s: 8 bit objects are not supported", name);
	if (magic != MAGIC_N1 && magic != MAGIC_N3 && magic != MAGIC_N4)
		fatal("%s: Bad format", name);
	imsize = be32(data + 2);
	segt = be16(data + 10);
	syms = be16(data + 12);
	flag = be16(data + 18);
	if (flag & 1)
		report(1, "No relocation bits", NULL);
	if (segt < 16)
		fatal("%s: Invalid segment attributes", name);
	sg = data + 24;
	f.magic = magic;
	f.ts = (be16(sg + 4) + 1) & ~1u;
	f.ds = (be16(sg + 6) + 1) & ~1u;
	f.bs = (be16(sg + 8) + 1) & ~1u;
	f.dadj = magic == MAGIC_N3 ? 0 : -(int)f.ts;
	f.badj = f.dadj - (int)f.ds;
	f.tb = ttotal;
	f.db = dtotal;
	f.bb = btotal;
	f.img = (unsigned char *)data + 24 + segt;
	f.rel = f.img + imsize;
	st = f.rel + imsize;
	if (flag & 1)
		st = f.img + imsize;
	f.nsyms = parsesyms(st, syms, &f.syms, &f.nslot);
	f.name = xstrdup(name);
	(void)len;
	nl = 1;				/* the FN symbol */
	for (i = 0; i < f.nsyms; i++) {
		struct sym *s = &f.syms[i];
		int t = s->type & 0x1F, g;
		uint32_t v;

		if (fS && (t == 1 || t > 4))
			continue;
		if (fX && t == N_SN)
			continue;
		if (!(s->type & N_EXT)) {
			if (fX && s->name[0] == 'L')
				continue;
			nl += s->nslots;
			continue;
		}
		v = symvalue(&f, s, f.tb, f.db, f.bb);
		g = glookup(s->name);
		if (g < 0) {
			gnew(s->name, s->type, v);
			newl++;
			continue;
		}
		if (gs[g].type == (N_EXT | N_UNDF)) {
			if (s->type == (N_EXT | N_UNDF)) {
				if (v > gs[g].val)
					gs[g].val = v;
			} else {
				if (gs[g].val != 0 && s->type == (N_EXT | N_CODE)) {
					if (member)
						continue;
					report(0, "warning: external data item refers to text symbol", s->name);
				}
				count++;
				gs[g].type = s->type;
				gs[g].val = v;
			}
		} else if (member && !fw) {
			const char *p = s->name;

			if (!(s->type == (N_EXT | N_UNDF)) && !(p[0] == '_' && p[1] == '_'))
				report(0, "- warning: symbol redefined in archive", s->name);
		}
	}
	if (member && count == 0) {
		gtruncate(saved);
		(void)savedslots;
		free(f.name);
		return 0;
	}
	f.name = f.name;
	ttotal += f.ts;
	dtotal += f.ds;
	btotal += f.bs;
	nlocalslots += nl;
	addfile(&f);
	return 1;
}

static int
isobject(const unsigned char *p, size_t len)
{
	unsigned m;

	if (len < 24)
		return 0;
	m = be16(p);
	return m == MAGIC_N1 || m == MAGIC_N3 || m == MAGIC_N4 || m == MAGIC_X1 || m == MAGIC_X3 ||
	    m == MAGIC_X4;
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
		snprintf(path, sizeof path, "%s/lib%s.a", libdirs[i], l);
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

struct osym {
	uint32_t val;
	int type, segt;
	const char *name;
};

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
	putsym(val, N_FN, 0, b);
}

int
main(int argc, char **argv)
{
	int i;
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
	for (i = 1; i < argc; i++) {
		char *a = argv[i];

		if (a[0] != '-' || a[1] == 0) {
			inputs[ni] = a;
			kind[ni++] = 0;
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
			case 'b':
			case 't': {
				int k = 0, c = *p;
				char *end;
				unsigned long v;

				if (p[1]) {
					if (p[1] == 't') k = 1;
					else if (p[1] == 'd') k = 2;
					else if (p[1] == 'b') k = 3;
					else
						fatal("-%.2s: invalid selector (must be t, d or b)", p);
					p++;
				}
				if (sel[k])
					fatal("-%.2s: -%c%c already specified", p - (k ? 1 : 0), sel[k] == 't' ? 't' : 'b',
					    k ? "?tdb"[k] : ' ');
				if (++i >= argc)
					fatal("-%c: arg missing", c);
				v = strtoul(argv[i], &end, 0);
				if (end != argv[i] && *end)
					fatal("-%.2s %s: bad address", p - (k ? 1 : 0), argv[i]);
				if (end == argv[i])
					v = 0;
				sel[k] = c;
				seladdr[k] = v;
				break;
			}
			default:
				fatal("-%c: bad flag", *p);
			}
		}
	}
	(void)fO;
	(void)fT;
	(void)entrysym;
	(void)entryidx;

	/* pass 1: in the order of the arguments */
	for (i = 0; i < ni; i++) {
		if (kind[i] == 2) {
			int g = glookup(inputs[i]);

			if (g < 0)
				gnew(inputs[i], N_EXT | N_UNDF, 0);
		} else if (kind[i] == 1) {
			char *path = findlib(inputs[i]);

			if (path == NULL)
				fatal("-l%s: cannot open", inputs[i]);
			processfile(path);
		} else
			processfile(inputs[i]);
	}

	/* undefined symbols turn the output into a relocatable one */
	if (!fr) {
		for (i = 0; i < ngs; i++) {
			if (gs[i].type == (N_EXT | N_UNDF) && gs[i].val == 0 &&
			    strcmp(gs[i].name, "_etext") && strcmp(gs[i].name, "_edata") &&
			    strcmp(gs[i].name, "_end")) {
				fr++;
				fd = 0;
				break;
			}
		}
	}
	if (fr_explicit && (sel[0] || sel[1] || sel[2] || sel[3]))
		fprintf(stderr, "ld: warning, -r option overrides -t, -b\n");
	if (fr) {
		fs = 0;
		fO = 0;
		memset(sel, 0, sizeof sel);
	}

	/* the special symbols and the common areas */
	if (fd || !fr) {
		int g;

		static const char *rs[3] = { "_etext", "_edata", "_end" };
		static const int rtype[3] = { N_EXT | N_CODE, N_EXT | N_DATA, N_EXT | N_BSS };
		uint32_t rval[3];
		int k;

		rval[0] = ttotal;
		rval[1] = dtotal;
		rval[2] = btotal;
		for (k = 0; k < 3; k++) {
			g = glookup(rs[k]);
			if (g < 0)
				continue;
			if (gs[g].type == (N_EXT | N_UNDF) && gs[g].val == 0) {
				gs[g].type = rtype[k];
				gs[g].val = rval[k];
			} else
				report(1, "-- Multiply defined", rs[k]);
		}
		for (i = 0; i < ngs; i++) {
			if (gs[i].type == (N_EXT | N_UNDF) && gs[i].val != 0) {
				uint32_t sz = (gs[i].val + 1) & ~1u;

				gs[i].val = ctotal;
				gs[i].type = N_EXT | N_BSS | 0x100;	/* common, bss area after the bss */
				ctotal += sz;
			}
		}
	}

	if (!fr_explicit) {
		int any = 0, warned8 = 0;

		for (i = 0; i < ngs; i++) {
			if (gs[i].type != (N_EXT | N_UNDF))
				continue;
			status |= 1;
			if (gs[i].val == 0) {
				if (!any)
					fprintf(stderr, "Undefined:\n");
				any = 1;
				if (fNshort && strlen(gs[i].name) > 8 && !warned8) {
					warned8 = 1;
					report(0, "Warning - only 8 characters of name are significant", NULL);
				}
				fprintf(stderr, "%.*s\n", fNshort ? 8 : 256, gs[i].name);
			}
		}
	}

	/* layout */
	{
		uint32_t tbase, dbase, bbase, cbase;
		uint32_t imsize, tout, dout;
		unsigned char hdr[24 + 16];
		unsigned char *image, *reloc = NULL;
		FILE *out;
		uint32_t syms, symbytes;
		int nlocal;
		size_t k;
		struct { int f; unsigned start, size, end; } R[5];
		static const char *rname[4] = { "program", "text", "data", "bss" };

		/*
		 * The areas: R[1] text, R[2] data, R[3] bss with the commons in front of it; R[0] is
		 * the anchor of -b/-t without a selector. An area starts where the area it follows
		 * ends (text follows the anchor, data the text, bss the data; with -i the text starts
		 * at 0 and the data follows the anchor). -bX sets the start (rounded up to 256), -tX
		 * the end (the size is rounded up to 256). An area in front of an area with a
		 * selector is padded to 256.
		 */
		if (sel[0]) {
			for (i = fi ? 2 : 1; i < 4; i++)
				if (sel[i]) {
					fprintf(stderr, "ld: warning, -%c overrides -%c%c\n", sel[0], sel[i], "?tdb"[i]);
					sel[i] = 0;
				}
		}
		memset(R, 0, sizeof R);
		R[0].f = 4;
		R[1].f = fi ? 4 : 0;
		R[2].f = fi ? 0 : 1;
		R[3].f = 2;
		R[1].size = (fi && !fr) ? (ttotal + 0xff) & ~0xffu : ttotal;
		R[2].size = dtotal;
		R[3].size = ctotal + btotal;
		R[0].size = dtotal + ctotal + btotal + fi == 0 ? ttotal : 0;
		for (i = sel[0] ? 0 : 1; i < 4; i++) {
			unsigned start;

			if (sel[i] == 'b') {
				start = (seladdr[i] + 0xff) & 0xff00;
				if (start != seladdr[i] && !(seladdr[i] == 0x10000))
					fprintf(stderr, "ld: %s: -b%c:warning, %s top or bottom rounded to 0x%x\n", curfile,
					    "?tdb"[i], rname[i], start);
				start &= 0xffff;
			} else if (sel[i] == 't') {
				if (R[i].size > seladdr[i])
					report(1, "top address too low", NULL);
				start = (seladdr[i] - R[i].size) & 0xff00;
				R[i].size = (R[i].size + 0xff) & 0xff00;
			} else
				start = R[R[i].f].end;
			R[0].size = 0;
			R[i].end = (start + R[i].size) & 0xffff;
			if (sel[i + 1]) {
				R[i].end = (R[i].end + 0xff) & 0xff00;
				R[i].size = (R[i].end - start) & 0xffff;
			}
			if (start > R[i].end)
				report(1, "area wrap around", rname[i]);
			{
				int j;

				for (j = fi ? 2 : 1; j < (int)i; j++)
					if (start < R[j].end && R[i].end > R[j].start) {
						fprintf(stderr, "ld: %s and %s areas overlap\n", rname[i], rname[j]);
						status = 1;
					}
			}
			R[i].start = start;
		}
		if (sel[0])
			sel[fi ? 2 : 1] = sel[0];
		tbase = R[1].start;
		dbase = R[2].start;
		cbase = R[3].start;
		bbase = cbase + ctotal;
		tout = R[1].size;
		dout = R[2].size;
		imsize = tout + dout;
		image = xmalloc(imsize + 1);
		memset(image, 0, imsize + 1);
		if (fr)
			reloc = xmalloc(imsize + 1);
		if (reloc)
			memset(reloc, 0, imsize + 1);

		/* global symbol values */
		for (i = 0; i < ngs; i++) {
			struct sym *s = &gs[i];

			switch (s->type & ~0x100) {
			case N_EXT | N_CODE: s->val += tbase; break;
			case N_EXT | N_DATA: s->val += dbase; break;
			case N_EXT | N_BSS:
				s->val += (s->type & 0x100) ? cbase : bbase;
				s->type = N_EXT | N_BSS;
				break;
			}
		}

		/* pass 2: files in load order */
		nlocal = 0;
		for (i = 0; i < nfiles; i++) {
			struct file *f = &files[i];
			uint32_t tb = tbase + f->tb, db = dbase + f->db, bb = bbase + f->bb;
			int j;
			f->map = xmalloc((f->nslot + 1) * sizeof(int));
			f->nmap = f->nslot;
			for (j = 0; j < f->nslot; j++)
				f->map[j] = -1;
			if (!fx) {
				putfn(f->name, tb);
				nlocal += 1;
			}
			for (j = 0; j < f->nsyms; j++) {
				struct sym *s = &f->syms[j];
				int t = s->type & 0x1F, g;
				uint32_t v;

				if (fS && (t == 1 || t > 4))
					continue;
				if (fX && t == N_SN)
					continue;
				if (!(s->type & N_EXT)) {
					if (fx)
						continue;
					if (fX && s->name[0] == 'L')
						continue;
					v = symvalue(f, s, tb, db, bb);
					putsym(v, s->type, s->segt, s->name);
					nlocal += s->nslots;
					continue;
				}
				g = glookup(s->name);
				if (g < 0)
					fatal("internal error: symbol not found");
				f->map[s->slot] = g;
				if (gs[g].type != (N_EXT | N_UNDF) && s->type != (N_EXT | N_UNDF)) {
					v = symvalue(f, s, tb, db, bb);
					if (gs[g].type != s->type || gs[g].val != v)
						report(1, "- Multiply defined", s->name);
				}
			}
		}
		/* pass 2b: the words of the files */
		for (i = 0; i < nfiles; i++) {
			struct file *f = &files[i];
			uint32_t tb = tbase + f->tb, db = dbase + f->db, bb = bbase + f->bb;
			unsigned w;

			/* the words of the file: text, then data */
			for (w = 0; w < f->ts + f->ds; w += 2) {
				unsigned x = be16(f->img + w), r = be16(f->rel + w);
				uint32_t dst = w < f->ts ? f->tb + w : tout + f->db + (w - f->ts);

				if (r & 8) {
					int g = f->map[r >> 4];

					if (g < 0)
						fatal("Relocation error");
					if (gs[g].type == (N_EXT | N_UNDF))
						r = (r & 7) | 8 | ((gs[g].slot + nlocal) << 4);
					else {
						x += gs[g].val;
						switch (gs[g].type & 0x1F) {
						case N_ABS: r = r & 1; break;
						case N_CODE: r = 2 | (r & 1); break;
						case N_DATA: r = 4 | (r & 1); break;
						case N_BSS: r = 6 | (r & 1); break;
						}
					}
				} else switch ((r >> 1) & 3) {
				case 1: x += tb; break;
				case 2: x += db + f->dadj; break;
				case 3: x += bb + f->badj; break;
				}
				put16(image + dst, x);
				if (reloc)
					put16(reloc + dst, r);
			}
		}

		/* the global symbols */
		for (i = 0; i < ngs; i++)
			putsym(gs[i].val, gs[i].type, gs[i].segt, gs[i].name);
		symbytes = osize;
		syms = fs ? 0 : symbytes;
		if (fs)
			osize = 0;
		(void)nlocal;

		memset(hdr, 0, sizeof hdr);
		put16(hdr, fO ? MAGIC_N4 : fi ? MAGIC_N3 : MAGIC_N1);
		put32(hdr + 2, imsize);
		put32(hdr + 6, R[3].size);
		put16(hdr + 10, 16);
		put16(hdr + 12, syms);
		{
			uint32_t entry = tbase;

			if (entryname) {
				int g = glookup(entryname);

				if (g < 0 || gs[g].type != (N_EXT | N_CODE))
					report(1, "Entry point not in text", NULL);
				else
					entry = gs[g].val;
			}
			put32(hdr + 14, entry);
		}
		put16(hdr + 18, fr ? 0 : 1);
		/* segment table */
		hdr[24 + 0] = 0;
		hdr[24 + 1] = sel[1] ? tbase >> 8 : 0;
		hdr[24 + 2] = sel[2] ? dbase >> 8 : 0;
		hdr[24 + 3] = sel[3] ? cbase >> 8 : 0;
		put16(hdr + 24 + 4, tout);
		put16(hdr + 24 + 6, dout);
		put16(hdr + 24 + 8, R[3].size);
		put16(hdr + 24 + 10, 7 | (sel[1] ? 0x10 : 0) | (sel[2] ? 0x20 : 0) | (sel[3] ? 0x40 : 0));

		out = fopen(outname, "wb");
		if (out == NULL) {
			fprintf(stderr, "ld: cannot create output\n");
			exit(2);
		}
		fwrite(hdr, 1, sizeof hdr, out);
		fwrite(image, 1, imsize, out);
		if (reloc)
			fwrite(reloc, 1, imsize, out);
		if (!fs)
			fwrite(obuf, 1, osize, out);
		fclose(out);
		if (status == 0)
			chmod(outname, 0755);
		else
			unlink(outname);
		(void)k;
	}
	return status;
}
