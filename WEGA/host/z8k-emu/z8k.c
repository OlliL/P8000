/*
 * z8k.c - Z8001/Z8002 user-mode instruction interpreter.
 * Opcode decoding follows tools/z8k.py, which was validated against every
 * relocation of the shipped WEGA library objects.
 */
#include <stdlib.h>
#include <string.h>
#include "z8k_int.h"

typedef struct { int kind; int n; Ad a; uint32_t v; } Opd; /* 0 reg 1 mem 2 imm */

#define FAIL(c, ...) do { fprintf(stderr, "z8k: " __VA_ARGS__); \
	fprintf(stderr, " at %02x:%04x\n", (c)->cur_seg, (c)->cur_pc); return -1; } while (0)

uint8_t *z8k_block(Z8K *c, int space, unsigned seg)
{
	uint8_t **t = space ? c->dmem : c->imem;
	seg &= 127;
	if (!t[seg]) {
		uint8_t *m = calloc(1, 65536);
		if (!m) { fprintf(stderr, "z8k: out of memory\n"); exit(255); }
		if (c->verbose)
			fprintf(stderr, "z8k: allocating %s segment %u (pc %02x:%04x)\n",
				space ? "data" : "code", seg, c->cur_seg, c->cur_pc);
		t[seg] = m;
		if (!c->sepid) {
			c->imem[seg] = c->dmem[seg] = m;
		}
	}
	return t[seg];
}


static inline uint16_t fetch(Z8K *c)
{
	uint8_t *m = iseg(c, c->pcseg);
	uint16_t w = (m[c->pc] << 8) | m[(uint16_t)(c->pc + 1)];
	c->pc += 2;
	return w;
}


/* direct address / indexed */
static Ad ea_dax(Z8K *c, int idx)
{
	Ad a;
	uint16_t w = fetch(c);
	if (!c->segmode) { a.seg = 0; a.off = w; }
	else if (w & 0x8000) { a.seg = (w >> 8) & 0x7F; a.off = fetch(c); }
	else { a.seg = (w >> 8) & 0x7F; a.off = w & 0xFF; }
	if (idx) a.off += c->r[idx];
	return a;
}


/* operand sources */
static uint32_t imm_fetch(Z8K *c, int sz)
{
	if (sz == 1) return fetch(c) & 0xFF;
	if (sz == 2) return fetch(c);
	{ uint32_t h = fetch(c); return (h << 16) | fetch(c); }
}

static Opd mk_opd(Z8K *c, int cls, int n, int sz, int imm_ok)
{
	Opd o; memset(&o, 0, sizeof o);
	if (cls == 2) { o.kind = 0; o.n = n; }
	else if (cls == 1) { o.kind = 1; o.a = ea_dax(c, n); }
	else if (n == 0 && imm_ok) { o.kind = 2; o.v = imm_fetch(c, sz); }
	else { o.kind = 1; o.a = ir_ad(c, n); }
	return o;
}
static uint32_t og(Z8K *c, Opd *o, int sz)
{
	if (o->kind == 0) return rreg(c, o->n, sz);
	if (o->kind == 1) return mrd(c, o->a, sz);
	return o->v;
}
static void op_(Z8K *c, Opd *o, int sz, uint32_t v)
{
	if (o->kind == 0) wreg(c, o->n, sz, v);
	else if (o->kind == 1) mwr(c, o->a, sz, v);
}


static void push_pc(Z8K *c)
{
	if (c->segmode) {
		c->r[15] -= 4;
		Ad a = { (c->r[14] >> 8) & 0x7F, c->r[15] };
		wrw(c, a, c->pcseg << 8);
		wrw(c, adda(a, 2), c->pc);
	} else {
		c->r[15] -= 2;
		Ad a = { 0, c->r[15] };
		wrw(c, a, c->pc);
	}
}
static void pop_pc(Z8K *c)
{
	if (c->segmode) {
		Ad a = { (c->r[14] >> 8) & 0x7F, c->r[15] };
		c->pcseg = (rdw(c, a) >> 8) & 0x7F;
		c->pc = rdw(c, adda(a, 2));
		c->r[15] += 4;
	} else {
		Ad a = { 0, c->r[15] };
		c->pc = rdw(c, a);
		c->r[15] += 2;
	}
}

static void jump_to(Z8K *c, Ad a)
{
	if (c->segmode) c->pcseg = a.seg;
	c->pc = a.off;
}

static int blk_ld(Z8K *c, int sz, int dec, int rep, int dn, int sn, int rn)
{
	uint16_t *dp = offreg(c, dn), *sp = offreg(c, sn);
	do {
		Ad d = ir_ad(c, dn), s = ir_ad(c, sn);
		mwr(c, d, sz, mrd(c, s, sz));
		*dp += dec ? -sz : sz; *sp += dec ? -sz : sz;
		c->r[rn]--;
	} while (rep && c->r[rn] != 0);
	sf(c, F_V, c->r[rn] == 0 ? F_V : 0);
	return 0;
}

static int blk_cp(Z8K *c, int sz, int dec, int rep, int rd, int sn, int rn, int cc, int string)
{
	uint16_t *sp = offreg(c, sn);
	int hit;
	do {
		Ad s = ir_ad(c, sn);
		uint32_t a = string ? mrd(c, ir_ad(c, rd), sz) : rreg(c, rd, sz);
		addsub(c, a, mrd(c, s, sz), 0, sz, 1);
		hit = cond(c, cc);
		*sp += dec ? -sz : sz;
		if (string) *offreg(c, rd) += dec ? -sz : sz;
		c->r[rn]--;
	} while (rep && !hit && c->r[rn] != 0);
	sf(c, F_Z | F_V, (hit ? F_Z : 0) | (c->r[rn] == 0 ? F_V : 0));
	return 0;
}

int z8k_step(Z8K *c)
{
	uint16_t w0;
	unsigned hi, lo, n2, n3, cls, base;
	Opd s, d;
	c->cur_pc = c->pc; c->cur_seg = c->pcseg;
	w0 = fetch(c);
	c->icount++;
	if (c->trace) {
		fprintf(stderr, "%02x:%04x %04x  r0-7 %04x %04x %04x %04x %04x %04x %04x %04x  sp %04x fl %02x\n",
			c->cur_seg, c->cur_pc, w0, c->r[0], c->r[1], c->r[2], c->r[3], c->r[4], c->r[5],
			c->r[6], c->r[7], c->r[15], c->fl);
	}
	hi = w0 >> 8; lo = w0 & 0xFF; n2 = (w0 >> 4) & 15; n3 = w0 & 15;
	cls = hi >> 6; base = hi & 0x3F;

	if (cls == 3) {
		switch (hi >> 4) {
		case 0xC: W8(c, hi & 15, lo); return 0;
		case 0xD: {
			int disp = w0 & 0xFFF; if (disp & 0x800) disp -= 0x1000;
			push_pc(c); c->pc = c->pc - 2 * disp; return 0; }
		case 0xE: {
			int disp = (int8_t)lo;
			if (cond(c, hi & 15)) c->pc += 2 * disp;
			return 0; }
		default: {
			int r = hi & 15;
			if (w0 & 0x80) { c->r[r]--; if (c->r[r]) c->pc -= 2 * (w0 & 0x7F); }
			else { uint8_t v = R8(c, r) - 1; W8(c, r, v); if (v) c->pc -= 2 * (w0 & 0x7F); }
			return 0; }
		}
	}

	switch (base) {
	/* ---- two-operand ALU, register destination */
	case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
	case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
	case 0x10: case 0x12: case 0x14: case 0x16: case 0x20: case 0x21: {
		static const struct { int sz, op; } T[0x22] = {
			[0x00] = {1, 0}, [0x01] = {2, 0}, [0x02] = {1, 1}, [0x03] = {2, 1},
			[0x04] = {1, 2}, [0x05] = {2, 2}, [0x06] = {1, 3}, [0x07] = {2, 3},
			[0x08] = {1, 4}, [0x09] = {2, 4}, [0x0A] = {1, 5}, [0x0B] = {2, 5},
			[0x10] = {4, 5}, [0x12] = {4, 1}, [0x14] = {4, 6}, [0x16] = {4, 0},
			[0x20] = {1, 6}, [0x21] = {2, 6} };
		int sz = T[base].sz, op = T[base].op;
		uint32_t a, b, r = 0;
		s = mk_opd(c, cls, n2, sz, 1);
		b = og(c, &s, sz); a = rreg(c, n3, sz);
		switch (op) {
		case 0: r = addsub(c, a, b, 0, sz, 0); break;
		case 1: r = addsub(c, a, b, 0, sz, 1); break;
		case 2: r = a | b; logf_(c, r, sz); break;
		case 3: r = a & b; logf_(c, r, sz); break;
		case 4: r = a ^ b; logf_(c, r, sz); break;
		case 5: addsub(c, a, b, 0, sz, 1); return 0;
		case 6: r = b; break;
		}
		wreg(c, n3, sz, r);
		return 0; }

	case 0x18: case 0x19: {		/* multl / mult */
		int big = base == 0x18;
		s = mk_opd(c, cls, n2, big ? 4 : 2, 1);
		if (big) {
			int64_t p = (int64_t)(int32_t)R32(c, n3 + 2) * (int32_t)og(c, &s, 4);
			c->r[n3] = p >> 48; c->r[n3 + 1] = p >> 32; c->r[n3 + 2] = p >> 16; c->r[n3 + 3] = p;
			sf(c, F_C | F_V, (p != (int64_t)(int32_t)p) ? F_C : 0);
			sf(c, F_Z | F_S, (p == 0 ? F_Z : 0) | (p < 0 ? F_S : 0));
		} else {
			int32_t p = (int32_t)(int16_t)c->r[n3 + 1] * (int16_t)og(c, &s, 2);
			W32(c, n3, p);
			sf(c, F_C | F_V, (p != (int32_t)(int16_t)p) ? F_C : 0);
			sf(c, F_Z | F_S, (p == 0 ? F_Z : 0) | (p < 0 ? F_S : 0));
		}
		return 0; }
	case 0x1A: case 0x1B: {		/* divl / div */
		int big = base == 0x1A;
		s = mk_opd(c, cls, n2, big ? 4 : 2, 1);
		if (big) {
			int64_t n = ((int64_t)(int32_t)R32(c, n3) << 32) | R32(c, n3 + 2);
			int32_t dv = (int32_t)og(c, &s, 4);
			if (dv == 0) { sf(c, F_Z | F_V, F_Z | F_V); return 0; }
			int64_t q = n / dv, rm = n % dv;
			if (q != (int32_t)q) { sf(c, F_V | F_C, F_V | F_C); return 0; }
			W32(c, n3, (uint32_t)rm); W32(c, n3 + 2, (uint32_t)q);
			sf(c, F_C | F_V, 0); sf(c, F_Z | F_S, (q == 0 ? F_Z : 0) | (q < 0 ? F_S : 0));
		} else {
			int32_t n = (int32_t)R32(c, n3);
			int16_t dv = (int16_t)og(c, &s, 2);
			if (dv == 0) { sf(c, F_Z | F_V, F_Z | F_V); return 0; }
			int32_t q = n / dv, rm = n % dv;
			if (q != (int16_t)q) { sf(c, F_V | F_C, F_V | F_C); return 0; }
			c->r[n3] = rm; c->r[n3 + 1] = q;
			sf(c, F_C | F_V, 0); sf(c, F_Z | F_S, (q == 0 ? F_Z : 0) | (q < 0 ? F_S : 0));
		}
		return 0; }

	case 0x2C: case 0x2D: {		/* exb / ex */
		int sz = base == 0x2C ? 1 : 2;
		uint32_t t;
		s = mk_opd(c, cls, n2, sz, 0);
		t = og(c, &s, sz); op_(c, &s, sz, rreg(c, n3, sz)); wreg(c, n3, sz, t);
		return 0; }

	case 0x2E: case 0x2F: case 0x1D:	/* stores */
		if (cls == 2) {
			if (base == 0x2E || base == 0x2F) {	/* tccb / tcc */
				int sz = base == 0x2E ? 1 : 2;
				if (cond(c, n3)) wreg(c, n2, sz, rreg(c, n2, sz) | 1);
				return 0;
			}
			if (base == 0x1D) FAIL(c, "illegal %04x", w0);
		}
		{
			int sz = base == 0x2E ? 1 : base == 0x2F ? 2 : 4;
			d = mk_opd(c, cls, n2, sz, 0);
			op_(c, &d, sz, rreg(c, n3, sz));
			return 0;
		}

	case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27: {  /* res set bit */
		int sz = (base & 1) ? 2 : 1, bit, kind = (base - 0x22) >> 1;  /* 0 res 1 set 2 bit */
		uint32_t v;
		if (cls == 0 && n2 == 0) {
			uint16_t w1 = fetch(c);
			d = mk_opd(c, 2, (w1 >> 8) & 15, sz, 0);
			bit = c->r[n3] & (sz == 1 ? 7 : 15);
		} else {
			d = mk_opd(c, cls, n2, sz, 0);
			bit = n3;
		}
		v = og(c, &d, sz);
		if (kind == 0) op_(c, &d, sz, v & ~(1u << bit));
		else if (kind == 1) op_(c, &d, sz, v | (1u << bit));
		else sf(c, F_Z, (v >> bit) & 1 ? 0 : F_Z);
		return 0; }

	case 0x28: case 0x29: case 0x2A: case 0x2B: {		/* inc dec */
		int sz = (base & 1) ? 2 : 1, dec = base >= 0x2A;
		uint32_t a, r, m = msb(sz), k = n3 + 1;
		d = mk_opd(c, cls, n2, sz, 0);
		a = og(c, &d, sz);
		r = dec ? (a - k) & MSK[sz] : (a + k) & MSK[sz];
		zs(c, r, sz);
		sf(c, F_V, (dec ? ((a ^ k) & (a ^ r) & m) : (~(a ^ k) & (a ^ r) & m)) ? F_V : 0);
		op_(c, &d, sz, r);
		return 0; }

	case 0x0C: case 0x0D: {
		int byte = base == 0x0C, sz = byte ? 1 : 2;
		if (cls == 2) {
			if (byte) {
				switch (n3) {
				case 0: W8(c, n2, ~R8(c, n2)); logf_(c, R8(c, n2), 1); return 0;
				case 1: W8(c, n2, c->fl); return 0;		/* ldctlb rbd,flags */
				case 2: { uint8_t a = R8(c, n2), r = addsub(c, 0, a, 0, 1, 1); W8(c, n2, r); return 0; }
				case 4: logf_(c, R8(c, n2), 1); return 0;
				case 6: sf(c, F_S, (R8(c, n2) & 0x80) ? F_S : 0); W8(c, n2, 0xFF); return 0;
				case 8: W8(c, n2, 0); return 0;
				case 9: c->fl = R8(c, n2) & 0xFC; return 0;	/* ldctlb flags,rbs */
				}
				FAIL(c, "illegal %04x", w0);
			}
			switch (n3) {
			case 0: c->r[n2] = ~c->r[n2]; zs(c, c->r[n2], 2); return 0;
			case 1: c->fl |= n2 << 4; return 0;		/* setflg */
			case 2: c->r[n2] = addsub(c, 0, c->r[n2], 0, 2, 1); return 0;
			case 3: c->fl &= ~(n2 << 4); return 0;		/* resflg */
			case 4: zs(c, c->r[n2], 2); return 0;
			case 5: c->fl ^= n2 << 4; return 0;		/* comflg */
			case 6: sf(c, F_S, (c->r[n2] & 0x8000) ? F_S : 0); c->r[n2] = 0xFFFF; return 0;
			case 7: return 0;				/* nop */
			case 8: c->r[n2] = 0; return 0;
			}
			FAIL(c, "illegal %04x", w0);
		}
		if (n3 == 9 && !byte) {		/* push @sp,#imm */
			Ad sa = ir_ad(c, n2);
			uint16_t v = fetch(c);
			*offreg(c, n2) -= 2; sa.off -= 2;
			wrw(c, sa, v);
			return 0;
		}
		d = mk_opd(c, cls, n2, sz, 0);
		switch (n3) {
		case 0: { uint32_t v = ~og(c, &d, sz) & MSK[sz]; op_(c, &d, sz, v); logf_(c, v, sz); return 0; }
		case 1: { uint32_t k = imm_fetch(c, sz); addsub(c, og(c, &d, sz), k, 0, sz, 1); return 0; }
		case 2: { uint32_t v = addsub(c, 0, og(c, &d, sz), 0, sz, 1); op_(c, &d, sz, v); return 0; }
		case 4: logf_(c, og(c, &d, sz), sz); return 0;
		case 5: { uint32_t k = imm_fetch(c, sz); op_(c, &d, sz, k); return 0; }
		case 6: { uint32_t v = og(c, &d, sz); sf(c, F_S, (v & msb(sz)) ? F_S : 0); op_(c, &d, sz, MSK[sz]); return 0; }
		case 8: op_(c, &d, sz, 0); return 0;
		}
		FAIL(c, "illegal %04x", w0); }

	case 0x11: case 0x13: {		/* pushl / push */
		int sz = base == 0x11 ? 4 : 2;
		Ad sa = ir_ad(c, n2);
		uint32_t v;
		s = mk_opd(c, cls, n3, sz, 0);
		v = og(c, &s, sz);
		*offreg(c, n2) -= sz; sa.off -= sz;
		mwr(c, sa, sz, v);
		return 0; }
	case 0x15: case 0x17: {		/* popl / pop */
		int sz = base == 0x15 ? 4 : 2;
		Ad sa = ir_ad(c, n2);
		d = mk_opd(c, cls, n3, sz, 0);
		op_(c, &d, sz, mrd(c, sa, sz));
		*offreg(c, n2) += sz;
		return 0; }

	case 0x1C: {
		if (cls == 2) {
			if (n3 != 8) FAIL(c, "illegal %04x", w0);
			zs(c, R32(c, n2), 4); return 0;
		}
		if (n3 == 8) { d = mk_opd(c, cls, n2, 4, 0); zs(c, og(c, &d, 4), 4); return 0; }
		if (n3 == 1 || n3 == 9) {
			uint16_t w1 = fetch(c);
			int r = (w1 >> 8) & 15, cnt = (w1 & 15) + 1, i;
			Ad a;
			d = mk_opd(c, cls, n2, 2, 0);
			a = d.a;
			for (i = 0; i < cnt; i++, a = adda(a, 2)) {
				if (n3 == 1) c->r[(r + i) & 15] = rdw(c, a);
				else wrw(c, a, c->r[(r + i) & 15]);
			}
			return 0;
		}
		FAIL(c, "illegal %04x", w0); }

	case 0x1E:
		if (cls == 2) {			/* ret cc */
			if (cond(c, n3)) pop_pc(c);
			return 0;
		} else {
			Ad a;
			if (cls == 0) a = ir_ad(c, n2); else a = ea_dax(c, n2);
			if (cond(c, n3)) jump_to(c, a);
			return 0;
		}
	case 0x1F: {
		Ad a;
		if (cls == 0) a = ir_ad(c, n2); else a = ea_dax(c, n2);
		push_pc(c);
		jump_to(c, a);
		return 0; }

	case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37:
		if (cls == 2) break;
		if (cls == 0 || cls == 1) {
			int k = base - 0x30;
			int sz = (k == 0 || k == 2) ? 1 : (k == 5 || k == 7) ? 4 : 2;
			int store = (k == 2 || k == 3 || k == 7);
			Ad a;
			if (cls == 0) {
				if (n2 == 0) {	/* pc relative */
					int16_t disp = (int16_t)fetch(c);
					a.seg = c->pcseg; a.off = c->pc + disp;
					if (!c->segmode) a.seg = 0;
					if (k == 4) {	/* ldar */
						if (c->segmode) { c->r[n3] = a.seg << 8; c->r[n3 + 1] = a.off; }
						else c->r[n3] = a.off;
						return 0;
					}
				} else {
					uint16_t disp = fetch(c);
					a = ir_ad(c, n2); a.off += disp;
					if (k == 4) {
						if (c->segmode) { c->r[n3] = a.seg << 8; c->r[n3 + 1] = a.off; }
						else c->r[n3] = a.off;
						return 0;
					}
				}
			} else {
				uint16_t w1 = fetch(c);
				a = ir_ad(c, n2); a.off += c->r[(w1 >> 8) & 15];
				if (k == 4) {
					if (c->segmode) { c->r[n3] = a.seg << 8; c->r[n3 + 1] = a.off; }
					else c->r[n3] = a.off;
					return 0;
				}
			}
			if (store) mwr(c, a, sz, rreg(c, n3, sz));
			else wreg(c, n3, sz, mrd(c, a, sz));
			return 0;
		}
		FAIL(c, "illegal %04x", w0);
	case 0x36:
		if (cls == 2) break;
		if (cls == 1) {		/* lda rd, DA/X */
			Ad a = ea_dax(c, n2);
			if (c->segmode) { c->r[n3] = a.seg << 8; c->r[n3 + 1] = a.off; }
			else c->r[n3] = a.off;
			return 0;
		}
		FAIL(c, "illegal %04x", w0);
	case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
		if (cls == 0) FAIL(c, "I/O instruction %04x in user mode", w0);
		break;
	case 0x39:
		if (cls != 2) FAIL(c, "privileged instruction %04x", w0);
		break;
	}

	switch (hi) {
	case 0x7A: c->halted = 1; return 0;
	case 0x7B: FAIL(c, "privileged instruction %04x", w0);
	case 0x7C: return 0;			/* di/ei */
	case 0x7D: FAIL(c, "ldctl %04x in user mode", w0);
	case 0x7F: {
		int r;
		r = c->syscall ? c->syscall(c, lo) : -1;
		return r; }

	case 0xB0: {			/* dab */
		uint8_t v = R8(c, n2), orig = v;
		if (!(c->fl & F_D)) {
			if ((orig & 15) > 9 || (c->fl & F_H)) v += 6;
			if (orig > 0x99 || (c->fl & F_C)) { v += 0x60; sf(c, F_C, F_C); }
		} else {
			if (c->fl & F_H) v -= 6;
			if (c->fl & F_C) v -= 0x60;
		}
		W8(c, n2, v); logf_(c, v, 1);
		return 0; }
	case 0xB1:
		if (n3 == 0) { c->r[n2] = (c->r[n2] & 0xFF) | ((c->r[n2] & 0x80) ? 0xFF00 : 0); return 0; }
		if (n3 == 0xA) { c->r[n2] = (c->r[n2 + 1] & 0x8000) ? 0xFFFF : 0; return 0; }
		if (n3 == 7) { uint32_t l = R32(c, n2 + 2); W32(c, n2, (l & 0x80000000u) ? 0xFFFFFFFFu : 0); return 0; }
		FAIL(c, "illegal %04x", w0);
	case 0xB4: case 0xB5: case 0xB6: case 0xB7: {	/* adc sbc */
		int sz = (hi & 1) ? 2 : 1, sub = hi >= 0xB6;
		uint32_t r = addsub(c, rreg(c, n3, sz), rreg(c, n2, sz), !!(c->fl & F_C), sz, sub);
		wreg(c, n3, sz, r);
		return 0; }
	case 0xB2: case 0xB3: {
		int sz = hi == 0xB2 ? 1 : 2;
		switch (n3) {
		case 0: case 2: case 4: case 6: case 8: case 0xA: case 0xC: case 0xE: {
			int left = !(n3 & 4), thru = !!(n3 & 8), cnt = (n3 & 2) ? 2 : 1;
			wreg(c, n2, sz, rotate(c, rreg(c, n2, sz), sz, left, cnt, thru));
			return 0; }
		case 1: case 9: case 5: case 0xD: {
			int s2 = sz, arith = !!(n3 & 8);
			int16_t cnt;
			if (n3 & 4) { if (hi == 0xB2) FAIL(c, "illegal %04x", w0); s2 = 4; }
			cnt = (int16_t)fetch(c);
			wreg(c, n2, s2, shiftn(c, rreg(c, n2, s2), s2, cnt, arith));
			return 0; }
		case 3: case 0xB: case 7: case 0xF: {
			int s2 = sz, arith = !!(n3 & 8);
			uint16_t w1 = fetch(c);
			int cnt;
			if (n3 & 4) { if (hi == 0xB2) FAIL(c, "illegal %04x", w0); s2 = 4; }
			cnt = (int8_t)(c->r[(w1 >> 8) & 15] & 0xFF);
			wreg(c, n2, s2, shiftn(c, rreg(c, n2, s2), s2, cnt, arith));
			return 0; }
		}
		FAIL(c, "illegal %04x", w0); }
	case 0xB8: {
		uint16_t w1 = fetch(c);
		int rn = (w1 >> 8) & 15, dn = n2, tn = (w1 >> 4) & 15, test = !!(n3 & 2), dec = !!(n3 & 8), rep = !!(n3 & 4);
		uint16_t *dp = offreg(c, dn);
		Ad t = ir_ad(c, tn);
		do {
			Ad da = ir_ad(c, dn);
			uint8_t b = rdb(c, da);
			uint8_t x = rdb(c, adda(t, b));
			if (test) W8(c, 1, x);	/* rh1 */
			else wrb(c, da, x);
			*dp += dec ? -1 : 1;
			c->r[rn]--;
			if (test && x) break;
		} while (rep && c->r[rn] != 0);
		sf(c, F_V, c->r[rn] == 0 ? F_V : 0);
		if (test) sf(c, F_Z, R8(c, 1) == 0 ? F_Z : 0);
		return 0; }
	case 0xBA: case 0xBB: {
		int sz = hi == 0xBB ? 2 : 1;
		uint16_t w1 = fetch(c);
		int rn = (w1 >> 8) & 15, dn = (w1 >> 4) & 15, cc = w1 & 15;
		switch (n3) {
		case 1: case 9: return blk_ld(c, sz, n3 == 9, cc == 0, dn, n2, rn);
		case 0: case 4: case 8: case 0xC:
			return blk_cp(c, sz, !!(n3 & 8), !!(n3 & 4), dn, n2, rn, cc, 0);
		case 2: case 6: case 0xA: case 0xE:
			return blk_cp(c, sz, !!(n3 & 8), !!(n3 & 4), dn, n2, rn, cc, 1);
		}
		FAIL(c, "illegal %04x", w0); }
	case 0xBC: case 0xBE: {		/* rrdb / rldb */
		uint8_t a = R8(c, n3), b = R8(c, n2), na, nb;
		if (hi == 0xBE) { na = (a & 0xF0) | (b >> 4); nb = (b << 4) | (a & 0x0F); }
		else { na = (a & 0xF0) | (b & 0x0F); nb = (a << 4) | (b >> 4); }
		W8(c, n3, na); W8(c, n2, nb);
		logf_(c, na, 1);
		return 0; }
	case 0xBD: c->r[n2] = n3; return 0;	/* ldk */
	case 0xAE: case 0xAF: break;
	}

	switch (hi) {
	case 0x0E: case 0x0F: case 0x4E: case 0x4F: case 0x8E: case 0x8F:
		return z8k_epu(c, w0);	/* trap to the kernel's emulator */
	}
	FAIL(c, "illegal instruction %04x", w0);
}

void z8k_dump(Z8K *c, FILE *f)
{
	int i;
	fprintf(f, "pc %02x:%04x fl %02x\n", c->pcseg, c->pc, c->fl);
	for (i = 0; i < 16; i++) fprintf(f, "r%-2d %04x%s", i, c->r[i], (i & 3) == 3 ? "\n" : "  ");
}
