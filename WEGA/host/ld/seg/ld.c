/*
 * ld.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include "ld.h"
#include "ldconf.h"

static char sccsid[] __attribute__((unused)) = "@[$]ld.c\t3.8  10/27/83 12:16:07 - 87wega3.2";
int16_t stksegn = -1;
int16_t cmpmode = 0;
int16_t mlhead = 0;
int16_t mlcnt = 0;
int32_t tcum = 0;
int32_t dcum = 0;
int16_t unused484 = 0;
int32_t *libp = (int32_t *)liblist;
char *aoutnam = "a.out";
char *ofilename = "l.out";
char tfname[] = "/tmp/ldaXXXXXX";	/* one X more than the original: mkstemp */
char *secnam[] = {"all", "text", "data", "bss"};
char symdef[] = "__.SYMDEF";
char premeof[] = "Premature EOF";
int16_t nundef = 0;
int16_t delarg = 4;
int16_t nlflag = 1;
int16_t maxblk = 2000;
uint16_t nsym = 2000;
int16_t nwsym = 0;

/* the handler of SIGINT and SIGTERM */
static void sigdel(int sig)
{
	(void)sig;
	delexit();
}

/* main: entry: initialise, scan the options, load the libraries and objects, write the output (0x078c..0x0a5c of /bin/sld) */
int main(int argc, char **argv)
{
	register int16_t j;
	register char *s;
	int16_t i;
	int16_t savei = 0;
	char **savep = 0;
	int16_t savec = 0;
	char **savev = 0;

	ldconf_load(argv[0]);	/* host only: ld.conf next to the program */
	init_signals();
	if (argc == 1)
		exit(4);
	if ((pname = strrchr(*argv, '/')) == 0)
		pname = *argv;
	else
		pname++;
	for (i = 0; i < 128; i++) {
		reslist[i].bssptr = -1;
		reslist[i].datptr = -1;
		reslist[i].txtptr = -1;
		reslist[i].res = -1;
	}
	symtab_alloc(nsym);
	scan_options(argc, argv);
	segtab_alloc(maxblk);
	incmd = 0;
	i = 1;
	curarg = argv + 1;
	for (;;) {
		while (i < argc) {
			filname = 0;
			s = *curarg++;
			if (s[0] != '-')
				goto lib;
			j = 1;
		optchar:
			if (s[j] == 0)
				goto next;
			switch (s[j]) {
			case 'f':
				savei = i;
				i = -1;
				savep = curarg;
				savev = argv;
				savec = argc;
				argv = cmdav;
				argc = cmdac;
				curarg = argv;
				incmd++;
				break;
			case 'A':
				j++;
			/* fall through */
			case 'e':
			case 'o':
			case 'u':
				i++;
				curarg++;
				j++;
				goto optchar;
			case 'l':
				load1arg(s + (j - 1));
				break;
			case 'b':
			case 't':
				if (mcnt)
					error(2, "Cannot use -t or -b options with -M");
				tbflag = 1;
				i++;
				if (i >= argc)
					error(2, "-%c: arg missing", s[j]);
				if (reg_sect_addr(s + j, *curarg))
					j++;
				curarg++;
				j++;
				goto optchar;
			case 'M':
				if (tbflag)
					error(2, "Cannot use -t or -b options with -M");
				mcnt++;
				scan_m_opt(s + j, &i, argc);
				break;
			default:
				error(2, "-%c: bad flag", s[j]);
				break;
			case 'N':
			case 'O':
			case 'R':
			case 'S':
			case 'T':
			case 'X':
			case 'd':
			case 'i':
			case 'r':
			case 's':
			case 'w':
			case 'x':
			case 'z':
				break;
			}
			goto next;
		lib:
			load1arg(s);
next:
			i++;
		}
		if (incmd == 0)
			break;
		i = savei + 2;
		savep++;
		curarg = savep;
		argv = savev;
		argc = savec;
		incmd = 0;
	}
	if (rflag && secspec)
		printf("%s: warning, -r option overrides -t, -b\n", pname);
	endload(argc, argv);
}


char tmpname[] = "l.out";

/* delexit: signal handler: remove the temporary files and exit (0x0a5c..0x0aba of /bin/sld) */
int16_t delexit(void)
{
	unlink(tmpname);
	if (!delarg && !(arflag && nundef) && !rflag) {
		chmod(ofilename, (~umask(0)) & 0x1ff);
	}
	exit(delarg);
}

/* init_signals: catch the interrupt signals (0x0aba..0x0b04 of /bin/sld) */
void init_signals(void)
{
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sigdel);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, sigdel);
}

/* scan_options: scan the options (-l, -o, -M, -f, ...) of the command line or of a command file (0x0b04..0x0f26 of /bin/sld) */
void scan_options(int argc, char **argv)
{
	int v08;
	int v0a;
	int16_t savec = 0;
	char **savev = 0;
	int16_t fflag;
	char **p;

	fflag = 0;
	argc--;
	argv++;
	for (;;) {
		while (argc > 0) {
			if (**argv == '-')
			switch ((*argv)[1]) {
	case 'f':
				if (fflag)
					error(2, "Duplicate -f flags");
				if (argc <= 1)
					error(2, "Bad command file");
				argv++;
				cmdfile = (char **)*argv;
				argc--;
				savev = argv;
				savec = argc;
				read_words((char *)cmdfile);
				argv = cmdav - 1;
				argc = cmdac + 1;
				fflag++;
				incmd++;
				goto next;
	case 'X':
				Xflag++;
				goto next;
	case 'S':
				Sflag++;
				goto next;
	case 'R':
				if (sscanf(*argv, "-R%d", &v0a) < 1)
					error(2, "invalid syntax for R option");
				nrflag++;
				resv_seg(v0a, 'r');
				goto next;
	case 'r':
				rflag++;
				arflag++;
				goto next;
	case 's':
				sflag++;
	/* fall through */
	case 'x':
				xflag++;
				goto next;
	case 'e':
	case 'u':
				if (argc <= 1)
					error(2, "Bad 'use' or 'entry'");
				p = (char **)slookup(argv[1]);
				enter((struct symbol **)p);
				if ((*argv)[1] == 'e')
					entrypt = lastsym;
				goto skip2;
	case 'd':
				dflag++;
				goto next;
	case 'i':
				iflag++;
				bseg = 1;
				dseg = 1;
				if (maxseg == 0)
					maxseg++;
				goto next;
	case 'O':
				Oflag++;
				goto next;
	case 'o':
				if (argc <= 1)
					error(2, "Bad output file");
				argv++;
				ofilename = *argv;
				if (access(ofilename, 0) == 0) {
					if (access(ofilename, 2) != 0)
						error(2, "%s is write protected", ofilename);
				}
				argc--;
				ofilfnd++;
				goto next;
	case 'A':
				if (argc <= 1)
					error(2, "Need size specification");
				switch ((*argv)[2]) {
		case 's':
					if (sscanf(argv[1], "%d", &v08) != 1)
						error(2, "Unable to convert symbol table size specification");
					if ((((int32_t)(uint16_t)v08 + 2) << 4) > 0xfff8L)
						error(2, "Symbol table size specification is too big");
					nsym = v08;
					resize_symtab(v08);
					goto skip2;
		case 'p':
					if (sscanf(argv[1], "%d", &v08) != 1)
						error(2, "Unable to convert pass 1 segment table size specification");
					if ((((int32_t)(uint16_t)v08 + 2) << 2) > 0xfff8L)
						error(2, "Pass 1 segment table size specification is too big");
					maxblk = v08;
					goto skip2;
		default:
					error(2, "Invalid character after -A");
				}
skip2:
				argc--;
				argv++;
				goto next;
	case 'w':
				wflag++;
				goto next;
	case 'z':
				resv_seg(0x7f, 's');
				zflag = 1;
				goto next;
	case 'N':
				switch ((*argv)[2]) {
		case 'l':
					nlflag = 1;
					break;
		case 's':
					nlflag = 0;
					break;
		default:
					error(2, "-N%c :bad argument (l or s)", (*argv)[2]);
				}
				goto next;
			}
next:
			argc--;
			argv++;
		}
		if (incmd-- == 0)
			break;
		savev++;
		argv = savev;
		argc = --savec;
	}
}


/* read_words: read a command file (-f) and split it into the words of the argument list (0x0f26..0x1070 of /bin/sld) */
int16_t read_words(char *name)
{
	int16_t fd;
	int16_t n;
	int16_t idx;
	char c;
	int16_t inword;
	char *p;

	p = cmdbuf;
	cmdargv[0] = cmdbuf;
	fd = open(name, 0);
	if (fd < 0)
		error(2, "Unable to open command file");
	inword = 0;
	idx = 0;
	while ((n = (read(fd, &c, 1) != 0))) {
	if (n == -1)
		error(2, "Bad command file read");
	if (c >= 0 && isspace(c)) {	/* ctab[c] & 8 of the old ctype */
		if (!inword) {
			inword++;
			*p = 0;
			p++;
			if (p > libname)
				error(2, "Command file arguments too long");
			idx++;
			if (idx > 2000)
				error(2, "Too many command file arguments");
			cmdargv[idx] = p;
		}
	} else {
		inword = 0;
		*p = c;
		p++;
		if (p > libname)
			error(2, "Command file arguments too long");
	}
	}
	cmdav = cmdargv;
	cmdac = idx;
	close(fd);
	return 0;
}

/* reg_sect_addr: read the address of a section option (-t, -d, -b) and check it (0x1070..0x11ea of /bin/sld) */
int16_t reg_sect_addr(char *s, char *t)
{
	register int16_t k = 0;
	register int16_t g;
	register char *u;
	char *fmt;
	int val;
	char junk;

	secspec++;
	switch (*(s + 1)) {
	case 't':
		k = 1;
		break;
	case 'd':
		k = 2;
		break;
	case 'b':
		k = 3;
		break;
	case 0:
		k = 0;
		break;
	default:
		error(2, "-%.2s: invalid selector (must be t, d or b)", s);
	}
	u = t;
	if (iflag && (k == 2 || k == 3))
		g = 1;
	else
		g = 0;
	if (secflg[g][k])
		error(2, "-%.2s: -%c%c already specified", s, secflg[g][k], s[1]);
	secflg[g][k] = *s;
	fmt = "%o%c";
	if (*u != '0')
		fmt = "%d%c";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "%x%c";
	}
	if (sscanf(u, fmt, &val, &junk) != 1)
		error(2, "-%.2s %s: bad address", s, t);
	secorg[g][k] = (uint16_t)val;	/* the conversion stores into a 16 bit int */
	if (k == 0)
		return 0;
	return 1;
}


/* endload: second pass over the arguments: load every object and library in order (0x11ea..0x1364 of /bin/sld) */
void endload(int argc, char **argv)
{
	register int16_t k;
	register char *a;
	register char **av;
	register int16_t j;
	int16_t flag;
	int16_t saved_argc;
	int16_t saved_k;
	char **saved_av;
	char **base;
	char *p;

	flag = 0;
	filname = 0;
	middle();
	setupout();
	av = argv + 1;
	nblk = 0;
	fblk = 0;
	libp = (int32_t *)liblist;
	k = 1;
	for (;;) {
		while (k < argc) {
			a = *av++;
			if (*a == '-') {
				for (j = 1; a[j] != 0; j++) {
					switch (a[j]) {
					case 'f':
						saved_k = k;
						k = -1;
						saved_av = av;
						base = argv;
						saved_argc = argc;
						argv = cmdav;
						argc = cmdac;
						av = argv;
						flag = 1;
						goto next;
					case 'b':
					case 't':
						goto bt;
					case 'A':
					case 'e':
					case 'o':
					case 'u':
					case 'v':
						goto consume;
					case 'N':
						goto nn;
					case 'l':
						goto ll;
					case 'M':
						goto mm;
					case 'R':
						if (1) goto next;
						load2arg(a);
					}
					continue;
				bt:
					if (a[j + 1])
						j++;
				consume:
					k++;
					av++;
					continue;
				nn:
					j++;
					continue;
				ll:
					j--;
					a[j] = '-';
					load2arg(a + j);
					goto next;
				mm:
					p = strchr(a, ':');
					if (p && !p[2]) {
						k++;
						av++;
					}
					if (**av != '+')
						goto next;
					do {
						k++;
						av++;
					} while (**av != '+');
					k++;
					av++;
					goto next;
				}
			} else
				load2arg(a);
next:
			k++;
		}
		if (flag == 0)
			break;
		k = saved_k + 2;
		saved_av++;
		av = saved_av;
		argv = base;
		argc = saved_argc;
		flag = 0;
	}
	finishout();
}


/* load1arg: load an archive: only the members that define an undefined symbol (ranlib table) (0x1364..0x14a4 of /bin/sld) */
void load1arg(char *p)
{
	register char *r;
	int32_t n;

	r = p;
	switch (getfile(r)) {
	case 0:
		load1(0, 0L);
		break;
	case 1:
		for (n = 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	case 2:
		rlcnt = archdr.asize / 12;
		rltab = (struct rlent *)ensure_buf(archdr.asize);
		lseek(infil, 28L, 0);
		if (read(infil, rltab, rlcnt * 12) < 0)
			rlcnt = 0;	/* the original does not test it */
		chk_ranlib(rltab, rlcnt);
		while (ranlib_scan())
			;
		*libp++ = -1;
		break;
	case 3:
		error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(infil);
}

/* load_header: read the header of an object file or archive member (0x14a4..0x1560 of /bin/sld) */
int16_t load_header(int32_t p)
{
	uint16_t magic;
	uint8_t raw[26];

	dseek(&text, p, 0x1c);
	if (text.size <= 0) {
		*libp++ = -1;
		return 0;
	}
	mget(raw, 0x1a);
	rd_archdr(raw, &archdr);
	mget(raw, 2);
	magic = rd16(raw);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe607 || magic == 0xe611) {
		if (load1(1, p + 13)) {
			*libp++ = p;
		}
	}
	return 1;
}

/* ranlib_scan: look up the undefined symbols in the ranlib table of an archive (0x1560..0x1664 of /bin/sld) */
int16_t ranlib_scan(void)
{
	int16_t i;
	struct rlent *end;
	struct symbol *t;
	int32_t *sv;

	sv = libp;
	end = rltab;
	for (i = 0; (uint16_t)i < (uint16_t)rlcnt; i++) {
		if (rltab + i < end)
			continue;
		if ((rltab[i].rname[0] & 0x80) != 0)
			end = (struct rlent *)(rltab[i].rname + (rltab[i].rname[0] & 0x7f));
		t = *pslookup(rltab[i].rname);
		if (!t)
			continue;
		if (t->stype != 0x20)
			continue;
		if (swapflg)
			swap_pair(&rltab[i]);
		load_header(ROFF(&rltab[i]) >> 1);
	}
	return sv != libp;
}

/* add: sum of two unsigned values; error (level 1) when it exceeds 16 bits (0x1664..0x1696 of /bin/sld) */
int16_t add(int16_t x, int16_t y, char *a)
{
	int32_t s;

	s = (int32_t)(uint16_t)x + (uint16_t)y;
	if (s >= 0x10000)
		error(1, a);
	return s;
}

/* load1: pass one for one object: read the symbol table, enter the symbols (0x1696..0x18f2 of /bin/sld) */
int16_t load1(int16_t a, int32_t c)	/* the original declares a third parameter b that the callers do not pass and the body does not use */
{
	register struct symbol *cur;
	register struct p1seg *blk;
	int16_t k;
	int16_t n;
	int16_t cnt;
	int16_t saved;
	char type;
	char d;
	struct symbol **p2;

	readhdr(c, 1);
	cnt = 0;
	k = 1;
	saved = symindex;
	if ((filhdr.relflg & 1) == 1) {
		error(1, "No relocation bits");
		return 0;
	}
	c = (int32_t)((uint16_t)(hdrsz + segtsz) / 2) + filhdr.tsize + filhdr.dsize + c;
	dseek(&text, c, filhdr.ssize);
	while (text.size > 0) {
		n = read_entry_hdr();
		type = cursym.stype;
		if (Sflag) {
			d = type & 0x1f;
			if (d == 1 || d > 4)
				continue;
		}
		if (!((type & 0x20) != 0)) {
			if (Xflag && cursym.sname[0] == 0x4c)
				continue;
			k += n + 1;
			continue;
		}
		if (enter(lookup_name())) {
			cur = lastsym;
			if (cur->stype == ' ')
				continue;
		} else {
			cur = lastsym;
			if (cur->stype != ' ') {
				if (a)
					warn_sym_redef(cur, &cursym);
				continue;
			}
			if (cursym.stype == ' ') {
				if (cursym.svalue > cur->svalue)
					cur->svalue = cursym.svalue;
				continue;
			}
			if (cur->svalue != 0 && cursym.stype == '"')
				continue;
			cnt++;
		}
		cur->sseg = fblk + cursym.sseg;
		ctrel = (blk = get_block(cur->sseg))->ctrel;
		cdrel = blk->cdrel;
		cbrel = blk->cbrel;
		cur->svalue &= 0xffff;	/* the high word */
		symreloc();
		cur->stype = cursym.stype;
		cur->svalue = cursym.svalue;
	}
	if (a == 0 || cnt != 0) {
		symbytes = add(symbytes, k << 4, "symbol table overflow");
		return 1;
	}
	while (symindex > saved) {
		symindex--;
		if ((p2 = symhash[symindex]) == 0)
			continue;
		*p2 = 0;
	}
	nblk = fblk;
	return 0;
}

/* place_sec: place the sections (text, data, bss) of the objects into segments and set the addresses (0x18f2..0x1f96 of /bin/sld) */
void place_sec(struct p2seg *p, int16_t k)
{
	register int16_t j;
	register int16_t i;
	register uint16_t r8;
	struct areas *q;
	int16_t a;
	int16_t b;
	int16_t c;

	bsplseg = 0;
	dsplseg = 0;
	a = p->s_code;
	b = p->s_data;
	c = p->s_bss;
	areas[1].kind = iflag ? 4 : 0;
	areas[2].kind = iflag ? 0 : 1;
	areas[3].kind = b ? 2 : 1;
	areas[1].tsize = a;
	areas[2].tsize = b;
	areas[3].tsize = c;
	for (i = 0; i < 4; i++) {
		areas[i].tend = 0;
		areas[i].tstart = 0;
	}
	if (secflg[k][0]) {
		i = iflag ? 2 : 1;
		for (; i < 4; i++) {
			if (secflg[k][i]) {
				printf("%s: warning, -%c overrides -%c%c\n", pname, secflg[k][0], secflg[k][i], *secnam[i]);
				secflg[k][i] = 0;
			}
		}
		areas[0].kind = 4;
		areas[1].kind = 0;
		areas[2].kind = 1;
		areas[0].tsize = a + b + c;
		placekd = 1;
		if (c)
			placekd = 3;
		else if (b)
			placekd = 2;
	}
	j = secflg[k][0] ? 0 : 1;
	for (; j < 4; j++) {
		q = &areas[j];
		r8 = areas[(int)q->kind].tend;
		switch (secflg[k][j]) {
		case 't':
			if (q->tsize > secorg[k][j])
				error(1, "top address of %s in segment %d too low", secnam[j], k);
			if ((r8 = secorg[k][j] & 0xff00) != secorg[k][j])
				printf("%s: warning, top address of %s in segment %d relocated to 0x%x\n", pname, secnam[j], k, r8);
			r8 = (r8 - q->tsize) & 0xff00;
			q->tsize = (q->tsize + 255) & 0xff00;
			break;
		case 'b':
			r8 = secorg[k][j] & 0xff00;
			if (r8 != secorg[k][j])
				printf("%s: warning, bottom address of %s in segment %d relocated to 0x%x\n", pname, secnam[j], k, r8);
			break;
		}
		areas[0].tsize = 0;
		q->tend = r8 + q->tsize;
		if (r8 > q->tend)
			error(1, "%s area wrap around", secnam[j]);
		for (i = 1; i < j; i++) {
			if (r8 < areas[i].tend && q->tend > areas[i].tstart) {
				printf("sld: warning, %s and %s overlap in segment %d - creating overlay segment\n", secnam[i], secnam[j], k);
				split_seg(p, j);
				if (j == 2) {
					areas[2].tend = 0;
					areas[3].kind = 1;
					dsplf++;
				} else {
					areas[3].tend = 0;
					bsplf++;
				}
				Oflag++;
			}
		}
		q->tstart = r8;
	}
	p->torig = areas[1].tstart;
	(dsplf ? dsplseg : p)->dorig = areas[2].tstart;
	(bsplf ? bsplseg : p)->borig = areas[3].tstart;
	if (secflg[k][0]) {
		i = 1;
		if (!a) {
			i = 2;
			if (!b)
				i = 3;
		}
		secflg[k][i] = 1;
	}
	if (secflg[k][1]) {
		p->atr |= 0x10;
		p->s_coff = high_byte(p->torig);
	}
	if (secflg[k][2]) {
		if (dsplf) {
			dsplseg->atr |= 0x20;
			dsplseg->s_doff = high_byte(dsplseg->dorig);
		} else {
			p->atr |= 0x20;
			p->s_doff = high_byte(p->dorig);
		}
	}
	if (bsplf) {
		bsplseg->atr |= 0x40;
		if (!secflg[k][3])
			bsplseg->borig = (p->s_data + p->dorig + 255) & 0xff00;
		bsplseg->s_boff = high_byte(bsplseg->borig);
	} else if (secflg[k][3]) {
		p->atr |= 0x40;
		p->s_boff = high_byte(p->borig);
	}
	if (!secflg[k][0]) {
		if (dsplf || bsplf)
			placekd = 0;
		else {
			placekd = 1;
			if (areas[1].tend < areas[2].tend)
				placekd = 2;
			else if (areas[1].tend < areas[3].tend)
				placekd = 3;
			switch (placekd) {
			case 2:
				if (areas[2].tend < areas[3].tend)
					placekd = 3;
				break;
			case 3:
				if (areas[2].tend > areas[3].tend)
					placekd = 2;
				break;
			}
		}
	}
}

/* split_seg: start a new pass-two segment when the section does not fit (0x1f96..0x2056 of /bin/sld) */
void split_seg(struct p2seg *p, int16_t kind)
{
	if (np2seg > 0x100)
		error(2, "Pass2 segment table overflow");
	if (kind == 2) {
		dsplseg = table32_entry(np2seg++);
		dsplseg->segno = 0xff;
		dsplseg->s_data = p->s_data;
		dsplseg->atr |= 2;
		p->s_data = 0;
		p->atr &= ~2;
		p->s_doff = 0;
	} else {
		bsplseg = table32_entry(np2seg++);
		bsplseg->segno = 0xff;
		bsplseg->s_bss = p->s_bss;
		bsplseg->atr |= 4;
		p->s_bss = 0;
		p->atr &= ~4;
		p->s_boff = 0;
	}
}

/* setupout: open the output and temporary files, write the header (0x2056..0x22a4 of /bin/sld) */
void setupout(void)
{
	register int16_t n;
	register struct p2seg *p;
	uint8_t arr[8192];
	uint8_t hdr[24];
	struct fseg fs;
	int tmpfd;

	tcreat(&toutb, 0);
	if ((tmpfd = mkstemp(tfname)) >= 0)	/* mktemp of the original */
		close(tmpfd);
	tcreat(&tmpoutb, 1);
	tcreat(&doutb, 1);
	if (!sflag || !xflag)
		tcreat(&soutb, 1);
	if (rflag) {
		tcreat(&troutb, 1);
		tcreat(&droutb, 1);
		tcreat(&routb, 1);
	}
	filhdr.fmagic = segmode == 1 ? (Oflag ? 0xe605 : iflag ? 0xe611 : 0xe607) : (Oflag ? 0xe705 : iflag ? 0xe711 : 0xe707);
	filhdr.tsize = tsize;
	filhdr.dsize = dsize;
	filhdr.bsize = bsize;
	filhdr.ssize = sflag ? 0 : (((uint16_t)symbytes >> 4) + symindex) * 12;
	if (entrypt) {
		if (entrypt->stype != '"')
			error(1, "Entry point not in text");
		else
			filhdr.entry = entrypt->svalue;
	} else {
		for (p = p2head; p; p = p->nxt) {
			if (p->s_code) {
				filhdr.entry = ((uint16_t)p->torig & 0xffffL) + ((int32_t)p->segno << 24);
				break;
			}
		}
	}
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	filhdr.relflg = rflag ? 0 : 1;
	n = 0;
	for (p = p2head; p; p = p->nxt) {
		memcpy(&fs, p, sizeof fs);	/* the first 16 bytes of a p2seg are a segment record */
		wr_fseg(arr + (n << 4), &fs);
		if (swapflg)
			swap_quad(arr + (n << 4));
		n++;
	}
	init_entry(hdr, n);
	if (swapflg)
		ld_swab(hdr, hdr, 0x18);
	mput(&toutb, hdr, 0x18);
	mput(&toutb, arr, n << 4);
}

/* tcreat: create a temporary or output file (0x22a4..0x2346 of /bin/sld) */
void tcreat(struct buf *buf, int16_t tempflg)
{
	register int16_t ufd;
	char *nam;

	if (tempflg)
		nam = tfname;
	else
		nam = ofilename;
	ufd = creat(nam, 0x1b6);
	if (ufd < 0)
		error(2, tempflg ? "cannot create temp" : "cannot create output");
	close(ufd);
	buf->fildes = open(nam, 2);
	if (tempflg)
		unlink(tfname);
	buf->nleft = 0x100;
	buf->xnext = buf->iobuf;
}

/* init_entry: fill the header of the output (objhdr) from the totals and the flags (0x2346..0x23fe of /bin/sld) */
void init_entry(uint8_t *raw, int16_t a)
{
	struct objhdr h;
	struct objhdr *p = &h;

	p->hmagic = filhdr.fmagic;
	p->hx = tsize + dsize;
	if (p->hx > 0x10000 && tbflag)
		error(2, "cannot have segment overflow with t or b option");
	p->hbss = bsize;
	p->hsegsz = a << 4;
	p->hrelflg = filhdr.relflg;
	if (zflag)
		p->hrelflg |= 0x20;
	p->hsymsz = 0;
	p->hentry = filhdr.entry;
	p->hz = 0;
	p->hy = 0;
	wr_objhdr(raw, p);
}

/* load2arg: pass two for one argument (object file or archive member) (0x23fe..0x2492 of /bin/sld) */
void load2arg(char *acp)
{
	register char *cp;
	register int32_t *lp;
	uint8_t raw[26];

	cp = acp;
	if (getfile(cp) == 0) {
		goto test0;
body0:
		cp++;
test0:
		if (*cp != 0)
			goto body0;
		for (;;) {
			if (cp < acp)
				break;
			if (*--cp == 0x2f)
				break;
		}
		load2(0L, ++cp);
	} else {
		lp = libp;
		goto test1;
body1:
		dseek(&text, *lp, 26);
		mget(raw, 26);
		rd_archdr(raw, &archdr);
		load2(*lp + 13, (char *)&archdr);
		lp++;
test1:
		if (*lp != -1)
			goto body1;
		lp++;
		libp = lp;
	}
	close(infil);
}

/* load2: pass two for one object: relocate its symbols and copy text and data (0x2492..0x2624 of /bin/sld) */
void load2(int32_t a, char *b)
{
	register int16_t k;
	register struct symbol *rec;
	register struct local *t;
	int16_t n;
	char c;
	char d;

	readhdr(a, 2);
	mkfsym(b);
	t = local;
	a += (uint16_t)(hdrsz + segtsz) / 2;
	dseek(&text, a + filhdr.tsize + filhdr.dsize, filhdr.ssize);
	k = 0;
	while (text.size > 0) {
		n = read_entry_hdr() + 1;
		map_sym();
		c = cursym.stype;
		if (Sflag) {
			d = c & 0x1f;
			if (d == 1 || d > 4)
				goto next;
		}
		if ((c & 0x20) == 0) {
			if (sflag || xflag)
				goto next;
			if (Xflag && cursym.sname[0] == 0x4c)
				goto next;
			put_sym(&soutb, &cursym);
			goto next;
		}
		rec = *lookup_name();
		if (!rec)
			error(2, "internal error: symbol not found");
		if (cursym.stype != 0x20)
			goto other;
		if (t >= local + 4081)	/* the end of local (the variable symtab follows it in memory) */
			error(2, "Local symbol overflow");
		t->locindex = k;
		t->locsymbol = rec;
		t++;
		goto next;
other:
		if (rec->stype == cursym.stype && cursym.svalue == rec->svalue)
			goto next;
		if (put_name(cursym.sname) < 8)
			putchar(9);
		putchar(9);
		error(1, "- Multiply defined");
next:
		k += n;
	}
	load2seg(a, t);
}

/* load2seg: pass two: load the text and data of one segment of an object (0x2624..0x27d8 of /bin/sld) */
void load2seg(int32_t a, struct local *b)
{
	struct {
		int16_t tw0, tw1, tw2, tw3, tw4, tw5, tw6, tw7;
	} tmp;
	uint16_t i;
	int16_t lim;
	int16_t n;
	int32_t base;
	int32_t acc;
	int32_t hw;
	uint8_t rtmp[16];

	acc = a;
	base = a - segtsz / 2;
	n = 8;
	lim = segtsz / (n * 2);
	hw = (filhdr.tsize + filhdr.dsize) >> 1;
	for (i = 0; i < lim; i++) {
		dseek(&text, base + i * n, n * 2);
		mget(rtmp, n * 2);
		if (swapflg)
			swap_quad(rtmp);
		tmp.tw0 = (int16_t)rd16(rtmp);
		tmp.tw1 = (int16_t)rd16(rtmp + 2);
		tmp.tw2 = (int16_t)rd16(rtmp + 4);
		tmp.tw3 = (int16_t)rd16(rtmp + 6);
		tmp.tw4 = (int16_t)rd16(rtmp + 8);
		tmp.tw5 = (int16_t)rd16(rtmp + 10);
		tmp.tw6 = (int16_t)rd16(rtmp + 12);
		tmp.tw7 = (int16_t)rd16(rtmp + 14);
		switch (tmp.tw5 & 3) {
		case 1:
			dseek(&text, acc, tmp.tw2);
			dseek(&reloc, acc + hw, tmp.tw2);
			acc += half(tmp.tw2);
			load2td(b, &tmpoutb, &troutb);
			break;
		case 3:
			dseek(&text, acc, tmp.tw2);
			dseek(&reloc, acc + hw, tmp.tw2);
			acc += half(tmp.tw2);
			load2td(b, &tmpoutb, &troutb);
		/* fall through */
		case 2:
			dseek(&text, acc, tmp.tw3);
			dseek(&reloc, acc + hw, tmp.tw3);
			acc += half(tmp.tw3);
			load2td(b, &doutb, &droutb);
			break;
		}
	}
}

/* load2td: pass two: copy text or data with relocation (0x27d8..0x2cbc of /bin/sld) */
void load2td(struct local *p, struct buf *q, struct buf *r)
{
	struct symbol *save;
	int16_t flag;
	register int16_t w13;
	register int16_t w12;
	register int16_t w11;
	register struct symbol *e;
	register int16_t w10;

	flag = 0;

top:
	{
		if (--text.size <= 0) {
			if (text.size < 0)
				goto end;
			text.size++;
			w12 = get(&text);
		} else if (--text.nibuf < 0) {
			text.nibuf++;
			text.size++;
			w12 = get(&text);
		} else {
			w12 = (int16_t)rd16(text.ptr);
			text.ptr += 2;
		}
		if (--reloc.size <= 0) {
			if (reloc.size < 0)
				error(2, "Relocation error");
			reloc.size++;
			w13 = get(&reloc);
		} else if (--reloc.nibuf < 0) {
			reloc.nibuf++;
			reloc.size++;
			w13 = get(&reloc);
		} else {
			w13 = (int16_t)rd16(reloc.ptr);
			reloc.ptr += 2;
		}
	if (swapflg) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	if (!(w13 & 8)) {
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 2:
	if (!(w13 & 1))
		error(2, "bad segmented bit in relocation word");
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (int32_t)(w12 & 0xff);
	cursym.sseg = (uint16_t)w13 >> 8;
	map_sym();
	if ((w12 = (int16_t)cursym.svalue) & 0xff00)
		error(2, "short segmented offset too large");
	w12 += ((int16_t)(cursym.svalue >> 16)) & 0xff00;
setseg:
	w13 = (cursym.sseg << 8) + (w13 & 0xff);
emit:
	if (swapflg) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	put_int_buf(w12, q);
	if (rflag)
		put_int_buf(w13, r);
	goto top;
		break;
	case 1:
	if (!(w13 & 1))
		error(2, "bad segmented bit in relocation word");
	w10 = w12;
	cursym.sseg = (uint16_t)w13 >> 8;
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (int32_t)w12;
	map_sym();
	w10 = (w10 & 0x8000) | (int16_t)(cursym.svalue >> 16);
	if (swapflg)
		w10 = swap_bytes(w10);
	put_int_buf(w10, q);
	w11 = w13;
	flag = 1;
	goto top;
	case 0:
	if ((w13 & 0xe) == 0)
		goto pairchk;
	if (!flag)
		goto mapsym;
	if (!(((uint16_t)w13 != 0xff0f) & w11 & 0xff0f))
		goto mapsym;
	w13 &= 0xf0;
	w13 |= w11 & 0xff0f;
mapsym:
	cursym.sseg = (uint16_t)w13 >> 8;
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (int32_t)w12;
	map_sym();
pairchk:
	if (!flag)
		goto symdone;
	flag = 0;
	w11 = (cursym.sseg << 8) + (w11 & 0xff);
	if (swapflg)
		w11 = swap_bytes(w11);
	if (rflag)
		put_int_buf(w11, r);
symdone:
	if ((w13 & 0xe) == 0)
		goto emit;
	w12 = (int16_t)cursym.svalue;
	goto setseg;
	default:
		error(2, "unrecognized relocation address type");
	}
	goto emit;
	} else {
		e = lookloc(p, w13);
		if (e->stype != 0x20)
			goto extsym;
		w13 = (((w13 & 8) ? w13 : (w13 >> 4)) & 7) + ((index16(e) + nsymf) << 4) + 8;
		goto emit;
	}

extsym:
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 2:
	w13 = (e->stype - 0x21) * 2 + 0x21;
	w13 = (e->sseg << 8) + (w13 & 0xff);
	if ((w12 = ((int16_t)e->svalue + 0xff) & w12) & 0xff00)
		error(2, "short segment offset too large");
	w12 += ((int16_t)(e->svalue >> 16)) & 0xff00;
	goto emit;
		break;
	case 1:
	if (!(w13 & 1))
		error(2, "bad segmented bit in relocation word");
	w10 = (w12 & 0x8000) | ((int16_t)(e->svalue >> 16) & 0xff00);
	if (swapflg)
		w10 = swap_bytes(w10);
	put_int_buf(w10, q);
	save = e;
	flag = 1;
	goto top;
	case 0:
	if (!flag)
		goto extoff;
	flag = 0;
	if (e != save)
		error(2, "mismatched segment & offset relocation words");
	w11 = (e->stype - 0x21) * 2 + 0x11;
	w11 = (e->sseg << 8) + (w11 & 0xff);
	if (swapflg)
		w11 = swap_bytes(w11);
	if (rflag)
		put_int_buf(w11, r);
extoff:
	w13 = (e->stype - 0x21) * 2 + 1;
	w13 = (e->sseg << 8) + (w13 & 0xff);
	w12 = w12 + (int16_t)e->svalue;
	goto emit;
	default:
		error(2, "unrecognized relocation address type");
	}
	goto emit;
	}end:
	;
}

/* finishout: write the remaining parts of the output and patch the header (0x2cbc..0x2d4c of /bin/sld) */
void finishout(void)
{
	copy_segs();
	if (rflag)
		copy(&routb);
	if (!sflag) {
		if (!xflag)
			copy(&soutb);
		put_all();
	}
	flush(&toutb);
	close(toutb.fildes);
	if (!ofilfnd) {
		unlink(aoutnam);
		if (link("l.out", aoutnam) < 0)
			delarg = 4;	/* the original does not test it */
		ofilename = aoutnam;
	}
	delarg = errlev;
	patch_hdr();
	delexit();
}

/* patch_hdr: rewrite the header of the output file with the final values (0x2d4c..0x2dde of /bin/sld) */
void patch_hdr(void)
{
	int16_t n;
	uint8_t nwraw[2];
	struct {
		int16_t pad[6];
		int16_t syms;
		int16_t rest[5];
	} h;

	n = open(ofilename, 1);
	if (n == -1)
		error(2, "Unable to reopen output file");
	if (lseek(n, (int32_t)((char *)&h.syms - (char *)&h), 0) == -1)
		error(2, "Unable to seek in output file");
	nwsym = nwsym * 14;
	wr16(nwraw, (uint16_t)nwsym);
	if (write(n, nwraw, 2) != 2)
		error(2, "Unable to write in output file");
}

/* copy: copy a temporary file to the output (0x2dde..0x2e4c of /bin/sld) */
void copy(struct buf *buf)
{
	register int16_t n;
	register int16_t c;
	register uint8_t *q;

	flush(buf);
	n = buf->fildes;
	lseek(n, 0L, 0);
	while ((c = read(n, tmpoutb.iobuf, 0x200)) > 1) {
		c >>= 1;
		q = tmpoutb.iobuf;
		do {
			put_int_buf((int16_t)rd16(q), &toutb);
			q += 2;
		} while (--c);
	}
	close(n);
}

/* high_byte: return x >> 8; internal error if the low byte of x is not 0 (0x2e4c..0x2e76 of /bin/sld) */
int16_t high_byte(int16_t x)
{
	if ((x & 0xff00) != x)
		error(2, "internal error - mkchar");
	return ((uint16_t)x >> 8) & 0xff;
}

/* mget: read n bytes from the current input into a buffer (0x2e76..0x2f04 of /bin/sld) */
void mget(void *aloc, int16_t an)
{
	register uint8_t *p;
	register uint8_t *d;
	register int16_t k;

	k = an;
	k >>= 1;
	d = aloc;
	text.nibuf -= k;
	if (text.nibuf >= 0) {
		text.size -= k;
		if (text.size > 0) {
			p = text.ptr;
			do {
				d[0] = p[0];
				d[1] = p[1];
				p += 2;
				d += 2;
			} while (--k);
			text.ptr = p;
			return;
		}
		text.size += k;
	}
	text.nibuf += k;
	do {
		wr16(d, (uint16_t)get(&text));
		d += 2;
	} while (--k);
}

/* mput: write n bytes to an output buffer (0x2f04..0x2f3a of /bin/sld) */
void mput(struct buf *buf, const void *a, int16_t an)
{
	register int16_t k;
	register const uint8_t *aloc;

	aloc = a;
	k = an >> 1;
	do {
		put_int_buf((int16_t)rd16(aloc), buf);
		aloc += 2;
	} while (--k);
}

/* dseek: seek in the input to a given position (page buffered) (0x2f3a..0x3090 of /bin/sld) */
void dseek(struct stream *sp, int32_t v, int16_t n)
{
	int16_t nw;
	int32_t hi;
	int32_t lo;
	register struct page *cur;
	register struct stream *obj;

	hi = (int32_t)(((uint32_t)v >> 8) & 0xffffff);
	lo = v & 0xff;
	obj = sp;
	obj->pno->nuser -= 1;
	if ((cur = &page[0])->bno == hi || (cur = &page[1])->bno == hi)
		goto common;
	if (cur->nuser == 0 || (cur = &page[0])->nuser == 0) {
		if (page[0].nuser == 0 && page[1].nuser == 0 && page[0].bno < page[1].bno)
			cur = &page[0];
		cur->bno = hi;
		lseek(infil, (off_t)((uint32_t)(v & 0xffffff00L) << 1), 0);
		nw = read(infil, cur->buff, 0x200) >> 1;
		if (nw < 0)
			nw = 0;
		cur->nibuf = nw;
	} else
		error(2, "No pages");
common:
	cur->nuser += 1;
	obj->bno = hi;
	obj->pno = cur;
	obj->ptr = cur->buff + (lo << 1);
	if (n != -1)
		obj->size = half(n);
	if ((obj->nibuf = cur->nibuf - (int16_t)lo) <= 0)
		obj->size = 0;
}


/* half: x / 2 as an unsigned word (0x3090..0x30a4 of /bin/sld) */
int16_t half(int16_t i)
{
	return (i >> 1) & 0x7fff;
}

/* get: next byte of a stream; reads the next page and checks the end of the member (0x30a4..0x3144 of /bin/sld) */
int16_t get(struct stream *sp)
{
	register struct stream *q;

	q = sp;
	if (--q->nibuf < 0) {
		dseek(q, (int32_t)(q->bno + 1) << 8, -1);
		--q->nibuf;
	}
	if (--q->size <= 0) {
		if (q->size < 0)
			error(2, premeof);
		fpage.nuser++;
		--q->pno->nuser;
		q->pno = &fpage;
	}
	{
		int16_t w = (int16_t)rd16(q->ptr);

		q->ptr += 2;
		return w;
	}
}

/*
 * The prefixes of the library search ("", "/usr", "/z/bin") lie in front of the other strings of getfile in the data of the
 * original and the empty string is shared by its three uses: they are strings of their own before the function.
 */
static char pfx0[] = "";
static char pfx1[] = "/usr";
static char pfx2[] = "/z/bin";

/* host only: open a file; a name that begins with / and does not exist is also looked up below $WEGA_ROOT (the file tree of
   the original system) */
static int hostopen(const char *path)
{
	int fd = open(path, 0);
	char *root = getenv("WEGA_ROOT");
	char buf[1024];

	if (fd < 0 && errno == ENOENT && root != 0 && path[0] == '/') {
		snprintf(buf, sizeof buf, "%s%s", root, path);
		fd = open(buf, 0);
	}
	return fd;
}

/* getfile: open an input file, searching the library directories (0x3144..0x3408 of /bin/sld) */
int16_t getfile(char *name)
{
	register char *s;
	register char *p;
	struct stat st;
	uint8_t raw[26];

	s = name;

	infil = -1;
	archdr.aname[0] = 0;
	filname = name;
	if (s[0] == '-' && s[1] == 'l') {
		if (segmode == 1) {
			strcpy(libname, "/lib/slib");
			p = strrchr(libname, '/') + 5;
		} else {
			strcpy(libname, "/lib/lib");
			p = strrchr(libname, '/') + 4;
		}
		filname = libpath;
		if (s[2] == 0)
			s = "-lc";
		strcpy(p, s + 2);
		strcat(p, ".a");
		strcpy(libpath, pfx0);
		strcat(libpath, libname);
		infil = hostopen(libpath);
		if (infil == -1) {
			strcpy(libpath, pfx0);
			strcat(libpath, pfx1);
			strcat(libpath, libname);
			infil = hostopen(libpath);
			if (infil == -1) {
				strcpy(libpath, pfx0);
				strcat(libpath, pfx2);
				strcat(libpath, libname);
				infil = hostopen(libpath);
				if (infil == -1)
					error(2, "cannot open library");
			}
		}
	}
	if (infil == -1) {
		infil = hostopen(filname);
		if (infil < 0)
			error(2, "cannot open");
	}
	page[1].bno = -1;
	page[0].bno = -1;
	page[1].nuser = 0;
	page[0].nuser = 0;
	reloc.pno = &fpage;
	text.pno = &fpage;
	fpage.nuser = 2;
	dseek(&text, 0L, 2);
	if (text.size <= 0)
		error(2, premeof);
	if (get(&text) != -155)
		return 0;
	dseek(&text, 1L, 0x1a);
	if (text.size <= 0)
		return 1;
	mget(raw, 0x1a);
	rd_archdr(raw, &archdr);
	if (strncmp(archdr.aname, symdef, 14))
		return 1;
	if (fstat(infil, &st) < 0)
		st.st_mtime = 0;	/* the original does not test it */
	if ((int32_t)st.st_mtime > archdr.atime)
		return 3;
	return 2;
}

/* error: print a message with the program and file name; fatal depending on the level (0x3408..0x34b8 of /bin/sld) */
void error(int16_t level, char *msg, ...)
{
	va_list ap;

	if (!errlev)
		printf("%s: ", pname);
	if (filname) {
		printf("%s", filname);
		if (archdr.aname[0])
			printf("(%.14s)", archdr.aname);
		printf(": ");
	}
	va_start(ap, msg);
	vprintf(msg, ap);
	va_end(ap);
	printf("\n");
	if (level > 1)
		delexit();
	errlev = level;
}

/* lookloc: find an entry of the local symbol table (botch on failure) (0x34b8..0x3516 of /bin/sld) */
struct symbol *lookloc(struct local *end, int16_t a)
{
	register int16_t k;
	register struct local *p;
	register struct local *e;

	e = end;
	k = (a >> 4) & 0x0fff;
	p = local;
	while (p < e) {
		if (k == p->locindex)
			return p->locsymbol;
		p++;
	}
	printf("botch on %x\n", a);
	error(2, "Local symbol botch");
	return 0;
}

/* readhdr: read and convert the header of an object file or member (0x3516..0x3760 of /bin/sld) */
int16_t readhdr(int32_t a, int16_t b)
{
	uint16_t magic;
	struct objhdr tmp;
	struct fseg rec[512];
	uint8_t rtmp[0x18];
	uint8_t rrec[512 * 16];
	register struct fseg *p;
	register int16_t n;
	register int16_t w;

	dseek(&text, a, 2);
	mget(rtmp, 2);
	magic = rd16(rtmp);
	if (swapflg)
		magic = swap_bytes(magic);
	if (magic == 0xe607 || magic == 0xe611)
		segmode = 1;
	if (magic != 0xe707 && magic != 0xe711) {
		if (!segmode)
			return 0;
	}
	segobj += 1;
	hdrsz = 0x18;
	dseek(&text, a, hdrsz);
	mget(rtmp, hdrsz);
	if (swapflg)
		ld_swab(rtmp, rtmp, hdrsz);
	rd_objhdr(rtmp, &tmp);
	segtsz = tmp.hsegsz;
	n = segtsz / 16;
	dseek(&text, a + (uint16_t)hdrsz / 2, segtsz);
	mget(rrec, segtsz);
	if (swapflg) {
		for (w = 0; w < n; w++)
			swap_quad(rrec + w * 16);
	}
	for (w = 0; w < n; w++)
		rd_fseg(rrec + w * 16, &rec[w]);
	filhdr.fmagic = magic & 0xf7ff;
	filhdr.tsize = 0;
	filhdr.dsize = 0;
	filhdr.bsize = tmp.hbss;
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	w = 0;
	for (p = rec; p < rec + n; p++) {
		p->s_code = (p->s_code + 1) & 0xfffe;
		p->s_data = (p->s_data + 1) & 0xfffe;
		p->s_bss = (p->s_bss + 1) & 0xfffe;
		filhdr.tsize += (uint16_t)p->s_code;
		filhdr.dsize += (uint16_t)p->s_data;
		w |= p->atr;
	}
	if (w & 0xf8)
		error(2, "Invalid segment attributes");
	filhdr.entry = tmp.hentry;
	filhdr.relflg = tmp.hrelflg & 1;
	filhdr.ssize = tmp.hsymsz;
	make_blk(rec, n, b);
	return 1;
}

/* make_blk: create the segment table block for an object (0x3760..0x3a30 of /bin/sld) */
void make_blk(struct fseg *src, int16_t n, int16_t kind)
{
	register int16_t j;
	int16_t idx;
	int16_t loc[3] = { 0, 0, 0 };
	char key[16];
	struct p1seg *blk;

	fblk = nblk;
	blk = get_block(nblk);
	if (kind == 1) {
		copy_leafname(filname, key);
		j = mlhead;
		idx = 0;
		while (idx < mlcnt) {
			if (eq(mlist[j].mname, key))
				break;
			j = mlist[j].mnext;
			idx++;
		}
		loc[0] = tseg;
		loc[1] = dseg;
		loc[2] = bseg;
		if (idx != mlcnt) {
			if (mlist[j].mtext > -1)
				loc[0] = mlist[j].mtext;
			if (mlist[j].mdata > -1)
				loc[1] = mlist[j].mdata;
			if (mlist[j].mbss > -1)
				loc[2] = mlist[j].mbss;
		}
	}
	while (n--) {
		if (nblk >= maxblk)
			error(2, "pass one segment table overflow --Try -Ap option");
		if (kind == 1) {
			*(struct fseg *)blk = *src++;
			if (filhdr.fmagic != 0xe707 && filhdr.fmagic != 0xe607)
				error(0, "warning: nonsegmented object");
			if (filhdr.fmagic == 0xe707 || filhdr.fmagic == 0xe607) {
				blk->cdrel = -blk->s_code;
				blk->cbrel = blk->cdrel - blk->s_data;
			} else {
				blk->cdrel = 0;
				blk->cbrel = 0;
			}
			blk->ctrel = 0;
			blk->ncoff = loc[0];
			blk->ndoff = loc[1];
			blk->nboff = loc[2];
			blk->xtnext = -1;
			blk->datnext = -1;
			blk->bssnext = -1;
		} else {
			blk->tbase = tcum;
			tcum += (uint16_t)blk->s_code;
			blk->dbase = dcum;
			dcum += (uint16_t)blk->s_data;
		}
		nblk++;
		blk = get_block(nblk);
	}
}

/* eq: compare two names: in cmpmode the difference of the first unequal characters, else 1 when equal and 0 when not (0x3a30..0x3a88 of /bin/sld) */
int16_t eq(char *p, char *q)
{
	while (*p == *q) {
		q++;
		if (*p++ == 0) {
			if (!cmpmode)
				return 1;
			return 0;
		}
	}
	if (cmpmode)
		return *p - *q;
	return 0;
}

/* put_int_buf: write a word to an output buffer (0x3a88..0x3ac6 of /bin/sld) */
void put_int_buf(int16_t v, struct buf *p)
{
	wr16(p->xnext, (uint16_t)v);
	p->xnext += 2;
	if (--p->nleft <= 0)
		flush(p);
}

/* flush: write an output buffer to its file (0x3ac6..0x3b1e of /bin/sld) */
void flush(struct buf *p)
{
	register int16_t n;

	n = p->xnext - p->iobuf;
	if (n > 0) {
		if (write(p->fildes, p->iobuf, n) != n)
			error(2, "output error");
	}
	p->xnext = p->iobuf;
	p->nleft = 0x100;
}

/* swab12: swap the bytes of 12 words (never called) (0x3b1e..0x3b58 of /bin/sld) */
void swab12(char *a)
{
	register int16_t k;
	register int16_t t;
	register char *p;

	p = a;
	k = 12;
	do {
		t = p[0];
		p[0] = p[1];
		p[1] = t;
		p += 2;
	} while (--k);
}

/* warn_sym_redef: warn that a symbol is redefined in an archive (0x3b58..0x3c16 of /bin/sld) */
void warn_sym_redef(struct symbol *p, struct symbol *a)
{
	register char *s;
	char c;

	if (wflag)
		return;
	if (p != 0) {
		c = p->stype;
		if ((!c) & 0x20 || c == 0x20)
			return;
	}
	c = a->stype;
	if ((!c) & 0x20 || c == 0x20)
		return;
	s = a->sname;
	if (*s & 0x80)
		s++;
	if (*s == 0x5f) {
		s++;
		if (*s == 0x5f)
			return;
	}
	if (put_name(a->sname) < 8)
		putchar(9);
	putchar(0x20);
	error(0, "- warning: symbol redefined in archive");
}
