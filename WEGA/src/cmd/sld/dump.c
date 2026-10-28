/*
 * dump.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]dump.c\t3.1  10/27/83 12:14:51 - 87wega3.2";

/* walk_entries: dump all entries of the symbol table (SYM TABLE DUMP) (0x01d2..0x0220 of /bin/sld) */
walk_entries()
{
	register struct symbol *a;
	register struct symbol *b;

	printf("\nSYM TABLE DUMP:\n");
	b = table16_entry(symindex);
	a = table16_entry(0);
	while (a < b) {
		dump_sym(a);
		a = next_entry(a);
	}
}

/* dump_sym: dump one symbol (value, segment type, kind) (0x0220..0x0334 of /bin/sld) */
int dump_sym(p)
register struct symbol *p;
{
	register char *s;
	int k;
	int n;

	if (p == 0)
		return 0;
	put_name(p->sname);
	printf("\tvalue 0x%lx, segt 0x%x, ", p->svalue, p->sseg & 0xff);
	if (p->stype & 0x20)
		printf("EXT+");
	switch (p->stype & 0x1f) {
	case 0:
		s = "UNDF";
		break;
	case 1:
		s = "ABS";
		break;
	case 4:
		s = "BSS";
		break;
	case 3:
		s = "DATA";
		break;
	case 2:
		s = "CODE";
		break;
	case 0x1f:
		s = "FN";
		break;
	case 0x1e:
		s = "SEC";
		break;
	case 5:
		s = "COMM";
		break;
	default:
		s = "";
	}
	printf("%s\n", s);
	if (p->sname[0] & 0x80) {
		n = (p->sname[0] & 0x7f) - 8;
		p++;
		k = 1;
		while (n > 0) {
			k++;
			n -= 16;
		}
		return k;
	}
	return 1;
}

/* dmp_p1: dump the pass-one segment table (0x0334..0x03fc of /bin/sld) */
int dmp_p1()
{
	register struct p1seg *p;
	register int i;
	int z;

	printf("\nPASS ONE SEG TABLE:\n");
	for (i = 0; i < nblk; i++) {
		p = get_block(i);
		printf("P1 %d:\t%d\n", i, p->segno);
		printf("\ts_code, s_data, s_bss:\t0x%x 0x%x 0x%x\n", p->s_code, p->s_data, p->s_bss);
		printf("\tctrel, cdrel, cbrel:\t0x%x 0x%x 0x%x\n", p->ctrel, p->cdrel, p->cbrel);
		printf("\tncoff, ndoff, nboff:\t0x%x 0x%x 0x%x\n", p->ncoff, p->ndoff, p->nboff);
		printf("\tncode, ndata, nbss:\t0x%x 0x%x 0x%x\n", p->ncode, p->ndata, p->nbss);
		printf("\txtnext,datnext,bssnext: %d %d %d \n", p->xtnext, p->datnext, p->bssnext);
	}
}

/* dump_p2: dump the pass-two segment table (0x03fc..0x0566 of /bin/sld) */
int dump_p2()
{
	register struct p2seg *p;

	printf("\nPASS TWO SEG TABLE:\n");
	for (p = p2head; p; p = p->nxt) {
		printf("P2 %d:\t segno %d\n", p->segidx & 0xff, p->segno & 0xff);
		printf("\ts_coff, s_doff, s_boff:\t0x%x 0x%x 0x%x\n", p->s_coff & 0xff, p->s_doff & 0xff, p->s_boff & 0xff);
		printf("\ts_code, s_data, s_bss:\t0x%x 0x%x 0x%x\n", p->s_code, p->s_data, p->s_bss);
		printf("\ttorig, dorig, borig:\t0x%x 0x%x 0x%x\n", p->torig, p->dorig, p->borig);
		printf("\tatr: ");
		if (p->atr & 1)
			printf(" SG_CODE");
		if (p->atr & 2)
			printf(" SG_DATA");
		if (p->atr & 4)
			printf(" SG_BSS");
		if (p->atr & 0x10)
			printf(" SG_OCODE");
		if (p->atr & 0x20)
			printf(" SG_ODATA");
		if (p->atr & 0x40)
			printf(" SG_OBSS");
		if (p->atr & 8)
			printf(" SG_STACK");
		if (p->atr & 0x100)
			printf(" SG_RESERVE");
		printf("\n");
	}
}

/* dump_res: dump the list of reserved segments (0x0566..0x06c6 of /bin/sld) */
int dump_res()
{
	int k;
	int j;

	printf("RESLIST DUMP\n");
	printf("t,d,b0 and 1 %d %d %d %d %d %d\n", reslist[0].txtptr, reslist[1].txtptr, reslist[0].datptr, reslist[1].datptr, reslist[0].bssptr, reslist[1].bssptr);
	printf(" res 0 1 %d %d", reslist[0].res, reslist[1].res);
	k = 2;
	do {
		printf("\n txtptr");
		for (j = 0; j < 9; j++)
			printf("  %d ", reslist[j + k].txtptr);
		printf("\n datptr");
		for (j = 0; j < 9; j++)
			printf("  %d", reslist[j + k].datptr);
		printf("\n bssptr");
		for (j = 0; j < 9; j++)
			printf("  %d", reslist[j + k].bssptr);
		printf("\n res");
		for (j = 0; j < 9; j++)
			printf("  %d", reslist[j + k].res);
		k = k + 9;
	} while (k < 128);
}
