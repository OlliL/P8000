/*
 * copyname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#include "ld.h"

static char sccsid[] = "@[$]copyname.c\t3.1  10/27/83 12:14:19 - 87wega3.2";

/*
 * The global variables of sld (all uninitialized ones; the initialized ones are defined in the file that owns them).
 * sld allocates the commons in the order of its global symbol table, i.e. in the order of the first mention in the objects
 * in link order. copyname.c is the first object of the program, so the order of this list is the order of the allocation: the
 * variables of the segment with the large tables first, then those of the segment of the data (tools/order_vars.py sorts them).
 */
/* the global variables of sld */
long spare0;
struct page page[2];
struct pghdr fpage;
struct stream text;
struct stream reloc;
int segtsz;
int hdrsz;
struct archdr archdr;
struct filhdr filhdr;
struct symbol cursym;
char lnbuf[128];
int symindex;
struct symbol *lastsym;
struct symbol **symhash[4090];
struct symbol *hshtab[4092];
struct local local[4081];
struct symbol *symtab;
struct symbol *p_etext;
struct symbol *p_edata;
struct symbol *p_end;
struct symbol *p_stkseg;
struct symbol *entrypt;
int arflag;
int dflag;
int iflag;
int mcnt;
int nrflag;
int Oflag;
int rflag;
int sflag;
int Sflag;
int tbflag;
int xflag;
int Xflag;
int zflag;
int fsegflg;
int segobj;
int segmode;
int swapflg;
int spare4b8;
int wflag;
int ofilfnd;
int infil;
char *filname;
long spare4c4;
long comsize;
int spare4d0;
int symbytes;
int fctrel;
int fcdrel;
int fcbrel;
char *pname;
struct buf toutb;
struct buf tmpoutb;
struct buf doutb;
struct buf droutb;
struct buf routb;
struct buf troutb;
struct buf soutb;
int nsymf;
char sparef314[12];
int dsplf;
int bsplf;
struct p2seg *dsplseg;
struct p2seg *bsplseg;
int nblk;
struct p2seg *p2head;
struct reslist reslist[128];
struct p2seg p2tab[256];
int fblk;
int np2seg;
char *lastd;
char *lastt;
char *lastb;
struct p2seg *p2tail;
long ttotal;
long dtotal;
long btotal;
struct mlist mlist[50];
int ncopied;
int dseg;
int tseg;
int bseg;
int maxseg;
struct rlent *rltab;
int rlcnt;
char liblist[1024];
struct areas areas[5];
char secspec;
char secflg[128][5];
int secorg[128][5];
int placekd;
char **curarg;
int exstat;
char **cmdfile;
char **cmdav;
int cmdac;
int incmd;
char *cmdargv[2000];
char cmdbuf[10000];
char libname[30];
char libpath[40];
long rlbuf;
long bE000;

/* unpack_run 0x0056..0x00ee: done */
int unpack_run(src, dst)
char *src;
char *dst;
{
	register char *s;
	register char *d;
	int n;
	char ch;

	n = 0;
	s = src;
	d = dst;
	if ((ch = *s) & 0x80) {
		ch &= 0x7f;
		n = (ch + 7) / 16;
		ch = n * 16 + 8;
	} else if (cmpmode > 0)
		ch = 14;
	else
		ch = 8;
	goto test;
body:
	if ((*d++ = *s) != 0)
		s++;
test:
	if (ch-- != 0)
		goto body;
	return n;
}

/* pack_name..0178: findings:D2 */
pack_name(src, dst)
char *src;
char *dst;
{
	register char *q;
	register char *d;
	register char *s;
	char cnt;

	s = src;
	q = src;
	d = dst;
	cnt = 0;
	goto test0;
body0:
	cnt++;
test0:
	if (*s++)
		goto body0;
	if (cnt <= 8)
		unpack_run(q, d);
	else {
		*d++ = cnt | 0x80;
		s--;
		goto test1;
body1:
		*d++ = *q++;
test1:
		if (q < s)
			goto body1;
		cnt -= 8;
		goto test2;
body2:
		cnt -= 16;
test2:
		if (cnt > 0)
			goto body2;
		goto test;
body:
		*d++ = 0;
test:
		if (cnt-- != 0)
			goto body;
	}
}

/* trunc_name..01d2: findings:D2 */
trunc_name(src, dst)
char *src;
char *dst;
{
	register char *s;

	s = src;
	if (*src & 0x80) {
		s++;
		if ((*src & 0x7f) > 8) {
			s--;
			put_name(s);
			report_error(1, "-- symbol name truncated\n");
			s++;
		}
	}
	return unpack_run(s, dst);
}
