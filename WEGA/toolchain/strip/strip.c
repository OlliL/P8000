static char sccsid[] = "@[$]strip.c\t1.2  11/19/82 14:08:39 - 87wega3.2";

/*
 * strip - remove symbols and relocation information (and with -h the
 * file header and the segment table) from an s.out file
 */

#include <stdio.h>
#include <s.out.h>

struct s_exec head;
struct segt segt[256];
FILE *tf;
FILE *rf;
long imsize;
unsigned segtsz;
extern int swap_flg;
char *mktemp();

main(argc, argv)
int argc;
char **argv;
{
	register i;
	char *tname;
	int status;
	int hflag;
	int u;

	if(argc < (*argv[1] == '-' ? 3 : 2)) {
		fprintf(stderr, "Usage: strip [-h] file-name ... \n");
		exit(2);
	}
	if(*argv[1] == '-') {
		if(!(hflag = (strcmp(argv[1], "-h") == 0))) {
			fprintf(stderr, "strip: illegal option: %s\n", argv[1]);
			exit(2);
		}
	}
	tname = mktemp("/tmp/sXXXXXX");
	tf = fopen(tname, "w+");
	if(tf == NULL) {
		fprintf(stderr, "strip: cannot create temporary file\n");
		exit(2);
	}
	unlink(tname);
	for(i = (*argv[1] == '-' ? 2 : 1); i < argc; i++) {
		status = strip(argv[i], hflag);
		if(status > 1)
			break;
	}
	fclose(tf);
	exit(status);
}

strip(name, hflag)
char *name;
int hflag;
{
	FILE *f;
	int mag;
	int flag;
	long size;

	f = fopen(name, "r");
	if(f == NULL) {
		fprintf(stderr, "strip: unable to open %s\n", name);
		return(1);
	}
	fread((char *)&mag, 2, 1, f);
	fseek(f, 0L, 0);
	if(!goodmagic(mag)) {
		fprintf(stderr, "strip: %s not in correct object format\n", name);
		fclose(f);
		return(1);
	}
	if(swap_flg)
		swab((char *)&mag, (char *)&mag, 2);
	fread((char *)&head, 24, 1, f);
	flag = 1;
	if(swap_flg)
		swab((char *)&flag, (char *)&flag, 2);
	if(head.s_syms == 0 && (head.s_flag & flag) == flag && hflag == 0) {
		fprintf(stderr, "strip: %s already stripped\n", name);
		fclose(f);
		return(1);
	}
	fseek(tf, 0L, 0);
	imsize = head.s_imsize;
	segtsz = head.s_segt;
	if(swap_flg) {
		longswap(&imsize);
		swab((char *)&segtsz, (char *)&segtsz, 2);
	}
	fread((char *)segt, 16, segtsz >> 4, f);
	if(swap_flg)
		swab((char *)segt, (char *)segt, segtsz);
	if(hflag == 0) {
		head.s_syms = 0;
		head.s_flag |= flag;
		if(mag == X_MAGIC1 || mag == X_MAGIC3 || mag == X_MAGIC4) {
			head.s_imsize = (long)segt[0].sg_code + segt[0].sg_data;
			head.s_codesz = segt[0].sg_code;
			if(swap_flg)
				longswap(&head.s_imsize);
		}
		if(fwrite((char *)&head, 24, 1, tf) != 1) {
			fprintf(stderr, "strip: write error\n");
			return(2);
		}
		if(mag == X_MAGIC1 || mag == X_MAGIC3 || mag == X_MAGIC4)
			size = (long)(segt[0].sg_code + segt[0].sg_data) + (long)segtsz + 24L;
		else
			size = (long)segtsz + imsize + 24L;
		if(swap_flg)
			swab((char *)segt, (char *)segt, segtsz);
		if(fwrite((char *)segt, 16, segtsz >> 4, tf) != segtsz >> 4) {
			fprintf(stderr, "strip: write error\n");
			return(2);
		}
	} else
		size = imsize;
	if((mag == X_MAGIC1 || mag == X_MAGIC3 || mag == X_MAGIC4) && hflag == 0) {
		if(rcopy(name, f, tf, imsize)) {
			fclose(f);
			return(2);
		}
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
	return(0);
}

copy(name, fr, to, size)
char *name;
FILE *fr, *to;
long size;
{
	register s, n;
	char buf[512];

	signal(1, 1);
	signal(2, 1);
	signal(3, 1);
	while(size != 0) {
		s = size > 512 ? 512 : size;
		n = fread(buf, 1, s, fr);
		if(s != n) {
			fprintf(stderr, "strip: unexpected eof on %s\n", name);
			return(1);
		}
		if(fwrite(buf, 1, n, to) != n) {
			fprintf(stderr, "strip: write error on %s\n", name);
			return(1);
		}
		size -= n;
	}
	signal(1, 0);
	signal(2, 0);
	signal(3, 0);
	return(0);
}

rcopy(name, fr, to, size)
char *name;
FILE *fr, *to;
long size;
{
	long off;
	char c;
	unsigned x;

	rf = fopen(name, "r");
	if(rf == NULL) {
		fprintf(stderr, "strip: unable to open %s\n", name);
		return(1);
	}
	off = (long)segtsz + 24L + imsize;
	fseek(rf, off, 0);
	signal(1, 1);
	signal(2, 1);
	signal(3, 1);
	while(size != 0) {
		fread(&c, 1, 1, fr);
		fread(&x, 1, 1, rf);
		if((x & 0xF) == 9 || (x & 0xF8) == 0x10 || (x & 0xF8) == 0x30 || (x & 0xF) == 0xB) {
			if(fwrite(&c, 1, 1, to) != 1) {
				fprintf(stderr, "strip: write error\n");
				return(1);
			}
		} else if((x & 0xF) == 0xA || (x & 0xF8) == 0x20) {
			x >>= 8;
			if(fwrite(&x, 1, 1, to) != 1) {
				fprintf(stderr, "strip: write error\n");
				return(1);
			}
		} else {
			if(fwrite(&c, 2, 1, to) != 1) {
				fprintf(stderr, "strip: write error\n");
				return(1);
			}
		}
		size -= 2;
	}
	signal(1, 0);
	signal(2, 0);
	signal(3, 0);
	fclose(rf);
	return(0);
}
