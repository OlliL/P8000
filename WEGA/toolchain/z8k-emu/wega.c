/*
 * wega.c - s.out loader and WEGA system call emulation on top of z8k.c.
 *
 * Usage: z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]
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
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include "z8k.h"

typedef struct { uint8_t seg; uint16_t off; } Ad;

static const char *root = "";
static char **envv;			/* guest default environment */
static int nenv;
static uint32_t sig_kernel[32];		/* kernel-level handler values per signal */
static uint32_t segsz[128];		/* current size in bytes of each program segment (segmented mode) */
static int mainseg;			/* the segment break() reports (last non-stack data segment) */

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
	int magic, segmented, sep, flags, nseg;
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
	if (n < 24) { free(x->file); return ENOEXEC; }
	x->magic = be16(x->file);
	switch (x->magic) {
	case 0xE607: x->segmented = 1; x->sep = 0; break;
	case 0xE611: x->segmented = 1; x->sep = 1; break;
	case 0xE707: x->segmented = 0; x->sep = 0; break;
	case 0xE711: x->segmented = 0; x->sep = 1; break;
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
	int i, stkseg = 0x7F;
	clear_mem(c);
	memset(segsz, 0, sizeof segsz);
	memset(c->r, 0, sizeof c->r);
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
		if (x->sg[i].data) memcpy(dm + dataoff, x->file + doff, x->sg[i].data);
		doff += x->sg[i].data;
		(void)SG_CODE; (void)SG_DATA; (void)SG_BSS;
		if (x->sg[i].atr & SG_STACK) stkseg = no;
		{
			uint32_t ext = (uint32_t)dataoff + x->sg[i].data + x->sg[i].bssz;
			uint32_t cex = x->sg[i].coff * 256 + x->sg[i].code;
			if (cex > ext) ext = cex;
			ext = (ext + 255) & ~255u;
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

static const char *hostpath(const char *g, char *buf, size_t n)
{
	if (g[0] != '/' || !*root) return g;
	if (!strncmp(g, "/dev/", 5)) return g;
	snprintf(buf, n, "%s%s", root, g);
	return buf;
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

static int sc_ok(Z8K *c, uint32_t v) { c->r[4] = v; c->fl &= ~F_C; return 0; }
static int sc_ok2(Z8K *c, uint16_t a, uint16_t b) { c->r[4] = a; c->r[5] = b; c->fl &= ~F_C; return 0; }
static int sc_long(Z8K *c, uint32_t v) { c->r[4] = v >> 16; c->r[5] = v; c->fl &= ~F_C; return 0; }
static int sc_err(Z8K *c, int e) { c->r[4] = e; c->r[5] = 0; c->fl |= F_C; return 0; }
static int sc_res(Z8K *c, long v) { return v < 0 ? sc_err(c, errno) : sc_ok(c, v); }

static void put_stat(Z8K *c, Ad a, const struct stat *st)
{
	gputw(c, a, st->st_dev); a.off += 2;
	gputw(c, a, st->st_ino); a.off += 2;
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
	if (access(hp, X_OK) < 0) return sc_err(c, errno);
	e = exec_open(hp, &x);
	if (e) return sc_err(c, e);
	/* copy argument and environment strings out of the old image */
	av = gvec(c, argvp, &na);
	ev = haveenv ? gvec(c, envp, &ne) : envv;
	exec_map(c, &x, av, ev);
	free(x.file);
	for (i = 0; i < na; i++) free(av[i]);
	free(av);
	if (haveenv) { for (i = 0; i < ne; i++) free(ev[i]); free(ev); }
	return 0;
}

static int sys_call(Z8K *c, unsigned n)
{
	Args a = { c, 0 };
	char gp[1024], hb[1100], gp2[1024], hb2[1100];
	const char *hp;
	Ad p, p2;
	int i, j, k;
	static unsigned char buf[65536];

	switch (n) {
	case 1:		/* exit */
		i = a_int(&a);
		exit(i & 0xFF);
	case 2: {	/* fork */
		pid_t pid = fork();
		if (pid < 0) return sc_err(c, errno);
		if (pid == 0) { c->pc += 0; return sc_ok(c, getppid()); }	/* child runs the following jr */
		c->pc += 2;						/* parent skips it */
		return sc_ok(c, pid); }
	case 3:		/* read */
		i = a_int(&a); p = a_ptr(&a); j = a_int(&a);
		if (j > (int)sizeof buf) j = sizeof buf;
		k = read(i, buf, j);
		if (k < 0) return sc_err(c, errno);
		gwrite(c, p, buf, k);
		return sc_ok(c, k);
	case 4:		/* write */
		i = a_int(&a); p = a_ptr(&a); j = a_int(&a);
		if (j > (int)sizeof buf) j = sizeof buf;
		gread(c, p, buf, j);
		k = write(i, buf, j);
		return sc_res(c, k);
	case 5:		/* open */
		p = a_ptr(&a); i = a_int(&a);
		gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		return sc_res(c, open(hp, i & 3));
	case 8:		/* creat */
		p = a_ptr(&a); i = a_int(&a);
		gstr(c, p, gp, sizeof gp);
		hp = hostpath(gp, hb, sizeof hb);
		return sc_res(c, open(hp, O_WRONLY | O_CREAT | O_TRUNC, i & 07777));
	case 6: i = a_int(&a); return sc_res(c, close(i));
	case 7: {	/* wait */
		int st; pid_t pid = wait(&st);
		if (pid < 0) return sc_err(c, errno);
		return sc_ok2(c, pid, st & 0xFFFF); }
	case 9:		/* link */
		p = a_ptr(&a); p2 = a_ptr(&a);
		gstr(c, p, gp, sizeof gp); gstr(c, p2, gp2, sizeof gp2);
		{ const char *h1 = hostpath(gp, hb, sizeof hb), *h2 = hostpath(gp2, hb2, sizeof hb2);
		  return sc_res(c, link(h1, h2)); }
	case 10:	/* unlink */
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		return sc_res(c, unlink(hostpath(gp, hb, sizeof hb)));
	case 11: {	/* exec */
		Ad pa, pv;
		pa = a_ptr(&a); pv = a_ptr(&a);
		return do_exec(c, pa, pv, 0, pv); }
	case 59: {	/* exece */
		Ad pa, pv, pe;
		pa = a_ptr(&a); pv = a_ptr(&a); pe = a_ptr(&a);
		return do_exec(c, pa, pv, 1, pe); }
	case 12:	/* chdir */
		p = a_ptr(&a); gstr(c, p, gp, sizeof gp);
		return sc_res(c, chdir(hostpath(gp, hb, sizeof hb)));
	case 13: return sc_long(c, time(NULL));
	case 15:	/* chmod */
		p = a_ptr(&a); i = a_int(&a); gstr(c, p, gp, sizeof gp);
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
				if (!segsz[sno]) return sc_err(c, ENOMEM);
				if (ncl * 256 > segsz[sno]) segsz[sno] = ncl * 256;
				return sc_ok(c, 0);
			}
		}
		i = a_int(&a);
		if (i != 0 && (uint16_t)i >= c->r[15]) return sc_err(c, ENOMEM);
		return sc_ok(c, 0);
	case 18:	/* stat */
		{ struct stat st; p = a_ptr(&a); p2 = a_ptr(&a);
		  gstr(c, p, gp, sizeof gp);
		  if (stat(hostpath(gp, hb, sizeof hb), &st) < 0) return sc_err(c, errno);
		  put_stat(c, p2, &st); return sc_ok(c, 0); }
	case 28:	/* fstat */
		{ struct stat st; i = a_int(&a); p = a_ptr(&a);
		  if (fstat(i, &st) < 0) return sc_err(c, errno);
		  put_stat(c, p, &st); return sc_ok(c, 0); }
	case 19: {	/* lseek */
		int fd = a_int(&a); int32_t off = a_long(&a); int wh = a_int(&a);
		off_t r = lseek(fd, off, wh);
		if (r < 0) return sc_err(c, errno);
		return sc_long(c, r); }
	case 20: return sc_ok(c, getpid());
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
		return sc_res(c, access(hostpath(gp, hb, sizeof hb), i & 7));
	case 34: return sc_ok(c, 0);
	case 36: sync(); return sc_ok(c, 0);
	case 37: i = a_int(&a); j = a_int(&a); return sc_res(c, kill(i, j));
	case 41:	/* dup */
		i = a_int(&a); j = a_int(&a);
		if (i & 0x40) return sc_res(c, dup2(i & 0x3F, j));
		return sc_res(c, dup(i));
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
	case 45: {	/* ssgbrk(segno, nbytes): grow a segment, returns (seg<<8, old size) */
		unsigned sno = a_int(&a) & 0x7F, nb = a_int(&a), old, ncl = (nb + 255) >> 8;
		if (!segsz[sno]) return sc_err(c, ENOMEM);
		old = segsz[sno];
		if (old + ncl * 256 > 0x10000) return sc_err(c, ENOMEM);
		segsz[sno] = old + ncl * 256;
		return sc_ok2(c, sno << 8, old & 0xFFFF); }
	case 48: {	/* signal(sig, kernel handler) */
		int sig = a_int(&a); uint32_t h = a_int(&a), old;
		if (c->segmode) h = 0;
		if (sig <= 0 || sig >= 20 || sig == SIGKILL) return sc_err(c, EINVAL);
		old = sig_kernel[sig];
		sig_kernel[sig] = h;
		signal(sig, h == 1 ? SIG_IGN : SIG_DFL);
		return sc_ok(c, old); }
	case 54:	/* ioctl */
		return sc_err(c, ENOTTY);
	case 60: i = a_int(&a); return sc_ok(c, umask(i));
	case 61: return sc_err(c, EPERM);
	default:
		fprintf(stderr, "z8k: unimplemented system call %u at %02x:%04x\n", n, c->cur_seg, c->cur_pc);
		return sc_err(c, EINVAL);
	}
}

/* -------------------------------------------------------------------- main */

int main(int argc, char **argv)
{
	static Z8K cpu;
	Z8K *c = &cpu;
	int i, err;
	Exec x;
	char hb[1100];
	const char *hp;

	envv = calloc(64, sizeof *envv);
	envv[nenv++] = "PATH=/bin:/usr/bin";
	envv[nenv++] = "HOME=/";
	for (i = 1; i < argc && argv[i][0] == '-'; i++) {
		if (!strcmp(argv[i], "-r") && i + 1 < argc) root = argv[++i];
		else if (!strcmp(argv[i], "-t")) c->trace = 1;
		else if (!strcmp(argv[i], "-v")) c->verbose = 1;
		else if (!strcmp(argv[i], "-e") && i + 1 < argc && nenv < 62) envv[nenv++] = argv[++i];
		else if (!strcmp(argv[i], "--")) { i++; break; }
		else { fprintf(stderr, "usage: z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]\n"); return 2; }
	}
	if (i >= argc) { fprintf(stderr, "usage: z8k-emu [-r ROOT] [-t] [-v] [-e NAME=VALUE] PROGRAM [ARG...]\n"); return 2; }
	if (*root) {
		char t[1100];
		snprintf(t, sizeof t, "%s/tmp", root);
		mkdir(t, 0777);
	}
	c->syscall = sys_call;
	hp = hostpath(argv[i], hb, sizeof hb);
	err = exec_open(hp, &x);
	if (err) { fprintf(stderr, "z8k-emu: %s: %s\n", argv[i], strerror(err)); return 126; }
	exec_map(c, &x, argv + i, envv);
	free(x.file);
	for (;;) {
		int r = z8k_step(c);
		if (r) { z8k_dump(c, stderr); return 255; }
		if (c->halted) return 0;
	}
}
