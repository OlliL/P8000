/*
 * gen - the code of pass 2
 *
 * Bytes, words and longs are written to the object file and their relocation
 * information to the relocation file; with -l they are also given to the listing.
 * Words are written with the high byte first, as the Z8000 holds them.
 */
#include "cas.h"

static int addrflag = 0;		/* an address is being generated (genaddr) */

/* a word to the object file or the relocation file */
static void
putword(FILE *f, int w, const char *name)
{
	if (putc((w >> 8) & 0xFF, f) == EOF || putc(w & 0xFF, f) == EOF) {
		perror(name);
		done(-1);
	}
}

/* n bytes c; a is the relocation word */
void
genb(int c, int a, unsigned n)
{
	unsigned i;

	if (cursub == 2)
		error(2, "Bss cannot be initialized\n");
	if (subsw)
		seekout();
	n &= 0xFFFF;
	for (i = 0; i < n; i++) {
		if (putc((unsigned char)c, outf) == EOF) {
			perror(outname);
			done(-1);
		}
		if ((lctab[cursec][cursub] & 1) != 0)
			putword(relf, a, rfname);
		if (lflag)
			lstb(c, a);
		locadv(1);
	}
}

/* n words w */
void
genw(int w, int a, unsigned n)
{
	unsigned i;

	if (cursub == 2)
		error(2, "Bss cannot be initialized\n");
	if (subsw)
		seekout();
	n &= 0xFFFF;
	for (i = 0; i < n; i++) {
		putword(outf, w, outname);
		putword(relf, a, rfname);
		if (lflag)
			lstw(w, a);
		locadv(2);
	}
}

/* n longs l, as two words */
void
genl(int32_t l, int32_t a, unsigned n)
{
	unsigned i;

	n &= 0xFFFF;
	for (i = 0; i < n; i++) {
		genw((int)((uint32_t)l >> 16), (int)((uint32_t)a >> 16), 1);
		genw((int)l, (int)a, 1);
	}
}

/* a long operand: a number, a name or an expression */
void
genlop(NODE *np, int flag)
{
	int idx;
	int32_t v, r;

	switch (np->n_type) {
	case 3:
		if (segmode != 0 && addrflag != 0)
			np->n_val = (uint32_t)np->n_val | 0x80000000u;
		genl(np->n_val, 0, 1);
		break;
	case 7:
		idx = np->n_w;
		r = symreloc(idx);
		v = symval(idx);
		if (segmode != 0 && flag != 0)
			v = (uint32_t)v | 0x80000000u;
		genl(v, r, 1);
		break;
	case 0x19:
		v = exprval(np);
		if (segmode != 0 && flag != 0)
			v = (uint32_t)v | 0x80000000u;
		r = exprreloc(np);
		if (shortop == 0)
			genl(v, r, 1);
		else
			genw((int)v, (int)r, 1);
		shortop = 0;
		break;
	default:
		error(2, "Must be number or expression\n");
	}
}

/* a word operand */
void
genwop(NODE *np, int flag)
{
	int32_t v, r;
	int idx;

	v = r = 0;
	switch (np->n_type) {
	case 2:
	case 3:
	case 0x13:
		v = np->n_val;
		r = 0;
		break;
	case 7:
	case 8:
		idx = np->n_w;
		r = symreloc(idx);
		v = symval(idx);
		if (segmode != 0 && flag != 0)
			v |= 0x8000;
		break;
	case 0x18:
	case 0x19:
		v = exprval(np);
		if (segmode != 0 && flag != 0)
			v |= 0x8000;
		r = exprreloc(np);
		break;
	default:
		error(2, "Must be number or expression\n");
	}
	rangechk(v, 0xFFFF0000);
	genw((int)v, (int)r, 1);
	shortop = 0;
}

/* an address */
void
genaddr(NODE *np)
{
	addrflag++;
	if (segmode != 0)
		genlop(np, 1);
	else
		genwop(np, 1);
	addrflag = 0;
}

/* a byte operand */
void
genbop(NODE *np, int flag)
{
	int32_t v, r;
	int idx;

	v = 0;
	switch (np->n_type) {
	case 1:
	case 3:
	case 0x13:
		v = np->n_val;
		break;
	case 7:
	case 9:
		idx = np->n_w;
		v = symval(idx);
		r = symreloc(idx);
		if ((int16_t)r != 0)
			error(2, "Bad relocation bits\n");
		/* fall through: a name is not accepted here (as in the original) */
	default:
		error(2, "Must be number or relative address\n");
	}
	rangechk(v, 0xFFFFFF00);
	if (flag != 0)
		putbits(8, (int16_t)v & 0xFF);
	else
		genb((unsigned char)v, 0, 1);
}

/* position the output and the relocation file at the location counter of a new section or subsection */
void
seekout(void)
{
	long base;

	subsw = 0;
	switch (cursub) {
	case 1:
		base = seektab[cursec].sk_end;
		break;
	case 0:
		base = seektab[cursec].sk_off;
		break;
	default:
		return;
	}
	if (fseek(outf, base + (uint16_t)imoff + lctab[cursec][cursub], SEEK_SET) == -1) {
		perror(outname);
		done(-1);
	}
	if (fseek(relf, base + (lctab[cursec][cursub] & ~1L), SEEK_SET) == -1) {
		perror(rfname);
		done(-1);
	}
}
