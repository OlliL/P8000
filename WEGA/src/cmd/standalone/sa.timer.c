/******************************************************************************
*******************************************************************************
 
	W E G A - Quelle	(C) ZFT/KEAW Abt. Basissoftware - 1988
 
	Standalone	Modul:	sa.timer.c
 
	Bearbeiter:	P. Hoge
	Datum:		10.5.88
	Version:	1.1
 
*******************************************************************************

	Datum/Zeit des Uhrmoduls setzen
	Gueltig: 1.1.80 - 31.12.2019

*******************************************************************************
******************************************************************************/
 

static	int  dmsize[] =		/* Tage/Monat (kumulierend) */
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

char	estring[20];
long	gettime();

main()
{
	register long time;

	if (inb(0xfef1) != 0xbf) {	/* Uhrmodul vorhanden ? */
		printf("Timer not available\n");
		exit(1);
	}
	time = gettime();	/* Uhrenmodul lesen */
	outtime(time);		/* Datum/Zeit ausgeben */
	settimer();		/* Uhrenmodul neu programmieren */
	time = gettime();	/* Uhrenmodul lesen */
	outtime(time);		/* Datum/Zeit ausgeben */
	exit(0);
}

/*
intime()
{
	register year, month, day, hour, mins, secs;
	register i;
	long time;

loop1:
	printf("Enter new Date (MM/DD/YY) : ");
	gets(estring, sizeof estring);
	month = getdat(1);
	day = getdat(2);
	year = getdat(3);
	if (month<1 || month>12 || day<1 || day>31)
		goto loop1;

	time = 0;
	i = dmsize[month-1];
	if (year%4 == 0 && month > 2) i++;
	if (year < 70) year += 100;
	if (year > 137) goto loop1;
	time = (year-69)/4;
	time += 365*(year-70)+i;
	time += day - 1;
loop2:
	printf("Enter new Time in GMT (HH:MM:SS) : ");
	gets(estring, sizeof estring);
	hour = getdat(1);
	mins = getdat(2);
	secs = getdat(3);
	if (mins<0 || mins>59 || secs<0 || secs>59 || hour<0 || hour>23)
		goto loop2;

	time = 24*time + hour;
	time = 60*time + mins;
	time = 60*time + secs;
	return(time);
}
*/

getdat(k)
register k;
{
	register i, c, n;

	n = 0;
	for (i = 0; c = estring[i]; i++) {
		if (k-1 == 0) {
			if (c == '/' || c== ':') return(n);
			if (c < '0' || c > '9') return(-1);
			n = n*10 + (c-'0');
		} else
			if (c == '/' || c == ':') k--;
	}
	return(n);
}

outtime(time)
long time;
{
	register year, month, day, hour, mins, secs;
	register i;

	secs = time % 60;	/* in Sekunden */
	time /= 60;		/* in Minuten */
	mins = time % 60;
	time /= 60;		/* in Stunden */
	hour = time % 24;
	time /= 24;		/* in Tagen */

	time -= (time+365)/(3*365+366);	/* Schalttage abziehen */
	year = time/365+1970;
	day = time % 365;		/* Tag des Jahres */
	day++;
	i = 0;
	for (month = 1; day > dmsize[month]+i; month++) {
		if (year%4 == 0)
			i = 1;
	}
	if (month <= 2)
		i = 0;
	day -= (dmsize[month-1]+i);
	printf("Date is: %d/%d/%d\n", month, day, year);
	printf("Time is: %d:%d:%d GMT\n", hour, mins, secs);
}

settimer()
{
	register year, month, day, hour, mins, secs;

loop1:
	printf("Enter new Date (MM/DD/YY) : ");
	gets(estring, sizeof estring);
	if (estring[0] == 0)
		goto loop2;
	month = getdat(1);
	day = getdat(2);
	year = getdat(3);
	if (month<1 || month>12 || day<1 || day>31 || (year<80 && year>19))
		goto loop1;

	outvalue(0x02);		/* Datum einstellen */
	outvalue(0x03);		/* Korrektur Monat */
	outvalue(0x00);
	while ((invalue(0xfefd,1) + invalue(0xfeff,1)*10) != month)
		;
	outvalue(0x01);		/* Korrektur Zehnerstelle Tag */
	outvalue(0x00);
	while (invalue(0xfefb,1) != day/10)
		;
	outvalue(0x01);		/* Korrektur Einerstelle Tag */
	outvalue(0x00);
	while (invalue(0xfef9,1) != day%10)
		;
	outvalue(0x01);		/* Korrektur Zehnerstelle Jahr */
	outvalue(0x00);
	while (invalue(0xfef7,1) != year/10)
		;
	outvalue(0x01);		/* Korrektur Einerstelle Jahr */
	outvalue(0x00);
	while (invalue(0xfef5,1) != year%10)
		;
	outvalue(0x02);		/* Korrektur Ende */
	outvalue(0x00);


loop2:
	printf("Enter new Time in GMT (HH:MM) : ");
	gets(estring, sizeof estring);
	if (estring[0] == 0)
		return;
	hour = getdat(1);
	mins = getdat(2);
	if (mins<0 || mins>59 || hour<0 || hour>23)
		goto loop2;

	outvalue(0x01);		/* Zeit stellen */
	outvalue(0x00);
	outvalue(0x02);		/* Sekunden auf Null */
	outvalue(0x00);
	outvalue(0x01);
	outvalue(0x00);
	outvalue(0x01);		/* Korrektur Stunden */
	outvalue(0x00);
	while ((invalue(0xfefd,1) + invalue(0xfeff,1)*10) != hour)
		;
	outvalue(0x01);		/* Korrektur Zehner-Minuten */
	outvalue(0x00);
	while (invalue(0xfefb,1) != mins/10)
		;
	outvalue(0x01);		/* Korrektur Einer-Minuten */
	outvalue(0x00);
	while (invalue(0xfef9,1) != mins%10)
		;
	outvalue(0x01);		/* Korrektur Stop */
	outvalue(0x00);
	printf("Enter Return to start time : ");
	gets(estring, 1);
	outvalue(0x01);		/* Start der Zeit */
	outvalue(0x00);
}

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
			;		/* Sekunden */
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
	return(invalue(addr,0) + invalue(addr+2,0)*10);
}

invalue(addr, flag)
register addr;
register flag;
{
	register seg7, takt;
	
	seg7 = inb(addr);
	if (seg7 != inb(addr))
		seg7 = inb(addr);
	takt = seg7 & 0x80;		/* nur gueltig bei 6. Stelle */
	if (seg7 & 0x20)
		seg7 = ~seg7;
	if (addr == 0xfeff)		/* 6.Stelle */
		seg7 &= 0x07;
	seg7 &= 0x1f;
	if (seg7 == 0x0f)
		return(0);
	if (seg7 == 0x00) {
		if (flag == 0)
			return(0); 
		if (addr == 0xfeff && takt)
			return(0);
	}
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
	return(0xff);
}
