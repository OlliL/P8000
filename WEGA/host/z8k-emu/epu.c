/*
 * epu.c - Z8070 EPU (floating point) instructions.
 *
 * WEGA has no EPU: an EPU instruction traps, and the kernel runs its
 * floating point emulator fpe.s (src/uts/conf/fpe.s, entry _fpepu) with
 * the saved CPU state and u.u_fpstate.  This file does the same.  fpe.s is
 * translated instruction by instruction into fpe_gen.c (by fpe2c.py) and
 * runs on a second Z8K context, the "kernel": same instruction semantics as
 * the CPU core, its own registers, one 64K kernel data segment holding
 * struct state, u_fpstate and the kernel stack.  The memory routines that
 * fpe.s takes from mch.s (gettext, getmem, putmem) are implemented here.
 */
#include <stdlib.h>
#include <string.h>
#include "z8k_int.h"

/* kernel data segment layout (segment 0 of the kernel context) */
#define K_STATE	0x0100		/* struct state (sys/state.h) */
#define K_FPS	0x0200		/* u.u_fpstate (sys/user.h) */
#define K_STACK	0xFF00		/* initial kernel stack pointer */
#define S_REG	0x00		/* s_reg[15], s_sp */
#define S_EVENT	0x20		/* s_eventid: first word of the EPU instruction */
#define S_PS	0x22		/* s_ps: FCW */
#define S_PCSEG	0x24
#define S_PC	0x26
#define STEP_LIMIT 10000000L

static Z8K *kctx;
static long steps;

static void fatal(const char *msg, unsigned v)
{
	fprintf(stderr, "z8k: EPU emulation: %s %x\n", msg, v);
	exit(255);
}

/* addressing helpers for the translated code */
static inline Ad BA(Z8K *k, int n, uint16_t d) { Ad a = ir_ad(k, n); a.off += d; return a; }
static inline Ad BX(Z8K *k, int n, int x) { Ad a = ir_ad(k, n); a.off += k->r[x]; return a; }

static uint32_t tab_(const uint32_t *t, unsigned n, uint16_t idx)
{
	if ((idx & 3) || idx / 4 >= n) fatal("table index out of range", idx);
	return t[idx / 4];
}
#define TAB(t, x) tab_(t, sizeof t / sizeof t[0], k->r[x])

static inline void push_(Z8K *k, int n, int sz, uint32_t v)
{
	*offreg(k, n) -= sz;
	mwr(k, ir_ad(k, n), sz, v);
}
static inline uint32_t pop_(Z8K *k, int n, int sz)
{
	uint32_t v = mrd(k, ir_ad(k, n), sz);
	*offreg(k, n) += sz;
	return v;
}

enum { O_ADD, O_SUB, O_ADC, O_SBC, O_AND, O_OR, O_XOR, O_CP };

static void alu(Z8K *k, int op, int sz, int n, uint32_t b)
{
	uint32_t a = rreg(k, n, sz), r = 0;
	int cy = !!(k->fl & F_C);
	switch (op) {
	case O_ADD: r = addsub(k, a, b, 0, sz, 0); break;
	case O_SUB: r = addsub(k, a, b, 0, sz, 1); break;
	case O_ADC: r = addsub(k, a, b, cy, sz, 0); break;
	case O_SBC: r = addsub(k, a, b, cy, sz, 1); break;
	case O_AND: r = a & b; logf_(k, r, sz); break;
	case O_OR:  r = a | b; logf_(k, r, sz); break;
	case O_XOR: r = a ^ b; logf_(k, r, sz); break;
	case O_CP:  addsub(k, a, b, 0, sz, 1); return;
	}
	wreg(k, n, sz, r);
}

static uint32_t incdec(Z8K *k, uint32_t a, int sz, uint32_t n, int dec)
{
	uint32_t r = dec ? (a - n) & MSK[sz] : (a + n) & MSK[sz], m = msb(sz);
	zs(k, r, sz);
	sf(k, F_V, (dec ? ((a ^ n) & (a ^ r) & m) : (~(a ^ n) & (a ^ r) & m)) ? F_V : 0);
	return r;
}

static void ldir_(Z8K *k, int d, int s, int n)
{
	do {
		mwr(k, ir_ad(k, d), 2, mrd(k, ir_ad(k, s), 2));
		*offreg(k, d) += 2; *offreg(k, s) += 2;
		k->r[n]--;
	} while (k->r[n] != 0);
	sf(k, F_V, F_V);
}

static void multl_(Z8K *k, int n, uint32_t b)
{
	int64_t p = (int64_t)(int32_t)R32(k, n + 2) * (int32_t)b;
	k->r[n] = p >> 48; k->r[n + 1] = p >> 32; k->r[n + 2] = p >> 16; k->r[n + 3] = p;
	sf(k, F_C | F_V, (p != (int64_t)(int32_t)p) ? F_C : 0);
	sf(k, F_Z | F_S, (p == 0 ? F_Z : 0) | (p < 0 ? F_S : 0));
}

static void digits(Z8K *k, int link, int src, int left)
{
	uint8_t a = R8(k, link), b = R8(k, src), na, nb;
	if (left) { na = (a & 0xF0) | (b >> 4); nb = (b << 4) | (a & 0x0F); }
	else { na = (a & 0xF0) | (b & 0x0F); nb = (a << 4) | (b >> 4); }
	W8(k, link, na); W8(k, src, nb);
	logf_(k, na, 1);
}

static void trace_(Z8K *k, int line)
{
	fprintf(stderr, "fpe.s:%-4d r0-7 %04x %04x %04x %04x %04x %04x %04x %04x"
		" r8-15 %04x %04x %04x %04x %04x %04x %04x %04x fl %02x\n", line,
		k->r[0], k->r[1], k->r[2], k->r[3], k->r[4], k->r[5], k->r[6], k->r[7],
		k->r[8], k->r[9], k->r[10], k->r[11], k->r[12], k->r[13], k->r[14],
		k->r[15], k->fl);
}

/*
 * mch.s routines used by fpe.s.  User addresses come in a register pair
 * (segment word, offset); a non-segmented program has everything in
 * emulator segment 0.  Counts are in words, as in mch.s.
 */
static Ad user_ad(Z8K *u, uint16_t segw, uint16_t off)
{
	Ad a;
	a.seg = u->segmode ? (segw >> 8) & 0x7F : 0;
	a.off = off;
	return a;
}

/* gettext: r3 = next instruction word at rr6; rl2 = 0 */
static void nat_gettext(Z8K *k, Z8K *u)
{
	Ad a = user_ad(u, k->r[6], k->r[7]);
	uint16_t w;
	if (!u->segmode) {	/* code space of a non-segmented program */
		uint8_t *m = iseg(u, 0);
		w = (m[a.off] << 8) | m[(uint16_t)(a.off + 1)];
		k->r[6] = 0x3F00;
		W8(k, 10, 0);
	} else {
		w = rdw(u, a);
		k->r[4] = w;
		k->r[2] = k->r[3] = 0;
	}
	k->r[3] = w;
}

/* getmem: copy r3 words from user rr6 to kernel rr4; r2 = 0 */
static void nat_getmem(Z8K *k, Z8K *u)
{
	Ad s = user_ad(u, k->r[6], k->r[7]), d = ir_ad(k, 4);
	unsigned n = k->r[3], i;
	for (i = 0; i < n; i++)
		wrw(k, adda(d, 2 * i), rdw(u, adda(s, 2 * i)));
	k->r[5] += 2 * n; k->r[7] += 2 * n;
	k->r[2] = k->r[3] = 0;
}

/* putmem: copy r3 words from kernel rr6 to user rr4; r2 = 0 */
static void nat_putmem(Z8K *k, Z8K *u)
{
	Ad s = ir_ad(k, 6), d = user_ad(u, k->r[4], k->r[5]);
	unsigned n = k->r[3], i;
	for (i = 0; i < n; i++)
		wrw(u, adda(d, 2 * i), rdw(k, adda(s, 2 * i)));
	k->r[5] += 2 * n; k->r[7] += 2 * n;
	k->r[2] = k->r[3] = 0;
}

/* macros used by fpe_gen.c */
#define TR(line)	do { if (k->trace) trace_(k, line); } while (0)
#define JMP(l)		do { if (++steps > STEP_LIMIT) fatal("runaway", 0); goto l; } while (0)
#define PUSHRET(n)	push_(k, 14, 4, n)
#define RET		do { tgt_ = pop_(k, 14, 4); goto dispatch; } while (0)
#define JPIND(n)	do { tgt_ = R32(k, n); goto dispatch; } while (0)
#define NATIVE(f)	nat_##f(k, u)
#define FELL_OFF	fatal("fell off the end", 0)
#define BAD_TARGET	fatal("bad jump target", tgt_)
#define LDA(n, a)	do { Ad a_ = (a); k->r[n] = a_.seg << 8; k->r[(n) + 1] = a_.off; } while (0)
#define PUSH(n, sz, v)	do { uint32_t v_ = (v); push_(k, n, sz, v_); } while (0)
#define POP(n, sz)	pop_(k, n, sz)
#define LDM_LOAD(rn, a, cnt) do { Ad a_ = (a); int i_; \
		for (i_ = 0; i_ < (cnt); i_++) k->r[((rn) + i_) & 15] = rdw(k, adda(a_, 2 * i_)); } while (0)
#define LDM_STORE(a, rn, cnt) do { Ad a_ = (a); int i_; \
		for (i_ = 0; i_ < (cnt); i_++) wrw(k, adda(a_, 2 * i_), k->r[((rn) + i_) & 15]); } while (0)
#define LDIR(d, s, n)	ldir_(k, d, s, n)
#define COM(n, sz)	do { uint32_t v_ = ~rreg(k, n, sz) & MSK[sz]; wreg(k, n, sz, v_); \
		if ((sz) == 1) logf_(k, v_, 1); else zs(k, v_, sz); } while (0)
#define INCDEC_R(n, sz, c, d) wreg(k, n, sz, incdec(k, rreg(k, n, sz), sz, c, d))
#define INCDEC_M(a, sz, c, d) do { Ad a_ = (a); mwr(k, a_, sz, incdec(k, mrd(k, a_, sz), sz, c, d)); } while (0)
#define BIT_R(n, sz, b)	sf(k, F_Z, (rreg(k, n, sz) >> (b)) & 1 ? 0 : F_Z)
#define SET_R(n, sz, b)	wreg(k, n, sz, rreg(k, n, sz) | (1u << (b)))
#define RES_R(n, sz, b)	wreg(k, n, sz, rreg(k, n, sz) & ~(1u << (b)))
#define BIT_M(a, sz, b)	sf(k, F_Z, (mrd(k, a, sz) >> (b)) & 1 ? 0 : F_Z)
#define SET_M(a, sz, b)	do { Ad a_ = (a); mwr(k, a_, sz, mrd(k, a_, sz) | (1u << (b))); } while (0)
#define RES_M(a, sz, b)	do { Ad a_ = (a); mwr(k, a_, sz, mrd(k, a_, sz) & ~(1u << (b))); } while (0)
#define MULTL(n, v)	multl_(k, n, v)
#define RLDB(a, b)	digits(k, a, b, 1)
#define RRDB(a, b)	digits(k, a, b, 0)

#include "fpe_gen.c"

/* fpstate offsets (bytes): uf_reg[8][5] at 0, uf_creg[32] at 0x50 */
void z8k_epu_reset(Z8K *c)
{
	int i;
	/* setregs() in sys1.c: all registers 1.0, control registers kept */
	for (i = 0; i < 8; i++) {
		memset(c->fps + 10 * i, 0, 10);
		c->fps[10 * i] = 0x3F; c->fps[10 * i + 1] = 0xFF;
		c->fps[10 * i + 2] = 0x80;
	}
}

/*
 * Execute the EPU instruction whose first word w0 has just been fetched
 * (c->pc points to its second word).  Mirrors uniminstr() in trap.c.
 */
int z8k_epu(Z8K *c, uint16_t w0)
{
	Z8K *k;
	uint8_t *km;
	int i, rc;

	if (!kctx) {
		kctx = calloc(1, sizeof *kctx);
		if (!kctx) fatal("out of memory", 0);
		kctx->segmode = 1;
	}
	k = kctx;
	k->trace = c->trace > 1;
	km = z8k_block(k, 1, 0);

	/* struct state */
	for (i = 0; i < 16; i++) {
		km[K_STATE + S_REG + 2 * i] = c->r[i] >> 8;
		km[K_STATE + S_REG + 2 * i + 1] = c->r[i];
	}
	wrw(k, (Ad){ 0, K_STATE + S_EVENT }, w0);
	wrw(k, (Ad){ 0, K_STATE + S_PS }, (c->segmode ? 0x8000 : 0) | c->fl);
	wrw(k, (Ad){ 0, K_STATE + S_PCSEG }, c->segmode ? c->pcseg << 8 : 0);
	wrw(k, (Ad){ 0, K_STATE + S_PC }, c->pc);
	memcpy(km + K_FPS, c->fps, sizeof c->fps);

	/* fpepu(state, &u.u_fpstate) */
	memset(k->r, 0, sizeof k->r);
	k->r[7] = K_STATE;
	k->r[5] = K_FPS;
	k->r[15] = K_STACK;
	k->fl = 0;
	steps = 0;
	fpe_run(k, c);
	rc = k->r[2];
	for (i = 1; i < 128; i++)
		if (k->dmem[i]) fatal("kernel access outside its data segment", i);

	for (i = 0; i < 16; i++)
		c->r[i] = (km[K_STATE + S_REG + 2 * i] << 8) | km[K_STATE + S_REG + 2 * i + 1];
	c->fl = rdw(k, (Ad){ 0, K_STATE + S_PS }) & 0xFC;
	c->pc = rdw(k, (Ad){ 0, K_STATE + S_PC });
	memcpy(c->fps, km + K_FPS, sizeof c->fps);

	/* signals are not delivered: both are fatal, like their default action */
	if (rc & 0x00FF) {
		fprintf(stderr, "z8k: EPU operand address fault (SIGSEG) at %02x:%04x\n",
			c->cur_seg, c->cur_pc);
		return -1;
	}
	if (rc & 0xFF00) {
		fprintf(stderr, "z8k: floating point exception (SIGFPE, %02x) at %02x:%04x\n",
			rc >> 8, c->cur_seg, c->cur_pc);
		return -1;
	}
	return 0;
}
