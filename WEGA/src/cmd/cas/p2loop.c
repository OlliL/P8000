static char sccsid[] = "@[$]p2loop.c\t1.2 05/17/83 17:30:47 - 87wega3.2";

/*
 * p2loop - the loop of pass 2
 *
 * Reads the intermediate file written by pass 1 (the tags are listed in ifile.c) and
 * calls the code generator for every record.  A record with the tag 6 repeats the
 * element that follows it (p2rep).
 */
#include "cas.h"

int subsw = 0;				/* the subsection was changed (gen.c) */
int savelflag = 0;

extern NODE *fn7FEE();			/* read an operand (getops.c) */

pass2()
{
	unsigned char c;

	symadjust();
	ohdrinit();
	osymout();
	ohdrput();
	cursec = 0;
	cursub = 1;
	curline = 1;
	setsub(1);
	subsw++;
	while ((c = igetb()) != 0xF0) {
		p2tag(c);
		chknodes();
	}
	if (lflag) {
		curline++;
		fn94D4(curline);
		fn9638();
	}
	if (semerr == 0)
		ofinish();
}

/* one record of the intermediate file */
p2tag(tag)
unsigned char tag;
{
	long lv;
	register int x;
	register NODE *np;
	register long l;

	switch (tag) {
	case 15:
		curline = igetw();
		if (lflag)
			fn94D4(curline);
		break;
	case 16:
		curline++;
		if (lflag)
			fn94D4(curline);
		break;
	case 17:
		curline += 2;
		if (lflag)
			fn94D4(curline);
		break;
	case 18:
		curline += 3;
		if (lflag)
			fn94D4(curline);
		break;
	case 11:
		x = igetw() + lcaux[cursec][cursub].la_0;
		if (cursub != 2) {
			if (lflag) {
				savelflag = lflag;
				lflag = 0;
			}
			fn6DA6(0, 0, x - (int)lctab[cursec][cursub]);
			if (savelflag)
				lflag = savelflag;
		} else
			lctab[cursec][cursub] = (unsigned)x;
		break;
	case 1:
		fn6DA6(igetb(), 0, 1);
		break;
	case 2:
		fn6E9A(igetw(), 0, 1);
		break;
	case 3:
		fn6F66(igetl(), 0L, 1);
		break;
	case 10:
		fn742A(igetaddr());
		break;
	case 12:
		setsub(0);
		subsw++;
		break;
	case 14:
		setsub(2);
		subsw++;
		break;
	case 41:
		setsec(igetb());
		setsub(1);
		subsw++;
		break;
	case 13:
		setsub(1);
		subsw++;
		break;
	case 6:
		p2rep(igetw());
		break;
	case 7:
		np = mknode7(igetw());
		fn6FBA(np, 0);
		nfreed++;
		break;
	case 8:
		np = mknode7(igetw());
		fn70B0(np, 0);
		nfreed++;
		break;
	case 9:
		np = mknode7(igetw());
		fn71C2(np, 0);
		nfreed++;
		break;
	case 23:
		np = fn7FEE();
		fn71C2(np, 0);
		nfreed++;
		break;
	case 24:
		np = fn7FEE();
		fn70B0(np, 0);
		nfreed++;
		break;
	case 25:
		np = fn7FEE();
		fn6FBA(np, 0);
		nfreed++;
		break;
	case 20:
		igetw();
		break;
	case 38:
	case 39:
		fn89C2(tag);
		break;
	default:
		error(2, "Unknown tag in intermediate file\n");
	}
}

/* a record that is repeated n times */
p2rep(n)
unsigned n;
{
	register int x;
	register int z;
	register unsigned i;
	register long l;
	unsigned char b;
	unsigned char c;
	NODE *np;
	int w;

	c = igetb();
	switch (c) {
	case 1:
		b = igetb();
		z = 0;
		fn6DA6(b, z, n);
		break;
	case 2:
		x = igetw();
		z = 0;
		fn6E9A(x, z, n);
		break;
	case 3:
		l = igetl();
		z = 0;
		fn6F66(l, (long)(unsigned)z, n);
		break;
	case 7:
	case 8:
	case 9:
	case 23:
	case 24:
	case 25:
		switch (c) {
		case 7:
		case 8:
		case 9:
			w = igetw();
			np = mknode7(w);
			break;
		case 23:
		case 24:
		case 25:
			np = fn7FEE();
			break;
		}
		for (i = 0; i < n; i++) {
			switch (c) {
			case 8:
			case 24:
				fn70B0(np, 0);
				break;
			case 7:
			case 25:
				fn6FBA(np, 0);
				break;
			case 9:
			case 23:
				fn71C2(np, 0);
				break;
			}
		}
		nfreed = nalloc;
		break;
	default:
		error(2, "Unknown tag in intermediate file\n");
	}
}
