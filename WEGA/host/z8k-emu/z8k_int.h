/*
 * z8k_int.h - memory, register and flag helpers shared by the CPU core
 * (z8k.c) and the EPU emulation (epu.c).
 */
#ifndef Z8K_INT_H
#define Z8K_INT_H
#include "z8k.h"

typedef struct { uint8_t seg; uint16_t off; } Ad;

static inline uint8_t *dseg(Z8K *c, unsigned s) { return c->dmem[s & 127] ? c->dmem[s & 127] : z8k_block(c, 1, s); }
static inline uint8_t *iseg(Z8K *c, unsigned s) { return c->imem[s & 127] ? c->imem[s & 127] : z8k_block(c, 0, s); }

static inline uint8_t  rdb(Z8K *c, Ad a) { return dseg(c, a.seg)[a.off]; }
static inline uint16_t rdw(Z8K *c, Ad a)
{
	uint8_t *m = dseg(c, a.seg);
	return (m[a.off] << 8) | m[(uint16_t)(a.off + 1)];
}
static inline void wrb(Z8K *c, Ad a, uint8_t v) { dseg(c, a.seg)[a.off] = v; }
static inline void wrw(Z8K *c, Ad a, uint16_t v)
{
	uint8_t *m = dseg(c, a.seg);
	m[a.off] = v >> 8; m[(uint16_t)(a.off + 1)] = v;
}
static inline Ad adda(Ad a, int d) { a.off = (uint16_t)(a.off + d); return a; }

static inline uint32_t mrd(Z8K *c, Ad a, int sz)
{
	if (sz == 1) return rdb(c, a);
	if (sz == 2) return rdw(c, a);
	return ((uint32_t)rdw(c, a) << 16) | rdw(c, adda(a, 2));
}
static inline void mwr(Z8K *c, Ad a, int sz, uint32_t v)
{
	if (sz == 1) wrb(c, a, v);
	else if (sz == 2) wrw(c, a, v);
	else { wrw(c, a, v >> 16); wrw(c, adda(a, 2), v); }
}

/* registers */
static inline uint8_t R8(Z8K *c, int n) { return n < 8 ? c->r[n] >> 8 : c->r[n - 8] & 0xFF; }
static inline void W8(Z8K *c, int n, uint8_t v)
{
	if (n < 8) c->r[n] = (c->r[n] & 0x00FF) | (v << 8);
	else c->r[n - 8] = (c->r[n - 8] & 0xFF00) | v;
}
static inline uint32_t R32(Z8K *c, int n) { return ((uint32_t)c->r[n] << 16) | c->r[(n + 1) & 15]; }
static inline void W32(Z8K *c, int n, uint32_t v) { c->r[n] = v >> 16; c->r[(n + 1) & 15] = v; }
static inline uint32_t rreg(Z8K *c, int n, int sz) { return sz == 1 ? R8(c, n) : sz == 2 ? c->r[n] : R32(c, n); }
static inline void wreg(Z8K *c, int n, int sz, uint32_t v)
{
	if (sz == 1) W8(c, n, v); else if (sz == 2) c->r[n] = v; else W32(c, n, v);
}

/* address held in a register (rr in segmented mode) */
static inline Ad ir_ad(Z8K *c, int n)
{
	Ad a;
	if (c->segmode) { a.seg = (c->r[n] >> 8) & 0x7F; a.off = c->r[n + 1]; }
	else { a.seg = 0; a.off = c->r[n]; }
	return a;
}
static inline uint16_t *offreg(Z8K *c, int n) { return c->segmode ? &c->r[n + 1] : &c->r[n]; }

/* flags */
static inline void sf(Z8K *c, unsigned mask, unsigned val) { c->fl = (c->fl & ~mask) | (val & mask); }
static const uint32_t MSK[5] = { 0, 0xFF, 0xFFFF, 0, 0xFFFFFFFFu };
static inline uint32_t msb(int sz) { return 1u << (8 * sz - 1); }
static inline int par8(uint8_t v) { v ^= v >> 4; v ^= v >> 2; v ^= v >> 1; return !(v & 1); }
static inline void zs(Z8K *c, uint32_t r, int sz)
{
	sf(c, F_Z | F_S, ((r & MSK[sz]) == 0 ? F_Z : 0) | ((r & msb(sz)) ? F_S : 0));
}
static inline void logf_(Z8K *c, uint32_t r, int sz)
{
	zs(c, r, sz);
	if (sz == 1) sf(c, F_V, par8(r) ? F_V : 0);
}

static inline uint32_t addsub(Z8K *c, uint32_t a, uint32_t b, int cin, int sz, int sub)
{
	uint64_t mask = MSK[sz], m = msb(sz), res;
	int carry, ov;
	a &= mask; b &= mask;
	if (!sub) {
		uint64_t f = (uint64_t)a + b + cin;
		res = f & mask; carry = (f >> (8 * sz)) & 1;
		ov = ((~(a ^ b)) & (a ^ res) & m) != 0;
	} else {
		res = ((uint64_t)a - b - cin) & mask;
		carry = (uint64_t)a < (uint64_t)b + cin;
		ov = (((a ^ b) & (a ^ res)) & m) != 0;
	}
	sf(c, F_C | F_V, (carry ? F_C : 0) | (ov ? F_V : 0));
	zs(c, res, sz);
	if (sz == 1) {
		int h = sub ? ((a & 15) < (unsigned)((b & 15) + cin)) : (((a & 15) + (b & 15) + cin) > 15);
		sf(c, F_D | F_H, (sub ? F_D : 0) | (h ? F_H : 0));
	}
	return res;
}

static inline int cond(Z8K *c, int cc)
{
	int C = !!(c->fl & F_C), Z = !!(c->fl & F_Z), S = !!(c->fl & F_S), V = !!(c->fl & F_V);
	switch (cc & 15) {
	case 0: return 0;
	case 1: return S ^ V;
	case 2: return Z | (S ^ V);
	case 3: return C | Z;
	case 4: return V;
	case 5: return S;
	case 6: return Z;
	case 7: return C;
	case 8: return 1;
	case 9: return !(S ^ V);
	case 10: return !(Z | (S ^ V));
	case 11: return !C && !Z;
	case 12: return !V;
	case 13: return !S;
	case 14: return !Z;
	default: return !C;
	}
}

/* shifts / rotates */
static inline uint32_t rotate(Z8K *c, uint32_t v, int sz, int left, int cnt, int thru)
{
	int bits = 8 * sz, i;
	uint32_t m = MSK[sz], sgn = msb(sz), orig = v & m;
	int cy = !!(c->fl & F_C);
	v &= m;
	for (i = 0; i < cnt; i++) {
		int out;
		if (left) { out = !!(v & sgn); v = (v << 1) & m; v |= thru ? (uint32_t)cy : (uint32_t)out; }
		else { out = v & 1; v >>= 1; if (thru ? cy : out) v |= sgn; }
		if (thru) cy = out; else cy = out;
	}
	(void)bits;
	sf(c, F_C, cy ? F_C : 0);
	zs(c, v, sz);
	sf(c, F_V, ((orig ^ v) & sgn) ? F_V : 0);
	return v;
}

static inline uint32_t shiftn(Z8K *c, uint32_t v, int sz, int cnt, int arith)
{
	uint32_t m = MSK[sz], sgn = msb(sz);
	int cy = !!(c->fl & F_C), ov = 0, i;
	v &= m;
	if (cnt >= 0) {
		for (i = 0; i < cnt; i++) {
			uint32_t o = v;
			cy = !!(v & sgn); v = (v << 1) & m;
			if ((o ^ v) & sgn) ov = 1;
		}
	} else {
		for (i = 0; i < -cnt; i++) {
			cy = v & 1;
			v = (v >> 1) | ((arith && (v & sgn)) ? sgn : 0);
		}
	}
	if (cnt) sf(c, F_C, cy ? F_C : 0);
	zs(c, v, sz);
	sf(c, F_V, (arith && ov) ? F_V : 0);
	return v;
}

#endif
