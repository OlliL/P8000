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
		tblF332[i].c = -1;
		tblF332[i].b = -1;
		tblF332[i].a = -1;
		tblF332[i].d = -1;
	}
	f6ffe(g04ee);
	scan_options(argc, argv);
	f0762(bE4EC);
	bE4A46 = 0;
	i = 1;
	bE4A36 = argv + 1;
	for (;;) {
		while (i < argc) {
			bE4C0 = 0;
			s = *bE4A36++;
			if (s[0] != '-') {
				f1364(s);
				goto next;
			}
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
				f1364(s + (j - 1));
				break;
			case 'b':
			case 't':
				if (bE49C)
					report_error(2, "Cannot use -t or -b options with -M");
				bE4A8 = 1;
				i++;
				if (i >= argc)
					report_error(2, "-%c: arg missing", s[j]);
				if (f1070(s + j, *bE4A36))
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
next:
			i++;
		}
		if (!bE4A46)
			break;
		i = savei + 2;
		savep++;
		bE4A36 = savep;
		argv = savev;
		argc = savec;
		bE4A46 = 0;
	}
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
	if (**argv != '-')
		goto next;
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

	fd = open(name, 0);
	if (fd < 0)
		report_error(2, "Unable to open command file");
	p = buf6988;
	tab4a48[0] = buf6988;
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

/* f1070..11ea: findings:D6,D15 */
int f1070(s, t)
char *s;
char *t;
{
	register int k;
	register int g;
	register char *u;
	char *fmt;

	bE42B2++;
	switch (*(s + 1)) {
	case 0:
		k = 0;
		break;
	case 'b':
		k = 3;
		break;
	case 'd':
		k = 2;
		break;
	case 't':
		k = 1;
		break;
	default:
		report_error(2, "-%.2s: invalid selector (must be t, d or b)", s);
		break;
	}
	u = t;
	g = 0;
	if (bE49A && (k == 2 || k == 3))
		g = 1;
	if (tab42b4[g * 5 + k])
		report_error(2, "-%.2s: -%c%c already specified", s, tab42b4[g * 5 + k], s[1]);
	tab42b4[g * 5 + k] = *s;
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
	int saved_k;
	char **saved_av;
	char **base;
	int saved_argc;
	int flag;
	char *p;

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
	if (*a != '-') {
		f23fe(a);
		goto next;
	}
	j = 1;
scan:
	if (!a[j])
		goto next;
	switch (a[j]) {
	case 'A':
	case 'e':
	case 'o':
	case 'u':
	case 'v':
		goto consume;
	case 'b':
	case 't':
		if (!a[j + 1])
			goto consume;
		j++;
		goto consume;
	case 'N':
		j += 2;
		goto scan;
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
	case 'f':
		saved_k = k;
		k = -1;
		saved_av = av;
		base = argv;
		saved_argc = argc;
		argc = bE4A44;
		av = g4a40;
		flag = 1;
		goto next;
	case 'l':
		j--;
		a[j] = '-';
		f23fe(a + j);
		goto next;
	case 'R':
		goto next;
	default:
		j++;
		goto scan;
	}
consume:
	k++;
	av++;
	j++;
	goto scan;
next:
	k++;
test:
	if (k < argc)
		goto body;
	if (flag) {
		k = saved_k + 2;
		av = saved_av + 1;
		argv = base;
		argc = saved_argc;
		flag = 0;
		goto test;
	}
	finish_output();
}

/* f1364..14a4: findings:D7,D15 */
int f1364(p)
char *p;
{
	register char *r;
	long n;

	r = p;
	switch (open_library(p)) {
	case 0:
		read_symbols(0, 0L);
		goto end;
	case 1:
		n = 1;
		if (!load_header(n))
			goto end;
		goto step;
	case 2:
		bE3E88 = bE44E / 12;
		bE3E84 = ensure_buf(bE44E);
		lseek(bE4BE, 28L, 0);
		read(bE4BE, bE3E84, bE3E88 * 12);
		f6f46(bE3E84, bE3E88);
		while (f1560())
			;
		*g0486++ = -1;
		goto end;
	case 3:
		report_error(0, "out of date (warning)");
		n = ((bE44E + 27) >> 1) + 1;
		goto check;
	}
step:
	n = ((bE44E + 27) >> 1) + n;
check:
	if (load_header(n))
		goto step;
end:
	close(bE4BE);
}

/* load_header..1560: done */
load_header(p)
long p;
{
	int magic;

	refill_wbuf(bE414, p, 0x1c);
	if (bE41E <= 0) {
		*g0486++ = -1;
		return 0;
	}
	f2e76(bE438, 0x1a);
	f2e76(&magic, 2);
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
	long *sv;
	char *end;
	int i;
	long t;

	sv = g0486;
	end = bE3E84;
	i = 0;
	goto test;
body:
	if (bE3E84 + i * 12 < end)
		goto next;
	if (*(bE3E84 + i * 12 + 4) & 0x80)
		end = bE3E84 + i * 12 + 4 + (*(bE3E84 + i * 12 + 4) & 0x7f);
	t = *(long *)f6b84(bE3E84 + i * 12 + 4);
	if (!t)
		goto next;
	if (*(char *)(t + 4) != 0x20)
		goto next;
	if (bE4B6)
		swap_pair(bE3E84 + i * 12);
	load_header((*(long *)(bE3E84 + i * 12)) >> 1);
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
	register char *cur;
	register char *blk;
	int k;
	int n;
	int cnt;
	int saved;
	char type;
	char d;
	long u1;
	char **p2;
	long u2;
	char **slot;

	f3516(c, 1);
	cnt = 0;
	k = 1;
	saved = bE4FA;
	if ((bE468 & 1) == 1) {
		report_error(1, "No relocation bits");
		return 0;
	}
	c = (long)(unsigned)(((bE436 + bE434) & 0xfffe) >> 1) + bE454 + bE458 + c;
	refill_wbuf(bE414, c, bE460);
	while (bE41E > 0) {
		n = read_entry_hdr();
		type = bE46E;
		if (bE4A6) {
			d = type & 0x1f;
			if (d == 1 || d > 4)
				continue;
		}
		if (!((type & 0x20) != 0)) {
			if (bE4AC && bE472[0] == 0x4c)
				continue;
			k += n + 1;
			continue;
		}
		slot = (char **)lookup_name();
		if (f6bae(slot)) {
			cur = bE4FC;
			if (cur[4] == ' ')
				continue;
		} else {
			cur = bE4FC;
			if (cur[4] != ' ') {
				if (a)
					f3b58(cur, &bE46A);
				continue;
			}
			if (bE46E == ' ') {
				if (bE46A <= *(long *)cur)
					continue;
				goto lstore;
			}
			if (*(long *)cur != 0 && bE46E == '"')
				continue;
			cnt++;
		}
		*(int *)(cur + 6) = bE3C02 + bE470;
		bE4D0 = *(int *)((blk = get_block(*(int *)(cur + 6))) + 0x10);
		bE4D2 = *(int *)(blk + 0x12);
		bE4D4 = *(int *)(blk + 0x14);
		*(int *)cur = 0;
		f6da8();
		cur[4] = bE46E;
lstore:
		*(long *)cur = bE46A;
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
register struct seg *p;
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
	a = p->f4;
	b = p->f6;
	c = p->f8;
	tab428a[1].kind = bE49A ? 4 : 0;
	tab428a[2].kind = bE49A ? 0 : 1;
	tab428a[3].kind = b ? 2 : 1;
	tab428a[1].f4 = a;
	tab428a[2].f4 = b;
	tab428a[3].f4 = c;
	for (i = 0; i < 4; i++) {
		tab428a[i].f6 = 0;
		tab428a[i].f2 = 0;
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
	tab428a[0].f4 = a + b + c;
	f4a34 = 1;
	if (c)
		f4a34 = 3;
	else if (b)
		f4a34 = 2;
	j = bE42B4[k][0] ? 0 : 1;
	for (; j < 4; j++) {
		q = &tab428a[j];
		r8 = tab428a[q->kind].f6;
		switch (bE42B4[k][j]) {
		case 't':
			if (q->f4 > tab4534[k][j])
				report_error(1, "top address of %s in segment %d too low", ptrs04ac[j], k);
			if ((r8 = tab4534[k][j] & 0xff00) != tab4534[k][j])
				printf("%s: warning, top address of %s in segment %d relocated to 0x%x\n", pE4D6, ptrs04ac[j], k, r8);
			r8 = (r8 - q->f4) & 0xff00;
			q->f4 = (q->f4 + 255) & 0xff00;
			break;
		case 'b':
			r8 = tab4534[k][j] & 0xff00;
			if (r8 != tab4534[k][j])
				printf("%s: warning, bottom address of %s in segment %d relocated to 0x%x\n", pE4D6, ptrs04ac[j], k, r8);
			break;
		}
		tab428a[0].f4 = 0;
		q->f6 = r8 + q->f4;
		if (r8 > q->f6)
			report_error(1, "%s area wrap around", ptrs04ac[j]);
		for (i = 1; i < j; i++) {
			if (r8 < tab428a[i].f6 && q->f6 > tab428a[i].f2) {
				printf("sld: warning, %s and %s overlap in segment %d - creating overlay segment\n", ptrs04ac[i], ptrs04ac[j], k);
				f1f96(p, j);
				if (j == 2) {
					tab428a[2].f6 = 0;
					tab428a[3].kind = 1;
					bF320++;
				} else {
					tab428a[3].f6 = 0;
					bF322++;
				}
				bE4A0++;
			}
		}
		q->f2 = r8;
	}
	p->f1a = tab428a[1].f2;
	(bF320 ? bF324 : p)->f1c = tab428a[2].f2;
	(bF322 ? bF328 : p)->f1e = tab428a[3].f2;
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
		p->flags |= 0x10;
		p->c[1] = high_byte(p->f1a);
	}
	if (bE42B4[k][2]) {
		if (bF320) {
			bF324->flags |= 0x20;
			bF324->c[2] = high_byte(bF324->f1c);
		} else {
			p->flags |= 0x20;
			p->c[2] = high_byte(p->f1c);
		}
	}
	if (bF322) {
		bF328->flags |= 0x40;
		if (!bE42B4[k][3])
			bF328->f1e = (p->f6 + p->f1c + 255) & 0xff00;
		bF328->c[3] = high_byte(bF328->f1e);
	} else if (bE42B4[k][3]) {
		p->flags |= 0x40;
		p->c[3] = high_byte(p->f1e);
	}
	if (!bE42B4[k][0]) {
		if (bF320 || bF322)
			f4a34 = 0;
		else {
			f4a34 = 1;
			if (tab428a[1].f6 < tab428a[2].f6)
				f4a34 = 2;
			else if (tab428a[1].f6 < tab428a[3].f6)
				f4a34 = 3;
			switch (f4a34) {
			case 2:
				if (tab428a[2].f6 < tab428a[3].f6)
					f4a34 = 3;
				break;
			case 3:
				if (tab428a[2].f6 > tab428a[3].f6)
					f4a34 = 2;
				break;
			}
		}
	}
}

/* f1f96..2056: findings:D4 */
f1f96(p, kind)
char *p;
long kind;
{
	register char *e;

	if (bE3C04 > 0x100)
		report_error(2, "Pass2 segment table overflow");
	if ((int)kind == 2) {
		e = table32_entry(bE3C04++);
		bF324 = e;
		*e = 0xff;
		*(int *)(e + 6) = *(int *)(p + 6);
		*(int *)(e + 10) |= 2;
		*(int *)(p + 6) = 0;
		*(int *)(p + 10) &= ~2;
		*(char *)(p + 2) = 0;
	} else {
		e = table32_entry(bE3C04++);
		bF328 = e;
		*e = 0xff;
		*(int *)(e + 8) = *(int *)(p + 8);
		*(int *)(e + 10) |= 4;
		*(int *)(p + 8) = 0;
		*(int *)(p + 10) &= ~4;
		*(char *)(p + 3) = 0;
	}
}

/* start_output..22a4: findings:D2 */
int start_output()
{
	register int n;
	register char *p;
	long u1;
	int u2;
	char arr[8192];
	char hdr[24];

	open_tmp(bE4DA, 0);
	mktemp(g049e);
	open_tmp(bE6E2, 1);
	open_tmp(bE8EA, 1);
	if (!bE4A4 || !bE4AA)
		open_tmp(bF10A, 1);
	if (bE4A2) {
		open_tmp(bEF02, 1);
		open_tmp(bEAF2, 1);
		open_tmp(bECFA, 1);
	}
	bE452 = bE4B4 == 1 ? (bE4A0 ? 0xe605 : bE49A ? 0xe611 : 0xe607) : (bE4A0 ? 0xe705 : bE49A ? 0xe711 : 0xe707);
	bE454 = bE3C16;
	bE458 = bE3C1A;
	bE45C = b3C1E;
	bE460 = bE4A4 ? 0 : (((unsigned)bE4CE >> 4) + bE4FA) * 12;
	if (bE492) {
		if (((char *)bE492)[4] != '"')
			report_error(1, "Entry point not in text");
		else
			bE462 = *(long *)bE492;
	} else {
		for (p = (char *)bF32E; p; p = *(char **)(p + 0x12)) {
			if (*(int *)(p + 4)) {
				bE462 = (long)(unsigned)*(int *)(p + 0x1a) + ((long)p[0] << 24);
				break;
			}
		}
	}
	bE466 = 0;
	bE467 = 0;
	bE468 = bE4A2 ? 0 : 1;
	n = 0;
	for (p = (char *)bF32E; p; p = *(char **)(p + 0x12)) {
		*(struct e16 *)(arr + (n << 4)) = *(struct e16 *)p;
		if (bE4B6)
			swap_quad(arr + (n << 4));
		n++;
	}
	init_entry(hdr, n);
	if (bE4B6)
		swab(hdr, hdr, 0x18);
	f2f04(bE4DA, hdr, 0x18);
	f2f04(bE4DA, arr, n << 4);
}

/* open_tmp..2346: findings:D7 */
open_tmp(out, flag)
char *out;
int flag;
{
	register int fd;
	char *name;

	if (flag)
		name = g049e;
	else
		name = p0494;
	fd = creat(name, 0x1b6);
	if (fd < 0) {
		if (flag)
			report_error(2, "cannot create temp");
		else
			report_error(2, "cannot create output");
	}
	close(fd);
	*(int *)out = open(name, 2);
	if (flag)
		unlink(g049e);
	*(int *)(out + 2) = 0x100;
	*(char **)(out + 4) = out + 8;
}

/* init_entry..23fe: done */
init_entry(p, a)
char *p;
int a;
{
	*(int *)p = bE452;
	*(long *)(p + 2) = bE3C16 + bE3C1A;
	if (*(long *)(p + 2) > 0x10000 && bE4A8)
		report_error(2, "cannot have segment overflow with t or b option");
	*(long *)(p + 6) = b3C1E;
	*(int *)(p + 10) = a << 4;
	*(int *)(p + 18) = bE468;
	if (bE4AE)
		*(int *)(p + 18) |= 0x20;
	*(int *)(p + 12) = 0;
	*(long *)(p + 14) = bE462;
	*(int *)(p + 22) = 0;
	*(int *)(p + 20) = 0;
}

/* f23fe..2492: findings:D2 */
f23fe(p)
char *p;
{
	register char *q;
	register long *lp;

	q = p;
	if (open_library(p) == 0) {
		while (*q != 0)
			q++;
		for (;;) {
			if (q < p)
				break;
			q--;
			if (*q == 0x2f)
				break;
		}
		q++;
		f2492(0L, q);
	} else {
		lp = g0486;
		while (*lp != -1) {
			refill_wbuf(bE414, *lp, 26);
			f2e76(bE438, 26);
			f2492(*lp + 13, bE438);
			lp++;
		}
		g0486 = lp;
	}
	close(bE4BE);
}

/* f2492..2624: findings:D2,D7 */
int f2492(a, b)
long a;
char *b;
{
	register int k;
	register char *rec;
	register char *t;
	long v;
	int n;
	char c;
	char d;

	f3516(a, 2);
	f6a08(b);
	t = tab84d8;
	v = (unsigned)((bE436 + bE434) & ~1) >> 1;
	v = v + a;
	refill_wbuf(bE414, v + bE454 + bE458, bE460);
	k = 0;
	while (bE41E > 0) {
		n = read_entry_hdr() + 1;
		f6c6a();
		c = bE46E;
		if (bE4A6) {
			d = c & 0x1f;
			if (d == 1 || d > 4)
				goto next;
		}
		if ((c & 0x20) == 0) {
			if (bE4A4 || bE4AA)
				goto next;
			if (bE4AC && bE472[0] == 0x4c)
				goto next;
			f6e1a(bF10A, &bE46A);
			goto next;
		}
		rec = *(char **)lookup_name();
		if (!rec)
			report_error(2, "internal error: symbol not found");
		if (bE46E != 0x20)
			goto other;
		if (t >= (char *)&gE47E)
			report_error(2, "Local symbol overflow");
		*(int *)t = k;
		*(char **)(t + 2) = rec;
		t += 6;
		goto next;
other:
		if (*(char *)(rec + 4) == bE46E && bE46A == *(long *)rec)
			goto next;
		if (put_name(bE472) < 8)
			putchar(9);
		putchar(9);
		report_error(1, "- Multiply defined");
next:
		k += n;
	}
	f2624(v, t);
}

/* f2624..27d8: findings:D7,D16 */
int f2624(a, b)
long a;
char *b;
{
	int i;
	long acc;
	long base;
	long half;
	int lim;
	char tmp[16];
	int key;
	int n;
	acc = a;
	base = a - (bE434 / 2);
	lim = bE434 / (n * 2);
	half = (bE454 + bE458) >> 1;
	for (i = 0; i < lim; i++) {
		refill_wbuf(bE414, base + i, n * 2);
		f2e76(tmp, n * 2);
		if (bE4B6)
			swap_quad(tmp);
		key = *(int *)(tmp + 10) & 3;
		switch (key) {
		case 1:
			refill_wbuf(bE414, acc, *(int *)(tmp + 4));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 4));
			acc += f3090(*(int *)(tmp + 4));
			f27d8(b, bE6E2, bEF02);
			break;
		case 3:
			refill_wbuf(bE414, acc, *(int *)(tmp + 4));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 4));
			acc += f3090(*(int *)(tmp + 4));
			f27d8(b, bE6E2, bEF02);
		case 2:
			refill_wbuf(bE414, acc, *(int *)(tmp + 6));
			refill_wbuf(bE424, acc + half, *(int *)(tmp + 6));
			acc += f3090(*(int *)(tmp + 6));
			f27d8(b, bE8EA, bEAF2);
			break;
		}
	}
}

/* f27d8..2cbc: findings:D2,D7,D15 */
int f27d8(p, q, r)
char *p;
char *q;
char *r;
{
	int flag;
	char *save;
	register int w13;
	register int w12;
	register int w11;
	register char *e;
	register int w10;

	flag = 0;

top:
	bE41E -= 1;
	if (bE41E > 0)
		goto c2826;
	if (bE41E >= 0)
		goto c2814;
	goto end;
c280e:
	*(int *)(bE414 + 4) += 1;
	goto c2814;
c2814:
	bE41E += 1;
	w12 = getw_buf(bE414);
	goto c2844;
c2826:
	*(int *)(bE414 + 4) -= 1;
	if (*(int *)(bE414 + 4) < 0)
		goto c280e;
	w12 = *(int *)*(char **)(bE414 + 6);
	*(int *)(bE414 + 8) += 2;
c2844:
	*(int *)(bE424 + 10) -= 1;
	if (*(int *)(bE424 + 10) > 0)
		goto c2882;
	if (*(int *)(bE424 + 10) >= 0)
		goto c2870;
	report_error(2, "Relocation error");
	goto c2870;
c286a:
	*(int *)(bE424 + 4) += 1;
c2870:
	*(int *)(bE424 + 10) += 1;
	w13 = getw_buf(bE424);
	goto c28a0;
c2882:
	*(int *)(bE424 + 4) -= 1;
	if (*(int *)(bE424 + 4) < 0)
		goto c286a;
	w13 = *(int *)*(char **)(bE424 + 6);
	*(int *)(bE424 + 8) += 2;
	goto c28a0;
c28a0:
	if (bE4B6) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	if (!(w13 & 8)) {
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 2:
	if (!(w13 & 1))
		report_error(2, "bad segmented bit in relocation word");
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)(w12 & 0xff);
	bE470 = (unsigned)w13 >> 8;
	f6c6a();
	w12 = (int)bE46A;
	if (w12 & 0xff00)
		report_error(2, "short segmented offset too large");
c2954:
	w12 += ((int)(bE46A >> 16)) & 0xff00;
c2964:
	w13 = ((bE470 & 0xff) << 8) + (w13 & 0xff);
emit:
	if (bE4B6) {
		w12 = swap_bytes(w12);
		w13 = swap_bytes(w13);
	}
	f3a88(w12, q);
	if (bE4A2)
		f3a88(w13, r);
	goto top;
		break;
	case 1:
	if (!(w13 & 1))
		report_error(2, "bad segmented bit in relocation word");
	w10 = w12;
	bE470 = (unsigned)w13 >> 8;
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)w12;
	f6c6a();
	w10 = (w10 & 0x8000) | (int)(bE46A >> 16);
	if (bE4B6)
		w10 = swap_bytes(w10);
	f3a88(w10, q);
	w11 = w13;
	flag = 1;
	goto top;
	case 0:
	if (!(w13 & 0xe))
		goto c2a96;
	if (!flag)
		goto c2a68;
	if (((w13 != 0xff0f) & w11 & 0xff0f) == 0)
		goto c2a68;
	w13 &= 0xf0;
	w13 |= w11 & 0xff0f;
c2a68:
	bE470 = (unsigned)w13 >> 8;
	bE46E = ((w13 & 7) >> 1) + 1;
	bE46A = (long)w12;
	f6c6a();
c2a96:
	if (!flag)
		goto c2ad6;
	flag = 0;
	w11 = ((bE470 & 0xff) << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = swap_bytes(w11);
	if (bE4A2)
		f3a88(w11, r);
c2ad6:
	if (!(w13 & 0xe))
		goto emit;
	w12 = (int)bE46A;
	goto c2964;
	default:
		report_error(2, "unrecognized relocation address type");
	}
	goto emit;
	} else {
		e = table_lookup(p, w13);
		if (e[4] != 0x20)
			goto c2b46;
		w13 = (w13 & 7) + ((index16(e) + bEF312) << 4) + 8;
		goto emit;
	}

c2b46:
	switch (((w13 & 8) ? w13 : (w13 >> 4)) & 7) {
	case 2:
	w13 = (e[4] - 0x21) * 2 + 0x21;
	w13 = ((*(int *)(e + 6)) << 8) + (w13 & 0xff);
	w12 = (w12 + 0xff) & (w12 & 0xffff);
	w12 = (w12 & 0xffff);
	if (w12 & 0xff00)
		report_error(2, "short segment offset too large");
	w12 += ((int)(*(long *)e >> 16)) & 0xff00;
	goto emit;
		break;
	case 1:
	if (!(w13 & 1))
		report_error(2, "bad segmented bit in relocation word");
	w10 = (w12 & 0x8000) | ((int)(*(long *)e >> 16) & 0xff00);
	if (bE4B6)
		w10 = swap_bytes(w10);
	f3a88(w10, q);
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
	w11 = (e[4] - 0x21) * 2 + 0x11;
	w11 = ((*(int *)(e + 6)) << 8) + (w11 & 0xff);
	if (bE4B6)
		w11 = swap_bytes(w11);
	if (bE4A2)
		f3a88(w11, r);
c2c72:
	w13 = (e[4] - 0x21) * 2 + 1;
	w13 = ((*(int *)(e + 6)) << 8) + (w13 & 0xff);
	w12 = w12 + (int)*(long *)e;
	goto emit;
	default:
		report_error(2, "unrecognized relocation address type");
	}
	goto emit;
end:
	;
}

/* finish_output 0x2cbc..0x2d4c: done */
finish_output()
{
	f6134();
	if (bE4A2)
		f2dde(bECFA);
	if (!bE4A4) {
		if (!bE4AA)
			f2dde(bF10A);
		f6efe();
	}
	flush_buffer(bE4DA);
	close(*(int *)bE4DA);
	if (!bE4BC) {
		unlink(bE048A);
		link("l.out", bE048A);
		p0494 = bE048A;
	}
	g04e8 = bE4A3A;
	f2d4c();
	sig_handler();
}

/* f2d4c..2dde: findings:D2 */
f2d4c()
{
	char buf[12];
	char rest[12];
	int n;

	n = open(p0494, 1);
	if (n == -1)
		report_error(2, "Unable to reopen output file");
	if (lseek(n, buf, 0) == -1)
		report_error(2, "Unable to seek in output file");
	bE4F0 = bE4F0 * 14;
	if (write(n, &bE4F0, 2) != 2)
		report_error(2, "Unable to write in output file");
}

/* f2dde..2e4c: done */
f2dde(p)
char *p;
{
	register int n;
	register int c;
	register int *q;

	flush_buffer(p);
	n = *(int *)p;
	lseek(n, 0L, 0);
	while ((c = read(n, bE6EA, 0x200)) > 1) {
		c >>= 1;
		q = (int *)bE6EA;
		do {
			f3a88(*q++, bE4DA);
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

/* f2e76..2f3a: findings:D2 */
f2e76(p, n)
register char *p;
int n;
{
	register int k;
	register char *q;

	k = n >> 1;
	bE418 -= k;
	if (bE418 >= 0) {
		bE41E -= k;
		if (bE41E > 0) {
			q = bE41A;
			do {
				*(int *)p = *(int *)q;
				q += 2;
				p += 2;
			} while (--k);
			bE41A = q;
			return;
		}
		bE41E += k;
	}
	bE418 += k;
	do {
		*(int *)p = getw_buf(bE414);
		p += 2;
	} while (--k);
}

/* f2f04..2f3a: findings:D2 */
f2f04(p, q, n)
register int *p;
char *q;
int n;
{
	register int k;

	k = n >> 1;
	do {
		f3a88(*p++, q);
	} while (--k);
}

/* refill_wbuf..3090: findings:D5,D9,D15 */
int refill_wbuf(q, v, n)
char *q;
long v;
int n;
{
	long hi;
	long lo;
	register char *cur;
	register char *obj;
	int nw;

	obj = q;
	hi = (v >> 8) & 0xff;
	lo = v & 0xff;
	*(int *)(*(char **)(obj + 12)) -= 1;
	cur = b0004;
	if ((long)*(int *)(cur + 2) == hi)
		goto common;
	cur = b020a;
	if ((long)*(int *)(cur + 2) == lo)
		goto common;
	if (*(int *)cur) {
		cur = b0004;
		if (*(int *)cur) {
			report_error(2, "No pages");
			goto common;
		}
	} else if (!*(int *)b0004 && *(int *)(b0004 + 2) < *(int *)(b020a + 2))
		cur = b0004;
	*(int *)(cur + 2) = hi;
	lseek(bE4BE, (v & 0xff00) << 1, 0);
	nw = read(bE4BE, cur + 6, 0x200) >> 1;
	if (nw < 0)
		nw = 0;
	*(int *)(cur + 4) = nw;
common:
	*(int *)cur += 1;
	*(int *)(obj + 2) = hi;
	*(char **)(obj + 12) = cur;
	*(char **)(obj + 6) = cur + 6 + (lo << 1);
	if (n != -1)
		*(int *)(obj + 10) = f3090(n);
	*(int *)(obj + 4) = *(int *)(cur + 4) - (int)lo;
	if (*(int *)(obj + 4) <= 0)
		*(int *)(obj + 10) = 0;
}

/* f3090 0x3090..0x30a4: done */
f3090(x)
int x;
{
	return (x >> 1) & 0x7fff;
}

/* getw_buf..3144: findings:D2 */
getw_buf(p)
char *p;
{
	register char *q;
	long v;
	int w;

	q = p;
	*(int *)(q + 4) -= 1;
	if (*(int *)(q + 4) < 0) {
		v = (long)(*(int *)(q + 2) + 1) << 8;
		refill_wbuf(q, v, -1);
		*(int *)(q + 4) -= 1;
	}
	*(int *)(q + 10) -= 1;
	if (*(int *)(q + 10) <= 0) {
		if (*(int *)(q + 10) < 0)
			report_error(2, g04d8);
		bE410++;
		*(int *)(q + 12) -= 1;
		*(char **)(q + 12) = (char *)&bE410;
	}
	w = *(int *)(*(char **)(q + 6));
	*(int *)(q + 8) += 2;
	return w;
}

/* open_library..3408: done */
int open_library(name)
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
	bE438[0] = 0;
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
		strcpy(buf90b6, "");
		strcat(buf90b6, buf9098);
		bE4BE = open(buf90b6, 0);
		if (bE4BE == -1) {
			strcpy(buf90b6, "");
			strcat(buf90b6, "/usr");
			strcat(buf90b6, buf9098);
			bE4BE = open(buf90b6, 0);
			if (bE4BE == -1) {
				strcpy(buf90b6, "");
				strcat(buf90b6, "/z/bin");
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
	bE20C = -1;
	bE006 = -1;
	bE20A = 0;
	bE004 = 0;
	bE430 = (char *)&bE410;
	bE420 = (char *)&bE410;
	bE410 = 2;
	refill_wbuf(bE414, 0L, 2);
	if (bE41E <= 0)
		report_error(2, g04d8);
	if (getw_buf(bE414) != -155)
		return 0;
	refill_wbuf(bE414, 1L, 0x1a);
	if (bE41E <= 0)
		return 1;
	f2e76(bE438, 0x1a);
	if (strncmp(bE438, g04ce, 14))
		return 1;
	fstat(bE4BE, &st);
	if (st.mtime > bE446)
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
	if (bE438[0])
		printf("(%.14s)", bE438);
	printf(": ");
	printf(msg, a, b, c, d, e);
	printf("\n");
	if (level > 1)
		sig_handler();
	bE4A3A = level;
}

/* table_lookup 0x34b8..0x3516: findings:D2 */
char *table_lookup(a, end)
long a;
char *end;
{
	register int k;
	register char *p;

	k = ((int)a >> 4) & 0x0fff;
	p = tab84d8;
	while (p < end) {
		if (k == *(int *)p)
			return *(char **)(p + 2);
		p += 6;
	}
	printf("botch on %x\n", (int)a);
	report_error(2, "Local symbol botch");
}

/* f3516..3760: findings:D2,D16 */
int f3516(a, b)
long a;
int b;
{
	int magic;
	char tmp[24];
	char rec[8208];
	register char *p;
	register int n;
	register int w;

	refill_wbuf(bE414, a, 2);
	f2e76(&magic, 2);
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
	refill_wbuf(bE414, a, bE436);
	f2e76(tmp, bE436);
	if (bE4B6)
		swab(tmp, tmp, bE436);
	bE434 = *(int *)(tmp + 10);
	n = bE434 / 16;
	refill_wbuf(bE414, a + ((bE436 & ~1) >> 1), bE434);
	f2e76(rec, bE434);
	if (bE4B6) {
		for (p = rec; p < rec + n * 16; p += 16)
			swap_quad(p);
	}
	bE452 = magic & 0xf7ff;
	bE454 = 0;
	bE458 = 0;
	bE45C = *(long *)(tmp + 6);
	bE466 = 0;
	bE467 = 0;
	w = 0;
	for (p = rec; p < rec + n * 16; p += 16) {
		*(int *)(p + 4) = (*(int *)(p + 4) + 1) & 0xfffe;
		*(int *)(p + 6) = (*(int *)(p + 6) + 1) & 0xfffe;
		*(int *)(p + 8) = (*(int *)(p + 8) + 1) & 0xfffe;
		bE454 += (unsigned)*(int *)(p + 4);
		bE458 += (unsigned)*(int *)(p + 6);
		w |= *(int *)(p + 10);
	}
	if (w & 0xf8)
		report_error(2, "Invalid segment attributes");
	bE462 = *(long *)(tmp + 14);
	bE468 = *(int *)(tmp + 18) & 1;
	bE460 = *(int *)(tmp + 12);
	make_blk(rec, n, b);
	return 1;
}

/* make_blk..3a30: done */
int make_blk(src, n, kind)
register struct e16 *src;
register int n;
register int kind;
{
	register int j;
	int idx;
	int loc[3];
	char key[16];
	char *blk;

	bE3C02 = bEF32C;
	blk = get_block(bEF32C);
	if (kind == 1) {
		copy_leafname(bE4C0, key);
		j = bE478;
		idx = 0;
		while (idx < g047a) {
			if (str_compare(tab3c22[j].str, key))
				break;
			j = tab3c22[j].nxt;
			idx++;
		}
		loc[0] = bE3E7E;
		loc[1] = bE3E7C;
		loc[2] = bE3E80;
		if (idx != g047a) {
			if (tab3c22[j].a > -1)
				loc[0] = tab3c22[j].a;
			if (tab3c22[j].b > -1)
				loc[1] = tab3c22[j].b;
			if (tab3c22[j].c > -1)
				loc[2] = tab3c22[j].c;
		}
	}
	while (n--) {
		if (bEF32C >= bE4EC)
			report_error(2, "pass one segment table overflow --Try -Ap option");
		if (kind == 1) {
			*(struct e16 *)blk = *src++;
			if (bE452 != 0xe707 && bE452 != 0xe607)
				report_error(0, "warning: nonsegmented object");
			if (bE452 == 0xe707 || bE452 == 0xe607) {
				*(int *)(blk + 0x12) = -*(int *)(blk + 4);
				*(int *)(blk + 0x14) = *(int *)(blk + 0x12) - *(int *)(blk + 6);
			} else {
				*(int *)(blk + 0x12) = 0;
				*(int *)(blk + 0x14) = 0;
			}
			*(int *)(blk + 0x10) = 0;
			*(int *)(blk + 0x22) = loc[0];
			*(int *)(blk + 0x24) = loc[1];
			*(int *)(blk + 0x26) = loc[2];
			*(int *)(blk + 0x30) = -1;
			*(int *)(blk + 0x32) = -1;
			*(int *)(blk + 0x34) = -1;
		} else {
			*(long *)(blk + 0x28) = bE47C;
			bE47C += (unsigned)*(int *)(blk + 4);
			*(long *)(blk + 0x2c) = bE480;
			bE480 += (unsigned)*(int *)(blk + 6);
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
		if (*q == *p) {
			p++;
			q++;
		} else {
			if (g0476)
				return *q - *p;
			return 0;
		}
	} while (*q);
	if (!g0476)
		return 1;
	return 0;
}

/* f3a88 0x3a88..0x3ac6: findings:D5 */
f3a88(v, a)
int v;
char *a;
{
	register int *p;

	p = a;
	p[2] = v;
	p[3] += 2;
	if (--p[1] <= 0)
		flush_buffer(p);
}

/* flush_buffer..3b1e: findings:D1 */
flush_buffer(a)
char *a;
{
	register char *p;
	register int n;

	p = a;
	n = *(char **)(p + 4) - (p + 8);
	if (n > 0) {
		if (write(*(int *)p, p + 8, n) != n)
			report_error(2, "output error");
	}
	*(char **)(p + 4) = p + 8;
	*(int *)(p + 2) = 0x100;
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

/* f3b58..3c16: findings:D7 */
f3b58(p, a)
char *p;
char *a;
{
	register int k;
	int n;

	if (bE4BA)
		return;
	if (p != 0 && p[4] == 0x20)
		return;
	if (a[4] == 0x20)
		return;
	if (a[8] < 0)
		k++;
	if (a[8] == 0x5f) {
		k++;
		if (a[8] == 0x5f)
			return;
	}
	n = put_name(a + 8);
	if (n < 8)
		putchar(9);
	putchar(0x20);
	report_error(0, "- warning: symbol redefined in archive");
}
