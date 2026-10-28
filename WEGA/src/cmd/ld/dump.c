/*
 * dump.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

/* walk_entries..0220: done */
walk_entries()
{
	register char *a;
	register char *b;

	printf("x");
	b = table16_entry(bE4FA);
	a = table16_entry(0);
	while (a < b) {
		dump_sym(a);
		a = next_entry(a);
	}
}

/* dump_sym..0334: done */
int dump_sym(p)
register char *p;
{
	register char *s;
	int k;
	int n;

	if (p == 0)
		return 0;
	put_name(p + 8);
	printf("\tvalue 0x%lx, segt 0x%x, ", *(long *)p, *(int *)(p + 6) & 0xff);
	if (p[4] & 0x20)
		printf("EXT+");
	switch (p[4] & 0x1f) {
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
	if (p[8] & 0x80) {
		n = (p[8] & 0x7f) - 8;
		p += 16;
		k = 1;
		while (n > 0) {
			k++;
			n -= 16;
		}
		return k;
	}
	return 1;
}

/* dmp_p1..03fc: done */
int dmp_p1()
{
	register char *p;
	register int i;
	int z;

	printf("\nPASS ONE SEG TABLE:\n");
	for (i = 0; i < bEF32C; i++) {
		p = get_block(i);
		printf("P1 %d:\t%d\n", i, p[0]);
		printf("\ts_code, s_data, s_bss:\t0x%x 0x%x 0x%x\n", *(int *)(p + 4), *(int *)(p + 6), *(int *)(p + 8));
		printf("\tctrel, cdrel, cbrel:\t0x%x 0x%x 0x%x\n", *(int *)(p + 0x10), *(int *)(p + 0x12), *(int *)(p + 0x14));
		printf("\tncoff, ndoff, nboff:\t0x%x 0x%x 0x%x\n", *(int *)(p + 0x22), *(int *)(p + 0x24), *(int *)(p + 0x26));
		printf("\tncode, ndata, nbss:\t0x%x 0x%x 0x%x\n", *(long *)(p + 0x16), *(long *)(p + 0x1a), *(long *)(p + 0x1e));
		printf("\txtnext,datnext,bssnext: %d %d %d \n", *(int *)(p + 0x30), *(int *)(p + 0x32), *(int *)(p + 0x34));
	}
}

/* dump_p2..0566: done */
int dump_p2()
{
	register char *p;

	printf("\nPASS TWO SEG TABLE:\n");
	for (p = (char *)bF32E; p; p = *(char **)(p + 0x12)) {
		printf("P2 %d:\t segno %d\n", *(int *)(p + 0x10) & 0xff, p[0] & 0xff);
		printf("\ts_coff, s_doff, s_boff:\t0x%x 0x%x 0x%x\n", p[1] & 0xff, p[2] & 0xff, p[3] & 0xff);
		printf("\ts_code, s_data, s_bss:\t0x%x 0x%x 0x%x\n", *(int *)(p + 4), *(int *)(p + 6), *(int *)(p + 8));
		printf("\ttorig, dorig, borig:\t0x%x 0x%x 0x%x\n", *(int *)(p + 0x1a), *(int *)(p + 0x1c), *(int *)(p + 0x1e));
		printf("\tatr: ");
		if (*(int *)(p + 0xa) & 1)
			printf(" SG_CODE");
		if (*(int *)(p + 0xa) & 2)
			printf(" SG_DATA");
		if (*(int *)(p + 0xa) & 4)
			printf(" SG_BSS");
		if (*(int *)(p + 0xa) & 0x10)
			printf(" SG_OCODE");
		if (*(int *)(p + 0xa) & 0x20)
			printf(" SG_ODATA");
		if (*(int *)(p + 0xa) & 0x40)
			printf(" SG_OBSS");
		if (*(int *)(p + 0xa) & 8)
			printf(" SG_STACK");
		if (*(int *)(p + 0xa) & 0x100)
			printf(" SG_RESERVE");
		printf("\n");
	}
}

/* dump_res..06c6: done */
int dump_res()
{
	int k;
	int j;

	printf("RESLIST DUMP\n");
	printf("t,d,b0 and 1 %d %d %d %d %d %d\n", tblF332[0].a, tblF332[1].a, tblF332[0].b, tblF332[1].b, tblF332[0].c, tblF332[1].c);
	printf(" res 0 1 %d %d", tblF332[0].d, tblF332[1].d);
	k = 2;
	do {
		printf("\n txtptr");
		for (j = 0; j < 9; j++)
			printf("  %d ", tblF332[j + k].a);
		printf("\n datptr");
		for (j = 0; j < 9; j++)
			printf("  %d", tblF332[j + k].b);
		printf("\n bssptr");
		for (j = 0; j < 9; j++)
			printf("  %d", tblF332[j + k].c);
		printf("\n res");
		for (j = 0; j < 9; j++)
			printf("  %d", tblF332[j + k].d);
		k = k + 9;
	} while (k < 128);
}
