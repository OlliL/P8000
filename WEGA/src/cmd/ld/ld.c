#include <stdio.h>
#include "ld.h"

static char sccsid[] = "@[$]ld.c\t1.18  06/27/83 16:44:24 - 87wega3.2";

char eofmsg[] = "Premature EOF";
char symdef[] = "__.SYMDEF";
extern long liblist[];
long *libp = liblist;
char *secnam[4] = {"?", "text", "data", "bss"};
char *ofilnam = "l.out";
int delarg = 4;
int nlflag = 1;
int nundef = 0;
char tmpname[] = "/tmp/ldaXXXXX";
char *aoutnam = "a.out";

/* bss of ld in the order of the original (addresses from 0x0abe): the order of these declarations is the link order
   of the commons; the gaps are spare variables (see HANDOFF_ld.md, step 5b) */
int ntab;
struct overlay tab[16];
struct page page[2];
struct pghdr fpage;
struct stream text;
struct stream reloc;
int segtsz;
int hdrsz;
struct archdr archdr;
struct filhdr filhdr;
struct symbol cursym;
char lnbuf[126];
struct symbol sym16[600];
char spare3212[140];
struct symbol **symhash[2339];
struct symbol *lastsym;
int symindex;
struct symbol *hshtab[2341];
struct local local[1000];
struct symbol *p_etext, *p_edata, *p_end;
struct symbol *entrypt;
int Tflag;	/* -T: print each argument and the symbol count at the end */
int xflag;
int Xflag;
int Sflag;
int rflag;
int arflag;	/* -r was given (rflag can be set later by end_pass1 for undefined symbols) */
int sflag;
int oflag;
int dflag;
int wflag;
int iflag;
int vflag;	/* -v was given */
int swapflg;
int noswap;	/* the program name does not start with s: libraries end in .a, else in .8 */
int idsep;	/* the current object has separate I and D space (e711, e511) */
int n8rel4;	/* 8-bit objects (e507, e511) with bit 2 of hrelflg: the bss is not relocated (meaning of the bit: guess) */
int n8swap;	/* 8-bit objects without that bit: their words are byte-swapped on input and output */
int ofilfnd;
int infil;
char *filname;
int ttotal;
int fttotal;	/* -r: sum of the text sizes in the files (ftsize) */
int dtotal;
int fdtotal;	/* -r: sum of the data sizes in the files (fdsize) */
int comsize;
int btotal;
int stotal;	/* size of the local symbols of the output in bytes */
int fctrel, fcdrel, fcbrel;
char *pname;
struct buf toutb;
struct buf tmpoutb;
struct buf routb;
struct buf doutb;
struct buf soutb;
int nlocsym;	/* number of local symbol entries in front of the globals (stotal / 14) */
int torigin, dorigin, borigin;	/* start of text, data and bss of the current object in the output (Plexus) */
int corigin;	/* start of the common symbols (Plexus) */
struct rlent *rltab;
int rlcnt;
long liblist[256];
struct areas areas[4];
char spare7024[8];
char secspec;
char secflg[5];	/* option letter (t or b) given for the area all, text, data, bss */
int secorg[4];
char spare703c[2];
int exstat;
char libpath[20];
char spare7054[6];

/* sig_handler: the exit path of ld (also the handler of SIGINT and SIGTERM): the symbol count with -T, remove l.out,
   make the output executable when there was no error, remove it on an error, exit with delarg (Plexus delexit;
   0x0042..0x009c of /bin/ld) */
sig_handler()
{
	int mode;

	if (Tflag)
		limit_msg();
	unlink("l.out");
	if (!delarg) {
		if (!arflag || !nundef) {
			mode = ~umask(0) & 0x1ff;
			chmod(ofilnam, mode);
		}
	}
	if (delarg)
		unlink(ofilnam);
	exit(delarg);
}

/* main: entry of ld: the signals, the program name (s* gives swapped output), the option scan of pass one over the
   arguments, then pass two (parse_args) (sld main, Plexus main; 0x009c..0x0370 of /bin/ld) */
main(argc, argv)
int argc;
char **argv;
{
	register int i;
	register int j;
	register char *s;
	register char **av;
	int spare;
	int found;
	int k;
	char c;
	struct symbol **sp;

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
		noswap++;
	av = argv + 1;
	i = 1;
	goto test;
body:
	if (Tflag)
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
				vflag = 1;
				k = ntab;
				found = 0;
				while (--k >= 0) {
					if (found)
						break;
					found = str_compare(tab[k].vname, *av);
				}
				if (found) {
					parse_args(i, argv);
					restore(k);
				} else
					record(i, *av);
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
				arflag++;
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
				Tflag++;
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

/* reg_sect_addr: read the address of a section option (-t, -d, -b with the selector t, d, b or none) into secorg; 1
   when a selector was given (sld reg_sect_addr; 0x0370..0x045e of /bin/ld) */
int reg_sect_addr(s, t)
char *s;
char *t;
{
	register int k;
	char *fmt;
	register char *u;

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
	if (secflg[k])
		report_error(2, "-%.2s: -%c%c already specified", s, secflg[k], s[1]);
	secflg[k] = *s;
	fmt = "%o%c";
	if (*u != '0')
		fmt = "%d%c";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "%x%c";
	}
	if (sscanf(u, fmt, &secorg[k]) != 1)
		report_error(2, "-%.2s %s: bad address", s, t);
	if (k == 0)
		return 0;
	return 1;
}

/* parse_args: pass two over the arguments: end of pass one, open the output, then every object and library in order,
   then finish_output (Plexus endload; 0x045e..0x050c of /bin/ld) */
parse_args(argc, argv)
int argc;
char **argv;
{
	register int k;
	register int j;
	register char *a;
	register char **av;
	int spare;

	filname = 0;
	end_pass1();
	start_output();
	av = argv + 1;
	libp = liblist;
	k = 1;
	while (k < argc) {
		a = *av++;
		if (Tflag)
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

/* record: save the state of the load before the object named y (argument index x) in the next entry of tab: the symbol
   count, libp, the totals and the relocation bases, for a later restore by -v (Plexus record; 0x050c..0x057c of
   /bin/ld) */
record(x, y)
int x;
char *y;
{
	register struct overlay *e;

	e = &tab[ntab++];
	e->argsav = x;
	e->symsav = symindex;
	e->libsav = libp;
	e->vname = y;
	e->offt = ttotal;
	e->offd = dtotal;
	e->offb = btotal;
	e->offs = stotal;
	e->ctsav = fctrel;
	e->cdsav = fcdrel;
	e->cbsav = fcbrel;
}

/* restore: go back to the state saved by record in entry i of tab (libp, the relocation bases, the totals, ntab) and
   clear the hash slots of the symbols entered since then (Plexus restore; 0x057c..0x0608 of /bin/ld) */
restore(i)
int i;
{
	register struct overlay *e;
	register int saved;

	e = &tab[i];
	ntab = i + 1;
	libp = e->libsav;
	fctrel = e->ctsav;
	fcdrel = e->cdsav;
	fcbrel = e->cbsav;
	ttotal = e->offt;
	dtotal = e->offd;
	btotal = e->offb;
	stotal = e->offs;
	saved = e->symsav;
	while (symindex > saved) {
		symindex--;
		*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
}

/* load_lib_syms: pass one for one argument: an object, an archive without table of contents (every member through
   load_header) or an archive with __.SYMDEF (ranlib_scan until nothing more is loaded) (sld load_lib_syms, Plexus
   load1arg; 0x0608..0x0714 of /bin/ld) */
load_lib_syms(p)
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
		lock_rlbuf();
		rlcnt = archdr.asize / 12;
		rltab = (struct rlent *)ensure_buf(archdr.asize);
		lseek(infil, 28L, 0);
		read(infil, rltab, rlcnt * 12);
		chk_ranlib(rltab, rlcnt);
		while (ranlib_scan())
			;
		*libp++ = -1;
		unlock_rlbuf();
		break;
	case 3:
		report_error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(infil);
}

/* load_header: read the header of the archive member at word offset p; an object member whose symbols are needed is
   entered in the library list (libp); 0 at the end of the archive (sld load_header; 0x0714..0x07b2 of /bin/ld) */
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

/* ranlib_scan: one pass over the ranlib table of contents: load the members that define a symbol that is still
   undefined; 1 when a member was loaded (sld ranlib_scan; 0x07b2..0x089a of /bin/ld) */
int ranlib_scan()
{
	int i;
	struct rlent *end;
	struct symbol *t;
	long *sv;

	sv = libp;
	end = rltab;
	for (i = 0; (unsigned)i < (unsigned)rlcnt; i++) {
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
			longswap(&rltab[i]);
		load_header(rltab[i].roff >> 1);
	}
	return sv != libp;
}

/* add16_checked: sum of two words, report_error(1) with the message a when the sum is 0x10000 or more (sld
   add16_checked, Plexus add; 0x089a..0x08cc of /bin/ld) */
add16_checked(x, y, a)
int x, y;
char *a;
{
	long s;

	s = (long)(unsigned)x + (long)(unsigned)y;
	if (s >= 0x10000)
		report_error(1, a);
	return(s);
}

/* read_symbols: pass one for one object (a = 1 for an archive member): read the header and the symbol table, enter the
   symbols and add the sizes to the totals; 0 when an archive member defines nothing that is needed (its symbols are
   removed again). The putc forms with (char) casts: see warn_sym_redef (sld read_symbols, Plexus load1; 0x08cc..0x0b6e
   of /bin/ld) */
read_symbols(a, c)
int a;
long c;
{
	register struct symbol *cur;
	int saved;
	int cnt;
	char ch;
	char type;
	int k;
	int n;
	struct symbol **p;

	readhdr(c);
	fctrel = ttotal;
	fcdrel += dtotal;
	fcbrel += btotal;
	cnt = 0;
	k = 1;
	saved = symindex;
	if ((filhdr.relflg & 1) == 1) {
		report_error(1, "No relocation bits");
		goto done;
	}
	c = ((unsigned long)((unsigned)(hdrsz + segtsz) >> 1) + filhdr.ftsize + filhdr.fdsize) + c;
	dseek(&text, c, filhdr.ssize);
	while (text.size > 0) {
		n = read_entry_hdr();
		ch = cursym.stype;
		type = ch & 0x1f;
		if (Sflag && (type == 1 || type > 4))
			continue;
		if (Xflag && type == 0x1e)
			continue;
		if ((ch & 0x20) == 0) {
			if (Xflag && cursym.sname[0] == 'L')
				continue;
			k += n + 1;
			continue;
		}
		symreloc();
		p = lookup_name();
		if (enter(p))
			continue;
		cur = lastsym;
		if (cur->stype != ' ') {
			if (a)
				warn_sym_redef(cur, &cursym);
			continue;
		}
		if (cursym.stype == ' ') {
			if (cursym.svalue <= cur->svalue)
				continue;
			cur->svalue = cursym.svalue;
			continue;
		}
		if (cur->svalue != 0 && cursym.stype == '"' && !a) {
			if (put_name(cursym.sname) < 8)
				--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = '\t') : _flsbuf(9, stderr);
			--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = ' ') : _flsbuf(' ', stderr);
			report_error(0, "warning: external data item refers to text symbol");
		}
		cnt++;
		cur->stype = cursym.stype;
		cur->svalue = cursym.svalue;
	}
	if (a && cnt == 0)
		goto clean;
	ttotal = add16_checked(ttotal, filhdr.tsize, "text overflow");
	dtotal = add16_checked(dtotal, filhdr.dsize, "data overflow");
	btotal = add16_checked(btotal, filhdr.bsize, "bss overflow");
	stotal = add16_checked(stotal, k * 14, "symbol table overflow");
	if (rflag) {
		fttotal = add16_checked(fttotal, filhdr.ftsize, "file text overflow");
		fdtotal = add16_checked(fdtotal, filhdr.fdsize, "file data overflow");
	}
	return 1;
clean:
	while (symindex > saved) {
		symindex--;
		*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
done:
	return 0;
}

/* end_pass1: end of pass one: _etext, _edata, _end, the rflag decision, the common symbols, the undefined symbols, the
   layout (place_sec) and the values of the defined symbols. The newline of the undefined message is cast to char: the
   stdio of the original had char *_ptr (sign extension), src/head has unsigned char * (sld end_pass1, Plexus middle;
   0x0b6e..0x0d9c of /bin/ld) */
end_pass1()
{
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
			secflg[i] = 0;
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
				if (!n8rel4 && !n8swap)
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
	nundef = 0;
	ep = table16_entry(0);
	while (ep < end) {
		switch (ep->stype) {
		case ' ':
			if (arflag == 0) {
				exstat |= 1;
				if (ep->svalue == 0) {
					if (nundef == 0)
						fprintf(stderr, "Undefined:\n");
					nundef++;
					put_name(ep->sname);
					--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = '\n') : _flsbuf(10, stderr);
				}
			}
			break;
		case '"':
			ep->svalue += (unsigned)torigin;
			break;
		case '#':
			ep->svalue += (unsigned)dorigin;
			break;
		case '$':
			ep->svalue += (unsigned)borigin;
			break;
		case '%':
			ep->stype = '$';
			ep->svalue += (unsigned)corigin;
			break;
		}
		ep = next_entry(ep);
	}
	if (sflag || xflag)
		stotal = 0;
	btotal = add16_checked(btotal, comsize, "bss overflow");
	nlocsym = (unsigned)stotal / 14;
}

/* place_sec: place the text, data and bss areas of the output: sizes, the -t/-b addresses (secorg), rounding to 256
   bytes, the wrap and overlap checks, then the relocation bases (sld place_sec; 0x0d9c..0x108e of /bin/ld) */
place_sec()
{
	register struct areas *q;
	register unsigned r8;
	register int j;
	int i;

	areas[1].kind = iflag ? 4 : 0;
	areas[3].kind = n8rel4 ? 4 : 2;
	areas[2].kind = iflag ? 0 : 1;
	areas[1].tsize = iflag && n8rel4 == 0 && n8swap == 0 && rflag == 0 ? (ttotal + 255) & 0xff00 : ttotal;
	areas[2].tsize = dtotal;
	areas[3].tsize = comsize + btotal;
	if (secflg[0]) {
		for (i = iflag ? 2 : 1; i < 4; i++) {
			if (secflg[i]) {
				fprintf(stderr, "%s: warning, -%c overrides -%c%c\n", pname, secflg[0], secflg[i], *secnam[i]);
				secflg[i] = 0;
			}
		}
		areas[0].kind = 4;
		areas[0].tsize = dtotal + comsize + btotal + iflag ? 0 : ttotal;
	}
	for (j = secflg[0] ? 0 : 1; j < 4; j++) {
		q = &areas[j];
		r8 = areas[q->kind].tend;
		switch (secflg[j]) {
		case 't':
			if ((unsigned)q->tsize > (unsigned)secorg[j])
				report_error(1, "-t%c: top address too low", *secnam[j]);
			r8 = (secorg[j] - q->tsize) & 0xff00;
			q->tsize = (q->tsize + 255) & 0xff00;
			break;
		case 'b':
			r8 = (secorg[j] + 255) & 0xff00;
			if (r8 != (unsigned)secorg[j])
				fprintf(stderr, "%s: -b%c:warning, %s top or bottom rounded to 0x%x\n", pname, *secnam[j], secnam[j], r8);
			break;
		}
		areas[0].tsize = 0;
		q->tend = r8 + q->tsize;
		if (secflg[j + 1]) {
			if (n8rel4 == 0 && n8swap == 0)
				q->tend = (q->tend + 255) & 0xff00;
			q->tsize = q->tend - r8;
		}
		if (r8 > (unsigned)q->tend)
			report_error(1, "%s area wrap around", secnam[j]);
		if (n8rel4 == 0) {
			for (i = iflag ? 2 : 1; i < j; i++) {
				if (r8 < (unsigned)areas[i].tend && (unsigned)q->tend > (unsigned)areas[i].tstart)
					report_error(1, "%s and %s areas overlap", secnam[j], secnam[i]);
			}
		}
		q->tstart = r8;
	}
	if (comsize + btotal != areas[3].tsize)
		btotal = areas[3].tsize - comsize;
	ttotal = areas[1].tsize;
	dtotal = areas[2].tsize;
	torigin = areas[1].tstart;
	dorigin = areas[2].tstart;
	borigin = (corigin = areas[3].tstart) + comsize;
	if (secflg[0])
		secflg[iflag ? 2 : 1] = secflg[0];
}

/* start_output: open the output and the temporary files and fill the header of the output (sld start_output, Plexus
   setupout; 0x108e..0x120e of /bin/ld) */
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
	filhdr.fmagic = n8rel4 || n8swap ? (oflag ? 0xe505 : iflag ? 0xe511 : 0xe507) : (oflag ? 0xe705 : iflag ? 0xe711 : 0xe707);
	filhdr.tsize = ttotal;
	filhdr.dsize = dtotal;
	filhdr.bsize = btotal;
	if (rflag) {
		filhdr.ftsize = fttotal;
		filhdr.fdsize = fdtotal;
	}
	filhdr.ssize = sflag ? 0 : stotal + symindex * 14;
	if (entrypt) {
		if (entrypt->stype != '"')
			report_error(1, "Entry point not in text");
		else
			filhdr.entry = entrypt->svalue;
	} else
		filhdr.entry = torigin;
	filhdr.fstamp[0] = 0;
	filhdr.fstamp[1] = 0;
	filhdr.relflg = rflag ? 0 : 1;
	if (n8rel4)
		filhdr.relflg |= 4;
	if (n8swap)
		filhdr.relflg |= 8;
	init_entry(&hdr, &seg);
	if (swapflg) {
		swab(&hdr, &hdr, 0x18);
		swap_quad(&seg);
	}
	mput(&toutb, &hdr, 0x18);
	mput(&toutb, &seg, 0x10);
}

/* open_tmp: create the temporary file (flag 1) or the output file and open it for the buffer out (sld open_tmp, Plexus
   tcreat; 0x120e..0x12a8 of /bin/ld) */
open_tmp(out, flag)
struct buf *out;
int flag;
{
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

/* init_entry: fill the header of the output (struct objhdr) and its segment record (struct fseg) from filhdr and the
   section options (sld init_entry; 0x12a8..0x13ca of /bin/ld) */
init_entry(hdr, seg)
struct objhdr *hdr;
struct fseg *seg;
{
	register char *q;
	register int n;

	hdr->hmagic = filhdr.fmagic;
	hdr->hx = (long)filhdr.tsize + filhdr.dsize;
	hdr->hbss = filhdr.bsize;
	hdr->hsegsz = 0x10;
	hdr->hrelflg = filhdr.relflg;
	hdr->hsymsz = filhdr.ssize;
	hdr->hentry = filhdr.entry;
	if (n8rel4 || n8swap)
		hdr->hy = filhdr.ftsize;
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
	if (secflg[1]) {
		seg->atr |= 0x10;
		seg->s_coff = high_byte(torigin);
	}
	if (secflg[2]) {
		seg->atr |= 0x20;
		seg->s_doff = high_byte(dorigin);
	}
	if (secflg[3]) {
		seg->atr |= 0x40;
		seg->s_boff = high_byte(corigin);
	}
}

/* load2arg: pass two for one argument (an object or the archive members of the library list) (sld load2arg, Plexus
   load2arg; 0x13ca..0x1452 of /bin/ld) */
load2arg(p)
char *p;
{
	register char *q;
	register long *lp;

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

/* load2: pass two for one object: its local symbols into the local table, its global symbols checked against the
   table, then the text and the data with load2td. The putc forms with (char) casts: see warn_sym_redef (sld load2,
   Plexus load2; 0x1452..0x172e of /bin/ld) */
load2(a)
long a;
{
	register struct symbol *rec;
	register struct local *t;
	register int k;
	char c;
	char d;
	int n;
	long u1;
	long u2;

	readhdr(a);
	fctrel = torigin;
	fcdrel += dorigin;
	fcbrel += borigin;
	t = local;
	a += (unsigned)((hdrsz + segtsz)) >> 1;
	dseek(&text, a + filhdr.ftsize + filhdr.fdsize, filhdr.ssize);
	k = 0;
	goto test;
body:
	n = read_entry_hdr() + 1;
	symreloc();
	c = cursym.stype;
	d = c & 0x1f;
	if (Sflag) {
		if (d == 1 || d > 4)
			goto next;
	}
	if (Xflag && d == 0x1e)
		goto next;
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
			--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = '\t') : _flsbuf(9, stderr);
		--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = ' ') : _flsbuf(' ', stderr);
		report_error(2, "internal error: symbol not found");
	}
	if (cursym.stype != 0x20)
		goto other;
	if (t >= &local[1000])
		report_error(2, "Local symbol overflow");
	t->locindex = k;
	t->locsymbol = (int)rec;
	t++;
	goto next;
other:
	if (cursym.stype == rec->stype && cursym.svalue == rec->svalue)
		goto next;
	if (put_name(cursym.sname) < 8)
		--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = '\t') : _flsbuf(9, stderr);
	--stderr->_cnt >= 0 ? (int)(char)(*stderr->_ptr++ = ' ') : _flsbuf(' ', stderr);
	report_error(1, "- Multiply defined");
next:
	k += n;
test:
	if (text.size > 0)
		goto body;
	dseek(&text, a, filhdr.ftsize);
	u1 = half(filhdr.fdsize);
	u2 = half(filhdr.ftsize) + a + u1;
	dseek(&reloc, u2, filhdr.ftsize);
	load2td(t, fctrel, &toutb, &routb);
	u1 = half(filhdr.ftsize) + a;
	dseek(&text, u1, filhdr.fdsize);
	u1 = filhdr.ftsize + a + half(filhdr.fdsize);
	dseek(&reloc, u1, filhdr.fdsize);
	load2td(t, fcdrel, &tmpoutb, &doutb);
	torigin += filhdr.tsize;
	dorigin += filhdr.dsize;
	borigin += filhdr.bsize;
}

/* load2td: relocate the words of the text or data of one object with their relocation words and write them to the
   output (q) and with -r the relocation to r (sld load2td, Plexus load2td; 0x172e..0x194a of /bin/ld) */
load2td(p, base, q, r)
struct local *p;
int base;
struct buf *q;
struct buf *r;
{
	register int w13;
	register int w14;
	register int w12;
	register struct symbol *e;
	char flag;
	unsigned char ch[2];

	flag = 0;
	for (;;) {
		if (--text.size <= 0) {
			if (text.size < 0)
				break;
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
			w12 = swap(w12);
			w13 = swap(w13);
		}
		w14 = w13;
		if (n8swap)
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
				w13 = (w13 & 1) + (index16(e) + nlocsym << 4) + 8;
			else {
				w12 += (int)e->svalue;
				w13 = (w13 & 1) + ((e->stype - 0x21) * 2);
			}
		}
		if (n8swap)
			w12 = swap(w12);
		if (n8rel4 || n8swap)
			switch (w14 & 0xf8) {
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
		switch (w14 & 0x0f) {
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
			if (swapflg) {
				ch[0] = swap(w12);
				put_int_buf(ch[0], q);
			} else
				put_int_buf((unsigned char)w12, q);
		} else {
			put_word(w12, q);
			if (rflag)
				put_word(w13, r);
		}
	}
}

/* finish_output: pad text and data to the area ends, copy the temporary files into the output, the symbols, then
   rewrite the header (sld finish_output, Plexus finishout; 0x194a..0x1a12 of /bin/ld) */
finish_output()
{
	register int *p;
	register int *end;

	p = (int *)torigin;
	end = (int *)areas[1].tend;
	goto test1;
body1:
	p++;
	put_word(0, &toutb);
	if (rflag)
		put_word(0, &routb);
test1:
	if (p < end)
		goto body1;
	p = (int *)dorigin;
	end = (int *)areas[2].tend;
	goto test2;
body2:
	p++;
	put_word(0, &tmpoutb);
	if (rflag)
		put_word(0, &doutb);
test2:
	if (p < end)
		goto body2;
	copy(&tmpoutb);
	if (rflag) {
		copy(&routb);
		copy(&doutb);
	}
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

/* copy: append the temporary file of the buffer p to the output (sld copy, Plexus copy; 0x1a12..0x1a74 of /bin/ld) */
copy(p)
struct buf *p;
{
	register fd;
	register n;
	unsigned char *q;

	flush_buffer(p);
	lseek(fd = p->fildes, 0L, 0);
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

/* high_byte: the page number of an address that must be a multiple of 256 ("internal error - mkchar" otherwise) (sld
   high_byte; 0x1a74..0x1a9a of /bin/ld) */
high_byte(x)
unsigned x;
{
	if ((x & 0xff00) != x)
		report_error(2, "internal error - mkchar");
	return((x >> 8) & 0xff);
}

/* mget: read n bytes of the text stream into p (sld mget, Plexus mget; 0x1a9a..0x1b0a of /bin/ld) */
mget(p, n)
int *p;
int n;
{
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

/* mput: write n bytes of a to the output buffer q (word by word) (sld mput, Plexus mput; 0x1b0a..0x1b38 of /bin/ld) */
mput(q, p, n)
struct buf *q;
int *p;
int n;
{
	register int *r;
	register i;

	r = p;
	i = n >> 1;
	do {
		put_word(*r++, q);
	} while (--i);
}

/* dseek: position the input stream sp at the word v of the file with n bytes to read: find or fill one of the two
   pages (sld dseek, Plexus dseek; 0x1b38..0x1c5a of /bin/ld) */
dseek(sp, v, n)
struct stream *sp;
long v;
int n;
{
	register struct stream *obj;
	register struct page *cur;
	long hi, lo;
	int nw;

	hi = v >> 8;
	lo = v & 0xff;
	obj = sp;
	obj->pno->nuser -= 1;
	if ((cur = &page[0])->bno == hi || (cur = &page[1])->bno == hi)
		goto common;
	if (cur->nuser == 0 || (cur = &page[0])->nuser == 0) {
		if (page[0].nuser == 0 && page[1].nuser == 0 && page[0].bno < page[1].bno)
			cur = &page[0];
		cur->bno = hi;
		lseek(infil, (v & 0xffffff00L) << 1, 0);
		nw = read(infil, cur->buff, 0x200) >> 1;
		if (nw < 0)
			nw = 0;
		cur->nibuf = nw;
	} else
		report_error(2, "No pages");
common:
	cur->nuser += 1;
	obj->bno = hi;
	obj->pno = (struct pghdr *)cur;
	obj->ptr = (int *)((char *)cur->buff + (lo << 1));
	if (n != -1)
		obj->size = half(n);
	if ((obj->nibuf = cur->nibuf - lo) <= 0)
		obj->size = 0;
}

/* half: a byte count in words (unsigned) (sld half, Plexus half; 0x1c5a..0x1c6c of /bin/ld) */
half(x)
int x;
{
	return((x >> 1) & 0x7fff);
}

/* get: next word of the input stream p; "Premature EOF" when the stream is exhausted (sld get, Plexus get;
   0x1c6c..0x1ce2 of /bin/ld) */
get(a)
struct stream *a;
{
	register struct stream *p;

	p = a;
	if (--p->nibuf < 0) {
		dseek(p, (long)(p->bno + 1) << 8, -1);
		--p->nibuf;
	}
	if (--p->size <= 0) {
		if (p->size < 0)
			report_error(2, eofmsg);
		fpage.nuser++;
		p->pno->nuser--;
		p->pno = &fpage;
	}
	return(*p->ptr++);
}

/*
 * The library prefixes and the library name are arrays between "/usr" and "/z/bin" in the data of the original: after
 * the jump tables of the file and before its string literals, so they are declared here, after the last switch.
 */
static char pfx1[] = "/usr";
char libname[] = "/lib/libxxxxxxxxxxx";
static char pfx2[] = "/z/bin";

/* getfile: open an input file; -lx searches /lib/libx.a (or .8), /usr/lib, /z/bin/lib; 0 for an object, 1 for an
   archive, 2 for an archive with a current __.SYMDEF, 3 for an outdated one (sld getfile, Plexus getfile;
   0x1ce2..0x1e86 of /bin/ld) */
getfile(name)
char *name;
{
	register char *s;
	register char *p;
	register int n;
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
		strncpy(p, s + 2, &libname[18] - p);
		strcat(p, noswap ? ".a" : ".8");
		infil = open(libname, 0);
		if (infil != -1)
			filname = libname;
		else {
			strcpy(libpath, pfx1);
			strcat(libpath, libname);
			infil = open(libpath, 0);
			if (infil == -1) {
				strcpy(libpath, pfx2);
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
		report_error(2, eofmsg);
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

/* report_error: print a message with the program name and the file (archive member) name; level 2 stops ld, level 1
   marks an error (sld report_error, Plexus error; 0x1e86..0x1f22 of /bin/ld) */
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

/* table_lookup: the symbol of the local index in the relocation word a, searched in the local table up to end (sld
   table_lookup, Plexus lookloc; 0x1f22..0x1f64 of /bin/ld) */
table_lookup(end, a)
struct local *end;
int a;
{
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

/* readhdr: read the header of the object at word offset a and set filhdr (sizes, entry, symbols, relocation flag) from
   its segment records (sld readhdr, Plexus readhdr; 0x1f64..0x21f2 of /bin/ld) */
readhdr(a)
long a;
{
	int magic;
	register int t;
	register int i;
	register int n;
	int c0, c1, c2, w;
	struct objhdr tmp;
	struct fseg rec[2];

	dseek(&text, a, 2);
	mget(&magic, 2);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe507 || magic == 0xe511) {
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
	} else
		report_error(2, "Bad format");
	idsep = filhdr.fmagic == 0xe711 || filhdr.fmagic == 0xe511 ? 1 : 0;
	if (magic == 0xe507 || magic == 0xe511) {
		if (arflag)
			report_error(2, "-r cannot be used with 8-bit files");
		if (tmp.hrelflg & 4)
			n8rel4++;
		else
			n8swap++;
		fcdrel = idsep ? 0 : -filhdr.tsize;
		fcbrel = n8rel4 ? 0 : fcdrel - filhdr.dsize;
		filhdr.ftsize = tmp.hy;
		filhdr.fdsize = (int)tmp.hx - tmp.hy;
	} else {
		t = (filhdr.tsize + 1) & 0xfffe;
		filhdr.tsize = t;
		fcdrel = idsep ? 0 : -t;
		i = (filhdr.dsize + 1) & 0xfffe;
		fcbrel = fcdrel - i;
		filhdr.bsize = (filhdr.bsize + 1) & 0xfffe;
		filhdr.ftsize = filhdr.tsize;
		filhdr.fdsize = filhdr.dsize;
	}
}

/* str_compare: 1 when the strings are equal, else 0 (sld str_compare, Plexus eq; 0x21f2..0x221e of /bin/ld) */
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

/* put_word: put a word into the output buffer p byte by byte, high byte first (the byte order of the output does not
   depend on the machine) (Plexus putw puts the word whole; 0x221e..0x226e of /bin/ld) */
put_word(x, p)
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

/* put_int_buf: put a byte into the output buffer p (sld put_int_buf; 0x226e..0x229c of /bin/ld) */
put_int_buf(v, p)
int v;
register struct buf *p;
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush_buffer(p);
}

/* flush_buffer: write the buffer to its file and reset it (sld flush_buffer, Plexus flush; 0x229c..0x22e8 of /bin/ld) */
flush_buffer(a)
register struct buf *a;
{
	register n;

	n = (char *)a->xnext - (char *)a->iobuf;
	if (n > 0) {
		if (write(a->fildes, a->iobuf, n) != n)
			report_error(2, "output error");
	}
	a->xnext = a->iobuf;
	a->nleft = 0x100;
}

