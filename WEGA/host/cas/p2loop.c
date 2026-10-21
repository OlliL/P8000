/*
 * p2loop - the loop of pass 2
 *
 * Reads the intermediate file written by pass 1 (the tags are listed in ifile.c) and
 * calls the code generator for every record. A record with the tag 6 repeats the
 * element that follows it (p2rep).
 */
#include "cas.h"

int subsw = 0;				/* the subsection was changed (gen.c) */
static int savelflag = 0;

static void p2rep(unsigned n);

void
pass2(void)
{
	int c;

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
		lstsync(curline);
		lstrest();
	}
	if (semerr == 0)
		ofinish();
}

/* one record of the intermediate file */
void
p2tag(int tag)
{
	int x;
	NODE *np;

	switch (tag) {
	case 15:
		curline = igetw();
		if (lflag)
			lstsync(curline);
		break;
	case 16:
		curline++;
		if (lflag)
			lstsync(curline);
		break;
	case 17:
		curline += 2;
		if (lflag)
			lstsync(curline);
		break;
	case 18:
		curline += 3;
		if (lflag)
			lstsync(curline);
		break;
	case 11:
		x = (int16_t)(igetw() + lcaux[cursec][cursub].la_0);
		if (cursub != 2) {
			if (lflag) {
				savelflag = lflag;
				lflag = 0;
			}
			genb(0, 0, (uint16_t)(x - (int16_t)lctab[cursec][cursub]));
			if (savelflag)
				lflag = savelflag;
		} else
			lctab[cursec][cursub] = (uint16_t)x;
		break;
	case 1:
		genb(igetb(), 0, 1);
		break;
	case 2:
		genw(igetw(), 0, 1);
		break;
	case 3:
		genl(igetl(), 0, 1);
		break;
	case 10:
		asmins(&itab1[igetl()]);
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
		p2rep((uint16_t)igetw());
		break;
	case 7:
		np = mknode7(igetw());
		genlop(np, 0);
		nfreed++;
		break;
	case 8:
		np = mknode7(igetw());
		genwop(np, 0);
		nfreed++;
		break;
	case 9:
		np = mknode7(igetw());
		genbop(np, 0);
		nfreed++;
		break;
	case 23:
		np = igetexpr();
		genbop(np, 0);
		nfreed++;
		break;
	case 24:
		np = igetexpr();
		genwop(np, 0);
		nfreed++;
		break;
	case 25:
		np = igetexpr();
		genlop(np, 0);
		nfreed++;
		break;
	case 20:
		igetw();
		break;
	case 38:
	case 39:
		jpr2(tag);
		break;
	default:
		error(2, "Unknown tag in intermediate file\n");
	}
}

/* a record that is repeated n times */
static void
p2rep(unsigned n)
{
	unsigned i;
	int c;
	NODE *np;

	c = igetb();
	switch (c) {
	case 1:
		genb(igetb(), 0, n);
		break;
	case 2:
		genw(igetw(), 0, n);
		break;
	case 3:
		genl(igetl(), 0, n);
		break;
	case 7:
	case 8:
	case 9:
	case 23:
	case 24:
	case 25:
		if (c < 10)
			np = mknode7(igetw());
		else
			np = igetexpr();
		for (i = 0; i < n; i++) {
			switch (c) {
			case 8:
			case 24:
				genwop(np, 0);
				break;
			case 7:
			case 25:
				genlop(np, 0);
				break;
			case 9:
			case 23:
				genbop(np, 0);
				break;
			}
		}
		nfreed = nalloc;
		break;
	default:
		error(2, "Unknown tag in intermediate file\n");
	}
}
