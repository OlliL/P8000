/******************************************************************************
*******************************************************************************

	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
	KERN	 3.2		Modul: alloc.c


	Bearbeiter:
	Datum:		$D$
	Version:	$R$

*******************************************************************************
******************************************************************************/

char debugwstr[] = "@[$]debug.c		Rev : 4.1 	09/17/83 02:56:07";
int  kdebug = 0;

#include	<sys/param.h>
#include	<sys/state.h>
#include	<sys/sysinfo.h>
#include	<sys/systm.h>
#include	<sys/callo.h>
#include	<sys/mmu.h>
#include	<sys/s.out.h>
#include	<sys/dir.h>
#include	<sys/user.h>
#include	<sys/proc.h>
#include	<sys/text.h>

#define sreg	state->s_reg
#define eventid state->s_eventid
#define ps	state->s_ps
#define pcseg	state->s_pcseg
#define pc	state->s_pc

#define NBPTS	5
#define NBPL	16
#define UMASK	0x3f00ffffL
#define KMASK	0x7f00ffffL
#define HMASK	0x0f

struct {
	unsigned a;
	union {
	unsigned int *b;
	struct {
		unsigned short b;
		unsigned short c;
	} l;
	} b;
	unsigned d;
	unsigned e;
} breakpt[NBPTS];

extern char *zpr();
extern mmut;
extern fubyte(), fpbyte(), fkbyte();
extern getchar();

static char kdbstr[6];
static char hzif[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static int foo;
static unsigned char format[17];
static unsigned int kval;

long
gethex()
{
	char	c;
	long	val;

	for (val = 0; ; )
		if (c = getchar()){
		if ((c >= 'A') && (c <= 'F'))
			c = c - 0x37;
		else if ((c >= 'a') && (c <= 'f'))
			c = c - 0x57;
		else if ((c >= '0') && (c <= '9'))
			c -= 0x30;
		else return(val);
		val = (val << 4) + (long)c;
		}
		else return;
}

char *
zpr(hval)
unsigned int hval;
{
	unsigned int h;
	char *phzif;

	phzif = hzif;
	h = hval;
	kdbstr[0] = phzif[(unsigned char)((int)h >> 12 & HMASK)];
	kdbstr[1] = phzif[(unsigned char)((int)h >> 8 & HMASK)];
	kdbstr[2] = phzif[(unsigned char)((int)h >> 4 & HMASK)];
	kdbstr[3] = phzif[(unsigned char)((int)h & HMASK)];
	kdbstr[4] = 0;
	return (kdbstr);
}

debug(state)
struct state *state;
{
	caddr_t dadr;
	unsigned int	*kadr;
	int	s, v2, v1;
	int	ic;
	int	bp_cnt, event;
	unsigned long	acount, count;
	int (*sad)();
	struct	segd	sdrf;
	unsigned char	attr;

	if (!kdebug)
		return;
	s = dvi();
	event = eventid & 0xff;
	bp_cnt = NBPTS;
	if (event == 2){
		for (bp_cnt = 0; bp_cnt < NBPTS; bp_cnt++)
			if ((breakpt[bp_cnt].a) &&
			    ((hiword(breakpt[bp_cnt].b) == (pcseg & USEGW)) &&
			    (breakpt[bp_cnt].b.l.c == (pc -2))))
				break;
		if (bp_cnt >= NBPTS)
			printf("KDB: Unset breakpoint\n");
		else {
			breakpt[bp_cnt].a = 0;
			kadr = breakpt[bp_cnt].b.b;
			printf("KDB: @%X Breakpoint\n",breakpt[bp_cnt].b.b);
			getsd(mmut, hibyte(kadr), &sdrf);
			attr = sdrf.sg_attr;
			sdrf.sg_attr &= 0xfe;
			loadsd(mmut, hibyte(kadr), &sdrf);
			*kadr = breakpt[bp_cnt].d;
			sdrf.sg_attr = attr;
			loadsd(mmut, hibyte(kadr), &sdrf);
			pc -= (unsigned) 2;
		}
	}
	while (1){
	printf("KDB>\007");
	ic = getchar();
	switch(ic){
	case 'J':
	case 'j':
	{	printf("ump call to @");
		sad = (int (*)())(((unsigned long)gethex()) & KMASK), (*sad)();
	}
	default:
	{	putchar('\n');
		break;
	}
	case 'B':
	case 'b':
	{	printf("reakpoint @");
		kadr = (unsigned int *)((long)(gethex() & UMASK) & KMASK);
		for (bp_cnt = 0; breakpt[bp_cnt].a && ++bp_cnt<NBPTS; );
		if (bp_cnt >= NBPTS){
			printf("KDB: No more break slots\n");
			break;
		}
		breakpt[bp_cnt].a = 1;
		breakpt[bp_cnt].b.b = kadr;
		breakpt[bp_cnt].d = *kadr;
		getsd(mmut, hibyte(kadr), &sdrf);
		attr = sdrf.sg_attr;
		sdrf.sg_attr &= ~RD_AT;
		loadsd(mmut, hibyte(kadr), &sdrf);
		if ((skbyte(kadr, 0x7f)) ||	/* 7f02  break */
		    (skbyte((((char *)kadr)+1), 0x02))){
			printf(" FAULT ");
			putchar('\n');
			break;
		}
		sdrf.sg_attr = attr;
		loadsd(mmut, hibyte(kadr), &sdrf);
		putchar('\n');
		break;
	} /* von break - Routine */
	case 'U':
	case 'u':
	{	printf("ser space d");
		sad = fubyte;
		goto Mdisplay;
	}	/* von user - Routine */
	case 'P':
	case 'p':
	{	printf("hysical space d");
		sad = fpbyte;
		goto Mdisplay;
	}
	case 'D':
	case 'd':
	{	sad = fkbyte;
Mdisplay:	printf("isplay @");
		dadr = (caddr_t)(gethex() & KMASK);
		printf("for ");
		count = gethex();
		printf("words\n");
		v1 = 0;
		count = (((unsigned long)(count + 7L))>>3)<<3;
		for (acount = 0; acount < count; acount++){
			if (!(v1)){
				if(acount)
					printf("\n");
				printf("%s", zpr(hiword(dadr)));
				printf("%s: ", zpr(loword(dadr)));
			}
			if ((*sad)(dadr, &format[v1])){
				printf(" FAULT ");
				break;
			}
			kval = format[v1]<<8;
			if ((format[v1] < ' ') ||
			    (format[v1] > '~'))
				format[v1] = ' ';
			v1++;
			dadr++;
			if ((*sad)(dadr, &format[v1])){
				printf(" FAULT ");
				break;
			}
			kval |= format[v1];
			if ((format[v1] < ' ') ||
			    (format[v1] > '~'))
				format[v1] = ' ';
			v1++;
			dadr++;
			printf("%s ", zpr(kval));
			if (!(v1 % 0x10)){
				printf("  |%s|", format);
				v1 = 0;
			}
		}
		putchar('\n');
		break;
	}
	case 'M':
	case 'm':
	{	printf("odify ");
		switch((getchar())){
			case 'R':
			case 'r':
			{	printf("egister ");
				count = gethex();
				printf("(%s) with ", (char *)zpr(sreg[count]));
				v2 = ((unsigned int)gethex());
				printf("OK ");
				switch(getchar()){
				case 'Y':
				case 'y':
					sreg[count] = v2;
				}
				break;
			}
			case 'M':
			case 'm':
			{	printf("emory @");
				kadr = (unsigned int *)(gethex() & KMASK);
				printf("(%s) with ", zpr(*kadr));
				v2 = ((unsigned int)gethex());
				printf("OK ");
				switch(getchar()){
					case 'Y':
					case 'y':
						getsd(mmut, hibyte(kadr), &sdrf);
						attr = sdrf.sg_attr;
						sdrf.sg_attr &= ~RD_AT;
						loadsd(mmut, hibyte(kadr), &sdrf);
						if (((skbyte(kadr, hibyte(v2)))) ||
						    (skbyte(((char *)kadr+1), hibyte(v2)))){
							printf(" FAULT ");
							break;
						}
						sdrf.sg_attr = attr;
						loadsd(mmut, hibyte(kadr), &sdrf);
						break;
				}
				break;
			}	/* von memory - Routine */
		}
		putchar('\n');
		break;
	}	/* von modify - Routine */
	case 'R':
	case 'r':
	{	printf("egisters\n");
		for (acount = 0; acount < 16; acount++){
			if (!(acount & 7))
				putchar('\n');
			printf("r%d:%s ", acount, zpr(sreg[acount]));
		}
		printf("\nfcw=%s ", zpr(ps));
		printf("pcseg=%s ", zpr(pcseg));
		printf("pcoff=%s", zpr(pc));
		putchar('\n');
		break;
	}	/* von register - Routine */
	case 'S':
	case 's':
	{	printf("ingle step\n");
		ps |= TBIT;
		rvi(s);
		return(1);
	}	/* von step - Routine */
	case 'T':
	case 't':
	{	printf("ape crash dump\n");
		printf("writable tape loaded in drive 0? ");
		switch(getchar()){
		{	case 'Y':
			case 'y':
				dump(state);
		}
		}
		putchar('\n');
		break;
	}
	case 'C':
	case 'c':
	{	printf("ontinue\n");
		rvi(s);
		return(1);
	}
	case 'E':
	case 'e':
	{	printf("xit\n");
		rvi(s);
		return(1);
	}
    } /* von switch */
    } /* von while (1) */
}
