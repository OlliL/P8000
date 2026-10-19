/* sobj - see sobj.h. goodmagic, swab and longswap follow WEGA's libc (gen/goodmagic.c, gen/swap.c). */
#include "sobj.h"

int sobj_swap_flg;

unsigned
sobj_be16(const unsigned char *p)
{
	return p[0] << 8 | p[1];
}

uint32_t
sobj_be32(const unsigned char *p)
{
	return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

void
sobj_put16(unsigned char *p, unsigned v)
{
	p[0] = v >> 8;
	p[1] = v;
}

void
sobj_put32(unsigned char *p, uint32_t v)
{
	p[0] = v >> 24;
	p[1] = v >> 16;
	p[2] = v >> 8;
	p[3] = v;
}

static unsigned
swap16(unsigned x)
{
	return (x & 0xFF) << 8 | (x >> 8 & 0xFF);
}

int
sobj_goodmagic(unsigned magic)
{
	static const unsigned magics[10] = {
		0x0000,
		N_MAGIC1, N_MAGIC3, N_MAGIC4,
		S_MAGIC1, S_MAGIC3, S_MAGIC4,
		X_MAGIC1, X_MAGIC3, X_MAGIC4
	};
	unsigned m[10];
	int ret, i;

	for (i = 0; i < 10; i++)
		m[i] = magics[i];
	magic &= 0xFFFF;
	sobj_swap_flg = 0;
	do {
		m[0] = magic;
		ret = 10;
		do
			ret--;
		while (magic != m[ret]);
		if (ret)
			break;
		magic = swap16(magic);
	} while (!sobj_swap_flg++);
	if (!ret)
		sobj_swap_flg = 0;
	return ret;
}

void
sobj_swab(unsigned char *p, int n)
{
	int i;
	unsigned char t;

	for (i = 0; i + 1 < n; i += 2) {
		t = p[i];
		p[i] = p[i+1];
		p[i+1] = t;
	}
}

void
sobj_longswap(unsigned char *p)
{
	sobj_swab(p, 2);
	sobj_swab(p + 2, 2);
}
