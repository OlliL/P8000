static char sccsid[] __attribute__((unused)) = "@[$]size.c\t2.1  01/14/83 20:29:03 - 87wega3.2";

/*
 * Host version (Linux, gcc): made from WEGA/src/cmd/size.c by the port rules in
 * host/HANDOFF_wega_disasm.md. The file structures are byte arrays in the Z8000 layout
 * (big endian, no padding; see sobj.h) and are read with the accessors; the swab of
 * objects with swapped bytes works on the raw bytes as in the original.
 */

#include <stdio.h>
#include <string.h>
#include "sobj.h"

unsigned char segt[65536];		/* 4095 entries at most (s_segt >> 4); the original has 256 */

int
main(int argc, char **argv)
{
	unsigned char buf[SEXEC_SIZE];
	uint32_t ttext, tdata, tbss, sum;
	unsigned n;
	int gorp, i;
	FILE *f;
	size_t got;		/* the result of fread is not used by the original; gcc insists on it being used */

	if(argc == 1) {
		*argv = "a.out";
		argc++;
		--argv;
	}
	gorp = argc;
	while(--argc) {
		++argv;
		ttext = 0;
		tdata = 0;
		tbss = 0;
		if((f = fopen(*argv, "r")) == NULL) {
			fprintf(stderr, "size: cannot open %s\n", *argv);
			continue;
		}
		memset(buf, 0, sizeof buf);
		got = fread(buf, 1, sizeof(buf), f);
		(void)got;
		if(!sobj_goodmagic(sobj_be16(buf))) {
			fprintf(stderr, "size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		if(sobj_swap_flg)
			sobj_swab(buf, sizeof(buf));
		n = sobj_be16(buf + 10) >> 4;
		memset(segt, 0, sizeof segt);
		got = fread(segt, n << 4, 1, f);
		(void)got;
		if(sobj_swap_flg)
			sobj_swab(segt, n << 4);
		if(gorp > 2)
			printf("%s:\n", *argv);
		for(i = 0; i <= (int)n - 1; i++) {
			unsigned char *s = segt + i * SEGT_SIZE;

			sum = (uint32_t)sobj_be16(s + 4) + sobj_be16(s + 6) + sobj_be16(s + 8);
			ttext += sobj_be16(s + 4);
			tdata += sobj_be16(s + 6);
			tbss += sobj_be16(s + 8);
		}
		printf("%u + %u + %u = ", ttext, tdata, tbss);
		sum = ttext + tdata + tbss;
		printf("%u = 0x%x\n", sum, sum);
		fclose(f);
	}
	return 0;
}
