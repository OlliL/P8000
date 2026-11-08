/*
 * copyname.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
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
#include <sys/stat.h>
#include "ld.h"

static char sccsid[] __attribute__((unused)) = "@[$]copyname.c\t3.1  10/27/83 12:14:19 - 87wega3.2";

/*
 * The global variables of sld (all uninitialized ones; the initialized ones are defined in the file that owns them).
 * sld allocates the commons in the order of its global symbol table, i.e. in the order of the first mention in the objects
 * in link order. copyname.c is the first object of the program, so the order of this list is the order of the allocation: the
 * variables of the segment with the large tables first, then those of the segment of the data (tools/order_vars.py sorts them).
 */
/* the global variables of sld */
int32_t spare0;
struct page page[2];
struct page fpage;
struct stream text;
struct stream reloc;
int16_t segtsz;
int16_t hdrsz;
struct archdr archdr;
struct filhdr filhdr;
struct curbuf curbuf;
int16_t symindex;
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
int16_t arflag;
int16_t dflag;
int16_t iflag;
int16_t mcnt;
int16_t nrflag;
int16_t Oflag;
int16_t rflag;
int16_t sflag;
int16_t Sflag;
int16_t tbflag;
int16_t xflag;
int16_t Xflag;
int16_t zflag;
int16_t fsegflg;
int16_t segobj;
int16_t segmode;
int16_t swapflg;
int16_t spare4b8;
int16_t wflag;
int16_t ofilfnd;
int16_t infil;
char *filname;
int32_t spare4c4;
int32_t comsize;
int16_t spare4d0;
int16_t symbytes;
int16_t ctrel;
int16_t cdrel;
int16_t cbrel;
char *pname;
struct buf toutb;
struct buf tmpoutb;
struct buf doutb;
struct buf droutb;
struct buf routb;
struct buf troutb;
struct buf soutb;
int16_t nsymf;
char sparef314[12];
int16_t dsplf;
int16_t bsplf;
struct p2seg *dsplseg;
struct p2seg *bsplseg;
int16_t nblk;
struct p2seg *p2head;
struct reslist reslist[128];
struct p2seg p2tab[256];
int16_t fblk;
int16_t np2seg;
struct p2seg *lastd;
struct p2seg *lastt;
struct p2seg *lastb;
struct p2seg *p2tail;
int32_t tsize;
int32_t dsize;
int32_t bsize;
struct mlist mlist[50];
int16_t ncopied;
int16_t dseg;
int16_t tseg;
int16_t bseg;
int16_t maxseg;
struct rlent *rltab;
int16_t rlcnt;
char liblist[1024];
struct areas areas[5];
char secspec;
char secflg[128][5];
uint16_t secorg[128][5];
int16_t placekd;
char **curarg;
int16_t errlev;
char **cmdfile;
char **cmdav;
int16_t cmdac;
int16_t incmd;
char *cmdargv[2000];
char cmdbuf[10000];
char libname[30];
char libpath[40];
char *rlbuf;

/* unpack_run: copy the name field of a symbol (8 or 14 bytes, or the long form with length byte 0x80|n) zero padded; returns the number of extra 16 byte entries (0x0056..0x00ee of /bin/sld) */
int16_t unpack_run(char *src, char *dst)
{
	register char *s;
	register char *d;
	int16_t n;
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

/* cp8c: build the table form of a C string: up to 8 characters padded, longer names with the length byte 0x80|n and padding to 16 bytes (0x00ee..0x0178 of /bin/sld) */
void cp8c(char *from, char *to)
{
	register char *q;
	register char *t;
	register char *f;
	char cnt;

	f = from;
	q = from;
	t = to;
	cnt = 0;
	while (*f++)
		cnt++;
	if (cnt <= 8)
		unpack_run(q, t);
	else {
		*t++ = cnt | 0x80;
		f--;
		while (q < f)
			*t++ = *q++;
		cnt -= 8;
		while (cnt > 0)
			cnt -= 16;
		while (cnt-- != 0)
			*t++ = 0;
	}
}

/* trunc_name: copy a name into the 8 character field; warns "symbol name truncated" when it is longer (0x0178..0x01d2 of /bin/sld) */
void trunc_name(char *src, char *dst)
{
	register char *s;

	s = src;
	if (*src & 0x80) {
		if ((*s++ & 0x7f) > 8) {
			s--;
			put_name(s);
			error(1, "-- symbol name truncated\n");
			s++;
		}
	}
	unpack_run(s, dst);
}
