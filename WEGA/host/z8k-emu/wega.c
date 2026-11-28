/*
 * wega.c - s.out loader and WEGA system call emulation on top of z8k.c.
 *
 * Usage: z8k-emu [-r ROOT] [-t|-T] [-v] [-e NAME=VALUE] PROGRAM [ARG...]
 *
 * PROGRAM is a WEGA s.out executable (E707/E711 non-segmented, E607/E611
 * segmented).  Absolute path names used by the guest are looked up below
 * ROOT (the directory holding the original WEGA tree: bin, lib, usr, ...).
 * Everything else (processes, pipes, descriptors) maps 1:1 onto Linux.
 *
 * System call ABI (see KNOWLEDGE.md): `sc #n`, arguments in r0, r1, ...
 * (a 32-bit value takes two registers, a segmented pointer is a register pair),
 * result in r4/r5, carry set on error with the error number in r4.
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <utime.h>
#include "z8k.h"

#ifdef __APPLE__
/* macOS: other names of the host's struct stat times; no SIGPWR (SIGINFO is the host signal for WEGA's 19: a guess);
   devname() of <sys/stat.h> clashes with the device name table below */
#define st_atim st_atimespec
#define st_mtim st_mtimespec
#define st_ctim st_ctimespec
#ifndef SIGPWR
#define SIGPWR SIGINFO
#endif
#define devname z8k_devname
#endif

typedef struct { uint8_t seg; uint16_t off; } Ad;

static const char *root = "";
static char rootlogf[1100];	/* Z8K_ROOTLOG, absolute (see rootlog) */
static char dirmoved[1100];	/* the old name of a directory given a new one by link (renamed) */
static char **envv;			/* guest default environment */
static int nenv;
static int um_on, um_ruid, um_euid, um_rgid, um_egid, um_fuid, um_fgid;	/* the user model (see um_start) */
static void um_owner(const char *hp, struct stat *st);
static void um_exec(const char *hp);
static uint32_t sig_kernel[32];		/* u.u_signal: the handler of each WEGA signal (0 default, odd ignore) */
static int plexus;			/* current program uses the Plexus system call ABI */
static uint8_t  segused[128];		/* segment exists (program image or mkseg) */
static uint32_t segsz[128];		/* current size in bytes of each program segment (segmented mode) */
static int mainseg;			/* the segment break() reports (last non-stack data segment) */
static unsigned brkend;			/* nonsegmented: the end of the data set by break() */

/* an open directory: its WEGA image (16-byte struct direct records) and the read position in it */
typedef struct Dir {
	int fd;
	uint8_t *buf;
	long len, pos;
	struct Dir *next;
} Dir;
static Dir *dirs;

/* ---------------------------------------------------------------- memory */

static uint8_t *dblk(Z8K *c, unsigned seg) { return z8k_block(c, 1, seg); }

static void gread(Z8K *c, Ad a, void *dst, size_t n)
{
	uint8_t *d = dst, *m = dblk(c, a.seg);
	while (n--) { *d++ = m[a.off]; a.off++; }
}
static void gwrite(Z8K *c, Ad a, const void *src, size_t n)
{
	const uint8_t *s = src;
	uint8_t *m = dblk(c, a.seg);
	while (n--) { m[a.off] = *s++; a.off++; }
}
static uint16_t gw(Z8K *c, Ad a) { uint8_t b[2]; gread(c, a, b, 2); return (b[0] << 8) | b[1]; }
static void gputw(Z8K *c, Ad a, uint16_t v) { uint8_t b[2] = { v >> 8, v }; gwrite(c, a, b, 2); }
static void gputl(Z8K *c, Ad a, uint32_t v) { gputw(c, a, v >> 16); a.off += 2; gputw(c, a, v); }

static int gstr(Z8K *c, Ad a, char *buf, size_t max)
{
	size_t i = 0;
	uint8_t *m = dblk(c, a.seg);
	for (;;) {
		char ch = m[a.off++];
		if (i + 1 < max) buf[i++] = ch;
		if (!ch) break;
	}
	buf[i < max ? i : max - 1] = 0;
	return i;
}

/* ------------------------------------------------------------- s.out load */

#define SG_CODE 1
#define SG_DATA 2
#define SG_BSS 4
#define SG_STACK 8

static uint16_t be16(const uint8_t *p) { return (p[0] << 8) | p[1]; }
static uint32_t be32(const uint8_t *p) { return ((uint32_t)be16(p) << 16) | be16(p + 2); }

typedef struct {
	int magic, segmented, sep, flags, nseg, plexus, overlay;
	uint32_t entry, bss;
	struct { int no, coff, doff, boff, code, data, bssz, atr; } sg[128];
	uint8_t *file; long size;
	long textoff;
} Exec;

static int exec_open(const char *host, Exec *x)
{
	FILE *f = fopen(host, "rb");
	long n;
	if (!f) return errno;
	fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
	x->file = malloc(n ? n : 1);
	if ((long)fread(x->file, 1, n, f) != n) { fclose(f); free(x->file); return EIO; }
	fclose(f);
	x->size = n;
	x->plexus = 0; x->overlay = 0;
	if (n < 16) { free(x->file); return ENOEXEC; }
	x->magic = be16(x->file);
	if (x->magic >= 0x107 && x->magic <= 0x109) {
		/* Plexus/PDP-11 style a.out: magic, text, data, bss, syms, entry, -, flag.
		   0407 combined I&D, 0410/0411 separate I&D (non-segmented only) */
		x->segmented = 0; x->sep = x->magic != 0x107; x->plexus = 1;
		x->bss = be16(x->file + 6); x->entry = be16(x->file + 10); x->flags = 0;
		x->nseg = 1;
		x->sg[0].no = 0; x->sg[0].coff = 0; x->sg[0].doff = 0; x->sg[0].boff = 0;
		x->sg[0].code = be16(x->file + 2); x->sg[0].data = be16(x->file + 4);
		x->sg[0].bssz = be16(x->file + 6); x->sg[0].atr = 7;
		x->textoff = 16;
		if (16 + x->sg[0].code + x->sg[0].data > n) { free(x->file); return ENOEXEC; }
		return 0;
	}
	if (n < 24) { free(x->file); return ENOEXEC; }
	switch (x->magic) {
	case 0xE607: x->segmented = 1; x->sep = 0; break;
	case 0xE611: x->segmented = 1; x->sep = 1; break;
	case 0xE707: x->segmented = 0; x->sep = 0; break;
	case 0xE711: x->segmented = 0; x->sep = 1; break;
	case 0xE705: x->segmented = 0; x->sep = 1; x->overlay = 1; break;
	default: free(x->file); return ENOEXEC;
	}
	{
		int segt = be16(x->file + 10), i;
		x->bss = be32(x->file + 6);
		x->entry = be32(x->file + 14);
		x->flags = be16(x->file + 18);
		x->nseg = segt / 16;
		if (x->nseg > 128 || 24 + segt > n) { free(x->file); return ENOEXEC; }
		for (i = 0; i < x->nseg; i++) {
			const uint8_t *p = x->file + 24 + 16 * i;
			x->sg[i].no = p[0]; x->sg[i].coff = p[1]; x->sg[i].doff = p[2]; x->sg[i].boff = p[3];
			x->sg[i].code = be16(p + 4); x->sg[i].data = be16(p + 6);
			x->sg[i].bssz = be16(p + 8); x->sg[i].atr = be16(p + 10);
		}
		x->textoff = 24 + segt;
	}
	return 0;
}

static void clear_mem(Z8K *c)
{
	int i;
	for (i = 0; i < 128; i++) {
		if (c->imem[i]) { if (c->dmem[i] == c->imem[i]) c->dmem[i] = NULL; free(c->imem[i]); c->imem[i] = NULL; }
		if (c->dmem[i]) { free(c->dmem[i]); c->dmem[i] = NULL; }
	}
}

/* build the process image from an opened executable */
static void exec_map(Z8K *c, Exec *x, char **argv, char **envp)
{
	long toff, doff;
	int i, stkseg = (x->flags & 0x20) ? 0x7F : 0x3F;	/* SF_7FSTK selects segment 0x7F, else the stack is in segment 0x3F */
	clear_mem(c);
	plexus = x->plexus;
	memset(segsz, 0, sizeof segsz);
	memset(segused, 0, sizeof segused);
	memset(c->r, 0, sizeof c->r);
	z8k_epu_reset(c);
	c->fl = 0;
	c->segmode = x->segmented;
	c->sepid = x->sep && !x->segmented;
	toff = x->textoff;
	for (i = 0, doff = toff; i < x->nseg; i++) doff += x->sg[i].code;
	for (i = 0; i < x->nseg; i++) {
		int no = x->segmented ? x->sg[i].no : 0;
		uint8_t *im, *dm;
		int dataoff;
		im = z8k_block(c, 0, no);
		if (c->sepid) dm = z8k_block(c, 1, no); else dm = im;
		if (x->sg[i].code) memcpy(im + x->sg[i].coff * 256, x->file + toff, x->sg[i].code);
		toff += x->sg[i].code;
		if (x->sg[i].doff) dataoff = x->sg[i].doff * 256;
		else if (c->sepid) dataoff = 0;
		else dataoff = x->sg[i].coff * 256 + x->sg[i].code;
		if (x->plexus && c->sepid && x->magic == 0x108) {
			/* 0410: data addresses start at the text size and wrap around 64K in D space */
			uint32_t t = x->sg[i].code, n1 = x->sg[i].data;
			if (t + n1 > 0x10000) { memcpy(dm + t, x->file + doff, 0x10000 - t); memcpy(dm, x->file + doff + (0x10000 - t), t + n1 - 0x10000); }
			else memcpy(dm + t, x->file + doff, n1);
			dataoff = t & 0xFFFF;
		} else if (x->sg[i].data) memcpy(dm + dataoff, x->file + doff, x->sg[i].data);
		doff += x->sg[i].data;
		(void)SG_CODE; (void)SG_DATA; (void)SG_BSS;
		if (x->sg[i].atr & SG_STACK) stkseg = no;
		{
			uint32_t ext = (uint32_t)dataoff + x->sg[i].data + x->sg[i].bssz;
			uint32_t cex = x->sg[i].coff * 256 + x->sg[i].code;
			if (cex > ext) ext = cex;
			ext = (ext + 255) & ~255u;
			segused[no & 127] = 1;
			if (ext > segsz[no & 127]) segsz[no & 127] = ext;
			if (!(x->sg[i].atr & SG_STACK) && (x->sg[i].data || x->sg[i].bssz)) mainseg = no;
		}
	}
	/* stack with argc, argv, envp, strings (see crt0.s) */
	{
		int argc = 0, ne = 0, k;
		size_t strbytes = 0;
		Ad sp;
		uint16_t top = 0xFFFE;
		int ptrsz = x->segmented ? 4 : 2;
		unsigned nptr;
		uint16_t base;
		uint16_t soff;
		while (argv[argc]) argc++;
		while (envp[ne]) ne++;
		for (k = 0; k < argc; k++) strbytes += strlen(argv[k]) + 1;
		for (k = 0; k < ne; k++) strbytes += strlen(envp[k]) + 1;
		strbytes = (strbytes + 1) & ~1u;
		nptr = (argc + 1 + ne + 1) * ptrsz + 2;
		soff = top - strbytes;
		base = (soff - nptr) & ~1u;
		sp.seg = x->segmented ? stkseg : 0;
		sp.off = base;
		gputw(c, sp, argc);
		{
			Ad p = sp; Ad s;
			s.seg = sp.seg; s.off = soff;
			p.off += 2;
			for (k = 0; k < argc + ne; k++) {
				char *str = k < argc ? argv[k] : envp[k - argc];
				if (k == argc) { if (x->segmented) gputl(c, p, 0); else gputw(c, p, 0); p.off += ptrsz; }
				if (x->segmented) gputl(c, p, ((uint32_t)s.seg << 24) | s.off); else gputw(c, p, s.off);
				p.off += ptrsz;
				gwrite(c, s, str, strlen(str) + 1);
				s.off += strlen(str) + 1;
			}
			if (k == argc) { if (x->segmented) gputl(c, p, 0); else gputw(c, p, 0); p.off += ptrsz; }
			if (x->segmented) gputl(c, p, 0); else gputw(c, p, 0);
		}
		if (x->segmented) { c->r[14] = stkseg << 8; c->r[15] = base; }
		else c->r[15] = base;
	}
	if (x->segmented) { c->pcseg = (x->entry >> 24) & 0x7F; c->pc = x->entry & 0xFFFF; }
	else { c->pcseg = 0; c->pc = x->entry & 0xFFFF; }
	c->halted = 0;
}

/* ------------------------------------------------------------ path mapping */

/* the guest /tmp: ROOT/tmp, or the directory Z8K_TMPDIR (one per checkall run, so that parallel runs do not share it) */
static const char *guesttmp(void)
{
	static const char *t;
	if (!t) { t = getenv("Z8K_TMPDIR"); if (!t || !*t) t = ""; }
	return t;
}

/* Z8K_ROOTLOG=FILE: every guest path looked up below ROOT (not /tmp, not /dev), one line each, appended (the files of the
   tree a run depends on: the corpus runner takes them into its cache key). The file is opened for every line and closed
   again, so that the guest never sees a file descriptor of its own taken; a relative name is made absolute at the start. */


static void rootlog(const char *g)
{
	char line[1100];
	int fd, n;

	if (!*rootlogf)
		return;
	n = snprintf(line, sizeof line - 1, "%s\n", g);
	if (n > (int)sizeof line - 2)
		return;
	fd = open(rootlogf, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		return;
	if (write(fd, line, n) < 0) {
		close(fd);
		return;
	}
	close(fd);
}

/* The guest /tmp as a mount (Z8K_TMPDIR): its parent is ROOT ("/"), the parent of ROOT is ROOT, and ROOT lists tmp with
   the inode of the Z8K_TMPDIR directory (dirload), so that a program that walks up with ".." (pwd, getcwd: tar) finds
   "/tmp/NAME" for a working directory below Z8K_TMPDIR. realroot, realtmp: the canonical host paths, for comparisons. */
static const char *realroot(void)
{
	static char r[PATH_MAX];
	if (!*r && *root && !realpath(root, r)) snprintf(r, sizeof r, "%s", root);
	return r;
}

static const char *realtmp(void)
{
	static char t[PATH_MAX];
	if (!*t && *guesttmp() && !realpath(guesttmp(), t)) snprintf(t, sizeof t, "%s", guesttmp());
	return t;
}

/* a guest path whose ".." leaves the guest /tmp (to ROOT) or would go above ROOT, resolved by the guest's rules above;
   NULL for every other path (the host resolves it as the WEGA kernel does: a ".." after a name that is no directory is
   an error) */
static const char *dotdot(const char *g, char *buf, size_t n)
{
	char cur[PATH_MAX], comp[256];
	const char *p = g, *q, *rr = realroot(), *rt = realtmp();
	size_t k;
	char *e;
	int jump = 0;

	if (!*root || !strstr(g, ".."))
		return NULL;
	for (q = g; *q; ) {		/* only for a component that is exactly ".." */
		const char *s2 = strchr(q, '/');
		k = s2 ? (size_t)(s2 - q) : strlen(q);
		if (k == 2 && q[0] == '.' && q[1] == '.') break;
		q += k;
		while (*q == '/') q++;
	}
	if (!*q)
		return NULL;
	if (g[0] == '/')
		snprintf(cur, sizeof cur, "%s", rr);
	else if (!getcwd(cur, sizeof cur))
		return NULL;
	while (*p) {
		while (*p == '/') p++;
		if (!*p) break;
		for (k = 0; *p && *p != '/' && k < sizeof comp - 1; ) comp[k++] = *p++;
		comp[k] = 0;
		if (!strcmp(comp, "."))
			continue;
		if (!strcmp(comp, "..")) {
			if (*rt && !strcmp(cur, rt)) { snprintf(cur, sizeof cur, "%s", rr); jump = 1; }
			else if (!strcmp(cur, rr)) jump = 1;
			else if ((e = strrchr(cur, '/')) != NULL) { if (e == cur) e[1] = 0; else *e = 0; }
			continue;
		}
		if (*rt && !strcmp(cur, rr) && !strcmp(comp, "tmp")) { snprintf(cur, sizeof cur, "%s", rt); jump = 1; continue; }
		if (strlen(cur) + k + 2 >= sizeof cur) return NULL;
		strcat(cur, "/");
		strcat(cur, comp);
	}
	if (!jump)
		return NULL;
	snprintf(buf, n, "%s", cur);
	return buf;
}

static int devtab_active(void);

/* ---------------------------------------------------------------- mounted file systems
 * Z8K_MOUNTS=FILE: the table of the file systems mounted by the guest (shared by the processes of a run): a line
 * "ENTRY GUESTDIR HOSTDIR". mount (sys3.c smount) unpacks the WEGA file system of the image of a block device of the
 * guest /dev (host/tools/wegafs.py extract; Z8K_WEGAFS its path) into HOSTDIR (FILE.ENTRY); the guest paths below
 * GUESTDIR are then those below HOSTDIR. Read only: what is written there is lost at umount (the image is not changed).
 * Not modelled: ".." above the mounted directory (the host goes to the parent of HOSTDIR). */
typedef struct { int ent; char g[1024]; char h[1200]; } Mnt;
static Mnt mnts[16];
static int nmnt;
static char mntf[1100];

static void mnt_load(void)
{
	static struct stat last;
	struct stat st;
	FILE *f;
	if (!*mntf) {
		const char *e = getenv("Z8K_MOUNTS");
		char cwd[1024];
		if (!e || !*e) return;
		if (e[0] == '/' || !getcwd(cwd, sizeof cwd)) snprintf(mntf, sizeof mntf, "%s", e);
		else snprintf(mntf, sizeof mntf, "%s/%s", cwd, e);
	}
	if (stat(mntf, &st) < 0) { nmnt = 0; memset(&last, 0, sizeof last); return; }
	if (st.st_mtim.tv_sec == last.st_mtim.tv_sec && st.st_mtim.tv_nsec == last.st_mtim.tv_nsec &&
	    st.st_size == last.st_size && st.st_ino == last.st_ino) return;
	last = st;
	nmnt = 0;
	if ((f = fopen(mntf, "r")) == NULL) return;
	while (nmnt < 16 && fscanf(f, "%d %1023s %1199s", &mnts[nmnt].ent, mnts[nmnt].g, mnts[nmnt].h) == 3)
		nmnt++;
	fclose(f);
}

static void mnt_save(void)
{
	char t[1200];
	FILE *f;
	int i;
	snprintf(t, sizeof t, "%s.new%d", mntf, (int)getpid());
	if ((f = fopen(t, "w")) == NULL) return;
	for (i = 0; i < nmnt; i++) fprintf(f, "%d %s %s\n", mnts[i].ent, mnts[i].g, mnts[i].h);
	fclose(f);
	rename(t, mntf);
}

static const char *mnt_path(const char *g, char *buf, size_t n)	/* a guest path below a mounted directory */
{
	int i, best = -1;
	size_t bl = 0;
	mnt_load();
	for (i = 0; i < nmnt; i++) {
		size_t l = strlen(mnts[i].g);
		if (!strncmp(g, mnts[i].g, l) && (g[l] == 0 || g[l] == '/') && l > bl) { best = i; bl = l; }
	}
	if (best < 0) return NULL;
	snprintf(buf, n, "%s%s", mnts[best].h, g + bl);
	return buf;
}

/* Whiteouts of the Z8K_OVERLAY layer: when a program removes a name that the layer holds and the tree holds too (passwd removes /etc/passwd and
   links a new one), the tree's file must not show through: the layer gets an empty file .wh.NAME beside it. hostpath then takes the name
   as not there (a new name is made in the layer), a listing hides the tree's entry. */
static void wh_name(const char *lp, char *out, size_t n)
{
	const char *b = strrchr(lp, '/');
	size_t d = b ? (size_t)(b - lp + 1) : 0;
	snprintf(out, n, "%.*s.wh.%s", (int)d, lp, b ? b + 1 : lp);
}

static void layer_whiteout(const char *hp)	/* after the removal of hp: a mark when hp is in the layer and the tree has the name */
{
	const char *ov = getenv("Z8K_OVERLAY");
	size_t ol = ov ? strlen(ov) : 0;
	struct stat st;
	char rp[1100], wh[1100];
	int fd;
	if (!ol || strncmp(hp, ov, ol) || hp[ol] != '/') return;
	snprintf(rp, sizeof rp, "%s%s", root, hp + ol);
	if (lstat(rp, &st) < 0) return;
	wh_name(hp, wh, sizeof wh);
	if ((fd = open(wh, O_WRONLY | O_CREAT, 0600)) >= 0) close(fd);
}

/* The working directory as the guest knows it (an absolute guest path after a chdir to one), for the layer: the host's working directory
   is the layer's directory then, and a name that exists only in the tree (the entries a listing shows from both) is not found by a
   relative name. With Z8K_OVERLAY set a relative name is resolved as gcwd/name by the same rules as an absolute one. */
static char gcwd[1100];

static void gcwd_normalize(char *out, size_t n, const char *abs)	/* a/b/../c -> a/c */
{
	char tmp[1100], *seg, *save, *parts[128];
	int np = 0, i;
	size_t l = 0;
	snprintf(tmp, sizeof tmp, "%s", abs);
	for (seg = strtok_r(tmp, "/", &save); seg; seg = strtok_r(NULL, "/", &save)) {
		if (!strcmp(seg, ".")) continue;
		if (!strcmp(seg, "..")) { if (np) np--; continue; }
		if (np < 128) parts[np++] = seg;
	}
	out[0] = 0;
	for (i = 0; i < np; i++) l += snprintf(out + l, n - l, "/%s", parts[i]);
	if (!np) snprintf(out, n, "/");
}

static const char *hostpath(const char *g, char *buf, size_t n);

static const char *hostpath_rel(const char *g, char *buf, size_t n)	/* NULL: not to be taken from gcwd */
{
	static int on = -1;
	char abs[1500];
	if (on < 0) { const char *o = getenv("Z8K_OVERLAY"); on = o && *o; }
	if (!on || !gcwd[0] || g[0] == '/') return NULL;
	snprintf(abs, sizeof abs, "%s/%s", gcwd[1] ? gcwd : "", g);	/* not //name below / */
	return hostpath(abs, buf, n);
}

static const char *hostpath(const char *g, char *buf, size_t n)
{
	const char *dd;
	{ const char *r = hostpath_rel(g, buf, n); if (r) return r; }
	if (!g[0]) return ".";	/* the empty name is the current directory (nami.c: no component, flag 0): the glob of csh opens it */
	if (g[0] == '/' && *root && !strncmp(g, "/tmp", 4) && (g[4] == 0 || g[4] == '/')) {	/* a /tmp in the layer is the whole /tmp (before dotdot and the mount of Z8K_TMPDIR: /tmp/man/.. is then in the layer too) */
		const char *ov = getenv("Z8K_OVERLAY");
		struct stat ost;
		char od[1100];
		snprintf(od, sizeof od, "%s/tmp", ov ? ov : "");
		if (ov && *ov && stat(od, &ost) == 0 && S_ISDIR(ost.st_mode)) {	/* what is in the tree's /tmp (tmp/man) does not show through */
			snprintf(buf, n, "%s%s", ov, g);
			return buf;
		}
	}
	if ((dd = dotdot(g, buf, n)) != NULL) {
		if (g[0] == '/') rootlog(g);
		return dd;
	}
	if (g[0] != '/' || !*root) return g;
	{
		const char *mp = mnt_path(g, buf, n);
		if (mp) return mp;
	}
	if (!strncmp(g, "/dev/", 5) && !(devtab_active() && (!g[5] || !strcmp(g + 5, "."))))
		return g;	/* the host /dev, unless the guest /dev is a table (then /dev itself is ROOT/dev, listed from it) */
	if (!strncmp(g, "/tmp", 4) && (g[4] == 0 || g[4] == '/')) {
		if (*guesttmp()) {
			snprintf(buf, n, "%s%s", guesttmp(), g + 4);
			return buf;
		}
	}
	{	/* Z8K_OVERLAY: a layer over ROOT (the files of a case: a program at its place in the tree, /etc/mtab): a path
		   that is in the layer is taken from there, and a new name whose directory is in the layer is made there */
		static const char *ov;
		if (!ov) ov = getenv("Z8K_OVERLAY") ? getenv("Z8K_OVERLAY") : "";
		if (*ov) {
			struct stat st;
			char par[1100];
			const char *sl;
			snprintf(buf, n, "%s%s", ov, g);
			if (lstat(buf, &st) == 0)
				return buf;
			{	/* removed from the layer, and the tree has the name: it is not there (a new name is made in the layer) */
				char wh[1100];
				wh_name(buf, wh, sizeof wh);
				if (lstat(wh, &st) == 0)
					return buf;
			}
			snprintf(par, sizeof par, "%s", buf);
			sl = strrchr(par, '/');
			if (sl && sl > par + strlen(ov)) {
				par[sl - par] = 0;
				if (stat(par, &st) == 0 && S_ISDIR(st.st_mode)) {
					char rp[1100];
					snprintf(rp, sizeof rp, "%s%s", root, g);
					if (lstat(rp, &st) < 0)
						return buf;	/* a new name: in the layer */
				}
			}
		}
	}
	rootlog(g);
	snprintf(buf, n, "%s%s", root, g);
	return buf;
}

static uint16_t rdw_pl(Z8K *c, Ad a) { uint8_t b[2]; gread(c, a, b, 2); return (b[0] << 8) | b[1]; }

/* ---------------------------------------------------------------- signals
 * The model of the kernel (src/uts/sys/sig.c psig, sys4.c ssig, machdep.c sendsig, sysiret): a signal with a handler
 * of the guest is taken by a host handler that only marks it pending; between two instructions (after a system call:
 * with its error, EINTR for an interrupted one, as trap.c) the lowest pending signal is delivered: PC and FCW are put
 * on the guest stack like an interrupt (stack - 6: the PC segment word, the PC, the FCW), the PC is set to the handler,
 * the handler goes back to the default (not for SIGILL, SIGTRAP). The libc stub (signal.s) saves the registers, calls
 * the function of the program, and returns by sc 0 (sysiret: PC segment, PC, the flags of the FCW from the stack).
 * Default and ignore are the host's own (the host process dies by the signal; its parent maps the number back).
 * The numbers of WEGA (System III) are not those of the host: sig_host. */
static const int sig_host_fixed[20] = { 0, SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, 0, SIGFPE, SIGKILL,
	SIGBUS, SIGSEGV, SIGSYS, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, SIGUSR2, SIGCHLD, SIGPWR };
static volatile sig_atomic_t sig_pend[20], sig_any;

static int sig_host(int w)	/* WEGA number -> host number; SIGEMT (7) has none on the host: a real-time signal */
{
	if (w <= 0 || w >= 20) return 0;
#ifdef SIGRTMIN
	return w == 7 ? SIGRTMIN + 7 : sig_host_fixed[w];
#else
	return w == 7 ? SIGEMT : sig_host_fixed[w];	/* macOS has SIGEMT, no real-time signals */
#endif
}

static int sig_wega(int h)	/* host number -> WEGA number (0: none) */
{
	int w;
	for (w = 1; w < 20; w++)
		if (sig_host(w) == h) return w;
	return 0;
}

static void sig_catch(int h)
{
	int w = sig_wega(h);
	if (w) { sig_pend[w] = 1; sig_any = 1; }
}

static void sig_disp(int w, uint32_t h)	/* the host disposition for the handler h of WEGA signal w */
{
	struct sigaction sa;
	if (w == 9) return;
	memset(&sa, 0, sizeof sa);
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = h == 0 ? SIG_DFL : (h & 1) ? SIG_IGN : sig_catch;
	sa.sa_flags = 0;	/* no restart: an interrupted system call ends with EINTR */
	sigaction(sig_host(w), &sa, NULL);
}

static void sig_start(void)	/* at the start: what the host process inherits as ignored is ignored for the guest */
{
	int w;
	struct sigaction sa;
	struct rlimit rl;
	sigset_t none;
	sigemptyset(&none);	/* WEGA has no signal mask: a mask the host process inherits (e.g. QUIT blocked) is dropped */
	sigprocmask(SIG_SETMASK, &none, NULL);
	/* no host core dump when the guest dies by QUIT, ILL, FPE, SEGV ...: the kernel's core() (a WEGA core file, status
	   | 0200) is not modelled; a core limit of 1 makes Linux skip also a piped core_pattern (systemd-coredump) */
	if (getrlimit(RLIMIT_CORE, &rl) == 0 && (rl.rlim_max == RLIM_INFINITY || rl.rlim_max >= 1)) {
		rl.rlim_cur = 1;
		setrlimit(RLIMIT_CORE, &rl);
	}
	for (w = 1; w < 20; w++) {
		sig_pend[w] = 0;
		sig_kernel[w] = sigaction(sig_host(w), NULL, &sa) == 0 && sa.sa_handler == SIG_IGN;
	}
	sig_any = 0;
}

static void sig_exec(void)	/* exec: caught signals go back to the default, ignored ones stay (sys1.c setregs) */
{
	int w;
	for (w = 1; w < 20; w++)
		if (sig_kernel[w] && !(sig_kernel[w] & 1)) { sig_kernel[w] = 0; sig_disp(w, 0); }
}

static int wstat(int st)	/* a host wait status with the WEGA number of the signal */
{
	if (WIFSIGNALED(st)) {
		int w = sig_wega(WTERMSIG(st));
		return (st & ~0x7F) | (w ? w : WTERMSIG(st));
	}
	return st;
}

static void sig_deliver(Z8K *c)	/* psig + sendsig: the lowest pending signal */
{
	int w;
	uint32_t h;
	uint16_t fcw;
	Ad sp;
	sig_any = 0;
	for (w = 1; w < 20; w++) {
		if (!sig_pend[w]) continue;
		sig_pend[w] = 0;
		h = sig_kernel[w];
		if (h == 0 || (h & 1)) continue;	/* changed meanwhile: ssig clears what is pending */
		if (w != 4 && w != 5) { sig_kernel[w] = 0; sig_disp(w, 0); }
		for (; ++w < 20;) if (sig_pend[w]) sig_any = 1;	/* the rest after the next instruction */
		sp.seg = c->segmode ? (c->r[14] >> 8) & 0x7F : 0;
		sp.off = c->r[15] - 6;
		fcw = (c->segmode ? 0x8000 : 0) | 0x1800 | c->fl;	/* the high byte: user mode, interrupts on (a guess:
									   no program reads it) */
		gputw(c, sp, c->segmode ? c->pcseg << 8 : 0);
		sp.off += 2; gputw(c, sp, c->pc);
		sp.off += 2; gputw(c, sp, fcw);
		c->r[15] -= 6;
		if (c->segmode) c->pcseg = (h >> 24) & 0x7F;
		c->pc = h & 0xFFFF;
		return;
	}
}

/* a trap of the CPU (the EPU emulation: SIGFPE): 0 when the guest catches it (delivered after the instruction) */
static int sig_trap(Z8K *c, int w)
{
	(void)c;
	if (!sig_kernel[w] || (sig_kernel[w] & 1)) return -1;
	sig_pend[w] = 1; sig_any = 1;
	return 0;
}

/* ---------------------------------------------------------------- syscalls */

typedef struct { Z8K *c; int i; } Args;

static uint16_t a_int(Args *a) { return a->c->r[a->i++]; }
static uint32_t a_long(Args *a) { uint32_t h = a->c->r[a->i++]; return (h << 16) | a->c->r[a->i++]; }
static Ad a_ptr(Args *a)
{
	Ad p;
	if (a->c->segmode) { p.seg = (a->c->r[a->i] >> 8) & 0x7F; p.off = a->c->r[a->i + 1]; a->i += 2; }
	else { p.seg = 0; p.off = a->c->r[a->i++]; }
	return p;
}

/*
 * The guest sees a process id as a 16-bit int: the host pid itself would be cut to 16 bits, so a host pid that is a multiple
 * of 65536 became 0 (the parent of a fork took itself for the child: cc ran popt in place of the rest of the chain) and one
 * above 32767 negative (fork() < 0). gpid maps a host pid to 1..GPIDMAX, the same in every emulator process (getpid, the
 * result of fork and wait); pids up to GPIDMAX stay as they are. kill maps back through the processes this one knows.
 * GPIDMAX is the last number the WEGA kernel gives (newproc, src/uts/sys/slp.c: mpid goes back to 0 at 30000); /bin/kill
 * takes no number above 32000 ("usage"), so a guest number of 32001..32767 made `kill -16 $$` of the signal test fail.
 */
#define GPIDMAX	29999
static int gpid_host(pid_t pid) { return pid <= 0 ? (int)pid : (int)((pid - 1) % GPIDMAX) + 1; }

/*
 * Z8K_PIDDIR=DIR: numbers of the guest's own, the same in every run (the corpus runner gives each step a new, empty directory): the
 * first process the guest asks about gets 100, each fork the next number (taken by the parent before the fork, so that the order of
 * the numbers is the order of the forks). DIR/next is the counter (locked), DIR/hPID holds the guest number of a host process,
 * DIR/gNUM the host pid of a guest number (kill). passwd makes its salt of the pid: without this adduser could not be compared.
 */
static const char *piddir(void)
{
	static const char *d;
	static int done;
	if (!done) { done = 1; d = getenv("Z8K_PIDDIR"); if (d && !*d) d = NULL; }
	return d;
}

static int my_g, fork_k;	/* the guest number of this process (0: not known yet) and how many times it has forked */

/* The number of a child is made of the parent's number and the count of its forks, not of a counter that all processes share: which
   process forks first is a matter of timing, the path in the tree of forks is not. A number in use (the file DIR/gNUM exists) is
   passed over to the next; collisions of two paths are rare (the numbers go from 100 to GPIDMAX). */
static int pid_child(int parent, int k)
{
	char f[1100];
	int fd, g = 100 + (int)(((unsigned)parent * 2654435761u + (unsigned)k * 40503u + 17u) >> 8) % (GPIDMAX - 99);
	for (;;) {
		snprintf(f, sizeof f, "%s/g%d", piddir(), g);
		if ((fd = open(f, O_WRONLY | O_CREAT | O_EXCL, 0600)) >= 0) { close(fd); return g; }
		if (++g > GPIDMAX) g = 100;
	}
}

static void pid_file(const char *f, const char *b, int n)	/* made whole at once (rename): a reader never sees a half written number */
{
	char t[1200];
	int fd;
	snprintf(t, sizeof t, "%s.%ld", f, (long)getpid());
	if ((fd = open(t, O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) return;
	if (write(fd, b, n) < 0) {}
	close(fd);
	rename(t, f);
}

static void pid_set(pid_t host, int g)
{
	char f[1100], b[16];
	int n;
	snprintf(f, sizeof f, "%s/h%ld", piddir(), (long)host);
	n = snprintf(b, sizeof b, "%d", g);
	pid_file(f, b, n);
	snprintf(f, sizeof f, "%s/g%d", piddir(), g);
	n = snprintf(b, sizeof b, "%ld", (long)host);
	pid_file(f, b, n);
}

static void pidlog(const char *fmt, ...)	/* DIR/log: what the guest's processes did, one line each (to see why numbers differ) */
{
	char f[1100], line[300];
	int fd, n;
	va_list ap;
	if (!piddir()) return;
	va_start(ap, fmt);
	n = vsnprintf(line, sizeof line - 2, fmt, ap);
	va_end(ap);
	line[n++] = '\n';
	snprintf(f, sizeof f, "%s/log", piddir());
	if ((fd = open(f, O_WRONLY | O_CREAT | O_APPEND, 0600)) >= 0) { if (write(fd, line, n) < 0) {} close(fd); }
}

static int gpid(pid_t pid)
{
	char f[1100], b[16];
	int fd, g;
	ssize_t n;
	if (pid <= 0 || !piddir()) return gpid_host(pid);
	if (pid == getpid() && my_g) return my_g;
	snprintf(f, sizeof f, "%s/h%ld", piddir(), (long)pid);
	if ((fd = open(f, O_RDONLY)) >= 0) {
		n = read(fd, b, sizeof b - 1);
		close(fd);
		if (n > 0) { b[n] = 0; return atoi(b); }
	}
	if (pid == getpid()) {	/* this process, not told of yet: the first one of the step */
		if (!my_g) { my_g = 100; fork_k = 0; }
		pid_set(pid, my_g);
		return my_g;
	}
	g = pid_child(1, (int)(pid & 0xFFFF));	/* an unknown process (not forked by the guest): a number of its own */
	pid_set(pid, g);
	return g;
}
#define NKIDS 64
static pid_t kids[NKIDS];	/* the children forked by this process and not yet waited for */
static void kidadd(pid_t pid) { for (int i = 0; i < NKIDS; i++) if (kids[i] == 0) { kids[i] = pid; return; } }
static void kiddel(pid_t pid) { for (int i = 0; i < NKIDS; i++) if (kids[i] == pid) kids[i] = 0; }
static pid_t hostpid(int g)
{
	if (g <= 0) return g;	/* a process group, all processes */
	if (piddir()) {		/* the numbers of Z8K_PIDDIR: the host pid of the guest number, when it still lives */
		char f[1100], b[16];
		int fd;
		ssize_t n;
		snprintf(f, sizeof f, "%s/g%d", piddir(), g);
		if ((fd = open(f, O_RDONLY)) >= 0) {
			n = read(fd, b, sizeof b - 1);
			close(fd);
			if (n > 0) { b[n] = 0; if (kill((pid_t)atol(b), 0) == 0) return (pid_t)atol(b); }
		}
		return -1;
	}
	if (g == gpid(getpid())) return getpid();
	for (int i = 0; i < NKIDS; i++) if (kids[i] && g == gpid(kids[i])) return kids[i];
	if (g == gpid(getppid())) return getppid();
	{	/* another emulator of this session: the host pids g + k*GPIDMAX (/proc: the same program, the same session) */
		char self[512], exe[512], path[64];
		ssize_t n = readlink("/proc/self/exe", self, sizeof self - 1);
		pid_t sid = getsid(0);
		if (n > 0) {
			self[n] = 0;
			for (long h = g; h <= 4194304; h += GPIDMAX) {
				ssize_t m;
				snprintf(path, sizeof path, "/proc/%ld/exe", h);
				if ((m = readlink(path, exe, sizeof exe - 1)) <= 0) continue;
				exe[m] = 0;
				if (strcmp(exe, self) == 0 && getsid((pid_t)h) == sid) return (pid_t)h;
			}
		}
	}
	return -1;
}

static int sc_ok(Z8K *c, uint32_t v) { c->r[4] = v; c->fl &= ~F_C; return 0; }
static int sc_ok2(Z8K *c, uint16_t a, uint16_t b) { c->r[4] = a; c->r[5] = b; c->fl &= ~F_C; return 0; }
static int sc_long(Z8K *c, uint32_t v) { c->r[4] = v >> 16; c->r[5] = v; c->fl &= ~F_C; return 0; }
static int sc_err(Z8K *c, int e) { c->r[4] = e; c->r[5] = 0; c->fl |= F_C; return 0; }
static int sc_res(Z8K *c, long v) { return v < 0 ? sc_err(c, errno) : sc_ok(c, v); }

/* Z8K_TMPLOG=FILE: one line per event, appended with one write (parallel processes): the pids the guest sees, its
   programs and what it does below /tmp, to find clashes of temporary files between parallel guest processes */
static void tmplog(const char *fmt, ...)
{
	static int fd = -2;
	char line[1400];
	struct timespec ts;
	va_list ap;
	int n;

	if (fd == -2) {
		const char *f = getenv("Z8K_TMPLOG");
		fd = f ? open(f, O_WRONLY | O_CREAT | O_APPEND, 0644) : -1;
		if (fd >= 0) {	/* the guest's descriptor numbers are the host's: this one must not take a low number (csh counts on 3, 4, ...) */
			int hi = fcntl(fd, F_DUPFD_CLOEXEC, 500);
			close(fd);
			fd = hi;
		}
	}
	if (fd < 0)
		return;
	clock_gettime(CLOCK_REALTIME, &ts);
	n = snprintf(line, sizeof line, "%ld.%06ld %d ", (long)ts.tv_sec, ts.tv_nsec / 1000, (int)getpid());
	va_start(ap, fmt);
	n += vsnprintf(line + n, sizeof line - n - 1, fmt, ap);
	va_end(ap);
	if (n > (int)sizeof line - 2)
		n = sizeof line - 2;
	line[n++] = '\n';
	if (write(fd, line, n) < 0)
		return;
}

static int istmp(const char *gp) { return strncmp(gp, "/tmp/", 5) == 0; }

/* a host inode as the 16 bits of WEGA; never 0, which marks a free directory slot */
/* the i-node number of the guest (16 bit) from the host's: not the low 16 bits (ext4 puts the directories of a tree into groups of
   8192 i-nodes: two directories of one tree often have the same low bits and rmdir took one for the current directory) but a
   hash of all of them */
static unsigned ino16(ino_t ino)
{
	uint32_t h;
	unsigned i;
	if (ino > 0 && ino < 0x10000) return (unsigned)ino;	/* the numbers of the guest /dev (devtab) and small host numbers stay as they are */
	h = ((uint32_t)ino ^ (uint32_t)((uint64_t)ino >> 32)) * 2654435761u;
	i = (unsigned)(h >> 16) & 0xFFFF;
	return i ? i : 0xFFFF;
}

static Dir *dirof(int fd)
{
	Dir *d;
	for (d = dirs; d && d->fd != fd; d = d->next)
		;
	return d;
}

/* ------------------------------------------------------------ devices (increment 117) */

/*
 * The guest /dev (Z8K_DEVTAB=FILE): a table in the form of devtab.wega (TYPE+MODE OWNER GROUP MAJOR MINOR NAME, "l NAME"
 * another name of the entry before), followed by lines "model NAME KIND[:ARG] [excl] [console]" that give a device its
 * model:
 *   image:FILE   a floppy image: read, write, lseek on FILE
 *   host:PATH    a host file (e.g. /dev/null)
 *   proc:CMD     a byte stream to the model program CMD (sh -c), its stdin and stdout one socket: read waits for one byte
 *                and returns what is there (the ud driver), close shuts the stream and waits until the model has ended it
 *   console      the console of the minimal terminal (opened: the step's stdin for reading, stdout for writing)
 *   excl         a second open while it is open: ENXIO (udopen); console: only with Z8K_CONSOLE (the controlling
 *                terminal is the console)
 * Without a model an open gives ENXIO; a name not in the table does not exist. Without Z8K_DEVTAB /dev/ is the host's.
 * Z8K_CONSOLE=1: fds 0, 1, 2 are the console (isatty, the tty ioctls, fstat gives its numbers: ttyname finds it).
 * Z8K_CONSOLE_STATE=FILE: the settings of the console are kept in FILE for every process and run (con_load, con_save).
 */
#define NDEV 160
typedef struct { char name[64]; int ent; } DevName;
typedef struct { char type; unsigned mode, uid, gid, major, minor, ino, nlink; char model[600]; int excl, needcon, opens; } DevEnt;
static DevEnt devent[NDEV];
static DevName devname[NDEV];
static int ndevent, ndevname, devtab_on = -1;
static int devof[256];			/* per fd: 1 + the entry of a device fd, 0 */
static int devproc[256];		/* per fd: a proc model's socket */
static pid_t devpid[256];		/* per fd: the process that opened the proc model (only it ends the stream) */
#define DEVDEV 0x0101			/* st_dev of the guest /dev */

static void devtab_load(void)
{
	const char *f = getenv("Z8K_DEVTAB");
	FILE *fp;
	char line[800], t[16], n[64], m[600];
	unsigned mo, u, g, ma, mi;
	int i, last = -1;

	devtab_on = 0;
	if (!f || !*f || !(fp = fopen(f, "r")))
		return;
	devtab_on = 1;
	while (fgets(line, sizeof line, fp)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (sscanf(line, "l %63s", n) == 1 && line[1] == ' ') {
			if (last >= 0 && ndevname < NDEV) {
				snprintf(devname[ndevname].name, sizeof devname[0].name, "%s", n);
				devname[ndevname++].ent = last;
				devent[last].nlink++;
			}
			continue;
		}
		if (!strncmp(line, "model ", 6)) {	/* model NAME KIND[:ARG with blanks] [excl] [console] */
			int ex = 0, con = 0, k;
			char *q;
			if (sscanf(line + 6, "%63s %n", n, &k) < 1) continue;
			snprintf(m, sizeof m, "%s", line + 6 + k);
			for (q = m + strlen(m); q > m && (q[-1] == '\n' || q[-1] == ' '); ) *--q = 0;
			for (;;) {	/* the options at the end */
				q = strrchr(m, ' ');
				if (q && !strcmp(q + 1, "excl")) { ex = 1; *q = 0; }
				else if (q && !strcmp(q + 1, "console")) { con = 1; *q = 0; }
				else break;
			}
			for (i = 0; i < ndevname; i++)
				if (!strcmp(devname[i].name, n)) {
					DevEnt *e = &devent[devname[i].ent];
					snprintf(e->model, sizeof e->model, "%s", m);
					e->excl = ex;
					e->needcon = con;
				}
			continue;
		}
		if (sscanf(line, "%15s %u %u %u %u %63s", t, &u, &g, &ma, &mi, n) == 6 && (t[0] == 'b' || t[0] == 'c') &&
		    ndevent < NDEV && ndevname < NDEV) {
			mo = strtoul(t + 4, NULL, 8);
			last = ndevent++;
			devent[last] = (DevEnt){ t[0], mo, u, g, ma, mi, 100 + last, 1, "", 0, 0, 0 };
			snprintf(devname[ndevname].name, sizeof devname[0].name, "%s", n);
			devname[ndevname++].ent = last;
		}
	}
	fclose(fp);
}

static int devtab_active(void)
{
	if (devtab_on < 0) devtab_load();
	return devtab_on;
}

/* the entry of a guest path /dev/NAME (with the table), -1 for none, -2 when the path is no /dev name */
static int devlookup(const char *g0)
{
	int i;
	char abs[1500], norm[1500];
	const char *g = g0;
	/* a relative name after chdir("/dev") (ps goes to /dev and stats the names of the listing): gcwd/name */
	if (g0[0] && g0[0] != '/' && gcwd[0] && devtab_active()) {
		snprintf(abs, sizeof abs, "%s/%s", gcwd, g0);
		gcwd_normalize(norm, sizeof norm, abs);
		g = norm;
	}
	if (!devtab_active() || strncmp(g, "/dev/", 5) || !g[5] || strchr(g + 5, '/'))
		return -2;
	for (i = 0; i < ndevname; i++)
		if (!strcmp(devname[i].name, g + 5)) return devname[i].ent;
	return -1;
}

static int console_on(void)
{
	static int on = -1;
	if (on < 0) { const char *s = getenv("Z8K_CONSOLE"); on = s && *s == '1'; }
	return on;
}

static int console_ent(void)
{
	int i;
	if (!devtab_active()) return -1;
	for (i = 0; i < ndevname; i++)
		if (!strcmp(devname[i].name, "console")) return devname[i].ent;
	return -1;
}

static void dev_stat(int e, struct stat *st)
{
	DevEnt *d = &devent[e];
	memset(st, 0, sizeof *st);
	st->st_dev = DEVDEV;
	st->st_ino = d->ino;
	st->st_mode = (d->type == 'b' ? S_IFBLK : S_IFCHR) | d->mode;
	st->st_nlink = d->nlink;
	st->st_uid = d->uid;
	st->st_gid = d->gid;
	st->st_rdev = (d->major << 8) | d->minor;
}

/* the fd is a device: its entry, or -1 */
static int stdcon[3] = { 1, 1, 1 };	/* fd 0, 1, 2 still are the console (Z8K_CONSOLE): closed, or made a file by dup2 or a new open, they are not */

static int devfd(int fd)
{
	if (fd >= 0 && fd < 256 && devof[fd]) return devof[fd] - 1;
	if (fd >= 0 && fd <= 2 && stdcon[fd] && console_on()) return console_ent();
	return -1;
}

/* an fd that an open of /dev/... gave for the console model: a terminal reads the keyboard (host fd 0) and writes the
 * screen (host fd 1) on the same descriptor (fse opens /dev/tty O_RDWR); fds 0, 1, 2 keep their own host fds */
static int conhost(int fd, int wr)
{
	if (fd > 2 && fd < 256 && devof[fd] && !strcmp(devent[devof[fd] - 1].model, "console")) return wr ? 1 : 0;
	return fd;
}

static void devfd_set(int fd, int e)
{
	if (fd >= 0 && fd < 256) { devof[fd] = e + 1; devproc[fd] = 0; }
}

/* close of a proc model's socket: end the stream and wait until the model has ended its side (its files are written).
 * macOS (XNU): a shutdown(SHUT_WR) sometimes does not wake the model blocked in its read (measured: lost wakeup, 4 of 8 test
 * processes in 15000 rounds hang, a close of the whole end never). So if the end of the stream does not come, this end is closed
 * (the descriptor stays, on /dev/null) and the model's process is waited for. */
static void devproc_end(int fd)
{
	char b[256];
	struct stat st;
	struct pollfd pf;
	int left = 120 * 100;		/* 10 ms steps: a model that does not end must not hold the guest process forever */
#ifdef __APPLE__
	pid_t peer = 0;
	socklen_t pl = sizeof peer;
	int quiet = 30;			/* 300 ms for the end of the stream, then the fallback */
#endif

	if (fstat(fd, &st) < 0 || !S_ISSOCK(st.st_mode)) return;
#ifdef __APPLE__
	if (getsockopt(fd, 0 /* SOL_LOCAL */, 0x002 /* LOCAL_PEERPID */, &peer, &pl) < 0) peer = 0;
#endif
	shutdown(fd, SHUT_WR);
	pf.fd = fd;
	pf.events = POLLIN;
	while (left > 0) {
		int n = poll(&pf, 1, 10);
		if (n < 0 && errno != EINTR) break;
		if (n == 0) {
			left--;
#ifdef __APPLE__
			if (peer > 0 && --quiet == 0) {
				int nul = open("/dev/null", O_RDWR);
				if (nul >= 0) { dup2(nul, fd); close(nul); }
				while (left-- > 0 && !(kill(peer, 0) < 0 && errno == ESRCH))
					usleep(10000);
				return;
			}
#endif
			continue;
		}
		if (n > 0 && read(fd, b, sizeof b) <= 0) break;
	}
}

static void devclose(int fd)
{
	if (fd < 0 || fd >= 256 || !devof[fd]) return;
	if (devproc[fd] && devpid[fd] == getpid()) devproc_end(fd);
	if (devent[devof[fd] - 1].opens > 0) devent[devof[fd] - 1].opens--;
	devof[fd] = 0;
	devproc[fd] = 0;
}

static void devexit(void)
{
	int fd;
	for (fd = 0; fd < 256; fd++)
		if (devproc[fd] && devpid[fd] == getpid()) devproc_end(fd);
}

/* open of a device entry: the host fd, or -errno */
/* the access mode of the host's open for the flags of a guest open: O_TRUNC (FTRUNC) of a read-only open needs write access */
static int open_acc(int i)
{
	return (i & 3) == 0 && (i & 01000) ? O_RDWR : (i & 3);
}

static int devopen(int e, int mode)
{
	DevEnt *d = &devent[e];
	const char *m = d->model;
	int fd = -1;

	if (!*m) return -ENXIO;
	if (d->excl && d->opens > 0) return -ENXIO;
	if (d->needcon && !console_on()) return -ENXIO;
	if (!strncmp(m, "image:", 6) || !strncmp(m, "host:", 5)) {
		fd = open(strchr(m, ':') + 1, mode & 3);
		if (fd < 0) return -errno;
	} else if (!strcmp(m, "console")) {
		fd = dup((mode & 3) == 1 ? 1 : 0);
		if (fd < 0) return -errno;
	} else if (!strncmp(m, "proc:", 5)) {
		int sv[2];
		pid_t pid;
		static int atex;
		if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) return -errno;
		pid = fork();
		if (pid < 0) { close(sv[0]); close(sv[1]); return -EAGAIN; }
		if (pid == 0) {		/* the model is a grandchild: the guest's wait never sees it */
			if (fork() != 0) _exit(0);
			setsid();	/* a device, not a part of the job: a signal to the process group (^C) does not reach it */
			close(sv[0]);
			dup2(sv[1], 0);
			dup2(sv[1], 1);
			{	/* the model holds nothing of the guest: no socket of another model, no pipe of the caller */
				int cl;
				for (cl = 3; cl < 1024; cl++) close(cl);
			}
			execl("/bin/sh", "sh", "-c", m + 5, (char *)0);
			_exit(127);
		}
		waitpid(pid, NULL, 0);
		close(sv[1]);
		fd = sv[0];
		if (!atex) { atexit(devexit); atex = 1; }
		devfd_set(fd, e);
		devproc[fd] = 1;
		devpid[fd] = getpid();
		d->opens++;
		return fd;
	} else
		return -ENXIO;
	devfd_set(fd, e);
	d->opens++;
	return fd;
}

/* the guest /dev as a directory: "." and ".." then every name with the inode of its entry */
static int devdir(const char *path)
{
	char rp[PATH_MAX], want[PATH_MAX];
	if (!devtab_active() || !*root || !realpath(path, rp)) return 0;
	snprintf(want, sizeof want, "%s/dev", realroot());
	return !strcmp(rp, want);
}

/* the tty ioctls of the console: the settings are kept (the minimal terminal does not act on them yet) */
static uint8_t con_sgtty[6] = { 13, 13, '#', '@', 0, 010 | 020 };	/* B9600, erase #, kill @, ECHO|CRMOD */
static uint8_t con_tchars[6] = { 0177, 034, 021, 023, 004, 0377 };
static uint8_t con_termio[18] = { 0x05, 0x26, 0x00, 0x05, 0x00, 0xbd, 0x00, 0x3b, 0, 0177, 034, '#', '@', 004, 0, 0, 0, 0 };

/* Z8K_CONSOLE_STATE=FILE: the settings of the console live in FILE (sgtty, tchars, termio: 30 bytes), read before every
   get and written after every set, so that every process of the run (a fork is a host fork) and every later run with the
   same FILE sees what one of them set; without FILE (or before the first set) the settings are the model's above */
static const char *con_statefile(void)
{
	static int done;
	static const char *f;
	if (!done) { done = 1; f = getenv("Z8K_CONSOLE_STATE"); if (f && !*f) f = NULL; }
	return f;
}

static void con_load(void)
{
	const char *f = con_statefile();
	uint8_t b[30];
	FILE *fp;
	if (!f || !(fp = fopen(f, "rb"))) return;
	if (fread(b, 1, 30, fp) == 30) {
		memcpy(con_sgtty, b, 6);
		memcpy(con_tchars, b + 6, 6);
		memcpy(con_termio, b + 12, 18);
	}
	fclose(fp);
}

static void con_save(void)
{
	const char *f = con_statefile();
	char tmp[1100];
	FILE *fp;
	if (!f) return;
	snprintf(tmp, sizeof tmp, "%s.%d", f, (int)getpid());	/* written whole and renamed: a reader never sees half of it */
	if (!(fp = fopen(tmp, "wb"))) return;
	fwrite(con_sgtty, 1, 6, fp);
	fwrite(con_tchars, 1, 6, fp);
	fwrite(con_termio, 1, 18, fp);
	fclose(fp);
	rename(tmp, f);
}

/* the blocks of 512 bytes of a floppy by its format (src/uts/dev/fd.c fdblocks: the upper 4 bits of the minor) */
static const int fdblocks[16] = { 0, 500, 640, 320, 800, 1280, 1440, 1600, 720, 360, 720 };

static int devioctl(Z8K *c, int fd, unsigned cmd, Ad p)
{
	int e = devfd(fd);
	if (e >= 0 && devent[e].major == 1) {	/* the floppy driver (fd.c fdioctl): SHOWFD, else EINVAL */
		int b = fdblocks[(devent[e].minor >> 4) & 15];
		if (cmd != (('f' << 8) | 0) || !b) return -EINVAL;
		gputw(c, p, b);
		return 0;
	}
	if (e < 0 || e != console_ent())
		return -ENOTTY;
	con_load();
	switch (cmd) {
	case ('t' << 8) | 8: gwrite(c, p, con_sgtty, 6); return 0;			/* TIOCGETP */
	case ('t' << 8) | 9: case ('t' << 8) | 10: gread(c, p, con_sgtty, 6); con_save(); return 0;	/* TIOCSETP, TIOCSETN */
	case ('t' << 8) | 18: gwrite(c, p, con_tchars, 6); return 0;			/* TIOCGETC */
	case ('t' << 8) | 17: gread(c, p, con_tchars, 6); con_save(); return 0;	/* TIOCSETC */
	case ('t' << 8) | 16: case ('t' << 8) | 2: return 0;				/* TIOCFLUSH, TIOCHPCL */
	case ('T' << 8) | 1: gwrite(c, p, con_termio, 18); return 0;			/* TCGETA */
	case ('T' << 8) | 2: case ('T' << 8) | 3: case ('T' << 8) | 4: gread(c, p, con_termio, 18); con_save(); return 0;	/* TCSETA, TCSETAW, TCSETAF */
	case ('T' << 8) | 5: case ('T' << 8) | 6: case ('T' << 8) | 7: return 0;	/* TCSBRK, TCXONC, TCFLSH */
	}
	return -EINVAL;
}

/* fd was opened on path: when it is a directory, build its WEGA image, "." and ".." first */
/* The size of a directory as stat gives it: the entries of its WEGA image (16 bytes each: . and .., the names of dirload), not the host's
   4096. find reads a directory in blocks up to st_size and reports "cannot read" at the end of the real entries. */
static long dirlen(const char *path)
{
	DIR *dp;
	struct dirent *de;
	char *names[2048], lp[1100], wh[1100], rd[1100];
	int nn = 0, i;
	long n = 2;
	const char *ov = getenv("Z8K_OVERLAY");
	size_t ol = ov ? strlen(ov) : 0;
	if (devdir(path)) return (2 + ndevname) * 16L;
	if (!(dp = opendir(path))) return 0;
	while ((de = readdir(dp)) != NULL && nn < 2048) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") || !strncmp(de->d_name, ".wh.", 4) || !strcmp(de->d_name, ".wegafs-manifest.tsv")) continue;
		if (*rootlogf) {
			snprintf(lp, sizeof lp, "%s/%s", path, de->d_name);
			if (!strcmp(lp, rootlogf)) continue;
		}
		names[nn++] = strdup(de->d_name);
		n++;
	}
	closedir(dp);
	if (ol && !strncmp(path, ov, ol) && path[ol] == '/') {	/* a directory of the layer that is in the tree too: its names are added */
		snprintf(rd, sizeof rd, "%s%s", root, path + ol);
		if ((dp = opendir(rd)) != NULL) {
			while ((de = readdir(dp)) != NULL) {
				int have = 0;
				if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..") || !strcmp(de->d_name, ".wegafs-manifest.tsv")) continue;
				snprintf(lp, sizeof lp, "%s/%s", path, de->d_name);
				wh_name(lp, wh, sizeof wh);
				if (access(wh, F_OK) == 0) continue;
				for (i = 0; i < nn; i++)
					if (!strncmp(names[i], de->d_name, 14) && strlen(de->d_name) <= 14) { have = 1; break; }
				if (!have) n++;
			}
			closedir(dp);
		}
	}
	for (i = 0; i < nn; i++) free(names[i]);
	return n * 16;
}

static void dirload(int fd, const char *path)
{
	struct stat st, st2;
	struct dirent *de;
	DIR *dp;
	Dir *d;
	const char *name, *dd;
	char sub[PATH_MAX], ddb[PATH_MAX], rp[PATH_MAX];
	int k;

	if (fstat(fd, &st) < 0 || !S_ISDIR(st.st_mode) || !(dp = opendir(path)))
		return;
	if (!(d = calloc(1, sizeof *d))) {
		closedir(dp);
		return;
	}
	if (devdir(path)) {	/* the guest /dev from the table */
		int n;
		for (k = 0; k < 2 + ndevname; k++) {
			unsigned ino;
			if (k < 2) {
				snprintf(sub, sizeof sub, "%s/%s", path, k ? ".." : ".");
				if (stat(sub, &st) < 0) continue;
				ino = ino16(st.st_ino);
				name = k ? ".." : ".";
			} else {
				ino = devent[devname[k - 2].ent].ino;
				name = devname[k - 2].name;
			}
			if (!(d->buf = realloc(d->buf, d->len + 16))) break;
			memset(d->buf + d->len, 0, 16);
			d->buf[d->len] = ino >> 8;
			d->buf[d->len + 1] = ino;
			n = strlen(name);
			memcpy(d->buf + d->len + 2, name, n < 14 ? n : 14);
			d->len += 16;
		}
		closedir(dp);
		d->fd = fd;
		d->next = dirs;
		dirs = d;
		return;
	}
	for (k = 0; ; k++) {
		if (k < 2) {
			name = k ? ".." : ".";
			snprintf(sub, sizeof sub, "%s/%s", path, name);
			if (k && (dd = dotdot(sub, ddb, sizeof ddb)) != NULL && sub[0] != '/') ;	/* relative: the guest's .. */
			else dd = sub;
			if (k && realpath(path, rp) && ((*realtmp() && !strcmp(rp, realtmp())) || !strcmp(rp, realroot())))
				dd = realroot();	/* the parent of the guest /tmp and of / is / */
			if (stat(dd, &st) < 0) continue;
		} else {
			if (!(de = readdir(dp))) break;
			name = de->d_name;
			if (!strcmp(name, ".") || !strcmp(name, "..")) continue;
			if (!strncmp(name, ".wh.", 4)) continue;	/* a whiteout of the layer (layer_whiteout) */
			/* the host's manifest of the tree is no file of the guest: its name is longer than 14, pwd cannot stat it */
			if (!strcmp(name, ".wegafs-manifest.tsv")) continue;
			if (*rootlogf) {	/* the log of Z8K_ROOTLOG is not part of the guest's directory */
				snprintf(sub, sizeof sub, "%s/%s", path, name);
				if (!strcmp(sub, rootlogf)) continue;
			}
			st.st_ino = de->d_ino;
#ifdef __APPLE__
			/* a firmlink (/private, /Users) lists the inode of the entry, stat gives the one of its target: pwd of the guest
			   compares them */
			snprintf(sub, sizeof sub, "%s/%s", path, name);
			if (lstat(sub, &st2) == 0) st.st_ino = st2.st_ino;
#endif
			if (*realtmp() && !strcmp(name, "tmp") && realpath(path, rp) && !strcmp(rp, realroot()) &&
			    stat(realtmp(), &st2) == 0)
				st.st_ino = st2.st_ino;	/* / lists tmp with the inode of the guest /tmp */
		}
		if (!(d->buf = realloc(d->buf, d->len + 16)))
			break;
		memset(d->buf + d->len, 0, 16);
		d->buf[d->len] = ino16(st.st_ino) >> 8;
		d->buf[d->len + 1] = ino16(st.st_ino);
		memcpy(d->buf + d->len + 2, name, strnlen(name, 14));
		d->len += 16;
	}
	closedir(dp);
	{	/* a directory of the Z8K_OVERLAY layer that is in ROOT too lists both: the names of ROOT it does not hold are added */
		const char *ov = getenv("Z8K_OVERLAY");
		size_t ol = ov ? strlen(ov) : 0;
		if (ol && !strncmp(path, ov, ol) && path[ol] == '/') {
			char rd[PATH_MAX];
			DIR *rp2;
			snprintf(rd, sizeof rd, "%s%s", root, path + ol);
			if ((rp2 = opendir(rd))) {
				while ((de = readdir(rp2))) {
					int i, have = 0;
					name = de->d_name;
					if (!strcmp(name, ".") || !strcmp(name, "..") || !strcmp(name, ".wegafs-manifest.tsv")) continue;
					{	/* removed in the layer: the tree's entry is hidden */
						char wh[1100], lp[1100];
						snprintf(lp, sizeof lp, "%s/%s", path, name);
						wh_name(lp, wh, sizeof wh);
						if (access(wh, F_OK) == 0) continue;
					}
					for (i = 0; i + 16 <= d->len; i += 16)
						if (!strncmp((char *)d->buf + i + 2, name, 14) && strlen(name) <= 14) { have = 1; break; }
					if (have || !(d->buf = realloc(d->buf, d->len + 16))) continue;
					memset(d->buf + d->len, 0, 16);
					d->buf[d->len] = ino16(de->d_ino) >> 8;
					d->buf[d->len + 1] = ino16(de->d_ino);
					memcpy(d->buf + d->len + 2, name, strnlen(name, 14));
					d->len += 16;
				}
				closedir(rp2);
			}
		}
	}
	d->fd = fd;
	d->next = dirs;
	dirs = d;
}

/* the host path of an open file (NULL: unknown): F_GETPATH on macOS, /proc/self/fd elsewhere */
static const char *fdpath(int fd, char *buf, size_t n)
{
#ifdef F_GETPATH
	(void)n;
	return fcntl(fd, F_GETPATH, buf) == 0 ? buf : NULL;
#else
	char l[32];
	ssize_t k;
	snprintf(l, sizeof l, "/proc/self/fd/%d", fd);
	k = readlink(l, buf, n - 1);
	if (k <= 0) return NULL;
	buf[k] = 0;
	return buf;
#endif
}

/* the layer of "/" is the same directory as ROOT for the guest: "." and ".." of / are one (pwd compares them) */
static void rootfix(const char *hp, struct stat *st)
{
	const char *ov = getenv("Z8K_OVERLAY");
	char rov[PATH_MAX];
	struct stat rs;
	int i;
	if (!hp) return;
	/* the guest /tmp (Z8K_TMPDIR) is a directory of ROOT for the guest, so on the same device as ROOT whatever the host says (pwd of
	   a working directory below it compares the device of "." and "..": equal, it takes the name of the entry with the same
	   inode, as it can only for a name of 14 characters; different, it stats every name, and the long ones are cut) */
	if (*guesttmp() && realpath(hp, rov)) {
		size_t tl = strlen(realtmp());
		if (!strncmp(rov, realtmp(), tl) && (!rov[tl] || rov[tl] == '/') && stat(realroot(), &rs) == 0)
			st->st_dev = rs.st_dev;
	}
	if (!ov || !*ov) return;
	for (i = 0; i < 2; i++) {	/* the name as given, and the real one (F_GETPATH) */
		const char *o = ov;
		size_t ol;
		if (i) { if (!realpath(ov, rov)) return; o = rov; }
		ol = strlen(o);
		if (!strncmp(hp, o, ol) && (!hp[ol] || !strcmp(hp + ol, "/") || !strcmp(hp + ol, "/.")) &&
		    stat(realroot(), &rs) == 0) {
			st->st_dev = rs.st_dev;
			st->st_ino = rs.st_ino;
			return;
		}
	}
}

static void put_stat(Z8K *c, Ad a, const struct stat *st)
{
	gputw(c, a, st->st_dev); a.off += 2;
	gputw(c, a, ino16(st->st_ino)); a.off += 2;
	gputw(c, a, st->st_mode); a.off += 2;
	gputw(c, a, st->st_nlink); a.off += 2;
	gputw(c, a, st->st_uid); a.off += 2;
	gputw(c, a, st->st_gid); a.off += 2;
	gputw(c, a, st->st_rdev); a.off += 2;
	gputl(c, a, st->st_size); a.off += 4;
	gputl(c, a, st->st_atime); a.off += 4;
	gputl(c, a, st->st_mtime); a.off += 4;
	gputl(c, a, st->st_ctime);
}

/* Fetch string number n of a guest pointer array (argv/envp); returns NULL at the terminator. */
static char *gptr_str(Z8K *c, Ad base, int n)
{
	char s[2048];
	Ad p = base;
	if (c->segmode) {
		uint32_t v;
		p.off += n * 4;
		v = ((uint32_t)gw(c, p) << 16) | gw(c, (Ad){ p.seg, p.off + 2 });
		if (v == 0)
			return NULL;
		gstr(c, (Ad){ (v >> 24) & 0x7F, v & 0xFFFF }, s, sizeof s);
	} else {
		uint16_t v;
		p.off += n * 2;
		v = gw(c, p);
		if (v == 0)
			return NULL;
		gstr(c, (Ad){ 0, v }, s, sizeof s);
	}
	return strdup(s);
}

/* Copy a whole guest pointer array into a NULL-terminated host vector. */
static char **gvec(Z8K *c, Ad base, int *count)
{
	char **v = NULL, *s;
	int n = 0;
	while ((s = gptr_str(c, base, n)) != NULL) {
		v = realloc(v, (n + 2) * sizeof *v);
		v[n++] = s;
	}
	if (!v)
		v = malloc(2 * sizeof *v);
	v[n] = NULL;
	*count = n;
	return v;
}

static int do_exec(Z8K *c, Ad path, Ad argvp, int haveenv, Ad envp)
{
	char gp[1024], hb[1100], **av, **ev;
	int na, ne = 0, e, i;
	Exec x;
	const char *hp;
	gstr(c, path, gp, sizeof gp);
	hp = hostpath(gp, hb, sizeof hb);
	tmplog("exec %s", gp);
	if (piddir()) pidlog("exec %d %s", gpid(getpid()), gp);
	if (access(hp, X_OK) < 0) return sc_err(c, errno);
	e = exec_open(hp, &x);
	av = NULL;
	for (i = 0; e == ENOEXEC && i < 4; i++) {	/* a text file that starts with #!: the kernel runs the program named there, with the file as its argument */
		char l[256], ip[300], *q, *arg = NULL, **nv;
		size_t n;
		FILE *f = fopen(hp, "rb");
		if (!f) break;
		n = fread(l, 1, sizeof l - 1, f);
		fclose(f);
		l[n] = 0;
		if (n < 3 || l[0] != '#' || l[1] != '!') break;
		if ((q = strchr(l, '\n')) != NULL) *q = 0;
		q = l + 2;
		while (*q == ' ' || *q == '\t')
			q++;
		{ char *w = q; while (*w && *w != ' ' && *w != '\t') w++; if (*w) { *w++ = 0; while (*w == ' ' || *w == '\t') w++; if (*w) arg = w; } }
		if (!*q) break;
		if (!strchr(q, '/')) {	/* no path: /bin, then /usr/bin (seen on the real system: #!sh in /usr/bin/sccsdiff) */
			char t[300];
			snprintf(t, sizeof t, "/bin/%s", q);
			if (access(hostpath(t, hb, sizeof hb), X_OK) < 0) snprintf(t, sizeof t, "/usr/bin/%s", q);
			snprintf(ip, sizeof ip, "%s", t);
		} else
			snprintf(ip, sizeof ip, "%s", q);
		if (!av) av = gvec(c, argvp, &na);
		nv = malloc((na + 4) * sizeof *nv);
		nv[0] = strdup(ip);
		{ int k = 1; if (arg) nv[k++] = strdup(arg); nv[k++] = strdup(gp); for (int j = 1; j < na; j++) nv[k++] = av[j]; free(av[0]); free(av); nv[k] = NULL; av = nv; na = k; }
		snprintf(gp, sizeof gp, "%s", ip);
		hp = hostpath(gp, hb, sizeof hb);
		if (access(hp, X_OK) < 0) { e = errno; break; }
		e = exec_open(hp, &x);
	}
	if (e) return sc_err(c, e);
	/* copy argument and environment strings out of the old image */
	if (!av) av = gvec(c, argvp, &na);
	if (getenv("Z8K_EXEC_TRACE")) {
		fprintf(stderr, "exec %s", gp);
		for (i = 0; i < na; i++) fprintf(stderr, " %s", av[i]);
		fprintf(stderr, "\n");
	}
	ev = haveenv ? gvec(c, envp, &ne) : envv;
	if (x.overlay) {
		/* overlay (N_MAGIC4): replace the code of the running separate I&D
		   program and continue at the overlay entry; data, stack and registers stay */
		if (!c->sepid || x.nseg < 1) { free(x.file); return sc_err(c, ENOMEM); }
		{
			uint8_t *im = z8k_block(c, 0, 0);
			memset(im, 0, 0x10000);
			memcpy(im + x.sg[0].coff * 256, x.file + x.textoff, x.sg[0].code);
			c->pc = x.entry & 0xFFFE;
			c->halted = 0;
		}
		free(x.file);
		for (i = 0; i < na; i++) free(av[i]);
		free(av);
		if (haveenv) { for (i = 0; i < ne; i++) free(ev[i]); free(ev); }
		return 0;
	}
	um_exec(hp);
	exec_map(c, &x, av, ev);
	sig_exec();
	free(x.file);
	for (i = 0; i < na; i++) free(av[i]);
	free(av);
	if (haveenv) { for (i = 0; i < ne; i++) free(ev[i]); free(ev); }
	return 0;
}


/* ---------------------------------------------------------------- users
 * The model of the kernel's users (Z8K_UID set; without it the guest sees the host's ids and the host decides): the
 * real and effective ids of the guest (sys4.c getuid, setuid, getgid, setgid), the owner of every file as the guest
 * sees it, and the permissions decided as the kernel does (fio.c access, owner, suser; sys4.c chmod, chown, utime;
 * namei: a new name needs write permission on its directory; mknod only for the super-user). The host operation runs
 * with the rights of the host user, who owns every file of the case and of ROOT: where the host mode is in the way of
 * what the guest may do (root writes a file of mode 444), the emulator gives the host user the right for the moment.
 * The owners: a table of host (device, inode) -> uid, gid in the file Z8K_OWNERS (shared by the processes of a run,
 * appended to); a file without an entry belongs to root (0, 0) below ROOT outside the guest /tmp (a model choice: the
 * owners of the original tree are not in it), to the user of the case (Z8K_UID, Z8K_GID) elsewhere. setuid and setgid
 * of a program take effect at exec. Not modelled: the search permission of the directories of a path. */
static char um_ownf[1100];

static void um_start(void)
{
	const char *u = getenv("Z8K_UID"), *g = getenv("Z8K_GID"), *o = getenv("Z8K_OWNERS");
	char cwd[1024];
	if (!u || !*u) return;
	um_on = 1;
	um_ruid = um_euid = um_fuid = atoi(u);
	um_rgid = um_egid = um_fgid = g ? atoi(g) : 0;
	if (o && *o) {
		if (o[0] == '/' || !getcwd(cwd, sizeof cwd)) snprintf(um_ownf, sizeof um_ownf, "%s", o);
		else snprintf(um_ownf, sizeof um_ownf, "%s/%s", cwd, o);
	}
}

static int um_inroot(const char *hp)	/* below ROOT and not in the guest /tmp */
{
	char r[PATH_MAX];
	size_t n;
	if (!*root || !realpath(hp, r)) return 0;
	n = strlen(realroot());
	if (strncmp(r, realroot(), n) || (r[n] && r[n] != '/')) return 0;
	{
		char t[PATH_MAX];
		snprintf(t, sizeof t, "%s", *guesttmp() ? realtmp() : "");
		if (!*t) snprintf(t, sizeof t, "%s/tmp", realroot());
		n = strlen(t);
		if (!strncmp(r, t, n) && (!r[n] || r[n] == '/')) return 0;
	}
	return 1;
}

static void um_owner(const char *hp, struct stat *st)	/* st_uid, st_gid as the guest sees them */
{
	FILE *f;
	unsigned long d, i;
	int u, g, found = 0;
	if (*um_ownf && (f = fopen(um_ownf, "r")) != NULL) {
		while (fscanf(f, "%lu %lu %d %d", &d, &i, &u, &g) == 4)
			if (d == (unsigned long)st->st_dev && i == (unsigned long)st->st_ino) {
				found = u >= 0;
				st->st_uid = u; st->st_gid = g;
			}
		fclose(f);
	}
	if (found) return;
	if (hp && um_inroot(hp)) { st->st_uid = 0; st->st_gid = 0; }
	else { st->st_uid = um_fuid; st->st_gid = um_fgid; }
}

static void um_setown(const struct stat *st, int u, int g)	/* u < 0: the inode is gone */
{
	char l[100];
	int fd, n;
	if (!*um_ownf) return;
	n = snprintf(l, sizeof l, "%lu %lu %d %d\n", (unsigned long)st->st_dev, (unsigned long)st->st_ino, u, g);
	if ((fd = open(um_ownf, O_WRONLY | O_APPEND | O_CREAT, 0644)) >= 0) {
		if (write(fd, l, n) < 0) {}
		close(fd);
	}
}

static int um_perm(const char *hp, int m, int uid, int gid)	/* fio.c access: m 4 read, 2 write, 1 exec; 0 or EACCES */
{
	struct stat st;
	int mm = m << 6;
	if (stat(hp, &st) < 0) return errno;
	if (uid == 0) return 0;
	um_owner(hp, &st);
	if (uid != (int)st.st_uid) {
		mm >>= 3;
		if (gid != (int)st.st_gid) mm >>= 3;
	}
	return (st.st_mode & mm) ? 0 : EACCES;
}

static void um_parent(const char *hp, char *dir, size_t n)
{
	const char *sl = strrchr(hp, '/');
	if (!sl) snprintf(dir, n, ".");
	else if (sl == hp) snprintf(dir, n, "/");
	else snprintf(dir, n, "%.*s", (int)(sl - hp), hp);
}

/* the host operation; when the host refuses it (EACCES) what the guest may do, once more with the right given to the
   host user on `on` for the moment (its mode afterwards as before) */
#define UM_RETRY(on, bits, expr) ({ long _r = (expr); \
	if (_r < 0 && errno == EACCES) { struct stat _s; \
		if (stat(on, &_s) == 0 && chmod(on, (_s.st_mode | (bits)) & 07777) == 0) { \
			_r = (expr); int _e = errno; chmod(on, _s.st_mode & 07777); errno = _e; } } _r; })

static int um_isowner(const char *hp, struct stat *st)	/* fio.c owner: the owner or the super-user; else EPERM */
{
	if (stat(hp, st) < 0) return errno;
	um_owner(hp, st);
	return (um_euid == (int)st->st_uid || um_euid == 0) ? 0 : EPERM;
}

static void um_exec(const char *hp)	/* set-user-id, set-group-id: the effective ids become the owner's (sys1.c) */
{
	struct stat st;
	if (!um_on || stat(hp, &st) < 0) return;
	um_owner(hp, &st);
	if (st.st_mode & S_ISUID) um_euid = st.st_uid;
	if (st.st_mode & S_ISGID) um_egid = st.st_gid;
}

static int sc_ok(Z8K *c, uint32_t v);
static int sc_ok2(Z8K *c, uint16_t a, uint16_t b);
static int sc_err(Z8K *c, int e);

/* the calls the model changes; -2: not one of them (the normal code runs) */
static int um_call(Z8K *c, unsigned n)
{
	Args a = { c, 0 };
	char gp[1024], hb[1100], gp2[1024], hb2[1100], dir[1100];
	const char *hp, *hp2;
	struct stat st;
	Ad p, p2;
	int i, j, k, e;
	switch (n) {
	case 24: return sc_ok2(c, um_ruid, um_euid);
	case 47: return sc_ok2(c, um_rgid, um_egid);
	case 23:	/* setuid */
		i = a_int(&a);
		if (um_ruid == i || um_euid == 0) { um_ruid = um_euid = i; return sc_ok(c, 0); }
		return sc_err(c, EPERM);
	case 46:	/* setgid */
		i = a_int(&a);
		if (um_rgid == i || um_euid == 0) { um_rgid = um_egid = i; return sc_ok(c, 0); }
		return sc_err(c, EPERM);
	case 18:	/* stat: the owner of the model */
		p = a_ptr(&a); p2 = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if (stat(hp, &st) < 0) return sc_err(c, errno);
		um_owner(hp, &st);
		rootfix(hp, &st);
		if (S_ISDIR(st.st_mode)) st.st_size = dirlen(hp);
		put_stat(c, p2, &st);
		return sc_ok(c, 0);
	case 28:	/* fstat */
		i = a_int(&a); p = a_ptr(&a);
		if (devfd(i) >= 0 || fstat(i, &st) < 0) return -2;
		hp = fdpath(i, hb, sizeof hb);
		um_owner(hp, &st);
		rootfix(hp, &st);
		if (S_ISDIR(st.st_mode)) { Dir *dd = dirof(i); if (dd) st.st_size = dd->len; }
		put_stat(c, p, &st);
		return sc_ok(c, 0);
	case 5:		/* open: read and write permission */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		int cm = (i & 0400) ? a_int(&a) : 0;	/* FCREAT: the mode of a new file is the third argument */
		if (stat(hp, &st) < 0) {
			if (!(i & 0400) || errno != ENOENT) return sc_err(c, errno);
			um_parent(hp, dir, sizeof dir);
			if ((e = um_perm(dir, 2, um_euid, um_egid))) return sc_err(c, e);
			k = UM_RETRY(dir, S_IWUSR | S_IXUSR, open(hp, open_acc(i) | O_CREAT | O_EXCL, cm & 0777));
			if (k < 0) return sc_err(c, errno);
			if (fstat(k, &st) == 0) um_setown(&st, um_euid, um_egid);
			return sc_ok(c, k);
		}
		if ((i & 0400) && (i & 02000)) return sc_err(c, EEXIST);	/* FEXCL */
		if ((i & 3) != 1 && (e = um_perm(hp, 4, um_euid, um_egid))) return sc_err(c, e);
		if (((i & 3) != 0 || (i & 01000)) && (e = um_perm(hp, 2, um_euid, um_egid))) return sc_err(c, e);
		k = UM_RETRY(hp, (i & 3) == 0 && !(i & 01000) ? S_IRUSR : (i & 3) == 1 ? S_IWUSR : S_IRUSR | S_IWUSR,
			     open(hp, open_acc(i) | ((i & 01000) ? O_TRUNC : 0)));
		if (k < 0) return sc_err(c, errno);
		dirload(k, hp);
		return sc_ok(c, k);
	case 8:		/* creat: an existing file needs write permission, a new name write permission on its directory */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if (stat(hp, &st) == 0) {
			if ((e = um_perm(hp, 2, um_euid, um_egid))) return sc_err(c, e);
			k = UM_RETRY(hp, S_IWUSR, open(hp, O_WRONLY | O_TRUNC));
			return k < 0 ? sc_err(c, errno) : sc_ok(c, k);
		}
		um_parent(hp, dir, sizeof dir);
		if ((e = um_perm(dir, 2, um_euid, um_egid))) return sc_err(c, e);
		k = UM_RETRY(dir, S_IWUSR | S_IXUSR, open(hp, O_WRONLY | O_CREAT | O_TRUNC, i & 07777));
		if (k < 0) return sc_err(c, errno);
		if (fstat(k, &st) == 0) um_setown(&st, um_euid, um_egid);
		return sc_ok(c, k);
	case 14:	/* mknod: the super-user only; the new name needs write permission on its directory */
		p = a_ptr(&a); i = a_int(&a); j = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (um_euid != 0) return sc_err(c, EPERM);
		hp = hostpath(gp, hb, sizeof hb);
		um_parent(hp, dir, sizeof dir);
		k = UM_RETRY(dir, S_IWUSR | S_IXUSR, (i & S_IFMT) == S_IFDIR ? mkdir(hp, i & 07777) : mknod(hp, i, j));
		if (k < 0) return sc_err(c, errno);
		if (stat(hp, &st) == 0) um_setown(&st, um_euid, um_egid);
		return sc_ok(c, 0);
	case 10:	/* unlink: write permission on the directory */
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		if (*dirmoved) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if (lstat(hp, &st) < 0) return sc_err(c, errno);
		um_parent(hp, dir, sizeof dir);
		if ((e = um_perm(dir, 2, um_euid, um_egid))) return sc_err(c, e);
		if (S_ISDIR(st.st_mode)) {
			const char *b = strrchr(hp, '/');
			b = b ? b + 1 : hp;
			if (um_euid != 0) return sc_err(c, EPERM);	/* a directory: the super-user only (sys2.c unlink) */
			if (!strcmp(b, ".") || !strcmp(b, "..")) return sc_ok(c, 0);
			k = UM_RETRY(dir, S_IWUSR | S_IXUSR, rmdir(hp));
		} else
			k = UM_RETRY(dir, S_IWUSR | S_IXUSR, unlink(hp));
		if (k < 0) return sc_err(c, errno);
		layer_whiteout(hp);
		if (st.st_nlink <= 1 || S_ISDIR(st.st_mode)) um_setown(&st, -1, -1);
		return sc_ok(c, 0);
	case 9:		/* link: write permission on the directory of the new name; a directory: the super-user only */
		p = a_ptr(&a); p2 = a_ptr(&a); gstr(c, p, gp, sizeof gp); gstr(c, p2, gp2, sizeof gp2);
		hp = hostpath(gp, hb, sizeof hb); hp2 = hostpath(gp2, hb2, sizeof hb2);
		if (stat(hp, &st) < 0) return sc_err(c, errno);
		if (S_ISDIR(st.st_mode) && um_euid != 0) return sc_err(c, EPERM);
		um_parent(hp2, dir, sizeof dir);
		if ((e = um_perm(dir, 2, um_euid, um_egid))) return sc_err(c, e);
		if (S_ISDIR(st.st_mode)) {	/* a directory link: a rename (as the normal code), both directories given */
			char dir1[1100];
			struct stat s2;
			if (lstat(hp2, &s2) == 0 || errno != ENOENT) return -2;
			um_parent(hp, dir1, sizeof dir1);
			k = UM_RETRY(dir, S_IWUSR | S_IXUSR, UM_RETRY(dir1, S_IWUSR | S_IXUSR, rename(hp, hp2)));
			if (k < 0) return sc_err(c, errno);
			snprintf(dirmoved, sizeof dirmoved, "%s", hp);
			return sc_ok(c, 0);
		}
		k = UM_RETRY(dir, S_IWUSR | S_IXUSR, link(hp, hp2));
		return k < 0 ? sc_err(c, errno) : sc_ok(c, 0);
	case 15:	/* chmod: the owner or the super-user; the sticky bit for the super-user only */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if ((e = um_isowner(hp, &st))) return sc_err(c, e);
		if (um_euid) i &= ~S_ISVTX;
		return chmod(hp, i & 07777) < 0 ? sc_err(c, errno) : sc_ok(c, 0);
	case 16:	/* chown: the owner or the super-user; set-id bits cleared unless the super-user */
		p = a_ptr(&a); i = a_int(&a); j = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if ((e = um_isowner(hp, &st))) return sc_err(c, e);
		um_setown(&st, (uint16_t)i, (uint16_t)j);
		if (um_euid && (st.st_mode & (S_ISUID | S_ISGID))) chmod(hp, st.st_mode & 07777 & ~(S_ISUID | S_ISGID));
		return sc_ok(c, 0);
	case 33:	/* access: with the real ids (sys4.c saccess) */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) != -2) return -2;
		hp = hostpath(gp, hb, sizeof hb);
		if (stat(hp, &st) < 0) return sc_err(c, errno);
		for (k = 4; k; k >>= 1)
			if ((i & k) && (e = um_perm(hp, k, um_ruid, um_rgid))) return sc_err(c, e);
		return sc_ok(c, 0);
	case 12:	/* chdir: search permission */
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		if (stat(hp, &st) < 0) return sc_err(c, errno);
		if (!S_ISDIR(st.st_mode)) return sc_err(c, ENOTDIR);
		if ((e = um_perm(hp, 1, um_euid, um_egid))) return sc_err(c, e);
		return -2;
	case 30:	/* utime: the owner or the super-user (or, for the time now, who may write) */
		p = a_ptr(&a); p2 = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		if ((e = um_isowner(hp, &st))) return sc_err(c, e);
		return -2;
	}
	return -2;
}

static int sys_call_impl(Z8K *c, unsigned n)
{
	Args a = { c, 0 };
	char gp[1024], hb[1100], gp2[1024], hb2[1100];
	const char *hp;
	Ad p, p2;
	int i, j, k;
	Dir *d;
	static unsigned char buf[65536];

	if (um_on && (k = um_call(c, n)) != -2)
		return k;
	switch (n) {
	case 0: {	/* sysiret: the return of a signal handler (machdep.c) */
		Ad sp;
		sp.seg = c->segmode ? (c->r[14] >> 8) & 0x7F : 0;
		sp.off = c->r[15];
		i = gw(c, sp);
		sp.off += 2; c->pc = gw(c, sp);
		sp.off += 2; c->fl = gw(c, sp) & 0xFC;
		if (c->segmode) c->pcseg = (i >> 8) & 0x7F;
		c->r[15] += 6;
		return 0; }
	case 1:		/* exit */
		i = a_int(&a);
		tmplog("exit %d", i & 0xFF);
		exit(i & 0xFF);
	case 2: {	/* fork */
		int ng = 0;
		pid_t pid;
		if (piddir()) { gpid(getpid()); ng = pid_child(my_g, ++fork_k); }	/* the parent has its number, then the child's */
		pid = fork();
		if (pid < 0) return sc_err(c, errno);
		if (pid == 0) {	/* the child: nothing pending (a new process) */
			if (ng) { my_g = ng; fork_k = 0; pid_set(getpid(), ng); }
			for (i = 1; i < 20; i++) sig_pend[i] = 0;
			sig_any = 0;
			memset(kids, 0, sizeof kids);
			return sc_ok(c, gpid(getppid())); }	/* child runs the following jr */
		c->pc += 2;						/* parent skips it */
		if (ng) { pid_set(pid, ng); pidlog("fork %d by %d", ng, gpid(getpid())); }
		kidadd(pid);
		tmplog("fork child %d guest %u", (int)pid, (unsigned)gpid(pid));
		return sc_ok(c, ng ? ng : gpid(pid)); }
	case 3:		/* read */
		i = a_int(&a); p = a_ptr(&a); j = a_int(&a);
		if (j > (int)sizeof buf) j = sizeof buf;
		if ((d = dirof(i)) != NULL) {	/* a directory: from its WEGA image */
			k = d->len - d->pos < j ? d->len - d->pos : j;
			if (k < 0) k = 0;
			memcpy(buf, d->buf + d->pos, k);
			d->pos += k;
		} else
			k = read(conhost(i, 0), buf, j);
		if (k < 0) return sc_err(c, errno);
		gwrite(c, p, buf, k);
		return sc_ok(c, k);
	case 4:		/* write */
		i = a_int(&a); p = a_ptr(&a); j = a_int(&a);
		if (j > (int)sizeof buf) j = sizeof buf;
		gread(c, p, buf, j);
		k = write(conhost(i, 1), buf, j);
		return sc_res(c, k);
	case 5:		/* open */
		p = a_ptr(&a); i = a_int(&a);
		gstr(c, p, gp, sizeof gp);
		if ((k = devlookup(gp)) != -2) {	/* the guest /dev */
			if (k == -1) return sc_err(c, ENOENT);
			k = devopen(k, i);
			return k < 0 ? sc_err(c, -k) : sc_ok(c, k);
		}
		hp = hostpath(gp, hb, sizeof hb);
		if (i & 0400) {		/* FCREAT, FEXCL, FTRUNC of the kernel's open (sys2.c copen); the mode is the third argument */
			int cm = a_int(&a);
			k = open(hp, open_acc(i) | O_CREAT | ((i & 02000) ? O_EXCL : 0) | ((i & 01000) ? O_TRUNC : 0), cm & 0777);
		} else
			k = open(hp, open_acc(i) | ((i & 01000) ? O_TRUNC : 0));
		if (istmp(gp)) tmplog("open %s mode %d -> %d", gp, i & 3, k < 0 ? -errno : k);
		if (k < 0) return sc_err(c, errno);
		dirload(k, hp);
		return sc_ok(c, k);
	case 8:		/* creat */
		p = a_ptr(&a); i = a_int(&a);
		gstr(c, p, gp, sizeof gp);
		if ((k = devlookup(gp)) != -2) {	/* a device is opened for writing; no new name in /dev */
			if (k == -1) return sc_err(c, EACCES);
			k = devopen(k, 1);
			return k < 0 ? sc_err(c, -k) : sc_ok(c, k);
		}
		hp = hostpath(gp, hb, sizeof hb);
		k = open(hp, O_WRONLY | O_CREAT | O_TRUNC, i & 07777);
		tmplog("creat %s -> %d", gp, k < 0 ? -errno : k);
		return sc_res(c, k);
	case 6: {	/* close */
		Dir **dp;
		i = a_int(&a);
		for (dp = &dirs; *dp; dp = &(*dp)->next)
			if ((*dp)->fd == i) {
				d = *dp;
				*dp = d->next;
				free(d->buf);
				free(d);
				break;
			}
		devclose(i);
		if (i >= 0 && i <= 2) stdcon[i] = 0;
		return sc_res(c, close(i)); }
	case 7: {	/* wait */
		int st; pid_t pid = wait(&st);
		if (pid < 0) return sc_err(c, errno);
		kiddel(pid);
		return sc_ok2(c, gpid(pid), wstat(st) & 0xFFFF); }
	case 9:		/* link */
		p = a_ptr(&a); p2 = a_ptr(&a);
		gstr(c, p, gp, sizeof gp); gstr(c, p2, gp2, sizeof gp2);
		{
			const char *h1 = hostpath(gp, hb, sizeof hb), *h2 = hostpath(gp2, hb2, sizeof hb2);
			struct stat sd;
			int r;
			/* a link to a directory (mv dir1 dir2 as root: link, then unlink of the old name): the host cannot link a
			   directory, it is renamed, and the unlink of the old name that follows is taken as done (dirmoved) */
			if (lstat(h1, &sd) == 0 && S_ISDIR(sd.st_mode) && lstat(h2, &sd) < 0 && errno == ENOENT) {
				if (rename(h1, h2) < 0)
					return sc_err(c, errno);
				snprintf(dirmoved, sizeof dirmoved, "%s", h1);
				return sc_ok(c, 0);
			}
			r = link(h1, h2);
			if (istmp(gp) || istmp(gp2)) tmplog("link %s %s -> %d", gp, gp2, r < 0 ? -errno : 0);
#ifdef __APPLE__
			/* macOS refuses a link to a directory (EPERM) before it finds the name taken */
			if (r < 0 && (errno == EEXIST || errno == EPERM)) {
#else
			if (r < 0 && errno == EEXIST) {
#endif
				struct stat s1, s2;
				/* Linux mkdir has already created the WEGA mkdir "."/".." links. */
				if (stat(h1, &s1) == 0 && stat(h2, &s2) == 0 &&
				    S_ISDIR(s1.st_mode) && s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino)
					return sc_ok(c, 0);
			}
			return sc_res(c, r);
		}
	case 10:	/* unlink */
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		{
			struct stat st;
			const char *base = strrchr(hp, '/');
			base = base ? base + 1 : hp;
			if (*dirmoved && !strcmp(hp, dirmoved)) {	/* the old name of a directory link (case 9) */
				*dirmoved = 0;
				return sc_ok(c, 0);
			}
			if (lstat(hp, &st) == 0 && S_ISDIR(st.st_mode)) {
				/* WEGA rmdir unlinks dir/.. and dir/. first; the host removes them with the directory */
				if (!strcmp(base, ".") || !strcmp(base, ".."))
					return sc_ok(c, 0);
				k = rmdir(hp);
				if (k == 0) layer_whiteout(hp);
				return sc_res(c, k);
			}
			k = unlink(hp);
			tmplog("unlink %s -> %d", gp, k < 0 ? -errno : 0);
			if (k == 0) layer_whiteout(hp);
			return sc_res(c, k);
		}
	case 11: {	/* exec */
		Ad pa, pv;
		pa = a_ptr(&a); pv = a_ptr(&a);
		return do_exec(c, pa, pv, 0, pv); }
	case 59: {	/* exece */
		Ad pa, pv, pe;
		pa = a_ptr(&a); pv = a_ptr(&a); pe = a_ptr(&a);
		return do_exec(c, pa, pv, 1, pe); }
	case 12: {	/* chdir */
		int r;
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		r = chdir(hostpath(gp, hb, sizeof hb));
		if (r == 0 && ((getenv("Z8K_OVERLAY") && *getenv("Z8K_OVERLAY")) || devtab_active())) {	/* the guest's idea of the directory (see gcwd) */
			char abs[sizeof gcwd + sizeof gp];
			if (gp[0] == '/') gcwd_normalize(gcwd, sizeof gcwd, gp);
			else if (gcwd[0]) { snprintf(abs, sizeof abs, "%s/%s", gcwd, gp); gcwd_normalize(gcwd, sizeof gcwd, abs); }
		}
		return sc_res(c, r); }
	case 13: {	/* time; Z8K_TIME=seconds fixes it (reproducible builds: ver.c is made by chkout from $D$) */
		const char *ft = getenv("Z8K_TIME");
		return sc_long(c, ft ? strtol(ft, NULL, 0) : (long)time(NULL)); }
	case 14:	/* mknod (mkdir uses this for directories) */
		p = a_ptr(&a); i = a_int(&a); j = a_int(&a); gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		return sc_res(c, (i & S_IFMT) == S_IFDIR
			? mkdir(hp, i & 07777)
			: mknod(hp, i, j));
	case 15:	/* chmod (of a device of the guest /dev: taken as done) */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if (devlookup(gp) >= 0) return sc_ok(c, 0);
		return sc_res(c, chmod(hostpath(gp, hb, sizeof hb), i & 07777));
	case 16:	/* chown: ignored */
		return sc_ok(c, 0);
	case 17:	/* break */
		if (c->segmode) {
			/* argument is a segmented address (seg word, offset); a zero
			 * segment word asks for the main data segment (see break.c) */
			uint16_t sw = a_int(&a), off = a_int(&a);
			if (!(sw & 0x8000) && sw == 0) return sc_ok2(c, mainseg << 8, segsz[mainseg]);
			{
				unsigned sno = (sw >> 8) & 0x7F, ncl = (off + 255) >> 8;
				if (!segused[sno]) return sc_err(c, ENOMEM);
				if (ncl * 256 > segsz[sno]) segsz[sno] = ncl * 256;
				return sc_ok(c, 0);
			}
		}
		i = a_int(&a);
		/* as the kernel: the data must end below the 256-byte click the stack occupies */
		if (i != 0 && (uint16_t)i > (c->r[15] & 0xFF00)) return sc_err(c, ENOMEM);
		if (i != 0) brkend = (uint16_t)i;
		return sc_ok(c, 0);
	case 18:	/* stat */
		{ struct stat st; p = a_ptr(&a); p2 = a_ptr(&a);
		  gstr(c, p, gp, sizeof gp);
		  if ((k = devlookup(gp)) != -2) {
			if (k == -1) return sc_err(c, ENOENT);
			dev_stat(k, &st); put_stat(c, p2, &st); return sc_ok(c, 0);
		  }
		  hp = hostpath(gp, hb, sizeof hb);
		  if (stat(hp, &st) < 0) return sc_err(c, errno);
		  if (S_ISDIR(st.st_mode)) st.st_size = dirlen(hp);
		  rootfix(hp, &st);
		  put_stat(c, p2, &st); return sc_ok(c, 0); }
	case 28:	/* fstat */
		{ struct stat st; i = a_int(&a); p = a_ptr(&a);
		  if ((k = devfd(i)) >= 0) { dev_stat(k, &st); put_stat(c, p, &st); return sc_ok(c, 0); }
		  if (fstat(i, &st) < 0) return sc_err(c, errno);
		  if (S_ISDIR(st.st_mode) && (d = dirof(i)) != NULL) st.st_size = d->len;
		  rootfix(fdpath(i, hb, sizeof hb), &st);
		  put_stat(c, p, &st); return sc_ok(c, 0); }
	case 19: {	/* lseek */
		int fd = a_int(&a); int32_t off = a_long(&a); int wh = a_int(&a);
		off_t r;
		if ((d = dirof(fd)) != NULL) {	/* a directory: the position in its WEGA image */
			r = (wh == 0 ? 0 : wh == 1 ? d->pos : d->len) + off;
			if (r < 0) { errno = EINVAL; r = -1; } else d->pos = r;
		} else
			r = lseek(fd, off, wh);
		if (r < 0) return sc_err(c, errno);
		return sc_long(c, r); }
	case 38: {	/* fcntl */
		int fd  = a_int(&a);
		int cmd = a_int(&a);
		int arg = a_int(&a);

		int r = fcntl(fd, cmd, arg);

		if (r < 0)
			return sc_err(c, errno);

		return sc_long(c, r); }
	case 20: {	/* getpid: r4 the pid, r5 the parent's (getppid of the libc); a parent that has no number yet is process 1 */
		int me = gpid(getpid()), par = 1;
		tmplog("getpid guest %u", (unsigned)me);
		if (piddir()) {
			char f[1100], b[16];
			int fd;
			ssize_t n;
			snprintf(f, sizeof f, "%s/h%ld", piddir(), (long)getppid());
			if ((fd = open(f, O_RDONLY)) >= 0) { n = read(fd, b, sizeof b - 1); close(fd); if (n > 0) { b[n] = 0; par = atoi(b); } }
		} else
			par = gpid(getppid());
		return sc_ok2(c, me, par); }
	case 21: {	/* mount(spec, dir, ronly) (sys3.c smount: getmdev, then EBUSY for a device or directory in use) */
		struct stat st;
		pid_t pid;
		int w;
		char hd[1200], img[1100];
		p = a_ptr(&a); p2 = a_ptr(&a); gstr(c, p, gp, sizeof gp); gstr(c, p2, gp2, sizeof gp2);
		mnt_load();
		if (!*mntf) return sc_err(c, EBUSY);
		if ((k = devlookup(gp)) < 0) return sc_err(c, ENOENT);
		if (devent[k].type != 'b') return sc_err(c, ENOTBLK);
		if (strncmp(devent[k].model, "image:", 6)) return sc_err(c, ENXIO);
		hp = hostpath(gp2, hb, sizeof hb);
		if (stat(hp, &st) < 0) return sc_err(c, errno);
		for (i = 0; i < nmnt; i++)
			if (mnts[i].ent == k || !strcmp(mnts[i].g, gp2)) return sc_err(c, EBUSY);
		if (nmnt >= 16) return sc_err(c, EBUSY);
		snprintf(hd, sizeof hd, "%s.%d", mntf, k);
		snprintf(img, sizeof img, "%s", devent[k].model + 6);
		if ((pid = fork()) == 0) {	/* the file system unpacked (wegafs.py; an old one removed first) */
			const char *t = getenv("Z8K_WEGAFS");
			execlp("sh", "sh", "-c", "rm -rf \"$1\" \"$1.owners\" && exec python3 \"$2\" extract \"$3\" \"$1\"", "sh", hd,
			       t && *t ? t : "wegafs.py", img, (char *)0);
			_exit(127);
		}
		if (pid < 0 || waitpid(pid, &w, 0) < 0 || !WIFEXITED(w) || WEXITSTATUS(w)) return sc_err(c, EINVAL);
		if (um_on) {	/* the owners of the files: from the file system */
			char ol[1300], hp2[2300];
			FILE *f;
			unsigned uid, gid, mode;
			snprintf(ol, sizeof ol, "%s.owners", hd);
			if ((f = fopen(ol, "r")) != NULL) {
				while (fscanf(f, "%u %u %o %1023s", &uid, &gid, &mode, gp) == 4) {
					snprintf(hp2, sizeof hp2, "%s%s", hd, strcmp(gp, "/") ? gp : "");
					if (lstat(hp2, &st) == 0) um_setown(&st, uid, gid);
				}
				fclose(f);
			}
		}
		mnts[nmnt].ent = k;
		snprintf(mnts[nmnt].g, sizeof mnts[nmnt].g, "%s", gp2);
		snprintf(mnts[nmnt].h, sizeof mnts[nmnt].h, "%s", hd);
		nmnt++;
		mnt_save();
		return sc_ok(c, 0); }
	case 22: {	/* umount(spec): EINVAL when the device is not mounted; the unpacked tree is removed */
		pid_t pid;
		int w;
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		mnt_load();
		if ((k = devlookup(gp)) < 0) return sc_err(c, ENOENT);
		if (devent[k].type != 'b') return sc_err(c, ENOTBLK);
		for (i = 0; i < nmnt && mnts[i].ent != k; i++) ;
		if (i == nmnt) return sc_err(c, EINVAL);
		if ((pid = fork()) == 0) {
			execlp("sh", "sh", "-c", "chmod -R u+rwx \"$1\" 2>/dev/null; rm -rf \"$1\" \"$1.owners\"", "sh", mnts[i].h, (char *)0);
			_exit(127);
		}
		if (pid > 0) waitpid(pid, &w, 0);
		mnts[i] = mnts[--nmnt];
		mnt_save();
		return sc_ok(c, 0); }
	case 23: case 46: return sc_ok(c, 0);
	case 24: return sc_ok2(c, getuid(), geteuid());
	case 47: return sc_ok2(c, getgid(), getegid());
	case 27: i = a_int(&a); return sc_ok(c, alarm(i));
	case 29: pause(); return sc_err(c, EINTR);
	case 30: {	/* utime */
		struct utimbuf ub; uint8_t b[8];
		p = a_ptr(&a); p2 = a_ptr(&a);
		gstr(c, p, gp, sizeof gp);
		gread(c, p2, b, 8);
		ub.actime = be32(b); ub.modtime = be32(b + 4);
		return sc_res(c, utime(hostpath(gp, hb, sizeof hb), &ub)); }
	case 33:	/* access */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
		if ((k = devlookup(gp)) != -2) {	/* the permission of "others" (the guest is not the owner of /dev) */
			if (k == -1) return sc_err(c, ENOENT);
			return ((i & 7) & ~(devent[k].mode & 7)) ? sc_err(c, EACCES) : sc_ok(c, 0);
		}
		return sc_res(c, access(hostpath(gp, hb, sizeof hb), i & 7));
	case 34: return sc_ok(c, 0);
	case 36: sync(); return sc_ok(c, 0);
	case 37: {	/* kill: the guest pid back to the host pid */
		pid_t hp;
		i = a_int(&a); j = a_int(&a);
		if ((hp = hostpid((int16_t)i)) == -1) {
			/* Z8K_KILLLOG=FILE: the processes of the process table model (the kmem image of ps) are no host processes: a signal to a
			   number that no guest process has is written to FILE ("PID SIG") and granted; kill with the signal 0 only asks */
			const char *kl = getenv("Z8K_KILLLOG");
			if (kl && *kl && (int16_t)i > 0 && j >= 0 && j < 20) {
				if (j) {
					int fd = open(kl, O_WRONLY | O_CREAT | O_APPEND, 0600);
					char ln[40];
					int n = snprintf(ln, sizeof ln, "%d %d\n", (int)(int16_t)i, j);
					if (fd >= 0) { if (write(fd, ln, n) < 0) {} close(fd); }
				}
				return sc_ok(c, 0);
			}
			return sc_err(c, ESRCH);
		}
		if (j < 0 || j >= 20) return sc_err(c, EINVAL);
		return sc_res(c, kill(hp, j ? sig_host(j) : 0)); }
	case 41:	/* dup */
		i = a_int(&a); j = a_int(&a);
		if (i & 0x40) {
			int src = i & 0x3F, con = devfd(src) >= 0 && !devof[src] && devfd(src) == console_ent();
			devclose(j);
			k = dup2(src, j);
			if (k >= 0 && devof[src]) devof[k] = devof[src];	/* a copy: only the opener ends a proc model */
			else if (k > 2 && k < 256 && con) devof[k] = console_ent() + 1;	/* a copy of fd 0, 1, 2 as the console */
			if (k >= 0 && k <= 2) stdcon[k] = con || (devof[src] && console_on() && devof[src] - 1 == console_ent());
			return sc_res(c, k);
		}
		k = dup(i);
		if (k >= 0 && k < 256 && i >= 0 && i < 256 && devof[i]) devof[k] = devof[i];
		else if (k > 2 && k < 256 && i >= 0 && i <= 2 && devfd(i) >= 0 && devfd(i) == console_ent()) devof[k] = console_ent() + 1;
		else if (k >= 0 && k <= 2) stdcon[k] = 0;
		return sc_res(c, k);
	case 42: {	/* pipe */
		int fd[2];
		if (pipe(fd) < 0) return sc_err(c, errno);
		return sc_ok2(c, fd[0], fd[1]); }
	case 43: {	/* times */
		struct tms t; uint8_t b[16]; clock_t r;
		p = a_ptr(&a);
		r = times(&t);
		{ uint32_t v[4] = { t.tms_utime, t.tms_stime, t.tms_cutime, t.tms_cstime };
		  for (i = 0; i < 4; i++) { b[4*i] = v[i] >> 24; b[4*i+1] = v[i] >> 16; b[4*i+2] = v[i] >> 8; b[4*i+3] = v[i]; } }
		gwrite(c, p, b, 16);
		return sc_long(c, r); }
	case 57: {	/* utssys(buf, mv, type): type 0 is uname, the utsname of the WEGA kernel (src/uts/h/ver.h, conf/ver.c+: release 3.2);
		   ustat (type 2) is not modelled: EINVAL */
		static const char u[36] = "P8000\0\0\0\0" "WEGA\0\0\0\0\0" "3.2\0\0\0\0\0\0" " 3.1 4/5";
		p = a_ptr(&a); i = a_int(&a); j = a_int(&a);
		if (j != 0) return sc_err(c, EINVAL);
		gwrite(c, p, (const uint8_t *)u, 36);
		return sc_ok(c, 0); }
	case 55: {	/* mkseg(segaddr): create a segment of the given size, returns seg<<8 */
		unsigned sno = (a_int(&a) >> 8) & 0x7F, nb = a_int(&a);
		if (!sno) {
			for (sno = 4; sno < 0x7F && segused[sno]; sno++)
				;
			if (sno >= 0x7F) return sc_err(c, ENOMEM);
		} else if (segused[sno]) {
			return sc_err(c, EEXIST);
		}
		segused[sno] = 1;
		segsz[sno] = ((nb + 255) >> 8) << 8;
		return sc_ok2(c, sno << 8, 0); }
	case 45: {	/* ssgbrk(segno, nbytes): grow a segment, returns (seg<<8, old size) */
		unsigned sno = a_int(&a) & 0x7F, nb = a_int(&a), old, ncl = (nb + 255) >> 8;
		if (!segused[sno]) return sc_err(c, ENOMEM);
		old = segsz[sno];
		if (old + ncl * 256 > 0x10000) return sc_err(c, ENOMEM);
		segsz[sno] = old + ncl * 256;
		return sc_ok2(c, sno << 8, old & 0xFFFF); }
	case 48: {	/* signal(sig, handler): ssig (sys4.c); segmented: the handler is a long (segment word, offset) */
		int sig = a_int(&a); uint32_t h = c->segmode ? a_long(&a) : a_int(&a), old;
		if (sig <= 0 || sig >= 20 || sig == 9) return sc_err(c, EINVAL);
		old = sig_kernel[sig];
		sig_kernel[sig] = h;
		sig_pend[sig] = 0;
		sig_disp(sig, h);
		return c->segmode ? sc_ok2(c, old >> 16, old & 0xFFFF) : sc_ok(c, old & 0xFFFF); }
	case 54: {	/* ioctl: the tty ioctls of the console (Z8K_CONSOLE), else ENOTTY */
		int fd = a_int(&a); unsigned cmd = (uint16_t)a_int(&a);
		p = a_ptr(&a);
		k = devioctl(c, fd, cmd, p);
		return k < 0 ? sc_err(c, -k) : sc_ok(c, 0); }
	case 49: case 50:	/* lkdata, unlk (record locks, lock.c): taken as granted. The processes of a case do not compete for a region;
				   passwd locks /etc/passwd with them */
		return sc_ok(c, 0);
	case 60: i = a_int(&a); return sc_ok(c, umask(i));
	case 61: return sc_err(c, EPERM);
	default:
		fprintf(stderr, "z8k: unimplemented system call %u at %02x:%04x\n", n, c->cur_seg, c->cur_pc);
		return sc_err(c, EINVAL);
	}
}

/* -------------------------------------------------------------------- main */

/*
 * Plexus (SysIII for Z8002) system call ABI, see lib/libc/z8000/sys of the
 * Plexus sources: numbers as in syscall.h; `sc INDIR; .word blk` runs the call
 * `sc N` stored at blk with its arguments as the words following it; some calls
 * take their first argument in r0 (and dup a second one in r1); results come
 * back in r0/r1, carry set and errno in r0 on error.
 */
static int sys_call_plexus(Z8K *c, unsigned n)
{
	uint16_t save[8], arg[8];
	int i, k = 0, rc;
	Ad blk;
	memcpy(save, c->r, sizeof save);
	blk.seg = 0; blk.off = c->pc;
	if (n == 0) {			/* indirect: next word is the block address */
		{ uint8_t *im = z8k_block(c, 0, 0); blk.off = (im[c->pc] << 8) | im[(uint16_t)(c->pc + 1)]; }
		c->pc += 2;
		n = rdw_pl(c, blk) & 0xFF;
		blk.off += 2;
	} else {
		blk.off = 0;
	}
	switch (n) {
	case 1: case 3: case 4: case 6: case 19: case 23: case 27: case 28:
	case 34: case 37: case 46: case 60: case 62:
		arg[k++] = save[0]; break;
	case 41: arg[k++] = save[0]; arg[k++] = save[1]; break;
	}
	if (blk.off)
		for (; k < 8; k++) { arg[k] = rdw_pl(c, blk); blk.off += 2; }
	for (; k < 8; k++) arg[k] = 0;
	if (n == 62) {			/* ugrow: stack grows by itself here */
		c->r[0] = 0; c->fl &= ~F_C; return 0;
	}
	for (i = 0; i < 6; i++) c->r[i] = arg[i];
	rc = sys_call_impl(c, n);
	{
		uint16_t r4 = c->r[4], r5 = c->r[5];
		int err = (c->fl & F_C) != 0;
		memcpy(c->r, save, sizeof save);
		c->r[0] = r4;
		if (!err) c->r[1] = r5;
	}
	return rc;
}

/* optional system call trace: Z8K_SC_TRACE=1 */
static int sys_call(Z8K *c, unsigned n)
{
	static int tr = -1;
	static FILE *tf;
	int rc;
	if (tr < 0) {
		const char *p = getenv("Z8K_SC_TRACE");
		tr = p != NULL;
		if (tr) tf = fopen(p, "a");	/* a host file: the guest may close fd 2 */
		if (tr && !tf) tr = 0;
	}
	if (tr) fprintf(tf, "sc %u r0-3=%04x %04x %04x %04x", n, c->r[0], c->r[1], c->r[2], c->r[3]);
	if (tr) fflush(tf);
	rc = plexus ? sys_call_plexus(c, n) : sys_call_impl(c, n);
	if (tr) fprintf(tf, " -> r4=%04x r5=%04x%s\n", c->r[4], c->r[5], (c->fl & F_C) ? " ERR" : "");
	if (tr) fflush(tf);
	return rc;
}

int main(int argc, char **argv)
{
	static Z8K cpu;
	Z8K *c = &cpu;
	int i, err;
	Exec x;
	char hb[1100];
	const char *hp, *argv0 = NULL;

	envv = calloc(64, sizeof *envv);
	envv[nenv++] = "PATH=/bin:/usr/bin";
	envv[nenv++] = "HOME=/";
	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (!strcmp(argv[i], "-r") && i + 1 < argc) root = argv[++i];
		else if (!strcmp(argv[i], "-t")) c->trace = 1;
		else if (!strcmp(argv[i], "-T")) c->trace = 2;
		else if (!strcmp(argv[i], "-v")) c->verbose = 1;
		else if (!strcmp(argv[i], "-e") && i + 1 < argc && nenv < 62) {
			/* a name that is there already (PATH, HOME) is replaced: getenv of the guest takes the first */
			const char *e = argv[++i], *q = strchr(e, '=');
			int k, n = q ? (int)(q - e) : (int)strlen(e);
			for (k = 0; k < nenv; k++)
				if (!strncmp(envv[k], e, n) && envv[k][n] == '=') break;
			if (k < nenv) envv[k] = (char *)e;
			else envv[nenv++] = (char *)e;
		}
		else if (!strcmp(argv[i], "-0") && i + 1 < argc) argv0 = argv[++i];
		else if (!strcmp(argv[i], "--")) { i++; break; }
		else { fprintf(stderr, "usage: z8k-emu [-r ROOT] [-t|-T] [-v] [-e NAME=VALUE] [-0 ARGV0] PROGRAM [ARG...]\n"); return 2; }
	}
	if (i >= argc) { fprintf(stderr, "usage: z8k-emu [-r ROOT] [-t|-T] [-v] [-e NAME=VALUE] [-0 ARGV0] PROGRAM [ARG...]\n"); return 2; }
	if (*root) {
		char t[1100];
		if (*guesttmp())
			snprintf(t, sizeof t, "%s", guesttmp());
		else
			snprintf(t, sizeof t, "%s/tmp", root);
		mkdir(t, 0777);
	}
	{
		const char *f = getenv("Z8K_ROOTLOG");
		char cwd[1024];
		if (f && *f) {
			if (f[0] == '/' || !getcwd(cwd, sizeof cwd))
				snprintf(rootlogf, sizeof rootlogf, "%s", f);
			else
				snprintf(rootlogf, sizeof rootlogf, "%s/%s", cwd, f);
		}
	}
	c->syscall = sys_call;
	c->trap = sig_trap;
	sig_start();
	um_start();
	{
		char cwd[1024];
		const char *ov = getenv("Z8K_OVERLAY");
		char od[1100];
		struct stat ost;
		snprintf(od, sizeof od, "%s/tmp", ov ? ov : "");	/* the host directory of the guest /tmp (tmpclash.py: names in different ones do not clash) */
		tmplog("start %s root %s cwd %s tmp %s", argv[i], root, getcwd(cwd, sizeof cwd) ? cwd : "?",
		       ov && *ov && stat(od, &ost) == 0 && S_ISDIR(ost.st_mode) ? od : *guesttmp() ? guesttmp() : "-");
	}
	hp = hostpath(argv[i], hb, sizeof hb);
	err = exec_open(hp, &x);
	if (err) { fprintf(stderr, "z8k-emu: %s: %s\n", argv[i], strerror(err)); return 126; }
	if (argv0) argv[i] = (char *)argv0;	/* -0: the guest's argv[0] (as a shell would give it), not the path */
	um_exec(hp);
	exec_map(c, &x, argv + i, envv);
	free(x.file);
	for (;;) {
		int r = z8k_step(c);
		if (brkend && !c->segmode && c->r[15] < brkend) {	/* the stack grew into the data */
			fprintf(stderr, "Memory fault\n");
			return 128 + SIGSEGV;
		}
		if (r) { z8k_dump(c, stderr); return 255; }
		if (c->halted) return 0;
		if (sig_any) sig_deliver(c);
	}
}
