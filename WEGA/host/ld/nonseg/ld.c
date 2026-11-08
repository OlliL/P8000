#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "ld.h"
#include "ldconf.h"
#include <errno.h>

static char sccsid[] __attribute__((unused)) = "@[$]ld.c\t1.18  06/27/83 16:44:24 - 87wega3.2";

char premeof[] = "Premature EOF";
char symdef[] = "__.SYMDEF";
extern int32_t liblist[];
int32_t *libp = liblist;
char *secnam[4] = {"?", "text", "data", "bss"};
char *ofilename = "l.out";
int16_t delarg = 4;
int16_t nlflag = 1;
int16_t nundef = 0;
char tfname[] = "/tmp/ldaXXXXXX";	/* one X more than the original: mkstemp */
char *aoutnam = "a.out";

/* bss of ld in the order of the original (addresses from 0x0abe): the order of these declarations is the link order
   of the commons; the gaps are spare variables */
int16_t vindex;
struct overlay vnodes[16];
struct page page[2];
struct pghdr fpage;
struct stream text;
struct stream reloc;
int16_t segtsz;
int16_t hdrsz;
struct archdr archdr;
struct filhdr filhdr;
struct curbuf curbuf;
struct symbol sym16[600 + 10];	/* 10 entries (140 bytes) more: spare3212 of the original, a long name at the end spills into it */
struct symbol **symhash[2339];
struct symbol *lastsym;
int16_t symindex;
struct symbol *hshtab[2341];
struct local local[1000];
struct symbol *p_etext, *p_edata, *p_end;
struct symbol *entrypt;
int16_t trace;	/* -T: print each argument and the symbol count at the end */
int16_t xflag;
int16_t Xflag;
int16_t Sflag;
int16_t rflag;
int16_t arflag;	/* -r was given (rflag can be set later by middle for undefined symbols) */
int16_t sflag;
int16_t oflag;
int16_t dflag;
int16_t wflag;
int16_t iflag;
int16_t vflag;	/* -v was given */
int16_t swapflg;
int16_t noswap;	/* the program name does not start with s: libraries end in .a, else in .8 */
int16_t idsep;	/* the current object has separate I and D space (e711, e511) */
int16_t n8rel4;	/* 8-bit objects (e507, e511) with bit 2 of hrelflg: the bss is not relocated (meaning of the bit: guess) */
int16_t n8swap;	/* 8-bit objects without that bit: their words are byte-swapped on input and output */
int16_t ofilfnd;
int16_t infil;
char *filname;
int16_t tsize;
int16_t fttotal;	/* -r: sum of the text sizes in the files (ftsize) */
int16_t dsize;
int16_t fdtotal;	/* -r: sum of the data sizes in the files (fdsize) */
int16_t comsize;
int16_t bsize;
int16_t ssize;	/* size of the local symbols of the output in bytes */
int16_t ctrel, cdrel, cbrel;
char *pname;
struct buf toutb;
struct buf tmpoutb;
struct buf routb;
struct buf doutb;
struct buf soutb;
int16_t nlocsym;	/* number of local symbol entries in front of the globals (ssize / 14) */
int16_t torigin, dorigin, borigin;	/* start of text, data and bss of the current object in the output (Plexus) */
int16_t corigin;	/* start of the common symbols (Plexus) */
struct rlent *rltab;
int16_t rlcnt;
int32_t liblist[256];
struct areas areas[5];	/* areas[4] is spare7024 of the original: place_sec reads it for a kind of 4 (all zero) */
char secspec;
char secflg[5];	/* option letter (t or b) given for the area all, text, data, bss */
int16_t secorg[4];
char spare703c[2];
int16_t errlev;
char libpath[20 + 6];	/* the name "/z/bin/lib/libxxxxxxxx.a" is longer than the 20 bytes of the original: it runs into spare7054 */

/* delexit: the exit path of ld (also the handler of SIGINT and SIGTERM): the symbol count with -T, remove l.out,
   make the output executable when there was no error, remove it on an error, exit with delarg (Plexus delexit;
   0x0042..0x009c of /bin/ld) */
void delexit(void)
{
	int16_t mode;

	if (trace)
		limit_msg();
	unlink("l.out");
	if (!delarg) {
		if (!arflag || !nundef) {
			mode = ~umask(0) & 0x1ff;
			chmod(ofilename, mode);
		}
	}
	if (delarg)
		unlink(ofilename);
	exit(delarg);
}

/* the handler of SIGINT and SIGTERM */
static void sigdel(int sig)
{
	(void)sig;
	delexit();
}

/* main: entry of ld: the signals, the program name (s* gives swapped output), the option scan of pass one over the
   arguments, then pass two (endload) (sld main, Plexus main; 0x009c..0x0370 of /bin/ld) */
int main(int argc, char **argv)
{
	register int16_t c;
	register int16_t i;
	register char *ap;
	register char **p;
	int16_t found;
	int16_t vscan;
	char save;
	struct symbol **sp;

	ldconf_load(argv[0]);	/* host only: ld.conf next to the program */
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, sigdel);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		signal(SIGTERM, sigdel);
	if (argc == 1)
		exit(4);
	ap = strrchr(*argv, '/');
	if (ap == 0)
		ap = *argv;
	else
		ap++;
	pname = ap;
	if (*ap == 's') {
		swapflg++;
		aoutnam = "s.out";
	} else
		noswap++;
	p = argv + 1;
	c = 1;
	goto test;
body:
	if (trace)
		printf("%s:\n", *p);
	filname = 0;
	ap = *p++;
	if (ap[0] == '-') {
		for (i = 1; ap[i] != 0; i++)
			switch (ap[i]) {
			case 'o':
				c++;
				if (c >= argc)
					error(2, "Bad output file");
				ofilename = *p++;
				ofilfnd++;
				break;
			case 'e':
			case 'u':
				c++;
				if (c >= argc)
					error(2, "Bad 'use' or 'entry'");
				sp = slookup(*p++);
				enter(sp);
				if (ap[i] == 'e')
					entrypt = lastsym;
				break;
			case 'v':
				c++;
				if (c >= argc)
					error(2, "-v: arg missing");
				vflag = 1;
				vscan = vindex;
				found = 0;
				while (--vscan >= 0) {
					if (found)
						break;
					found = eq(vnodes[vscan].vname, *p);
				}
				if (found) {
					endload(c, argv);
					restore(vscan);
				} else
					record(c, *p);
				p++;
				break;
			case 'l':
				i--;
				save = ap[i];
				ap[i] = '-';
				load1arg(ap + i);
				ap[i] = save;
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
			/* fall through */
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
				trace++;
				break;
			case 'b':
			case 't':
				c++;
				if (c >= argc)
					error(2, "-%c: arg missing", ap[i]);
				if (reg_sect_addr(ap + i, *p))
					i++;
				p++;
				break;
			case 'w':
				wflag++;
				break;
			case 'N':
				i++;
				switch (ap[i]) {
				case 'l':
					nlflag = 1;
					break;
				case 's':
					nlflag = 0;
					break;
				default:
					error(2, "-N%c: bad argument (l or s)", ap[i]);
				}
				break;
			default:
				error(2, "-%c: bad flag", ap[i]);
				goto next;
			}
	} else
		load1arg(ap);
next:
	c++;
test:
	if (c < argc)
		goto body;
	if (rflag && secspec)
		fprintf(stderr, "%s: warning, -r option overrides -t, -b\n", pname);
	endload(argc, argv);
}

/* reg_sect_addr: read the address of a section option (-t, -d, -b with the selector t, d, b or none) into secorg; 1
   when a selector was given (sld reg_sect_addr; 0x0370..0x045e of /bin/ld) */
int16_t reg_sect_addr(char *s, char *t)
{
	register int16_t k = 0;
	char *fmt;
	register char *u;
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
	if (secflg[k])
		error(2, "-%.2s: -%c%c already specified", s, secflg[k], s[1]);
	secflg[k] = *s;
	fmt = "%o%c";
	if (*u != '0')
		fmt = "%d%c";
	else if ((u[1] | 0x20) == 'x') {
		u += 2;
		fmt = "%x%c";
	}
	if (sscanf(u, fmt, &val, &junk) != 1)
		error(2, "-%.2s %s: bad address", s, t);
	secorg[k] = (int16_t)val;	/* the conversion stores into a 16 bit int */
	if (k == 0)
		return 0;
	return 1;
}

/* endload: pass two over the arguments: end of pass one, open the output, then every object and library in order,
   then finishout (Plexus endload; 0x045e..0x050c of /bin/ld) */
int16_t endload(int16_t argc, char **argv)
{
	register int16_t c;
	register int16_t i;
	register char *ap;
	register char **p;

	filname = 0;
	middle();
	setupout();
	p = argv + 1;
	libp = liblist;
	c = 1;
	while (c < argc) {
		ap = *p++;
		if (trace)
			printf("%s:\n", ap);
		if (*ap == '-') {
			for (i = 1; ap[i] != 0; i++)
				switch (ap[i]) {
				case 'b':
				case 't':
					if (ap[i + 1])
						i++;
				/* fall through */
				case 'e':
				case 'o':
				case 'u':
				case 'v':
					c++;
					p++;
					break;
				case 'N':
					i++;
					break;
				case 'l':
					i--;
					ap[i] = '-';
					load2arg(ap + i);
					goto next;
				}
		} else
			load2arg(ap);
next:
		c++;
	}
	finishout();
}

/* record: save the state of the load before the object named y (argument index x) in the next entry of vnodes: the symbol
   count, libp, the totals and the relocation bases, for a later restore by -v (Plexus record; 0x050c..0x057c of
   /bin/ld) */
void record(int16_t c, char *nam)
{
	register struct overlay *v;

	v = &vnodes[vindex++];
	v->argsav = c;
	v->symsav = symindex;
	v->libsav = libp;
	v->vname = nam;
	v->offt = tsize;
	v->offd = dsize;
	v->offb = bsize;
	v->offs = ssize;
	v->ctsav = ctrel;
	v->cdsav = cdrel;
	v->cbsav = cbrel;
}

/* restore: go back to the state saved by record in entry i of vnodes (libp, the relocation bases, the totals, vindex) and
   clear the hash slots of the symbols entered since then (Plexus restore; 0x057c..0x0608 of /bin/ld) */
void restore(int16_t vscan)
{
	register struct overlay *v;
	register int16_t saved;

	v = &vnodes[vscan];
	vindex = vscan + 1;
	libp = v->libsav;
	ctrel = v->ctsav;
	cdrel = v->cdsav;
	cbrel = v->cbsav;
	tsize = v->offt;
	dsize = v->offd;
	bsize = v->offb;
	ssize = v->offs;
	saved = v->symsav;
	while (symindex > saved) {
		symindex--;
		if (symhash[symindex])	/* the entries behind a long name have no slot: the original stores 0 at address 0 */
			*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
}

/* load1arg: pass one for one argument: an object, an archive without table of contents (every member through
   load_header) or an archive with __.SYMDEF (ranlib_scan until nothing more is loaded) (sld load1arg, Plexus
   load1arg; 0x0608..0x0714 of /bin/ld) */
void load1arg(char *acp)
{
	register char *cp;
	int32_t n;

	cp = acp;
	switch (getfile(cp)) {
	case 0:
		load1(0, 0L);
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
		if (read(infil, rltab, rlcnt * 12) < 0)
			rlcnt = 0;	/* the original does not test it */
		chk_ranlib(rltab, rlcnt);
		while (ranlib_scan())
			;
		*libp++ = -1;
		unlock_rlbuf();
		break;
	case 3:
		error(0, "out of date (warning)");
		for (n = ((archdr.asize + 27) >> 1) + 1; load_header(n); n = ((archdr.asize + 27) >> 1) + n)
			;
		break;
	}
	close(infil);
}

/* load_header: read the header of the archive member at word offset p; an object member whose symbols are needed is
   entered in the library list (libp); 0 at the end of the archive (sld load_header; 0x0714..0x07b2 of /bin/ld) */
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
	magic = (int16_t)rd16(raw);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe507 || magic == 0xe511) {
		if (load1(1, p + 13))
			*libp++ = p;
	}
	return 1;
}

/* ranlib_scan: one pass over the ranlib table of contents: load the members that define a symbol that is still
   undefined; 1 when a member was loaded (sld ranlib_scan; 0x07b2..0x089a of /bin/ld) */
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
			longswap(&rltab[i]);
		load_header(ROFF(&rltab[i]) >> 1);
	}
	return sv != libp;
}

/* add: sum of two words, error(1) with the message a when the sum is 0x10000 or more (sld
   add, Plexus add; 0x089a..0x08cc of /bin/ld) */
int16_t add(int16_t x, int16_t y, char *a)
{
	int32_t s;

	s = (int32_t)(uint16_t)x + (int32_t)(uint16_t)y;
	if (s >= 0x10000)
		error(1, a);
	return(s);
}

/* load1: pass one for one object (a = 1 for an archive member): read the header and the symbol table, enter the
   symbols and add the sizes to the totals; 0 when an archive member defines nothing that is needed (its symbols are
   removed again). The putc forms with (char) casts: see warn_sym_redef (sld load1, Plexus load1; 0x08cc..0x0b6e
   of /bin/ld) */
int16_t load1(int16_t libflg, int32_t c)
{
	register struct symbol *sp;
	int16_t savindex;
	int16_t cnt;
	char ch;
	char type;
	int16_t k;
	int16_t n;

	readhdr(c);
	ctrel = tsize;
	cdrel += dsize;
	cbrel += bsize;
	cnt = 0;
	k = 1;
	savindex = symindex;
	if ((filhdr.relflg & 1) == 1) {
		error(1, "No relocation bits");
		goto done;
	}
	c = ((uint32_t)((uint16_t)(hdrsz + segtsz) >> 1) + filhdr.ftsize + filhdr.fdsize) + c;
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
		if (enter(lookup_name()))
			continue;
		sp = lastsym;
		if (sp->stype != ' ') {
			if (libflg)
				warn_sym_redef(sp, &cursym);
			continue;
		}
		if (cursym.stype == ' ') {
			if (SVALUE(&cursym) > SVALUE(sp))
				SETSV(sp, SVALUE(&cursym));
			continue;
		}
		if (SVALUE(sp) != 0 && cursym.stype == '"') {
			if (libflg)
				continue;
			if (put_name(cursym.sname) < 8)
				putc('\t', stderr);
			putc(' ', stderr);
			error(0, "warning: external data item refers to text symbol");
		}
		cnt++;
		sp->stype = cursym.stype;
		SETSV(sp, SVALUE(&cursym));
	}
	if (libflg && cnt == 0)
		goto clean;
	tsize = add(tsize, filhdr.tsize, "text overflow");
	dsize = add(dsize, filhdr.dsize, "data overflow");
	bsize = add(bsize, filhdr.bsize, "bss overflow");
	ssize = add(ssize, k * 14, "symbol table overflow");
	if (rflag) {
		fttotal = add(fttotal, filhdr.ftsize, "file text overflow");
		fdtotal = add(fdtotal, filhdr.fdsize, "file data overflow");
	}
	return 1;
clean:
	while (symindex > savindex) {
		symindex--;
		if (symhash[symindex])	/* the entries behind a long name have no slot: the original stores 0 at address 0 */
			*symhash[symindex] = 0;
		symhash[symindex] = 0;
	}
done:
	return 0;
}

/* middle: end of pass one: _etext, _edata, _end, the rflag decision, the common symbols, the undefined symbols, the
   layout (place_sec) and the values of the defined symbols. The newline of the undefined message is cast to char: the
   stdio of the original had char *_ptr (sign extension), src/head has unsigned char * (sld middle, Plexus middle;
   0x0b6e..0x0d9c of /bin/ld) */
void middle(void)
{
	register struct symbol *ep;
	register struct symbol *end;
	register int16_t i;

	p_etext = *slookup("_etext");
	p_edata = *slookup("_edata");
	p_end = *slookup("_end");
	end = table16_entry(symindex);
	if (!rflag) {
		ep = table16_entry(0);
		while (entry_lt(ep, end)) {
			if (ep->stype == ' ' && SVALUE(ep) == 0 && ep != p_end && ep != p_edata && ep != p_etext) {
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
	ldrsym(p_etext, (uint32_t)(uint16_t)tsize, '"');
	ldrsym(p_edata, (uint32_t)(uint16_t)dsize, '#');
	ldrsym(p_end, (uint32_t)(uint16_t)bsize, '$');
	ep = table16_entry(0);
	while (entry_lt(ep, end)) {
		if (ep->stype == ' ') {
			i = SVALUE(ep);
			if (i) {
				if (!n8rel4 && !n8swap)
					i = (i + 1) & ~1;
				SETSV(ep, (uint16_t)comsize);
				ep->stype = '%';
				comsize = add(comsize, i, "bss overflow");
			}
		}
		ep = next_entry(ep);
	}
dolayout:
	place_sec();
	nundef = 0;
	ep = table16_entry(0);
	while (entry_lt(ep, end)) {
		switch (ep->stype) {
		case ' ':
			if (arflag == 0) {
				errlev |= 1;
				if (SVALUE(ep) == 0) {
					if (nundef == 0)
						fprintf(stderr, "Undefined:\n");
					nundef++;
					put_name(ep->sname);
					putc('\n', stderr);
				}
			}
			break;
		case '"':
			SETSV(ep, SVALUE(ep) + (uint16_t)torigin);
			break;
		case '#':
			SETSV(ep, SVALUE(ep) + (uint16_t)dorigin);
			break;
		case '$':
			SETSV(ep, SVALUE(ep) + (uint16_t)borigin);
			break;
		case '%':
			ep->stype = '$';
			SETSV(ep, SVALUE(ep) + (uint16_t)corigin);
			break;
		}
		ep = next_entry(ep);
	}
	if (sflag || xflag)
		ssize = 0;
	bsize = add(bsize, comsize, "bss overflow");
	nlocsym = (uint16_t)ssize / 14;
}

/* place_sec: place the text, data and bss areas of the output: sizes, the -t/-b addresses (secorg), rounding to 256
   bytes, the wrap and overlap checks, then the relocation bases (sld place_sec; 0x0d9c..0x108e of /bin/ld) */
void place_sec(void)
{
	register struct areas *q;
	register uint16_t r8;
	register int16_t j;
	int16_t i;

	areas[1].kind = iflag ? 4 : 0;
	areas[3].kind = n8rel4 ? 4 : 2;
	areas[2].kind = iflag ? 0 : 1;
	areas[1].tsize = iflag && n8rel4 == 0 && n8swap == 0 && rflag == 0 ? (tsize + 255) & 0xff00 : tsize;
	areas[2].tsize = dsize;
	areas[3].tsize = comsize + bsize;
	if (secflg[0]) {
		for (i = iflag ? 2 : 1; i < 4; i++) {
			if (secflg[i]) {
				fprintf(stderr, "%s: warning, -%c overrides -%c%c\n", pname, secflg[0], secflg[i], *secnam[i]);
				secflg[i] = 0;
			}
		}
		areas[0].kind = 4;
		areas[0].tsize = dsize + comsize + bsize + iflag ? 0 : tsize;
	}
	for (j = secflg[0] ? 0 : 1; j < 4; j++) {
		q = &areas[j];
		r8 = areas[(int)q->kind].tend;
		switch (secflg[j]) {
		case 't':
			if ((uint16_t)q->tsize > (uint16_t)secorg[j])
				error(1, "-t%c: top address too low", *secnam[j]);
			r8 = secorg[j] - q->tsize;
			r8 = r8 & 0xff00;
			q->tsize = (q->tsize + 255) & 0xff00;
			break;
		case 'b':
			r8 = (secorg[j] + 255) & 0xff00;
			if (r8 != (uint16_t)secorg[j])
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
		if (r8 > (uint16_t)q->tend)
			error(1, "%s area wrap around", secnam[j]);
		if (n8rel4 == 0) {
			for (i = iflag ? 2 : 1; i < j; i++) {
				if (r8 < (uint16_t)areas[i].tend && (uint16_t)q->tend > (uint16_t)areas[i].tstart)
					error(1, "%s and %s areas overlap", secnam[j], secnam[i]);
			}
		}
		q->tstart = r8;
	}
	if (comsize + bsize != areas[3].tsize)
		bsize = areas[3].tsize - comsize;
	tsize = areas[1].tsize;
	dsize = areas[2].tsize;
	torigin = areas[1].tstart;
	dorigin = areas[2].tstart;
	borigin = (corigin = areas[3].tstart) + comsize;
	if (secflg[0])
		secflg[iflag ? 2 : 1] = secflg[0];
}

/* setupout: open the output and the temporary files and fill the header of the output (sld setupout, Plexus
   setupout; 0x108e..0x120e of /bin/ld) */
void setupout(void)
{
	struct objhdr hdr;
	struct fseg seg;
	uint8_t rhdr[0x18];
	uint8_t rseg[0x10];
	int tmpfd;

	tcreat(&toutb, 0);
	if ((tmpfd = mkstemp(tfname)) >= 0)	/* mktemp of the original */
		close(tmpfd);
	tcreat(&tmpoutb, 1);
	if (!sflag || !xflag)
		tcreat(&soutb, 1);
	if (rflag) {
		tcreat(&routb, 1);
		tcreat(&doutb, 1);
	}
	filhdr.fmagic = n8rel4 || n8swap ? (oflag ? 0xe505 : iflag ? 0xe511 : 0xe507) : (oflag ? 0xe705 : iflag ? 0xe711 : 0xe707);
	filhdr.tsize = tsize;
	filhdr.dsize = dsize;
	filhdr.bsize = bsize;
	if (rflag) {
		filhdr.ftsize = fttotal;
		filhdr.fdsize = fdtotal;
	}
	filhdr.ssize = sflag ? 0 : ssize + symindex * 14;
	if (entrypt) {
		if (entrypt->stype != '"')
			error(1, "Entry point not in text");
		else
			filhdr.entry = SVALUE(entrypt);
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
	wr_objhdr(rhdr, &hdr);
	wr_fseg(rseg, &seg);
	if (swapflg) {
		ld_swab(rhdr, rhdr, 0x18);
		swap_quad(rseg);
	}
	mput(&toutb, rhdr, 0x18);
	mput(&toutb, rseg, 0x10);
}

/* tcreat: create the temporary file (flag 1) or the output file and open it for the buffer out (sld tcreat, Plexus
   tcreat; 0x120e..0x12a8 of /bin/ld) */
void tcreat(struct buf *buf, int16_t tempflg)
{
	register int16_t ufd;
	char *nam;

	nam = tempflg ? tfname : ofilename;
	ufd = creat(nam, 0x1b6);
	if (ufd < 0) {
		if (tempflg)
			error(2, "cannot create temp");
		else {
			fprintf(stderr, "ld: cannot create output\n");
			unlink("l.out");
			exit(2);
		}
	}
	close(ufd);
	buf->fildes = open(nam, 2);
	if (tempflg)
		unlink(tfname);
	buf->nleft = 0x100;
	buf->xnext = buf->iobuf;
}

/* init_entry: fill the header of the output (struct objhdr) and its segment record (struct fseg) from filhdr and the
   section options (sld init_entry; 0x12a8..0x13ca of /bin/ld) */
void init_entry(struct objhdr *hdr, struct fseg *seg)
{
	register char *q;
	register int16_t n;

	hdr->hmagic = filhdr.fmagic;
	hdr->hx = (int32_t)filhdr.tsize + filhdr.dsize;
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
		cp++;
		mkfsym(cp);
		load2(0L);
	} else {
		lp = libp;
		goto test1;
body1:
		dseek(&text, *lp, 26);
		mget(raw, 26);
		rd_archdr(raw, &archdr);
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
void load2(int32_t a)
{
	register struct symbol *sp;
	register struct local *lp;
	register int16_t symno;
	char c;
	char d;
	int16_t n;
	int32_t u1;
	int32_t u2;

	readhdr(a);
	ctrel = torigin;
	cdrel += dorigin;
	cbrel += borigin;
	lp = local;
	a += (uint16_t)((hdrsz + segtsz)) >> 1;
	dseek(&text, a + filhdr.ftsize + filhdr.fdsize, filhdr.ssize);
	symno = 0;
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
	sp = *lookup_name();
	if (!sp) {
		if (put_name(cursym.sname) < 8)
			putc('\t', stderr);
		putc(' ', stderr);
		error(2, "internal error: symbol not found");
	}
	if (cursym.stype != 0x20)
		goto other;
	if (lp >= &local[1000])
		error(2, "Local symbol overflow");
	lp->locindex = symno;
	lp->locsymbol = sp;
	lp++;
	goto next;
other:
	if (cursym.stype == sp->stype && SVALUE(&cursym) == SVALUE(sp))
		goto next;
	if (put_name(cursym.sname) < 8)
		putc('\t', stderr);
	putc(' ', stderr);
	error(1, "- Multiply defined");
next:
	symno += n;
test:
	if (text.size > 0)
		goto body;
	dseek(&text, a, filhdr.ftsize);
	u1 = half(filhdr.fdsize);
	u2 = half(filhdr.ftsize) + a + u1;
	dseek(&reloc, u2, filhdr.ftsize);
	load2td(lp, ctrel, &toutb, &routb);
	u1 = half(filhdr.ftsize) + a;
	dseek(&text, u1, filhdr.fdsize);
	u1 = filhdr.ftsize + a + half(filhdr.fdsize);
	dseek(&reloc, u1, filhdr.fdsize);
	load2td(lp, cdrel, &tmpoutb, &doutb);
	torigin += filhdr.tsize;
	dorigin += filhdr.dsize;
	borigin += filhdr.bsize;
}

/* load2td: relocate the words of the text or data of one object with their relocation words and write them to the
   output (q) and with -r the relocation to r (sld load2td, Plexus load2td; 0x172e..0x194a of /bin/ld) */
void load2td(struct local *lp, int16_t creloc, struct buf *b1, struct buf *b2)
{
	(void)creloc;
	register int16_t w13;
	register int16_t w14;
	register int16_t w12;
	register struct symbol *sp;
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
			w12 = swap(w12);
			w13 = swap(w13);
		}
		w14 = w13;
		if (n8swap)
			w12 = swap(w12);
		switch (w13 & 0x0e) {
		case 2:
			w12 += ctrel;
			break;
		case 4:
			w12 += cdrel;
			break;
		case 6:
			w12 += cbrel;
			break;
		}
		if ((w13 & 8) == 8) {
			sp = lookloc(lp, w13);
			if (sp->stype == 0x20)
				w13 = (w13 & 1) + ((index16(sp) + nlocsym) << 4) + 8;
			else {
				w12 += (int16_t)SVALUE(sp);
				w13 = (w13 & 1) + ((sp->stype - 0x21) * 2);
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
		case 10:
			w12 = swap(w12);
		/* fall through */
		case 9:
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
				put_int_buf(ch[0], b1);
			} else
				put_int_buf((unsigned char)w12, b1);
		} else {
			putword(w12, b1);
			if (rflag)
				putword(w13, b2);
		}
	}
}

/* finishout: pad text and data to the area ends, copy the temporary files into the output, the symbols, then
   rewrite the header (sld finishout, Plexus finishout; 0x194a..0x1a12 of /bin/ld) */
int16_t finishout(void)
{
	register uint16_t p;	/* an address of the output image, counted in bytes (a word pointer in the original) */
	register uint16_t end;

	p = torigin;
	end = areas[1].tend;
	goto test1;
body1:
	p += 2;
	putword(0, &toutb);
	if (rflag)
		putword(0, &routb);
test1:
	if (p < end)
		goto body1;
	p = dorigin;
	end = areas[2].tend;
	goto test2;
body2:
	p += 2;
	putword(0, &tmpoutb);
	if (rflag)
		putword(0, &doutb);
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
	flush(&toutb);
	close(toutb.fildes);
	if (!ofilfnd) {
		unlink(aoutnam);
		if (link("l.out", aoutnam) < 0)
			delarg = 4;	/* the original does not test it */
		ofilename = aoutnam;
	}
	delarg = errlev;
	delexit();
}

/* copy: append the temporary file of the buffer p to the output (sld copy, Plexus copy; 0x1a12..0x1a74 of /bin/ld) */
void copy(struct buf *buf)
{
	register int16_t f;
	register int16_t n;
	unsigned char *q;

	flush(buf);
	lseek(f = buf->fildes, 0L, 0);
	goto test;
body:
	q = (unsigned char *)tmpoutb.iobuf;
	do
		put_int_buf(*q++, &toutb);
	while (--n);
test:
	n = read(f, tmpoutb.iobuf, 256);
	if (n > 1)
		goto body;
	close(f);
}

/* high_byte: the page number of an address that must be a multiple of 256 ("internal error - mkchar" otherwise) (sld
   high_byte; 0x1a74..0x1a9a of /bin/ld) */
int16_t high_byte(uint16_t x)
{
	if ((x & 0xff00) != x)
		error(2, "internal error - mkchar");
	return((x >> 8) & 0xff);
}

/* mget: read n bytes of the text stream into p (sld mget, Plexus mget; 0x1a9a..0x1b0a of /bin/ld) */
void mget(void *aloc, int16_t an)
{
	register uint8_t *loc;
	register int16_t n;
	register uint8_t *p;

	n = an;
	n >>= 1;
	loc = aloc;
	text.nibuf -= n;
	if (text.nibuf >= 0) {
		text.size -= n;
		if (text.size > 0) {
			p = text.ptr;
			do {
				*loc++ = *p++;
				*loc++ = *p++;
			} while (--n);
			text.ptr = p;
			return;
		}
		text.size += n;
	}
	text.nibuf += n;
	do {
		wr16(loc, (uint16_t)get(&text));
		loc += 2;
	} while (--n);
}

/* mput: write n bytes of a to the output buffer q (word by word) (sld mput, Plexus mput; 0x1b0a..0x1b38 of /bin/ld) */
void mput(struct buf *buf, const void *aloc, int16_t an)
{
	register const uint8_t *loc;
	register int16_t n;

	loc = aloc;
	n = an >> 1;
	do {
		putword((int16_t)rd16(loc), buf);
		loc += 2;
	} while (--n);
}

/* dseek: position the input stream sp at the word v of the file with n bytes to read: find or fill one of the two
   pages (sld dseek, Plexus dseek; 0x1b38..0x1c5a of /bin/ld) */
void dseek(struct stream *asp, int32_t v, int16_t n)
{
	register struct stream *sp;
	register struct page *p;
	int32_t hi, lo;
	int16_t nw;

	hi = (int32_t)((uint32_t)v >> 8);
	lo = v & 0xff;
	sp = asp;
	sp->pno->nuser -= 1;
	if ((p = &page[0])->hdr.bno == hi || (p = &page[1])->hdr.bno == hi)
		goto common;
	if (p->hdr.nuser == 0 || (p = &page[0])->hdr.nuser == 0) {
		if (page[0].hdr.nuser == 0 && page[1].hdr.nuser == 0 && page[0].hdr.bno < page[1].hdr.bno)
			p = &page[0];
		p->hdr.bno = hi;
		lseek(infil, (off_t)((uint32_t)(v & 0xffffff00L) << 1), 0);
		nw = read(infil, p->buff, 0x200) >> 1;
		if (nw < 0)
			nw = 0;
		p->nibuf = nw;
	} else
		error(2, "No pages");
common:
	p->hdr.nuser += 1;
	sp->bno = hi;
	sp->pno = &p->hdr;
	sp->ptr = p->buff + (lo << 1);
	if (n != -1)
		sp->size = half(n);
	if ((sp->nibuf = p->nibuf - lo) <= 0)
		sp->size = 0;
}

/* half: a byte count in words (unsigned) (sld half, Plexus half; 0x1c5a..0x1c6c of /bin/ld) */
int16_t half(int16_t i)
{
	return((i >> 1) & 0x7fff);
}

/* get: next word of the input stream p; "Premature EOF" when the stream is exhausted (sld get, Plexus get;
   0x1c6c..0x1ce2 of /bin/ld) */
int16_t get(struct stream *asp)
{
	register struct stream *sp;

	sp = asp;
	if (--sp->nibuf < 0) {
		dseek(sp, (int32_t)(sp->bno + 1) << 8, -1);
		--sp->nibuf;
	}
	if (--sp->size <= 0) {
		if (sp->size < 0)
			error(2, premeof);
		fpage.nuser++;
		sp->pno->nuser--;
		sp->pno = &fpage;
	}
	{
		int16_t w = (int16_t)rd16(sp->ptr);

		sp->ptr += 2;
		return w;
	}
}

/*
 * The library prefixes and the library name are arrays between "/usr" and "/z/bin" in the data of the original: after
 * the jump tables of the file and before its string literals, so they are declared here, after the last switch.
 */
static char pfx1[] = "/usr";
char libname[] = "/lib/libxxxxxxxxxxx";
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

/* getfile: open an input file; -lx searches /lib/libx.a (or .8), /usr/lib, /z/bin/lib; 0 for an object, 1 for an
   archive, 2 for an archive with a current __.SYMDEF, 3 for an outdated one (sld getfile, Plexus getfile;
   0x1ce2..0x1e86 of /bin/ld) */
int16_t getfile(char *acp)
{
	register char *cp;
	register char *p;
	uint8_t raw[26];
	struct stat st;

	cp = acp;
	infil = -1;
	archdr.aname[0] = 0;
	filname = acp;
	if (cp[0] == '-' && cp[1] == 'l') {
		filname = libpath;
		if (cp[2] == 0)
			cp = "-lc";
		p = strrchr(libname, '/') + 4;
		strncpy(p, cp + 2, &libname[18] - p);
		strcat(p, noswap ? ".a" : ".8");
		infil = hostopen(libname);
		if (infil != -1)
			filname = libname;
		else {
			strcpy(libpath, pfx1);
			strcat(libpath, libname);
			infil = hostopen(libpath);
			if (infil == -1) {
				strcpy(libpath, pfx2);
				strcat(libpath, libname);
			}
		}
	}
	if (infil == -1) {
		infil = hostopen(filname);
		if (infil < 0)
			error(2, "cannot open");
	}
	page[1].hdr.bno = -1;
	page[0].hdr.bno = -1;
	page[1].hdr.nuser = 0;
	page[0].hdr.nuser = 0;
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

/* error: print a message with the program name and the file (archive member) name; level 2 stops ld, level 1
   marks an error (sld error, Plexus error; 0x1e86..0x1f22 of /bin/ld) */
void error(int16_t n, char *s, ...)
{
	va_list ap;

	if (!errlev)
		fprintf(stderr, "%s: ", pname);
	if (filname) {
		fprintf(stderr, "%s", filname);
		if (archdr.aname[0])
			fprintf(stderr, "(%.14s)", archdr.aname);
		fprintf(stderr, ": ");
	}
	va_start(ap, s);
	vfprintf(stderr, s, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	if (n > 1)
		delexit();
	errlev = n;
}

/* lookloc: the symbol of the local index in the relocation word a, searched in the local table up to end (sld
   lookloc, Plexus lookloc; 0x1f22..0x1f64 of /bin/ld) */
struct symbol *lookloc(struct local *alp, int16_t r)
{
	register struct local *clp;
	register struct local *lp;
	register int16_t sn;

	lp = alp;
	sn = (r >> 4) & 0x0fff;
	clp = local;
	while (clp < lp) {
		if (clp->locindex == sn)
			return(clp->locsymbol);
		clp++;
	}
	error(2, "Local symbol botch");
	return 0;
}

/* readhdr: read the header of the object at word offset a and set filhdr (sizes, entry, symbols, relocation flag) from
   its segment records (sld readhdr, Plexus readhdr; 0x1f64..0x21f2 of /bin/ld) */
void readhdr(int32_t a)
{
	uint16_t magic;
	register int16_t t;
	register int16_t i;
	register int16_t n;
	int16_t c0, c1, c2, w;
	struct objhdr tmp;
	struct fseg rec[2];
	uint8_t rtmp[0x18];
	uint8_t rrec[0x20];

	dseek(&text, a, 2);
	mget(rtmp, 2);
	magic = (int16_t)rd16(rtmp);
	if (swapflg)
		magic = swap(magic);
	if (magic == 0xe707 || magic == 0xe711 || magic == 0xe507 || magic == 0xe511) {
		hdrsz = 0x18;
		dseek(&text, a, hdrsz);
		mget(rtmp, hdrsz);
		if (swapflg)
			ld_swab(rtmp, rtmp, hdrsz);
		rd_objhdr(rtmp, &tmp);
		segtsz = tmp.hsegsz;
		n = segtsz / 16;
		dseek(&text, a + (uint16_t)(hdrsz >> 1), segtsz);
		memset(rrec, 0, sizeof rrec);
		mget(rrec, segtsz);
		if (swapflg) {
			swap_quad(rrec);
			if (n > 1)
				swap_quad(rrec + 16);
		}
		rd_fseg(rrec, &rec[0]);
		rd_fseg(rrec + 16, &rec[1]);
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
			error(2, "Invalid segment attributes");
		filhdr.entry = tmp.hentry;
		filhdr.relflg = tmp.hrelflg & 1;
		filhdr.ssize = tmp.hsymsz;
	} else
		error(2, "Bad format");
	idsep = filhdr.fmagic == 0xe711 || filhdr.fmagic == 0xe511 ? 1 : 0;
	if (magic == 0xe507 || magic == 0xe511) {
		if (arflag)
			error(2, "-r cannot be used with 8-bit files");
		if (tmp.hrelflg & 4)
			n8rel4++;
		else
			n8swap++;
		cdrel = idsep ? 0 : -filhdr.tsize;
		cbrel = n8rel4 ? 0 : cdrel - filhdr.dsize;
		filhdr.ftsize = tmp.hy;
		filhdr.fdsize = (int16_t)tmp.hx - tmp.hy;
	} else {
		t = (filhdr.tsize + 1) & 0xfffe;
		filhdr.tsize = t;
		cdrel = idsep ? 0 : -t;
		i = (filhdr.dsize + 1) & 0xfffe;
		cbrel = cdrel - i;
		filhdr.bsize = (filhdr.bsize + 1) & 0xfffe;
		filhdr.ftsize = filhdr.tsize;
		filhdr.fdsize = filhdr.dsize;
	}
}

/* eq: 1 when the strings are equal, else 0 (sld eq, Plexus eq; 0x21f2..0x221e of /bin/ld) */
int16_t eq(char *s1, char *s2)
{
	for (;;) {
		if (*s1 == *s2++) {
			if (*s1++)
				continue;
			return(1);
		}
		return(0);
	}
}

/* putword: put a word into the output buffer p byte by byte, high byte first (the byte order of the output does not
   depend on the machine) (Plexus putword puts the word whole; 0x221e..0x226e of /bin/ld) */
void putword(int16_t w, struct buf *b)
{
	*b->xnext++ = w >> 8;
	b->nleft--;
	if (b->nleft <= 0)
		flush(b);
	*b->xnext++ = w;
	b->nleft--;
	if (b->nleft <= 0)
		flush(b);
}

/* put_int_buf: put a byte into the output buffer p (sld put_int_buf; 0x226e..0x229c of /bin/ld) */
void put_int_buf(int16_t v, struct buf *p)
{
	*p->xnext++ = v;
	if (--p->nleft <= 0)
		flush(p);
}

/* flush: write the buffer to its file and reset it (sld flush, Plexus flush; 0x229c..0x22e8 of /bin/ld) */
void flush(struct buf *b)
{
	register int16_t n;

	n = (char *)b->xnext - (char *)b->iobuf;
	if (n > 0) {
		if (write(b->fildes, b->iobuf, n) != n)
			error(2, "output error");
	}
	b->xnext = b->iobuf;
	b->nleft = 0x100;
}

