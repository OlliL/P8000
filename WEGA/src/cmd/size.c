static char sccsid[] = "@[$]size.c\t2.1  01/14/83 20:29:03 - 87wega3.2";

#include <stdio.h>
#include <s.out.h>

extern int swap_flg;
struct segt segt[256];

main(argc, argv)
int argc;
char **argv;
{
	struct s_exec buf;
	long ttext, tdata, tbss, sum;
	unsigned n;
	int gorp, i;
	FILE *f;

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
		fread((char *)&buf, sizeof(buf), 1, f);
		if(!goodmagic(buf.s_magic)) {
			fprintf(stderr, "size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		if(swap_flg)
			swab((char *)&buf, (char *)&buf, sizeof(buf));
		n = buf.s_segt >> 4;
		fread((char *)segt, n << 4, 1, f);
		if(swap_flg)
			swab((char *)segt, (char *)segt, n << 4);
		if(gorp > 2)
			printf("%s:\n", *argv);
		for(i = 0; i <= n - 1; i++) {
			sum = (long)segt[i].sg_code + (long)segt[i].sg_data + (long)segt[i].sg_bss;
			ttext += segt[i].sg_code;
			tdata += segt[i].sg_data;
			tbss += segt[i].sg_bss;
		}
		printf("%U + %U + %U = ", ttext, tdata, tbss);
		sum = ttext + tdata + tbss;
		printf("%U = 0x%lx\n", sum, sum);
		fclose(f);
	}
}
