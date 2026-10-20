static char sccsid[] = "@[$]gen.c\t1.3  07/18/83 13:25:03 Copr. 1981 87wega3.2.";

/*
 * gen - the code of pass 2
 *
 * Bytes, words and longs are written to the object file and their relocation
 * information to the relocation file; with -l they are also given to the listing.
 */
#include "cas.h"

extern char *outname, *rfname;

int addrflag = 0;			/* an address is being generated (genaddr) */

/* n bytes c; a is the relocation word */
genb(c, a, n)
unsigned char c;
int a;
unsigned n;
{
	register unsigned i;

	if (cursub == 2)
		error(2, "Bss cannot be initialized\n");
	if (subsw)
		seekout();
	for (i = 0; i < n; i++) {
		if (fwrite(&c, 1, 1, outf) != 1) {
			perror(outname);
			done(-1);
		}
		if ((lctab[cursec][cursub] & 1) != 0) {
			if (fwrite(&a, 2, 1, relf) != 1) {
				perror(rfname);
				done(-1);
			}
		}
		if (lflag)
			fn941C(c, a);
		locadv(1);
	}
}

/* n words w */
genw(w, a, n)
int w;
int a;
unsigned n;
{
	register unsigned i;

	if (cursub == 2)
		error(2, "Bss cannot be initialized\n");
	if (subsw)
		seekout();
	for (i = 0; i < n; i++) {
		if (fwrite(&w, 2, 1, outf) != 1) {
			perror(outname);
			done(-1);
		}
		if (fwrite(&a, 2, 1, relf) != 1) {
			perror(rfname);
			done(-1);
		}
		if (lflag)
			fn93EC(w, a);
		locadv(2);
	}
}

/* n longs l, as two words */
genl(l, a, n)
long l;
long a;
unsigned n;
{
	register unsigned i;

	for (i = 0; i < n; i++) {
		genw((int)((unsigned long)l >> 16), (int)((unsigned long)a >> 16), 1);
		genw((int)l, (int)a, 1);
	}
}

/* a long operand: a number, a name or an expression */
genlop(np, flag)
NODE *np;
int flag;
{
	register int idx;
	register long v;
	register long r;

	switch (np->n_type) {
	case 3:
		if (segmode != 0 && addrflag != 0)
			np->n_u.l |= 0x80000000L;
		genl(np->n_u.l, 0L, 1);
		break;
	case 7:
		idx = np->n_u.pw.w;
		r = symreloc(idx);
		v = symval(idx);
		if (segmode != 0 && flag != 0)
			v |= 0x80000000L;
		genl(v, r, 1);
		break;
	case 0x19:
		v = exprval(np);
		if (segmode != 0 && flag != 0)
			v |= 0x80000000L;
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
genwop(np, flag)
NODE *np;
int flag;
{
	register long v;
	register long r;
	register int idx;
	unsigned char tag;

	tag = np->n_type;
	switch (tag) {
	case 2:
	case 3:
	case 0x13:
		v = np->n_u.l;
		r = 0;
		break;
	case 7:
	case 8:
		idx = np->n_u.pw.w;
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
	rangechk(v, 0xFFFF0000L);
	genw((int)v, (int)r, 1);
	shortop = 0;
}

/* an address */
genaddr(np)
NODE *np;
{
	addrflag++;
	if (segmode != 0)
		genlop(np, 1);
	else
		genwop(np, 1);
	addrflag = 0;
}

/* a byte operand */
genbop(np, flag)
NODE *np;
int flag;
{
	register long v;
	register long r;
	register int idx;
	unsigned char tag;

	tag = np->n_type;
	switch (tag) {
	case 1:
	case 3:
	case 0x13:
		v = np->n_u.l;
		break;
	case 7:
	case 9:
		idx = np->n_u.pw.w;
		v = symval(idx);
		r = symreloc(idx);
		if ((int)r != 0)
			error(2, "Bad relocation bits\n");
	default:
		error(2, "Must be number or relative address\n");
	}
	rangechk(v, 0xFFFFFF00L);
	if (flag != 0)
		putbits(8, (int)v & 0xFF);
	else
		genb((unsigned char)v, 0, 1);
}

/* position the output and the relocation file at the location counter of a new section or subsection */
seekout()
{
	subsw = 0;
	switch (cursub) {
	case 1:
		if (fseek(outf, seektab[cursec].sk_end + (unsigned)imoff + lctab[cursec][cursub], 0) == -1) {
			perror(outname);
			done(-1);
		}
		if (fseek(relf, seektab[cursec].sk_end + (lctab[cursec][cursub] & ~1L), 0) == -1) {
			perror(rfname);
			done(-1);
		}
		break;
	case 0:
		if (fseek(outf, seektab[cursec].sk_off + (unsigned)imoff + lctab[cursec][cursub], 0) == -1) {
			perror(outname);
			done(-1);
		}
		if (fseek(relf, seektab[cursec].sk_off + (lctab[cursec][cursub] & ~1L), 0) == -1) {
			perror(rfname);
			done(-1);
		}
		break;
	}
}
