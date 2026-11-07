static char sccsid[] __attribute__((unused)) = "@[$]strip.c\t1.2  11/19/82 14:08:39 - 87wega3.2";

/*
 * strip - remove symbols and relocation information (and with -h the
 * file header and the segment table) from an s.out file
 *
 * Host version (Linux, gcc): made from WEGA/src/cmd/strip.c by the port rules in
 * host/HANDOFF_wega_disasm.md. The file structures are byte arrays in the Z8000 layout
 * (see sobj.h); the swab and longswap calls are those of the original, applied to the
 * raw bytes, so objects with swapped bytes are handled the same way.
 *
 * Not ported: rcopy (8-bit objects, magic E507, E511, E505, when the relocation is
 * dropped). Its result depends on uninitialised variables of the original (the low byte of
 * c and x), and no such file exists in the system; the host version refuses the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "sobj.h"

unsigned char head[SEXEC_SIZE];
unsigned char segt[65536 + SEGT_SIZE];		/* 4095 entries at most; the original has 256 */
FILE *tf;
uint32_t imsize;
unsigned segtsz;

static int strip(char *name, int hflag);
static int copy(char *name, FILE *fr, FILE *to, uint32_t size);

#define IS8BIT(m)	((m) == X_MAGIC1 || (m) == X_MAGIC3 || (m) == X_MAGIC4)

int
main(int argc, char **argv)
{
	int status = 0;
	int hflag = 0;		/* uninitialised in the original when no option is given */
	int i;

	if(argc < (argc > 1 && *argv[1] == '-' ? 3 : 2)) {
		fprintf(stderr, "Usage: strip [-h] file-name ... \n");
		exit(2);
	}
	if(*argv[1] == '-') {
		if(!(hflag = (strcmp(argv[1], "-h") == 0))) {
			fprintf(stderr, "strip: illegal option: %s\n", argv[1]);
			exit(2);
		}
	}
	tf = tmpfile();
	if(tf == NULL) {
		fprintf(stderr, "strip: cannot create temporary file\n");
		exit(2);
	}
	for(i = (*argv[1] == '-' ? 2 : 1); i < argc; i++) {
		status = strip(argv[i], hflag);
		if(status > 1)
			break;
	}
	fclose(tf);
	exit(status);
}

static int
strip(char *name, int hflag)
{
	FILE *f;
	unsigned char magb[2];
	unsigned mag;
	unsigned flag;
	uint32_t size;
	unsigned char ib[4], sb[2];
	size_t got;		/* the results of fread are not used by the original; gcc insists on it */

	f = fopen(name, "r");
	if(f == NULL) {
		fprintf(stderr, "strip: unable to open %s\n", name);
		return(1);
	}
	magb[0] = magb[1] = 0;
	got = fread(magb, 1, 2, f);
	(void)got;
	mag = sobj_be16(magb);
	fseek(f, 0L, 0);
	if(!sobj_goodmagic(mag)) {
		fprintf(stderr, "strip: %s not in correct object format\n", name);
		fclose(f);
		return(1);
	}
	if(sobj_swap_flg)
		sobj_swab(magb, 2);
	mag = sobj_be16(magb);
	memset(head, 0, sizeof head);
	got = fread(head, 1, SEXEC_SIZE, f);
	(void)got;
	flag = 1;
	if(sobj_swap_flg)
		flag = 0x0100;			/* swab of the word 1, as the head is still as in the file */
	if(sobj_be16(head + 12) == 0 && (sobj_be16(head + 18) & flag) == flag && hflag == 0) {
		fprintf(stderr, "strip: %s already stripped\n", name);
		fclose(f);
		return(1);
	}
	fseek(tf, 0L, 0);
	memcpy(ib, head + 2, 4);		/* s_imsize */
	imsize = sobj_be32(ib);
	memcpy(sb, head + 10, 2);		/* s_segt */
	segtsz = sobj_be16(sb);
	if(sobj_swap_flg) {
		sobj_longswap(ib);
		imsize = sobj_be32(ib);
		sobj_swab(sb, 2);
		segtsz = sobj_be16(sb);
	}
	memset(segt, 0, sizeof segt);
	got = fread(segt, SEGT_SIZE, segtsz >> 4, f);
	(void)got;
	if(sobj_swap_flg)
		sobj_swab(segt, segtsz);
	if(hflag == 0) {
		uint32_t code = sobj_be16(segt + 4), data = sobj_be16(segt + 6);

		sobj_put16(head + 12, 0);					/* s_syms */
		sobj_put16(head + 18, sobj_be16(head + 18) | flag);		/* s_flag */
		if(IS8BIT(mag)) {
			unsigned char l[4];

			sobj_put32(l, code + data);
			sobj_put16(head + 20, code);				/* s_codesz */
			if(sobj_swap_flg)
				sobj_longswap(l);
			memcpy(head + 2, l, 4);					/* s_imsize */
		}
		if(fwrite(head, 1, SEXEC_SIZE, tf) != SEXEC_SIZE) {
			fprintf(stderr, "strip: write error\n");
			return(2);
		}
		if(IS8BIT(mag))
			size = (uint32_t)((code + data) & 0xFFFF) + (uint32_t)segtsz + 24L;
		else
			size = (uint32_t)segtsz + imsize + 24L;
		if(sobj_swap_flg)
			sobj_swab(segt, segtsz);
		if(fwrite(segt, SEGT_SIZE, segtsz >> 4, tf) != segtsz >> 4) {
			fprintf(stderr, "strip: write error\n");
			return(2);
		}
	} else
		size = imsize;
	if(IS8BIT(mag) && hflag == 0) {
		fprintf(stderr, "strip: %s: 8-bit objects are not supported by the host version\n", name);
		fclose(f);
		return(2);
	} else if(copy(name, f, tf, imsize)) {
		fclose(f);
		return(2);
	}
	fclose(f);
	f = fopen(name, "w");
	if(f == NULL) {
		fprintf(stderr, "strip: cannot recreate %s\n", name);
		return(1);
	}
	fseek(tf, 0L, 0);
	if(copy(name, tf, f, size)) {
		fclose(f);
		return(2);
	}
	fclose(f);		/* the original leaves it open: the data is there at once for a second use of the file */
	return(0);
}

static int
copy(char *name, FILE *fr, FILE *to, uint32_t size)
{
	int s, n;
	char buf[512];

	signal(1, SIG_IGN);
	signal(2, SIG_IGN);
	signal(3, SIG_IGN);
	while(size != 0) {
		s = size > 512 ? 512 : size;
		n = fread(buf, 1, s, fr);
		if(s != n) {
			fprintf(stderr, "strip: unexpected eof on %s\n", name);
			return(1);
		}
		if((int)fwrite(buf, 1, n, to) != n) {
			fprintf(stderr, "strip: write error on %s\n", name);
			return(1);
		}
		size -= n;
	}
	signal(1, SIG_DFL);
	signal(2, SIG_DFL);
	signal(3, SIG_DFL);
	return(0);
}
