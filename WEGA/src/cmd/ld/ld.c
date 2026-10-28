/*
 * ld.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]ld.c\t3.8  10/27/83 12:16:07 - 87wega3.2";
int g0474 = -1;
int g0476 = 0;
int bE478 = 0;
int g047a = 0;
long bE47C = 0;
long bE480 = 0;
int g0484 = 0;
long *g0486 = (long *)tab3e8a;
char *bE048A = "a.out";
char *p0494 = "l.out";
char g049e[] = "/tmp/ldaXXXXX";
char *ptrs04ac[] = {"all", "text", "data", "bss"};
char g04ce[] = "__.SYMDEF";
char g04d8[] = "Premature EOF";
int g04e6 = 0;
int g04e8 = 4;
int g04ea = 1;
int bE4EC = 2000;
unsigned g04ee = 2000;
int bE4F0 = 0;

/* main..0a5c: findings:D2,D7 */
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
	pE4D6 = rindex(*argv, '/');
	if (pE4D6 == 0)
		pE4D6 = *argv;
	else
		pE4D6++;
	for (i = 0; i < 128; i++) {
		tblF332[i].bssptr = -1;
		tblF332[i].datptr = -1;
		tblF332[i].txtptr = -1;
		tblF332[i].res = -1;
	}
	symtab_alloc(g04ee);
	scan_options(argc, argv);
	segtab_alloc(bE4EC);
	bE4A46 = 0;
	i = 1;
	bE4A36 = argv + 1;
	goto test;
body:
	bE4C0 = 0;
	s = *bE4A36++;
	if (s[0] != '-')
		goto lib;
	j = 1;
l08a2:
	if (s[j] == 0)
		goto next;
	switch (s[j]) {
	case 'f':
		savei = i;
		i = -1;
		savep = bE4A36;
		savev = argv;
		savec = argc;
		argv = g4a40;
		argc = bE4A44;
		bE4A36 = argv;
		bE4A46++;
		break;
	case 'A':
		j++;
	case 'e':
	case 'o':
	case 'u':
		i++;
		bE4A36++;
		j++;
		goto l08a2;
	case 'l':
		load_lib_syms(s + (j - 1));
		break;
	case 'b':
	case 't':
		if (bE49C)
			report_error(2, "Cannot use -t or -b options with -M");
		bE4A8 = 1;
		i++;
		if (i >= argc)
			report_error(2, "-%c: arg missing", s[j]);
		if (reg_sect_addr(s + j, *bE4A36))
			j++;
		bE4A36++;
		j++;
		goto l08a2;
	case 'M':
		if (bE4A8)
			report_error(2, "Cannot use -t or -b options with -M");
		bE49C++;
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
	bE4A36 = savep;
	argv = savev;
	argc = savec;
	bE4A46 = 0;
test:
	if (i < argc)
		goto body;
	if (bE4A46)
		goto restore;
	if (bE4A2 && bE42B2)
		printf("%s: warning, -r option overrides -t, -b\n", pE4D6);
	parse_args(argc, argv);
}

char tmpname[] = "l.out";

/* sig_handler 0x0a5c..0x0aba: done */
sig_handler()
{
	unlink(tmpname);
	if (!g04e8 && !(bE496 && g04e6) && !bE4A2) {
		chmod(p0494, (~umask(0)) & 0x1ff);
	}
	exit(g04e8);
}

/* init_signals 0x0aba..0x0b04: done */
init_signals()
{
	if (signal(2, 1L) != 1)
		signal(2, sig_handler);
	if (signal(15, 1L) != 1)
		signal(15, sig_handler);
}

/* scan_options..0f26: findings:D2,D7 */
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
		bE4A3C = (char **)*argv;
		argc--;
		savev = argv;
		savec = argc;
		read_words(bE4A3C);
		argv = g4a40 - 1;
		argc = bE4A44 + 1;
		fflag++;
		bE4A46++;
		goto next;
	case 'X':
		bE4AC++;
		goto next;
	case 'S':
		bE4A6++;
		goto next;
	case 'R':
		if (sscanf(*argv, "-R%d", &v0a) < 1)
			report_error(2, "invalid syntax for R option");
		(*(int *)bE49E)++;
		f42f8(v0a, 'r');
		goto next;
	case 'r':
		bE4A2++;
		bE496++;
		goto next;
	case 's':
		bE4A4++;
	case 'x':
		bE4AA++;
		goto next;
	case 'e':
	case 'u':
		if (argc <= 1)
			report_error(2, "Bad 'use' or 'entry'");
		p = (char **)f6b5a(argv[1]);
		f6bae(p);
		if ((*argv)[1] == 'e')
			bE492 = (long)bE4FC;
		goto skip2;
	case 'd':
		bE498++;
		goto next;
	case 'i':
		bE49A++;
		bE3E80 = 1;
		bE3E7C = 1;
		if (bE3E82 == 0)
			bE3E82++;
		goto next;
	case 'O':
		bE4A0++;
		goto next;
	case 'o':
		if (argc <= 1)
			report_error(2, "Bad output file");
		argv++;
		p0494 = *argv;
		if (access(p0494, 0) == 0) {
			if (access(p0494, 2) != 0)
				report_error(2, "%s is write protected", p0494);
		}
		argc--;
		bE4BC++;
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
			g04ee = v08;
			f702a(v08);
			goto skip2;
		case 'p':
			if (sscanf(argv[1], "%d", &v08) != 1)
				report_error(2, "Unable to convert pass 1 segment table size specification");
			if ((long)(unsigned)v08 + 2 << 2 > 0xfff8L)
				report_error(2, "Pass 1 segment table size specification is too big");
			bE4EC = v08;
			goto skip2;
		default:
			report_error(2, "Invalid character after -A");
		}
skip2:
		argc--;
		argv++;
		goto next;
	case 'w':
		bE4BA++;
		goto next;
	case 'z':
		f42f8(0x7f, 's');
		bE4AE = 1;
		goto next;
	case 'N':
		switch ((*argv)[2]) {
		case 'l':
			g04ea = 1;
			break;
		case 's':
			g04ea = 0;
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
	if (bE4A46--) {
		savev++;
		argv = savev;
		argc = --savec;
		goto test;
	}
}

/* read_words..1070: findings:D15 */
int read_words(name)
char *name;
{
	int fd;
	int n;
	int idx;
	char c;
	int inword;
	char *p;

	p = buf6988;
	tab4a48[0] = buf6988;
	fd = open(name, 0);
	if (fd < 0)
		report_error(2, "Unable to open command file");
	inword = 0;
	idx = 0;
	goto rd;
body:
	if (n == -1)
		report_error(2, "Bad command file read");
	if (!(tab1aad[c] & 8)) {
		inword = 0;
		*p = c;
		p++;
		if (p > buf9098)
			report_error(2, "Command file arguments too long");
	} else if (!inword) {
		inword++;
		*p = 0;
		p++;
		if (p > buf9098)
			report_error(2, "Too many command file arguments");
		idx++;
		if (idx > 2000)
			report_error(2, "Command file arguments too long");
		tab4a48[idx] = p;
	}
rd:
	n = read(fd, &c, 1) != 0;
	if (n)
		goto body;
	g4a40 = tab4a48;
	bE4A44 = idx;
	close(fd);
	return 0;
}

/* reg_sect_addr..11ea: findings:D6,D15 */
int reg_sect_addr(s, t)
char *s;
char *t;
{
	register int k;
	register int g;
	register char *u;
	char *fmt;

	bE42B2++;
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
	if (bE49A && (k == 2 || k == 3))
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
	if (sscanf(u, fmt, &tab4534[g][k]) != 1)
		report_error(2, "-%.2s %s: bad address", s, t);
	return k != 0;
}

/* parse_args..1364: findings:D2,D5,D6,D7,D15,D16 */
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
	bE4C0 = 0;
	end_pass1();
	start_output();
	av = argv + 1;
	bEF32C = 0;
	bE3C02 = 0;
	g0486 = (long *)tab3e8a;
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
		argv = g4a40;
		argc = bE4A44;
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

/* load_lib_syms 0x1364..0x14a4: findings:D15 */
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
		bE3E88 = archdr.asize / 12;
		bE3E84 = (struct rlent *)ensure_buf(archdr.asize);
		lseek(bE4BE, 28L, 0);
		read(bE4BE, bE3E84, bE3E88 * 12);
		f6f46(bE3E84, bE3E88);
		while (f1560())
			;
		*g0486++ = -1;
		break;
	case 3:
		report_error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(bE4BE);
}

/* load_header..1560: done */
load_header(p)
long p;
{
	int magic;

	dseek(&text, p, 0x1c);
	if (text.size <= 0) {
		*g0486++ = -1;
		return 0;
	}
	mget(&archdr, 0x1a);
	mget(&magic, 2);
	if (bE4B6)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe607 || magic == 0xe611) {
		if (read_symbols(1, p + 13)) {
			*g0486++ = p;
		}
	}
	return 1;
}

/* f1560..1664: findings:D2,D5,D8 */
int f1560()
{
	int i;
	struct rlent *end;
	struct symbol *t;
	long *sv;

	sv = g0486;
	end = bE3E84;
	i = 0;
	goto test;
body:
	if (bE3E84 + i < end)
		goto next;
	if ((bE3E84[i].rname[0] & 0x80) != 0)
		end = (struct rlent *)(bE3E84[i].rname + (bE3E84[i].rname[0] & 0x7f));
	t = *f6b84(bE3E84[i].rname);
	if (!t)
		goto next;
	if (t->stype != 0x20)
		goto next;
	if (bE4B6)
		swap_pair(&bE3E84[i]);
	load_header(bE3E84[i].roff >> 1);
next:
	i++;
test:
	if ((unsigned)i < (unsigned)bE3E88)
		goto body;
	return sv != g0486;
}

/* add16_checked..1696: findings:D4 */
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

/* read_symbols..18f2: findings:D1,D2 */
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

	f3516(c, 1);
	cnt = 0;
	k = 1;
	saved = bE4FA;
	if ((filhdr.relflg & 1) == 1) {
		report_error(1, "No relocation bits");
		return 0;
	}
	c = (long)(unsigned)(((bE436 + bE434) & 0xfffe) >> 1) + filhdr.tsize + filhdr.dsize + c;
	dseek(&text, c, filhdr.ssize);
	while (text.size > 0) {
		n = read_entry_hdr();
		type = cursym.stype;
		if (bE4A6) {
			d = type & 0x1f;
			if (d == 1 || d > 4)
				continue;
		}
		if (!((type & 0x20) != 0)) {
			if (bE4AC && cursym.sname[0] == 0x4c)
				continue;
			k += n + 1;
			continue;
		}
		slot = lookup_name();
		if (f6bae(slot)) {
			cur = bE4FC;
			if (cur->stype == ' ')
				continue;
		} else {
			cur = bE4FC;
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
		cur->sseg = bE3C02 + cursym.sseg;
		bE4D0 = (blk = get_block(cur->sseg))->ctrel;
		bE4D2 = blk->cdrel;
		bE4D4 = blk->cbrel;
		*(int *)&cur->svalue = 0;
		f6da8();
		cur->stype = cursym.stype;
lstore:
		cur->svalue = cursym.svalue;
	}
	if (a == 0 || cnt != 0) {
		bE4CE = add16_checked(bE4CE, k << 4, "symbol table overflow");
		return 1;
	}
	while (bE4FA > saved) {
		bE4FA--;
		p2 = tblE500[bE4FA];
		if (p2)
			*p2 = 0;
	}
	bEF32C = bE3C02;
	return 0;
}

/* place_sec..1f96: done */
int place_sec(p, k)
register struct p2seg *p;
int k;
{
	register int j;
	register int i;
	register int z;
	register unsigned r8;
	int u0;
	struct tab428a *q;
	int u1;
	int a;
	int b;
	int c;

	bF328 = 0;
	bF324 = 0;
	a = p->s_code;
	b = p->s_data;
	c = p->s_bss;
	tab428a[1].kind = bE49A ? 4 : 0;
	tab428a[2].kind = bE49A ? 0 : 1;
	tab428a[3].kind = b ? 2 : 1;
	tab428a[1].tsize = a;
	tab428a[2].tsize = b;
	tab428a[3].tsize = c;
	for (i = 0; i < 4; i++) {
		tab428a[i].tend = 0;
		tab428a[i].tstart = 0;
	}
	if (bE42B4[k][0]) {
		i = bE49A ? 2 : 1;
		for (; i < 4; i++) {
			if (bE42B4[k][i]) {
				printf("%s: warning, -%c overrides -%c%c\n", pE4D6, bE42B4[k][0], bE42B4[k][i], *ptrs04ac[i]);
				bE42B4[k][i] = 0;
			}
		}
	}
	tab428a[0].kind = 4;
	tab428a[1].kind = 0;
	tab428a[2].kind = 1;
	tab428a[0].tsize = a + b + c;
	f4a34 = 1;
	if (c)
		f4a34 = 3;
	else if (b)
		f4a34 = 2;
	j = bE42B4[k][0] ? 0 : 1;
	for (; j < 4; j++) {
		q = &tab428a[j];
		r8 = tab428a[q->kind].tend;
		switch (bE42B4[k][j]) {
		case 't':
			if (q->tsize > tab4534[k][j])
				report_error(1, "top address of %s in segment %d too low", ptrs04ac[j], k);
			if ((r8 = tab4534[k][j] & 0xff00) != tab4534[k][j])
				printf("%s: warning, top address of %s in segment %d relocated to 0x%x\n", pE4D6, ptrs04ac[j], k, r8);
			r8 = (r8 - q->tsize) & 0xff00;
			q->tsize = (q->tsize + 255) & 0xff00;
			break;
		case 'b':
			r8 = tab4534[k][j] & 0xff00;
			if (r8 != tab4534[k][j])
				printf("%s: warning, bottom address of %s in segment %d relocated to 0x%x\n", pE4D6, ptrs04ac[j], k, r8);
			break;
		}
		tab428a[0].tsize = 0;
		q->tend = r8 + q->tsize;
		if (r8 > q->tend)
			report_error(1, "%s area wrap around", ptrs04ac[j]);
		for (i = 1; i < j; i++) {
			if (r8 < tab428a[i].tend && q->tend > tab428a[i].tstart) {
				printf("sld: warning, %s and %s overlap in segment %d - creating overlay segment\n", ptrs04ac[i], ptrs04ac[j], k);
				f1f96(p, j);
				if (j == 2) {
					tab428a[2].tend = 0;
					tab428a[3].kind = 1;
					bF320++;
				} else {
					tab428a[3].tend = 0;
					bF322++;
				}
				bE4A0++;
			}
		}
		q->tstart = r8;
	}
	p->torig = tab428a[1].tstart;
	(bF320 ? bF324 : p)->dorig = tab428a[2].tstart;
	(bF322 ? bF328 : p)->borig = tab428a[3].tstart;
	if (bE42B4[k][0]) {
		i = 1;
		if (!a) {
			i = 2;
			if (!b)
				i = 3;
		}
		bE42B4[k][i] = 1;
	}
	if (bE42B4[k][1]) {
		p->atr |= 0x10;
		p->s_coff = high_byte(p->torig);
	}
	if (bE42B4[k][2]) {
		if (bF320) {
			bF324->atr |= 0x20;
			bF324->s_doff = high_byte(bF324->dorig);
		} else {
			p->atr |= 0x20;
			p->s_doff = high_byte(p->dorig);
		}
	}
	if (bF322) {
		bF328->atr |= 0x40;
		if (!bE42B4[k][3])
			bF328->borig = (p->s_data + p->dorig + 255) & 0xff00;
		bF328->s_boff = high_byte(bF328->borig);
	} else if (bE42B4[k][3]) {
		p->atr |= 0x40;
		p->s_boff = high_byte(p->borig);
	}
	if (!bE42B4[k][0]) {
		if (bF320 || bF322)
			f4a34 = 0;
		else {
			f4a34 = 1;
			if (tab428a[1].tend < tab428a[2].tend)
				f4a34 = 2;
			else if (tab428a[1].tend < tab428a[3].tend)
				f4a34 = 3;
			switch (f4a34) {
			case 2:
				if (tab428a[2].tend < tab428a[3].tend)
					f4a34 = 3;
				break;
			case 3:
				if (tab428a[2].tend > tab428a[3].tend)
					f4a34 = 2;
				break;
			}
		}
	}
}

/* f1f96..2056: findings:D4 */
f1f96(p, kind)
struct p2seg *p;
int kind;
{
	if (bE3C04 > 0x100)
		report_error(2, "Pass2 segment table overflow");
	if (kind == 2) {
		bF324 = table32_entry(bE3C04++);
		bF324->segno = 0xff;
		bF324->s_data = p->s_data;
		bF324->atr |= 2;
		p->s_data = 0;
		p->atr &= ~2;
		p->s_coff = 0;
	} else {
		bF328 = table32_entry(bE3C04++);
		bF328->segno = 0xff;
		bF328->s_bss = p->s_bss;
		bF328->atr |= 4;
		p->s_bss = 0;
		p->atr &= ~4;
		p->s_boff = 0;
	}
}

/* start_output..22a4: findings:D2 */
int start_output()
{
	register int n;
	register struct p2seg *p;
	long u1;
	int u2;
	char arr[8192];
	char hdr[24];

	open_tmp(&bE4DA, 0);
	mktemp(g049e);
	open_tmp(&bE6E2, 1);
	open_tmp(&bE8EA, 1);
	if (!bE4A4 || !bE4AA)
		open_tmp(&bF10A, 1);
	if (bE4A2) {
		open_tmp(&bEF02, 1);
		open_tmp(&bEAF2, 1);
		open_tmp(&bECFA, 1);
	}
	filhdr.fmagic = bE4B4 == 1 ? (bE4A0 ? 0xe605 : bE49A ? 0xe611 : 0xe607) : (bE4A0 ? 0xe705 : bE49A ? 0xe711 : 0xe707);
	filhdr.tsize = bE3C16;
	filhdr.dsize = bE3C1A;
	filhdr.bsize = b3C1E;
	filhdr.ssize = bE4A4 ? 0 : (((unsigned)bE4CE >> 4) + bE4FA) * 12;
	if (bE492) {
		if (bE492->stype != '"')
			report_error(1, "Entry point not in text");
		else
			filhdr.entry = bE492->svalue;
	} else {
		for (p = bF32E; p; p = p->nxt) {
			if (p->s_code) {
				filhdr.entry = (long)(unsigned)p->torig + ((long)p->segno << 24);
				break;
			}
		}
	}
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	filhdr.relflg = bE4A2 ? 0 : 1;
	n = 0;
	for (p = bF32E; p; p = p->nxt) {
		*(struct fseg *)(arr + (n << 4)) = *(struct fseg *)p;
		if (bE4B6)
			swap_quad(arr + (n << 4));
		n++;
	}
	init_entry(hdr, n);
	if (bE4B6)
		swab(hdr, hdr, 0x18);
	f2f04(&bE4DA, hdr, 0x18);
	f2f04(&bE4DA, arr, n << 4);
}

/* open_tmp 0x22a4..0x2346: done */
open_tmp(out, flag)
struct buf *out;
int flag;
{
	register int fd;
	char *name;

	if (flag)
		name = g049e;
	else
		name = p0494;
	fd = creat(name, 0x1b6);
	if (fd < 0)
		report_error(2, flag ? "cannot create temp" : "cannot create output");
	close(fd);
	out->fildes = open(name, 2);
	if (flag)
		unlink(g049e);
	out->nleft = 0x100;
	out->xnext = out->iobuf;
}

/* init_entry..23fe: done */
init_entry(p, a)
struct objhdr *p;
int a;
{
	p->hmagic = filhdr.fmagic;
	p->hx = bE3C16 + bE3C1A;
	if (p->hx > 0x10000 && bE4A8)
		report_error(2, "cannot have segment overflow with t or b option");
	p->hbss = b3C1E;
	p->hsegsz = a << 4;
	p->hrelflg = filhdr.relflg;
	if (bE4AE)
		p->hrelflg |= 0x20;
	p->hsymsz = 0;
	p->hentry = filhdr.entry;
	p->hz = 0;
	p->hy = 0;
}

/* load2arg..2492: findings:D2 */
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
		lp = g0486;
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
		g0486 = lp;
	}
	close(bE4BE);
}

/* load2..2624: findings:D2,D7 */
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

	f3516(a, 2);
	f6a08(b);
	t = tab84d8;
	a += (unsigned)((bE436 + bE434) & ~1) >> 1;
	dseek(&text, a + filhdr.tsize + filhdr.dsize, filhdr.ssize);
	k = 0;
	while (text.size > 0) {
		n = read_entry_hdr() + 1;
		f6c6a();
		c = cursym.stype;
		if (bE4A6) {
			d = c & 0x1f;
			if (d == 1 || d > 4)
				goto next;
		}
		if ((c & 0x20) == 0) {
			if (bE4A4 || bE4AA)
				goto next;
			if (bE4AC && cursym.sname[0] == 0x4c)
				goto next;
			f6e1a(&bF10A, &cursym);
			goto next;
		}
		rec = *lookup_name();
		if (!rec)
			report_error(2, "internal error: symbol not found");
		if (cursym.stype != 0x20)
			goto other;
		if (t >= (struct local *)&gE47E)
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
	f2624(a, t);
}

/* f2624 0x2624..0x27d8: done */
int f2624(a, b)
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
	base = a - bE434 / 2;
	n = 8;
	lim = bE434 / (n * 2);
	hw = (filhdr.tsize + filhdr.dsize) >> 1;
	for (i = 0; i < lim; i++) {
		dseek(&text, base + i * n, n * 2);
		mget(&tmp, n * 2);
		if (bE4B6)
			swap_quad(&tmp);
		switch (tmp.tw5 & 3) {
		case 1:
			dseek(&text, acc, tmp.tw2);
			dseek(&reloc, acc + hw, tmp.tw2);
			acc += half(tmp.tw2);
			f27d8(b, &bE6E2, &bEF02);
			break;
		case 3:
			dseek(&text, acc, tmp.tw2);
			dseek(&reloc, acc + hw, tmp.tw2);
			acc += half(tmp.tw2);
			f27d8(b, &bE6E2, &bEF02);
		case 2:
			dseek(&text, acc, tmp.tw3);
			dseek(&reloc, acc + hw, tmp.tw3);
			acc += half(tmp.tw3);
			f27d8(b, &bE8EA, &bEAF2);
			break;
		}
	}
}

/* f27d8 0x27d8..0x2cbc: done */
int f27d8(p, q, r)
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
	if (bE4B6) {
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
	f6c6a();
	if ((w12 = (int)cursym.svalue) & 0xff00)
		report_error(2, "short segmented offset too large");
c2954:
	w12 += ((int)(cursym.svalue >> 16)) & 0xff00;
c2964:
	w13 = (cursym.sseg << 8) + (w13 & 0xff);
emit:
	if (bE4B6) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	put_int_buf(w12, q);
	if (bE4A2)
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
	f6c6a();
	w10 = (w10 & 0x8000) | (int)(cursym.svalue >> 16);
	if (bE4B6)
		w10 = swap_bytes(w10);
	put_int_buf(w10, q);
	w11 = w13;
	flag = 1;
	goto top;
	case 0:
	if ((w13 & 0xe) == 0)
		goto c2a96;
	if (!flag)
		goto c2a68;
	if (!((w13 != 0xff0f) & w11 & 0xff0f))
		goto c2a68;
	w13 &= 0xf0;
	w13 |= w11 & 0xff0f;
c2a68:
	cursym.sseg = (unsigned)w13 >> 8;
	cursym.stype = ((w13 & 7) >> 1) + 1;
	cursym.svalue = (long)w12;
	f6c6a();
c2a96:
	if (!flag)
		goto c2ad6;
	flag = 0;
	w11 = (cursym.sseg << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = swap_bytes(w11);
	if (bE4A2)
		put_int_buf(w11, r);
c2ad6:
	if ((w13 & 0xe) == 0)
		goto emit;
	w12 = (int)cursym.svalue;
	goto c2964;
	default:
		report_error(2, "unrecognized relocation address type");
	}
	goto emit;
	} else {
		e = table_lookup(p, w13);
		if (e->stype != 0x20)
			goto c2b46;
		w13 = (((w13 & 8) ? w13 : (w13 >> 4)) & 7) + ((index16(e) + bEF312) << 4) + 8;
		goto emit;
	}

c2b46:
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
	if (bE4B6)
		w10 = swap_bytes(w10);
	put_int_buf(w10, q);
	save = e;
	w11 = w13;
	flag = 1;
	goto top;
	case 0:
	if (!flag)
		goto c2c72;
	flag = 0;
	if (e != save)
		report_error(2, "mismatched segment & offset relocation words");
	w11 = (e->stype - 0x21) * 2 + 0x11;
	w11 = (e->sseg << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = swap_bytes(w11);
	if (bE4A2)
		put_int_buf(w11, r);
c2c72:
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

/* finish_output 0x2cbc..0x2d4c: done */
finish_output()
{
	f6134();
	if (bE4A2)
		f2dde(&bECFA);
	if (!bE4A4) {
		if (!bE4AA)
			f2dde(&bF10A);
		f6efe();
	}
	flush_buffer(&bE4DA);
	close(bE4DA.fildes);
	if (!bE4BC) {
		unlink(bE048A);
		link("l.out", bE048A);
		p0494 = bE048A;
	}
	g04e8 = bE4A3A;
	f2d4c();
	sig_handler();
}

/* f2d4c 0x2d4c..0x2dde: done */
f2d4c()
{
	int n;
	struct {
		int pad[6];
		int syms;
		int rest[5];
	} h;

	n = open(p0494, 1);
	if (n == -1)
		report_error(2, "Unable to reopen output file");
	if (lseek(n, (long)((char *)&h.syms - (char *)&h), 0) == -1)
		report_error(2, "Unable to seek in output file");
	bE4F0 = bE4F0 * 14;
	if (write(n, &bE4F0, 2) != 2)
		report_error(2, "Unable to write in output file");
}

/* f2dde..2e4c: done */
f2dde(p)
struct buf *p;
{
	register int n;
	register int c;
	register int *q;

	flush_buffer(p);
	n = p->fildes;
	lseek(n, 0L, 0);
	while ((c = read(n, bE6E2.iobuf, 0x200)) > 1) {
		c >>= 1;
		q = (int *)bE6E2.iobuf;
		do {
			put_int_buf(*q++, &bE4DA);
		} while (--c);
	}
	close(n);
}

/* high_byte 0x2e4c..0x2e76: done */
high_byte(x)
int x;
{
	if ((x & 0xff00) != x)
		report_error(2, "internal error - mkchar");
	return ((unsigned)x >> 8) & 0xff;
}

/* mget 0x2e76..0x2f04: done */
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

/* f2f04 0x2f04..0x2f3a: done */
f2f04(q, a, n)
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

/* dseek..3090: findings:D5,D9,D15 */
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
	lseek(bE4BE, (v & 0xffffff00L) << 1, 0);
	nw = read(bE4BE, cur->buff, 0x200) >> 1;
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

/* half 0x3090..0x30a4: done */
half(x)
int x;
{
	return (x >> 1) & 0x7fff;
}

/* get 0x30a4..0x3144: done */
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
			report_error(2, g04d8);
		fpage.nuser++;
		--q->pno->nuser;
		q->pno = (struct page *)&fpage;
	}
	return *q->ptr++;
}

/* getfile..3408: done */
/*
 * The prefixes of the library search ("", "/usr", "/z/bin") lie in front of the other strings of getfile in the data of the
 * original and the empty string is shared by its three uses: they are strings of their own before the function.
 */
static char pfx0[] = "";
static char pfx1[] = "/usr";
static char pfx2[] = "/z/bin";

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

	bE4BE = -1;
	archdr.aname[0] = 0;
	bE4C0 = name;
	if (s[0] == '-' && s[1] == 'l') {
		if (bE4B4 == 1) {
			strcpy(buf9098, "/lib/slib");
			p = rindex(buf9098, '/') + 5;
		} else {
			strcpy(buf9098, "/lib/lib");
			p = rindex(buf9098, '/') + 4;
		}
		bE4C0 = buf90b6;
		if (s[2] == 0)
			s = "-lc";
		strcpy(p, s + 2);
		strcat(p, ".a");
		strcpy(buf90b6, pfx0);
		strcat(buf90b6, buf9098);
		bE4BE = open(buf90b6, 0);
		if (bE4BE == -1) {
			strcpy(buf90b6, pfx0);
			strcat(buf90b6, pfx1);
			strcat(buf90b6, buf9098);
			bE4BE = open(buf90b6, 0);
			if (bE4BE == -1) {
				strcpy(buf90b6, pfx0);
				strcat(buf90b6, pfx2);
				strcat(buf90b6, buf9098);
				bE4BE = open(buf90b6, 0);
				if (bE4BE == -1)
					report_error(2, "cannot open library");
			}
		}
	}
	if (bE4BE == -1) {
		bE4BE = open(bE4C0, 0);
		if (bE4BE < 0)
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
		report_error(2, g04d8);
	if (get(&text) != -155)
		return 0;
	dseek(&text, 1L, 0x1a);
	if (text.size <= 0)
		return 1;
	mget(&archdr, 0x1a);
	if (strncmp(archdr.aname, g04ce, 14))
		return 1;
	fstat(bE4BE, &st);
	if (st.mtime > archdr.atime)
		return 3;
	return 2;
}

/* report_error..34b8: done */
int report_error(level, msg, a, b, c, d, e)
int level;
char *msg;
int a;
int b;
int c;
int d;
int e;
{
	if (!bE4A3A)
		printf("%s: ", pE4D6);
	if (bE4C0)
		printf("%s", bE4C0);
	if (archdr.aname[0])
		printf("(%.14s)", archdr.aname);
	printf(": ");
	printf(msg, a, b, c, d, e);
	printf("\n");
	if (level > 1)
		sig_handler();
	bE4A3A = level;
}

/* table_lookup 0x34b8..0x3516: done */
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
	p = tab84d8;
	while (p < e) {
		if (k == p->locindex)
			return p->locsymbol;
		p++;
	}
	printf("botch on %x\n", a);
	report_error(2, "Local symbol botch");
}

/* f3516..3760: findings:D2,D16 */
int f3516(a, b)
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
	if (bE4B6)
		magic = swap_bytes(magic);
	if (magic == 0xe607 || magic == 0xe611)
		bE4B4 = 1;
	if (magic != 0xe707 && magic != 0xe711) {
		if (!bE4B4)
			return;
	}
	bE4B2 += 1;
	bE436 = 0x18;
	dseek(&text, a, bE436);
	mget(&tmp, bE436);
	if (bE4B6)
		swab(&tmp, &tmp, bE436);
	bE434 = tmp.hsegsz;
	n = bE434 / 16;
	dseek(&text, a + ((unsigned)(bE436 & ~1) >> 1), bE434);
	mget(rec, bE434);
	if (bE4B6) {
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

/* make_blk..3a30: done */
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

	bE3C02 = bEF32C;
	blk = get_block(bEF32C);
	if (kind == 1) {
		copy_leafname(bE4C0, key);
		j = bE478;
		idx = 0;
		while (idx < g047a) {
			if (str_compare(tab3c22[j].mname, key))
				break;
			j = tab3c22[j].mnext;
			idx++;
		}
		loc[0] = bE3E7E;
		loc[1] = bE3E7C;
		loc[2] = bE3E80;
		if (idx != g047a) {
			if (tab3c22[j].mtext > -1)
				loc[0] = tab3c22[j].mtext;
			if (tab3c22[j].mdata > -1)
				loc[1] = tab3c22[j].mdata;
			if (tab3c22[j].mbss > -1)
				loc[2] = tab3c22[j].mbss;
		}
	}
	while (n--) {
		if (bEF32C >= bE4EC)
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
			blk->tbase = bE47C;
			bE47C += (unsigned)blk->s_code;
			blk->dbase = bE480;
			bE480 += (unsigned)blk->s_data;
		}
		bEF32C++;
		blk = get_block(bEF32C);
	}
}

/* str_compare 0x3a30..0x3a88: findings:D2 */
int str_compare(p, q)
char *p;
char *q;
{
	do {
		if (*p == *q)
			q++;
		else {
			if (g0476)
				return *p - *q;
			return 0;
		}
	} while (*p++);
	if (!g0476)
		return 1;
	return 0;
}

/* put_int_buf 0x3a88..0x3ac6: done */
put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

/* flush_buffer..3b1e: findings:D1 */
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

/* swab12..3b58: findings:D2 */
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

/* warn_sym_redef 0x3b58..0x3c16: done */
warn_sym_redef(p, a)
struct symbol *p;
struct symbol *a;
{
	register char *s;
	char c;
	int n;
	int m;

	if (bE4BA)
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
