/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
 
	KERN 3.2	Modul:	timer.c
 
	Bearbeiter:	P. Hoge
	Datum:		10.5.88
	Version:	1.1
 
*******************************************************************************

	Datum/Zeit des Uhrmoduls
	Gueltig: 1.1.70 - 31.12.2037

*******************************************************************************
******************************************************************************/
 

static	int  dmsize[] =		/* Tage/Monat (kumulierend) */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

long
gettime()
{
	register long time;
	register d1, d2, d3;
	register i;

	time = 0;
	if (inb(0xfef1) != 0xbf)	/* Uhrmodul vorhanden ? */
		return(time);
	outvalue(0x02);			/* Umschalten auf Datum */
	d1 = rdvalue(0xfef5);		/* Jahr */
	d2 = rdvalue(0xfef9);		/* Tag */
	d3 = rdvalue(0xfefd);		/* Monat */
	outvalue(0x00);
	if (d1 > 100 || d2 > 31 || d3 > 12)
		return(time);
	i = dmsize[d3-1];
	if (d1%4 == 0 && d3 > 2) i++;
	if (d1 < 70) d1 += 100;
	if (d1 > 137) return(time);
	time = (d1-69)/4;
	time += 365*(d1-70)+i;
	time += d2 - 1;

	d1 = inb(0xfeff) & 0x28;	/* Umschalten auf Zeit */
	if (d1 == 0x08 || d1 == 0x20) {
		outvalue(0x02);
		outvalue(0x00);
	}	
	while ((d1 = rdvalue(0xfef5)) == 59)
		;			/* Sekunden */
	d2 = rdvalue(0xfef9);		/* Minuten */
	d3 = rdvalue(0xfefd);		/* Stunden */
	if (d1 > 59 || d2 > 59 || d3 > 23)
		return(time);
	time = 24*time + d3;
	time = 60*time + d2;
	time = 60*time + d1;
	return(time);
}

outvalue(t)
register t;
{
	outb(0xfef1, t);
	t = 10000;
	while (t--)
		;
}

rdvalue(addr)
unsigned addr;
{
	return(invalue(addr) + invalue(addr+2)*10);
}

invalue(addr)
register addr;
{
	register seg7;
	
	seg7 = inb(addr);
	if (seg7 != inb(addr))
		seg7 = inb(addr);
	if (seg7 & 0x20)
		seg7 = ~seg7;
	if (addr == 0xfeff)		/* 6.Stelle */
		seg7 &= 0x07;
	seg7 &= 0x1f;
	if (seg7 == 0x0f || seg7 == 0x00) /* case ist wesentlich laenger */
		return(0);
	if (seg7 == 0x02)
		return(1);
	if (seg7 == 0x17 || seg7 == 0x05)
		return(2);
	if (seg7 == 0x13)
		return(3);
	if (seg7 == 0x1a)
		return(4);
	if (seg7 == 0x19)
		return(5);
	if (seg7 == 0x1d)
		return(6);
	if (seg7 == 0x03)
		return(7);
	if (seg7 == 0x1f)
		return(8);
	if (seg7 == 0x1b)
		return(9);
	return(-1);
}
