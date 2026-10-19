/*
 * size - print the sizes of the sections of an s.out file, host version of the
 * WEGA /bin/size (size.c 2.1 01/14/83); derived from WEGA/src/cmd/size.c.
 * The file structures are byte arrays in the Z8000 layout (see sobj.h).
 */
#include <stdio.h>
#include <string.h>
#include "sobj.h"

static unsigned char segt[65536];

int
main(int argc, char **argv)
{
	unsigned char buf[SEXEC_SIZE];
	uint32_t ttext, tdata, tbss, sum;
	unsigned n;
	int gorp, i;
	FILE *f;
	size_t got;

	if (argc == 1) {
		*argv = "a.out";
		argc++;
		--argv;
	}
	gorp = argc;
	while (--argc) {
		++argv;
		ttext = 0;
		tdata = 0;
		tbss = 0;
		if ((f = fopen(*argv, "r")) == NULL) {
			fprintf(stderr, "size: cannot open %s\n", *argv);
			continue;
		}
		memset(buf, 0, sizeof buf);
		fread(buf, 1, sizeof buf, f);
		if (!sobj_goodmagic(sobj_be16(buf))) {
			fprintf(stderr, "size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		if (sobj_swap_flg)
			sobj_swab(buf, sizeof buf);
		n = sobj_be16(buf + 10) >> 4;		/* s_segt: size of the segment table */
		memset(segt, 0, sizeof segt);
		got = fread(segt, 1, n << 4, f);
		(void)got;
		if (sobj_swap_flg)
			sobj_swab(segt, n << 4);
		if (gorp > 2)
			printf("%s:\n", *argv);
		for (i = 0; i < (int)n; i++) {
			unsigned char *s = segt + i * SEGT_SIZE;

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
