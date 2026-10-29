#include <stdio.h>
#include "ld.h"

static char sccsid[] = "@[$]ld.c\t1.18  06/27/83 16:44:24 - 87wega3.2";

struct page page[2];
struct pghdr fpage;
int infil;

int ntab;
struct tent tab[1];
int symindex;
long *libp;
int ttotal, dtotal, btotal, v66ce;
int fctrel, fcdrel, fcbrel;

char *ofilnam = "l.out";
int delarg, v0080;
char tmpname[] = "/tmp/ldaXXXXX";
int v669a, v66a4;

struct stream text;
struct filhdr filhdr;
char x702f, x7030, x7031;
int x66b8, x66ba, x6bfe;
int v6bfc;
int Sflag, Xflag;
int v6bf6;
long liblist[64];
long *libp;
struct symbol *p_etext, *p_edata, *p_end;
int dflag, oflag, comsize;
struct areas areas[4];
char tab42b4[5];
char *secnam[4];
int secorg[4];
int iflag;
int v66b0;
int x66b6;
int hdrsz, segtsz;
struct stream reloc;
char libname[] = "/lib/libxxxxxxxxxxx";
char libpath[20];
int v66b4;

struct buf toutb;
struct buf soutb, tmpoutb, routb, doutb;
int rflag, sflag, xflag, ofilfnd;
int swapflg;
char secspec;

struct rlent *rltab;
int rlcnt;
char *aoutnam = "a.out";
int v6bf8, v6bfa;
int oflag, iflag, v66c4, v66c8;
struct symbol *entrypt;

struct archdr archdr;
char *pname;
char *filname;
int exstat;

sig_handler()
{
	int mode;

	if (v669a)
		limit_msg();
	unlink("l.out");
	if (!delarg) {
		if (!v66a4 || !v0080) {
			mode = ~umask(0) & 0x1ff;
			chmod(ofilnam, mode);
		}
	}
	if (delarg)
		unlink(ofilnam);
	_exit(delarg);
}

/* main: entry of ld: the signals, the program name, the option scan over the arguments, the final pass (0x009c..0x0370 of
   /bin/ld; exact in the whole-file compile of ld.c; the draft alone differs in the jump widths, see HANDOFF_ld.md) */
main(argc, argv)
int argc;
char **argv;
{
	int spare1, spare2, spare3, spare4, spare5;
	int found;
	int k;
	char c;
	struct symbol **sp;
	register int i;
	register int j;
	register char *s;
	register char **av;

	if (signal(2, 1) != 1)
		signal(2, sig_handler);
	if (signal(15, 1) != 1)
		signal(15, sig_handler);
	if (argc == 1)
		exit(4);
	s = rindex(*argv, '/');
	if (s == 0)
		s = *argv;
	else
		s++;
	pname = s;
	if (*s == 's') {
		swapflg++;
		aoutnam = "s.out";
	} else
		v66b4++;
	av = argv + 1;
	i = 1;
	goto test;
body:
	if (v669a)
		printf("%s:\n", *av);
	filname = 0;
	s = *av++;
	if (s[0] == '-') {
		for (j = 1; s[j] != 0; j++)
			switch (s[j]) {
			case 'o':
				i++;
				if (i >= argc)
					report_error(2, "Bad output file");
				ofilnam = *av++;
				ofilfnd++;
				break;
			case 'e':
			case 'u':
				i++;
				if (i >= argc)
					report_error(2, "Bad 'use' or 'entry'");
				sp = slookup(*av++);
				enter(sp);
				if (s[j] == 'e')
					entrypt = lastsym;
				break;
			case 'v':
				i++;
				if (i >= argc)
					report_error(2, "-v: arg missing");
				v66b0 = 1;
				k = ntab;
				found = 0;
				while (--k >= 0) {
					if (found)
						break;
					found = str_compare(tab[k].t3, *av);
				}
				if (found) {
					parse_args(i, argv);
					f057c(k);
				} else
					f050c(i, *av);
				av++;
				break;
			case 'l':
				j--;
				c = s[j];
				s[j] = '-';
				load_lib_syms(s + j);
				s[j] = c;
				goto next;
			case 'X':
				Xflag++;
				break;
			case 'S':
				Sflag++;
				break;
			case 'r':
				rflag++;
				v66a4++;
				break;
			case 's':
				sflag++;
			case 'x':
				xflag++;
				break;
			case 'd':
				dflag++;
				break;
			case 'i':
				iflag++;
				break;
			case 'O':
				oflag++;
				break;
			case 'T':
				v669a++;
				break;
			case 'b':
			case 't':
				i++;
				if (i >= argc)
					report_error(2, "-%c: arg missing", s[j]);
				if (reg_sect_addr(s + j, *av))
					j++;
				av++;
				break;
			case 'w':
				wflag++;
				break;
			case 'N':
				j++;
				switch (s[j]) {
				case 'l':
					nlflag = 1;
					break;
				case 's':
					nlflag = 0;
					break;
				default:
					report_error(2, "-N%c: bad argument (l or s)", s[j]);
				}
				break;
			default:
				report_error(2, "-%c: bad flag", s[j]);
				goto next;
			}
	} else
		load_lib_syms(s);
next:
	i++;
test:
	if (i < argc)
		goto body;
	if (rflag && secspec)
		fprintf(stderr, "%s: warning, -r option overrides -t, -b\n", pname);
	parse_args(argc, argv);
}

/* reg_sect_addr: read the address of a section option (-t, -d, -b) and check it (sld reg_sect_addr; finding: the frame
   offset of fmt differs; 0x0370..0x045e of /bin/ld) */
int reg_sect_addr(s, t)
char *s;
char *t;
{
	int spare1, spare2;

	register int k;
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
	if (tab42b4[k])
		report_error(2, "-%.2s: -%c%c already specified", s, tab42b4[k], s[1]);
	tab42b4[k] = *s;
	fmt = "%o%c";
	if (*u != '0')
		fmt = "%d%c";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "%x%c";
	}
	if (sscanf(u, fmt, &secorg[k]) != 1)
		report_error(2, "-%.2s %s: bad address", s, t);
	if (k != 0)
		return 1;
	return 0;
}

/* parse_args: the argument pass of ld: the output, then every object and library in order (0x045e..0x050c of /bin/ld) */
parse_args(argc, argv)
int argc;
char **argv;
{
	int spare1, spare2, spare3, spare4, spare5;
	register int k;
	register int j;
	register char *a;
	register char **av;

	filname = 0;
	end_pass1();
	start_output();
	av = argv + 1;
	libp = liblist;
	k = 1;
	while (k < argc) {
		a = *av++;
		if (v669a)
			printf("%s:\n", a);
		if (*a == '-') {
			for (j = 1; a[j] != 0; j++)
				switch (a[j]) {
				case 'b':
				case 't':
					if (a[j + 1])
						j++;
				case 'e':
				case 'o':
				case 'u':
				case 'v':
					k++;
					av++;
					break;
				case 'N':
					j++;
					break;
				case 'l':
					j--;
					a[j] = '-';
					load2arg(a + j);
					goto next;
				}
		} else
			load2arg(a);
next:
		k++;
	}
	finish_output();
}

f050c(x, y)
int x, y;
{
	int spare;

	register struct tent *e;

	e = &tab[ntab++];
	e->t0 = x;
	e->t1 = symindex;
	e->t2 = (int)libp;
	e->t3 = y;
	e->t4 = fctrel;
	e->t5 = fcdrel;
	e->t6 = fcbrel;
	e->t7 = ttotal;
	e->t8 = dtotal;
	e->t9 = btotal;
	e->t10 = v66ce;
}
/* f057c: restore the state of the object at entry i of tab (the bases, libp and ntab) and clear the symbol hash entries
   of its symbols (sld read_symbols tail for the hash; no sld function; 0x057c..0x0608 of /bin/ld) */

f057c(i)
int i;
{
	int spare1, spare2;
	register struct tent *e;
	register int saved;

	e = &tab[i];
	ntab = i + 1;
	libp = (long *)e->t2;
	fctrel = e->t4;
	fcdrel = e->t5;
	fcbrel = e->t6;
	ttotal = e->t7;
	dtotal = e->t8;
	btotal = e->t9;
	v66ce = e->t10;
	saved = e->t1;
	while (symindex > saved) {
		symindex--;
		*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
}

/* add16_checked: sum of two words, report_error(1) with the message a when the sum is 0x10000 or more (sld add16_checked;
   0x089a..0x08cc of /bin/ld) */

/* load_lib_syms: load the members of a library or the object named by p (sld load_lib_syms; 0x0608..0x0714 of /bin/ld) */
load_lib_syms(p)
char *p;
{
	int spare;
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
		f2bf6();
		rlcnt = archdr.asize / 12;
		rltab = (struct rlent *)ensure_buf(archdr.asize);
		lseek(infil, 28L, 0);
		read(infil, rltab, rlcnt * 12);
		chk_ranlib(rltab, rlcnt);
		while (ranlib_scan())
			;
		*libp++ = -1;
		f2bfc();
		break;
	case 3:
		report_error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(infil);
}

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
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe507 || magic == 0xe511) {
		if (read_symbols(1, p + 13))
			*libp++ = p;
	}
	return 1;
}

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
		longswap(&rltab[i]);
	load_header(rltab[i].roff >> 1);
next:
	i++;
test:
	if ((unsigned)i < (unsigned)rlcnt)
		goto body;
	return sv != libp;
}

add16_checked(x, y, a)
unsigned x, y;
char *a;
{
	long s;

	s = (long)x + (long)y;
	if (s >= 0x10000)
		report_error(1, a);
	return(s);
}

read_symbols(a, c)
int a;
long c;
{
	register struct symbol *cur;
	int cnt;
	int saved;
	int n;
	int k;

	readhdr(c);
	cnt = 0;
	k = 1;
	saved = symindex;
	if ((filhdr.relflg & 1) == 1) {
		report_error(1, "No relocation bits");
		return 0;
	}
	dseek(&text, c, filhdr.ssize);
	while (text.size > 0) {
		n = read_entry_hdr();
		cur = 0;
		if (cursym.stype == 0x20 || (cursym.stype & 0x20) == 0) {
			k += n + 1;
			continue;
		}
		enter(lookup_name());
		cnt++;
	}
	while (symindex > saved) {
		symindex--;
		*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
	return cnt != 0;
}

/* end_pass1: the symbols of the output before the layout: _etext, _edata, _end, the rflag decision, the common symbols and
   the undefined and external symbols (sld end_pass1; 0x0b6e..0x0d9c of /bin/ld). The newline of the undefined message
   is a char in the original (the cast (char) gives extsb); the calls to in-file functions are calr there. */
end_pass1()
{
	int spare1, spare2, spare3;
	register struct symbol *ep;
	register struct symbol *end;
	register int i;

	p_etext = *slookup("_etext");
	p_edata = *slookup("_edata");
	p_end = *slookup("_end");
	end = table16_entry(symindex);
	if (!rflag) {
		ep = table16_entry(0);
		while (ep < end) {
			if (ep->stype == ' ' && ep->svalue == 0 && ep != p_end && ep != p_edata && ep != p_etext) {
				rflag++;
				dflag = 0;
				break;
			}
			ep = next_entry(ep);
		}
	}
	if (rflag) {
		oflag = 0;
		sflag = 0;
		secspec = 0;
		for (i = 0; i < 4; i++)
			tab42b4[i] = 0;
	}
	comsize = 0;
	if (dflag == 0 && rflag)
		goto dolayout;
	ldrsym(p_etext, (unsigned long)(unsigned)ttotal, '"');
	ldrsym(p_edata, (unsigned long)(unsigned)dtotal, '#');
	ldrsym(p_end, (unsigned long)(unsigned)btotal, '$');
	ep = table16_entry(0);
	while (ep < end) {
		if (ep->stype == ' ') {
			i = ep->svalue;
			if (i) {
				if (!x66b8 && !x66ba)
					i = (i + 1) & ~1;
				ep->svalue = (unsigned)comsize;
				ep->stype = '%';
				comsize = add16_checked(comsize, i, "bss overflow");
			}
		}
		ep = next_entry(ep);
	}
dolayout:
	place_sec();
	v0080 = 0;
	ep = table16_entry(0);
	while (ep < end) {
		switch (ep->stype) {
		case ' ':
			if (v66a4 == 0) {
				exstat |= 1;
				if (ep->svalue == 0) {
					if (v0080 == 0)
						fprintf(stderr, "Undefined:\n");
					v0080++;
					put_name(ep->sname);
					--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = '\n') : _flsbuf(10, stderr);
				}
			}
			break;
		case '"':
			ep->svalue += (unsigned)v6bf8;
			break;
		case '#':
			ep->svalue += (unsigned)v6bfa;
			break;
		case '$':
			ep->svalue += (unsigned)v6bfc;
			break;
		case '%':
			ep->stype = '$';
			ep->svalue += (unsigned)x6bfe;
			break;
		}
		ep = next_entry(ep);
	}
	if (sflag || xflag)
		v66ce = 0;
	btotal = add16_checked(btotal, comsize, "bss overflow");
	v6bf6 = (unsigned)v66ce / 14;
}

place_sec()
{
	int spare1, spare2, spare3;
	int i;
	register int j;
	register struct areas *q;
	register int r8;

	if (iflag)
		areas[1].kind = 4;
	else
		areas[1].kind = 0;
	if (x66b8)
		areas[3].kind = 4;
	else
		areas[3].kind = 2;
	if (iflag)
		areas[2].kind = 0;
	else
		areas[2].kind = 1;
	areas[1].tsize = ttotal;
	if (iflag == 0 && x66b8 == 0 && x66ba == 0 && rflag == 0)
		areas[1].tsize = (ttotal + 255) & 0xff00;
	areas[2].tsize = dtotal;
	areas[3].tsize = comsize + btotal;
	if (tab42b4[0]) {
		for (i = iflag ? 2 : 1; i < 4; i++) {
			if (tab42b4[i]) {
				fprintf(stderr, "%s: warning, -%c overrides -%c%c\n", pname, tab42b4[0], tab42b4[i], *secnam[i]);
				tab42b4[i] = 0;
			}
		}
	}
	areas[0].kind = 4;
	areas[0].tsize = (dtotal + comsize + btotal + iflag) == 0 ? ttotal : 0;
	j = tab42b4[0] ? 0 : 1;
	for (; j < 4; j++) {
		q = &areas[j];
		r8 = areas[q->kind].tend;
		switch (tab42b4[j]) {
		case 't':
			if (q->tsize > secorg[j])
				report_error(1, "-t%c: top address too low", *secnam[j]);
			r8 = (secorg[j] - q->tsize) & 0xff00;
			q->tsize = (q->tsize + 255) & 0xff00;
			break;
		case 'b':
			r8 = (secorg[j] + 255) & 0xff00;
			if (r8 != secorg[j])
				fprintf(stderr, "%s: warning, bottom address of %s in segment %d relocated to 0x%x\n", pname, secnam[j], 0, r8);
			break;
		}
		areas[0].tsize = 0;
		q->tend = r8 + q->tsize;
		if (tab42b4[j + 1]) {
			if (x66b8 == 0 && x66ba == 0)
				q->tend = (q->tend + 255) & 0xff00;
		}
		q->tsize = q->tend - r8;
		if (r8 > q->tend)
			report_error(1, "%s area wrap around", secnam[j]);
		if (x66b8 == 0) {
			for (i = iflag ? 2 : 1; i < j; i++) {
				if (r8 < areas[i].tend && q->tend > areas[i].tstart)
					report_error(1, "%s and %s areas overlap", secnam[j], secnam[i]);
			}
		}
		q->tstart = r8;
	}
	if (areas[3].tsize != comsize + btotal)
		btotal = areas[3].tsize - comsize;
	ttotal = areas[1].tsize;
	dtotal = areas[2].tsize;
	v6bf8 = areas[1].tstart;
	v6bfa = areas[2].tstart;
	x6bfe = areas[3].tstart;
	v6bfc = areas[3].tstart + comsize;
	if (tab42b4[0])
		tab42b4[iflag ? 2 : 1] = tab42b4[0];
}

/* start_output: open the output and temporary files and write the header (sld start_output; 0x108e..0x120e of /bin/ld) */
start_output()
{
	struct objhdr hdr;
	struct fseg seg;

	open_tmp(&toutb, 0);
	mktemp(tmpname);
	open_tmp(&tmpoutb, 1);
	if (!sflag || !xflag)
		open_tmp(&soutb, 1);
	if (rflag) {
		open_tmp(&routb, 1);
		open_tmp(&doutb, 1);
	}
	filhdr.fmagic = x66b8 || x66ba ? (oflag ? 0xe505 : iflag ? 0xe511 : 0xe507) : (oflag ? 0xe705 : iflag ? 0xe711 : 0xe707);
	filhdr.tsize = ttotal;
	filhdr.dsize = dtotal;
	filhdr.bsize = btotal;
	if (rflag) {
		filhdr.x1066 = v66c4;
		filhdr.x106a = v66c8;
	}
	filhdr.ssize = sflag ? 0 : v66ce + symindex * 14;
	if (entrypt) {
		if (entrypt->stype != '"')
			report_error(1, "Entry point not in text");
		else
			filhdr.entry = entrypt->svalue;
	} else
		filhdr.entry = v6bf8;
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	filhdr.relflg = rflag ? 0 : 1;
	if (x66b8)
		filhdr.relflg |= 4;
	if (x66ba)
		filhdr.relflg |= 8;
	init_entry(&hdr, &seg);
	if (swapflg) {
		swab(&hdr, &hdr, 0x18);
		swap_quad(&seg);
	}
	mput(&toutb, &hdr, 0x18);
	mput(&toutb, &seg, 0x10);
}

/* open_tmp: create the temporary file (flag) or the output file and open it for the buffer out (0x120e..0x12a8 of /bin/ld) */
open_tmp(out, flag)
struct buf *out;
int flag;
{
	int spare;
	register int fd;
	char *name;

	name = flag ? tmpname : ofilnam;
	fd = creat(name, 0x1b6);
	if (fd < 0) {
		if (flag)
			report_error(2, "cannot create temp");
		else {
			fprintf(stderr, "ld: cannot create output\n");
			unlink("l.out");
			exit(2);
		}
	}
	close(fd);
	out->fildes = open(name, 2);
	if (flag)
		unlink(tmpname);
	out->nleft = 0x100;
	out->xnext = out->iobuf;
}

init_entry(hdr, seg)
struct objhdr *hdr;
struct fseg *seg;
{
	int spare1, spare2;

	register char *q;
	register int n;

	hdr->hmagic = filhdr.fmagic;
	hdr->hx = (long)filhdr.tsize + filhdr.dsize;
	hdr->hbss = filhdr.bsize;
	hdr->hsegsz = 0x10;
	hdr->hrelflg = filhdr.relflg;
	hdr->hsymsz = filhdr.ssize;
	hdr->hentry = filhdr.entry;
	if (x66b8 || x66ba)
		hdr->hy = filhdr.x1066;
	else
		hdr->hy = 0;
	hdr->hz = 0;
	q = (char *)seg;
	n = 16;
	do {
		*q++ = 0;
		n--;
	} while (n != 0);
	seg->s_code = filhdr.tsize;
	seg->atr = 7;
	seg->s_data = filhdr.dsize;
	seg->s_bss = filhdr.bsize;
	if (x702f) {
		seg->atr |= 0x10;
		seg->s_coff = high_byte(v6bf8);
	}
	if (x7030) {
		seg->atr |= 0x20;
		seg->s_doff = high_byte(v6bfa);
	}
	if (x7031) {
		seg->atr |= 0x40;
		seg->s_boff = high_byte(x6bfe);
	}
}

/* load2arg: pass two for one argument (object file or archive member; sld load2arg; 0x13ca..0x1452 of /bin/ld) */
load2arg(p)
char *p;
{
	register char *q;
	register long *lp;
	int spare[2];

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
		mkfsym(q);
		load2(0L);
	} else {
		lp = libp;
		goto test1;
body1:
		dseek(&text, *lp, 26);
		mget(&archdr, 26);
		mkfsym(archdr.aname);
		load2(*lp + 13);
		lp++;
test1:
		if (*lp != -1)
			goto body1;
		lp++;
		libp = lp;
	}
	close(infil);
}

load2(a)
long a;
{
	int spare1, spare2;
	char spare3, spare4, spare5;
	char c;
	char spare6;
	char d;
	int n;
	long u1;
	long u2;
	register struct symbol *rec;
	register struct local *t;
	register int k;

	readhdr(a);
	fctrel = v6bf8;
	fcdrel += v6bfa;
	fcbrel += v6bfc;
	t = local;
	a += (unsigned)((hdrsz + segtsz) & ~1) >> 1;
	dseek(&text, a + filhdr.x1066 + filhdr.x106a, filhdr.ssize);
	k = 0;
	goto test;
body:
	n = read_entry_hdr() + 1;
	symreloc();
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
	if (!rec) {
		if (put_name(cursym.sname) < 8)
			putc(9, stderr);
		putc(0x20, stderr);
		report_error(2, "internal error: symbol not found");
	}
	if (cursym.stype != 0x20)
		goto other;
	if (t >= (struct local *)0x6692)
		report_error(2, "Local symbol overflow");
	t->locindex = k;
	t->locsymbol = (int)rec;
	t++;
	goto next;
other:
	if (rec->stype == cursym.stype && cursym.svalue == rec->svalue)
		goto next;
	if (put_name(cursym.sname) < 8)
		putc(9, stderr);
	putc(0x20, stderr);
	report_error(1, "- Multiply defined");
next:
	k += n;
test:
	if (text.size > 0)
		goto body;
	dseek(&text, a, filhdr.x1066);
	u1 = half(filhdr.x106a);
	u2 = half(filhdr.x1066) + a + u1;
	dseek(&reloc, u2, filhdr.x1066);
	load2td(t, fctrel, &toutb, &routb);
	dseek(&text, half(filhdr.x1066) + a + u1, filhdr.x106a);
	u1 = filhdr.x1066 + a + half(filhdr.x106a);
	dseek(&reloc, u1, filhdr.x106a);
	load2td(t, fcdrel, &tmpoutb, &doutb);
	v6bf8 += filhdr.tsize;
	v6bfa += filhdr.dsize;
	v6bfc += filhdr.bsize;
}

load2td(p, base, q, r)
struct local *p;
int base;
struct buf *q;
struct buf *r;
{
	int spare1, spare2;
	int w10, w11;
	char flag;
	int spare4;
	register int w13;
	register int w14;
	register int w12;
	register struct symbol *e;

	flag = 0;
	goto test;
body:
	if (--text.nibuf >= 0)
		w12 = *text.ptr++;
	else {
		text.nibuf++;
		text.size++;
		w12 = get(&text);
	}
relpart:
	if (--reloc.size > 0) {
		if (--reloc.nibuf < 0) {
			reloc.nibuf++;
			reloc.size++;
			w13 = get(&reloc);
		} else
			w13 = *reloc.ptr++;
	} else {
		if (reloc.size < 0)
			report_error(2, "Relocation error");
		reloc.size++;
		w13 = get(&reloc);
	}
	if (swapflg) {
		w12 = swap(w12);
		w13 = swap(w13);
	}
	if (x66ba)
		w12 = swap(w12);
	switch (w13 & 0x0e) {
	case 2:
		w12 += fctrel;
		break;
	case 4:
		w12 += fcdrel;
		break;
	case 6:
		w12 += fcbrel;
		break;
	}
	if ((w13 & 8) == 8) {
		e = (struct symbol *)table_lookup(p, w13);
		if (e->stype == 0x20)
			w13 = (index16(e) + v6bf6 << 4) + (w13 & 1) + 8;
		else {
			w12 += (int)e->svalue;
			w13 = (w13 & 1) + ((e->stype - 0x21) * 2) + 0x21;
		}
	}
	w14 = w13;
	switch (w13 & 0xf8) {
	case 0x10:
		flag++;
		break;
	case 0x20:
		w12 = swap(w12);
		flag++;
		break;
	case 0x30:
		flag++;
		break;
	}
	switch (w13 & 0x0f) {
	case 9:
		flag++;
		break;
	case 10:
		w12 = swap(w12);
		flag++;
		break;
	case 11:
		flag++;
		break;
	}
	if (swapflg) {
		w12 = swap(w12);
		w13 = swap(w13);
	}
	if (flag) {
		flag = 0;
		if (swapflg)
			w12 = swap(w12);
		put_int_buf(w12, q);
	} else {
		f221e(w12, q);
		if (rflag)
			f221e(w13, r);
	}
test:
	if (--text.size > 0)
		goto body;
	if (text.size >= 0) {
		text.size++;
		w12 = get(&text);
		goto relpart;
	}
}

/* finish_output: write the remaining parts of the output and patch the header (sld finish_output; 0x194a..0x1a12 of /bin/ld) */
finish_output()
{
	int spare1, spare2;

	register int *p;
	register int *end;

	p = (int *)v6bf8;
	end = (int *)areas[1].tend;
	goto test1;
body1:
	p++;
	f221e(0, &toutb);
	if (rflag)
		f221e(0, &soutb);
test1:
	if (p < end)
		goto body1;
	p = (int *)v6bfa;
	end = (int *)areas[2].tend;
	goto test2;
body2:
	p++;
	f221e(0, &tmpoutb);
	if (rflag)
		f221e(0, &doutb);
test2:
	if (p < end)
		goto body2;
	copy(&tmpoutb);
	if (rflag)
		copy(&routb);
	copy(&doutb);
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
	sig_handler();
}

copy(p)
struct buf *p;
{
	int spare1, spare2;
	unsigned char *q;
	register fd;
	register n;

	flush_buffer(p);
	fd = p->fildes;
	lseek(fd, 0L, 0);
	goto test;
body:
	q = (unsigned char *)tmpoutb.iobuf;
	do
		put_int_buf(*q++, &toutb);
	while (--n);
test:
	n = read(fd, tmpoutb.iobuf, 256);
	if (n > 1)
		goto body;
	close(fd);
}

high_byte(x)
unsigned x;
{
	if ((x & 0xff00) != x)
		report_error(2, "internal error - mkchar");
	return((x >> 8) & 0xff);
}

struct local local[1];

/* mget: read n bytes of the text stream into the buffer p (0x1a9a..0x1b0a of /bin/ld) */
mget(p, n)
int *p;
int n;
{
	int spare1, spare2, spare3;

	register int *r;
	register i;
	register int *q;

	i = n;
	i >>= 1;
	r = p;
	text.nibuf -= i;
	if (text.nibuf >= 0) {
		text.size -= i;
		if (text.size > 0) {
			q = text.ptr;
			do
				*r++ = *q++;
			while (--i);
			text.ptr = q;
			return;
		}
		text.size += i;
	}
	text.nibuf += i;
	do
		*r++ = get(&text);
	while (--i);
}

/* mput: write n bytes (words) of the array a to the output buffer q (0x1b0a..0x1b38 of /bin/ld) */
mput(q, p, n)
struct buf *q;
int *p;
int n;
{
	int spare1, spare2;

	register int *r;
	register i;

	r = p;
	i = n >> 1;
	do {
		f221e(*r++, q);
	} while (--i);
}

dseek(sp, v, n)
struct stream *sp;
long v;
int n;
{
	int spare1, spare2;
	int nw;
	long hi, lo;
	register struct stream *obj;
	register struct page *cur;

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
	obj->pno = (struct pghdr *)cur;
	obj->ptr = (int *)((char *)cur->buff + (lo << 1));
	if (n != -1)
		obj->size = half(n);
	obj->nibuf = cur->nibuf - (int)lo;
	if (obj->nibuf <= 0)
		obj->size = 0;
}

half(x)
int x;
{
	return((x >> 1) & 0x7fff);
}

get(p)
register struct stream *p;
{
	int spare;

	if (--p->nibuf < 0) {
		dseek(p, -1, (long)(p->bno + 1) << 8);
		--p->nibuf;
	}
	if (--p->size <= 0) {
		if (p->size < 0)
			report_error(2, "Premature EOF");
		fpage.nuser++;
		p->pno->nuser--;
		p->pno = &fpage;
	}
	return(*p->ptr++);
}

getfile(name)
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
		filname = libpath;
		if (s[2] == 0)
			s = "-lc";
		p = rindex(libname, '/') + 4;
		strncpy(p, s + 2, 0x168 - (int)p);
		strcat(p, v66b4 ? ".a" : ".8");
		infil = open(libname, 0);
		if (infil == -1) {
			strcpy(libpath, "/usr");
			strcat(libpath, libname);
			infil = open(libpath, 0);
			if (infil == -1) {
				strcpy(libpath, "/z/bin");
				strcat(libpath, libname);
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
	reloc.pno = (struct pghdr *)&fpage;
	text.pno = (struct pghdr *)&fpage;
	fpage.nuser = 2;
	dseek(&text, 0L, 2);
	if (text.size <= 0)
		report_error(2, "Premature EOF");
	if (get(&text) != -155)
		return 0;
	dseek(&text, 1L, 0x1a);
	if (text.size <= 0)
		return 1;
	mget(&archdr, 0x1a);
	if (strncmp(archdr.aname, "__.SYMDEF", 14))
		return 1;
	fstat(infil, &st);
	if (st.mtime > archdr.atime)
		return 3;
	return 2;
}

/* report_error: print the message with the file name and stop when the level is above 1 (0x1e86..0x1f22 of /bin/ld) */
report_error(level, msg, a, b, c, d, e)
int level;
char *msg;
int a;
int b;
int c;
int d;
int e;
{
	if (!exstat)
		fprintf(stderr, "%s: ", pname);
	if (filname)
		fprintf(stderr, "%s", filname);
	if (archdr.aname[0])
		fprintf(stderr, "(%.14s)", archdr.aname);
	fprintf(stderr, ": ");
	fprintf(stderr, msg, a, b, c, d, e);
	fprintf(stderr, "\n");
	if (level > 1)
		sig_handler();
	exstat = level;
}

/* table_lookup: find an entry of the local symbol table (0x1f22..0x1f64 of /bin/ld) */
table_lookup(end, a)
struct local *end;
int a;
{
	int spare1, spare2, spare3;

	register struct local *p;
	register struct local *e;
	register k;

	e = end;
	k = (a >> 4) & 0x0fff;
	p = local;
	while (p < e) {
		if (p->locindex == k)
			return(p->locsymbol);
		p++;
	}
	report_error(2, "Local symbol botch");
}

readhdr(a)
long a;
{
	int magic;
	int spare1, spare2, spare3;
	int c0, c1, c2, w;
	struct objhdr tmp;
	struct fseg rec[2];
	register int t;
	register int i;
	register int n;

	dseek(&text, a, 2);
	mget(&magic, 2);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe507 || magic == 0xe511)
		;
	else
		report_error(2, "Bad format");
	hdrsz = 0x18;
	dseek(&text, a, hdrsz);
	mget(&tmp, hdrsz);
	if (swapflg)
		swab(&tmp, &tmp, hdrsz);
	segtsz = tmp.hsegsz;
	n = segtsz / 16;
	dseek(&text, a + (unsigned)(hdrsz >> 1), segtsz);
	mget(rec, segtsz);
	if (swapflg) {
		swap_quad(&rec[0]);
		if (n > 1)
			swap_quad(&rec[1]);
	}
	c2 = 0;
	c1 = 0;
	c0 = 0;
	if (n > 1) {
		w = rec[1].atr;
		if (w & 1)
			c0++;
		if (w & 2)
			c1++;
		if (w & 4)
			c2++;
	} else
		w = 0;
	filhdr.fmagic = magic & 0xf7ff;
	filhdr.bsize = rec[c2].s_bss;
	filhdr.tsize = rec[c0].s_code;
	filhdr.fstamp[0] = 0;
	filhdr.dsize = rec[c1].s_data;
	filhdr.fstamp[1] = 0;
	if ((rec[0].atr | w) & 0xf8)
		report_error(2, "Invalid segment attributes");
	filhdr.entry = tmp.hentry;
	filhdr.relflg = tmp.hrelflg & 1;
	filhdr.ssize = tmp.hsymsz;
	if (filhdr.fmagic == 0xe711 || filhdr.fmagic == 0xe511)
		x66b6 = 1;
	else
		x66b6 = 0;
	if (magic == 0xe507 || magic == 0xe511) {
		if (v66a4)
			report_error(2, "-r cannot be used with 8-bit files");
		if (tmp.hrelflg & 4)
			x66b8++;
		else
			x66ba++;
		fcdrel = x66b6 ? 0 : -filhdr.tsize;
		fcbrel = x66b8 ? 0 : fcdrel - filhdr.dsize;
		filhdr.x1066 = tmp.hy;
		filhdr.x106a = (int)tmp.hx - tmp.hy;
	} else {
		t = (filhdr.tsize + 1) & 0xfffe;
		filhdr.tsize = t;
		fcdrel = x66b6 ? 0 : -t;
		t = (filhdr.dsize + 1) & 0xfffe;
		fcbrel = fcdrel - t;
		filhdr.dsize = t;
		filhdr.bsize = (filhdr.bsize + 1) & 0xfffe;
		filhdr.x1066 = filhdr.tsize;
		filhdr.x106a = filhdr.dsize;
	}
}

/* str_compare: 1 when the strings are equal, else 0 (sld str_compare without cmpmode; 0x21f2..0x221e of /bin/ld) */
str_compare(p, q)
char *p, *q;
{
	for (;;) {
		if (*p == *q++) {
			if (*p++)
				continue;
			return(1);
		}
		return(0);
	}
}

f221e(x, p)
int x;
register struct buf *p;
{
	*p->xnext++ = x >> 8;
	p->nleft--;
	if (p->nleft <= 0)
		flush_buffer(p);
	*p->xnext++ = x;
	p->nleft--;
	if (p->nleft <= 0)
		flush_buffer(p);
}

put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

/* flush_buffer: write the buffer to its file and reset it (sld flush_buffer; 0x229c..0x22e8 of /bin/ld) */
flush_buffer(a)
register struct buf *a;
{
	int spare1;

	register n;

	n = (char *)a->xnext - (char *)a->iobuf;
	if (n > 0) {
		if (write(a->fildes, a->iobuf, n) != n)
			report_error(2, "output error");
	}
	a->xnext = a->iobuf;
	a->nleft = 0x100;
}

/* getfile: open an input file, searching the library directories (sld getfile; 0x1ce2..0x1e86 of /bin/ld; finding:
   three registers in the original, the count of strncpy and the last branch differ) */

/* load_header: read the header of an object file or archive member; a symbol-bearing object is entered in the list of
   the library (sld load_header; 0x0714..0x07b2 of /bin/ld) */

/* ranlib_scan: look up the undefined symbols in the ranlib table of an archive and load the members that define them (sld
   ranlib_scan; finding: the bound test of the table is computed in another register; 0x07b2..0x089a of /bin/ld) */

/* init_entry: fill the header of the output (struct objhdr) and the segment record (struct fseg) from the totals and the
   flags (sld init_entry; finding: the store of hy goes through the frame copy of hdr; 0x12a8..0x13ca of /bin/ld) */

/* readhdr: read the header of an object (or of the first member of an archive) and set filhdr from its segments (sld readhdr,
   nonsegmented form; finding: the error blocks are placed inline, the original jumps to them; 0x1f64..0x21f2 of /bin/ld) */

/* load2: pass two for one object: relocate its symbols, load the symbols of the output and the text and data of the object
   (sld load2; finding: the frame and the tail (the dseek and load2td calls of the text and the data) are approximate; 0x1452..0x172e of /bin/ld) */

/* load2td: relocate the words of the text and the relocation stream of one object and write them to the output (sld load2td;
   finding: the switch targets are decoded, the loop is in the order of the original but not yet exact; 0x172e..0x1926 of /bin/ld) */

/* place_sec: place the text, data and bss areas of the output (sld place_sec in the nonsegmented form of ld; finding: the
   kinds and the sizes are equal in the code, the instruction order of the stores differs; 0x0d9c..0x108e of /bin/ld) */

struct symbol **symhash[2339];
struct symbol *hshtab[2339];
struct symbol *lastsym;
char lnbuf[16];
int wflag, nlflag;
struct symbol *nsym;

