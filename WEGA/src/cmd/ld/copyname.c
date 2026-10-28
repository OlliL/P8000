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
long bE000;
char b0004[518];
char b020a[518];
int bE410;
int bE412;
char bE414[12];
char *bE420;
char bE424[12];
char *bE430;
int bE434;
int bE436;
char bE438[14];
long bE446;
long bE44A;
long bE44E;
int bE452;
long bE454;
long bE458;
long bE45C;
int bE460;
long bE462;
struct bytes2 bE466s;
int bE468;
long bE46A;
char bE46E;
int bE470;
char bE472[8];
char k047a[128];
int bE4FA;
char *bE4FC;
char **tblE500[4090];
char tab44E8[16368];
char tab84d8[24486];
char *gE47E;
long bE482;
long bE486;
long bE48A;
long bE48E;
long bE492;
int bE496;
int bE498;
int bE49A;
int bE49C;
char bE49E[2];
int bE4A0;
int bE4A2;
int bE4A4;
int bE4A6;
int bE4A8;
int bE4AA;
int bE4AC;
int bE4AE;
int bE4B0;
int bE4B2;
int bE4B4;
int bE4B6;
int bE4B8;
int bE4BA;
int bE4BC;
int bE4BE;
char *bE4C0;
long bE4C4;
long bE4C8;
int bE4CC;
int bE4CE;
int bE4D0;
int bE4D2;
int bE4D4;
char *pE4D6;
char bE4DA[520];
char bE6E2[520];
char bE8EA[520];
char bEAF2[520];
char bECFA[520];
char bEF02[520];
char bF10A[520];
int bEF312;
char bF314[12];
int bF320;
int bF322;
struct seg *bF324;
struct seg *bF328;
int bEF32C;
long bF32E;
struct sg4 tblF332[128];
char tab1c02[8192];
int bE3C02;
int bE3C04;
char *pE3C06;
char *pE3C0A;
char *pE3C0E;
long b3C12;
long bE3C16;
long bE3C1A;
long b3C1E;
struct sg5 tab3c22[50];
int bE3E7A;
int bE3E7C;
int bE3E7E;
int bE3E80;
int bE3E82;
char *bE3E84;
int bE3E88;
char tab3e8a[1024];
struct tab428a tab428a[5];
char bE42B2;
char bE42B4[128][5];
int tab4534[128][5];
int f4a34;
char **bE4A36;
int bE4A3A;
char **bE4A3C;
char **g4a40;
int bE4A44;
int bE4A46;
char *tab4a48[2000];
char buf6988[10000];
char buf9098[30];
char buf90b6[40];
long bE190DE;

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
	} else if (g0476 > 0)
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

/* f0178..01d2: findings:D2 */
f0178(src, dst)
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
