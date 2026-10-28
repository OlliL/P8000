/*
 * ext.c - part of sld (WEGA 3.1), reconstructed from /bin/sld; the code range is in tools/module_ranges.txt.
 */

#define DEFINE_VARS
#include "ld.h"

/* the global variables of sld */
int g0476;
long g03ba;
int g04e8;
int g04e6;
int bE496;
int bE4A2;
char tmpname[];
char *p0494;
char tab84d8[];
int g3e7a;
int bE4A4;
int bE4AA;
char bE472[];
char bE46E;
long bE46A;
int bE470;
char bF10A[];
int g04fa;
char bE4DA[];
unsigned g04ee;
char *gE47E;
long g1688;
char tab1c02[];
char bE6EA[];
int bE3E7A;
int bE3E82;
char bE6E2[];
char bE8EA[];
char bEF02[];
char bEAF2[];
struct { int a; int b; int c; int d; } tblF332[];
int bE4FA;
char *bE4FC;
char **tblE500[];
int bE4D0;
int bE4D2;
int bE4D4;
long bF32E;
long b3C12;
int bE4BA;
int bE4BC;
int bE4E8;
int bE4A3A;
char bECFA[];
char *bE048A;
char *bE0494;
int bE4F0;
int bE418;
int bE41E;
char *bE41A;
char bE414[];
int bE452;
long bE3C16;
long bE3C1A;
long b3C1E;
long bE462;
int bE468;
int bE4A8;
int bE4AE;
long b3486;
int b488;
int bE4B6;
char bE438[];
int bE4EC;
int bE3B8;
long bE3BA[];
char *bE494;
char bE49E[];
int bE4EA;
int bE1532;
unsigned bE15DA;
long bE190DE;
long bE486;
int bE4BE;
int bE3C04;
struct seg *bF324;
struct seg *bF328;
char *bE47E;
int bE410;
char bE47A[];
char bE15CC[];
int bE15D4;
int bE15D6;
int bE4B0;
int bE4B2;
char buf6988[];
char buf9098[];
char tab1aad[];
char *tab4a48[];
char **g4a40;
int g4a44;
char *pE4D6;
char *pE4C0;
int tabF338[];
char b0004[];
char b020a[];
int bE474;
int bE3C02;
int bEF32C;
char *bE3E84;
int bE3E88;
long bE44E;
int bE488;
char bE42B2;
int bE49A;
char tab42b4[];
int tab4534[][5];
long bE4A40;
int bE4A44;
char tab3e8a[];
char *pE3C0A;
char *pE3C06;
char *pE3C0E;
int bE434;
int bE436;
long bE454;
long bE458;
int bE460;
int bE4A6;
int bE4AC;
char bE424[];
int bEF312;
int bE4B4;
long bE45C;
char bE466;
char bE467;
int bE476;
int bE478;
struct { int a; int b; int c; int nxt; char *str; } tab3c22[];
int b04EA;
char tab44E8[];
int bE4CE;
char buf90b6[];
char *bE4C0;
int bE20C;
int bE006;
int bE20A;
int bE004;
char *bE430;
char *bE420;
long bE446;
long bE47C;
long bE480;
int bE3E7C;
int bE3E7E;
int bE3E80;
int bE4A0;
long bE492;
int bE4D8;
char **bE4A36;
int bE4A46;
int bE49C;
int bE498;
int g04ea;
char **bE4A3C;
long bE482;
long bE48A;
long bE48E;
long bE4C8;
int g0474;
char bE42B4[][5];
struct tab428a tab428a[];
int bF320;
int bF322;
int f4a34;
char *ptrs04ac[];
int bE0F8E;

/* get_block..0762: findings:D7 */
char *get_block(n)
int n;
{
	long p;

	if (n > bE4EC)
		report_error(2, "x");
	while (n > bE3B8) {
		p = calloc(1, 0x36);
		bE3B8++;
		bE3BA[bE3B8] = p;
		if (p == 0)
			report_error(2, "y");
	}
	return bE3BA[n];
}

/* f0762 0x0762..0x078c: done */
f0762(a)
int a;
{
	g03ba = calloc(a, 4);
	if (!g03ba)
		report_error(2, "x");
}
