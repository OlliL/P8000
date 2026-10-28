/*
 * ld.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ld.c\t3.8  10/27/83 12:16:07 - 87wega3.2";
int stksegn = -1;
int cmpmode = 0;
int mlhead = 0;
int mlcnt = 0;
long tcum = 0;
long dcum = 0;
int unused484 = 0;
long *libp = (long *)liblist;
char *aoutnam = "a.out";
char *ofilnam = "l.out";
char tfname[] = "/tmp/ldaXXXXX";
char *secnam[] = {"all", "text", "data", "bss"};
char symdef[] = "__.SYMDEF";
char premeof[] = "Premature EOF";
int nundef = 0;
int delarg = 4;
int nlflag = 1;
int maxblk = 2000;
unsigned nsym = 2000;
int nwsym = 0;

/* main: entry: initialise, scan the options, load the libraries and objects, write the output (0x078c..0x0a5c of /bin/sld) */
int main(argc, argv)
int argc;
char **argv;
{
	register int j;
	register char *s;
	int i;
	int savei;
	char **savep;
	int savec;
	char **savev;

	init_signals();
	if (argc == 1)
		exit(4);
	pname = rindex(*argv, '/');
	if (pname == 0)
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
	goto test;
body:
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
	case 'e':
	case 'o':
	case 'u':
		i++;
		curarg++;
		j++;
		goto optchar;
	case 'l':
		load_lib_syms(s + (j - 1));
		break;
	case 'b':
	case 't':
		if (mcnt)
			report_error(2, "Cannot use -t or -b options with -M");
		tbflag = 1;
		i++;
		if (i >= argc)
			report_error(2, "-%c: arg missing", s[j]);
		if (reg_sect_addr(s + j, *curarg))
			j++;
		curarg++;
		j++;
		goto optchar;
	case 'M':
		if (tbflag)
			report_error(2, "Cannot use -t or -b options with -M");
		mcnt++;
		scan_m_opt(s + j, &i, argc);
		break;
	default:
		report_error(2, "-%c: bad flag", s[j]);
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
	load_lib_syms(s);
next:
	i++;
	goto test;
restore:
	i = savei + 2;
	savep++;
	curarg = savep;
	argv = savev;
	argc = savec;
	incmd = 0;
test:
	if (i < argc)
		goto body;
	if (incmd)
		goto restore;
	if (rflag && secspec)
		printf("%s: warning, -r option overrides -t, -b\n", pname);
	parse_args(argc, argv);
}

char tmpname[] = "l.out";

/* sig_handler: signal handler: remove the temporary files and exit (0x0a5c..0x0aba of /bin/sld) */
sig_handler()
{
	unlink(tmpname);
	if (!delarg && !(arflag && nundef) && !rflag) {
		chmod(ofilnam, (~umask(0)) & 0x1ff);
	}
	exit(delarg);
}

/* init_signals: catch the interrupt signals (0x0aba..0x0b04 of /bin/sld) */
init_signals()
{
	if (signal(2, 1L) != 1)
		signal(2, sig_handler);
	if (signal(15, 1L) != 1)
		signal(15, sig_handler);
}

/* scan_options: scan the options (-l, -o, -M, -f, ...) of the command line or of a command file (0x0b04..0x0f26 of /bin/sld) */
int scan_options(argc, argv)
int argc;
char **argv;
{
	int v08;
	int v0a;
	int savec;
	char **savev;
	int fflag;
	char **p;

	fflag = 0;
	goto test;
loop:
	if (**argv == '-')
	switch ((*argv)[1]) {
	case 'f':
		if (fflag)
			report_error(2, "Duplicate -f flags");
		if (argc <= 1)
			report_error(2, "Bad command file");
		argv++;
		cmdfile = (char **)*argv;
		argc--;
		savev = argv;
		savec = argc;
		read_words(cmdfile);
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
			report_error(2, "invalid syntax for R option");
		nrflag++;
		resv_seg(v0a, 'r');
		goto next;
	case 'r':
		rflag++;
		arflag++;
		goto next;
	case 's':
		sflag++;
	case 'x':
		xflag++;
		goto next;
	case 'e':
	case 'u':
		if (argc <= 1)
			report_error(2, "Bad 'use' or 'entry'");
		p = (char **)slookup(argv[1]);
		enter(p);
		if ((*argv)[1] == 'e')
			entrypt = (long)lastsym;
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
			report_error(2, "Bad output file");
		argv++;
		ofilnam = *argv;
		if (access(ofilnam, 0) == 0) {
			if (access(ofilnam, 2) != 0)
				report_error(2, "%s is write protected", ofilnam);
		}
		argc--;
		ofilfnd++;
		goto next;
	case 'A':
		if (argc <= 1)
			report_error(2, "Need size specification");
		switch ((*argv)[2]) {
		case 's':
			if (sscanf(argv[1], "%d", &v08) != 1)
				report_error(2, "Unable to convert symbol table size specification");
			if ((long)(unsigned)v08 + 2 << 4 > 0xfff8L)
				report_error(2, "Symbol table size specification is too big");
			nsym = v08;
			resize_symtab(v08);
			goto skip2;
		case 'p':
			if (sscanf(argv[1], "%d", &v08) != 1)
				report_error(2, "Unable to convert pass 1 segment table size specification");
			if ((long)(unsigned)v08 + 2 << 2 > 0xfff8L)
				report_error(2, "Pass 1 segment table size specification is too big");
			maxblk = v08;
			goto skip2;
		default:
			report_error(2, "Invalid character after -A");
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
			report_error(2, "-N%c :bad argument (l or s)", (*argv)[2]);
		}
		goto next;
	}
next:
	argc--;
	argv++;
test:
	if (argc > 0)
		goto loop;
	if (incmd--) {
		savev++;
		argv = savev;
		argc = --savec;
		goto test;
	}
}

/* read_words: read a command file (-f) and split it into the words of the argument list (0x0f26..0x1070 of /bin/sld) */
int read_words(name)
char *name;
{
	int fd;
	int n;
	int idx;
	char c;
	int inword;
	char *p;

	p = cmdbuf;
	cmdargv[0] = cmdbuf;
	fd = open(name, 0);
	if (fd < 0)
		report_error(2, "Unable to open command file");
	inword = 0;
	idx = 0;
	goto rd;
body:
	if (n == -1)
		report_error(2, "Bad command file read");
	if (!(ctab[c] & 8)) {
		inword = 0;
		*p = c;
		p++;
		if (p > libname)
			report_error(2, "Command file arguments too long");
	} else if (!inword) {
		inword++;
		*p = 0;
		p++;
		if (p > libname)
			report_error(2, "Too many command file arguments");
		idx++;
		if (idx > 2000)
			report_error(2, "Command file arguments too long");
		cmdargv[idx] = p;
	}
rd:
	n = read(fd, &c, 1) != 0;
	if (n)
		goto body;
	cmdav = cmdargv;
	cmdac = idx;
	close(fd);
	return 0;
}

/* reg_sect_addr: read the address of a section option (-t, -d, -b) and check it (0x1070..0x11ea of /bin/sld) */
int reg_sect_addr(s, t)
char *s;
char *t;
{
	register int k;
	register int g;
	register char *u;
	char *fmt;

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
		report_error(2, "-%.2s: invalid selector (must be t, d or b)", s);
	}
	u = t;
	g = 0;
	if (iflag && (k == 2 || k == 3))
		g = 1;
	if ((tab42b4 + g * 5)[k])
		report_error(2, "-%.2s: -%c%c already specified", s, (tab42b4 + g * 5)[k], s[1]);
	(tab42b4 + g * 5)[k] = *s;
	fmt = "%o%c";
	if (*u != '0')
		fmt = "%d%c";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "%x%c";
	}
	if (sscanf(u, fmt, &secorg[g][k]) != 1)
		report_error(2, "-%.2s %s: bad address", s, t);
	return k != 0;
}

/* parse_args: second pass over the arguments: load every object and library in order (0x11ea..0x1364 of /bin/sld) */
int parse_args(argc, argv)
int argc;
char **argv;
{
	register int k;
	register char *a;
	register char **av;
	register int j;
	int flag;
	int saved_argc;
	int saved_k;
	char **saved_av;
	char **base;
	char *p;

	flag = 0;
	filname = 0;
	end_pass1();
	start_output();
	av = argv + 1;
	nblk = 0;
	fblk = 0;
	libp = (long *)liblist;
	k = 1;
	goto test;
body:
	a = *av++;
	if (*a != '-')
		goto lib;
	j = 1;
scan:
	if (!a[j])
		goto next;
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
		if (!a[j + 1])
			goto consume;
		j++;
	case 'A':
	case 'e':
	case 'o':
	case 'u':
	case 'v':
consume:
		k++;
		av++;
	default:
dflt:
		j++;
		goto scan;
	case 'N':
		j++;
		goto dflt;
	case 'l':
		j--;
		a[j] = '-';
		load2arg(a + j);
		goto next;
	case 'M':
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
	case 'R':
		goto next;
	}
lib:
	load2arg(a);
next:
	k++;
test:
	if (k < argc)
		goto body;
	if (flag) {
		k = saved_k + 2;
		saved_av++;
		av = saved_av;
		argv = base;
		argc = saved_argc;
		flag = 0;
		goto test;
	}
	finish_output();
}

/* load_lib_syms: load an archive: only the members that define an undefined symbol (ranlib table) (0x1364..0x14a4 of /bin/sld) */
int load_lib_syms(p)
char *p;
{
	register char *r;
	long n;

	r = p;
	switch (getfile(r)) {
	case 0:
		read_symbols(0, 0L);
		break;
	case 1:
		for (n = 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	case 2:
		rlcnt = archdr.asize / 12;
		rltab = (struct rlent *)ensure_buf(archdr.asize);
		lseek(infil, 28L, 0);
		read(infil, rltab, rlcnt * 12);
		chk_ranlib(rltab, rlcnt);
		while (ranlib_scan())
			;
		*libp++ = -1;
		break;
	case 3:
		report_error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(infil);
}

/* load_header: read the header of an object file or archive member (0x14a4..0x1560 of /bin/sld) */
load_header(p)
long p;
{
	int magic;

	dseek(&text, p, 0x1c);
	if (text.size <= 0) {
		*libp++ = -1;
		return 0;
	}
	mget(&archdr, 0x1a);
	mget(&magic, 2);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe607 || magic == 0xe611) {
		if (read_symbols(1, p + 13)) {
			*libp++ = p;
		}
	}
	return 1;
}

/* ranlib_scan: look up the undefined symbols in the ranlib table of an archive (0x1560..0x1664 of /bin/sld) */
int ranlib_scan()
{
	int i;
	struct rlent *end;
	struct symbol *t;
	long *sv;

	sv = libp;
	end = rltab;
	i = 0;
	goto test;
body:
	if (rltab + i < end)
		goto next;
	if ((rltab[i].rname[0] & 0x80) != 0)
		end = (struct rlent *)(rltab[i].rname + (rltab[i].rname[0] & 0x7f));
	t = *pslookup(rltab[i].rname);
	if (!t)
		goto next;
	if (t->stype != 0x20)
		goto next;
	if (swapflg)
		swap_pair(&rltab[i]);
	load_header(rltab[i].roff >> 1);
next:
	i++;
test:
	if ((unsigned)i < (unsigned)rlcnt)
		goto body;
	return sv != libp;
}

/* add16_checked: sum of two unsigned values; error (level 1) when it exceeds 16 bits (0x1664..0x1696 of /bin/sld) */
int add16_checked(x, y, a)
unsigned x;
unsigned y;
long a;
{
	long s;

	s = (long)x + y;
	if (s >= 0x10000)
		report_error(1);
	return s;
}

/* read_symbols: pass one for one object: read the symbol table, enter the symbols (0x1696..0x18f2 of /bin/sld) */
int read_symbols(a, b, c)
int a;
int b;
long c;
{
	register struct symbol *cur;
	register struct p1seg *blk;
	int k;
	int n;
	int cnt;
	int saved;
	char type;
	char d;
	long u1;
	struct symbol **p2;
	long u2;
	struct symbol **slot;

	readhdr(c, 1);
	cnt = 0;
	k = 1;
	saved = symindex;
	if ((filhdr.relflg & 1) == 1) {
		report_error(1, "No relocation bits");
		return 0;
	}
	c = (long)(unsigned)(((hdrsz + segtsz) & 0xfffe) >> 1) + filhdr.tsize + filhdr.dsize + c;
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
		slot = lookup_name();
		if (enter(slot)) {
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
				if (cursym.svalue <= cur->svalue)
					continue;
				goto lstore;
			}
			if (cur->svalue != 0 && cursym.stype == '"')
				continue;
			cnt++;
		}
		cur->sseg = fblk + cursym.sseg;
		fctrel = (blk = get_block(cur->sseg))->ctrel;
		fcdrel = blk->cdrel;
		fcbrel = blk->cbrel;
		*(int *)&cur->svalue = 0;
		symreloc();
		cur->stype = cursym.stype;
lstore:
		cur->svalue = cursym.svalue;
	}
	if (a == 0 || cnt != 0) {
		symbytes = add16_checked(symbytes, k << 4, "symbol table overflow");
		return 1;
	}
	while (symindex > saved) {
		symindex--;
		p2 = symhash[symindex];
		if (p2)
			*p2 = 0;
	}
	nblk = fblk;
	return 0;
}

/* place_sec: place the sections (text, data, bss) of the objects into segments and set the addresses (0x18f2..0x1f96 of /bin/sld) */
int place_sec(p, k)
register struct p2seg *p;
int k;
{
	register int j;
	register int i;
	register int z;
	register unsigned r8;
	int u0;
	struct areas *q;
	int u1;
	int a;
	int b;
	int c;

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
	j = secflg[k][0] ? 0 : 1;
	for (; j < 4; j++) {
		q = &areas[j];
		r8 = areas[q->kind].tend;
		switch (secflg[k][j]) {
		case 't':
			if (q->tsize > secorg[k][j])
				report_error(1, "top address of %s in segment %d too low", secnam[j], k);
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
			report_error(1, "%s area wrap around", secnam[j]);
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
split_seg(p, kind)
struct p2seg *p;
int kind;
{
	if (np2seg > 0x100)
		report_error(2, "Pass2 segment table overflow");
	if (kind == 2) {
		dsplseg = table32_entry(np2seg++);
		dsplseg->segno = 0xff;
		dsplseg->s_data = p->s_data;
		dsplseg->atr |= 2;
		p->s_data = 0;
		p->atr &= ~2;
		p->s_coff = 0;
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

/* start_output: open the output and temporary files, write the header (0x2056..0x22a4 of /bin/sld) */
int start_output()
{
	register int n;
	register struct p2seg *p;
	long u1;
	int u2;
	char arr[8192];
	char hdr[24];

	open_tmp(&toutb, 0);
	mktemp(tfname);
	open_tmp(&tmpoutb, 1);
	open_tmp(&doutb, 1);
	if (!sflag || !xflag)
		open_tmp(&soutb, 1);
	if (rflag) {
		open_tmp(&troutb, 1);
		open_tmp(&droutb, 1);
		open_tmp(&routb, 1);
	}
	filhdr.fmagic = segmode == 1 ? (Oflag ? 0xe605 : iflag ? 0xe611 : 0xe607) : (Oflag ? 0xe705 : iflag ? 0xe711 : 0xe707);
	filhdr.tsize = ttotal;
	filhdr.dsize = dtotal;
	filhdr.bsize = btotal;
	filhdr.ssize = sflag ? 0 : (((unsigned)symbytes >> 4) + symindex) * 12;
	if (entrypt) {
		if (entrypt->stype != '"')
			report_error(1, "Entry point not in text");
		else
			filhdr.entry = entrypt->svalue;
	} else {
		for (p = p2head; p; p = p->nxt) {
			if (p->s_code) {
				filhdr.entry = (long)(unsigned)p->torig + ((long)p->segno << 24);
				break;
			}
		}
	}
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	filhdr.relflg = rflag ? 0 : 1;
	n = 0;
	for (p = p2head; p; p = p->nxt) {
		*(struct fseg *)(arr + (n << 4)) = *(struct fseg *)p;
		if (swapflg)
			swap_quad(arr + (n << 4));
		n++;
	}
	init_entry(hdr, n);
	if (swapflg)
		swab(hdr, hdr, 0x18);
	mput(&toutb, hdr, 0x18);
	mput(&toutb, arr, n << 4);
}

/* open_tmp: create a temporary or output file (0x22a4..0x2346 of /bin/sld) */
open_tmp(out, flag)
struct buf *out;
int flag;
{
	register int fd;
	char *name;

	if (flag)
		name = tfname;
	else
		name = ofilnam;
	fd = creat(name, 0x1b6);
	if (fd < 0)
		report_error(2, flag ? "cannot create temp" : "cannot create output");
	close(fd);
	out->fildes = open(name, 2);
	if (flag)
		unlink(tfname);
	out->nleft = 0x100;
	out->xnext = out->iobuf;
}

/* init_entry: fill the header of the output (objhdr) from the totals and the flags (0x2346..0x23fe of /bin/sld) */
init_entry(p, a)
struct objhdr *p;
int a;
{
	p->hmagic = filhdr.fmagic;
	p->hx = ttotal + dtotal;
	if (p->hx > 0x10000 && tbflag)
		report_error(2, "cannot have segment overflow with t or b option");
	p->hbss = btotal;
	p->hsegsz = a << 4;
	p->hrelflg = filhdr.relflg;
	if (zflag)
		p->hrelflg |= 0x20;
	p->hsymsz = 0;
	p->hentry = filhdr.entry;
	p->hz = 0;
	p->hy = 0;
}

/* load2arg: pass two for one argument (object file or archive member) (0x23fe..0x2492 of /bin/sld) */
load2arg(p)
char *p;
{
	register char *q;
	register long *lp;
	int spare[2];	/* unused: the frame of the original has 4 more bytes */

	q = p;
	if (getfile(q) == 0) {
		goto test0;
body0:
		q++;
test0:
		if (*q != 0)
			goto body0;
		for (;;) {
			if (q < p)
				break;
			if (*--q == 0x2f)
				break;
		}
		q++;
		load2(0L, q);
	} else {
		lp = libp;
		goto test1;
body1:
		dseek(&text, *lp, 26);
		mget(&archdr, 26);
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
int load2(a, b)
long a;
char *b;
{
	register int k;
	register struct symbol *rec;
	register struct local *t;
	int n;
	char c;
	char d;
	int spare[5];	/* unused: the frame of the original has 10 more bytes */

	readhdr(a, 2);
	mkfsym(b);
	t = local;
	a += (unsigned)((hdrsz + segtsz) & ~1) >> 1;
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
			report_error(2, "internal error: symbol not found");
		if (cursym.stype != 0x20)
			goto other;
		if (t >= (struct local *)&symtab)
			report_error(2, "Local symbol overflow");
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
		report_error(1, "- Multiply defined");
next:
		k += n;
	}
	load2seg(a, t);
}

/* load2seg: pass two: load the text and data of one segment of an object (0x2624..0x27d8 of /bin/sld) */
int load2seg(a, b)
long a;
struct local *b;
{
	struct {
		int tw0, tw1, tw2, tw3, tw4, tw5, tw6, tw7;
	} tmp;
	unsigned i;
	int lim;
	int n;
	long base;
	long acc;
	long hw;

	acc = a;
	base = a - segtsz / 2;
	n = 8;
	lim = segtsz / (n * 2);
	hw = (filhdr.tsize + filhdr.dsize) >> 1;
	for (i = 0; i < lim; i++) {
		dseek(&text, base + i * n, n * 2);
		mget(&tmp, n * 2);
		if (swapflg)
			swap_quad(&tmp);
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
int load2td(p, q, r)
struct local *p;
struct buf *q;
struct buf *r;
{
	struct symbol *save;
	int spare[3];	/* unused: the frame of the original has 6 more bytes */
	int flag;
	register int w13;
	register int w12;
	register int w11;
	register struct symbol *e;
	register int w10;

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
		} else
			w12 = *text.ptr++;
		if (--reloc.size <= 0) {
			if (reloc.size < 0)
				report_error(2, "Relocation error");
			reloc.size++;
			w13 = get(&reloc);
		} else if (--reloc.nibuf < 0) {
			reloc.nibuf++;
			reloc.size++;
			w13 = get(&reloc);
		} else
			w13 = *reloc.ptr++;
	if (swapflg) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	if (!(w13 & 8)) {
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 2:
	if (!(w13 & 1))
		report_error(2, "bad segmented bit in relocation word");
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (long)(w12 & 0xff);
	cursym.sseg = (unsigned)w13 >> 8;
	map_sym();
	if ((w12 = (int)cursym.svalue) & 0xff00)
		report_error(2, "short segmented offset too large");
addhi:
	w12 += ((int)(cursym.svalue >> 16)) & 0xff00;
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
		report_error(2, "bad segmented bit in relocation word");
	w10 = w12;
	cursym.sseg = (unsigned)w13 >> 8;
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (long)w12;
	map_sym();
	w10 = (w10 & 0x8000) | (int)(cursym.svalue >> 16);
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
	if (!((w13 != 0xff0f) & w11 & 0xff0f))
		goto mapsym;
	w13 &= 0xf0;
	w13 |= w11 & 0xff0f;
mapsym:
	cursym.sseg = (unsigned)w13 >> 8;
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (long)w12;
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
	w12 = (int)cursym.svalue;
	goto setseg;
	default:
		report_error(2, "unrecognized relocation address type");
	}
	goto emit;
	} else {
		e = table_lookup(p, w13);
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
	if ((w12 = ((int)e->svalue + 0xff) & w12) & 0xff00)
		report_error(2, "short segment offset too large");
	w12 += ((int)(e->svalue >> 16)) & 0xff00;
	goto emit;
		break;
	case 1:
	if (!(w13 & 1))
		report_error(2, "bad segmented bit in relocation word");
	w10 = (w12 & 0x8000) | ((int)(e->svalue >> 16) & 0xff00);
	if (swapflg)
		w10 = swap_bytes(w10);
	put_int_buf(w10, q);
	save = e;
	w11 = w13;
	flag = 1;
	goto top;
	case 0:
	if (!flag)
		goto extoff;
	flag = 0;
	if (e != save)
		report_error(2, "mismatched segment & offset relocation words");
	w11 = (e->stype - 0x21) * 2 + 0x11;
	w11 = (e->sseg << 8) + (w11 & 0xff);
	if (swapflg)
		w11 = swap_bytes(w11);
	if (rflag)
		put_int_buf(w11, r);
extoff:
	w13 = (e->stype - 0x21) * 2 + 1;
	w13 = (e->sseg << 8) + (w13 & 0xff);
	w12 = w12 + (int)e->svalue;
	goto emit;
	default:
		report_error(2, "unrecognized relocation address type");
	}
	goto emit;
	}end:
	;
}

/* finish_output: write the remaining parts of the output and patch the header (0x2cbc..0x2d4c of /bin/sld) */
finish_output()
{
	copy_segs();
	if (rflag)
		copy(&routb);
	if (!sflag) {
		if (!xflag)
			copy(&soutb);
		put_all();
	}
	flush_buffer(&toutb);
	close(toutb.fildes);
	if (!ofilfnd) {
		unlink(aoutnam);
		link("l.out", aoutnam);
		ofilnam = aoutnam;
	}
	delarg = exstat;
	patch_hdr();
	sig_handler();
}

/* patch_hdr: rewrite the header of the output file with the final values (0x2d4c..0x2dde of /bin/sld) */
patch_hdr()
{
	int n;
	struct {
		int pad[6];
		int syms;
		int rest[5];
	} h;

	n = open(ofilnam, 1);
	if (n == -1)
		report_error(2, "Unable to reopen output file");
	if (lseek(n, (long)((char *)&h.syms - (char *)&h), 0) == -1)
		report_error(2, "Unable to seek in output file");
	nwsym = nwsym * 14;
	if (write(n, &nwsym, 2) != 2)
		report_error(2, "Unable to write in output file");
}

/* copy: copy a temporary file to the output (0x2dde..0x2e4c of /bin/sld) */
copy(p)
struct buf *p;
{
	register int n;
	register int c;
	register int *q;

	flush_buffer(p);
	n = p->fildes;
	lseek(n, 0L, 0);
	while ((c = read(n, tmpoutb.iobuf, 0x200)) > 1) {
		c >>= 1;
		q = (int *)tmpoutb.iobuf;
		do {
			put_int_buf(*q++, &toutb);
		} while (--c);
	}
	close(n);
}

/* high_byte: return x >> 8; internal error if the low byte of x is not 0 (0x2e4c..0x2e76 of /bin/sld) */
high_byte(x)
int x;
{
	if ((x & 0xff00) != x)
		report_error(2, "internal error - mkchar");
	return ((unsigned)x >> 8) & 0xff;
}

/* mget: read n bytes from the current input into a buffer (0x2e76..0x2f04 of /bin/sld) */
mget(p, n)
char *p;
int n;
{
	register int *q;
	register char *d;
	register int k;

	k = n;
	k >>= 1;
	d = p;
	text.nibuf -= k;
	if (text.nibuf >= 0) {
		text.size -= k;
		if (text.size > 0) {
			q = text.ptr;
			do {
				*(int *)d = *q;
				q++;
				d += 2;
			} while (--k);
			text.ptr = q;
			return;
		}
		text.size += k;
	}
	text.nibuf += k;
	do {
		*(int *)d = get(&text);
		d += 2;
	} while (--k);
}

/* mput: write n bytes to an output buffer (0x2f04..0x2f3a of /bin/sld) */
mput(q, a, n)
struct buf *q;
int *a;
int n;
{
	register int k;
	register int x;
	register int *p;

	p = a;
	k = n >> 1;
	do {
		put_int_buf(*p++, q);
	} while (--k);
}

/* dseek: seek in the input to a given position (page buffered) (0x2f3a..0x3090 of /bin/sld) */
int dseek(sp, v, n)
struct stream *sp;
long v;
int n;
{
	int nw;
	long hi;
	long lo;
	register struct page *cur;
	register struct stream *obj;

	hi = (v >> 8) & 0xffffff;
	lo = v & 0xff;
	obj = sp;
	obj->pno->nuser -= 1;
	cur = &page[0];
	if ((long)cur->bno == hi)
		goto common;
	cur = &page[1];
	if ((long)cur->bno == hi)
		goto common;
	if (cur->nuser) {
		cur = &page[0];
		if (cur->nuser) {
			report_error(2, "No pages");
			goto common;
		}
	}
	if (page[0].nuser == 0 && page[1].nuser == 0 && page[0].bno < page[1].bno)
		cur = &page[0];
	cur->bno = hi;
	lseek(infil, (v & 0xffffff00L) << 1, 0);
	nw = read(infil, cur->buff, 0x200) >> 1;
	if (nw < 0)
		nw = 0;
	cur->nibuf = nw;
common:
	cur->nuser += 1;
	obj->bno = hi;
	obj->pno = cur;
	obj->ptr = (int *)((char *)cur->buff + (lo << 1));
	if (n != -1)
		obj->size = half(n);
	obj->nibuf = cur->nibuf - (int)lo;
	if (obj->nibuf <= 0)
		obj->size = 0;
}

/* half: x / 2 as an unsigned word (0x3090..0x30a4 of /bin/sld) */
half(x)
int x;
{
	return (x >> 1) & 0x7fff;
}

/* get: next byte of a stream; reads the next page and checks the end of the member (0x30a4..0x3144 of /bin/sld) */
get(sp)
struct stream *sp;
{
	register struct stream *q;

	q = sp;
	if (--q->nibuf < 0) {
		dseek(q, (long)(q->bno + 1) << 8, -1);
		--q->nibuf;
	}
	if (--q->size <= 0) {
		if (q->size < 0)
			report_error(2, premeof);
		fpage.nuser++;
		--q->pno->nuser;
		q->pno = (struct page *)&fpage;
	}
	return *q->ptr++;
}

/*
 * The prefixes of the library search ("", "/usr", "/z/bin") lie in front of the other strings of getfile in the data of the
 * original and the empty string is shared by its three uses: they are strings of their own before the function.
 */
static char pfx0[] = "";
static char pfx1[] = "/usr";
static char pfx2[] = "/z/bin";

/* getfile: open an input file, searching the library directories (0x3144..0x3408 of /bin/sld) */
int getfile(name)
char *name;
{
	register char *s;
	register char *p;
	struct {
		char pad[0x16];
		long mtime;
		long ctime;
	} st;

	s = name;

	infil = -1;
	archdr.aname[0] = 0;
	filname = name;
	if (s[0] == '-' && s[1] == 'l') {
		if (segmode == 1) {
			strcpy(libname, "/lib/slib");
			p = rindex(libname, '/') + 5;
		} else {
			strcpy(libname, "/lib/lib");
			p = rindex(libname, '/') + 4;
		}
		filname = libpath;
		if (s[2] == 0)
			s = "-lc";
		strcpy(p, s + 2);
		strcat(p, ".a");
		strcpy(libpath, pfx0);
		strcat(libpath, libname);
		infil = open(libpath, 0);
		if (infil == -1) {
			strcpy(libpath, pfx0);
			strcat(libpath, pfx1);
			strcat(libpath, libname);
			infil = open(libpath, 0);
			if (infil == -1) {
				strcpy(libpath, pfx0);
				strcat(libpath, pfx2);
				strcat(libpath, libname);
				infil = open(libpath, 0);
				if (infil == -1)
					report_error(2, "cannot open library");
			}
		}
	}
	if (infil == -1) {
		infil = open(filname, 0);
		if (infil < 0)
			report_error(2, "cannot open");
	}
	page[1].bno = -1;
	page[0].bno = -1;
	page[1].nuser = 0;
	page[0].nuser = 0;
	reloc.pno = (struct page *)&fpage;
	text.pno = (struct page *)&fpage;
	fpage.nuser = 2;
	dseek(&text, 0L, 2);
	if (text.size <= 0)
		report_error(2, premeof);
	if (get(&text) != -155)
		return 0;
	dseek(&text, 1L, 0x1a);
	if (text.size <= 0)
		return 1;
	mget(&archdr, 0x1a);
	if (strncmp(archdr.aname, symdef, 14))
		return 1;
	fstat(infil, &st);
	if (st.mtime > archdr.atime)
		return 3;
	return 2;
}

/* report_error: print a message with the program and file name; fatal depending on the level (0x3408..0x34b8 of /bin/sld) */
int report_error(level, msg, a, b, c, d, e)
int level;
char *msg;
int a;
int b;
int c;
int d;
int e;
{
	if (!exstat)
		printf("%s: ", pname);
	if (filname)
		printf("%s", filname);
	if (archdr.aname[0])
		printf("(%.14s)", archdr.aname);
	printf(": ");
	printf(msg, a, b, c, d, e);
	printf("\n");
	if (level > 1)
		sig_handler();
	exstat = level;
}

/* table_lookup: find an entry of the local symbol table (botch on failure) (0x34b8..0x3516 of /bin/sld) */
struct symbol *table_lookup(end, a)
struct local *end;
int a;
{
	register int k;
	register int x;
	register struct local *p;
	register struct local *e;
	int spare[5];	/* unused: the frame of the original has 10 more bytes */

	e = end;
	k = (a >> 4) & 0x0fff;
	p = local;
	while (p < e) {
		if (k == p->locindex)
			return p->locsymbol;
		p++;
	}
	printf("botch on %x\n", a);
	report_error(2, "Local symbol botch");
}

/* readhdr: read and convert the header of an object file or member (0x3516..0x3760 of /bin/sld) */
int readhdr(a, b)
long a;
int b;
{
	int magic;
	struct objhdr tmp;
	struct fseg rec[512];
	int spare[3];	/* unused: the frame of the original has 6 more bytes (rec is 8198 bytes?) */
	register struct fseg *p;
	register int n;
	register int w;

	dseek(&text, a, 2);
	mget(&magic, 2);
	if (swapflg)
		magic = swap_bytes(magic);
	if (magic == 0xe607 || magic == 0xe611)
		segmode = 1;
	if (magic != 0xe707 && magic != 0xe711) {
		if (!segmode)
			return;
	}
	segobj += 1;
	hdrsz = 0x18;
	dseek(&text, a, hdrsz);
	mget(&tmp, hdrsz);
	if (swapflg)
		swab(&tmp, &tmp, hdrsz);
	segtsz = tmp.hsegsz;
	n = segtsz / 16;
	dseek(&text, a + ((unsigned)(hdrsz & ~1) >> 1), segtsz);
	mget(rec, segtsz);
	if (swapflg) {
		for (p = rec; p < rec + n; p++)
			swap_quad(p);
	}
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
		filhdr.tsize += (unsigned)p->s_code;
		filhdr.dsize += (unsigned)p->s_data;
		w |= p->atr;
	}
	if (w & 0xf8)
		report_error(2, "Invalid segment attributes");
	filhdr.entry = tmp.hentry;
	filhdr.relflg = tmp.hrelflg & 1;
	filhdr.ssize = tmp.hsymsz;
	make_blk(rec, n, b);
	return 1;
}

/* make_blk: create the segment table block for an object (0x3760..0x3a30 of /bin/sld) */
int make_blk(src, n, kind)
register struct fseg *src;
register int n;
register int kind;
{
	register int j;
	int idx;
	int loc[3];
	char key[16];
	struct p1seg *blk;

	fblk = nblk;
	blk = get_block(nblk);
	if (kind == 1) {
		copy_leafname(filname, key);
		j = mlhead;
		idx = 0;
		while (idx < mlcnt) {
			if (str_compare(mlist[j].mname, key))
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
			report_error(2, "pass one segment table overflow --Try -Ap option");
		if (kind == 1) {
			*(struct fseg *)blk = *src++;
			if (filhdr.fmagic != 0xe707 && filhdr.fmagic != 0xe607)
				report_error(0, "warning: nonsegmented object");
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
			tcum += (unsigned)blk->s_code;
			blk->dbase = dcum;
			dcum += (unsigned)blk->s_data;
		}
		nblk++;
		blk = get_block(nblk);
	}
}

/* str_compare: compare two names: in cmpmode the difference of the first unequal characters, else 1 when equal and 0 when not (0x3a30..0x3a88 of /bin/sld) */
int str_compare(p, q)
char *p;
char *q;
{
	do {
		if (*p == *q)
			q++;
		else {
			if (cmpmode)
				return *p - *q;
			return 0;
		}
	} while (*p++);
	if (!cmpmode)
		return 1;
	return 0;
}

/* put_int_buf: write a word to an output buffer (0x3a88..0x3ac6 of /bin/sld) */
put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

/* flush_buffer: write an output buffer to its file (0x3ac6..0x3b1e of /bin/sld) */
flush_buffer(a)
struct buf *a;
{
	register struct buf *p;
	register int n;

	p = a;
	n = (char *)p->xnext - (char *)p->iobuf;
	if (n > 0) {
		if (write(p->fildes, p->iobuf, n) != n)
			report_error(2, "output error");
	}
	p->xnext = p->iobuf;
	p->nleft = 0x100;
}

/* swab12: swap the bytes of 12 words (never called) (0x3b1e..0x3b58 of /bin/sld) */
swab12(p)
register char *p;
{
	register int k;
	register int t;

	k = 12;
	do {
		t = p[0];
		p[0] = p[1];
		p[1] = t;
		p += 2;
	} while (--k);
}

/* warn_sym_redef: warn that a symbol is redefined in an archive (0x3b58..0x3c16 of /bin/sld) */
warn_sym_redef(p, a)
struct symbol *p;
struct symbol *a;
{
	register char *s;
	char c;
	int n;
	int m;

	if (wflag)
		return;
	if (p != 0) {
		c = p->stype;
		if (!c & 0x20 || c == 0x20)
			return;
	}
	c = a->stype;
	if (!c & 0x20 || c == 0x20)
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
	report_error(0, "- warning: symbol redefined in archive");
}
