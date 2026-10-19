/* wctime - see wctime.h. Follows WEGA's libc ctime.c step by step. */
#include <stdlib.h>
#include "wctime.h"

struct wtm {
	int	tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year;
	int	tm_wday, tm_yday, tm_isdst;
};

static char	cbuf[26];
static int	dmsize[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static int32_t	timezone_ = 5*60*60;
static char	tzname_[2][4] = { "EST", "EDT" };
static int	daylight = 1;
static struct { int daylb, dayle; } daytab[] = {
	{ 5,	333 },		/* 1974: Jan 6 - last Sun. in Nov */
	{ 58,	303 },		/* 1975: Last Sun. in Feb - last Sun in Oct */
};

static int
dysize(int y)
{
	if ((y % 4) == 0)
		return 366;
	return 365;
}

static struct wtm *
gmtime_(int32_t tim)
{
	static struct wtm xtime;
	int d0, d1;
	int32_t hms, day;

	hms = tim % 86400L;
	day = tim / 86400L;
	if (hms < 0) {
		hms += 86400L;
		day -= 1;
	}
	xtime.tm_sec = hms % 60;
	d1 = hms / 60;
	xtime.tm_min = d1 % 60;
	d1 /= 60;
	xtime.tm_hour = d1;

	/* 1/1/1970 was a Thursday */
	xtime.tm_wday = (day + 7340036L) % 7;

	if (day >= 0)
		for (d1 = 70; day >= dysize(d1); d1++)
			day -= dysize(d1);
	else
		for (d1 = 70; day < 0; d1--)
			day += dysize(d1-1);
	xtime.tm_year = d1;
	xtime.tm_yday = d0 = day;

	if (dysize(d1) == 366)
		dmsize[1] = 29;
	for (d1 = 0; d0 >= dmsize[d1]; d1++)
		d0 -= dmsize[d1];
	dmsize[1] = 28;
	xtime.tm_mday = d0+1;
	xtime.tm_mon = d1;
	xtime.tm_isdst = 0;
	return &xtime;
}

/* the day number of the first Sunday on or after day d */
static int
sunday(struct wtm *t, int d)
{
	if (d >= 58)
		d += dysize(t->tm_year) - 365;
	return d - (d - t->tm_yday + t->tm_wday + 700) % 7;
}

static void
tzset_(void)
{
	char *p, *q;
	int n, sign;

	if ((p = getenv("TZ")) && *p) {
		n = 3;
		q = tzname_[0];
		do *q++ = *p ? *p++ : ' ';
		while (--n);
		if ((sign = (p[0] == '-')))
			p++;
		n = 0;
		while (*p >= '0' && *p <= '9')
			n = (n * 10) + *p++ - '0';
		if (sign)
			n = -n;
		timezone_ = ((int32_t)(n * 60)) * 60;
		if ((daylight = (*p != '\0'))) {
			q = tzname_[1];
			n = 3;
			do *q++ = *p ? *p++ : ' ';
			while (--n);
		}
	}
}

static struct wtm *
localtime_(int32_t tim)
{
	int dayno;
	struct wtm *ct;
	int daylbegin, daylend;
	int32_t copyt;

	tzset_();
	copyt = (int32_t)((uint32_t)tim - (uint32_t)timezone_);
	ct = gmtime_(copyt);
	dayno = ct->tm_yday;
	daylbegin = 119;	/* last Sun in Apr */
	daylend = 303;		/* Last Sun in Oct */
	if (ct->tm_year == 74 || ct->tm_year == 75) {
		daylbegin = daytab[ct->tm_year-74].daylb;
		daylend = daytab[ct->tm_year-74].dayle;
	}
	daylbegin = sunday(ct, daylbegin);
	daylend = sunday(ct, daylend);
	if (daylight &&
	    (dayno > daylbegin || (dayno == daylbegin && ct->tm_hour >= 2)) &&
	    (dayno < daylend || (dayno == daylend && ct->tm_hour < 1))) {
		copyt = (int32_t)((uint32_t)copyt + 1*60*60);
		ct = gmtime_(copyt);
		ct->tm_isdst++;
	}
	return ct;
}

static char *
ct_numb(char *cp, int n)
{
	cp++;
	if (n >= 10)
		*cp++ = (n/10)%10 + '0';
	else
		*cp++ = ' ';
	*cp++ = n%10 + '0';
	return cp;
}

static char *
asctime_(struct wtm *t)
{
	char *cp, *ncp;

	cp = cbuf;
	for (ncp = "Day Mon 00 00:00:00 1900\n"; (*cp++ = *ncp++); )
		;
	ncp = &"SunMonTueWedThuFriSat"[3*t->tm_wday];
	cp = cbuf;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp++;
	ncp = &"JanFebMarAprMayJunJulAugSepOctNovDec"[t->tm_mon*3];
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp = ct_numb(cp, t->tm_mday);
	cp = ct_numb(cp, t->tm_hour+100);
	cp = ct_numb(cp, t->tm_min+100);
	cp = ct_numb(cp, t->tm_sec+100);
	if (t->tm_year >= 100) {
		cp[1] = '2';
		cp[2] = '0';
	}
	cp += 2;
	cp = ct_numb(cp, t->tm_year+100);
	return cbuf;
}

char *
wega_ctime(int32_t t)
{
	return asctime_(localtime_(t));
}
